//
//  playerMgr.cpp
//  gameHall
//
//  Created by duanxiuyun on 14-12-11.
//  Copyright (c) 2014年 duanxiuyun. All rights reserved.
//

#include "srv_common/playerMgr.h"
#include "srv_common/gameRole.h"

#include <vector>

//extern BattleInstance &get_instance() ;
extern NDSafeConnector &getWorldConnect() ;
extern NDConnector &getSyncServerConnector();
// extern NDConnector &getNationConnect();
// extern NDConnector &getLogConnect();

#define USE_SOCIAL_SYNC_MESSAGE  1


PlayerMgr::PlayerMgr() 
{
	nd_mutex_init(&m_lock) ;
}

PlayerMgr::~PlayerMgr() 
{
	nd_mutex_destroy(&m_lock);
}

bool PlayerMgr::Add(Role *player) 
{
	ND_TRACE_FUNC() ;
	player_node_info pinfo ;
	
	pinfo.playerId  = player->GetID() ;
	pinfo.sessionid = player->GetSession()->GetSessionID() ;
	
	LockHelper __lockhelper(&m_lock) ;
	{
		m_player_map[pinfo.playerId] = pinfo;
	}	
	
	return true ;
}


bool PlayerMgr::Del(Role *player) 
{
	return Del((account_index_t)player->GetID());
}

bool PlayerMgr::Del(account_index_t pid) 
{
	ND_TRACE_FUNC() ;
	
	LockHelper __lockhelper(&m_lock) ;
	{
		player_online_map::iterator it = m_player_map.find(pid) ;
		if(it != m_player_map.end()) {
			m_player_map.erase(it) ;
		}
	}
	
	return true ;
}


NDUINT16 PlayerMgr::GetSessionID(account_index_t playerid) 
{
	ND_TRACE_FUNC() ;
	LockHelper __lockhelper(&m_lock) ;
	{
		player_online_map::iterator it = m_player_map.find(playerid) ;
		if (it!=m_player_map.end()) {
			return it->second.sessionid ;
		}
	}
	
	return 0 ;
}


int PlayerMgr::Send(account_index_t playerid, nd_usermsghdr_t *msghdr, bool encrypt, bool isSaved)
{
	ND_TRACE_FUNC() ;
	int ret = -1 ;
	NDUINT16 session_id = GetSessionID(playerid) ;
	if(session_id) {
		//GameInstance &inst = get_instance() ;
		NDInstanceBase *inst = getbase_inst() ;
		ret = nd_send_toclient_ex(session_id, msghdr, inst->GetDeftListener()->GetHandle(),encrypt?1:0) ; 
	}
	else {
		ret = wrapToWorld(msghdr, ND_MAIN_ID_SYS, ND_MSG_SYS_DIRECTLY_TO_CLIENT_WRAPPER, playerid,encrypt,isSaved) ;
	}
	return ret ;
}
// 
// int PlayerMgr::SendToNation(account_index_t playerid, NDOStreamMsg &omsg, bool encrypt, bool isSaved)
// {
// 	ND_TRACE_FUNC() ;
// 	return SendToNation(playerid, &omsg.GetMsgAddr()->msg_hdr, encrypt, isSaved);
// }
// int PlayerMgr::SendToNation(account_index_t playerid, nd_usermsghdr_t *msghdr, bool encrypt, bool isSaved)
// {
// 	ND_TRACE_FUNC();
// 	int ret = -1;
// 	NDUINT16 session_id = GetSessionID(playerid);
// 	if (session_id) {
// 		//GameInstance &inst = get_instance() ;
// 		NDInstanceBase *inst = getbase_inst();
// 		ret = nd_send_toclient_ex(session_id, msghdr, inst->GetDeftListener()->GetHandle(), encrypt ? 1 : 0);
// 	}
// 	else {
// 		ret = wrapToNation(msghdr, ND_MAIN_ID_SYS, ND_MSG_SYS_DIRECTLY_TO_CLIENT_WRAPPER, playerid, encrypt, isSaved);
// 	}
// 	return ret;
// }

