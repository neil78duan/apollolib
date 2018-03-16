//
//  playerMgr.h
//  gameHall
//
//  Created by duanxiuyun on 14-12-11.
//  Copyright (c) 2014年 duanxiuyun. All rights reserved.
//

#ifndef __gameHall__playerMgr__
#define __gameHall__playerMgr__


#include "common_inc.h"
#include <map>

using namespace::std;

class Role;

struct player_node_info 
{
	roleid_t playerId ;	
	NDUINT16 sessionid ;
};

class PlayerMgr : public NDObject 
{
public:
	PlayerMgr() ;
	virtual ~PlayerMgr() ;
	bool Add(Role *player) ;
	bool Del(Role *player) ;
	bool Del(roleid_t playerid) ;
	NDUINT16 GetSessionID(roleid_t playerid) ;
	
	int Send(roleid_t playerid, nd_usermsghdr_t *msghdr, bool encrypt = false, bool isSaved = false );
	int Send(roleid_t playerid, NDOStreamMsg &omsg, bool encrypt = false, bool isSaved = false );
// 
// 	int SendToNation(roleid_t playerid, nd_usermsghdr_t *msghdr, bool encrypt = false, bool isSaved = false);
// 	int SendToNation(roleid_t playerid, NDOStreamMsg &omsg, bool encrypt = false, bool isSaved = false);
	
	int BroadCastInHost(nd_usermsghdr_t *msghdr, bool encrypt = false);	
	int BroadCastInWorld(nd_usermsghdr_t *msghdr, bool encrypt = false);
	
	int CallMsgProc(roleid_t playerid, nd_usermsghdr_t *msghdr, bool isSaved = false);
	int CallMsgProc(roleid_t playerid, NDOStreamMsg &omsg, bool isSaved = false);
// 	
// 	int CallMsgProcToNation(account_index_t playerid, nd_usermsghdr_t *msghdr, bool isSaved = false);
// 	int CallMsgProcToNation(account_index_t playerid, NDOStreamMsg &omsg, bool isSaved = false);

	int CallMsgProcInHost(nd_usermsghdr_t *msghdr);	
	int CallMsgProcInWorld(nd_usermsghdr_t *msghdr);
	
	int GetNumOfPlayers(){ return (int)m_player_map.size(); }
		
private:
	int wrapToWorld(nd_usermsghdr_t *msg, int wrap_maxID, int wrap_minID, NDUINT32 playerID, bool ecnrypt = false, bool isSaved = false);
	//int wrapToNation(nd_usermsghdr_t *msg, int wrap_maxID, int wrap_minID, NDUINT32 playerID, bool ecnrypt, bool isSaved);
	typedef std::map<account_index_t ,player_node_info> player_online_map;
	
	nd_mutex m_lock;
	player_online_map m_player_map ;
	
};

extern PlayerMgr *get_playerMgr() ; 
extern NDSafeConnector &getWorldConnect() ;
extern NDConnector &getSyncServerConnector();


#endif /* defined(__gameHall__playerMgr__) */
