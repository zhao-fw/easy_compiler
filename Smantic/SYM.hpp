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
	int type;		//符号类型，数字（int=1）
	vector<int> plist;//形参列表，记录形参类型
	SemanticListNode* nextlist;
	SemanticListNode* falselist;
	SemanticListNode* truelist;
public:
	SYM();
	SYM(int sym_id, string n, int l);
	SYM(int sym_id, string n, int l, int t);
	//~SYM();
	void set_line(int l);
	int get_line();
	void set_name(string n);
	string get_name();
	void set_symbol(int sym);
	int get_symbol();
	void set_quad(int q) { quad = q; }
	int get_quad() { return quad; }
	void set_plist(vector<int> p) { plist = p; }
	vector<int> get_plist() { return plist; }
	void set_truelist(SemanticListNode* truelist);
	SemanticListNode* get_truelist();
	void set_falselist(SemanticListNode* falselist);
	SemanticListNode* get_falselist();
	void set_nextlist(SemanticListNode* nextlist);
	SemanticListNode* get_nextlist();
	void set_type(int t) { type = t; }
	int get_type() { return type; }
	void set_symbol_name(const char* c);
};
SYM::SYM()
{
	symbol_id = 0;
	name = "";
	place = 0;
	line = 0;
	quad = 0;
	type = 0;
	nextlist = NULL;
	falselist = NULL;
	truelist = NULL;
}
SYM::SYM(int sym_id, string n, int l)
{
	symbol_id = sym_id;
	name = n;
	line = l;
	quad = 0;
	type = 0;
	nextlist = NULL;
	falselist = NULL;
	truelist = NULL;
}
SYM::SYM(int sym_id, string n, int l, int t)
{
	symbol_id = sym_id;
	name = n;
	line = l;
	quad = 0;
	type = t;
	nextlist = NULL;
	falselist = NULL;
	truelist = NULL;
}

// 都是些set、get方法
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
void SYM::set_truelist(SemanticListNode* t)
{
	truelist = t;
}
SemanticListNode* SYM::get_truelist()
{
	return truelist;
}
void SYM::set_falselist(SemanticListNode* f)
{
	falselist = f;
}
SemanticListNode* SYM::get_falselist()
{
	return falselist;
}
void SYM::set_nextlist(SemanticListNode* n)
{
	nextlist = n;
}
SemanticListNode* SYM::get_nextlist()
{
	return nextlist;
}
void SYM::set_symbol_name(const char* c)
{
	strcpy(symbol_name, c);
}