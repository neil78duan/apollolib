/* file stat_data.cpp
 *
 * data of state machine
 *
 * create by duan 
 * 2011/2/21 17:43:08
 */

#include "attribute/stat_machine.h"
#include <stdlib.h>
#include "attribute/stat_data.h"

#include "ndapplib/applib.h"
#include "game_parser/dbl_mgr.h"


int StatMachine::s_forbid_num = 0 ;	//策划填的禁止列表个数
int StatMachine::s_stat_num = 0;		//策划填的状态个数
int StatMachine::s_operate_num =0;
 
int StatMachine::s_main_stat_num =0;	

char StatMachine::s_main_stat_names[STATE_BUF_SIZE][STAT_NAME_SIZE] ;
char StatMachine::s_stat_names[StatMachine::STAT_INDEX_BUF][StatMachine::STAT_NAME_SIZE] ;	//状态名字
char StatMachine::s_forbid_names[StatMachine::FORBID_BUF_SIZE][StatMachine::STAT_NAME_SIZE] ;	//禁止列表
char StatMachine::s_operate_names[StatMachine::OPERATE_NUMBERS][StatMachine::STAT_NAME_SIZE] ;	//动作名字

//每个对应的状态是按值还是按位操作
unsigned char StatMachine::s_stat_valbit[StatMachine::STATE_BUF_SIZE] ;
//每个动作需要检测的禁止列表
int StatMachine::s_operate_check_forbid[StatMachine::OPERATE_NUMBERS][StatMachine::FORBID_BUF_SIZE+1] ;
//每个状态对应的索引
int StatMachine::s_stat_index_buf[StatMachine::STAT_INDEX_BUF][2];

int StatMachine::s_stat_need_counter[StatMachine::STAT_INDEX_BUF];

//每个状态对应的禁止列表值
int StatMachine::s_stat_forbid_buf[StatMachine::STAT_INDEX_BUF][StatMachine::FORBID_BUF_SIZE+1];


//StateDataHeaper g_stat_script ;

//每个状态的进入/离开函数
stat_entry StatMachine::s_insted_enter[STAT_INDEX_BUF] ;
stat_entry StatMachine::s_insted_leave[STAT_INDEX_BUF] ;

//把字符串解析成数组
static int text2StringArray(const char *src, char outbuf[StatMachine::FORBID_BUF_SIZE][StatMachine::STAT_NAME_SIZE])
{
	int orgCodeType = -1;
	DBLDatabase *pdbl = DBLDatabase::get_Instant();
	if (pdbl){
		orgCodeType = ndstr_set_code(pdbl->GetEncodeType());
	}

	int ret = 0;
	const char *p = src;
	char buf[128];

	while (p && *p) {
		buf[0] = 0;
		p = ndstr_str_end(p, outbuf[ret], ';');
		if (p && *p == ';')	{
			++p;
		}
		if (outbuf[ret][0])	{
			++ret;
		}
	}

	if (orgCodeType!=-1) {
		ndstr_set_code(orgCodeType);
	}
	return ret;
}

