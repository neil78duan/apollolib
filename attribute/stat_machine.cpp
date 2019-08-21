/* file stat_machine.cpp
 * 
 * role state 
 *
 * create by duan 
 * 2011/2/21 10:33:56
 */

#include "attribute/stat_machine.h"
#include "nd_common/nd_common.h"

StatMachine::StatMachine()
{
	memset(m_refcount,0,sizeof(m_refcount) );
	memset(m_stbuf,0,sizeof(m_stbuf) );
	memset(m_forbid_buf,0,sizeof(m_forbid_buf) );
	memset(m_forbid_changed_list,0xff,sizeof(m_forbid_changed_list) );
}


StatMachine::~StatMachine()
{

}


bool StatMachine::Set(const char *statname) 
{
	int *p ;
	int stindex = StatMachine::get_index(statname) ;
	if(stindex==-1 || stindex>=s_stat_num){
		return false;
	}
	p = StatMachine::s_stat_index_buf[stindex] ;
	statval_t old = SetStat(p[0], (statval_t)p[1]) ;
	if(old ==m_stbuf[p[0]]) {
		return false ;
	}
	return true;
}


bool StatMachine::Clear(const char *statname) 
{
	int *p ;
	int stindex = StatMachine::get_index(statname) ;
	if(stindex==-1 || stindex>=s_stat_num){
		return false;
	}
	p = StatMachine::s_stat_index_buf[stindex] ;
	statval_t old = ClearStat(p[0], (statval_t)p[1]) ;
	if(old ==m_stbuf[p[0]]) {
		return false ;
	}
	return true;	

}

//清空状态域
bool StatMachine::ClearField(const char *mainstat)
{
	int field = get_main_index(mainstat) ;
	if (-1== field) {
		return false ;
	}
	Replace((statfieldid_t)field, 0) ;
	return true ;
}

bool StatMachine::Check(const char *statname) 
{
	int *p ;
	int stindex = StatMachine::get_index(statname) ;
	if(stindex==-1 || stindex>=s_stat_num){
		return false;
	}
	p = StatMachine::s_stat_index_buf[stindex] ;

	return CheckStat(p[0], (statval_t)p[1]);
}

bool StatMachine::CheckOp(const char *statname) 
{
	int stindex = StatMachine::get_operate_index(statname) ;
	if(stindex==-1 || stindex>=s_operate_num){
		return true;
	}
	return CheckOperate(stindex);
}


forbidval_t StatMachine::GetForbidVal(const char *forbid_name) 
{

	int stindex = StatMachine::get_forbid_index(forbid_name) ;
	if(stindex==-1 || stindex>=s_forbid_num){
		return 0 ;
	}
	return  m_forbid_buf[stindex] ;
	
}

//得到禁止列表被哪个状态enable
const char *StatMachine::GetForbidReason(int forbid_index)
{
	int st_index = -1 ;
	if (forbid_index>=0 && forbid_index<s_forbid_num){
		for (int i=0; i<FORBID_CHANGE_TIMES; i++) {
			if(m_forbid_changed_list[forbid_index][i] != (statindex_t)-1) {
				st_index = m_forbid_changed_list[forbid_index][i] ;
				break ;
			}
		}
	}
	if (st_index!= -1){
		return StatMachine::get_name(st_index) ;
	}
	return NULL;
}
//得到动作被哪个状态给禁止的
const char *StatMachine::GetUnoperateReason(const char *opname)
{
	int stindex = StatMachine::get_operate_index(opname) ;
	if(stindex==-1 || stindex>=s_operate_num){
		return NULL;
	}

	int num ;
	int *p = s_operate_check_forbid[stindex] ;
	num = p[0] ;
	for(int i=0; i<num; i++,p++) {
		const char *ret = GetForbidReason(*p) ;
		if (ret){
			return ret ;
		}
	}
	return NULL;
}

size_t StatMachine::GetStatDesc(char *in_buf, size_t buf_size)
{
	char *p = in_buf ;
	for(int i=0; i<s_main_stat_num; i++) {
		if(E_OP_VAL==GetOpType(i)) {
			statindex_t st_index = get_state_index((statfieldid_t)i, m_stbuf[i]) ;
			int add_size = snprintf(p, buf_size- (p-in_buf), "%s=%s;" ,s_main_stat_names[i], get_name(st_index) ) ; 
			p+= add_size ;
		}
		else {
			statval_t  v1 = m_stbuf[i] ;
			if (!v1){
				continue;
			}
			int add_size = snprintf(p, buf_size- (p-in_buf), "%s=" ,s_main_stat_names[i] ) ; 
			p+= add_size ;

			while(v1) {
				statval_t tmp = (v1 & (v1-1)) ;
				statval_t index = tmp ^ (v1) ;
				v1 = tmp ;
				statindex_t st_index = get_state_index(i,  index) ;

				add_size = snprintf(p, buf_size- (p-in_buf), " %s" , get_name(st_index) ) ; 
				p+= add_size ;			

			}
			add_size = snprintf(p, buf_size- (p-in_buf), "; " ) ; 
			p+= add_size ;
		}
	}
	return p-in_buf ;
}

