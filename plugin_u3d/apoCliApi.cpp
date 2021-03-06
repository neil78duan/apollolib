//
//  netui_atl.cpp
//  clientDemo
//
//  Created by duanxiuyun on 14-12-2.
//  Copyright (c) 2014 duanxiuyun. All rights reserved.
//

#define APO_RESULT_T  RESULT_T

#include "nd_common/nd_common.h"
#include "nd_common/nd_iconv.h"
#include "cli_common/login_apollo.h"
#include "cli_common/apoClientObj.h"
#include "apollo_errors.h"
//#include "ndcli/nd_api_c.h"
#include "msg_def.h"

#include "apollo_errors.h"

#include "apoCliApi.h"

struct msgRecvInfo
{
	nd_usermsgbuf_t msgbuf;
	NDIStreamMsg msgReader;

	msgRecvInfo() : msgReader(NULL)
	{
		nd_usermsghdr_init(&msgbuf.msg_hdr);
	}
};

int apoCli_init(const char *workingPath, const char *logPath, const char *udid, const char *devDesc)
{
	LoginApollo::SetDeviceInfo(udid, devDesc);
	ApoClient *pInstant = ApoClient::getInstant();
	nd_logmsg("net Initilized!\n");
	if (pInstant) {
#ifdef __ND_WIN__

		char tmpBuf[1024];
		pInstant->setWorkingPath(nd_utf8_to_ndcode(workingPath, tmpBuf, sizeof(tmpBuf)));
		pInstant->setLogPath(nd_utf8_to_ndcode(logPath, tmpBuf, sizeof(tmpBuf)));
#else 
		pInstant->setWorkingPath(workingPath);
		pInstant->setLogPath(logPath);

#endif 
		return 1;
	}
	return 0;
}
void apoCli_destroy()
{
	ApoClient::destroyInstant();
	nd_logmsg("net Destroyed!\n");
}

void* get_NDNetObject()
{
	ApoClient *apoCli = ApoClient::getInstant();
	if (apoCli)	{
		return (void*) apoCli->getConn();
	}
	return NULL;
}


int apoCli_CheckConnValid()
{
	ApoClient *apoCli = ApoClient::getInstant();
	if (apoCli && apoCli->getConn())	{
		return nd_connector_valid((nd_netui_handle)apoCli->getConn());
	}
	return 0;
}

void apoCli_resetConnector()
{
	ApoClient *apoCli = ApoClient::getInstant();
	if (apoCli )	{
		apoCli->ResetConnect();
	}
}

RESULT_T apoCli_send(char *bufferFram, int frameSize)
{
	ApoClient *apoCli = ApoClient::getInstant();
	if (apoCli)	{
		if (! apoCli->IsInConnect())	{
			return NDSYS_ERR_CLOSED;
		}
		int ret = nd_connector_raw_write(apoCli->getConn(), bufferFram, frameSize);
		if (ret == -1)		{
			return (RESULT_T) nd_object_lasterror(apoCli->getConn());
		}
		return ESERVER_ERR_SUCCESS;
	}
	return NDSYS_ERR_NOT_INIT;
}

RESULT_T apoCli_sendMsg(int messageId, void *messageBody, int bodySize)
{
	NDOStreamMsg omsg((NDUINT16) messageId);
	if (messageBody && bodySize > 0) {
		if (-1 == omsg.WriteStream((char*)messageBody, bodySize)) {
			return NDSYS_ERR_INVALID_INPUT;
		}
	}
	
	ApoClient *apoCli = ApoClient::getInstant();
	if (apoCli)	{
		if (!apoCli->IsInConnect())	{
			nd_logerror("can not send data, MUST LOGIN \n") ;
			return NDSYS_ERR_NEED_LOGIN;
		}

		nd_connector_send(apoCli->getConn(), &(omsg.GetMsgAddr()->msg_hdr.packet_hdr), 0);
		return ESERVER_ERR_SUCCESS;
	}
	return NDSYS_ERR_NOT_INIT;
}

