#pragma once

#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <algorithm>
#include <iomanip>
#include <fstream>
#include <map>
#include <set>
#include <vector>

using namespace std;

#define SYMBOL_NUM 100
#define PRODUCTION_NUM 100
#define MAX_NUM 200

struct symbol
{
	char name[20] = { 0 };
	int id = 0;
	bool is_terminal = false;
	bool operator==(const symbol& another)
	{
		if (is_terminal != another.is_terminal || id != another.id)
			return false;
		for (int i = 0; i < 20; i++)
			if (name[i] != another.name[i])
				return false;
		return true;
	}
	symbol& operator=(const symbol& another)
	{
		if (*this == another)
			return *this;
		is_terminal = another.is_terminal;
		id = another.id;
		for (int i = 0; i < 20; i++)
			name[i] = another.name[i];
		return *this;
	}
};

struct production
{
	int id = 0;
	char product[100] = { 0 };
	int left_id = 0;
	int right_id[20] = { 0 };
	int right_count = 0;
	bool operator==(const production& another)
	{
		if (id != another.id || left_id != another.left_id || right_count != another.right_count)
			return false;
		for (int i = 0; i < 100; i++)
			if(product[i] != another.product[i])
				return false;
		for (int j = 0; j < 20; j++)
			if(right_id[j] != another.right_id[j])
				return false;
		return true;
	}
	production& operator=(const production& another)
	{
		if (*this == another)
			return *this;
		id = another.id;
		left_id = another.left_id;
		right_count = another.right_count;
		for (int i = 0; i < 100; i++)
			product[i] = another.product[i];
		for (int j = 0; j < 20; j++)
			right_id[j] = another.right_id[j];
		return *this;
	}
};

struct f_set
{
	vector<int> id;
	int count;
	f_set(int i)
	{
		id.push_back(i);
		count = 1;
	}
	f_set() { count = 0; }
	void append_id(int add_id)
	{
		bool f = true;
		for(int i = 0; i < count; i++)
			if (id[i] == add_id)
			{
				f = false;
				break;
			}
		if (f)
		{
			id.push_back(add_id);
			count++;
		}
		return;
	}
	int find_id(int find_id)
	{
		for (int i = 0; i < count; i++)
			if (id[i] == find_id)
				return i;
		return -1;
	}
};

struct Item
{
	// ??????id??
	int item_id;
	// ????????id??
	int p_id;
	// ??????????0????????????????????right_count??????????
	int notpos;
	// ??????symbol??id
	int outlook;
	Item()
	{
		item_id = 0;
		p_id = 0;
		notpos = 0;
		outlook = 0;
	}
	Item(int id, int production_id, int one_notpos, int one_outlook)
	{
		item_id = id;
		p_id = production_id;
		notpos = one_notpos;
		outlook = one_outlook;
	}
	~Item() {}
	bool operator< (const Item& another) const
	{
		// ????????????????????????
		// set??????????  ??????a<b   ??????b<a  ????a==b
		if (item_id < another.item_id)
			return true;
		else
			return false;
	}
	bool operator==(const Item& another) const
	{
		if (item_id == another.item_id && p_id == another.p_id && notpos == another.notpos && outlook == another.outlook)
			return true;
		else
			return false;
	}
};

struct Item_set
{
	int item_set_id;
	vector<int> item_id;
	int item_count;
	Item_set()
	{
		item_set_id = 0;
		item_count = 0;
	}
	Item_set(int set_id, Item one_item)
	{
		item_set_id = set_id;
		item_id.push_back(one_item.item_id);
		item_count = 1;
	}
	Item_set(int set_id, int one_item_id)
	{
		item_set_id = set_id;
		item_id.push_back(one_item_id);
		item_count = 1;
	}
	~Item_set() {}

