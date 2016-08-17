//
//  dbDataConvert.h
//  worldManager
//
//  Created by duanxiuyun on 15-4-15.
//  Copyright (c) 2015年 duanxiuyun. All rights reserved.
//

#ifndef worldManager_dbDataConvert_h
#define worldManager_dbDataConvert_h

//#include "UserDataSaveDB.h"
//#include "UserDefineData.h"
//#include "UserDataSaveDB_v1.h"

#include "ndapplib/applib.h"

//using namespace DataTypeDB ;

/*
namespace DataTypeDB
{
	using namespace DataTypeDB ;
	
	template<typename DataType>
	int Stream2Data(char *buf, long size, DataType &data)
	{
		nd_usermsgbuf_t msgbuf ;
		nd_usermsghdr_init(&msgbuf.msg_hdr);
		char *p = buf ;
		
		NDUINT16 versionID = *((*(NDUINT16**)&p)++) ;
		size -= 2 ;
		
		
		memcpy(msgbuf.data, p,  size) ;
		ND_USERMSG_LEN(&msgbuf) += size ;
		NDIStreamMsg inmsg(&msgbuf) ;
		
		return ReadStream(inmsg,data) ;
	}
	
	template<typename DataType>
	int Data2Stream(DataType &data, char *buf, long &size)
	{
		NDOStreamMsg omsg(0,0) ;
		
		if(WriteStream(omsg, data)==-1) {
			return -1 ;
		}
		if (size < omsg.GetDataLen() + 2) {
			return -1;
		}
		
		char *p = buf ;
		*((*(NDUINT16**)&p)++) =DATA_IN_DB_VERSION ;
		memcpy(p, omsg.MsgData(), omsg.GetDataLen()) ;
		size = omsg.GetDataLen() + 2 ;
		return 0;
	}
	
};
*/
// 
// int UserProfessionToCurVersion(char *buf, long size, NetMessage::UserProfessionData &roledata);
// namespace NetMessage
// {
// 	
// 	//using namespace Protocol;
// 	template<typename DataType>
// 	int Stream2Data(char *buf, long size, DataType &data)
// 	{
// 		nd_usermsgbuf_t msgbuf ;
// 		nd_usermsghdr_init(&msgbuf.msg_hdr);
// 		char *p = buf ;
// 		
// 		NDUINT16 versionID = *((*(NDUINT16**)&p)++) ;
// 		size -= 2 ;
// 		
// 		
// 		memcpy(msgbuf.data, p,  size) ;
// 		ND_USERMSG_LEN(&msgbuf) +=(NDUINT16) size ;
// 		NDIStreamMsg inmsg(&msgbuf) ;
// 		
// 		return ReadStream(inmsg,data) ;
// 	}
// 	
// 	template<typename DataType>
// 	int Data2Stream(DataType &data, char *buf, long &size)
// 	{
// 		NDOStreamMsg omsg(0,0) ;
// 		
// 		if(WriteStream(omsg, data)==-1) {
// 			return -1 ;
// 		}
// 		if (size < omsg.GetDataLen() + 2) {
// 			return -1;
// 		}
// 		
// 		char *p = buf ;
// 		*((*(NDUINT16**)&p)++) =DATA_IN_DB_VERSION ;
// 		memcpy(p, omsg.MsgData(), omsg.GetDataLen()) ;
// 		size = omsg.GetDataLen() + 2 ;
// 		return 0;
// 	}
// }

#endif
