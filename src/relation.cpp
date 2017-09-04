#include "../headers/relation.h"
#include "../headers/parse.h"
#include <stdexcept>
#include <iostream>
#include <stack>
#include <unordered_set>
#include <unordered_map>
#include <functional>	//hash
#include <cstring>

using namespace std;


Relation::Relation(){

};
Relation::Relation(string name,vector<vector<string> > r, vector<string> col_names, vector<string> col_dts){
	this->name = name;
	this->r = r;
	this->col_names = col_names;
	this->col_dts = col_dts;
	for(int i=0;i<col_names.size();i++)
		this->col2int[col_names[i]] = i;
}
Relation::Relation(string name,vector<vector<string> > r, vector<string> col_names, vector<string> col_dts,map<string,int> col2int){
	this->name = name;
	this->r = r;
	this->col_names = col_names;
	this->col_dts = col_dts;
	this->col2int = col2int;
}
Relation::Relation(vector<string> crt){
	//constructor for a create statement
	if(crt[0] != "create")
		throw invalid_argument("Error in create: Not a create statement 1");
	
	if(crt.size()<4)
		throw invalid_argument("Error in create: Invalid create statement 2");

	this->name = crt[1];
	int k = 0;
	
	for(int i=2;i<crt.size();i++){
		//add colomn name and mapping of colomn name to it's index for each colomn
		col_names.push_back(crt[i]);
		col2int[crt[i]] = k++;

		//add colomn name datatypes
		if(i+1<crt.size() && (crt[i+1]=="int" || crt[i+1]=="float" || crt[i+1]=="string")){
			col_dts.push_back(crt[i+1]);
			i++;
		}
		else		
			throw invalid_argument("Error in create: Invalid create statement 3");
		
		if(i+1<crt.size() && crt[i+1]!=",")		
			throw invalid_argument("Error in create: Invalid create statement 4");
		else
			i++;
	}
}
string Relation::getname(){
	return this->name;
}
int Relation::getsize(){
	return this->r.size();
}
vector<string> Relation::getrow(int i){
	return this->r[i];
}
vector<string> Relation::get_col_names(){
	return this->col_names;
}

vector<string> Relation::get_col_dts(){
	return this->col_dts;
}
void Relation::desc(){
	//describe table
	if(col_names.size() != col_dts.size())
		throw invalid_argument("Error in desc: Not a describe statement");
	cout<<"TABLE NAME:"<<this->name<<endl;
	for(int i=0;i<col_names.size();i++){
		cout<<"| "<<col_names[i]<<" | "<<col_dts[i]<<" |"<<endl;
	}
}
void Relation::show(){
	//show table's data
	//cout<<r[2].size()<<" "<<col_names.size()<<endl;
	cout<<"TABLE DATA:"<<endl;
	for(int i = 0; i<r.size(); i++){
		cout<<"| ";
		for(int j=0;j<col_names.size();j++){
			cout<<r[i][j]<<" | ";
		}
		cout<<endl;
	}
}
void Relation::insert(vector<string> ist){
	if(ist[0] != "insert")
		throw invalid_argument("Error in insert: Not a insert statement 1");
	
	if(ist.size()<1+col_names.size()*2)
		throw invalid_argument("Error in insert: Invalid insert statement 2");

	if(ist[1] != this->name)
		throw invalid_argument("Error in insert: Invalid insert statement 3");

	int k = 0;
	vector<string> v;
	for(int i=2;i<ist.size();i++){
		//does not check for type of string
		v.push_back(ist[i]);
		
		if(i+1<ist.size() && ist[i+1]!=",")		
			throw invalid_argument("Error in insert: Invalid insert statement 4");
		else
			i++;
	}
	r.push_back(v);
}

struct hash_vector {
	std::hash<std::string> str_hash;
    inline size_t operator()(const  vector<string> & v) const {
        string s;
        for(int i=0;i<v.size();i++){
        	s.append(v[i]);
        }
        return str_hash(s);
    }
};

