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
#include "httpScriptApi.h"

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

APOLLO_SCRIPT_API_DEF(apollo_http_get_body, "http_获得body(requestObj)")
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
///////////////////////////////////////////////

apoHttpScriptMgr::apoHttpScriptMgr()
{
}

apoHttpScriptMgr::~apoHttpScriptMgr()
{
}


bool apoHttpScriptMgr::getOtherObject(const char*objName, DBLDataNode &val)
{

	/*if (0 == ndstricmp(objName, "world-connector")) {
		NDConnector &conn = getWorldConnect();
		val.InitSet((void*)conn.GetHandle(), OT_OBJ_NDHANDLE);
		return true;
	}

	else if (0 == ndstricmp(objName, "FormatMsgData")) {
		userDefineDataType_map_t &msgObj = get_msgFormat();
		val.InitSet((void*)&msgObj);
		return true;
	}

	else*/ {
		return _myBase::getOtherObject(objName, val);
	}
	return false;
}
///////////////////////

apoHttpListener::apoHttpListener(nd_fectory_base *sf) : _myBase(sf)
{

}
apoHttpListener::~apoHttpListener()
{

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
