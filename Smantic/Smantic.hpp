#pragma once
#include"SYM.hpp"
#include"IntermediateCode.hpp"
#include<string.h>
#include<vector>

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
	string get_name() { return name; }
	int get_type() { return type; }
	vector<int> get_param() { return param; }
	int get_enterpoint() { return enterpoint; }
};
//ʵ���б�
struct AP_list {
	string name;	// ʵ������
	int type;		// ʵ����������
	AP_list* next;
};

class Smantic {
public:
	IntermediateCode INTcode;			//�м����
	vector<funcEntry> funcTable;		//������
	vector<vector<tblEntry>> varTable;	//���ű�

	int tmp_num;		//��ʱ�����ĸ�����newtemp()������
	bool return_flag;	//��ǰ�����Ƿ��з���ֵ�� void=false, other=true
	int return_line;	//return�����У�������
	//��ǰwhichTable��varTableʹ�ã�С�ı���
	int whichTable;		//��ǰ����һ�����ű�ҳ��ָ��

	AP_list* ap_list;	//���̵���ʵ���б�������ƺ�����

	Smantic()
	{
		tmp_num = 0;
		return_flag = false;
		return_line = 0;
		whichTable = 0;
		mkVarTable();
		ap_list = NULL;
	}

	//�½�һ������
	string newtemp();
	//����һ������i������������i����Ԫʽ�����һ���±�(���)����������ָ���������ָ��
	SemanticListNode* makelist(int i);
	//�ϲ���������
	SemanticListNode* merge(SemanticListNode* p, SemanticListNode* q);
	//һ��ɨ��������������
	SYM Smantic_analyze(string s, int left_id, vector<SYM> node);
	//���ҳ�������س����������ڵ���
	funcEntry* lookupFunc(string s);
	//���ҷ��ű����ر������������У����ҷ�Χ����varTable[whichTable]��
	tblEntry* lookupVar(string s, int k);
	//������ű�varTable
	void enterVar(int whichTable, tblEntry* v);
	//����һ��ҳ��varTable
	void mkVarTable();
	//ɾ��һ�������еı�
	void delVar();
	//���ʵ���б���β��б������
	bool match(AP_list* p, vector<int> q);
	//����ʵ���б�
	void destroyAPlist(AP_list* p);
	//��ӡ���ű�
	void printVarTable();
	//��ӡ�����
	void printFuncTable();
};

