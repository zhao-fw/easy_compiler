/************************************************************************
* 定义了IntermediateCode类，用来存储生成过程中的全部中间代码（四元式）  *
*		code：vector容器，存放四元式的集合				                *
************************************************************************/

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

public:
	IntermediateCode() {}
	~IntermediateCode() {}
	
	// 输出四元式
	void emit(string op, string arg1, string arg2, string des);
	void emit(Quaternary q);

	// 回填：将list链中的所有四元式的des置为quad
	void back_patch(SemanticListNode* nextlist, int quad);

	// 返回值：添加新的四元式的序号（即当前大小）
	int nextquad();
	int size() { return code.size(); }

	// 打印结果
	void print(string file = "out_quad.txt");

	// 返回code
	vector<Quaternary>& getCode() { return code; }
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
void IntermediateCode::print(string file)
{
	ofstream out; //文件写
	out.open(file);
	int count = 0;
	for (vector<Quaternary>::iterator iter = code.begin(); iter != code.end(); iter++) {
		out << count++ << ' ';
		out << "(" << iter->op << ",";
		out << iter->arg1 << ",";
		out << iter->arg2 << ",";
		out << iter->des << ")" << endl;
	}
	out.close();
	return;
}
int IntermediateCode::nextquad()
{
	return code.size();
}