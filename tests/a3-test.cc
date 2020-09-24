#include "a3-test.h"

Attribute IA = {"int", Int};
Attribute SA = {"string", String};
Attribute DA = {"double", Double};

int clear_pipe (Pipe &in_pipe, Schema *schema, bool print) {
    Record rec;
    int cnt = 0;
    while (in_pipe.Remove (&rec)) {
        if (print) {
            rec.Print (schema);
        }
        cnt++;
    }
    return cnt;
}

int clear_pipe (Pipe &in_pipe, Schema *schema, Function &func, bool print) {
    Record rec;
    int cnt = 0;
    double sum = 0;
    while (in_pipe.Remove (&rec)) {
        if (print) {
            rec.Print (schema);
        }
        int ival = 0; double dval = 0;
        func.Apply (rec, ival, dval);
        sum += (ival + dval);
        cnt++;
    }
    cout << " Sum: " << sum << endl;
    return cnt;
}
int pipesz = 100; // buffer sz allowed for each pipe
int buffsz = 100; // pages of memory allowed for operations

SelectFile SF_ps, SF_p, SF_s, SF_o, SF_li, SF_c;
DBFile dbf_ps, dbf_p, dbf_s, dbf_o, dbf_li, dbf_c;
Pipe _ps (pipesz), _p (pipesz), _s (pipesz), _o (pipesz), _li (pipesz), _c (pipesz);
CNF cnf_ps, cnf_p, cnf_s, cnf_o, cnf_li, cnf_c;
Record lit_ps, lit_p, lit_s, lit_o, lit_li, lit_c;
Function func_ps, func_p, func_s, func_o, func_li, func_c;

int pAtts = 9;
int psAtts = 5;
int liAtts = 16;
int oAtts = 9;
int sAtts = 7;
int cAtts = 8;
int nAtts = 4;
int rAtts = 3;

void init_SF_ps (char *pred_str, int numpgs) {
    dbf_ps.Open (ps->path());
    get_cnf (pred_str, ps->schema (), cnf_ps, lit_ps);
    SF_ps.Use_n_Pages (numpgs);
}

void init_SF_p (char *pred_str, int numpgs) {
    dbf_p.Open (p->path());
    get_cnf (pred_str, p->schema (), cnf_p, lit_p);
    SF_p.Use_n_Pages (numpgs);
}

void init_SF_s (char *pred_str, int numpgs) {
    dbf_s.Open (s->path());
    get_cnf (pred_str, s->schema (), cnf_s, lit_s);
    SF_s.Use_n_Pages (numpgs);
}

void init_SF_o (char *pred_str, int numpgs) {
    dbf_o.Open (o->path());
    get_cnf (pred_str, o->schema (), cnf_o, lit_o);
    SF_o.Use_n_Pages (numpgs);
}

void init_SF_li (char *pred_str, int numpgs) {
    dbf_li.Open (li->path());
    get_cnf (pred_str, li->schema (), cnf_li, lit_li);
    SF_li.Use_n_Pages (numpgs);
}

void init_SF_c (char *pred_str, int numpgs) {
    dbf_c.Open (c->path());
    get_cnf (pred_str, c->schema (), cnf_c, lit_c);
    SF_c.Use_n_Pages (numpgs);
}

// select * from partsupp where ps_supplycost <1.03
// expected output: 31 records
void q1 () {

    char *pred_ps = "(ps_supplycost < 1.03)";
    init_SF_ps (pred_ps, 100);

    SF_ps.Run (dbf_ps, _ps, cnf_ps, lit_ps);
    SF_ps.WaitUntilDone ();

    int cnt = clear_pipe (_ps, ps->schema (), true);
    cout << "\n\n query1 returned " << cnt << " records \n";

    dbf_ps.Close ();
}


