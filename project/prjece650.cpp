#include<iostream>
#include<string>
#include<regex>
#include<vector>
#include<utility>
#include "minisat/core/Solver.h"
#include "minisat/core/SolverTypes.h"
#include "InputValidation.cpp"
using namespace std;

/*#define handleError(msg) \
       do { perror(msg); exit(EXIT_FAILURE); } while (0)

#define handleErrorEn(en, msg) \
       do { errno = en; perror(msg); exit(EXIT_FAILURE); } while (0)


static void
pclock(string msg, clockid_t cid)
{
    struct timespec ts;
    cout<<msg;
    if (clock_gettime(cid, &ts) == -1)
        handleError("clock_gettime");
    printf("%4ld.%03ld\n", ts.tv_sec, ts.tv_nsec);
}*/

class Graph{

    int v;
    vector<int> *adjList;
    vector<pair<int,int>> edges;

    public:

    Graph(int v1){
        this->v=v1;
        adjList = new vector<int>[v1];
    }

    void addEdges(vector< std::pair<int,int> > e){
        this->edges = e;
    }

    int getVertices(){
        return v;
    }

    vector<pair<int,int>> getEdges(){
        return edges;
    }


};

Graph g(1);
pthread_t IO_thread, vc_thread, vc1_thread, vc2_thread;
vector<int> vc_result, vc1_result, vc2_result;

void printResult(string n,vector<int> result)
{
    cout<<n<< ": ";
    if (result.size() == 0){
        cout<<endl;
        return;
    }
	sort(result.begin(),result.end());
    for (int i = 0; i < (int)result.size(); i++)
    {
        int j= i+1;
        if (j == (int)result.size())
            cout <<result[i];
        else
            cout << result[i] << ",";
    }
    cout<<endl;
}


vector<pair<int,int>> parseEdge(string str){
    pair<int,int> edges;
    vector<pair<int,int>> parsedEgdes;
    regex re ("-?[0-9]+");
    auto begin = sregex_iterator(str.begin(), str.end(), re);
    auto end = sregex_iterator();
    for(std::sregex_iterator i = begin; i != end; ++i){
      smatch match1 = *i;
      smatch match2 = *++i;
      edges.first = stoi(match1.str());
      edges.second = stoi(match2.str());
      parsedEgdes.push_back(edges);
      //cout << edges.first << " " << edges.second<<endl;
   }

    return parsedEgdes;

}

    Minisat::Var singleValue(int r, int c, int k){
        return (c +r*k);
    }

    vector<int> getPath(Minisat::Solver& solver, int k, int v){

        vector<int> path;
        //cout<<"INSIDE GET PATH"<<endl;
        for(int r=0; r<v; r++){
            for(int c=0; c< k; c++){
                if(solver.modelValue(singleValue(r,c,k))== Minisat::toLbool(0)){
                    path.push_back(r);
                    //cout<<"FINAL "<<r<<endl;
                }
            }
        }
        sort(path.begin(),path.end());
        return path;
    }

    void clause1_atleastOne(Minisat::Solver& solver, int k, int v){
        for(int i=0;i<k; i++){
            Minisat::vec<Minisat::Lit> clause;
            for(int j =0; j< v;j++){
                clause.push(Minisat::mkLit(singleValue(j,i,k)));
            }
            solver.addClause(clause);
        }
    }

    void clause2_onlyOnce(Minisat::Solver& solver, int k, int v){
        for(int i =0;i<v;i++){
            for(int q=0;q<k;q++){
                for(int p=0;p<q;p++){
                    solver.addClause(~Minisat::mkLit(singleValue(i,p,k)), ~Minisat::mkLit(singleValue(i,q,k)));
                }
            }

        }
    }

    void clause3_onePerCover(Minisat::Solver& solver, int k, int v){
        for(int i=0;i<k;i++){
            for(int q=0;q<v;q++){
                for(int p=0;p<q;p++){
                    solver.addClause( ~Minisat::mkLit(singleValue(p,i,k)), ~Minisat::mkLit(singleValue(q,i,k)));
                }
            }
        }
    }

    void clause4_coverEveryEdge(Minisat::Solver& solver, int k, int v, vector< pair<int,int> > edges){

        for (auto i = edges.begin(); i != edges.end(); ++i){
            Minisat::vec<Minisat::Lit> lt;
            for(int j =0;j<k;j++){
                lt.push(Minisat::mkLit(singleValue((*i).first, j, k)));
                lt.push(Minisat::mkLit(singleValue((*i).second, j, k)));
            }
            solver.addClause(lt);
        }
    }

    bool satSolver(Minisat::Solver& solver, int k, int v,vector< pair<int,int> > edges){
        for(int r =0; r<v; r++){
            for(int c=0;c<k;c++){
                solver.newVar();
            }
        }
        clause1_atleastOne(solver,k,v);
        clause2_onlyOnce(solver,k,v);
        clause3_onePerCover(solver,k,v);
        clause4_coverEveryEdge(solver,k,v,edges);
        return solver.solve();
    }

    vector<int> vertexCover(Graph g1){
        int v = g1.getVertices();
        vector< pair<int,int> > edges = g1.getEdges();

        if(edges.empty()){
            //cerr<<endl;
            vector<int> {};
        }

        int output[v];
        vector<int> outputPath[v];
        // for(int i =0;i<v;i++){
        //     output[i]=-1;
        // }
        // for(int i = 0;i<=v; i++){
        //     Minisat::Solver solver;
        //     output[i] = satSolver(solver,i,v,edges);
        //     //cout<<"Ouput is "<<output[i]<<endl;
        //     if(output[i]){
        //         outputPath[i] = getPath(solver,i,v);
        //         //print(outputPath[i]);
        //         //cout<<"Printed"<<endl;
        //         //return print_vector(outputPath[i]);
        //         return outputPath[i];
        //     }
        // }

        int l = 0,h = v, m;
        fill_n(output, v, -1);

        while (l <= h) {
            m = (h+l)/2;
            Minisat::Solver solver;
            output[m] = satSolver(solver,m,v ,edges);
            if(output[m]) {
                outputPath[m] = getPath(solver,m,v);
            }

            if(output[m] ==1 && output[m-1] ==0 && m!=0) {
                return (outputPath[m]);
            }

            if(output[m]== 0 && output[m+1]== 1 && m!= v) {
                return (outputPath[m+1]);
            }

            if (output[m]) {
                h = m - 1;
            }
            else {
                l = m + 1;
            }
        }
        return vector<int> {};
    }

