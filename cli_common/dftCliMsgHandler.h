/* file dftCliMsgHandler.h
 *
 * define client demo program message handler 
 *
 * create by duan 
 *
 * 2016-1-15
 */

#ifndef _DFT_CLI_MSG_HANDLER_H_
#define _DFT_CLI_MSG_HANDLER_H_

#ifndef WITHOUT_LOGIC_PARSER
#include "logic_parser/dbldata2netstream.h"
#endif 

namespace ClientMsgHandler
{
void defaultCliMsgHandlerInit(NDIConn *pconn) ;	

#ifndef WITHOUT_LOGIC_PARSER
void destroyDftClientMsgHandler(NDIConn *pConn);
bool initDftClientMsgHandler(NDIConn *pConn, const char*script_file, userDefineDataType_map_t &dataConfig, ndxml *msg_def, logic_print outfunc);
#endif

	
	int msg_get_version_handler(NDIConn* pconn, nd_usermsgbuf_t *msg);
	int msg_get_rlimit_handler(NDIConn* pconn, nd_usermsgbuf_t *msg);
	int msg_show_msg_name_handler(NDIConn* pconn, nd_usermsgbuf_t *msg);
	int msg_show_server_time_handler(NDIConn* pconn, nd_usermsgbuf_t *msg);
	int msg_show_game_time_handler(NDIConn* pconn, nd_usermsgbuf_t *msg);
	int msg_broadcast_handler(NDIConn* pconn, nd_usermsgbuf_t *msg);

};
#endif 
