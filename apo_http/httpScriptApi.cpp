/* file http_script_api.cpp
 *
 * export api for http script 
 *
 * create by duan 
 *
 */


#include "ndapplib/applib.h"
#include "ndapplib/nd_httpListener.h"
#include "logic_parser/logicEngineRoot.h"
#include "logic_parser/dbldata2netstream.h"
#include "apo_http/httpScriptApi.h"

APOLLO_SCRIPT_API_DEF(apollo_set_http_handler, "http_安装处理函数(listenerName, reqPath,scriptName)")
{
	CHECK_ARGS_NUM(args, 4, parser);

	DBLDataNode netHandle;
	LogicObjectBase *owner = parser->getOwner();

	if (owner && owner->getOtherObject(args[1].GetText(), netHandle)) {
		nd_handle hListen = (nd_handle)netHandle.GetObjectAddr();
		if (hListen) {
			NDHttpListener *pL = dynamic_cast<NDHttpListener*>(NDGetListener(hListen));
			if (pL) {
				return pL->installRequest_script(args[2].GetText(), args[3].GetText());
			}
		}
	}

	parser->setErrno(LOGIC_ERR_AIM_OBJECT_NOT_FOUND);
	nd_logerror("install http script error\n");
	return false;
}

APOLLO_SCRIPT_API_DEF(apollo_http_error, "HTTP_发送错误(session, errorId, desc)")
{
	CHECK_ARGS_NUM(args, 3, parser);

	NDHttpSession *pSession = NULL;
	if (args[1].GetDataType() == OT_OBJ_NDHANDLE) {
		nd_handle h = (nd_handle)args[1].GetObjectAddr();
		if (!h) {
			parser->setErrno(NDERR_PARAM_TYPE_NOT_MATCH);
			return false;
		}
		pSession = dynamic_cast<NDHttpSession*>(NDGetSession(h));
	}
	else if (args[1].GetDataType() == OT_OBJ_NDOBJECT) {
		pSession = dynamic_cast<NDHttpSession*>((NDObject*)args[1].GetObjectAddr());
	}
	if (!pSession) {
		parser->setErrno(NDERR_BAD_GAME_OBJECT);
		return false;
	}
	const char *errorDesc = "unknown-error";
	if (args.size() > 3) {
		errorDesc = args[3].GetText();
	}
	pSession->sendErrorResponse(args[2].GetInt(), errorDesc);
	nd_logmsg("send http error response %d\n", args[2].GetInt());
	return true;
}

APOLLO_SCRIPT_API_DEF(apollo_http_respone, "HTTP_发送回复(session,status, header, body)")
{
	CHECK_ARGS_NUM_ONLY(args, 5, parser);
	CHECK_ARGS_NUM(args, 2, parser);

	NDHttpSession *pSession = NULL;
	if (args[1].GetDataType() == OT_OBJ_NDHANDLE) {
		nd_handle h = (nd_handle)args[1].GetObjectAddr();
		if (!h) {
			parser->setErrno(NDERR_PARAM_TYPE_NOT_MATCH);
			return false;
		}
		pSession = dynamic_cast<NDHttpSession*>(NDGetSession(h));
	}
	else if (args[1].GetDataType() == OT_OBJ_NDOBJECT) {
		pSession = dynamic_cast<NDHttpSession*>((NDObject*)args[1].GetObjectAddr());
	}
	if (!pSession) {
		parser->setErrno(NDERR_BAD_GAME_OBJECT);
		return false;
	}

	NDHttpResponse response;
	response.setStatus(args[2].GetInt());
	if (args[3].CheckValid()) {
		if (args[3].GetDataType() == OT_USER_DEFINED) {
			LogicUserDefStruct *pHeader =(LogicUserDefStruct *) args[3].getUserDef() ;
			if (pHeader) {
				for (size_t i = 0; i < pHeader->count(); i++) {
					const DBLDataNode *valNode = pHeader->ref(i);
					const char *pName = pHeader->getName((int)i);
					if (pName && valNode) {
						std::string valText = valNode->GetString();
						response.addHeader(pName, valText.c_str());
					}
				}
			}
		}
	}

	if (args[4].CheckValid()) {
		if (args[4].GetDataType() == OT_USER_DEFINED) {
			args[4].setOutAsJson(true);
		}
		args[4].toStdString(response.m_body);
	}

	pSession->SendResponse(response, "success");
	return true;
}


