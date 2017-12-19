//
//  CommonDataSql.h
//  worldManager
//
//  Created by duanxiuyun on 15-5-26.
// COMMON DATA save load update sql stm
//  Copyright (c) 2015年 duanxiuyun. All rights reserved.
//

#ifndef __SRV_COMMON_DATASQL__
#define __SRV_COMMON_DATASQL__


#include "db_common/mydatabase.h"
#include "common_inc.h"


//update rider
class CreateCommonDataSql: public SqlBindStm
{
public:
	int Create( CMyDatabase* dbhandle ) ;
	RESULT_T CommonDataCreate(const char *name ,int serverId, void *data, size_t size);
protected:
	enum {BIND_NUMB = 3 };
	MYSQL_BIND    m_binds[BIND_NUMB];
	
	unsigned long m_serverId ;
	unsigned long m_data_len ;
	unsigned long m_name_len;
	char m_name[COMMON_DATA_NAME_SIZE] ;
	char m_databuf[0xffff] ;
};

class LoadCommonDataSql: public SqlBindStm
{
public:
	int Create( CMyDatabase* dbhandle ) ;
	RESULT_T LoadData(const char *name , int serverId, char *buf, size_t &size) ;
	
protected:
	enum {BIND_NUMB = 2 };
	MYSQL_BIND    m_binds[BIND_NUMB];
	
	unsigned long m_serverId ;
	
	unsigned long m_name_len;
	char m_name[COMMON_DATA_NAME_SIZE] ;
};


class SaveCommonDataSql: public SqlBindStm
{
public:
	int Create(CMyDatabase *dbhandle) ;
	
	RESULT_T SaveData(const char *name, int serverId, void *data, size_t size );
protected:
	
	enum {BIND_NUMB = 3};
	MYSQL_BIND    m_binds[BIND_NUMB];
	
	unsigned long m_serverId ;

	unsigned long m_data_len ;
	
	unsigned long m_name_len;
	char m_name[COMMON_DATA_NAME_SIZE] ;
	char m_databuf[0xffff] ;
};


////////////////////////////////
class SaveOutlineMsgSql: public SqlBindStm
{
public:
	int Create(CMyDatabase *dbhandle) ;
	RESULT_T SaveMsg(roleid_t,  void *data , NDUINT32 msg_len) ;
protected:
	enum {BIND_NUMB = 2};
	MYSQL_BIND    m_binds[BIND_NUMB];
	
	unsigned long m_accIndex ;
	unsigned long  m_datalen  ;
	char m_data[0xffff] ;
};


class SaveMail: public SqlBindStm
{
public:
	int Create(CMyDatabase *dbhandle) ;
	RESULT_T Save(NDUINT32 to_id, NDUINT32 from_id, NDUINT32 server_id, NDUINT32 nation_id, bool isSystem, const char *title, void *data, size_t size);
	int Read(NDUINT32 mailId, void *data, size_t size);
protected:
	enum {BIND_NUMB = 7};
	MYSQL_BIND    m_binds[BIND_NUMB];
	
	unsigned long m_fromId, m_toId , m_isSystem,m_serverId,m_nationId;
	long m_dataLen,m_titleLen ;
	
	char m_title[MAIL_TITLE_SIZE] ;
	
	char m_data[MAIL_BODY_SIZE] ;
};

class ReceiveMail : public SqlBindStm
{
public:
	int Create(CMyDatabase *dbhandle);
	RESULT_T Receive(NDUINT32 mailId, void *data, size_t size);
protected:
	enum { BIND_NUMB = 2 };
	MYSQL_BIND    m_binds[BIND_NUMB];
	
	unsigned long mail_id;
	long m_dataLen;
	
	char m_data[MAIL_BODY_SIZE];
};


#endif /* defined(__worldManager__CommonDataSql__) */
