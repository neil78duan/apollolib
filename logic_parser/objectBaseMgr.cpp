/* file objectBaseMgr.cpp
 *
 * game object base manager 
 *
 * create by duan 
 * 2015-5-13
 */
#include "logic_parser/objectBaseMgr.h"
#include "logic_parser/logicParser.h"
//#include "logic_parser/logicEngineRoot.h"

LogicObjectBase::LogicObjectBase() :m_count(0), m_errParam(0), m_opCheckShortRate(0), m_bOpCheckFoundObj(false)
{

}
LogicObjectBase::~LogicObjectBase()
{

}

void LogicObjectBase::Destroy()
{

}
bool LogicObjectBase::opRead(const DBLDataNode& id, DBLDataNode &val)
{
	m_error = NDERR_NOT_SURPORT;
	return false;
}

bool LogicObjectBase::opWrite(const DBLDataNode& id, const  DBLDataNode &val)
{
	m_error = NDERR_NOT_SURPORT;
	return false;
}

bool LogicObjectBase::opAdd(const DBLDataNode& id, const DBLDataNode &val)
{
	m_error = NDERR_NOT_SURPORT;
	return false;
}

bool LogicObjectBase::opSub(const DBLDataNode& id, const DBLDataNode &val)
{
	m_error = NDERR_NOT_SURPORT;
	return false;
}
// 
// bool LogicObjectBase::opClear(const DBLDataNode& id, const DBLDataNode &val)
// {
// 	m_error = NDERR_NOT_SURPORT;
// 	return false;
// }

bool LogicObjectBase::opCheck(const DBLDataNode& id, const  DBLDataNode &val)
{
	m_error = NDERR_NOT_SURPORT;
	return false;
}
//common operate 
bool LogicObjectBase::opOperate(const char *cmd, const DBLDataNode& id,  DBLDataNode &val)
{

	m_error = NDERR_NOT_SURPORT;
	return false;
}


// bool LogicObjectBase::getObject(eOperatorObjType type, const  DBLDataNode& id, DBLDataNode &val)
// {
// 	return false;
// }

bool LogicObjectBase::getOtherObject(const char*objName, DBLDataNode &val)
{
	m_error = NDERR_NOT_SURPORT;
	return false ;
}

LogicObjectBase *LogicObjectBase::getObjectMgr(const char* destName)
{
	m_error = NDERR_NOT_SURPORT;
	return NULL;
}
int LogicObjectBase::Print(logic_print f, void *pf)
{
	return f(pf, "[LogicObjectBase]");
}

//affaie function
bool LogicObjectBase::BeginAffair()
{
	return m_count++ == 0;
}
bool LogicObjectBase::CommitAffair()
{
	return --m_count==0;
}
bool LogicObjectBase::RollbackAffair()
{
	return --m_count == 0;
}

bool LogicObjectBase::CheckInAffair()
{
	return m_count > 0;
}


NDUINT32 LogicObjectBase::getErrParam()
{
// 	NDUINT32 ret = m_errParam ;
// 	m_errParam = 0 ;
// 	return  ret ;
	return m_errParam;
}

int LogicObjectBase::getError()
{
	return m_error;
// 	NDUINT32 ret = m_error;
// 	m_error = 0;
// 	return  ret;

}


void LogicObjectBase::clearError()
{
	m_error = 0;
	m_errParam = 0;
}

// 
// bool LogicObjectBase::registeObject(const char *name, LogicObjectBase*pobj)
// {
// 	return false;
// }
// void LogicObjectBase::delObject(const char *name)
// {
// 
// }