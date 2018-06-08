//
//  atlantis_test.cpp
//  nddebugger
//
//  Created by duanxiuyun on 14-12-17.
//  Copyright (c) 2014年 duanxiuyun. All rights reserved.
//

#include "atlantis_test.h"

#include "nd_msg.h"
#include "msg_def.h"
#include "ndcli/nd_api_c.h"
#include "apollo_data.h"
//#include "commonTest.h"

#include "apollo_errors.h"
#include "login_apollo.h"
#include "apoClientU3d.h"

#define myfprintf fprintf

int msg_default_handler(NDIConn* pconn, nd_usermsgbuf_t *msg)
{
	int reLine = 0;
	myfprintf(stdout, "recv (%d,%d) message data-len =%d\n", ND_USERMSG_MAXID(msg), ND_USERMSG_MINID(msg), ND_USERMSG_LEN(msg));
	
	return 0;
}

int msg_get_version_handler(NDIConn* pconn, nd_usermsgbuf_t *msg)
{
	NDIStreamMsg inmsg(msg);
	NDUINT32 ver_id = 0;
	NDUINT8 isDebug = 0;
	NDUINT8 buf[1024];
	buf[0] = 0;
	
	inmsg.Read(ver_id);
	inmsg.Read(isDebug);
	inmsg.Read(buf, sizeof(buf));
	
	myfprintf(stdout, "Get Server Version ver-id=%d %s\n Version description=%s\n", ver_id, isDebug ? "debug" : "release", buf[0] == 0 ? "unknow" : (const char*)buf);
	
	return 0;
}

int msg_get_rlimit_handler(NDIConn* pconn, nd_usermsgbuf_t *msg)
{
	NDIStreamMsg inmsg(msg);
	NDUINT8 buf[1024];
	buf[0] = 0;
	
	inmsg.Read(buf, sizeof(buf));
	
	myfprintf(stdout, "server rlimit info :\n%s\n", buf[0] == 0 ? "unknow" : (const char*)buf);
	
	return 0;
}


int msg_show_msg_name_handler(NDIConn* pconn, nd_usermsgbuf_t *msg)
{
	NDIStreamMsg inmsg(msg);
	NDUINT16 maxID, minID;
	NDUINT8 buf[1024];
	inmsg.Read(maxID);
	
	inmsg.Read(minID);
	inmsg.Read(buf, sizeof(buf));
	
	myfprintf(stdout, "message (%d, %d)  name=%s \n", maxID, minID, buf);
	return 0;
}


int msg_show_server_time_handler(NDIConn* pconn, nd_usermsgbuf_t *msg)
{
	NDIStreamMsg inmsg(msg);
	NDUINT64 tm1;
	if (0 == inmsg.Read(tm1)) {
		time_t tm2 = (time_t)tm1;
		
		char str[128];
		myfprintf(stdout, "%s\n", nd_get_datetimestr_ex(tm2, str, 128));
	}
	
	
	return 0;
}

int msg_show_game_time_handler(NDIConn* pconn, nd_usermsgbuf_t *msg)
{
	NDIStreamMsg inmsg(msg);
	NDUINT64 tm1;
	if (0 == inmsg.Read(tm1)) {
		time_t tm2 = (time_t)tm1;
		
		char str[128];
		myfprintf(stdout, "%s\n", nd_get_datetimestr_ex(tm2, str, 128));
	}
	
	
	return 0;
}


void atlantis_base_message(NDIConn *pconn)
{
	CONNECT_INSTALL_MSG(pconn, msg_show_server_time_handler, ND_MAIN_ID_SYS, ND_MSG_SYS_TIME);
	CONNECT_INSTALL_MSG(pconn, msg_show_game_time_handler, ND_MAIN_ID_SYS, ND_MSG_SYS_GAME_TIME);
	
	CONNECT_INSTALL_MSG(pconn, msg_show_msg_name_handler, ND_MAIN_ID_SYS, ND_MSG_SYS_GET_MESSAGE_NAME);
	CONNECT_INSTALL_MSG(pconn, msg_get_version_handler, ND_MAIN_ID_SYS, ND_MSG_SYS_GETVERSION);
	CONNECT_INSTALL_MSG(pconn, msg_get_rlimit_handler, ND_MAIN_ID_SYS, ND_MSG_SYS_GET_RLIMIT);
	
	pconn->SetDftMsgHandler(msg_default_handler);
	
}

