//
//  sqlThMsgHandler.hpp
//  nationServer
//
//  Created by duanxiuyun on 2017/10/17.
//  Copyright © 2017 duanxiuyun. All rights reserved.
//

#ifndef sqlThMsgHandler_hpp
#define sqlThMsgHandler_hpp

#include "ndapplib/applib.h"
#include "apollo_data.h"
//#include "srv_define.h"
//#include "netMessage/message_inc.h"

struct db_msg_header {
	db_msg_header() : mySessionID(0),peerSessionID(0)
	{
	}
	NDUINT16 mySessionID ;  //session in world server
	NDUINT16 peerSessionID ;//session in game server of room server
};

struct db_outline_msg : public db_msg_header
{
	roleid_t rid;
	nd_usermsgbuf_t msg_buf;
};


struct db_common_data_msg : public db_msg_header
{
	db_common_data_msg()
	{
		serverId = 0;
		datalen = 0;
		param = 0;
		name[0]=0;
	}
	NDUINT32 serverId;
	NDUINT32 datalen;
	NDUINT64 param;
	char name[COMMON_DATA_NAME_SIZE]; 
	char data[0x10000];
	
};


struct db_mail_data_msg : public db_msg_header
{
	db_mail_data_msg()
	{
		to_id = 0 ;
		from_id = 0;
		serverId = 0;
		nationId = 0;
		is_system = 0;
		body_size = 0 ;
		title[0] = 0 ;
		
	}
	roleid_t to_id, from_id ;
	int is_system ;
	NDUINT32 serverId;
	NDUINT32 nationId;
	int body_size ;
	char title[MAIL_TITLE_SIZE];
	char body[MAIL_BODY_SIZE] ;
	
};

struct db_blob_data_update_msg : public db_msg_header
{
	db_blob_data_update_msg()
	{
		serverId = 0;
		datalen = 0;
		id = 0;
		type = 0;
		name[0] = 0;
	}
	int msgSize(){
		return sizeof(*this) - sizeof(data) + datalen;
	}
	NDUINT32 serverId;
	NDUINT32 id;
	NDUINT32 type;
	NDUINT32 datalen;
	char name[COMMON_DATA_NAME_SIZE];
	char data[0x10000];
};
enum eDbBaseThreadMsg{
	DBTHMSG_RUN_SQL = THMSGID_USER_START +1 ,
	
	DBTHMSG_COMMON_LOAD,
	DBTHMSG_COMMON_SAVE,
	DBTHMSG_COMMON_CREATE,
	
	DBTHMSG_OUTLINE_MESSAGE,
	
	DBTHMSG_LOAD_NEW_MAIL,
	DBTHMSG_SAVE_MAIL,
	DBTHMSG_READ_MAIL,


	DBTHMSG_BLOBDATA_UPDATE,
	DBTHMSG_BLOBDATA_CREATE,

	//
	DBTHMSG_SERVER_BASE_MSG_END
};


int dbThread_runsql_handler(nd_thsrv_msg *msg);
int dbThread_save_outline_handler(nd_thsrv_msg *msg);
int dbThread_save_common_data(nd_thsrv_msg *msg);
int dbThread_load_common_data(nd_thsrv_msg *msg);
int dbThread_create_common_data(nd_thsrv_msg *msg);
//int dbThread_create_blob_data(nd_thsrv_msg *msg);
//int dbThread_update_blob_data(nd_thsrv_msg *msg);
int dbThread_save_mail(nd_thsrv_msg *msg);
int dbThread_read_mail(nd_thsrv_msg *msg);
int netThread_common_data_load_ack(nd_thsrv_msg *msg);
int netThread_common_data_create_ack(nd_thsrv_msg *msg);

#endif /* sqlThMsgHandler_hpp */
