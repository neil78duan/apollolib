/* file apoGameCommon.h
 * 
 * define apollo plugin for game server
 *
 * create by duan
 *
 * 2018.11.6
 */

#ifndef _APO_GAME_COMMON_H_
#define _APO_GAME_COMMON_H_

#include "nd_common/nd_common.h"

#include "logic_parser/objectBaseMgr.h"
#include "logic_parser/logicEngineRoot.h"
#include <vector>


 //LogicObjectBase
class apoLogicObject4Game : public LogicObjectBase
{
	typedef LogicObjectBase _myBase;
public:
	apoLogicObject4Game();
	virtual~apoLogicObject4Game();
	
	bool opCheck(const LogicDataObj& id, const  LogicDataObj &val);
	bool getOtherObject(const char*objName, LogicDataObj &val);
	LogicObjectBase *getObjectMgr(const char* destName);
	
	virtual LogicParserEngine *getScriptHandler();

};

class NDBaseConnector ;
int apollo_message_script_entry(void *engine, NDBaseConnector*  handle, nd_usermsgbuf_t *msg, const char *script);
nd_handle apollo_getNetHanlder(ILogicObject *owner, LogicDataObj &netObj);

#endif
