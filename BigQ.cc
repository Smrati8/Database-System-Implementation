#include "BigQ.h"

BigQ::BigQ(Pipe &in, Pipe &out, OrderMaker &sortOrder, int runlen) {
    // Initialize worker thread data.
    WorkerThreadData *my_data = new WorkerThreadData();
    my_data->inputPipe = &in;
    my_data->outputPipe = &out;
    my_data->sortOrder = &sortOrder;
    my_data->runLength = runlen;

    // Create the worker thread.
    pthread_create(&workerThread, NULL, TPMMS, (void *) my_data);
}

BigQ::~BigQ() {
}

void *TPMMS(void *threadData) {
    WorkerThreadData *workerThreadData = (WorkerThreadData *) threadData;
    InitializeWorkerThreadData(workerThreadData);

    // Phase-1 (Generate sorted runs of runLength pages)
    RunGeneration(workerThreadData);

    // Phase-2 (Merge sorted runs)
    MergeRuns(workerThreadData);

    CleanUp(workerThreadData);
}

void InitializeWorkerThreadData(WorkerThreadData *threadData) {
    // Create buffer page array to store current runs' records
    Page *currentRunPages = new(std::nothrow) Page[threadData->runLength];
    if (currentRunPages == NULL) {
        cerr << "ERROR : Not enough memory. EXIT !!!\n";
        exit(1);
    }

    threadData->currentRunPages = currentRunPages;
    threadData->currentRunPageNumber = 0;
    threadData->numberOfRuns = 0;
    threadData->overflowIsThere = false;

    // Create temporary file for storing runs.
    sprintf(threadData->bigQFileName, "%d.bin", pthread_self());
    threadData->bigQFile.Open(0, threadData->bigQFileName);
}

void RunGeneration(WorkerThreadData *threadData) {
    Record *nextRecord = new Record();

    while (threadData->inputPipe->Remove(nextRecord)) {
        if (!AddRecordToCurrentRun(threadData, nextRecord)) {
            CreateRun(threadData);
            AddRecordToCurrentRun(threadData, nextRecord);
        }
    }
    do {
        CreateRun(threadData);
    } while (threadData->overflowIsThere);
}

int AddRecordToCurrentRun(WorkerThreadData *threadData, Record *nextRecord) {
    Page *currentRunPage = &threadData->currentRunPages[threadData->currentRunPageNumber];
    if (!currentRunPage->Append(nextRecord)) {
        if (threadData->currentRunPageNumber + 1 == threadData->runLength) {
            return 0;
        } else {
            currentRunPage = &threadData->currentRunPages[++threadData->currentRunPageNumber];
        }
        currentRunPage->Append(nextRecord);
    }
    return 1;
}

void CreateRun(WorkerThreadData *workerThreadData) {
    SortAndStoreCurrentRun(workerThreadData);
    ClearCurrentRun(workerThreadData);
}

void SortAndStoreCurrentRun(WorkerThreadData *workerThreadData) {
    // Using priority queue for in memory sort.
    priority_queue<Record *, vector<Record *>, RecordComparator> pq(workerThreadData->sortOrder);
    LoadCurrentRunPriorityQueue(workerThreadData, pq);

    // Get the records from the priority queue and add sorted pages of records in the BigQFile.
    WritePriorityQueueContentToBigQFile(workerThreadData, pq);
}

// Put all the current run's record to the priority queue.
void LoadCurrentRunPriorityQueue(WorkerThreadData *workerThreadData,
                                 priority_queue<Record *, vector<Record *>, RecordComparator> &pq) {
    for (int i = 0; i <= workerThreadData->currentRunPageNumber; i++) {
        Page *currentRunPage = &workerThreadData->currentRunPages[i];
        Record *temp = new Record();
        while (currentRunPage->GetFirst(temp)) {
            pq.push(temp);
            temp = new Record();
        }
        currentRunPage->EmptyItOut();
    }
}