int StatMachine::load_forbid_names(const char *tablename) 
{
	StatMachine::s_forbid_num = 0;
	const char *pfields[] = { "name"} ;
	int i = 0 ;
	DBL_BEGIN_CURSOR(tablename, pfields) {
		char* p = StatMachine::s_forbid_names[i++] ;
		if (!cursor[0].CheckValid()){
			return -1;
		}
		strncpy(p, cursor[0].GetText() , StatMachine::STAT_NAME_SIZE) ;
		++(StatMachine::s_forbid_num ) ;
	}
	return 0;
}
//加载状态表
int StatMachine::load_stat_info(const char *tablename)
{
	int cur_main  ,cur_sub ;
	int i= 0;
	const char *lastStatName = NULL;
	int main_stat_id[StatMachine::STATE_BUF_SIZE] = { 0 };
	
	const char* pfields[] = {"stname", "mutex", "subname", "forbid_list","iscount"};
	
	cur_sub = 0 ;
	cur_main = -1;

	DBL_BEGIN_CURSOR(tablename, pfields) {
		int is_new_field = 0 ;
		const char *mainName = cursor[0].GetText();
		//int input_id = cursor[0].GetInt();
		if (cur_main==-1 || strcmp(lastStatName, mainName) ){
			//开始读取一个新的域
			cur_main++ ;
			cur_sub= 0 ;
			main_stat_id[cur_main] = cur_main ;
			s_stat_valbit[cur_main] = cursor[1].GetBool() ? E_OP_BIT:E_OP_VAL;
			strncpy(s_main_stat_names[cur_main], mainName, STAT_NAME_SIZE);
			lastStatName = mainName;
			is_new_field = 1 ;
		}

		//读取状态名字
		strncpy(s_stat_names[i], cursor[2].GetText(), STAT_NAME_SIZE) ;		
		s_stat_index_buf[i][0] = cur_main ;	//状态索引表记录当前子状态属于那个变量(主状态)

		s_stat_need_counter[i] = cursor[4].GetInt() ;;//记录每个状态是否需要计数器

		//按值还是按位操作
		if (s_stat_valbit[cur_main] ==E_OP_VAL)	{
			s_stat_index_buf[i][1] = cur_sub ;
		}
		else {
			s_stat_index_buf[i][1] = 1<<cur_sub;
		}
		cur_sub++ ;

		//解析禁止列表
		s_stat_forbid_buf[i][0] = 0;

		const char *forbidText = cursor[3].GetText();
		char outbufTmp[StatMachine::FORBID_BUF_SIZE][StatMachine::STAT_NAME_SIZE];
		int num = text2StringArray(forbidText, outbufTmp);

		for(int n=0; n<num; n++) {
			int a = StatMachine::get_forbid_index(outbufTmp[n]);
			if (0 <= a && a < s_forbid_num ){
				s_stat_forbid_buf[i][n+1] = a ;
				(s_stat_forbid_buf[i][0])++ ;
			}
		}
		if (is_new_field && s_stat_forbid_buf[i][0] && s_stat_valbit[cur_main]==E_OP_VAL) {
			nd_logerror("[%s]表格错误 [%d] 行, 互斥状态[%s]不允许有禁止列 \n " AND tablename AND i AND s_stat_names[i]) ;
			return -1 ;
		}
		++i;
	}
	StatMachine::s_main_stat_num = cur_main +1 ;
	StatMachine::s_stat_num = i ;
	return 0;
}

//加载状态表
int StatMachine::load_op_info(const char *tablename)
{
	const char *pfields[] = { "name", "checklist"} ;
	int i= 0;
	DBL_BEGIN_CURSOR(tablename, pfields) {
		strncpy(s_operate_names[i], cursor[0].GetText(), STAT_NAME_SIZE) ;		
		//解析操作对应的禁止列表
		s_operate_check_forbid[i][0] = 0 ;		

		const char *forbidText = cursor[1].GetText();
		char outbufTmp[StatMachine::FORBID_BUF_SIZE][StatMachine::STAT_NAME_SIZE];
		int num = text2StringArray(forbidText, outbufTmp);

		for (int n = 0; n < num; n++) {
			int a = StatMachine::get_forbid_index(outbufTmp[n]);
			if (a < s_forbid_num || a >= 0){
				s_operate_check_forbid[i][n + 1] = a;
				(s_operate_check_forbid[i][0])++;
			}
		}

		++i ;
	}
	StatMachine::s_operate_num = i ;
	return 0;
}
// 
// int StateDataHeaper::Init()
// {
// 	int i= 0;
// 
// 	const char *pfields[] =  {"状态名称","玩家进入事件","玩家退出事件","怪物进入事件","怪物退出事件"};
// 	
// 	LogicDataObj *pDataObj = NULL;
// 	DBL_BEGIN_CURSOR("角色_状态_事件通知", pfields) {
// 		statindex_t stindx = StatMachine::get_index(cursor[0].GetText()) ;
// 		if (-1==stindx) {
// 			continue;
// 		}
// 
// 		pDataObj = &cursor[1];
// 		if ( pDataObj->CheckValid() ){
// 			strncpy(m_player_enter_script[stindx] , pDataObj->GetText() , E_STAT_SCRIPT_SIZE) ;
// 		}
// 
// 		pDataObj = &cursor[2];
// 		if ( pDataObj->CheckValid()){
// 			strncpy(m_player_leave_script[stindx] , pDataObj->GetText() , E_STAT_SCRIPT_SIZE) ;
// 		}
// 
// 		pDataObj = &cursor[3];
// 		if (pDataObj->CheckValid() ){
// 			strncpy(m_monster_enter_script[stindx] , pDataObj->GetText() , E_STAT_SCRIPT_SIZE) ;
// 		}
// 
// 		pDataObj = &cursor[4];
// 		if (pDataObj->CheckValid()){
// 			strncpy(m_monster_leave_script[stindx] , pDataObj->GetText() , E_STAT_SCRIPT_SIZE) ;
// 		}
// 	}
// 	return 0;
// 	
// }

