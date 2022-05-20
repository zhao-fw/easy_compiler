#pragma once

#include <string>

using namespace std;

struct SemanticListNode {
	int pos;
	SemanticListNode* next;
};

/*
 * 建立符号（终结与非终结符）记录符号的属性
*/
class SYM
{
private:
	int symbol_id;	// 符号的id号
	char symbol_name[20] = { 0 };
	string name;	// 标识符的名称
	int place;		// 符号在符号表中的位置
	int line;		// 符号的行数，与Lexer中对应
	int quad;		// M,N需要考虑回填
	SemanticListNode* nextlist;
	SemanticListNode* falselist;
	SemanticListNode* truelist;
public:
	SYM();
	SYM(int sym_id, string n, int l) { symbol_id = sym_id; name = n; line = l; };
	//~SYM();
	void set_line(int l);
	int get_line();
	void set_name(string n);
	string get_name();
	void set_symbol(int sym);
	int get_symbol();

	void set_symbol_name(const char* c);
};
SYM::SYM()
{
	symbol_id = 0;
	name = "";
	place = 0;
	line = 0;
	quad = 0;
	nextlist = NULL;
	falselist = NULL;
	truelist = NULL;
}
void SYM::set_line(int l)
{
	line = l;
}
int SYM::get_line()
{
	return line;
}
void SYM::set_name(string n)
{
	name = n;
}
string SYM::get_name()
{
	return name;
}
void SYM::set_symbol(int sym)
{
	symbol_id = sym;
}
int SYM::get_symbol()
{
	return symbol_id;
}

void SYM::set_symbol_name(const char* c)
{
	strcpy(symbol_name, c);
}
