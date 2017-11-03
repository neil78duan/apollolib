//
//  ServerPublicData.h
//  worldManager
//
//  Created by duanxiuyun on 15-5-26.
//  Copyright (c) 2015 duanxiuyun. All rights reserved.
//

#ifndef __db_common_srvdatamgr__
#define __db_common_srvdatamgr__

#include "ndapplib/applib.h"
#include "common_inc.h"
#include "sqlThMsgHandler.hpp"

class CommonDataMgr
{
public:
	CommonDataMgr() ;
	~CommonDataMgr() ;
	
	bool InitLoad(const char *name ,int serverId,void *data, size_t &size) ;
	bool Save(const char *name ,int serverId, void *data, size_t size) ;
	bool Create(const char *name ,int serverId, void *data, size_t size) ;
	
	bool LoadAsync(const char *name ,int serverId, NDUINT64 reqParam) ;
	bool CreateAsync(const char *name ,int serverId, void *data, size_t size, NDUINT64 reqParam) ;
	bool SaveAsync(const char *name ,int serverId, void *data, size_t size) ;
	
	virtual void onDataLoad(const char *name ,int serverId,void *data, size_t size, NDUINT64 reqParam);
	virtual void onDataCreate(const char *name ,int serverId,void *data, size_t size, NDUINT64 reqParam);
private:
	
};

// need implement this function
extern int sqlSendtoNetThread(int msgId, void *data, int dataLen) ;
extern int netSendtoSqlThread(int msgId, void *data, int dataLen) ;
extern  CommonDataMgr* get_CommonDataMgr() ;

#endif /* defined(__worldManager__ServerPublicData__) */
