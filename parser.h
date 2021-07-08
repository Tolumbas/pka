#ifndef PARSER
#define PARSER
#include <string>
string parseString(string in,int from, int& to);
void parseChar(string in,int from, int&to, int ch);
automata* parseUnion(string in, int from, int& to);
automata* parseConcat(string in, int from, int& to);
automata* parseAutomata(string in, int from,int& to);
#endif // PARSER
