/* file mydatabase.cpp
 * wrap mysql database API
 *
 * Create by duan 
 *
 * 2009-5-10
 */

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include "mydatabase.h"
#include "mysql/errmsg.h"

#pragma comment (lib, "libmysql.lib" )

CMyDatabase::CMyDatabase() 
{
	m_port = 0;
	db_handle= 0;
	m_row=0;
	m_result_sql=0;
	m_nOpenstat = 0 ;
	m_nIsneedFree = 0 ;

	m_serverVer=0; 
	m_clientVer = 0;
	m_errno = 0;
	m_fetchResultStmt = 0 ;
#ifdef ND_DEBUG 
	m_fetchResStack[0]=0 ;
	m_myThid = 0 ;
#endif
}

int  CMyDatabase::open_database(const char *host, int port, const char *user, const char* passwd, const char *dbinstance,const char *characterSet )
{
	if (!host || host[0]==0 || !user || user[0]==0) {		
		nd_logerror("Please input host or user \n") ;
		return -1 ;
	}
	m_port = port;
	mysql_init(&m_mysqlinst );
	if (characterSet && *characterSet) {
		mysql_set_character_set(&m_mysqlinst, characterSet);
	}

// 	unsigned int oflag = 360 ;//测试120秒断开
// 	my_bool reconn = 1 ;
// 	mysql_options(&m_mysqlinst, MYSQL_OPT_CONNECT_TIMEOUT,(const void*) &oflag) ;
	//mysql_options(&m_mysqlinst, MYSQL_OPT_RECONNECT, &reconn) ; // --不能使用mysql自动重连因为STMT不会随着自动重连而更新
	
	db_handle =   mysql_real_connect(&m_mysqlinst,host, user, passwd, dbinstance,m_port,NULL,CLIENT_MULTI_RESULTS|CLIENT_MULTI_STATEMENTS);
	if(!db_handle || 0!=mysql_errno(&m_mysqlinst)){		
		db_handle = 0;
		nd_logerror("connect %s %s error \n" AND host AND dbinstance) ;
		if (db_handle) {			
			nd_logerror("mysql error =%s \n" ,mysql_error(&m_mysqlinst)) ;
		}
		return -1 ;
	}
	strncpy(m_host, host, sizeof(m_host)) ;
	strncpy(m_database, dbinstance, sizeof(m_database)) ;
	strncpy(m_user, user, sizeof(m_user)) ;
	strncpy(m_password, passwd, sizeof(m_password)) ;
	if(-1==on_connect() ) {
		mysql_close(&m_mysqlinst);
		db_handle = NULL ;
		nd_logerror("mysql on_connect()  ERROR in open_database() \n") ;
		return -1;
	}
	else {
		nd_logmsg("connect %s %s SUCCESS!  MYSQL server ver=%d client ver=%d\n" 
			AND host AND dbinstance AND get_server_version() AND get_version()) ;
	}
	return 0;
}

int CMyDatabase::on_error_tryto_reconnect(int mysql_error) 
{
	if (mysql_error ==CR_SERVER_GONE_ERROR || mysql_error == CR_SERVER_LOST || 
		mysql_error==CR_CONNECTION_ERROR || mysql_error ==CR_CONN_HOST_ERROR){
		if (0==sql_ping() )	{
			close_database() ;

			nd_logerror("mysql error %d tryto re-connect \n", m_errno);
			return open_database(m_host,m_port, m_user, m_password, m_database) ;
		}
	}
	return -1;
}

int  CMyDatabase::sql_execute(const char *sqlstatement )
{
	nd_assert(m_myThid && m_myThid==nd_thread_self()) ;
	int re_times = 3 ;
	m_errno = 0;
//	if(m_nIsneedFree) {
//		sql_free_result() ;
//	}
	tryto_free_reqult() ;
	
RE_EXE:
	if (!db_handle)	{
		return -1 ;
	}
	if(mysql_query(db_handle, sqlstatement) != 0 ) {
		m_errno = mysql_errno(db_handle) ;
		if (--re_times > 0 && 0==on_error_tryto_reconnect(m_errno) ){
			goto RE_EXE ;
		}
		else {
			nd_logdebug("run sql %s errcode = %d error: %s \n " AND sqlstatement AND m_errno AND mysql_error(db_handle));
		}		
		return -1;
	}

	return 0;
}


