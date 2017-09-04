#include "../headers/parse.h"
#include <string>
#include <sstream>
#include <vector>
#include <locale>  // std::tolower
#include <algorithm>
#include <iostream>

using namespace std;
vector<string> split(const string &s, char delim);
void printVec(vector<string> v){
	cout<<"Vector: ";
	for(int i=0;i<v.size();i++){
		cout<<'"'<<v[i]<<'"'<<' ';
	}
	cout<<endl;
}

void split(const string &s, char delim, vector<string> &elems) {
    stringstream ss;
    ss.str(s);
    string item;
    while (getline(ss, item, delim)) {
        transform(item.begin(), item.end(), item.begin(), ::tolower);
        elems.push_back(item);
    }
}


vector<string> split(const string &s, char delim) {
    //split a string by vector
    vector<string> elems;
    split(s, delim, elems);
    return elems;
}

vector<string> split(const vector<string> &v, char delim){
	//split each string vector by delim and return complete vector.
	vector<string> words;
	string delim_s;
	delim_s.push_back(delim);
	
	for(int i=0;i<v.size();i++){
		if(v[i]==delim_s){
			words.push_back(v[i]);
			continue;
		}

		vector<string> w = split(v[i],delim);
		//printVec(w);
		for(int j=0;j<w.size()-1;j++){
			if(w[j].length()!=0)
				words.push_back(w[j]);
			words.push_back(delim_s);
		}
		if(w[w.size()-1].length()!=0)
			words.push_back(w[w.size()-1]);

		if(v[i][v[i].size()-1] == delim){
			words.push_back(delim_s);
		}
	}

	return words;
}

vector<string> tokenize(string str){
	vector<string> word = split(str,' ');
	word = split(word,',');
	//printVec(word);
	word = split(word,'(');
	word = split(word,')');
	word = split(word,'{');
	word = split(word,'}');
	word = split(word,'=');
	word = split(word,'!');
	word = split(word,'<');
	word = split(word,'>');
	//printVec(word);
	return word;
}
