/*  file apoGameObj.h
* implemention of apollo logic parser for game
*
* create by duan
*
* 2017.6.23
*/


#include "apo_http/apoGameObj.h"
#include "ndapplib/applib.h"
#include "logic_parser/logicEngineRoot.h"
#include "logic_parser/dbldata2netstream.h"
//#include "srv_common/playerMgr.h"


//bool apollo_func_machine_version(LogicParserEngine*parser, parse_arg_list_t &args, DBLDataNode &result)
APOLLO_SCRIPT_API_DEF_GLOBAL(apollo_func_machine_version, "��ȡ������Ϣ()")
{
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


APOLLO_SCRIPT_API_DEF(apollo_create_tcp_connector, "TCP_connector_create(host,port)")
{
	CHECK_ARGS_NUM(args, 3, parser);

	NDConnector *pConnector = new NDConnector;

	if (-1 == pConnector->Create("tcp-connector")) {
		nd_logerror("create tcp connector error\n");
		return false;
	}

	if (-1 == pConnector->Open(args[1].GetText(), args[2].GetInt(), "tcp-connector")) {
		nd_logerror("con not open %s %d\n", args[1].GetText(), args[2].GetInt());
		return false;
	}

	LogicParserEngine &logicEngine = LogicEngineRoot::get_Instant()->getGlobalParser();
	if (-1 == nd_message_set_script_engine(pConnector->GetHandle(), &logicEngine, apollo_message_script_entry)) {
		nd_logerror("set script engine for GMConnector error\n");
	}

	NDInstanceBase *pBase = getbase_inst();
	if (pBase && pBase->GetDeftListener()) {
		pBase->GetDeftListener()->Attach(*pConnector);
	}

	result.InitSet(pConnector);
	return true;
}

ApoGameObj::ApoGameObj() : m_logicEngine(this)
{

}
ApoGameObj::~ApoGameObj()
{
	
}


bool ApoGameObj::RunScript(parse_arg_list_t &args, DBLDataNode &result)
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

bool ApoGameObj::RunScript(const char *script)
{
	ND_TRACE_FUNC();

	parse_arg_list_t args;
	DBLDataNode val, result;
	val.InitSet(script);
	args.push_back(val);

	bool ret = m_logicEngine.runScript(script, args, result);
	if (!ret) {
		nd_logerror("run %s error %d\n", script, m_logicEngine.getErrno());
	}
	return ret;

}
bool ApoGameObj::SendScriptEvent(int event_id, int argc, ...)
{
	ND_TRACE_FUNC();

	parse_arg_list_t params;

	params.push_back( DBLDataNode ((void*)this, OT_OBJ_BASE_OBJ));

	va_list arg;
	va_start(arg, argc);
	while (argc-- > 0) {
		DBLDataNode *data1 = va_arg(arg, DBLDataNode*);
		params.push_back(*data1);
	}
	va_end(arg);

	return m_logicEngine.eventNtf(event_id, params);
}

bool ApoGameObj::SendEvent0(int event)
{
	ND_TRACE_FUNC();
	return SendScriptEvent(event, 0);
}
bool ApoGameObj::SendEvent1(int event, const DBLDataNode &val1)
{
	ND_TRACE_FUNC();
	return SendScriptEvent(event, 1, &val1);
}

//////////////////////////////////////////////////////////////////////////

bool ApoGameObj::opRead(const DBLDataNode& id, DBLDataNode &val)
{
	PARSE_TRACE("logic_engine_test: opRead(%d) \n", id.GetInt());
	//_setval(val);
	return true;
}

bool ApoGameObj::opWrite(const DBLDataNode& id, const DBLDataNode &val)
{
	PARSE_TRACE("logic_engine_test: opWrite(%d) \n", id.GetInt());
	//_setval(val);
	return true;
}


bool ApoGameObj::opAdd(const DBLDataNode& id, const DBLDataNode &val)
{
	PARSE_TRACE("logic_engine_test: opAdd(%d) \n", id.GetInt());
	//_setval(val);
	return true;
}


bool ApoGameObj::opSub(const DBLDataNode& id, const  DBLDataNode &val)
{
	PARSE_TRACE("logic_engine_test: opSub(%d) \n", id.GetInt());
	//_setval(val);
	return true;
}

LogicObjectBase *ApoGameObj::getObjectMgr(const char* destName)
{
	if (0 == ndstricmp(destName, "owner")) {
		return  this;
	}
	return NULL;
}

bool ApoGameObj::getOtherObject(const char*objName, DBLDataNode &val)
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

	else if (0 == ndstricmp(objName, "FormatMsgData"))	{
		userDefineDataType_map_t &msgObj = LogicEngineRoot::get_Instant()->getGlobalDataType();
		val.InitSet((void*)&msgObj);
		return true;
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
	else if (0 == ndstricmp(objName, "WritablePath")) {
		val.InitSet(nd_getcwd());
		return true;
	}

	else if (0 == ndstricmp(objName, "self")) {
		val.InitSet((void*)this, OT_OBJ_BASE_OBJ);
		return true;
	}

	else if (0 == ndstricmp(objName, "machineInfo")) {
		parse_arg_list_t lists;
		apollo_func_machine_version(NULL, lists, val);
		return true;
	}

	return false;
}