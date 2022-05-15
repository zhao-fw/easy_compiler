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
	// 项目的id号
	int item_id;
	// 产生式的id号
	int p_id;
	// ·的位置，0表示在开头，产生式的right_count表示在末尾
	int notpos;
	// 展望的symbol的id
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
		// 重载运算符变量必须都用上
		// set的比较方式  先判断a<b   在判断b<a  否则a==b
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
		// 重载运算符变量必须都用上
		// set的比较方式  先判断a<b   在判断b<a  否则a==b
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
			// 对于this中的所有的item_id，如果都能找到，则相同（并且大小相同）
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

	// 针对数组的查找、添加操作
	int find_in_symbol(char* c);		/* 在已有符号表中查找 */
	int find_in_symbol(const char* c);
	void append_symbol(char* c);		/* 添加新的符号 */
	void append_symbol(const char* c);
	int find_in_production(char* c);	/* 在已有的产生式中寻找 */
	int find_in_production(const char* c);
	void append_item(Item one);							// 产生某个项目，基本没用
	void append_item(int p_id, int location, int outlook);	// 产生某个项目
	int find_item(Item one);							//判断某个项目已经存在，找到返回id号，否则返回-1，基本没用
	int find_item(int p_id, int location, int outlook);	//判断某个项目已经存在，找到返回id号，否则返回-1
	void append_item_set(Item_set one);						// 插入新的项目集
	int find_item_set(Item_set one_item_set);				// 判断项目集是否已经存在，找到返回id号，否则返回-1

	// 读文法
	void read(const char* file_name);	/* 读文法并初始化 */

	// 计算first、follow集合
	void merge_fset_no_epsilon(f_set& one, f_set another);
	void get_one_prod_first(int p_id, int right_location);
	void get_first();
	f_set get_one_array_first(int arr[], int num);
	void get_follow();

	// 计算DFA，和两张表
	void get_closure(Item_set& one);						// 得到某个项目集
	void get_func_go();										// 得到GO函数（DFA）
	void get_func_table();									//得到ACTION表和GOTO表
	void analyze();

	// 打印
	void print_grammer();
	void print_first_follow();
	void print_item_set();
	void print_func_go();
	void print_table();

	// 返回
	int return_symbol(symbol* all_sym);
	int return_production(production* all_prod);
	void return_table(int** state, int** table);