int CMyDatabase::sql_executeex(const char * sqlstm,unsigned long length) 
{
	nd_assert(m_myThid && m_myThid==nd_thread_self()) ;
	int re_times = 3 ;
//	if(m_nIsneedFree) {
//		sql_free_result() ;
//	}
	tryto_free_reqult() ;
	
RE_EXE:
	if (!db_handle)	{
		return -1 ;
	}
	if(mysql_real_query(db_handle,sqlstm,length)   != 0 ) {
		m_errno = mysql_errno(db_handle) ;
		if (--re_times > 0 && 0==on_error_tryto_reconnect(m_errno) ){
			goto RE_EXE ;
		}
		else {
			nd_logdebug("run sql %s errcode = %d error: %s \n " AND sqlstm AND m_errno AND mysql_error(db_handle));
		}		
		return -1;
	}
	return 0;
}


int CMyDatabase::sql_num_rows()
{
	if (m_result_sql) {
		return mysql_num_rows(m_result_sql);
	}
	return 0;
}
int CMyDatabase::sql_num_fields()
{
	if (m_result_sql) {
		return mysql_num_fields(m_result_sql);
	}
	return 0;
}

FIELD_INFO CMyDatabase::sql_field_info(int filed_index)
{

	if (m_result_sql) {
		return mysql_fetch_field_direct(m_result_sql, filed_index);
	}
	return 0;
	
}

int  CMyDatabase::sql_affected_rows( )
{
	 return (int) mysql_affected_rows(db_handle);
}	

int CMyDatabase::sql_field_count() 
{
	return (int) mysql_field_count(db_handle);
}

int CMyDatabase::sql_store_result( ) 
{
	nd_assert(m_nIsneedFree==0) ;
	nd_assert(m_result_sql==0) ;
	m_result_sql = mysql_store_result(db_handle);
	if(m_result_sql) {
		m_nIsneedFree = 1 ;
#ifdef ND_DEBUG		
		nd_get_callstack_desc(m_fetchResStack,  sizeof(m_fetchResStack)) ;	
#endif
		return 0 ;
	}
	
	return -1 ;
}

/* 初始化结果集 */
int CMyDatabase::sql_use_result() 
{
	m_result_sql = mysql_use_result(db_handle);
	if(m_result_sql) {
		m_nIsneedFree = 1 ;
		return 0 ;
	}
	return -1 ;
}

ROW_SQL CMyDatabase::sql_fetch( ) 
{
	if(!m_result_sql)
		return NULL ;
	return mysql_fetch_row(m_result_sql) ;
}


unsigned long* CMyDatabase::sql_fetch_lengths()
{
	if(!m_result_sql)
		return NULL ;
	return mysql_fetch_lengths(m_result_sql) ;
}

void CMyDatabase::sql_free_result( ) 
{
	if(!m_result_sql && !m_nIsneedFree)
		return ;
	mysql_free_result(m_result_sql);
	m_result_sql = NULL ;m_nIsneedFree = 0 ;
}


void CMyDatabase::tryto_free_reqult() 
{
	if (m_nIsneedFree) {
		sql_free_result() ;
		//mysql_next_result() ;
#ifdef ND_DEBUG
		nd_logdebug("mysql use result but not free in : %s \n \n", m_fetchResStack) ;	
#endif
	}
	else if(m_fetchResultStmt) {
		stmt_free_result(m_fetchResultStmt) ;
		m_fetchResultStmt = NULL;
#ifdef ND_DEBUG
		nd_logdebug("mysql use result but not free in : %s \n \n", m_fetchResStack) ;	
#endif
	}
	
}