int msg_echo_handler(NDIConn* pconn, nd_usermsgbuf_t *msg )
{
	NDUINT32 sendTm = 0;
	NDIStreamMsg inmsg(msg) ;
	if (0==inmsg.Read(sendTm)) {
		ndtime_t now = nd_time() ;
		fprintf(stdout, "time-trip = %d (ms) \n", now-sendTm) ;
	}
	else {
		fprintf(stdout, "time-trip test error \n") ;
	}
	fflush(stdout) ;
	return 0;
}

#define CHECK_LOGIN() 	\
NDIConn* pconn = (NDIConn*) root->userdata ;	\
if (!pconn ) {						\
	fprintf(stderr, "need to login \n") ;		\
	return -1 ;									\
}


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

void LogoutServer()
{
	ApoClient *pcliInst = ApoClient::getInstant();
	if (pcliInst) {
		pcliInst->Close() ;
	}
	ApoClient::destroyInstant() ;
}

NDIConn* LoginServer( const char * host,int port, const char* user, const char *passwd, const char*sessionFile,NDUINT32 *accID )
{
	
	LoginApollo::SetDeviceInfo(COMMON_TEST_UDID, "cmd-tool");
	ApoClient *pcliInst = ApoClient::getInstant();
	nd_assert(pcliInst) ;
	pcliInst->setWorkingPath(nd_getcwd()) ;
	pcliInst->setLogPath(nd_getcwd()) ;
	
	RESULT_T res = pcliInst->Open(host, port) ;
	if (res != NDERR_SUCCESS) {
		fprintf(stderr, "open host %s error =%d\n", host, res) ;
		return 0 ;
	}
	
	res = pcliInst->LoginOnly(user, passwd, ACC_APOLLO, 0, false) ;
	if (res == NDSYS_ERR_NOUSER) {
		res = pcliInst->CreateOnly(user, passwd, 0) ;
	}
	if (res != (RESULT_T)NDERR_SUCCESS) {
		fprintf(stderr, "can not login %s username=%s\n", host, user) ;
		return 0 ;
	}
	return pcliInst->getNetObj() ;
//	NDIConn *pConn = CreateConnectorObj(NULL) ;
//	if (!pConn) {
//		return 0 ;
//	}
//	//initCommonMessageHandle(pConn) ;
//	if (!host || !host[0]) {
//		host = "localhost" ;
//		port = 6600 ;
//	}
//
//	if(-1==pConn->Open(host, port, "tcp-connector", NULL) ) {
//		fprintf(stderr, "connect %s:%d ERROR \n",  host, port) ;
//		DestroyConnectorObj(pConn) ;
//		return 0 ;
//	}
//
//	fprintf(stdout, "connect %s:%d success \n",host, port ) ;
//
//	if (!user || !user[0]) {
//		user = "testTooluser0" ;
//		passwd = "testUser123" ;
//	}
//
//	int bSuccess = 0;
//	if (sessionFile && sessionFile[0]) {
//		bSuccess =	RloginTrylogin(pConn->GetHandle(),ACC_APOLLO, user, passwd,sessionFile, accID) ;
//	}
//	else {
//		bSuccess = loginAndCreate(pConn->GetHandle(),ACC_APOLLO,user, passwd,NULL,accID);
//	}
//
//	if (0== bSuccess) {
//		fprintf(stdout,"%s Login Success \n",user) ;
//	}
//	else {
//		fprintf(stderr,"%s Login Failed errorcode = %d\n", user, pConn->LastError()) ;
//		pConn->Close(1) ;
//		DestroyConnectorObj(pConn) ;
//		return 0;
//	}
//	return pConn;
}

ND_CMDLINE_FUNC_INSTANCE(atlantis_round_trip_time)
{
	ND_CMDLINE_CHECK_SHOW_HELP(argc, argv,"triptime") ;
	CHECK_LOGIN() ;
	NDOStreamMsg omsg(ND_MAIN_ID_SYS,ND_MSG_SYS_ECHO) ;
	ndtime_t sendTm = nd_time() ;
	omsg.Write((NDUINT32)sendTm) ;
	if(pconn->SendMsg(omsg,ESF_ENCRYPT | ESF_URGENCY) <= 0 ) {
		fprintf(stderr, "send trip package error  \n") ;
		return -1;
	}
	nd_usermsgbuf_t recvbuf ;
	if (-1== pconn->WaitMsg(&recvbuf, WAITMSG_TIMEOUT)) {
		
		fprintf(stderr, "read message error \n") ;
		return -1;
	}
	if(ND_USERMSG_MAXID(&recvbuf)==ND_MAIN_ID_SYS && ND_USERMSG_MINID(&recvbuf) == ND_MSG_SYS_ECHO) {		
		return  msg_echo_handler( pconn, &recvbuf) ;
	}
	
	fprintf(stderr, "Not wait trip message \n") ;
	return  -1 ;
	
}

