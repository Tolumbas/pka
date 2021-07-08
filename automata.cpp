#include "automata.h"
#include <iostream>
#include <math.h>
using namespace std;


link make_link(int p, string label, string output){
    link g;
    g.to = p;
    g.label1 = label;
    g.label2 = output;
    return g;
}

automata* createEmptyAutomata(int siz){
    automata* M = new automata();
    M->d = new myvector<link>[siz];
    M->siz = siz;
    return M;
}

automata* createSingletonAutomata(string& label, string& output){
    automata* M = createEmptyAutomata(2);
    addTransition(M,0,label,output,1);
    M->i.push(0);
    M->f.push(1);
    return M;
}

int longestPrefix(string sa,string sb){
    int a=0;
    int s = min(sa.size(),sb.size());
    for (;a<s;a++){
        if (sa[a]!=sb[a])break;
    }
    return a;
}

string safeconcat(string& sa, string& sb){
    string out="";
    for (int a=0;a<sa.size();a++){
        if (sa[a]!='$'){
            out+=sa[a];
        }
    }
    for (int a=0;a<sb.size();a++){
        if (sb[a]!='$'){
            out+=sb[a];
        }
    }
    if (out.size()==0){
        out +="$";
    }
    return out;
}

bool findFunctionalProgress(automata* X, int start, string p1, string p2 ,pair<string,string>* adm, bool* visited){
    int pref = longestPrefix(p1,p2);
    string op1 = p1.substr(pref,-1);
    string op2 = p2.substr(pref,-1);
    if (adm[start].first[0]=='~'){
        adm[start].first = op1;
        adm[start].second = op2;
    }
    if (adm[start].first.compare(op1) != 0 || adm[start].second.compare(op2) != 0){
        cout << "failed at (multiple): " <<start << endl;
        return false;
    }
    if (visited[start])return true;
    visited[start] = true;
    for (int a=0;a<X->d[start].length;a++){
        if (!findFunctionalProgress(
            X,X->d[start][a].to,
            safeconcat(op1,X->d[start][a].label1),
            safeconcat(op2,X->d[start][a].label2),
            adm,
            visited
        )){
            return false;
        }
    }
    return true;
}

bool isFunctional(automata* X){
    pair<string,string>* adm = new pair<string,string>[X->siz];
    bool* visited = new bool[X->siz];
    for (int a=0;a<X->siz;a++){
        adm[a]=make_pair("~","~");
    }

    for (int a=0;a<X->i.length;a++){
        for (int b=0;b<X->siz;b++){
            visited[b]=false;
        }
        bool isf = findFunctionalProgress(X,X->i[a],"","",adm,visited);
        if (!isf){
            delete[] visited;
            delete[] adm;
            return false;
        }
    }
    /*
    automata* T= createEmptyAutomata(X->siz);
    T->i = X->i;
    T->f = X->f;
    for (int a=0;a<X->siz;a++){
        addTransition(T,a,adm[a].first,adm[a].second,a);
    }
    cout << "======= Adm\n";
    printAutomata(T);
    destroyAutomata(T);
    */
    for (int a=0;a<X->f.length;a++){
        if (adm[X->f[a]].first.size()!=0 || adm[X->f[a]].second.size()!=0){
            cout << "failed at (final): " <<X->f[a] << " (" << adm[X->f[a]].first << ") (" << adm[X->f[a]].second << ")"<< endl;
            delete[] visited;
            delete[] adm;
            return false;
        }
    }
    delete[] visited;
    delete[] adm;
    return true;
}

void reverseAutomata(automata*& X){

    automata* O = createEmptyAutomata(X->siz);

    O->f = X->i;
    O->i = X->f;

    for (int a=0;a<X->siz;a++){
        for (int b=0;b<X->d[a].length;b++){
            addTransition(O,X->d[a][b].to,X->d[a][b].label1,X->d[a][b].label2,a);
        }
    }

    destroyAutomata(X);
    X = O;
}

void trimcountDFS(automata* X, int start, bool* visited){
    if(visited[start])
        return;
    visited[start] = true;

    for (int a=0;a<X->d[start].length;a++){
        trimcountDFS(X,X->d[start][a].to,visited);
    }

}

