/* file mydatabase.h
*
* wrap mysql database API
*
* Create by duan 
*
* 2009-5-10
*/

#ifndef _MYDATABASE_H_
#define _MYDATABASE_H_

#include "nd_common/nd_common.h"
#include "mysql/mysql.h"
#include "mysql/errmsg.h"

// 
// typedef struct struct_db{
// 	MYSQL *handle_sql ;
// }*HANDLE_DB ;		

typedef MYSQL_RES *HANDLE_SQLRESULT ;		//结果集
typedef MYSQL_ROW ROW_SQL ;					//结果集的行指针

typedef MYSQL_STMT *STMT_CONTEXT ;
typedef MYSQL_STMT **STMT_HANDLE ;
typedef MYSQL *HANDLE_DB;

#define STMT_CONTEXT_T0_HANDLE(_context) (&(_context))
#define STMT_HANDLE_TO_CONTEXT(_handle) (*(_handle))

//major_version*10000 + minor_version *100 + sub_version
#define MYDB_VERSION(a,b,c) ((a)*10000 + (b)*100 +(c))
//定义数据库操作的接口实例
//数据库访问是不可重入的, reentry disable 
class CMyDatabase
{
private:
	CMyDatabase(CMyDatabase &);		//prevent copy construct
public :
	CMyDatabase() ;
	virtual ~CMyDatabase() {}
	virtual int on_connect() {return 0;}
	virtual int on_disconnect() {return 0;}
	//int Create() {return open_database()}
#ifdef ND_DEBUG 
	void set_thread(ndthread_t thid) {m_myThid = thid;}
#else
	void set_thread(ndthread_t thid) { }
#endif
private :

	MYSQL m_mysqlinst ;
	HANDLE_DB db_handle ;
	ROW_SQL m_row ;
	HANDLE_SQLRESULT m_result_sql ;
	int m_nOpenstat ;
	int m_nIsneedFree ;
	STMT_HANDLE m_fetchResultStmt ;
	int m_serverVer, m_clientVer ;
	int m_errno ;
	
	int m_port;
	char m_host[128],m_database[128],m_user[32],m_password[32] ;
	
	int on_error_tryto_reconnect(int mysql_error) ;
	void tryto_free_reqult() ;
#ifdef ND_DEBUG 
	char m_fetchResStack[1024] ;
	ndthread_t m_myThid ;
#endif
public:
	MYSQL *get_mysql_handle() {return db_handle;}
	/* 打开数据库 on error return -1  */
	int open_database(const char *host, 		/*数据库服务器*/
						int port ,
						const char *user, 		/*用户名*/
						const char* passwd, 	/* 密码*/
						const char *dbinstance);/*数据库实例*/						
	
	/*得到客户端的版本号*/
	/* for mysql
	Returns an integer that represents the client library version. 
	The value has the format XYYZZ where X is the major version, 
	YY is the release level, and ZZ is the version number within the release level. 
	For example, a value of 40102 represents a client library version of 4.1.2. 
	This function was added in MySQL 4.0.16. 
	*/
	unsigned long get_version() ;

	/*得到服务器端版本号*/
	unsigned long get_server_version() ;

	/*打开多个查询功能
	 *flag = 1 support multi query 0 single query
	 * return 0 success!
	*/
	int set_multi_query(int flag=1) ;

	/*查询数据库 hconnect： 数据库句柄
	 * return 0 success !
	 */
	int sql_execute(const char *sqlstatement );
	
	int sql_executeex(const char * sqlstm,unsigned long length) ;

	unsigned long* sql_fetch_lengths() ;
	/* 存储结果集 */
	int sql_store_result() ;

	/* 初始化结果集 */
	int sql_use_result() ;
	/* 提取结果集，每次一个记录
	 * return NULL ,没有取到结果
	 */
	ROW_SQL sql_fetch() ;
	/* 结果集合中的记录数 */
	int sql_affected_rows();
	/*返回select时得到的记录*/
	

