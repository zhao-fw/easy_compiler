#pragma once

#include"BaseBlock.hpp"
#include<vector>
#include<string>
#include<map>
#include<list>
using namespace std;

void outputError(string err) {
	cerr << err << endl;
	exit(-1);
}

class ObjectGenerator
{
public:
	ObjectGenerator();
	~ObjectGenerator();
	// 将所有的四元式按照函数划分，每个函数中再划分为基本块（计算待用信息时，需要在每个函数块中完成）
	void Init(vector<Quaternary> all_code, vector<funcEntry> funcTable, vector<vector<tblEntry>> varTable);
	// 计算活跃变量，再计算待用信息
	void calculateInfo();

	// 存储变量
	void storeVar(string reg, string var);
	void releaseVar(string var);
	void storeOutLiveVar(set<string>& out);
	// 分配寄存器
	string allocateReg();			// 在现有寄存器中（都被占用）找到一个最合适的来存
	void cleanReg(string reg);		// 将reg中存放的变量内容清除，为存放des做准备
	string getDesReg();				// 为当前目标分配一个寄存器
	string getArgReg(string arg);	// 为指定的引用变量分配一个寄存器
	// 生成目标代码
	void generateCodeForQuatenary(int nowBaseBlockIndex, int& arg_num, int& par_num, list<pair<string, bool>>& par_list);
	void generateCodeForBaseBlocks(int nowBaseBlockIndex);
	void generateCodeForFuncBlocks(map<string, vector<BaseBlock*>>::iterator& now_func);
	void generateCode();
	// 打印
	void print(ostream& out = std::cout);
	void printObj(ostream& out = std::cout);

private:
	// 基本信息
	map<string, vector<BaseBlock*>> block;
	vector<funcEntry> funcTable;
	vector<vector<tblEntry>> varTable;
	map<string, vector<set<string> > >funcOUT;	// 各函数块中基本块的出口活跃变量集
	map<string, vector<set<string> > >funcIN;	// 各函数块中基本块的入口活跃变量集
	int blockNum;

	// 生成目标代码时使用
	string nowFunc;										// 当前分析的函数
	vector<BaseBlock*>::iterator nowBlock;				// 当前分析的基本块
	vector<QuaternaryWithInfo>::iterator nowQuatenary;	// 当前分析的四元式
	int top; // 当前栈顶
	map<string, int> varOffset;			// 各变量的存储位置
	list<string> freeReg;				// 空闲的寄存器编号
	map<string, set<string> > Avalue;	// 变量的存储情况
	map<string, set<string> > Rvalue;	// 寄存器的使用情况

	vector<string>objectCodes; //目标代码
};

ObjectGenerator::ObjectGenerator()
{
	blockNum = 0;
}
ObjectGenerator::~ObjectGenerator()
{
}

