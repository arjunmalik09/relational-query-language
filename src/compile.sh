g++ -std=c++11 -c relation.cpp database.cpp parse.cpp
g++ parse.o relation.o database.o main.cpp -o rql