	bool item_is_in(int one_item_id)const
	{
		for (int i = 0; i != item_count; i++)
		{
			if (item_id[i] == one_item_id)
				return true;
		}
		return false;
	}
	bool operator< (const Item_set& another) const
	{
		// ????????????????????????
		// set??????????  ??????a<b   ??????b<a  ????a==b
		if (item_set_id < another.item_set_id)
			return true;
		else
			return false;
	}
	bool operator==(const Item_set& another) const
	{
		if (item_count != another.item_count)
			return false;
		else 
		{
			bool f = true;
			// ????this??????????item_id??????????????????????????????????????
			for (int i = 0; i < item_count; i++)
				if (!another.item_is_in(item_id[i]))
					return false;
			return true;
		}
	}

	void append_item_id(int one_item_id)
	{
		bool f = true;
		for (int i = 0; i < item_count; i++)
			if (item_id[i] == one_item_id)
				f = false;
		if (f)
		{
			item_id.push_back(one_item_id);
			item_count++;
		}
	}
};

class LR1
{
public:
	LR1();
	~LR1();

	// ????????????????????????
	int find_in_symbol(char* c);		/* ?????????????????? */
	int find_in_symbol(const char* c);
	void append_symbol(char* c);		/* ???????????? */
	void append_symbol(const char* c);
	int find_in_production(char* c);	/* ???????????????????? */
	int find_in_production(const char* c);
	void append_item(Item one);							// ??????????????????????
	void append_item(int p_id, int location, int outlook);	// ????????????
	int find_item(Item one);							//??????????????????????????????id????????????-1??????????
	int find_item(int p_id, int location, int outlook);	//??????????????????????????????id????????????-1
	void append_item_set(Item_set one);						// ??????????????
	int find_item_set(Item_set one_item_set);				// ????????????????????????????????id????????????-1

	// ??????
	void read(const char* file_name);	/* ?????????????? */

	// ????first??follow????
	void merge_fset_no_epsilon(f_set& one, f_set another);
	void get_one_prod_first(int p_id, int right_location);
	void get_first();
	f_set get_one_array_first(int arr[], int num);
	void get_follow();

	// ????DFA??????????
	void get_closure(Item_set& one);						// ??????????????
	void get_func_go();										// ????GO??????DFA??
	void get_func_table();									//????ACTION????GOTO??
	void analyze();

	// ????
	void print_grammer();
	void print_first_follow();
	void print_item_set();
	void print_func_go();
	void print_table();

	// ????
	int return_symbol(symbol* all_sym);
	int return_production(production* all_prod);
	void return_table(int** state, int** table);

private:
	// ????????????????
	symbol all_symbol[SYMBOL_NUM];
	int symbol_count;
	// ??????
	production all_production[PRODUCTION_NUM];
	int production_count;
	// first??	follow??
	f_set first[SYMBOL_NUM];
	f_set follow[SYMBOL_NUM];
	// ????
	int item_count;
	vector<Item> item;
	// ??????
	int item_set_count;
	vector<Item_set> item_set;

	int GO[MAX_NUM][SYMBOL_NUM];
	// table????????????????????????????????????????
	int analysis_table[MAX_NUM][SYMBOL_NUM];
	// 1????acc			2????????
	// 3????????????	4??????????
	// 5????????????	6??????????
	int analysis_state[MAX_NUM][SYMBOL_NUM];

};