void ObjectGenerator::Init(vector<Quaternary> all_code, vector<funcEntry> funcTable, vector<vector<tblEntry>> varTable) {
	int num = all_code.size();
	bool* is_entry = new bool[num];
	bool* is_exit = new bool[num];
	memset(is_entry, 0, num * sizeof(bool));
	memset(is_exit, 0, num * sizeof(bool));
	vector<BaseBlock*> res;

	// 复制funcTable和varTable
	for (unsigned i = 0; i < funcTable.size(); ++i) {
		this->funcTable.push_back(funcTable[i]);
		block[funcTable[i].get_name()] = vector<BaseBlock*>();
		is_entry[funcTable[i].get_enterpoint()] = true;
	}
	for (unsigned i = 0; i < varTable.size(); ++i) {
		vector<tblEntry> tem;
		for (unsigned j = 0; j < varTable[i].size(); ++j) {
			tem.push_back(varTable[i][j]);
		}
		this->varTable.push_back(tem);
	}

	// 计算入口节点和结束节点
	for (int i = 0; i < num; ++i) {
		if (i == 0) {
			// 程序的第一个语句
			is_entry[i] = true;
		}
		if (i == num - 1) {
			// 程序的最后一个语句
			is_exit[i] = true;
		}
		if (all_code[i].op[0] == 'j') {
			// 紧跟在条件转移语句后面的语句
			if (i + 1 < num)
				is_entry[i + 1] = true;
			// 能由条件转移语句或无条件转移语句转移到的语句
			int des = std::stoi(all_code[i].des);
			is_entry[des] = true;

			// 条件转移语句是基本块的结束语句
			is_exit[i] = true;
			// 能由条件转移语句或无条件转移语句转移到的语句的前一条也是结束语句
			if (des - 1 >= 0)
				is_exit[des - 1] = true;
		}
		if (all_code[i].op == "return") {
			is_exit[i] = true;
		}
	}

	// 划分基本块
	int label = 0;
	string now_name;
	for (int i = 0; i < num; ++i) {
		if (is_entry[i]) {
			BaseBlock* tem = new BaseBlock;
			// 设置一个基本块（ID = blockNum）
			tem->setBlockID(this->blockNum);
			tem->setFirstNum(i);
			// 设置基本块的名字
			for (unsigned j = 0; j < funcTable.size(); j++) {
				if (i == funcTable[j].get_enterpoint()) {
					tem->setName(funcTable[j].get_name());
					now_name = funcTable[j].get_name();
				}
			}
			if (tem->getName() == "") {
				string name_add = "Label" + to_string(label);
				tem->setName(name_add);
				label++;
			}
			// 该基本块的四元式
			while (true) {
				// 如果不是结束语句，就一直将四元式加入基本块，如果是，则加入并退出
				if (is_exit[i]) {
					tem->addCode(all_code[i]);
					break;
				}
				else {
					tem->addCode(all_code[i]);
					i++;
				}
			}
			// 一个基本块结束，加入当前函数的vector，blockNum++
			res.push_back(tem);
			block[now_name].push_back(tem);
			this->blockNum++;
		}
	}

	// 制作流图
	for (int i = 0; i < blockNum; ++i) {
		BaseBlock* tem = res[i];
		vector<QuaternaryWithInfo> quat = tem->getQuaternarys();
		QuaternaryWithInfo end = quat.back();

		// 设置next
		if (i < blockNum - 1) {
			// 如果最后一条不是return语句，是在一个函数块内部，则next指向顺序的下一个位置
			if (end.op != "return" && end.op != "j")
				tem->setNext(res[i + 1]);
		}
		// 如果该block的最后一条语句是转移语句，则设置jump，同时将转移的目标设置为那个基本块的名字
		if (end.op[0] == 'j') {
			int des = std::stoi(end.des);
			for (int j = 0; j < blockNum; ++j) {
				if (res[j]->getFirstNum() == des) {
					tem->setJump(res[j]);
					tem->getQuaternarys().back().des = tem->getJump()->getName();
					break;
				}
			}
		}
	}

	// 结束时释放内存
	delete[] is_exit;
	delete[] is_entry;
	return;
}

