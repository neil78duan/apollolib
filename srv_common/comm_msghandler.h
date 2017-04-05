//
//  comm_msghandler.h
//  gameHall
//
//  Created by duanxiuyun on 15-1-29.
//  Copyright (c) 2015年 duanxiuyun. All rights reserved.
//

#ifndef gameHall_comm_msghandler_h
#define gameHall_comm_msghandler_h

#include "ndapplib/applib.h"

MSG_ENTRY_DECLARE(register2MgrAck) ;
MSG_ENTRY_DECLARE(unwrap_sendto_player_entry);	/* 把包内容作为消息读起发送给客户的 */
MSG_ENTRY_DECLARE(unwrap_call_session_msgproc_entry); /* 把包内容作为消息读起调用客户端消息处理过程 */
MSG_ENTRY_DECLARE(bridge_to_client_dirctly_entry); /* 把消息直接转发给客户端 */

MSG_ENTRY_DECLARE(resend_to_world_handler) ;
MSG_ENTRY_DECLARE(resend_playermsg_to_world_handler );


MSG_ENTRY_DECLARE(echo_handler);
MSG_ENTRY_DECLARE(broadcast_handler);
MSG_ENTRY_INSTANCE(sys_get_error_desc);


#endif
