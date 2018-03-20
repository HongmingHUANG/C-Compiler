#pragma once
#include "lex.h"
#include "sym_table.h"
#include<stdio.h>
#define SYN_FAIL -1
#define SYN_SUCCESS 1
#define SYN_VAR_DEF_TO_FUNC_DEF 2
#define SYN_PROC_DEF_TO_MAIN_DEF 3
#define SYN_EXPRESSION_IS_CHAR 4

#define LAB_FUNC				"$Lab_func_"
#define LAB_WHILE				"$Lab_loop_"
#define LAB_WHILEEND		"$Lab_loopend_"
#define LAB_CASE				"$Lab_case_"
#define LAB_CASEEND		"$Lab_caseend_"
#define LAB_SWITCHEND	"$Lab_switend_"
#define LAB_DEFAULT			"$Lab_default_"
#define LAB_IFEND				"$Lab_ifend_"
/*
temp 
*/
#define TMP_TERM				"$Tmp_term_"
#define TMP_FACT				"$Tmp_factor_"
#define TMP_SWITOP			"$Tmp_switop_"
#define TMP_ANS				"$ans"
#define TMP_1					"$tmp1"
#define TMP_2					"$tmp2"
#define STRING_PRE			"_string_"

#define FOUR_DELED				-1
#define FOUR_NOP				30000
#define FOUR_MOV			30001
#define FOUR_ADD				30002
#define FOUR_SUB				30003
#define FOUR_MUL				30004
#define FOUR_DIV				30005
#define FOUR_BIG				30006
#define FOUR_LESS				30007
#define FOUR_GEQ				30008
#define FOUR_LEQ				30009
#define FOUR_SAME			30010
#define FOUR_NEQ				30011
#define FOUR_JUMP			30012
#define FOUR_BEQZ			30013
#define FOUR_SCANF			30014
#define FOUR_PRINTF			30015
#define FOUR_RETURN		30016
#define FOUR_LABEL			30017
#define FOUR_GETARR		30018
#define FOUR_SETARR		30019
#define FOUR_LI					30020
#define FOUR_VERSE			30021
#define FOUR_CALL			30022
#define FOUR_PUSH			30023
#define FOUR_GETPC			30024
#define FOUR_END				30025
#define FOUR_GRETURN	30026
#define FOUR_BNE				30027
#define FOUR_PUSHFP		30028



class four {
public:
	int type, src1, src2, des;
	int is_block;
	void  newfour(int type1, int src11, int src21, int des1);
	const char* typestr();
};

class four_table {
public:
	int cnt;
	four fours[20000];
	four_table();
	void  newfour(int type1, int src11, int src21, int des1);
	void output(sym_table *symtable);
	void symplify();
	void insert(four x, int index);

};

class syntax {
private:
	int nowlevel;
	int globaladdr ;
	int func_cnt, loop_cnt, case_cnt, switch_cnt, if_cnt;
	const char *sym;
	int tk;
	lex lexer;
	void error();
	int const_declare();
	int const_define(int type);
	int uint_sys();
	int int_sys();
	int int_judge(char *buf);
	int identifier();
	int declare_head();
	int var_declare();
	int var_define();
	int const_val();
	int func_define(int is_moved);
	int proc_define();
	int compound();
	int parameters();
	int main_func();
	int expression();
	int term();
	int factor();
	int statement();
	int assign_state();
	int if_state();
	int if_case();
	int while_state();
	int switch_state();
	int caselist();
	int case_state();
	int default_state();
	int func_call();
	int proc_call();
	int real_para_list();
	int state_list();
	int read_state();
	int print_state();
	int return_state();
	void sprint_four(char *buf,four &f);


public:
	syntax();
	int program(FILE *fin,char *dir);
	int mips(FILE *fout);

};
