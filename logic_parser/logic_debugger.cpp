/* file : logic_debugger.cpp
 *
 * debugger for logic parser
 *
 * create by duan 
 *
 * 2017.5.2
 */

#include "logic_debugger.h"

LocalDebugger::LocalDebugger(LogicParserEngine *parser, LogicObjectBase *owner) :m_parser(parser), m_owner(owner)
{
	ndxml_initroot(&m_parserInfo);
}

LocalDebugger::~LocalDebugger()
{
}


bool LocalDebugger::isDebugging()
{
	return m_parser->isStepModeRunning();
}

// debug function, return 0 success ,renturn -1 error , error== would_block , in step-debug
int LocalDebugger::runCmdline(int argc, const char *argv[], int encodeType )
{
	nd_assert(m_parser);
	m_parser->setStepMode();
	int ret = m_parser->runCmdline(argc, argv, encodeType);
	if (0==ret)	{
		return 0;
	}
	if (getLastError() == NDERR_WOULD_BLOCK){
		ret = 0;
	}
	else {
		return ret;
	}
	while (ret==0 && getLastError() == NDERR_WOULD_BLOCK){
		ret = runContinue();
	}
	return ret;
}

//return  0 success ,renturn -1 error , error== would_block , in step-debug
int LocalDebugger::runStep()
{
	m_parser->runStep();
	return 0;
}


int LocalDebugger::runContinue()
{
	int ret = runStep();
	while (ret == 0 && getLastError() == NDERR_WOULD_BLOCK){

// 		runningStack *stack = m_parser->getCurStack();
// 		nd_assert(stack);
// 		const char *nodeName = m_parser->getLastErrorNode();
// 
// 		if (isBreakPoint(stack->cmd->cmdname, nodeName)) {
// 			return ret;
// 		}
		ret = runStep();
	}
	return 0;
}

int LocalDebugger::runTo(const char *func, const char *node)
{
	int ret = runStep();
	while (ret == -1 && getLastError() == NDERR_WOULD_BLOCK){

		runningStack *stack = m_parser->getCurStack();
		nd_assert(stack);
		const char *nodeName = m_parser->getLastErrorNode();

		if (0 == ndstricmp(stack->cmd->cmdname, func) &&
			0 == ndstricmp(nodeName, node))	{
			return -1;
		}

		ret = runStep();
	}
	return 0;
}


bool LocalDebugger::isBreakPoint(const char *func, const char *node)
{
	for (breakPoint_vct::iterator it = m_breakPoints.begin(); it != m_breakPoints.end(); it++) {
		if (0 == ndstricmp(it->functionName.c_str(),func) &&
			0 == ndstricmp(it->nodeName.c_str(), node))	{
			return true;
		}

	}
	return false;
}

void LocalDebugger::addBreakPoint(const char *funcName, const char *nodeName)
{
	if (!isBreakPoint(funcName, nodeName))	{
		m_breakPoints.push_back(BreakPointInfo(funcName, nodeName));
	}
}
void LocalDebugger::delBreakPoint(const char *funcName, const char *nodeName)
{
	for (breakPoint_vct::iterator it = m_breakPoints.begin(); it != m_breakPoints.end(); it++) {
		if (0 == ndstricmp(it->functionName.c_str(), funcName) &&
			0 == ndstricmp(it->nodeName.c_str(), nodeName))	{
			m_breakPoints.erase(it);
			break;
		}
	}
}

ndxml_root & LocalDebugger::getParserInfo()
{
	if (m_parser->isStepModeRunning())	{
		makeParserInfo();
	}
	else {
		ndxml_destroy(&m_parserInfo);
	}
	return m_parserInfo;
}

bool LocalDebugger::makeParserInfo()
{
	ndxml_destroy( &m_parserInfo );
	runningStack *stack = m_parser->getCurStack();
	
	int len;
	char buf[1024];
	char *p = buf;
	
	len = snprintf(p, sizeof(buf), "%s(", stack->cmd->cmdname);
	p += len;

	for (int  i = 0; i < stack->params.size(); i++)	{
		len = stack->params[i].Sprint(p, sizeof(buf) - (p - buf));
		p += len;
	}
	len = snprintf(p, sizeof(buf) - (p - buf), ")");
	p += len;

	ndxml *xmlInfo = ndxml_addnode(&m_parserInfo, "local",NULL);
	if (!xmlInfo)	{
		return false;
	}

	ndxml_addnode(xmlInfo, "function", buf);

	ndxml_addnode(xmlInfo, "instruct_flag", m_parser->m_registorFlag? "true": "false");
	ndxml_addnode(xmlInfo, "control_flag", m_parser->m_registorCtrl ? "true" : "false");

	const char *val = m_parser->m_registerVal.GetText();
	ndxml_addnode(xmlInfo, "$curValue", val ? val: "0");


	for (int i = 0; i < stack->local_vars.size(); i++)	{
		const char *val = stack->local_vars[i].var.GetText();
		ndxml_addnode(xmlInfo, stack->local_vars[i].name.c_str(), val ? val : "0");
	}
	return true;
}