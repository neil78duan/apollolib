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
#include "srv_define.h"
#include "commonTest.h"

#include "apollo_errors.h"
#include "login_apollo.h"


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
//
//ND_CMDLINE_FUNC_INSTANCE(atlantisChat)
//{
//	ND_CMDLINE_CHECK_SHOW_HELP(argc, argv,"chat [message-text]") ;
//	CHECK_LOGIN() ;
//	NDOStreamMsg omsg(NETMSG_MAX_SOCIAL, SOCIAL_MSG_CHAT_REQ) ;
//	omsg.Write((NDUINT8)ECHAT_BROADCAST) ;
//	omsg.Write((NDUINT32)0) ;
//	if (argc >= 2) {
//		omsg.Write((NDUINT8*)argv[1]) ;
//	}
//	pconn->SendMsg(omsg,ESF_ENCRYPT | ESF_URGENCY) ;
//	return 0;
//}
//
//ND_CMDLINE_FUNC_INSTANCE(privateChat)
//{
//	ND_CMDLINE_CHECK_SHOW_HELP(argc, argv,"pchat [recv_player_id] [private-message-text]") ;
//	CHECK_LOGIN() ;
//	
//	if (argc >= 3) {
//		NDOStreamMsg omsg(NETMSG_MAX_SOCIAL, SOCIAL_MSG_CHAT_REQ) ;
//		omsg.Write((NDUINT8)ECHAT_PRIVATE) ;
//		omsg.Write((NDUINT32)atoi(argv[1])) ;
//
//		omsg.Write((NDUINT8*)argv[2]) ;
//		pconn->SendMsg(omsg,ESF_ENCRYPT | ESF_URGENCY) ;
//	}
//	else {
//		fprintf(stderr, "USAGE: pchat [recv_player_id] [private-message-text]\n") ;
//		
//	}
//	return 0;
//}
//
//ND_CMDLINE_FUNC_INSTANCE(atlantisBraodcast)
//{
//	ND_CMDLINE_CHECK_SHOW_HELP(argc, argv,"remotecall [echo-message-text]") ;
//	CHECK_LOGIN() ;
//	NDOStreamMsg omsg(NETMSG_MAX_SOCIAL, SOCIAL_MSG_BRAODCAST_REQ) ;
//	
//	if (argc >= 2) {
//		omsg.Write((NDUINT8*)argv[1]) ;
//	}
//	pconn->SendMsg(omsg,ESF_ENCRYPT | ESF_URGENCY) ;
//	return 0;
//
//}


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
				
				ndip_t ip = ND_HIDWORD(serverID) ;
				NDUINT32 port = ND_LODWORD(serverID) ;
				port = ND_HIWORD(port) ;
				
				fprintf(stdout, "%s:%d online_number =%d\n", nd_inet_ntoa(ip, NULL),port, number) ;
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
	
	NDOStreamMsg omsg(NETMSG_MAX_ROLE, ROLE_MSG_CREATE_ROLE_REQ ) ;
	omsg.Write((NDUINT8*)argv[1]) ;
	
	omsg.Write((NDUINT16)1) ;
	omsg.Write((NDUINT8)20) ;
	omsg.Write((float)0) ;
	
	nd_handle h = pconn->GetHandle() ;
	nd_usermsgbuf_t recv_msg ;
	TEST_SEND_AND_WAIT(h, omsg, &recv_msg, NETMSG_MAX_ROLE,ROLE_MSG_CREATE_ROLE_ACK,0)
	else {
		NDUINT32 roleid = 0;
		NDUINT32 error_code = 0;
		NDUINT8 name[USER_NAME_SIZE] ;
		
		NDIStreamMsg inmsg(&recv_msg) ;
		inmsg.Read(roleid) ;
		
		if (roleid == 0) {
			inmsg.Read(error_code) ;
			if (error_code) {
				fprintf(stderr, "create role list error : %d\n", error_code) ;
			}
			return -1 ;
		}
		else {
			inmsg.Read(name,sizeof(name)) ;
			fprintf(stdout, "create role %s success \n", name) ;
			//read attribute
			NDUINT16 num =0;
			if(0==inmsg.Read(num) ) {
				for (int i=0; i<num; ++i) {
					NDUINT8 aid ;
					float val ;
					inmsg.Read(aid) ;inmsg.Read(val) ;
					fprintf(stderr, "create role attribute id = %d val =%f \n", aid, val) ;
				}
			}
			
			LoginApollo login(h) ;
			login.ReadyGame() ;
		}

	}
	return 0;
}


ND_CMDLINE_FUNC_INSTANCE(getRoleList)
{
	ND_CMDLINE_CHECK_SHOW_HELP(argc, argv,"role_list [ create_role_name_if_list_is_emt]") ;
	CHECK_LOGIN() ;
	
	NDOStreamMsg omsg(NETMSG_MAX_ROLE, ROLE_MSG_GET_ROLE_LIST_REQ ) ;
	
	nd_handle h = pconn->GetHandle() ;
	nd_usermsgbuf_t recv_msg ;
	
	TEST_SEND_AND_WAIT(h, omsg, &recv_msg, NETMSG_MAX_ROLE,ROLE_MSG_GET_ROLE_LIST_ACK,0)
	else {
		NDUINT32 roleid = 0;
		NDUINT32 error_code = 0;
		NDUINT8 name[USER_NAME_SIZE] ;
		
		NDIStreamMsg inmsg(&recv_msg) ;
		inmsg.Read(roleid) ;
		
		if (roleid == 0) {
			inmsg.Read(error_code) ;
			if (error_code) {
				fprintf(stderr, "get role list error : %d\n", error_code) ;
				return -1 ;
			}
			
			if (argc >= 2) {
				//create role ;
				const char * create_args[2] ;
				create_args[0] = "role_create" ;
				create_args[1] = argv[1] ;
				
				return createRole(root, 2, create_args) ;
			}
			else {
				fprintf(stderr, "role list is empty, role number = 0\n") ;
			}
			
		}
		else {
			inmsg.Read(name,sizeof(name)) ;
			fprintf(stdout, "get role %s success \n", name) ;
			
			//read attribute
			NDUINT16 num =0;
			if(0==inmsg.Read(num) ) {
				for (int i=0; i<num; ++i) {
					NDUINT8 aid ;
					float val ;
					inmsg.Read(aid) ;inmsg.Read(val) ;
					fprintf(stderr, "load role attribute id = %d val =%f \n", aid, val) ;
				}
			}
			LoginApollo login(h) ;
			login.ReadyGame() ;
		}
	}
	return 0;
}

