Relational Query Language (RQL) implements Relational Algebra operations on relational tables. It is a kind of a naive version of SQL supporting some standard commands given in section [Features](#features).


## Compiling
Ensure that source code directory is writable.
Run the following commands from src folder:
	```g++ -std=c++11 -c relation.cpp database.cpp parse.cpp```
	```g++ parse.o relation.o database.o main.cpp -o rql```

OR

Run compile.sh

## Run
Run rql file from src folder.

## Features
The following RQL statements can be used to create, query and modify database.
#### Standard Statements
```
1. create  tablename column_name1 datatype1, column_name2 datatype2,.....,  column_nameN datatypeN

2. insert tablename column_name1_value, column_name2_value,.....,  column_nameN_value

3. project tablename with column_name1 , column_name2 ,.....,  column_nameM 

4. select tablename with condition
	(where condition is fully bracketed.
	operands supported: ==,!=,>,<
	predicates can be joined using "and" or "or".
	)

5. rename old_tablename to new_tablename with column_name1 , column_name2 ,.....,  column_nameN 

6. tablename1 product tablename2

7. tablename1 join tablename2

8. tablename1 union tablename2

9. tablename1 intersect tablename2

10. tablename1 difference tablename2

11. input filename
(read commands from filename
sample: run "input query" to run sample query file in src folder 
)

12. show table tablename 
```
#### Nested query

Combine standard queries(except create,insert,input,show table) using braces(should be completely bracketed).
Example:
```{{project {select student_grade with (id>2)} with id } intersect {project student_hostel with id} }```

## File Structure
* Folder db contains files for database tables.
* Folder headers contains header files made.
* Folder src contains c++ files for implementation of header files and main.cpp. It also contains sample query file.