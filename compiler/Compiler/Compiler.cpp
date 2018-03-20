// compiler.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include"stdio.h"
#include<iostream>
#include <stdlib.h>
#include<ctype.h>
#include "dictionary.h"
#include "lex.h"
#include "syntax.h"



int main(int argc, char* argv[])
{
	FILE *fp;
	system("cd");
	char filename[233],buf[233];
	printf("Please input the source file:");

	scanf("%s", filename);
//	strcpy(filename, "d:\\test.txt");

	FILE* file = fopen(filename, "r");
	printf("Please specify the output dir:");
	
	scanf("%s", filename);
	//strcpy(filename, "d:\\syntax_out.txt");


	syntax syn;
	syn.program(file,filename);

	sprintf(buf, "%smips.asm", filename);
	fp = fopen(buf, "w");
	syn.mips(fp);
	char ch;
	ch = getchar();
	return 0;
}
