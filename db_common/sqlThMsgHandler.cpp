//
//  sqlThMsgHandler.cpp
//  nationServer
//
//  Created by duanxiuyun on 2017/10/17.
//  Copyright © 2017年 duanxiuyun. All rights reserved.
//

#include "sqlThMsgHandler.hpp"
#include "CommonDataSql.h"
#include "db_common/gameDBConnector.h"
#include "common_inc.h"
#include "db_common/commonDataMgr.h"


int dbThread_runsql_handler(nd_thsrv_msg *msg)
{
	ND_TRACE_FUNC() ;
	
	ServerDbConnBase* dbconn= get_DatabaseHandle();
	if (dbconn){
		char revsed = msg->data[msg->data_len-1] ;
		msg->data[msg->data_len-1] = 0 ;
		dbconn->RunSql((char*)msg->data);
// 		
// 		if(0==pdb->sql_execute((char*)msg->data) ) {
// 			if (0== pdb->sql_store_result()){
// 				pdb->sql_free_result() ;
// 			}
// 		}
// 		else {
// 			nd_logerror("run sql %s error = %s\n", (char*)msg->data, pdb->sql_error()) ;
// 		}
		msg->data[msg->data_len-1] = revsed ;
	}
	return 0 ;
}

int dbThread_save_outline_handler(nd_thsrv_msg *msg)
{
	ND_TRACE_FUNC();
	db_outline_msg *pmsg = (db_outline_msg*)msg->data;
	
	RESULT_T ret = get_SaveOutlineMsgSql()->SaveMsg(pmsg->rid, (void*)&pmsg->msg_buf, ND_USERMSG_LEN(&pmsg->msg_buf));
	if (ret != ESERVER_ERR_SUCCESS) {
		nd_logerror("SAVE %u outline message(%d,%d) error  %d\n" AND
					pmsg->rid AND ND_USERMSG_MAXID(&pmsg->msg_buf) AND ND_USERMSG_MINID(&pmsg->msg_buf) AND ret);
	}
	
	return 0;
}

int dbThread_save_common_data(nd_thsrv_msg *msg)
{
	ND_TRACE_FUNC();
	db_common_data_msg *recvmsg = (db_common_data_msg*)msg->data;
	db_common_data_msg loadMsgData;
	
	if (recvmsg->name[0] == 0) {
		return 0;
	}
	
	RESULT_T ret = get_SaveCommonDataSql()->SaveData(recvmsg->name,recvmsg->serverId, recvmsg->data, recvmsg->datalen);
	if (ESERVER_ERR_SUCCESS != ret) {
		nd_logerror("save common data error %d\n", ret);
	}
	return 0;
}


RESULT_T _load_common_data (int msgId, db_common_data_msg *recvmsg)
{
	ND_TRACE_FUNC();
	db_common_data_msg loadMsgData ;
	
	if (recvmsg->name[0]==0) {
		return (RESULT_T)NDERR_INVALID_INPUT;
	}
	
	size_t size = sizeof(loadMsgData.data) ;
	RESULT_T ret = get_LoadCommonDataSql()->LoadData(recvmsg->name,recvmsg->serverId, loadMsgData.data, size);
	
	if (ret == ESERVER_ERR_SUCCESS) {
		
		loadMsgData.datalen = (NDUINT32) size ;
		loadMsgData.param = recvmsg->param ;
		loadMsgData.mySessionID = recvmsg->mySessionID ;
		loadMsgData.peerSessionID = recvmsg->peerSessionID ;
		
		int msg_size = sizeof(loadMsgData) - sizeof(loadMsgData.data) + loadMsgData.datalen ;
		sqlSendtoNetThread(msgId, &loadMsgData, msg_size) ;
		
	}
	
	return ret ;
}

int dbThread_load_common_data(nd_thsrv_msg *msg)
{
	ND_TRACE_FUNC() ;
	db_common_data_msg *recvmsg = (db_common_data_msg*) msg->data ;
	
	_load_common_data (msg->msg_id, recvmsg);
	
	return 0 ;
}


