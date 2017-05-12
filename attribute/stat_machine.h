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

//状态变化通知函数
typedef void (*stat_entry)(StatMachine *pStmache,statindex_t index,const char*st_name) ;

class StatMachine
{
public:
	enum { STATE_BUF_SIZE = APOLLO_STATUS_NUMBER };
	enum {FORBID_BUF_SIZE = 32};
	enum {OPERATE_NUMBERS = 32 };
	enum {STAT_INDEX_BUF = 64};
	enum {STAT_NAME_SIZE = 32};
	enum {FORBID_CHANGE_TIMES = 8};		//禁止列表最多被累加的次数,记录每个禁止类别被哪个状态所改变

	enum {
		E_OP_VAL ,		//按值操作
		E_OP_BIT		//按位操作
	};

	typedef statval_t *iterator ;
	StatMachine() ;
	virtual ~StatMachine() ;	

	//得到状态值
	statval_t GetStat(statfieldid_t statid) ;
	statval_t SetStat(statfieldid_t statid, statval_t val) ;
	statval_t Replace(statfieldid_t statid, statval_t val);
	statval_t ClearStat(statfieldid_t statid, statval_t val) ;
	bool CheckStat(statfieldid_t statid, statval_t val) ;//检测是否是某个状态,return true 在这个状态,false不在这个状态

	bool Set(const char *statname) ;
	bool Clear(const char *statname) ;
	bool Check(const char *statname) ;			//return true 在这个状态,false不在这个状态
	bool CheckOp(const char *operation_name) ;	//true 表示可以执行该动作,false动作被禁止
	forbidval_t GetForbidVal(const char *forbid_name) ;	//得道某个禁止列表的值
	const char *GetUnoperateReason(const char *opname) ;	//得到动作被哪个状态给禁止的
	bool CheckForbidUnset(const char *forbid_name){return GetForbidVal(forbid_name)==0;}	//检测禁止列表是否未被设置,true 没有被设置
	bool ClearField(const char *mainstat);		//清空状态域(清空主状态)
	
	size_t GetStatDesc(char *in_buf, size_t buf_size) ;	//得到当前所处状态的信息(文本)return text size
	size_t GetForbidDesc(char *in_buf, size_t buf_size) ;//得到当前禁止列表描述信息(文本)
	
	//检测禁止状态
	bool CheckForbid(int forbid_index) ;		//检测某个禁止状态 forbid_index , return true 禁止列表被禁止
	bool CheckForbids(int forbid_buf[], int num) ;	//检测forbid_buf对应的禁止状态(同上)
	bool CheckOperate(int operate_index) ;	//检测操编号作对应的动作是否被禁止 true 表示可以执行该动作,false动作被禁止
	const char *GetForbidReason(int forbid_index) ;	//得到禁止列表被哪个状态enable

	void IncForbid(int forbid_index ,statindex_t raise_stat) ;
	void DecForbid(int forbid_index ,statindex_t raise_stat) ;

	bool SetByIndex(statindex_t stindex) ;
	bool ClearByIndex(statindex_t  stindex) ;

	iterator begin() ;
	iterator end() ;
protected :
	
	//状态改变后通知函数
	virtual void OnChange(statfieldid_t /*statid*/, statval_t /*newval*/) {}

	//进入状态通知函数
	virtual void OnEnter(statindex_t /*st_index*/) {}

	//离开状态通知函数
	virtual void OnLeave(statindex_t /*st_index*/) {}

	//进入禁止列表通知函数
	virtual void OnEnterForbid(int /*forbidid*/){}

	//离开禁止通知函数
	virtual void OnLeaveForbid(int /*forbidid*/){}

	//得到状态域属性的默认值
	virtual statval_t GetDefvalue(statfieldid_t /*statid*/) {return 0;}	


	//状态被设置的回调函数(给禁止列表加1)
	void on_status_set(statfieldid_t statid, statval_t val) ;
	//状态被清除的回调函数(给禁止列表减1)
	void on_status_clear(statfieldid_t statid, statval_t val) ;

	statval_t *ref_status(statfieldid_t statid) ;
	forbidval_t *ref_forbid(int statid) ;

	//得到每个状态值对应的禁止列表
	//int* load_forbid_values(int stat_index, int *num) ;
	//修改禁止列表的值,flag = 1增加 0减少(根据状态域对应的值,找到禁止列表,然后修改禁止列表值)
	void change_forbids_values(statindex_t stat_index, int flag) ;
// 
// 	inline void inc_refcount(int stat_index) {	++(m_refcount[stat_index]) ;}
// 	void dec_refcount(int stat_index) {	--(m_refcount[stat_index]) ;}
// 	unsigned char get_refcount(int stat_index) {return m_refcount[stat_index] ;	}
	
	statval_t m_stbuf[STATE_BUF_SIZE] ;			//状态值
	forbidval_t m_forbid_buf[FORBID_BUF_SIZE];	//禁止列表值
	unsigned char m_refcount[STAT_INDEX_BUF] ;				//状态对应的引用计数
	statindex_t m_forbid_changed_list[FORBID_BUF_SIZE][FORBID_CHANGE_TIMES] ;//记录每个禁止列表被那些状态所改变

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

	static int s_forbid_num ;	//策划填的禁止列表个数
	static int s_stat_num;		//策划填的状态个数
	static int s_operate_num;
	static int s_main_stat_num;	
	static unsigned char s_stat_valbit[STATE_BUF_SIZE] ;	//每个对应的状态是按值还是按位操作
	static int s_operate_check_forbid[OPERATE_NUMBERS][FORBID_BUF_SIZE+1] ;	//每个动作需要检测的禁止列表
	static int s_stat_index_buf[STAT_INDEX_BUF][2] ;		//每个状态对应的索引(录当前子状态属于那个变量(主状态),和操作类型)
	static int s_stat_forbid_buf[STAT_INDEX_BUF][FORBID_BUF_SIZE+1] ; //每个状态对应的禁止列表值
	static int s_stat_need_counter[STAT_INDEX_BUF];//记录每个状态是否需要计数器

	static char s_stat_names[STAT_INDEX_BUF][STAT_NAME_SIZE] ;	//状态名字
	static char s_forbid_names[FORBID_BUF_SIZE][STAT_NAME_SIZE] ;	//禁止列表
	static char s_operate_names[OPERATE_NUMBERS][STAT_NAME_SIZE] ;	//动作名字
	static char s_main_stat_names[STATE_BUF_SIZE][STAT_NAME_SIZE] ;

	static stat_entry s_insted_enter[STAT_INDEX_BUF] ;
	static stat_entry s_insted_leave[STAT_INDEX_BUF] ;
};


//注册状态变化处理函数,与OnEnter /onleve并列
int GlobalInsStatEntry(const char *st_name, stat_entry func, bool bEnter = true) ;

#endif
