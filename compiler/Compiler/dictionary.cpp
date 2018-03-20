#include "stdafx.h"
#include<cstring>
#include "dictionary.h"

inline int ord(char ch) {
	if (ch >= 'A'&&ch <= 'Z')
		return ch - 'A' + 26;
	else if (ch >= 'a'&&ch <= 'z')
		return ch - 'a';
	return 52;
}

dictionary::dictionary(int nul) {
	memset(words, 0, sizeof(words));
	memset(vals, 0, sizeof(vals));
	wordcnt = 0;
	defv = nul;
}

int dictionary::match(const char* mode) {
	for (int i = 1; i<=wordcnt; i++) {
		if (strcmp(mode, words[i])==0)
			return vals[i];
	}
	return defv;
}

void dictionary::addword(const char* word, int val) {
	int flag = 0,len=strlen(word);
	for (int i = 1; i<=wordcnt; i++) {
		if (strcmp(words[i],word)==0) {
			flag = 1;
			break;
		}
	}
	if (!flag) {
		wordcnt++;
		strcpy(words[wordcnt], word);
		vals[wordcnt] = val;
	}
}



dictionary::~dictionary()
{

}