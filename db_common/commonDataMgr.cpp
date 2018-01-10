//
//  ServerPublicData.cpp
//  worldManager
//
//  Created by duanxiuyun on 15-5-26.
//  Copyright (c) 2015年 duanxiuyun. All rights reserved.
//

#include "commonDataMgr.h"
//#include "threadmsg.h"
//#include "db_common/CommonDataSql.h"
#include "db_common/gameDBConnector.h"
//#include "nationInstance.h"

CommonDataMgr::CommonDataMgr()
{
	
}
CommonDataMgr::~CommonDataMgr()
{
	
}

bool CommonDataMgr::InitLoad(const char *name ,int serverId,void *data, size_t &size)
{
	ND_TRACE_FUNC();
	RESULT_T ret = get_LoadCommonDataSql()->LoadData(name, serverId, (char*)data, size);
	if (ret == ESERVER_ERR_SUCCESS) {
		return true;
	}
	size = 0;
	return  false ;
}

bool CommonDataMgr::Save(const char *name ,int serverId, void *data, size_t size)
{
	ND_TRACE_FUNC();
	RESULT_T ret = get_SaveCommonDataSql()->SaveData(name, serverId, (char*)data, (int)size);
	if (ret == ESERVER_ERR_SUCCESS) {
		return true;
	}
	return  false ;
}

bool CommonDataMgr::Create(const char *name ,int serverId, void *data, size_t size)
{
	ND_TRACE_FUNC();
	
	RESULT_T ret = get_CreateCommonDataSql()->CommonDataCreate(name, serverId, data, (int)size);
	if (ret == ESERVER_ERR_SUCCESS) {
		return true;
	}
	return  false ;
}


bool CommonDataMgr::LoadAsync(const char *name ,int serverId, NDUINT64 reqParam)
{
	ND_TRACE_FUNC();
	db_common_data_msg msg ;
	
	msg.serverId = serverId ;
	msg.param = reqParam ;
	msg.datalen = 0 ;
	strncpy(msg.name,name, sizeof(msg.name)) ;
	
	int msg_size = sizeof(msg) - sizeof(msg.data) ;
	netSendtoSqlThread(DBTHMSG_COMMON_LOAD, &msg, msg_size) ;
	return true ;
}
bool CommonDataMgr::CreateAsync(const char *name ,int serverId, void *data, size_t size, NDUINT64 reqParam)
{
	ND_TRACE_FUNC();
	db_common_data_msg msg ;
	
	msg.serverId = serverId ;
	msg.param = reqParam ;
	msg.datalen = 0 ;
	strncpy(msg.name,name, sizeof(msg.name)) ;
	memcpy(msg.data, data, size) ;
	msg.datalen = (NDUINT32)size ;
	
	int msg_size = (int)(sizeof(msg) - sizeof(msg.data) + size);
	netSendtoSqlThread(DBTHMSG_COMMON_CREATE, &msg, msg_size) ;
	return  true;
}
bool CommonDataMgr::SaveAsync(const char *name ,int serverId, void *data, size_t size)
{
	ND_TRACE_FUNC();
	db_common_data_msg msg ;
	
	msg.serverId = serverId ;
	msg.param = 0 ;
	msg.datalen = 0 ;
	strncpy(msg.name,name, sizeof(msg.name)) ;
	memcpy(msg.data, data, size) ;
	msg.datalen = (NDUINT32)size ;
	
	int msg_size = (int)(sizeof(msg) - sizeof(msg.data) + size);
	netSendtoSqlThread(DBTHMSG_COMMON_SAVE, &msg, msg_size) ;


	return  true;
}

void CommonDataMgr::onDataLoad(const char *name ,int serverId,void *data, size_t size, NDUINT64 reqParam)
{
	nd_logdebug("load data %s success len =%d param =%lld\n", name,size, reqParam) ;
}
void CommonDataMgr::onDataCreate(const char *name ,int serverId,void *data, size_t size, NDUINT64 reqParam)
{
	nd_logdebug("create data %s success len =%d param =%lld\n", name,size, reqParam) ;
	
}