void ObjectGenerator::calculateInfo() {
	// 要每个函数单独计算待用信息（因为函数之间是不同的变量表）
	for (auto i = block.begin(); i != block.end(); ++i) {
		vector<BaseBlock*> now_func = (*i).second;

		// 1. 计算def和use，为每个基本块的定值变量和声明变量
		vector<set<string>> USE, DEF;
		// 2. 通过USE和DEF来计算IN和OUT，为入口活跃变量和出口活跃变量
		vector<set<string>> IN, OUT;
		map<string, int> block_name;
		int block_num = 0;
		for (auto now_block = now_func.begin(); now_block != now_func.end(); now_block++) { // 函数中的每个基本块
			set<string> def, use;
			vector<QuaternaryWithInfo> all_quat = (*now_block)->getQuaternarys();
			for (auto now_quat = all_quat.begin(); now_quat != all_quat.end(); now_quat++) { // 每个四元式
				if ((*now_quat).op == "j" || (*now_quat).op == "call") {
					// 这些中间代码无变量
				}
				else {
					if (isalpha((*now_quat).arg1[0]) && def.count((*now_quat).arg1) == 0) { // 源操作数1是变量且还没有被定值
						use.insert((*now_quat).arg1);
					}
					if (isalpha((*now_quat).arg2[0]) && def.count((*now_quat).arg2) == 0) { // 源操作数2是变量且还没有被定值
						use.insert((*now_quat).arg2);
					}
					if ((*now_quat).op[0] != 'j') { // j>= j<= j== j!= j> j< 这些中间语句不需要判断目的操作数
						if (isalpha((*now_quat).des[0]) && use.count((*now_quat).des) == 0) { // 如果目的操作数还没有被引用
							def.insert((*now_quat).des);
						}
					}
				}
			}
			DEF.push_back(def);
			USE.push_back(use);
			IN.push_back(use);
			OUT.push_back(set<string>());
			block_name[(*now_block)->getName()] = block_num;
			block_num++;
		}

		bool change = true;
		while (change) { // 当IN和OUT集合不再变化时结束
			change = false;
			for (auto now_block = now_func.begin(); now_block != now_func.end(); now_block++) {
				// 对于block的每一个后继，计算OUT[B] = ∪ IN[S] ; (S时B的一个后继)
				// 然后计算IN[B] = use[B] ∪ (OUT[B] - def[B]);
				int blockIndex = block_name[(*now_block)->getName()];
				BaseBlock* next = (*now_block)->getNext();
				BaseBlock* jump = (*now_block)->getJump();
				// 后继1
				if (next != NULL) {
					int n = block_name[next->getName()];
					// 遍历IN[next]中的每一个var，并入OUT[now_block]中
					for (auto now_var = IN[n].begin(); now_var != IN[n].end(); now_var++) {
						if (OUT[blockIndex].insert(*now_var).second == true) {
							if (DEF[blockIndex].count(*now_var) == 0) {
								IN[blockIndex].insert(*now_var);
							}
							change = true;
						}
					}
				}
				// 后继2
				if (jump != NULL) {
					int n = block_name[jump->getName()];
					// 遍历IN[jump]中的每一个var，并入OUT[now_block]中
					for (auto now_var = IN[n].begin(); now_var != IN[n].end(); now_var++) {
						if (OUT[blockIndex].insert(*now_var).second == true) {
							if (DEF[blockIndex].count(*now_var) == 0) {
								IN[blockIndex].insert(*now_var);
							}
							change = true;
						}
					}
				}
			}
		}
		funcIN[(*i).first] = IN;
		funcOUT[(*i).first] = OUT;

		vector<map<string, VarInfomation> > symTables; // 每个基本块对应一张符号表
		// 初始化符号表
		for (auto now_block = now_func.begin(); now_block != now_func.end(); now_block++) {
			map<string, VarInfomation> symTable;
			vector<QuaternaryWithInfo> all_quat = (*now_block)->getQuaternarys();
			// 将所有的变量加入到symTable中（初始化）
			for (auto now_quat = all_quat.begin(); now_quat != all_quat.end(); now_quat++) {
				if ((*now_quat).op == "j" || (*now_quat).op == "call") {
					//pass
				}
				else{
					if (isalpha((*now_quat).arg1[0])) {
						symTable[(*now_quat).arg1] = VarInfomation{ -1,false };
					}
					if (isalpha((*now_quat).arg2[0])) {
						symTable[(*now_quat).arg1] = VarInfomation{ -1,false };
					}
					if ((*now_quat).op[0] != 'j') { // j>= j<= j== j!= j> j< 这些中间语句不需要判断目的操作数
						if (isalpha((*now_quat).des[0])) {
							symTable[(*now_quat).des] = VarInfomation{ -1,false };
						}
					}
				}
			}
			symTables.push_back(symTable);
		}
		// 将上述求出的活跃变量对应的符号表中的内容赋值
		for (auto now_block = now_func.begin(); now_block != now_func.end(); now_block++) { 
			// 遍历该基本块的OUT集合，修改symTable中内容
			int blockIndex = block_name[(*now_block)->getName()];
			for (auto now_var = OUT[blockIndex].begin(); now_var != OUT[blockIndex].end(); now_var++) {
				symTables[blockIndex][(*now_var)].active = true;
				symTables[blockIndex][(*now_var)].next = -1;
			}
		}

		// 计算每个四元式的待用信息和活跃信息（课本上的内容）
		for (auto now_block = now_func.begin(); now_block != now_func.end(); now_block++) {
			// 对于每一个基本块，从后向前遍历其中的四元式
			int blockIndex = block_name[(*now_block)->getName()];
			vector<QuaternaryWithInfo> &all_quat = (*now_block)->getQuaternarys();
			int n = all_quat.size() - 1;
			for (auto now_quat = all_quat.rbegin(); now_quat != all_quat.rend(); now_quat++, n--) {
				if ((*now_quat).op == "j" || (*now_quat).op == "call") {
					// pass
				}
				else {
					if (isalpha((*now_quat).arg1[0])) {
						(*now_quat).info1 = symTables[blockIndex][(*now_quat).arg1];
						symTables[blockIndex][(*now_quat).arg1] = VarInfomation(n, true);
					}
					if (isalpha((*now_quat).arg2[0])) {
						(*now_quat).info2 = symTables[blockIndex][(*now_quat).arg2];
						symTables[blockIndex][(*now_quat).arg2] = VarInfomation(n, true);
					}
					if ((*now_quat).op[0] != 'j' && isalpha((*now_quat).des[0])) {
						(*now_quat).info3 = symTables[blockIndex][(*now_quat).des];
						symTables[blockIndex][(*now_quat).des] = VarInfomation(-1, true);
					}
				}
			}
		}
	}	
}

