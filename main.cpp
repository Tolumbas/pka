#include <iostream>
#include <string>
#include <utility>
#include "myvector.h"
#include "automata.h"
#include "parser.h"

using namespace std;



int main()
{
    string regexp="",test,rawin;
    //cin >> regexp;
    getline(cin,rawin);
    for (int a=0;a<rawin.size();a++){
        if (rawin[a]!=' '){
            regexp+=rawin[a];
        }
    }


    int to=0,total;
    automata* X = parseAutomata(regexp,0,to);

    /*
    automata* X = createEmptyAutomata(5);

    X->i.push(0);
    X->f.push(4);
    X->f.push(2);
    addTransition(X,0,"a","c",1);
    addTransition(X,0,"a","cc",2);
    addTransition(X,1,"b","ccd",3);
    addTransition(X,2,"b","cd",3);
    addTransition(X,3,"a","d",4);
    addTransition(X,3,"a","d",2);
    */

    if (X== nullptr){
        cout << "error parsing.\n";
        return 1;
    }
    cout << "======= parsed\n";
    printAutomata(X);
    //cout << "is Functional packet: " <<  isFunctionalPacket(X) << endl;
    cout << "======= remove pure epsilon:\n";
    removePureEpsilon(X);
    //printAutomata(X);

    //testEpsilonCliques(X);
    cout << "======= symbolization\n";
    symbolizeAutomata(X);
    //printAutomata(X);
    bool eps = hasEpsilonCycles(X);
    cout << "is Infinite Ambiguous: " <<  eps << endl;
    cout << "======= all words\n";
    string in;
    cin >> in;
    myvector<string> out;
    getAllWords(X,in,out);
    cout << "out:";
    for (int a=0;a<out.length;a++){
        cout << out[a] << ",";
    }
    cout << endl;
    if (eps){
        destroyAutomata(X);
        return 0;
    }

    cout << "======= realtime\n";
    realtimeAutomata(X);
    //printAutomata(X);

    cout << "======= realtime words:\n";
    out.length=0;
    getAllWords(X,in,out);
    cout << "out:";
    for (int a=0;a<out.length;a++){
        cout << out[a] << ",";
    }
    cout << endl;

    cout << "======= trim\n";

    trimAutomata(X);
    total = removeAllDuplicateTransitions(X);
    cout << "======= square\n";
    automata* Y = squareAutomata(X,total);
    //printAutomata(Y);

    cout << "======= trim\n";
    trimAutomata(Y);
    //printAutomata(Y);
    bool isf = isFunctional(Y);
    cout << "is Functional: " <<  isf << endl;


    //cout << evalAutomata(X,test);
    destroyAutomata(X);
    destroyAutomata(Y);
    return 0;
}