void CMyDatabase::close_database( )
{
	if(!db_handle)
		return ;
	on_disconnect() ;
	mysql_close(&m_mysqlinst);
 	db_handle = NULL;

}

const char * CMyDatabase::sql_error()
{
	if (db_handle){
		return mysql_error(db_handle) ;
	}
	else {
		return "unknown" ;
	}
}

int CMyDatabase::sql_errno() 
{
	if (m_errno ){
		int ret = m_errno  ;
		m_errno = 0;
		return ret ;
	}
	if (db_handle){
		return mysql_errno(db_handle) ;
	}
	else {
		m_errno = CR_SERVER_LOST;
		return m_errno ;
	}
}

int CMyDatabase::sql_ping() 
{
	if(-1==mysql_ping(db_handle) ) {
		m_errno = mysql_errno(db_handle) ;
		nd_logerror("ping mysql server error %d\n", m_errno);
		return -1;
	}
	return 0;
}

unsigned long CMyDatabase::sql_insert_id()
{
	//检查是否时insert语句
	unsigned long used_id ;
	MYSQL_RES *result ;
	if ((result = mysql_store_result(db_handle)) == 0 &&
		mysql_field_count(db_handle) == 0 &&
		mysql_insert_id(db_handle) != 0)
		 
	{
		used_id = (unsigned long)mysql_insert_id(db_handle);
	}
	else {
		used_id = -1 ;
	}
	mysql_free_result(result) ;
	return used_id ;
}

int CMyDatabase::sql_lock_table(  const char *tablename ,int flag )
{ 
	char sql_smt[1024];
	snprintf(sql_smt,1024,"lock table %s  %s ", tablename, (flag) ? "write" : "read" );
	return sql_execute(sql_smt);
}

void CMyDatabase::sql_unlock_table( )
{ 
	sql_execute("unlock table");
}

int CMyDatabase::check_sql_statments(const char *sql_statment) 
{
	while(*sql_statment)
	{
		if(*sql_statment==0x27 || 
			*sql_statment==0x22 ||
			*sql_statment==0x3b)
		{
			return -1 ;
		}
		sql_statment++ ;
	}
	return 0 ;
}

int CMyDatabase::escape_sql_string(char *result_str, char *original_str, unsigned long orginal_len) 
{
	unsigned long data_len = mysql_real_escape_string(db_handle,result_str,original_str, orginal_len) ;
	if(data_len>0) 
		result_str[data_len] = 0 ;
	
	return (int)data_len ;
}

unsigned long CMyDatabase::get_version() 
{
	if(0==m_clientVer )
		m_clientVer= (int)mysql_get_client_version() ;
	return m_clientVer ;
}

unsigned long CMyDatabase::get_server_version() 
{
	if(0==m_serverVer)
		m_serverVer =(int) mysql_get_server_version(db_handle) ;
	return m_serverVer ;
}

int CMyDatabase::set_multi_query(int flag) 
{
	enum_mysql_set_option opt = flag?MYSQL_OPTION_MULTI_STATEMENTS_ON:MYSQL_OPTION_MULTI_STATEMENTS_OFF ;
	if(get_server_version()>=MYDB_VERSION(4,1,1)) {
		return mysql_set_server_option(db_handle,opt) ;
	}
	return -1;
}
int CMyDatabase::set_auto_commit(int flag) 
{
	flag = flag ? 1:0;
	if(get_server_version()>=MYDB_VERSION(4,1,0)) {
		return mysql_autocommit(db_handle,flag) ;
	}
	else {
		return -1 ;
	}
}

int CMyDatabase::sql_commit()
{
	if(get_server_version()>=MYDB_VERSION(4,1,0)) {
		return mysql_commit(db_handle) ;
	}
	else {
		return mysql_query(db_handle, "commit") ;
	}
}