void ObjectGenerator::storeVar(string reg, string var) {
	if (varOffset.find(var) != varOffset.end()) { // 如果已经为*iter分配好了存储空间
		objectCodes.push_back(string("sw ") + reg + " " + to_string(varOffset[var]) + "($sp)");
	}
	else {
		varOffset[var] = top;
		top += 4;
		objectCodes.push_back(string("sw ") + reg + " " + to_string(varOffset[var]) + "($sp)");
	}
	Avalue[var].insert(var);
}

void ObjectGenerator::releaseVar(string var)	 {
	for (auto iter = Avalue[var].begin(); iter != Avalue[var].end(); iter++) {
		if ((*iter)[0] == '$') {
			Rvalue[*iter].erase(var);
			if (Rvalue[*iter].size() == 0 && (*iter)[1] == 's') {
				freeReg.push_back(*iter);
			}
		}
	}
	Avalue[var].clear();
}

void ObjectGenerator::storeOutLiveVar(set<string>& out) {
	for (auto now_var = out.begin(); now_var != out.end(); ++now_var) {
		string reg;	//活跃变量所在的寄存器名称
		bool inFlag = false;	//活跃变量在内存中的标志
		for (auto now_reg = Avalue[*now_var].begin(); now_reg != Avalue[*now_var].end(); ++now_reg) {
			if ((*now_reg)[0] != '$') {//该活跃变量已经存储在内存中
				inFlag = true;
				break;
			}
			else {
				reg = *now_reg;
			}
		}
		if (!inFlag) {//如果该活跃变量不在内存中，则将reg中的var变量存入内存
			storeVar(reg, *now_var);
		}
	}
}

