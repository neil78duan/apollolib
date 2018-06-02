//
//  login_atlantis.h
//  clientDemo
//
//  Created by duanxiuyun on 14-11-17.
//  Copyright (c) 2014年 duanxiuyun. All rights reserved.
//

#ifndef __clientDemo__login_apollo__
#define __clientDemo__login_apollo__

// 
// #ifndef BUILD_AS_THIRD_PARTY
// #include "ndcli/nd_iconn.h"
// #endif

#include "ndlib.h"
#include "nd_msg.h"
#include "msg_def.h"
#include "apollo_data.h"
//#include "srv_define.h"

// struct ApolloServerInfo
// {
// 	host_list_node host ;
// 	char ip_addr[20] ;
// };
typedef host_list_node ApolloServerInfo;


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
class  LoginBase
{
public:

	virtual void Init(nd_handle hConn, const char * session_filename) = 0;
	virtual void Destroy() = 0;
	//return 0 success ,else return error code
	virtual int Login(const char *userName, const char *passwd, ACCOUNT_TYPE type,int chanelId=0, bool skipAuth=false) = 0;
	virtual int Relogin() = 0;
	virtual int Logout() = 0;
	virtual int TrytoGetCryptKey() =0;
	//int RedirectTo(const char *inet_addr, int port) ;
	virtual int EnterServer(ndip_t ip, NDUINT16 port) = 0;
	virtual int EnterServer(const char *host_name, NDUINT16 port) = 0 ;
	virtual int EnterServer(const char *host_name, NDUINT16 port, const char *session_file) = 0;

	virtual int CreateAccount(account_base_info *acc_info) = 0;
	virtual int GetServerList(ApolloServerInfo *buf, int size) = 0;
	virtual int GetSubHostList(const char *groupEntryHost, NDUINT16 port, ApolloServerInfo *buf, int size)=0;

	virtual int CreateRole(const char *roleName, role_base_info &roleInfo) = 0;
	virtual int GetRoleList(role_base_info role_buf[], int number, NDUINT8 isRelogin=0) = 0;
	virtual int SelectRole(roleid_t rid) =0;

	virtual size_t GetSessionSize() = 0;
	virtual size_t GetSessionData(void *session_buf, size_t buf_size) = 0;
	virtual int ReloginEx(void *session_data, size_t size) = 0;

	virtual int ReadyGame() = 0;

	virtual int GetLastError() = 0;//{return ndGetLastError(m_conn) ;}
	
	virtual NDUINT32 getAccountID() = 0;
	virtual const char *getAccountName() = 0;
	virtual int getServerId() = 0 ;

	static void SetDeviceInfo(const char *udid, const char *devDesc); 

	LoginBase() {}
	virtual~LoginBase(){}
protected:
};

ND_CONNCLI_API LoginBase *ApolloCreateLoginInst();
ND_CONNCLI_API void ApolloDestroyLoginInst(LoginBase *pLogin);

class  LoginApollo : public LoginBase
#else 

class  LoginApollo;
ND_CONNCLI_API LoginApollo *ApolloCreateLoginInst();
ND_CONNCLI_API void ApolloDestroyLoginInst(LoginApollo *pLogin);
//typedef LoginApollo LoginBase;
#define LoginBase LoginApollo
class  LoginApollo 
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
	//bool buildAccountName(ACCOUNT_TYPE type,const char *inputName, char *outName, size_t size);
	
	nd_handle  m_conn ;
	account_index_t m_accIndex;
	NDUINT16 m_sessionID ;
	char *m_session_file ;
	
	NDUINT16 m_serverGroupId;
	NDUINT8 m_accType ;
	NDUINT8 m_accName[ACCOUNT_NAME_SIZE] ;
	static NDUINT8 m_udid[DEVICE_UDID_SIZE] ;
	static NDUINT8 m_deviceDesc[DEVICE_UDID_SIZE];
	//static NDUINT8 m_myLoginToken[ACCOUNT_NAME_SIZE];
	static NDUINT64 m_localToken;
	
//
//	struct {
//		char keymd5[16] ;
//		R_RSA_PUBLIC_KEY pub_key ;
//	} m_srv_key  ;
	char m_srv_key[sizeof(rsa_key_from_srv) + 1024];
};


CPPAPI void myInitAccCreateInfo(account_base_info &acc, int accType, const char *userName, const char *passwd) ;
//ND_CONNCLI_API const char *apollo_error(int errcode);
//CPPAPI int ndSendAndWaitMessage(nd_handle nethandle, nd_usermsgbuf_t *sendBuf, nd_usermsgbuf_t* recvBuf, ndmsgid_t waitMaxid, ndmsgid_t waitMinid, int sendFlag = 0);
//CPPAPI int ndSendAndWaitMessage(NDIConn *conn, NDOStreamMsg &omsg, nd_usermsgbuf_t* recvBuf, ndmsgid_t waitMaxid, ndmsgid_t waitMinid, int sendFlag = 0);
#endif /* defined(__clientDemo__login_atlantis__) */
