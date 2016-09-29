/* file objectBaseMgr.cpp
 *
 * game object base manager 
 *
 * create by duan 
 * 2015-5-13
 */
#include "logic_parser/objectBaseMgr.h"
#include "logic_parser/logicParser.h"

LogicObjectBase::LogicObjectBase() :m_count(0)
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

bool LogicObjectBase::opClear(const DBLDataNode& id, const DBLDataNode &val)
{
	m_error = NDERR_NOT_SURPORT;
	return false;
}

bool LogicObjectBase::opCheck(const DBLDataNode& id, const  DBLDataNode &val)
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
	return f(pf, "unknow object\n");
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
	NDUINT32 ret = m_errParam ;
	m_errParam = 0 ;
	return  ret ;
	
}

int LogicObjectBase::getError()
{
	NDUINT32 ret = m_error;
	m_error = 0;
	return  ret;

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
////////////////////////////////////////////////////
///test object manager
TestLogicObject::TestLogicObject()
{

}
TestLogicObject::~TestLogicObject()
{

}

bool TestLogicObject::opRead(const DBLDataNode& id, DBLDataNode &val)
{
	PARSE_TRACE("logic_engine_test: opRead(%d) \n", id.GetInt());
	_setval(val);
	return true;
}

bool TestLogicObject::opWrite(const DBLDataNode& id, const DBLDataNode &val)
{
	PARSE_TRACE("logic_engine_test: opWrite(%d) \n", id.GetInt());
	//_setval(val);
	return true;
}


bool TestLogicObject::opAdd(const DBLDataNode& id, const DBLDataNode &val)
{
	PARSE_TRACE("logic_engine_test: opAdd(%d) \n", id.GetInt());
	//_setval(val);
	return true;
}


bool TestLogicObject::opSub(const DBLDataNode& id, const  DBLDataNode &val)
{
	PARSE_TRACE("logic_engine_test: opSub(%d) \n", id.GetInt());
	//_setval(val);
	return true;
}

bool TestLogicObject::opClear(const DBLDataNode& id, const  DBLDataNode &val)
{
	PARSE_TRACE("logic_engine_test: opClear(%d) \n", id.GetInt());
	return true;
}


bool TestLogicObject::opCheck(const DBLDataNode& id, const  DBLDataNode &val)
{
	if (val.GetDataType()==OT_STRING){
		const char *pText = val.GetText();
		if (pText && *pText){
			if (0==ndstricmp(pText,"test-error"))	{
				m_error = NDERR_SUCCESS;
				return false;
			}
// 			else if (0 == ndstricmp(pText, "test-success"))	{
// 				m_error = NDERR_SUCCESS;
// 				return true;
// 			}
		}
	}
	//m_error = NDERR_NOT_SURPORT;
	return true;
}


bool TestLogicObject::getOtherObject(const char*objName, DBLDataNode &val)
{
	//PARSE_TRACE("logic_engine_test: getObject(%d, %d) \n",type);
	_setval(val);
	return true;

}
LogicObjectBase *TestLogicObject::getObjectMgr(const char* destName)
{
	PARSE_TRACE("logic_engine_test: getObjectMgr(%s) \n", destName);
	return this;
}


int TestLogicObject::Print(logic_print f, void *pf)
{
	PARSE_TRACE("this is object for test_engine\n");
	return 31;
}

bool TestLogicObject::BeginAffair()
{
	PARSE_TRACE("logic_engine_test: BeginAffair() \n");
	return true;
}

bool TestLogicObject::CommitAffair()
{
	PARSE_TRACE("logic_engine_test: CommitAffair \n");
	return true;
}

bool TestLogicObject::RollbackAffair()
{
	PARSE_TRACE("logic_engine_test: RollbackAffair() \n");
	return true;
}



void TestLogicObject::_setval(DBLDataNode &val)
{
	DBLDataNode data;
	data.InitSet("0");
	val = data;
}