string ObjectGenerator::getDesReg() {
	// A := B op C 
	
	/*
	* 规则1：
	*      如果 B 的现行值在某个寄存器 Ri 中， RVALUE[Ri]中只包含B
	*      并且 B与A是同一个标识符 或者 B的现行值在执行四元式 A:=B op C之后不会再引用
	*      则选取Ri为所需要的寄存器R
	*/
	string A = (*nowQuatenary).des;
	string B = (*nowQuatenary).arg1;
	string C = (*nowQuatenary).arg2;
	string res;
	if (isalpha(B[0])) { // B是变量
		for (auto var_reg = Avalue[B].begin(); var_reg != Avalue[B].end(); var_reg++) {
			if ((*var_reg)[0] == '$') { // B存在寄存器中
				if (Rvalue[*var_reg].size() == 1) { // 并且该寄存器只放了这一个变量
					// B与A相同 或者 B的值之后不在引用
					if (B == A || (!(*nowQuatenary).info1.active && (*nowQuatenary).info1.next == -1)) { 
						// 让该寄存器中存储A变量
						res = *var_reg;
						Avalue[A].insert(res);
						Rvalue[res].insert(A);
						return *var_reg;
					}
				}
			}
		}
	}

	/*
	* 规则2：
	*      如果尚有未分配的寄存器，则从中选取一个Ri为要分配的寄存器
	*/
	if (freeReg.size() > 0) { // 空闲寄存器数量大于0
		res = freeReg.back();
		freeReg.pop_back();
		Avalue[A].insert(res);
		Rvalue[res].insert(A);
		return res;
	}

	/*
	* 规则3：
	*     从已分配的寄存器中选取一个Ri为所需要的寄 存器R。最好使得Ri满足以下条件： 
	*     占用Ri的变量的值同时存放在该变量的贮存单元中 或者 在基本块中要在最远的将来才会引用到或不会引用到。
	*/
	res = allocateReg();
	cleanReg(res);
	Avalue[A].insert(res);
	Rvalue[res].insert(A);
	return res;
}

string ObjectGenerator::getArgReg(string arg) {
	// 数字
	if (isdigit(arg[0])) {
		string ret = allocateReg();
		objectCodes.push_back(string("addi ") + ret + " $zero " + arg);
		return ret;
	}

	for (set<string>::iterator iter = Avalue[arg].begin(); iter != Avalue[arg].end(); iter++) {
		if ((*iter)[0] == '$') {//如果变量已经保存在某个寄存器中
			return *iter;//直接返回该寄存器
		}
	}

	//如果该变量没有在某个寄存器中
	string ret = allocateReg();
	for (auto now_var = Rvalue[ret].begin(); now_var != Rvalue[ret].end(); ++now_var) {
		//对ret的寄存器中保存的变量，他们都将不再存储在ret中
		Avalue[*now_var].erase(ret);
		//如果V的地址描述数组AVALUE[V]说V还保存在R之外的其他地方，则不需要生成存数指令
		if (Avalue[*now_var].size() > 0) {
			//pass
		}
		//如果V不会在此之后被使用，则不需要生成存数指令
		else {
			bool storeFlag = true;
			vector<QuaternaryWithInfo>::iterator quat;
			for (quat = nowQuatenary; quat != (*nowBlock)->getQuaternarys().end(); ++quat) { // 如果即被引用又被赋值？？？
				if (quat->arg1 == *now_var || quat->arg2 == *now_var) {//如果V在本基本块中被引用
					storeFlag = true;
					break;
				}
				if (quat->des == *now_var) {//如果V在本基本块中被赋值
					storeFlag = false;
					break;
				}
			}
			if (quat == (*nowBlock)->getQuaternarys().end()) {//如果V在本基本块中未被引用，且也没有被赋值
				int index = nowBlock - block[nowFunc].begin();
				if (funcOUT[nowFunc][index].count(*now_var) == 1) {//如果此变量是出口之后的活跃变量
					storeFlag = true;
				}
				else {
					storeFlag = false;
				}
			}
			if (storeFlag) {//生成存数指令
				storeVar(ret, *now_var);
			}
		}
	}
	Rvalue[ret].clear();//清空ret寄存器中保存的变量
	objectCodes.push_back(string("lw ") + ret + " " + to_string(varOffset[arg]) + "($sp)");
	Avalue[arg].insert(ret);
	Rvalue[ret].insert(arg);
	return ret;
}

