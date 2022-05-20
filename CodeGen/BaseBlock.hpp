#pragma once

#include<string>
#include<vector>
#include"IntermediateCode.hpp"
using namespace std;

struct DAG_node {
	bool is_leaf;			 // 该节点是否是叶子节点
	string val;				 // 该节点的值：叶子节点（常量或标识符）、中间节点（运算符）
	vector<string> identify; // 该节点所代表的变量
	vector<DAG_node*> child;	 // 该节点的孩子节点（按顺序，执行操作）
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
	int next;		// 待用信息
	bool active;	// 活跃信息

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
	VarInfomation info1;	// arg1的
	VarInfomation info2;	// arg2的
	VarInfomation info3;	// des的

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
	vector<QuaternaryWithInfo> code;	// 记录基本块中的中间代码
	int blockID;
	int firstNum;
	bool is_func;
	string name;
	BaseBlock* next;	// 流图表示（该基本块可能转移的对象）
	BaseBlock* jump;	// 流图表示（该基本块可能转移的对象）
};
