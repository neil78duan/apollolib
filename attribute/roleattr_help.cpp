/* file warattr_help.cpp
 *
 * war attribute calcutor helper
 *
 * create by duan 
 * 2011/3/14 17:21:26
 */

#include "attribute/roleattr_help.h"
#include "nd_vm/nd_vm.h"
#include "nd_common/nd_common.h"
#include "logic_parser/dbl_mgr.h"
#include "ndapplib/ndsingleton.h"

#include <math.h>

int place_name_entry(char *input, char *buf, int size, void *user_data);

RoleAttrHelper *get_Instant()
{
	static RoleAttrHelper *g_rah;
	if (g_rah) {
		return g_rah;
	}

	g_rah = new RoleAttrHelper();
	return g_rah;
}


RoleAttrHelper::RoleAttrHelper() 
{
	Destroy() ;
}

RoleAttrHelper::~RoleAttrHelper() 
{
}

void RoleAttrHelper::Destroy() 
{
	//m_level_nums = 0 ;
	m_current_parse = 0 ;
	m_wa_num = 0;
	//m_base_num = 0 ;
	memset(&m_run_sort , 0, sizeof(m_run_sort) ) ;
	//memset(m_base_name , 0, sizeof(m_base_name) ) ;
	memset( &m_save_attrs, 0, sizeof(m_save_attrs) ) ;
	memset( &m_sync_attrs, 0, sizeof(m_sync_attrs) ) ;
	memset( m_wahelper_bufs, 0, sizeof(m_wahelper_bufs) ) ;
	//memset(&m_change_byclient, 0, sizeof(m_change_byclient));
	//memset( m_base_name, 0, sizeof(m_base_name) ) ;
	//memset( m_isInRank, 0xff, sizeof(m_isInRank) ) ;
	//memset( m_PEDInRank, 0xff, sizeof(m_PEDInRank) ) ;
	
	m_maxLevel = 0;
	memset(m_upLevelExp, 0, sizeof(m_upLevelExp));

	for(int i=0; i<ROLE_ATTR_CAPACITY; i++) {
		m_wahelper_bufs[i].wa_id = INVALID_ATTR_ID ;
	}
	/*
	for(int i=0; i<BASE_ATTR_CAPACITY; i++) {
		m_base_name[i].wa_id = INVALID_ATTR_ID ;
	}*/
}

int RoleAttrHelper::Init()
{
	return 0;
}


int RoleAttrHelper::loadUplevelExp(const char *file)
{
	const char *pfields[] = { "id", "UplevelExp"};

	m_maxLevel = 0;
	memset(m_upLevelExp, 0, sizeof(m_upLevelExp));

	DBLTable * pTable = DBL_FindTable(file);
	nd_assert(pTable);

	DBLCursor cursor(pTable, pfields, ND_ELEMENTS_NUM(pfields), E_SRC_CODE_ANSI);
	for (int result = cursor.FetchFirst(); result != -1; result = cursor.FetchNext()) {
		NDUINT32 level = cursor[0].GetInt();
		NDUINT32 exp = cursor[1].GetInt();
		if (level && exp && level < MAX_PLAYER_LEVEL){
			m_upLevelExp[level] = exp;
			m_maxLevel = NDMAX(level, m_maxLevel);
		}
	}
	for (int i = 2; i < m_maxLevel + 1; ++i){
		if (m_upLevelExp[i] ==0)	{
			nd_logerror("load up-level exp error UP TO level %d need exp is ZERO\n", i);
			return -1;
		}
	}

	return 0;
}