LR1::LR1()
{
	symbol_count = 0;
	production_count = 0;
	item_count = 0;
	item_set_count = 0;
	for(int i = 0; i < MAX_NUM; i++)
		for (int j = 0; j < SYMBOL_NUM; j++)
		{
			GO[i][j] = -1;
			analysis_state[i][j] = -1;
			analysis_table[i][j] = -1;
		}
}
LR1::~LR1()
{
}
void LR1::read(const char* file_name)
{
	// ????????
	ifstream in(file_name);
	if (!in.is_open())
	{
		cout << "????????????????" << endl;
		exit(-1);
	}

	// ??????????????????
	char left[20] = { 0 };
	int left_id = 0;
	char c;
	while ((c = in.get()) != EOF)
	{
		char p[100] = { 0 };
		// production??id
		int p_id = production_count;
		production_count++;
		all_production[p_id].id = p_id;

		// ??????????????????NULL?????? '|' ??????????????????????left????
		if (left[0] == NULL)
		{
			// ????symbol
			left[0] = c;
			int i = 1;
			while ((c = in.get()) != ' ')
				left[i++] = c;
			left[i] = '\0';
			// ????????????" -> "
			c = in.get();//'-'
			c = in.get();//'>'
			c = in.get();//' '

			// ??????all_symbol
			append_symbol(left);
			left_id = find_in_symbol(left);
		}

		// copy????????p??????????" -> "
		strcpy(p, left);
		strcat(p, " ->");
		// production??left
		all_production[p_id].left_id = left_id;

		// ????????
		int right_n = 0;
		while (true)
		{
			char tem_right[20] = { 0 };
			int i = 0;
			// ????????????
			while ((c = in.get()) != ' ' && c != '\n' && c != '|')
				tem_right[i++] = c;
			tem_right[i] = '\0';
			// ??????????
			if (i != 0)
			{
				append_symbol(tem_right);
				all_production[p_id].right_id[right_n] = find_in_symbol(tem_right);
				all_production[p_id].right_count++;
				right_n++;

				strcat(p, " ");
				strcat(p, tem_right);
			}

			// ??????????????('\n')
			if (c == '\n')
			{
				// ????????????
				left_id = 0;
				for (int i = 0; i < 20; i++)
					left[i] = NULL;
				break;
			}
			// ??????????????('|')
			else if (c == '|')
			{
				// ????????????
				break;
			}
			// ????????????????????
			else continue;
		}
		strcpy(all_production[p_id].product, p);
	}

	append_symbol("#");
	in.close();
}

int LR1::find_in_symbol(char* c)
{
	for (int i = 0; i < symbol_count; i++)
	{
		if (strcmp(c, all_symbol[i].name) == 0)
			return i;
	}
	return -1;
}
int LR1::find_in_symbol(const char* c)
{
	for (int i = 0; i < symbol_count; i++)
	{
		if (strcmp(c, all_symbol[i].name) == 0)
			return i;
	}
	return -1;
}
void LR1::append_symbol(char* c)
{
	if (find_in_symbol(c) == -1)
	{
		int id = symbol_count;
		int len = int(strlen(c));
		// ????????symbol
		all_symbol[id].id = id;
		for (int i = 0; i < len; i++)
		{
			all_symbol[id].name[i] = c[i];
		}
		all_symbol[id].name[len] = '\0';
		if (c[0] == '<' && c[len - 1] == '>')
			all_symbol[id].is_terminal = false;
		else
			all_symbol[id].is_terminal = true;
		symbol_count++;
	}
}
void LR1::append_symbol(const char* c)
{
	if (find_in_symbol(c) == -1)
	{
		int id = symbol_count;
		int len = int(strlen(c));
		// ????????symbol
		all_symbol[id].id = id;
		for (int i = 0; i < len; i++)
		{
			all_symbol[id].name[i] = c[i];
		}
		all_symbol[id].name[len] = '\0';
		if (c[0] == '<' && c[len - 1] == '>')
			all_symbol[id].is_terminal = false;
		else
			all_symbol[id].is_terminal = true;
		symbol_count++;
	}
}
int LR1::find_in_production(char* c)
{
	for (int i = 0; i < production_count; i++)
	{
		if (strcmp(c, all_production[i].product) == 0)
			return i;
	}
	return -1;
}
int LR1::find_in_production(const char* c)
{
	for (int i = 0; i < production_count; i++)
	{
		if (strcmp(c, all_production[i].product) == 0)
			return i;
	}
	return -1;
}
void LR1::append_item(Item one)
{
	item_count++;
	item.push_back(one);
	return;
}
void LR1::append_item(int p_id, int notpos, int outlook)
{
	Item tem = Item(item_count, p_id, notpos, outlook);
	item_count++;
	item.push_back(tem);
	return;
}
int LR1::find_item(Item one)
{
	for (int i = 0; i < item_count; i++)
	{
		if (item[i] == one)
			return i;
	}
	return -1;
}
int LR1::find_item(int p_id, int notpos, int outlook)
{
	for (int i = 0; i < item_count; i++)
	{
		if (item[i].notpos == notpos && item[i].p_id == p_id && item[i].outlook == outlook)
			return i;
	}
	return -1;
}
void LR1::append_item_set(Item_set one)
{
	item_set_count++;
	item_set.push_back(one);
	return;
}
int LR1::find_item_set(Item_set one)
{
	for (int i = 0; i < item_set_count; i++)
	{
		Item_set tem = item_set[i];
		if (tem == one)
			return i;
	}
	return -1;
}