size_t StatMachine::GetForbidDesc(char *in_buf, size_t buf_size)
{
	char *p = in_buf ;
	for(int i=0; i<s_forbid_num; i++) {
		if (m_forbid_buf[i]==0)	{
			continue; 
		}

		int add_size = snprintf(p, buf_size- (p-in_buf), "%s=%d;" ,s_forbid_names[i], m_forbid_buf[i] ) ; 
		p+= add_size ;

	}
	return p-in_buf ;
}


//得到操作类型
unsigned char StatMachine::GetOpType(statfieldid_t statid)
{
	return StatMachine::s_stat_valbit[statid] ;
}

//得到状态值
statval_t StatMachine::GetStat(statfieldid_t statid) 
{
	statval_t *p = ref_status(statid) ;
	return p? *p : 0 ;
}


//设置状态值
statval_t StatMachine::SetStat(statfieldid_t statid, statval_t val) 
{
	statval_t old ;
	statval_t *p = ref_status(statid) ;
	if(!p) {
		return 0 ;
	}

	old = *p ;
	if(E_OP_VAL==GetOpType(statid)) {
		if(old != val) {
			*p = val ;
			on_status_clear(statid, old) ;
			on_status_set(statid, val) ;
			OnChange(statid, val)  ;
		}
	}
	else {
		statval_t changed = (val^ *p) & val ;		//得到状态改变的位(如果原因已经是1,则不需要改变)
		statval_t unchang = *p & val ;			//得到没有改变的
		old = *p & val ;
		if(changed) {
			*p |= val ;
			on_status_set(statid, changed) ;
			OnChange(statid, *p)  ;		//通知客户状态已经改变
		}
		if (unchang){
			//把引用计数加1
			while(unchang) {
				statval_t tmp = (unchang & (unchang-1)) ;
				statval_t index = tmp ^ (unchang) ;		//找到最低位是1的位置
				unchang = tmp ;
				statindex_t st_index = get_state_index(statid,  index) ;
				if ((statindex_t)-1==st_index){
					break ;
				}
				++(m_refcount[st_index]);
				if (0==s_stat_need_counter[st_index]){
					m_refcount[st_index] = 1;
				}
			}
		}
	}
	return old ;
}

//用新值替换原来的值(客户端用)
statval_t StatMachine::Replace(statfieldid_t statid, statval_t val) 
{

	statval_t old ;
	statval_t *p = ref_status(statid) ;
	if(!p) {
		return 0 ;
	}

	old = *p ;
	if(E_OP_VAL==GetOpType(statid)) {
		if(old != val) {
			*p = val ;
			on_status_clear(statid, old) ;
			on_status_set(statid, val) ;
			OnChange(statid, val)  ;
		}
	}
	else {
		statval_t change = val ^ old ;
		if(change==0)
			return old;
		//得到被设置的位
		statval_t setbits =  val & change ;
		on_status_set(statid, setbits) ;

		//得到被清除的位
		statval_t clearbits =  old & change ;

		*p = val ;
		on_status_clear(statid, clearbits) ;
		OnChange(statid, val)  ;
	}
	return old ;	
}


//清除状态域的值
statval_t StatMachine::ClearStat(statfieldid_t statid, statval_t val) 
{
	statval_t old, deft_val ;
	statval_t *p = ref_status(statid) ;
	if(!p) {
		return 0 ;
	}

	old = *p ;
	if(E_OP_VAL==GetOpType(statid)) {
		deft_val = GetDefvalue(statid) ;
		if(old == val && deft_val != old) {
			*p = deft_val ;
			on_status_clear(statid, old) ;
			on_status_set(statid, *p);
			OnChange(statid, *p)  ;
		}
	}
	else {
		statval_t changed  = *p & val ;		//得到需要置0的位
		if(changed) {
			statval_t v1 = changed ;
			while(v1) {
				statval_t tmp = (v1 & (v1-1)) ;
				statval_t index = tmp ^ (v1) ;
				v1 = tmp ;
				statindex_t st_index = get_state_index(statid,  index) ;
				if ((statindex_t)-1==st_index){
					break ;
				}
				if ((--m_refcount[st_index]) > 0){
					changed &= ~index ;		//这个位有多次引用,不能清除
				}
			}
			if (changed) {
				*p &= ~ changed;
				on_status_clear(statid, changed) ;
				OnChange(statid, *p)  ;
			}
		}
		old = changed;
	}
	return old ;
}

