//
//  commonTest.cpp
//  clientDemo
//
//  Created by duanxiuyun on 15-2-4.
//  Copyright (c) 2015年 duanxiuyun. All rights reserved.
//

#include "commonTest.h"
#include "apollo_errors.h"
//#include "gameMessage.h"
#include "netui_atl.h"

#include "nd_msg.h"
#include "msg_def.h"
//#include "apollo_data.h"
#include "login_apollo.h"

#include "dftCliMsgHandler.h"

int getConnectStat(NDIConn *pConn)
{
	if (pConn && pConn->CheckValid()) {		
		int priv_level = 0 ;
		int size = sizeof(priv_level) ;
		
		pConn->ioctl(NDIOCTL_GET_LEVEL, &priv_level, &size) ;
		return  priv_level ;
	}
	return 0;
}



NDIConn* LoginServer( const char * host,int port, const char* user, const char *passwd, const char*sessionFile,NDUINT32 *accID ) 
{
	NDIConn *pConn = CreateConnectorObj(NULL) ;
	if (!pConn) {
		return 0 ;
	}
	//initCommonMessageHandle(pConn) ;
	if (!host || !host[0]) {
		host = "localhost" ;
		port = 6600 ;
	}
	
	if(-1==pConn->Open(host, port, "tcp-connector", NULL) ) {		
		fprintf(stderr, "connect %s:%d ERROR \n",  host, port) ;		
		DestroyConnectorObj(pConn) ;
		return 0 ;
	}
	
	fprintf(stdout, "connect %s:%d success \n",host, port ) ;
	
	if (!user || !user[0]) {
		user = "testTooluser0" ;
		passwd = "testUser123" ;
	}
	
	int bSuccess = 0;
	if (sessionFile && sessionFile[0]) {
		bSuccess =	RloginTrylogin(pConn->GetHandle(),ACC_APOLLO, user, passwd,sessionFile, accID) ;
	}
	else {
		bSuccess = loginAndCreate(pConn->GetHandle(),ACC_APOLLO,user, passwd,NULL,accID);
	}
	
	if (0== bSuccess) {
		fprintf(stdout,"%s Login Success \n",user) ;
	}
	else {
		fprintf(stderr,"%s Login Failed errorcode = %d\n", user, pConn->LastError()) ;
		pConn->Close(1) ; 
		DestroyConnectorObj(pConn) ;
		return 0;
	}
	return pConn;
}
//
//NDIConn * EnterRoom(NDIConn *hallConnector,NDUINT32 playerID, ndip_t server, int port, NDUINT32 roomID, NDUINT32 key, NDUINT8 *passwd)
//{
//	//check connect is ok
//	if (!hallConnector || !hallConnector->CheckValid()) {
//		fprintf(stdout, "connect status error!\n") ;		
//		return 0 ;
//	}
//	int priv_level = 0 ;
//	int size = sizeof(priv_level) ;
//	
//	hallConnector->ioctl(NDIOCTL_GET_LEVEL, &priv_level, &size) ;
//	if (priv_level < EPL_LOGIN) {
//		fprintf(stdout, "login status error!\n") ;
//		return 0;
//	}	
//	
//	NDIConn *pConn = CreateConnectorObj(NULL) ;
//	if (!pConn) {
//		return 0 ;
//	}
//	initCommonMessageHandle(pConn) ;
//	
//	if(-1==pConn->Open(server, port, "tcp-connector", NULL) ) {		
//		fprintf(stderr, "connect %x:%d ERROR \n",  server, port) ;
//		DestroyConnectorObj(pConn) ;
//		return 0;
//	}
//	
//	//create message 
//	
//	NDUINT32 _val = 0 ;
//	size = sizeof(_val) ;
//	hallConnector->ioctl(NDIOCTL_GET_SESSIONID,&_val, &size);
//	
//	NDUINT16 session_key = (NDUINT16)_val;
//	
//	//userid+ session-key + 32bits:room + 32bits:key [+ text:password]
//	NDOStreamMsg internalMsg(NETMSG_MAX_BATTLE,BATTLE_MSG_ENTER_REQ) ;
//	internalMsg.Write((NDUINT16)session_key);
//	internalMsg.Write(roomID) ;
//	internalMsg.Write(key) ;
//	if (passwd && passwd[0]) {
//		internalMsg.Write(passwd) ;
//	}
//	hallConnector->CryptMsg( internalMsg.GetMsgAddr(), true) ;
//	
//	size_t msg_len = ND_USERMSG_LEN(internalMsg.GetMsgAddr()) ;
//	
//	NDOStreamMsg omsg(NETMSG_MAX_BATTLE,BATTLE_MSG_ENTER_REQ) ;	
//	omsg.Write(playerID) ;
//	omsg.WriteBin(internalMsg.GetMsgAddr(), msg_len) ;
//	
//	fprintf(stdout, "user %d key=%d enter room %d enter key=%d connector =%p\n",playerID, session_key, roomID, key,hallConnector) ;
//	if(pConn->SendMsg(omsg) > 0){
//		return  pConn ;
//	}
//	DestroyConnectorObj(pConn) ;
//	return 0;
//	
//}
//
//NDIConn* handleEnterRoomMsg(NDIConn* pconnGame,NDUINT32 playerID, NDIStreamMsg &inmsg, NDUINT32 *outRoomID, NDUINT32* roomKey )
//{
//	//close last connecting
//	NDUINT32  roomID, key, mapID;
//	NDUINT64 serverID ;
//	
//	if (-1==inmsg.Read(serverID)) { return 0;}
//	if (-1==inmsg.Read(roomID)) { return 0;}
//	if (-1==inmsg.Read(key)) { return 0;}
//	inmsg.Read(mapID) ;
//	
//	ndip_t ip = ND_HIDWORD(serverID) ;
//	NDUINT32 port = ND_LODWORD(serverID) ;
//	port = ND_HIWORD(port) ;
//	
//	fprintf(stdout, "room info: ip=%s:%d id=%d key=%d\n", nd_inet_ntoa(ip, NULL),port, roomID, key) ;
//	
//	if (outRoomID) {
//		*outRoomID = roomID ;
//	}
//	if (roomKey) {
//		*roomKey = key;
//	}
//	
//	return EnterRoom(pconnGame,playerID, ip,  port,  roomID,  key, NULL) ;
//}
//
//
//int CreateRoomReq(NDIConn *pConn) 
//{
//	NDOStreamMsg omsg(NETMSG_MAX_ROOM,ROOM_MSG_CREATE_REQ) ;
//	
//	// format :32bits:mapID + 16bits:player-number +8bits:is-open ＋[text:name+text:password]
//	omsg.Write((NDUINT32)1) ;
//	omsg.Write((NDUINT16)2) ;
//	omsg.Write((NDUINT8)1) ;
//		
//	int ret = pConn->SendMsg(omsg, ESF_URGENCY|ESF_ENCRYPT) ;	
//	
//	fprintf(stdout, "Create room message send  =%d \n", ret) ;
//	return 0;
//
//}
//
//int invietPlayerInroom(NDIConn *pConn, NDUINT32 playerID, NDUINT32 roomID, NDUINT32 key) 
//{
//	//ROOM_MSG_INVITE_REQ ,	//invite friend ,format :playerid + roomid + key
//	NDOStreamMsg omsg(NETMSG_MAX_ROOM, ROOM_MSG_INVITE_REQ) ;
//	
//	omsg.Write(playerID) ;
//	omsg.Write(roomID) ;
//	omsg.Write(key) ;
//	
//	int ret = pConn->SendMsg(omsg, ESF_URGENCY|ESF_ENCRYPT) ;	
//	
//	fprintf(stdout, "inviete %d player message send datalen =%d\n",playerID, ret) ;
//	
//	return 0 ;
//}


