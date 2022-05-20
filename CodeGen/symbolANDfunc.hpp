#pragma once

#include<string>
#include<vector>
using namespace std;

enum TYPE {
	T_UNDEFINED = 0,
	T_INT,
	T_VOID
};

enum KIND {
	K_UNDEFINED = 0,
	CONSTANT,		//常量
	VARIABLE,		//变量
	FUNCTION,		//函数
	GLOBAL_V,		//全局变量
	FORMAL_PARA		//形参
};

//名字表
class tblEntry {
private:
	string name;	//符号名称
	int place;		//在表中的位置
	int kind;		//种类KIND 常量1、变量2、函数3，全局变量4，形参5，初始值为0
	int typ;		//数据类型TYPE void 2,int 1，初始值为0
	bool normal;	//标明名字是否为变量形参名，当名字是形参则填false，否则true，默认为实参，初始值为0
	int val;		//当名字为变量名时，填入相应值
public:
	tblEntry(const tblEntry& other) {
		name = other.name;
		place = other.place;
		kind = other.kind;
		typ = other.typ;
		normal = other.normal;
		val = other.val;
	}
	tblEntry(string n, int k)
	{
		normal = true;
		name = n;
		kind = k;
		val = -99;
	}
	tblEntry(string n, int k, int t)
	{
		normal = true;
		name = n;
		kind = k;
		typ = t;
		val = -99;
	}
	void set_place(int p) { place = p; }
	int get_place() { return place; }
	string get_name() { return name; }
	void set_name(string s) { name = s; }
	void set_kind(int k) { kind = k; }
	int get_kind() { return kind; }

	int get_typ() { return typ; }
	void set_typ(int t) { typ = t; }
	bool get_normal() { return normal; }
	void set_normal(bool n) { normal = n; }
	int get_val() { return val; }
	void set_val(int v) { val = v; }
};
//程序表
class funcEntry {
private:
	string name;	// 函数名称
	int type;		// 函数返回值类型 int1 void2
	vector<int> param;	//形参列表
	int enterpoint;	// 函数起始位置
public:
	funcEntry(string s, int t, vector<int> p, int point)
	{
		name = s;
		type = t;
		param = p;
		enterpoint = point;
	}
	funcEntry(const funcEntry& other)
	{
		name = other.name;
		type = other.type;
		param = other.param;
		enterpoint = other.enterpoint;
	}
	string get_name() { return name; }
	int get_type() { return type; }
	vector<int> get_param() { return param; }
	int get_enterpoint() { return enterpoint; }
};
