/* file apoClient.h
 *
 * wrapper of apollo-client , implemention of connect to server .
 * 
 * create by duan 
 * 2016.8.18
 */

#ifndef _APO_CLIENT_H_
#define _APO_CLIENT_H_


#include "apollo_client.h"

#include "logic_parser/logicEngineRoot.h"
#include "logic_parser/dbl_mgr.h"
#include "ndapplib/ndsingleton.h"

#include <string>


#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
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

	NDIConn *getConn() { return m_pconn; }
	LoginBase *getLoginObj() { return m_login; }
	DBLDatabase *getExcelDatabase();
	LogicParserEngine *getScriptParser();
	LogicEngineRoot *getScriptRoot();

	////
	RESULT_T Open(const char *host, int port, const char *dev_udid);

	RESULT_T ReloginBackground(const char *host, int port, const char *dev_udid);

	RESULT_T TrytoRelogin();
	RESULT_T LoginAccount(const char *account, const char *passwd);

	RESULT_T CreateAccount(const char *userName, const char *passwd, const char *phone, const char *email);
	RESULT_T createRole(const char *roleName);
	RESULT_T EnterGame(const char *host=NULL, int port=0) { return _enterGame(host, port, NULL); }

	int GetGameAreaList( ApolloServerInfo bufs[], size_t number) ;
	
	RESULT_T testOneKeyLogin(const char *host, int port, const char *user, const char *passwd) ;
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
private:
	void onInit();
	void onLogin();
	RESULT_T _connectHost(const char *host, int port) ;
	int _trytoOpen();
	//bool _moveFileToWritable();
	bool _moveFileToWritable(const char *infileName, const char*outFileName);
	std::string _getWritableFile(const char *file);
	RESULT_T _enterGame(const char *host, int port, const char *roleName);
	//RESULT_T _login();
	//RESULT_T _roleCreate();
	enum eRunningUpdate
	{
		ERUN_UP_NORMAL=0x126a4fb0,
		ERUN_UP_STOP = 0x8400ff80,
	};
	NDUINT32 m_runningUpdate;
	NDUINT32 m_accId;
	roleid_t m_roleId;
	int m_port;
	NDIConn *m_pconn;
	LoginBase *m_login;
	std::string m_path;
	std::string m_accName;
	std::string m_host;
	std::string m_udid ;
	time_t m_serverTm; // server time 
	time_t m_localTm;  //local time when got server tm
	int m_updateIndex;
};


#endif
