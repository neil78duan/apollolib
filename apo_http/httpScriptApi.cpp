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
#include "game_parser/dbldata2netstream.h"
#include "apo_http/httpScriptApi.h"


apoHttpScriptMgr::apoHttpScriptMgr():m_logicEngine(this)
{

}
apoHttpScriptMgr::~apoHttpScriptMgr()
{

}

bool apoHttpScriptMgr::RunScript(parse_arg_list_t &args, LogicDataObj &result)
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
	LogicDataObj val, result;
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

	params.push_back(LogicDataObj((void*)this, OT_OBJ_BASE_OBJ));

	va_list arg;
	va_start(arg, argc);
	while (argc-- > 0) {
		LogicDataObj *data1 = va_arg(arg, LogicDataObj*);
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
bool apoHttpScriptMgr::SendEvent1(int event, const LogicDataObj &val1)
{
	ND_TRACE_FUNC();
	return SendScriptEvent(event, 1, &val1);
}

//////////////////////////////////////////////////////////////////////////

bool apoHttpScriptMgr::opRead(const LogicDataObj& id, LogicDataObj &val)
{
	PARSE_TRACE("logic_engine_test: opRead(%d) \n", id.GetInt());
	//_setval(val);
	return true;
}

bool apoHttpScriptMgr::opWrite(const LogicDataObj& id, const LogicDataObj &val)
{
	PARSE_TRACE("logic_engine_test: opWrite(%d) \n", id.GetInt());
	//_setval(val);
	return true;
}


bool apoHttpScriptMgr::opAdd(const LogicDataObj& id, const LogicDataObj &val)
{
	PARSE_TRACE("logic_engine_test: opAdd(%d) \n", id.GetInt());
	//_setval(val);
	return true;
}


bool apoHttpScriptMgr::opSub(const LogicDataObj& id, const  LogicDataObj &val)
{
	PARSE_TRACE("logic_engine_test: opSub(%d) \n", id.GetInt());
	//_setval(val);
	return true;
}

LogicObjectBase *apoHttpScriptMgr::getObjectMgr(const char* destName)
{
	ND_TRACE_FUNC();
	if (0 == ndstricmp(destName, "owner")) {
		return  this;
	}
	return NULL;
}

bool apoHttpScriptMgr::getOtherObject(const char*objName, LogicDataObj &val)
{
	ND_TRACE_FUNC();
	if (0 == ndstricmp(objName, "machineInfo")) {
		char buf[256];
		val.InitSet(nd_common_machine_info(buf, sizeof(buf)));
	}
	else if (0 == ndstricmp(objName, "self")) {
		val.InitSet((void*)this, OT_OBJ_BASE_OBJ);
	}
	
	if (0 == ndstricmp(objName, "listener")) {
		nd_handle hListen = getbase_inst()->GetDeftListener()->GetHandle();
		val.InitSet((void*)hListen, OT_OBJ_NDHANDLE);
		return true;
	}
	m_error = LOGIC_ERR_AIM_OBJECT_NOT_FOUND;
	return false;
}


//////////////////////////////////////////
static  NDHttpSession *_getSession(const LogicDataObj &objData)
{
	ND_TRACE_FUNC();
	NDHttpSession *pSession = NULL;
	if (objData == OT_OBJ_NDHANDLE) {
		pSession = dynamic_cast< NDHttpSession*>(NDObject::FromHandle(objData.GetNDHandle()));
	}
	else if (objData.GetDataType() == OT_OBJ_NDOBJECT) {
		pSession = dynamic_cast< NDHttpSession*>(objData.GetNDObj());
	}
	return pSession;
}

APOLLO_SCRIPT_API_DEF(apollo_set_http_handler, "http_install_req_handler(listenerName, reqPath,scriptName)")
{
	ND_TRACE_FUNC();
	CHECK_ARGS_NUM(args, 4, parser);

	LogicDataObj netHandle;
	LogicObjectBase *owner = (LogicObjectBase *)parser->getOwner();

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

APOLLO_SCRIPT_API_DEF(apollo_http_save_session, "HTTP_set_session_val(connectObj, session_name, value)")
{
	ND_TRACE_FUNC();
	CHECK_ARGS_NUM(args, 4, parser);

	NDHttpSession *pSession = _getSession(args[1]);
	if (!pSession) {
		parser->setErrno(NDERR_BAD_GAME_OBJECT);
		nd_logerror("get session error \n");
		return false;
	}

	return pSession->sessionIdSetValue(args[2].GetText(), args[3].GetText());

}

APOLLO_SCRIPT_API_DEF(apollo_http_get_session, "HTTP_get_session_val(connectObj, name)")
{
	ND_TRACE_FUNC();
	CHECK_ARGS_NUM(args, 3, parser);

	NDHttpSession *pSession = _getSession(args[1]);
	if (!pSession) {
		parser->setErrno(NDERR_BAD_GAME_OBJECT);
		nd_logerror("get session error \n");
		return false;
	}

	result = pSession->sessionIdGetValue(args[2].GetText());
	
	return true;
}

APOLLO_SCRIPT_API_DEF(apollo_http_session_id, "HTTP_get_sessionId(connectObj)")
{
	ND_TRACE_FUNC();
	
	NDHttpSession *pSession = _getSession(args[1]);
	if (!pSession) {
		parser->setErrno(NDERR_BAD_GAME_OBJECT);
		nd_logerror("get session error \n");
		return false;
	}
	result = pSession->sessionIdGet();
	return true;
}


APOLLO_SCRIPT_API_DEF(apollo_http_error, "HTTP_error_response(connectObj, errorId, desc)")
{
	ND_TRACE_FUNC();
	CHECK_ARGS_NUM(args, 3, parser);

	NDHttpSession *pSession = _getSession(args[1]);
	if (!pSession) {
		parser->setErrno(NDERR_BAD_GAME_OBJECT);
		nd_logerror("get session error \n");
		return false;
	}
	int outEncode = E_SRC_CODE_UTF_8;
	apoHttpListener*pListen = dynamic_cast<apoHttpListener*>( pSession->GetParent());
	if (pListen) {
		outEncode = pListen->getEncodeType();
	}

	std::string errorDesc = "unknown-error"; 
	if (args.size() > 3) {
		args[3].ConvertEncode(ND_ENCODE_TYPE,outEncode);
		errorDesc = args[3].GetText();
	}
	pSession->sendErrorResponse(args[2].GetInt(), errorDesc.c_str());
	nd_logmsg("send http error response %d\n", args[2].GetInt());
	return true;
}

APOLLO_SCRIPT_API_DEF(apollo_http_respone, "HTTP_response(connectObj,status, header, body)")
{
	ND_TRACE_FUNC();
	CHECK_ARGS_NUM_ONLY(args, 5, parser);
	CHECK_ARGS_NUM(args, 2, parser);

	NDHttpSession *pSession = _getSession(args[1]);
	if (!pSession) {
		parser->setErrno(NDERR_BAD_GAME_OBJECT);
		nd_logerror("get session error \n");
		return false;
	}

	NDHttpResponse response;
	response.setStatus(args[2].GetInt());
	if (args[3].CheckValid()) {
		if (args[3].GetDataType() == OT_USER_DEFINED) {
			LogicUserDefStruct *pHeader =(LogicUserDefStruct *) args[3].getUserDef() ;
			if (pHeader) {
				for (size_t i = 0; i < pHeader->count(); i++) {
					const LogicDataObj *valNode = pHeader->ref(i);
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
		int outEncode = E_SRC_CODE_UTF_8;
		apoHttpListener*pListen = dynamic_cast<apoHttpListener*>(pSession->GetParent());
		if (pListen) {
			outEncode = pListen->getEncodeType();
		}
		args[4].ConvertEncode(ND_ENCODE_TYPE, outEncode);

		if (args[4].GetDataType() == OT_USER_DEFINED) {
			args[4].setOutAsJson(true);
		}
		std::string bodyString;
		if (args[4].toStdString(bodyString) > 0) {
			response.setBody(bodyString.c_str());
		}
	}

	pSession->SendResponse(response, "success");
	return true;
}


APOLLO_SCRIPT_API_DEF(apollo_http_redirect, "HTTP_redirect(connectObj, newURL)")
{
	ND_TRACE_FUNC();
	CHECK_ARGS_NUM(args, 3, parser);

	NDHttpSession *pSession = _getSession(args[1]);
	if (!pSession) {
		parser->setErrno(NDERR_BAD_GAME_OBJECT);
		nd_logerror("get session error \n");
		return false;
	}
	return -1==pSession->SendRedirect(args[2].GetText()) ? false : true;
}


APOLLO_SCRIPT_API_DEF(apollo_http_file_down, "HTTP_download_file(connectObj,request,filepath)")
{
	ND_TRACE_FUNC();
	CHECK_ARGS_NUM(args, 4, parser);
	
	NDHttpSession *pSession = _getSession(args[1]);
	if (!pSession) {
		parser->setErrno(NDERR_BAD_GAME_OBJECT);
		nd_logerror("get session error \n");
		return false;
	}

	const NDHttpRequest *request = dynamic_cast<const NDHttpRequest*>(args[2].GetNDObj());
	if (!request) {
		parser->setErrno(NDERR_BAD_GAME_OBJECT);
		nd_logerror("get request error \n");
		return false;
	}


	LogicObjectBase *owner = (LogicObjectBase *)parser->getOwner();
	nd_assert(owner);
	LogicDataObj val;
	if (!owner->getOtherObject("listener", val)) {
		parser->setErrno(NDERR_BAD_GAME_OBJECT);
		return false;
	}
	apoHttpListener *pListen = dynamic_cast<apoHttpListener*>(NDObject::FromHandle((nd_handle)val.GetObjectAddr()));
	if (!pListen) {
		parser->setErrno(NDERR_SYSTEM);
		return false;
	}
	return pListen->downloadFile(args[3].GetText(), pSession, *request);
}


APOLLO_SCRIPT_API_DEF(apollo_get_upload_file, "HTTP_get_upload_file(request,varName)")
{
	ND_TRACE_FUNC();
	CHECK_ARGS_NUM(args, 3, parser);

	const NDHttpRequest *request = dynamic_cast<const NDHttpRequest*>(args[1].GetNDObj());
	if (!request) {
		parser->setErrno(NDERR_BAD_GAME_OBJECT);
		nd_logerror("get request error \n");
		return false;
	}
	const NDHttpRequest::fileCacheInfo *fileInfo = request->getUploadFile(args[2].GetText());
	if (!fileInfo) {
		parser->setErrno(NDERR_FILE_NOT_EXIST);
		nd_logerror("%s file not upload from client \n", args[2].GetText());
		return false;
	}
	result.InitSet((void*)fileInfo->dataAddr, fileInfo->size);
	return true;
}

APOLLO_SCRIPT_API_DEF(apollo_http_set_session_age, "HTTP_set_session_age(connectObj, ageOfSeconds)")
{
	ND_TRACE_FUNC();
	CHECK_ARGS_NUM(args, 3, parser);

	NDHttpSession *pSession = _getSession(args[1]);
	if (!pSession) {
		parser->setErrno(NDERR_BAD_GAME_OBJECT);
		nd_logerror("get session error \n");
		return false;
	}
	pSession->setSessionAge(args[2].GetInt());
	return true;
}


APOLLO_SCRIPT_API_DEF(apollo_http_set_root_session_age, "HTTP_set_root_session_age( ageOfSeconds)")
{
	ND_TRACE_FUNC();
	CHECK_ARGS_NUM(args, 2, parser);

	LogicObjectBase *owner = (LogicObjectBase *)parser->getOwner();
	nd_assert(owner);
	LogicDataObj val;
	if (!owner->getOtherObject("listener", val)) {
		parser->setErrno(NDERR_BAD_GAME_OBJECT);
		return false;
	}
	apoHttpListener *pListen = dynamic_cast<apoHttpListener*>(NDObject::FromHandle((nd_handle)val.GetObjectAddr()));
	if (!pListen) {
		parser->setErrno(NDERR_SYSTEM);
		return false;
	}
	pListen->setSessionAge(args[1].GetInt());
	return true;
}


APOLLO_SCRIPT_API_DEF(apollo_http_cache_file, "HTTP_cache_to_mem(filepath)")
{
	ND_TRACE_FUNC();
	CHECK_ARGS_NUM(args, 2, parser);
	if (!args[1].GetText()) {
		parser->setErrno(NDERR_INVALID_INPUT);
		return false;
	}
	LogicObjectBase *owner = (LogicObjectBase *)parser->getOwner();
	nd_assert(owner);
	LogicDataObj val;
	if (!owner->getOtherObject("listener", val)) {
		parser->setErrno(NDERR_BAD_GAME_OBJECT);
		return false;
	}
	apoHttpListener *pListen = dynamic_cast<apoHttpListener*>(NDObject::FromHandle((nd_handle)val.GetObjectAddr()));
	if (!pListen) {
		parser->setErrno(NDERR_SYSTEM);
		return false;
	}
	
	return pListen->cacheFile(args[1].GetText());
}

APOLLO_SCRIPT_API_DEF(apollo_http_uncache_file, "HTTP_uncache_from_mem(filepath)")
{
	ND_TRACE_FUNC();
	CHECK_ARGS_NUM(args, 2, parser);
	if (!args[1].GetText()) {
		parser->setErrno(NDERR_INVALID_INPUT);
		return false;
	}
	LogicObjectBase *owner = (LogicObjectBase *)parser->getOwner();
	nd_assert(owner);
	LogicDataObj val;
	if (!owner->getOtherObject("listener", val)) {
		parser->setErrno(NDERR_BAD_GAME_OBJECT);
		return false;
	}
	apoHttpListener *pListen = dynamic_cast<apoHttpListener*>(NDObject::FromHandle((nd_handle)val.GetObjectAddr()));
	if (!pListen) {
		parser->setErrno(NDERR_SYSTEM);
		return false;
	}

	return pListen->uncacheFile(args[1].GetText());
}

APOLLO_SCRIPT_API_DEF(apollo_http_set_working_path, "HTTP_working_path(readPath, writePath)")
{
	ND_TRACE_FUNC();
	CHECK_ARGS_NUM(args, 3, parser);
	
	LogicObjectBase *owner = (LogicObjectBase *)parser->getOwner();
	nd_assert(owner);
	LogicDataObj val;
	if (!owner->getOtherObject("listener", val)) {
		parser->setErrno(NDERR_BAD_GAME_OBJECT);
		return false;
	}
	apoHttpListener *pListen = dynamic_cast<apoHttpListener*>(NDObject::FromHandle((nd_handle)val.GetObjectAddr()));
	if (!pListen) {
		parser->setErrno(NDERR_SYSTEM);
		return false;
	}

	pListen->setPath(args[1].GetText(), args[2].GetText());
	return true;
}

APOLLO_SCRIPT_API_DEF(apollo_http_get_header, "http_get_header(requestObj)")
{
	ND_TRACE_FUNC();
	CHECK_ARGS_NUM(args, 2, parser);
	
	NDHttpRequest *request = dynamic_cast<NDHttpRequest*>(args[1].GetNDObj());
	if (!request) {
		parser->setErrno(NDERR_BAD_GAME_OBJECT);
		nd_logerror("argment 1 need OT_OBJ_NDOBJECT\n");
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

APOLLO_SCRIPT_API_DEF(apollo_http_get_body, "http_get_body(requestObj)")
{
	ND_TRACE_FUNC();
	CHECK_ARGS_NUM(args, 2, parser);
	
	NDHttpRequest *request = dynamic_cast<NDHttpRequest*>(args[1].GetNDObj());
	if (!request) {
		parser->setErrno(NDERR_BAD_GAME_OBJECT);
		nd_logerror("argment 1 need OT_OBJ_NDOBJECT\n");
		return false;
	}

	result.InitSet(request->getBody());
	return true;
}

APOLLO_SCRIPT_API_DEF(apollo_http_get_listener, "http_get_Listener(requestObj)")
{
	ND_TRACE_FUNC();
	CHECK_ARGS_NUM(args, 2, parser);
	
	NDHttpRequest *request = dynamic_cast<NDHttpRequest*>(args[1].GetNDObj());
	if (!request) {
		parser->setErrno(NDERR_BAD_GAME_OBJECT);
		nd_logerror("argment 1 need OT_OBJ_NDOBJECT\n");
		return false;
	}

	result.InitSet(request->m_userData, OT_OBJ_NDOBJECT);
	return true;
}


APOLLO_SCRIPT_API_DEF(apollo_http_build_body, "http_build_body( body_text)")
{
	ND_TRACE_FUNC();
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
			int len = 0;
			char myName[USER_DEF_VAR_NAME_SIZE];
			if (pVarName[1] == '(') {
				const char *pReadEnd = ndstr_nstr_end(pVarName +1 , myName, ')',sizeof(myName));
				if (!pReadEnd || !*pReadEnd) {
					nd_logerror("parse %s error\n", pVarName);
					parser->setErrno(NDERR_PARAM_INVALID);
					return false;
				}
				len = (int)(pReadEnd + 1 - pVarName);
			}
			else {
				len = ndstr_parse_variant_n(pVarName, myName, sizeof(myName));
				if (len == 0) {
					nd_logerror("parse %s error\n", pVarName);
					parser->setErrno(NDERR_PARAM_INVALID);
					return false;
				}
			}
			int offset = 1;
			if (IS_NUMERALS(myName[1]) || 0 == ndstricmp(&myName[1], "value")) {
				offset = 0;
			}
			LogicDataObj var1;
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


APOLLO_SCRIPT_API_DEF(apollo_http_get, "httpShort_get(host,port, path, handler,userData)")
{
	ND_TRACE_FUNC();
	CHECK_ARGS_NUM(args, 5, parser);

	ApoHttpClientShort *pConn = new ApoHttpClientShort;
	
	NDHttpRequest req;
	req.setAction(NDHttpParser::E_ACTION_GET);

	if (args.size() >= 6 && args[5].CheckValid()) {
		pConn->setUserData(args[5]);
	}

	if (-1 == pConn->Request(req, args[1].GetText(), args[2].GetInt(), args[3].GetText(), args[4].GetText())) {
		delete pConn;
		parser->setErrno(NDERR_HOST_UNAVAILABLE);
		return false;
	}

	return true;
}

APOLLO_SCRIPT_API_DEF(apollo_http_post, "httpShort_post(host,port, path,body,handler,userData)")
{
	ND_TRACE_FUNC();
	CHECK_ARGS_NUM(args,6, parser);

	ApoHttpClientShort *pConn = new ApoHttpClientShort;

	NDHttpRequest req;
	req.setAction(NDHttpParser::E_ACTION_POST);

	if (args[4].GetDataType() == OT_STRING) {
		req.setBody(args[4].GetText());
	}
	else if (args[4].GetDataType() == OT_USER_DEFINED) {
		LogicUserDefStruct *pJson = (LogicUserDefStruct *) args[4].getUserDef();
		if (pJson) {
			for (int i = 0; i < pJson->count(); i++) {
				LogicDataObj *pData = pJson->ref(i);
				const char *name = pJson->getName(i);
				if (pData->GetText()) {
					req.addRequestFormVal(name, pData->GetText());
				}
			}
		}
	}

	if (args.size() >= 7 && args[6].CheckValid()) {
		pConn->setUserData(args[6]);
	}

	if (-1 == pConn->Request(req, args[1].GetText(), args[2].GetInt(), args[3].GetText(), args[5].GetText())) {
		delete pConn;
		parser->setErrno(NDERR_HOST_UNAVAILABLE);
		return false;
	}

	return true;
}


APOLLO_SCRIPT_API_DEF(apollo_convert_sysErr_to_http, "http_get_response_error(systemError)")
{
	ND_TRACE_FUNC();
	CHECK_ARGS_NUM(args, 2, parser);

	int inputError = args[1].GetInt();
	int outError = 200;
	if (NDERR_NO_PRIVILAGE == inputError) {
		outError = 401;

	}
	else if (NDERR_SKIP == inputError) {
		outError = 204;
	}
	else if (inputError != 0) {
		outError = 500;
	}

	return true;
}

////////////////////////////////////


static void _deattach_from_listen(nd_handle handle, void *param)
{
	ND_TRACE_FUNC();
	NDInstanceBase *pBase = getbase_inst();
	if (pBase && pBase->GetDeftListener()) {
		nd_listensrv_deattach(pBase->GetDeftListener()->GetHandle(), handle, 0);
	}
	ApoHttpClientShort *pobj = dynamic_cast<ApoHttpClientShort *>( NDObject::FromHandle(handle));
	if (pobj) {
		delete pobj;
	}
}

ApoHttpClientShort::ApoHttpClientShort()
{
	nd_logmsg("create http connector \n");
}
ApoHttpClientShort::~ApoHttpClientShort()
{
	nd_logmsg("Http connector destroyed\n");

}

int ApoHttpClientShort::Request(NDHttpRequest &request, const char *host, int port, const char *path, const char* repHandler)
{
	ND_TRACE_FUNC();
	m_handler = repHandler;

	if (-1 == Create(NULL)) {
		nd_logerror("create http connector error\n");
		return -1;
	}


	if (-1 == Open(host, port)) {
		nd_logerror("connector %s : %d\n", host, port);
		return -1;
	}

	nd_connector_add_close_callback(GetHandle(), _deattach_from_listen, NULL);
	
	nd_connector_set_timeout((nd_netui_handle)GetHandle(), 10);

	NDInstanceBase *pBase = getbase_inst();
	if (pBase && pBase->GetDeftListener()) {
		pBase->GetDeftListener()->Attach(*this);
	}

	return SendRequest(request, host, port, path);
}

void ApoHttpClientShort::onResponse(NDHttpResponse *response)
{
	ND_TRACE_FUNC();

	parse_arg_list_t args;
	args.push_back(LogicDataObj(m_handler.c_str()));
	args.push_back(LogicDataObj(this));
	args.push_back(LogicDataObj(response->getBody()));
	args.push_back(LogicDataObj(response));
	args.push_back(m_userData);

	LogicDataObj result;
	if (!getHttpScriptObj()->RunScript(args, result)) {
		nd_logerror("run http script %s error\n", m_handler.c_str());
	}
	setResponseSuccess();
}

void ApoHttpClientShort::OnClose()
{
	//delete this;
}


///////////////////////

apoHttpListener::apoHttpListener(nd_fectory_base *sf) : _myBase(sf), m_encodeType(E_SRC_CODE_UTF_8)
{

}
apoHttpListener::~apoHttpListener()
{

}

void apoHttpListener::Destroy(int flag)
{
	ND_TRACE_FUNC();
	destroyCache();
	NDHttpListener::Destroy(flag);

}

void apoHttpListener::SetHttpEncode(const char *encodeName)
{
	m_encodeType = nd_get_encode_val(encodeName);
}
int apoHttpListener::getEncodeType()
{
	return m_encodeType;
}

void apoHttpListener::setPath(const char *readable, const char *writable)
{

	char tmp_path[ND_FILE_PATH_SIZE];
	if (readable && *readable) {
		m_readablePath = nd_absolute_path(readable, tmp_path, sizeof(tmp_path));
	}
	if (writable && *writable) {
		m_writablePath = nd_absolute_path(writable, tmp_path, sizeof(tmp_path));
	}
}

void *apoHttpListener::getScriptEngine()
{
	return (void*)getHttpScriptObj();
}
int apoHttpListener::onRequestScript(const char* script, NDHttpSession *session, const NDHttpRequest &request)
{
	ND_TRACE_FUNC();
	LogicUserDefStruct formJson;

	NDHttpRequest::HttpHeader_t::const_iterator it = request.m_requestForms.begin();
	for (; it != request.m_requestForms.end(); ++it) {
		formJson.push_back(it->name.c_str(), it->value.c_str());
	}

	parse_arg_list_t args;
	args.push_back(LogicDataObj(script));
	args.push_back(LogicDataObj(session));
	args.push_back(LogicDataObj(formJson));
	args.push_back(LogicDataObj((NDIBaseObj*)&request));

	LogicDataObj result;
	if (!getHttpScriptObj()->RunScript(args, result)) {
		nd_logerror("run http script %s error\n", script);
		return -1;
	}
	return 0;
}

void apoHttpListener::destroyCache()
{
	ND_TRACE_FUNC();
	for (fileCache_map_t::iterator it = m_fileCache.begin(); it != m_fileCache.end(); ++it) {
		if (it->second.dataAddr) {
			nd_unload_file(it->second.dataAddr);
			it->second.dataAddr;
		}
	}
	m_fileCache.clear();
}

bool apoHttpListener::cacheFile(const char *filePath)
{
	ND_TRACE_FUNC();
	size_t size = 0;
	fileCacheInfo cacheInfo;
	cacheInfo.dataAddr = (char*) nd_load_file(filePath, &size); 
	if (!cacheInfo.dataAddr) {
		return false;
	}
	MD5Crypt32(cacheInfo.dataAddr,(int)size, cacheInfo.md5);
	cacheInfo.size = size;

	m_fileCache[filePath] = cacheInfo;
	return true;
}

bool apoHttpListener::uncacheFile(const char*filePath)
{
	ND_TRACE_FUNC();
	fileCache_map_t::iterator it = m_fileCache.find(filePath);
	if (it != m_fileCache.end()) {
		nd_unload_file(it->second.dataAddr);
		m_fileCache.erase(it);
	}
	return true;
}

bool apoHttpListener::downloadFile(const char *filePath, NDHttpSession *session, const NDHttpRequest &request)
{
	ND_TRACE_FUNC();
	NDINT64 offset = 0;
	NDINT64 size = 0;
	size_t totalSize = 0;

	//check file path is valid
	if (strstr(filePath,"..") ){
		nd_logerror("request file is invalid : %s\n", filePath);
		return false; 
	}
	char tmp_path[ND_FILE_PATH_SIZE];

	filePath = nd_full_path(m_readablePath.c_str(),filePath, tmp_path, sizeof(tmp_path));
	if (!filePath) {
		nd_logerror("Can not acccess %s\n", filePath);
		return false;
	}

	const char *pRange = request.getHeader("Range");
	if (pRange ) {
		pRange = ndstristr(pRange, "bytes=");
		if (pRange) {
			pRange += 6;
			if (!getFileOffset(pRange, size, offset)) {
				return false;
			}
		}
	}

	char *pData = (char*) loadFile( filePath,  offset, size,totalSize);
	if (!pData) {
		return false;
	}
	//send data from session
	NDHttpResponse response;

	if (pRange) {
		char tmp[128];
		snprintf(tmp, sizeof(tmp), "bytes %lld-%lld/%lld", offset, (offset + size), totalSize);
		response.addHeader("Content-Range", tmp);
		response.setStatus(206);
		response.addHeader("Connection", "Keep-Alive");
	}
	else {
		response.setStatus(200);
		response.addHeader("Connection", "Closed");
	}

	response.addHeader("Accept-Ranges", "bytes");
	response.addHeader("Content-Type", apoHttpListener::getFileType(filePath));

	//get file name 
	std::string fname = NDHttpParser::textToURLcode(nd_filename(filePath));
	if (fname.size()) {
		snprintf(tmp_path, sizeof(tmp_path), "attachment;filename=%s", fname.c_str());
		response.addHeader("Content-Disposition", tmp_path);
	}
	
	session->sendBinaryData(response,pData,size, response.getStatus() == 206 ? "Partial Content" : "OK");
	
	releaseFile(pData);
	return true;
}

bool apoHttpListener::getFileOffset(const char*pRange, NDINT64 &size, NDINT64 &offset)
{
	ND_TRACE_FUNC();
	char *p = (char*)ndstr_first_valid(pRange);
	if (!p) {
		return false;
	}
	offset = strtoll(p, &p, 0);
	if (offset < 0) {
		size = 0;
	}
	else {
		p = (char*)ndstr_first_valid(p);
		if (*p != '-') {
			return false;
		}
		++p;
		if (!*p) {
			size = 0;
		}
		else {
			NDINT64 endPos = strtoll(p, &p, 0);
			if (endPos < offset) {
				return false;
			}
			size = endPos - offset + 1;
		}
	}
	return true;


}

void apoHttpListener::releaseFile(void *fileData)
{
	ND_TRACE_FUNC();
	for (fileCache_map_t::iterator it = m_fileCache.begin(); it != m_fileCache.end(); ++it) {
		if (fileData >= it->second.dataAddr && fileData< it->second.dataAddr + it->second.size) {
			return;
		}
	}
	free(fileData);
}
void *apoHttpListener::loadFile(const char *filePath, NDINT64 offset, NDINT64 &size, size_t &fileSize)
{
	ND_TRACE_FUNC();
	fileCache_map_t::iterator it = m_fileCache.find(filePath);
	if (it == m_fileCache.end()) {
		return _loadFile(filePath, offset, size, fileSize);
	}

	const char *pRet = NULL;
	fileCacheInfo &fileInfo = it->second;

	fileSize = fileInfo.size;
	if (offset < 0) {
		size = -offset;
		if (size > fileSize)
			return NULL;
		return fileInfo.dataAddr + fileInfo.size  + offset;
	}
	else if (size == 0) {
		size = fileSize;
		return fileInfo.dataAddr;
	}
	else {
		if (offset >= fileSize) {
			return NULL;
		}
		NDINT64 dataSize = fileSize - offset;
		if (size > dataSize)
			size = dataSize;

		return fileInfo.dataAddr +  offset;
	}

}
void *apoHttpListener::_loadFile(const char *filePath, NDINT64 offset, NDINT64 &size, size_t &fileSize)
{
	ND_TRACE_FUNC();
	FILE *fp = fopen(filePath, "rb");
	if (!fp) {
		return NULL;
	}
	//get total size 
	fseek(fp, 0, SEEK_END);
	fileSize = ftell(fp);
	fseek(fp, 0, SEEK_SET);


	if (offset < 0) {
		fseek(fp, offset, SEEK_END);
		size = -offset;
	}
	else if (size == 0) {
		size = fileSize;
	}
	else {
		fseek(fp, offset, SEEK_SET);
	}
	char *pData = (char*)malloc(size);
	nd_assert(pData);

	size_t readlen = fread(pData,1,size,fp);
	fclose(fp);
	size = readlen;
	return pData;
}

////////////////////////////////////////////
struct fileInfo {
	const char *fileExt;
	const char *httpHeader;
};

static fileInfo _s_fileInfo[] = {
	{ "3gp"   , "video/3gpp" },
	{ "a" , "application/octet-stream" },
	{ "ai" , "application/postscript" },
	{ "aif" , "audio/x-aiff" },
	{ "aiff" , "audio/x-aiff" },
	{ "asc" , "application/pgp-signature" },
	{ "asf" , "video/x-ms-asf" },
	{ "asm" , "text/x-asm" },
	{ "asx" , "video/x-ms-asf" },
	{ "atom" , "application/atom+xml" },
	{ "au" , "audio/basic" },
	{ "avi" , "video/x-msvideo" },
	{ "bat" , "application/x-msdownload" },
	{ "bin" , "application/octet-stream" },
	{ "bmp" , "image/bmp" },
	{ "bz2" , "application/x-bzip2" },
	{ "c" , "text/x-c" },
	{ "cab" , "application/vnd.ms-cab-compressed" },
	{ "cc" , "text/x-c" },
	{ "chm" , "application/vnd.ms-htmlhelp" },
	{ "class" , "application/octet-stream" },
	{ "com" , "application/x-msdownload" },
	{ "conf" , "text/plain" },
	{ "cpp" , "text/x-c" },
	{ "crt" , "application/x-x509-ca-cert" },
	{ "css" , "text/css" },
	{ "csv" , "text/csv" },
	{ "cxx" , "text/x-c" },
	{ "deb" , "application/x-debian-package" },
	{ "der" , "application/x-x509-ca-cert" },
	{ "diff" , "text/x-diff" },
	{ "djv" , "image/vnd.djvu" },
	{ "djvu" , "image/vnd.djvu" },
	{ "dll" , "application/x-msdownload" },
	{ "dmg" , "application/octet-stream" },
	{ "doc" , "application/msword" },
	{ "dot" , "application/msword" },
	{ "dtd" , "application/xml-dtd" },
	{ "dvi" , "application/x-dvi" },
	{ "ear" , "application/java-archive" },
	{ "eml" , "message/rfc822" },
	{ "eps" , "application/postscript" },
	{ "exe" , "application/x-msdownload" },
	{ "f" , "text/x-fortran" },
	{ "f77" , "text/x-fortran" },
	{ "f90" , "text/x-fortran" },
	{ "flv" , "video/x-flv" },
	{ "for" , "text/x-fortran" },
	{ "gem" , "application/octet-stream" },
	{ "gemspec" , "text/x-script.ruby" },
	{ "gif" , "image/gif" },
	{ "gz" , "application/x-gzip" },
	{ "h" , "text/x-c" },
	{ "hh" , "text/x-c" },
	{ "htm" , "text/html" },
	{ "html" , "text/html" },
	{ "ico" , "image/vnd.microsoft.icon" },
	{ "ics" , "text/calendar" },
	{ "ifb" , "text/calendar" },
	{ "iso" , "application/octet-stream" },
	{ "jar" , "application/java-archive" },
	{ "java" , "text/x-java-source" },
	{ "jnlp" , "application/x-java-jnlp-file" },
	{ "jpeg" , "image/jpeg" },
	{ "jpg" , "image/jpeg" },
	{ "js" , "application/javascript;charset=utf-8" },
	{ "json" , "application/json" },
	{ "log" , "text/plain;charset=utf-8" },
	{ "m3u" , "audio/x-mpegurl" },
	{ "m4v" , "video/mp4" },
	{ "man" , "text/troff" },
	{ "mathml" , "application/mathml+xml" },
	{ "mbox" , "application/mbox" },
	{ "mdoc" , "text/troff" },
	{ "me" , "text/troff" },
	{ "mid" , "audio/midi" },
	{ "midi" , "audio/midi" },
	{ "mime" , "message/rfc822" },
	{ "mml" , "application/mathml+xml" },
	{ "mng" , "video/x-mng" },
	{ "mov" , "video/quicktime" },
	{ "mp3" , "audio/mpeg" },
	{ "mp4" , "video/mp4" },
	{ "mp4v" , "video/mp4" },
	{ "mpeg" , "video/mpeg" },
	{ "mpg" , "video/mpeg" },
	{ "ms" , "text/troff" },
	{ "msi" , "application/x-msdownload" },
	{ "odp" , "application/vnd.oasis.opendocument.presentation" },
	{ "ods" , "application/vnd.oasis.opendocument.spreadsheet" },
	{ "odt" , "application/vnd.oasis.opendocument.text" },
	{ "ogg" , "application/ogg" },
	{ "p" , "text/x-pascal" },
	{ "pas" , "text/x-pascal" },
	{ "pbm" , "image/x-portable-bitmap" },
	{ "pdf" , "application/pdf" },
	{ "pem" , "application/x-x509-ca-cert" },
	{ "pgm" , "image/x-portable-graymap" },
	{ "pgp" , "application/pgp-encrypted" },
	{ "pkg" , "application/octet-stream" },
	{ "pl" , "text/x-script.perl" },
	{ "pm" , "text/x-script.perl-module" },
	{ "png" , "image/png" },
	{ "pnm" , "image/x-portable-anymap" },
	{ "ppm" , "image/x-portable-pixmap" },
	{ "pps" , "application/vnd.ms-powerpoint" },
	{ "ppt" , "application/vnd.ms-powerpoint" },
	{ "ps" , "application/postscript" },
	{ "psd" , "image/vnd.adobe.photoshop" },
	{ "py" , "text/x-script.python" },
	{ "qt" , "video/quicktime" },
	{ "ra" , "audio/x-pn-realaudio" },
	{ "rake" , "text/x-script.ruby" },
	{ "ram" , "audio/x-pn-realaudio" },
	{ "rar" , "application/x-rar-compressed" },
	{ "rb" , "text/x-script.ruby" },
	{ "rdf" , "application/rdf+xml" },
	{ "roff" , "text/troff" },
	{ "rpm" , "application/x-redhat-package-manager" },
	{ "rss" , "application/rss+xml" },
	{ "rtf" , "application/rtf" },
	{ "ru" , "text/x-script.ruby" },
	{ "s" , "text/x-asm" },
	{ "sgm" , "text/sgml" },
	{ "sgml" , "text/sgml" },
	{ "sh" , "application/x-sh" },
	{ "sig" , "application/pgp-signature" },
	{ "snd" , "audio/basic" },
	{ "so" , "application/octet-stream" },
	{ "svg" , "image/svg+xml" },
	{ "svgz" , "image/svg+xml" },
	{ "swf" , "application/x-shockwave-flash" },
	{ "t" , "text/troff" },
	{ "tar" , "application/x-tar" },
	{ "tbz" , "application/x-bzip-compressed-tar" },
	{ "tcl" , "application/x-tcl" },
	{ "tex" , "application/x-tex" },
	{ "texi" , "application/x-texinfo" },
	{ "texinfo" , "application/x-texinfo" },
	{ "text" , "text/plain" },
	{ "tif" , "image/tiff" },
	{ "tiff" , "image/tiff" },
	{ "torrent" , "application/x-bittorrent" },
	{ "tr" , "text/troff" },
	{ "txt" , "text/plain" },
	{ "vcf" , "text/x-vcard" },
	{ "vcs" , "text/x-vcalendar" },
	{ "vrml" , "model/vrml" },
	{ "war" , "application/java-archive" },
	{ "wav" , "audio/x-wav" },
	{ "wma" , "audio/x-ms-wma" },
	{ "wmv" , "video/x-ms-wmv" },
	{ "wmx" , "video/x-ms-wmx" },
	{ "wrl" , "model/vrml" },
	{ "wsdl" , "application/wsdl+xml" },
	{ "xbm" , "image/x-xbitmap" },
	{ "xhtml" , "application/xhtml+xml" },
	{ "xls" , "application/vnd.ms-excel" },
	{ "xml" , "application/xml" },
	{ "xpm" , "image/x-xpixmap" },
	{ "xsl" , "application/xml" },
	{ "xslt" , "application/xslt+xml" },
	{ "yaml" , "text/yaml" },
	{ "yml" , "text/yaml" },
	{ "zip" , "application/zip" }
};


const char *apoHttpListener::getFileType(const char *fileName)
{
	const char *fileExt = nd_file_ext_name(fileName);
	if (fileExt && *fileExt) {
		for (int i = 0; i < sizeof(_s_fileInfo) / sizeof(_s_fileInfo[0]); i++) {
			if (ndstricmp(_s_fileInfo[i].fileExt, fileExt) == 0) {
				return _s_fileInfo[i].httpHeader;
			}
		}
	}
	return "application/octet-stream";
}