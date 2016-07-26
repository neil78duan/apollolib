//
//  logicStruct.cpp
//  gameHall
//
//  Created by duanxiuyun on 16/5/9.
//  Copyright 2016 duanxiuyun. All rights reserved.
//

#include "logicStruct.hpp"

LogicUserDefStruct::LogicUserDefStruct(const LogicUserDefStruct &orgData) 
{
	m_members = orgData.m_members;
}
LogicUserDefStruct::LogicUserDefStruct()
{
	
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
DBLDataNode LogicUserDefStruct::get(const char *name)
{
	DBLDataNode* pdata = fetch(name) ;
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
		ret += outFunc(pf, "\t%s:", it->m_name);
		ret += it->m_val.Print(outFunc, pf);
		if (i < m_members.size()){
			ret += outFunc(pf, ",\n");
		}
	}
	ret += outFunc(pf, "}");
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
