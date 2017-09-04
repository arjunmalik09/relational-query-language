#ifndef DATABASE_H
#define DATABASE_H
#include "relation.h"
#include <vector>
#include <string>
#include <map>
using namespace std;
std::istream& deserialize(std::istream &in,vector<string> &v);
std::ostream& serialize(std::ostream &out,vector<string> v);
class Database
{
	string name;
	vector<Relation> relations;
	map<string,int> rel2int;
	int table_no;
public:
	Database(string);
	void load();
	void store();
	void show();
	void insert(Relation);
	void query(string s);
	void nested_query(vector<string>);
	Relation execute(vector<string> v,vector<Relation>,map<string,int> rel2int);
	void input(vector<string>);
	void quit();
};


#endif