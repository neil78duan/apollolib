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
#include "game_parser/apoObjSrv.h"

//typedef ApoGameObj apoHttpScriptMgr;

class apoHttpScriptMgr : public LogicObjectBase
{
public:
	apoHttpScriptMgr();
	virtual ~apoHttpScriptMgr();

	LogicParserEngine *getScriptHandler() { return &m_logicEngine; }
	bool RunScript(parse_arg_list_t &args, LogicDataObj &result);
	bool RunScript(const char *script);
	bool SendScriptEvent(int event_id, int args, ...);
	bool SendEvent0(int event);
	bool SendEvent1(int event, const LogicDataObj &val1);
	int getScriptError() { return m_logicEngine.getErrno(); }

	bool opRead(const LogicDataObj& id, LogicDataObj &val);
	bool opWrite(const LogicDataObj& id, const LogicDataObj &val);
	bool opAdd(const LogicDataObj& id, const LogicDataObj &val);
	bool opSub(const LogicDataObj& id, const  LogicDataObj &val);
	LogicObjectBase *getObjectMgr(const char* destName);

	bool getOtherObject(const char*objName, LogicDataObj &val);

	typedef std::map<std::string, LogicObjectBase*, apoStringLess<std::string> > objectMgr_vct;
protected:

	//objectMgr_vct m_objectMgr;
	LogicParserEngine m_logicEngine;
};
class ApoHttpClientShort : public HttpConnector
{
	typedef HttpConnector _myBase;
public:
	ApoHttpClientShort();
	virtual ~ApoHttpClientShort();

	int Request(NDHttpRequest &request, const char *host, int port, const char *path, const char* repHandler);

	virtual void onResponse(NDHttpResponse *response);
	void setHandler(const char*handler) { m_handler = handler; }
	void setUserData(const LogicDataObj&userData) { m_userData = userData; }

	virtual void OnClose();
protected:
	std::string m_handler;
	LogicDataObj m_userData;
};

struct fileCacheInfo
{
	size_t size;
	char md5[33];
	char *dataAddr;
};
typedef std::map<std::string, fileCacheInfo> fileCache_map_t;

class apoHttpListener : public NDHttpListener
{
	typedef NDHttpListener _myBase;
public:
	apoHttpListener(nd_fectory_base *sf = NULL);
	virtual ~apoHttpListener();
	void setPath(const char *readable, const char *writable);
	bool downloadFile(const char *filePath, NDHttpSession *session, const NDHttpRequest &request);
	bool cacheFile(const char *filePath);
	bool uncacheFile(const char*filePath);
	void Destroy(int flag);
	void SetHttpEncode(const char *encodeName);
	int getEncodeType();
	static const char *getFileType(const char *fileName);
protected:
	virtual void *getScriptEngine();
	virtual int onRequestScript(const char* script, NDHttpSession *session, const NDHttpRequest &request);
	void *loadFile(const char *filePath, NDINT64 offset, NDINT64 &size, size_t &fileSize);
	void *_loadFile(const char *filePath, NDINT64 offset, NDINT64 &size, size_t &fileSize);
	void releaseFile(void *fileData);
	bool getFileOffset(const char*range, NDINT64 &size, NDINT64 &offset);

	void destroyCache();

	int m_encodeType;
	std::string m_readablePath;
	std::string m_writablePath;

	fileCache_map_t m_fileCache;
};

extern apoHttpScriptMgr * getHttpScriptObj(); //need implement this function

#endif
