/*  file LogicObject4Server.h
* implemention of apollo logic parser for game
*
* create by duan
*
* 2017.6.23
*/


#include "ndapplib/applib.h"
#include "logic_parser/logicEngineRoot.h"
#include "game_parser/apoObjSrv.h"
#include "game_parser/dbldata2netstream.h"
#include "game_parser/apoGameCommon.h"
//#include "srv_common/playerMgr.h"


//bool apollo_func_machine_version(LogicParserEngine*parser, parse_arg_list_t &args, LogicDataObj &result)
APOLLO_SCRIPT_API_DEF_GLOBAL(apollo_func_machine_version, "获取主机信息()")
{
	ND_TRACE_FUNC();
	char buf[512];

	if (getbase_inst()->CheckIsDeveVer()) {
		char machInfo[256];
		snprintf(buf, sizeof(buf), "%s-develop", nd_common_machine_info(machInfo, sizeof(machInfo)));
	}
	else {
		nd_common_machine_info(buf, sizeof(buf));
	}
	result.InitSet(buf);
	return true;
}


LogicObject4Server::LogicObject4Server() : m_logicEngine(this)
{

}
LogicObject4Server::~LogicObject4Server()
{
	
}


bool LogicObject4Server::RunScript(parse_arg_list_t &args, LogicDataObj &result)
{
	ND_TRACE_FUNC();
	if (args.size() < 1 || args[0].GetDataType() != OT_STRING){
		return false;
	}
	const char *script = args[0].GetText();

	bool ret = m_logicEngine.runScript(script, args, result);
	if (!ret) {
		nd_logerror("run %s error %d\n", script, m_logicEngine.getErrno());
	}
	return ret;
}

bool LogicObject4Server::RunScript(const char *script)
{
	ND_TRACE_FUNC();

	parse_arg_list_t args;
	LogicDataObj val, result;
	val.InitSet(script);
	args.push_back(val);

	bool ret = m_logicEngine.runScript(script, args, result);
	if (!ret) {
		nd_logerror("run %s error %d\n", script, m_logicEngine.getErrno());
	}
	return ret;

}
bool LogicObject4Server::SendScriptEvent(int event_id, int argc, ...)
{
	ND_TRACE_FUNC();

	parse_arg_list_t params;

	params.push_back( LogicDataObj ((void*)this, OT_OBJ_BASE_OBJ));

	va_list arg;
	va_start(arg, argc);
	while (argc-- > 0) {
		LogicDataObj *data1 = va_arg(arg, LogicDataObj*);
		params.push_back(*data1);
	}
	va_end(arg);

	return m_logicEngine.eventNtf(event_id, params);
}

bool LogicObject4Server::SendEvent0(int event)
{
	ND_TRACE_FUNC();
	return SendScriptEvent(event, 0);
}
bool LogicObject4Server::SendEvent1(int event, const LogicDataObj &val1)
{
	ND_TRACE_FUNC();
	return SendScriptEvent(event, 1, &val1);
}


bool LogicObject4Server::getOtherObject(const char*objName, LogicDataObj &val)
{
	ND_TRACE_FUNC();
	if (0 == ndstricmp(objName, "listener"))	{
		nd_handle hListen = getbase_inst()->GetDeftListener()->GetHandle();
		val.InitSet((void*)hListen, OT_OBJ_NDHANDLE);
		return true;
	}

	else if (0 == ndstricmp(objName, "config-file"))	{

		const char *pFile = getbase_inst()->Getcfgfile();
		if (pFile) {
			val.InitSet(pFile);
			return true;
		}
	}
	
	else if (0 == ndstricmp(objName, "LogPath")) {
		NDInstanceBase *pInst = getbase_inst();
		server_config *pcfg = pInst->GetInstConfig();
		if (pcfg && pcfg->i_cfg.log_file[0])	{
			char buf[ND_FILE_PATH_SIZE];
			val.InitSet(nd_getpath(pcfg->i_cfg.log_file, buf, sizeof(buf)));
			return true;
		}
	}
	else if (0 == ndstricmp(objName, "LogFile")) {
		NDInstanceBase *pInst = getbase_inst();
		server_config *pcfg = pInst->GetInstConfig();
		if (pcfg && pcfg->i_cfg.log_file[0])	{
			val.InitSet(pcfg->i_cfg.log_file);
			return true;
		}
	}

	else if (0 == ndstricmp(objName, "machineInfo")) {
		parse_arg_list_t lists;
		apollo_func_machine_version(NULL, lists, val);
		return true;
	}
	else {
		return apoLogicObject4Game::getOtherObject(objName, val);
	}

	return false;
}