APOLLO_SCRIPT_API_DEF(apollo_http_get_header, "http_获得header(requestObj)")
{
	CHECK_ARGS_NUM(args, 2, parser);
	if (args[1].GetDataType() != OT_OBJ_NDOBJECT) {
		nd_logerror("argment 1 need OT_OBJ_NDOBJECT\n");
		parser->setErrno(NDERR_PARAM_TYPE_NOT_MATCH);
		return false;
	}

	NDHttpRequest *request = (NDHttpRequest *)args[1].GetObjectAddr();
	if (!request) {
		parser->setErrno(NDERR_BAD_GAME_OBJECT);
		return false;
	}

	LogicUserDefStruct headerJson;
	int count = request->getHeaderSize();
	for (int i = 0; i < count; i++) {
		const char *pName = request->getHeaderName(i);
		const char *pVal = request->getHeaderVal(i);
		if (pName && pVal) {
			headerJson.push_back(pName, pVal);
		}
	}

	result.InitSet(headerJson);
	return true;
}

APOLLO_SCRIPT_API_DEF(apollo_http_get_body, "http_获得请求body(requestObj)")
{
	CHECK_ARGS_NUM(args, 2, parser);
	if (args[1].GetDataType() != OT_OBJ_NDOBJECT) {
		nd_logerror("argment 1 need OT_OBJ_NDOBJECT\n");
		parser->setErrno(NDERR_PARAM_TYPE_NOT_MATCH);
		return false;
	}

	NDHttpRequest *request = (NDHttpRequest *)args[1].GetObjectAddr();
	if (!request) {
		parser->setErrno(NDERR_BAD_GAME_OBJECT);
		return false;
	}

	result.InitSet(request->getBody());
	return true;
}

APOLLO_SCRIPT_API_DEF(apollo_http_get_listener, "http_获得Listener(requestObj)")
{
	CHECK_ARGS_NUM(args, 2, parser);
	if (args[1].GetDataType() != OT_OBJ_NDOBJECT) {
		nd_logerror("argment 1 need OT_OBJ_NDOBJECT\n");
		parser->setErrno(NDERR_PARAM_TYPE_NOT_MATCH);
		return false;
	}

	NDHttpRequest *request = (NDHttpRequest *)args[1].GetObjectAddr();
	if (!request) {
		parser->setErrno(NDERR_BAD_GAME_OBJECT);
		return false;
	}

	result.InitSet(request->m_userData, OT_OBJ_NDOBJECT);
	return true;
}


APOLLO_SCRIPT_API_DEF(apollo_http_build_body, "http_生成body( body_text)")
{
	CHECK_ARGS_NUM(args, 2, parser);
	CHECK_DATA_TYPE(args[1], OT_STRING, parser);

	const char *pInput = args[1].GetText();
	if (!pInput) {
		parser->setErrno(NDERR_PARAM_INVALID);
		return false;
	}

	char sql_buf[0x10000];
	char *pOutaddr = sql_buf;
	size_t size = sizeof(sql_buf);
	//std::string 
	do {
		const char *pVarName = strchr(pInput, '$');
		if (pVarName) {
			//find var name 
			char myName[USER_DEF_VAR_NAME_SIZE];
			int len = ndstr_parse_variant_n(pVarName, myName, sizeof(myName));
			if (len == 0) {
				nd_logerror("parse %s error\n", pVarName);
				parser->setErrno(NDERR_PARAM_INVALID);
				return false;
			}
			int offset = 1;
			if (IS_NUMERALS(myName[1]) || 0 == ndstricmp(&myName[1], "value")) {
				offset = 0;
			}
			DBLDataNode var1;
			if (!parser->getVarValue(&myName[offset], var1)) {
				nd_logerror("can not found %s \n", pVarName);
				parser->setErrno(NDERR_PARAM_INVALID);
				return false;
			}
			if (!var1.CheckValid()) {
				nd_logerror("var %s is invalid\n", pVarName);
				parser->setErrno(NDERR_PARAM_INVALID);
				return false;
			}

			//copy before this var
			size_t datalen = pVarName - pInput;
			memcpy(pOutaddr, pInput, datalen);
			pOutaddr += datalen;
			size -= datalen;
			pInput = pVarName + len;

			datalen = var1.Sprint(pOutaddr, size);
			pOutaddr += datalen;
			size -= datalen;
		}
		else {
			strncpy(pOutaddr, pInput, size);
			break;
		}
	} while (pInput && size > 0);
	result.InitSet(sql_buf);

	return true;
}

