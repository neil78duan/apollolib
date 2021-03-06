//
//  testConnector.h
//  clientDemo
//
//  Created by duanxiuyun on 15-2-4.
//  Copyright (c) 2015年 duanxiuyun. All rights reserved.
//

#ifndef __clientDemo__testConnector__
#define __clientDemo__testConnector__


#include "ndcli/nd_iconn.h"
#include "apollo_robort.h"
#include "replayNetStream.hpp"

#include <string>
class SendHelper: public ReplayStream
{
public:
	SendHelper() ;
	virtual ~SendHelper() ;
	virtual int Send(void *msgData, int size) ;
	void SetConn(NDIConn *conn) ;
protected:
	NDIConn *m_pConn;
};

class TestConnector  : public  ApolloRobort
{
public:
	TestConnector() ;
	virtual ~TestConnector() ;
	int initStream(replay_vct &msgvct);
	int update() ;
	
protected:
	SendHelper m_replay ;
};


#define  MAX_CONNECTOR_NUM 256

class TestconnsMgr
{
public:
	
	TestconnsMgr() ;
	virtual ~TestconnsMgr() ;
	
	int initStreamMsg(const char *file) ;
	int init(const char*host, int port, int num, int acc_start_index = 0);
	int initWithSdk(const char *sdkAccFile, const char*host, int port,  int num, int acc_start_index = 0);
	void setAccountPrefixName(const char* prefix)	{ m_accPrefix = prefix; }
	int Update() ;
	void Destroy() ;
	
	
	int m_maxNum ;
	
	TestConnector m_conns[MAX_CONNECTOR_NUM] ;
	
	NDUINT32 m_tickIndex ;
	//char *m_streamMessage ;
	//size_t m_streamSize ;
	std::string m_accPrefix;

	replay_vct m_replyMsgbuf;
};



#endif /* defined(__clientDemo__testConnector__) */
