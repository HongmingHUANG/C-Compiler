#pragma once
#include"stdafx.h"
#include "syntax.h"
#include "sym_table.h"
#include"lex.h"
#include<stdio.h>
#include<cstring>
#include<vector>
#include<string>
#include<stdlib.h>

char buf[2333];
four_table fours;
sym_table symtable;
int is_func;
char tmp_ident[30];
int  para_cnt = 0;
int is_main = 0;
int tmp_const = 0;
int tmp_head = 0;
unsigned int tmp_uconst = 0;
int nowexlevel = 0;
int maxexlevel = 0;
int nowswlevel = 0;
int maxswlevel = 0;
int global_syntax_err = 0, global_file_end = 0;
int while_cnt = 0, if_cnt = 0, swit_cnt = 0, case_cnt = 0,string_cnt=0;
int main_maxoffset;

void  four::newfour(int type1, int src11, int src21, int des1)
{
	type = type1;
	src1 = src11;
	src2 = src21;
	des = des1;
	if (type1 == FOUR_NOP)
		is_block = 1;
	else
		is_block = 0;
}

const char * four::typestr()
{
	switch (type) {
	case FOUR_NOP: return "nop";
	case FOUR_MOV:return "mov";
	case FOUR_ADD:return "add";
	case FOUR_SUB:return "sub";
	case FOUR_MUL:return "mul";
	case FOUR_DIV:return "div";
	case FOUR_BIG:return "big";
	case FOUR_LESS:return "less";
	case FOUR_GEQ:return "geq";
	case FOUR_LEQ:return "leq";
	case FOUR_SAME:return "same";
	case FOUR_NEQ:return "neq";
	case FOUR_JUMP:return "j";
	case FOUR_BEQZ:return "beqz";
	case FOUR_SCANF:return "scanf";
	case FOUR_PRINTF:return "printf";
	case FOUR_RETURN:return "return";
	case FOUR_LABEL:return "label";
	case FOUR_GETARR:return "getarr";
	case FOUR_SETARR:return "setarr";
	case FOUR_LI:return "li";
	case FOUR_VERSE:return "verse";
	case FOUR_CALL:return "call";
	case FOUR_PUSH:return "push";
	case FOUR_GETPC:return "getpc";
	case FOUR_END:return "end";
	case FOUR_GRETURN:return "getreturn";
	case FOUR_BNE:return "bne";
	case FOUR_PUSHFP:return "pushfp";
	}
	return NULL;
}

void syntax::error()
{
	global_syntax_err = 1;
	printf("There is an syntax error at line %d!\n",lexer.lineno());
}

int syntax::const_declare()
//have red a token "const"
{	
	while (tk == TK_CONST) {
		lexer.nextsym();
		sym = lexer.text();
		tk = lexer.type();
		if (tk != TK_TYPINT && tk != TK_TYPCHAR) {
			puts("Illegal const type! Should be int or char!");
			error();
			return SYN_FAIL;
		}
		if (const_define(tk)==SYN_FAIL) {
			return SYN_FAIL;
		}
		lexer.nextsym();
		sym = lexer.text();
		tk = lexer.type();
	}
//	printf("A const declare part has been analyzed.\n");

	return SYN_SUCCESS;
}

int syntax::const_define(int type)
{
	//have red a token "int" or "char"
	char buf[23];
	int tmp_type;
	int fail_flag = 0;
	//TODO different deal with token INT and CHAR
	while (1) {
		lexer.nextsym();
		sym = lexer.text();
		tk = lexer.type();
		if (lexer.type() != TK_IDENT) {
		/*	global_syntax_err = 1;
			printf("\"%s\"is not a legal identifier name!", sym);
			error();
			if (lexer.skipline() != 1) {
				global_file_end = 1;
				return SYN_FAIL;
			}
			sym = lexer.text();
			tk = lexer.type();
			*/
			printf("\"%s\"is not a legal identifier name!", sym);
			error();
			return SYN_FAIL;
		}

		if (symtable.checksame(sym,nowlevel) > 0)
			fail_flag = 1;
		strcpy(buf, lexer.text());

		lexer.nextsym();
		sym = lexer.text();
		tk = lexer.type();
		if (lexer.type() != '=') {
			puts("Expect a '=' after an identifier");
			error();
			return SYN_FAIL;
		}
		if (type == TK_TYPINT) {
			tmp_type = TYPE_INT;
			lexer.nextsym();
			sym = lexer.text();
			tk = lexer.type();
			if (int_sys() == SYN_FAIL) {
				puts("This is not an integer value! Cannot assign to an integer constant");
				error();
				return SYN_FAIL;
			}
		}
		if (type == TK_TYPCHAR) {
			tmp_type = TYPE_CHAR;
			lexer.nextsym();
			sym = lexer.text();
			tk = lexer.type();
			if (lexer.type() != TK_VALCHAR || sym[0] != '\'') {
				puts("This is not a char value! Cannot assign to a char constant");
				error();
				return SYN_FAIL;
			}
			tmp_const = sym[1];
		}
		lexer.nextsym();
		sym = lexer.text();
		tk = lexer.type();
		symtable.insert(buf, nowlevel, tmp_type, KIND_CONST, tmp_const, 4);
		if (nowlevel > 0) {
			fours.newfour(FOUR_LI, tmp_const, 0, symtable.tablecnt);
		}
		if (lexer.type() == ',') {
			continue;
		}
		if (lexer.type() == ';') {
			break;
		}
		puts("Expect ',' or ';' after you define a constant");
		error();
		return SYN_FAIL;
	}
	if (is_main && symtable.table[symtable.tablecnt].addroffset + symtable.table[symtable.tablecnt].size / 4 > main_maxoffset)
		main_maxoffset = symtable.table[symtable.tablecnt].addroffset + symtable.table[symtable.tablecnt].size / 4;
	//printf("This is a const define!\n");
	return SYN_SUCCESS;
}

int syntax::uint_sys()
//has red a number string
{
	if (lexer.type() != TK_VALINT || sym[0] == '0') {
		puts("There is an illegal integer value!");
		error();
		return SYN_FAIL;
	}
	if (strlen(sym) > 10 || (strlen(sym)==10 && strcmp(sym, "2147483648")> 0) ) {
		puts("This integer value is too big!");
		error();
		return SYN_FAIL;
	}
	tmp_uconst = 0;
	for (int i = 0; sym[i] != '\0'; i++) {
		tmp_uconst = tmp_uconst * 10 + sym[i] - '0';
	}
//	printf("This is an unsigned integer!\n");
	return SYN_SUCCESS;
}

int syntax::int_sys()
//has red a '+','-'or a number
{
	int pos = 1;
	if (tk == TK_VALINT && strlen(sym) == 1 && sym[0] == '0') {
	//	printf("This is a integer!\n");
		tmp_const = 0;
		return SYN_SUCCESS;
	}
	if (tk == '+' || tk == '-') {
		if (tk == '-')
			pos = -1;
		lexer.nextsym();
		sym = lexer.text();
		tk = lexer.type();
		if (uint_sys() == SYN_FAIL) {
			return SYN_FAIL;
		}
		if (pos == 1 && strcmp(sym,"2147483648")==0) {
			puts("There is an illegal int value!");
			error();
			return SYN_FAIL;
		}
		tmp_const = pos*tmp_uconst;
	}
	else {
		if (uint_sys() == SYN_FAIL || strcmp(sym,"2147483648")==0) {
			return SYN_FAIL;
		}
		tmp_const = tmp_uconst;
	}
	//printf("This is a integer!\n");
	return 0;
}

int syntax::int_judge(char * buf)
{
	return 0;
}

int syntax::identifier()
{
	if (tk == TK_IDENT) {
//		printf("This is an identifier!\n");
		strcpy(tmp_ident, sym);
		return SYN_SUCCESS;
	}
	puts("There is an illegal identifier, or missing an identifier!");
	return SYN_FAIL;
}

int syntax::declare_head()
//Has red a token "int" or "char"
{
	if (tk == TK_TYPCHAR)
		tmp_head = TYPE_CHAR;
	else
		tmp_head = TYPE_INT;
	lexer.nextsym();
	sym = lexer.text();
	tk = lexer.type();
	if (identifier() == SYN_SUCCESS) {
	//	printf("This is an declare head!\n");
		if (symtable.checksame(sym,nowlevel) > 0) {
			printf("The identifier \"%s\" has been declared before, cannot re-declare it.\n",sym);
			error();
			return SYN_FAIL;
		}
		return SYN_SUCCESS;
	}
	return SYN_FAIL;
}

int syntax::var_declare()
//Has red a token "int" or "char"
{
	int tmp;
	if (tk == TK_TYPCHAR)
		tmp_head = TYPE_CHAR;
	else
		tmp_head = TYPE_INT;

	if (tk != TK_TYPINT && tk != TK_TYPCHAR) {
		puts("Cannot analyze the variable type.\n A variable declare should have a type \"int\" or \"char\"");
		error();
		return SYN_FAIL;
	}
	tmp = var_define();
	if (tmp == SYN_FAIL) {
		return SYN_FAIL;
	}
	if (tmp == SYN_VAR_DEF_TO_FUNC_DEF) {
		return tmp;
	}
	lexer.nextsym();
	sym = lexer.text();
	tk = lexer.type();
	while (tk == TK_TYPINT || tk == TK_TYPCHAR) {
		if (tk == TK_TYPCHAR)
			tmp_head = TYPE_CHAR;
		else
			tmp_head = TYPE_INT;
		tmp=var_define();
		if (tmp == SYN_VAR_DEF_TO_FUNC_DEF) {
		//	printf("This is a var declaration!\n");
			return tmp;
		}
		if (tmp == SYN_FAIL) {
			return SYN_FAIL;
		}
		lexer.nextsym();
		sym = lexer.text();
		tk = lexer.type();
	}
//	printf("This is a var declaration part!\n");
	return SYN_SUCCESS;
}