int PlayerMgr::Send(account_index_t playerid, NDOStreamMsg &omsg, bool encrypt, bool isSaved)
{
	ND_TRACE_FUNC();
	return Send(playerid, &omsg.GetMsgAddr()->msg_hdr, encrypt, isSaved);
}
int PlayerMgr::CallMsgProc(account_index_t playerid, nd_usermsghdr_t *msghdr, bool isSaved )
{
	ND_TRACE_FUNC() ;
	if (playerid==0) {
		return -1;
	}
	int ret = -1 ;
	
	NDUINT16 session_id = GetSessionID(playerid) ;
	if(session_id) {
		//GameInstance &inst = get_instance() ;		
		NDInstanceBase *inst = getbase_inst() ;
		ret = nd_netmsg_handle(session_id, msghdr, inst->GetDeftListener()->GetHandle()) ; 
	}
	else {
		ret = wrapToWorld(msghdr, ND_MAIN_ID_SYS, ND_MSG_SYS_CALL_SESSION_MSGPROC_WRAPPER, playerid,false,isSaved) ;
	}
	return ret ;
}

int PlayerMgr::CallMsgProc(account_index_t playerid, NDOStreamMsg &omsg, bool isSaved)
{
	ND_TRACE_FUNC() ;
	return CallMsgProc( playerid,&omsg.GetMsgAddr()->msg_hdr,isSaved) ;
}
// 
// int PlayerMgr::CallMsgProcToNation(account_index_t playerid, nd_usermsghdr_t *msghdr, bool isSaved)
// {
// 	ND_TRACE_FUNC();
// 	if (playerid == 0) {
// 		return -1;
// 	}
// 	int ret = -1;
// 
// 	NDUINT16 session_id = GetSessionID(playerid);
// 	if (session_id) {
// 		//GameInstance &inst = get_instance() ;		
// 		NDInstanceBase *inst = getbase_inst();
// 		ret = nd_netmsg_handle(session_id, msghdr, inst->GetDeftListener()->GetHandle());
// 	}
// 	else {
// 		ret = wrapToNation(msghdr, ND_MAIN_ID_SYS, ND_MSG_SYS_CALL_SESSION_MSGPROC_WRAPPER, playerid, false, isSaved);
// 	}
// 	return ret;
// }
// 
// int PlayerMgr::CallMsgProcToNation(account_index_t playerid, NDOStreamMsg &omsg, bool isSaved)
// {
// 	ND_TRACE_FUNC();
// 	return CallMsgProc(playerid, &omsg.GetMsgAddr()->msg_hdr, isSaved);
// }


int PlayerMgr::BroadCastInHost(nd_usermsghdr_t *msghdr,bool encrypt)
{
	ND_TRACE_FUNC() ;
	
	nd_handle lh = getbase_inst()->GetDeftListener()->GetHandle() ;
	return nd_sendto_all_ex(msghdr, lh, EPL_READY,encrypt?1:0) ;
	
}

int PlayerMgr::BroadCastInWorld(nd_usermsghdr_t *msghdr,bool encrypt)
{
	ND_TRACE_FUNC() ;
	
	wrapToWorld(msghdr, ND_MAIN_ID_SYS, ND_MSG_SYS_DIRECTLY_TO_CLIENT_WRAPPER, 0,encrypt) ;
#ifndef USE_SOCIAL_SYNC_MESSAGE
	BroadCastInHost(msghdr,encrypt);
#endif 
	return 0;

}

int PlayerMgr::BroadCastInHost(NDOStreamMsg &omsg, bool encrypt)
{
	return BroadCastInHost((nd_usermsghdr_t *)omsg.GetMsgAddr(), encrypt);
}
int PlayerMgr::BroadCastInWorld(NDOStreamMsg &omsg, bool encrypt)
{
	return BroadCastInWorld((nd_usermsghdr_t *)omsg.GetMsgAddr(), encrypt);
}


int PlayerMgr::CallMsgProcInHost(nd_usermsghdr_t *msghdr)
{
	ND_TRACE_FUNC() ;
	
	nd_handle lh = getbase_inst()->GetDeftListener()->GetHandle() ;
	return nd_netmsg_2all_handle(msghdr, lh, EPL_READY) ;
	
}

