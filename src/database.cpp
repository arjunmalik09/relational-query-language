#include "../headers/database.h"
#include "../headers/parse.h"
#include "../headers/relation.h"
#include <stdexcept>
#include <iostream>
#include <stack>
#include <fstream>
#include <iostream>
#include <cstring>
#define NAME_SIZE 1000
#define QUERY_SIZE 10000

using namespace std;
Database::Database(string name){
	this->name = name;
	this->table_no = 0;
	//load tables
	this->load();	
}
void Database::load(){
	//reads table names from rel_names file and loads tables.
	fstream file,name_file;
	name_file.open("../db/rel_names",ios::in);
	while(name_file){

		char name[NAME_SIZE];
		name_file.getline(name,NAME_SIZE);
		
		if(strlen(name) == 0)
			break;
		char path[NAME_SIZE] ="../db/";
		file.open(strcat(path,name),ios::in);
		file.seekg(0,ios::beg);
		
		vector<string> colmn_names;
		vector<string> colmn_dts;
		vector<vector<string> > rel;
		
		cout<<"loading: "<<name<<endl;
		//reading relation schema 
		deserialize(file,colmn_names);
		deserialize(file,colmn_dts);
		//reading relation data
		char end;
		while(file){			
			vector<string> v;
			deserialize(file,v);
			//printVec(v);
			file>>end;
			if(end == ',')
				file.seekg(-1,ios::cur);
			rel.push_back(v);
		}
		
		
		Relation R(name,rel,colmn_names,colmn_dts);
		/*R.desc();
		R.show();*/
		this->insert(R);
		file.close();
	}
	name_file.close();
}
void Database::insert(Relation Rel){
	//inserts table in db.
	if(this->rel2int.find(Rel.getname()) == this->rel2int.end()) {
		this->relations.push_back(Rel);
		this->rel2int[Rel.getname()] = this->table_no;
		this->table_no++;
	}
	else{
		throw invalid_argument("Error create0: Invalid create statement");
	}
}
void Database::query(string s){
	//parses and runs query
	//empty query
	if(s=="")
		return;

	//get tokens from query
	vector<string> v = tokenize(s);
	//call functions based on query
	if(v[0] == "create"){
			Relation R(v);
			this->insert(R);
	}
	else if(v[0] == "insert"){
		if(this->rel2int.find(v[1]) == this->rel2int.end()) {
			throw invalid_argument("Error query1: Invalid query statement");
		}
		else{
			relations[rel2int[v[1]]].insert(v);
		}
	}
	else if(v[0] == "show" && v[1] == "table"){
		if(this->rel2int.find(v[2]) == this->rel2int.end()) {
			throw invalid_argument("Error query1: Invalid query statement");
		}
		else{
			relations[rel2int[v[2]]].show();
		}
	}
	else if(v[0] == "desc"){
		if(this->rel2int.find(v[1]) == this->rel2int.end()) {
			throw invalid_argument("Error query1: Invalid query statement");
		}
		else{
			relations[rel2int[v[1]]].desc();
		}
	}
	else if(v[0] == "{"){
		//nested query
		this->nested_query(v);
	}
	else if(v[0] == "quit"){
		this->quit();
	}
	else if(v[0] == "input"){
		this->input(v);
	}
	
	else{
		this->execute(v,vector<Relation> (),map<string,int> ());
	}
}

void Database::nested_query(vector<string> v){
	//sample query:{{project {select student_grade with (id>2)} with id } join s_grade}
	//takes innermost query and executes it.
	//query is modified after execution.
	//recusively do above using stack
	vector<Relation> aux_rels;
	map<string,int> aux_rel2int;
	//aux_rels contian temp. relations for nested query
	stack<int> stk;			//stacks opening bracket's index
	
	vector<string> v_mod=v;
	int index;
	for(int i=0;i<v.size();i++){
		if(v[i]=="{")
			stk.push(i);
		if(v[i]=="}"){
			if(!stk.empty()){
				int j = stk.top();stk.pop();

				vector<string> v_exec;		//contains vector to be executed
				v_exec.insert(v_exec.end(),v.begin()+j+1,v.begin()+i);

				aux_rels.push_back(this->execute(v_exec,aux_rels,aux_rel2int));
				aux_rel2int[aux_rels[aux_rels.size()-1].getname()] = aux_rels.size()-1;

				vector<string> v_temp;
				for(int k=0;k<v.size();k++){
					if(k == j){
						v_temp.push_back(aux_rels[aux_rels.size()-1].getname());
						k = i;
						continue;
					}
					v_temp.push_back(v[k]);
				}
				v = v_temp;
				//printVec(v);
				i = j;					
			}
			else
				throw invalid_argument("Error nested_query7: Invalid nested_query statement. Incorrect Parantheses Arrangement.");
		}
	}
}