// select p_partkey(0), p_name(1), p_retailprice(7) from part where (p_retailprice > 931.01) AND (p_retailprice < 931.3);
// expected output: 22 records
void q2 () {

    char *pred_p = "(p_retailprice > 931.01) AND (p_retailprice < 931.3)";
    init_SF_p (pred_p, 100);

    Project P_p;
    Pipe _out (pipesz);
    int keepMe[] = {0,1,7};
    int numAttsIn = pAtts;
    int numAttsOut = 3;
    P_p.Use_n_Pages (buffsz);

    SF_p.Run (dbf_p, _p, cnf_p, lit_p);
    P_p.Run (_p, _out, keepMe, numAttsIn, numAttsOut);

    SF_p.WaitUntilDone ();
    P_p.WaitUntilDone ();

    Attribute att3[] = {IA, SA, DA};
    Schema out_sch ("out_sch", numAttsOut, att3);
    int cnt = clear_pipe (_out, &out_sch, true);

    cout << "\n\n query2 returned " << cnt << " records \n";

    dbf_p.Close ();
}

// select sum (s_acctbal + (s_acctbal * 1.05)) from supplier;
// expected output: 9.24623e+07
void q3 () {

    char *pred_s = "(s_suppkey = s_suppkey)";
    init_SF_s (pred_s, 100);

    Sum T;
    // _s (input pipe)
    Pipe _out (1);
    Function func;
    char *str_sum = "(s_acctbal + (s_acctbal * 1.05))";
    get_cnf (str_sum, s->schema (), func);
    T.Use_n_Pages (1);
    SF_s.Run (dbf_s, _s, cnf_s, lit_s);
    T.Run (_s, _out, func, 0);

    SF_s.WaitUntilDone ();
    T.WaitUntilDone ();

    Schema out_sch ("out_sch", 1, &DA);
    int cnt = clear_pipe (_out, &out_sch, true);

    cout << "\n\n query3 returned " << cnt << " records \n";

    dbf_s.Close ();
}


// select sum (ps_supplycost) from supplier, partsupp
// where s_suppkey = ps_suppkey;
// expected output: 4.00406e+08
void q4 () {

    cout << " query4 \n";
    char *pred_s = "(s_suppkey = s_suppkey)";
    init_SF_s (pred_s, 100);
    SF_s.Run (dbf_s, _s, cnf_s, lit_s); // 10k recs qualified

    char *pred_ps = "(ps_suppkey = ps_suppkey)";
    init_SF_ps (pred_ps, 100);

    Join J;
    // left _s
    // right _ps
    Pipe _s_ps (pipesz);
    CNF cnf_p_ps;
    Record lit_p_ps;
    get_cnf ("(s_suppkey = ps_suppkey)", s->schema(), ps->schema(), cnf_p_ps, lit_p_ps);

    int outAtts = sAtts + psAtts;
    Attribute ps_supplycost = {"ps_supplycost", Double};
    Attribute joinatt[] = {IA,SA,SA,IA,SA,DA,SA, IA,IA,IA,ps_supplycost,SA};
    Schema join_sch ("join_sch", outAtts, joinatt);

    Sum T;
    // _s (input pipe)
    Pipe _out (1);
    Function func;
    char *str_sum = "(ps_supplycost)";
    get_cnf (str_sum, &join_sch, func);
    T.Use_n_Pages (1);

    SF_ps.Run (dbf_ps, _ps, cnf_ps, lit_ps); // 161 recs qualified
    J.Run (_s, _ps, _s_ps, cnf_p_ps, lit_p_ps);
    T.Run (_s_ps, _out, func, 0);

    SF_ps.WaitUntilDone ();
    J.WaitUntilDone ();
    T.WaitUntilDone ();

    Schema sum_sch ("sum_sch", 1, &DA);
    int cnt = clear_pipe (_out, &sum_sch, true);
    cout << " query4 returned " << cnt << " recs \n";
}

