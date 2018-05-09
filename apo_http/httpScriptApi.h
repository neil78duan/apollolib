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

class ApoHttpClientShort : public HttpConnector
{
	typedef HttpConnector _myBase;
public:
	ApoHttpClientShort();
	virtual ~ApoHttpClientShort();

	int Request(NDHttpRequest &request, const char *host, int port, const char *path, const char* repHandler);

	virtual void onResponse(NDHttpResponse *response);
	void setHandler(const char*handler) { m_handler = handler; }
	void setUserData(const DBLDataNode&userData) { m_userData = userData; }

	virtual void OnClose();
protected:
	std::string m_handler;
	DBLDataNode m_userData;
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