ND_CMDLINE_FUNC_INSTANCE(get_total_online)
{
	ND_CMDLINE_CHECK_SHOW_HELP(argc, argv,"online_number") ;
	CHECK_LOGIN() ;
	NDOStreamMsg omsg(NETMSG_MAX_SERVER,SERVER_MSG_GET_ONLINE_NUMBER) ;
	ndtime_t sendTm = nd_time() ;
	omsg.Write((NDUINT32)sendTm) ;
	if(pconn->SendMsg(omsg, ESF_URGENCY) <= 0 ) {
		fprintf(stderr, "send reques online_number error \n") ;
		return -1;
	}
	nd_usermsgbuf_t recvbuf ;
	if (-1== pconn->WaitMsg(&recvbuf, WAITMSG_TIMEOUT)) {
		
		fprintf(stderr, "online_number: read message error \n") ;
		return -1;
	}
	
	if(ND_USERMSG_MAXID(&recvbuf)==NETMSG_MAX_SERVER && ND_USERMSG_MINID(&recvbuf) == SERVER_MSG_GET_ONLINE_NUMBER) {		
		NDIStreamMsg inmsg(&recvbuf) ;
		NDUINT32 number =0;
		inmsg.Read(number) ;
		fprintf(stdout, " Current total number=%d \n",  number) ;
		return 0;
	}
	
	fprintf(stderr, "online_number Not wait message \n") ;
	return  -1 ;
	
}

ND_CMDLINE_FUNC_INSTANCE(get_per_hosts_online_number)
{
	ND_CMDLINE_CHECK_SHOW_HELP(argc, argv,"per_host_number") ;
	CHECK_LOGIN() ;
	NDOStreamMsg omsg(NETMSG_MAX_SERVER, SERVER_MSG_GET_NUMBER_PER_HOST) ;
	ndtime_t sendTm = nd_time() ;
	omsg.Write((NDUINT32)sendTm) ;
	if(pconn->SendMsg(omsg, ESF_URGENCY) <= 0 ) {
		fprintf(stderr, "send request per_host_number message error  \n") ;
		return -1;
	}
	nd_usermsgbuf_t recvbuf ;
	if (-1== pconn->WaitMsg(&recvbuf, WAITMSG_TIMEOUT)) {
		
		fprintf(stderr, "per_host_number : read message error \n") ;
		return -1;
	}
	
	if(ND_USERMSG_MAXID(&recvbuf)==NETMSG_MAX_SERVER && ND_USERMSG_MINID(&recvbuf) == SERVER_MSG_GET_NUMBER_PER_HOST) {		
		NDIStreamMsg inmsg(&recvbuf) ;
		NDUINT64 serverID ;
		NDUINT32 number =0;
		NDUINT16 count =0;
		
		if(0==inmsg.Read(count) ) {
			for (int i = 0; i<count; i++) {
				inmsg.Read(serverID) ;
				inmsg.Read(number) ;
				
				NDUINT32 ip = ND_HIDWORD(serverID) ;
				NDUINT32 port = ND_LODWORD(serverID) ;
				port = ND_HIWORD(port) ;
				
				//fprintf(stdout, "%s:%d online_number =%d\n", nd_inet_ntoa(ip, NULL),port, number) ;
			}
		}
		
		return 0;
	}
	
	fprintf(stderr, "Not wait trip message \n") ;
	return  -1 ;
	
}