void LR1::merge_fset_no_epsilon(f_set &res, f_set one)
{
	for (int i = 0; i < one.count; i++) {
		if (one.id[i] == find_in_symbol("@"))
			continue;
		res.append_id(one.id[i]);
	}
}
void LR1::get_one_prod_first(int p_id, int right_location)
{
	int* right = all_production[p_id].right_id;
	int right_count = all_production[p_id].right_count;
	int right_id = right[right_location];
	int left_id = all_production[p_id].left_id;
	// ??Epsilon????????????1??
	if (right_location == 0 && right[0] == find_in_symbol("@") && right_count == 1)
	{
		first[left_id].append_id(find_in_symbol("@"));
		return;
	}
	// ????????
	else if (all_symbol[right_id].is_terminal)
	{
		first[left_id].append_id(right_id);
		return;
	}
	// ????????????????????????
	else if (right_location != right_count - 1)
	{
		merge_fset_no_epsilon(first[left_id], first[right_id]);
		if (first[right_id].find_id(find_in_symbol("@")) != -1)
			get_one_prod_first(p_id, right_location + 1);
		else
			return;
	}
	// ??????????????????????????????
	else
	{
		merge_fset_no_epsilon(first[left_id], first[right_id]);
		if (first[right_id].find_id(find_in_symbol("@")) != -1)
			first[left_id].append_id(find_in_symbol("@"));
		return;
	}
}
void LR1::get_first()
{
	int first_length[SYMBOL_NUM] = { 0 };
	// ????????first??????#??
	for (int i = 0; i < symbol_count; i++)
	{
		if (all_symbol[i].is_terminal)
		{
			first[i] = f_set(i);
			first_length[i] = 1;
		}
	}

	// ??????????????first??????1??????0??epsilon??
	bool done = false;
	while (!done)
	{
		// ????????????????
		for (int i = 0; i < production_count; i++)
			get_one_prod_first(i, 0);

		// ??????????????????????????????????first????????????????#??
		done = true;
		for (int i = 0; i < symbol_count; i++)
		{
			if (first_length[i] != first[i].count)
			{
				first_length[i] = first[i].count;
				done = false;
			}
		}
	}
}
f_set LR1::get_one_array_first(int arr[], int num)
{
	f_set res;
	if (num == 1)
		return first[arr[0]];
	else
	{
		// ??????????????????id????
		for (int i = 0; i < num; i++)
		{
			// ????????????????????????first????????@????????????????res??
			int symbol_id = arr[i];
			merge_fset_no_epsilon(res, first[symbol_id]);
			// ????????????????
			if (all_symbol[symbol_id].is_terminal || first[symbol_id].find_id(find_in_symbol("@")) == -1)
				return res;
		}
		// ??????????????????????????Epsilon
		res.append_id(find_in_symbol("@"));
		return res;
	}
}
void LR1::get_follow()
{
	// follow????????????Epsilon??????#
	bool done = false;
	int follow_num[SYMBOL_NUM] = { 0 };
	follow[0] = f_set(find_in_symbol("#"));
	follow_num[0] = 1;
	while (!done)
	{
		// ????????????????????????
		for (int i = 0; i < production_count; i++)
		{
			// ??????????
			production now_p = all_production[i];
			int* right = now_p.right_id;
			int right_count = now_p.right_count;
			int left_id = now_p.left_id;
			for (int j = 0; j < right_count; j++)
			{
				// ??????????????
				int right_id = right[j];
				// ??????????????????????????
				if (j == right_count - 1 && !all_symbol[right_id].is_terminal)
					merge_fset_no_epsilon(follow[right_id], follow[left_id]);
				// ??????????????????????????????
				else if (!all_symbol[right_id].is_terminal)
				{
					int beta[SYMBOL_NUM] = { 0 };
					int num = 0;
					for (int k = j + 1; k < right_count; k++)
					{
						beta[num++] = right[k];
					}
					f_set beta_first = get_one_array_first(beta, num);

					// ??beta??first??????????@????????right_id??follow??????
					merge_fset_no_epsilon(follow[right_id], beta_first);

					// ??????@????????????follow??????????????
					if (beta_first.find_id(find_in_symbol("@")) != -1)
						merge_fset_no_epsilon(follow[right_id], follow[left_id]);
				}
			}
		}
		// ??????????????????????#??
		done = true;
		for (int i = 0; i < symbol_count; i++)
		{
			if (follow_num[i] != follow[i].count)
			{
				follow_num[i] = follow[i].count;
				done = false;
			}
		}
	}
	return;
}

