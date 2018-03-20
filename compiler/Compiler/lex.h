#pragma once

#define MAX_TOKEN_LENGTH 100000

#define TK_ERROR		-1

#define TK_REALVAL		1001
#define TK_VALINT		1002
#define TK_VALSTRING	1003
#define TK_UNKNOWN		1004
#define TK_VALCHAR		1005
#define TK_TYPCHAR		1006
#define TK_TYPINT		1007
#define TK_GEQ			1008
#define TK_LEQ			1009
#define TK_IDENT		1010
#define TK_IF			1011
#define TK_ELSE			1012
#define TK_SWITCH		1013
#define TK_CASE			1014
#define TK_VOID			1015
#define TK_CONST		1016
#define TK_DEFAULT		1017
#define TK_RETURN		1018
#define TK_WHILE		1019
#define TK_SAME			1020
#define TK_NEQ			1021
#define TK_SCANF		1022
#define TK_PRINTF		1023

#define TK_EQUAL		'='
#define TK_COMMA		','
#define TK_LPAR			'('
#define TK_RPAR			')'
#define TK_LBRACKET		'['
#define TK_RBRACKET		']'
#define TK_LBRACE		'{'
#define TK_RBRACE		'}'
#define TK_DOT			'.'
#define TK_GTR			'>'
#define TK_LES			'<'
#define TK_COLON		':'
#define TK_MUL			'*'
#define TK_DIV			'/'
#define TK_PLUS			'+'
#define TK_SUB			'-'
#define TK_SEMICOLON	';'

#define TK_NOT			TK_UNKNOWN

#define ERR_EOF			-1

extern char *token_str[2333];

class lex {
private:
	char ch;
	char buf[MAX_TOKEN_LENGTH];
	int ty, len,linecnt;
	FILE* fin;
	int errno;

public:
	void nextchar();
	lex();
	int type() { 
		return ty; 
	}
	int lineno() {
		return linecnt;
	}
	const char* text() { 
		return buf; 
	}
	int nextsym();
	void setfile(FILE* fin) { 
		this->fin = fin; 
	}
	void close() { 
		fclose(fin); 
	}
	int skipline();
//	void reset() { fseek(fin, 0, SEEK_SET); ch = ' '; }
	
};