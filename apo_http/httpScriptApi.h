/* file httpScriptApi.h
 *
 * export api for http script 
 *
 * create by duan 
 *
 */

#ifndef _HTTP_SCRIPT_API_H_
#define _HTTP_SCRIPT_API_H_

#include "ndapplib/applib.h"
#include "ndapplib/nd_httpListener.h"
#include "logic_parser/logicEngineRoot.h"
#include "apo_http/apoGameObj.h"

typedef ApoGameObj apoHttpScriptMgr;
// 
// class apoHttpScriptMgr : public LogicObjectBase
// {
// 	typedef LogicObjectBase _myBase;
// public:
// 	apoHttpScriptMgr();
// 	virtual ~apoHttpScriptMgr();
// 	
// 	LogicParserEngine *getScriptHandler() { return &m_logicEngine; }
// 	bool RunScript(parse_arg_list_t &args, DBLDataNode &result);
// 	bool RunScript(const char *script);
// 	bool SendScriptEvent(int event_id, int args, ...);
// 	bool SendEvent0(int event);
// 	bool SendEvent1(int event, const DBLDataNode &val1);
// 	int getScriptError() { return m_logicEngine.getErrno(); }
// 
// 	bool opRead(const DBLDataNode& id, DBLDataNode &val);
// 	bool opWrite(const DBLDataNode& id, const DBLDataNode &val);
// 	bool opAdd(const DBLDataNode& id, const DBLDataNode &val);
// 	bool opSub(const DBLDataNode& id, const  DBLDataNode &val);
// 
// 	LogicObjectBase *getObjectMgr(const char* destName);
// 	bool getOtherObject(const char*objName, DBLDataNode &val);
// 
// protected:
// 	LogicParserEngine m_logicEngine;
// };
class ApoHttpClientShort : public HttpConnector
{
	typedef HttpConnector _myBase;
public:
	ApoHttpClientShort();
	virtual ~ApoHttpClientShort();

	int Request(NDHttpRequest &request, const char *host, int port, const char *path, const char* repHandler);

	virtual void onResponse(NDHttpResponse *response);
protected:
	std::string m_handler;
};


class apoHttpListener : public NDHttpListener
{
	typedef NDHttpListener _myBase;
public:
	apoHttpListener(nd_fectory_base *sf = NULL);
	virtual ~apoHttpListener();
protected:
	virtual void *getScriptEngine();
	virtual int onRequestScript(const char* script, NDHttpSession *session, const NDHttpRequest &request);

};

extern apoHttpScriptMgr * getHttpScriptObj(); //need implement this function

#endif