Relation Relation::project(vector<string> pjt){
	if(pjt[0] != "project")
		throw invalid_argument("Error project1: Not a project statement");
	
	if(pjt.size()<4)
		throw invalid_argument("Error project2: Invalid project statement");

	if(pjt[1] != this->name || pjt[2] != "with")
		throw invalid_argument("Error project3: Invalid project statement");
	
	vector<string> colmn_names;		//for schema 
	vector<string> colmn_dts;
	vector<vector<string> > rel;	//for data
	vector<int> indices;			//index for each column in pjt
	unordered_set<vector<string>,hash_vector> exists;	//to check no 2 rows are same (set algebra)

	for(int i=3;i<pjt.size();i++){
		if(this->col2int.find(pjt[i])!=this->col2int.end()){

			indices.push_back(this->col2int[pjt[i]]);
			colmn_names.push_back(pjt[i]);
			colmn_dts.push_back(col_dts[this->col2int[pjt[i]]]);
		}
		else{
			throw invalid_argument("Error project4: Invalid project statement. Column "+pjt[i]+" not found.");
		}
				
		if(i+1<pjt.size() && pjt[i+1]!=",")		
			throw invalid_argument("Error project5: Invalid project statement");
		else
			i++;
	}
	
	for(int i=0;i<r.size();i++){
		vector<string> v;
		for(int j=0;j<indices.size();j++){
			v.push_back(r[i][indices[j]]);
		}
		if(exists.find(v) == exists.end()){
			exists.insert(v);
			rel.push_back(v);	
		}
	}

	Relation R("Project:"+this->name,rel,colmn_names,colmn_dts);
	R.desc();
	R.show();
	return R;
}

void Relation::operate(string a1,string a2,string op,vector<stack<bool> > &stacks){
	//given attributes of table and operator, exectues on each row to check if predicate is true
	if(op!="==" && op!="!=" && op!="<" && op!=">") 
		throw invalid_argument("Error operate1: Invalid operate statement. Operator "+op+" not found.");
	if(this->col2int.find(a1)==this->col2int.end())
		throw invalid_argument("Error operate1: Invalid operate statement. Column "+a1+" not found.");
	
	int x=col2int[a1];
	string datatype = col_dts[x];
	//cout<<"x"<<x<<endl;

	if(this->col2int.find(a2)==this->col2int.end()){
		//check a2 is constant of same type
		//if a2 is a constant
		if(datatype == "int" || datatype == "float" ){
			double a2_d = atof(a2.data());
			//if (*endptr) throw invalid_argument("Error operate7: Invalid operate statement. Invalid values in attributes.");
			//cout<<"a1_d"<<a2_d<<endl;
			for (int i = 0; i<r.size(); i++){
					double a1_d = atof(r[i][x].data());
					if(op=="=="){
						if(a1_d==a2_d)
							stacks[i].push(true);
						else
							stacks[i].push(false);
					}
					else if(op=="!="){
						if(a1_d!=a2_d)
							stacks[i].push(true);
						else
							stacks[i].push(false);

					}
					else if(op=="<"){
						if(a1_d<a2_d)
							stacks[i].push(true);
						else
							stacks[i].push(false);
					}
					else if(op==">"){
						if(a1_d>a2_d)
							stacks[i].push(true);
						else
							stacks[i].push(false);
					}
			}
		}
		else{
			for (int i = 0; i<r.size(); i++){
					if(op=="=="){
						if(r[i][x].compare(a2)==0)
							stacks[i].push(true);
						else
							stacks[i].push(false);
					}
					else if(op=="!="){
						if(r[i][x].compare(a2)!=0)
							stacks[i].push(true);
						else
							stacks[i].push(false);

					}
					else if(op=="<"){
						if(r[i][x].compare(a2)<0)
							stacks[i].push(true);
						else
							stacks[i].push(false);
					}
					else if(op==">"){
						if(r[i][x].compare(a2)>0)
							stacks[i].push(true);
						else
							stacks[i].push(false);
					}
			}
		}

	}
	else{
		//if a2 is a attrib.
		int y=col2int[a2];
		if(datatype == "int" || datatype == "float" ){
			
			for (int i = 0; i<r.size(); i++){
					double a1_d = atof(r[i][x].data());
					double a2_d = atof(r[i][y].data());
					//cout<<"a1_d"<<a1_d<<endl;
					if(op=="=="){
						if(a1_d==a2_d)
							stacks[i].push(true);
						else
							stacks[i].push(false);
					}
					else if(op=="!="){
						if(a1_d!=a2_d)
							stacks[i].push(true);
						else
							stacks[i].push(false);

					}
					else if(op=="<"){
						if(a1_d<a2_d)
							stacks[i].push(true);
						else
							stacks[i].push(false);
					}
					else if(op==">"){
						if(a1_d>a2_d)
							stacks[i].push(true);
						else
							stacks[i].push(false);
					}
			}
		}
		else{
			for (int i = 0; i<r.size(); i++){
					if(op=="=="){
						if(r[i][x].compare(r[i][y])==0){
							stacks[i].push(true);
						}
						else
							stacks[i].push(false);
					}
					else if(op=="!="){
						if(r[i][x].compare(r[i][y])!=0)
							stacks[i].push(true);
						else
							stacks[i].push(false);

					}
					else if(op=="<"){
						if(r[i][x].compare(r[i][y])<0)
							stacks[i].push(true);
						else
							stacks[i].push(false);

					}
					else if(op==">"){
						if(r[i][x].compare(r[i][y])>0)
							stacks[i].push(true);
						else
							stacks[i].push(false);
					}
			}
		}
	}
	/*cout<<a1<<" "<<a2<<" "<<op<<endl;
	for(int k = 0; k<r[0].size(); k++)
		cout<<stacks[k].top();
	cout<<endl;*/
	
}