int syntax::var_define()
//Has red a token "int" or "char"
{
	int tmp_special=0, tmp_size=4;
	int first_define_flag = 0;
	lexer.nextsym();
	sym = lexer.text();
	tk = lexer.type();
	while (identifier() == SYN_SUCCESS) {
		tmp_special = 0;
		tmp_size = 4;
		if (symtable.checksame(sym,nowlevel) > 0) {
			printf("The identifier \"%s\" has been declared before, cannot re-declare it.\n", sym);
			error();
			return SYN_FAIL;
		}

		lexer.nextsym();
		sym = lexer.text();
		tk = lexer.type();
		if (tk == '(')
			if (first_define_flag == 0)
				return SYN_VAR_DEF_TO_FUNC_DEF;
			else {
				puts("Find a '(' in a variable define!");
				return SYN_FAIL;
			}
		first_define_flag = 1;
		if (tk == '[') {
			lexer.nextsym();
			sym = lexer.text();
			tk = lexer.type();
			if (uint_sys() == SYN_SUCCESS) {
				lexer.nextsym();
				sym = lexer.text();
				tk = lexer.type();
			}
			else {
				return SYN_FAIL;
			}
			if (tk != ']') {
				puts("Expect a ')' in a variable array define!");
				error();
				return SYN_FAIL;
			}
		//	symtable.insert(buf, nowlevel, tmp_head, KIND_VAR, tmp_const, tmp_const * 4);
			tmp_special = tmp_uconst;
			tmp_size = tmp_uconst * 4;
			lexer.nextsym();
			sym = lexer.text();
			tk = lexer.type();
		}
		if (tk == ',') {
			symtable.insert(tmp_ident, nowlevel, tmp_head, KIND_VAR, tmp_special,tmp_size);
			lexer.nextsym();
			sym = lexer.text();
			tk = lexer.type();
			continue;
		}
		if (tk == ';') {
			symtable.insert(tmp_ident, nowlevel, tmp_head, KIND_VAR, tmp_special, tmp_size);
			if (is_main && symtable.table[symtable.tablecnt].addroffset + symtable.table[symtable.tablecnt].size / 4 > main_maxoffset)
				main_maxoffset = symtable.table[symtable.tablecnt].addroffset + symtable.table[symtable.tablecnt].size / 4;
		//	printf("This is a var definition!\n");
			return SYN_SUCCESS;
		}
		break;
	}
	return SYN_FAIL;
}

int syntax::const_val()
//Has red a symbol
{
	char buf[23];
	strcpy(buf, sym);
	int tmp_special = 0;
	int tmp_type = 0;
	if (tk == TK_VALCHAR) {
		tmp_const = sym[1];
	}
	else{
		//should be an integer
		if (int_sys() == SYN_FAIL) {
			return SYN_FAIL;
		}
	}
	//printf("This is a const value!\n");
	return SYN_SUCCESS;
}

int syntax::func_define(int is_moved)
//If is_moved then has red <int identifier(>
//If not is_moved then has red token "int" or "char"
//tmp_head has saved the type
{
	char buf[2333];
	int tmp_symid,maxoffset=0,labelid,tmp_funcid;
	is_func = 1;
	strcpy(buf, tmp_ident);
	para_cnt = 0;
	int tmp;
	nowlevel = 0;
	if (!is_moved) {
		lexer.nextsym();
		sym = lexer.text();
		tk = lexer.type();
		if (identifier() == SYN_FAIL) {
			return SYN_FAIL;
		}
		lexer.nextsym();
		sym = lexer.text();
		tk = lexer.type();
		if (tk != '(') {
			puts("Expect a '(' in a function define!");
			error();
			return SYN_FAIL;
		}
	}
	if (strcmp(sym, "main") == 0) {
		puts("Main function must be void type!");
		error();
		return SYN_FAIL;
	}
	if (symtable.checksame(tmp_ident, nowlevel) > 0) {
		printf("The identifier \"%s\" has been declared before, cannot re-declare it.\n", sym);
		error();
		return SYN_FAIL;
	}
	symtable.insert(tmp_ident, 0, tmp_head, KIND_FUNC, para_cnt, 0);
	tmp_funcid = symtable.tablecnt;
	tmp_symid = symtable.tablecnt;
	sprintf(buf, "%s%s", LAB_FUNC, tmp_ident);
	symtable.insert(buf, 0, TYPE_INT, KIND_LABLE, 0, 0);
	
	fours.newfour(FOUR_LABEL, 0, 0, symtable.search(buf, nowlevel));
	labelid = fours.cnt;

	nowlevel = 1;
	lexer.nextsym();
	sym = lexer.text();
	tk = lexer.type();
	if (tk == TK_TYPCHAR || tk == TK_TYPINT) {
		if (parameters() == SYN_FAIL) {
			return SYN_FAIL;
		}
	}
	else if (tk != ')') {
		puts("Expect a ')' in a function define!");
		error();
		return SYN_FAIL;
	}
	symtable.table[tmp_funcid].special = para_cnt;
	
	symtable.insert("$tmp1", nowlevel, TYPE_INT, KIND_VAR, 0, 4);
	symtable.insert("$tmp2", nowlevel, TYPE_INT, KIND_VAR, 0, 4);
	symtable.insert("$ans", nowlevel, TYPE_INT, KIND_VAR, 0, 4);
	maxexlevel = 0;
	symtable.insert("$Tmp_term_0",nowlevel,TYPE_INT,KIND_VAR,0,4);
	symtable.insert("$Tmp_factor_0", nowlevel, TYPE_INT, KIND_VAR, 0, 4);
	lexer.nextsym();
	sym = lexer.text();
	tk = lexer.type();
	if (tk != '{') {
		puts("Expect a '{' at the start of a function!");
		error();
		return SYN_FAIL;
	}
	lexer.nextsym();
	sym = lexer.text();
	tk = lexer.type();
	if (compound() == SYN_FAIL) {
		return SYN_FAIL;
	}
	nowlevel = 0;
	printf("This is a function define!\n");

	maxoffset = symtable.table[symtable.tablecnt].addroffset+ symtable.table[symtable.tablecnt].size;
	fours.fours[labelid].src1 = maxoffset;
	fours.newfour(FOUR_RETURN, 0, 0, 0);

	return SYN_SUCCESS;
}

int syntax::proc_define()
//Has red a token "void"
{
	char buf[233];
	int labelid;
	int tmp_symid = 0, tmp_funcid = 0;
	is_func = 0;
	nowlevel = 0;
	para_cnt = 0;
	lexer.nextsym();
	sym = lexer.text();
	tk = lexer.type();
	strcpy(buf, sym);
	if (identifier() == SYN_FAIL) {
		return SYN_FAIL;
	}
	if (strcmp(sym, "main") == 0) {
		nowlevel = 0;
		return SYN_PROC_DEF_TO_MAIN_DEF;
	}
	if (symtable.checksame(sym, nowlevel) > 0) {
		printf("The identifier \"%s\" has been declared before, cannot re-declare it.\n", sym);
		error();
		return SYN_FAIL;
	}
	symtable.insert(tmp_ident, 0, TYPE_INT, KIND_PROC, 0, 0);
	tmp_funcid = symtable.tablecnt;
	sprintf(buf, "%s%s", LAB_FUNC, tmp_ident);
	symtable.insert(buf, 0, TYPE_INT, KIND_LABLE, 0, 0);

	fours.newfour(FOUR_LABEL, 0, 0, symtable.search(buf, nowlevel));
	labelid = fours.cnt;
	tmp_symid = symtable.tablecnt;
	
	
	lexer.nextsym();
	sym = lexer.text();
	tk = lexer.type();
	if (tk != '(') {
		puts("Expect a '(' in a function define!");
		error();
		return SYN_FAIL;
	}
	nowlevel = 1;
	lexer.nextsym();
	sym = lexer.text();
	tk = lexer.type();
	if (tk == TK_TYPCHAR || tk == TK_TYPINT) {
		if (parameters() == SYN_FAIL) {
			return SYN_FAIL;
		}
	}
	else if (tk != ')') {
		puts("Expect a ')' in a function define!");
		error();
		return SYN_FAIL;
	}
	symtable.table[tmp_funcid].special = para_cnt;
	symtable.insert("$tmp1", nowlevel, TYPE_INT, KIND_VAR, 0, 4);
	symtable.insert("$tmp2", nowlevel, TYPE_INT, KIND_VAR, 0, 4);
	symtable.insert("$ans", nowlevel, TYPE_INT, KIND_VAR, 0, 4);
	maxexlevel = 0;
	symtable.insert("$Tmp_term_0", nowlevel, TYPE_INT, KIND_VAR, 0, 4);
	symtable.insert("$Tmp_factor_0", nowlevel, TYPE_INT, KIND_VAR, 0, 4);
	
	lexer.nextsym();
	sym = lexer.text();
	tk = lexer.type();
	if (tk != '{') {
		puts("Expect a '{' at the start of a function!");
		error();
		return SYN_FAIL;
	}
	lexer.nextsym();
	sym = lexer.text();
	tk = lexer.type();
	if (compound() == SYN_FAIL) {
		return SYN_FAIL;
	}
	nowlevel = 0;
	printf("This is aprocedure!\n");

	
	fours.newfour(FOUR_RETURN, 0, 0, 0);
	int maxoffset = symtable.table[symtable.tablecnt].addroffset + symtable.table[symtable.tablecnt].size;
	fours.fours[labelid].src1 = maxoffset;
	return SYN_SUCCESS;
}

