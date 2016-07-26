//
//  dataStream.cpp
//  gameHall
//
//  Created by duanxiuyun on 15-4-16.
//  Copyright (c) 2015 duanxiuyun. All rights reserved.
//

#include "dataStream.h"

#ifndef BUILD_AS_THIRD_PARTY
namespace NetMessage
#endif
{
	int ReadStream(NDIStreamMsg &inmsg,NDUINT8 &data)
	{
		return inmsg.Read(data) ;
	}
	int ReadStream(NDIStreamMsg &inmsg,NDUINT16 &data)
	{
		return inmsg.Read(data) ;
	}
	int ReadStream(NDIStreamMsg &inmsg,NDUINT32 &data)
	{
		return inmsg.Read(data) ;
	}
	int ReadStream(NDIStreamMsg &inmsg,NDUINT64 &data)
	{
		return inmsg.Read(data) ;
	}
	
	int ReadStream(NDIStreamMsg &inmsg,NDINT8 &data)
	{
		return inmsg.Read((NDUINT8&)data) ;
	}
	int ReadStream(NDIStreamMsg &inmsg,NDINT16 &data)
	{
		return inmsg.Read((NDUINT16&)data) ;
	}
	int ReadStream(NDIStreamMsg &inmsg,NDINT32 &data)
	{
		return inmsg.Read((NDUINT32&)data) ;
	}
	int ReadStream(NDIStreamMsg &inmsg,NDINT64 &data)
	{
		return inmsg.Read((NDUINT64&)data) ;
	}
	int ReadStream(NDIStreamMsg &inmsg,float &data)
	{
		return inmsg.Read((float&)data) ;
	}
	//WRITE STREAM
	
	int WriteStream(NDOStreamMsg &omsg,NDUINT8 data)
	{
		return omsg.Write(data) ;
	}
	int WriteStream(NDOStreamMsg &omsg,NDUINT16 data)
	{
		return omsg.Write(data) ;
	}
	int WriteStream(NDOStreamMsg &omsg,NDUINT32 data)
	{
		return omsg.Write(data) ;
	}
	int WriteStream(NDOStreamMsg &omsg,NDUINT64 data)
	{
		return omsg.Write(data) ;
	}
	
	int WriteStream(NDOStreamMsg &omsg,NDINT8 data)
	{
		return omsg.Write((NDUINT8)data) ;
	}
	int WriteStream(NDOStreamMsg &omsg,NDINT16 data)
	{
		return omsg.Write((NDUINT16)data) ;
	}
	int WriteStream(NDOStreamMsg &omsg,NDINT32 data)
	{
		return omsg.Write((NDUINT32)data) ;
	}
	int WriteStream(NDOStreamMsg &omsg,NDINT64 data)
	{
		return omsg.Write((NDUINT64)data);
	}
	int WriteStream(NDOStreamMsg &omsg,float data)
	{
		return omsg.Write(data) ;
	}
	
	
	int ReadStream(NDIStreamMsg &inmsg,char* buf, int buf_size)
	{
		inmsg.Read((NDUINT8*)buf, buf_size) ;
		return 0 ;
	}
	int WriteStream(NDOStreamMsg &omsg,const char* data)
	{
		return omsg.Write((const NDUINT8 *)data) ;
	}

};