int RoleAttrHelper::Load(const char *attr_file, const char *up_level_file)
{
	std::string attrmax[ROLE_ATTR_CAPACITY][3];
	//const char *attrmax[ROLE_ATTR_CAPACITY][2] = { 0 };
	const char *pfields[] = { "id", "name", "formula", "save_db", "sync", "min_val", "max_val",			//7
		"client_change", "real_name", "forBuilding", "affect_buildings", "forRole", "affect_card",	"forCard", //14
		"showInGuild", "save_log", "lostOnAttacked", "unlimited_max"};
	
	m_wa_num = 0 ;
	role_attr_description *pwa_desc ;

	int i = 0 ;
	//DBL_BEGIN_CURSOR(attr_file, pfields) {
	DBLTable * pTable = DBL_FindTable(attr_file);
	nd_assert(pTable);
	
	DBLCursor cursor(pTable, pfields, ND_ELEMENTS_NUM(pfields), E_SRC_CODE_ANSI);
	int result = cursor.FetchFirst();
	if (result == -1) {
		nd_logfatal("load file %s error can not fetch role \n", attr_file);
		return -1;
	}
	for (; result != -1; result = cursor.FetchNext()) {
		NDUINT32 aid  = cursor[0].GetInt();
		if (aid > ROLE_ATTR_CAPACITY) {
			nd_logfatal("attr_test: 编号为%d属性超过了%d\n" , aid , ROLE_ATTR_CAPACITY);
			return -1 ;
		}
		pwa_desc = &m_wahelper_bufs[aid] ;

		if ( !cursor[1].CheckValid() )	{
			nd_logfatal("attr_test: 第%d行错误,属性名字为空\n" AND i) ;
			return -1;
		}
		strncpy(pwa_desc->name.attrname, cursor[1].GetText(), ATTR_NAME_SIZE);
		if (cursor[8].CheckValid())	{
			strncpy(pwa_desc->real_name.attrname, cursor[8].GetText(), ATTR_NAME_SIZE);
		}
		else {
			pwa_desc->real_name.attrname[0] = 0;
		}
		

		if ( cursor[2].CheckValid() )
			strncpy(pwa_desc->input_for, cursor[2].GetText(),NDVM_CMD_SIZE );

		pwa_desc->issave = cursor[3].GetInt();
		pwa_desc->issync = cursor[4].GetInt();
		pwa_desc->isChangeByclient = cursor[7].GetInt();

		pwa_desc->forBuilding = cursor[9].GetInt();
		pwa_desc->isSyncBuilding = cursor[10].GetInt();
		pwa_desc->forRole = cursor[11].GetInt();

		pwa_desc->isAffectCard = cursor[12].GetInt();
		pwa_desc->forCard = cursor[13].GetInt();
		pwa_desc->showInGuild = cursor[14].GetInt();
		pwa_desc->islog = cursor[15].GetInt();
		pwa_desc->isLostOnAttacked = cursor[16].GetInt();
		//pwa_desc->issyncai = cursor[7].GetInt();

		pwa_desc->wa_id = (attrid_t ) aid ;
		if (m_wa_num < aid)
			m_wa_num = aid ;

		attrmax[aid][1] = cursor[5].GetString();
		attrmax[aid][0] = cursor[6].GetString();
		attrmax[aid][2] = cursor[17].GetString();
		++i;
	}
	m_wa_num++ ;
	pwa_desc = m_wahelper_bufs;

	for(int i = 0; i < m_wa_num; i++,pwa_desc++){
		if (pwa_desc->wa_id == INVALID_ATTR_ID){
			continue ;	
		}
		parse_minmax(attrmax[i][0].c_str(), attrmax[i][1].c_str(), attrmax[i][2].c_str(), pwa_desc);
	}
	if (ParseFormula()==-1){
		return -1 ;
	}
	
	if (up_level_file)	{
		return loadUplevelExp(up_level_file);
	}
	return 0;
}


//获得需要同步给其他玩家的属性
int RoleAttrHelper::FetchAttr2Others(attrval_t *inbufs,int bufsize, attrval_node *out_nodes, int out_size ) 
{
	int ret = 0 ;
	for(int i=0; i<bufsize  && ret <out_size; i++) {
		if (m_wahelper_bufs[i].issync) {
			out_nodes[ret].id = i ;
			out_nodes[ret].val =inbufs[i] ;
			++ret ;
		}
	}
	

	return ret ;
}

attrid_t RoleAttrHelper::GetID(const char *name) 
{
	if (!name || !*name){
		return INVALID_ATTR_ID;
	}
	role_attr_description  *pwa = m_wahelper_bufs;
	for(int i=0; i<m_wa_num; i++,pwa++ ) {
		if (pwa->wa_id == INVALID_ATTR_ID)	{
			continue;
		}
		else if(0==strcmp(name,pwa->name.attrname)) {
			return pwa->wa_id ;
		}
		else if (0 == ndstricmp(name, pwa->real_name.attrname)) {
			return pwa->wa_id;
		}
	}
	
	return INVALID_ATTR_ID;
}
// 
// attrid_t RoleAttrHelper::GetIDByRealName(const char *name)
// {
// 	role_attr_description  *pwa = m_wahelper_bufs;
// 	for (int i = 0; i < m_wa_num; i++, pwa++) {
// 		if (pwa->wa_id == INVALID_ATTR_ID)	{
// 			continue;
// 		}
// 		else if (pwa->real_name.attrname[0] == 0){
// 			continue;
// 		}
// 		else if (0 == ndstricmp(name, pwa->real_name.attrname)) {
// 			return pwa->wa_id;
// 		}
// 	}
// 	return INVALID_ATTR_ID;
// }

