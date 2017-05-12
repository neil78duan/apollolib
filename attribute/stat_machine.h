/* file stat_machine.h
 * 
 * role state 
 *
 * create by duan 
 * 2011/2/21 10:33:56
 */
 

#ifndef _STAT_MACHINE_H_
#define _STAT_MACHINE_H_

#include <stdlib.h>

class StatMachine ;

typedef int statfieldid_t;
typedef int statindex_t;
typedef unsigned int statval_t;
typedef unsigned int forbidval_t;

#define APOLLO_STATUS_NUMBER 8

//״̬�仯֪ͨ����
typedef void (*stat_entry)(StatMachine *pStmache,statindex_t index,const char*st_name) ;

class StatMachine
{
public:
	enum { STATE_BUF_SIZE = APOLLO_STATUS_NUMBER };
	enum {FORBID_BUF_SIZE = 32};
	enum {OPERATE_NUMBERS = 32 };
	enum {STAT_INDEX_BUF = 64};
	enum {STAT_NAME_SIZE = 32};
	enum {FORBID_CHANGE_TIMES = 8};		//��ֹ�б���౻�ۼӵĴ���,��¼ÿ����ֹ����ĸ�״̬���ı�

	enum {
		E_OP_VAL ,		//��ֵ����
		E_OP_BIT		//��λ����
	};

	typedef statval_t *iterator ;
	StatMachine() ;
	virtual ~StatMachine() ;	

	//�õ�״ֵ̬
	statval_t GetStat(statfieldid_t statid) ;
	statval_t SetStat(statfieldid_t statid, statval_t val) ;
	statval_t Replace(statfieldid_t statid, statval_t val);
	statval_t ClearStat(statfieldid_t statid, statval_t val) ;
	bool CheckStat(statfieldid_t statid, statval_t val) ;//����Ƿ���ĳ��״̬,return true �����״̬,false�������״̬

	bool Set(const char *statname) ;
	bool Clear(const char *statname) ;
	bool Check(const char *statname) ;			//return true �����״̬,false�������״̬
	bool CheckOp(const char *operation_name) ;	//true ��ʾ����ִ�иö���,false��������ֹ
	forbidval_t GetForbidVal(const char *forbid_name) ;	//�õ�ĳ����ֹ�б��ֵ
	const char *GetUnoperateReason(const char *opname) ;	//�õ��������ĸ�״̬����ֹ��
	bool CheckForbidUnset(const char *forbid_name){return GetForbidVal(forbid_name)==0;}	//����ֹ�б��Ƿ�δ������,true û�б�����
	bool ClearField(const char *mainstat);		//���״̬��(�����״̬)
	
	size_t GetStatDesc(char *in_buf, size_t buf_size) ;	//�õ���ǰ����״̬����Ϣ(�ı�)return text size
	size_t GetForbidDesc(char *in_buf, size_t buf_size) ;//�õ���ǰ��ֹ�б�������Ϣ(�ı�)
	
	//����ֹ״̬
	bool CheckForbid(int forbid_index) ;		//���ĳ����ֹ״̬ forbid_index , return true ��ֹ�б���ֹ
	bool CheckForbids(int forbid_buf[], int num) ;	//���forbid_buf��Ӧ�Ľ�ֹ״̬(ͬ��)
	bool CheckOperate(int operate_index) ;	//���ٱ������Ӧ�Ķ����Ƿ񱻽�ֹ true ��ʾ����ִ�иö���,false��������ֹ
	const char *GetForbidReason(int forbid_index) ;	//�õ���ֹ�б��ĸ�״̬enable

	void IncForbid(int forbid_index ,statindex_t raise_stat) ;
	void DecForbid(int forbid_index ,statindex_t raise_stat) ;

	bool SetByIndex(statindex_t stindex) ;
	bool ClearByIndex(statindex_t  stindex) ;

	iterator begin() ;
	iterator end() ;
protected :
	
	//״̬�ı��֪ͨ����
	virtual void OnChange(statfieldid_t /*statid*/, statval_t /*newval*/) {}

	//����״̬֪ͨ����
	virtual void OnEnter(statindex_t /*st_index*/) {}

	//�뿪״̬֪ͨ����
	virtual void OnLeave(statindex_t /*st_index*/) {}

	//�����ֹ�б�֪ͨ����
	virtual void OnEnterForbid(int /*forbidid*/){}

