//
//  replayNetStream.cpp
//  roborts
//
//  Created by duanxiuyun on 15/11/26.
//  Copyright © 2015年 duanxiuyun. All rights reserved.
//

#include "replayNetStream.hpp"

/////////////////////////////

ReplayStream::ReplayStream() : m_replayMsg(0)
{
// 	m_orgData= 0 ;
// 	m_size = 0 ;
// 	m_curPlayAddr = 0 ;

	m_currentSendIndex = 0;

	m_lastRecordFrameTm = 0;		//last original time 
	m_lastRunTime = 0; //replay time

}
ReplayStream::~ReplayStream()
{
	
}

int ReplayStream::update()
{
	if (!m_replayMsg->size()) {
		return 0;
	}
	ndtime_t now = nd_time() ;
	ndtime_t tminterval = now - m_lastRunTime ;
	
	ndtime_t nextFrameTm = 0 ;
	if(0!=getNextFrameTime(&nextFrameTm) ) {
		return -1 ;
	}
	ndtime_t frameInterval = nextFrameTm - m_lastRecordFrameTm;
	if (m_lastRecordFrameTm == 0) {
		frameInterval = 0;
	}
	
	if (tminterval >= frameInterval) {
		int length;
		char * pdata = fetchFrame(&nextFrameTm, &length ) ;
		if (!pdata) {
			return -1 ;
		}
		m_lastRecordFrameTm = nextFrameTm;
		m_lastRunTime = now;

		return Send(pdata, length) ;
//		nd_usermsgbuf_t *pMsg = (nd_usermsgbuf_t*) pdata ;
//		NDUINT8 isCrypt = pMsg->msg_hdr.packet_hdr.encrypt ;
//		pMsg->msg_hdr.packet_hdr.encrypt = 0 ;
//		
//		int ret = m_pconn->SendMsg(pMsg, isCrypt ? ESF_ENCRYPT: ESF_NORMAL) ;
//		
//		pMsg->msg_hdr.packet_hdr.encrypt =  isCrypt ;
//		
//		return ret ;
		//return m_sender->streamReplaySend(msgID, pdata, length) ;
	}
	return 0 ;
}


int ReplayStream::Send(void *msgData, int size)
{
	return -1 ;
}
// int ReplayStream::initStream(const char *data, int size)
// {
// 	m_orgData = (char*) data ;
// 	m_curPlayAddr =(char*) data ;
// 	m_size = size ;
// 	m_lastFrameTm = nd_time() ;
// 	
// 	if(0!=getNextFrameTime(&m_curReplayTm) ) {
// 		fprintf(stderr, "data format error \n") ;
// 		return -1;
// 	}
// 	return 0;
// }
// 
// #define CHECK_READ_END()	\
// if (p >= (m_orgData + m_size)) {	\
// 	fprintf(stderr, "read stream end ROBORT TEST END !!!!!!!!!!!!\n");\
// 	return 0 ;				\
// }


int ReplayStream::getNextFrameTime(ndtime_t *sendtm)
{
	if (m_currentSendIndex >= m_replayMsg->size()) {
		return 1;
	}

	replayMsgData *pData = (*m_replayMsg)[m_currentSendIndex];
	*sendtm = pData->sendTm;

	return 0;

// 	char *p = m_curPlayAddr ;
// 	NDUINT16 mark = *((*(NDUINT16**)&p)++) ;
// 	*sendtm = *((*(NDUINT32**)&p)++) ;
// 	
// 	if (p > (m_orgData + m_size)) {
// 		fprintf(stderr, "read stream end ROBORT TEST END !!!!!!!!!!!!\n");
// 		return 1 ;
// 	}
// 	
// 	if (mark != ND_STREAM_MESSAGE_START) {
// 		fprintf(stderr, "read stream data ERROR :( ----------- :(\n");
// 		return -1 ;
// 	}
// 	return 0 ;
}

char *ReplayStream::fetchFrame(ndtime_t *sendtm, int *length )
{

	if (m_currentSendIndex >= m_replayMsg->size()) {
		return 0;
	}

	replayMsgData *pData = (*m_replayMsg)[m_currentSendIndex];
	*sendtm = pData->sendTm;
	*length = pData->size;

	++m_currentSendIndex;
	return pData->data;

// 	char *ret = NULL ;
// 	char *p = m_curPlayAddr ;
// 	NDUINT16 mark = *((*(NDUINT16**)&p)++);
// 	CHECK_READ_END();
// 	
// 	*sendtm = *((*(NDUINT32**)&p)++);
// 
// 	CHECK_READ_END();
// 
// 	char msg_desc[256];
// 
// 	CHECK_READ_END() ;
// 	if (mark!=ND_STREAM_MESSAGE_START ) {
// 		return 0 ;
// 	}
// 	NDUINT16 desclen = *((*(NDUINT16**)&p)++);
// 	if (p + desclen >= m_orgData + m_size)	{
// 		fprintf(stderr, "read stream end ROBORT TEST END !!!!!!!!!!!!\n");
// 		return 0;
// 	}
// 	memcpy(msg_desc, p, desclen);
// 	p += desclen;
// 	msg_desc[desclen] = 0;
// 	
// 	
// 	CHECK_READ_END();
// 	
// 	*length = *((*(NDUINT16**)&p)++) ;
// 	ret = p  ;
// 	p += *length ;
// 	
// 	CHECK_READ_END();
// 	
// 	mark = *((*(NDUINT16**)&p)++) ;
// 	
// 	CHECK_READ_END() ;
// 	if (mark != ND_STREAM_MESSAGE_END) {
// 		fprintf(stderr, "read stream data ERROR no END MARK :( ----------- :(\n");
// 		return 0 ;
// 	}
// 	
// 	m_curPlayAddr = p ;
// 	fprintf(stdout, "fetch %s success\n", msg_desc);
// 	return ret ;
}