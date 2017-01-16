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

int msg_echo_handler(NDIConn* pconn, nd_usermsgbuf_t *msg ) ;

void atlantis_base_message(NDIConn *pconn);
//
//CPPAPI ND_CMDLINE_FUNC_INSTANCE(atlantisChat);
//
//CPPAPI ND_CMDLINE_FUNC_INSTANCE(atlantisAddfriend);
//CPPAPI ND_CMDLINE_FUNC_INSTANCE(atlantisBraodcast) ;
//
//CPPAPI ND_CMDLINE_FUNC_INSTANCE(privateChat) ;

CPPAPI ND_CMDLINE_FUNC_INSTANCE(get_total_online);
CPPAPI ND_CMDLINE_FUNC_INSTANCE(get_per_hosts_online_number);


ND_CMDLINE_FUNC_INSTANCE(atlantis_round_trip_time) ;

CPPAPI void init_atlantis(NDIConn* pconn) ;


CPPAPI ND_CMDLINE_FUNC_INSTANCE(gen_rsa_key_bin) ;
CPPAPI ND_CMDLINE_FUNC_INSTANCE(gen_rsa_key_cpp) ;


CPPAPI ND_CMDLINE_FUNC_INSTANCE(getRoleList);
CPPAPI ND_CMDLINE_FUNC_INSTANCE(createRole);

#endif /* defined(__nddebugger__atlantis_test__) */