string ObjectGenerator::allocateReg() {
	string res;
	string* reg_name = new string[Rvalue.size()];
	double* cost = new double[Rvalue.size()];
	for (unsigned i = 0; i < Rvalue.size(); ++i) {
		cost[i] = 0.0;
		reg_name[i] = "";
	}
	int regIndex = 0;

	// 在所有的寄存器中找到一个合适的
	for (auto now_reg = Rvalue.begin(); now_reg != Rvalue.end(); ++now_reg, ++regIndex) {
		// 遍历该寄存器存储所有的变量，计算移除该变量的代价
		reg_name[regIndex] = (*now_reg).first;
		for (auto now_val = (*now_reg).second.begin(); now_val != (*now_reg).second.end(); ++now_val) {
			if (Avalue[*now_val].size() > 1) { // 有其他位置保存了该变量的值
				cost[regIndex] += 0.0;
			}
			else {
				for (auto quat = nowQuatenary; quat != (*nowBlock)->getQuaternarys().end(); ++quat) {
					if (*now_val == quat->arg1 || *now_val == quat->arg2) { // 之后又被引用，计算其中的代价
						cost[regIndex] += double(quat - nowQuatenary) / 1.0;
						break;
					}
					else if (*now_val == quat->des) { // 之后又被定值，代价为0
						cost[regIndex] += 0.0;
						break;
					}
				}
			}
		}
	}

	// 选取代价最小的寄存器
	double min_cost = cost[0];
	for (unsigned i = 0; i < Rvalue.size(); ++i) {
		if (cost[i] < min_cost) {
			min_cost = cost[i];
			res = reg_name[i];
		}
	}

	delete[] cost;
	delete[] reg_name;
	return res;
}

void ObjectGenerator::cleanReg(string reg) {
	string A = (*nowQuatenary).des;
	string B = (*nowQuatenary).arg1;
	string C = (*nowQuatenary).arg2;
	/*
	* 对RVALUE[Ri]中每一变量M：如果 <M不是A> 或者 <M是A又是C，但不是B并且B也不在RVALUE[Ri]中> 则执行：
	*	1.如果AVALUE[M]不包含M，则生成目标代码	ST	Ri,	M
	*
	*	2.如果M是B，或者M是C但同时B也在RVALUE[Ri]中，则令AVALUE[M] = {M, Ri} ，否则令 AVALUE[M] = {M}
	*
	*	3.删除RVALUE[Ri]中的M
	*/
	for (auto now_var = Rvalue[reg].begin(); now_var != Rvalue[reg].end(); ++now_var) {
		string M = *now_var;
		if (M != A || (M == A && M == C && M != B && Rvalue[reg].find(B) == Rvalue[reg].end())) {
			if (Avalue[M].find(M) == Avalue[M].end()) {
				storeVar(reg, M);
			}
			if (M == B || (M == C && Rvalue[reg].find(B) != Rvalue[reg].end())) {
				Avalue[M].insert(M);
				Avalue[M].insert(reg);
			}
			else {
				Avalue[M].insert(M);
				Avalue[M].erase(reg);
			}
			Rvalue[reg].erase(M);
		}
	}
}

