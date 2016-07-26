//
//  logicStruct.hpp
//  gameHall
//
//  Created by duanxiuyun on 16/5/9.
//  Copyright 2016 duanxiuyun. All rights reserved.
//

/*
 * implemention of struct for script 
 * this is json like data struct
 */
#ifndef logicStruct_hpp
#define logicStruct_hpp

#include "logicDataType.h"
#include <vector>

#define USER_DEF_VAR_NAME_SIZE 32

class ParamNode
{
public:
	ParamNode():m_val()
	{
		m_name[0] = 0 ;
		
	}
	ParamNode(const char * name, const DBLDataNode &val)
	{
		Init(name, val) ;
	}

	ParamNode &operator =(const ParamNode &r)
	{
		Init(r.m_name, r.m_val) ;
		return  *this ;
	}	
	
	bool operator== (const char *name) const
	{
		return (0 == ndstricmp(this->m_name, name) ) ;
	}
	
	DBLDataNode &getVal() {return m_val;}
	
	friend class LogicUserDefStruct ;
private:
	void Init(const char * name,const DBLDataNode &val)
	{
		m_val = val;
		strncpy(m_name, name, sizeof(m_name)) ;
	}
	char m_name[USER_DEF_VAR_NAME_SIZE] ;
	DBLDataNode m_val ;
};

class LogicUserDefStruct
{
public:
	LogicUserDefStruct(const LogicUserDefStruct &orgData) ;
	LogicUserDefStruct() ;
	
	LogicUserDefStruct &operator =(const LogicUserDefStruct &r);
	bool operator == (const LogicUserDefStruct &r) const;
	
	bool push_back(const char *name,const DBLDataNode &val) ; //add member ,if the name is exist return false
	void set(const char *name, const DBLDataNode &val) ;// add member if name is exist replace it .
	DBLDataNode get(const char *name) ;
	bool pop_back() { m_members.pop_back(); return true; }
	
	
	int Print(logic_print, void *pf) const;
	int Sprint(char *buf, size_t size) const;

	size_t count() const { return m_members.size(); }
	DBLDataNode *ref(size_t index);

protected:
	//void Destroy();
	DBLDataNode *fetch(const char *name) ;
	typedef std::vector<ParamNode> param_vct_t ;
	
	param_vct_t m_members ;
};

#endif /* logicStruct_hpp */