ND_CMDLINE_FUNC_INSTANCE(createRole)
{
	ND_CMDLINE_CHECK_SHOW_HELP(argc, argv,"role_create rolename") ;
	CHECK_LOGIN() ;
	if (argc < 2) {
		fprintf(stderr, "USAGE: role_create rolename\n") ;
		return -1;
	}
	ApoClient *pcliInst = ApoClient::getInstant();
	LoginApollo *pLogin = pcliInst->getLoginObj() ;
	
	if (!pLogin) {
		fprintf(stderr, "error: not login\n") ;
		return -1;
	}
	role_base_info roleInfo ;
	if(0!=pLogin->CreateRole(argv[1], roleInfo ) ) {
		fprintf(stderr, "error: create role error %d\n", pLogin->GetLastError() ) ;
		return -1;
	}
	pLogin->ReadyGame() ;
	return 0 ;
//
//	NDOStreamMsg omsg(NETMSG_MAX_LOGIN, LOGIN_MSG_CREATE_ROLE_REQ ) ;
//	omsg.Write((NDUINT8*)argv[1]) ;
//
//	omsg.Write((NDUINT16)1) ;
//	omsg.Write((NDUINT8)20) ;
//	omsg.Write((float)0) ;
//
//	nd_handle h = pconn->GetHandle() ;
//	nd_usermsgbuf_t recv_msg ;
//	TEST_SEND_AND_WAIT(h, omsg, &recv_msg, NETMSG_MAX_LOGIN, LOGIN_MSG_CREATE_ROLE_ACK,0)
//	else {
//		NDUINT32 roleid = 0;
//		NDUINT32 error_code = 0;
//		NDUINT8 name[USER_NAME_SIZE] ;
//
//		NDIStreamMsg inmsg(&recv_msg) ;
//		inmsg.Read(roleid) ;
//
//		if (roleid == 0) {
//			inmsg.Read(error_code) ;
//			if (error_code) {
//				fprintf(stderr, "create role list error : %d\n", error_code) ;
//			}
//			return -1 ;
//		}
//		else {
//			inmsg.Read(name,sizeof(name)) ;
//			fprintf(stdout, "create role %s success \n", name) ;
//			//read attribute
//			NDUINT16 num =0;
//			if(0==inmsg.Read(num) ) {
//				for (int i=0; i<num; ++i) {
//					NDUINT8 aid ;
//					float val ;
//					inmsg.Read(aid) ;inmsg.Read(val) ;
//					fprintf(stderr, "create role attribute id = %d val =%f \n", aid, val) ;
//				}
//			}
//
//			LoginApollo login(h) ;
//			login.ReadyGame() ;
//		}
//
//	}
//	return 0;
}


ND_CMDLINE_FUNC_INSTANCE(getRoleList)
{
	ND_CMDLINE_CHECK_SHOW_HELP(argc, argv,"role_list [ create_role_name_if_list_is_emt]") ;
	CHECK_LOGIN() ;
	
	ApoClient *pcliInst = ApoClient::getInstant();
	LoginApollo *pLogin = pcliInst->getLoginObj() ;
	
	if (!pLogin) {
		fprintf(stderr, "error: not login\n") ;
		return -1;
	}
	
	role_base_info roleInfo ;
	int ret = pLogin->GetRoleList(&roleInfo, 1) ;
	if (0==ret) {
		if(0!=pLogin->CreateRole(argv[1], roleInfo ) ) {
			fprintf(stderr, "error: create role error %d\n", pLogin->GetLastError() ) ;
			return -1;
		}
	}
	else if(-1==ret){
		fprintf(stderr, "error: select role error %d\n", pLogin->GetLastError()) ;
		return -1;
	}
	
	pLogin->ReadyGame() ;
	return 0 ;
	
//	NDOStreamMsg omsg(NETMSG_MAX_LOGIN, LOGIN_MSG_GET_ROLE_LIST_REQ ) ;
//	omsg.Write((NDUINT8)0);
//
//	nd_handle h = pconn->GetHandle() ;
//	nd_usermsgbuf_t recv_msg ;
//
//	TEST_SEND_AND_WAIT(h, omsg, &recv_msg, NETMSG_MAX_LOGIN, LOGIN_MSG_GET_ROLE_LIST_ACK,0)
//	else {
//		NDUINT32 roleid = 0;
//		NDUINT32 error_code = 0;
//		NDUINT8 name[USER_NAME_SIZE] ;
//
//		NDIStreamMsg inmsg(&recv_msg) ;
//		inmsg.Read(roleid) ;
//
//		if (roleid == 0) {
//			inmsg.Read(error_code) ;
//			if (error_code == NDERR_NOUSER) {
//
//				if (argc >= 2) {
//					//create role ;
//					const char * create_args[2];
//					create_args[0] = "role_create";
//					create_args[1] = argv[1];
//
//					return createRole(root, 2, create_args);
//				}
//				return -1;
//			}
//			else if (error_code) {
//				fprintf(stderr, "get role list error : %d\n", error_code) ;
//				return -1 ;
//			}
//
//		}
//		else {
//			inmsg.Read(name,sizeof(name)) ;
//			fprintf(stdout, "get role %s success \n", name) ;
//
//			//read attribute
//			NDUINT16 num =0;
//			if(0==inmsg.Read(num) ) {
//				for (int i=0; i<num; ++i) {
//					NDUINT8 aid ;
//					float val ;
//					inmsg.Read(aid) ;inmsg.Read(val) ;
//					fprintf(stderr, "load role attribute id = %d val =%f \n", aid, val) ;
//				}
//			}
//			LoginApollo login(h) ;
//			login.ReadyGame() ;
//		}
//	}
//	return 0;
}

