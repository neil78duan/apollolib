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
//#include "srv_define.h"
#include "logic_parser/logicEngineRoot.h"

//class NDSession ;
class Role;
class apolloSession : public NDSession
{
public:
	apolloSession(){} 
	virtual ~apolloSession(){}
	virtual Role *GetRole() { return NULL; }
};



class Role : public LogicObjectBase
{
public:
	
	Role () ;
	virtual ~Role() ;
	
	roleid_t GetID() {return m_roleID;} 
	apolloSession *GetSession() { return m_mySession; }
	const char *GetNickname() {return (const char*) m_nick;} 
	void SetIDName(roleid_t roleid, NDUINT8 *name) ;
	void SetId(roleid_t rid);
	void SetName(const char *name);

	virtual LogicParserEngine *getScriptHandler() { return NULL; }
	//userdata_info *GetUserData() {return &m_userData ;}
	
protected:
	roleid_t m_roleID ;	
	NDUINT8 m_nick[USER_NAME_SIZE];
	
	apolloSession *m_mySession;
	
	//userdata_info m_userData ;
	
};


#endif /* defined(__gameHall__gameRole__) */
