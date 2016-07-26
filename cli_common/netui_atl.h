//
//  netui_atl.h
//  clientDemo
//
//  Created by duanxiuyun on 14-12-2.
//  Copyright (c) 2014年 duanxiuyun. All rights reserved.
//

#ifndef __clientDemo__netui_atl__
#define __clientDemo__netui_atl__

#if !defined(BUILD_AS_THIRD_PARTY)
#include "ndcli/nd_iconn.h"
#endif
#include "ndcli/nd_api_c.h"
#include "apollo_data.h"

ND_CONNCLI_API void initAccCreateInfo(account_base_info &acc, int accType, const char *userName, const char *passwd);
ND_CONNCLI_API int RloginTrylogin(const char*udid, nd_handle h, int accType, const char *userName, const char *passwd, const char *session_file = NULL, NDUINT32 *accID = NULL);

ND_CONNCLI_API int loginAndCreate(const char*udid, nd_handle h, int accType, const char *userName, const char *passwd, const char *save_session_file = NULL, NDUINT32 *accID = NULL);

ND_CONNCLI_API int redirectServer(nd_handle h, const char *host, int port, const char*session_file);

ND_CONNCLI_API int TryLogin(const char*udid, nd_handle h, int accType, const char *userName, const char *passwd, const char *save_session_file = NULL);

ND_CONNCLI_API int TryRegister(const char*udid, nd_handle h, int accType, const char *userName, const char *passwd, const char *save_session_file = NULL);

ND_CONNCLI_API int TryLogout(nd_handle h);

ND_CONNCLI_API int TryReLogin(nd_handle h, const char *save_session_file = NULL);

//get host list when login success
ND_CONNCLI_API int getServerList(nd_handle h, host_list_node *buf, int size);


#endif /* defined(__clientDemo__netui_atl__) */