void LR1::get_closure(Item_set& one)
{
	// ????item_count??????????????????
	// ??????????????????????
	for (int i = 0; i < one.item_count; i++)
	{
		// ????????????
		int now_item_id = one.item_id[i];
		Item now = item[now_item_id];
		production now_p = all_production[now.p_id];
		int next_symbol_id = now_p.right_id[now.notpos];
		// ????????????????????????????????????????????????????????????
		if (now.notpos != now_p.right_count && !all_symbol[next_symbol_id].is_terminal)
		{
			// ??????????????????
			int B = next_symbol_id;

			// first??
			int beta_a[SYMBOL_NUM] = { 0 };
			int num = 0;
			for (int i = now.notpos + 1; i < now_p.right_count; i++)
				beta_a[num++] = now_p.right_id[i];
			beta_a[num++] = now.outlook;
			f_set first_beta_a = get_one_array_first(beta_a, num);

			// ??????B????????????????????????????????????????????????????????
			for (int j = 0; j < production_count; j++)
			{
				production tem = all_production[j];
				if (tem.left_id == B)
					// ????first????????????
					for (int k = 0; k < first_beta_a.count; k++)
					{
						// ??????????????????????????????????
						if (find_item(tem.id, 0, first_beta_a.id[k]) == -1)
						{
							// ????count????????????id
							one.append_item_id(item_count);
							// ??????????????
							append_item(tem.id, 0, first_beta_a.id[k]);
						}
						// ??????????????????????????????????
						else
						{
							int n = find_item(tem.id, 0, first_beta_a.id[k]);
							one.append_item_id(n);
						}
					}
			}
		}
	}
}
void LR1::get_func_go()
{
	bool done = false;
	// ??????????????????id??1????
	Item start(0, all_production[0].id, 0, find_in_symbol("#"));
	append_item(start);

	Item_set start_set(0, start);
	get_closure(start_set);
	append_item_set(start_set);

	// ??????????????????????
	for (int now_item_set_id = 0; now_item_set_id < item_set_count; now_item_set_id++)
	{
		Item_set now_item_set = item_set[now_item_set_id];

		// ??????????????????????????????
		// ????#????????????????Epsilon??0 ?????????????????????????????????????????? 1 ????
		for (int symbol_id = 1; symbol_id < symbol_count; symbol_id++)
		{
			int X = symbol_id;

			// GO(now_item_set_id, X)
			// ??????????????????X??symbol_id????????????
			Item_set obj_item_set;
			obj_item_set.item_set_id = item_set_count;

			// ??????????????????
			for (int j = 0; j < now_item_set.item_count; j++)
			{
				int now_item_id = now_item_set.item_id[j];
				Item now_item = item[now_item_id];
				production now_p = all_production[now_item.p_id];
				// ????????????????????????????X??????????????????????????????
				if (now_item.notpos < now_p.right_count && now_p.right_id[now_item.notpos] == X)
				{
					int n = find_item(now_item.p_id, now_item.notpos + 1, now_item.outlook);
					if (n == -1)
					{
						n = item_count;
						append_item(now_item.p_id, now_item.notpos + 1, now_item.outlook);
					}
					obj_item_set.append_item_id(n);
				}
			}

			// ??????????????????????????????????????clouser??
			get_closure(obj_item_set);

			// ??????????????????GO????
			if (find_item_set(obj_item_set) == -1 && obj_item_set.item_count != 0)
				append_item_set(obj_item_set);
			GO[now_item_set_id][X] = find_item_set(obj_item_set);
		}
	}
	return;
}
void LR1::get_func_table()
{
	// 1????acc			-1????????
	// 3????????????	4??????????
	// 5????????????	6??????????

	// ????????????????
	for (int now_item_set_id = 0; now_item_set_id < item_set_count; now_item_set_id++)
	{
		Item_set now_item_set = item_set[now_item_set_id];

		// ????????????????????????????????????????????
		bool has_epsilon = false;
		int epsilon_state = -1;
		// ??????????????
		for (int j = 0; j < now_item_set.item_count; j++)
		{
			int now_item_id = now_item_set.item_id[j];
			Item now_item = item[now_item_id];
			production now_p = all_production[now_item.p_id];
			// ??????????????
			if (now_p.right_count == 1 && now_p.right_id[0] == find_in_symbol("@"))
			{
				// ????6
				if (now_item.notpos == 1)
				{
					if (analysis_state[now_item_set_id][now_item.outlook] != 6 && analysis_state[now_item_set_id][now_item.outlook] != -1)
					{
						cout << "??????????????????" << endl;
					}
					analysis_state[now_item_set_id][now_item.outlook] = 6;
					analysis_table[now_item_set_id][now_item.outlook] = now_p.id;
				}
				// ????4
				else
				{
					has_epsilon = true;
					epsilon_state = GO[now_item_set_id][find_in_symbol("@")];
					continue;
				}
			}
			// ??????????1
			else if (now_item.p_id == 0 && now_item.notpos == 1 && now_item.outlook == find_in_symbol("#"))
			{
				if (analysis_state[now_item_set_id][now_item.outlook] != 1 && analysis_state[now_item_set_id][now_item.outlook] != -1)
				{
					cout << "??????????????????" << endl;
				}
				analysis_state[now_item_set_id][now_item.outlook] = 1;
				analysis_table[now_item_set_id][now_item.outlook] = 0;
			}
			// ??????????????5
			else if (now_item.notpos == now_p.right_count)
			{
				if (analysis_state[now_item_set_id][now_item.outlook] != 5 && analysis_state[now_item_set_id][now_item.outlook] != -1)
				{
					cout << "??????????????????" << endl;
				}
				analysis_state[now_item_set_id][now_item.outlook] = 5;
				analysis_table[now_item_set_id][now_item.outlook] = now_p.id;
			}
			// ??????????????3
			else
			{
				int next_symbol_id = now_p.right_id[now_item.notpos];
				if (analysis_state[now_item_set_id][next_symbol_id] != 3 && analysis_state[now_item_set_id][next_symbol_id] != -1)
				{
					cout << "??????????????????" << endl;
				}
				analysis_state[now_item_set_id][next_symbol_id] = 3;
				analysis_table[now_item_set_id][next_symbol_id] = GO[now_item_set_id][next_symbol_id];
			}
		}
		// ????4
		if (has_epsilon)
		{
			for (int j = 0; j < symbol_count; j++)
			{
				if (analysis_state[now_item_set_id][j] == -1)
				{
					analysis_state[now_item_set_id][j] = 4;
					analysis_table[now_item_set_id][j] = epsilon_state;
				}
			}
		}
	}
	return;
}
void LR1::analyze()
{
	read("in_grammer.txt");
	get_first();
	get_follow();
	get_func_go();
	get_func_table();

	ofstream out("out_grammer.txt");
	if (!out.is_open())
	{
		cout << "out_grammer.txt??????????????????" << endl;
		return;
	}
	out << "**************************??????**********************************" << endl;
	for (int i = 0; i < production_count; i++)
	{
		production tem = all_production[i];
		out << "?? " << tem.id << " ????????????";
		out << all_symbol[tem.left_id].name;
		out << " ->";
		for (int j = 0; j < tem.right_count; j++)
			out << " " << all_symbol[tem.right_id[j]].name;
		out << endl;
	}
	out << "**************************firstw????***************************" << endl;
	for (int i = 0; i < symbol_count; i++)
	{
		out << setw(20) << all_symbol[i].name << " : ";
		for (int j = 0; j < first[i].count; j++)
			out << setw(20) << all_symbol[first[i].id[j]].name;
		out << endl;
	}
	out << "*************************follow????***************************" << endl;
	for (int i = 0; i < symbol_count; i++)
		if (!all_symbol[i].is_terminal)
		{
			out << setw(20) << all_symbol[i].name << " : ";
			for (int j = 0; j < follow[i].count; j++)
				out << setw(20) << all_symbol[follow[i].id[j]].name;
			out << endl;
		}
	out.close();

	out.open("out_item.txt");
	if (!out.is_open())
	{
		cout << "out_item.txt??????????????????" << endl;
		return;
	}
	out << "*****************************item_set********************************" << endl;
	for (int i = 0; i < item_set_count; i++)
	{
		out << "?????? " << i << " : " << endl;
		Item_set now = item_set[i];
		// ??????????????
		for (int j = 0; j < now.item_count; j++)
		{
			out << "\t";
			Item now_i = item[now.item_id[j]];
			production now_p = all_production[now_i.p_id];
			out << all_symbol[now_p.left_id].name << " -> ";
			int k = 0;
			for (; k < now_p.right_count; k++)
				if (k == now_i.notpos)
					out << "?? " << all_symbol[now_p.right_id[k]].name << " ";
				else
					out << all_symbol[now_p.right_id[k]].name << " ";
			if (now_i.notpos == now_p.right_count)
				out << "?? ";
			out << ", " << all_symbol[now_i.outlook].name;
			out << endl;
		}
	}
	out.close();
}


