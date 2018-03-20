#pragma once
#include"stdafx.h"
#include"sym_table.h"
#include<stdlib.h>
#include<cstring>
#include<vector>
#include<string>
sym_table::sym_table()
{
	level_last[0] = 0;
	level_last[1] = 0;
	tablecnt = 0;
}
void sym_table::insert(const char* name1, int level1, int type1, int kind1, int special1, int size1)
{
	tablecnt++;
	if (level1 == 0) {
		table[tablecnt].prev = level_last[0];
		level_last[0] = tablecnt;
	}
	else {
		table[tablecnt].prev = tablecnt - 1;
		{
			if (table[tablecnt - 1].addroffset == 0)
				table[tablecnt].addroffset = 8;
			else
				table[tablecnt].addroffset = table[tablecnt - 1].addroffset + table[tablecnt - 1].size;
		}
		level_last[1] = tablecnt;
	}
	table[tablecnt].level = level1;
	table[tablecnt].type = type1;
	table[tablecnt].kind = kind1;
	table[tablecnt].special = special1;
	table[tablecnt].size = size1;
	table[tablecnt].name = (char*)malloc(strlen(name1));
	strcpy(table[tablecnt].name, name1);
}

int sym_table::search(const char * name,int level)
{
//	int level = level_last[1] > level_last[0] ? 1 : 0;
	int p = level_last[level];
	while (p > 0) {
		if (strcmp(table[p].name, name) == 0)
			return p;
		p = table[p].prev;
	}
	return 0;
}

int sym_table::checksame(const char *str,int level)
{
//	int level = level_last[1] > level_last[0] ? 1 : 0;
	int p = level_last[level];
	if (level == 1 && level_last[1] <= level_last[0])
		return 0;
	while (p > 0 && table[p].level==level) {
		if (strcmp(table[p].name, str) == 0)
			return p;
		p = table[p].prev;
	}
	return 0;
}

void sym_table::setaddroffset(int index, int offset)
{
	table[index].addroffset = offset;
}

void sym_table::output()
{
	int i;
	for (i = 1; i <= level_last[0] || i <= level_last[1]; i++)
		printf("name=%s\t\t\tlevel=%d\ttype=%d\tkind=%d\tspecial=%6d\toffset=%6d\tprev=%4d\t\n", table[i].name, table[i].level, table[i].type, table[i].kind, table[i].special, table[i].addroffset, table[i].prev);
	printf("\n\n\n");
	std::vector<std::string>::iterator it;
	for (it=strconst.begin(); it !=strconst.end(); it++)
		printf("%s\n", it->c_str());
}
