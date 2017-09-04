#include <iostream>
#include <vector>
#include <string>
#include <sys/types.h>
#include <sys/stat.h>

#include "../headers/relation.h"
#include "../headers/database.h"
#include "../headers/parse.h"

using namespace std;

int main()
{	
    mkdir("../db", S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);

    Database d("Database1");
    while(true){
    	string s;
    	cout<<">>"; 
		getline(cin, s);
    	//cout<<s;
    	d.query(s);
    }
    return 0;
}

//vector<string> v2 = tokenize("select { create student_grade name string , percent float , id int} with attr1>10 and attr2<20");     
/*vector<string> v1 = tokenize("create student_grade name string, percent float , id int");     
vector<string> v2 = tokenize("insert student_grade Arjun, 95.4 , 2");     
vector<string> v3 = tokenize("insert student_grade Janvijay, 97.6 , 3");     
vector<string> v4 = tokenize("insert student_grade Happy, 100.0 , 4");    
vector<string> v5 = tokenize("project student_grade with id");   
vector<string> v6 = tokenize("select student_grade with ((name==Arjun) and ((percent==95.4) or (id>1)))");  
vector<string> v7 = tokenize("select student_grade with (id>1)");     
vector<string> v8 = tokenize("select student_grade with ((percent>97.0) and (id>1))");   
vector<string> v9 = tokenize("rename student_grade to grade");     
vector<string> v10 = tokenize("rename student_grade to s_grade with name, percentage, id");   
vector<string> v11 = tokenize("student_grade product s_grade");     
vector<string> v12 = tokenize("student_grade join s_grade");     
vector<string> v13 = tokenize("{{project {select student_grade with (id>2)} with id } join s_grade}");      Relation R(v1);    
R.insert(v2);     R.insert(v3);     R.insert(v4);     R.show();     
R.project(v5);     Relation R2 = R.select(v6); 	Relation R3 = R.select(v7);     Relation R4 = R.select(v8);   
Relation R5 = R.rename(v9);     Relation R6 = R.rename(v10);      
Relation R7 = cart_prod(R,R6,v11);     R6.insert(tokenize("insert s_grade Amit, 98 , 5"));    
Relation R8 = join(R,R6,v12);     Relation R9 = intersection(R,R6,vector<string> ());   
Relation R10 = set_union(R,R6,vector<string> ());    
Relation R11 = set_difference(R6,R,vector<string> ());*/   
/*Database d("Database1");     d.query("create student_grade name string, percent float , id int");  
d.query("insert student_grade Arjun, 95.4 , 2");    
d.query("insert student_grade Janvijay, 97.6 , 3");     d.query("insert student_grade Happy, 100.0 , 4");    
d.query("{project {select student_grade with (id>2)} with id }");    
d.query("create student_hostel id int, hostel_name string");     d.query("insert student_hostel 4, ramanujan");  
d.query("insert student_hostel 3, cvr");     d.query("insert student_hostel 2, limbdi");     
d.query("{{project {select student_grade with (id>2)} with id } join student_hostel}");     

d.query("{{project {select student_grade with (id>2)} with id } union {project student_hostel with id} }");     
d.query("show table student_grade");          d.quit();     */     
