//
//  login_atlantis.h
//  clientDemo
//
//  Created by duanxiuyun on 14-11-17.
//  Copyright (c) 2014 duanxiuyun. All rights reserved.
//

#ifndef __clientDemo__login_apollo__
#define __clientDemo__login_apollo__

#include "loginBase.h"
#include "loginToken.h"

struct rsa_key_from_srv{
	char keymd5[16];
	R_RSA_PUBLIC_KEY pub_key;
};

struct login_session_load{
	NDUINT32 acc_index ;
	NDUINT32 session_size ;
	char session_buf[1024];

	char keymd5[16] ;
	R_RSA_PUBLIC_KEY srv_key;
	char reserved[16];
};

#ifdef BUILD_AS_THIRD_PARTY
class  LoginApollo : public LoginBase
#else 
class  LoginApollo;
#define LoginBase LoginApollo

ND_CONNCLI_API LoginBase *ApolloCreateLoginInst();
ND_CONNCLI_API void ApolloDestroyLoginInst(LoginBase *pLogin);
ND_CONNCLI_API void ApolloSetDeviceInfo(const char *udid, const char *devDesc);
class ND_CONNCLI_CLASS LoginApollo
#endif 
{
public:
	LoginApollo(nd_handle hConn, const char * session_filename = NULL);
	LoginApollo();
	virtual ~LoginApollo() ;


	static void SetDeviceInfo(const char *udid, const char *devDesc);
	static const char *GetLocalToken();

	void Init(nd_handle hConn, const char * session_filename);
	void Destroy();
	//return 0 success ,else return error code
	int Login(const char *userName, const char *passwd, ACCOUNT_TYPE type,int channelId, bool skipAuth);//return -1 if error ESERVER_ERR_NOUSER
	int Relogin() ;
	int Logout() ;
	int TrytoGetCryptKey();

	int EnterServer(ndip_t ip, NDUINT16 port, bool bNotLoadBalance=false) ;
	int EnterServer(const char *host_name, NDUINT16 port, bool bNotLoadBalance = false);
	int EnterServer(const char *host_name, NDUINT16 port, const char *session_file, bool bNotLoadBalance = false);
	
	int CreateAccount(account_base_info *acc_info) ;
	int GetServerList(ApolloServerInfo *buf, int size) ;//get server group list 
	//get gameservers list in the selected logic-group 
	int GetSubHostList(const char *groupEntryHost, NDUINT16 port, ApolloServerInfo *buf, int size);

	int CreateRole(const char *roleName, role_base_info &roleInfo);
	int GetRoleList(role_base_info role_buf[], int number, NDUINT8 isRelogin = 0);
	int SelectRole(roleid_t rid);

	size_t GetSessionSize() ;
	size_t GetSessionData(void *session_buf, size_t buf_size) ;
	int ReloginEx(void *session_data, size_t size);

	int ReadyGame() ;
    
    int GetLastError() ;//{return ndGetLastError(m_conn) ;}
	
	NDUINT32 getAccountID() ;
	const char *getAccountName() ;
	int getServerId();
protected:
	void SetSessionFile(const char *session_filepath);
	int onLogin(NDIStreamMsg &inmsg) ;
	int checkCryptVersion(char *savedVersion) ;
	int switchServer(ndip_t ip, NDUINT16 port, int sendMsg, int waitMsg, bool bNotLoadBalance = false);
	int switchServer(const char *host, NDUINT16 port, int sendMsg, int waitMsg, bool bNotLoadBalance = false);
	int jumptoGame(const char *host, NDUINT16 port) ;
	int relogin(void *tokenBuf, int sendMsgID, int waitMsgID, bool bNotLoadBalance = false);
	int getReloginSessionInfo(login_session_load *outbuf);
	int getLoginToken(login_token_info *outToken);
	
	nd_handle  m_conn ;
	account_index_t m_accIndex;
	NDUINT16 m_sessionID ;
	char *m_session_file ;
	
	NDUINT16 m_serverGroupId;
	NDUINT8 m_accType ;
	NDUINT8 m_accName[ACCOUNT_NAME_SIZE] ;
	
	char m_srv_key[sizeof(rsa_key_from_srv) + 1024];
public:
	static NDUINT8 m_udid[DEVICE_UDID_SIZE] ;
	static NDUINT8 m_deviceDesc[DEVICE_UDID_SIZE];
	static NDUINT64 m_localToken;
};


CPPAPI void myInitAccCreateInfo(account_base_info &acc, int accType, const char *userName, const char *passwd) ;

#endif /* defined(__clientDemo__login_atlantis__) */
