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
	//m_RunningSem = 0;
	//m_cliSem = 0;

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
	processHeaderInfo *pAddr = getProcessInfo(0);
	if (!pAddr){
		return NULL;
	}

	pAddr->processId = processId;
	pAddr->debuggerProcessId = processId;

	strncpy(pAddr->processName, procName, PROCESS_NAME_SIZE);
	snprintf(pAddr->shareMemName, PROCESS_NAME_SIZE, "%s_smm", procName);
	snprintf(pAddr->semName, PROCESS_NAME_SIZE, "%s_sem", procName);
	snprintf(pAddr->semClient, PROCESS_NAME_SIZE, "%s_semCli", procName);

	pAddr->runStat = E_RUN_DBG_STAT_NONE;
	pAddr->inputCmd = 0;
	pAddr->outputCmd = 0;
	
// 
// 	int ret = nd_mem_share_create(pAddr->shareMemName, 1024 * 1024, &m_outPutMem);
// 	if (-1 == ret) {
// 		return NULL;
// 	}
// 
// 	m_RunningSem = nd_sem_open(pAddr->semName);
// 	if (!m_RunningSem) {
// 		return NULL;
// 	}
// 
// 	m_cliSem = nd_sem_open(pAddr->semClient);
// 	if (!m_cliSem) {
// 		return NULL;
// 	}
	return pAddr;
}

void ShareProcessLists::delProcess(NDUINT32 processId)
{
	processHeaderInfo *proceInfo = getProcessInfo(processId);
	if (!proceInfo)	{
		return;
	}

	//nd_mem_share_close(&m_outPutMem);
	//nd_sem_destroy(m_RunningSem);
	proceInfo->processId = 0;
}

processHeaderInfo *ShareProcessLists::getProcessInfo(NDUINT32 processId)
{
	if (!nd_mem_share_addr(&g_debugHeader)) {
		return NULL;
	}

	processHeaderInfo *proHeader = (processHeaderInfo *)nd_mem_share_addr(&g_debugHeader);
	for (int i = 0; i < LOGIC_MAX_PROCESS; i++){
		if (proHeader[i].processId == processId){
			return &proHeader[i];
		}
	}
	return NULL;
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
	ShareProcessLists *pProcessRoot = ShareProcessLists::get_Instant();

	processHeaderInfo * proceInfo = pProcessRoot->getProcessInfo(processId);

	if (!proceInfo)	{
		delete pcmdInfo;
		return (void*)-1;
	}

	proceInfo->runStat = 1;
	proceInfo->inputCmd = E_RUN_DBG_STAT_RUNNING;

	int ret = parser->runCmdline(pcmdInfo->argc, (const char**)pcmdInfo->argv, debugger.getEncodeType());

	delete pcmdInfo;

	proceInfo->runStat = E_RUN_DBG_STAT_TERMINATE;
	proceInfo->outputCmd = E_DBG_OUTPUT_CMD_TERMINATED;

	ndsem_t  sem = debugger.getClientSem();
	nd_sem_post(sem);
	//pProcessRoot->delProcess(processId);

	if (0 == ret)	{
		return (void*)0;
	}
	return (void*)parser->getErrno();
}

//////////////////////////////////////////////////////////////////////////

LogicDebugClient::LogicDebugClient()
{

	m_aimProcessId = 0;
	m_isAttached = 0 ;
}
LogicDebugClient::~LogicDebugClient()
{

}


int LogicDebugClient::localStrat(LogicParserEngine *parser, int argc, const char *argv[], int encodeType)
{
 	m_aimProcessId = nd_processid();

	LocalDebugger & debuggerHost = LogicEngineRoot::get_Instant()->getGlobalDebugger();
	debuggerHost.setParser(parser);
	if (-1 == debuggerHost.runCmdline(argc, argv, encodeType)) {
		return -1;
	}

	if (-1 == waitEvent()) {
		return -1;
	}
	return 0;
}


int LogicDebugClient::Attach(NDUINT32 processId)
{
	processHeaderInfo *pAddr = ShareProcessLists::get_Instant()->getProcessInfo(processId);
	if (!pAddr)	{
		return -1;
	}

	m_aimProcessId = processId;
	m_isAttached = true;
		
	pAddr->debuggerProcessId = nd_processid();

	LocalDebugger & debuggerHost = LogicEngineRoot::get_Instant()->getGlobalDebugger();

	return 0;
}


