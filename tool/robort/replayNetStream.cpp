//
//  replayNetStream.cpp
//  roborts
//
//  Created by duanxiuyun on 15/11/26.
//  Copyright © 2015年 duanxiuyun. All rights reserved.
//

#include "replayNetStream.hpp"

/////////////////////////////

ReplayStream::ReplayStream()
{
	m_orgData= 0 ;
	m_size = 0 ;
	m_curPlayAddr = 0 ;
	m_curReplayTm = 0 ; //replay time
	
	m_lastFrameTm = nd_time() ;
	
}
ReplayStream::~ReplayStream()
{
	
}

int ReplayStream::update()
{
	if (!m_orgData) {
		return 0;
	}
	ndtime_t now = nd_time() ;
	ndtime_t tminterval = now - m_lastFrameTm ;
	
	m_curReplayTm += tminterval ;
	m_lastFrameTm = now ;
	
	ndtime_t nextFrameTm = 0 ;
	if(0!=getNextFrameTime(&nextFrameTm) ) {
		return -1 ;
	}
	
	if (m_curReplayTm >= nextFrameTm) {
		int length;
		char * pdata = fetchFrame(&nextFrameTm, &length ) ;
		if (!pdata) {
			return -1 ;
		}
		
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
int ReplayStream::initStream(const char *data, int size)
{
	m_orgData = (char*) data ;
	m_curPlayAddr =(char*) data ;
	m_size = size ;
	m_lastFrameTm = nd_time() ;
	
	if(0!=getNextFrameTime(&m_curReplayTm) ) {
		fprintf(stderr, "data format error \n") ;
		return -1;
	}
	return 0;
}

#define CHECK_READ_END()	\
if (p > (m_orgData + m_size)) {	\
return 0 ;				\
}


int ReplayStream::getNextFrameTime(ndtime_t *sendtm)
{
	char *p = m_curPlayAddr ;
	NDUINT16 mark = *((*(NDUINT16**)&p)++) ;
	*sendtm = *((*(ndtime_t**)&p)++) ;
	
	if (p > (m_orgData + m_size)) {
		return 1 ;
	}
	
	if (mark!=ND_STREAM_MESSAGE_START ) {
		return -1 ;
	}
	return 0 ;
}

char *ReplayStream::fetchFrame(ndtime_t *sendtm, int *length )
{
	char *ret = NULL ;
	char *p = m_curPlayAddr ;
	NDUINT16 mark = *((*(NDUINT16**)&p)++) ;
	
	CHECK_READ_END() ;
	if (mark!=ND_STREAM_MESSAGE_START ) {
		return 0 ;
	}
	
	*sendtm = *((*(ndtime_t**)&p)++) ;
	
	CHECK_READ_END();
	
	*length = *((*(NDUINT16**)&p)++) ;
	ret = p  ;
	p += *length ;
	
	CHECK_READ_END();
	
	mark = *((*(NDUINT16**)&p)++) ;
	
	CHECK_READ_END() ;
	if (mark!=ND_STREAM_MESSAGE_END ) {
		return 0 ;
	}
	
	m_curPlayAddr = p ;
	return ret ;
}