int  CMyDatabase::begin_affairs() 
{
	
	nd_assert(m_myThid && m_myThid==nd_thread_self()) ;
	
	if(get_server_version()>=MYDB_VERSION(4,1,0)) {
		return set_auto_commit(0);
	}
	else {
		return mysql_query(db_handle, "begin") ;
	}	
}

int  CMyDatabase::sql_rollback() 
{
	if(get_server_version()>=MYDB_VERSION(4,1,0)) {
		int ret = 0 ;
		if( mysql_rollback(db_handle) ) {
			ret = -1;
		}
		set_auto_commit(1) ;
		return ret ;
	}
	else {
		return mysql_query(db_handle, "rollback") ;
	}
}
	
int  CMyDatabase::end_affairs() 
{
	if(get_server_version()>=MYDB_VERSION(4,1,0)) {
		int ret = sql_commit() ;
		set_auto_commit(1) ;
		return ret ;
	}
	else {
		return mysql_query(db_handle, "commit") ;
	}
}

//////////////////////////////////////////////////////////////////////////

STMT_HANDLE CMyDatabase::stmt_init(STMT_HANDLE h)
{
	*h = mysql_stmt_init(db_handle) ;
	if(!*h ) {
		char buf[ND_FILE_PATH_SIZE] = "unknow";
		nd_get_callstack_desc(buf, sizeof(buf)) ;

		nd_logerror("%s mysql_stmt_init() error :%s\n" AND buf AND mysql_error(db_handle) ) ;
		return NULL ;
	}
	return h ;
}

int CMyDatabase::stmt_prepare(STMT_HANDLE stmt, const char *query, unsigned long length)
{
	int ret = mysql_stmt_prepare(STMT_HANDLE_TO_CONTEXT(stmt), query, length) ;

	if(0!=ret ) {
		char buf[ND_FILE_PATH_SIZE] = "unknow";
		nd_get_callstack_desc(buf, sizeof(buf)) ;

		nd_logerror("%s run %s error,code = %d sql=%s error :%s\n" AND buf  AND __FUNC__ 
			AND mysql_stmt_errno(STMT_HANDLE_TO_CONTEXT(stmt)) AND query AND 
			mysql_stmt_error(STMT_HANDLE_TO_CONTEXT(stmt)) ) ;
	}
	return ret ;
}

int CMyDatabase::stmt_execute(STMT_HANDLE stmt)
{
	
	nd_assert(m_myThid && m_myThid==nd_thread_self()) ;
	
	int re_times = 2 ;

RE_EXE:
	if (!stmt || !STMT_HANDLE_TO_CONTEXT(stmt)){
		return -1;
	}
	tryto_free_reqult() ;

	if(mysql_stmt_execute(STMT_HANDLE_TO_CONTEXT(stmt)) != 0 ) {		
		m_errno = mysql_stmt_errno(STMT_HANDLE_TO_CONTEXT(stmt));

		
		if (--re_times > 0 && 0==on_error_tryto_reconnect(m_errno) ){
			goto RE_EXE ;
		}
		else {
			char buf[1024] = "unknow";
			nd_get_callstack_desc(buf, sizeof(buf)-1) ;
			nd_logerror("%s mysql_stmt_execute() errno = %d error :%s\n" 
				AND buf AND m_errno AND stmt_error(stmt) ) ;
		}

		return -1;
	}
	return 0 ;
}

int CMyDatabase::stmt_fetch(STMT_HANDLE stmt)
{
	int ret = mysql_stmt_fetch(STMT_HANDLE_TO_CONTEXT(stmt));	
	if (1==ret){
		nd_logerror("run %s error,code = %d error :%s\n" AND __FUNC__ 
			AND mysql_stmt_errno(STMT_HANDLE_TO_CONTEXT(stmt)) AND mysql_stmt_error(STMT_HANDLE_TO_CONTEXT(stmt)) ) ;
		//nd_logerror("mysql_stmt_fetch() error :%s\n" AND mysql_stmt_error(stmt) ) ;
	}
// 	else if(MYSQL_NO_DATA==ret) {
// 		nd_logdebug("mysql_stmt_fetch(): fetch data is NUll\n") ;
// 	}
	else if(MYSQL_DATA_TRUNCATED==ret) {
		nd_logdebug("mysql_stmt_fetch(): data is too long \n" ) ;
	}
	
	return ret ;
}