int LogicDebugClient::cmdAddBreakPoint(const char *funcName, const char *nodeName, bool isTemp)
{
	if (m_isAttached) {
		return inputCmd(isTemp ? E_DBG_INPUT_CMD_ADD_TEMP_BREAKPOINT:E_DBG_INPUT_CMD_NEW_BREAKPOINT, funcName, nodeName);
	}
	else {
		LocalDebugger & debuggerHost = LogicEngineRoot::get_Instant()->getGlobalDebugger();
		debuggerHost.addBreakPoint(funcName, nodeName,isTemp);
	}
	return 0;
}

int LogicDebugClient::cmdDelBreakPoint(const char *funcName, const char *nodeName)
{
	if (m_isAttached) {
		return inputCmd(E_DBG_INPUT_CMD_DEL_BREAKPOINT, funcName, nodeName);
	}
	else {
		LocalDebugger & debuggerHost = LogicEngineRoot::get_Instant()->getGlobalDebugger();
		debuggerHost.delBreakPoint(funcName, nodeName);
	}
	return 0;
}

int LogicDebugClient::postEndDebug()
{
	LocalDebugger & debuggerHost = LogicEngineRoot::get_Instant()->getGlobalDebugger();
	debuggerHost.postEndDebug();
	return 0;
}
int LogicDebugClient::waitEvent()
{
	int ret = 0;
	ShareProcessLists *processList = ShareProcessLists::get_Instant();
	processHeaderInfo * proceInfo = processList->getProcessInfo(m_aimProcessId);

	if (!proceInfo){
		return 0;
	}

	ndsem_t sem = getClientSem();

RE_WIAT:

	ret = nd_sem_wait(sem, INFINITE);
	if (0 != ret)	{
		nd_sem_destroy(sem);
		return -1;
	}

	//handler event 
	switch (proceInfo->outputCmd)
	{
	case E_DBG_OUTPUT_CMD_HIT_BREAKPOINT:
	case 	E_DBG_OUTPUT_CMD_STEP:
	{
		ndxml *xml = ndxml_from_text(proceInfo->cmdBuf);
		if (xml){
			onEnterStep(ndxml_getattr_val(xml, "func"), ndxml_getattr_val(xml, "exenode"));
			ndxml_free(xml);
		}

		break;
	}
	case E_DBG_OUTPUT_CMD_TERMINATED:
		onTerminate();
		break; 
	case E_DBG_OUTPUT_CMD_HANDLE_ACK:
		onCommandOk();
		break;

	default:
		goto RE_WIAT;
		break;
	}

	//nd_sem_destroy(sem);
	return 0;
}

int LogicDebugClient::inputCmd(int cmdId)
{
	processHeaderInfo * proceInfo = ShareProcessLists::get_Instant()->getProcessInfo(m_aimProcessId);
	if (!proceInfo)	{
		return -1;
	}

	proceInfo->inputCmd = cmdId;

	ndsem_t sem = getHostSem();

	nd_sem_post(sem);
	return waitEvent();
}

int LogicDebugClient::inputCmd(int cmdId, const char *func, const char *nodeName)
{
	processHeaderInfo * proceInfo = ShareProcessLists::get_Instant()->getProcessInfo(m_aimProcessId);
	if (!proceInfo)	{
		return -1;
	}

	proceInfo->inputCmd = cmdId;

	snprintf(proceInfo->cmdBuf, sizeof(proceInfo->cmdBuf), "<node func=\"%s\" exenode=\"%s\" />", func, nodeName);

	ndsem_t sem = getHostSem();
	nd_sem_post(sem);
	return waitEvent();
}


ndsem_t LogicDebugClient::getHostSem()
{
	LocalDebugger &debugger = LogicEngineRoot::get_Instant()->getGlobalDebugger();
	return debugger.m_RunningSem;

}
ndsem_t LogicDebugClient::getClientSem()
{

	LocalDebugger &debugger = LogicEngineRoot::get_Instant()->getGlobalDebugger();
	return debugger.m_cliSem;
}

bool LogicDebugClient::onEnterStep(const char *function, const char *node)
{
	nd_logdebug("run to step\n");
	return false;
}


void LogicDebugClient::onTerminate()
{
	nd_logdebug("terminate\n");

}

void LogicDebugClient::onCommandOk()
{
	nd_logdebug("input command run ok\n");

}

