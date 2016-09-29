//
//  commonTest.h
//  clientDemo
//
//  Created by duanxiuyun on 15-2-4.
//  Copyright (c) 2015年 duanxiuyun. All rights reserved.
//

#ifndef __clientDemo__commonTest__
#define __clientDemo__commonTest__


#include "ndcli/nd_iconn.h"
#include "nd_msg.h"
#include "apollo_data.h"
#include "ndcli/nd_api_c.h"


#define TEST_SEND_AND_WAIT(_conn, _omsg, _rmsg_buf,_wait_maxid, _wait_minid,_sendflag) \
	if(0!=ndSendAndWaitMessage(_conn,_omsg.GetMsgAddr(),_rmsg_buf,_wait_maxid, _wait_minid,_sendflag,WAITMSG_TIMEOUT) ) {	\
		fprintf(stderr, "send and wait data error code =%d\n", ndGetLastError(_conn)); \
		return -1;		\
	}

// if (nd_connector_send(_conn,(nd_packhdr_t*) (_omsg.GetMsgAddr()), _sendflag)<=0) {			\
// 	nd_object_seterror(_conn,NDERR_WRITE) ;	\
// 	fprintf(stderr, "send data error\n") ; \
// 	return -1 ;							\
// }										\
// if (-1==nd_connector_waitmsg(_conn, (nd_packetbuf_t *)_rmsg_buf,WAITMSG_TIMEOUT)) {	\
// 	nd_object_seterror(_conn,NDERR_TIMEOUT) ;				\
// 	fprintf(stderr, "wait message timeout\n") ; \
// 	return -1 ;							\
// }										\
// else if (nd_checkErrorMsg(_conn, (ndMsgData*)_rmsg_buf) ) {	\
// 	fprintf(stderr, "receive error message \n") ; \
// 	return -1 ;							\
// }										\
// else if(ND_USERMSG_MAXID(_rmsg_buf)!=_wait_maxid || ND_USERMSG_MINID(_rmsg_buf) != _wait_minid) { \
// 	nd_object_seterror(_conn, ESERVER_ERR_UNEXCEPTED_MSG) ;	\
// 	fprintf(stderr,"receive message not match with input(%d,%d)\n", ND_USERMSG_MAXID(_rmsg_buf), ND_USERMSG_MINID(_rmsg_buf)) ; \
// 	return -1 ;								\
// }
//show error 

CPPAPI int getConnectStat(NDIConn *pConn) ;
//CPPAPI int gloable_connector_on_error(nd_handle nethandle ,unsigned char *data, int dataLen) ;

//CPPAPI void conn_on_closed_cb(netObject handle, void *param) ;

CPPAPI NDIConn* LoginServer( const char * host,int port, const char* user, const char *passwd, const char*sessionFile=NULL,NDUINT32 *accID =NULL) ;
CPPAPI NDIConn * EnterRoom(NDIConn *hallConnector,NDUINT32 playerID,ndip_t server, int port, NDUINT32 roomID, NDUINT32 key, NDUINT8 *passwd);
CPPAPI NDIConn* handleEnterRoomMsg(NDIConn* pconnGame,NDUINT32 playerID, NDIStreamMsg &inmsg, NDUINT32 *outRoomID, NDUINT32* roomKey );

CPPAPI int CreateRoomReq(NDIConn *pConn) ;

CPPAPI int invietPlayerInroom(NDIConn *pConn, NDUINT32 playerID, NDUINT32 roomID, NDUINT32 key)  ;

#endif /* defined(__clientDemo__commonTest__) */