attrid_t RoleAttrHelper::GetIDEx(const char *name,int &MinOrMax)
{
	if (!name || !*name){
		return INVALID_ATTR_ID;
	}
	role_attr_description  *pwa = m_wahelper_bufs;
	MinOrMax = 0 ;
	for(int i=0; i<m_wa_num; i++,pwa++ ) {
		if (pwa->wa_id == INVALID_ATTR_ID)	{
			continue;
		}
		if(0==strncmp(name,pwa->name.attrname, strlen(pwa->name.attrname) )) {
			if (ndstristr((char*)name, "max"))	{
				MinOrMax= 2 ;
			}
			else if (ndstristr((char*)name, "min")) {
				MinOrMax= 1 ;
			}
			else {
				MinOrMax= 0 ;
			}
			return (attrid_t)pwa->wa_id;
		}
	}

	return INVALID_ATTR_ID ;
}

const char *RoleAttrHelper::Getname(attrid_t wa_id)
{
	if (wa_id >= m_wa_num || m_wahelper_bufs[wa_id].wa_id == INVALID_ATTR_ID) {
		return NULL ;
	}
	return m_wahelper_bufs[wa_id].name.attrname ;
}

const char *RoleAttrHelper::GetRealname(attrid_t wa_id)
{
	if (wa_id >= m_wa_num || m_wahelper_bufs[wa_id].wa_id == INVALID_ATTR_ID) {
		return NULL;
	}
	return m_wahelper_bufs[wa_id].real_name.attrname;
}


role_attr_description *RoleAttrHelper::get_wa_desc(attrid_t wa_id)
{
	if (wa_id >= m_wa_num || m_wahelper_bufs[wa_id].wa_id == INVALID_ATTR_ID) {
		return NULL ;
	}
	return &m_wahelper_bufs[wa_id];

}

int RoleAttrHelper::place_param_name(char *input, char *buf, int size)
{
	int i ;
	role_attr_description  *pwa = m_wahelper_bufs;
	for(i=0; i<m_wa_num; i++,pwa++ ) {
		if (pwa->wa_id == INVALID_ATTR_ID){
			continue;
		}
		if(0==strcmp(input,pwa->name.attrname)) {
			snprintf(buf,size,"[%d]", pwa->wa_id ) ;
			if(m_current_parse != i) {
				pwa->infections[(pwa->infection_num)++] =  i ;		//记录属性相关
				m_wahelper_bufs[m_current_parse].need_buf[(m_wahelper_bufs[m_current_parse].need_num)++] = i  ;		//记录属性相关
			}
			return 0 ;
		}
	}
	
	//
	nd_logfatal("公式解析错误:不能找到变量[%s]\n" AND input) ;
	return -1 ;
}

//运行时名字替换
int place_name_runtime(char *input, char *buf, int size,void *user_data) 
{
	int i ;
	RoleAttrHelper *waHelper = (RoleAttrHelper*)user_data;
	role_attr_description  *pwa = waHelper->m_wahelper_bufs;
	for (i = 0; i<waHelper->m_wa_num; i++, pwa++) {
		if (pwa->wa_id == INVALID_ATTR_ID)	{
			continue;
		}
		if(0==strcmp(input,pwa->name.attrname) || 0==ndstricmp(input, pwa->real_name.attrname) ) {
			snprintf(buf,size,"[%d]", pwa->wa_id) ;			
			return 0 ;
		}
	}
	

	nd_logfatal("公式解析错误:不能找到变量[%s]\n" AND input) ;
	return -1 ;
}

