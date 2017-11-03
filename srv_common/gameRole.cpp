//
//  gameRole.cpp
//  gameHall
//
//  Created by duanxiuyun on 15-1-29.
//  Copyright (c) 2015年 duanxiuyun. All rights reserved.
//

#include "srv_common/gameRole.h"
#include "srv_common/playerMgr.h"


Role::Role () 
{
	m_roleID =0 ;	
	m_nick[0] = 0;	
	m_mySession =0;
	
	//m_userData.size = 0 ;
	//m_userData.data[0] = 0 ;
}

Role:: ~Role() 
{
	
}
void Role::SetIDName(roleid_t roleid, NDUINT8 *name) 
{
	ND_TRACE_FUNC() ;
	m_roleID = roleid ;
	if (name && name[0]) {		
		strncpy((char*)m_nick, (const char*)name, sizeof(m_nick));
	}
}

void Role::SetId(roleid_t rid)
{
	m_roleID = rid;
}
void Role::SetName(const char *name)
{
	if (name && name[0]) {
		strncpy((char*)m_nick, name, sizeof(m_nick));
	}
}
void Role::SetGuildName(const char *name)
{
	if (name && name[0]) {
		strncpy((char*)m_guildName, name, sizeof(m_guildName));
	}
}


int Role::Send(NDOStreamMsg &omsg, int flag)
{
	nd_assert(m_mySession);
	return m_mySession->SendMsg(omsg, flag);
}
int Role::Send(nd_usermsgbuf_t *msgBuf, int flag)
{
	nd_assert(m_mySession);
	return m_mySession->SendMsg(&msgBuf->msg_hdr, flag);
}
int Role::SendtoOther(roleid_t roleId, NDOStreamMsg &omsg, bool bSave )
{
	return get_playerMgr()->SendToNation(roleId, omsg,false,bSave);
}
int Role::SendtoOther(roleid_t roleId, nd_usermsgbuf_t *msgBuf, bool bSave)
{
	return get_playerMgr()->SendToNation(roleId, &msgBuf->msg_hdr, false, bSave);
}
int Role::CallMsgProc(roleid_t playerid, nd_usermsgbuf_t *msgBuf, bool bSave)
{
	return get_playerMgr()->CallMsgProc(playerid, &msgBuf->msg_hdr,bSave);
}
int Role::CallMsgProc(roleid_t playerid, NDOStreamMsg &omsg, bool bSave)
{
	return get_playerMgr()->CallMsgProc(playerid, omsg,bSave);
}
int Role::CallMsgProcToNation(roleid_t playerid, nd_usermsgbuf_t *msgBuf, bool bSave)
{
	return get_playerMgr()->CallMsgProcToNation(playerid, &msgBuf->msg_hdr, bSave);
}
int Role::CallMsgProcToNation(roleid_t playerid, NDOStreamMsg &omsg, bool bSave)
{
	return get_playerMgr()->CallMsgProcToNation(playerid, omsg, bSave);
}