void ObjectGenerator::generateCodeForQuatenary(int nowBaseBlockIndex, int& arg_num, int& par_num, list<pair<string, bool> >& par_list) {
	if (nowQuatenary->op[0] != 'j' && nowQuatenary->op != "call") {
		if (isalpha(nowQuatenary->arg1[0]) && Avalue[nowQuatenary->arg1].empty()) {
			outputError(string("变量") + nowQuatenary->arg1 + "在引用前未赋值");
		}
		if (isalpha(nowQuatenary->arg2[0]) && Avalue[nowQuatenary->arg2].empty()) {
			outputError(string("变量") + nowQuatenary->arg2 + "在引用前未赋值");
		}
	}
	
	// 无条件转移j
	if (nowQuatenary->op == "j") {
		objectCodes.push_back(nowQuatenary->op + " " + nowQuatenary->des);
	}
	// 条件转移j>= j<= j== j!= j> j<
	else if (nowQuatenary->op[0] == 'j') {
		string op;
		if (nowQuatenary->op == "j>=")
			op = "bge";
		else if (nowQuatenary->op == "j>")
			op = "bgt";
		else if (nowQuatenary->op == "j==")
			op = "beq";
		else if (nowQuatenary->op == "j!=")
			op = "bne";
		else if (nowQuatenary->op == "j<")
			op = "blt";
		else if (nowQuatenary->op == "j<=")
			op = "ble";
		string pos1 = getArgReg(nowQuatenary->arg1);	// 源操作数1
		string pos2 = getArgReg(nowQuatenary->arg2);	// 源操作数2
		objectCodes.push_back(op + " " + pos1 + " " + pos2 + " " + nowQuatenary->des);
		if (!nowQuatenary->info1.active) {
			releaseVar(nowQuatenary->arg1);
		}
		if (!nowQuatenary->info2.active) {
			releaseVar(nowQuatenary->arg2);
		}
	}
	// 添加参数par
	else if (nowQuatenary->op == "par") {
		par_list.push_back(pair<string, bool>(nowQuatenary->arg1, nowQuatenary->info1.active));
	}
	// 函数调用call
	else if (nowQuatenary->op == "call") {
		/*将参数压栈*/
		for (auto now_par = par_list.begin(); now_par != par_list.end(); ++now_par) {
			string pos = getArgReg(now_par->first);
			objectCodes.push_back(string("sw ") + pos + " " + to_string(top + 4 * (++arg_num + 1)) + "($sp)");
			if (!now_par->second) {
				releaseVar(now_par->first);
			}
		}
		/*更新$sp*/
		objectCodes.push_back(string("sw $sp ") + to_string(top) + "($sp)");
		objectCodes.push_back(string("addi $sp $sp ") + to_string(top));

		/*跳转到对应函数*/
		objectCodes.push_back(string("jal ") + nowQuatenary->arg1);

		/*恢复现场*/
		objectCodes.push_back(string("lw $sp 0($sp)"));
	}
	// 函数返回return
	else if (nowQuatenary->op == "return") {
		if (isdigit(nowQuatenary->arg1[0])) { // 返回值为数字
			objectCodes.push_back("addi $v0 $zero " + nowQuatenary->arg1);
		}
		else if (isalpha(nowQuatenary->arg1[0])) { // 返回值为变量
			set<string>::iterator now_reg = Avalue[nowQuatenary->arg1].begin();
			if ((*now_reg)[0] == '$') {
				objectCodes.push_back(string("add $v0 $zero ") + *now_reg);
			}
			else {
				objectCodes.push_back(string("lw $v0 ") + to_string(varOffset[*now_reg]) + "($sp)");
			}
		}
		if (nowFunc == "main") {
			objectCodes.push_back("j end");
		}
		else {
			objectCodes.push_back("lw $ra 4($sp)");
			objectCodes.push_back("jr $ra");
		}
	}
	// 获取参数get
	else if (nowQuatenary->op == "get") {
		//varOffset[nowQuatenary->arg1] = 4 * (par_num++ + 2);
		varOffset[nowQuatenary->des] = top;
		top += 4;
		Avalue[nowQuatenary->des].insert(nowQuatenary->des);
	}
	// 赋值=
	else if (nowQuatenary->op == "=") {
		//Avalue[nowQuatenary->q.des] = set<string>();
		string src1Pos;
		src1Pos = getArgReg(nowQuatenary->arg1);
		Rvalue[src1Pos].insert(nowQuatenary->des);
		Avalue[nowQuatenary->des].insert(src1Pos);
	}
	// 运算 + - * /
	else {
		string src1Pos = getArgReg(nowQuatenary->arg1);
		string src2Pos = getArgReg(nowQuatenary->arg2);
		string desPos = getDesReg();
		if (nowQuatenary->op == "+") {
			objectCodes.push_back(string("add ") + desPos + " " + src1Pos + " " + src2Pos);
		}
		else if (nowQuatenary->op == "-") {
			objectCodes.push_back(string("sub ") + desPos + " " + src1Pos + " " + src2Pos);
		}
		else if (nowQuatenary->op == "*") {
			objectCodes.push_back(string("mul ") + desPos + " " + src1Pos + " " + src2Pos);
		}
		else if (nowQuatenary->op == "/") {
			objectCodes.push_back(string("div ") + src1Pos + " " + src2Pos);
			objectCodes.push_back(string("mflo ") + desPos);
		}
		if (!nowQuatenary->info1.active) {
			releaseVar(nowQuatenary->arg1);
		}
		if (!nowQuatenary->info2.active) {
			releaseVar(nowQuatenary->arg2);
		}
	}
}

