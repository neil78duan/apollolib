/* file : logic_debugger.cpp
 *
 * debugger for logic parser
 *
 * create by duan 
 *
 * 2017.5.2
 */

#include "logic_debugger.h"
#include "logicEngineRoot.h"

static nd_filemap_t g_debugHeader = { 0 };
ShareProcessLists::ShareProcessLists()
{
}

ShareProcessLists::~ShareProcessLists()
{
}

int ShareProcessLists::Init()
{
	size_t size = sizeof(processHeaderInfo) * LOGIC_MAX_PROCESS;
	int ret = nd_mem_share_create(LOGIC_DEBUGER_HEADER_NAME,  size, &g_debugHeader);
	if (-1 == ret) {
		return -1;
	}
	return 0;
}
void ShareProcessLists::Destroy()
{
	if (nd_mem_share_addr(&g_debugHeader)) {
		nd_mem_share_close(&g_debugHeader);
	}
}


processHeaderInfo *ShareProcessLists::createProcessInfo(NDUINT32 processId, const char *procName)
{
	if (!nd_mem_share_addr(&g_debugHeader)) {
		return NULL;
	}
	processHeaderInfo * pAddr = NULL;
	processHeaderInfo *proHeader = (processHeaderInfo *)nd_mem_share_addr(&g_debugHeader);
	for (int i = 0; i < LOGIC_MAX_PROCESS; i++){
		if (proHeader[i].processId == 0){
			pAddr = &proHeader[i];
			
			break;
		}
	}

	if (!pAddr){
		return NULL;
	}

	pAddr->processId = processId;

	strncpy(pAddr->processName, procName, PROCESS_NAME_SIZE);

	snprintf(pAddr->shareMemName, PROCESS_NAME_SIZE, "%s_smm", procName);
	snprintf(pAddr->semName, PROCESS_NAME_SIZE, "%s_sem", procName);

	int ret = nd_mem_share_create(pAddr->shareMemName, 1024 * 1024, &m_outPutMem);
	if (-1 == ret) {
		return NULL;
	}

	m_RunningSem = nd_sem_open(pAddr->semName);
	if (!m_RunningSem) {
		return NULL;
	}

	return pAddr;
}

void ShareProcessLists::delProcess(NDUINT32 processId)
{
	processHeaderInfo *proceInfo = getProcessInfo(processId);
	if (!proceInfo)	{
		return;
	}

	nd_mem_share_close(&m_outPutMem);
	nd_sem_destroy(m_RunningSem);
	proceInfo->processId = 0;
}

processHeaderInfo *ShareProcessLists::getProcessInfo(NDUINT32 processId)
{
	processHeaderInfo * pAddr = NULL;
	processHeaderInfo *proHeader = (processHeaderInfo *)nd_mem_share_addr(&g_debugHeader);
	for (int i = 0; i < LOGIC_MAX_PROCESS; i++){
		if (proHeader[i].processId == 0){
			return &proHeader[i];
		}
	}
	return NULL;
}


bool ShareProcessLists::OutPutInfo(ndxml *dataInfo)
{
	if (!nd_mem_share_addr(&m_outPutMem)) {
		return false;
	}
	size_t size = ndxml_to_buf(dataInfo, (char *)nd_mem_share_addr(&m_outPutMem), (int) nd_mem_share_size(&m_outPutMem));
	if (size ==0)	{
		return false;
	}
	return true;
}

//////////////////////////////////////////////////////////////////////////



void* parseThreadRun(void* param)
{
	ParserCmdLine *pcmdInfo = (ParserCmdLine*)param;

	LogicEngineRoot *root = LogicEngineRoot::get_Instant();
	nd_assert(root);
	LocalDebugger &debugger = root->getGlobalDebugger();
	LogicParserEngine *parser = debugger.getParser();

	//pre debug 
	NDUINT32 processId = nd_processid();
	processHeaderInfo * proceInfo = ShareProcessLists::get_Instant()->createProcessInfo(processId, nd_process_name());
	if (!proceInfo)	{
		delete pcmdInfo;
		return (void*)-1;
	}

	proceInfo->runStat = 1;
	proceInfo->inputCmd = 0;

	int ret = parser->runCmdline(pcmdInfo->argc, (const char**)pcmdInfo->argv, debugger.getEncodeType());

	delete pcmdInfo;

	ShareProcessLists::get_Instant()->delProcess(processId);

	if (0 == ret)	{
		return (void*)0;
	}
	return (void*)parser->getErrno();
}


LocalDebugger::LocalDebugger(LogicParserEngine *parser, LogicObjectBase *owner) :m_parser(parser), m_owner(owner)
{
	m_encodeType = ND_ENCODE_TYPE;
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

	ParserCmdLine *cmdLine = new ParserCmdLine(argc, argv);
	ndthread_t thId = 0;
	ndth_handle h = nd_createthread(parseThreadRun, (void*)cmdLine, &thId, 0);
	if (h){
		return 0;
	}
	return -1;
}

//return  0 success ,renturn -1 error , error== would_block , in step-debug
int LocalDebugger::runStep()
{
	return inputCmd(E_DBG_INPUT_CMD_STEP_RUN);
}


int LocalDebugger::runContinue()
{
	return inputCmd(E_DBG_INPUT_CMD_CONTINUE);
}


int LocalDebugger::stopDebug()
{
	return inputCmd(E_DBG_INPUT_CMD_TERMINATED);
}


int LocalDebugger::runTo(const char *func, const char *node)
{
	return inputCmd(E_DBG_INPUT_CMD_RUN_TO_CURSOR, func, node);
}


