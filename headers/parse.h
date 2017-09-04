#ifndef PARSE_H
#define PARSE_H

#include <string>
#include <sstream>
#include <vector>
#include <locale>  // std::tolower
#include <algorithm>
#include <iostream>

using namespace std;
vector<string> split(const string &s, char delim);
void printVec(vector<string> v);
void split(const string &s, char delim, vector<string> &elems);
vector<string> split(const vector<string> &v, char delim);
vector<string> tokenize(string str);

#endif 