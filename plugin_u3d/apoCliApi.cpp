//
//  netui_atl.cpp
//  clientDemo
//
//  Created by duanxiuyun on 14-12-2.
//  Copyright (c) 2014 duanxiuyun. All rights reserved.
//

#include "nd_common/nd_common.h"
#include "cli_common/login_apollo.h"
#include "apollo_errors.h"
#include "cli_common/netui_atl.h"
#include "ndcli/nd_api_c.h"
#include "msg_def.h"

#include "apoClientU3d.h"
#include "apollo_errors.h"

bool apoCli_init(const char *workingPath, const char *logPath)
{
	if (ApoClient::getInstant()) {
		return true;
	}
	return false;
}
void apoCli_destroy()
{
	ApoClient::destroyInstant();
}

void* get_NDNetObject()
{
	ApoClient *apoCli = ApoClient::getInstant();
	if (apoCli)	{
		return (void*) apoCli->getConn();
	}
	return NULL;
}
RESULT_T apoCli_open(const char *host, int port, const char *dev_udid)
{
	ApoClient *apoCli = ApoClient::getInstant();
	if (!apoCli)	{
		return NDSYS_ERR_NOT_INIT;
	}

	return apoCli->Open(host, port, dev_udid);
}

RESULT_T apoCli_ReloginBackground(const char *host, int port, const char *dev_udid)
{
	ApoClient *apoCli = ApoClient::getInstant();
	if (!apoCli)	{
		return NDSYS_ERR_NOT_INIT;
	}
	return apoCli->ReloginBackground(host, port, dev_udid);
}

RESULT_T apoCli_TrytoRelogin()
{
	ApoClient *apoCli = ApoClient::getInstant();
	if (!apoCli)	{
		return NDSYS_ERR_NOT_INIT;
	}
	return apoCli->TrytoRelogin();

}
RESULT_T apoCli_LoginAccount(const char *account, const char *passwd)
{
	ApoClient *apoCli = ApoClient::getInstant();
	if (!apoCli)	{
		return NDSYS_ERR_NOT_INIT;
	}
	return apoCli->LoginAccount(account, passwd);
}
RESULT_T apoCli_CreateAccount(const char *userName, const char *passwd, const char *phone, const char *email)
{

	ApoClient *apoCli = ApoClient::getInstant();
	if (!apoCli)	{
		return NDSYS_ERR_NOT_INIT;
	}
	return apoCli->CreateAccount(userName,passwd,phone,email);
}
RESULT_T apoCli_testOneKeyLogin(const char *host, int port, const char *user, const char *passwd)
{
	ApoClient *apoCli = ApoClient::getInstant();
	if (!apoCli)	{
		return NDSYS_ERR_NOT_INIT;
	}
	return apoCli->testOneKeyLogin(host, port, user, passwd);

}
void apoCli_Logout()
{

	ApoClient *apoCli = ApoClient::getInstant();
	if (apoCli)	{
		apoCli->Logout();
	}
}
void apoCli_ClearLoginHistory()
{
	ApoClient *apoCli = ApoClient::getInstant();
	if (apoCli)	{
		apoCli->ClearLoginHistory();
	}

}
bool apoCli_Update()
{
	ApoClient *apoCli = ApoClient::getInstant();
	if (apoCli)	{
		return apoCli->Update();
	}
	return false;
}
NDUINT32 apoCli_GetCurAccId()
{
	ApoClient *apoCli = ApoClient::getInstant();
	if (apoCli)	{
		return apoCli->GetCurAccId();
	}
	return 0;
}
NDUINT32 apoCli_GetCurRoleId()
{
	ApoClient *apoCli = ApoClient::getInstant();
	if (apoCli)	{
		return apoCli->GetCurRoleId();
	}
	return 0;

}
time_t apoCli_getServerTime()
{
	ApoClient *apoCli = ApoClient::getInstant();
	if (apoCli)	{
		return apoCli->getServerTime();
	}
	return 0;


}
