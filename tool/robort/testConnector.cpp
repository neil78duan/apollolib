//
//  testConnector.cpp
//  clientDemo
//
//  Created by duanxiuyun on 15-2-4.
//  Copyright (c) 2015年 duanxiuyun. All rights reserved.
//


#include "testConnector.h"
#include "msg_def.h"
//#include "tinyxml/tinyxml.h"
//#include "tinyxml/tinystr.h"
#include "apollo_data.h"


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
	if (pMsg->msg_hdr.maxid < NETMSG_MAX_NUMBER ) {
		return size;
	}

	fprintf(stdout, "replay send (%d,%d) len =%d \n", pMsg->msg_hdr.maxid, pMsg->msg_hdr.minid, pMsg->msg_hdr.packet_hdr.length);

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

int TestConnector::initStream(replay_vct &msgvct)
{
	m_replay.SetConn(m_pConn) ;
	return m_replay.initStream(msgvct) ;
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
	//m_streamMessage = 0 ;
	//m_streamSize = 0 ;
	m_accPrefix = "rbt";
}
TestconnsMgr::~TestconnsMgr() 
{
	
}


int TestconnsMgr::init(const char*host, int port, int num, int acc_start_index)
{
	m_maxNum = num;
	if (m_maxNum > MAX_CONNECTOR_NUM) {
		m_maxNum = MAX_CONNECTOR_NUM;
	}
	char passwd[100];
	char username[32];

	strncpy(passwd, "test123", sizeof(passwd));


	for (int i = 0; i < m_maxNum; ++i) {
		snprintf(username, sizeof(username), "%s_%d", m_accPrefix.c_str(), i + acc_start_index);

		if (m_conns[i].Create(host, port, username, passwd, ACC_APOLLO) == -1) {
			fprintf(stderr, "%s login error \n", username);
		}
		else {
			m_conns[i].initStream(m_replyMsgbuf);
		}
	}
	return 0;
}
int TestconnsMgr::initWithSdk(const char *sdkAccFile, const char*host, int port, int num, int acc_start_index)
{

	ndxml_root doc;
	if (-1 == ndxml_load(sdkAccFile, &doc)) {
		return -1;
	}
	ndxml *root = ndxml_getnode(&doc, "root");
	if (!root){
		return -1;
	}
	for (int i = 0; i < ndxml_getsub_num(root); i++)	{
		ndxml *element = ndxml_getnodei(root, i);
		const char* username = ndxml_getattr_val(element, "name");
		const char* pwd = ndxml_getattr_val(element, "pwd");

		if (username == 0 || pwd == 0){
			continue;
		}
		if (m_maxNum >= MAX_CONNECTOR_NUM){
			break;
		}
		if (m_conns[m_maxNum].Create(host, port, username, pwd, ACC_OTHER_3_ACCID) == -1) {
			fprintf(stderr, "%s login error \n", username);
		}
		else {
			m_conns[m_maxNum].initStream(m_replyMsgbuf);
		}
		m_maxNum++;
	}
	return 0;

}


int TestconnsMgr::initStreamMsg(const char *file)
{
	//int descLength;
	char msg_desc[128];
	FILE *fp = fopen(file, "rb");
	if (!fp) {
		return -1;
	}

	NDUINT32 now = 0;
	NDUINT16 mark = ND_STREAM_MESSAGE_START;

	while (!feof(fp)) {
		if (0 == fread(&mark, 1, sizeof(mark), fp)) {
			fclose(fp);
			return 0;
		}
		if (mark != ND_STREAM_MESSAGE_START) {
			fclose(fp);
			fprintf(stderr, "load data from stream file  BEGIN MARK error\n");
			return -1;
		}

		//write time
		fread(&now, 1, sizeof(now), fp);

		//write message descript
		fread(&mark, 1, sizeof(mark), fp);
		fread(msg_desc, 1, mark, fp);
		msg_desc[mark] = 0;


		//data size
		mark = 0;
		if (fread(&mark, sizeof(mark), 1,  fp) > 0 && mark > 0) {
			replayMsgData *pdata = (replayMsgData *)malloc(mark + sizeof(replayMsgData));
			//WRITE data
			fread(pdata->data, 1, mark, fp);
			pdata->sendTm = now;
			pdata->size = mark;
			fread(&mark, 1, sizeof(mark),  fp);
			if (mark != ND_STREAM_MESSAGE_END) {
				fprintf(stderr, "load data from stream file  ENDMARK error\n");
				free(pdata);
				fclose(fp);
				return -1;
			}
			m_replyMsgbuf.push_back(pdata);
		}
		else {
			fclose(fp);
			fprintf(stderr, "load data from stream file  error data size =0\n");
			return -1;
		}

	}
	
	fclose(fp);
	return 0;

// 	m_streamMessage = (char*) nd_load_file(file, &m_streamSize) ;
// 	if (m_streamMessage) {
// 		return 0;
// 	}
// 	return -1 ;
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
	for (size_t i = 0; i < m_replyMsgbuf.size(); i++)	{
		free(m_replyMsgbuf[i]);
 	}
	m_replyMsgbuf.clear();
// 	if (m_streamMessage) {
// 		nd_unload_file(m_streamMessage) ;
// 		m_streamMessage = 0 ;
// 		m_streamSize = 0 ;
// 	}
}