	 /*得到结果集中的记录数,在mysql_store_result() 后使用*/
	 int sql_field_count() ;
	/*释放结果集*/
	 void sql_free_result() ;
	/* 关闭数据库 */
	 void close_database();
	/* 返回最近一次操作错误描述 */
	 const char * sql_error();
	 int sql_errno() ;
	 /*返回最后一次insert 时AUTO_INCREMENT  字段的值*/
	 unsigned long sql_insert_id() ;
	
	 int sql_ping() ;

	/* 锁表
	 * flag = 0 read lock 
	 * flag = 1 write lock 
	 * 锁表成功返回0 
	 * 如果不成功进程将被阻塞
	 * 建议正常情况下不要使用，
	 * 锁表之后应该立即释放
	 * 
	 * NOTE ！！！！！！！！！！！！！！！！！
	 * 锁表是在进程的级别上上锁的，如果当前进程锁住表，其他进程不能释放，也不能访问
	 * 使用时请务必注意
	 */
	 int sql_lock_table(  const char *tablename, int flag );
	 void sql_unlock_table( );
	/* 申明需要一个事务
	 * 使用方法和sql_lock_table差不多，在概念上基本一致，
	 * 使用时一定要小心
	 * 
	 * 目前的MySQL. 的六种表类型中有两种支持事务操作的表.
	 * 类型BDB, InnoDB，还有四种不支持事务的. 表类型(HEAP, ISAM, MERGE, and MyISAM)
	 */
	 int begin_affairs( ) ;
	/* 结束事务*/
	 int end_affairs( ) ;
	/* 回滚事务*/
	 int sql_rollback( ) ;
	 
	/*设置自动提交还是手动提交
	 * flag = 1 自动, flag= 0 手动
	 * return 0 success
	*/
	int set_auto_commit(int flag=1) ;

	int sql_commit() ;

	/* 检查sql语法是否含有非法字符串
	 * 主要是过滤掉诸如 ; ' "等字符
	 * return value : return 0 ,没有非法字符
	 * return -1 含有非法字符
	 */
	 int check_sql_statments(const char *sql_statment) ;
	/* 字符转义，将一些特殊字符转换成数据支持的字符
	 * db_handle 数据库句柄
	 * result_str 结果字符串
	 * original_str 需要转换字符串
	 * orginal_len 字符川original_str 的长度
	 * return value : result_str 字符的长度， result_str 有结尾 '\0'
	 */
	 int escape_sql_string(  char *result_str, char *original_str, unsigned long orginal_len) ;
	 
	 //stm 
	 STMT_HANDLE stmt_init(STMT_HANDLE h);
	 int stmt_prepare(STMT_HANDLE stmt, const char *query, unsigned long length);
	 int stmt_execute(STMT_HANDLE stmt);
	 int stmt_fetch(STMT_HANDLE stmt);
	 my_bool stmt_free_result(STMT_HANDLE stmt);
	 int stmt_store_result(STMT_HANDLE stmt);
	 my_bool stmt_bind_param(STMT_HANDLE  stmt, MYSQL_BIND * bnd);
	 my_bool stmt_bind_result(STMT_HANDLE  stmt, MYSQL_BIND * bnd);
	 unsigned long stmt_param_count(STMT_HANDLE  stmt);
	 unsigned int stmt_errno(STMT_HANDLE  stmt);
	 const char * stmt_error(STMT_HANDLE  stmt);
	 const char * stmt_sqlstate(STMT_HANDLE  stmt);
	 my_ulonglong stmt_num_rows(STMT_HANDLE stmt);
	 my_ulonglong stmt_affected_rows(STMT_HANDLE stmt);
	 my_ulonglong stmt_insert_id(STMT_HANDLE stmt);
	 unsigned int stmt_field_count(STMT_HANDLE stmt);

