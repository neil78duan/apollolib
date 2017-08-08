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


void apoCli_resetConnector()
{
	ApoClient *apoCli = ApoClient::getInstant();
	if (apoCli && apoCli->getConn())	{
		nd_handle h = apoCli->getConn();
		nd_connector_close(h, 0);
		nd_connector_set_crypt(h, NULL, 0);
	}
}

RESULT_T apoCli_send(char *bufferFram, int frameSize)
{
	ApoClient *apoCli = ApoClient::getInstant();
	if (apoCli)	{
		nd_connector_raw_write(apoCli->getConn(), bufferFram, frameSize);
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
		nd_logerror("net client not init\n");
		return -1;
	}
	nd_usermsgbuf_t msgBuf;

	int ret = nd_connector_waitmsg(h, (nd_packetbuf_t*)&msgBuf, timeOutMS);
	if (ret == 0){

		nd_logerror("wait message time out \n");
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

	return apoCli->Open(host, port, dev_udid);
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
	ApoClient *apoCli = ApoClient::getInstant();
	if (!apoCli)	{
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
	return apoCli->LoginAccount(account, passwd,accType,skipAuth);
}
RESULT_T apoCli_CreateAccount(const char *userName, const char *passwd, const char *phone, const char *email)
{

	ApoClient *apoCli = ApoClient::getInstant();
	if (!apoCli)	{
		return NDSYS_ERR_NOT_INIT;
	}
	return apoCli->CreateAccount(userName,passwd,phone,email);
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