int syntax::compound()
//Has red a '{' and a extra symbol
{
	if (tk == TK_CONST) {
		if (const_declare() == SYN_FAIL) {
			return SYN_FAIL;
		}
	}
	if (tk == TK_TYPCHAR || tk == TK_TYPINT) {
		if (var_declare() == SYN_FAIL) {
			return SYN_FAIL;
		}
	}
	if (state_list() == SYN_FAIL) {
		return SYN_FAIL;
	}
	if (tk != '}') {
		puts("Expect a '}' at the end of a compound statement!");
		error();
		return SYN_FAIL;
	}
//	printf("This is a compound statement!\n");
	return SYN_SUCCESS;
}

int syntax::parameters()
//Has red a token "int" or "char"
{
	lexer.nextsym();
	sym = lexer.text();
	tk = lexer.type();
	para_cnt = 0;
	while (1) {
		if (identifier() == SYN_FAIL) {
			return SYN_FAIL;
		}
		if (symtable.checksame(sym,nowlevel) > 0) {
			printf("The identifier \"%s\" has been declared before, cannot re-declare it.\n", sym);
			error();
			return SYN_FAIL;
		}
		symtable.insert(sym, nowlevel, TYPE_INT, KIND_VAR, 0, 4);
		para_cnt++;
		lexer.nextsym();
		sym = lexer.text();
		tk = lexer.type();
		if (tk == ',') {
			lexer.nextsym();
			sym = lexer.text();
			tk = lexer.type();
			if (tk != TK_TYPCHAR && tk != TK_TYPINT) {
				puts("Invalid parameter type.\nA parameter should have a type int or char!");
				error();
				return SYN_FAIL;
			}
			lexer.nextsym();
			sym = lexer.text();
			tk = lexer.type();
			continue;
		}
		if (tk == ')')
			break;
		puts("Expect a ')' at the end of a function declare!");
		error();
		return SYN_FAIL;
	}
//	printf("This is a parameter list!\n");
	return SYN_SUCCESS;
}

int syntax::main_func()
//Has red "void main"
{
	is_main = 1;
	is_func = 0;
	char buf[233];
	if (symtable.checksame("main",0) > 0) {
		printf("The identifier \"main\" has been declared before, cannot re-declare it.\n", sym);
		error();
		return SYN_FAIL;
	}
	symtable.insert("main", 0, TYPE_INT, KIND_PROC, 0, 0);
	symtable.insert("$Lab_func_main", 0, TYPE_INT, KIND_LABLE, 0, 4);

	
	fours.newfour(FOUR_LABEL, 0, 0, symtable.search("$Lab_func_main", nowlevel));

	nowlevel = 1;
	lexer.nextsym();
	sym = lexer.text();
	tk = lexer.type();
	if (tk != '(') {
		puts("Expect a '(' in a main function define!");
		error();
		return SYN_FAIL;
	}
	lexer.nextsym();
	sym = lexer.text();
	tk = lexer.type();
	if (tk != ')') {
		puts("Expect a ')' in a main function define!");
		error();
		return SYN_FAIL;
	}
	symtable.insert("$tmp1", nowlevel, TYPE_INT, KIND_VAR, 0, 4);
	symtable.insert("$tmp2", nowlevel, TYPE_INT, KIND_VAR, 0, 4);
	symtable.insert("$ans", nowlevel, TYPE_INT, KIND_VAR, 0, 4);
	maxexlevel = 0;
	symtable.insert("$Tmp_term_0", nowlevel, TYPE_INT, KIND_VAR, 0, 4);
	symtable.insert("$Tmp_factor_0", nowlevel, TYPE_INT, KIND_VAR, 0, 4);

	lexer.nextsym();
	sym = lexer.text();
	tk = lexer.type();
	if (tk != '{') {
		puts("Expect a '{' at the start of main function!");
		error();
		return SYN_FAIL;
	}
	lexer.nextsym();
	sym = lexer.text();
	tk = lexer.type();
	if (compound() == SYN_FAIL) {
		return SYN_FAIL;
	}
	

	fours.newfour(FOUR_END, 0, 0, 0);
	return SYN_SUCCESS;
}

int syntax::expression()
//Has red a symbol
{
	int ischar = 1, tmp;
	int pos = 1, first = 1,termid,ansid;
	if (tk == '+' || tk == '-') {
		if (tk == '-')
			pos = 0;
		lexer.nextsym();
		sym = lexer.text();
		tk = lexer.type();
		ischar = 0;
	}
	sprintf(buf, "%s%d", TMP_TERM, nowexlevel);
	termid = symtable.search(buf, nowlevel);
	sprintf(buf, "%s", TMP_ANS);
	ansid = symtable.search(buf, nowlevel);
	while (1) {
		tmp = term();
		if (tmp == SYN_FAIL) {
			return SYN_FAIL;
		}		
		if (tmp != SYN_EXPRESSION_IS_CHAR)
			ischar = 0;
		if (first) {
			if(pos==0)
				fours.newfour(FOUR_VERSE, 0, 0, ansid);
			fours.newfour(FOUR_MOV, ansid, 0, termid);
			first = 0;
		}
		else {
			if (pos == 0)
				fours.newfour(FOUR_SUB, termid, ansid, termid);
			else
				fours.newfour(FOUR_ADD, termid, ansid, termid);
			ischar = 0;
		}
		if (tk == '+' || tk =='-') {
			if (tk == '+')
				pos = 1;
			else
				pos = 0;
			lexer.nextsym();
			sym = lexer.text();
			tk = lexer.type();
			continue;
		}
		break;
	}
	fours.newfour(FOUR_MOV, termid, 0, ansid);
//	printf("This is an expression!\n");
	if (ischar)
		return SYN_EXPRESSION_IS_CHAR;
	return SYN_SUCCESS;
}

int syntax::term()
//has red a symbol
{
	int ischar = 1,tmp;
	int mul = 1, ansid, facid, first = 1;
	sprintf(buf, "%s%d", TMP_FACT, nowexlevel);
	facid = symtable.search(buf, nowlevel);
	sprintf(buf, "%s", TMP_ANS);
	ansid = symtable.search(buf, nowlevel);
	while (1) {
		tmp = factor();
		if (tmp == SYN_FAIL) {
			return SYN_FAIL;
		}
		if (tmp != SYN_EXPRESSION_IS_CHAR)
			ischar = 0;
		if (first) {
			fours.newfour(FOUR_MOV, ansid, 0, facid);
			first = 0;
		}
		else if (mul) {
			fours.newfour(FOUR_MUL, facid, ansid, facid);
			ischar = 0;
		}
		else {
			fours.newfour(FOUR_DIV, facid, ansid, facid);
			ischar = 0;
		}
		if (tk == '*' || tk=='/') {
			if (tk == '/')
				mul = 0;
			else
				mul = 1;
			lexer.nextsym();
			sym = lexer.text();
			tk = lexer.type();
			continue;
		}
		break;
	}
	fours.newfour(FOUR_MOV, facid, 0, ansid);
	//printf("This is a term!\n");
	if (ischar)
		return SYN_EXPRESSION_IS_CHAR;
	return SYN_SUCCESS;
}

int syntax::factor()
//Has red a symbol
{
	int ischar = 0, ispos = 1, havepos = 0;
	int id, ansid;
	sprintf(buf, "%s", TMP_ANS);
	ansid = symtable.search(buf, nowlevel);
	if (tk == '+' || tk == '-') {
		havepos = 1;
		if (tk == '-')
			ispos = 0;
		lexer.nextsym();
		sym = lexer.text();
		tk = lexer.type();
	}
	if (havepos != 0 && tk != TK_VALINT) {
		puts("In a factor, an '+' or '-' must be followed by an int value!");
		error();
		return SYN_FAIL;
	}
	if (tk == TK_VALINT) {//is a int
		if (int_sys() == SYN_FAIL) {
			return SYN_FAIL;
		}
		fours.newfour(FOUR_LI, tmp_const, 0, ansid);
		if (ispos == 0)
			fours.newfour(FOUR_VERSE, 0, 0, ansid);
	}
	else if (tk == TK_VALCHAR) {//is a char
		fours.newfour(FOUR_LI, sym[1], 0, ansid);
		ischar = 1;
	}
	else if (tk == '(') {
		lexer.nextsym();
		sym = lexer.text();
		tk = lexer.type();
		nowexlevel++;
		if (nowexlevel > maxexlevel) {
			maxexlevel = nowexlevel;
			sprintf(buf, "%s%d", TMP_TERM, maxexlevel);
			symtable.insert(buf, nowlevel, TYPE_INT, KIND_VAR, 0, 4);
			sprintf(buf, "%s%d", TMP_FACT, maxexlevel);
			symtable.insert(buf, nowlevel, TYPE_INT, KIND_VAR, 0, 4);
		}
		int tmp = expression();
		if (tmp == SYN_FAIL) {
			return SYN_FAIL;
		}
		if (tmp == SYN_EXPRESSION_IS_CHAR)
			ischar = 1;
		nowexlevel--;
		if (tk != ')') {
			puts("Expect a ')' at the end of a factor!");
			error();
			return SYN_FAIL;
		}
	}
	else {//is an identifier
		if (identifier() == SYN_FAIL) {
			error();
			return SYN_FAIL;
		}
		id = symtable.search(tmp_ident, nowlevel);
		if (id == 0) {
			printf("Identifier \"%s\" has not been declared before!",tmp_ident);
			error();
			return SYN_FAIL;
		}
		if (symtable.table[id].type == TYPE_CHAR)
			ischar = 1;
		lexer.nextsym();
		sym = lexer.text();
		tk = lexer.type();
		if (tk == '[') {//should be an array
			if (symtable.table[id].type == TYPE_CHAR)
				ischar = 1;
			if (symtable.table[id].kind != KIND_VAR || symtable.table[id].special == 0) {
				printf("Identifier \"%s\" is not an array!", tmp_ident);
				error();
				return SYN_FAIL;
			}
			lexer.nextsym();
			sym = lexer.text();
			tk = lexer.type();

			nowexlevel++;
			if (nowexlevel > maxexlevel) {
				maxexlevel = nowexlevel;
				sprintf(buf, "%s%d", TMP_TERM, maxexlevel);
				symtable.insert(buf, nowlevel, TYPE_INT, KIND_VAR, 0, 4);
				sprintf(buf, "%s%d", TMP_FACT, maxexlevel);
				symtable.insert(buf, nowlevel, TYPE_INT, KIND_VAR, 0, 4);
			}
			if (expression() == SYN_FAIL) {
				return SYN_FAIL;
			}
			fours.newfour(FOUR_GETARR, id, ansid, ansid);
			nowexlevel--;
			if (tk != ']') {
				puts("Expect a ']' to express an array !");
				error();
				return SYN_FAIL;
			}
		}
		else if (tk == '(') {//should be an function
			if (symtable.table[id].kind != KIND_FUNC) {
				printf("Identifier \"%s\" is not a function!", tmp_ident);
				error();
				return SYN_FAIL;
			}
			
			if (func_call() == SYN_FAIL) {
				return SYN_FAIL;
			}
			fours.newfour(FOUR_GRETURN, 0, 0, ansid);
		}
		else {
			//is an var
//			printf("There is a factor!\n");
			if(!((symtable.table[id].kind==KIND_VAR && symtable.table[id].special==0) || symtable.table[id].kind==KIND_CONST)){
				error();
				return SYN_FAIL;
			}
			fours.newfour(FOUR_MOV, id, 0, ansid);
			if (ischar)
				return SYN_EXPRESSION_IS_CHAR;
			return SYN_SUCCESS;
		}
	}
	lexer.nextsym();
	sym = lexer.text();
	tk = lexer.type();
	//printf("There is a factor!\n");
	if (ischar)
		return SYN_EXPRESSION_IS_CHAR;
	return SYN_SUCCESS;
}