Relation Database::execute(vector<string> v,vector<Relation> aux_rels,map<string,int> aux_rel2int){
	//execute various operations
	//aux_rels contian temp. relations from nested query
	if(v[0] == "select"){
		if(this->rel2int.find(v[1]) != this->rel2int.end()) {
			return relations[rel2int[v[1]]].select(v);
		}
		else if(aux_rel2int.find(v[1]) != aux_rel2int.end()){
			return aux_rels[aux_rel2int[v[1]]].select(v);
		}
		else{
			throw invalid_argument("Error execute1: Invalid select statement");
		}
	}
	else if(v[0] == "project"){
		if(this->rel2int.find(v[1]) != this->rel2int.end()) {
			return relations[rel2int[v[1]]].project(v);
		}
		else if(aux_rel2int.find(v[1]) != aux_rel2int.end()){
			return aux_rels[aux_rel2int[v[1]]].project(v);
		}
		else{
			throw invalid_argument("Error execute2: Invalid project statement");
		}
	}

	else if(v[0] == "rename"){		
		if(this->rel2int.find(v[1]) != this->rel2int.end()) {
			return relations[rel2int[v[1]]].rename(v);
		}
		else if(aux_rel2int.find(v[1]) != aux_rel2int.end()){
			return aux_rels[aux_rel2int[v[1]]].rename(v);
		}
		else{
			throw invalid_argument("Error execute3: Invalid rename statement");
		}
	}
	else if(v[1] == "product"){
		if(this->rel2int.find(v[0]) != this->rel2int.end()){ 
			if(this->rel2int.find(v[2]) != this->rel2int.end()) 
				return cart_prod(relations[rel2int[v[0]]],relations[rel2int[v[2]]],v);
			if(aux_rel2int.find(v[2]) != aux_rel2int.end()) 
				return cart_prod(relations[rel2int[v[0]]],aux_rels[aux_rel2int[v[2]]],v);
		}
		else if(aux_rel2int.find(v[0]) != aux_rel2int.end()){
			if(this->rel2int.find(v[2]) != this->rel2int.end()) 
				return cart_prod(aux_rels[aux_rel2int[v[0]]],relations[rel2int[v[2]]],v);
			if(aux_rel2int.find(v[2]) != aux_rel2int.end()) 
				return cart_prod(aux_rels[aux_rel2int[v[0]]],aux_rels[aux_rel2int[v[2]]],v);
		}
		else{
			throw invalid_argument("Error execute4: Invalid product statement");
		}
	}
	else if(v[1] == "join"){
		if(this->rel2int.find(v[0]) != this->rel2int.end()){ 
			if(this->rel2int.find(v[2]) != this->rel2int.end()) 
				return join(relations[rel2int[v[0]]],relations[rel2int[v[2]]],v);
			if(aux_rel2int.find(v[2]) != aux_rel2int.end()) 
				return join(relations[rel2int[v[0]]],aux_rels[aux_rel2int[v[2]]],v);
		}
		else if(aux_rel2int.find(v[0]) != aux_rel2int.end()){
			if(this->rel2int.find(v[2]) != this->rel2int.end()) 
				return join(aux_rels[aux_rel2int[v[0]]],relations[rel2int[v[2]]],v);
			if(aux_rel2int.find(v[2]) != aux_rel2int.end()) 
				return join(aux_rels[aux_rel2int[v[0]]],aux_rels[aux_rel2int[v[2]]],v);
		}
		else{
			throw invalid_argument("Error execute5: Invalid join statement");
		}
	}
	else if(v[1] == "difference"){
		if(this->rel2int.find(v[0]) != this->rel2int.end()){ 
			if(this->rel2int.find(v[2]) != this->rel2int.end()) 
				return set_difference(relations[rel2int[v[0]]],relations[rel2int[v[2]]],v);
			if(aux_rel2int.find(v[2]) != aux_rel2int.end()) 
				return set_difference(relations[rel2int[v[0]]],aux_rels[aux_rel2int[v[2]]],v);
		}
		else if(aux_rel2int.find(v[0]) != aux_rel2int.end()){
			if(this->rel2int.find(v[2]) != this->rel2int.end()) 
				return set_difference(aux_rels[aux_rel2int[v[0]]],relations[rel2int[v[2]]],v);
			if(aux_rel2int.find(v[2]) != aux_rel2int.end()) 
				return set_difference(aux_rels[aux_rel2int[v[0]]],aux_rels[aux_rel2int[v[2]]],v);
		}
		else{
			throw invalid_argument("Error execute6: Invalid difference statement");
		}
	}
	else if(v[1] == "union"){
		if(this->rel2int.find(v[0]) != this->rel2int.end()){ 
			if(this->rel2int.find(v[2]) != this->rel2int.end()) 
				return set_union(relations[rel2int[v[0]]],relations[rel2int[v[2]]],v);
			if(aux_rel2int.find(v[2]) != aux_rel2int.end()) 
				return set_union(relations[rel2int[v[0]]],aux_rels[aux_rel2int[v[2]]],v);
		}
		else if(aux_rel2int.find(v[0]) != aux_rel2int.end()){
			if(this->rel2int.find(v[2]) != this->rel2int.end()) 
				return set_union(aux_rels[aux_rel2int[v[0]]],relations[rel2int[v[2]]],v);
			if(aux_rel2int.find(v[2]) != aux_rel2int.end()) 
				return set_union(aux_rels[aux_rel2int[v[0]]],aux_rels[aux_rel2int[v[2]]],v);
		}
		else{
			throw invalid_argument("Error execute7: Invalid union statement");
		}
	}
	else if(v[1] == "intersect"){
		if(this->rel2int.find(v[0]) != this->rel2int.end()){ 
			if(this->rel2int.find(v[2]) != this->rel2int.end()) 
				return intersection(relations[rel2int[v[0]]],relations[rel2int[v[2]]],v);
			if(aux_rel2int.find(v[2]) != aux_rel2int.end()) 
				return intersection(relations[rel2int[v[0]]],aux_rels[aux_rel2int[v[2]]],v);
		}
		else if(aux_rel2int.find(v[0]) != aux_rel2int.end()){
			if(this->rel2int.find(v[2]) != this->rel2int.end()) 
				return intersection(aux_rels[aux_rel2int[v[0]]],relations[rel2int[v[2]]],v);
			if(aux_rel2int.find(v[2]) != aux_rel2int.end()) 
				return intersection(aux_rels[aux_rel2int[v[0]]],aux_rels[aux_rel2int[v[2]]],v);
		}
		else{
			throw invalid_argument("Error execute8: Invalid intersection statement");
		}
	}	
}
void Database::store(){
	//saves all tables in relations vector in seperate files
	fstream file,name_file;
	name_file.open("../db/rel_names",ios::out | ios::trunc);

	for(int i=0;i<relations.size();i++){
		cout<<"stroing: "<<relations[i].getname()<<endl;
		name_file<<(relations[i].getname()+"\n");

		char path[NAME_SIZE] ="../db/";
		file.open(strcat(path,relations[i].getname().data()),ios::out | ios::trunc);
		
		//file.write((char*)&relations[i],sizeof(relations[i]));
		file.seekp(0,ios::end);
		serialize(file,relations[i].get_col_names());
		file<<"$";
		serialize(file,relations[i].get_col_dts());
		file<<"$";
		for(int j=0;j<relations[i].getsize();j++){
			serialize(file,relations[i].getrow(j));
			file<<"$";
		}
		file<<"\n";
		file.close();
	}
	name_file.close();
}
void Database::quit(){
	this->store();
	exit(0);
}

