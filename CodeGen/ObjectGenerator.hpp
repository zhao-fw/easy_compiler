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
	// �����е���Ԫʽ���պ������֣�ÿ���������ٻ���Ϊ�����飨���������Ϣʱ����Ҫ��ÿ������������ɣ�
	void Init(vector<Quaternary> all_code, vector<funcEntry> funcTable, vector<vector<tblEntry>> varTable);
	// �����Ծ�������ټ��������Ϣ
	void calculateInfo();

	// �洢����
	void storeVar(string reg, string var);
	void releaseVar(string var);
	void storeOutLiveVar(set<string>& out);
	// ����Ĵ���
	string allocateReg();			// �����мĴ����У�����ռ�ã��ҵ�һ������ʵ�����
	void cleanReg(string reg);		// ��reg�д�ŵı������������Ϊ���des��׼��
	string getDesReg();				// Ϊ��ǰĿ�����һ���Ĵ���
	string getArgReg(string arg);	// Ϊָ�������ñ�������һ���Ĵ���
	// ����Ŀ�����
	void generateCodeForQuatenary(int nowBaseBlockIndex, int& arg_num, int& par_num, list<pair<string, bool>>& par_list);
	void generateCodeForBaseBlocks(int nowBaseBlockIndex);
	void generateCodeForFuncBlocks(map<string, vector<BaseBlock*>>::iterator& now_func);
	void generateCode();
	// ��ӡ
	void print(ostream& out = std::cout);
	void printObj(ostream& out = std::cout);

private:
	// ������Ϣ
	map<string, vector<BaseBlock*>> block;
	vector<funcEntry> funcTable;
	vector<vector<tblEntry>> varTable;
	map<string, vector<set<string> > >funcOUT;	// ���������л�����ĳ��ڻ�Ծ������
	map<string, vector<set<string> > >funcIN;	// ���������л��������ڻ�Ծ������
	int blockNum;

	// ����Ŀ�����ʱʹ��
	string nowFunc;										// ��ǰ�����ĺ���
	vector<BaseBlock*>::iterator nowBlock;				// ��ǰ�����Ļ�����
	vector<QuaternaryWithInfo>::iterator nowQuatenary;	// ��ǰ��������Ԫʽ
	int top; // ��ǰջ��
	map<string, int> varOffset;			// �������Ĵ洢λ��
	list<string> freeReg;				// ���еļĴ������
	map<string, set<string> > Avalue;	// �����Ĵ洢���
	map<string, set<string> > Rvalue;	// �Ĵ�����ʹ�����

	vector<string>objectCodes; //Ŀ�����
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

	// ����funcTable��varTable
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

	// ������ڽڵ�ͽ����ڵ�
	for (int i = 0; i < num; ++i) {
		if (i == 0) {
			// ����ĵ�һ�����
			is_entry[i] = true;
		}
		if (i == num - 1) {
			// ��������һ�����
			is_exit[i] = true;
		}
		if (all_code[i].op[0] == 'j') {
			// ����������ת������������
			if (i + 1 < num)
				is_entry[i + 1] = true;
			// ��������ת������������ת�����ת�Ƶ������
			int des = std::stoi(all_code[i].des);
			is_entry[des] = true;

			// ����ת������ǻ�����Ľ������
			is_exit[i] = true;
			// ��������ת������������ת�����ת�Ƶ�������ǰһ��Ҳ�ǽ������
			if (des - 1 >= 0)
				is_exit[des - 1] = true;
		}
		if (all_code[i].op == "return") {
			is_exit[i] = true;
		}
	}

	// ���ֻ�����
	int label = 0;
	string now_name;
	for (int i = 0; i < num; ++i) {
		if (is_entry[i]) {
			BaseBlock* tem = new BaseBlock;
			// ����һ�������飨ID = blockNum��
			tem->setBlockID(this->blockNum);
			tem->setFirstNum(i);
			// ���û����������
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
			// �û��������Ԫʽ
			while (true) {
				// ������ǽ�����䣬��һֱ����Ԫʽ��������飬����ǣ�����벢�˳�
				if (is_exit[i]) {
					tem->addCode(all_code[i]);
					break;
				}
				else {
					tem->addCode(all_code[i]);
					i++;
				}
			}
			// һ����������������뵱ǰ������vector��blockNum++
			res.push_back(tem);
			block[now_name].push_back(tem);
			this->blockNum++;
		}
	}

	// ������ͼ
	for (int i = 0; i < blockNum; ++i) {
		BaseBlock* tem = res[i];
		vector<QuaternaryWithInfo> quat = tem->getQuaternarys();
		QuaternaryWithInfo end = quat.back();

		// ����next
		if (i < blockNum - 1) {
			// ������һ������return��䣬����һ���������ڲ�����nextָ��˳�����һ��λ��
			if (end.op != "return" && end.op != "j")
				tem->setNext(res[i + 1]);
		}
		// �����block�����һ�������ת����䣬������jump��ͬʱ��ת�Ƶ�Ŀ������Ϊ�Ǹ������������
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

	// ����ʱ�ͷ��ڴ�
	delete[] is_exit;
	delete[] is_entry;
	return;
}