int RoleAttrHelper::parse_form(const char *cmd_txt, ndvm_cmd_node *cmdnode) 
{
	if (!cmd_txt || !cmd_txt[0]){
		return -1;
	}
	cmdnode->size = (int)vm_parse_expression((char*)cmd_txt, cmdnode->cmd_buf, NDVM_CMD_SIZE, (vm_param_replace_func)place_name_runtime, this);
	if(cmdnode->size==0) {
		return -1 ;
	}
	return 0;
}

int RoleAttrHelper::ParseFormula() 
{
	//翻译公式
	role_attr_description  *pwa = m_wahelper_bufs;
	for(int i=0; i<m_wa_num; i++,pwa++){
		if (pwa->wa_id == INVALID_ATTR_ID){
			continue; 
		}
		if (pwa->issave){
			m_save_attrs.sort_buf[m_save_attrs.num++] = (attrid_t)pwa->wa_id ;
		}
		if (pwa->issync){
			m_sync_attrs.sort_buf[m_sync_attrs.num++] = (attrid_t)pwa->wa_id ;
		}
		
		//char f_buf[FORMULA_BUF_SIZE] ;
		if(pwa->input_for[0]) {
			m_current_parse = i ;
			size_t size = vm_parse_expression((char*)pwa->input_for, pwa->cmd_data.cmd_buf, NDVM_CMD_SIZE, (vm_param_replace_func)place_name_entry, this);
			if(size==0) {
				nd_logfatal("解析公式%s错误 \n row = %d " AND (char*)pwa->input_for AND i+1) ;
				//show_error("解析公式%s错误 \n row = %d ",vct_warattr[i].str_formula.c_str(), i+2) ;
				return -1 ;
			}
			pwa->cmd_data.size = (int) size;
		}
	}


	if (parse_run_sort()==-1){
		return -1;
	}
#if 0
	float val ;
	role_baseinfo ri = {0};
	RoleAttr roleAttr(NULL,&ri) ;

	pwa = m_wahelper_bufs;
	for(int i=0; i<m_wa_num; i++,pwa++){
		if (pwa->wa_id == INVALID_ATTR_ID){
			continue; 
		}
		//char f_buf[FORMULA_BUF_SIZE] ;
		if(pwa->cmd_data.size > 0) {
			int ret = roleAttr.Runcmd(pwa->cmd_data.cmd_buf,pwa->cmd_data.size,&val, &ri) ;
			nd_assert(ret ==0);
		}
	}
#endif
#ifdef MH_TOOLS

	if(-1==check_attr_associate(vct_warattr) ) 
		return -1 ;
	return check_attr_formula(vct_warattr) ;
#else 
	return 0;
#endif

}


int RoleAttrHelper::parse_minmax(const char *maxval, const char *minval, const char*unlimitedMax, role_attr_description *pwa)
{
	if (maxval && *maxval){
		attrid_t attid = GetID(maxval) ;
		if(INVALID_ATTR_ID!=attid)  {
			pwa->ismax = 2 ;
			pwa->maxval =  (attrval_t )attid ;
		}
		else {
			pwa->ismax = 1 ;
			pwa->maxval =  (attrval_t )atof(maxval) ;
		}
	}
	else {
		pwa->ismax = 0;
	}
	
	if (minval && *minval){
		attrid_t attid = GetID(minval) ;
		if(INVALID_ATTR_ID!=attid)  {
			pwa->ismin = 2 ;
			pwa->minval =  (attrval_t )attid  ;
		}
		else {
			pwa->ismin = 1 ;
			pwa->minval =  (attrval_t )atof(minval) ;
		}
	}
	else {
		pwa->ismin = 0 ;
	}
	
	if (unlimitedMax && *unlimitedMax){
		attrid_t attid = GetID(unlimitedMax);
		if (INVALID_ATTR_ID != attid)  {
			pwa->isUnlimitedMax = 2;
			pwa->unlimitMax = (attrval_t)attid;
		}
		else {
			pwa->isUnlimitedMax = 1;
			pwa->unlimitMax = (attrval_t)atof(maxval);
		}
	}
	else {
		pwa->isUnlimitedMax = 0;
	}

	return 0;
}