void trimAutomataHelper(automata*& X, myvector<int>& usefull, int* rev, bool* visited){

    automata* O = createEmptyAutomata(usefull.length);

    for (int a=0;a<usefull.length;a++){
        for (int b=0;b<X->d[usefull[a]].length;b++){
            addTransition(
              O,
              a,
              X->d[usefull[a]][b].label1,
              X->d[usefull[a]][b].label2,
              rev[X->d[usefull[a]][b].to]
            );
        }
    }
    for (int a=0;a<X->i.length;a++){
        O->i.push(rev[X->i[a]]);
    }
    for (int a=0;a<X->f.length;a++){
        if (visited[X->f[a]]){
            O->f.push(rev[X->f[a]]);
        }
    }
    destroyAutomata(X);
    X = O;
}

void trimAutomata(automata*& X){
    myvector<int> usefull;
    bool* visited = new bool[X->siz];
    int* rev = new int[X->siz];
    for (int a=0;a<X->siz;a++){
        visited[a] = false;
        rev[a]=-1;
    }
    for (int a=0;a<X->i.length;a++){
        trimcountDFS(X,X->i[a],visited);
    }

    for (int a=0;a<X->siz;a++){
        if(visited[a]){
            rev[a] = usefull.length;
            usefull.push(a);
        }
    }
    trimAutomataHelper(X,usefull,rev,visited);

    reverseAutomata(X);

    usefull.length=0;
    for (int a=0;a<X->siz;a++){
        visited[a] = false;
    }
    for (int a=0;a<X->i.length;a++){
        trimcountDFS(X,X->i[a],visited);
    }

    for (int a=0;a<X->siz;a++){
        if(visited[a]){
            rev[a] = usefull.length;
            usefull.push(a);
        }
    }

    trimAutomataHelper(X,usefull,rev,visited);
    reverseAutomata(X);

    delete[] visited;
    delete[] rev;
}

void symbolizeAutomata(automata*& X){
    int fsize = X->siz;

    for (int a=0;a<X->siz;a++){
        for (int b=0;b<X->d[a].length;b++){
            if(X->d[a][b].label1.size()>1){
                fsize += X->d[a][b].label1.size() - 1;
            }
        }
    }

    automata* N = createEmptyAutomata(fsize);
    int firstunused = X->siz;
    for (int a=0;a<X->siz;a++){
        for (int b=0;b<X->d[a].length;b++){
            if(X->d[a][b].label1.size()>1){
                int from = a;
                int to = X->d[a][b].to;
                for (unsigned int c=0;c<X->d[a][b].label1.size()-1;c++){
                    string second = "$";
                    if(c < X->d[a][b].label2.size()){
                        second = X->d[a][b].label2.substr(c,1);
                    }
                    string first = X->d[a][b].label1.substr(c,1);
                    addTransition(N,from,first,second,firstunused);
                    from = firstunused;
                    firstunused ++;
                }
                string second = "$";
                if(X->d[a][b].label2.size()>=X->d[a][b].label1.size()){
                    second = X->d[a][b].label2.substr(X->d[a][b].label1.size()-1,-1);
                }
                string first = X->d[a][b].label1.substr(X->d[a][b].label1.size()-1,-1);
                addTransition(N,from,first,second,to);
            }
            else{
                addTransition(N,a,X->d[a][b].label1,X->d[a][b].label2,X->d[a][b].to);
            }
        }
    }
    N->f = X->f;
    N->i = X->i;
    destroyAutomata(X);
    X=N;
}

void findAllPureEpsilonConnected(automata* X, int start, myvector<int>& clique){
    for (int a=0;a<clique.length;a++){
        if (clique[a] == start)return;
    }
    clique.push(start);
    for (int a=0;a<X->d[start].length;a++){
        if (X->d[start][a].label1[0] == '$' && X->d[start][a].label2[0] == '$'){
            findAllPureEpsilonConnected(X,X->d[start][a].to,clique);
        }
    }
};

