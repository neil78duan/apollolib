/* file gameDBConnector.cpp
 *
 * define mysql db connect of server
 *
 * create by duan 
 *
 * 2017.8.22
 */

 #include "srv_common/gameDBConnector.h" 
#include "mysql/errmsg.h"
#include "ndapplib/applib.h"

//////////////////////////////////////////////////////////////////////////
//


NDDbConnect::NDDbConnect(GameDbConnBase &host) : CMyDatabase(), m_host(host) {};
NDDbConnect::~NDDbConnect() {}

int NDDbConnect::on_connect()
{ 
	return m_host.OnCreate();
}

int NDDbConnect::on_disconnect()
{
	return m_host.OnDestroy();
}

//ndatomic_t GameDbConnBase::s_online_num = 0;
//////////////////////////////////////////////////////////////////////////

GameDbConnBase::GameDbConnBase() : m_mydb(*this)
{
	m_lasterror= 0;
	m_bReallocSTMT = false ;
}

GameDbConnBase::~GameDbConnBase() 
{

}

bool GameDbConnBase::SetReallocSTMT(bool bval)
{
	ND_TRACE_FUNC() ;
	bool ret = m_bReallocSTMT ;
	m_bReallocSTMT = bval ;
	return ret ;
}

int GameDbConnBase::Create(const char *cfgFilename, const char *dbCfgName)
{
	ND_TRACE_FUNC() ;
	//WorldInstance &dbInst = get_instance()  ;
	SetReallocSTMT(true);
	//int ret = m_mydb.open_database(dbInst.m_dbhost,dbInst.m_dbuser,dbInst.m_dbpassword,dbInst.m_dbdatabase);
	nd_db_config dbCfg = {0} ;
	int ret = read_dbconfig(cfgFilename, dbCfgName/*"database_config"*/, &dbCfg);
	if (0!=ret) {
		error_exit("read database %s config file %s error  \n" AND dbCfgName, cfgFilename );
	}
	ret = m_mydb.open_database(dbCfg.db_host, dbCfg.port,dbCfg.db_user, dbCfg.db_password,dbCfg.db_database);
	if (-1==ret) {
		error_exit("connect to mysql %s:%s user=%s " AND 
			dbCfg.db_host AND  dbCfg.db_database AND dbCfg.db_user);
	}
	
	m_lasterror = 0 ;
	m_mydb.set_multi_query(1) ;

	SetReallocSTMT(false); //不需要new stmt
	return ret ;
}

void GameDbConnBase::Destroy() 
{
	ND_TRACE_FUNC() ;
	m_lasterror = 0 ;

	SetReallocSTMT(true); //不需要new stmt

	m_mydb.close_database() ;
#if !defined(PG_CREATE_ACCOUNT) && defined(PG_READ_7JOYDB_DIRECT)
	m_accountdb.close_database() ;
#endif
}


int GameDbConnBase::OnCreate()
{
	return 0;
}
int GameDbConnBase::OnDestroy()
{
	return 0;
}



char *GetEncryptPasswd(char *inpasswd, char *outpasswd)
{
	ND_TRACE_FUNC() ;
	char buf[128] ;
	int size = snprintf(buf, sizeof(buf) ,"%s_citrusjoy", inpasswd) ;
	return MD5CryptToStr32(buf,size, outpasswd) ;
}



int GameDbConnBase::RunSql(const char *sqlstm)
{
	ND_TRACE_FUNC() ;
	if(0==m_mydb.sql_execute(sqlstm)) {
		if (0== m_mydb.sql_store_result()){
			m_mydb.sql_free_result() ;
		}
		return 0 ;
	}
	return -1;
}

