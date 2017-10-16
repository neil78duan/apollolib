//
//  netui_atl.cpp
//  clientDemo
//
//  Created by duanxiuyun on 14-12-2.
//  Copyright (c) 2014 duanxiuyun. All rights reserved.
//

#define APO_RESULT_T  RESULT_T

#include "nd_common/nd_common.h"
#include "cli_common/login_apollo.h"
#include "apollo_errors.h"
#include "cli_common/netui_atl.h"
#include "ndcli/nd_api_c.h"
#include "msg_def.h"

#include "apoClientU3d.h"
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

int apoCli_init(const char *workingPath, const char *logPath)
{
	ApoClient *pInstant = ApoClient::getInstant();
	if (pInstant) {
		pInstant->setWorkingPath(workingPath);
		pInstant->setLogPath(logPath);
		return 1;
	}
	return 0;
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
		int ret = nd_connector_raw_write(apoCli->getConn(), bufferFram, frameSize);
		if (ret == -1)		{
			return (RESULT_T) nd_object_lasterror(apoCli->getConn());
		}
		return ESERVER_ERR_SUCCESS;
	}
	return NDSYS_ERR_INVALID_HANDLE;
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
		nd_connector_send(apoCli->getConn(), &(omsg.GetMsgAddr()->msg_hdr.packet_hdr), 0);
		return ESERVER_ERR_SUCCESS;
	}
	return NDSYS_ERR_INVALID_HANDLE;
}

int apoCli_recv(char *bufferFram, int bufsize, int timeOutMS)
{
	nd_handle h = (nd_handle)get_NDNetObject();
	if (!h) {
		nd_logerror("net client not init\n");
		return -1;
	}

	int ret = nd_connector_waitmsg(h, (nd_packetbuf_t*)bufferFram, timeOutMS);
	if (ret > bufsize) {
		nd_object_seterror(h, NDSYS_ERR_INVALID_INPUT);
		return -1;
	}
	return ret;
}

