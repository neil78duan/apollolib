/* file logicGameBaseObj.cpp
 *
 * define common base LogicObject for game development
 *
 * create by duan 
 *
 * 2018-11-7
 *
 */


#include "game_parser/apoGameCommon.h"
#include "game_parser/dbldata2netstream.h"
#include "game_parser/dbl_mgr.h"

#include "ndapplib/applib.h"
#include "logic_parser/logicEngineRoot.h"
#include "logic_parser/logicParser.h"
#include "logic_parser/logic_compile.h"
#include "logic_parser/logic_function.h"
#include "logic_parser/logic_editor_helper.h"
#include "nd_common/cJSON.h"


int apollo_message_script_entry(void *engine, nd_handle  handle, nd_usermsgbuf_t *msg, const char *script)
{
	LogicParserEngine *scriptEngine = (LogicParserEngine *)engine;
	if (!scriptEngine) {
		NDObject *pObj = NDObject::FromHandle(handle);
		if (!pObj) {
			return -1;
		}
		scriptEngine = (LogicParserEngine *)pObj->getScriptEngine();
		if (!scriptEngine) {
			nd_logerror("can not get role script parser\n");
			return -1;
		}
	}

	NDIStreamMsg inmsg(msg);
	//call function 
	parse_arg_list_t params;

	//function name 
	params.push_back(LogicDataObj(script));

	//receive message user
	params.push_back(LogicDataObj((void*)handle, OT_OBJ_NDHANDLE));

	//message object
	params.push_back(LogicDataObj((void*)&inmsg, OT_OBJ_MSGSTREAM));

	LogicDataObj result;
	scriptEngine->runScript(script, params, result);

	return 0;

}


 ////////////////////////////////////////////////////
 ///test object manager
apoLogicObject4Game::apoLogicObject4Game()
{

}
apoLogicObject4Game::~apoLogicObject4Game()
{

}

bool apoLogicObject4Game::opRead(const LogicDataObj& id, LogicDataObj &val)
{
	PARSE_TRACE("logic_engine_test: opRead(%d) \n", id.GetInt());
	val.InitSet(0);
	return true;
}

bool apoLogicObject4Game::opWrite(const LogicDataObj& id, const LogicDataObj &val)
{
	PARSE_TRACE("logic_engine_test: opWrite(%d) \n", id.GetInt());
	//_setval(val);
	return true;
}


bool apoLogicObject4Game::opAdd(const LogicDataObj& id, const LogicDataObj &val)
{
	PARSE_TRACE("logic_engine_test: opAdd(%d) \n", id.GetInt());
	//_setval(val);
	return true;
}


bool apoLogicObject4Game::opSub(const LogicDataObj& id, const  LogicDataObj &val)
{
	PARSE_TRACE("logic_engine_test: opSub(%d) \n", id.GetInt());
	//_setval(val);
	return true;
}



bool apoLogicObject4Game::opCheck(const LogicDataObj& id, const  LogicDataObj &val)
{
	if (val.GetDataType() == OT_STRING) {
		const char *pText = val.GetText();
		if (pText && *pText) {
			if (0 == ndstricmp(pText, "test-error")) {
				m_error = NDERR_SUCCESS;
				return false;
			}
		}
	}
	return true;
}

bool apoLogicObject4Game::opOperate(const char *cmd, const LogicDataObj& id, LogicDataObj &val)
{
	PARSE_TRACE("logic_engine_test: opOperate(%s,%d,%d) \n", cmd, id.GetInt(), val.GetInt());
	return true;
}


bool apoLogicObject4Game::getOtherObject(const char*objName, LogicDataObj &val)
{
	if (0 == ndstricmp(objName, "machineInfo")) {
		char buf[256];
		val.InitSet(nd_common_machine_info(buf, sizeof(buf)));
	}
	else if (0 == ndstricmp(objName, "FormatMsgData")) {
		val.InitSet((void*)&getGlobalNetMsgFormat());
	}
	else if (0 == ndstricmp(objName, "self")) {
		val.InitSet((void*)this, OT_OBJ_BASE_OBJ);
	}	
	
	else if (0 == ndstricmp(objName, "settingFile")) {
		const char *pSettingFile = LogicCompiler::get_Instant()->getConfigFileName();
		val.InitSet(pSettingFile);
	}
	else if (0 == ndstricmp(objName, "globalParser")) {
		LogicParserEngine &parser = LogicEngineRoot::get_Instant()->getGlobalParser();
		val.InitSet(&parser);
	}
	else if (0 == ndstricmp(objName, "WritablePath")) {
		val.InitSet(nd_getcwd());
	}
	else if (0 == ndstricmp(objName, "myParser")) {
		LogicParserEngine *parser = getScriptHandler();
		if (parser) {
			val.InitSet(&parser);
		}
		else {
			m_error = LOGIC_ERR_AIM_OBJECT_NOT_FOUND;
			return false;
		}
	}
	else {
		m_error = LOGIC_ERR_AIM_OBJECT_NOT_FOUND;
		return false;
	}
	return true;
}

LogicObjectBase *apoLogicObject4Game::getObjectMgr(const char* destName)
{
	if (0 == ndstricmp(destName, "owner")) {
		return  this;
	}
	m_error = LOGIC_ERR_AIM_OBJECT_NOT_FOUND;
	PARSE_TRACE("logic_engine_test: getObjectMgr(%s) \n", destName);
	return NULL;
}


int apoLogicObject4Game::Print(logic_print f, void *pf)
{
	PARSE_TRACE("this is object for test_engine\n");
	return 31;
}

bool apoLogicObject4Game::BeginAffair()
{
	PARSE_TRACE("logic_engine_test: BeginAffair() \n");
	return true;
}

bool apoLogicObject4Game::CommitAffair()
{
	PARSE_TRACE("logic_engine_test: CommitAffair \n");
	return true;
}

bool apoLogicObject4Game::RollbackAffair()
{
	PARSE_TRACE("logic_engine_test: RollbackAffair() \n");
	return true;
}

LogicParserEngine *apoLogicObject4Game::getScriptHandler()
{
	PARSE_TRACE("LogicParserEngine: get my parser not implemention \n");
	return NULL;
}

