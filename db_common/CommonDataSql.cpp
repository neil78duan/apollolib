//
//  CommonDataSql.cpp
//  worldManager
//
//  Created by duanxiuyun on 15-5-26.
//  Copyright (c) 2015年 duanxiuyun. All rights reserved.
//

#include "CommonDataSql.h"


int CreateCommonDataSql::Create( CMyDatabase* dbhandle )
{
	ND_TRACE_FUNC() ;
	const char *SQL_STM = "insert into at_common_data (server_id, name, userdata,last_update)"
		" values(?,?,?,now() )" ;
	
	memset(m_binds, 0, sizeof(m_binds));
	//account ID
	
	SET_BIND_CONTEXT(&m_binds[0], MYSQL_TYPE_LONG, &m_serverId,0, 0, 0 ) ;
	SET_BIND_CONTEXT(&m_binds[1], MYSQL_TYPE_STRING, m_name,sizeof(m_name), 0, &m_name_len ) ;
	SET_BIND_CONTEXT(&m_binds[2], MYSQL_TYPE_BLOB, m_databuf,sizeof(m_databuf), 0, &m_data_len ) ;
	
	m_dbhandle = dbhandle;
	PREPARE_STM(m_stmt, dbhandle,SQL_STM, m_binds,BIND_NUMB) ;
	return 0 ;
	
}

RESULT_T CreateCommonDataSql::CommonDataCreate(const char *name ,int serverId, void *data, size_t size)
{
	ND_TRACE_FUNC() ;
	
	RESULT_T ret = ESERVER_ERR_SUCCESS;
	
	memcpy(&m_databuf, data, size) ;
	m_data_len =(unsigned long) size ;
	
	m_serverId = serverId ;
	
	m_name_len = (unsigned long)strlen(name) ;
	if (m_name_len > 0) {
		strncpy(m_name, name, sizeof(m_name)) ;
	}
	
	if (stmt_execute()){
		return NDSYS_ERR_SYSTEM;
	}
	
	return  ret ;
}

/////////////////////////////

int LoadCommonDataSql::Create( CMyDatabase* dbhandle )
{
	ND_TRACE_FUNC() ;
	const char *SQL_STM = "select userdata from at_common_data where server_id = ? and name = ? ;" ;
	
	memset(m_binds, 0, sizeof(m_binds));
	
	SET_BIND_CONTEXT(&m_binds[0], MYSQL_TYPE_LONG, &m_serverId,0,0,0 ) ;
	SET_BIND_CONTEXT(&m_binds[1], MYSQL_TYPE_STRING, m_name,sizeof(m_name), 0, &m_name_len ) ;
	
	m_dbhandle = dbhandle;
	PREPARE_STM(m_stmt, dbhandle,SQL_STM, m_binds,BIND_NUMB) ;
	return 0 ;
	
}


RESULT_T LoadCommonDataSql::LoadData(const char *name , int serverId, char *buf, size_t &size)
{
	ND_TRACE_FUNC() ;
#define  RESULT_NUM 1
	RESULT_T ret = NDSYS_ERR_SYSTEM ;
	unsigned long length[RESULT_NUM] = {0} ;
	MYSQL_BIND    bind[RESULT_NUM] ={0};
	my_bool       is_null[RESULT_NUM];
	
	m_serverId = serverId ;
	
	m_name_len = (unsigned long)strlen(name) ;
	if (m_name_len > 0) {
		strncpy(m_name, name, sizeof(m_name)) ;
	}
	
	if (stmt_execute()){
		return NDSYS_ERR_SYSTEM;
	}
	
	SET_BIND_CONTEXT(&bind[0], MYSQL_TYPE_BLOB, buf,size, &is_null[0], &length[0]) ;
	
	if (start_result(bind))
		return NDSYS_ERR_SYSTEM;
	
	ret = ESERVER_ERR_SUCCESS ;
	if(0==m_dbhandle->stmt_fetch(m_stmt) ) {
		size = length[0] ;
	}
	else {
		ret = NDSYS_ERR_MYSQL;
	}
	
	end_result() ;
	
	return  ret ;
#undef RESULT_NUM
}
//////////////

