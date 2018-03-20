#pragma once
class dictionary
{
private:
	char words[200][30];
	int vals[200];
	int defv, wordcnt;
public:
	dictionary(int nul);
	void addword(const char* word, int val);
	int match(const char* str);
	~dictionary();
};
