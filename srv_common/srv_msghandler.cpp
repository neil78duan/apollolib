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
    
    //NDConnector *pconn =  htoConnector(nethandle);
    
    if (0==inmsg.Read(isOk) && isOk==0) {
        int isRegisterOk = 1 ;
        int size = sizeof(int) ;

        netconn->Ioctl(NDIOCTL_SET_LEVEL, &isRegisterOk, &size) ;
    }
	else {
		netconn->Close(0);
	}
	nd_logmsg("%s regisger to server  %s\n",netconn->getName(),  isOk ? "FAILED":"SUCCESS") ;
    
    return 0;
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
		netconn->getName(), inmsg.MsgMaxid(), inmsg.MsgMinid(), inmsg.MsgLength(), sid);
	
	if (0==sid) {		
		//nd_logdebug("world request broadcast (%d, %d) length = %d\n", ND_USERMSG_MAXID(&realMsg),  ND_USERMSG_MINID(&realMsg), ND_USERMSG_LEN(&realMsg)) ;
		playerMgr->BroadCastInHost(&realMsg.msg_hdr, encrypt?true:false) ;
	}
	else {
		nd_handle h_listen = netconn->GetListenerHandle() ;
		if(!h_listen){
			h_listen = getbase_inst()->GetDeftListener()->GetHandle() ;
			nd_assert(h_listen) ;
		}		
		//nd_logdebug("world translate to client real message (%d, %d)\n", ND_USERMSG_MAXID(&realMsg), ND_USERMSG_MAXID(&realMsg)) ;
		nd_session_send_id(sid, &realMsg.msg_hdr, h_listen, (int)encrypt) ; 
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
		netconn->getName(), inmsg.MsgMaxid(), inmsg.MsgMinid(), inmsg.MsgLength(), sid);
	
	if (0==sid) {		
		playerMgr->CallMsgProcInHost(&realMsg.msg_hdr) ;
	}
	else {
		nd_handle h_listen = netconn->GetListenerHandle() ;
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
	
	nd_handle h_listen = netconn->GetListenerHandle() ;
	if(!h_listen){
		h_listen = getbase_inst()->GetDeftListener()->GetHandle() ;
		nd_assert(h_listen) ;
	}
	

	nd_logdebug("bridge message  (%d,%d ) length = %d from %s to client\n",
		inmsg.MsgMaxid(), inmsg.MsgMinid(), inmsg.MsgLength(),	netconn->getName());

	//nd_logdebug("world server direct send to client  (%d, %d)\n", inmsg.MsgMaxid(), inmsg.MsgMinid()) ;
	
	if (0==sid) {
		PlayerMgr*playerMgr = get_playerMgr() ; 
		nd_assert(playerMgr) ;		
		playerMgr->BroadCastInHost((nd_usermsghdr_t*)omsg.GetMsgAddr()) ;
	}
	else {
		if(-1==nd_session_msg_send_id(sid,(nd_usermsghdr_t*)omsg.GetMsgAddr(), h_listen) ){
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
	
	omsg.Write(netconn->GetSessionID()) ;
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
	
	NDSession *pSession = static_cast<NDSession*>(netconn);
	
	omsg.Write(pSession->GetSessionID()) ;
	omsg.Write((NDUINT32)pSession->GetID()) ;
	if (inmsg.LeftData() > 0) {
		inmsg.Read(omsg) ;
	}	
	getWorldConnect().SendMsg(omsg) ;
	nd_logdebug("receive (%d, %d) and resend to world server \n", inmsg.MsgMaxid(), inmsg.MsgMinid()) ;
	return 0 ;
}



MSG_ENTRY_INSTANCE(sys_get_error_desc)
{
	ND_TRACE_FUNC();
	//send ack success
	NDIStreamMsg inmsg(msg);
	NDOStreamMsg omsg(inmsg.MsgMaxid(), inmsg.MsgMinid());

	NDUINT32 errorId = 0 ;

	inmsg.Read(errorId);
	
	omsg.Write(errorId);
	const char *perr =apollo_error(errorId);

	if (perr){
		omsg.Write(perr);
	}
	netconn->SendMsg(omsg);
	return 0;
}
