//
//  apoCliApi.h
//  clientDemo
//
//  Created by duanxiuyun on 14-12-2.
//  Copyright (c) 2014年 duanxiuyun. All rights reserved.
//

#ifndef _APO_CLI_API_H_
#define _APO_CLI_API_H_

#if !defined(BUILD_AS_THIRD_PARTY)
#include "ndcli/nd_iconn.h"
#endif
//#include "ndcli/nd_api_c.h"

#ifndef APO_RESULT_T 
typedef int  APO_RESULT_T;
#endif 

#if  defined(_MSC_VER )

#ifdef APOLLO_U3D_EXPORTS
#define APOLLO_CLI_API 				CPPAPI  __declspec(dllexport)
#define APOLLO_CLI_CLASS 			__declspec(dllexport)
#else 
#define APOLLO_CLI_API 				CPPAPI __declspec(dllimport)
#define APOLLO_CLI_CLASS 			__declspec(dllimport)
#endif

#else

#define APOLLO_CLI_API 				CPPAPI
#define APOLLO_CLI_CLASS 			
#endif



APOLLO_CLI_API int apoCli_init(const char *workingPath, const char *logPath,const char *udid, const char *devDesc=NULL);

APOLLO_CLI_API void apoCli_destroy();
APOLLO_CLI_API void* get_NDNetObject();
APOLLO_CLI_API int apoCli_CheckConnValid();
APOLLO_CLI_API void apoCli_resetConnector();

APOLLO_CLI_API APO_RESULT_T apoCli_open(const char *host, int port);
APOLLO_CLI_API APO_RESULT_T apoCli_close();
APOLLO_CLI_API APO_RESULT_T apoCli_send(char *bufferFram, int frameSize);
APOLLO_CLI_API APO_RESULT_T apoCli_sendMsg(int messageId, void *messageBody, int bodySize);

APOLLO_CLI_API int apoCli_recv(char *bufferFram, int bufsize, int timeOutMS);
APOLLO_CLI_API int apoCli_recvMsg(int *messageId, char *msgBody, int bufsize, int timeOutMS);

APOLLO_CLI_API int apoCli_getConnStat(); //0 unconnect ,1 connected, 2 login, 3 ingame ,4 GM

//APOLLO_CLI_API APO_RESULT_T apoCli_ReloginBackground();

APOLLO_CLI_API APO_RESULT_T apoCli_ReloginEx(const char *sessionData, int sessionSize, bool bReloginOffline);
APOLLO_CLI_API int apoCli_fetchSessionKey( char *outBuf, int bufsize); //return session key size

//APOLLO_CLI_API APO_RESULT_T apoCli_TrytoRelogin();
APOLLO_CLI_API APO_RESULT_T apoCli_LoginAccount(const char *account, const char *passwd, int accType, int channel,bool skipAuth);
APOLLO_CLI_API APO_RESULT_T apoCli_CreateAccount(const char *userName, const char *passwd, int channel);

//APOLLO_CLI_API APO_RESULT_T apoCli_EnterGame(const char *host, int port);
//APOLLO_CLI_API APO_RESULT_T apoCli_testOneKeyLogin(const char *host, int port, const char *user, const char *passwd);
APOLLO_CLI_API void apoCli_Logout();
APOLLO_CLI_API void apoCli_ClearLoginHistory();
//APOLLO_CLI_API bool apoCli_Update();
APOLLO_CLI_API NDUINT32 apoCli_GetCurAccId();
APOLLO_CLI_API NDUINT32 apoCli_GetCurRoleId();
APOLLO_CLI_API APO_RESULT_T apoCli_GetLastError();

APOLLO_CLI_API void apoCli_EnableRecvLog(int bIsEnable);
APOLLO_CLI_API void apoCli_EnableSendLog(int bIsEnable);

APOLLO_CLI_API void apoCli_EnableStreamRecord();

APOLLO_CLI_API APO_RESULT_T apoCli_LoginOnly(const char *account, const char *passwd, int accType, int channel, bool skipAuth);
APOLLO_CLI_API APO_RESULT_T apoCli_CreateAccountOnly(const char *userName, const char *passwd, int channel);

APOLLO_CLI_API int apoCli_GetServerList(char *buf, int size);//xml
APOLLO_CLI_API APO_RESULT_T apoCli_SelectServer(const char *host, int port); // use select server
APOLLO_CLI_API int apoCli_GetRoleList(char *buf, int size); // xml
APOLLO_CLI_API APO_RESULT_T apoCli_CreateRole(const char *roleName);
APOLLO_CLI_API APO_RESULT_T apoCli_SelectRole(unsigned roleId);

APOLLO_CLI_API int apoCli_SetTimeout(int val);
APOLLO_CLI_API int apoCli_GetRoleBeloneServerId();

APOLLO_CLI_API const char* apoCli_GetLocalToken();

//APOLLO_CLI_API time_t apoCli_getServerTime();


#endif /* defined(_APO_CLI_API_H_) */