Relation Relation::select(vector<string> slt){
	if(slt[0] != "select")
		throw invalid_argument("Error select1: Not a select statement");
	
	if(slt.size()<6)
		throw invalid_argument("Error select2: Invalid select statement");

	if(slt[1] != this->name || slt[2] != "with")
		throw invalid_argument("Error select3: Invalid select statement");
	if(r.size() == 0 || r[0].size() == 0)
		throw invalid_argument("Error select4: Invalid select statement");
	
	vector<vector<string> > rel;
	stack<int> stk;			//stacks opening bracket's index
	map<int, int> ind;		//maps closing bracket's index to it's opening index
	
	vector<stack<bool> > truth;		//stores the truth value upto currently evaluated select predicate for each row
	for (int i = 0; i<r.size(); i++){
		stack<bool> s;
		truth.push_back(s);
	}
	//evalutaing conditon on each row
	for(int i=3;i<slt.size();i++){
		if(slt[i]=="(")
			stk.push(i);
		if(slt[i]==")"){
			if(!stk.empty()){
				int j = stk.top();stk.pop();
				//cout<<")"<<" found at "<<i<<" with corresponding "<<j<<endl;
				ind[j] = i;
				
				//if brackets contain and/or
				//check inner braket ((...) and/or (...))
				if(slt[j+1]=="("){
					//pop partial truth values and take their and/or  ... and push those 
					if( slt[ind[j+1]+1]=="and" ){
						for(int k = 0; k<r.size(); k++){
							bool b1 = truth[k].top();truth[k].pop(); 
							bool b2 = truth[k].top();truth[k].pop();
							truth[k].push(b1 && b2);
						}
					}
					else if( slt[ind[j+1]+1]=="or" ){
						for(int k = 0; k<r.size(); k++){
							bool b1 = truth[k].top();truth[k].pop(); 
							bool b2 = truth[k].top();truth[k].pop();
							truth[k].push(b1 || b2);
						}
					}
					else{
						//cout<<"Bracket before "<<slt[j+2]<<" At "<<ind[j+1]<<" found "<<slt[ind[j+1]]<<endl;
						throw invalid_argument("Error select5: Invalid select statement. Incorrect Parantheses Arrangement.");
					}
					//cout<<"After and or "<<truth[0].top()<<endl;
					continue;
				}

				//parse and evaluate basic predicates -> (opnd op opnd).
				j++;
				int attr1=j,attr2;
				string op1;
				j++;
				while(j+1<i){
					op1.append(slt[j]);
					
					j++;
				}
				attr2 = j;
				j++;
				//cout<<slt[attr1]<<endl;cout<<slt[attr2]<<endl;cout<<op1<<endl;
				
				
				this->operate(slt[attr1],slt[attr2],op1,truth);
					
			}
			else
				throw invalid_argument("Error select7: Invalid select statement. Incorrect Parantheses Arrangement.");
		}		
	}
	//cout<<truth[0].size()<<endl;

	if(truth[0].size() != 1 )
		throw invalid_argument("Error select8: Invalid select statement. Incorrect Parantheses Arrangement.");

	for(int k = 0; k<r.size(); k++)
			if(truth[k].top())
				rel.push_back(r[k]);

	Relation R("Select:"+this->name,rel,this->col_names,this->col_dts,this->col2int);
	R.desc();
	R.show();
	return R;
}

