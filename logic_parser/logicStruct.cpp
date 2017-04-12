//
//  logicStruct.cpp
//  gameHall
//
//  Created by duanxiuyun on 16/5/9.
//  Copyright 2016 duanxiuyun. All rights reserved.
//

#include "logic_parser/logicStruct.hpp"
#include "logic_parser/logicEndian.h"

static int _read_string(const char *pCmdStream, char *outbuf, size_t size, int byteOrder)
{
	NDUINT16 size16 = 0;
	lp_stream_t p = (lp_stream_t) pCmdStream;
	//size16 = *((*(NDUINT16**)&p)++);
	p = lp_read_stream(p, size16, byteOrder);
	size -= 2;

	if (size16 >= size){
		return -1;
	}
	memcpy(outbuf, p, size16);
	outbuf[size16] = 0;
	p += size16;
	return (int)(p - pCmdStream);
}

LogicUserDefStruct::LogicUserDefStruct(const LogicUserDefStruct &orgData) 
{
	m_members = orgData.m_members;
}
LogicUserDefStruct::LogicUserDefStruct()
{
	
}


int LogicUserDefStruct::FromStream(void *data, size_t size, int byteOrder )
{
	const char *p = (const char*)data;
	char name[256];
	NDUINT16 data_len = 0;
	p = lp_read_stream((lp_stream_t)p, data_len, byteOrder);
	if (data_len > size - 2)
		return	 -1;
	
	const char *pend = p + data_len;

	while (p < pend){
		name[0] = 0;
		int len = _read_string(p, name, sizeof(name),byteOrder);
		if (len == -1){
			return -1;
		}
		p += len;
		size -= len;

		DBLDataNode val;
		len = val.ReadStream(p,size, byteOrder);
		if (-1 == len){
			return -1;
		}
		p += len;
		size -= len;
		push_back(name,  val);

	}
	return (int)(p - (const char*)data);
}

int LogicUserDefStruct::ToStream(void *buf, size_t bufsize, int byteOrder ) const
{
	lp_stream_t p = lp_write_stream((char*)buf, (NDUINT16)0, byteOrder);
	bufsize -= 2;

	param_vct_t::const_iterator it = m_members.begin();
	for (; it != m_members.end(); it++){
		NDUINT16 len = (NDUINT16) strlen(it->m_name);
		if (len >= bufsize)	{
			return -1;
		}
		
		p = lp_write_stream(p, len, byteOrder);
		bufsize -= 2;

		strncpy(p, it->m_name, bufsize );
		bufsize -= len ;
		p += len;
		len = it->m_val.WriteStream(p,bufsize, byteOrder);
		if (-1 == len){
			return -1;
		}
		p += len;
		bufsize -= len;

	}
	NDUINT16 size = (NDUINT16)(p - (char*)buf) -2;
	lp_write_stream((lp_stream_t)buf, size , byteOrder);
	return size + 2 ;
}


LogicUserDefStruct & LogicUserDefStruct::operator =(const LogicUserDefStruct &r)
{
	m_members = r.m_members ;
	return *this ;
}
bool LogicUserDefStruct::operator == (const LogicUserDefStruct &r) const
{
	return  false ;
}


bool LogicUserDefStruct::push_back(const char *name, const DBLDataNode &val)
{
	if (fetch(name)) {
		return  false ;
	}
	m_members.push_back(ParamNode(name, val)) ;
	return true ;
}

void LogicUserDefStruct::set(const char *name, const DBLDataNode &val)
{
	DBLDataNode* pdata = fetch(name) ;
	if (pdata) {
		*pdata = val ;
	}
	else {
		m_members.push_back(ParamNode(name, val)) ;
	}
}
DBLDataNode LogicUserDefStruct::get(const char *name)const
{
	DBLDataNode* pdata = ((LogicUserDefStruct*)this)->fetch(name);
	if (!pdata) {
		return DBLDataNode() ;
	}
	return  *pdata ;
}

int LogicUserDefStruct::Print(logic_print outFunc, void *pf) const
{
	int ret = 0 ;
	param_vct_t::const_iterator it ;
	
	ret += outFunc(pf, "{");
	size_t i = 0;
	for (it = m_members.begin(); it != m_members.end(); ++it) {
		++i;
		ret += outFunc(pf, " %s:", it->m_name);
		ret += it->m_val.Print(outFunc, pf);
		if (i < m_members.size()){
			ret += outFunc(pf, ", ");
		}
		//ret += outFunc(pf, "\n");
	}
	ret += outFunc(pf, "}\n");
	return ret ;
}

int LogicUserDefStruct::Sprint(char *buf, size_t size) const
{
	int ret = 0 ;
	param_vct_t::const_iterator it ;
	for (it=m_members.begin(); it != m_members.end(); ++it) {
		ret +=it->m_val.Sprint(buf, size) ;
	}
	return ret ;
}


DBLDataNode *LogicUserDefStruct::ref(size_t index)
{
	if (index < m_members.size()){
		return & (m_members[index].m_val);
	}
	return NULL;
}

DBLDataNode *LogicUserDefStruct::ref(const char *name)
{
	return fetch(name);
}

DBLDataNode* LogicUserDefStruct::fetch(const char *name)
{
	param_vct_t::iterator it ;
	for (it=m_members.begin(); it != m_members.end(); ++it) {
		if ( (*it) == name ) {
			return  &(it->getVal() );
		}
	}
	return  NULL;
}