int syntax::statement()
//has red a symbol
{
	int ansid;
//	printf("Analyse a statement at line %d\n", lexer.lineno());
	sprintf(buf, "%s", TMP_ANS);
	ansid = symtable.search(buf, nowlevel);
	switch (tk)
	{
	case TK_IF: {
		if (if_state() == SYN_FAIL) {
			return SYN_FAIL;
		}
		break;
	}
	case TK_WHILE: {
		if (while_state() == SYN_FAIL) {
			return SYN_FAIL;
		}
		break;
	}
	case TK_SCANF: {
		if (read_state() == SYN_FAIL) {
			return SYN_FAIL;
		}
		lexer.nextsym();
		sym = lexer.text();
		tk = lexer.type();
		if (tk != ';') {
			puts("Expect a ';' at the end of a scanf statement!");
			error();
			return SYN_FAIL;
		}
		break;
	}
	case TK_PRINTF: {
		if (print_state() == SYN_FAIL) {
			return SYN_FAIL;
		}
		lexer.nextsym();
		sym = lexer.text();
		tk = lexer.type();
		if (tk != ';') {
			puts("Expect a ';' at the end of a printf statement!");
			error();
			return SYN_FAIL;
		}
		break;
	}
	case ';': {
		break;
	}
	case TK_SWITCH: {
		if (switch_state() == SYN_FAIL) {
			return SYN_FAIL;
		}
		break;
	}
	case TK_RETURN: {
		if (return_state() == SYN_FAIL) {
			return SYN_FAIL;
		}
		if (tk != ';') {
			puts("Expect a ';' at the end of a return statement!");
			error();
			return SYN_FAIL;
		}
		break;
	}
	case TK_IDENT: {
		if (identifier() == SYN_FAIL) {
			return SYN_FAIL;
		}
		lexer.nextsym();
		sym = lexer.text();
		tk = lexer.type();
		int id = symtable.search(tmp_ident, nowlevel);
		switch (tk)
		{
		case '=': {
			//assignment statement put ans in $ans
			if (symtable.table[id].kind != KIND_VAR) {
				printf("Identifier \"%s\"is not a variable!\n", tmp_ident);
				error();
				return SYN_FAIL;
			}
			if (assign_state() == SYN_FAIL) {
				//assign_state just put into ansid
				return SYN_FAIL;
			}
			fours.newfour(FOUR_MOV, ansid, 0, id);
			if (tk != ';') {
				puts("Expect a ';' at the end of a assignment statement!");
				error();
				return SYN_FAIL;
			}
			break;
		}
		case '[': {
			//should be an array
			lexer.nextsym();
			sym = lexer.text();
			tk = lexer.type();
			if (expression() == SYN_FAIL) {
				return SYN_FAIL;
			}
			int tmp1id = symtable.search(TMP_1, nowlevel);
			fours.newfour(FOUR_MOV, ansid, 0, tmp1id);
			if (tk != ']') {
				puts("Expect a ']' to express an array!");
				error();
				return SYN_FAIL;
			}
			if (symtable.table[id].kind != KIND_VAR || symtable.table[id].special == 0) {
				printf("Identifier \"%s\" is not an array!", tmp_ident);
				error();
				return SYN_FAIL;
			}
			lexer.nextsym();
			sym = lexer.text();
			tk = lexer.type();
			if (tk != '=') {
				puts("Expect a '=' at the start of an assign statement!");
				error();
				return SYN_FAIL;
			}
			if (assign_state() == SYN_FAIL) {
				return SYN_FAIL;
			}
			fours.newfour(FOUR_SETARR, ansid, tmp1id, id);
			if (tk != ';') {
				puts("Expect a ';' at the end of an assignment statement!");
				error();
				return SYN_FAIL;
			}
			break;
		}
		case '(': {
			// there should be func and proc
			if (symtable.table[id].kind == KIND_FUNC) {
				if (func_call() == SYN_FAIL) {
					return SYN_FAIL;
				}
			}
			else if (symtable.table[id].kind == KIND_PROC) {
				if (proc_call() == SYN_FAIL) {
					return SYN_FAIL;
				}
			}
			else {
				printf("Identifier \"%s\" is not a function!\n", tmp_ident);
				error();
				return SYN_FAIL;
			}
			lexer.nextsym();
			sym = lexer.text();
			tk = lexer.type();
			if (tk != ';') {
				puts("Expect a ';' at the end of a function call statement!");
				error();
				return SYN_FAIL;
			}
			break;
		}
		default: {
			puts("Expect a '=' or '[' or '(' after an identifier!");
			error();
			return SYN_FAIL;
		}
		}
		break;
	}
	case '{': {
		lexer.nextsym();
		sym = lexer.text();
		tk = lexer.type();
		if (tk == '}')
			break;
		if (state_list() == SYN_FAIL) {
			return SYN_FAIL;
		}
		if (tk != '}') {
			puts("Expect a '}' at the end of a compound statement!");
			error();
			return SYN_FAIL;
		}
		break;
	}
		  defult: {
			  puts("Cannot analyze this satement! Could be meeting some unknown charactor");
			  error();
			  return SYN_FAIL;
	}
	}
//	printf("This is a statement!\n");
	return SYN_SUCCESS;
}

int syntax::assign_state()
//Has red '='
{
	lexer.nextsym();
	sym = lexer.text();
	tk = lexer.type();
	if (expression() == SYN_FAIL) {
		return SYN_FAIL;
	}
	//printf("This is a assignment statement!\n");
	return SYN_SUCCESS;
}

int syntax::if_state()
//Has red token "if"
{
	int ansid,labid;
	ansid = symtable.search(TMP_ANS, nowlevel);
	if_cnt++;
	sprintf(buf, "%s%d", LAB_IFEND, if_cnt);
	symtable.insert(buf, nowlevel, TYPE_INT, KIND_LABLE, 0, 0);
	labid = symtable.search(buf, nowlevel);
	
	lexer.nextsym();
	sym = lexer.text();
	tk = lexer.type();
	if (tk != '(') {
		puts("Expect a '(' in a if-statement!");
		error();
		return SYN_FAIL;
	}
	lexer.nextsym();
	sym = lexer.text();
	tk = lexer.type();
	if (if_case() == SYN_FAIL) {
		//if_case put ans into $ans
		return SYN_FAIL;
	}
	fours.newfour(FOUR_BEQZ, ansid, 0, labid);
	if (tk != ')') {
		puts("Expect a ')' in a if-statement!");
		error();
		return SYN_FAIL;
	}
	lexer.nextsym();
	sym = lexer.text();
	tk = lexer.type();
	if (statement() == SYN_FAIL) {
		return SYN_FAIL;
	}
	fours.newfour(FOUR_LABEL, 0, 0, labid);
//	printf("This is a if-statement\n");
	return SYN_SUCCESS;
}

int syntax::if_case()
//has read a symble
{
	int ansid, tmp2id,thistk;
	ansid = symtable.search(TMP_ANS, nowlevel);
	tmp2id = symtable.search(TMP_2, nowlevel);
	//putans into ansid
	if (expression() == SYN_FAIL) {
		return SYN_FAIL;
	}
	if (tk == '<' || tk == '>' || tk == TK_GEQ || tk == TK_LEQ || tk == TK_SAME || tk == TK_NEQ) {
		thistk = tk;
		fours.newfour(FOUR_MOV, ansid, 0, tmp2id);
		lexer.nextsym();
		sym = lexer.text();
		tk = lexer.type();
		if (expression() == SYN_FAIL) {
			return SYN_FAIL;
		}
		if (thistk == '<')
			fours.newfour(FOUR_LESS, tmp2id, ansid, ansid);
		if (thistk == '>')
			fours.newfour(FOUR_BIG, tmp2id, ansid, ansid);
		if (thistk == TK_GEQ)
			fours.newfour(FOUR_GEQ, tmp2id, ansid, ansid);
		if (thistk == TK_LEQ)
			fours.newfour(FOUR_LEQ, tmp2id, ansid, ansid);
		if (thistk == TK_SAME)
			fours.newfour(FOUR_SAME, tmp2id, ansid, ansid);
		if (thistk == TK_NEQ)
			fours.newfour(FOUR_NEQ, tmp2id, ansid, ansid);
	}
//	printf("This is a if-condition!\n");
	return SYN_SUCCESS;
}

