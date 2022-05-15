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
	string get_name() { return name; }
	int get_type() { return type; }
	vector<int> get_param() { return param; }
	int get_enterpoint() { return enterpoint; }
};
//实参列表
struct AP_list {
	string name;	// 实参名称
	int type;		// 实参数据类型
	AP_list* next;
};

class Smantic {
public:
	IntermediateCode INTcode;			//中间代码
	vector<funcEntry> funcTable;		//函数表
	vector<vector<tblEntry>> varTable;	//符号表

	int tmp_num;		//临时变量的个数，newtemp()计数君
	bool return_flag;	//当前函数是否有返回值， void=false, other=true
	int return_line;	//return语句的行，报错用
	//当前whichTable被varTable使用，小心变量
	int whichTable;		//当前是哪一个符号表，页表指针

	AP_list* ap_list;	//过程调用实参列表，存放名称和类型

	Smantic()
	{
		tmp_num = 0;
		return_flag = false;
		return_line = 0;
		whichTable = 0;
		mkVarTable();
		ap_list = NULL;
	}

	//新建一个变量
	string newtemp();
	//创建一个仅含i的新链表，其中i是四元式数组的一个下标(标号)；函数返回指向这个链的指针
	SemanticListNode* makelist(int i);
	//合并两个链表
	SemanticListNode* merge(SemanticListNode* p, SemanticListNode* q);
	//一遍扫描的语义分析方法
	SYM Smantic_analyze(string s, int left_id, vector<SYM> node);
	//查找程序表，返回程序名称所在的行
	funcEntry* lookupFunc(string s);
	//查找符号表，返回变量名称所在行，查找范围仅在varTable[whichTable]中
	tblEntry* lookupVar(string s, int k);
	//插入符号表varTable
	void enterVar(int whichTable, tblEntry* v);
	//创建一个页表varTable
	void mkVarTable();
	//删除一组语句块中的表
	void delVar();
	//检查实参列表和形参列表的属性
	bool match(AP_list* p, vector<int> q);
	//销毁实参列表
	void destroyAPlist(AP_list* p);
	//打印符号表
	void printVarTable();
	//打印程序表
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
	if (varTable.size() == 0)//这个判断条件还要再思考一下
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
	cout << "输出函数表" << endl;
	for (int i = 0; i < funcTable.size(); i++) {
		cout << "name: " << funcTable[i].get_name();
		cout << " type: " << funcTable[i].get_type();
		cout << " enterpoint: " << funcTable[i].get_enterpoint();
		cout << endl;
	}
	cout << "函数表结束" << endl;
}
void Smantic::printVarTable()
{
	cout << "输出符号表" << endl;
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
	cout << "符号表结束" << endl;
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
		if (p == NULL)//参数个数不匹配, 指针必须先判断
			return false;
		else if (*itq != p->type)//类型不同
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
	@param: string s 产生式
	@param: int left_id 产生式左部符号的id号
	@param: vector<SYM> node 产生式右部所有规约的符号 node顺序相反
	return: left_node 产生式左部的符号，继承一些属性
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
		mkVarTable();	//这里必须建表！！！
		whichTable++;	//mkVarTable中不会维护whichTable指针
	}
	else if (s._Equal("<start> -> <program>")) { //开始 Program
		left_node = SYM(left_id, "", 0);
	}
	else if (s._Equal("<program> -> <N> <declare_list>")) {//program N 声明串
		left_node = SYM(left_id, "", 0);
		//注意N还没有处理
		//假如是有main函数的情况，可以在此处回填
		funcEntry* f = lookupFunc("main");
		if (f) {
			INTcode.back_patch(node[1].get_nextlist(), f->get_enterpoint());
		}
	}
	else if (s._Equal("<declare_list> -> <declare>")) { //声明串 声明
		left_node = SYM(left_id, "", -1);
	}
	else if (s._Equal("<declare_list> -> <declare> <declare_list>")) {//声明串 声明 声明串
		left_node = SYM(left_id, "", 0);
	}
	/* 别忘了空产生式 */

	else if (s._Equal("<declare> -> int ID <M> <A> <func_declare>")) {//声明 int 标识符 M A 函数声明
		/* 类型int1*/
		SYM func = node[3];
		SYM M = node[2];
		//1. 查询函数重定义
		funcEntry* f = lookupFunc(func.get_name());
		if (f) {
			cout << "第" << func.get_line() << "行 " << func.get_name() << " 函数重定义" << endl;
			left_node = SYM(left_id, "error", -1);
			return left_node;
		}
		//2. 函数存在返回值，检查return_flag
		if (!return_flag) {
			cout << "第" << return_line << "行 " << func.get_name() << " 函数 有返回值" << endl;
		}
		//3. 函数表插入
		f = new funcEntry(func.get_name(), T_INT, node[0].get_plist(), M.get_quad());
		funcTable.push_back(*f);

		delVar();

		left_node = SYM(left_id, "", 0);
	}
	else if (s._Equal("<declare> -> int ID <var_declare>")) {//声明 int 标识符 变量声明
		/* 全局变量4 类型int1*/
		SYM var = node[1];
		//1. 查询全局变量重定义
		tblEntry* e = lookupVar(var.get_name(), GLOBAL_V);
		if (e) {
			cout << "第" << var.get_line() << "行 " << var.get_name() << " 变量重定义" << endl;
			left_node = SYM(left_id, "error", -1);
			return left_node;
		}
		//2. 符号表插入
		e = new tblEntry(var.get_name(), GLOBAL_V, T_INT);
		enterVar(0, e);	//whichTable=0全局变量

		left_node = SYM(left_id, "", 0);
	}
	else if (s._Equal("<declare> -> void ID <M> <A> <func_declare>")) {//声明 void 标识符 M A 函数声明
		/* 类型void2*/
		SYM func = node[3];
		SYM M = node[2];
		//1. 查询函数重定义
		funcEntry* f = lookupFunc(func.get_name());
		if (f) {
			cout << "第" << func.get_line() << "行 " << func.get_name() << " 函数重定义" << endl;
			left_node = SYM(left_id, "error", -1);
			return left_node;
		}
		//2. 函数无返回值，检查return_flag
		if (return_flag) {
			cout << "第" << return_line << "行 " << func.get_name() << " 函数 无返回值" << endl;
		}
		//3. 函数表插入
		f = new funcEntry(func.get_name(), T_VOID, node[0].get_plist(), M.get_quad());
		funcTable.push_back(*f);

		delVar();

		left_node = SYM(left_id, "", 0);

	}
	else if (s._Equal("<var_declare> -> ;")) {//变量声明 ；
		left_node = SYM(left_id, "", node[0].get_line());
		//假如缺少分号，是否可以报错
	}
	else if (s._Equal("<func_declare> -> ( <fparam> ) <sent_block>")) {//函数声明 (形参) 语句块
		left_node = SYM(left_id, "", 0);
		left_node.set_plist(node[2].get_plist());
	}
	else if (s._Equal("<fparam> -> <param_list>")) {//形参 参数列表
		left_node = SYM(left_id, "", 0);
		left_node.set_plist(node[0].get_plist());
	}
	else if (s._Equal("<fparam> -> void")) {//形参 void
		left_node = SYM(left_id, "", 0);
		//形参为空
	}
	else if (s._Equal("<param_list> -> <param>")) {//参数列表 参数
		left_node = SYM(left_id, "", 0);
		left_node.set_plist(node[0].get_plist());
	}
	else if (s._Equal("<param_list> -> <param> , <param_list>")) {//参数列表 参数 , 参数列表
		left_node = SYM(left_id, "", 0);
		vector<int> temp = node[2].get_plist();		//param
		vector<int> Hplist = node[0].get_plist();	//param_list
		Hplist.insert(Hplist.end(), temp.begin(), temp.end());	//"参数列表"中压入"参数"

		left_node.set_plist(Hplist);
	}
	else if (s._Equal("<param> -> int ID")) {//参数 int 标识符
		/*形参5, int1*/
		SYM fparam = node[0];
		/*1. 查询形参是否重复*/
		tblEntry* e = lookupVar(fparam.get_name(), FORMAL_PARA);
		if (e) {
			cout << "第" << fparam.get_line() << "行 " << fparam.get_name() << " 变量重定义" << endl;
			left_node = SYM(left_id, "error", 0);
			return left_node;
		}
		/*2. 插入符号表*/
		e = new tblEntry(fparam.get_name(), FORMAL_PARA, T_INT);
		e->set_normal(false);
		enterVar(whichTable, e);
		/*3. 中间代码生成*/
		//INTcode.emit("get", "-", "-", fparam.get_name());

		left_node = SYM(left_id, "", 0);
		vector<int>plist = { T_INT };//形参列表，记录形参类型，int1
		left_node.set_plist(plist);
	}
	else if (s._Equal("<sent_block> -> { <inner_declare> <sent_list> }")) {//语句块 '{'内部声明 语句串'}'
		/*删表, whichTable函数内部-1*/
		//delVar();
		left_node = SYM(left_id, "", 0);
		left_node.set_nextlist(node[1].get_nextlist());
	}
	else if (s._Equal("<sent_block> -> { <sent_list> }")) {//语句块 '{'语句串'}'
		//delVar();
		left_node = SYM(left_id, "", 0);
		left_node.set_nextlist(node[1].get_nextlist());
	}
	else if (s._Equal("<inner_declare> -> <inner_var_dec>")) {//内部声明 内部变量声明
		left_node = SYM(left_id, node[0].get_name(), 0);
	}
	else if (s._Equal("<inner_declare> -> <inner_var_dec> <inner_declare>")) {//内部声明 内部变量声明 内部声明
		left_node = SYM(left_id, node[0].get_name(), 0);
	}
	else if (s._Equal("<inner_var_dec> -> int ID ;")) {//内部变量声明 int 标识符 ;
		/*内部变量2， int1*/
		SYM var = node[1];
		/*1. 查表*/
		tblEntry* e = lookupVar(var.get_name(), VARIABLE);
		if (e) {
			cout << "第" << var.get_line() << "行 " << var.get_name() << " 变量重定义" << endl;
			left_node = SYM(left_id, "error", -1);
			return left_node;
		}
		/*2. 插入符号表*/
		e = new tblEntry(var.get_name(), VARIABLE, T_INT);
		enterVar(whichTable, e);

		left_node = SYM(left_id, var.get_name(), 0);
	}
	else if (s._Equal("<sent_list> -> <sent> <M> <sent_list>")) {//语句串 语句 M 语句串
		/* 别忘了空产生式 */
		SYM M = node[1];
		/*回填*/
		INTcode.back_patch(node[2].get_nextlist(), M.get_quad());
		left_node = SYM(left_id, "", 0);
		left_node.set_nextlist(node[0].get_nextlist());
	}
	else if (s._Equal("<sent_list> -> <sent>")) {//语句串 语句 
		left_node = SYM(left_id, "", 0);
		left_node.set_nextlist(node[0].get_nextlist());
	}
	else if (s._Equal("<sent> -> <if_sent>")) {//语句 if语句
		left_node = SYM(left_id, "", 0);
		left_node.set_nextlist(node[0].get_nextlist());
	}
	else if (s._Equal("<sent> -> <while_sent>")) {//语句 while语句
		left_node = SYM(left_id, "", 0);
		left_node.set_nextlist(node[0].get_nextlist());
	}
	else if (s._Equal("<sent> -> <return_sent>")) {//语句 return语句
		left_node = SYM(left_id, "", 0);
		left_node.set_nextlist(node[0].get_nextlist());
	}
	else if (s._Equal("<sent> -> <assign_sent>")) {//语句 赋值语句
		left_node = SYM(left_id, "", 0);
		/* 唯赋值语句不考虑nextlist */
	}
	else if (s._Equal("<if_sent> -> if ( <expression> ) <M> <A> <sent_block>")) {//if语句 if ( <表达式> ）M A 语句块
		//回填
		INTcode.back_patch(node[4].get_truelist(), node[2].get_quad());
		//合并链表
		SemanticListNode* p = node[0].get_nextlist();
		SemanticListNode* q = node[4].get_falselist();
		left_node = SYM(left_id, "", 0);
		left_node.set_nextlist(merge(p, q));
		//删表
		delVar();
	}
	else if (s._Equal("<if_sent> -> if ( <expression> ) <M> <A> <sent_block> <N> else <M> <A> <sent_block>")) {
		//回填
		//expression->falselist, M2.quad
		INTcode.back_patch(node[9].get_falselist(), node[2].get_quad());
		//expression->truelist, M1.quad
		INTcode.back_patch(node[9].get_truelist(), node[7].get_quad());
		SemanticListNode* N = node[4].get_nextlist();
		SemanticListNode* S2 = node[0].get_nextlist();
		SemanticListNode* S1 = node[5].get_nextlist();
		left_node = SYM(left_id, "", 0);
		left_node.set_nextlist(merge(S1, merge(N, S2)));
		//删表，两次
		delVar();
		delVar();
	}
	else if (s._Equal("<while_sent> -> while <M> ( <expression> ) <M> <A> <sent_block>")) {
		//回填
		//sent_block->nextlist, M1.quad
		INTcode.back_patch(node[0].get_nextlist(), node[6].get_quad());
		//expression->truelist, M2.quad
		INTcode.back_patch(node[4].get_truelist(), node[2].get_quad());

		left_node = SYM(left_id, "", 0);
		left_node.set_nextlist(node[4].get_falselist());
		//中间代码生成
		INTcode.emit("j", "-", "- ", to_string(node[6].get_quad()));
		//删表
		delVar();
	}
	else if (s._Equal("<return_sent> -> return ;")) {//return语句 return ;
		left_node = SYM(left_id, "", 0);
		/* 中间代码生成 */
		INTcode.emit("return", "-", "-", "-");
		/* 当前函数无返回值 */
		return_flag = false;
		return_line = node[1].get_line();
	}
	else if (s._Equal("<return_sent> -> return <expression> ;")) {//return语句 return 表达式 ;
		left_node = SYM(left_id, "", 0);
		/* 中间代码生成 */
		INTcode.emit("return", node[1].get_name(), "-", "-");
		/* 当前函数存在返回值 */
		return_flag = true;
		return_line = node[2].get_line();
	}
	else if (s._Equal("<assign_sent> -> ID = <expression> ;")) {//赋值语句 ID = 表达式 ；
		/*查表*/
		SYM var = node[3];
		SYM exp = node[1];
		//标识符可能是全局变量，也可能是形参
		tblEntry* v = lookupVar(var.get_name(), GLOBAL_V);//全局变量4
		if (!v) {
			v = lookupVar(var.get_name(), VARIABLE); // 局部变量2
			if (!v) {
				v = lookupVar(var.get_name(), FORMAL_PARA);//形参5
				if (!v) {
					cout << "第" << var.get_line() << "行 " << var.get_name() << " 变量未声明" << endl;
					left_node = SYM(left_id, "error", -1);
					return left_node;
				}
			}
		}
		/*函数类型为void，标识符为int*/
		funcEntry* f = lookupFunc(exp.get_name());
		if (f) {
			if (v->get_typ() != f->get_type()) {
				cout << "第" << var.get_line() << "行 "
					<< "函数" << f->get_name() << " 类型与变量" << v->get_name() << "不匹配" << endl;
				left_node = SYM(left_id, "error", -1);
				return left_node;
			}
		}

		INTcode.emit("=", exp.get_name(), "-", var.get_name());
		left_node = SYM(left_id, "", 0);
	}
	else if (s._Equal("<expression> -> <add_expression>")) {
		SYM addexp = node[0];
		//加法表达式加入符号表
		tblEntry* e = new tblEntry(addexp.get_name(), VARIABLE, addexp.get_type());
		enterVar(whichTable, e);

		left_node = SYM(left_id, addexp.get_name(), 0, addexp.get_type());
	}
	else if (s._Equal("<expression> -> <add_expression> <relop> <expression>")) {
		//生成新的变量名
		string nu = newtemp();
		//表达式加入符号表
		tblEntry* e = new tblEntry(nu, VARIABLE, T_INT); //relop计算结果是0或1，不清楚是常量还是变量
		enterVar(whichTable, e);

		left_node = SYM(left_id, nu, 0, node[2].get_type());
		//设定表达式的truelist和falselist
		SemanticListNode* p = new SemanticListNode;
		p->pos = INTcode.nextquad();
		p->next = NULL;
		left_node.set_truelist(p);
		SemanticListNode* q = new SemanticListNode;
		q->pos = INTcode.nextquad() + 1;
		q->next = NULL;
		left_node.set_falselist(q);
		//中间代码生成
		INTcode.emit("j" + node[1].get_name(),
			node[2].get_name(),
			node[0].get_name(),
			"0");
		INTcode.emit("j", "-", "-", "0");
	}
	else if (s._Equal("<add_expression> -> <item>")) {//加法表达式 项
		left_node = SYM(left_id, node[0].get_name(), 0, node[0].get_type());
	}
	else if (s._Equal("<add_expression> -> <item> + <add_expression>")) {//加法表达式 项 + 加法表达式
		string nv = newtemp();
		INTcode.emit(node[1].get_name(), node[2].get_name(), node[0].get_name(), nv);
		left_node = SYM(left_id, nv, 0, node[2].get_type());
	}
	else if (s._Equal("<add_expression> -> <item> - <add_expression>")) {//加法表达式 项 - 加法表达式
		string nv = newtemp();
		INTcode.emit(node[1].get_name(), node[2].get_name(), node[0].get_name(), nv);
		left_node = SYM(left_id, nv, 0, node[2].get_type());
	}
	else if (s._Equal("<item> -> <factor>")) {//项 因子
		left_node = SYM(left_id, node[0].get_name(), 0, node[0].get_type());
	}
	else if (s._Equal("<item> -> <factor> * <item>")) {//项 因子 * 项
		string nv = newtemp();
		INTcode.emit(node[1].get_name(), node[2].get_name(), node[0].get_name(), nv);
		left_node = SYM(left_id, nv, 0, node[2].get_type());
	}
	else if (s._Equal("<item> -> <factor> / <item>")) {//项 因子 / 项
		string nv = newtemp();
		INTcode.emit(node[1].get_name(), node[2].get_name(), node[0].get_name(), nv);
		left_node = SYM(left_id, nv, 0, node[2].get_type());
	}
	else if (s._Equal("<factor> -> NUM")) {//因子 数字
		left_node = SYM(left_id, node[0].get_name(), 0, T_INT);	//数字 int1
	}
	else if (s._Equal("<factor> -> ID")) {//因子 标识符
		/*查表*/
		SYM var = node[0];
		//标识符可能是全局变量，也可能是形参
		tblEntry* v = lookupVar(var.get_name(), GLOBAL_V);//全局变量4
		if (!v) {
			v = lookupVar(var.get_name(), VARIABLE); // 局部变量2
			if (!v) {
				v = lookupVar(var.get_name(), FORMAL_PARA);//形参5
				if (!v) {
					cout << "第" << var.get_line() << "行 " << var.get_name() << " 变量未声明" << endl;
					left_node = SYM(left_id, "error", -1);
					return left_node;
				}
			}
		}
		left_node = SYM(left_id, var.get_name(), 0, v->get_typ());	//标识符在符号表中记录的类型
	}
	else if (s._Equal("<factor> -> ( <expression> )")) {//因子 (表达式)
		left_node = SYM(left_id, node[1].get_name(), 0, node[1].get_type());

	}
	else if (s._Equal("<factor> -> ID ( <aparam_list> )")) {//因子 标识符(实参列表)
		/*查表*/
		SYM g = node[3];
		funcEntry* f = lookupFunc(g.get_name());
		if (!f)
		{
			cout << "第" << g.get_line() << "行 " << "引用的函数 " << g.get_name() << " 未定义" << endl;
			left_node = SYM(left_id, "error", -1);
			return left_node;
		}
		if (!match(ap_list, f->get_param()))	//此处应该对实参列表和形参列表进行比较
		{
			cout << "第" << g.get_line() << "行 函数" << g.get_name() << "的参数类型或个数不匹配" << endl;
			left_node = SYM('X', "error", -1);
			return left_node;
		}
		//中间代码生成
		AP_list* head = ap_list;
		while (head) {
			INTcode.emit("par", head->name, "-", "-");
			head = head->next;
		}
		//销毁实参列表
		head = ap_list;
		destroyAPlist(head);
		//中间代码生成
		INTcode.emit("call", g.get_name(), "-", to_string(f->get_enterpoint()));
		left_node = SYM(left_id, g.get_name(), 0, f->get_type());	//函数返回类型
	}
	else if (s._Equal("<aparam_list> -> @")) {//实参列表 空
		left_node = SYM(left_id, "", 0);
	}
	else if (s._Equal("<aparam_list> -> <expression>")) {//实参列表 表达式
		ap_list = new AP_list;
		ap_list->name = node[0].get_name();
		ap_list->type = node[0].get_type();
		ap_list->next = NULL;
		left_node = SYM(left_id, node[0].get_name(), 0);
	}
	else if (s._Equal("<aparam_list> -> <expression> , <aparam_list>")) {//实参列表 表达式 分隔符 实参列表
		AP_list* tmp = new AP_list;
		tmp->name = node[2].get_name();
		tmp->type = node[2].get_type();
		tmp->next = NULL;

		//新实参接入实参列表末尾
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