/* file gameDBConnector.h
 *
 * define mysql db connect of server
 *
 * create by duan 
 *
 * 2017.8.22
 */
 
#ifndef _GAMEDB_CONNECTOR_H_
#define _GAMEDB_CONNECTOR_H_


#include "db_common/mydatabase.h"
#include "db_common/CommonDataSql.h"

#define CREATE_SQL_MOD(_type,addr, _connector)	\
	do {						\
		addr = new _type ;		\
		if(-1==addr->Create(_connector) ) {\
			delete addr ;		\
			addr = NULL;		\
			nd_logerror("DATABASE INIT ERROR :create " #_type " error\n" ) ;\
			return -1 ;			\
		}						\
	}while(0)


#define DESTROY_SQL_MOD(_type, addr,_connector)\
	if (addr)	{					\
		addr->Destroy() ;			\
		if(m_bReallocSTMT==true){	\
			delete addr ;			\
			addr = NULL;			\
		}							\
	}

enum eDBthmsgErrno
{
	EDBERR_SUCCESS = 0 ,
	EDBERR_SYSTEM_ERR = NDERR_SYSTEM,
	EDBERR_PASSWORD = NDERR_USERNAME,
	EDBERR_NO_ACCOUNT = NDERR_NOUSER,
	EDBERR_NO_ROLE = NDERR_NOUSER,
	EDBERR_ROLE_NAME_EXIST = NDERR_NAME_EXIST,
	EDBERR_TIMEOUT = NDERR_TIMEOUT,
	EDBERR_VERSION = NDERR_VERSION,
	EDBERR_INUSING = NDERR_INUSING,
	EDBERR_UNKNOW = NDERR_UNKNOWN
    
};

class ServerDbConnBase;
class NDDbConnect : public CMyDatabase
{
public:
	NDDbConnect(ServerDbConnBase &host);
	virtual ~NDDbConnect() ;

	int on_connect() ;
	int on_disconnect() ;
	ServerDbConnBase& m_host;
};

class ServerDbConnBase
{
public:
	ServerDbConnBase();
	virtual ~ServerDbConnBase();
	
	int Create(const char *cfgFilename, const char *dbCfgName);
	void Destroy() ;

	virtual int OnCreate(NDDbConnect *dbconnector);
	virtual int OnDestroy(NDDbConnect *dbconnector);
	
    
	int GetLastError() {return m_lasterror;}
	CMyDatabase *GetDBHandle(bool bReadHandler = false);
	
	//int ReConnectDB() ;
	
	int RunSql(const char *sqlstm) ;	
	bool SetReallocSTMT(bool bval);
	void setThread(ndthread_t thid);
	bool checkIsSelect(const char *sqlText);
    
protected:
	bool m_bSpecialReader;
	bool m_bReallocSTMT ;
	int m_lasterror ;
	NDDbConnect m_mydb ;
	NDDbConnect m_readerDB;
	
};

static inline bool check_password(const char *input,const char *passwd)
{
	return ndstricmp((char*)input, (char*)passwd)==0 ;
}

class GameDbConnBase : public ServerDbConnBase
{
public:
	GameDbConnBase() ;
	virtual ~GameDbConnBase() ;
	
	virtual int OnCreate(NDDbConnect *dbconnector);
	virtual int OnDestroy(NDDbConnect *dbconnector);
	
	static GameDbConnBase * get_Instant() ;
	
public:
	
	SaveOutlineMsgSql *m_sqlSaveOutlineMsg;
	CreateCommonDataSql *m_sqlcommonCreate;
	LoadCommonDataSql *m_sqlcommonLoad;
	SaveCommonDataSql *m_sqlcommonSave;
	
	SaveMail *m_sqlMailSave ;
	ReceiveMail *m_sqlMailReceive;
private:
	static GameDbConnBase *s_instant ;
};


extern CreateCommonDataSql *get_CreateCommonDataSql() ;
extern LoadCommonDataSql *get_LoadCommonDataSql();
extern SaveCommonDataSql* get_SaveCommonDataSql();
extern SaveOutlineMsgSql*get_SaveOutlineMsgSql();
extern SaveMail* get_SaveMail();
extern ReceiveMail* get_ReceiveMail() ;

extern ServerDbConnBase* get_DatabaseHandle();

#endif
