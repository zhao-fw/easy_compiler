/************************************************************************
* ������IntermediateCode�࣬�����洢���ɹ����е�ȫ���м���루��Ԫʽ��  *
*		code��vector�����������Ԫʽ�ļ���				                *
************************************************************************/

#pragma once

#include"SYM.hpp"
#include<iostream>
#include<string>
#include<vector>
using namespace std;
//��Ԫʽ
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
	
	// �����Ԫʽ
	void emit(string op, string arg1, string arg2, string des);
	void emit(Quaternary q);

	// �����list���е�������Ԫʽ��des��Ϊquad
	void back_patch(SemanticListNode* nextlist, int quad);

	// ����ֵ������µ���Ԫʽ����ţ�����ǰ��С��
	int nextquad();
	int size() { return code.size(); }

	// ��ӡ���
	void print(string file = "out_quad.txt");

	// ����code
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
	ofstream out; //�ļ�д
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