void LR1::print_grammer()
{
	for (int i = 0; i < production_count; i++)
	{
		production tem = all_production[i];
		cout << "?? " << tem.id << " ????????????";
		cout << all_symbol[tem.left_id].name;
		cout << " ->";
		for (int j = 0; j < tem.right_count; j++)
			cout << " " << all_symbol[tem.right_id[j]].name;
		cout << endl;

		//cout << "?? " << tem.id << " ????????????" << tem.product << endl;;
	}
}
void LR1::print_first_follow()
{
	ofstream out("first_follow.txt");
	if (!out.is_open())
	{
		cout << "first??????????????????????????" << endl;
		return;
	}
	out << "**************************firstw????***************************" << endl;
	for (int i = 0; i < symbol_count; i++)
	{
		out << setw(20) << all_symbol[i].name << " : ";
		for (int j = 0; j < first[i].count; j++)
			out << setw(20) << all_symbol[first[i].id[j]].name;
		out << endl;
	}
	out << "*************************follow????***************************" << endl;
	for (int i = 0; i < symbol_count; i++)
		if (!all_symbol[i].is_terminal)
		{
			out << setw(20) << all_symbol[i].name << " : ";
			for (int j = 0; j < follow[i].count; j++)
				out << setw(20) << all_symbol[follow[i].id[j]].name;
			out << endl;
		}
	out.close();
}
void LR1::print_item_set()
{
	for (int i = 0; i < item_set_count; i++)
	{
		cout << "?????? " << i << " : " << endl;
		Item_set now = item_set[i];
		// ??????????????
		for (int j = 0; j < now.item_count; j++)
		{
			cout << "\t";
			Item now_i = item[now.item_id[j]];
			production now_p = all_production[now_i.p_id];
			cout << all_symbol[now_p.left_id].name << " -> ";
			int k = 0;
			for (; k < now_p.right_count; k++)
				if (k == now_i.notpos)
					cout << "?? " << all_symbol[now_p.right_id[k]].name << " ";
				else
					cout << all_symbol[now_p.right_id[k]].name << " ";
			if (now_i.notpos == now_p.right_count)
				cout << "?? ";
			cout << ", " << all_symbol[now_i.outlook].name;
			cout << endl;
		}
	}
}
void LR1::print_func_go()
{
	cout << "           ";
	for (int j = 0; j < symbol_count; j++)
	{
		if (all_symbol[j].is_terminal)
			cout << setw(6) << all_symbol[j].name;
	}
	for (int j = 0; j < symbol_count; j++)
	{
		if (!all_symbol[j].is_terminal)
			cout << setw(6) << all_symbol[j].name;
	}
	cout << endl;
	for (int i = 0; i < item_set_count; i++)
	{
		cout << "??????" << setw(2) << i << " : ";
		for (int j = 0; j < symbol_count; j++)
		{
			if (all_symbol[j].is_terminal)
				if (GO[i][j] != -1)
					cout << setw(6) << GO[i][j];
				else
					cout << "      ";
		}
		for (int j = 0; j < symbol_count; j++)
		{
			if (!all_symbol[j].is_terminal)
				if (GO[i][j] != -1)
					cout << setw(6) << GO[i][j];
				else
					cout << "      ";
		}
		cout << endl;
	}
}
void LR1::print_table()
{
	cout << "           ";
	for (int j = 0; j < symbol_count; j++)
	{
		if (all_symbol[j].is_terminal)
			cout << setw(5) << all_symbol[j].name << "    ";
	}
	for (int j = 0; j < symbol_count; j++)
	{
		if (!all_symbol[j].is_terminal)
			cout << setw(5) << all_symbol[j].name << "    ";
	}
	cout << endl;

	for (int i = 0; i < item_set_count; i++)
	{
		cout << "??????" << setw(2) << i << " : ";
		for (int j = 0; j < symbol_count; j++)
		{
			if (all_symbol[j].is_terminal)
			switch (analysis_state[i][j])
			{
				case 1:
					cout << "???? " << setw(2) << analysis_table[i][j] << "  ";
					break;
				case 3:
					cout << "???? " << setw(2) << analysis_table[i][j] << "  ";
					break;
				case 4:
					cout << "???? " << setw(2) << analysis_table[i][j] << "  ";
					break;
				case 5:
					cout << "???? " << setw(2) << analysis_table[i][j] << "  ";
					break;
				case 6:
					cout << "???? " << setw(2) << analysis_table[i][j] << "  ";
					break;
				default:
					cout << "   "<< setw(2) << analysis_table[i][j]<<"    ";
					break;
			}
		}
		for (int j = 0; j < symbol_count; j++)
		{
			if (!all_symbol[j].is_terminal)
				switch (analysis_state[i][j])
				{
					case 1:
						cout << "???? " << setw(2) << analysis_table[i][j] << "  ";
						break;
					case 3:
						cout << "???? " << setw(2) << analysis_table[i][j] << "  ";
						break;
					case 4:
						cout << "???? " << setw(2) << analysis_table[i][j] << "  ";
						break;
					case 5:
						cout << "???? " << setw(2) << analysis_table[i][j] << "  ";
						break;
					case 6:
						cout << "???? " << setw(2) << analysis_table[i][j] << "  ";
						break;
					default:
						cout << "   " << setw(2) << analysis_table[i][j] << "    ";
						break;
				}
		}
		cout << endl;
	}

}

int LR1::return_symbol(symbol* all_sym)
{
	for (int i = 0; i < SYMBOL_NUM; i++)
		all_sym[i] = all_symbol[i];
	return symbol_count;
}
int LR1::return_production(production* all_prod)
{
	for (int i = 0; i < PRODUCTION_NUM; i++)
		all_prod[i] = all_production[i];
	return production_count;
}
void LR1::return_table(int** state, int** table)
{
	for (int i = 0; i < MAX_NUM; i++)
	{
		for (int j = 0; j < SYMBOL_NUM; j++)
		{
			state[i][j] = analysis_state[i][j];
			table[i][j] = analysis_table[i][j];
		}
	}
}