private:
	// 终结符和非终结符
	symbol all_symbol[SYMBOL_NUM];
	int symbol_count;
	// 产生式
	production all_production[PRODUCTION_NUM];
	int production_count;
	// first集	follow集
	f_set first[SYMBOL_NUM];
	f_set follow[SYMBOL_NUM];
	// 项目
	int item_count;
	vector<Item> item;
	// 项目集
	int item_set_count;
	vector<Item_set> item_set;

	int GO[MAX_NUM][SYMBOL_NUM];
	// table中是移进时转移的状态，规约时使用的产生式
	int analysis_table[MAX_NUM][SYMBOL_NUM];
	// 1表示acc			2表示错误
	// 3表示非空移进	4表示空移进
	// 5表示非空规约	6表示空规约
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
	// 打开文件
	ifstream in(file_name);
	if (!in.is_open())
	{
		cout << "文件打开失败！！" << endl;
		exit(-1);
	}

	// 可能需要记录的东西
	char left[20] = { 0 };
	int left_id = 0;
	char c;
	while ((c = in.get()) != EOF)
	{
		char p[100] = { 0 };
		// production的id
		int p_id = production_count;
		production_count++;
		all_production[p_id].id = p_id;

		// 读入左边，如果不为NULL表示为 '|' 之后的内容，即用上次的left内容
		if (left[0] == NULL)
		{
			// 读入symbol
			left[0] = c;
			int i = 1;
			while ((c = in.get()) != ' ')
				left[i++] = c;
			left[i] = '\0';
			// 把多余的读掉" -> "
			c = in.get();//'-'
			c = in.get();//'>'
			c = in.get();//' '

			// 添加到all_symbol
			append_symbol(left);
			left_id = find_in_symbol(left);
		}

		// copy到产生式p中，再加上" -> "
		strcpy(p, left);
		strcat(p, " ->");
		// production的left
		all_production[p_id].left_id = left_id;

		// 读入右边
		int right_n = 0;
		while (true)
		{
			char tem_right[20] = { 0 };
			int i = 0;
			// 读入一个符号
			while ((c = in.get()) != ' ' && c != '\n' && c != '|')
				tem_right[i++] = c;
			tem_right[i] = '\0';
			// 读入不为空
			if (i != 0)
			{
				append_symbol(tem_right);
				all_production[p_id].right_id[right_n] = find_in_symbol(tem_right);
				all_production[p_id].right_count++;
				right_n++;

				strcat(p, " ");
				strcat(p, tem_right);
			}

			// 产生式读取结束('\n')
			if (c == '\n')
			{
				// 需要清空左边
				left_id = 0;
				for (int i = 0; i < 20; i++)
					left[i] = NULL;
				break;
			}
			// 产生式读取结束('|')
			else if (c == '|')
			{
				// 直接退出就好
				break;
			}
			// 也可能是空格输入多了
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
		// 添加新的symbol
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
		// 添加新的symbol
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
	// 是Epsilon（右边长度为1）
	if (right_location == 0 && right[0] == find_in_symbol("@") && right_count == 1)
	{
		first[left_id].append_id(find_in_symbol("@"));
		return;
	}
	// 是终结符
	else if (all_symbol[right_id].is_terminal)
	{
		first[left_id].append_id(right_id);
		return;
	}
	// 是非终结符，不为最后一个
	else if (right_location != right_count - 1)
	{
		merge_fset_no_epsilon(first[left_id], first[right_id]);
		if (first[right_id].find_id(find_in_symbol("@")) != -1)
			get_one_prod_first(p_id, right_location + 1);
		else
			return;
	}
	// 是非终结符，是产生式的最后一个
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
	// 终结符的first集（无#）
	for (int i = 0; i < symbol_count; i++)
	{
		if (all_symbol[i].is_terminal)
		{
			first[i] = f_set(i);
			first_length[i] = 1;
		}
	}

	// 计算非终结符的first集（从1开始，0为epsilon）
	bool done = false;
	while (!done)
	{
		// 将每个产生式判断
		for (int i = 0; i < production_count; i++)
			get_one_prod_first(i, 0);

		// 判断是否还需要继续（如果一遍中没有first集合的变化）（无#）
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
		// 对于数组中的每一个id标号
		for (int i = 0; i < num; i++)
		{
			// 对于其中的一个标号，将其first集合除了@外的元素，加入到res中
			int symbol_id = arr[i];
			merge_fset_no_epsilon(res, first[symbol_id]);
			// 判断是否需要继续
			if (all_symbol[symbol_id].is_terminal || first[symbol_id].find_id(find_in_symbol("@")) == -1)
				return res;
		}
		// 此时还没有返回的话，可能为Epsilon
		res.append_id(find_in_symbol("@"));
		return res;
	}
}
void LR1::get_follow()
{
	// follow集不需要考虑Epsilon，只有#
	bool done = false;
	int follow_num[SYMBOL_NUM] = { 0 };
	follow[0] = f_set(find_in_symbol("#"));
	follow_num[0] = 1;
	while (!done)
	{
		// 将所有的产生式做一个判断
		for (int i = 0; i < production_count; i++)
		{
			// 当前产生式
			production now_p = all_production[i];
			int* right = now_p.right_id;
			int right_count = now_p.right_count;
			int left_id = now_p.left_id;
			for (int j = 0; j < right_count; j++)
			{
				// 当前右边的符号
				int right_id = right[j];
				// 最后一个符号，且为非终结符
				if (j == right_count - 1 && !all_symbol[right_id].is_terminal)
					merge_fset_no_epsilon(follow[right_id], follow[left_id]);
				// 不是最后一个符号，且为非终结符
				else if (!all_symbol[right_id].is_terminal)
				{
					int beta[SYMBOL_NUM] = { 0 };
					int num = 0;
					for (int k = j + 1; k < right_count; k++)
					{
						beta[num++] = right[k];
					}
					f_set beta_first = get_one_array_first(beta, num);

					// 将beta的first集合，除去@，加入到right_id的follow集合中
					merge_fset_no_epsilon(follow[right_id], beta_first);

					// 如果有@，则将左边的follow集合加入到右边
					if (beta_first.find_id(find_in_symbol("@")) != -1)
						merge_fset_no_epsilon(follow[right_id], follow[left_id]);
				}
			}
		}
		// 判断是否还需要继续（无#）
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
	// 如果item_count不在增长，即为结束
	// 对于项目集中的每个项目
	for (int i = 0; i < one.item_count; i++)
	{
		// 该项目的信息
		int now_item_id = one.item_id[i];
		Item now = item[now_item_id];
		production now_p = all_production[now.p_id];
		int next_symbol_id = now_p.right_id[now.notpos];
		// 如果现在·的位置不是最后一个，并且·之后的位置是一个非终结符
		if (now.notpos != now_p.right_count && !all_symbol[next_symbol_id].is_terminal)
		{
			// 紧跟着·的非终结符
			int B = next_symbol_id;

			// first集
			int beta_a[SYMBOL_NUM] = { 0 };
			int num = 0;
			for (int i = now.notpos + 1; i < now_p.right_count; i++)
				beta_a[num++] = now_p.right_id[i];
			beta_a[num++] = now.outlook;
			f_set first_beta_a = get_one_array_first(beta_a, num);

			// 找到以B开头的产生式，将其加入到集合中，只要有新的插入就没有结束
			for (int j = 0; j < production_count; j++)
			{
				production tem = all_production[j];
				if (tem.left_id == B)
					// 对于first集的每个元素
					for (int k = 0; k < first_beta_a.count; k++)
					{
						// 这个项目之前不存在，就创建并且加入
						if (find_item(tem.id, 0, first_beta_a.id[k]) == -1)
						{
							// 当前count即为新项目的id
							one.append_item_id(item_count);
							// 再添加至数组中
							append_item(tem.id, 0, first_beta_a.id[k]);
						}
						// 这个项目之前存在，判断是否需要插入
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
	// 插入起始的项目集，id从1开始
	Item start(0, all_production[0].id, 0, find_in_symbol("#"));
	append_item(start);

	Item_set start_set(0, start);
	get_closure(start_set);
	append_item_set(start_set);

	// 当项目集不在增长的时候
	for (int now_item_set_id = 0; now_item_set_id < item_set_count; now_item_set_id++)
	{
		Item_set now_item_set = item_set[now_item_set_id];

		// 该项目集对每一个符号的转移函数
		// （有#，无开始符号，无Epsilon）0 是开始符号，所以只出现在左边，跳过分析，从 1 开始
		for (int symbol_id = 1; symbol_id < symbol_count; symbol_id++)
		{
			int X = symbol_id;

			// GO(now_item_set_id, X)
			// 从当前项目集，遇到X（symbol_id）的转移目标
			Item_set obj_item_set;
			obj_item_set.item_set_id = item_set_count;

			// 计算转移后的项目集
			for (int j = 0; j < now_item_set.item_count; j++)
			{
				int now_item_id = now_item_set.item_id[j];
				Item now_item = item[now_item_id];
				production now_p = all_production[now_item.p_id];
				// 找到某个项目：·的位置之后为X，生成新项目（·后移一个位置）
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

			// 对转移项目集的初始元素搜索完毕，获得其clouser集
			get_closure(obj_item_set);

			// 不存在就创建，得到GO的值
			if (find_item_set(obj_item_set) == -1 && obj_item_set.item_count != 0)
				append_item_set(obj_item_set);
			GO[now_item_set_id][X] = find_item_set(obj_item_set);
		}
	}
	return;
}
void LR1::get_func_table()
{
	// 1表示acc			-1表示错误
	// 3表示非空移进	4表示空移进
	// 5表示非空规约	6表示空规约

	// 对于所有的项目集
	for (int now_item_set_id = 0; now_item_set_id < item_set_count; now_item_set_id++)
	{
		Item_set now_item_set = item_set[now_item_set_id];

		// 判断是否存在空产生式，以及空产生式的转移状态
		bool has_epsilon = false;
		int epsilon_state = -1;
		// 遍历所有的项目
		for (int j = 0; j < now_item_set.item_count; j++)
		{
			int now_item_id = now_item_set.item_id[j];
			Item now_item = item[now_item_id];
			production now_p = all_production[now_item.p_id];
			// 如果是空产生式
			if (now_p.right_count == 1 && now_p.right_id[0] == find_in_symbol("@"))
			{
				// 规则6
				if (now_item.notpos == 1)
				{
					if (analysis_state[now_item_set_id][now_item.outlook] != 6 && analysis_state[now_item_set_id][now_item.outlook] != -1)
					{
						cout << "多重入口，存在冲突" << endl;
					}
					analysis_state[now_item_set_id][now_item.outlook] = 6;
					analysis_table[now_item_set_id][now_item.outlook] = now_p.id;
				}
				// 规则4
				else
				{
					has_epsilon = true;
					epsilon_state = GO[now_item_set_id][find_in_symbol("@")];
					continue;
				}
			}
			// 接受，规则1
			else if (now_item.p_id == 0 && now_item.notpos == 1 && now_item.outlook == find_in_symbol("#"))
			{
				if (analysis_state[now_item_set_id][now_item.outlook] != 1 && analysis_state[now_item_set_id][now_item.outlook] != -1)
				{
					cout << "多重入口，存在冲突" << endl;
				}
				analysis_state[now_item_set_id][now_item.outlook] = 1;
				analysis_table[now_item_set_id][now_item.outlook] = 0;
			}
			// 非空规约，规则5
			else if (now_item.notpos == now_p.right_count)
			{
				if (analysis_state[now_item_set_id][now_item.outlook] != 5 && analysis_state[now_item_set_id][now_item.outlook] != -1)
				{
					cout << "多重入口，存在冲突" << endl;
				}
				analysis_state[now_item_set_id][now_item.outlook] = 5;
				analysis_table[now_item_set_id][now_item.outlook] = now_p.id;
			}
			// 非空移进，规则3
			else
			{
				int next_symbol_id = now_p.right_id[now_item.notpos];
				if (analysis_state[now_item_set_id][next_symbol_id] != 3 && analysis_state[now_item_set_id][next_symbol_id] != -1)
				{
					cout << "多重入口，存在冲突" << endl;
				}
				analysis_state[now_item_set_id][next_symbol_id] = 3;
				analysis_table[now_item_set_id][next_symbol_id] = GO[now_item_set_id][next_symbol_id];
			}
		}
		// 规则4
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
		cout << "out_grammer.txt文件，写入失败！！" << endl;
		return;
	}
	out << "**************************产生式**********************************" << endl;
	for (int i = 0; i < production_count; i++)
	{
		production tem = all_production[i];
		out << "第 " << tem.id << " 条产生式为：";
		out << all_symbol[tem.left_id].name;
		out << " ->";
		for (int j = 0; j < tem.right_count; j++)
			out << " " << all_symbol[tem.right_id[j]].name;
		out << endl;
	}
	out << "**************************firstw集合***************************" << endl;
	for (int i = 0; i < symbol_count; i++)
	{
		out << setw(20) << all_symbol[i].name << " : ";
		for (int j = 0; j < first[i].count; j++)
			out << setw(20) << all_symbol[first[i].id[j]].name;
		out << endl;
	}
	out << "*************************follow集合***************************" << endl;
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
		cout << "out_item.txt文件，写入失败！！" << endl;
		return;
	}
	out << "*****************************item_set********************************" << endl;
	for (int i = 0; i < item_set_count; i++)
	{
		out << "项目集 " << i << " : " << endl;
		Item_set now = item_set[i];
		// 输出每一个项目
		for (int j = 0; j < now.item_count; j++)
		{
			out << "\t";
			Item now_i = item[now.item_id[j]];
			production now_p = all_production[now_i.p_id];
			out << all_symbol[now_p.left_id].name << " -> ";
			int k = 0;
			for (; k < now_p.right_count; k++)
				if (k == now_i.notpos)
					out << "· " << all_symbol[now_p.right_id[k]].name << " ";
				else
					out << all_symbol[now_p.right_id[k]].name << " ";
			if (now_i.notpos == now_p.right_count)
				out << "· ";
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
		cout << "第 " << tem.id << " 条产生式为：";
		cout << all_symbol[tem.left_id].name;
		cout << " ->";
		for (int j = 0; j < tem.right_count; j++)
			cout << " " << all_symbol[tem.right_id[j]].name;
		cout << endl;

		//cout << "第 " << tem.id << " 条产生式为：" << tem.product << endl;;
	}
}
void LR1::print_first_follow()
{
	ofstream out("first_follow.txt");
	if (!out.is_open())
	{
		cout << "first集合输出文件，打开失败！！" << endl;
		return;
	}
	out << "**************************firstw集合***************************" << endl;
	for (int i = 0; i < symbol_count; i++)
	{
		out << setw(20) << all_symbol[i].name << " : ";
		for (int j = 0; j < first[i].count; j++)
			out << setw(20) << all_symbol[first[i].id[j]].name;
		out << endl;
	}
	out << "*************************follow集合***************************" << endl;
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
		cout << "项目集 " << i << " : " << endl;
		Item_set now = item_set[i];
		// 输出每一个项目
		for (int j = 0; j < now.item_count; j++)
		{
			cout << "\t";
			Item now_i = item[now.item_id[j]];
			production now_p = all_production[now_i.p_id];
			cout << all_symbol[now_p.left_id].name << " -> ";
			int k = 0;
			for (; k < now_p.right_count; k++)
				if (k == now_i.notpos)
					cout << "· " << all_symbol[now_p.right_id[k]].name << " ";
				else
					cout << all_symbol[now_p.right_id[k]].name << " ";
			if (now_i.notpos == now_p.right_count)
				cout << "· ";
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
		cout << "项目集" << setw(2) << i << " : ";
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
		cout << "项目集" << setw(2) << i << " : ";
		for (int j = 0; j < symbol_count; j++)
		{
			if (all_symbol[j].is_terminal)
			switch (analysis_state[i][j])
			{
				case 1:
					cout << "接受 " << setw(2) << analysis_table[i][j] << "  ";
					break;
				case 3:
					cout << "非移 " << setw(2) << analysis_table[i][j] << "  ";
					break;
				case 4:
					cout << "空移 " << setw(2) << analysis_table[i][j] << "  ";
					break;
				case 5:
					cout << "非规 " << setw(2) << analysis_table[i][j] << "  ";
					break;
				case 6:
					cout << "空规 " << setw(2) << analysis_table[i][j] << "  ";
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
						cout << "接受 " << setw(2) << analysis_table[i][j] << "  ";
						break;
					case 3:
						cout << "非移 " << setw(2) << analysis_table[i][j] << "  ";
						break;
					case 4:
						cout << "空移 " << setw(2) << analysis_table[i][j] << "  ";
						break;
					case 5:
						cout << "非规 " << setw(2) << analysis_table[i][j] << "  ";
						break;
					case 6:
						cout << "空规 " << setw(2) << analysis_table[i][j] << "  ";
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
