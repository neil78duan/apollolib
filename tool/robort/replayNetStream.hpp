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

// replay net stream to simulate the real player actions
class ReplayStream {
public:
	ReplayStream() ;
	virtual~ReplayStream() ;
	
	int update() ;
	int initStream(const char *data, int size) ;
	virtual int Send(void *msgData, int size) ;
	
protected:
	int getNextFrameTime(ndtime_t *sendtm) ;
	char *fetchFrame(ndtime_t *sendtm,  int *length ) ;
	
	ndtime_t m_lastFrameTm ;
	ndtime_t m_curReplayTm ; //replay time
	
	char *m_orgData ;
	char *m_curPlayAddr ;
	size_t m_size ;
	
};


#endif /* replayNetStream_hpp */