int apoCli_recv(char *bufferFram, int bufsize, int timeOutMS)
{
	ApoClient *apoCli = ApoClient::getInstant();
	if (!apoCli)	{
		return -1;
	}
	nd_handle h = apoCli->getConn();
	if (!h) {
		nd_logerror("can not RECV data, MUST connected\n") ;
		return -1;
	}

	if (!apoCli->IsInConnect())	{
		nd_logerror("can not RECV data, MUST LOGIN \n") ;
		nd_object_seterror(h, NDSYS_ERR_NEED_LOGIN);
		return -1;
	}

	int ret = nd_connector_waitmsg(h, (nd_packetbuf_t*)bufferFram, timeOutMS);
	if (ret > bufsize) {
		nd_object_seterror(h, NDSYS_ERR_INVALID_INPUT);
		return -1;
	}
	else if (ret > 0) {
		apoCli->trytoHandle((nd_usermsgbuf_t *)bufferFram);
	}
	return ret;
}

int apoCli_recvMsg(int *messageId, char *msgBody, int bufsize, int timeOutMS)
{
	nd_usermsgbuf_t msgBuf;
	int ret = apoCli_recv((char*)&msgBuf, sizeof(msgBuf), timeOutMS);
	if (ret > 0) {

		*messageId = ND_MAKE_WORD(msgBuf.msg_hdr.maxid, msgBuf.msg_hdr.minid);
		memcpy(msgBody, msgBuf.data, ND_USERMSG_LEN(&msgBuf));
		return ND_USERMSG_LEN(&msgBuf);
	}
	return ret;
}

RESULT_T apoCli_GetLastError()
{
	nd_handle h = (nd_handle)get_NDNetObject();
	if (!h) {
		return NDSYS_ERR_NOT_INIT;
	}

	return (RESULT_T) nd_object_lasterror(h);
}

int apoCli_getConnStat()
{
	nd_handle h = (nd_handle)get_NDNetObject();
	if (!h) {
		return 0;
	}
	return (RESULT_T)nd_connect_level_get(h);
}



RESULT_T apoCli_open(const char *host, int port)
{
	ApoClient *apoCli = ApoClient::getInstant();
	if (!apoCli)	{
		
		nd_logerror("open net, MUST INIT before open\n") ;
		
		return NDSYS_ERR_NOT_INIT;
	}
	if (!host || !*host) {
		
		nd_logerror("open net error aim address is NULL\n") ;
		return NDSYS_ERR_INVALID_INPUT;
	}
	RESULT_T res = apoCli->Open(host, port);
	nd_logdebug("open net %s code = %d\n", res ? "success":"error", res) ;
	return  res ;
}


RESULT_T apoCli_close()
{
	ApoClient *apoCli = ApoClient::getInstant();
	if (!apoCli)	{
		return NDSYS_ERR_NOT_INIT;
	}

	apoCli->Close();
	nd_logdebug("net closed\n");
	return ESERVER_ERR_SUCCESS;
}



RESULT_T apoCli_ReloginEx(const char *sessionData, int sessionSize, bool bReloginOffline)
{
	nd_logmsg("Begining to relogin!!!!!\n");
	ApoClient *apoCli = ApoClient::getInstant();
	if (!apoCli)	{
		nd_logerror("net client instant not init\n");
		return NDSYS_ERR_NOT_INIT;
	}
	if(!sessionData || sessionSize == 0) {
		nd_logerror("can not relogin , input session data is NULL \n");
		return  NDSYS_ERR_PARAM_NUMBER_ZERO ;
	}
	return apoCli->ReloginEx((void*)sessionData,sessionSize,bReloginOffline);
}

int apoCli_fetchSessionKey(char *outbuf, int bufsize)
{
	ApoClient *apoCli = ApoClient::getInstant();
	if (!apoCli)	{
		nd_logerror("can not fetch session key, net connector is NULL\n") ;
		return -1;
	}
	LoginBase *pLogin = apoCli->getLoginObj();
	if (!pLogin)	{
		nd_logerror("can not fetch session key, login manager is NULL\n") ;
		return -1;
	}
	if(!outbuf || bufsize == 0) {
		nd_logerror("can not catch ssession key, input buffer is NULL \n");
		return  -1 ;
	}
	
	
	return (int)pLogin->GetSessionData(outbuf, bufsize);
}