	 int stmt_fetch_column(STMT_HANDLE stmt, MYSQL_BIND *bind_arg,  unsigned int column,unsigned long offset);
	 my_bool stmt_attr_set(STMT_HANDLE stmt, enum enum_stmt_attr_type attr_type, const void *attr);
	 my_bool stmt_attr_get(STMT_HANDLE stmt, enum enum_stmt_attr_type attr_type,	 void *attr);
	 my_bool stmt_close(STMT_HANDLE  stmt);
	 my_bool stmt_reset(STMT_HANDLE  stmt);
	 my_bool stmt_send_long_data(STMT_HANDLE stmt,  unsigned int param_number, const char *data, unsigned long length);
	 HANDLE_SQLRESULT stmt_result_metadata(STMT_HANDLE stmt);
	 HANDLE_SQLRESULT stmt_param_metadata(STMT_HANDLE stmt);
	 MYSQL_ROW_OFFSET stmt_row_seek(STMT_HANDLE stmt, 	 MYSQL_ROW_OFFSET offset);
	 MYSQL_ROW_OFFSET stmt_row_tell(STMT_HANDLE stmt);
	 void stmt_data_seek(STMT_HANDLE stmt, my_ulonglong offset);

	 int IdleUpdate() ;
	 
};

class SqlBindStm
{
public:
	SqlBindStm() :m_dbhandle(0),m_stmt(0), m_stm_context(0){}
	virtual ~SqlBindStm() {}
	
	virtual int Create(CMyDatabase *dbhandle) = 0 ;
	virtual void Destroy() 
	{ 
		mysql_stmt_close(STMT_HANDLE_TO_CONTEXT(m_stmt)) ;
		m_stmt = 0 ;
		m_stm_context = 0 ;
	}
	int stmt_execute() {return m_dbhandle->stmt_execute(m_stmt);}
	int start_result(MYSQL_BIND *bindbuf)
	{
		m_dbhandle->stmt_bind_result(m_stmt, bindbuf) ;
		if (m_dbhandle->stmt_store_result(m_stmt)){
			return -1;
		}
		return 0 ;
	}
	int end_result() 
	{
		m_dbhandle->stmt_free_result(m_stmt) ;
		return 0 ;
	}
	
protected:
	CMyDatabase *m_dbhandle ;
	STMT_HANDLE  m_stmt;
	STMT_CONTEXT m_stm_context ;
};

#define SET_BIND_CONTEXT(bindcont, _type, _data,_buflen, isnull_addr, len_addr ) \
	(bindcont)->buffer_type= _type;				\
	(bindcont)->buffer= (char *)(_data);			\
	(bindcont)->buffer_length= _buflen;			\
	(bindcont)->is_null= isnull_addr;			\
	(bindcont)->length=(unsigned long*)( len_addr)

#define PREPARE_STM(_stm, _dbhandle,_sql_text, _bind_buf,_field_num)		\
do {										\
	_stm = _dbhandle->stmt_init(STMT_CONTEXT_T0_HANDLE(m_stm_context)) ;			\
	if (!_stm){								\
		nd_logerror("stmt_init() error\n") ;\
		return -1 ;							\
	}										\
	if (_dbhandle->stmt_prepare(_stm, _sql_text,(unsigned long) strlen(_sql_text))) {	\
		nd_logerror("stmt_prepare() error\n") ;\
		return -1 ;							\
	}										\
	if(_field_num > 0) {					\
		int count =(int) _dbhandle->stmt_param_count(_stm);	\
		if (count != _field_num) {			\
			nd_logerror("stmt_param_count() != field number\n") ;\
			return -1 ;						\
		}									\
		if(_dbhandle->stmt_bind_param(_stm, _bind_buf)) {\
			nd_logerror("stmt_bind_param() :%d %s \n" AND _dbhandle->stmt_errno(_stm) AND _dbhandle->stmt_error(_stm)) ;\
			return -1;						\
		}									\
	}										\
} while (0)

#endif



