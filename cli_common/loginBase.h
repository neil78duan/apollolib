/* file loginBase.h
 *
 * define base api of login 
 *
 * create by duan 
 *
 * 2019.1.29
 */

#ifndef _APOLLO_LOGIN_BASE_H_
#define _APOLLO_LOGIN_BASE_H_


#include "ndlib.h"
#include "nd_msg.h"
#include "msg_def.h"
#include "apollo_data.h"

typedef host_list_node ApolloServerInfo;

#ifdef BUILD_AS_THIRD_PARTY
class  LoginBase
{
public:

	virtual void Init(nd_handle hConn, const char * session_filename) = 0;
	virtual void Destroy() = 0;
	//return 0 success ,else return error code
	virtual int Login(const char *userName, const char *passwd, ACCOUNT_TYPE type, int chanelId = 0, bool skipAuth = false) = 0;
	virtual int Relogin() = 0;
	virtual int Logout() = 0;
	virtual int TrytoGetCryptKey() = 0;
	//int RedirectTo(const char *inet_addr, int port) ;
	virtual int EnterServer(ndip_t ip, NDUINT16 port, bool bNotLoadBalance = false) = 0;
	virtual int EnterServer(const char *host_name, NDUINT16 port, bool bNotLoadBalance = false) = 0;
	virtual int EnterServer(const char *host_name, NDUINT16 port, const char *session_file, bool bNotLoadBalance = false) = 0;

	virtual int CreateAccount(account_base_info *acc_info) = 0;
	virtual int GetServerList(ApolloServerInfo *buf, int size) = 0;
	virtual int GetSubHostList(const char *groupEntryHost, NDUINT16 port, ApolloServerInfo *buf, int size) = 0;

	virtual int CreateRole(const char *roleName, role_base_info &roleInfo) = 0;
	virtual int GetRoleList(role_base_info role_buf[], int number, NDUINT8 isRelogin = 0) = 0;
	virtual int SelectRole(roleid_t rid) = 0;

	virtual size_t GetSessionSize() = 0;
	virtual size_t GetSessionData(void *session_buf, size_t buf_size) = 0;
	virtual int ReloginEx(void *session_data, size_t size) = 0;

	virtual int ReadyGame() = 0;

	virtual int GetLastError() = 0;//{return ndGetLastError(m_conn) ;}

	virtual NDUINT32 getAccountID() = 0;
	virtual const char *getAccountName() = 0;
	virtual int getServerId() = 0;

	static void SetDeviceInfo(const char *udid, const char *devDesc);

	LoginBase() {}
	virtual~LoginBase() {}
protected:
};

ND_CONNCLI_API LoginBase *ApolloCreateLoginInst();
ND_CONNCLI_API void ApolloDestroyLoginInst(LoginBase *pLogin);
ND_CONNCLI_API void ApolloSetDeviceInfo(const char *udid, const char *devDesc);
#endif


#endif 