int syntax::while_state()
//Has red token "while"
{
	int labid,labendid,ansid;
	ansid = symtable.search(TMP_ANS, nowlevel);
	while_cnt++;
	sprintf(buf, "%s%d",LAB_WHILE,while_cnt);
	symtable.insert(buf,nowlevel,TYPE_INT,KIND_LABLE,0,0);
	labid = symtable.search(buf, nowlevel);
	sprintf(buf, "%s%d", LAB_WHILEEND, while_cnt);
	symtable.insert(buf, nowlevel, TYPE_INT, KIND_LABLE, 0, 0);
	labendid = symtable.search(buf, nowlevel);

	fours.newfour(FOUR_LABEL, 0, 0, labid);
	lexer.nextsym();
	sym = lexer.text();
	tk = lexer.type();
	if (tk != '(') {
		puts("Expect a '(' in a while statement!");
		error();
		return SYN_FAIL;
	}
	lexer.nextsym();
	sym = lexer.text();
	tk = lexer.type();
	if (if_case() == SYN_FAIL) {
	
		return SYN_FAIL;
	}
	fours.newfour(FOUR_BEQZ, ansid, 0, labendid);
	if (tk != ')') {
		puts("Expect a ')' in a while statement!");
		error();
		return SYN_FAIL;
	}
	lexer.nextsym();
	sym = lexer.text();
	tk = lexer.type();
	if (statement() == SYN_FAIL) {
		return SYN_FAIL;
	}
	fours.newfour(FOUR_JUMP, 0, 0, labid);
	fours.newfour(FOUR_LABEL, 0, 0, labendid);
//	printf("This is a while statement!\n");
	return SYN_SUCCESS;
}

int syntax::switch_state()
//Has red token "switch"
{
	int labid, labendid, ansid,opid,defid;

	ansid = symtable.search(TMP_ANS, nowlevel);
	swit_cnt++;
	sprintf(buf, "%s%d", LAB_SWITCHEND, switch_cnt);
	symtable.insert(buf, nowlevel, TYPE_INT, KIND_LABLE, 0, 0);
	labendid = symtable.search(buf, nowlevel);
	sprintf(buf, "%s%d", LAB_DEFAULT, switch_cnt);
	symtable.insert(buf, nowlevel, TYPE_INT, KIND_LABLE, 0, 0);
	defid = symtable.search(buf, nowlevel);

	nowswlevel++;
	if (nowswlevel > maxswlevel) {
		maxswlevel = nowswlevel;
		sprintf(buf, "%s%d",TMP_SWITOP, maxexlevel);
		symtable.insert(buf, nowlevel, TYPE_INT, KIND_VAR, 0, 4);
	}
	sprintf(buf, "%s%d", TMP_SWITOP, nowswlevel);
	opid = symtable.search(buf, nowlevel);

	lexer.nextsym();
	sym = lexer.text();
	tk = lexer.type();
	if (tk != '(') {
		puts("Expect a '(' in a switch statement!");
		error();
		return SYN_FAIL;
	}
	lexer.nextsym();
	sym = lexer.text();
	tk = lexer.type();
	if (expression() == SYN_FAIL) {
		return SYN_FAIL;
	}
	fours.newfour(FOUR_MOV, ansid, 0, opid);
	if (tk != ')') {
		puts("Expect a ')' in a switch statement!");
		error();
		return SYN_FAIL;
	}
	lexer.nextsym();
	sym = lexer.text();
	tk = lexer.type();
	if (tk != '{') {
		puts("Expect a '{' in a switch statement!");
		error();
		return SYN_FAIL;
	}
	lexer.nextsym();
	sym = lexer.text();
	tk = lexer.type();
	if (caselist() == SYN_FAIL) {
		return SYN_FAIL;
	}
	if (tk == TK_DEFAULT) {
		if (default_state() == SYN_FAIL) {
			return SYN_FAIL;
		}
		lexer.nextsym();
		sym = lexer.text();
		tk = lexer.type();
	}
	if (tk == '}') {
		nowswlevel--;
		fours.newfour(FOUR_LABEL, 0, 0, labendid);
	//	printf("This is a switch statement!\n");
		return SYN_SUCCESS;
	}
	puts("Expect a '}' in a switch statement!");
	error();
	return SYN_FAIL;
}

int syntax::caselist()
//Has red a symbol
{
	if (tk != TK_CASE) {
		puts("Expect a 'case' in a switch statement!");
		error();
		return SYN_FAIL;
	}
	while (1) {
		if (case_state() == SYN_FAIL) {
			return SYN_FAIL;
		}
		lexer.nextsym();
		sym = lexer.text();
		tk = lexer.type();
		if (tk == TK_CASE)
			continue;
		if (tk == TK_DEFAULT || tk =='}')
			break;
		puts("Expect a 'case' or 'default' or '}' in a switch statement!");
		error();
		return SYN_FAIL;
	}
	//printf("This is a case list!\n");
	return SYN_SUCCESS;
}

int syntax::case_state()
//Has red token "case"
{
	int opid,tmpid,caseendid,caseid;
	sprintf(buf, "%s%d", TMP_SWITOP, nowswlevel);
	opid = symtable.search(buf, nowlevel);
	tmpid = symtable.search(TMP_1, nowlevel);

	case_cnt++;
	sprintf(buf, "%s%d", LAB_CASE, case_cnt);
	symtable.insert(buf, nowlevel, TYPE_INT, KIND_LABLE, 0, 0);
	caseid = symtable.search(buf, nowlevel);
	sprintf(buf, "%s%d", LAB_CASEEND, case_cnt);
	symtable.insert(buf, nowlevel, TYPE_INT, KIND_LABLE, 0, 0);
	caseendid = symtable.search(buf, nowlevel);

	fours.newfour(FOUR_LABEL, 0, 0, caseid);
	lexer.nextsym();
	sym = lexer.text();
	tk = lexer.type();
	if (const_val() == SYN_FAIL) {
		puts("Expect a const value in a case statement!");
		return SYN_FAIL;
	}
	fours.newfour(FOUR_LI, tmp_const, 0, tmpid);
	fours.newfour(FOUR_BNE, tmpid, opid, caseendid);
	lexer.nextsym();
	sym = lexer.text();
	tk = lexer.type();
	if (tk != ':') {
		puts("Expect a ':' in a case statement!");
		error();
		return SYN_FAIL;
	}

	lexer.nextsym();
	sym = lexer.text();
	tk = lexer.type();
	if (statement() == SYN_FAIL) {
		return SYN_FAIL;
	}
	sprintf(buf, "%s%d", LAB_SWITCHEND, switch_cnt);
	int jumpout = symtable.search(buf, nowlevel);
	fours.newfour(FOUR_JUMP, 0, 0, jumpout);
	fours.newfour(FOUR_LABEL, 0, 0, caseendid);
	//printf("This is a case-statement!\n");
	return SYN_SUCCESS;
}

int syntax::default_state()
//Has red token "default"
{
	int defid;
	sprintf(buf, "%s%d", LAB_DEFAULT, switch_cnt);
	defid = symtable.search(buf, nowlevel);
	fours.newfour(FOUR_LABEL, 0, 0, defid);
	lexer.nextsym();
	sym = lexer.text();
	tk = lexer.type();
	if (tk != ':') {
		puts("Expect a ':' in a default statement!");
		error();
		return SYN_FAIL;
	}
	lexer.nextsym();
	sym = lexer.text();
	tk = lexer.type();
	if (statement() == SYN_FAIL) {
		return SYN_FAIL;
	}
	//printf("This is a default state!\n");
	return SYN_SUCCESS;
}

int syntax::func_call()
//has red "<id>(",sure a func 
{
	int funcid,ansid,paracnt,funclab;
	funcid = symtable.search(tmp_ident, nowlevel);
	ansid = symtable.search(TMP_ANS, nowlevel);
	sprintf(buf, "%s%s", LAB_FUNC, tmp_ident);
	funclab = symtable.search(buf, nowlevel);
	fours.newfour(FOUR_PUSHFP, 0, 0, 0);
	lexer.nextsym();
	sym = lexer.text();
	tk = lexer.type();
	if (tk == ')') {
		if (symtable.table[funcid].special != 0) {
			puts("Incorrect number of arguments!");
			error();
			return SYN_FAIL;
		}
	}
	else {
		paracnt = real_para_list();
		if (paracnt != symtable.table[funcid].special) {
			puts("Incorrect number of arguments!");
			error();
			return SYN_FAIL;
		}
		if (tk != ')') {
			puts("Expect a ')' in a function call!");
			error();
			return SYN_FAIL;
		}
	}
	fours.newfour(FOUR_GETPC, 0, 0, 0);
	fours.newfour(FOUR_CALL, 0, 0, funclab);
	fours.newfour(FOUR_GRETURN, 0, 0, ansid);
	//printf("This is a function call!\n");
	return SYN_SUCCESS;
}