Relation Relation::rename(vector<string> rnm){
	if(rnm[0] != "rename")
		throw invalid_argument("Error rename1: Not a rename statement");
	
	if(rnm[1] != this->name || rnm[2] != "to")
		throw invalid_argument("Error rename2: Invalid rename statement");

	Relation R;
	//rename table1 to table2
	if(rnm.size() == 4){
		if(rnm[1] != this->name || rnm[2] != "to")
			throw invalid_argument("Error rename3: Invalid rename statement");
		R = Relation(rnm[3],this->r,this->col_names,this->col_dts);	
	}
	
	//rename table1 to table2 with col1,col2,....,coln
	else{
		if(rnm.size()<5+this->col_names.size())
			throw invalid_argument("Error rename4: Invalid rename statement");
	
		if(rnm[4] != "with")
			throw invalid_argument("Error rename5: Invalid rename statement");

		vector<string> colmn_names;
		
		int k=0;
		for(int i=5;i<rnm.size();i++){
			
			colmn_names.push_back(rnm[i]);
			if(i+1<rnm.size() && rnm[i+1]!=",")		
				throw invalid_argument("Error rename6: Invalid rename statement");
			else
				i++;
		}
		
		R = Relation(rnm[3],this->r,colmn_names,this->col_dts);
	}
	R.desc();
	R.show();
	return R;
}
vector<pair<int,int> > get_common_atr(Relation r1,Relation r2){
	vector<pair<int,int> > com_ind;
	unordered_set<string> exists;
	for(int i=0;i<r1.col_names.size();i++)
		exists.insert(r1.col_names[i]);
	for(int i=0;i<r2.col_names.size();i++)
		if(exists.find(r2.col_names[i]) != exists.end())
			com_ind.push_back(make_pair(r2.col2int[r2.col_names[i]],i));
	return com_ind;
}
Relation cart_prod(Relation r1,Relation r2,vector<string> pdt){
	
	if(pdt[1] != "product")
		throw invalid_argument("Error cart_prod1: Not a cartesian product statement");
	
	if(pdt.size() != 3)
		throw invalid_argument("Error cart_prod2: Invalid cartesian product statement");

	if(pdt[0] != r1.name || pdt[2] != r2.name)
		throw invalid_argument("Error cart_prod3: Invalid cartesian product statement");
	
	vector<pair<int,int> > com_ind = get_common_atr(r1,r2);
	//rename
	Relation re_r1 = r1, re_r2 =r2;
	for(int i = 0; i < com_ind.size(); ++i){
			re_r1.col_names[com_ind[i].first] = re_r1.name + "." + re_r1.col_names[com_ind[i].first];
			re_r2.col_names[com_ind[i].second] = re_r2.name + "." + re_r2.col_names[com_ind[i].second];
	}
	
	//take product
	vector<string> colmn_names;
	vector<string> colmn_dts;
	vector<vector<string> > rel;
	
	for(int i=0;i<re_r1.col_names.size();i++){
		colmn_names.push_back(re_r1.col_names[i]);
		colmn_dts.push_back(re_r1.col_dts[i]);
	}
	for(int j=0;j<re_r2.col_names.size();j++){
		colmn_names.push_back(re_r2.col_names[j]);
		colmn_dts.push_back(re_r2.col_dts[j]);	
	}
	
	for(int i=0;i<re_r1.r.size();i++){
		for(int j=0;j<re_r2.r.size();j++){
			vector<string> v;
			v.insert(v.end(),re_r1.r[i].begin(),re_r1.r[i].end());
			v.insert(v.end(),re_r2.r[j].begin(),re_r2.r[j].end());
			rel.push_back(v);
		}
	}
	
	Relation R("Product:"+re_r1.name+"*"+re_r2.name,rel,colmn_names,colmn_dts);
	R.desc();
	R.show();
	return R;
} 