int dbThread_create_common_data(nd_thsrv_msg *msg)
{
	ND_TRACE_FUNC() ;
	db_common_data_msg *recvmsg = (db_common_data_msg*) msg->data ;
	
	if (recvmsg->name[0]==0) {
		return 0;
	}
	RESULT_T ret = get_CreateCommonDataSql()->CommonDataCreate(recvmsg->name,recvmsg->serverId, recvmsg->data, recvmsg->datalen);
	
	if (ret == ESERVER_ERR_SUCCESS) {
		sqlSendtoNetThread(msg->msg_id, recvmsg, msg->data_len) ;
	}
	return 0 ;
}
// 
// int dbThread_create_blob_data(nd_thsrv_msg *msg){
// 	ND_TRACE_FUNC();
// 	db_blob_data_update_msg* recvmsg = (db_blob_data_update_msg*)msg->data;
// 	CMyDatabase*pdb = get_DatabaseHandle()->GetDBHandle();
// 	if (!pdb){
// 		return 0;
// 	}
// 
// 	char sql[10240];
// 	memset(sql, 0, sizeof(sql));
// 	int len = snprintf(sql, sizeof(sql), "insert into %s ( id,serverid,type,blobdata ) values ( %d, %d,%d,'", recvmsg->name, recvmsg->id, (int)recvmsg->serverId, (int)recvmsg->type);
// 	len += mysql_real_escape_string(pdb->get_mysql_handle(), sql + len, recvmsg->data, recvmsg->datalen);
// 	len += snprintf( sql + len, sizeof(sql)-len, "' );" );
// 	if (pdb->sql_execute(sql) != 0) {
// 		nd_logerror("run sql error : %s\n" AND pdb->sql_error());
// 		return false;
// 	}
// 	return 0;
// }
// 
// int dbThread_update_blob_data(nd_thsrv_msg *msg){
// 	ND_TRACE_FUNC();
// 	db_blob_data_update_msg* recvmsg = (db_blob_data_update_msg*)msg->data;
// 	CMyDatabase*pdb = get_DatabaseHandle()->GetDBHandle();
// 	if (!pdb){
// 		return 0;
// 	}
// 
// 	char sql[10240];
// 	memset(sql, 0, sizeof(sql));
// 	int len = snprintf(sql, sizeof(sql), "update %s set blobdata ='", recvmsg->name);
// 	len += mysql_real_escape_string(pdb->get_mysql_handle(), sql + len, recvmsg->data, recvmsg->datalen);
// 	len += snprintf(sql + len, sizeof(sql) - len, "' where id = %d and serverid = %d and type = %d;", (int)recvmsg->id, (int)recvmsg->serverId,(int)recvmsg->type);
// 	if (pdb->sql_execute(sql) != 0) {
// 		nd_logerror("run sql error : %s\n" AND pdb->sql_error());
// 		return false;
// 	}
// 	return 0;
// }


int dbThread_save_mail(nd_thsrv_msg *msg)
{
	ND_TRACE_FUNC() ;
	db_mail_data_msg *recvmsg = (db_mail_data_msg*) msg->data ;
	
	get_SaveMail()->Save(recvmsg->to_id, recvmsg->from_id, recvmsg->serverId,
									   recvmsg->is_system?true:false, recvmsg->title,
									   recvmsg->body, recvmsg->body_size) ;
	
	
	return 0 ;
}
int dbThread_read_mail(nd_thsrv_msg *msg)
{
	ND_TRACE_FUNC();
	db_mail_data_msg *recvmsg = (db_mail_data_msg*)msg->data;
	get_ReceiveMail()->Receive(recvmsg->from_id, recvmsg->body, recvmsg->body_size);
	return 0;
}


int netThread_common_data_load_ack(nd_thsrv_msg *msg)
{
	ND_TRACE_FUNC() ;
	db_common_data_msg *rmsg = (db_common_data_msg*) msg->data ;
	
	CommonDataMgr* dataMgr = get_CommonDataMgr() ;
	if (dataMgr) {
		dataMgr->onDataLoad(rmsg->name, rmsg->serverId, rmsg->data, rmsg->datalen, rmsg->param) ;
	}
	//WorldInstance &inst = get_instance() ;
	//inst.m_commonData.OnLoadData((RESULT_T)recvmsg->errorCode,recvmsg) ;
	return  0 ;
}

int netThread_common_data_create_ack(nd_thsrv_msg *msg)
{
	ND_TRACE_FUNC() ;
	db_common_data_msg *rmsg = (db_common_data_msg*) msg->data ;
	
	CommonDataMgr* dataMgr = get_CommonDataMgr() ;
	if (dataMgr) {
		dataMgr->onDataCreate(rmsg->name, rmsg->serverId, rmsg->data, rmsg->datalen, rmsg->param) ;
	}
	
	//WorldInstance &inst = get_instance() ;
	//inst.m_commonData.OnCreateData((RESULT_T)recvmsg->errorCode ,recvmsg) ;
	return  0 ;
}
