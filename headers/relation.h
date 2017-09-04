#ifndef RELATION_H
#define RELATION_H

#include <vector>
#include <string>
#include <map>
#include <stack>

using namespace std;
class Relation;

Relation cart_prod(Relation,Relation,vector<string>);
Relation join(Relation,Relation,vector<string>);
Relation set_difference(Relation &r1,Relation &r2,vector<string> dif);
Relation set_union(Relation &r1,Relation &r2,vector<string> un);
Relation intersection(Relation &r1,Relation &r2,vector<string> irt);
vector<pair<int,int> > get_common_atr(Relation,Relation);

class Relation
{
	string name;
	vector<string> col_names;
	vector<string> col_dts;
	vector<vector<string> > r;
	map<string,int> col2int;
	map<vector<string >, int> r_map; 
public:
	Relation();
	Relation(string name,vector<vector<string> > r, vector<string> col_names, vector<string> col_dts);
	Relation(string name,vector<vector<string> > r, vector<string> col_names, vector<string> col_dts,map<string,int> col2int);
	Relation(vector<string>);
	void show();
	void desc();
	string getname();
	int getsize();
	vector<string> getrow(int);
	vector<string> get_col_names();
	vector<string> get_col_dts();
	void insert(vector<string>);
	Relation select(vector<string>);
	Relation project(vector<string>);
	Relation rename(vector<string>);
	void operate(string a1,string a2,string op,vector<stack<bool> > &stacks);
	friend Relation cart_prod(Relation,Relation,vector<string>);
	friend Relation join(Relation,Relation,vector<string>);
	friend Relation set_union(Relation &r1,Relation &r2,vector<string> un);
	friend Relation set_difference(Relation &r1,Relation &r2,vector<string> dif);
	friend Relation intersection(Relation &r1,Relation &r2,vector<string> irt);
	friend vector<pair<int,int> > get_common_atr(Relation,Relation);
	friend bool check_compatibility(Relation,Relation);
};
#endif