void removePureEpsilon(automata *& X){
    myvector<int> cliques;

    automata* O = createEmptyAutomata(X->siz);
    bool* isI = new bool[X->siz];
    for (int r=0;r<X->siz;r++){
        isI[r] = false;
    }
    for (int r=0;r<X->i.length;r++){
        isI[X->i[r]] = true;
    }
    O->f = X->f;
    for (int a=0;a<X->siz;a++){
        for (int b=0;b<X->d[a].length;b++){
            if (X->d[a][b].label1.compare("$")!=0 || X->d[a][b].label2.compare("$")!=0){
                cliques.length=0;
                findAllPureEpsilonConnected(X,X->d[a][b].to,cliques);
                for (int c=0;c<cliques.length;c++){
                    addTransition(O,a,X->d[a][b].label1,X->d[a][b].label2,cliques[c]);
                }
            }
        }
        if (isI[a]){
            cliques.length=0;
            findAllPureEpsilonConnected(X,a,cliques);
            for (int b=0;b<cliques.length;b++){
                isI[cliques[b]]=true;
            }
        }
    }
    for (int a=0;a<X->siz;a++){
        if(isI[a]){
            O->i.push(a);
        }
    }
    delete[] isI;
    destroyAutomata(X);
    X = O;
}



void findAllEpsilonConnected(automata* X, int start, myvector<pair<int,string>>& out){
    pair<int,string> t;
    t.first = start;
    t.second = string("");
    out.push(t);
    for (int a=0;a<X->d[start].length;a++){
        if (X->d[start][a].label1[0] == '$'){
            int lenstart = out.length;
            findAllEpsilonConnected(X,X->d[start][a].to,out);
            for (int b=lenstart;b<out.length;b++){
                out[b].second = safeconcat(X->d[start][a].label2,out[b].second);
            }
        }
    }
};

int removeAllDuplicateTransitions(automata* A){
    int totalSize=0;
    for (int q=0;q<A->siz;q++){
        myvector<link> striped;
        for (int a=0;a<A->d[q].length;a++){
            bool inc = true;
            for (int b=0;b<striped.length;b++){
                if (A->d[q][a].to == striped[b].to &&
                    A->d[q][a].label1 == striped[b].label1 &&
                    A->d[q][a].label2 == striped[b].label2
                ){
                    inc = false;
                }
            }
            if (inc){
                striped.push(make_link(A->d[q][a].to,A->d[q][a].label1,A->d[q][a].label2));
            }
        }
        A->d[q] = striped;
        totalSize+=striped.length;
    }
    cout << "stripped: " << totalSize << endl;
    return totalSize;
}

automata* squareAutomata(automata* X,int total){
    cout << "Square Size: ";
    cout << X->siz << "*" << X->siz  << "=" << X->siz*X->siz << endl;
    automata* O = createEmptyAutomata(X->siz*X->siz);
    cout << "automata is ready\n";

    cout << "success!\n";
    for (int a1=0;a1<X->siz;a1++){
        for(int b1=0;b1<X->d[a1].length;b1++){ //17k
            for (int a2=a1;a2<X->siz;a2++){
                for(int b2=0;b2<X->d[a2].length;b2++){//17k
                    if (X->d[a1][b1].label1.compare(X->d[a2][b2].label1) != 0 ){
                        continue;
                    }
                    addTransition(
                        O,
                        a1*X->siz+a2,
                        X->d[a1][b1].label2,
                        X->d[a2][b2].label2,
                        X->d[a1][b1].to * X->siz + X->d[a2][b2].to
                    );

                }
            }
        }
        cout << a1 << endl;
    }
    cout << "labels done!\n";
    for (int a=0;a<X->i.length;a++){
        for (int b=0;b<X->i.length;b++){
            O->i.push(X->i[a] * X->siz + X->i[b]);
        }
    }
    for (int a=0;a<X->f.length;a++){
        for (int b=0;b<X->f.length;b++){
            O->f.push(X->f[a] * X->siz + X->f[b]);
        }
    }
    return O;
}

void testEpsilonCliques(automata* X){
    cout << "\nTest Epsilon Cliques:\n";
    myvector<pair<int,string>> out;
    for (int a=0;a<X->siz;a++){
        out.length = 0;
        findAllEpsilonConnected(X,a,out);
        cout << a << " -> ";
        for (int b=0;b<out.length;b++){
            cout << out[b].first << " ";
        }
        cout << endl;
    }
    cout << endl;
}

