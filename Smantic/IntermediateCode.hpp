#pragma once
#include"SYM.hpp"
#include<iostream>
#include<string>
#include<vector>
using namespace std;
//四元式
struct Quaternary {
	string op;
	string arg1;
	string arg2;
	string des;
};

class IntermediateCode {
private:
	vector<Quaternary> code;
	ofstream out;               //文件写

public:
	IntermediateCode() {
		out.open("out_quad.txt");
	}
	~IntermediateCode() {
		out.close();
	}
	// 输出四元式
	void emit(string op, string arg1, string arg2, string des);
	void emit(Quaternary q);
	// 回填
	void back_patch(SemanticListNode* nextlist, int quad);
	void print();
	int nextquad();
	int size() { return code.size(); }
};

void IntermediateCode::emit(string op, string arg1, string arg2, string des)
{
	emit(Quaternary{ op, arg1, arg2, des });
	return;
}
void IntermediateCode::emit(Quaternary q)
{
	code.push_back(q);
	return;
}
void IntermediateCode::back_patch(SemanticListNode* nextlist, int quad)
{
	SemanticListNode* head = nextlist;
	while (head != NULL)
	{
		code[head->pos].des = to_string(quad);
		head = head->next;
	}
	return;
}
void IntermediateCode::print()
{
	int count = 0;
	for (vector<Quaternary>::iterator iter = code.begin(); iter != code.end(); iter++) {
		out << count++ << ' ';
		out << "(" << iter->op << ",";
		out << iter->arg1 << ",";
		out << iter->arg2 << ",";
		out << iter->des << ")" << endl;
	}
	return;
}
int IntermediateCode::nextquad()
{
	return code.size();
}