void ObjectGenerator::calculateInfo() {
	// Ҫÿ�������������������Ϣ����Ϊ����֮���ǲ�ͬ�ı�����
	for (auto i = block.begin(); i != block.end(); ++i) {
		vector<BaseBlock*> now_func = (*i).second;

		// 1. ����def��use��Ϊÿ��������Ķ�ֵ��������������
		vector<set<string>> USE, DEF;
		// 2. ͨ��USE��DEF������IN��OUT��Ϊ��ڻ�Ծ�����ͳ��ڻ�Ծ����
		vector<set<string>> IN, OUT;
		map<string, int> block_name;
		int block_num = 0;
		for (auto now_block = now_func.begin(); now_block != now_func.end(); now_block++) { // �����е�ÿ��������
			set<string> def, use;
			vector<QuaternaryWithInfo> all_quat = (*now_block)->getQuaternarys();
			for (auto now_quat = all_quat.begin(); now_quat != all_quat.end(); now_quat++) { // ÿ����Ԫʽ
				if ((*now_quat).op == "j" || (*now_quat).op == "call") {
					// ��Щ�м�����ޱ���
				}
				else {
					if (isalpha((*now_quat).arg1[0]) && def.count((*now_quat).arg1) == 0) { // Դ������1�Ǳ����һ�û�б���ֵ
						use.insert((*now_quat).arg1);
					}
					if (isalpha((*now_quat).arg2[0]) && def.count((*now_quat).arg2) == 0) { // Դ������2�Ǳ����һ�û�б���ֵ
						use.insert((*now_quat).arg2);
					}
					if ((*now_quat).op[0] != 'j') { // j>= j<= j== j!= j> j< ��Щ�м���䲻��Ҫ�ж�Ŀ�Ĳ�����
						if (isalpha((*now_quat).des[0]) && use.count((*now_quat).des) == 0) { // ���Ŀ�Ĳ�������û�б�����
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
		while (change) { // ��IN��OUT���ϲ��ٱ仯ʱ����
			change = false;
			for (auto now_block = now_func.begin(); now_block != now_func.end(); now_block++) {
				// ����block��ÿһ����̣�����OUT[B] = �� IN[S] ; (SʱB��һ�����)
				// Ȼ�����IN[B] = use[B] �� (OUT[B] - def[B]);
				int blockIndex = block_name[(*now_block)->getName()];
				BaseBlock* next = (*now_block)->getNext();
				BaseBlock* jump = (*now_block)->getJump();
				// ���1
				if (next != NULL) {
					int n = block_name[next->getName()];
					// ����IN[next]�е�ÿһ��var������OUT[now_block]��
					for (auto now_var = IN[n].begin(); now_var != IN[n].end(); now_var++) {
						if (OUT[blockIndex].insert(*now_var).second == true) {
							if (DEF[blockIndex].count(*now_var) == 0) {
								IN[blockIndex].insert(*now_var);
							}
							change = true;
						}
					}
				}
				// ���2
				if (jump != NULL) {
					int n = block_name[jump->getName()];
					// ����IN[jump]�е�ÿһ��var������OUT[now_block]��
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

		vector<map<string, VarInfomation> > symTables; // ÿ���������Ӧһ�ŷ��ű�
		// ��ʼ�����ű�
		for (auto now_block = now_func.begin(); now_block != now_func.end(); now_block++) {
			map<string, VarInfomation> symTable;
			vector<QuaternaryWithInfo> all_quat = (*now_block)->getQuaternarys();
			// �����еı������뵽symTable�У���ʼ����
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
					if ((*now_quat).op[0] != 'j') { // j>= j<= j== j!= j> j< ��Щ�м���䲻��Ҫ�ж�Ŀ�Ĳ�����
						if (isalpha((*now_quat).des[0])) {
							symTable[(*now_quat).des] = VarInfomation{ -1,false };
						}
					}
				}
			}
			symTables.push_back(symTable);
		}
		// ����������Ļ�Ծ������Ӧ�ķ��ű��е����ݸ�ֵ
		for (auto now_block = now_func.begin(); now_block != now_func.end(); now_block++) { 
			// �����û������OUT���ϣ��޸�symTable������
			int blockIndex = block_name[(*now_block)->getName()];
			for (auto now_var = OUT[blockIndex].begin(); now_var != OUT[blockIndex].end(); now_var++) {
				symTables[blockIndex][(*now_var)].active = true;
				symTables[blockIndex][(*now_var)].next = -1;
			}
		}

		// ����ÿ����Ԫʽ�Ĵ�����Ϣ�ͻ�Ծ��Ϣ���α��ϵ����ݣ�
		for (auto now_block = now_func.begin(); now_block != now_func.end(); now_block++) {
			// ����ÿһ�������飬�Ӻ���ǰ�������е���Ԫʽ
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
	if (varOffset.find(var) != varOffset.end()) { // ����Ѿ�Ϊ*iter������˴洢�ռ�
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
		string reg;	//��Ծ�������ڵļĴ�������
		bool inFlag = false;	//��Ծ�������ڴ��еı�־
		for (auto now_reg = Avalue[*now_var].begin(); now_reg != Avalue[*now_var].end(); ++now_reg) {
			if ((*now_reg)[0] != '$') {//�û�Ծ�����Ѿ��洢���ڴ���
				inFlag = true;
				break;
			}
			else {
				reg = *now_reg;
			}
		}
		if (!inFlag) {//����û�Ծ���������ڴ��У���reg�е�var���������ڴ�
			storeVar(reg, *now_var);
		}
	}
}

string ObjectGenerator::getDesReg() {
	// A := B op C 
	
	/*
	* ����1��
	*      ��� B ������ֵ��ĳ���Ĵ��� Ri �У� RVALUE[Ri]��ֻ����B
	*      ���� B��A��ͬһ����ʶ�� ���� B������ֵ��ִ����Ԫʽ A:=B op C֮�󲻻�������
	*      ��ѡȡRiΪ����Ҫ�ļĴ���R
	*/
	string A = (*nowQuatenary).des;
	string B = (*nowQuatenary).arg1;
	string C = (*nowQuatenary).arg2;
	string res;
	if (isalpha(B[0])) { // B�Ǳ���
		for (auto var_reg = Avalue[B].begin(); var_reg != Avalue[B].end(); var_reg++) {
			if ((*var_reg)[0] == '$') { // B���ڼĴ�����
				if (Rvalue[*var_reg].size() == 1) { // ���ҸüĴ���ֻ������һ������
					// B��A��ͬ ���� B��ֵ֮��������
					if (B == A || (!(*nowQuatenary).info1.active && (*nowQuatenary).info1.next == -1)) { 
						// �øüĴ����д洢A����
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
	* ����2��
	*      �������δ����ļĴ����������ѡȡһ��RiΪҪ����ļĴ���
	*/
	if (freeReg.size() > 0) { // ���мĴ�����������0
		res = freeReg.back();
		freeReg.pop_back();
		Avalue[A].insert(res);
		Rvalue[res].insert(A);
		return res;
	}

	/*
	* ����3��
	*     ���ѷ���ļĴ�����ѡȡһ��RiΪ����Ҫ�ļ� ����R�����ʹ��Ri�������������� 
	*     ռ��Ri�ı�����ֵͬʱ����ڸñ��������浥Ԫ�� ���� �ڻ�������Ҫ����Զ�Ľ����Ż����õ��򲻻����õ���
	*/
	res = allocateReg();
	cleanReg(res);
	Avalue[A].insert(res);
	Rvalue[res].insert(A);
	return res;
}

string ObjectGenerator::getArgReg(string arg) {
	// ����
	if (isdigit(arg[0])) {
		string ret = allocateReg();
		objectCodes.push_back(string("addi ") + ret + " $zero " + arg);
		return ret;
	}

	for (set<string>::iterator iter = Avalue[arg].begin(); iter != Avalue[arg].end(); iter++) {
		if ((*iter)[0] == '$') {//��������Ѿ�������ĳ���Ĵ�����
			return *iter;//ֱ�ӷ��ظüĴ���
		}
	}

	//����ñ���û����ĳ���Ĵ�����
	string ret = allocateReg();
	for (auto now_var = Rvalue[ret].begin(); now_var != Rvalue[ret].end(); ++now_var) {
		//��ret�ļĴ����б���ı��������Ƕ������ٴ洢��ret��
		Avalue[*now_var].erase(ret);
		//���V�ĵ�ַ��������AVALUE[V]˵V��������R֮��������ط�������Ҫ���ɴ���ָ��
		if (Avalue[*now_var].size() > 0) {
			//pass
		}
		//���V�����ڴ�֮��ʹ�ã�����Ҫ���ɴ���ָ��
		else {
			bool storeFlag = true;
			vector<QuaternaryWithInfo>::iterator quat;
			for (quat = nowQuatenary; quat != (*nowBlock)->getQuaternarys().end(); ++quat) { // ������������ֱ���ֵ������
				if (quat->arg1 == *now_var || quat->arg2 == *now_var) {//���V�ڱ��������б�����
					storeFlag = true;
					break;
				}
				if (quat->des == *now_var) {//���V�ڱ��������б���ֵ
					storeFlag = false;
					break;
				}
			}
			if (quat == (*nowBlock)->getQuaternarys().end()) {//���V�ڱ���������δ�����ã���Ҳû�б���ֵ
				int index = nowBlock - block[nowFunc].begin();
				if (funcOUT[nowFunc][index].count(*now_var) == 1) {//����˱����ǳ���֮��Ļ�Ծ����
					storeFlag = true;
				}
				else {
					storeFlag = false;
				}
			}
			if (storeFlag) {//���ɴ���ָ��
				storeVar(ret, *now_var);
			}
		}
	}
	Rvalue[ret].clear();//���ret�Ĵ����б���ı���
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

	// �����еļĴ������ҵ�һ�����ʵ�
	for (auto now_reg = Rvalue.begin(); now_reg != Rvalue.end(); ++now_reg, ++regIndex) {
		// �����üĴ����洢���еı����������Ƴ��ñ����Ĵ���
		reg_name[regIndex] = (*now_reg).first;
		for (auto now_val = (*now_reg).second.begin(); now_val != (*now_reg).second.end(); ++now_val) {
			if (Avalue[*now_val].size() > 1) { // ������λ�ñ����˸ñ�����ֵ
				cost[regIndex] += 0.0;
			}
			else {
				for (auto quat = nowQuatenary; quat != (*nowBlock)->getQuaternarys().end(); ++quat) {
					if (*now_val == quat->arg1 || *now_val == quat->arg2) { // ֮���ֱ����ã��������еĴ���
						cost[regIndex] += double(quat - nowQuatenary) / 1.0;
						break;
					}
					else if (*now_val == quat->des) { // ֮���ֱ���ֵ������Ϊ0
						cost[regIndex] += 0.0;
						break;
					}
				}
			}
		}
	}

	// ѡȡ������С�ļĴ���
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
	* ��RVALUE[Ri]��ÿһ����M����� <M����A> ���� <M��A����C��������B����BҲ����RVALUE[Ri]��> ��ִ�У�
	*	1.���AVALUE[M]������M��������Ŀ�����	ST	Ri,	M
	*
	*	2.���M��B������M��C��ͬʱBҲ��RVALUE[Ri]�У�����AVALUE[M] = {M, Ri} �������� AVALUE[M] = {M}
	*
	*	3.ɾ��RVALUE[Ri]�е�M
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
			outputError(string("����") + nowQuatenary->arg1 + "������ǰδ��ֵ");
		}
		if (isalpha(nowQuatenary->arg2[0]) && Avalue[nowQuatenary->arg2].empty()) {
			outputError(string("����") + nowQuatenary->arg2 + "������ǰδ��ֵ");
		}
	}
	
	// ������ת��j
	if (nowQuatenary->op == "j") {
		objectCodes.push_back(nowQuatenary->op + " " + nowQuatenary->des);
	}
	// ����ת��j>= j<= j== j!= j> j<
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
		string pos1 = getArgReg(nowQuatenary->arg1);	// Դ������1
		string pos2 = getArgReg(nowQuatenary->arg2);	// Դ������2
		objectCodes.push_back(op + " " + pos1 + " " + pos2 + " " + nowQuatenary->des);
		if (!nowQuatenary->info1.active) {
			releaseVar(nowQuatenary->arg1);
		}
		if (!nowQuatenary->info2.active) {
			releaseVar(nowQuatenary->arg2);
		}
	}
	// ��Ӳ���par
	else if (nowQuatenary->op == "par") {
		par_list.push_back(pair<string, bool>(nowQuatenary->arg1, nowQuatenary->info1.active));
	}
	// ��������call
	else if (nowQuatenary->op == "call") {
		/*������ѹջ*/
		for (auto now_par = par_list.begin(); now_par != par_list.end(); ++now_par) {
			string pos = getArgReg(now_par->first);
			objectCodes.push_back(string("sw ") + pos + " " + to_string(top + 4 * (++arg_num + 1)) + "($sp)");
			if (!now_par->second) {
				releaseVar(now_par->first);
			}
		}
		/*����$sp*/
		objectCodes.push_back(string("sw $sp ") + to_string(top) + "($sp)");
		objectCodes.push_back(string("addi $sp $sp ") + to_string(top));

		/*��ת����Ӧ����*/
		objectCodes.push_back(string("jal ") + nowQuatenary->arg1);

		/*�ָ��ֳ�*/
		objectCodes.push_back(string("lw $sp 0($sp)"));
	}
	// ��������return
	else if (nowQuatenary->op == "return") {
		if (isdigit(nowQuatenary->arg1[0])) { // ����ֵΪ����
			objectCodes.push_back("addi $v0 $zero " + nowQuatenary->arg1);
		}
		else if (isalpha(nowQuatenary->arg1[0])) { // ����ֵΪ����
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
	// ��ȡ����get
	else if (nowQuatenary->op == "get") {
		//varOffset[nowQuatenary->arg1] = 4 * (par_num++ + 2);
		varOffset[nowQuatenary->des] = top;
		top += 4;
		Avalue[nowQuatenary->des].insert(nowQuatenary->des);
	}
	// ��ֵ=
	else if (nowQuatenary->op == "=") {
		//Avalue[nowQuatenary->q.des] = set<string>();
		string src1Pos;
		src1Pos = getArgReg(nowQuatenary->arg1);
		Rvalue[src1Pos].insert(nowQuatenary->des);
		Avalue[nowQuatenary->des].insert(src1Pos);
	}
	// ���� + - * /
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
	int arg_num = 0;//par��ʵ�θ���
	int par_num = 0;//get���βθ���
	list<pair<string, bool>> par_list;//���������õ���ʵ�μ�list<ʵ����,�Ƿ��Ծ>

	Avalue.clear();
	Rvalue.clear();
	set<string>& in = funcIN[nowFunc][nowBaseBlockIndex];
	for (set<string>::iterator in_var = in.begin(); in_var != in.end(); ++in_var) {
		Avalue[*in_var].insert(*in_var);
	}

	//��ʼ�����мĴ���
	freeReg.clear();
	for (int i = 0; i <= 7; i++) {
		freeReg.push_back(string("$s") + to_string(i));
	}

	// �����������
	objectCodes.push_back((*nowBlock)->getName() + ":");
	// �Ƿ��Ǻ�����ĵ�һ�������飨���̵������⣩
	if (nowBaseBlockIndex == 0) {
		if (nowFunc == "main") {
			top = 8;
		}
		else {
			objectCodes.push_back("sw $ra 4($sp)");//�ѷ��ص�ַѹջ
			top = 8;
		}
	}
	// ����ÿһ����Ԫʽ
	for (auto now_quat = (*nowBlock)->getQuaternarys().begin(); now_quat != (*nowBlock)->getQuaternarys().end(); ++now_quat) {
		nowQuatenary = now_quat;
		//����ǻ���������һ�����
		if (now_quat + 1 == (*nowBlock)->getQuaternarys().end()) {
			string op = (*now_quat).op;
			//������һ������ǿ�����䣬���Ƚ����ڻ�Ծ�������棬�ٽ�����ת(j,call,return)
			if (op[0] == 'j' || op == "call" || op == "return" || op == "get") {
				storeOutLiveVar(funcOUT[nowFunc][nowBaseBlockIndex]);
				generateCodeForQuatenary(nowBaseBlockIndex, arg_num, par_num, par_list);
			}
			//������һ����䲻�ǿ�����䣨�Ǹ�ֵ��䣩�����ȼ��㣬�ٽ����ڻ�Ծ��������
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
	for (auto now_block = now_func->second.begin(); now_block != now_func->second.end(); ++now_block) {//��ÿһ��������
		nowBlock = now_block;
		generateCodeForBaseBlocks(nowBlock - now_func->second.begin());
	}
}

void ObjectGenerator::generateCode() {
	// objectCodes.push_back("lui $sp,0x1001");
	objectCodes.push_back("j main");
	for (auto now_func = block.begin(); now_func != block.end(); ++now_func) {//��ÿһ��������
		generateCodeForFuncBlocks(now_func);
	}
	objectCodes.push_back("end:");
}

void ObjectGenerator::print(ostream& out) {
	// ��ӡ���к���
	for (auto fun = block.begin(); fun != block.end(); fun++) {
		vector<BaseBlock*> now_fun = fun->second;
		// ��ӡ�ú��������л�����
		for (auto now_blk = now_fun.begin(); now_blk != now_fun.end(); now_blk++) {
			// ��ӡ�������������Ԫʽ
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

