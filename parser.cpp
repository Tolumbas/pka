
#include "myvector.h"
#include "automata.h"
#include "parser.h"
#include <string>
#include <iostream>
using namespace std;


string parseString(string in,int from, int& to){
    int index = from;
    while(index < in.size()){
        if (in[index]!='.' && in[index]!=',' && in[index]!=')' && in[index]!='|' && in[index]!='*' && in[index]!='"'){
            index++;
        }
        else{
            break;
        }
    }
    to=index;
    return in.substr(from,to-from);
}

void parseChar(string in,int from, int&to, int ch){
    if (from < in.size() && in[from] == ch){
        to++;
    }
}


automata* parseBrackets(string in, int from, int& to){
    int saveto = to;
    int saveinito = to;

    parseChar(in,from,to,'(');

        if (to == saveto){
            to = saveinito;
            return nullptr;
        }
    saveto = to;
    automata* R = parseAutomata(in,to,to);

        if (to == saveto){
            to = saveinito;
            return nullptr;
        }
        saveto = to;

    parseChar(in,to,to,')');

        if (to == saveto){
            to = saveinito;
            return nullptr;
        }
        saveto = to;
    return R;

}


automata* parseSingleton(string in, int from, int& to){
    int saveto = to;
    int saveinito = to;

    parseChar(in,from,to,'"');

        if (to == saveto){
            to = saveinito;
            return nullptr;
        }
    saveto = to;
    string label = parseString(in,to,to);

        if (to == saveto){
            label="$";
        }

    parseChar(in,from,to,'"');

        if (to == saveto){
            to = saveinito;
            return nullptr;
        }
    saveto = to;
    parseChar(in,to,to,'^');

        if (to == saveto){
            to = saveinito;
            return nullptr;
        }
        saveto = to;
    parseChar(in,from,to,'"');

        if (to == saveto){
            to = saveinito;
            return nullptr;
        }
    saveto = to;
    string output = parseString(in,to,to);

        if (to == saveto){
            output = "$";
        }

    parseChar(in,from,to,'"');

        if (to == saveto){
            to = saveinito;
            return nullptr;
        }
    saveto = to;
    return createSingletonAutomata(label,output);
}

automata* parseAutomataPrim(string in, int from, int& to, automata* I){
    int saveto = to;
    int saveinito = to;
    automata* X;

    from = to;
    parseChar(in,from,to,'*');
    if (to != saveto){
        X = parseAutomataPrim(in,to,to,kleenestar(I));
        destroyAutomata(I);
        return X;
    }
    parseChar(in,to,to,'|');
    if (to != saveto){
        saveto = to;
        X = parseAutomata(in,to,to);
        if (to != saveto){
            automata* Y = parseAutomataPrim(in,to,to,unionAutomata(I,X));
            destroyAutomata(I);
            destroyAutomata(X);
            return Y;
        }
    }
    parseChar(in,from,to,'.');
    if (to != saveto){
        saveto = to;
        X = parseAutomata(in,to,to);
        if (to != saveto){
            automata* Y = parseAutomataPrim(in,to,to,concatAutomata(I,X));
            destroyAutomata(I);
            destroyAutomata(X);
            return Y;
        }
    }
    return I;

}

automata* parseAutomata(string in, int from,int& to){

    int saveto = to;
    int saveinito = to;

    automata* X;

    parseChar(in,from,to,'@');
    if (to != saveto){
        X=createEmptyAutomata(0);
        return parseAutomataPrim(in,to,to,X);
    }

    X = parseBrackets(in,from,to);
    if (to != saveto){
        return parseAutomataPrim(in,to,to,X);
    }

    X = parseSingleton(in,from,to);
    if (to != saveto){
        return parseAutomataPrim(in,to,to,X);
    }


    to = saveinito;
    return nullptr;
}
