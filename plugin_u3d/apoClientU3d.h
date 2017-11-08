/* file apoClientU3d.h
 *
 * wrapper of apollo-client , implemention of connect to server for unity-3d.
 * 
 * create by duan 
 * 2016.8.18
 */

#ifndef _APO_CLIENT_U3D_H_
#define _APO_CLIENT_U3D_H_


#include "apollo_client.h"

#include "logic_parser/logicEngineRoot.h"
#include "logic_parser/dbl_mgr.h"
#include "ndapplib/ndsingleton.h"

#include <string>


//#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
#if 0
#define APO_EXCEL_DATA_FILE_TMP "cehua.dat.tmp"
#define APO_SCRIPT_FILE_TMP "script.bin.tmp"
#define APO_EXCEL_DATA_FILE_ORG "cehua_data.dat"
#define APO_SCRIPT_FILE_ORG "client_script.bin"
#define APO_EXCEL_DATA_FILE _getWritableFile(APO_EXCEL_DATA_FILE_TMP)
#define APO_SCRIPT_FILE _getWritableFile(APO_SCRIPT_FILE_TMP)
#else
#define APO_EXCEL_DATA_FILE "cehua_data.dat"
#define APO_SCRIPT_FILE "client_script.bin"
#endif 


#define APO_LOGIN_SESSION_FILE "acc_login.sec"
#define APO_LOG_FILE_NAME "apoclient.log"

class ApoClient
{
public:
	ApoClient ();
	~ApoClient();

	static ApoClient *getInstant();
	static void destroyInstant();
	void setWorkingPath(const char *pathText);
	void setLogPath(const char *logPath);
	void SetEnableStreamRecord(bool bEnable) {m_bEnableStreamRecord = bEnable;}

	nd_handle getConn() { return m_pconn; }
	LoginBase *getLoginObj() { return m_login; }
	std::string getPath() { return m_path; }
	DBLDatabase *getExcelDatabase();
#ifndef WITHOUT_LOGIC_PARSER

	LogicParserEngine *getScriptParser();
	LogicEngineRoot *getScriptRoot();
#endif
	////
	RESULT_T Open(const char *host=NULL, int port =0, const char *dev_udid=NULL);
	void Close();
	void ResetConnect();

	RESULT_T ReloginBackground();
	RESULT_T ReloginEx(void *session, size_t sessionSize, bool bReloginOffline);
	
	RESULT_T LoginAccountOneKey(const char *account, const char *passwd, int accType, bool skipAuth);
	RESULT_T LoginOnly(const char *account, const char *passwd, int accType, bool skipAuth);
	RESULT_T CreateAccount(const char *userName, const char *passwd, const char *phone, const char *email);
	RESULT_T CreateOnly(const char *userName, const char *passwd, const char *phone, const char *email);

	RESULT_T SelectServer(const char *host = NULL, int port = 0);
	//int getRoleList(char *xmlBuf, size_t size);
	//RESULT_T selRole(roleid_t rid);
	//RESULT_T createRole(const char *roleName);

	//enter game incluse select server, get rolelist, create role ..enter game
	RESULT_T EnterGame(const char *host=NULL, int port=0) { return _enterGame(host, port, NULL,true); }

	int GetGameAreaList( ApolloServerInfo bufs[], size_t number) ;
	
	//RESULT_T testOneKeyLogin(const char *host, int port, const char *user, const char *passwd,int accType) ;


	void Logout();
	void ClearLoginHistory() ;

	void EnterBackground();
	void LeaveBackground();
	
	bool Update();
	
	int Init();
	void Destroy();
	bool IsLoginOk();

	NDUINT32 GetCurAccId(){ return m_accId; }
	roleid_t GetCurRoleId() { return m_roleId; }

	time_t getServerTime();
	void setServerTime(time_t srvTime);
	NDUINT8 getReloginStat() { return m_isRelogin; }
private:
	RESULT_T TrytoRelogin();
	RESULT_T TrytoReloginEx(void *session, size_t sessionSize);
	void onInit();
	void onLogin();
	RESULT_T _connectHost(const char *host, int port) ;
	void _closeConnect();
	int _trytoOpen();
	//bool _moveFileToWritable();
	//bool _moveFileToWritable(const char *infileName, const char*outFileName);
	//std::string _getWritableFile(const char *file);
	RESULT_T _enterGame(const char *host, int port, const char *roleName,bool bWithoutLoadBalance=false);
	//RESULT_T _login();
	//RESULT_T _roleCreate();
	enum eRunningUpdate
	{
		ERUN_UP_NORMAL=0x126a4fb0,
		ERUN_UP_STOP = 0x8400ff80,
	};
	bool m_bEnableStreamRecord;
	NDUINT32 m_runningUpdate;
	NDUINT32 m_accId;
	roleid_t m_roleId;
	int m_port;
	NDUINT8 m_isRelogin;		// 0 NEW LOGIN, 1 relogin after offline
	//NDIConn *m_pconn;
	nd_handle m_pconn;
	LoginBase *m_login;
	std::string m_logPath;
	std::string m_path;
	std::string m_sessionFile;
	std::string m_accName;
	std::string m_host;
	std::string m_udid ;
	time_t m_serverTm; // server time 
	time_t m_localTm;  //local time when got server tm
	int m_updateIndex;
	
};


#endif
