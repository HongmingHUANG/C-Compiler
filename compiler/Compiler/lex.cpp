#pragma once
#include "stdafx.h"
#include "dictionary.h"
#include "lex.h"
#include<ctype.h>
#include<cstring>

static dictionary *preserve_words;
static int kw_init_flg = 0;
char *token_str[2333];

inline int in_stringval_range(char ch) {
	if (ch == 32 || ch == 33 || (ch >= 35 && ch <= 126))
		return 1;
	else
		return 0;
}

inline int in_indentifier_range(char ch) {
	if (isalpha(ch) || ch == '_' || isdigit(ch))
		return 1;
	else
		return 0;
}

inline int in_charval_range(char ch) {
	if (ch == '+' || ch == '-' || isalpha(ch) || isdigit(ch))
		return 1;
	else
		return 0;
}

void lex::nextchar()
{
	ch = fgetc(fin);
	if (ch == '\n')
		linecnt++;
}

lex::lex() {
	ch = ' ';
	linecnt = 0;
		preserve_words = new dictionary(TK_IDENT);
		preserve_words->addword("int", TK_TYPINT);
		preserve_words->addword("char", TK_TYPCHAR);
		preserve_words->addword("void", TK_VOID);
		preserve_words->addword("const", TK_CONST);
		preserve_words->addword("return", TK_RETURN);

		preserve_words->addword("scanf", TK_SCANF);
		preserve_words->addword("printf", TK_PRINTF);

		preserve_words->addword("if", TK_IF);
		preserve_words->addword("switch", TK_SWITCH);
		preserve_words->addword("case", TK_CASE);
		preserve_words->addword("default", TK_DEFAULT);
		preserve_words->addword("while", TK_WHILE);
		
}

int lex::nextsym() {
	while (isspace(ch) && !feof(fin)) {
		nextchar();
	}
	if (feof(fin)) {
		len = 0;
		return ty = TK_UNKNOWN;
	}
	if (isdigit(ch)) {
	//is a number 
		len = 0;
		ty = TK_VALINT;
		while (!feof(fin) && isdigit(ch)) {
			buf[len] = ch;
			len++;
			nextchar();
		}
		buf[len] = 0;
		return ty;
	}
	if (ch == '\'') {
		//is a char
		buf[0] = ch;
		len = 1;
		nextchar();
		if (!feof(fin) || in_charval_range(ch) == 1) {
			buf[len] = ch;
			len++;
			nextchar();
			if (feof(fin) || ch != '\'') {
				buf[len] = 0;
				ty = TK_UNKNOWN;
			}
			else {
				buf[len] = ch;
				len++;
				buf[len] = 0;
				ty = TK_VALCHAR;
				nextchar();
			}
		}
		else {
			buf[len] = 0;
			ty = TK_UNKNOWN;
		}
		return ty;
	}
	if (ch == '\"') {
		//is a string
		buf[0] = ch;
		len = 1;
		nextchar();
		ty = TK_VALSTRING;
		while (!feof(fin) && ch != '\"' && ch != '\n') {
			if (in_stringval_range(ch) == 0) {
				ty = TK_UNKNOWN;
			}
			buf[len] = ch;
			len++;
			nextchar();
		}
		if (ch == '\"') {
			buf[len] = ch;
			len++;
			buf[len] = 0;
			nextchar();
		}
		else {
			buf[len] = 0;
			ty = TK_UNKNOWN;
		}
		return ty;
	}
	if (in_indentifier_range(ch)) {
	//is an identifier
		len = 0;
		while (in_indentifier_range(ch)) {
			buf[len] = ch;
			len++;
			nextchar();
		}
		buf[len] = 0;
		ty = preserve_words->match(buf);
		return ty;
	}
	
	
	
	switch (ch) {
		case '{': case '}': 
		case '[': case ']': 
		case '(': case ')':
		case '+':case '-': 
		case '*':case '/':
		case ',': case ':': 
		case ';':
		//simple symbol without consequence symbols
			ty = ch;
			buf[0] = ch;
			buf[1] = 0;
			nextchar();
			break;

		case '<': case '>': case '=': case '!':
		//logistic symbols
			buf[0] = ch;
			nextchar();
			if (ch == '=') {
				buf[1] = ch;
				buf[2] = 0;
				ty = buf[0] == '<' ? TK_LEQ : buf[0] == '>' ? TK_GEQ : buf[0] == '=' ? TK_SAME : TK_NEQ;
				if (buf[0] == '<')
					ty = TK_LEQ;
				else if (buf[0] == '>')
					ty = TK_GEQ;
				else if (buf[0] == '!')
					ty = TK_NEQ;
				else
					ty = TK_SAME;
				nextchar();
				break;
			}
			else {
				buf[1] = 0;
				ty = buf[0] == '!' ? TK_UNKNOWN : buf[0];
			}
			break;
		default:
			ty = TK_UNKNOWN;
			buf[0] = ch;
			buf[1] = 0;
			nextchar();
			break;
		}
		len = strlen(buf);
	
	return ty;
}

int lex::skipline()
{
	int lastline = linecnt;
	while (lastline == linecnt) {
		if (feof(fin))
			return ERR_EOF;
		nextchar();
	}
	return 1;
}
