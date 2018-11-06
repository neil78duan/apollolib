/*  file apoGameObj.h
 * implemention of apollo logic parser for game 
 *
 * create by duan 
 *
 * 2017.6.23
 */

#ifndef _APO_GAMEOBJ_
#define _APO_GAMEOBJ_

#include "ndapplib/applib.h"
#include "logic_parser/logicEngineRoot.h"
#include "game_parser/dbldata2netstream.h"

template <class _Tk>
struct apoStringLess
{

	bool operator() (const _Tk& l, const _Tk& r) const
	{
		int ret = ndstricmp(l.c_str(), r.c_str());
		return ret < 0;

	}

};

class ApoGameObj : public LogicObjectBase
{
public:	
	ApoGameObj();
	virtual ~ApoGameObj();

	LogicParserEngine *getScriptHandler() { return &m_logicEngine; }
	bool RunScript(parse_arg_list_t &args, LogicDataObj &result);
	bool RunScript(const char *script);
	bool SendScriptEvent(int event_id, int args, ...);
	bool SendEvent0(int event);
	bool SendEvent1(int event, const LogicDataObj &val1);
	int getScriptError() { return m_logicEngine.getErrno(); }

	bool opRead(const LogicDataObj& id, LogicDataObj &val);
	bool opWrite(const LogicDataObj& id, const LogicDataObj &val);
	bool opAdd(const LogicDataObj& id, const LogicDataObj &val);
	bool opSub(const LogicDataObj& id, const  LogicDataObj &val);

	LogicObjectBase *getObjectMgr(const char* destName);
	bool getOtherObject(const char*objName, LogicDataObj &val);

	typedef std::map<std::string, LogicObjectBase*, apoStringLess<std::string> > objectMgr_vct;
protected:
	
	//objectMgr_vct m_objectMgr;
	LogicParserEngine m_logicEngine;
};


//bool apollo_func_machine_version(LogicParserEngine*parser, parse_arg_list_t &args, LogicDataObj &result);



#endif 