Relation join(Relation r1,Relation r2,vector<string> jn){
	if(jn[1] != "join")
		throw invalid_argument("Error join1: Not a join statement");
	
	if(jn.size() != 3)
		throw invalid_argument("Error join2: Invalid join statement");

	if(jn[0] != r1.name || jn[2] != r2.name)
		throw invalid_argument("Error join3: Invalid join statement");
	
	vector<pair<int,int> > com_ind = get_common_atr(r1,r2);
	
	//add funtionality to return cross product when no common attribute

	//add column names
	vector<string> colmn_names;
	vector<string> colmn_dts;
	vector<vector<string> > rel;
	
	for(int i=0;i<r1.col_names.size();i++){
		colmn_names.push_back(r1.col_names[i]);
		colmn_dts.push_back(r1.col_dts[i]);
	}
	int k = 0;
	for(int j=0;j<r2.col_names.size();j++){
		if(j == com_ind[k].second){
			k++;
			if(k>=com_ind.size()){
				k = com_ind.size()-1;
			}
			continue;
		}
		colmn_names.push_back(r2.col_names[j]);
		colmn_dts.push_back(r2.col_dts[j]);	
	}
	printVec(colmn_names);
	//join on common attributes
	
	unordered_map<vector<string >,vector<int>,hash_vector > r2_map;
	for(int j=0;j<r2.r.size();j++){
		vector<string> v;
		for(int k=0;k<com_ind.size();k++){
			v.push_back(r2.r[j][com_ind[k].second]);
		}
		if(r2_map.find(v) == r2_map.end()){
			vector<int> u;
			u.push_back(j);
			r2_map[v] = u;
		}
		else
			r2_map[v].push_back(j);
	}
	
	for(int i=0;i<r1.r.size();i++){
		vector<string> u,v;
		for(int k=0;k<com_ind.size();k++)
			u.push_back(r1.r[i][com_ind[k].first]);
		
		for(int j=0;j<r2_map[u].size();j++){
			int ind = r2_map[u][j];
			v.insert(v.end(),r1.r[i].begin(),r1.r[i].end());
			
			int k = 0;
			for(int l=0;l<r2.col_names.size();l++){
				if(l == com_ind[k].second){
					k++;
					if(k>=com_ind.size()){
						k = com_ind.size()-1;
					}	
					continue;
				}
				v.push_back(r2.r[ind][l]);
			}
			rel.push_back(v);
		}
	}
	
	Relation R("Join:"+r1.name+"<>"+r2.name,rel,colmn_names,colmn_dts);
	R.desc();
	R.show();
	return R;
}
bool check_compatibility(Relation r1,Relation r2){
	//check arity
	if(r1.col_dts.size() != r2.col_dts.size())
		return false;
	
	//check domain compatibility
	for(int i=0;i<r1.col_dts.size();i++)
		if(r1.col_dts[i] != r2.col_dts[i])
			return false;

	return true;
}
Relation set_difference(Relation &r1,Relation &r2,vector<string> dif){
	if(dif.size() == 0){
		dif.push_back(r1.name);
		dif.push_back("difference");
		dif.push_back(r2.name);
	}
	else{
		if(dif[1] != "difference")
			throw invalid_argument("Error difference1: Not a difference statement");

		if(dif.size() != 3)
			throw invalid_argument("Error difference2: Invalid difference statement");

		if(dif[0] != r1.name || dif[2] != r2.name)
			throw invalid_argument("Error difference3: Invalid difference statement");
	}
	if(!check_compatibility(r1,r2))
		throw invalid_argument("Error difference4: Relations difference incompatible");

	Relation r3 = intersection(r1,r2,vector<string> ());
	vector<vector<string> > rel;
	int k=0;
	//add r1 tuples if they don't exist in intersection
	//r3 is ordered by r1
	for(int i=0;i<r1.r.size();i++){
		if( r1.r[i] == r3.r[k] ){
			k++;
			if(k>=r3.r.size()){
				k = r3.r.size()-1;
			}	
			continue;
		}
		rel.push_back(r1.r[i]);
	}
	
	Relation R("Difference:"+r1.name+" - "+r2.name,rel,r1.col_names,r1.col_dts);
	R.desc();
	R.show();
	return R;	
}
Relation set_union(Relation &r1,Relation &r2,vector<string> un){
	if(un.size() == 0){
		un.push_back(r1.name);
		un.push_back("union");
		un.push_back(r2.name);
	}
	else{
	if(un[1] != "union")
		throw invalid_argument("Error union1: Not a union statement");
	
	if(un.size() != 3)
		throw invalid_argument("Error union2: Invalid union statement");

	if(un[0] != r1.name || un[2] != r2.name)
		throw invalid_argument("Error union3: Invalid union statement");
	}
	if(!check_compatibility(r1,r2))
		throw invalid_argument("Error union4: Relations union incompatible");

	Relation r3 = intersection(r2,r1,vector<string> ());
	
	//add r1 tuples
	vector<vector<string> > rel = r1.r;
	
	int k=0;
	//add r2 tuples if they don't exist in intersection
	//r3 is ordered by r2

	for(int i=0;i<r2.r.size();i++){
		if( r2.r[i] == r3.r[k] ){
			k++;
			if(k>=r3.r.size()){
				k = r3.r.size()-1;
			}	
			continue;
		}
		rel.push_back(r2.r[i]);
	}
	
	Relation R("Union:"+r1.name+" U "+r2.name,rel,r1.col_names,r1.col_dts);
	R.desc();
	R.show();
	return R;
}

