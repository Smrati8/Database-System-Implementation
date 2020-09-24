#include "DBFile.h"

DBFile::DBFile() {
}

DBFile::~DBFile() {
}

int DBFile::Create(const char *f_path, fType f_type, void *startup) {

    // Meta data file path.
    PathConfig *pathConfig = PathConfig::GetInstance();
    char *metadataPath = pathConfig->GetMetadataFilePath(f_path);

    // Open file for writing.
    ofstream fOut;
    fOut.open(metadataPath);

    fOut << f_type << "\n";

    switch (f_type) {
        case heap: {
            myInternalVar = new HeapDBFile();
            break;
        }
        case sorted: {
            SortInfo *sortInfo = (SortInfo *) startup;
            fOut << sortInfo->runLength << "\n";
            fOut << sortInfo->myOrder->ToString() << "\n";

            myInternalVar = new SortedDBFile(sortInfo);
            break;
        }
        default: {
            cerr << "Not yet implemented";
            exit(1);
        }
    }

    fOut.close();
    return myInternalVar->Create(f_path);
}

int DBFile::Open(const char *f_path) {

    // Meta data file path.
    PathConfig *pathConfig = PathConfig::GetInstance();
    char *metadataPath = pathConfig->GetMetadataFilePath(f_path);

    ifstream fIn;
    string readLine;

    fIn.open(metadataPath);
    if (!fIn.is_open()) return 0;

    getline(fIn, readLine);

    switch (stoi(readLine)) {
        case heap: {
            myInternalVar = new HeapDBFile();
            break;
        }
        case sorted: {
            SortInfo *sortInfo = new SortInfo();
            sortInfo->myOrder = new OrderMaker();

            getline(fIn, readLine);
            sortInfo->runLength = stoi(readLine);

            getline(fIn, readLine);
            sortInfo->myOrder->FromString(readLine);

            myInternalVar = new SortedDBFile(sortInfo);
            break;
        }
        default:
            cerr << "Not yet implemented";
            return 0;
    }

    fIn.close();
    return myInternalVar->Open(f_path);
}

void DBFile::Load(Schema &f_schema, const char *loadpath) {
    myInternalVar->Load(f_schema, loadpath);
}

void DBFile::Add(Record &rec) {
    myInternalVar->Add(rec);
}

void DBFile::MoveFirst() {
    myInternalVar->MoveFirst();
}

int DBFile::GetNext(Record &fetchme) {
    return myInternalVar->GetNext(fetchme);
}

int DBFile::GetNext(Record &fetchme, CNF &cnf, Record &literal) {
    return myInternalVar->GetNext(fetchme, cnf, literal);
}

int DBFile::Close() {
    return myInternalVar->Close();
}
