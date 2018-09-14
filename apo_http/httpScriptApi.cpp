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

static  NDHttpSession *_getSession(const DBLDataNode &objData)
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

APOLLO_SCRIPT_API_DEF(apollo_http_save_session, "HTTP_session_value_set(connectObj, name, value)")
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

APOLLO_SCRIPT_API_DEF(apollo_http_get_session, "HTTP_session_value_get(connectObj, name)")
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

APOLLO_SCRIPT_API_DEF(apollo_http_session_id, "HTTP_session_get_id(connectObj)")
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


	LogicObjectBase *owner = parser->getOwner();
	nd_assert(owner);
	DBLDataNode val;
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

APOLLO_SCRIPT_API_DEF(apollo_http_cache_file, "HTTP_cache_to_mem(filepath)")
{
	ND_TRACE_FUNC();
	CHECK_ARGS_NUM(args, 2, parser);
	if (!args[1].GetText()) {
		parser->setErrno(NDERR_INVALID_INPUT);
		return false;
	}
	LogicObjectBase *owner = parser->getOwner();
	nd_assert(owner);
	DBLDataNode val;
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
	LogicObjectBase *owner = parser->getOwner();
	nd_assert(owner);
	DBLDataNode val;
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
	
	LogicObjectBase *owner = parser->getOwner();
	nd_assert(owner);
	DBLDataNode val;
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
				DBLDataNode *pData = pJson->ref(i);
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
	if (inputError != 0) {
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
	args.push_back(DBLDataNode(m_handler.c_str()));
	args.push_back(DBLDataNode(this));
	args.push_back(DBLDataNode(response->getBody()));
	args.push_back(DBLDataNode(response));
	args.push_back(m_userData);

	DBLDataNode result;
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
	args.push_back(DBLDataNode(script));
	args.push_back(DBLDataNode(session));
	args.push_back(DBLDataNode(formJson));
	args.push_back(DBLDataNode((NDIBaseObj*)&request));

	DBLDataNode result;
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

	//
	char tmp_path[ND_FILE_PATH_SIZE];
	const char *pLoadPath = nd_getpath(filePath,tmp_path, sizeof(tmp_path));
	if (!nd_is_subpath(m_readablePath.c_str(), pLoadPath)) {
		if (!nd_is_subpath(m_writablePath.c_str(), pLoadPath)) {
			nd_logerror("Can not acccess %s\n", filePath);
			return false; 
		}
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
	response.addHeader("Content-Type","application/octet-stream");
	
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