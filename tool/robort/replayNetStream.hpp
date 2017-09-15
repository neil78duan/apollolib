//
//  replayNetStream.hpp
//  roborts
//
//  Created by duanxiuyun on 15/11/26.
//  Copyright © 2015年 duanxiuyun. All rights reserved.
//

#ifndef replayNetStream_hpp
#define replayNetStream_hpp


#include "ndcli/nd_iconn.h"
#include <vector>

struct replayMsgData
{
	NDUINT32 sendTm;
	size_t size;
	char data[1];	
};

typedef std::vector<replayMsgData *> replay_vct;

// replay net stream to simulate the real player actions
class ReplayStream {
public:
	ReplayStream();
	virtual~ReplayStream() ;
	
	int update() ;

	//int initStream(const char *data, int size) ;
	int initStream(replay_vct& msgVct){ m_replayMsg = &msgVct; return 0; }
	virtual int Send(void *msgData, int size) ;
	
protected:
	int getNextFrameTime(ndtime_t *sendtm) ;
	char *fetchFrame(ndtime_t *sendtm,  int *length ) ;
	
	ndtime_t m_lastRecordFrameTm ;		//last original time 
	ndtime_t m_lastRunTime ; //replay time
	

	replay_vct *m_replayMsg;

	int m_currentSendIndex;
// 
// 	char *m_orgData ;
// 	char *m_curPlayAddr ;
// 	size_t m_size ;
	
};


#endif /* replayNetStream_hpp */
