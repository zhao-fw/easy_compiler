#include "LR1.hpp"
#include "SYM.hpp"
#include "Smantic.hpp"
#include "Lexer.hpp"
#include "ObjectGenerator.hpp"
#include <stack>

using namespace std;

// ��Lexer�õ������SYM����
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
	* ����Ϊ�ʷ�����
	* ���ļ�Ϊin_source.txt������ļ�Ϊout_lexer.txt
	*/
	Lexer* L = new Lexer;
	L->Scan();
	L->wt_type2name();


	/*
	* ����Ϊ�ķ�����
	* ���ļ�Ϊin_grammer.txt����first����follow��������ļ�out_grammer.txt�У���Ŀ�������out_item.txt��
	*������Ҫ�ĺ�����G.find_in_symbol("@")�շ��ŵ�id��...
	*
	* symbol�ṹ�������int id�����ŵ�id��
	*			  char name[20]���������ƣ�
	*			  is_terminal��true���ս����false�Ƿ��ս����
	* ʹ��ĳ�����ţ�ֱ��all_sym[i]���õ��ýṹ��
	*
	* production�ṹ�������int id���ڲ����棩
	* 			  char product[100]	������ʽ�����ݣ�
	* 			  int left_id		������ʽ��ߵķ���id��
	* 			  int right_id[20]	������ʽ�ұ����з��ŵ�id��
	* 			  int right_count	������ʽ�ұߵķ��Ÿ�����
	* ʹ��ĳ������ʽ��ֱ��all_prod[i]���õ��ýṹ��
	*
	* table�����ƽ�ʱת�Ƶ�״̬����Լʱʹ�õĲ���ʽ
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
	// ��ʼ��
	G->analyze();
	symbol_num = G->return_symbol(all_symbol);
	production_num = G->return_production(all_production);
	G->return_table(analysis_state, analysis_table);

	/************************************************************************************
	*                        ����Ϊ�������򣨷���ʱ�����﷨����                         *
	* �ٶ�һ������ջ��input������ķ���ջ��symbol���ִ����ջ��state���ִ�״̬ջ        *
	* ��Ҫ�ĳ�ʼ����                                                                    *
	*    input ���ȷ��롰#������˳��������еķ���                                      *
	*    symbol��Ϊ��                                                                   *
	*	 state ���ȷ���0����Ϊ��ʼ״̬                                                  *
	* ���²�����ֻ����ά������ջ�����                                                  *
	*************************************************************************************/
	stack<SYM> input_stack;		// �������ջ
	stack<SYM> symbol_stack;	// �ִ����ջ
	stack<int>  state_stack;	// �ִ�״̬ջ
	SYM symend = SYM(G->find_in_symbol("#"), "end", -1);	// ��������

	symbol_stack.push(symend);						// ��ʼ���ִ����ջ
	state_stack.push(0);							// ��ʼ���ִ�״̬ջ
	input_stack.push(symend);						// ��ʼ���������ջ���ȷ��롰#������˳��������еķ��ţ�
	vector<SYM> lexer_sym = lexer_to_SYM(G, L);
	for (int i = lexer_sym.size() - 1; i >= 0; i--)
		input_stack.push(lexer_sym[i]);
	Smantic S;										// ʹ�õ������������

	while (true)
	{
		int now_state_id = state_stack.top();	// ��ǰ��״̬id
		SYM now_sym = input_stack.top();		// ��ǰ������SYM
		int now_input_id = now_sym.get_symbol();// ��ǰ�����symbol_id

		// 1��ʾacc			-1��ʾ����
		// 3��ʾ�ǿ��ƽ�	4��ʾ���ƽ�
		// 5��ʾ�ǿչ�Լ	6��ʾ�չ�Լ
		if (analysis_state[now_state_id][now_input_id] == 1)
		{
			// acc���õ�һ������ʽ���й�Լ�������һ����Լ
			cout << "�ɹ�����" << endl;
			break;
		}
		else if (analysis_state[now_state_id][now_input_id] == -1)
		{
			// wrong��table�������ִ�з�ʽ����ʧ��
			cout << "ʧ�ܣ���" << endl;
			break;
		}
		else if (analysis_state[now_state_id][now_input_id] == 3)
		{
			/*
			* �ǿ��ƽ���input��	popһ��
			*			symbol��push�����SYM
			*			state��	pushת�ƺ��state_id
			*/

			// ת�ƺ��״̬
			int n = analysis_table[now_state_id][now_input_id];
			// �����м�ڵ�
			SYM c = input_stack.top();
			// ��ջ�Ĳ���������Ҫ����
			input_stack.pop();
			symbol_stack.push(c);
			state_stack.push(n);
		}
		else if (analysis_state[now_state_id][now_input_id] == 4)
		{
			/*
			* ���ƽ���input��	����
			* 		  symbol��	����
			* 		  state��	pushת�ƺ��state_id
			*/

			// ת�ƺ��״̬
			int n = analysis_table[now_state_id][now_input_id];
			// ��ջ�Ĳ���������Ҫ����
			state_stack.push(n);
		}
		else if (analysis_state[now_state_id][now_input_id] == 5)
		{
			/*
			* �ǿչ�Լ��input.push	(����ʽ�����)
			* 			symbol.pop	����ʽ�ұ߷��ŵ�������
			*			state.pop	����ʽ�ұ߷��ŵ�������
			*/

			production now_p = all_production[analysis_table[now_state_id][now_input_id]];
			string p_name = now_p.product;
			vector<SYM> node;						// ��¼��ǰ���й�Լ���ţ�����ʽ�ң�
			int right_count = now_p.right_count;
			for (int i = 0; i < right_count; i++) {
				node.push_back(symbol_stack.top());
				symbol_stack.pop();
				state_stack.pop();
			}
			int left_id = now_p.left_id;			// ��¼��ǰ����ʽ����ߵķ���id
			string left_name = all_symbol[left_id].name;

			/***   ���������ʼ   ***/

			//��������������Լ�ķ���
			SYM A = S.Smantic_analyze(p_name, left_id, node);
			//����error�Ĵ�����
			if (A.get_name() == "error")
			{
				cout << "�����������" << endl;
				return 0;
			}
			//������ʽ��ѹջ
			input_stack.push(A);

			/***   �����������   ***/
		}
		else if (analysis_state[now_state_id][now_input_id] == 6)
		{
			/*
			* �չ�Լ��input.push (����ʽ�����)
			* 		  symbol	 ����
			*		  state.pop  һ��
			*/

			production now_p = all_production[analysis_table[now_state_id][now_input_id]];
			string p_name = now_p.product;
			int left_id = now_p.left_id;					// ����ʽ����ߵķ���id
			string left_name = all_symbol[left_id].name;

			/***   ���������ʼ   ***/

			//��������������Լ�ķ���
			SYM B = S.Smantic_analyze(p_name, left_id, {});//�˴�vector node������
			//����error�Ĵ�����

			input_stack.push(B);

			/***   �����������   ***/

			state_stack.pop();
		}
		else 
		{
			cout << analysis_state[now_state_id][now_input_id] << endl;
			cout << analysis_table[now_state_id][now_input_id] << endl;
			cout << "�ķ�����" << endl;
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
