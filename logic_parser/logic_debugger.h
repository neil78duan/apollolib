/* file : logic_debugger.h
 *
 * debugger for logic parser
 *
 * create by duan 
 *
 * 2017.5.2
 */

#ifndef _LOGIC_DEBUGGER_H_
#define _LOGIC_DEBUGGER_H_

#include "logic_parser/logicParser.h"
#include "ndapplib/ndsingleton.h"

#include <vector>
#include <string>

struct BreakPointInfo
{
	bool tempBreak;
	std::string functionName;
	std::string nodeName;

	BreakPointInfo(const char *func, const char *node,bool bTemp) :functionName(func), nodeName(node), tempBreak(bTemp)
	{

	}

	bool operator<(const BreakPointInfo &r)const
	{
		if (functionName < r.functionName && nodeName < r.nodeName) {
			return true;
		}
		return false;
	}

	bool operator==(const BreakPointInfo &r)const
	{
		if (0 == ndstricmp(functionName.c_str(), r.functionName.c_str()) && 
			0 == ndstricmp(nodeName.c_str(), r.nodeName.c_str()))
		{
			return true;
		}
		return false;
	}
	
};
typedef std::vector<BreakPointInfo> breakPoint_vct;
//////////////////////////////////////////////////////////////////////////

// processHeaderInfo[LOGIC_MAX_PROCESS] stored in share memory name "apoLogicDebugHeader"
#define PROCESS_NAME_SIZE 64
#define LOGIC_MAX_PROCESS 10
#define LOGIC_DEBUGER_HEADER_NAME "apoLogicDebugHeader"

enum parserRunStat
{
	E_RUN_DBG_STAT_NONE,
	E_RUN_DBG_STAT_RUNNING,
	E_RUN_DBG_STAT_WAIT_CMD,
	E_RUN_DBG_STAT_TERMINATE
};

enum parserDebugInputCmd
{
	E_DBG_INPUT_CMD_NONE,
	E_DBG_INPUT_CMD_NEW_BREAKPOINT,
	E_DBG_INPUT_CMD_STEP_RUN,
	E_DBG_INPUT_CMD_CONTINUE,
	E_DBG_INPUT_CMD_RUN_TO_CURSOR,
	E_DBG_INPUT_CMD_DEL_BREAKPOINT,
	E_DBG_INPUT_CMD_TERMINATED,

};

struct processHeaderInfo 
{
	NDUINT32 processId;		//nd_processid()
	ndatomic_t runStat;		//0 common/not-run , 1 running, 2 wait-debug, 3 terminate 
	ndatomic_t inputCmd;	//0 none , 1 input break point, 2 run-step, 3 run-continue , 4 run-to-current-node
	char processName[PROCESS_NAME_SIZE];
	char shareMemName[PROCESS_NAME_SIZE];
	char semName[PROCESS_NAME_SIZE];
	char cmdBuf[1024];
};

class ShareProcessLists
{
public:
	ShareProcessLists();
	~ShareProcessLists();


	static ShareProcessLists*get_Instant()
	{
		return NDSingleton<ShareProcessLists>::Get();
	}
	static void destroy_Instant()
	{
		NDSingleton<ShareProcessLists>::Destroy();
	}

	int Init();
	void Destroy();

	processHeaderInfo *createProcessInfo(NDUINT32 processId, const char *procName);
	void delProcess(NDUINT32 processId);

	processHeaderInfo *getProcessInfo(NDUINT32 processId);

	bool OutPutInfo(ndxml *dataInfo);
public:

	nd_filemap_t m_outPutMem;
	ndsem_t m_RunningSem;
};

//////////////////////////////////////////////////////////////////////////

class LocalDebugger
{
public:
	LocalDebugger(LogicParserEngine *parser,LogicObjectBase *owner=NULL);
	~LocalDebugger();

	//command for client
	int runCmdline(int argc, const char *argv[], int encodeType = ND_ENCODE_TYPE); 
	int runStep(); //return  0 success ,renturn -1 error , error== would_block , in step-debug
	int runContinue();	
	int runTo(const char *funcName, const char *nodeName);
	int stopDebug();
	int addBreakPoint(const char *funcName, const char *nodeName,bool isTemp=false);
	int delBreakPoint(const char *funcName, const char *nodeName);

	//run on parser
	int onEnterStep(const char *funcName, const char *nodeName);

	ndxml_root & getParserInfo();
	int getLastError() { return m_parser->getErrno(); }


	LogicObjectBase *getOwner() { return m_owner; }
	void setOwner(LogicObjectBase *owner) { m_owner = owner; }
	LogicParserEngine *getParser() { return m_parser; }
	bool isDebugging();
	int getEncodeType() { return m_encodeType; }
private:
	int inputCmd(int cmdId);
	int inputCmd(int cmdId, const char *func, const char *nodeName);

	int waitEvent();

	int m_encodeType;
	NDUINT32 m_aimProcessId;
	bool m_isAttached;
	bool isBreakPoint(const char *func, const char *node, bool bTrytoDel=false);
	bool makeParserInfo();
	bool getParamFromInput(std::string &funcName, std::string &nodeName);

	LogicParserEngine *m_parser;
	LogicObjectBase *m_owner;
	ndxml_root m_parserInfo;

	breakPoint_vct m_breakPoints;

};


#endif