// select distinct ps_suppkey from partsupp where ps_supplycost < 100.11;
// expected output: 9996 rows
void q5 () {

    char *pred_ps = "(ps_supplycost < 100.11)";
    init_SF_ps (pred_ps, 100);

    Project P_ps;
    Pipe __ps (pipesz);
    int keepMe[] = {1};
    int numAttsIn = psAtts;
    int numAttsOut = 1;
    P_ps.Use_n_Pages (buffsz);

    DuplicateRemoval D;
    // inpipe = __ps
    Pipe ___ps (pipesz);
    Schema __ps_sch ("__ps", 1, &IA);

    WriteOut W;
    // inpipe = ___ps
    char *fwpath = "ps.w.tmp";
    FILE *writefile = fopen (fwpath, "w");

    SF_ps.Run (dbf_ps, _ps, cnf_ps, lit_ps);
    P_ps.Run (_ps, __ps, keepMe, numAttsIn, numAttsOut);
    D.Run (__ps, ___ps,__ps_sch);
    W.Run (___ps, writefile, __ps_sch);

    SF_ps.WaitUntilDone ();
    P_ps.WaitUntilDone ();
    D.WaitUntilDone ();
    W.WaitUntilDone ();

    cout << " query5 finished..output written to file " << fwpath << "\n";
}

// select sum (ps_supplycost) from supplier, partsupp
// where s_suppkey = ps_suppkey groupby s_nationkey;
// expected output: 25 rows
void q6 () {

    cout << " query6 \n";
    char *pred_s = "(s_suppkey = s_suppkey)";
    init_SF_s (pred_s, 100);
    SF_s.Run (dbf_s, _s, cnf_s, lit_s); // 10k recs qualified

    char *pred_ps = "(ps_suppkey = ps_suppkey)";
    init_SF_ps (pred_ps, 100);

    Join J;
    // left _s
    // right _ps
    Pipe _s_ps (pipesz);
    CNF cnf_p_ps;
    Record lit_p_ps;
    get_cnf ("(s_suppkey = ps_suppkey)", s->schema(), ps->schema(), cnf_p_ps, lit_p_ps);

    int outAtts = sAtts + psAtts;
    Attribute s_nationkey = {"s_nationkey", Int};
    Attribute ps_supplycost = {"ps_supplycost", Double};
    Attribute joinatt[] = {IA,SA,SA,s_nationkey,SA,DA,SA,IA,IA,IA,ps_supplycost,SA};
    Schema join_sch ("join_sch", outAtts, joinatt);

    char *group_cnf_str = "(s_nationkey)";
    get_cnf (group_cnf_str, &join_sch, cnf_c, lit_c);
    OrderMaker groupOrderMaker, dummyOrderMaker;
    cnf_c.GetSortOrders(groupOrderMaker, dummyOrderMaker);
    GroupBy G;
    // _s (input pipe)
    Pipe _out (pipesz);
    Function func;
    char *str_sum = "(ps_supplycost)";
    get_cnf (str_sum, &join_sch, func);
    OrderMaker grp_order (&join_sch);
    G.Use_n_Pages (1);

    SF_ps.Run (dbf_ps, _ps, cnf_ps, lit_ps); // 161 recs qualified
    J.Run (_s, _ps, _s_ps, cnf_p_ps, lit_p_ps);
    G.Run (_s_ps, _out, groupOrderMaker, func, 0);

    SF_ps.WaitUntilDone ();
    J.WaitUntilDone ();
    G.WaitUntilDone ();

    Schema sum_sch ("sum_sch", 1, &DA);
    int cnt = clear_pipe (_out, &sum_sch, true);
    cout << " query6 returned sum for " << cnt << " groups (expected 25 groups)\n";
}

void q7 () {
/*
select sum(ps_supplycost)
from part, supplier, partsupp
where p_partkey = ps_partkey and
s_suppkey = ps_suppkey and
s_acctbal > 2500;

ANSWER: 274251601.96 (5.91 sec)

possible plan:
	SF(s_acctbal > 2500) => _s
	SF(p_partkey = p_partkey) => _p
	SF(ps_partkey = ps_partkey) => _ps
	On records from pipes _p and _ps:
		J(p_partkey = ps_partkey) => _p_ps
	On _s and _p_ps:
		J(s_suppkey = ps_suppkey) => _s_p_ps
	On _s_p_ps:
		S(s_supplycost) => __s_p_ps
	On __s_p_ps:
		W(__s_p_ps)

Legend:
SF : select all records that satisfy some simple cnf expr over recs from in_file
SP: same as SF but recs come from in_pipe
J: select all records (from left_pipe x right_pipe) that satisfy a cnf expression
P: project some atts from in-pipe
T: apply some aggregate function
G: same as T but do it over each group identified by ordermaker
D: stuff only distinct records into the out_pipe discarding duplicates
W: write out records from in_pipe to a file using out_schema
*/
    cout << " TBA\n";
}