ndxml *LogicDebugClient::getParserInfo()
{
	ShareProcessLists *processList = ShareProcessLists::get_Instant();
	processHeaderInfo * proceInfo = processList->getProcessInfo(m_aimProcessId);

	if (!proceInfo){
		return NULL;
	}

	nd_filemap_t fileMap;
	int ret = nd_mem_share_create(proceInfo->shareMemName,0, &fileMap);
	if (-1 == ret) {
		return NULL;
	}
	ndxml *xml = ndxml_from_text((char *)nd_mem_share_addr(&fileMap));

	nd_mem_share_close(&fileMap);
	return xml;
}


bool LogicDebugClient::getRunningProcess(Process_vct_t &processes)
{

	return false;
}
//////////////////////////////////////////////////////////////////////////

LocalDebugger::LocalDebugger(LogicParserEngine *parser, LogicObjectBase *owner) :m_parser(parser)
{
	//m_client = NULL;
	//m_isAttached = false;
	m_bStep = false;
	m_bRunOut = false;
	m_encodeType = ND_ENCODE_TYPE;
	
	m_RunningSem = NULL;
	m_cliSem = NULL;
	memset(&m_outPutMem, 0, sizeof(m_outPutMem));

	ndxml_initroot(&m_parserInfo);
}

LocalDebugger::~LocalDebugger()
{
	if (m_RunningSem){
		postEndDebug();
	}
}

// debug function, return 0 success ,renturn -1 error , error== would_block , in step-debug
int LocalDebugger::runCmdline(int argc, const char *argv[], int encodeType )
{
	nd_assert(m_parser);

	ParserCmdLine *cmdLine = new ParserCmdLine(argc, argv);

	if (!preStartDebug(0)) {
		return -1;
	}

	ndthread_t thId = 0;
	ndth_handle h = nd_createthread(parseThreadRun, (void*)cmdLine, &thId, 0);
	if (h){
		return 0;
	}
	return -1;
}


int LocalDebugger::addBreakPoint(const char *funcName, const char *nodeName, bool isTemp)
{
	if (!isBreakPoint(funcName, nodeName))	{
		m_breakPoints.push_back(BreakPointInfo(funcName, nodeName, isTemp));
	}
	return 0;
}
int LocalDebugger::delBreakPoint(const char *funcName, const char *nodeName)
{
	for (breakPoint_vct::iterator it = m_breakPoints.begin(); it != m_breakPoints.end(); it++) {
		if (0 == ndstricmp(it->functionName.c_str(), funcName) &&
			0 == ndstricmp(it->nodeName.c_str(), nodeName))	{
			m_breakPoints.erase(it);
			break;
		}
	}
	return 0;
}


void LocalDebugger::clearBreakpoint()
{
	m_breakPoints.clear();
}

bool LocalDebugger::preStartDebug(NDUINT32 processId)
{
	processHeaderInfo * proceInfo = NULL;
	ShareProcessLists *pProcessRoot = ShareProcessLists::get_Instant();
	if (processId == 0)	{

		NDUINT32 myProcessId = nd_processid();
		proceInfo = pProcessRoot->createProcessInfo(myProcessId, nd_process_name());
	}
	else {
		proceInfo = pProcessRoot->getProcessInfo(processId);

	}
	if (!proceInfo)	{
		nd_logerror("create debug info for %d process error\n", processId);
		return false;
	}


	int ret = nd_mem_share_create(proceInfo->shareMemName, 1024 * 1024, &m_outPutMem);
	if (-1 == ret) {
		return false;
	}

	m_RunningSem = nd_sem_open(proceInfo->semName);
	if (!m_RunningSem) {
		return false;
	}

	m_cliSem = nd_sem_open(proceInfo->semClient);
	if (!m_cliSem) {
		return false;
	}
	return true;
}
bool LocalDebugger::postEndDebug()
{
	ShareProcessLists *root = ShareProcessLists::get_Instant();
	root->delProcess( nd_processid() ); 
	if (m_RunningSem) {
		nd_sem_destroy(m_RunningSem);
		m_RunningSem = 0;
	}

	if (m_cliSem) {
		nd_sem_destroy(m_cliSem);
		m_cliSem = 0;
	}

	if (nd_mem_share_addr(&m_outPutMem))	{
		nd_mem_share_close(&m_outPutMem);
	}
	return true;
}

