//
//  gameRole.h
//  gameHall
//
//  Created by duanxiuyun on 15-1-29.
//  Copyright (c) 2015年 duanxiuyun. All rights reserved.
//

#ifndef __gameHall__gameRole__
#define __gameHall__gameRole__

#include "ndapplib/applib.h"
#include "common_inc.h"
#include "srv_common/apoGameObj.h"

//class NDSession ;
class Role;
class apolloSession : public NDSession
{
public:
	apolloSession(){} 
	virtual ~apolloSession(){}
	virtual Role *GetRole() { return NULL; }
};



class Role //: public ApoGameObj
{
public:
	
	Role () ;
	virtual ~Role() ;
	
	roleid_t GetID() {return m_roleID;} 
	apolloSession *GetSession() { return m_mySession; }
	const char *GetNickname() {return (const char*) m_nick;} 
	//const char *GetGuildName() { return (const char*)m_guildName; }

	void SetIDName(roleid_t roleid, NDUINT8 *name);
	void SetId(roleid_t rid);
	void SetName(const char *name);
	//void SetGuildName(const char *name);


	int Send(NDOStreamMsg &omsg, int flag = ESF_NORMAL);
	int Send(nd_usermsgbuf_t *msgBuf, int flag = ESF_NORMAL);
	int SendtoOther(roleid_t roleId, NDOStreamMsg &omsg,bool bSave = false);
	int SendtoOther(roleid_t roleId, nd_usermsgbuf_t *msgBuf, bool bSave = false);
	int CallMsgProc(roleid_t playerid, nd_usermsgbuf_t *msghdr, bool bSave = false);
	int CallMsgProc(roleid_t playerid, NDOStreamMsg &omsg, bool bSave = false);

	virtual LogicParserEngine *getScriptHandler();
protected:
	roleid_t m_roleID ;	
	NDUINT8 m_nick[USER_NAME_SIZE];
	//NDUINT8 m_guildName[USER_NAME_SIZE];

	apolloSession *m_mySession;
	
	//userdata_info m_userData ;
	
};


#endif /* defined(__gameHall__gameRole__) */
