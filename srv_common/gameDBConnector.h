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


#include "mydatabase.h"
//update rider

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

class GameDbConnBase;
char *GetEncryptPasswd(char *inpasswd, char *outpasswd) ;
class NDDbConnect : public CMyDatabase
{
public:
	NDDbConnect(GameDbConnBase &host);
	virtual ~NDDbConnect() ;

	int on_connect() ;
	int on_disconnect() ;
	GameDbConnBase& m_host;
};

class GameDbConnBase
{
public:
	GameDbConnBase();
	virtual ~GameDbConnBase();
	
	int Create(const char *cfgFilename, const char *dbCfgName);
	void Destroy() ;

	virtual int OnCreate() ;
	virtual int OnDestroy();
	
    
	int GetLastError() {return m_lasterror;}
	CMyDatabase *GetDBHandle() {return &m_mydb ;}
	
	//int ReConnectDB() ;
	
	int RunSql(const char *sqlstm) ;	
	bool SetReallocSTMT(bool bval);
	
    
protected:
	    
	bool m_bReallocSTMT ;
	int m_lasterror ;
	NDDbConnect m_mydb ;
	
};

static inline bool check_password(const char *input,const char *passwd)
{
	return ndstricmp((char*)input, (char*)passwd)==0 ;
}



#endif