int SaveCommonDataSql::Create( CMyDatabase* dbhandle )
{
	ND_TRACE_FUNC() ;

	const char *SQL_STM = "call save_common_data(?,?,?)";	
	memset(m_binds, 0, sizeof(m_binds));
	SET_BIND_CONTEXT(&m_binds[0], MYSQL_TYPE_STRING, m_name, sizeof(m_name), 0, &m_name_len);
	SET_BIND_CONTEXT(&m_binds[1], MYSQL_TYPE_LONG, &m_serverId, 0, 0, 0);
	SET_BIND_CONTEXT(&m_binds[2], MYSQL_TYPE_BLOB, m_databuf,sizeof(m_databuf), 0, &m_data_len ) ;
	
	m_dbhandle = dbhandle;
	PREPARE_STM(m_stmt, dbhandle,SQL_STM, m_binds,BIND_NUMB) ;
	return 0 ;
	
}

RESULT_T SaveCommonDataSql::SaveData(const char *name, int serverId, void *data, size_t size )
{
	ND_TRACE_FUNC() ;
	
	
	RESULT_T ret = ESERVER_ERR_SUCCESS;
	
	memcpy(&m_databuf, data, size) ;
	m_data_len = (unsigned long)size ;
	
	m_serverId = serverId ;
	
	m_name_len = (unsigned long)strlen(name) ;
	if (m_name_len > 0) {
		strncpy(m_name, name, sizeof(m_name)) ;
	}
	
	if (stmt_execute()){
		return NDSYS_ERR_SYSTEM;
	}

	return ESERVER_ERR_SUCCESS;
}

/////////////////////////
//
int SaveOutlineMsgSql::Create( CMyDatabase* dbhandle )
{
	ND_TRACE_FUNC() ;
	const char *SQL_STM = "select save_outline_msg(? , ?) " ;
	
	memset(m_binds, 0, sizeof(m_binds));
	
	SET_BIND_CONTEXT(&m_binds[0], MYSQL_TYPE_LONG, &m_accIndex,0, 0, 0 ) ;
	SET_BIND_CONTEXT(&m_binds[1], MYSQL_TYPE_BLOB, &m_data,sizeof(m_data), 0, &m_datalen) ;
	
	m_dbhandle = dbhandle;
	PREPARE_STM(m_stmt, dbhandle,SQL_STM, m_binds,BIND_NUMB) ;
	return 0 ;
	
}

RESULT_T SaveOutlineMsgSql::SaveMsg(roleid_t rid,  void *data , NDUINT32 msg_len)
{
	ND_TRACE_FUNC() ;
	
#define  RESULT_NUM 1
	MYSQL_BIND    bind[RESULT_NUM] ={0};
	
	RESULT_T ret = ESERVER_ERR_SUCCESS;
	unsigned long saveOk = 0;
	
	if (msg_len <= 0) {
		return (RESULT_T)NDERR_INVALID_INPUT ;
	}
	memcpy(&m_data, data, msg_len) ;
	m_datalen = msg_len ;
	m_accIndex = rid ;
	
	if (stmt_execute()){
		return NDSYS_ERR_SYSTEM;
	}
	SET_BIND_CONTEXT(&bind[0], MYSQL_TYPE_LONG, &saveOk,0, 0, 0 ) ;
	
	if (start_result(bind))
		return NDSYS_ERR_SYSTEM;
	
	m_dbhandle->stmt_fetch(m_stmt) ;
	end_result() ;
	return  saveOk == 0 ? ret : ESERVER_ERR_AIM_ROLE_NOT_EXIST;
}
///////////////////////



