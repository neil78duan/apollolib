/* file objectBaseMgr.h
 *
 * game object base manager 
 *
 * create by duan 
 * 2015-5-13
 */

 #ifndef _OBJECT_BASE_MGR_H_
 #define _OBJECT_BASE_MGR_H_

#include "logic_parser/logicDataType.h"


typedef NDUINT32 operator_index_t;


#define CHECK_ID_INVALID(_id) \
	if (!id.CheckValid())	{	\
		m_error = NDERR_SCRIPT_INSTRUCT; \
		return false;			\
	}

//logicParser Engine run the command Read/Write/Add/Sub/Clear/Check only throw objects LogicObjectBase
//so you need inherit LogicObjectBase if your object need be operated by script.
class LogicObjectBase
{
public:
	LogicObjectBase();
	virtual ~LogicObjectBase();
	virtual 	void Destroy();

public:
	virtual bool opRead(const DBLDataNode& id, DBLDataNode &val) = 0; //copy data

	virtual bool opWrite(const DBLDataNode& id, const  DBLDataNode &val) = 0;

	virtual bool opAdd(const DBLDataNode& id, const  DBLDataNode &val) = 0;

	virtual bool opSub(const DBLDataNode& id, const  DBLDataNode &val) = 0;

	//virtual bool opClear(const DBLDataNode& id, const  DBLDataNode &val) = 0;

	virtual bool opCheck(const DBLDataNode& id, const DBLDataNode &val) ;

	//common operate @val  input-output
	virtual bool opOperate(const char *cmd, const DBLDataNode& id,  DBLDataNode &val); 

	
	//get object manager for opRead/write/...
	virtual LogicObjectBase *getObjectMgr(const char* destName);
	
	//get other object
	virtual bool getOtherObject(const char*objName, DBLDataNode &val) ;

	virtual int Print(logic_print f, void *pf); //print object self info 

	//affair function
	virtual bool CheckInAffair();
	virtual bool BeginAffair();
	virtual bool CommitAffair();
	virtual bool RollbackAffair();
	
	NDUINT32 getErrParam() ;
	int getError();
	void clearError();

	int getCheckShortRate() { return m_opCheckShortRate; }
	bool checkObjectIsExist() { return m_bOpCheckFoundObj; }
	void clearCheckShortRate() { m_bOpCheckFoundObj= false; m_opCheckShortRate = 0; }
protected:
	int m_count;

	int m_error;
	NDUINT32 m_errParam ;
	NDUINT8 m_opCheckShortRate;
	bool m_bOpCheckFoundObj;
};

class LogicObjAffairHelper
{
public:
	LogicObjAffairHelper(LogicObjectBase *ta) :m_affair(ta)
	{
		m_affair->BeginAffair();
	}
	virtual ~LogicObjAffairHelper()
	{
		if (m_affair){
			m_affair->CommitAffair();
			m_affair = NULL;
		}
	}
	void Rollback()
	{
		m_affair->RollbackAffair();
		m_affair = NULL;
	}
private:
	LogicObjectBase *m_affair;

};

//LogicObjectBase
class TestLogicObject : public LogicObjectBase
{
public:
	TestLogicObject();
	virtual~TestLogicObject ();

	bool opRead(const DBLDataNode& id, DBLDataNode &val);
	bool opWrite(const DBLDataNode& id, const DBLDataNode &val);
	bool opAdd(const DBLDataNode& id, const DBLDataNode &val);
	bool opSub(const DBLDataNode& id, const  DBLDataNode &val);
	//bool opClear(const DBLDataNode& id, const  DBLDataNode &val);
	bool opCheck(const DBLDataNode& id, const  DBLDataNode &val);
	//common operate 
	bool opOperate(const char *cmd, const DBLDataNode& id,  DBLDataNode &val);
	//bool getObject(eOperatorObjType type, const DBLDataNode &id, DBLDataNode &val);
	//LogicObjectBase *getObjectMgr(eOperatorDestMgr destID);
	
	bool getOtherObject(const char*objName, DBLDataNode &val) ;
	LogicObjectBase *getObjectMgr(const char* destName);
	int Print(logic_print f, void *pf); //print object self info 

	bool BeginAffair();
	bool CommitAffair();
	bool RollbackAffair();

private:

	void _setval(DBLDataNode &val);
};

#endif 
 