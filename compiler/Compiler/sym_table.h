#pragma once
#include<cstring>
#include<stdlib.h>
#include<vector>
#include<string>
#include"lex.h"
#define TYPE_INT			21001
#define TYPE_CHAR		21002
#define TYPE_STRING		21003
#define KIND_VAR			22001
#define KIND_CONST		22002
#define KIND_PROC		22003
#define KIND_FUNC		22004
#define KIND_LABLE		22005


struct sym_term {
public:
	char *name;
	int level, type,kind,special;
	int prev;
	int size;
	int addroffset;
};

class sym_table {
public:
	int level_last[2] ;
	sym_term table[10000];
	std::vector<std::string> strconst;
	int tablecnt;
	sym_table();
	void insert(const char* name1,int level1,int type1, int kind1, int special1, int size1);
	int search(const char *name,int level);
	int checksame(const char *str,int level);
	void setaddroffset(int index, int offset);
	void output();
};