int PlayerMgr::CallMsgProcInWorld(nd_usermsghdr_t *msghdr)
{
	ND_TRACE_FUNC() ;	
	
	wrapToWorld(msghdr, ND_MAIN_ID_SYS, ND_MSG_SYS_CALL_SESSION_MSGPROC_WRAPPER, 0) ; 
#ifndef USE_SOCIAL_SYNC_MESSAGE
	CallMsgProcInHost(msghdr);
#endif 
	return 0;
}

int PlayerMgr::CallMsgProcInHost(NDOStreamMsg &omsg)
{
	return CallMsgProcInHost((nd_usermsghdr_t *)omsg.GetMsgAddr());

}
int PlayerMgr::CallMsgProcInWorld(NDOStreamMsg &omsg)
{
	return CallMsgProcInWorld((nd_usermsghdr_t *)omsg.GetMsgAddr());
}


int PlayerMgr::wrapToWorld(nd_usermsghdr_t *msg, int wrap_maxID, int wrap_minID, NDUINT32 playerID , bool ecnrypt, bool isSaved)
{
	ND_TRACE_FUNC() ;
	NDUINT16 wrapId = ND_MAKE_WORD(wrap_maxID, wrap_minID);
	NDUINT16 sendId = ND_MAKE_WORD(ND_USERMSG_MAXID(msg), ND_USERMSG_MINID(msg));
	nd_assert(wrapId != sendId);
	//nd_assert(wrap_maxID!=(int) ND_USERMSG_MAXID(msg) && wrap_minID!=(int)ND_USERMSG_MINID(msg)) ;
	NDOStreamMsg omsg(wrap_maxID, wrap_minID) ;
	size_t size = ND_USERMSG_LEN(msg) ;
	
	omsg.Write(playerID) ;
	omsg.Write((NDUINT8)(ecnrypt ? 1: 0)) ;
	omsg.Write((NDUINT8)(isSaved ? 1 : 0));
	omsg.WriteBin(msg, size) ;	
	nd_logdebug("using (%d,%d)  wrap message (%d, %d) length =%d \n",  wrap_maxID, wrap_minID, ND_USERMSG_MAXID(msg),ND_USERMSG_MINID(msg),ND_USERMSG_LEN(msg) );
#ifdef USE_SOCIAL_SYNC_MESSAGE
	return getSyncServerConnector().SendMsg(omsg);
#else 
	return getWorldConnect().SendMsg(omsg) ;
#endif 
}
// 
// int PlayerMgr::wrapToNation(nd_usermsghdr_t *msg, int wrap_maxID, int wrap_minID, NDUINT32 playerID, bool ecnrypt, bool isSaved)
// {
// 	ND_TRACE_FUNC();
// 	NDUINT16 wrapId = ND_MAKE_WORD(wrap_maxID, wrap_minID);
// 	NDUINT16 sendId = ND_MAKE_WORD(ND_USERMSG_MAXID(msg), ND_USERMSG_MINID(msg));
// 	nd_assert(wrapId != sendId);
// 	//nd_assert(wrap_maxID!=(int) ND_USERMSG_MAXID(msg) && wrap_minID!=(int)ND_USERMSG_MINID(msg)) ;
// 	NDOStreamMsg omsg(wrap_maxID, wrap_minID);
// 	size_t size = ND_USERMSG_LEN(msg);
// 
// 	omsg.Write(playerID);
// 	omsg.Write((NDUINT8)(ecnrypt ? 1 : 0));
// 	omsg.Write((NDUINT8)(isSaved ? 1 : 0));
// 	omsg.WriteBin(msg, size);
// 	nd_logdebug("using (%d,%d)  wrap message (%d, %d) length =%d \n", wrap_maxID, wrap_minID, ND_USERMSG_MAXID(msg), ND_USERMSG_MINID(msg), ND_USERMSG_LEN(msg));
// 	return getNationConnect().SendMsg(omsg);
// }