	//�뿪��ֹ֪ͨ����
	virtual void OnLeaveForbid(int /*forbidid*/){}

	//�õ�״̬�����Ե�Ĭ��ֵ
	virtual statval_t GetDefvalue(statfieldid_t /*statid*/) {return 0;}	


	//״̬�����õĻص�����(����ֹ�б��1)
	void on_status_set(statfieldid_t statid, statval_t val) ;
	//״̬������Ļص�����(����ֹ�б��1)
	void on_status_clear(statfieldid_t statid, statval_t val) ;

	statval_t *ref_status(statfieldid_t statid) ;
	forbidval_t *ref_forbid(int statid) ;

	//�õ�ÿ��״ֵ̬��Ӧ�Ľ�ֹ�б�
	//int* load_forbid_values(int stat_index, int *num) ;
	//�޸Ľ�ֹ�б��ֵ,flag = 1���� 0����(����״̬���Ӧ��ֵ,�ҵ���ֹ�б�,Ȼ���޸Ľ�ֹ�б�ֵ)
	void change_forbids_values(statindex_t stat_index, int flag) ;
// 
// 	inline void inc_refcount(int stat_index) {	++(m_refcount[stat_index]) ;}
// 	void dec_refcount(int stat_index) {	--(m_refcount[stat_index]) ;}
// 	unsigned char get_refcount(int stat_index) {return m_refcount[stat_index] ;	}
	
	statval_t m_stbuf[STATE_BUF_SIZE] ;			//״ֵ̬
	forbidval_t m_forbid_buf[FORBID_BUF_SIZE];	//��ֹ�б�ֵ
	unsigned char m_refcount[STAT_INDEX_BUF] ;				//״̬��Ӧ�����ü���
	statindex_t m_forbid_changed_list[FORBID_BUF_SIZE][FORBID_CHANGE_TIMES] ;//��¼ÿ����ֹ�б���Щ״̬���ı�

private:
	unsigned char GetOpType(statfieldid_t statid) ;
	statindex_t get_state_index(statfieldid_t statid, statval_t val) ;
public:
	static int get_index(const char *name) ;
	static int get_forbid_index(const char *name) ;
	static int get_operate_index(const char *name);
	static int get_main_index(const char *name);
	static const char *get_name(statindex_t stat_index) ;
	static const char *get_main_name(const char* substat_name) ;
	//load data
	static int load(const char *forbid_file, const char* state_file, const char*operate_file);
	static void Destroy() ;
	static int load_forbid_names(const char *tablename) ;
	static int load_stat_info(const char *tablename) ;
	static int load_op_info(const char *tablename);

	static int s_forbid_num ;	//�߻���Ľ�ֹ�б����
	static int s_stat_num;		//�߻����״̬����
	static int s_operate_num;
	static int s_main_stat_num;	
	static unsigned char s_stat_valbit[STATE_BUF_SIZE] ;	//ÿ����Ӧ��״̬�ǰ�ֵ���ǰ�λ����
	static int s_operate_check_forbid[OPERATE_NUMBERS][FORBID_BUF_SIZE+1] ;	//ÿ��������Ҫ���Ľ�ֹ�б�
	static int s_stat_index_buf[STAT_INDEX_BUF][2] ;		//ÿ��״̬��Ӧ������(¼��ǰ��״̬�����Ǹ�����(��״̬),�Ͳ�������)
	static int s_stat_forbid_buf[STAT_INDEX_BUF][FORBID_BUF_SIZE+1] ; //ÿ��״̬��Ӧ�Ľ�ֹ�б�ֵ
	static int s_stat_need_counter[STAT_INDEX_BUF];//��¼ÿ��״̬�Ƿ���Ҫ������

	static char s_stat_names[STAT_INDEX_BUF][STAT_NAME_SIZE] ;	//״̬����
	static char s_forbid_names[FORBID_BUF_SIZE][STAT_NAME_SIZE] ;	//��ֹ�б�
	static char s_operate_names[OPERATE_NUMBERS][STAT_NAME_SIZE] ;	//��������
	static char s_main_stat_names[STATE_BUF_SIZE][STAT_NAME_SIZE] ;

	static stat_entry s_insted_enter[STAT_INDEX_BUF] ;
	static stat_entry s_insted_leave[STAT_INDEX_BUF] ;
};


//ע��״̬�仯������,��OnEnter /onleve����
int GlobalInsStatEntry(const char *st_name, stat_entry func, bool bEnter = true) ;

#endif
