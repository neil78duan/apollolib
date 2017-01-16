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

typedef int RESULT_T;
ND_CONNCLI_API bool apoCli_init(const char *workingPath, const char *logPath);
ND_CONNCLI_API void apoCli_destroy();
ND_CONNCLI_API void* apoCli_getConnObj();
ND_CONNCLI_API RESULT_T apoCli_open(const char *host, int port, const char *dev_udid);
ND_CONNCLI_API RESULT_T apoCli_ReloginBackground(const char *host, int port, const char *dev_udid);
ND_CONNCLI_API RESULT_T apoCli_TrytoRelogin();
ND_CONNCLI_API RESULT_T apoCli_LoginAccount(const char *account, const char *passwd);
ND_CONNCLI_API RESULT_T apoCli_CreateAccount(const char *userName, const char *passwd, const char *phone, const char *email);
ND_CONNCLI_API RESULT_T apoCli_testOneKeyLogin(const char *host, int port, const char *user, const char *passwd);
ND_CONNCLI_API void apoCli_Logout();
ND_CONNCLI_API void apoCli_ClearLoginHistory();
ND_CONNCLI_API bool apoCli_Update();
ND_CONNCLI_API NDUINT32 apoCli_GetCurAccId();
ND_CONNCLI_API NDUINT32 apoCli_GetCurRoleId();
ND_CONNCLI_API time_t apoCli_getServerTime();


#endif /* defined(_APO_CLI_API_H_) */