void realtimeAutomata(automata*& X){

    automata* O = createEmptyAutomata(X->siz);

    O->i = X->i;
    O->f = X->f;

    myvector<pair<int,string>> firststage,secondstage;

    for (int a=0;a<X->siz;a++){
        int qstate= a;// for each state q
        firststage.length=0;
        findAllEpsilonConnected(X,qstate,firststage);

        for (int b=0;b<firststage.length;b++){
            int qprim = firststage[b].first;// for each epsilon connected state q' to q
            for (int c=0;c<X->d[qprim].length;c++){
                if (X->d[qprim][c].label1[0]=='$')
                    continue;
                // for each directly non-epsilon connected state r' to q'
                int rprim = X->d[qprim][c].to;
                string qprimtorprim = X->d[qprim][c].label1;
                secondstage.length=0;
                findAllEpsilonConnected(X,rprim,secondstage);
                for(int d=0;d<secondstage.length;d++){
                    int rstate = secondstage[d].first;// for each epsilon connected state r to r'
                    string concatresult =
                        firststage[b].second +
                        X->d[qprim][c].label2 +
                        secondstage[d].second;
                    string concatfinal="";
                    for (int e=0;e<concatresult.size();e++){
                        if (concatresult[e]!='$'){
                            concatfinal+=concatresult[e];
                        }
                    }
                    if (concatfinal.size()==0){
                        concatfinal+='$';
                    }
                    addTransition(O,qstate,qprimtorprim,concatfinal,rstate);
                }
            }
        }
    }
    destroyAutomata(X);
    copyAutomata(O,X);
}

bool isFunctionalPacket(automata* X){
    automata* Y=nullptr;
    copyAutomata(X,Y);
    trimAutomata(Y);
    removePureEpsilon(Y);
    symbolizeAutomata(Y);
    bool eps = hasEpsilonCycles(Y);
    if (eps)return false;
    realtimeAutomata(Y);
    trimAutomata(Y);
    int  total = removeAllDuplicateTransitions(Y);
    automata* Z = squareAutomata(Y,total);
    trimAutomata(Z);
    return isFunctional(Z);
}
bool isInfiniteAmbiguous(automata* X){
    automata* Y=nullptr;
    copyAutomata(X,Y);
    trimAutomata(Y);
    removePureEpsilon(Y);
    bool eps = hasEpsilonCycles(Y);
    return eps;
}


bool hasEpsilonCyclesDFS(automata* X,int start, bool* visited, myvector<int>& path){
    if (visited[start]){
        for(int a=0;a<path.length;a++){
            if (path[a] == start)return true;
        }
        return false;
    }
    path.push(start);
    visited[start] = true;
    for (int a=0;a<X->d[start].length;a++){
        if(X->d[start][a].label1 == "$"){
            if(hasEpsilonCyclesDFS(X,X->d[start][a].to,visited,path))
                return true;
        }
    }
    path.length -- ;
    return false;
}

bool hasEpsilonCycles(automata* X){
    bool* visited = new bool[X->siz];
    myvector<int> path;
    for (int a=0;a<X->siz;a++){
        for (int b=0;b<X->siz;b++){
            visited[b] = false;
        }
        if(hasEpsilonCyclesDFS(X,a,visited,path)){
            delete[] visited;
            return true;
        }
    }
    delete[] visited;
    return false;
}

void addTransition(automata* A,int q,string a,string b,int p){
    A->d[q].push(make_link(p,a,b));

}

automata* unionAutomata(automata* S, automata* R){
    int siz = S->siz + R->siz;
    automata* O = createEmptyAutomata(siz);
    for (int a=0;a<S->siz;a++){
        for (int b=0;b<S->d[a].length;b++){
            string ch1 = S->d[a][b].label1;
            string ch2 = S->d[a][b].label2;
            int t = S->d[a][b].to;
            O->d[a].push(make_link(t,ch1,ch2));
        }
    }
    for (int a=0;a<R->siz;a++){
        for (int b=0;b<R->d[a].length;b++){
            O->d[a+S->siz].push(
                make_link(
                    R->d[a][b].to + S->siz,
                    R->d[a][b].label1,
                    R->d[a][b].label2
                )
            );
        }
    }

    for (int a=0;a<S->i.length;a++){
        O->i.push(S->i[a]);
    }
    for (int a=0;a<R->i.length;a++){
        O->i.push(R->i[a]+S->siz);
    }
    for (int a=0;a<S->f.length;a++){
        O->f.push(S->f[a]);
    }
    for (int a=0;a<R->f.length;a++){
        O->f.push(R->f[a]+S->siz);
    }

    return O;
}

void destroyAutomata(automata*& a){
    if (a!= nullptr){
        delete[] a->d;
        delete a;
        a=nullptr;
    }
}