int syntax::proc_call()
//has red <id>(
{
	int funcid, ansid, paracnt, funclab;
	funcid = symtable.search(tmp_ident, nowlevel);
	ansid = symtable.search(TMP_ANS, nowlevel);
	sprintf(buf, "%s%s", LAB_FUNC, tmp_ident);
	funclab = symtable.search(buf, nowlevel);
	lexer.nextsym();
	sym = lexer.text();
	tk = lexer.type();
	fours.newfour(FOUR_PUSHFP, 0, 0, 0);
	if (tk == ')') {
		if (symtable.table[funcid].special != 0) {
			puts("Incorrect number of arguments!");
			error();
			return SYN_FAIL;
		}
	}
	else {
		paracnt = real_para_list();
		if (paracnt != symtable.table[funcid].special) {
			puts("Incorrect number of arguments!");
			error();
			return SYN_FAIL;
		}
		if (tk != ')') {
			puts("Expect a ')' in a function call!");
			error();
			return SYN_FAIL;
		}
	}
	fours.newfour(FOUR_GETPC, 0, 0, 0);
	fours.newfour(FOUR_CALL, 0, 0, funclab);

	//printf("This is a procedure call!\n");
	return SYN_SUCCESS;
}

int syntax::real_para_list()
//Has red a symbol
{
	int paracnt=0,ansid;
	ansid = symtable.search(TMP_ANS, nowlevel);
	while (1) {
		nowexlevel++;
		if (nowexlevel > maxexlevel) {
			maxexlevel = nowexlevel;
			sprintf(buf, "%s%d", TMP_TERM, maxexlevel);
			symtable.insert(buf, nowlevel, TYPE_INT, KIND_VAR, 0, 4);
			sprintf(buf, "%s%d", TMP_FACT, maxexlevel);
			symtable.insert(buf, nowlevel, TYPE_INT, KIND_VAR, 0, 4);
		}
		if (expression() == SYN_FAIL) {
			return SYN_FAIL;
		}
		nowexlevel--;
		paracnt++;
		fours.newfour(FOUR_PUSH, 0, 0, ansid);
		if (tk == ',') {
			lexer.nextsym();
			sym = lexer.text();
			tk = lexer.type();
			continue;
		}
		if (tk == ')')
			break;
		puts("Expect a ')' at the end of a arguments list!");
		error();
		return SYN_FAIL;
	}
//	printf("This is a real parameters list!\n");
	return paracnt;
}

int syntax::state_list()
//Has red a symbol
{
	while (1) {
		if (statement() == SYN_FAIL) {
			if(lexer.skipline()!=1)
				return SYN_FAIL;
			lexer.nextsym();
			sym = lexer.text();
			tk = lexer.type();
			if (tk == '}')
				break;
			continue;
		}
		lexer.nextsym();
		sym = lexer.text();
		tk = lexer.type();
		if (tk == '}')
			break;
	}
//	printf("This is a statement list!\n");
	return SYN_SUCCESS;
}

int syntax::read_state()
//Has red token "scanf"
{
	int id;
	lexer.nextsym();
	sym = lexer.text();
	tk = lexer.type();
	if (tk != '(') {
		puts("Expect a '(' in a scanf statement!");
		error();
		return SYN_FAIL;
	}
	lexer.nextsym();
	sym = lexer.text();
	tk = lexer.type();
	while (1) {
		if (identifier() == SYN_FAIL) {
			return SYN_FAIL;
		}
		id = symtable.search(tmp_ident, nowlevel);
		if (symtable.table[id].kind != KIND_VAR) {
			puts("Can only read into a variable!");
			error();
			return SYN_FAIL;
		}
		else if (symtable.table[id].special > 0) {
			puts("Cannot read into an array!");
			error();
			return SYN_FAIL;
		}

		fours.newfour(FOUR_SCANF, symtable.table[id].type, symtable.table[id].special, id);
		lexer.nextsym();
		sym = lexer.text();
		tk = lexer.type();
		if (tk == ',') {
			lexer.nextsym();
			sym = lexer.text();
			tk = lexer.type();
			continue;
		}
		if (tk == ')')
			break;
		puts("Expect a ')' in a scanf statement!");
		error();
		return SYN_FAIL;
	}
	//printf("This is a scanf statement!\n");
	return SYN_SUCCESS;
}

int syntax::print_state()
//Has red token "printf"
{
	int id,ansid,ischar;
	ansid = symtable.search(TMP_ANS, nowlevel);
	lexer.nextsym();
	sym = lexer.text();
	tk = lexer.type();
	if (tk != '(') {
		puts("Expect a '(' in a printf statement!");
		error();
		return SYN_FAIL;
	}
	lexer.nextsym();
	sym = lexer.text();
	tk = lexer.type();
	if (tk == TK_VALSTRING) {
		//add into string table
		symtable.strconst.push_back(sym);
		sprintf(buf, "%s%d", STRING_PRE, symtable.strconst.size());
		symtable.insert(buf, nowlevel, TYPE_STRING, KIND_CONST, symtable.strconst.size() - 1, 0);
		id = symtable.search(buf, nowlevel);
		fours.newfour(FOUR_PRINTF, TYPE_STRING, 0, id);
		lexer.nextsym();
		sym = lexer.text();
		tk = lexer.type();
		if (tk == ')') {
			//printf("This is a printf statement!\n");
			return SYN_SUCCESS;
		}
		else if (tk == ',') {
			lexer.nextsym();
			sym = lexer.text();
			tk = lexer.type();
			if (expression() == SYN_FAIL) {
				return SYN_FAIL;
			}
			ansid = symtable.search(TMP_ANS, nowlevel);
			fours.newfour(FOUR_PRINTF, symtable.table[ansid].type, 0, ansid);
			if (tk != ')') {
				puts("Expect a ')' in a printf statement!");
				error();
				return SYN_FAIL;
			}
		}
		else {
			puts("Expect a ')' or ',' in a printf statement!");
			error();
			return SYN_FAIL;
		}
	}
	else{
		ischar = expression();
		if (ischar == SYN_FAIL) {
			return SYN_FAIL;
		}
		if(ischar==SYN_EXPRESSION_IS_CHAR)
			fours.newfour(FOUR_PRINTF, TYPE_CHAR, 0, ansid);
		else
			fours.newfour(FOUR_PRINTF, TYPE_INT, 0, ansid);
		if (tk != ')') {
			puts("Expect a ')' in a printf statement!");
			error();
			return SYN_FAIL;
		}
	}
	//printf("This is a printf statement!\n");
	return SYN_SUCCESS;
}

int syntax::return_state()
//Has red token "return"
{
	int ansid;
	ansid = symtable.search(TMP_ANS, nowlevel);
	lexer.nextsym();
	sym = lexer.text();
	tk = lexer.type();
	if (tk == '(') {
		if (!is_func) {
			puts("Cannot return with value in a void function!");
			error();
			return SYN_FAIL;
		}
		lexer.nextsym();
		sym = lexer.text();
		tk = lexer.type();
		if (expression() == SYN_FAIL) {
		
			return SYN_FAIL;
		}
		if (tk != ')') {
			puts("Expect a ')' in a return statement!");
			error();
			return SYN_FAIL;
		}
		lexer.nextsym();
		sym = lexer.text();
		tk = lexer.type();
		if (is_main)
			fours.newfour(FOUR_END, 0, 0, 0);
		else
			fours.newfour(FOUR_RETURN, 0, 0, ansid);
	}
	else if (is_main)
		fours.newfour(FOUR_END, 0, 0, 0);
	else
		fours.newfour(FOUR_RETURN, 0, 0, 0);
//	printf("This is a return statement!\n");
	return SYN_SUCCESS;
}



syntax::syntax()
{
	nowlevel = 0;
	globaladdr = 0;
	func_cnt = 0;
	loop_cnt = 0;
	case_cnt = 0;
	switch_cnt = 0;
	if_cnt = 0;
}

int syntax::program(FILE *fin,char *dir)
{
	int tmp;
	int flg_var_to_func = 0;
	int flg_main = 0;
	symtable.table[0].name = " ";

	nowlevel = 0;
	lexer.setfile(fin);
	lexer.nextsym();
	sym = lexer.text();
	tk = lexer.type();
	//const declaration
	while (tk == TK_CONST) {
		if (const_declare() == SYN_FAIL) {
			fclose(stdout);
			return SYN_FAIL;
		}
	}
	//var declaration
	while (tk == TK_TYPCHAR || tk == TK_TYPINT) {
		tmp = var_declare();
		if (tmp == SYN_FAIL) {
			fclose(stdout);
			return SYN_FAIL;
		}
		if (tmp == SYN_VAR_DEF_TO_FUNC_DEF) {
			flg_var_to_func = 1;
			break;
		}
	}
	if (flg_var_to_func) {
		func_define(1);
		lexer.nextsym();
		sym = lexer.text();
		tk = lexer.type();
	}
	//function declaration
	while (tk == TK_VOID || tk == TK_TYPINT || tk == TK_TYPCHAR) {
		if (tk == TK_VOID) {
			tmp = proc_define();
			if (tmp == SYN_PROC_DEF_TO_MAIN_DEF) {
				flg_main = 1;
				break;
			}
			if (tmp == SYN_FAIL) {
				fclose(stdout);
				return SYN_FAIL;
			}
		}
		else {
			if (tk == TK_TYPCHAR)
				tmp_head = TYPE_CHAR;
			else
				tmp_head = TYPE_INT;
			if (func_define(0) == SYN_FAIL) {
				fclose(stdout);
				return SYN_FAIL;
			}
		}
		lexer.nextsym();
		sym = lexer.text();
		tk = lexer.type();
	}
	if (flg_main == 0) {
		puts("There should be an function define!");
		error();
		fclose(stdout);
		return SYN_FAIL;
	}
	nowlevel = 0;
	if (main_func() == SYN_FAIL) {
		fclose(stdout);
		return SYN_FAIL;
	}
	printf("All code has been analyzed!\n");
	if (global_syntax_err) {
		return SYN_FAIL;
	}
	fclose(stdout);
	sprintf(buf, "%ssyn_table_out.txt", dir);
	freopen(buf, "w", stdout);
	symtable.output();
	fclose(stdout);


	sprintf(buf, "%sfour.txt", dir);
	freopen(buf, "w", stdout);
	fours.output(&symtable);
	fclose(stdout);

	fours.symplify();

	sprintf(buf, "%sfour_simp.txt", dir);
	freopen(buf, "w", stdout);
	fours.output(&symtable);
	fclose(stdout);
///	printf("Mips code has been generated!\n");
	return SYN_SUCCESS;

}

