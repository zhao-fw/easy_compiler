#include "LR1.hpp"
#include "SYM.hpp"
#include "Smantic.hpp"
#include "Lexer.hpp"
#include "ObjectGenerator.hpp"
#include <stack>

using namespace std;

// 由Lexer得到输入的SYM序列
vector<SYM> lexer_to_SYM(LR1* G, Lexer* L)
{
	vector<SYM> input;
	WT* wt = L->get_head()->next;
	while (wt) {
		SYM tem(G->find_in_symbol(wt->type_name), wt->word, wt->line);
		tem.set_symbol_name(wt->type_name);
		input.push_back(tem);
		wt = wt->next;
	}
	return input;
}							   

int main()
{
	/*
	* 以下为词法分析
	* 打开文件为in_source.txt，输出文件为out_lexer.txt
	*/
	Lexer* L = new Lexer;
	L->Scan();
	L->wt_type2name();


	/*
	* 以下为文法分析
	* 打开文件为in_grammer.txt，将first集、follow集输出到文件out_grammer.txt中，项目集输出到out_item.txt中
	*可能需要的函数：G.find_in_symbol("@")空符号的id号...
	*
	* symbol结构体包括：int id（符号的id）
	*			  char name[20]（符号名称）
	*			  is_terminal（true是终结符，false是非终结符）
	* 使用某个符号，直接all_sym[i]，得到该结构体
	*
	* production结构体包括：int id（内部储存）
	* 			  char product[100]	（产生式的内容）
	* 			  int left_id		（产生式左边的符号id）
	* 			  int right_id[20]	（产生式右边所有符号的id）
	* 			  int right_count	（产生式右边的符号个数）
	* 使用某个产生式，直接all_prod[i]，得到该结构体
	*
	* table中是移进时转移的状态，规约时使用的产生式
	*/
	LR1* G = new LR1;
	symbol* all_symbol = new symbol[SYMBOL_NUM];
	int symbol_num = 0;
	production* all_production = new production[PRODUCTION_NUM];
	int production_num = 0;
	int** analysis_table = new int* [MAX_NUM];
	int** analysis_state = new int* [MAX_NUM];
	for (int i = 0; i < MAX_NUM; i++)
	{
		analysis_table[i] = new int[SYMBOL_NUM];
		analysis_state[i] = new int[SYMBOL_NUM];
	}
	// 初始化
	G->analyze();
	symbol_num = G->return_symbol(all_symbol);
	production_num = G->return_production(all_production);
	G->return_table(analysis_state, analysis_table);

	/************************************************************************************
	*                        以下为分析程序（分析时生成语法树）                         *
	* 假定一共三个栈，input是输入的符号栈，symbol是现存符号栈，state是现存状态栈        *
	* 需要的初始化：                                                                    *
	*    input 中先放入“#”，在顺序放入所有的符号                                      *
	*    symbol中为空                                                                   *
	*	 state 中先放入0，作为起始状态                                                  *
	* 以下操作中只包括维护三个栈的情况                                                  *
	*************************************************************************************/
	stack<SYM> input_stack;		// 输入符号栈
	stack<SYM> symbol_stack;	// 现存符号栈
	stack<int>  state_stack;	// 现存状态栈
	SYM symend = SYM(G->find_in_symbol("#"), "end", -1);	// 结束符号

	symbol_stack.push(symend);						// 初始化现存符号栈
	state_stack.push(0);							// 初始化现存状态栈
	input_stack.push(symend);						// 初始化输入符号栈（先放入“#”，在顺序放入所有的符号）
	vector<SYM> lexer_sym = lexer_to_SYM(G, L);
	for (int i = lexer_sym.size() - 1; i >= 0; i--)
		input_stack.push(lexer_sym[i]);
	Smantic S;										// 使用的语义分析程序

	while (true)
	{
		int now_state_id = state_stack.top();	// 当前的状态id
		SYM now_sym = input_stack.top();		// 当前的输入SYM
		int now_input_id = now_sym.get_symbol();// 当前输入的symbol_id

		// 1表示acc			-1表示错误
		// 3表示非空移进	4表示空移进
		// 5表示非空规约	6表示空规约
		if (analysis_state[now_state_id][now_input_id] == 1)
		{
			// acc：用第一个产生式进行规约，即最后一步规约
			cout << "成功！！" << endl;
			break;
		}
		else if (analysis_state[now_state_id][now_input_id] == -1)
		{
			// wrong：table中无相关执行方式，即失败
			cout << "失败！！" << endl;
			break;
		}
		else if (analysis_state[now_state_id][now_input_id] == 3)
		{
			/*
			* 非空移进：input：	pop一次
			*			symbol：push输入的SYM
			*			state：	push转移后的state_id
			*/

			// 转移后的状态
			int n = analysis_table[now_state_id][now_input_id];
			// 建立中间节点
			SYM c = input_stack.top();
			// 对栈的操作（不需要动）
			input_stack.pop();
			symbol_stack.push(c);
			state_stack.push(n);
		}
		else if (analysis_state[now_state_id][now_input_id] == 4)
		{
			/*
			* 空移进：input：	不变
			* 		  symbol：	不变
			* 		  state：	push转移后的state_id
			*/

			// 转移后的状态
			int n = analysis_table[now_state_id][now_input_id];
			// 对栈的操作（不需要动）
			state_stack.push(n);
		}
		else if (analysis_state[now_state_id][now_input_id] == 5)
		{
			/*
			* 非空规约：input.push	(产生式的左边)
			* 			symbol.pop	产生式右边符号的数量次
			*			state.pop	产生式右边符号的数量次
			*/

			production now_p = all_production[analysis_table[now_state_id][now_input_id]];
			string p_name = now_p.product;
			vector<SYM> node;						// 记录当前所有规约符号（产生式右）
			int right_count = now_p.right_count;
			for (int i = 0; i < right_count; i++) {
				node.push_back(symbol_stack.top());
				symbol_stack.pop();
				state_stack.pop();
			}
			int left_id = now_p.left_id;			// 记录当前产生式的左边的符号id
			string left_name = all_symbol[left_id].name;

			/***   语义分析开始   ***/

			//由语义分析构造规约的符号
			SYM A = S.Smantic_analyze(p_name, left_id, node);
			//返回error的错误处理
			if (A.get_name() == "error")
			{
				cout << "语义分析错误" << endl;
				return 0;
			}
			//将产生式左部压栈
			input_stack.push(A);

			/***   语义分析结束   ***/
		}
		else if (analysis_state[now_state_id][now_input_id] == 6)
		{
			/*
			* 空规约：input.push (产生式的左边)
			* 		  symbol	 不变
			*		  state.pop  一次
			*/

			production now_p = all_production[analysis_table[now_state_id][now_input_id]];
			string p_name = now_p.product;
			int left_id = now_p.left_id;					// 产生式的左边的符号id
			string left_name = all_symbol[left_id].name;

			/***   语义分析开始   ***/

			//由语义分析构造规约的符号
			SYM B = S.Smantic_analyze(p_name, left_id, {});//此处vector node不存在
			//返回error的错误处理

			input_stack.push(B);

			/***   语义分析结束   ***/

			state_stack.pop();
		}
		else 
		{
			cout << analysis_state[now_state_id][now_input_id] << endl;
			cout << analysis_table[now_state_id][now_input_id] << endl;
			cout << "文法错误" << endl;
			break;
		}
	}

	
	/****************************************************************/
	S.INTcode.print();
	
	ObjectGenerator* O = new ObjectGenerator;
	O->Init(S.getIntermediateCode().getCode(), S.getFuncTable(), S.getVarTable());
	O->calculateInfo();
	ofstream quad_with_info("out_quad_with_info.txt");
	O->print(quad_with_info);
	O->generateCode();
	ofstream object("out_object.txt");
	O->printObj(object);

	return 0;
}
