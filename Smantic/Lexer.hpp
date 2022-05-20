#pragma once
#include "LR1.hpp"
#include <fstream>
#include <iostream>
#include <stdlib.h>
#include <string>
#include <map>
#include <vector>
#define KEY_NUM 6
#define ALL_NUM 27
#define SPECIAL_NUM 21
#define SINGLE_NUM 6
using namespace std;
string key[KEY_NUM] = { "int","void","if","else","while","return" };//1~KEY_NUM
char singleC[SINGLE_NUM + 8] = { ';' ,',' ,'(', ')' ,'{' , '}', '#', '+', '-', '*', '/', '=', '>', '<' };

typedef struct WT {
	string word = "";
	char type_name[20] = { 0 };
	int type = 0;
	//�ؼ���1-6����ʶ��7(KEY_NUM+1)����ֵ8(KEY_NUM+2)�����9����ֵ��21�����10���ָ���11��
	//������12��������13���������14���Ҵ�����15��������16
	WT* next = NULL;
	int line = 0;
}WT;
class Lexer
{
public:
	WT* head;
	WT* tail;                   //����
	map<string, int> mapKey;
	map<char, int> mapSingle;   //map
	ifstream in;
	ofstream out;               //�ļ���д
	string buffer;
	char cha;
	string word;
	int line;
	int index;
	bool flag;                  //�Ƿ�Ϊע��
	char GetChar();             //����1�ַ�����index++
	int RemoveBlank();			//�����հ�
	bool IfLetter();            //�Ƿ�Ϊ��ĸ
	void GetWord();             //���µ���
	bool Ifdigit();             //�Ƿ�Ϊ����
	int IfKey();                //�Ƿ�Ϊ�ؼ���
	void Retract();             //����
	void Insert(int type);      //��������
	int IfSingle();             //�Ƿ�Ϊ������
	void Scanner();             //ɨ��
	void wt_type2name();		//����type����ս��������
	WT* get_head();
	Lexer() {
		flag = 0;//��ʼ��
		line = 1;
		head = new WT;
		tail = head;
		head->type = -1;
		head->word = "";
		head->next = NULL;
		head->line = -1;
		for (int i = 0; i < KEY_NUM; i++) mapKey.insert(pair <string, int>(key[i], i + 1));
		for (int i = 0; i < 14; i++)  mapSingle.insert(pair < char, int >(singleC[i], i + 10));
		in.open("in_source.txt");
		out.open("out_lexer.txt");
	}
	void Scan();
	void Print();
	~Lexer() {
		in.close();
		out.close();
		//��������ûд
	}
};
char Lexer::GetChar() {
	cha = buffer[index++];
	return cha;
}
void Lexer::GetWord() {
	word += cha;
}
//ȥ���հ�
int Lexer::RemoveBlank() {
	if (cha == ' ' || cha == '\t') {
		while (cha == ' ' || cha == '\t') GetChar();
	}
	if (cha == '\n') {
		cha = '\0';
		return 1;
	}
	return 0;
}
//��ĸ��
bool Lexer::IfLetter() {
	if ((cha >= 'a' && cha <= 'z') || (cha >= 'A' && cha <= 'Z')) return true;
	else return false;
}
//���֣�
bool Lexer::Ifdigit() {
	if (cha >= '0' && cha <= '9') return true;
	else return false;
}
//�ؼ��֣�
int Lexer::IfKey() {
	map<string, int>::iterator iter;
	iter = mapKey.find(word);
	if (iter == mapKey.end()) return KEY_NUM + 1;//���ǹؼ����򷵻ر�ʶ��
	else return iter->second;
}
// ����һ���ַ�
void Lexer::Retract() {
	index--;
	cha = buffer[index];
}
void Lexer::Insert(int type) {//����β��
	WT* newWT = new WT;
	newWT->word = word;
	newWT->type = type;
	newWT->next = NULL;
	newWT->line = line;
	tail->next = newWT;
	tail = tail->next;
}
//+17 -18 *19 /20 =21 >22 <23
//==24 >=25 <=26 !=27
int Lexer::IfSingle() {
	map<char, int>::iterator iter;
	iter = mapSingle.find(cha);
	word += cha;
	if (iter == mapSingle.end()) {//�������map��
		if (cha == '!') {//����!=
			GetChar();
			if (cha == '=') return ALL_NUM;
		}
		return -999;//ɶҲ����
	}
	else {
		GetChar();
		if (cha == '=' && iter->second >= SPECIAL_NUM) {//�ڶ����޶�= > <
			word += cha;
			return iter->second + 3;//eg: 21+3=24 ==
		}
		else {
			Retract();
			return iter->second;
		}
	}
}
void Lexer::Scanner() {
	if (flag == 1) {//�����ע����
		while (true) {
			while (cha != '*') GetChar();
			if (buffer[index] == '/') {
				flag = 0;
				GetChar();
				break;
			}
		}
	}
	while (cha != '\n') {//δ������ĩβ
		GetChar();
		if (RemoveBlank()) return;
		if (cha == '/') {//�ж�ע��
			GetChar();
			if (cha == '/') return;
			else if (cha == '*') {
				flag = 1;
				GetChar();
				while (!(cha == '*' && buffer[index] == '/') && cha != '\n') GetChar();
				if (cha == '\n') {
					cha = '\0';
					return;
				}
				else {
					flag = 0;
					GetChar();
					continue;
				}
			}
			else {
				if (cha == '\n') return;//?
				Retract();
				Retract();
				Insert(IfSingle());
				index++;
			}
		}
		else if (IfLetter()) {//��ĸ����ĸ|���֣�*
			while (IfLetter() || Ifdigit()) {
				GetWord();
				GetChar();
			}
			Retract();
			Insert(IfKey());
		}
		else if (Ifdigit()) {//���� ����*
			while (Ifdigit()) {
				GetWord();
				GetChar();
			}
			Retract();
			Insert(KEY_NUM + 2);
		}
		else Insert(IfSingle());
		word = "";
	}
	cha = '\0';
}
void Lexer::Print() {
	WT* wt = head->next;
	while (wt) {
		//cout << wt->type << "\t" << wt->word << endl;
		out << wt->type << "\t" << wt->word << "\t" << wt->line << endl;
		wt = wt->next;
	}
}
void Lexer::Scan() {
	if (!in.is_open()) {
		cout << "Error opening file";
		exit(1);
	}
	while (!in.eof()) {
		getline(in, buffer);
		buffer += 10;//ĩβ��\n
		index = 0;
		Scanner();
		line++;
	}
	Print();
}
void Lexer::wt_type2name()
{
	//�ؼ���1-6����ʶ��7(KEY_NUM+1)����ֵ8(KEY_NUM+2)�����9����ֵ��21�����10���ָ���11��
	//������12��������13���������14���Ҵ�����15��������16
	WT* wt = head->next;
	while (wt) {
		char tem_name[20];
		switch (wt->type)
		{
			case 1:
				strcpy(tem_name, "int");
				break;
			case 2:
				strcpy(tem_name, "void");
				break;
			case 3:
				strcpy(tem_name, "if");
				break;
			case 4:
				strcpy(tem_name, "else");
				break;
			case 5:
				strcpy(tem_name, "while");
				break;
			case 6:
				strcpy(tem_name, "return");
				break;
			case 7:
				strcpy(tem_name, "ID");
				break;
			case 8:
				strcpy(tem_name, "NUM");
				break;
			case 10:
				strcpy(tem_name, ";");
				break;
			case 11:
				strcpy(tem_name, ",");
				break;
			case 12:
				strcpy(tem_name, "(");
				break;
			case 13:
				strcpy(tem_name, ")");
				break;
			case 14:
				strcpy(tem_name, "{");
				break;
			case 15:
				strcpy(tem_name, "}");
				break;
			case 16:
				strcpy(tem_name, "#");
				break;
			case 17:
				strcpy(tem_name, "+");
				break;
			case 18:
				strcpy(tem_name, "-");
				break;
			case 19:
				strcpy(tem_name, "*");
				break;
			case 20:
				strcpy(tem_name, "/");
				break;
			case 21:
				strcpy(tem_name, "=");
				break;
			case 22:
				strcpy(tem_name, ">");
				break;
			case 23:
				strcpy(tem_name, "<");
				break;
			case 24:
				strcpy(tem_name, "==");
				break;
			case 25:
				strcpy(tem_name, ">=");
				break;
			case 26:
				strcpy(tem_name, "<=");
				break;
			case 27:
				strcpy(tem_name, "!=");
				break;
			default:
				strcpy(tem_name, "wrong");
				cout << "�޷�ʶ����ַ�" << endl;
				break;
		}
		strcpy(wt->type_name, tem_name);
		wt = wt->next;
	}
}
WT* Lexer::get_head()
{
	return head;
}

/*
* 1: int
* 2: void
* 3: if
* 4: else
* 5: while
* 6: return
* 7: ID
* 8: NUM
* 9: δʹ��
* 10: ;
* 11: ,
* 12: (
* 13: )
* 14: {
* 15: }
* 16: #
* 17: +
* 18: -
* 19: *
* 20: /
* 21: =
* 22: >
* 23: <
* 24: ==
* 25: >=
* 26: <=
* 27: !=
*/