int CMyDatabase::stmt_store_result(STMT_HANDLE stmt)
{
	int ret = mysql_stmt_store_result(STMT_HANDLE_TO_CONTEXT(stmt)) ;
		
	if(0!=ret ) {		
		nd_logerror(" run %s error,code = %d error :%s\n" AND __FUNC__ 
			AND mysql_stmt_errno(STMT_HANDLE_TO_CONTEXT(stmt)) AND mysql_stmt_error(STMT_HANDLE_TO_CONTEXT(stmt)) ) ;
		//nd_logerror("mysql_stmt_store_result() error :%s\n" AND mysql_stmt_error(stmt) ) ;
	}
	else {
		m_fetchResultStmt = stmt ;
#ifdef ND_DEBUG		
		nd_get_callstack_desc(m_fetchResStack,  sizeof(m_fetchResStack)) ;	
#endif
	}
	return ret ;
}
my_bool CMyDatabase::stmt_free_result(STMT_HANDLE stmt)
{
	nd_assert( m_fetchResultStmt == stmt );
	m_fetchResultStmt = 0 ;
	return mysql_stmt_free_result(STMT_HANDLE_TO_CONTEXT(stmt));
}
my_bool CMyDatabase::stmt_bind_param(STMT_HANDLE  stmt, MYSQL_BIND * bnd)
{
	return mysql_stmt_bind_param( STMT_HANDLE_TO_CONTEXT(stmt),  bnd) ;
}
my_bool CMyDatabase::stmt_bind_result(STMT_HANDLE  stmt, MYSQL_BIND * bnd)
{
	return mysql_stmt_bind_result( STMT_HANDLE_TO_CONTEXT(stmt),  bnd);
}
unsigned long CMyDatabase::stmt_param_count(STMT_HANDLE  stmt)
{
	return mysql_stmt_param_count( STMT_HANDLE_TO_CONTEXT(stmt));
}
unsigned int CMyDatabase::stmt_errno(STMT_HANDLE  stmt)
{
	if (stmt && STMT_HANDLE_TO_CONTEXT(stmt)){
		return mysql_stmt_errno( STMT_HANDLE_TO_CONTEXT(stmt));
	}
	else {
		return CR_SERVER_LOST;
	}
	
}
const char * CMyDatabase::stmt_error(STMT_HANDLE  stmt)
{
	if (stmt && STMT_HANDLE_TO_CONTEXT(stmt)){
		return mysql_stmt_error( STMT_HANDLE_TO_CONTEXT(stmt));
	}
	else {
		return "lost server";
	}	
}
const char * CMyDatabase::stmt_sqlstate(STMT_HANDLE  stmt)
{
	return mysql_stmt_sqlstate( STMT_HANDLE_TO_CONTEXT(stmt));
}
my_ulonglong CMyDatabase::stmt_num_rows(STMT_HANDLE stmt)
{
	return mysql_stmt_num_rows(STMT_HANDLE_TO_CONTEXT(stmt));
}
my_ulonglong CMyDatabase::stmt_affected_rows(STMT_HANDLE stmt)
{
	return mysql_stmt_affected_rows(STMT_HANDLE_TO_CONTEXT(stmt));
}
my_ulonglong CMyDatabase::stmt_insert_id(STMT_HANDLE stmt)
{
	return mysql_stmt_insert_id(STMT_HANDLE_TO_CONTEXT(stmt));
}
unsigned int CMyDatabase::stmt_field_count(STMT_HANDLE stmt)
{
	return mysql_stmt_field_count( STMT_HANDLE_TO_CONTEXT(stmt) );
}