int RoleAttrHelper::check_canbe_run(role_attr_description *node, attrid_t *buf)
{
	for(int i=0; i<node->need_num; i++) {
		if(buf[node->need_buf[i]] == 0) {
			return 1 ;
		}
	}
	return 0 ;

}
//解析公式的执行顺序,如果A使用了B,那么应该先运算B
int RoleAttrHelper::parse_run_sort()
{
	int i ;
	int number  = m_wa_num;
	int done = 0 ;

	//int deep = 0 ;

	attrid_t walked[ROLE_ATTR_CAPACITY*2] ;

	memset(walked, 0 , sizeof(walked)) ;

	do {
		done = 0 ; 
		for (i=0; i<m_wa_num; i++){
			if (m_wahelper_bufs[i].wa_id == INVALID_ATTR_ID){
				continue; 
			}

			if(walked[m_wahelper_bufs[i].wa_id]) {
				continue ;
			}
			if(m_wahelper_bufs[i].cmd_data.size > 0) {
				if(0== check_canbe_run(&m_wahelper_bufs[i],walked) ) {
					m_run_sort.sort_buf[m_run_sort.num++] = m_wahelper_bufs[i].wa_id;
					walked[m_wahelper_bufs[i].wa_id] = 1 ;
				}
				else {
					++done ;
				}
			}
			else {
				walked[m_wahelper_bufs[i].wa_id] = 1 ;
			}
		}
	}while(done && (--number > 0) ) ;
	if(number <= 0) {

		nd_logfatal("公式嵌套过深,请检查公式") ;
		//show_error( "公式嵌套过深,请检查公式")  ;
		return -1 ;
	}

	return 0 ;
}

bool RoleAttrHelper::check_save(attrid_t wa_id) 
{
	if (wa_id < ROLE_ATTR_CAPACITY){
		return true ;
	}

	if (wa_id < (attrid_t)m_wa_num){
		if (m_wahelper_bufs[wa_id].wa_id==INVALID_ATTR_ID) {
			return false;
		}
		return m_wahelper_bufs[wa_id].issave? true :false ;
	}
	return false ;

}
bool RoleAttrHelper::check_log(attrid_t wa_id)
{
	if (wa_id < (attrid_t)m_wa_num){
		if (m_wahelper_bufs[wa_id].wa_id == INVALID_ATTR_ID) {
			return false;
		}
		return m_wahelper_bufs[wa_id].islog ? true : false;
	}
	return false;
}

bool RoleAttrHelper::check_readonly(attrid_t wa_id)
{
	if (wa_id < (attrid_t)m_wa_num){
		if (m_wahelper_bufs[wa_id].wa_id == INVALID_ATTR_ID) {
			return true;
		}
		return m_wahelper_bufs[wa_id].isReadOnly ? true : false;
	}
	return false;
}

void RoleAttrHelper::set_readonly(attrid_t wa_id,bool bReadOnly)
{

	if (wa_id < (attrid_t)m_wa_num){
		if (m_wahelper_bufs[wa_id].wa_id == INVALID_ATTR_ID) {
			return ;
		}
		 m_wahelper_bufs[wa_id].isReadOnly = bReadOnly? 1: 0;
	}

}


bool RoleAttrHelper::check_sync(attrid_t wa_id) 
{
	if (wa_id < (attrid_t)m_wa_num){
		if (m_wahelper_bufs[wa_id].wa_id==INVALID_ATTR_ID) {
			return false;
		}
		return m_wahelper_bufs[wa_id].issync? true :false ;
	}
	return false ;
}

bool RoleAttrHelper::check_client_change(attrid_t wa_id)
{
	
	if (wa_id < (attrid_t)m_wa_num){
		if (m_wahelper_bufs[wa_id].wa_id==INVALID_ATTR_ID) {
			return false;
		}
		return m_wahelper_bufs[wa_id].isChangeByclient? true :false ;
	}
	return false ;
}

bool RoleAttrHelper::check_gen_event(attrid_t wa_id)
{
	if (wa_id < (attrid_t)m_wa_num){
		if (m_wahelper_bufs[wa_id].wa_id == INVALID_ATTR_ID) {
			return false;
		}
		return m_wahelper_bufs[wa_id].iscallEvent ? true : false;
	}
	return false;

}


int place_name_entry(char *input, char *buf, int size, void *user_data)
{
	RoleAttrHelper *waHelper = (RoleAttrHelper*)user_data;
	if (!waHelper){
		return -1;
	}

	return waHelper->place_param_name(input, buf, size);
}

