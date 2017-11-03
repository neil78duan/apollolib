/* file gameDBConnector.cpp
 *
 * define mysql db connect of server
 *
 * create by duan 
 *
 * 2017.8.22
 */

 #include "db_common/gameDBConnector.h" 
#include "mysql/errmsg.h"
#include "ndapplib/applib.h"

//////////////////////////////////////////////////////////////////////////
//


NDDbConnect::NDDbConnect(ServerDbConnBase &host) : CMyDatabase(), m_host(host) {};
NDDbConnect::~NDDbConnect() {}

int NDDbConnect::on_connect()
{ 
	return m_host.OnCreate(this);
}

int NDDbConnect::on_disconnect()
{
	return m_host.OnDestroy(this);
}

//ndatomic_t ServerDbConnBase::s_online_num = 0;
//////////////////////////////////////////////////////////////////////////

ServerDbConnBase::ServerDbConnBase() : m_mydb(*this), m_readerDB(*this)
{
	m_bSpecialReader = 0;
	m_lasterror= 0;
	m_bReallocSTMT = false ;
}

ServerDbConnBase::~ServerDbConnBase()
{
	Destroy();
}

bool ServerDbConnBase::SetReallocSTMT(bool bval)
{
	ND_TRACE_FUNC() ;
	bool ret = m_bReallocSTMT ;
	m_bReallocSTMT = bval ;
	return ret ;
}

void ServerDbConnBase::setThread(ndthread_t thid)
{
	if (m_bSpecialReader)	{
		m_readerDB.set_thread(thid);
	}
	m_mydb.set_thread(thid);
}