RESULT_T apoCli_LoginAccount(const char *account, const char *passwd, int accType, int channel, bool skipAuth)
{
	ApoClient *apoCli = ApoClient::getInstant();
	if (!apoCli)	{
		return NDSYS_ERR_NOT_INIT;
	}
	RESULT_T res = apoCli->LoginAccountOneKey(account, passwd,accType,channel,skipAuth);
	if (res != ESERVER_ERR_SUCCESS)	{
		apoCli->Close();
		nd_logerror("login failed code =%d\n", res) ;
	}
	return res;
}
RESULT_T apoCli_CreateAccount(const char *userName, const char *passwd,int channel)
{
	ApoClient *apoCli = ApoClient::getInstant();
	if (!apoCli)	{
		return NDSYS_ERR_NOT_INIT;
	}
	RESULT_T res = apoCli->CreateAccount(userName,passwd,channel);
	if (res != ESERVER_ERR_SUCCESS)	{
		apoCli->Close();
		nd_logerror("account create failed code =%d\n", res) ;
	}
	return res;
}

void apoCli_Logout()
{
	ApoClient *apoCli = ApoClient::getInstant();
	if (apoCli)	{
		apoCli->Logout();
	}
	
	nd_logmsg("account logout\n") ;
}
void apoCli_ClearLoginHistory()
{
	ApoClient *apoCli = ApoClient::getInstant();
	if (apoCli)	{
		apoCli->ClearLoginHistory();
	}

}

// bool apoCli_Update()
// {
// 	ApoClient *apoCli = ApoClient::getInstant();
// 	if (apoCli)	{
// 		return apoCli->Update();
// 	}
// 	return false;
// }
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
void apoCli_EnableRecvLog(int bIsEnable)
{

	ApoClient *apoCli = ApoClient::getInstant();
	if (apoCli && apoCli->getConn())	{

		int val = bIsEnable ? 1 : 0;
		int size = sizeof(val);
		nd_net_ioctl((nd_netui_handle)apoCli->getConn(), NDIOCTL_LOG_RECV_MSG, &val, &size);
	}
}

void apoCli_EnableSendLog(int bIsEnable)
{

	ApoClient *apoCli = ApoClient::getInstant();
	if (apoCli && apoCli->getConn())	{

		int val = bIsEnable ? 1 : 0;
		int size = sizeof(val);
		nd_net_ioctl((nd_netui_handle)apoCli->getConn(), NDIOCTL_LOG_SEND_MSG, &val, &size);
	}
}

void apoCli_EnableStreamRecord()
{
	ApoClient *apoCli = ApoClient::getInstant();
	if (apoCli)	{
		apoCli->SetEnableStreamRecord(true);
	}
}


APO_RESULT_T apoCli_CreateAccountOnly(const char *userName, const char *passwd, int channel)
{
	ApoClient *apoCli = ApoClient::getInstant();
	if (!apoCli)	{
		return NDSYS_ERR_NOT_INIT;
	}
	RESULT_T res = apoCli->CreateOnly(userName, passwd, channel);
	if (res != ESERVER_ERR_SUCCESS)	{
		apoCli->Close();
	}
	return res;
}

APO_RESULT_T apoCli_LoginOnly(const char *account, const char *passwd, int accType, int channel, bool skipAuth)
{
	ApoClient *apoCli = ApoClient::getInstant();
	if (!apoCli)	{
		return NDSYS_ERR_NOT_INIT;
	}
	RESULT_T res = apoCli->LoginOnly(account, passwd, accType,channel, skipAuth);
	if (res != ESERVER_ERR_SUCCESS)	{
		apoCli->Close();
	}
	return res;
}
int apoCli_GetServerList(char *buf, int size)
{

	ApoClient *apoCli = ApoClient::getInstant();
	if (!apoCli)	{
		return NDSYS_ERR_NOT_INIT;
	}

	if (!apoCli->IsLoginOk()){
		return NDSYS_ERR_NEED_LOGIN;
	}
	ApolloServerInfo srvbuf[10];
	char *p = buf;
	int num = apoCli->GetGameAreaList(srvbuf,10);
	if (num > 0){
		int len = snprintf(buf, size, "<root>");
		buf += len;
		size -= len; 

		for (int i = 0; i < num; i++)	{
			len = snprintf(buf, size, "<node ip=\"%s\" port=\"%d\" onlines=\"%d\" max=\"%d\" groupName=\"%s\" defaultEntry=\"%d\" />\n", 
				srvbuf[i].inet_ip, srvbuf[i].port,srvbuf[i].cur_number, srvbuf[i].max_number, srvbuf[i].name, srvbuf[i].isdefault_entry);

			buf += len;
			size -= len;

		}

		len = snprintf(buf, size, "</root>");
		buf += len;
		size -= len;
	}
	return (int)(buf - p);
}


