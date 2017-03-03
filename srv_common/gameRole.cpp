//
//  gameRole.cpp
//  gameHall
//
//  Created by duanxiuyun on 15-1-29.
//  Copyright (c) 2015年 duanxiuyun. All rights reserved.
//

#include "srv_common/gameRole.h"


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