void WritePriorityQueueContentToBigQFile(WorkerThreadData *workerThreadData,
                                         priority_queue<Record *, vector<Record *>, RecordComparator> &pq) {
    workerThreadData->overflowIsThere = false;
    Page *bufferPage = new Page();
    // Get next empty page offset of BigQFile.
    off_t currentPageIndexOfBigQFile =
            workerThreadData->bigQFile.GetLength() - 2 < 0 ? 0 : workerThreadData->bigQFile.GetLength() - 1;
    off_t maxRunPageNumber = currentPageIndexOfBigQFile + workerThreadData->runLength - 1;
    int currentRunPageNumberOfOverflowRecords = 0;

    while (!pq.empty()) {
        // Overflow condition
        if (currentPageIndexOfBigQFile > maxRunPageNumber) {
            if (!(&workerThreadData->currentRunPages[currentRunPageNumberOfOverflowRecords])->Append(pq.top())) {
                currentRunPageNumberOfOverflowRecords++;
            } else {
                pq.pop();
            }
        }
            // If the current buffer is full.
        else if (!bufferPage->Append(pq.top())) {
            workerThreadData->bigQFile.AddPage(bufferPage, currentPageIndexOfBigQFile++);
            bufferPage->EmptyItOut();
        } else {
            pq.pop();
        }
    }

    // If data is not over flow, store the last page in the BigQFile.
    if (currentPageIndexOfBigQFile <= maxRunPageNumber) {
        workerThreadData->bigQFile.AddPage(bufferPage, currentPageIndexOfBigQFile);
    } else {
        workerThreadData->overflowIsThere = true;
    }
}

void ClearCurrentRun(WorkerThreadData *workerThreadData) {
    workerThreadData->numberOfRuns++;
    workerThreadData->currentRunPageNumber = 0;
}

void MergeRuns(struct WorkerThreadData *workerThreadData) {
    priority_queue<PriorityQueueItem, vector<PriorityQueueItem>, RecordComparator> pq(workerThreadData->sortOrder);

    LoadMergeRunPriorityQueue(workerThreadData, pq);

    LoadOutputPipeWithPriorityQueueData(workerThreadData, pq);
}

void LoadMergeRunPriorityQueue(WorkerThreadData *workerThreadData,
                               priority_queue<PriorityQueueItem,
                                       vector<PriorityQueueItem>,
                                       RecordComparator> &pq) {
    PriorityQueueItem *priorityQueueItem = new PriorityQueueItem[workerThreadData->numberOfRuns];
    for (int i = 0; i < workerThreadData->numberOfRuns; i++) {
        priorityQueueItem[i].currentPageNumber = i * workerThreadData->runLength;
        priorityQueueItem[i].maxPageNumberOfCurrentRun =
                std::min((off_t) priorityQueueItem[i].currentPageNumber + workerThreadData->runLength - 1,
                         workerThreadData->bigQFile.GetLength() - 2);

        priorityQueueItem[i].page = new Page();
        workerThreadData->bigQFile.GetPage(priorityQueueItem[i].page, priorityQueueItem[i].currentPageNumber);
        priorityQueueItem[i].head = new Record();
        priorityQueueItem[i].page->GetFirst(priorityQueueItem[i].head);
        pq.push(priorityQueueItem[i]);
    }
}

void LoadOutputPipeWithPriorityQueueData(WorkerThreadData *workerThreadData,
                                         priority_queue<PriorityQueueItem,
                                                 vector<PriorityQueueItem>, RecordComparator> &pq) {
    while (!pq.empty()) {
        PriorityQueueItem item = pq.top();
        pq.pop();
        workerThreadData->outputPipe->Insert(item.head);
        if (!item.page->GetFirst(item.head)) {
            if (item.currentPageNumber + 1 <= item.maxPageNumberOfCurrentRun) {
                item.currentPageNumber++;
                workerThreadData->bigQFile.GetPage(item.page, item.currentPageNumber);
                item.page->GetFirst(item.head);
                pq.push(item);
            }
        } else {
            pq.push(item);
        }
    }
}

void CleanUp(WorkerThreadData *workerThreadData) {
    // Close and delete the file.
    workerThreadData->bigQFile.Close();
    remove(workerThreadData->bigQFileName);

    delete[] workerThreadData->currentRunPages;

    // Shut down output pipe.
    if (workerThreadData->outputPipe) {
        workerThreadData->outputPipe->ShutDown();
    }
}