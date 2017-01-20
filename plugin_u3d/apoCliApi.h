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
#include "ndcli/nd_api_c.h"

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



APOLLO_CLI_API bool apoCli_init(const char *workingPath, const char *logPath);
APOLLO_CLI_API void apoCli_destroy();
APOLLO_CLI_API void* get_NDNetObject();

APOLLO_CLI_API APO_RESULT_T apoCli_open(const char *host, int port, const char *dev_udid);
APOLLO_CLI_API APO_RESULT_T apoCli_send(char *bufferFram, int frameSize);
APOLLO_CLI_API APO_RESULT_T apoCli_sendMsg(int messageId, void *messageBody, int bodySize);
APOLLO_CLI_API int apoCli_recv(char *bufferFram, int bufsize, int timeOutMS);
APOLLO_CLI_API int apoCli_recvMsg(int *messageId, char *msgBody, int bufsize, int timeOutMS);

APOLLO_CLI_API APO_RESULT_T apoCli_ReloginBackground(const char *host, int port, const char *dev_udid);
APOLLO_CLI_API APO_RESULT_T apoCli_TrytoRelogin();
APOLLO_CLI_API APO_RESULT_T apoCli_LoginAccount(const char *account, const char *passwd);
APOLLO_CLI_API APO_RESULT_T apoCli_CreateAccount(const char *userName, const char *passwd, const char *phone, const char *email);
APOLLO_CLI_API APO_RESULT_T apoCli_testOneKeyLogin(const char *host, int port, const char *user, const char *passwd);
APOLLO_CLI_API void apoCli_Logout();
APOLLO_CLI_API void apoCli_ClearLoginHistory();
//APOLLO_CLI_API bool apoCli_Update();
APOLLO_CLI_API NDUINT32 apoCli_GetCurAccId();
APOLLO_CLI_API NDUINT32 apoCli_GetCurRoleId();
//APOLLO_CLI_API time_t apoCli_getServerTime();


#endif /* defined(_APO_CLI_API_H_) */