void syntax::sprint_four(char * buf, four & f)
{
	int type,id;
	char reg1[30], reg2[30],op[30];
	sym_term src1s, src2s, dess;
	buf[0] = 0;
	type = f.type;
	dess = symtable.table[f.des];
	if (type != FOUR_SCANF && type != FOUR_PRINTF && type != FOUR_LI && type!=FOUR_LABEL)
		src1s = symtable.table[f.src1];
	if (type != FOUR_SCANF)
		src2s = symtable.table[f.src2];
	switch (type) {
	case FOUR_NOP:return;
	case FOUR_MOV: {
		if (strcmp(src1s.name, TMP_ANS) != 0) {
			//if move from var,read into $t0
			if (src1s.level == 0)
				sprintf(buf, "lw $t0 %s\n", src1s.name);
			else
				sprintf(buf, "lw $t0 -%d($fp)\n", src1s.addroffset);

			if (strcmp(dess.name, TMP_ANS) != 0) {
				//if move to var
				if (dess.level == 0)
					sprintf(buf, "%ssw $t0 %s\n", buf, dess.name);
				else
					sprintf(buf, "%ssw $t0 -%d($fp)\n", buf, dess.addroffset);
			}
			else//move to $s0
				sprintf(buf, "%smove $s0 $t0\n", buf);
		}
		else {
			//move from $s0
			if (strcmp(dess.name, TMP_ANS) != 0) {
				//if move to var
				if (dess.level == 0)
					sprintf(buf, "%ssw $s0 %s\n", buf, dess.name);
				else
					sprintf(buf, "%ssw $s0 -%d($fp)\n", buf, dess.addroffset);
			}
		}
		return;
	}
	case FOUR_ADD:case FOUR_SUB:case FOUR_BIG:case FOUR_LESS:
	case FOUR_GEQ:case FOUR_LEQ:case FOUR_NEQ:case FOUR_SAME: {
		if (type == FOUR_ADD) strcpy(op, "add");
		if (type == FOUR_SUB) strcpy(op, "sub");
		if (type == FOUR_BIG) strcpy(op, "sgt");
		if (type == FOUR_LESS) strcpy(op, "slt");
		if (type == FOUR_GEQ) strcpy(op, "sge");
		if (type == FOUR_LEQ) strcpy(op, "sle");
		if (type == FOUR_NEQ) strcpy(op, "sne");
		if (type == FOUR_SAME) strcpy(op, "seq");
		if (strcmp(src1s.name, TMP_ANS) == 0)
			strcpy(reg1, "$s0");
		else {
			//load $t0 from var
			strcpy(reg1, "$t0");
			if (src1s.level == 0)
				sprintf(buf, "%slw $t0 %s\n",buf, src1s.name);
			else
				sprintf(buf, "%slw $t0 -%d($fp)\n",buf, src1s.addroffset);
		}
		if (strcmp(src2s.name, TMP_ANS) == 0)
			strcpy(reg2, "$s0");
		else {
			//load $t1 from var
			strcpy(reg2, "$t1");
			if (src2s.level == 0)
				sprintf(buf, "%slw $t1 %s\n",buf, src2s.name);
			else
				sprintf(buf, "%slw $t1 -%d($fp)\n",buf, src2s.addroffset);
		}
		sprintf(buf, "%s%s $s0 %s %s\n", buf, op, reg1, reg2);
		if (strcmp(dess.name, TMP_ANS) != 0) {
			//save the answer to var
			if (dess.level == 0)
				sprintf(buf, "%ssw $s0 %s\n", buf, dess.name);
			else
				sprintf(buf, "%ssw $s0 -%d($fp)\n", buf, dess.addroffset);
		}
		return;
	}

	case FOUR_MUL:case FOUR_DIV: {
		if (type == FOUR_MUL) strcpy(op, "mult");
		if (type == FOUR_DIV) strcpy(op, "div");
		if (strcmp(src1s.name, TMP_ANS) == 0)
			strcpy(reg1, "$s0");
		else {
			//load $t0 from var
			strcpy(reg1, "$t0");
			if (src1s.level == 0)
				sprintf(buf, "%slw $t0 %s\n",buf, src1s.name);
			else
				sprintf(buf, "%slw $t0 -%d($fp)\n",buf, src1s.addroffset);
		}
		if (strcmp(src2s.name, TMP_ANS) == 0)
			strcpy(reg2, "$s0");
		else {
			//load $t1 from var
			strcpy(reg2, "$t1");
			if (src2s.level == 0)
				sprintf(buf, "%slw $t1 %s\n", buf,src2s.name);
			else
				sprintf(buf, "%slw $t1 -%d($fp)\n", buf,src2s.addroffset);
		}
		sprintf(buf, "%s%s %s %s\n", buf, op, reg1, reg2);
		//move ans to $s0
		sprintf(buf, "%smflo $s0\n",buf);
		if (strcmp(dess.name, TMP_ANS) != 0) {
			if (dess.level == 0)
				sprintf(buf, "%ssw $s0 %s\n", buf, dess.name);
			else
				sprintf(buf, "%ssw $s0 -%d($fp)\n", buf, dess.addroffset);
		}
		return;
	}

	case FOUR_LI: {
		sprintf(buf, "li $s0 %d\n", f.src1);
		if (strcmp(dess.name, TMP_ANS) == 0)
			return;
		if (dess.level == 0)
			sprintf(buf, "%ssw $s0 %s\n", buf, dess.name);
		else
			sprintf(buf, "%ssw $s0 -%d($fp)\n", buf, dess.addroffset);
		return;
	}

	case FOUR_GETARR: {
		if (src1s.level == 0) {
			//read from .data
			strcpy(reg1, src1s.name);
			if (strcmp(src2s.name, TMP_ANS) == 0)
				strcpy(reg2, "$s0");
			else {
				if (src2s.level == 0)
					sprintf(buf, "%slw $t0 %s\n", buf, src2s.name);
				else
					sprintf(buf, "%slw $t0 -%d($fp)\n", buf, src2s.addroffset);
				strcpy(reg2, "$t0");
			}
			sprintf(buf, "%ssll %s %s 2\n", buf, reg2, reg2);
			sprintf(buf, "%slw $s0 %s(%s)\n", buf, reg1, reg2);
			return;
		}
		else {
			//read from local var
			//base offset= src1s.addroffset
			//get arr offset(src2) in reg1
			if (strcmp(src2s.name, TMP_ANS) == 0)
				strcpy(reg1, "$s0");
			else {
				if (src2s.level == 0)
					sprintf(buf, "%slw $t1 %s\n", buf, src2s.name);
				else
					sprintf(buf, "%slw $t1 -%d($fp)\n", buf, src2s.addroffset);
				strcpy(reg1, "$t1");
			}
			sprintf(buf, "%ssll %s %s 2\n", buf, reg1, reg1);
			//get total addr in $t0=$fp-baseoffset-arroffset
			sprintf(buf, "%ssub $t0 $fp %d\n", buf, src1s.addroffset);
			sprintf(buf, "%ssub $t0 $t0 %s\n", buf, reg1);
			//load word to $s0
			sprintf(buf, "%slw $s0 ($t0)\n", buf);
		}
		return;
	}

	case FOUR_SETARR: {
		if (dess.level == 0) {
			//write to  .data
			strcpy(reg1, dess.name);
			if (strcmp(src2s.name, TMP_ANS) == 0)
				strcpy(reg2, "$s0");
			else {
				if (src2s.level == 0)
					sprintf(buf, "%slw $t0 %s\n", buf, src2s.name);
				else
					sprintf(buf, "%slw $t0 -%d($fp)\n", buf, src2s.addroffset);
				strcpy(reg2, "$t0");
			}
			sprintf(buf, "%ssll %s %s 2\n", buf, reg2, reg2);
			sprintf(buf, "%ssw $s0 %s(%s)\n", buf, dess.name, reg2);
			return;
		}
		else {
			//read from local var
			//base offset= dess.addroffset
			//get arr offset(src2) in reg1
			if (strcmp(src2s.name, TMP_ANS) == 0)
				strcpy(reg1, "$s0");
			else {
				if (src2s.level == 0)
					sprintf(buf, "%slw $t1 %s\n", buf, src2s.name);
				else
					sprintf(buf, "%slw $t1 -%d($fp)\n", buf, src2s.addroffset);
				strcpy(reg1, "$t1");
			}
			sprintf(buf, "%ssll %s %s 2\n", buf, reg1, reg1);
			//get total addr in $t0=$fp-baseoffset-arroffset
			sprintf(buf, "%ssub $t0 $fp %d\n", buf, dess.addroffset);
			sprintf(buf, "%ssub $t0 $t0 %s\n", buf, reg1);
			//save word to addr
			sprintf(buf, "%slw $s0 ($t0)\n", buf);
		}
		return;
	}

	case FOUR_VERSE: {
		sprintf(buf, "not $s0 $s0\naddi $s0 $s0 1\n");
		return;
	}

	case FOUR_JUMP: {
		sprintf(buf, "j %s\n", dess.name);
		return;
	}
	case FOUR_BEQZ: {
		sprintf(buf, "beqz $s0 %s\n", dess.name);
		return;
	}
	case FOUR_BNE: {
		//load left to reg1($t0 or $s0)
		if (strcmp(src1s.name, TMP_ANS) == 0)
			strcpy(reg1, "$s0");
		else {
			//load $t0 from var
			strcpy(reg1, "$t0");
			if (src1s.level == 0)
				sprintf(buf, "%slw $t0 %s\n", buf, src1s.name);
			else
				sprintf(buf, "%slw $t0 -%d($fp)\n", buf, src1s.addroffset);
		}
		if (strcmp(src2s.name, TMP_ANS) == 0)
			strcpy(reg2, "$s0");
		else {
			//load $t1 from var
			strcpy(reg2, "$t1");
			if (src2s.level == 0)
				sprintf(buf, "%slw $t1 %s\n", buf, src2s.name);
			else
				sprintf(buf, "%slw $t1 -%d($fp)\n", buf, src2s.addroffset);
		}
		sprintf(buf, "%sbne %s %s %s\n", buf, reg1, reg2, dess.name);
		return;
	}


	case FOUR_LABEL: {
		sprintf(buf, "%s:\n", dess.name);
		if (strstr(dess.name, LAB_FUNC) ==NULL)
			return ;
		sprintf(buf, "%smove $fp $s1\n", buf);
		sprintf(buf, "%ssubi $sp $fp %d\n", buf, f.src1);
		sprintf(buf,"%smove $s1 $sp\n",buf);
		sprintf(buf, "%ssw $ra -4($fp)\n",buf);
		return;
	}
	case FOUR_CALL: {
		sprintf(buf, "j %s\n", dess.name);
		return;
	}
	case FOUR_PUSH: {
		if (strcmp(dess.name, TMP_ANS) == 0)
			strcpy(reg1, "$s0");
		else {
			//load $t0 from var
			strcpy(reg1, "$t0");
			if (src1s.level == 0)
				sprintf(buf, "%slw $t0 %s\n", buf, dess.name);
			else
				sprintf(buf, "%slw $t0 -%d($fp)\n", buf, dess.addroffset);
		}
		sprintf(buf, "%ssw %s ($sp)\n", buf, reg1);
		sprintf(buf, "%ssubi $sp $sp 4\n", buf);
		return;
	}
	case FOUR_GETPC: {
		sprintf(buf, "jal $getpc\naddi $ra $ra 8\n");
		return;
	}
	case FOUR_GRETURN: {
		sprintf(buf, "move $s0 $v0\n");

		return;
	}
	case FOUR_PUSHFP: {
		sprintf(buf, "sw $fp ($s1)\nsubi $sp $sp 8\n");
		return;
	}
	case FOUR_RETURN: {
		if (strcmp(dess.name, TMP_ANS) == 0)
			sprintf(buf, "move $v0 $s0\n");
		sprintf(buf, "%slw $ra -4($fp)\n", buf);
		sprintf(buf, "%smove $sp $fp\nlw $fp ($fp)\nmove $s1 $sp\n", buf);
		sprintf(buf, "%sjr $ra\n", buf);
		return;
	}
	case FOUR_PRINTF: {
		if (f.src1 == TYPE_INT) {
			sprintf(buf, "li $v0 1\nmove $a0 $s0\nsyscall\n");
		}
		else if (f.src1 == TYPE_CHAR) {
			sprintf(buf, "li $v0 11\nandi $a0 $s0 0x00ff\nsyscall\n");
		}
		else if (f.src1 == TYPE_STRING) {
			sprintf(buf, "li $v0 4\nla $a0 %s\nsyscall\n", dess.name);
		}
		sprintf(buf, "%sli $v0 11\nli $a0 %d\nsyscall\n",buf,'\n');
		return;
	}
	case FOUR_SCANF: {
		if (f.src1 == TYPE_CHAR && f.src2==0) 
			sprintf(buf, "li $v0 12\nsyscall\n");
		else if (f.src1 == TYPE_CHAR && f.src2 > 0) {
			//set $a0 to be the address of buffer
			if (symtable.table[f.des].level == 0) {
				sprintf(buf, "%sla $a0 %s\nli $a1 %d\n", buf, symtable.table[f.des].name,symtable.table[f.des].special);
			}
			else {
				sprintf(buf, "%ssubi $a0 $fp %d\n", buf, symtable.table[f.des].addroffset);
			}
			sprintf(buf, "%sli $v0 8\nsyscall\n",buf);
		}
		else
			sprintf(buf, "li $v0 5\nsyscall\n");
		if (strcmp(dess.name, TMP_ANS) == 0)
			sprintf(buf, "%smove $s0 $v0\n", buf);
		else {
			if (dess.level == 0)
				sprintf(buf, "%ssw $v0 %s\n", buf, dess.name);
			else
				sprintf(buf, "%ssw $v0 -%d($fp)\n", buf, dess.addroffset);
		}
		return;
	}
	case FOUR_END: {
		sprintf(buf, "li $v0 10\nsyscall\n");
		return;
	}
	}

}

