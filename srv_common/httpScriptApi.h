/* file httpScriptApi.h
 *
 * export api for http script 
 *
 * create by duan 
 *
 */


#include "ndapplib/applib.h"
#include "ndapplib/nd_httpListener.h"
#include "logic_parser/logicEngineRoot.h"
#include "srv_common/apoGameObj.h"

#ifndef _HTTP_SCRIPT_API_H_
#define _HTTP_SCRIPT_API_H_

class apoHttpScriptMgr : public ApoGameObj
{
	typedef ApoGameObj _myBase;
public:
	apoHttpScriptMgr();
	virtual ~apoHttpScriptMgr();

	bool getOtherObject(const char*objName, DBLDataNode &val);
};


class apoHttpListener : public NDHttpListener
{
	typedef NDHttpListener _myBase;
public:
	apoHttpListener(nd_fectory_base *sf = NULL);
	virtual ~apoHttpListener();
protected:

	virtual int onRequestScript(const char* script, NDHttpSession *session, const NDHttpRequest &request);
};

extern apoHttpScriptMgr * getHttpScriptObj();

#endif
