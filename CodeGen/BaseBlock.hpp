#pragma once

#include<string>
#include<vector>
#include"IntermediateCode.hpp"
using namespace std;

struct DAG_node {
	bool is_leaf;			 // �ýڵ��Ƿ���Ҷ�ӽڵ�
	string val;				 // �ýڵ��ֵ��Ҷ�ӽڵ㣨�������ʶ�������м�ڵ㣨�������
	vector<string> identify; // �ýڵ�������ı���
	vector<DAG_node*> child;	 // �ýڵ�ĺ��ӽڵ㣨��˳��ִ�в�����
};
class DAG
{
public:
	DAG() {};
	~DAG() {};

private:
	vector<DAG_node*> graph;
	DAG_node* head;
};

struct VarInfomation {
	int next;		// ������Ϣ
	bool active;	// ��Ծ��Ϣ

	VarInfomation(int next, bool active) : next(next), active(active) {};
	VarInfomation(const VarInfomation& other) { next = other.next; active = other.active; };
	VarInfomation() { next = -1; active = false; };
};

struct QuaternaryWithInfo {
public:
	string op;
	string arg1;
	string arg2;
	string des;
	VarInfomation info1;	// arg1��
	VarInfomation info2;	// arg2��
	VarInfomation info3;	// des��

	QuaternaryWithInfo(Quaternary q, VarInfomation info1, VarInfomation info2, VarInfomation info3) {
		op = q.op;
		arg1 = q.arg1;
		arg2 = q.arg2;
		des = q.des;
		this->info1 = info1;
		this->info2 = info2;
		this->info3 = info3;
	}
	QuaternaryWithInfo(Quaternary q) {
		op = q.op;
		arg1 = q.arg1;
		arg2 = q.arg2;
		des = q.des;
		info1 = VarInfomation(-1, false);
		info2 = VarInfomation(-1, false);
		info3 = VarInfomation(-1, false);
	}
};
ostream& operator<< (ostream& out, const VarInfomation& info) {
	out << "(";
	if (info.next == -1) {
		out << "^" << ", ";
	}
	else {
		out << info.next << ", ";
	}
	if (info.active) {
		out << "y)";
	}
	else {
		out << "^)";
	}
	return out;
}
ostream& operator<< (ostream& out, const QuaternaryWithInfo& quat) {
	out << "(" << quat.op << ", " << quat.arg1 << ", " << quat.arg2 << ", " << quat.des << ")";
	out << "\t" << quat.info1 << " " << quat.info2 << " " << quat.info3;
	return out;
}


class BaseBlock {
public:
	BaseBlock() { blockID = 0; firstNum = 0; next = NULL; jump = NULL; is_func = false; name = ""; }
	~BaseBlock() {}
	void calculateOptimize(){};

	vector<QuaternaryWithInfo>& getQuaternarys() { return this->code; }
	int getBlockID() { return blockID; }
	int getFirstNum() { return firstNum; }
	BaseBlock* getNext() { return next; }
	BaseBlock* getJump() { return jump; }
	bool getIsFunc() { return is_func; }
	string getName() { return name; }

	void setBlockID(int id) { blockID = id; }
	void setFirstNum(int num) { firstNum = num; }
	void setNext(BaseBlock* next) { this->next = next; }
	void setJump(BaseBlock* jump) { this->jump = jump; }
	void setIsFunc(bool is_func) { this->is_func = is_func; }
	void setName(string name) { this->name = name; }

	void addCode(QuaternaryWithInfo one) { code.push_back(one); }

private:
	vector<QuaternaryWithInfo> code;	// ��¼�������е��м����
	int blockID;
	int firstNum;
	bool is_func;
	string name;
	BaseBlock* next;	// ��ͼ��ʾ���û��������ת�ƵĶ���
	BaseBlock* jump;	// ��ͼ��ʾ���û��������ת�ƵĶ���
};