//run on parser
int LocalDebugger::onEnterStep(const char *funcName, const char *nodeName)
{
	if (isBreakPoint(funcName, nodeName, true) || m_bStep){
		return waitEvent(funcName, nodeName);
	}
	else if (m_bRunOut && m_parser->checkLastStepIsCall()) {
		m_bRunOut = false;
		return waitEvent(funcName, nodeName);
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


int LocalDebugger::ntfClient(int cmd, const char *func, const char *nodeName)
{
	NDUINT32 processId = nd_processid();
	ShareProcessLists *processList = ShareProcessLists::get_Instant();
	processHeaderInfo * proceInfo = processList->getProcessInfo(processId);
	if (!proceInfo){
		return -1;
	}

	proceInfo->outputCmd = cmd;

	if (func && nodeName) {
		snprintf(proceInfo->cmdBuf, sizeof(proceInfo->cmdBuf), "<node func=\"%s\" exenode=\"%s\" />", func, nodeName);
	}
	nd_sem_post(m_cliSem);
	return 0;
}

int LocalDebugger::waitEvent(const char *runningFunc, const char *runningNode)
{
	int ret = 0;
	NDUINT32 processId = nd_processid();
	ShareProcessLists *processList = ShareProcessLists::get_Instant();

	processHeaderInfo * proceInfo = processList->getProcessInfo(processId);

	if (proceInfo->debuggerProcessId == 0){
		return 0;
	}

	std::string funcName, nodeName;

	ndxml_root &parserInfo = getParserInfo();

	void *outAddr = nd_mem_share_addr(&m_outPutMem);
	size_t outSize = (size_t)nd_mem_share_size(&m_outPutMem);
	ndxml_to_buf(&parserInfo,(char*)outAddr , outSize);

	if (-1 == ntfClient(E_DBG_OUTPUT_CMD_STEP, runningFunc, runningNode)) {
		nd_logerror("notify debugger client error\n");
		return 0;
	}
	
RE_WIAT:

	//m_client->onEnterStep(runningFunc, runningNode);
	ret = nd_sem_wait(m_RunningSem, INFINITE);
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

		ntfClient(E_DBG_OUTPUT_CMD_HANDLE_ACK, NULL,NULL );

		goto RE_WIAT;
		break;
	case E_DBG_INPUT_CMD_STEP_RUN:
		m_bStep = true;
		break;
	case E_DBG_INPUT_CMD_CONTINUE:
		m_bStep = false;
		break;
	case E_DBG_INPUT_CMD_RUN_TO_CURSOR:
		//m_parser->setStepMode(false);
		m_bStep = false;
		if (getParamFromInput(funcName, nodeName)) {
			addBreakPoint(funcName.c_str(), nodeName.c_str(),true);
		}
		break;
	case E_DBG_INPUT_CMD_RUN_OUT:
		m_bStep = false;
		m_bRunOut = true;
		break;
	case E_DBG_INPUT_CMD_DEL_BREAKPOINT:
		if (getParamFromInput(funcName, nodeName)) {
			delBreakPoint(funcName.c_str(), nodeName.c_str());
		}
		ntfClient(E_DBG_OUTPUT_CMD_HANDLE_ACK, NULL, NULL);
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
	makeParserInfo();
	return m_parserInfo;
}



bool LocalDebugger::OutPutInfo(ndxml *dataInfo)
{
	if (!nd_mem_share_addr(&m_outPutMem)) {
		return false;
	}
	size_t size = ndxml_to_buf(dataInfo, (char *)nd_mem_share_addr(&m_outPutMem), (int)nd_mem_share_size(&m_outPutMem));
	if (size == 0)	{
		return false;
	}
	return true;
}


bool LocalDebugger::getParamFromInput(std::string &funcName, std::string &nodeName)
{
	NDUINT32 processId = nd_processid();
	ShareProcessLists *processList = ShareProcessLists::get_Instant();
	processHeaderInfo * proceInfo =  processList->getProcessInfo(processId);

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

	for (int  i = 1; i < stack->params.size(); i++)	{
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

	
	len = m_parser->m_registerVal.Sprint(buf, sizeof(buf));
	ndxml_addnode(xmlInfo, "$curValue", len>0 ? buf : "0");


	for (int i = 0; i < stack->local_vars.size(); i++)	{
		len = stack->local_vars[i].var.Sprint(buf, sizeof(buf));
		ndxml_addnode(xmlInfo, stack->local_vars[i].name.c_str(), len>0 ? buf : "0");
	}
	return true;
}