void q8 () {
    cout << " query8 \n";
    char *pred_li = "(o_orderkey = o_orderkey)";
    init_SF_o(pred_li, 100);

    Project P_li;
    Pipe _out (pipesz);
    int keepMe[] = {1};
    int numAttsIn = oAtts;
    int numAttsOut = 1;
    P_li.Use_n_Pages (buffsz);

    DuplicateRemoval DR;
    Pipe _o_dr(pipesz);

    Attribute projectAtt[] = {IA};
    Schema projectSchema("project_schema", numAttsOut, projectAtt);

    SF_o.Run(dbf_o, _o, cnf_o, lit_o);
    P_li.Run(_o, _out, keepMe, numAttsIn, numAttsOut);
    DR.Run(_out, _o_dr, projectSchema);

    int cnt = clear_pipe(_o_dr, &projectSchema, false);

    SF_li.WaitUntilDone();
    P_li.WaitUntilDone();
    DR.WaitUntilDone();

    cout << "Number of records : " << cnt << '\n';
}

void q9 () {
    cout << " query9 - Number of distincts \"ps_supplycost\" \n";

    char *pred_ps = "(ps_supplycost = ps_supplycost)";
    init_SF_ps(pred_ps, 100);

    Project P_ps;
    Pipe _out (pipesz);

    int keepMe[] = {3};
    int numAttsIn = psAtts;
    int numAttsOut = 1;
    P_ps.Use_n_Pages (buffsz);

    DuplicateRemoval DR;
    Pipe _ps_dr(pipesz);

    Attribute projectAtt[] = {DA};
    Schema projectSchema("project_schema", numAttsOut, projectAtt);

    SF_ps.Run(dbf_ps, _ps, cnf_ps, lit_ps);
    P_ps.Run(_ps, _out, keepMe, numAttsIn, numAttsOut);
    DR.Run(_out, _ps_dr, projectSchema);

    int cnt = clear_pipe(_ps_dr, &projectSchema, false);

    SF_ps.WaitUntilDone();
    P_ps.WaitUntilDone();
    DR.WaitUntilDone();

    cout << "Number of records : " << cnt << '\n';
}

void q10 () {
    cout << " query10 - Number of distincts \"s_acctbal\" \n";

    char *pred_s = "(s_acctbal = s_acctbal)";
    init_SF_s(pred_s, 100);

    Project P_s;
    Pipe _out (pipesz);

    int keepMe[] = {5};
    int numAttsIn = sAtts;
    int numAttsOut = 1;
    P_s.Use_n_Pages (buffsz);

    DuplicateRemoval DR;
    Pipe _ps_dr(pipesz);

    Attribute projectAtt[] = {DA};
    Schema projectSchema("project_schema", numAttsOut, projectAtt);

    SF_ps.Run(dbf_s, _s, cnf_s, lit_s);
    P_s.Run(_s, _out, keepMe, numAttsIn, numAttsOut);
    DR.Run(_out, _ps_dr, projectSchema);

    int cnt = clear_pipe(_ps_dr, &projectSchema, false);

    SF_ps.WaitUntilDone();
    P_s.WaitUntilDone();
    DR.WaitUntilDone();

    cout << "Number of records : " << cnt << '\n';
}

int main (int argc, char *argv[]) {

    if (argc != 2) {
        cerr << " Usage: ./test.out [1-9] \n";
        exit (0);
    }

    void (*query_ptr[]) () = {&q1, &q2, &q3, &q4, &q5, &q6, &q7, &q8, &q9, &q10};
    void (*query) ();
    int qindx = atoi (argv[1]);

    if (qindx > 0 && qindx < 11) {
        setup ();
        query = query_ptr [qindx - 1];
        query ();
        cleanup ();
        cout << "\n\n";
    }
    else {
        cout << " ERROR!!!!\n";
    }
}