vector< vector<int> > createGraph(int v, vector<pair<int,int> > edges) {
    vector<vector<int> > graph(v);
    for(int i=0;i<v;i++ )
        graph[i].resize(v);

    for(auto i = edges.begin(); i!=edges.end(); ++i){
        graph[(*i).first][(*i).second] = 1;
        graph[(*i).second][(*i).first] = 1;
    }

    return graph;
}


vector<int> vertexCover1(Graph g1) {
    int vertex = g1.getVertices();
    vector< pair<int,int> > edges = g1.getEdges();

    //cout<<"INSIDE vertex cover approx 1"<<endl;
    if(edges.empty()){
        return vector<int> {};
    }

    vector<int> vc1;
    vector<vector<int>> graph = createGraph(vertex, edges);
    int s = graph.size();
    while(true) {
        int v = -1;
        int maxDegree = 0;
        for (int i=0; i < s; i++) {
            int d = 0;
            for (auto j = graph[i].begin(); j!= graph[i].end(); ++j){
                d = d + *j;
            }
            if(d>maxDegree) {
                v=i;
                maxDegree=d;
            }
        }

        if (v < 0) {
            break;
        }

        for (int i=0; i < s; i++) {
            graph[i][v] = 0;
            graph[v][i] = 0;
        }
        vc1.push_back(v);
    }

    sort(vc1.begin(), vc1.end());

    return vc1;
}


vector<int> vertexCover2(Graph g1) {
    int vertex = g1.getVertices();
    vector< std::pair<int,int> > edges = g1.getEdges();
    //cout<<"INSIDE vertex cover approx 2"<<endl;
    if (edges.empty()) {
        return vector<int>{};
    }
    vector<int> vc2;
    vector<vector<int>> graph = createGraph(vertex, edges);

    while(true) {
        int u,v;
        bool check = false;
        for (int i =0; i< vertex; i++) {
            for (int j=0; j < vertex; j++) {
                if (graph[i][j] == 1) {
                    u= i;
                    v = j;
                    check = true;
                    break;
                }
            }
            if (check) {
            break;
            }
        }
        if(!check){
        break;
        }

        vc2.push_back(u);
        vc2.push_back(v);
        for (int i=0; i < (int)graph.size(); i++) {
            graph[u][i] = 0;
            graph[v][i] = 0;
            graph[i][u] = 0;
            graph[i][v] = 0;

        }
    }

    sort(vc2.begin(), vc2.end());

    return vc2;
}

