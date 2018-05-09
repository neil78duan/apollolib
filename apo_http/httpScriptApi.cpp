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

APOLLO_SCRIPT_API_DEF(apollo_set_http_handler, "http_install_req_handler(listenerName, reqPath,scriptName)")
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

APOLLO_SCRIPT_API_DEF(apollo_http_error, "HTTP_error_response(session, errorId, desc)")
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

APOLLO_SCRIPT_API_DEF(apollo_http_respone, "HTTP_response(session,status, header, body)")
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


APOLLO_SCRIPT_API_DEF(apollo_http_get_header, "http_get_header(requestObj)")
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

APOLLO_SCRIPT_API_DEF(apollo_http_get_body, "http_get_body(requestObj)")
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

APOLLO_SCRIPT_API_DEF(apollo_http_get_listener, "http_get_Listener(requestObj)")
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


APOLLO_SCRIPT_API_DEF(apollo_http_build_body, "http_build_body( body_text)")
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


APOLLO_SCRIPT_API_DEF(apollo_http_get, "httpShort_get(host,port, path, handler,userData)")
{
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



////////////////////////////////////


static void _deattach_from_listen(nd_handle handle, void *param)
{
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

	parse_arg_list_t args;
	args.push_back(DBLDataNode(m_handler.c_str()));
	args.push_back(DBLDataNode((void*)this, OT_OBJ_NDOBJECT));
	args.push_back(DBLDataNode(response->getBody()));
	args.push_back(DBLDataNode((void*)&response, OT_OBJ_NDOBJECT));
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
