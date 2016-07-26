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

class  LoginBase
{
public:

	virtual void InitUdid(const char *udid) = 0;
	virtual void ReInit(nd_handle hConn, const char * session_filename) = 0;
	virtual void InitSessionFile(const char *session_filepath) = 0;
	virtual void Destroy() = 0;
	//return 0 success ,else return error code
	virtual int Login(const char *userName, const char *passwd, ACCOUNT_TYPE type) = 0;
	virtual int Relogin() = 0;
	virtual int Logout() = 0;
	//int RedirectTo(const char *inet_addr, int port) ;
	virtual int EnterServer(ndip_t ip, NDUINT16 port) = 0;
	virtual int EnterServer(const char *host_name, NDUINT16 port, const char *session_file) = 0;

	virtual int CreateAccount(account_base_info *acc_info) = 0;
	virtual int GetServerList(host_list_node *buf, int size) = 0;

	virtual size_t GetSessionSize() = 0;
	virtual size_t GetSessionData(void *session_buf, size_t buf_size) = 0;
	virtual int ReloginEx(void *session_data, size_t size) = 0;

	virtual int ReadyGame() = 0;

	virtual int GetLastError() = 0;//{return ndGetLastError(m_conn) ;}
	
	virtual NDUINT32 getAccountID() = 0;
	virtual const char *getAccountName() = 0;
	

	LoginBase() {}
	virtual~LoginBase(){}
protected:
};


class  LoginApollo : public LoginBase
{
public:
	LoginApollo(nd_handle hConn, bool saveSession = true, const char * session_filename = NULL,const char*udid=NULL);
	LoginApollo();
	virtual ~LoginApollo() ;

	void InitUdid(const char *udid) ;
	void ReInit(nd_handle hConn, const char * session_filename);
	void InitSessionFile(const char *session_filepath) ;
	void Destroy();
	//return 0 success ,else return error code
	int Login(const char *userName, const char *passwd, ACCOUNT_TYPE type) ;
	int Relogin() ;
	int Logout() ;
	//int RedirectTo(const char *inet_addr, int port) ;
	int EnterServer(ndip_t ip, NDUINT16 port) ;
	int EnterServer(const char *host_name, NDUINT16 port,const char *session_file) ;
	
	int CreateAccount(account_base_info *acc_info) ;
	int GetServerList(host_list_node *buf, int size) ;

	size_t GetSessionSize() ;
	size_t GetSessionData(void *session_buf, size_t buf_size) ;
	int ReloginEx(void *session_data, size_t size);

	int ReadyGame() ;
    
    int GetLastError() ;//{return ndGetLastError(m_conn) ;}
	
	NDUINT32 getAccountID() ;
	const char *getAccountName() ;
	
protected:
	int onLogin(NDIStreamMsg &inmsg) ;
	int checkCryptVersion(char *savedVersion) ;
	int switchServer(ndip_t ip, NDUINT16 port,int sendMsg, int waitMsg) ;
	int jumptoGame(NDUINT64 serverid) ;
	int relogin(void *tokenBuf, int sendMsgID, int waitMsgID) ;
	int getReloginSessionInfo(void *tokenBuf) ;
	
	
	nd_handle  m_conn ;
	account_index_t m_accIndex;
	bool m_saveSession ;
	NDUINT16 m_sessionID ;
	char *m_session_file ;
	
	NDUINT8 m_accType ;
	NDUINT8 m_accName[ACCOUNT_NAME_SIZE] ;
	NDUINT8 m_udid[DEVICE_UDID_SIZE] ;
	
	
//
//	struct {
//		char keymd5[16] ;
//		R_RSA_PUBLIC_KEY pub_key ;
//	} m_srv_key  ;
	char m_srv_key[4096] ;
};

ND_CONNCLI_API LoginBase *ApolloCreateLoginInst();
ND_CONNCLI_API void ApolloDestroyLoginInst(LoginBase *pLogin);

ND_CONNCLI_API const char *atlantis_error(int errcode);
CPPAPI int ndSendAndWaitMessage(nd_handle nethandle, nd_usermsgbuf_t *sendBuf, nd_usermsgbuf_t* recvBuf, ndmsgid_t waitMaxid, ndmsgid_t waitMinid, int sendFlag = 0);
#endif /* defined(__clientDemo__login_atlantis__) */