// 
// StateDataHeaper::StateDataHeaper() 
// {
// 	Destroy() ;
// }
// 
// StateDataHeaper::~StateDataHeaper() 
// {
// 
// }
// 
// const char *StateDataHeaper::GetLeaveScript(int index, int unit_type) 
// {
// 	if (index <0 || index>=StatMachine::s_stat_num) {
// 		return NULL ;
// 	}
// 	/*
// 	if (unit_type == UNIT_MONSTER){
// 		return m_monster_leave_script[index] ;
// 	}
// 	else if (unit_type == UNIT_PLAYER){
// 		return m_player_leave_script[index] ;
// 	}*/
// 	return NULL ;
// 
// }
// 
// const char *StateDataHeaper::GetEnterScript(int index, int unit_type) 
// {
// 	if (index <0 || index>=StatMachine::s_stat_num) {
// 		return NULL ;
// 	}
// 	/*if (unit_type == UNIT_MONSTER){
// 		return m_monster_enter_script[index] ;
// 	}
// 	else if (unit_type == UNIT_PLAYER){
// 		return m_player_enter_script[index] ;
// 	}*/
// 	return NULL ;
// 
// }
// 
// 
// void StateDataHeaper::Destroy() 
// {
// 	memset(m_player_enter_script, 0, sizeof(m_player_enter_script)) ;
// 	memset(m_player_leave_script, 0, sizeof(m_player_leave_script)) ;
// 	memset(m_monster_enter_script, 0, sizeof(m_monster_enter_script)) ;
// 	memset(m_monster_leave_script, 0, sizeof(m_monster_leave_script)) ;
// }


//#include "nd_common/nd_common.h"
int StatMachine::load(const char *forbid_file, const char* state_file, const char*operate_file) 
{
	if (load_forbid_names("operate_forbid_list.xlsx")==-1){
		return -1;
	}
	if (-1==load_stat_info("role_state.xlsx") ){
		return -1;
	}
	if (-1==load_op_info("role_operate_check.xlsx") ){
		return -1;
	}
	
	
	return 0;
}


void StatMachine::Destroy() 
{
	s_forbid_num = 0 ;	//策划填的禁止列表个数
	s_stat_num = 0;		//策划填的状态个数
	s_operate_num = 0;
	s_main_stat_num = 0;

	memset( s_stat_valbit, 0 , sizeof(s_stat_valbit)) ;
	memset( s_operate_check_forbid, 0 , sizeof(s_operate_check_forbid)) ;
	memset( s_stat_index_buf, 0 , sizeof(s_stat_index_buf)) ;
	memset( s_stat_forbid_buf, 0 , sizeof(s_stat_forbid_buf)) ;
	memset( s_stat_names, 0 , sizeof(s_stat_names)) ;
	memset( s_forbid_names, 0 , sizeof(s_forbid_names)) ;
	memset( s_operate_names, 0 , sizeof(s_operate_names)) ;
	memset( s_main_stat_names, 0 , sizeof(s_main_stat_names)) ;

}