int apollo_message_script_entry(void *engine, nd_handle  handle, nd_usermsgbuf_t *msg, const char *script)
{
	NDObject *pObj = NDObject::FromHandle(handle);
	if (!pObj) {
		return -1;
	}

	LogicParserEngine *scriptEngine = (LogicParserEngine *)pObj->getScriptEngine();
	if (!scriptEngine) {
		nd_logerror("can not get role script parser\n");
		return 0;
	}
	NDIStreamMsg inmsg(msg);
	//call function 
	//DBLDataNode data;
	parse_arg_list_t params;

	//function name 
	//data.InitSet(script);
	params.push_back(DBLDataNode(script));

	//receive message user
	//data.InitSet((void*)pSession, OT_OBJ_BASE_OBJ);
	params.push_back(DBLDataNode((void*)handle, OT_OBJ_NDHANDLE));

	//message object
	//data.InitSet((void*)&inmsg, OT_OBJ_MSGSTREAM);
	params.push_back(DBLDataNode((void*)&inmsg, OT_OBJ_MSGSTREAM));

	DBLDataNode result;
	scriptEngine->runScript(script, params, result);
	//bool ret = LogicEngineRoot::get_Instant()->runScript(script, scriptEngine, params, result);

	return 0;

}

///////////////////////////////////////////////

apoHttpScriptMgr::apoHttpScriptMgr(): m_logicEngine(this)
{
}

apoHttpScriptMgr::~apoHttpScriptMgr()
{
}


