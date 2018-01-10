//
//  srv_msghandler.cpp
//  gameHall
//
//  Created by duanxiuyun on 14-9-30.
//  Copyright (c) 2014ๅนด duanxiuyun. All rights reserved.
//

#include "common_inc.h"
#include "srv_common/comm_msghandler.h"
#include "srv_common/playerMgr.h"

//#include "msghandler.h"
//#include "gameInstant.h"

MSG_ENTRY_INSTANCE(register2MgrAck)
{
    ND_TRACE_FUNC() ;
    NDIStreamMsg inmsg(msg) ;
    NDUINT8 isOk =1;
    
    NDConnector *pconn =  htoConnector(nethandle);
    
    if (0==inmsg.Read(isOk) && isOk==0) {
        int isRegisterOk = 1 ;
        int size = sizeof(int) ;

        pconn->Ioctl(NDIOCTL_SET_LEVEL, &isRegisterOk, &size) ;
    }
	else {
		pconn->Close(0);
	}
	nd_logmsg("%s regisger to server  %s\n",pconn->getName(),  isOk ? "FAILED":"SUCCESS") ;
    
    return 0;
}


MSG_ENTRY_INSTANCE(echo_handler)
{
	ND_TRACE_FUNC() ;
	ND_MSG_SEND( nethandle,(nd_usermsghdr_t *)msg, h_listen) ;
	nd_logdebug("receive echo message ECHO length=%d\n", ND_USERMSG_LEN(msg));
	return 0 ;
}

MSG_ENTRY_INSTANCE(broadcast_handler)
{
	//ND_BROAD_CAST(nethandle,(nd_usermsghdr_t *)msg,ESF_URGENCY,0) ;
	return 0 ;
}



MSG_ENTRY_INSTANCE(unwrap_sendto_player_entry)
{
	ND_TRACE_FUNC() ;
	NDUINT16 sid ;
	NDUINT8 encrypt = 0 ;
	NDIStreamMsg inmsg(msg) ;
	nd_usermsgbuf_t realMsg ;
	
	if(-1==inmsg.Read(sid) ) {		
		nd_logmsg("world message transfer error \n") ;
		return 0 ;
	}
	if(-1==inmsg.Read(encrypt) ) {		
		nd_logmsg("world message transfer error \n") ;
		return 0 ;
	}
	
	//nd_logdebug("%s wrap message by(%d, %d) length=%d to client %d\n", 		nd_object_get_instname(nethandle), inmsg.MsgMaxid(), inmsg.MsgMinid(), inmsg.MsgLength(), sid);
	
	size_t size = inmsg.ReadBin(&realMsg, sizeof(realMsg)) ;
	if (size < ND_USERMSG_HDRLEN || size!= ND_USERMSG_LEN(&realMsg)) {
		nd_logerror("recv wrapped message error length to small size =%d\n", (int)size) ;
		return 0;
	}
	
	PlayerMgr*playerMgr = get_playerMgr() ; 
	nd_assert(playerMgr) ;
	
	nd_logdebug("recved (%d,%d ) length = %d from %s wrapped by (%d, %d) direct-to-client session=%d\n",
		ND_USERMSG_MAXID(&realMsg), ND_USERMSG_MINID(&realMsg), ND_USERMSG_LEN(&realMsg),
		nd_object_get_instname(nethandle), inmsg.MsgMaxid(), inmsg.MsgMinid(), inmsg.MsgLength(), sid);
	
	if (0==sid) {		
		//nd_logdebug("world request broadcast (%d, %d) length = %d\n", ND_USERMSG_MAXID(&realMsg),  ND_USERMSG_MINID(&realMsg), ND_USERMSG_LEN(&realMsg)) ;
		playerMgr->BroadCastInHost(&realMsg.msg_hdr, encrypt?true:false) ;
	}
	else {		
		if(!h_listen){
			h_listen = getbase_inst()->GetDeftListener()->GetHandle() ;
			nd_assert(h_listen) ;
		}		
		//nd_logdebug("world translate to client real message (%d, %d)\n", ND_USERMSG_MAXID(&realMsg), ND_USERMSG_MAXID(&realMsg)) ;
		nd_send_toclient_ex(sid, &realMsg.msg_hdr, h_listen, (int)encrypt) ; 
	}
	return 0;
}


MSG_ENTRY_INSTANCE(unwrap_call_session_msgproc_entry)
{
	ND_TRACE_FUNC() ;
	NDUINT16 sid ;
	NDUINT8 encrypt = 0 ;
	NDIStreamMsg inmsg(msg) ;
	nd_usermsgbuf_t realMsg ;
	
	if(-1==inmsg.Read(sid) ) {
		
		nd_logmsg("world message transfer error \n") ;
		return 0 ;
	}
	if(-1==inmsg.Read(encrypt) ) {		
		nd_logmsg("world message transfer error \n") ;
		return 0 ;
	}

	
	size_t size = inmsg.ReadBin(&realMsg, sizeof(realMsg)) ;
	if (size < ND_USERMSG_HDRLEN || size!= ND_USERMSG_LEN(&realMsg)) {
		nd_logerror("recv wrapped message to msg proc error length to small\n") ;
		return 0;
	}
	
	PlayerMgr*playerMgr = get_playerMgr() ; 
	nd_assert(playerMgr) ;
	
	nd_logdebug("recved (%d,%d ) length = %d from %s wrapped by (%d, %d) call-msg-process session=%d\n",
		ND_USERMSG_MAXID(&realMsg), ND_USERMSG_MINID(&realMsg), ND_USERMSG_LEN(&realMsg),
		nd_object_get_instname(nethandle), inmsg.MsgMaxid(), inmsg.MsgMinid(), inmsg.MsgLength(), sid);
	
	if (0==sid) {		
		playerMgr->CallMsgProcInHost(&realMsg.msg_hdr) ;
	}
	else {		
		if(!h_listen){
			h_listen = getbase_inst()->GetDeftListener()->GetHandle() ;
			nd_assert(h_listen) ;
		}
		nd_netmsg_handle(sid, &realMsg.msg_hdr, h_listen) ; 
	}
	return 0;
}