int ServerDbConnBase::Create(const char *cfgFilename, const char *dbCfgName)
{
	ND_TRACE_FUNC() ;
	SetReallocSTMT(true);
	nd_db_config dbCfg = {0} ;
	int ret = read_dbconfig(cfgFilename, dbCfgName, &dbCfg);
	if (0!=ret) {
		error_exit("read database %s config file %s error  \n" AND dbCfgName, cfgFilename );
	}


	if (dbCfg.special_read_port > 0){
		m_bSpecialReader = true;
		ret = m_readerDB.open_database(dbCfg.db_host, dbCfg.special_read_port, dbCfg.db_user, dbCfg.db_password, dbCfg.db_database);
		if (-1== ret) {
			m_bSpecialReader = false;
			error_exit("connect to mysql %s:%s user=%s " AND
				dbCfg.db_host AND  dbCfg.db_database AND dbCfg.db_user);
		}
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

void ServerDbConnBase::Destroy()
{
	ND_TRACE_FUNC() ;
	m_lasterror = 0 ;
	if (m_mydb.get_mysql_handle()) {

		SetReallocSTMT(true); //不需要new stmt

		m_mydb.close_database() ;
	}

	if (m_bSpecialReader)	{
		m_readerDB.close_database();
	}

}


CMyDatabase *ServerDbConnBase::GetDBHandle(bool bReadHandler ) 
{ 
	if (bReadHandler)	{
		return &m_readerDB;
	}
	return &m_mydb;
}


int ServerDbConnBase::OnCreate(NDDbConnect *dbconnector)
{
	return 0;
}
int ServerDbConnBase::OnDestroy(NDDbConnect *dbconnector)
{
	return 0;
}



int ServerDbConnBase::RunSql(const char *sqlstm)
{
	ND_TRACE_FUNC() ;
	if (strlen(sqlstm) <= 10){
		return -1;
	}
	CMyDatabase *dbhandle = NULL;

	if (m_bSpecialReader){
		dbhandle = GetDBHandle(checkIsSelect(sqlstm));
	}
	else {
		dbhandle = &m_mydb;
	}


	if (0 == dbhandle->sql_execute(sqlstm)) {
		if (0 == dbhandle->sql_store_result()){
			dbhandle->sql_free_result();
		}
		return 0 ;
	}
	return -1;
}

bool ServerDbConnBase::checkIsSelect(const char *sqlText)
{
	char *p = (char *) ndstr_first_valid(sqlText);
	if (!p || !*p)	{
		return false; 
	}
	
	char tmpback =  p[6];
	p[6] = 0;
	bool ret = (0 == ndstricmp(p, "select"));
	p[6] = tmpback;
	return ret;
}

////////////////////////////////////////////////

GameDbConnBase *GameDbConnBase::s_instant = NULL;
GameDbConnBase * GameDbConnBase::get_Instant()
{
	return s_instant ;
}

GameDbConnBase::GameDbConnBase()
{
	s_instant = this ;
}
GameDbConnBase::~GameDbConnBase()
{
	s_instant= NULL ;
}


int GameDbConnBase::OnCreate(NDDbConnect *dbconnector)
{
	ND_TRACE_FUNC() ;
	
	if (m_bSpecialReader) {
		if (dbconnector == &m_mydb)	{
			CREATE_SQL_MOD(SaveOutlineMsgSql, m_sqlSaveOutlineMsg, dbconnector);
			CREATE_SQL_MOD(CreateCommonDataSql, m_sqlcommonCreate, dbconnector);
			CREATE_SQL_MOD(SaveCommonDataSql, m_sqlcommonSave, dbconnector);
			CREATE_SQL_MOD(SaveMail, m_sqlMailSave, dbconnector);
			CREATE_SQL_MOD(ReceiveMail, m_sqlMailReceive, dbconnector);
		}
		else {
			CREATE_SQL_MOD(LoadCommonDataSql, m_sqlcommonLoad, dbconnector);
		}
	}
	else {
		CREATE_SQL_MOD(SaveOutlineMsgSql, m_sqlSaveOutlineMsg, dbconnector);
		CREATE_SQL_MOD(CreateCommonDataSql, m_sqlcommonCreate, dbconnector);
		CREATE_SQL_MOD(LoadCommonDataSql, m_sqlcommonLoad, dbconnector);
		CREATE_SQL_MOD(SaveCommonDataSql, m_sqlcommonSave, dbconnector);
		CREATE_SQL_MOD(SaveMail, m_sqlMailSave, dbconnector);
		CREATE_SQL_MOD(ReceiveMail, m_sqlMailReceive, dbconnector);
	}
	return 0;
}

int GameDbConnBase::OnDestroy(NDDbConnect *dbconnector)
{
	ND_TRACE_FUNC() ;
	
	
	
	if (m_bSpecialReader) {
		if (dbconnector == &m_mydb)	{
			DESTROY_SQL_MOD(SaveOutlineMsgSql, m_sqlSaveOutlineMsg, dbconnector);
			DESTROY_SQL_MOD(CreateCommonDataSql, m_sqlcommonCreate, dbconnector);
			DESTROY_SQL_MOD(SaveCommonDataSql, m_sqlcommonSave, dbconnector);
			DESTROY_SQL_MOD(SaveMail, m_sqlMailSave, dbconnector);
			DESTROY_SQL_MOD(ReceiveMail, m_sqlMailReceive, dbconnector);
		}
		else {

			DESTROY_SQL_MOD(LoadCommonDataSql, m_sqlcommonLoad, dbconnector);
		}
	}
	else {
		DESTROY_SQL_MOD(SaveOutlineMsgSql, m_sqlSaveOutlineMsg, dbconnector);
		DESTROY_SQL_MOD(CreateCommonDataSql, m_sqlcommonCreate, dbconnector);
		DESTROY_SQL_MOD(LoadCommonDataSql, m_sqlcommonLoad, dbconnector);
		DESTROY_SQL_MOD(SaveCommonDataSql, m_sqlcommonSave, dbconnector);
		DESTROY_SQL_MOD(SaveMail, m_sqlMailSave, dbconnector);
		DESTROY_SQL_MOD(ReceiveMail, m_sqlMailReceive, dbconnector);
	}
	return 0;
}



ServerDbConnBase* get_DatabaseHandle()
{
	return GameDbConnBase::get_Instant();
}
CreateCommonDataSql *get_CreateCommonDataSql()
{
	if (GameDbConnBase::get_Instant()) {
		return GameDbConnBase::get_Instant()->m_sqlcommonCreate ;
	}
	return NULL ;
}
LoadCommonDataSql *get_LoadCommonDataSql()
{
	if (GameDbConnBase::get_Instant()) {
		return GameDbConnBase::get_Instant()->m_sqlcommonLoad ;
	}
	return NULL ;

}
SaveCommonDataSql* get_SaveCommonDataSql()
{
	if (GameDbConnBase::get_Instant()) {
		return GameDbConnBase::get_Instant()->m_sqlcommonSave ;
	}
	return NULL ;

}
SaveOutlineMsgSql*get_SaveOutlineMsgSql()
{
	if (GameDbConnBase::get_Instant()) {
		return GameDbConnBase::get_Instant()->m_sqlSaveOutlineMsg ;
	}
	return NULL ;

}
SaveMail* get_SaveMail()
{
	if (GameDbConnBase::get_Instant()) {
		return GameDbConnBase::get_Instant()->m_sqlMailSave ;
	}
	return NULL ;

	
}
ReceiveMail* get_ReceiveMail()
{
	if (GameDbConnBase::get_Instant()) {
		return GameDbConnBase::get_Instant()->m_sqlMailReceive ;
	}
	return NULL ;

}




