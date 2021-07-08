#include "myvector.h"
#ifndef AUTOMATA
#define AUTOMATA
#include <string>
using namespace std;

struct link{
    int to;
    string label1;
    string label2;
};

struct automata{
    myvector<link>* d;
    myvector<int> i;
    myvector<int> f;
    int siz;
};

int removeAllDuplicateTransitions(automata* X);

bool isFunctionalPacket(automata* X);
bool isInfiniteAmbiguous(automata* X);

void getAllWords(automata* X, string w, myvector<string>& out);

void removePureEpsilon(automata *& X);

bool findFunctionalProgress(automata* X, int start, string p1, string p2 ,pair<string,string>* adm, bool* visited);
bool isFunctional(automata* X);

void reverseAutomata(automata*& X);

void trimAutomataHelper(automata*& X, myvector<int>& usefull, int* rev, bool* visited);
void trimcountDFS(automata* X, int start, bool* visited);
void trimAutomata(automata*& X);

automata* squareAutomata(automata* X,int total);

void testEpsilonCliques(automata* X);
void findAllEpsilonConnected(automata* X, int start, myvector<pair<int,string>>& out);

void realtimeAutomata(automata*& X);
void symbolizeAutomata(automata*& X);

bool hasEpsilonCyclesDFS(automata* X,int start, bool* visited, myvector<int>& path);
bool hasEpsilonCycles(automata* X);

automata* createSingletonAutomata(string& label, string& output);
link make_link(int p, string label, string output);
automata* createEmptyAutomata(int siz);
void addTransition(automata* A,int q,string a,string b,int p);
void printAutomata(automata* X);
automata* unionAutomata(automata* S, automata* R);
void destroyAutomata(automata*& a);
bool evalAutomata(automata* A, string test);
void copyAutomata(automata* copyA, automata*& pasteA);
automata* kleenestar(automata* X);
automata* concatAutomata(automata* S, automata* R);

#endif // AUTOMATA
