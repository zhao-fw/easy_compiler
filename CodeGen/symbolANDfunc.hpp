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
	CONSTANT,		//����
	VARIABLE,		//����
	FUNCTION,		//����
	GLOBAL_V,		//ȫ�ֱ���
	FORMAL_PARA		//�β�
};

//���ֱ�
class tblEntry {
private:
	string name;	//��������
	int place;		//�ڱ��е�λ��
	int kind;		//����KIND ����1������2������3��ȫ�ֱ���4���β�5����ʼֵΪ0
	int typ;		//��������TYPE void 2,int 1����ʼֵΪ0
	bool normal;	//���������Ƿ�Ϊ�����β��������������β�����false������true��Ĭ��Ϊʵ�Σ���ʼֵΪ0
	int val;		//������Ϊ������ʱ��������Ӧֵ
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
//�����
class funcEntry {
private:
	string name;	// ��������
	int type;		// ��������ֵ���� int1 void2
	vector<int> param;	//�β��б�
	int enterpoint;	// ������ʼλ��
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