MSG_ENTRY_INSTANCE(bridge_to_client_dirctly_entry)
{
	ND_TRACE_FUNC() ;
	//check privilege
	NDIStreamMsg inmsg(msg) ;
	
	NDUINT16 sid ;
	if (-1==inmsg.Read(sid)) { return 0;}
	
	
	NDOStreamMsg omsg(inmsg.MsgMaxid(), inmsg.MsgMinid()) ;
	inmsg.Read(omsg) ;	
	
	if(!h_listen){
		h_listen = getbase_inst()->GetDeftListener()->GetHandle() ;
		nd_assert(h_listen) ;
	}
	

	nd_logdebug("bridge message  (%d,%d ) length = %d from %s to client\n",
		inmsg.MsgMaxid(), inmsg.MsgMinid(), inmsg.MsgLength(),	nd_object_get_instname(nethandle));

	//nd_logdebug("world server direct send to client  (%d, %d)\n", inmsg.MsgMaxid(), inmsg.MsgMinid()) ;
	
	if (0==sid) {
		PlayerMgr*playerMgr = get_playerMgr() ; 
		nd_assert(playerMgr) ;		
		playerMgr->BroadCastInHost((nd_usermsghdr_t*)omsg.GetMsgAddr()) ;
	}
	else {
		if(-1==nd_send_tocliet(sid,(nd_usermsghdr_t*)omsg.GetMsgAddr(), h_listen) ){
			nd_logmsg("nd_netmsg_handle() to %d error\n", sid) ;		
		}	
	}
	
	return 0;	
}


// send client message to world server 
MSG_ENTRY_INSTANCE(resend_to_world_handler)
{
	ND_TRACE_FUNC() ;
	//send ack success
	NDIStreamMsg inmsg(msg) ;
	NDOStreamMsg omsg(inmsg.MsgMaxid(), inmsg.MsgMinid()) ;
	
	omsg.Write(nd_session_getid(nethandle)) ;
	if (inmsg.LeftData() > 0) {
		inmsg.Read(omsg) ;
	}	
	getWorldConnect().SendMsg(omsg) ;
	nd_logdebug("receive (%d, %d) and resend to world server \n", inmsg.MsgMaxid(), inmsg.MsgMinid()) ;
	return 0 ;
}

// send client message to world server 
MSG_ENTRY_INSTANCE(resend_playermsg_to_world_handler)
{
	ND_TRACE_FUNC() ;
	//send ack success
	NDIStreamMsg inmsg(msg) ;
	NDOStreamMsg omsg(inmsg.MsgMaxid(), inmsg.MsgMinid()) ;
	
	NDSession *pSession = (NDSession *) NDGetSession(nethandle);
	
	omsg.Write(pSession->GetSessionID()) ;
	omsg.Write((NDUINT32)pSession->GetID()) ;
	if (inmsg.LeftData() > 0) {
		inmsg.Read(omsg) ;
	}	
	getWorldConnect().SendMsg(omsg) ;
	nd_logdebug("receive (%d, %d) and resend to world server \n", inmsg.MsgMaxid(), inmsg.MsgMinid()) ;
	return 0 ;
}


//show error
const char *apollo_error(int errcode)
{
	ND_TRACE_FUNC();
	static char errdesc[128];

	const char *nd_err[] = {
		"UNKNOW_ERROR"
#undef ErrorElement 
#define ErrorElement(a,_err_desc) "game(ESERVER_"#a "):" _err_desc
#include "_error_def.h"
#undef ErrorElement 
	};

	if (errcode <= NDERR_UNKNOWN) {

		nd_error_convert  oldFunc = nd_register_error_convert(NULL);
		const char *perr = nd_error_desc(errcode);
		nd_register_error_convert(oldFunc);
		return perr;
	}
	else if (errcode > ApolloError_Start && errcode < ESERVER_ERR_NUMBER){
		return nd_err[errcode - ApolloError_Start-1];
	}
	else {
		snprintf(errdesc, sizeof(errdesc), "Error code =%d", errcode);
		return errdesc;
	}
}


MSG_ENTRY_INSTANCE(sys_get_error_desc)
{
	ND_TRACE_FUNC();
	//send ack success
	NDIStreamMsg inmsg(msg);
	NDOStreamMsg omsg(inmsg.MsgMaxid(), inmsg.MsgMinid());

	NDSession *pSession = (NDSession *) NDGetSession(nethandle);

	NDUINT32 errorId = 0 ;

	inmsg.Read(errorId);
	
	omsg.Write(errorId);
	const char *perr =apollo_error(errorId);

	if (perr){
		omsg.Write(perr);
	}
	pSession->SendMsg(omsg);
	return 0;
}