void *vcHandler(void *arg){

    vc_result = vertexCover(g);
    printResult("CNF-SAT-VC",vc_result);
    // clockid_t cid;
	// int s;
    // s = pthread_getcpuclockid(pthread_self(), &cid);
	// if (s != 0)
	// 	handleErrorEn(s, "pthread_getcpuclockid");
	// pclock("CNF-SAT-VC Time Taken:", cid);
	// cout<<endl;
    return NULL;

}

void *vc1Handler(void *arg){

    vc1_result = vertexCover1(g);
    printResult("APPROX-VC-1",vc1_result);
    // clockid_t cid;
	// int s;
    // s = pthread_getcpuclockid(pthread_self(), &cid);
	// if (s != 0)
	// 	handleErrorEn(s, "pthread_getcpuclockid");
	// pclock("APPROX-VC-1 Time Taken:", cid);
	// cout<<endl;
    return NULL;
}

void *vc2Handler(void *arg){

    vc2_result = vertexCover2(g);
    printResult("APPROX-VC-2",vc2_result);
    // clockid_t cid;
	// int s;
    // s = pthread_getcpuclockid(pthread_self(), &cid);
	// if (s != 0)
	// 	handleErrorEn(s, "pthread_getcpuclockid");
	// pclock("APPROX-VC-2 Time Taken:", cid);
	// cout<<endl;
    return NULL;
}

void *IOHandler(void* args) {
    char command;
    int vertices = 0;
    string edges = "";
    //VertexCover* vc = new VertexCover(0);
    while(cin>>command){

        if(command == 'V' || command == 'v'){
            cin>>vertices;
            // cout<<"Vertices: "<<vertices<<endl;
            cin.ignore();
            if(vertices<0){
                cerr<<"Error: Number of vertices can't be less than 0"<<endl;
                continue;
            }
            //delete vc;
            //vc = new VertexCover(vertices);
            g = Graph(vertices);

        }
        else if(command == 'E' || command == 'e'){
            getline(cin, edges);
            //cout<<"Edges "<<edges<<endl;
            vector<pair<int,int>> ed = parseEdge(edges);
            // for (auto i = ed.begin(); i != ed.end(); ++i)
            //     cout << (*i).first << " " << (*i).second<<" ";
            InputValidation validator;
            if(!validator.validEdges(ed, vertices)){
                cerr<<"Error: Invalid Edge Entered"<<endl;
                continue;
            }
            //vc->addEdges(ed);
            //vc->printEdges();
            //vc->vertexCover();
            g.addEdges(ed);
            // cout<<"CNF-SAT-VC: "<<vertexCover(g)<<endl;
            // cout<<"APPROX-VC-1: "<<vertexCover1(g)<<endl;
            // cout<<"APPROX-VC-1: "<<vertexCover2(g)<<endl;
            pthread_create(&vc_thread, NULL, &vcHandler, NULL);
            //pthread_join(vc_thread, NULL);
            struct timespec ts;
            int s;
            if (clock_gettime(CLOCK_REALTIME, &ts) == -1) {
                cout<<"ERROR1";
            }
            ts.tv_sec += 120;

            s = pthread_timedjoin_np(vc_thread, NULL, &ts);
            if (s != 0) {
                cout<<"CNF-SAT-VC: timeout"<<endl;
            }
            pthread_create(&vc1_thread, NULL, &vc1Handler, NULL);
            pthread_join(vc1_thread, NULL);
            pthread_create(&vc2_thread, NULL, &vc2Handler, NULL);
            pthread_join(vc2_thread, NULL);
            cin.ignore();
        }
    }

    // cout<<"INSIDE THREAD"<<endl;
//    delete vc;
    return NULL;

}


int main()
{
    int t;
    t = pthread_create (&IO_thread, NULL, &IOHandler, NULL);
    if(t){
        cerr<<"Error: Unable to create thread"<<endl;
    }
    pthread_join(IO_thread, NULL);
    pthread_exit(NULL);
}