void Database::input(vector<string> v){
	//reads an input file containing rql statements
	if(v[0] != "input")
		throw invalid_argument("Error input: Invalid input statement");
	if(v.size() != 2)
		throw invalid_argument("Error input: Invalid input statement");
	fstream file;
	file.open(v[1],ios::in);
	char line[QUERY_SIZE];
	while(file){
		file.getline(line,QUERY_SIZE);
		string s = line;
		this->query(s);
	}
	
	file.close();
}
std::ostream& serialize(std::ostream &out,vector<string> v) {
    //to write strings to file
    for(int i=0;i<v.size();i++){
	    out << ','; //number seperator
	    out << v[i].size(); //serialize size of string
	    out << ',';
	    out << v[i]; //serialize characters of string
	}
    return out;
}
std::istream& deserialize(std::istream &in,vector<string> &v) {
    //to read strings from file
    if (in) {
    	v.clear();
        int len=0;
        char comma;
        int i=0;
        while(true){
	        in >> comma; //read in the seperator
	        if(comma == '$'){
	        	//printVec(v);
	        	break;
	        }
	        in >> len;  //deserialize size of string
	        
	        in >> comma; //read in the seperator
		        if (in && len) {
		            std::vector<char> tmp(len);
		            in.read(tmp.data() , len); //deserialize characters of string
		            v.push_back(string ());
		            v[i++].assign(tmp.data(), len);
		      
		        }
    	}
    }
    return in;
}