////////
int SaveMail::Create( CMyDatabase* dbhandle )
{
	ND_TRACE_FUNC() ;
	const char *SQL_STM = "select save_mail(?, ?, ? ,?, ?, ? )" ;
	memset(m_binds, 0, sizeof(m_binds));
	
	SET_BIND_CONTEXT(&m_binds[0], MYSQL_TYPE_LONG, &m_fromId,0, 0, 0 ) ;
	SET_BIND_CONTEXT(&m_binds[1], MYSQL_TYPE_LONG, &m_toId,0, 0, 0 ) ;
	SET_BIND_CONTEXT(&m_binds[2], MYSQL_TYPE_LONG, &m_serverId, 0, 0, 0);
	SET_BIND_CONTEXT(&m_binds[3], MYSQL_TYPE_STRING, &m_title,sizeof(m_title), 0, &m_titleLen ) ;
	SET_BIND_CONTEXT(&m_binds[4], MYSQL_TYPE_LONG, &m_isSystem,0, 0, 0 ) ;
	SET_BIND_CONTEXT(&m_binds[5], MYSQL_TYPE_MEDIUM_BLOB, &m_data,sizeof(m_data), 0, &m_dataLen) ;
	
	m_dbhandle = dbhandle;
	PREPARE_STM(m_stmt, dbhandle,SQL_STM, m_binds,BIND_NUMB) ;
	return 0 ;
	
}

RESULT_T SaveMail::Save(NDUINT32 to_id, NDUINT32 from_id, NDUINT32 server_id, bool isSystem, const char *title, void *data, size_t size)
{
	ND_TRACE_FUNC() ;
	int ret =-1;
#define  RESULT_NUM 1
	MYSQL_BIND    bind[RESULT_NUM] = { 0 };
	
	m_fromId = from_id ;
	m_toId = to_id ;
	m_isSystem = isSystem ;
	m_serverId = server_id;
	
	if (title && *title) {
		m_titleLen = (long)strlen(title);
		strncpy(m_title, title, sizeof(m_title)) ;
	}
	
	memcpy(m_data, data, size) ;
	m_dataLen = (long)size;
		
	if (stmt_execute()){
		return NDSYS_ERR_SYSTEM;
	}
	
	/////////////////
	
	SET_BIND_CONTEXT(&bind[0], MYSQL_TYPE_LONG, &ret, 0, 0, 0);
	if (start_result(bind))
		return NDSYS_ERR_SYSTEM;
	
	if (0 == m_dbhandle->stmt_fetch(m_stmt)) {
		
	}
	end_result();
	
	return ret == -1 ? NDSYS_ERR_MYSQL : ESERVER_ERR_SUCCESS;
}
int ReceiveMail::Create(CMyDatabase *dbhandle)
{
	ND_TRACE_FUNC();
	const char *SQL_STM = "update at_mails set context=? where mail_id =? ";
	//a_from int unsigned ,a_to int unsigned, a_title char(40), is_system int , a_data BLOB
	memset(m_binds, 0, sizeof(m_binds));
	
	SET_BIND_CONTEXT(&m_binds[0], MYSQL_TYPE_MEDIUM_BLOB, &m_data, sizeof(m_data), 0, &m_dataLen);
	SET_BIND_CONTEXT(&m_binds[1], MYSQL_TYPE_LONG, &mail_id, 0, 0, 0);
	
	m_dbhandle = dbhandle;
	PREPARE_STM(m_stmt, dbhandle, SQL_STM, m_binds, BIND_NUMB);
	return 0;
}

RESULT_T ReceiveMail::Receive(NDUINT32 mailId, void *data, size_t size)
{
	ND_TRACE_FUNC();
	
	mail_id = mailId;
	memcpy(m_data, data, size);
	m_dataLen = (long)size;
	
	if (stmt_execute()){
		return NDSYS_ERR_SYSTEM;
	}
	
	
	return ESERVER_ERR_SUCCESS ;
	
}