bool apoHttpScriptMgr::getOtherObject(const char*objName, DBLDataNode &val)
{
	ND_TRACE_FUNC();
	if (0 == ndstricmp(objName, "listener")) {
		nd_handle hListen = getbase_inst()->GetDeftListener()->GetHandle();
		val.InitSet((void*)hListen, OT_OBJ_NDHANDLE);
		return true;
	}

	else if (0 == ndstricmp(objName, "FormatMsgData")) {
		userDefineDataType_map_t &msgObj = LogicEngineRoot::get_Instant()->getGlobalDataType();
		val.InitSet((void*)&msgObj);
		return true;
	}
	else if (0 == ndstricmp(objName, "LogPath")) {
		NDInstanceBase *pInst = getbase_inst();
		server_config *pcfg = pInst->GetInstConfig();
		if (pcfg && pcfg->i_cfg.log_file[0]) {
			char buf[ND_FILE_PATH_SIZE];
			val.InitSet(nd_getpath(pcfg->i_cfg.log_file, buf, sizeof(buf)));
			return true;
		}
	}
	else if (0 == ndstricmp(objName, "LogFile")) {
		NDInstanceBase *pInst = getbase_inst();
		server_config *pcfg = pInst->GetInstConfig();
		if (pcfg && pcfg->i_cfg.log_file[0]) {
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
		char buf[512];
		nd_common_machine_info(buf, sizeof(buf));
		val.InitSet(buf);
		return true;
	}
	else if (0 == ndstricmp(objName, "SelfName")) {
		val.InitSet("httpServer");
		return true;
	}
	else {
		return _myBase::getOtherObject(objName, val);
	}
	return false;
	
}

bool apoHttpScriptMgr::RunScript(parse_arg_list_t &args, DBLDataNode &result)
{
	ND_TRACE_FUNC();
	if (args.size() < 1 || args[0].GetDataType() != OT_STRING) {
		return false;
	}
	const char *script = args[0].GetText();

	bool ret = m_logicEngine.runScript(script, args, result);
	if (!ret) {
		nd_logerror("run %s error %d\n", script, m_logicEngine.getErrno());
	}
	return ret;
}

bool apoHttpScriptMgr::RunScript(const char *script)
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
bool apoHttpScriptMgr::SendScriptEvent(int event_id, int argc, ...)
{
	ND_TRACE_FUNC();

	parse_arg_list_t params;

	params.push_back(DBLDataNode((void*)this, OT_OBJ_BASE_OBJ));

	va_list arg;
	va_start(arg, argc);
	while (argc-- > 0) {
		DBLDataNode *data1 = va_arg(arg, DBLDataNode*);
		params.push_back(*data1);
	}
	va_end(arg);

	return m_logicEngine.eventNtf(event_id, params);
}

bool apoHttpScriptMgr::SendEvent0(int event)
{
	ND_TRACE_FUNC();
	return SendScriptEvent(event, 0);
}
bool apoHttpScriptMgr::SendEvent1(int event, const DBLDataNode &val1)
{
	ND_TRACE_FUNC();
	return SendScriptEvent(event, 1, &val1);
}

//////////////////////////////////////////////////////////////////////////

bool apoHttpScriptMgr::opRead(const DBLDataNode& id, DBLDataNode &val)
{
	PARSE_TRACE("logic_engine_test: opRead(%d) \n", id.GetInt());
	//_setval(val);
	return true;
}

bool apoHttpScriptMgr::opWrite(const DBLDataNode& id, const DBLDataNode &val)
{
	PARSE_TRACE("logic_engine_test: opWrite(%d) \n", id.GetInt());
	//_setval(val);
	return true;
}


bool apoHttpScriptMgr::opAdd(const DBLDataNode& id, const DBLDataNode &val)
{
	PARSE_TRACE("logic_engine_test: opAdd(%d) \n", id.GetInt());
	//_setval(val);
	return true;
}


bool apoHttpScriptMgr::opSub(const DBLDataNode& id, const  DBLDataNode &val)
{
	PARSE_TRACE("logic_engine_test: opSub(%d) \n", id.GetInt());
	//_setval(val);
	return true;
}

LogicObjectBase *apoHttpScriptMgr::getObjectMgr(const char* destName)
{
	if (0 == ndstricmp(destName, "owner")) {
		return  this;
	}
	return NULL;
}


///////////////////////

apoHttpListener::apoHttpListener(nd_fectory_base *sf) : _myBase(sf)
{

}
apoHttpListener::~apoHttpListener()
{

}

void *apoHttpListener::getScriptEngine()
{
	return (void*)getHttpScriptObj();
}
int apoHttpListener::onRequestScript(const char* script, NDHttpSession *session, const NDHttpRequest &request)
{
	LogicUserDefStruct formJson;

	NDHttpRequest::HttpHeader_t::const_iterator it = request.m_requestForms.begin();
	for (; it != request.m_requestForms.end(); ++it) {
		formJson.push_back(it->name.c_str(), it->value.c_str());
	}

	parse_arg_list_t args;
	args.push_back(DBLDataNode(script));
	args.push_back(DBLDataNode((void*)session, OT_OBJ_NDOBJECT));
	args.push_back(DBLDataNode(formJson));
	args.push_back(DBLDataNode((void*)&request, OT_OBJ_NDOBJECT));

	DBLDataNode result;
	if (!getHttpScriptObj()->RunScript(args, result)) {
		nd_logerror("run http script %s error\n", script);
		return -1;
	}
	return 0;
}