/*
NDUINT64 get_uplevel_exp(int grade, int occ ) 
{
    RoleAttrHelper *pwh = get_attr_helper() ;
    if (pwh->m_level_nums <= grade){
        return (NDUINT64)-1 ;
    }
    return pwh->m_levels[grade].need_exp ;
}*/

// 
// RoleAttrHelper *RoleAttrHelper::get_Instant()
// {
// 	static RoleAttrHelper * s_proleAttrhelp;
// 	if (!s_proleAttrhelp){
// 		s_proleAttrhelp = new RoleAttrHelper();
// 	}
// 	return s_proleAttrhelp;
// }
RoleAttrHelper *get_attr_helper()
{
	return  NDSingleton<RoleAttrHelper> ::Get();
}

//通过表查询角色属性ID和列名
int DBL_GetAttrIDName(const char *table, attrid_t attr_ids[], const char *attr_name[], int bufsize) 
{
	RoleAttrHelper *wahelper = get_attr_helper() ;
	DBLDatabase* pdblMgr = DBLDatabase::get_Instant();
	nd_assert(pdblMgr) ;
	DBLTable *pTable = pdblMgr->FindTable(table) ;
	if(!pTable){
		return -1 ;
	}
	DBLCursor cursor(pTable) ;
	int num = 0 ;	
	for(int i=0; i<pTable->GetCols(); i++ ) {

		attrid_t waid = wahelper->GetID(pTable->GetColName(i)) ;
		if (waid != INVALID_ATTR_ID )	{
			attr_name[num] = pTable->GetColName(i) ;
			attr_ids[num] = waid ;
			++num;
			if (num >= bufsize)	{
				return num;
			}
		}
	}
	return num ;
}

int DBL_ReadAttrList(const char *tableName, int index, attr_node_buf &attrbuf)
{
	attrid_t attr_ids[ROLE_ATTR_CAPACITY];
	const char *attr_name[ROLE_ATTR_CAPACITY];
	RoleAttrHelper *pRoleHelp = NDSingleton<RoleAttrHelper>::Get();
	DBLDatabase* pdbl = NDSingleton<DBLDatabase>::Get();

	DBLTable *pTable = pdbl->FindTable(tableName);
	if (!pTable){
		return -1;
	}
	int num = 0;
	for (int i = 0; i < pTable->GetCols(); i++) {

		attrid_t waid = pRoleHelp->GetID(pTable->GetColName(i));
		if (waid != INVALID_ATTR_ID)	{
			attr_name[num] = pTable->GetColName(i);
			attr_ids[num] = waid;
			++num;
			if (num >= ROLE_ATTR_CAPACITY)	{
				break;
			}
		}
	}

	DBLCursor cursor(pTable, attr_name, num, pdbl->GetEncodeType());
	if (-1 == cursor.Fetch(index)) {
		return -1;
	}
	int n = 0;
	for (int i = 0; i < num; i++){
		float val = cursor[i].GetFloat(); 
		if (fabsf(val) > ATTR_VALUE_DETA)	{
			attrbuf.buf[n].val = cursor[i].GetFloat();
			attrbuf.buf[n].id = attr_ids[i];
			++n;
		}
	}
	attrbuf.number = n;
	return n;
}


int Dbl_TableStringToAttr(const DBLDataNode &dataString, attr_node_buf &attrbuf)
{
	RoleAttrHelper *root = get_attr_helper();
	nd_assert(root);

	if (dataString.GetDataType() == OT_ARRAY && dataString.GetArrayType() == OT_STRING) {
		DBLDataNode tmp;
		if (tmp.StringToArrayString(dataString.GetarrayText(0))) {
			for (int i = 0; i < dataString.GetArraySize(); i++)	{
				const char *p = dataString.GetarrayText(i);
				if (!p || !*p)
					continue;
				DBLDataNode node;
				if (node.StringToArrayString(p)) {
					attrid_t aid = root->GetID( node.GetarrayText(0));
					if (aid == INVALID_ATTR_ID) {
						return -1;
					}
					attrval_t val = node.GetarrayFloat(1);
					attrbuf.push_back(aid, val);
				}
			}
		}
		else {
			attrid_t aid = root->GetID(dataString.GetarrayText(0));
			if (aid == INVALID_ATTR_ID) {
				return -1;
			}
			attrval_t val = dataString.GetarrayFloat(1);
			attrbuf.push_back(aid, val);
		}
	}
	return attrbuf.number;
}