string Smantic::newtemp()
{
	return "T" + std::to_string(++tmp_num);
}
SemanticListNode* Smantic::makelist(int i)
{
	SemanticListNode* p = new SemanticListNode;
	p->pos = i;
	p->next = NULL;
	return p;
}
SemanticListNode* Smantic::merge(SemanticListNode* p, SemanticListNode* q)
{
	if (p == NULL)
		return q;

	SemanticListNode* head = p;
	while (p->next)
		p = p->next;

	p->next = q;
	return head;
}
void Smantic::mkVarTable()
{
	vector<tblEntry> tmp;
	varTable.push_back(tmp);
}
funcEntry* Smantic::lookupFunc(string s)
{
	for (vector<funcEntry>::iterator it = funcTable.begin(); it != funcTable.end(); it++) {
		if (it->get_name() == s)
			return &(*it);
	}
	return NULL;
}
tblEntry* Smantic::lookupVar(string s, int k)
{
	if (varTable.size() == 0)//����ж�������Ҫ��˼��һ��
		return NULL;
	for (int i = 0; i <= whichTable; i++) {
		if (varTable[i].size() != 0) {
			for (vector<tblEntry>::iterator it = varTable[i].begin();
				it != varTable[i].end(); it++) {
				if (it->get_name() == s && it->get_kind() == k)
					return &(*it);
			}
		}
	}

	return NULL;
}
void Smantic::enterVar(int whichTable, tblEntry* v)
{
	varTable[whichTable].push_back(*v);
}
void Smantic::printFuncTable()
{
	cout << "���������" << endl;
	for (int i = 0; i < funcTable.size(); i++) {
		cout << "name: " << funcTable[i].get_name();
		cout << " type: " << funcTable[i].get_type();
		cout << " enterpoint: " << funcTable[i].get_enterpoint();
		cout << endl;
	}
	cout << "���������" << endl;
}
void Smantic::printVarTable()
{
	cout << "������ű�" << endl;
	for (int i = 0; i < varTable.size(); i++)
	{
		for (int j = 0; j < varTable[i].size(); j++)
		{
			cout << "i:" << i << " j:" << j << " ";
			cout << "name: " << varTable[i][j].get_name() << "\tkind: " << varTable[i][j].get_kind();
			cout << "\ttype: " << varTable[i][j].get_typ();
			cout << endl;
		}
	}
	cout << "���ű����" << endl;
}
void Smantic::delVar()
{
	if (whichTable <= 0)
		return;
	vector<tblEntry>().swap(varTable[whichTable]);
	varTable.pop_back();
	whichTable--;
}
bool Smantic::match(AP_list* p, vector<int> q) {
	if (p == NULL)
		return false;

	for (vector<int>::iterator itq = q.begin(); itq != q.end(); itq++) {
		if (p == NULL)//����������ƥ��, ָ��������ж�
			return false;
		else if (*itq != p->type)//���Ͳ�ͬ
			return false;
		p = p->next;
	}
	if (p != NULL)
		return false;
	return true;
}
void Smantic::destroyAPlist(AP_list* head)
{
	if (head == NULL)
		return;
	AP_list* p;
	while (head) {
		p = head->next;
		free(head);
		head = p;
	}
	return;
}
/*
	@param: string s ����ʽ
	@param: int left_id ����ʽ�󲿷��ŵ�id��
	@param: vector<SYM> node ����ʽ�Ҳ����й�Լ�ķ��� node˳���෴
	return: left_node ����ʽ�󲿵ķ��ţ��̳�һЩ����
*/
SYM Smantic::Smantic_analyze(string s, int left_id, vector<SYM> node)
{
	SYM left_node;
	if (s._Equal("<N> -> @")) {
		left_node = SYM(left_id, "", 0);
		left_node.set_nextlist(makelist(INTcode.nextquad()));
		INTcode.emit("j", "-", "-", "-1");
	}
	else if (s._Equal("<M> -> @")) {
		left_node = SYM(left_id, "", 0);
		left_node.set_quad(INTcode.nextquad());
	}
	else if (s._Equal("<A> -> @")) {
		left_node = SYM(left_id, "", 0);
		mkVarTable();	//������뽨������
		whichTable++;	//mkVarTable�в���ά��whichTableָ��
	}
	else if (s._Equal("<start> -> <program>")) { //��ʼ Program
		left_node = SYM(left_id, "", 0);
	}
	else if (s._Equal("<program> -> <N> <declare_list>")) {//program N ������
		left_node = SYM(left_id, "", 0);
		//ע��N��û�д���
		//��������main����������������ڴ˴�����
		funcEntry* f = lookupFunc("main");
		if (f) {
			INTcode.back_patch(node[1].get_nextlist(), f->get_enterpoint());
		}
	}
	else if (s._Equal("<declare_list> -> <declare>")) { //������ ����
		left_node = SYM(left_id, "", -1);
	}
	else if (s._Equal("<declare_list> -> <declare> <declare_list>")) {//������ ���� ������
		left_node = SYM(left_id, "", 0);
	}
	/* �����˿ղ���ʽ */

	else if (s._Equal("<declare> -> int ID <M> <A> <func_declare>")) {//���� int ��ʶ�� M A ��������
		/* ����int1*/
		SYM func = node[3];
		SYM M = node[2];
		//1. ��ѯ�����ض���
		funcEntry* f = lookupFunc(func.get_name());
		if (f) {
			cout << "��" << func.get_line() << "�� " << func.get_name() << " �����ض���" << endl;
			left_node = SYM(left_id, "error", -1);
			return left_node;
		}
		//2. �������ڷ���ֵ�����return_flag
		if (!return_flag) {
			cout << "��" << return_line << "�� " << func.get_name() << " ���� �з���ֵ" << endl;
		}
		//3. ���������
		f = new funcEntry(func.get_name(), T_INT, node[0].get_plist(), M.get_quad());
		funcTable.push_back(*f);

		delVar();

		left_node = SYM(left_id, "", 0);
	}
	else if (s._Equal("<declare> -> int ID <var_declare>")) {//���� int ��ʶ�� ��������
		/* ȫ�ֱ���4 ����int1*/
		SYM var = node[1];
		//1. ��ѯȫ�ֱ����ض���
		tblEntry* e = lookupVar(var.get_name(), GLOBAL_V);
		if (e) {
			cout << "��" << var.get_line() << "�� " << var.get_name() << " �����ض���" << endl;
			left_node = SYM(left_id, "error", -1);
			return left_node;
		}
		//2. ���ű����
		e = new tblEntry(var.get_name(), GLOBAL_V, T_INT);
		enterVar(0, e);	//whichTable=0ȫ�ֱ���

		left_node = SYM(left_id, "", 0);
	}
	else if (s._Equal("<declare> -> void ID <M> <A> <func_declare>")) {//���� void ��ʶ�� M A ��������
		/* ����void2*/
		SYM func = node[3];
		SYM M = node[2];
		//1. ��ѯ�����ض���
		funcEntry* f = lookupFunc(func.get_name());
		if (f) {
			cout << "��" << func.get_line() << "�� " << func.get_name() << " �����ض���" << endl;
			left_node = SYM(left_id, "error", -1);
			return left_node;
		}
		//2. �����޷���ֵ�����return_flag
		if (return_flag) {
			cout << "��" << return_line << "�� " << func.get_name() << " ���� �޷���ֵ" << endl;
		}
		//3. ���������
		f = new funcEntry(func.get_name(), T_VOID, node[0].get_plist(), M.get_quad());
		funcTable.push_back(*f);

		delVar();

		left_node = SYM(left_id, "", 0);

	}
	else if (s._Equal("<var_declare> -> ;")) {//�������� ��
		left_node = SYM(left_id, "", node[0].get_line());
		//����ȱ�ٷֺţ��Ƿ���Ա���
	}
	else if (s._Equal("<func_declare> -> ( <fparam> ) <sent_block>")) {//�������� (�β�) ����
		left_node = SYM(left_id, "", 0);
		left_node.set_plist(node[2].get_plist());
	}
	else if (s._Equal("<fparam> -> <param_list>")) {//�β� �����б�
		left_node = SYM(left_id, "", 0);
		left_node.set_plist(node[0].get_plist());
	}
	else if (s._Equal("<fparam> -> void")) {//�β� void
		left_node = SYM(left_id, "", 0);
		//�β�Ϊ��
	}
	else if (s._Equal("<param_list> -> <param>")) {//�����б� ����
		left_node = SYM(left_id, "", 0);
		left_node.set_plist(node[0].get_plist());
	}
	else if (s._Equal("<param_list> -> <param> , <param_list>")) {//�����б� ���� , �����б�
		left_node = SYM(left_id, "", 0);
		vector<int> temp = node[2].get_plist();		//param
		vector<int> Hplist = node[0].get_plist();	//param_list
		Hplist.insert(Hplist.end(), temp.begin(), temp.end());	//"�����б�"��ѹ��"����"

		left_node.set_plist(Hplist);
	}
	else if (s._Equal("<param> -> int ID")) {//���� int ��ʶ��
		/*�β�5, int1*/
		SYM fparam = node[0];
		/*1. ��ѯ�β��Ƿ��ظ�*/
		tblEntry* e = lookupVar(fparam.get_name(), FORMAL_PARA);
		if (e) {
			cout << "��" << fparam.get_line() << "�� " << fparam.get_name() << " �����ض���" << endl;
			left_node = SYM(left_id, "error", 0);
			return left_node;
		}
		/*2. ������ű�*/
		e = new tblEntry(fparam.get_name(), FORMAL_PARA, T_INT);
		e->set_normal(false);
		enterVar(whichTable, e);
		/*3. �м��������*/
		//INTcode.emit("get", "-", "-", fparam.get_name());

		left_node = SYM(left_id, "", 0);
		vector<int>plist = { T_INT };//�β��б���¼�β����ͣ�int1
		left_node.set_plist(plist);
	}
	else if (s._Equal("<sent_block> -> { <inner_declare> <sent_list> }")) {//���� '{'�ڲ����� ��䴮'}'
		/*ɾ��, whichTable�����ڲ�-1*/
		//delVar();
		left_node = SYM(left_id, "", 0);
		left_node.set_nextlist(node[1].get_nextlist());
	}
	else if (s._Equal("<sent_block> -> { <sent_list> }")) {//���� '{'��䴮'}'
		//delVar();
		left_node = SYM(left_id, "", 0);
		left_node.set_nextlist(node[1].get_nextlist());
	}
	else if (s._Equal("<inner_declare> -> <inner_var_dec>")) {//�ڲ����� �ڲ���������
		left_node = SYM(left_id, node[0].get_name(), 0);
	}
	else if (s._Equal("<inner_declare> -> <inner_var_dec> <inner_declare>")) {//�ڲ����� �ڲ��������� �ڲ�����
		left_node = SYM(left_id, node[0].get_name(), 0);
	}
	else if (s._Equal("<inner_var_dec> -> int ID ;")) {//�ڲ��������� int ��ʶ�� ;
		/*�ڲ�����2�� int1*/
		SYM var = node[1];
		/*1. ���*/
		tblEntry* e = lookupVar(var.get_name(), VARIABLE);
		if (e) {
			cout << "��" << var.get_line() << "�� " << var.get_name() << " �����ض���" << endl;
			left_node = SYM(left_id, "error", -1);
			return left_node;
		}
		/*2. ������ű�*/
		e = new tblEntry(var.get_name(), VARIABLE, T_INT);
		enterVar(whichTable, e);

		left_node = SYM(left_id, var.get_name(), 0);
	}
	else if (s._Equal("<sent_list> -> <sent> <M> <sent_list>")) {//��䴮 ��� M ��䴮
		/* �����˿ղ���ʽ */
		SYM M = node[1];
		/*����*/
		INTcode.back_patch(node[2].get_nextlist(), M.get_quad());
		left_node = SYM(left_id, "", 0);
		left_node.set_nextlist(node[0].get_nextlist());
	}
	else if (s._Equal("<sent_list> -> <sent>")) {//��䴮 ��� 
		left_node = SYM(left_id, "", 0);
		left_node.set_nextlist(node[0].get_nextlist());
	}
	else if (s._Equal("<sent> -> <if_sent>")) {//��� if���
		left_node = SYM(left_id, "", 0);
		left_node.set_nextlist(node[0].get_nextlist());
	}
	else if (s._Equal("<sent> -> <while_sent>")) {//��� while���
		left_node = SYM(left_id, "", 0);
		left_node.set_nextlist(node[0].get_nextlist());
	}
	else if (s._Equal("<sent> -> <return_sent>")) {//��� return���
		left_node = SYM(left_id, "", 0);
		left_node.set_nextlist(node[0].get_nextlist());
	}
	else if (s._Equal("<sent> -> <assign_sent>")) {//��� ��ֵ���
		left_node = SYM(left_id, "", 0);
		/* Ψ��ֵ��䲻����nextlist */
	}
	else if (s._Equal("<if_sent> -> if ( <expression> ) <M> <A> <sent_block>")) {//if��� if ( <���ʽ> ��M A ����
		//����
		INTcode.back_patch(node[4].get_truelist(), node[2].get_quad());
		//�ϲ�����
		SemanticListNode* p = node[0].get_nextlist();
		SemanticListNode* q = node[4].get_falselist();
		left_node = SYM(left_id, "", 0);
		left_node.set_nextlist(merge(p, q));
		//ɾ��
		delVar();
	}
	else if (s._Equal("<if_sent> -> if ( <expression> ) <M> <A> <sent_block> <N> else <M> <A> <sent_block>")) {
		//����
		//expression->falselist, M2.quad
		INTcode.back_patch(node[9].get_falselist(), node[2].get_quad());
		//expression->truelist, M1.quad
		INTcode.back_patch(node[9].get_truelist(), node[7].get_quad());
		SemanticListNode* N = node[4].get_nextlist();
		SemanticListNode* S2 = node[0].get_nextlist();
		SemanticListNode* S1 = node[5].get_nextlist();
		left_node = SYM(left_id, "", 0);
		left_node.set_nextlist(merge(S1, merge(N, S2)));
		//ɾ������
		delVar();
		delVar();
	}
	else if (s._Equal("<while_sent> -> while <M> ( <expression> ) <M> <A> <sent_block>")) {
		//����
		//sent_block->nextlist, M1.quad
		INTcode.back_patch(node[0].get_nextlist(), node[6].get_quad());
		//expression->truelist, M2.quad
		INTcode.back_patch(node[4].get_truelist(), node[2].get_quad());

		left_node = SYM(left_id, "", 0);
		left_node.set_nextlist(node[4].get_falselist());
		//�м��������
		INTcode.emit("j", "-", "- ", to_string(node[6].get_quad()));
		//ɾ��
		delVar();
	}
	else if (s._Equal("<return_sent> -> return ;")) {//return��� return ;
		left_node = SYM(left_id, "", 0);
		/* �м�������� */
		INTcode.emit("return", "-", "-", "-");
		/* ��ǰ�����޷���ֵ */
		return_flag = false;
		return_line = node[1].get_line();
	}
	else if (s._Equal("<return_sent> -> return <expression> ;")) {//return��� return ���ʽ ;
		left_node = SYM(left_id, "", 0);
		/* �м�������� */
		INTcode.emit("return", node[1].get_name(), "-", "-");
		/* ��ǰ�������ڷ���ֵ */
		return_flag = true;
		return_line = node[2].get_line();
	}
	else if (s._Equal("<assign_sent> -> ID = <expression> ;")) {//��ֵ��� ID = ���ʽ ��
		/*���*/
		SYM var = node[3];
		SYM exp = node[1];
		//��ʶ��������ȫ�ֱ�����Ҳ�������β�
		tblEntry* v = lookupVar(var.get_name(), GLOBAL_V);//ȫ�ֱ���4
		if (!v) {
			v = lookupVar(var.get_name(), VARIABLE); // �ֲ�����2
			if (!v) {
				v = lookupVar(var.get_name(), FORMAL_PARA);//�β�5
				if (!v) {
					cout << "��" << var.get_line() << "�� " << var.get_name() << " ����δ����" << endl;
					left_node = SYM(left_id, "error", -1);
					return left_node;
				}
			}
		}
		/*��������Ϊvoid����ʶ��Ϊint*/
		funcEntry* f = lookupFunc(exp.get_name());
		if (f) {
			if (v->get_typ() != f->get_type()) {
				cout << "��" << var.get_line() << "�� "
					<< "����" << f->get_name() << " ���������" << v->get_name() << "��ƥ��" << endl;
				left_node = SYM(left_id, "error", -1);
				return left_node;
			}
		}

		INTcode.emit("=", exp.get_name(), "-", var.get_name());
		left_node = SYM(left_id, "", 0);
	}
	else if (s._Equal("<expression> -> <add_expression>")) {
		SYM addexp = node[0];
		//�ӷ����ʽ������ű�
		tblEntry* e = new tblEntry(addexp.get_name(), VARIABLE, addexp.get_type());
		enterVar(whichTable, e);

		left_node = SYM(left_id, addexp.get_name(), 0, addexp.get_type());
	}
	else if (s._Equal("<expression> -> <add_expression> <relop> <expression>")) {
		//�����µı�����
		string nu = newtemp();
		//���ʽ������ű�
		tblEntry* e = new tblEntry(nu, VARIABLE, T_INT); //relop��������0��1��������ǳ������Ǳ���
		enterVar(whichTable, e);

		left_node = SYM(left_id, nu, 0, node[2].get_type());
		//�趨���ʽ��truelist��falselist
		SemanticListNode* p = new SemanticListNode;
		p->pos = INTcode.nextquad();
		p->next = NULL;
		left_node.set_truelist(p);
		SemanticListNode* q = new SemanticListNode;
		q->pos = INTcode.nextquad() + 1;
		q->next = NULL;
		left_node.set_falselist(q);
		//�м��������
		INTcode.emit("j" + node[1].get_name(),
			node[2].get_name(),
			node[0].get_name(),
			"0");
		INTcode.emit("j", "-", "-", "0");
	}
	else if (s._Equal("<add_expression> -> <item>")) {//�ӷ����ʽ ��
		left_node = SYM(left_id, node[0].get_name(), 0, node[0].get_type());
	}
	else if (s._Equal("<add_expression> -> <item> + <add_expression>")) {//�ӷ����ʽ �� + �ӷ����ʽ
		string nv = newtemp();
		INTcode.emit(node[1].get_name(), node[2].get_name(), node[0].get_name(), nv);
		left_node = SYM(left_id, nv, 0, node[2].get_type());
	}
	else if (s._Equal("<add_expression> -> <item> - <add_expression>")) {//�ӷ����ʽ �� - �ӷ����ʽ
		string nv = newtemp();
		INTcode.emit(node[1].get_name(), node[2].get_name(), node[0].get_name(), nv);
		left_node = SYM(left_id, nv, 0, node[2].get_type());
	}
	else if (s._Equal("<item> -> <factor>")) {//�� ����
		left_node = SYM(left_id, node[0].get_name(), 0, node[0].get_type());
	}
	else if (s._Equal("<item> -> <factor> * <item>")) {//�� ���� * ��
		string nv = newtemp();
		INTcode.emit(node[1].get_name(), node[2].get_name(), node[0].get_name(), nv);
		left_node = SYM(left_id, nv, 0, node[2].get_type());
	}
	else if (s._Equal("<item> -> <factor> / <item>")) {//�� ���� / ��
		string nv = newtemp();
		INTcode.emit(node[1].get_name(), node[2].get_name(), node[0].get_name(), nv);
		left_node = SYM(left_id, nv, 0, node[2].get_type());
	}
	else if (s._Equal("<factor> -> NUM")) {//���� ����
		left_node = SYM(left_id, node[0].get_name(), 0, T_INT);	//���� int1
	}
	else if (s._Equal("<factor> -> ID")) {//���� ��ʶ��
		/*���*/
		SYM var = node[0];
		//��ʶ��������ȫ�ֱ�����Ҳ�������β�
		tblEntry* v = lookupVar(var.get_name(), GLOBAL_V);//ȫ�ֱ���4
		if (!v) {
			v = lookupVar(var.get_name(), VARIABLE); // �ֲ�����2
			if (!v) {
				v = lookupVar(var.get_name(), FORMAL_PARA);//�β�5
				if (!v) {
					cout << "��" << var.get_line() << "�� " << var.get_name() << " ����δ����" << endl;
					left_node = SYM(left_id, "error", -1);
					return left_node;
				}
			}
		}
		left_node = SYM(left_id, var.get_name(), 0, v->get_typ());	//��ʶ���ڷ��ű��м�¼������
	}
	else if (s._Equal("<factor> -> ( <expression> )")) {//���� (���ʽ)
		left_node = SYM(left_id, node[1].get_name(), 0, node[1].get_type());

	}
	else if (s._Equal("<factor> -> ID ( <aparam_list> )")) {//���� ��ʶ��(ʵ���б�)
		/*���*/
		SYM g = node[3];
		funcEntry* f = lookupFunc(g.get_name());
		if (!f)
		{
			cout << "��" << g.get_line() << "�� " << "���õĺ��� " << g.get_name() << " δ����" << endl;
			left_node = SYM(left_id, "error", -1);
			return left_node;
		}
		if (!match(ap_list, f->get_param()))	//�˴�Ӧ�ö�ʵ���б���β��б���бȽ�
		{
			cout << "��" << g.get_line() << "�� ����" << g.get_name() << "�Ĳ������ͻ������ƥ��" << endl;
			left_node = SYM('X', "error", -1);
			return left_node;
		}
		//�м��������
		AP_list* head = ap_list;
		while (head) {
			INTcode.emit("par", head->name, "-", "-");
			head = head->next;
		}
		//����ʵ���б�
		head = ap_list;
		destroyAPlist(head);
		//�м��������
		INTcode.emit("call", g.get_name(), "-", to_string(f->get_enterpoint()));
		left_node = SYM(left_id, g.get_name(), 0, f->get_type());	//������������
	}
	else if (s._Equal("<aparam_list> -> @")) {//ʵ���б� ��
		left_node = SYM(left_id, "", 0);
	}
	else if (s._Equal("<aparam_list> -> <expression>")) {//ʵ���б� ���ʽ
		ap_list = new AP_list;
		ap_list->name = node[0].get_name();
		ap_list->type = node[0].get_type();
		ap_list->next = NULL;
		left_node = SYM(left_id, node[0].get_name(), 0);
	}
	else if (s._Equal("<aparam_list> -> <expression> , <aparam_list>")) {//ʵ���б� ���ʽ �ָ��� ʵ���б�
		AP_list* tmp = new AP_list;
		tmp->name = node[2].get_name();
		tmp->type = node[2].get_type();
		tmp->next = NULL;

		//��ʵ�ν���ʵ���б�ĩβ
		AP_list* head = ap_list;
		while (head->next != NULL)
			head = head->next;
		head->next = tmp;

		left_node = SYM(left_id, node[0].get_name(), 0);
	}
	else if (s._Equal("<relop> -> <")) {//relop > < == >= <= !=
		left_node = SYM(left_id, node[0].get_name(), node[0].get_line());
	}
	else if (s._Equal("<relop> -> >")) {//relop > < == >= <= !=
		left_node = SYM(left_id, node[0].get_name(), node[0].get_line());
	}
	else if (s._Equal("<relop> -> ==")) {//relop > < == >= <= !=
		left_node = SYM(left_id, node[0].get_name(), node[0].get_line());
	}
	else if (s._Equal("<relop> -> <=")) {//relop > < == >= <= !=
		left_node = SYM(left_id, node[0].get_name(), node[0].get_line());
	}
	else if (s._Equal("<relop> -> >=")) {//relop > < == >= <= !=
		left_node = SYM(left_id, node[0].get_name(), node[0].get_line());
	}
	else if (s._Equal("<relop> -> !=")) {//relop > < == >= <= !=
		left_node = SYM(left_id, node[0].get_name(), node[0].get_line());
	}
	return left_node;
}