int syntax::mips(FILE * fout)
{
	int i,len,type;
	char mainlab[30];
	std::string str;
	//.data declare
	fprintf(fout, ".data\n");
	for (i = 1; i <= symtable.tablecnt; i++) {
		if (symtable.table[i].level > 0 || (symtable.table[i].kind != KIND_VAR && symtable.table[i].kind != KIND_CONST))
			break;
		len = symtable.table[i].special;
		if (len == 0 || symtable.table[i].kind==KIND_CONST	)
			len = 1;
		if (len > 1)
			fprintf(fout, "%s: .word 0:%d\n", symtable.table[i].name, len);
		else
			fprintf(fout, "%s: .word %d\n", symtable.table[i].name, symtable.table[i].special);
	}
	std::vector<std::string>::iterator it; i = 0;
	for (it = symtable.strconst.begin(); it != symtable.strconst.end(); it++) {
		i++;
		sprintf(buf, "%s%d", STRING_PRE, i);
		fprintf(fout, "%s: .asciiz %s\n", buf, it->c_str());
	}
	//prework
	fprintf(fout, ".text\n");
	fprintf(fout, "j %smain\n", LAB_FUNC);
	fprintf(fout, "$getpc: jr $ra\n");
	//functions
	sprintf(mainlab, "%smain", LAB_FUNC);
	for (i = 1; i <= fours.cnt; i++) {
		type = fours.fours[i].type;
		if (type == FOUR_NOP)
			continue;
		if (type == FOUR_LABEL && strcmp(symtable.table[fours.fours[i].des].name,mainlab)==0) {
			break;
		}
		sprint_four(buf, fours.fours[i]);
		fprintf(fout, "%s", buf);
	}
	//main function
	fprintf(fout, "%s:\n", mainlab);
	fprintf(fout, "li $fp 0x7fff0000\n");
	fprintf(fout, "li $sp,0x%x\n", 0x7ffe0000);
	fprintf(fout, "move $s1 $sp\n");
	i++;
	for (; i <= fours.cnt; i++) {
		sprint_four(buf, fours.fours[i]);
		fprintf(fout, "%s", buf);
	}
	return 0;
}

four_table::four_table()
{
	cnt = 0;
}

void four_table::newfour(int type1, int src11, int src21, int des1)
{
	cnt++;
	fours[cnt].newfour( type1,  src11,  src21,  des1);
}

void four_table::output(sym_table *symtable)
{
	int type;
	FILE *fp;
	
	for (int i = 1; i <= cnt; i++) {
		type = fours[i].type;
		if (type == FOUR_DELED)
			continue;
		printf( "%s ", fours[i].typestr());
		if (type != FOUR_SCANF && type != FOUR_PRINTF && type != FOUR_LI) {
			if (symtable->table[fours[i].src1].name != NULL)
				printf("%s ", symtable->table[fours[i].src1].name);
			else
				putchar(' ');
		}
		else
			printf( "%d ", fours[i].src1);
		if (type != FOUR_SCANF) {
			if (symtable->table[fours[i].src2].name != NULL)
				printf( "%s ", symtable->table[fours[i].src2].name);
			else
				putchar(' ');
		}
		else
			printf( "%d ", fours[i].src2);
		printf("%s\n", symtable->table[fours[i].des].name);
	}
	
}

void four_table::symplify()
{
	int oldcnt,type,i,left,right;
	four x;
	for (i = 1; i <= cnt; i++) {
		type = fours[i].type;
		if (type == FOUR_LABEL) {
			x.newfour(FOUR_NOP, 0, 0, 0);
			insert(x, i);
			i++;
			continue;
		}
		if (type == FOUR_JUMP || type == FOUR_BEQZ || type==FOUR_CALL || type==FOUR_RETURN||type==FOUR_END) {
			i++;
			x.newfour(FOUR_NOP, 0, 0, 0);
			insert(x, i);
		}
	}
	x.newfour(FOUR_NOP, 0, 0, 0);
	insert(x, cnt);
	oldcnt = cnt + 1;
	while (oldcnt != cnt) {
		oldcnt = cnt;
		left = 1; right = 1;
		while (left < cnt) {
			while (fours[right].type != FOUR_NOP)
				right++;
			for (i = left; i < right; i++) {
				strcpy(buf, symtable.table[fours[i].des].name);
				if (fours[i].type == FOUR_MOV && fours[i + 1].type == FOUR_MOV && fours[i].des == fours[i + 1].src1 && buf[0]=='$') {
					fours[i + 1].src1 = fours[i].src1;
					fours[i].type = FOUR_DELED;
				}
				else {
					if (fours[i].type == FOUR_MOV && fours[i].src1 == fours[i].des)
						fours[i].type = FOUR_DELED;
				}
			}
			left = right;
			right = right + 1;
		}
		break;
		
		left = 0; right = 1;
		while (right <= cnt) {
			if (fours[right].type != FOUR_DELED){
				left++;
				fours[left] = fours[right];
			}
			right++;
		}
		cnt = left;
	}

}

void four_table::insert(four x, int index)
{
	int i;
	for (i = cnt + 1; i > index; i--)
		fours[i] = fours[i - 1];
	fours[i] = x;
	cnt++;
}