APO_RESULT_T apoCli_SelectServer(const char *host, int port)
{
	ApoClient *apoCli = ApoClient::getInstant();
	if (!apoCli)	{
		return NDSYS_ERR_NOT_INIT;
	}

	if (!apoCli->IsLoginOk()){
		return NDSYS_ERR_NEED_LOGIN;
	}
	return apoCli->SelectServer(host, port);
}

int apoCli_GetRoleList(char *buf, int size)
{
	ApoClient *apoCli = ApoClient::getInstant();
	if (!apoCli)	{
		return NDSYS_ERR_NOT_INIT;
	}

	if (!apoCli->IsLoginOk()){
		return NDSYS_ERR_NEED_LOGIN;
	}

	role_base_info roles; 
	int ret = apoCli->getLoginObj()->GetRoleList(&roles, 1, apoCli->getReloginStat());
	if (ret ==0){
		return ret;
	}

	snprintf(buf, size, "<role name=\"%s\" id=\"%d\" />\n", roles.name, roles.rid);

	return ret;
}

APO_RESULT_T apoCli_CreateRole(const char *roleName)
{
	ApoClient *apoCli = ApoClient::getInstant();
	if (!apoCli)	{
		return NDSYS_ERR_NOT_INIT;
	}

	if (!apoCli->IsLoginOk()){
		return NDSYS_ERR_NEED_LOGIN;
	}

	role_base_info roles;
	int ret = apoCli->getLoginObj()->CreateRole(roleName, roles);
	if (ret == -1){
		return (APO_RESULT_T) apoCli->getLoginObj()->GetLastError();
	}

	return ESERVER_ERR_SUCCESS;
}
APO_RESULT_T apoCli_SelectRole(unsigned roleId)
{
	ApoClient *apoCli = ApoClient::getInstant();
	if (!apoCli)	{
		return NDSYS_ERR_NOT_INIT;
	}

	if (!apoCli->IsLoginOk()){
		return NDSYS_ERR_NEED_LOGIN;
	}

	int ret = apoCli->getLoginObj()->SelectRole(roleId);
	if (ret == -1){
		return (APO_RESULT_T)apoCli->getLoginObj()->GetLastError();
	}

	return ESERVER_ERR_SUCCESS;
}

// APO_RESULT_T apoCli_EnterGame(const char *host, int port)
// {
// 	ApoClient *apoCli = ApoClient::getInstant();
// 	if (!apoCli)	{
// 		return NDSYS_ERR_NOT_INIT;
// 	}
// 	return apoCli->EnterGame(host, port);
// }

int apoCli_SetTimeout(int val)
{
	return ndSetTimeoutVal(val);
}

int apoCli_GetRoleBeloneServerId()
{
	ApoClient *apoCli = ApoClient::getInstant();
	if (!apoCli)	{
		return -1;
	}

	if (!apoCli->IsLoginOk()){
		return NDSYS_ERR_NEED_LOGIN;
	}

	LoginBase  *pLogin = apoCli->getLoginObj();
	if (!pLogin)	{
		return -1;
	}
	return pLogin->getServerId();
}


const char* apoCli_GetLocalToken()
{
	return LoginApollo::GetLocalToken();
}
// 
// time_t apoCli_getServerTime()
// {
// 	ApoClient *apoCli = ApoClient::getInstant();
// 	if (apoCli)	{
// 		return apoCli->getServerTime();
// 	}
// 	return 0;
// 
// 
// }