Relation intersection(Relation &r1,Relation &r2,vector<string> irt){
	//map r2 nad check if rows of r1 in r2
	if(irt.size() == 0){
		irt.push_back(r1.name);
		irt.push_back("intersect");
		irt.push_back(r2.name);
	}
	else{
		//cout<<"IN intersect"<<endl;
		if(irt[1] != "intersect")
			throw invalid_argument("Error intersect1: Not a intersect statement");
		
		if(irt.size() != 3)
			throw invalid_argument("Error intersect2: Invalid intersect statement");

		if(irt[0] != r1.name || irt[2] != r2.name)
			throw invalid_argument("Error intersect3: Invalid intersect statement");
	}
	if(!check_compatibility(r1,r2))
		throw invalid_argument("Error intersect4: Relations intersect incompatible");

	int i,j;
	
	if(r2.r_map.size() != r2.r.size()){
		for(i=r2.r.size()-1;i>=0;i--){
			if(r2.r_map.find(r2.r[i]) != r2.r_map.end())
				break;
			r2.r_map[r2.r[i]] = i;
		}
	}

	if(r2.r_map.size() != r2.r.size())
		throw invalid_argument("Error intersect4: Relations could not be mapped for intersect.");

	vector<vector<string> > rel;

	for(int i=0;i<r1.r.size();i++){
		//see if row of r1 exists in r2 and then add it. 
		if(r2.r_map.find(r1.r[i]) != r2.r_map.end()){
			rel.push_back(r1.r[i]);
		}
	}

	Relation R("Intersect:"+r1.name+" ^ "+r2.name,rel,r1.col_names,r1.col_dts);
	R.desc();
	R.show();
	return R;
}	