void copyAutomata(automata* copyA, automata*& pasteA){
    destroyAutomata(pasteA);
    pasteA = createEmptyAutomata(copyA->siz);
    for (int a=0;a<copyA->i.length;a++){
        pasteA->i.push(copyA->i[a]);
    }
    for (int a=0;a<copyA->f.length;a++){
        pasteA->f.push(copyA->f[a]);
    }
    for (int a=0;a<copyA->siz;a++){
        for (int b=0;b<copyA->d[a].length;b++){
            pasteA->d[a].push(make_link(
                copyA->d[a][b].to,
                copyA->d[a][b].label1,
                copyA->d[a][b].label2
            ));
        }
    }
}

automata* kleenestar(automata* X){
    automata* O = createEmptyAutomata(X->siz+1);
    for (int a=0;a<X->siz;a++){
        O->d[a] = X->d[a];
    }
    O->f=X->f;
    O->i=X->i;
    string e = "$";
    for (int a=0;a<X->f.length;a++){
        addTransition(O,X->f[a],e,e,X->siz);
    }
    for (int a=0;a<X->i.length;a++){
        addTransition(O,X->siz,e,e,X->i[a]);
    }
    O->f.push(X->siz);
    O->i.push(X->siz);
    return O;
}

void printAutomata(automata* X){
    cout << "Q size :" << X->siz << endl;
    cout << "I size : (" << X->i.length  << ") ";
    for (int a=0;a<X->i.length;a++){
        cout << X->i[a] << " ";
    }
    cout << endl;
    cout << "F size : (" << X->f.length << ") ";
    for (int a=0;a<X->f.length;a++){
        cout << X->f[a] << " ";
    }
    cout << endl;
    for (int a=0;a<X->siz;a++){
        for (int b=0;b<X->i.length;b++){
            if (X->i[b] == a){
                cout << "-> ";
            }
        }
        for (int b=0;b<X->f.length;b++){
            if (X->f[b] == a){
                cout << "* ";
            }
        }
        cout << a << " : (" << X->d[a].length << ") [";
        for (int b=0;b<X->d[a].length;b++){
            cout << "{ " << X->d[a][b].to << " : " << X->d[a][b].label1 << " : " << X->d[a][b].label2<< " } ";
        }
        cout << " ]\n";
    }
}

automata* concatAutomata(automata* S, automata* R){
    int siz = S->siz + R->siz;
    automata* O = createEmptyAutomata(siz);
    for (int a=0;a<S->siz;a++){
        for (int b=0;b<S->d[a].length;b++){
            O->d[a].push(make_link(S->d[a][b].to,S->d[a][b].label1,S->d[a][b].label2));
        }
    }
    for (int a=0;a<R->siz;a++){
        for (int b=0;b<R->d[a].length;b++){
            O->d[a+S->siz].push(make_link(R->d[a][b].to + S->siz,R->d[a][b].label1,R->d[a][b].label2));
        }
    }

    for (int a=0;a<S->f.length;a++){
        for(int b=0;b<R->i.length;b++){
            O->d[S->f[a]].push(make_link(R->i[b]+S->siz,"$","$"));
        }
    }

    for (int a=0;a<S->i.length;a++){
        O->i.push(S->i[a]);
    }

    for (int a=0;a<R->f.length;a++){
        O->f.push(R->f[a]+S->siz);
    }
    return O;
}


void getAllWordsHelp(automata* X, string& w, int start ,int sind, myvector<string>& out, string str){
    if (sind > w.size()){
        return;
    }
    if (sind == w.size() || w.compare("$")==0){
        for (int a=0;a<X->f.length;a++){
            if (start == X->f[a]){
                string eps = "$";
                out.push(safeconcat(str,eps));
            }
        }
    }
    for (int a=0;a<X->d[start].length;a++){
        int ds = X->d[start][a].to;
        if (X->d[start][a].label1[0] == w[sind]){
            getAllWordsHelp(X,w,ds,sind+1,out,safeconcat(str,X->d[start][a].label2));
        }
        if (X->d[start][a].label1[0] == '$'){
            getAllWordsHelp(X,w,ds,sind,out,safeconcat(str,X->d[start][a].label2));
        }
    }
}


void getAllWords(automata* X, string w, myvector<string>& out){
    for (int a=0;a<X->i.length;a++){
        getAllWordsHelp(X,w,X->i[a],0,out,"");
    }
}