int apoCli_recvMsg(int *messageId, char *msgBody, int bufsize, int timeOutMS)
{
	nd_handle h = (nd_handle)get_NDNetObject();
	if (!h) {
		//nd_logerror("net client not init\n");
		return -1;
	}
	nd_usermsgbuf_t msgBuf;

	int ret = nd_connector_waitmsg(h, (nd_packetbuf_t*)&msgBuf, timeOutMS);
	if (ret == 0){

		//nd_logerror("wait message time out \n");
		return 0;

	}
	if (-1 == ret) {
		return -1;
	}
	*messageId = ND_MAKE_WORD(msgBuf.msg_hdr.maxid, msgBuf.msg_hdr.minid);
	memcpy(msgBody, msgBuf.data, ND_USERMSG_LEN(&msgBuf));
	return ND_USERMSG_LEN(&msgBuf);
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

// 
// RESULT_T apoCli_recv(void *recvObject, int waitTime)
// {
// 	msgRecvInfo *pRecvBuf = (msgRecvInfo*)recvObject;
// 	if (!pRecvBuf) {
// 		return NDSYS_ERR_INVALID_INPUT;
// 	}
// 
// 	nd_handle h = (nd_handle) get_NDNetObject();
// 	if (!h) {
// 		return NDSYS_ERR_NOT_INIT;
// 	}
// 
// 	int ret = nd_connector_waitmsg(h, (nd_packetbuf_t *)&pRecvBuf->msgbuf, waitTime);
// 	if (ret ==0 ){
// 		return NDSYS_ERR_TIMEOUT;
// 	}
// 	if (-1 == ret) {
// 		return NDSYS_ERR_IO;
// 	}
// 
// 	pRecvBuf->msgReader.Init(&pRecvBuf->msgbuf);
// 	return ESERVER_ERR_SUCCESS;
// }
// 
// void *apoCli_createRecvObject()
// {
// 	return new msgRecvInfo;
// }
// void apoCli_destroyRecvObject(void *recvObject)
// {
// 	if (recvObject){
// 		msgRecvInfo *p = (msgRecvInfo*)recvObject;
// 		delete p;
// 	}
// }
// 
// int apoCli_readMsgIdFromObj(void *recvObject)
// {
// 	msgRecvInfo *pRecvBuf = (msgRecvInfo*)recvObject;
// 	if (!pRecvBuf) {
// 		return NDSYS_ERR_INVALID_INPUT;
// 	}
// 
// 	return ND_MAKE_WORD(pRecvBuf->msgReader.MsgMaxid(), pRecvBuf->msgReader.MsgMinid());
// }
// 
// RESULT_T apoCli_readMsgBodyFromObj(void *recvObject, char *buf, int bufSize)
// {
// 	msgRecvInfo *pRecvBuf = (msgRecvInfo*)recvObject;
// 	if (!pRecvBuf) {
// 		return NDSYS_ERR_INVALID_INPUT;
// 	}
// 	pRecvBuf->msgReader.ReadLeftStream(buf, bufSize);
// 	return ESERVER_ERR_SUCCESS;
// }


RESULT_T apoCli_open(const char *host, int port, const char *dev_udid)
{
	ApoClient *apoCli = ApoClient::getInstant();
	if (!apoCli)	{
		return NDSYS_ERR_NOT_INIT;
	}
	if (!host || !*host) {
		return NDSYS_ERR_INVALID_INPUT;
	}
	return apoCli->Open(host, port, dev_udid);
}


RESULT_T apoCli_close()
{
	ApoClient *apoCli = ApoClient::getInstant();
	if (!apoCli)	{
		return NDSYS_ERR_NOT_INIT;
	}

	apoCli->Close();
	return ESERVER_ERR_SUCCESS;
}


// 
// RESULT_T apoCli_ReloginBackground()
// {
// 	ApoClient *apoCli = ApoClient::getInstant();
// 	if (!apoCli)	{
// 		return NDSYS_ERR_NOT_INIT;
// 	}
// 	return apoCli->ReloginBackground();
// }

RESULT_T apoCli_ReloginEx(const char *sessionData, int sessionSize, bool bReloginOffline)
{
	nd_logmsg("Begining to relogin!!!!!\n");
	ApoClient *apoCli = ApoClient::getInstant();
	if (!apoCli)	{
		nd_logerror("net client instant not init\n");
		return NDSYS_ERR_NOT_INIT;
	}
	return apoCli->ReloginEx((void*)sessionData,sessionSize,bReloginOffline);
}

int apoCli_fetchSessionKey(char *outbuf, int bufsize)
{
	ApoClient *apoCli = ApoClient::getInstant();
	if (!apoCli)	{
		return -1;
	}
	LoginBase *pLogin = apoCli->getLoginObj();
	if (!pLogin)	{
		return -1;
	}
	return (int)pLogin->GetSessionData(outbuf, bufsize);
}
// 
// RESULT_T apoCli_TrytoRelogin()
// {
// 	ApoClient *apoCli = ApoClient::getInstant();
// 	if (!apoCli)	{
// 		return NDSYS_ERR_NOT_INIT;
// 	}
// 	return apoCli->TrytoRelogin();
// 
// }
RESULT_T apoCli_LoginAccount(const char *account, const char *passwd, int accType, bool skipAuth)
{
	ApoClient *apoCli = ApoClient::getInstant();
	if (!apoCli)	{
		return NDSYS_ERR_NOT_INIT;
	}
	RESULT_T res = apoCli->LoginAccountOneKey(account, passwd,accType,skipAuth);
	if (res != ESERVER_ERR_SUCCESS)	{
		apoCli->Close();
	}
	return res;
}
RESULT_T apoCli_CreateAccount(const char *userName, const char *passwd, const char *phone, const char *email)
{
	ApoClient *apoCli = ApoClient::getInstant();
	if (!apoCli)	{
		return NDSYS_ERR_NOT_INIT;
	}
	RESULT_T res = apoCli->CreateAccount(userName,passwd,phone,email);
	if (res != ESERVER_ERR_SUCCESS)	{
		apoCli->Close();
	}
	return res;
}
// RESULT_T apoCli_testOneKeyLogin(const char *host, int port, const char *user, const char *passwd)
// {
// 	ApoClient *apoCli = ApoClient::getInstant();
// 	if (!apoCli)	{
// 		return NDSYS_ERR_NOT_INIT;
// 	}
// 	return apoCli->testOneKeyLogin(host, port, user, passwd);
// 
// }
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


APO_RESULT_T apoCli_CreateAccountOnly(const char *userName, const char *passwd, const char *phone, const char *email)
{
	ApoClient *apoCli = ApoClient::getInstant();
	if (!apoCli)	{
		return NDSYS_ERR_NOT_INIT;
	}
	RESULT_T res = apoCli->CreateOnly(userName, passwd, phone, email);
	if (res != ESERVER_ERR_SUCCESS)	{
		apoCli->Close();
	}
	return res;
}

APO_RESULT_T apoCli_LoginOnly(const char *account, const char *passwd, int accType, bool skipAuth)
{
	ApoClient *apoCli = ApoClient::getInstant();
	if (!apoCli)	{
		return NDSYS_ERR_NOT_INIT;
	}
	RESULT_T res = apoCli->LoginOnly(account, passwd, accType, skipAuth);
	if (res != ESERVER_ERR_SUCCESS)	{
		apoCli->Close();
	}
	return res;
}
int apoCli_GetServerList(char *buf, size_t size)
{

	ApoClient *apoCli = ApoClient::getInstant();
	if (!apoCli)	{
		return NDSYS_ERR_NOT_INIT;
	}
	ApolloServerInfo srvbuf[10];
	char *p = buf;
	int num = apoCli->GetGameAreaList(srvbuf,10);
	if (num > 0){
		int len = snprintf(buf, size, "<root>");
		buf += len;
		size -= len; 

		for (int i = 0; i < num; i++)	{
			len = snprintf(buf, size, "<node ip=\"%s\" port=\"%d\" onlines=\"%d\" max=\"%d\" groupName=\"%d\" defaultEntry=\"%d\">\n", 
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
APO_RESULT_T apoCli_EnterGame(const char *host, int port)
{
	ApoClient *apoCli = ApoClient::getInstant();
	if (!apoCli)	{
		return NDSYS_ERR_NOT_INIT;
	}
	return apoCli->EnterGame(host, port);
	//const char *account, const char *passwd, int accType, bool skipAuth
}

int apoCli_SetTimeout(int val)
{
	return ndSetTimeoutVal(val);
}


int apoCli_GetServerGroupId()
{
	ApoClient *apoCli = ApoClient::getInstant();
	if (!apoCli)	{
		return -1;
	}

	LoginBase  *pLogin = apoCli->getLoginObj();
	if (!pLogin)	{
		return -1;
	}
	return pLogin->getServerId();
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