int LocalDebugger::addBreakPoint(const char *funcName, const char *nodeName, bool isTemp )
{
	if (m_isAttached) {
		return inputCmd(E_DBG_INPUT_CMD_NEW_BREAKPOINT, funcName, nodeName);
	}
	else {

		if (!isBreakPoint(funcName, nodeName))	{
			m_breakPoints.push_back(BreakPointInfo(funcName, nodeName,isTemp));
		}
	}
	return 0;
}
int LocalDebugger::delBreakPoint(const char *funcName, const char *nodeName)
{
	if (m_isAttached) {
		return inputCmd(E_DBG_INPUT_CMD_DEL_BREAKPOINT, funcName, nodeName);
	}
	else {

		for (breakPoint_vct::iterator it = m_breakPoints.begin(); it != m_breakPoints.end(); it++) {
			if (0 == ndstricmp(it->functionName.c_str(), funcName) &&
				0 == ndstricmp(it->nodeName.c_str(), nodeName))	{
				m_breakPoints.erase(it);
				break;
			}
		}
	}
	return 0;
}


//run on parser
int LocalDebugger::onEnterStep(const char *funcName, const char *nodeName)
{
	
	if (isBreakPoint(funcName, nodeName, true) || m_parser->getStepMode()){

		return waitEvent();
	}
	return 0;
}

bool LocalDebugger::isBreakPoint(const char *func, const char *node, bool bTrytoDel )
{
	for (breakPoint_vct::iterator it = m_breakPoints.begin(); it != m_breakPoints.end(); it++) {
		if (0 == ndstricmp(it->functionName.c_str(),func) &&
			0 == ndstricmp(it->nodeName.c_str(), node))	{

			if (bTrytoDel && it->tempBreak)	{
				m_breakPoints.erase(it);
			}
			return true;
		}

	}
	return false;
}

int LocalDebugger::inputCmd(int cmdId)
{
	processHeaderInfo * proceInfo = ShareProcessLists::get_Instant()->getProcessInfo(m_aimProcessId);
	if (!proceInfo)	{
		return -1;
	}

	proceInfo->inputCmd = cmdId;

	ndsem_t sem = nd_sem_open(proceInfo->semName);
	if (!sem)	{
		return -1;
	}

	nd_sem_post(sem);
	nd_sem_destroy(sem);
	return 0;
}
int LocalDebugger::inputCmd(int cmdId, const char *func, const char *nodeName)
{
	processHeaderInfo * proceInfo = ShareProcessLists::get_Instant()->getProcessInfo(m_aimProcessId);
	if (!proceInfo)	{
		return -1;
	}

	proceInfo->inputCmd = cmdId;

	snprintf(proceInfo->cmdBuf, sizeof(proceInfo->cmdBuf), "<node func=\"%s\" exenode=\"%s\" />", func, nodeName);

	ndsem_t sem = nd_sem_open(proceInfo->semName);
	if (!sem)	{
		return -1;
	}

	nd_sem_post(sem);
	nd_sem_destroy(sem);

	return 0;
}


int LocalDebugger::waitEvent()
{
	NDUINT32 processId = nd_processid();
	ShareProcessLists *processList = ShareProcessLists::get_Instant();

	int ret = 0;
	processHeaderInfo * proceInfo = NULL;
	std::string funcName, nodeName;

	ndxml_root &parserInfo = getParserInfo();

	ndxml_to_buf(&parserInfo,(char*) nd_mem_share_addr(&processList->m_outPutMem), (size_t)nd_mem_share_size(&processList->m_outPutMem));

RE_WIAT:

	ret = nd_sem_wait(processList->m_RunningSem, INFINITE);
	if (0!=ret)	{
		return -1;
	}
	
	proceInfo = processList->getProcessInfo(processId);
	//handler event 
	switch (proceInfo->inputCmd)
	{
	case E_DBG_INPUT_CMD_NONE:
		goto RE_WIAT;
		break;
	case E_DBG_INPUT_CMD_NEW_BREAKPOINT:
		if (getParamFromInput(funcName, nodeName)) {
			addBreakPoint(funcName.c_str(), nodeName.c_str());
		}
		goto RE_WIAT;
		break;
	case E_DBG_INPUT_CMD_STEP_RUN:
		m_parser->setStepMode(true);
		break;
	case E_DBG_INPUT_CMD_CONTINUE:
		m_parser->setStepMode(false);
		break;
	case E_DBG_INPUT_CMD_RUN_TO_CURSOR:
		m_parser->setStepMode(false);
		if (getParamFromInput(funcName, nodeName)) {
			addBreakPoint(funcName.c_str(), nodeName.c_str(),true);
		}

		break;
	case E_DBG_INPUT_CMD_DEL_BREAKPOINT:
		if (getParamFromInput(funcName, nodeName)) {
			delBreakPoint(funcName.c_str(), nodeName.c_str());
		}

		goto RE_WIAT;

		break;
	case E_DBG_INPUT_CMD_TERMINATED:
		return -1;
		break;
	default:
		goto RE_WIAT;
		break;
	}

	return 0;
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


bool LocalDebugger::getParamFromInput(std::string &funcName, std::string &nodeName)
{
	NDUINT32 processId = nd_processid();
	ShareProcessLists *processList = ShareProcessLists::get_Instant();
	processHeaderInfo * proceInfo =  processList->getProcessInfo(processId);

	//snprintf(proceInfo->cmdBuf, sizeof(proceInfo->cmdBuf), "<node func=\"%s\" exenode=\"%s\" />", func, nodeName);
	ndxml *xml = ndxml_from_text(proceInfo->cmdBuf);
	if (xml){
		funcName = ndxml_getattr_val(xml, "func");
		nodeName = ndxml_getattr_val(xml, "exenode");
		ndxml_free(xml);
		return true;
	}
	return false;	
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