void ObjectGenerator::generateCodeForBaseBlocks(int nowBaseBlockIndex) {
	int arg_num = 0;//par的实参个数
	int par_num = 0;//get的形参个数
	list<pair<string, bool>> par_list;//函数调用用到的实参集list<实参名,是否活跃>

	Avalue.clear();
	Rvalue.clear();
	set<string>& in = funcIN[nowFunc][nowBaseBlockIndex];
	for (set<string>::iterator in_var = in.begin(); in_var != in.end(); ++in_var) {
		Avalue[*in_var].insert(*in_var);
	}

	//初始化空闲寄存器
	freeReg.clear();
	for (int i = 0; i <= 7; i++) {
		freeReg.push_back(string("$s") + to_string(i));
	}

	// 基本块的名字
	objectCodes.push_back((*nowBlock)->getName() + ":");
	// 是否是函数块的第一个基本块（过程调用问题）
	if (nowBaseBlockIndex == 0) {
		if (nowFunc == "main") {
			top = 8;
		}
		else {
			objectCodes.push_back("sw $ra 4($sp)");//把返回地址压栈
			top = 8;
		}
	}
	// 翻译每一条四元式
	for (auto now_quat = (*nowBlock)->getQuaternarys().begin(); now_quat != (*nowBlock)->getQuaternarys().end(); ++now_quat) {
		nowQuatenary = now_quat;
		//如果是基本块的最后一条语句
		if (now_quat + 1 == (*nowBlock)->getQuaternarys().end()) {
			string op = (*now_quat).op;
			//如果最后一条语句是控制语句，则先将出口活跃变量保存，再进行跳转(j,call,return)
			if (op[0] == 'j' || op == "call" || op == "return" || op == "get") {
				storeOutLiveVar(funcOUT[nowFunc][nowBaseBlockIndex]);
				generateCodeForQuatenary(nowBaseBlockIndex, arg_num, par_num, par_list);
			}
			//如果最后一条语句不是控制语句（是赋值语句），则先计算，再将出口活跃变量保存
			else {
				generateCodeForQuatenary(nowBaseBlockIndex, arg_num, par_num, par_list);
				storeOutLiveVar(funcOUT[nowFunc][nowBaseBlockIndex]);
			}
		}
		else {
			generateCodeForQuatenary(nowBaseBlockIndex, arg_num, par_num, par_list);
		}

	}
}

void ObjectGenerator::generateCodeForFuncBlocks(map<string, vector<BaseBlock*>>::iterator& now_func) {
	varOffset.clear();
	nowFunc = now_func->first;
	vector<BaseBlock*>& Blocks = now_func->second;
	for (auto now_block = now_func->second.begin(); now_block != now_func->second.end(); ++now_block) {//对每一个基本块
		nowBlock = now_block;
		generateCodeForBaseBlocks(nowBlock - now_func->second.begin());
	}
}

void ObjectGenerator::generateCode() {
	// objectCodes.push_back("lui $sp,0x1001");
	objectCodes.push_back("j main");
	for (auto now_func = block.begin(); now_func != block.end(); ++now_func) {//对每一个函数块
		generateCodeForFuncBlocks(now_func);
	}
	objectCodes.push_back("end:");
}

void ObjectGenerator::print(ostream& out) {
	// 打印所有函数
	for (auto fun = block.begin(); fun != block.end(); fun++) {
		vector<BaseBlock*> now_fun = fun->second;
		// 打印该函数的所有基本块
		for (auto now_blk = now_fun.begin(); now_blk != now_fun.end(); now_blk++) {
			// 打印基本块的所有四元式
			vector<QuaternaryWithInfo> now_code = (*now_blk)->getQuaternarys();
			out << (*now_blk)->getName() << endl;
			for (unsigned j = 0; j < now_code.size(); ++j) {
				out << now_code[j] << endl;
			}
		}
	}
}

void ObjectGenerator::printObj(ostream& out) {
	for (auto code = objectCodes.begin(); code != objectCodes.end(); ++code) {
		out << *code << endl;
	}
}

