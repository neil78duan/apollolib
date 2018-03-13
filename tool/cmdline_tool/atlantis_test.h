//
//  atlantis_test.h
//  nddebugger
//
//  Created by duanxiuyun on 14-12-17.
//  Copyright (c) 2014年 duanxiuyun. All rights reserved.
//

#ifndef __nddebugger__atlantis_test__
#define __nddebugger__atlantis_test__

#include "ndcli/nd_iconn.h"
//#include "atls_room.h"

#define COMMON_TEST_UDID "common_cmd_tool_udid"

#define TEST_SEND_AND_WAIT(_conn, _omsg, _rmsg_buf,_wait_maxid, _wait_minid,_sendflag) \
	if(0!=ndSendAndWaitMessage(_conn,_omsg.GetMsgAddr(),_rmsg_buf,_wait_maxid, _wait_minid,_sendflag,WAITMSG_TIMEOUT) ) {	\
		fprintf(stderr, "send and wait data error code =%d\n", ndGetLastError(_conn)); \
		return -1;		\
	}


CPPAPI int getConnectStat(NDIConn *pConn) ;

CPPAPI NDIConn* LoginServer( const char * host,int port, const char* user, const char *passwd, const char*sessionFile=NULL,NDUINT32 *accID =NULL) ;

CPPAPI void LogoutServer();

int msg_echo_handler(NDIConn* pconn, nd_usermsgbuf_t *msg ) ;

void atlantis_base_message(NDIConn *pconn);

CPPAPI ND_CMDLINE_FUNC_INSTANCE(get_total_online);
CPPAPI ND_CMDLINE_FUNC_INSTANCE(get_per_hosts_online_number);


ND_CMDLINE_FUNC_INSTANCE(atlantis_round_trip_time) ;

CPPAPI void init_atlantis(NDIConn* pconn) ;


CPPAPI ND_CMDLINE_FUNC_INSTANCE(gen_rsa_key_bin) ;
CPPAPI ND_CMDLINE_FUNC_INSTANCE(gen_rsa_key_cpp) ;


CPPAPI ND_CMDLINE_FUNC_INSTANCE(getRoleList);
CPPAPI ND_CMDLINE_FUNC_INSTANCE(createRole);

#endif /* defined(__nddebugger__atlantis_test__) */
