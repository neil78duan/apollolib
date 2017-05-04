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

#include <vector>
#include <string>

struct BreakPointInfo
{
	std::string functionName;
	std::string nodeName;

	BreakPointInfo(const char *func, const char *node) :functionName(func), nodeName(node)
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

class LocalDebugger
{
public:
	LocalDebugger(LogicParserEngine *parser,LogicObjectBase *owner=NULL);
	~LocalDebugger();

	// debug function, return 0 success ,renturn -1 error , error== would_block , in step-debug
	int runCmdline(int argc, const char *argv[], int encodeType = ND_ENCODE_TYPE); 
	int runStep(); //return  0 success ,renturn -1 error , error== would_block , in step-debug
	int runContinue();	
	int runTo(const char *funcName, const char *nodeName);

	ndxml_root & getParserInfo();

	int getLastError() { return m_parser->getErrno(); }

	void addBreakPoint(const char *funcName, const char *nodeName);
	void delBreakPoint(const char *funcName, const char *nodeName);

	LogicObjectBase *getOwner() { return m_owner; }
	void setOwner(LogicObjectBase *owner) { m_owner = owner; }
	
	bool isDebugging();

private:

	bool isBreakPoint(const char *func, const char *node);
	bool makeParserInfo();
	LogicParserEngine *m_parser;
	LogicObjectBase *m_owner;
	ndxml_root m_parserInfo;

	breakPoint_vct m_breakPoints;

};


#endif