int CMyDatabase::stmt_fetch_column(STMT_HANDLE stmt, MYSQL_BIND *bind_arg,  unsigned int column,unsigned long offset)
{
	int ret = mysql_stmt_fetch_column( STMT_HANDLE_TO_CONTEXT(stmt), bind_arg,   column, offset);

	if(0!=ret ) {
		nd_logerror("run %s error,code = %d error :%s\n" AND __FUNC__ 
			AND mysql_stmt_errno(STMT_HANDLE_TO_CONTEXT(stmt)) AND mysql_stmt_error(STMT_HANDLE_TO_CONTEXT(stmt)) ) ;
		//nd_logerror("mysql_stmt_fetch_column() error :%s\n" AND mysql_stmt_error(stmt) ) ;
	}
	return ret ;
}
my_bool CMyDatabase::stmt_attr_set(STMT_HANDLE stmt, enum enum_stmt_attr_type attr_type, const void *attr)
{
	return mysql_stmt_attr_set( STMT_HANDLE_TO_CONTEXT(stmt),  attr_type,attr);
};
my_bool CMyDatabase::stmt_attr_get(STMT_HANDLE stmt, enum enum_stmt_attr_type attr_type,	 void *attr)
{
	return mysql_stmt_attr_get(STMT_HANDLE_TO_CONTEXT(stmt), attr_type,	 attr);
};
my_bool CMyDatabase::stmt_close(STMT_HANDLE  stmt)
{
	return mysql_stmt_close( STMT_HANDLE_TO_CONTEXT(stmt));
}
my_bool CMyDatabase::stmt_reset(STMT_HANDLE  stmt)
{
	return mysql_stmt_reset( STMT_HANDLE_TO_CONTEXT(stmt));
}
my_bool CMyDatabase::stmt_send_long_data(STMT_HANDLE stmt,  unsigned int param_number, const char *data, unsigned long length)
{
	return mysql_stmt_send_long_data(STMT_HANDLE_TO_CONTEXT(stmt), param_number,data,  length);
}

HANDLE_SQLRESULT CMyDatabase::stmt_result_metadata(STMT_HANDLE stmt)
{
	HANDLE_SQLRESULT ret = mysql_stmt_result_metadata( STMT_HANDLE_TO_CONTEXT(stmt));

	if(NULL == ret ) {
		nd_logerror("run %s error,code = %d error :%s\n" AND __FUNC__ 
			AND mysql_stmt_errno(STMT_HANDLE_TO_CONTEXT(stmt)) AND mysql_stmt_error(STMT_HANDLE_TO_CONTEXT(stmt)) ) ;
		//nd_logerror("mysql_stmt_result_metadata() error :%s\n" AND mysql_stmt_error(stmt) ) ;
	}
	return ret ;
}
HANDLE_SQLRESULT CMyDatabase::stmt_param_metadata(STMT_HANDLE stmt)
{
	return mysql_stmt_param_metadata(STMT_HANDLE_TO_CONTEXT(stmt));
}
MYSQL_ROW_OFFSET CMyDatabase::stmt_row_seek(STMT_HANDLE stmt, 	 MYSQL_ROW_OFFSET offset)
{
	return mysql_stmt_row_seek(STMT_HANDLE_TO_CONTEXT(stmt), offset);
}
MYSQL_ROW_OFFSET CMyDatabase::stmt_row_tell(STMT_HANDLE stmt)
{
	return mysql_stmt_row_tell( STMT_HANDLE_TO_CONTEXT(stmt));
}
void CMyDatabase::stmt_data_seek(STMT_HANDLE stmt, my_ulonglong offset)
{
	return mysql_stmt_data_seek( STMT_HANDLE_TO_CONTEXT(stmt), offset);
}

int CMyDatabase::IdleUpdate() 
{
	/*int ret = sql_ping() ;
	if (-1==ret){
		return on_error_tryto_reconnect(m_errno)  ;
	}	
	return ret;
	*/
	return 0;
}