//
//  dataStream.h
//  gameHall
//
//  Created by duanxiuyun on 15-4-14.
//  Copyright (c) 2015 duanxiuyun. All rights reserved.
//

#ifndef gameHall_dataStream_h
#define gameHall_dataStream_h


#ifndef BUILD_AS_THIRD_PARTY
#include "nd_common/nd_common.h"
#include "ndapplib/nd_msgpacket.h"
#else
#include "ndlib.h"
#endif

#ifndef BUILD_AS_THIRD_PARTY
#include "netMessage/MessageID.h"
namespace NetMessage
#endif
{
	
	int ReadStream(NDIStreamMsg &inmsg,NDUINT8 &data);
	
	int ReadStream(NDIStreamMsg &inmsg,NDUINT16 &data);
	int ReadStream(NDIStreamMsg &inmsg,NDUINT32 &data);
	int ReadStream(NDIStreamMsg &inmsg,NDUINT64 &data);
	
	int ReadStream(NDIStreamMsg &inmsg,NDINT8 &data);
	int ReadStream(NDIStreamMsg &inmsg,NDINT16 &data);
	int ReadStream(NDIStreamMsg &inmsg,NDINT32 &data);
	int ReadStream(NDIStreamMsg &inmsg,NDINT64 &data);
	int ReadStream(NDIStreamMsg &inmsg,float &data);
	//WRITE STREAM
	
	int WriteStream(NDOStreamMsg &omsg,NDUINT8 data);
	int WriteStream(NDOStreamMsg &omsg,NDUINT16 data);
	int WriteStream(NDOStreamMsg &omsg,NDUINT32 data);
	int WriteStream(NDOStreamMsg &omsg,NDUINT64 data);
	
	int WriteStream(NDOStreamMsg &omsg,NDINT8 data);
	int WriteStream(NDOStreamMsg &omsg,NDINT16 data);
	int WriteStream(NDOStreamMsg &omsg,NDINT32 data);
	int WriteStream(NDOStreamMsg &omsg,NDINT64 data);
	int WriteStream(NDOStreamMsg &omsg,float data);

	int ReadStream(NDIStreamMsg &inmsg,char* buf, int buf_size);
	int WriteStream(NDOStreamMsg &omsg, const char* data);
	
	
#ifndef BUILD_AS_THIRD_PARTY
	const char *getNameByID(NDUINT32 id);
	NDUINT32 getIDByName(const char *name);
	
	//using namespace Protocol;
	template<typename DataType>
	int Stream2Data(const char *StreamBuf, size_t stream_size, DataType &data)
	{
		nd_usermsgbuf_t msgbuf ;
		nd_usermsghdr_init(&msgbuf.msg_hdr);
		
		memcpy(msgbuf.data, StreamBuf, stream_size);
		ND_USERMSG_LEN(&msgbuf) += (NDUINT16)stream_size;
		NDIStreamMsg inmsg(&msgbuf) ;
		
		return ReadStream(inmsg,data) ;
	}
	
	template<typename DataType>
	int Data2Stream(DataType &data, char *StreamBuf, size_t size)
	{
		NDOStreamMsg omsg(0,0) ;
		
		if(WriteStream(omsg, data)==-1) {
			return -1 ;
		}
		if(omsg.GetDataLen() > size) {
			return -1 ;
		}
		int data_len =(int) omsg.GetDataLen() ;
		memcpy(StreamBuf, omsg.MsgData(), data_len);
		
		return (int)data_len;
	}
#endif
	
};

#ifndef BUILD_AS_THIRD_PARTY
//using namespace Protocol ;
#define READ_STREAM(_msg_stream, _data) \
	do {								\
		if(-1==NetMessage::ReadStream(_msg_stream, _data) ) {	\
			return -1 ; \
		}				\
	}while(0)

#define READ_STREAM_STRING(_msg_stream, _data, _len) \
	do {								\
		if(-1==NetMessage::ReadStream(_msg_stream, _data, _len) ) {	\
			return -1 ; \
		}				\
	}while(0)


#define WRITE_STREAM(_msg_stream, _data) \
	do {								\
		if(-1==NetMessage::WriteStream(_msg_stream, _data) ) {	\
			return -1 ; \
		}				\
	}while(0)
#else 

//using namespace Protocol ;
#define READ_STREAM(_msg_stream, _data) \
do {								\
	if(-1==ReadStream(_msg_stream, _data) ) {	\
		return -1 ; \
	}				\
}while(0)

#define READ_STREAM_STRING(_msg_stream, _data, _len) \
do {								\
	if(-1==ReadStream(_msg_stream, _data, _len) ) {	\
		return -1 ; \
	}				\
}while(0)


#define WRITE_STREAM(_msg_stream, _data) \
do {								\
	if(-1==WriteStream(_msg_stream, _data) ) {	\
		return -1 ; \
	}				\
}while(0)

#endif

#endif