//检测状态值
bool StatMachine::CheckStat(statfieldid_t statid, statval_t val) 
{
	statval_t *p = ref_status(statid) ;
	if(!p) {
		return false;
	}

	if(E_OP_VAL==GetOpType(statid)) {
		return (val==*p);
	}
	else {
		return (val & *p)? true:false ;
	}
}


bool StatMachine::SetByIndex(statindex_t stindex) 
{
	if (stindex >=s_stat_num){
		return false ;
	}

	int *p = StatMachine::s_stat_index_buf[stindex] ;
	statval_t old = SetStat(p[0], (statval_t)p[1]) ;
	if(old ==m_stbuf[p[0]]) {
		return false ;
	}
	return true;
	
}
bool StatMachine::ClearByIndex(statindex_t  stindex) 
{
	if (stindex >=s_stat_num){
		return false ;
	}

	int *p = StatMachine::s_stat_index_buf[stindex] ;
	statval_t old = ClearStat(p[0], (statval_t)p[1]) ;
	if(old ==m_stbuf[p[0]]) {
		return false ;
	}
	return true ;
}

//增加禁止状态
void StatMachine::IncForbid(int forbid_index ,statindex_t raise_stat) 
{
	forbidval_t *p = ref_forbid(forbid_index) ;
	if(p) {
		++(*p) ;
		if((*p) == 1){
			OnEnterForbid(forbid_index);
		}
		for (int i=0; i<FORBID_CHANGE_TIMES; i++) {
			if(m_forbid_changed_list[forbid_index][i] == (statindex_t)-1) {
				m_forbid_changed_list[forbid_index][i] = raise_stat ;
				break ;
			}
		}
	}
}


//减少禁止状态
void StatMachine::DecForbid(int forbid_index ,statindex_t raise_stat)
{
	forbidval_t *p = ref_forbid(forbid_index) ;
	if(p && *p > 0) {
		--(*p) ;
		if((*p) == 0){
			OnLeaveForbid(forbid_index);
		}
		for (int i=0; i<FORBID_CHANGE_TIMES; i++) {
			if(m_forbid_changed_list[forbid_index][i] == raise_stat) {
				m_forbid_changed_list[forbid_index][i] = (statindex_t)-1 ;
				break ;
			}
		}
	}
}

//检测禁止状态
bool StatMachine::CheckForbid(int statid) 
{
	forbidval_t *p = ref_forbid(statid) ;
	if(p) {
		return (*p)>0 ;
	}
	else 
		return false ;
}

bool StatMachine::CheckForbids(int fields[], int num)
{
	int *p = fields ;
	for(int i=0; i<num ;i++,p++) {
		if(*p>=0 && *p<s_forbid_num) {
			if(m_forbid_buf[*p]>0)
				return true;
		}
	}
	return false;
	
}
bool StatMachine::CheckOperate(int operate_index)
{
	if(operate_index < 0 || operate_index >=s_operate_num) {
		return true;
	}

	int *p = s_operate_check_forbid[operate_index] ;
	if(p[0] >0 ) {
		return (!CheckForbids(p+1, p[0])) ;
	}
	return true;

}



statval_t *StatMachine::ref_status(statfieldid_t statid) 
{
	if(statid>=0 && statid<s_main_stat_num)
		return &m_stbuf[statid] ;
	else 
		return NULL ;
}

forbidval_t *StatMachine::ref_forbid(int statid) 
{
	if(statid>=0 && statid<s_forbid_num)
		return &(m_forbid_buf[statid]) ;
	else 
		return NULL;
}


StatMachine::iterator StatMachine::begin() 
{
	return m_stbuf ;
}

StatMachine::iterator StatMachine::end() 
{
	return &m_stbuf[StatMachine::s_main_stat_num];
}

//状态被设置的回调函数
void StatMachine::on_status_set(statfieldid_t statid, statval_t val) 
{
	statindex_t  st_index;
	if(GetOpType(statid)==E_OP_VAL) {
		st_index = get_state_index(statid,  val) ;
		if ((statindex_t)-1==st_index){
			return ;
		}
		change_forbids_values( st_index, 1) ;
		OnEnter( st_index ) ;
		m_refcount[st_index] = 1 ;		//增加引用计数
		if (s_insted_enter[st_index]){
			s_insted_enter[st_index](this, st_index, s_stat_names[st_index]) ;
		}
	}
	else {
		//找到对应位的禁止列表
		while(val) {
			statval_t tmp = (val & (val-1)) ;
			statval_t index = tmp ^ (val) ;		//找到最低位是1的位置
			val = tmp ;
			st_index = get_state_index(statid,  index) ;
			if ((statindex_t)-1==st_index){
				return ;
			}
			change_forbids_values( st_index,1) ;
			OnEnter( st_index ) ;
			m_refcount[st_index] = 1 ;
			if (s_insted_enter[st_index]){
				s_insted_enter[st_index](this, st_index, s_stat_names[st_index]) ;
			}
		}
	}
}


