//
//  testConnector.cpp
//  clientDemo
//
//  Created by duanxiuyun on 15-2-4.
//  Copyright (c) 2015年 duanxiuyun. All rights reserved.
//


#include "testConnector.h"
#include "msg_def.h"



SendHelper::SendHelper()
{
	m_pConn = 0 ;
}
SendHelper::~SendHelper()
{
	
}
int SendHelper::Send(void *data , int size )
{
	nd_usermsgbuf_t *pMsg = (nd_usermsgbuf_t*) data ;
	
	//skip login and create role message
	if (pMsg->msg_hdr.maxid == NETMSG_MAX_LOGIN) {
		return 0;
	}
	if (pMsg->msg_hdr.maxid == NETMSG_MAX_ROLE && pMsg->msg_hdr.minid <= ROLE_MSG_CREATE_ROLE_ACK) {
		return 0;
	}
	
	NDUINT8 isCrypt = pMsg->msg_hdr.packet_hdr.encrypt ;
	pMsg->msg_hdr.packet_hdr.encrypt = 0 ;
	
	int ret = m_pConn->SendMsg(pMsg, isCrypt ? ESF_ENCRYPT: ESF_NORMAL) ;
	
	pMsg->msg_hdr.packet_hdr.encrypt =  isCrypt ;
	
	return ret ;
	
}
void SendHelper::SetConn(NDIConn *conn)
{
	m_pConn = conn ;
}


///////////

TestConnector::TestConnector()
{
	
}
TestConnector::~TestConnector()
{
	
}

int TestConnector::initStream(const char *stream, size_t size)
{
	m_replay.SetConn(m_pConn) ;
	return m_replay.initStream(stream, (int) size) ;
}

int TestConnector::update()
{
	int ret = -1 ;
	if (m_pConn) {
		m_pConn->Update(0) ;
		ret = m_replay.update() ;
	}
	return ret ;
}


/////////////////////////

TestconnsMgr::TestconnsMgr() 
{
	//m_capacity = 0 ;
	m_maxNum = 0 ;
	m_tickIndex = 0 ;
	m_streamMessage = 0 ;
	m_streamSize = 0 ;
	
}
TestconnsMgr::~TestconnsMgr() 
{
	
}

int TestconnsMgr::init( const char*host, int port , int num, int acc_start_index)
{
	m_maxNum = num;
	if (m_maxNum> MAX_CONNECTOR_NUM) {
		m_maxNum = MAX_CONNECTOR_NUM;
	}
	char passwd[100] ;
	char username[32] ;
	
	strncpy(passwd, "test123", sizeof(passwd)) ;
	
	
	for (int i=0; i<m_maxNum; ++i) {		
		snprintf(username, sizeof(username), "roborts_%d", i+acc_start_index) ;
		
		if(m_conns[i].Create(host, port, username, passwd) ==-1) {
			fprintf(stderr, "%s login error \n", username) ;
		}
		else {
			m_conns[i].initStream(m_streamMessage, m_streamSize) ;
		}
	}
	
	return 0;
	
}


int TestconnsMgr::initStreamMsg(const char *file)
{
	m_streamMessage = (char*) nd_load_file(file, &m_streamSize) ;
	if (m_streamMessage) {
		return 0;
	}
	return -1 ;
}

int TestconnsMgr::Update()
{
	int success = 0 ;
	for (int i=0; i<m_maxNum; ++i) {
		if (m_conns[i].CheckValid()) {
			if(-1==m_conns[i].update() ) {
				m_conns[i].Close(1) ;
			}
			success ++ ;
		}
		
	}
	++ m_tickIndex ;
	if (success) {
		return 0 ;
	}
	return -1;
}

void TestconnsMgr::Destroy()
{
	for (int i=0; i<m_maxNum; ++i) {
		m_conns[i].Close(0);
		//m_waitInviteConns[i].Destroy();
	}
	if (m_streamMessage) {
		nd_unload_file(m_streamMessage) ;
		m_streamMessage = 0 ;
		m_streamSize = 0 ;
	}
}