//状态被清除的回调函数
void StatMachine::on_status_clear(statfieldid_t statid, statval_t val)
{
	statindex_t  st_index;
	if(GetOpType(statid)==E_OP_VAL) {
		st_index = get_state_index(statid,  val) ;
		if ((statindex_t)-1==st_index){
			return ;
		}
		change_forbids_values( st_index, 0) ;
		OnLeave( st_index) ;
		m_refcount[st_index] = 0 ;
		if (s_insted_leave[st_index]){
			s_insted_leave[st_index](this, st_index, s_stat_names[st_index]) ;
		}
	}
	else {
		//找到对应位的禁止列表
		while(val) {
			statval_t tmp = (val & (val-1)) ;
			statval_t index = tmp ^ (val) ;
			val = tmp ;
			
			st_index = get_state_index(statid,  index) ;
			if ((statindex_t)-1==st_index){
				return ;
			}
			change_forbids_values(st_index,0) ;
			OnLeave(st_index) ;
			m_refcount[st_index] = 0 ;
			if (s_insted_leave[st_index]){
				s_insted_leave[st_index](this, st_index, s_stat_names[st_index]) ;
			}
		}
	}
}

//修改禁止列表的值,flag = 1增加 0减少
void StatMachine::change_forbids_values(statindex_t stat_index, int flag)
{
	int num , i;
	int *list = StatMachine::s_stat_forbid_buf[stat_index] ;

	num = list[0] ;
	if(num <= 0) {
		return  ;
	}
	++list ;

	if(flag){
		for(i=0; i<num; i++, list++ ) {
			IncForbid(*list, stat_index) ;
		}
	}
	else {
		for(i=0; i<num; i++, list++ ) {
			DecForbid(*list, stat_index) ;
		}
	}

}

statindex_t StatMachine::get_state_index(statfieldid_t statid, statval_t val) 
{
	for(int i=0; i<s_stat_num; i++) {
		if (StatMachine::s_stat_index_buf[i][0]==statid && 
			StatMachine::s_stat_index_buf[i][1] == (int)val)	{
			return i;
			//return s_stat_index_buf[i][2] ;
		}
	}
	return -1;
}

//得到每个状态值对应的禁止列表
//列表为一个整数数组
//@statid 对应状态域,val状态值,如果是bit操作,val只能是一个bit位(val==1<<n
/*
int* StatMachine::load_forbid_values(int statid, statval_t val, int *num)
{
	int *ret ;
	int index ;
	
	index = get_state_index(statid,  val) ; 
	if(index < 0 || index>=s_stat_num) {
		*num = 0 ;
		return NULL ;
	}
	ret = StatMachine::s_stat_forbid_buf[index] ;

	*num = ret[0] ;
	if(*num <= 0) {
		return NULL ;
	}
	return (ret + 1) ;

}
*/


#define GET_STAT_INDEX(namebuf,name, size)	\
	for (int i=0; i<size; i++){				\
		if (strcmp(namebuf[i],name)==0)	{	\
			return i ;						\
		}									\
	}										\
	return -1 

int StatMachine::get_index(const char *name) 
{
	GET_STAT_INDEX(StatMachine::s_stat_names,name, StatMachine::s_stat_num);
}
int StatMachine::get_forbid_index(const char *name) 
{
	GET_STAT_INDEX(StatMachine::s_forbid_names,name, StatMachine::s_forbid_num);
}

int StatMachine::get_operate_index(const char *name)
{
	GET_STAT_INDEX(StatMachine::s_operate_names,name, StatMachine::s_operate_num);
}

int StatMachine::get_main_index(const char *name)
{
	GET_STAT_INDEX(StatMachine::s_main_stat_names,name, StatMachine::s_main_stat_num);
}
const char *StatMachine::get_name(statindex_t stat_index) 
{
	if (stat_index>=0 && stat_index<s_stat_num)	{
		return s_stat_names[stat_index] ;
	}
	return NULL;
}

const char *StatMachine::get_main_name(const char* substat_name) 
{
	int stindex = StatMachine::get_index(substat_name) ;
	if(stindex==-1 || stindex>=s_stat_num){
		return NULL;
	}
	int *p = StatMachine::s_stat_index_buf[stindex] ;

	if (p[0]>=0 && p[0]<s_main_stat_num)	{
		return s_main_stat_names[p[0]] ;
	}
	return NULL;
}

int GlobalInsStatEntry(const char *st_name, stat_entry func, bool bEnter ) 
{
	int index = StatMachine::get_index(st_name) ;
	if (-1==index){
		return -1 ;
	}
	if (bEnter) {
		StatMachine::s_insted_enter[index] = func ;
	}
	else {
		StatMachine::s_insted_leave[index] = func ;
	}
	return 0 ;
}