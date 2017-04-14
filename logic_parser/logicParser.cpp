//
//  logicParser.cpp
//  gameHall
//
//  Created by duanxiuyun on 15-3-24.
//  Copyright (c) 2015 duanxiuyun. All rights reserved.
//

#include <stdarg.h>
#include <math.h>

#include "logic_parser/logicEndian.h"
#include "logic_parser/logicParser.h"
#include "logic_parser/dbl_mgr.h"
#include "logic_parser/logic_compile.h"
#include "logic_parser/logicEngineRoot.h"
#include "logic_parser/logicStruct.hpp"


// such as arr[varUser.arr[1]]  ,read varUser.arr[1]
static const char *ndstr_fetch_array_index(const char *input, char *output, size_t size)
{
	int markNum = 1;
	const char *p = (char*)strchr(input, _ARRAR_BEGIN_MARK);
	if (p && *p == _ARRAR_BEGIN_MARK){
		++p;
	}
	else {
		return NULL;
	}

	while (*p && size > 0) {

		if (*p == _ARRAR_BEGIN_MARK) {
			++markNum;
		}
		else if (*p == _ARRAR_END_MARK) {
			--markNum;
			if (markNum == 0)	{
				break;
			}
		}
		*output++ = *p++;
		--size;
	}
	*output++ = 0;
	if (*p == _ARRAR_END_MARK)	{
		++p;
	}
	return p;
}



//float LogicObjectBase::opCalc(void *func, int size)
//{
//	return 0 ;
//}
//////////////////////////////
//
//LogicParserEngine::script_func_map LogicParserEngine::s_func_table;

LogicParserEngine::LogicParserEngine() :m_registorFlag(false), m_simulate(false), m_owner(NULL), m_cmdByteOrder(0)
{
	m_curStack = NULL;
	m_ownerIsNew = false;

	m_dbg_cur_node_index = 0;
	m_dbg_node[0]= 0;
	m_dbg_fileInfo[0] = 0;

	for (int i = 0; i < LOGIC_ERR_NUMBER; i++)	{
		m_skipErrors[i] = 0;
	}
}

LogicParserEngine::LogicParserEngine(LogicObjectBase *owner) : m_registorFlag(false), m_simulate(false), m_owner(owner), m_cmdByteOrder(0)
{
	m_curStack = NULL;
	m_ownerIsNew = false;

	m_dbg_cur_node_index = 0;
	m_dbg_node[0] = 0;
	m_dbg_fileInfo[0] = 0;

	for (int i = 0; i < LOGIC_ERR_NUMBER; i++)	{
		m_skipErrors[i] = 0;
	}
}
LogicParserEngine::~LogicParserEngine()
{
	if (m_ownerIsNew && m_owner) {
		delete m_owner;
		m_owner = NULL;
	}
	UserDefData_map_t::iterator it;
	for (it = m_useDefType.begin(); it != m_useDefType.end(); it++)	{
		if (it->second)	{
			delete it->second;
			it->second = 0;
		}
	}	
	m_useDefType.clear();
}


int LogicParserEngine::runCmdline(int argc, const char *argv[], int encodeType )
{
	parse_arg_list_t params;
	for (int i = 0; i < argc; i++)	{
		params.push_back(DBLDataNode(argv[i]));
	}

	DBLDataNode result;

	if (runScript(argv[0], params, result, encodeType)) {
		return 0;
	}
	return getErrno();
}

bool LogicParserEngine::runScript(int encodeType ,const char *scriptName,DBLDataNode &result, int param_num, ...)
{
	DBLDataNode fname;
	parse_arg_list_t params;
	const char *argName = scriptName;
	
	fname.InitSet(argName);
	params.push_back(fname);
	
	va_list arg;
	int num = param_num;
	
	va_start(arg, param_num);
	while (num-- > 0) {
		DBLDataNode data1;
		data1.InitSet(va_arg(arg, const char*));
		params.push_back(data1);
	}
	va_end(arg);
	
	return runScript(scriptName, params, result, encodeType);
}
bool LogicParserEngine::runScript(const char *scriptName, parse_arg_list_t &args, DBLDataNode &result, int encodeType)
{
	LogicEngineRoot *pRoot = LogicEngineRoot::get_Instant() ;
	nd_assert(pRoot) ;
	
	const char *pFuncname = pRoot->_convertFuncName(scriptName, encodeType);
	const char *moduleName = NULL;
	const scriptCmdBuf *pcmdBuf = pRoot->getScript(pFuncname, NULL, &moduleName);
	if (pcmdBuf) {
		setErrno(LOGIC_ERR_SUCCESS) ;
		if (true == _runCmdBuf(moduleName,pcmdBuf, args)) {
			result = getValue() ;
			return  true ;
		}
		nd_logerror("run %s script error %d: %s\n", scriptName, getErrno(),nd_error_desc(getErrno()));
		
	}
	else {
		setErrno(LOGIC_ERR_FUNCTION_NOT_FOUND);
		nd_logerror("script engine not fount %s script-file\n", scriptName);
	}
	return false;
}

bool LogicParserEngine::eventNtf(int event_id, int num, ...)
{
	parse_arg_list_t params;
	va_list arg;
	va_start(arg, num);
	while (num-- > 0) {
		DBLDataNode data1;
		data1.InitSet(va_arg(arg, const char*));
		params.push_back(data1);
	}
	va_end(arg);
	
	
	return eventNtf(event_id, params);
}

bool LogicParserEngine::eventNtf(int event_id, parse_arg_list_t &args)
{
	LogicEngineRoot *pRoot = LogicEngineRoot::get_Instant() ;
	nd_assert(pRoot) ;

	DBLDataNode result;
	onEvent(event_id, args);
	
	DBLDataNode fname;
	parse_arg_list_t params;
	const char *script_name = pRoot->getEventFunc(event_id);
	if (!script_name || !*script_name){
		//nd_logwarn("event %d function not registered \n", event_id);
		setErrno(LOGIC_ERR_FUNCTION_NOT_FOUND);
		return false;
	}
	
	fname.InitSet(script_name);
	params.push_back(fname);
	
	parse_arg_list_t::iterator it = args.begin();
	for (; it != args.end(); it++) {
		params.push_back(*it);
	}
	
	return runScript(script_name, params, result);
}

LogicObjectBase *LogicParserEngine::getLogicObjectMgr(const char *objname)
{
	if (m_owner){
		return m_owner->getObjectMgr(objname);
	}
	return NULL;
}

bool LogicParserEngine::_runCmdBuf(const char *moduleName ,const scriptCmdBuf *buf, parse_arg_list_t &params)
{
	int ret = -1;
	runningStack stack;

	Reset();
	stack.params = params;
	
	stack.cur_point = buf->buf;
	stack.cmd = buf;
	if (moduleName && *moduleName){
		stack.curModuleName = moduleName;
	}
	else {
		stack.curModuleName.clear();
	}

	runningStack *orgStack = m_curStack;
	
	ret = _runCmd(&stack);

	int funcError = m_sys_errno;
	handleScriptGenEvent();
	m_sys_errno = funcError;

	if (m_simulate == false)	{		
		if (stack.affairHelper)	{
			if (ret != 0 || !m_registorFlag ) {
				stack.affairHelper->Rollback();
			}
			delete stack.affairHelper;
			stack.affairHelper = NULL;
		}		
	}
	else {		
		if (stack.affairHelper)	{
			delete stack.affairHelper;
			stack.affairHelper = NULL;
		}
	}
	m_curStack = orgStack;
	return ret == 0;
}
bool LogicParserEngine::_runCmdBuf(const char *moduleName,const scriptCmdBuf *buf, int param_num, ...)
{
	DBLDataNode fname;
	parse_arg_list_t params;

	fname.InitSet(buf->cmdname);
	params.push_back(fname);

	va_list arg;
	int num = param_num ;
	
	va_start (arg, param_num);
	while (num-- > 0 ) {
		DBLDataNode data1;
		data1.InitSet(va_arg(arg, const char*));
		params.push_back(data1);
	}
	va_end (arg);
	
	return _runCmdBuf(moduleName, buf, params);
}


const char *LogicParserEngine::getCurMoudle()
{
	if (m_curStack && m_curStack->curModuleName.size() ) {
		return  m_curStack->curModuleName.c_str() ;
	}
	return  NULL;
}

bool LogicParserEngine::getFuncHeader(const scriptCmdBuf *cmd, char *buf, size_t size)
{
	const char *p = cmd->buf;
	NDUINT32 opCmd = *((*(NDUINT32**)&p)++);
	if (opCmd != E_OP_HEADER_INFO){
		return false;
	}
	NDUINT16 len = *((*(NDUINT16**)&p)++); //string type
	len = *((*(NDUINT16**)&p)++); //string len
	if (len >= size){
		len = (NDUINT16) (size -1);
	}
	memcpy(buf, p, len);
	return true;
}
bool LogicParserEngine::runFunction(const char *moduleName ,const scriptCmdBuf *buf, parse_arg_list_t &args, DBLDataNode &result)
{
	bool ret = false;
	runningStack stack;
	ResetStep();
	runningStack *beforeCall = m_curStack;
	stack.params = args;
	stack.cur_point = buf->buf;
	stack.cmd = ( scriptCmdBuf *) buf;
	stack.curModuleName = moduleName;

	//fetchModuleName(buf->cmdname, stack.curModuleName) ;

	//bool orgExitFlag = m_OnErrorExit;
	int orgCount = m_registerCount;

	if (_runCmd(&stack) != -1) {
		ret = true;
		result = m_registerVal;


	}
	if (stack.affairHelper)	{
		if (ret ==false ) {
			stack.affairHelper->Rollback();
		}
		delete stack.affairHelper;
		stack.affairHelper = NULL;
	}

	//m_OnErrorExit = orgExitFlag;
	m_curStack = beforeCall;
	m_registerCount = orgCount ;
	return ret ;
}


int LogicParserEngine::_runCmd(runningStack *stack)
{
	int readlen = 0;
	char *p = (char *) stack->cur_point ;
	NDUINT32 opCmd , opAim = 0 ;
	bool isdebug_step = false;
	bool inException = false;
	NDUINT32 cur_step_size = -1;
	char *step_start = 0 ;
	const char *exception_addr = stack->exception_addr;
	
	operator_value_t val ;
	operator_index_t index ;
	NDUINT32 num ;
	NDUINT32 errorBeforeException = 0;
	
	LogicObjectBase* objAim = NULL ;
	DBLDataNode tmpInputVal, tmpIndexVal;


	m_curStack = stack; 
	m_cmdByteOrder = stack->cmd->byteOrder;
	m_sys_errno = 0;

	for (size_t i = 0; i < LOGIC_ERR_NUMBER; i++){
		if (m_curStack->skipErrors[i] != 0) {
			m_skipErrors[i] = m_curStack->skipErrors[i];
		}
	}

#define CHECK_INSTRUCTION_OVER_FLOW() \
	if (p < stack->cmd->buf || p >(stack->cmd->buf + stack->cmd->size)) {	\
		m_sys_errno = LOGIC_ERR_INPUT_INSTRUCT;	break;		\
			}

#define SHORT_JUMP(_p, _offset) \
	do { \
	if (_offset >= 0) (_p) += (_offset);	\
				else (_p) -= (size_t)(-(_offset));		\
			} while (0)

#define GET_VAR_FROM_STREAM(_stack, _pstream, _val) \
	do { \
		int _len = _getValue(_stack, _pstream, _val);	\
		if (_len <= 0){								\
			m_sys_errno = LOGIC_ERR_INPUT_INSTRUCT;		\
			break;										\
						}				\
		_pstream += _len;	\
			}while(0)

#define GET_TEXT_FROM_STREAM(_buf, _size, _pstream) \
	do {	\
		int _len = _read_string(_pstream, _buf,_size);	\
		if (_len <= 0){	\
			m_sys_errno = LOGIC_ERR_INPUT_INSTRUCT;	\
			break;		\
				}				\
		_pstream += _len ;\
		CHECK_INSTRUCTION_OVER_FLOW();	\
		} while (0)


#define BEGIN_GET_FUNC_ARGS(_stack, _pstream) \
	_pstream = lp_read_stream(_pstream, num,m_cmdByteOrder);\
	if (num > 0) {							\
		DBLDataNode tmval1;					\
		parse_arg_list_t args;				\
		for (int i = 0; i < num; ++i) {		\
			readlen = _getValue(_stack, _pstream, tmval1);	\
			if (readlen <= 0){								\
				break;						\
						}								\
			_pstream += readlen;			\
			args.push_back(tmval1);			\
			CHECK_INSTRUCTION_OVER_FLOW();	\
								}									\
		if (m_sys_errno != LOGIC_ERR_INPUT_INSTRUCT) {	\

#define END_GET_FUNC_ARGS() \
				}\
		}

#define OBJ_OP_FUNC(_opfunc) do {\
		GET_TEXT_FROM_STREAM(name, sizeof(name), p);	\
		GET_VAR_FROM_STREAM(stack, p, tmpIndexVal);		\
		GET_VAR_FROM_STREAM(stack, p, tmpInputVal);		\
		objAim = getLogicObjectMgr(name);				\
		if (objAim) {									\
			m_registorFlag = objAim->_opfunc(tmpIndexVal, tmpInputVal);	\
			if(!m_registorFlag) {						\
				m_sys_errno = objAim->getError();		\
				nd_logdebug(#_opfunc" run on %s error code =%d\n",name, m_sys_errno ) ; \
			}			\
		}				\
		else {			\
			m_sys_errno = LOGIC_ERR_AIM_OBJECT_NOT_FOUND;	\
		}					\
	}while(0)


	while (p < (stack->cmd->buf + stack->cmd->size) ) {
		char name[128];
		name[0] = 0;

		isdebug_step = false;
		ResetStep() ;
		//opCmd = *((*(NDUINT32**)&p)++) ;
		p = lp_read_stream(p, opCmd, m_cmdByteOrder);
		
		switch (opCmd) {
			case E_OP_EXIT:
				//index = *((*(operator_index_t**)&p)++);
				p = lp_read_stream(p, (NDUINT32&)index, m_cmdByteOrder);
				m_registorFlag= (index==0) ? true: false ;
				p = (char*) (stack->cmd->buf + stack->cmd->size);
				m_sys_errno = index;
				break ;
			case E_OP_EXCEPTION_BLOCK:
			{
				NDUINT16 len;
				p = lp_read_stream(p, len, m_cmdByteOrder);

				if (0 == exception_addr) {
					exception_addr = p;
					stack->exception_addr = exception_addr ;
				}
				p += len;
				m_registorFlag = true;
			}
				break;
			case E_OP_THROW:
				p = lp_read_stream(p, (NDUINT32&)index, m_cmdByteOrder);
				if (!inException) {
					m_registorFlag = false;
					m_sys_errno = index + LOGIC_ERR_USER_DEFINE_ERROR;
				}
				break;
			case  E_OP_CHECK_IS_SIMULATE:
				m_registorCtrl = m_simulate;
				m_registorFlag = true;
				break;
			case E_OP_GET_LAST_ERROR:
				if (inException) {
					m_registerVal.InitSet((int)errorBeforeException);
				}
				else {
					m_registerVal.InitSet(m_sys_errno);
				}
				m_registorFlag = true;
				break;
			case E_OP_DEBUG_INFO:
				isdebug_step = true;
				p = lp_read_stream(p, m_dbg_cur_node_index, m_cmdByteOrder);
				GET_TEXT_FROM_STREAM(m_dbg_node, sizeof(m_dbg_node),p);
				
				p = lp_read_stream(p, cur_step_size, m_cmdByteOrder);
				step_start = p;
				CHECK_INSTRUCTION_OVER_FLOW();
				apollo_script_printf("logic_script %s: %s %d\n", stack->cmd->cmdname, m_dbg_node, m_dbg_cur_node_index);				
				break;
			case E_OP_FILE_INFO:
				GET_TEXT_FROM_STREAM(m_dbg_fileInfo, sizeof(m_dbg_fileInfo), p);
				CHECK_INSTRUCTION_OVER_FLOW();
				apollo_script_printf("logic_script %s:read file info :\n", stack->cmd->cmdname, m_dbg_fileInfo);
				break; 
			case E_OP_HEADER_INFO:
			{
				NDUINT16 a;
				p = lp_read_stream(p, a, m_cmdByteOrder);
				opAim = a;
				p = lp_read_stream(p, a, m_cmdByteOrder);
				readlen = a;

				p += readlen;
				CHECK_INSTRUCTION_OVER_FLOW();
				m_registorFlag = true;
				break;
			}
			case E_OP_ON_ERROR_CONTINUE:
				p = lp_read_stream(p, (NDUINT32&)val, m_cmdByteOrder);
				m_curStack->error_continue = val ? true : false;
				m_registorFlag = true;
				break;
			case E_OP_COMMIT_AFFAIR:
				_commitAffair();
				m_registorFlag = true;
				break;
			case E_OP_BEGIN_AFFAIR:
				m_registorFlag = _beginHelper();
				break;
			
			case E_OP_RAND:
				p = lp_read_stream(p, opAim, m_cmdByteOrder);
				p = lp_read_stream(p, (NDUINT32&)index, m_cmdByteOrder);

				val = (operator_value_t)logic_rand(opAim, index);
				m_registorFlag = true;
				m_registerVal.InitSet((int)val);
				break;
			case E_OP_SKIP_ERROR:
				p = lp_read_stream(p, opAim, m_cmdByteOrder);
				_pushSKipError(opAim);
				m_registorFlag = true;
				break;

			case E_OP_READ :			//	读 eOperatorDest + operator_index_t
				GET_TEXT_FROM_STREAM(name, sizeof(name), p);
				GET_VAR_FROM_STREAM(stack, p, tmpIndexVal);
				objAim = getLogicObjectMgr(name);
				if (objAim) {
					m_registorFlag = objAim->opRead(tmpIndexVal, m_registerVal);
					if (!m_registorFlag) {
						m_sys_errno = objAim->getError();
					}
				}
				else {
					m_sys_errno = LOGIC_ERR_AIM_OBJECT_NOT_FOUND;
				}
				break ;
			case E_OP_WRITE:			//	写 eOperatorDest + operator_index_t + operator_value_t
				OBJ_OP_FUNC(opWrite);
				break ;

			case E_OP_ADD :			//	增加 eOperatorDest + operator_index_t + operator_value_t + int
				OBJ_OP_FUNC(opAdd);
				break ;
			case E_OP_SUB :			//	减少 eOperatorDest + operator_index_t + operator_value_t + int
				OBJ_OP_FUNC(opSub);
				break ;
			//case E_OP_CLEAR:			//  清除 eOperatorDest + operator_index_t	
			//	OBJ_OP_FUNC(opClear);
				break ;
			case E_OP_TEST:
				OBJ_OP_FUNC(opCheck);
				break;

			case E_OP_OPERATE:
			{
				char cmd_buf[128];
				GET_TEXT_FROM_STREAM(name, sizeof(name), p);
				GET_TEXT_FROM_STREAM(cmd_buf, sizeof(cmd_buf), p);
				GET_VAR_FROM_STREAM(stack, p, tmpIndexVal);
				GET_VAR_FROM_STREAM(stack, p, tmpInputVal);
				objAim = getLogicObjectMgr(name);

				if (objAim) {
					m_registorFlag = objAim->opOperate(cmd_buf, tmpIndexVal, tmpInputVal);
					if (!m_registorFlag) {
						m_sys_errno = objAim->getError();
					}
					else {
						m_registerVal = tmpInputVal;
					}
				}
				else {
					m_sys_errno = LOGIC_ERR_AIM_OBJECT_NOT_FOUND;
				}

				break;
			}
				
 			case  E_OP_GET_OTHER_OBJECT:				
				GET_TEXT_FROM_STREAM(name, sizeof(name), p);
				if (m_owner){
					m_registorFlag = m_owner->getOtherObject(name,m_registerVal);
					if (!m_registorFlag){
						m_sys_errno = LOGIC_ERR_AIM_OBJECT_NOT_FOUND;
					}
				}
 				else {
 					m_sys_errno = LOGIC_ERR_AIM_OBJECT_NOT_FOUND;
 				}
 				break;

			case E_OP_CHANGE_DIR: //change current working direct  format: string:new_direct
				GET_TEXT_FROM_STREAM(name, sizeof(name), p);
				m_registorFlag = _chdir(name);
				if (!m_registorFlag) {
					m_sys_errno = LOGIC_ERR_FILE_NOT_EXIST;
				}
				break;
			case E_OP_REMOVE_FILE: //remove file or direct format :string:path_name
				GET_TEXT_FROM_STREAM(name, sizeof(name), p);
				m_registorFlag = _rmfile(name);
				if (!m_registorFlag) {
					m_sys_errno = LOGIC_ERR_FILE_NOT_EXIST;
				}
				break;
			case E_OP_MAKE_DIR:		//create direct : foramt : string 
				GET_TEXT_FROM_STREAM(name, sizeof(name), p);
				m_registorFlag = _mkdir(name);
				if (!m_registorFlag) {
					m_sys_errno = LOGIC_ERR_FILE_NOT_EXIST;
				}
				break;
			case E_OP_DEL_TIMER:
			{
				//opAim = *((*(NDUINT32**)&p)++);
				p = lp_read_stream(p, opAim, m_cmdByteOrder);

				readlen = _read_string(p, name, sizeof(name));
				if (readlen <= 0){
					m_sys_errno = LOGIC_ERR_INPUT_INSTRUCT;
					break;
				}

				p += readlen;
				m_registorFlag = _delTimer(opAim!=0 , name);
			}
				break;
			case E_OP_EVENT_REMOVE:
			case E_OP_EVENT_INSTALL:
			{
				//index = *((*(operator_index_t**)&p)++);
				p = lp_read_stream(p, (NDUINT32&)index, m_cmdByteOrder);

				int readlne = _read_string(p, name, sizeof(name ));
				if (readlne <= 0){
					m_sys_errno = LOGIC_ERR_INPUT_INSTRUCT;
					break;
				}
				p += readlne;
				if (index && name[0])	{
					if (opCmd == E_OP_EVENT_REMOVE)	{
						_removeHandler(index, name);
					}
					else {
						_installHandler(index, name);
					}
				}
				m_registorFlag = true;
			}
				break;

			case E_OP_SET_COUNT_REG:
				readlen = _getValue(stack, p, tmpInputVal);
				if (readlen > 0)	{
					p += readlen;
					m_registerCount = tmpInputVal.GetInt();
					m_registorFlag = true;
				}
				else {
					m_sys_errno = LOGIC_ERR_INPUT_INSTRUCT;
				}
				break;
			
			case E_OP_SET_LOOP_INDEX:
				p = lp_read_stream(p, (NDUINT32&)index, m_cmdByteOrder);
				m_registorFlag =true;
				m_curStack->loopIndex = index;
				break;

			case E_OP_GET_ARRAY_SIZE:
				readlen = _getValue(stack, p, tmpInputVal);
				if (readlen > 0)	{
					p += readlen;
					m_registorFlag = true;
					m_registerVal.InitSet(tmpInputVal.GetArraySize());
				}
				else {
					m_sys_errno = LOGIC_ERR_INPUT_INSTRUCT;
				}
				break;
				
			case E_OP_MAKE_VAR:		//  make local variant	 name + DBLDataNode-stream
				readlen = _makeVar(stack, p);
				if (readlen > 0){
					p += readlen;
					m_registorFlag = true;
				}
				else {
					m_sys_errno = LOGIC_ERR_AIM_OBJECT_NOT_FOUND;
				}
				break ;
			case E_OP_MATH_OPERATE:
				p = lp_read_stream(p, opAim, m_cmdByteOrder);
				
				GET_VAR_FROM_STREAM(stack, p, tmpIndexVal);
				GET_VAR_FROM_STREAM(stack, p, tmpInputVal);
				//m_registerVal.InitSet(tmpIndexVal.)
				m_registorFlag = _mathOperate((eMathOperate)opAim,tmpIndexVal,tmpInputVal) ;
				break ;
			case E_OP_ASSIGNIN:
				
				GET_TEXT_FROM_STREAM(name, sizeof(name), p);
				GET_VAR_FROM_STREAM(stack, p, tmpInputVal);
				m_registorFlag = _varAssignin(stack,name, tmpInputVal );
				break ;
				
			case E_OP_DATATYPE_TRANSFER:
				//opAim = *((*(NDUINT32**)&p)++);
				name[0] = 0;
				GET_TEXT_FROM_STREAM(name, sizeof(name), p);
				p = lp_read_stream(p, opAim, m_cmdByteOrder);
				if (name[0]) {
					DBLDataNode*pData = _getLocalVar(stack, name);
					if (pData) {
						m_registorFlag = pData->TransferType((DBL_ELEMENT_TYPE)opAim);
					}
				}
				if (!m_registorFlag)	{
					nd_logerror("datatype transfer error : read type or name from stream error\n");
					m_sys_errno = LOGIC_ERR_INPUT_INSTRUCT;
				}
				break;
			case E_OP_MAKE_USER_DATA:
				BEGIN_GET_FUNC_ARGS(stack, p);
				m_registorFlag =  _buildUserDefData(stack,args);
				END_GET_FUNC_ARGS();
				
				break;
			case E_OP_CREATE_TYPE:
				BEGIN_GET_FUNC_ARGS(stack, p);
				m_registorFlag = _CreateUserDefType(stack, args);
				END_GET_FUNC_ARGS();
				break;
				
			case E_OP_READ_TABLE: 	//	read game-static-data  string(tablename) + id + col-name
				readlen = _readGameDataTable(stack,p, m_registerVal);
				if (readlen>0){
					p += readlen;
					m_registorFlag = true;
// 					if (m_registerVal.CheckValid())	{						
// 					}
// 					else {
// 						m_sys_errno = LOGIC_ERR_AIM_OBJECT_NOT_FOUND;
// 					}
				}
				else {
					m_sys_errno = LOGIC_ERR_INPUT_INSTRUCT;
				}
				break ;
			case E_OP_COMP:	 		//	比较 eOperatorDest + eParserCompare  + operator_value_t				
				//opAim = *((*(NDUINT32**)&p)++) ;
				p = lp_read_stream(p, opAim, m_cmdByteOrder);

				GET_VAR_FROM_STREAM(stack, p, tmpIndexVal);
				GET_VAR_FROM_STREAM(stack, p, tmpInputVal);

				m_registorCtrl = _opCmp(tmpIndexVal, tmpInputVal, (eParserCompare)opAim);
				m_registorFlag = true;

				break ;
			case E_OP_TEST_LAST_VAL:
				if (m_registerVal.CheckValid())	{
					m_registorCtrl = m_registerVal.GetBool();
				}
				else {
					m_registorCtrl = false;
				}
				m_registorFlag = true;
				break;				
			case E_OP_CALC:	 		//	公式计算 NDUINT32 + BUF
			{
				float result = 0;
				char formula_text[1024];
				readlen = _read_string(p, formula_text, sizeof(formula_text));
				if (readlen > 0)	{
					p += readlen;
					m_registorFlag = (0 == _runFormula(formula_text, &result));
					if (m_registorFlag)	{
						m_registerVal.InitSet(result);
					}
					else {
						m_sys_errno = LOGIC_ERR_INPUT_INSTRUCT;
					}
				}

			}
				break ;							
			
			case E_OP_GET_TIME:
			{
				time_t now = app_inst_time(NULL);
				m_registerVal.InitSet(now);
				m_registorFlag = true;
			}
				break;
			case E_OP_GET_COMPILE_TM:
			{
				LogicEngineRoot *root = LogicEngineRoot::get_Instant();
				if (root){
					root->getCompileTime(m_registerVal);
					m_registorFlag = true;
				}
				else {
					m_sys_errno = LOGIC_ERR_NOT_INIT;
				}
			}
				break;
			
			case E_OP_ADD_TIMER:
				BEGIN_GET_FUNC_ARGS(stack, p);
				m_registorFlag =  _addTimer(args);
				END_GET_FUNC_ARGS();
				break;

			case E_OP_SEND_EVENT:
				BEGIN_GET_FUNC_ARGS(stack, p);
				int eventID = args[0].GetInt();
				args.erase(args.begin());
				m_registorFlag = SendEvent(eventID, args);
				END_GET_FUNC_ARGS();
				break;
			case E_OP_WAIT_EVENT:			//  等待事件 int + operator_value_t[3]
				BEGIN_GET_FUNC_ARGS(stack, p);
				if (!inException){
					stack->cur_point = p;
					m_sys_errno = LOGIC_ERR_WOULD_BLOCK;
					int eventID = args[0].GetInt();
					args.erase(args.begin());
					m_registorFlag = waitEvent(stack, eventID, args);
					return 0;
				}
				END_GET_FUNC_ARGS();
				break;
			case E_OP_OUT_PUT:
				BEGIN_GET_FUNC_ARGS(stack, p);
				apollo_printf(this, args, tmpInputVal);
				m_registorFlag = true;
				END_GET_FUNC_ARGS();
				break;
			case E_OP_LOG:
				BEGIN_GET_FUNC_ARGS(stack, p);
				apollo_log(this, args, tmpInputVal);
				m_registorFlag = true;
				END_GET_FUNC_ARGS();
				break;
			case E_OP_TIME_CMD:
				BEGIN_GET_FUNC_ARGS(stack, p);
				args.insert(args.begin(), DBLDataNode("time_func"));
				m_registorFlag = apollo_time_func(this, args, m_registerVal);
				if (!m_registorFlag)	{
					m_sys_errno = LOGIC_ERR_INPUT_INSTRUCT;
				}
				END_GET_FUNC_ARGS();
				break;

			case E_OP_CALL_FUNC:			//  调用外部函数 string(function_name) + string1 + string2	
				BEGIN_GET_FUNC_ARGS(stack, p);
				m_registorFlag = _callFunction(args);
				END_GET_FUNC_ARGS();
				break;

			case E_OP_SHORT_JUMP:
			{
				//int offset = *((*(NDUINT32**)&p)++);
				int offset;
				p = lp_read_stream(p, offset, m_cmdByteOrder);
				
				//nd_logmsg("begin short jump offset=%d\n", offset) ;
				SHORT_JUMP(p, offset);
				CHECK_INSTRUCTION_OVER_FLOW();
				m_registorFlag = true;
			}
				break ;
			case  E_OP_LONG_JUMP:
			{
				//NDUINT32 offset = *((*(NDUINT32**)&p)++);
				NDUINT32 offset;
				p = lp_read_stream(p, offset, m_cmdByteOrder);

				p = (char*)stack->cmd->buf + offset;

				CHECK_INSTRUCTION_OVER_FLOW();
				m_registorFlag = true;
				break;
			}
			case E_OP_TEST_SUCCESS_SHORT_JUMP:
			{
				//int offset = *((*(int**)&p)++);
				int offset;
				p = lp_read_stream(p, offset, m_cmdByteOrder);

				if (m_registorCtrl == true)	{
					//p += offset;
					SHORT_JUMP(p, offset);
					CHECK_INSTRUCTION_OVER_FLOW();
				}
				m_registorFlag = true;

			}
			break;
			case E_OP_TEST_FALSE_SHORT_JUMP:
			{
				//int offset = *((*(int**)&p)++);
				int offset;
				p = lp_read_stream(p, offset, m_cmdByteOrder);

				if (m_registorCtrl==false)	{
					//p += offset;
					SHORT_JUMP(p, offset);
					CHECK_INSTRUCTION_OVER_FLOW();
				}
				m_registorFlag = true;

			}
			break;
			case E_OP_TEST_FALSE_LONG_JUMP:
			{
				//NDUINT32 offset = *((*(NDUINT32**)&p)++);
				NDUINT32 offset;
				p = lp_read_stream(p, offset, m_cmdByteOrder);
				if (m_registorCtrl==false)	{
					p = (char*)stack->cmd->buf + offset;

					CHECK_INSTRUCTION_OVER_FLOW();
				}
				m_registorFlag = true;

			}
			break;

			case E_OP_TEST_COUNT_JUMP:
			{
				//int offset = *((*(int**)&p)++);
				int offset;
				p = lp_read_stream(p, offset, m_cmdByteOrder);

				if (m_registerCount-- > 1)	{//this block only jump ahead. so count end at 1
					SHORT_JUMP(p, offset);
					//p += offset;
				}
				CHECK_INSTRUCTION_OVER_FLOW();
				m_registorFlag = true;
				++m_curStack->loopIndex;
			}
				break;

			default:
				m_registorFlag = false;
				m_sys_errno = LOGIC_ERR_INPUT_INSTRUCT;
				stack->cur_point = p ;
				break ;
		}
		stack->cur_point = p ;

		//check step size 
		if (isdebug_step == false){
			if (cur_step_size != -1 && step_start)	{
				NDUINT32 ss = (NDUINT32)(p - step_start);
				if (cur_step_size != ss) {
					apollo_script_printf("%s :run step errror %s node-index=%d compile-size=%d, parser-size=%d",
						stack->cmd->cmdname, m_dbg_node, m_dbg_cur_node_index, cur_step_size, ss);
				}
				else {
					apollo_script_printf("%s :cmd %d step %s node-index=%d size=%d %s\n",
						stack->cmd->cmdname, opCmd, m_dbg_node, m_dbg_cur_node_index, cur_step_size,
						m_registorFlag?"success":"failed");
				}
			}
			cur_step_size = -1;
			step_start = 0;
		}

		
		if (m_registorFlag==false ) {
			if (m_simulate && _checkIsSkip(m_sys_errno)) {
				continue;
			}
			if (inException == true){
				//on error when exception-handle
				m_sys_errno = errorBeforeException;
				return -1;
			}
			else if (!_checkIsSystemError(m_sys_errno) && m_curStack->error_continue) {				
				continue;
			}
			else if (exception_addr)	{
				//jump to exception 
				p = (char*)exception_addr;
				inException = true;
				errorBeforeException = m_sys_errno;
				m_registorFlag = true;
				nd_logerror("run %s script error : %s \n", stack->cmd->cmdname, nd_error_desc(m_sys_errno)) ;
			}
			else {
				return -1;
			}
			
		}
	}
	
	//exit
	if (inException == true){
		m_sys_errno = errorBeforeException;
		return -1;
	}
	return 0;
	
}

int LogicParserEngine::_makeVar(runningStack *runstack, char *pCmdStream)//make variant from instruction 
{
	char*p = pCmdStream;
	char name[128];
	int len = _read_string(p, name, sizeof(name));
	if (len <= 0){

		m_sys_errno = LOGIC_ERR_PARSE_STRING;
		return -1;
	}
	p += len;

	logcal_variant node;	

	len = _getValue(runstack, p, node.var);
	if (len == -1){
		m_sys_errno = LOGIC_ERR_READ_STREAM;
		return -1;
	}
	p += len;

	node.name = name;
	runstack->local_vars.push_back(node);

	return (int)(p - pCmdStream);
}

bool LogicParserEngine::_getArg(runningStack *stack, int index, DBLDataNode &outValue)
{
	if (index < stack->params.size()){
		outValue = stack->params[index];
		return true;
	}

	else if (m_simulate)	{
		DBLDataNode data;
		data.InitSet("0");
		outValue = data;
		return true;
	}

	return false;
}

bool LogicParserEngine::_getVarValue(runningStack *stack, const char *varname, DBLDataNode &outValue)
{
	if (ndstr_is_naturalnumber(varname)){
		outValue.InitSet(atoi(varname));
		return true;
	}
	else if (ndstr_is_numerals(varname)){
		outValue.InitSet((float)atof(varname));
		return true;
	}
	return _getVarEx(stack, varname, outValue);
}

DBLDataNode* LogicParserEngine::_getLocalVar(runningStack *stack, const char *varname)
{
	if (0 == ndstricmp(varname, "$CurValue") || 0 == ndstricmp(varname, "$LastValue") 
		|| 0 == ndstricmp(varname, "$?")
		|| 0 == ndstricmp(varname, "$value") ){
		return &m_registerVal ;
	}
	else if (0 == ndstricmp(varname, "$index")) {
		m_loopIndex.InitSet(stack->loopIndex);
		return &m_loopIndex;
	}
	else if (*varname == '$' && (varname[1] >= '0' && varname[1] <= '9') ) {
		const char *p = varname + 1;
		int index = atoi(p);
		if (index>0 && index <stack->params.size())	{
			return &stack->params[index];
		}
		return NULL;
	}
	for (LogicData_vct::iterator it = stack->local_vars.begin(); it != stack->local_vars.end(); it++)	{
		if (ndstricmp((char*)it->name.c_str(), (char*)varname) == 0) {
			return &it->var;
		}
	}
	return NULL;
}


bool LogicParserEngine::_getVarEx(runningStack *stack, const char *inputVarName, DBLDataNode &outVar)
{
	char name[128];
	char subName[128];
	const char *p = inputVarName;
	
	p = ndstr_parse_word(p, name);

	DBLDataNode* pdata = _getLocalVar(stack, name);
	if (!pdata)	{
		nd_logerror("var %s not exist\n", name);
		return false;
	}
	//DBLDataNode value = *pdata;
	DBLDataNode subVar;
	
	while (p && *p){		
		if (*p == _ARRAR_BEGIN_MARK) {
			p = ndstr_fetch_array_index(p, subName, sizeof(subName));
			if (!p)	{
				nd_logerror("get %s var error\n", inputVarName);
				return false;
			}
			DBLDataNode subIndex;
			if (_getVarValue(stack, subName, subIndex)) {
				int index = subIndex.GetInt();

				subVar = pdata->GetArray(index);
				if (!subVar.CheckValid()) {
					return false;
				}
			}
			else {
				return false;
			}
		}
		else if (*p == '.') {
			++p;
			p = ndstr_parse_word(p, subName);

			//subVar = pdata->unsafeRefMember(subName);
			subVar = pdata->getUserDefMember(subName);
			if (!subVar.CheckValid()) {
				return false;
			}
		}
		else {
			break;
		}		
		pdata = &subVar;
	}

	outVar = *pdata;
	return true;
}


bool LogicParserEngine::_chdir(const char *curDir)
{
	if (m_curStack->workingPath.empty()) {
		m_curStack->workingPath = nd_getcwd();
	}
	return 0==nd_chdir(curDir);
}

bool LogicParserEngine::_rmfile(const char *filename)
{
	if (nd_existfile(filename)) {
		return 0 == nd_rmfile(filename);
	}
	return 0 == nd_rmdir(filename);
}

bool LogicParserEngine::_mkdir(const char *curDir)
{
	return 0 == nd_mkdir(curDir);
}


int LogicParserEngine::_getValue(runningStack *stack, char *pCmdStream, DBLDataNode &outValue)
{
	NDUINT16 type;
	char*p = lp_read_stream(pCmdStream, type, m_cmdByteOrder);

	if (OT_VARIABLE == (DBL_ELEMENT_TYPE)type){
		//get data from local vars 
		char name[1024];
		int len = _read_string(p, name, sizeof(name));
		if (len == -1){
			m_sys_errno = LOGIC_ERR_INPUT_INSTRUCT;
			nd_logerror("read var name error \n");
			return -1;
		}
		p += len;
		if (!_getVarEx(stack, name, outValue))	{
			nd_logerror(" var %s not found\n", name);
			m_sys_errno = LOGIC_ERR_VARIANT_NOT_EXIST;
		}
		
		return (int)(p - pCmdStream);
		
	}
	else if (OT_PARAM == (DBL_ELEMENT_TYPE)type) {
		//int index = *((*(int**)&p)++);
		NDUINT32 index;
		p = lp_read_stream(p, index, m_cmdByteOrder);

		if (_getArg(stack, index, outValue) ) {
			return (int)(p - pCmdStream);
		}
		else {
			m_sys_errno = LOGIC_ERR_PARAM_NOT_EXIST;
			nd_logerror("get param(%d) error max_index=%d\n", index, stack->params.size());
			return -1;
		}
	}
	else if (OT_LAST_RET == (DBL_ELEMENT_TYPE)type) {
		outValue = m_registerVal;
		return (int)(p - pCmdStream);
	}
	else if (OT_USER_DEFINED_ARRAY == (DBL_ELEMENT_TYPE)type) {
		char name[1024];
		int len = _read_string(p, name, sizeof(name));
		if (len == -1){
			m_sys_errno = LOGIC_ERR_INPUT_INSTRUCT;
			nd_logerror("read var name error \n");
			return -1;
		}
		p += len;
		if (!_getValueFromArray(stack,name, outValue) )	{
			m_sys_errno = LOGIC_ERR_VARIANT_NOT_EXIST;
			nd_logerror("make array %s error\n", name);
			return -1;
		}
		return (int)(p - pCmdStream);
	}
	else if (OT_USER_DEFINED == (DBL_ELEMENT_TYPE)type) {
		char name[1024];
		int len = _read_string(p, name, sizeof(name));
		if (len == -1){
			m_sys_errno = LOGIC_ERR_PARSE_STRING;
			nd_logerror("read var name error \n");
			return -1;
		}
		if (-1==_getValueFromUserDef(name, outValue)) {
			nd_logerror("read user define var %s error \n", name);
			m_sys_errno = LOGIC_ERR_VARIANT_NOT_EXIST;
			//return -1;
		}
		p += len;
		return (int)(p - pCmdStream);
	}
	else {
		size_t size = stack->cmd->buf + stack->cmd->size - pCmdStream;
		return outValue.ReadStream(pCmdStream,size, m_cmdByteOrder);
	}

	m_sys_errno = LOGIC_ERR_INPUT_INSTRUCT;
	return -1;
}

// get user define from text , format : userDataType(arg,....) ;
int LogicParserEngine::_getValueFromUserDef(const char *inputName, DBLDataNode &outValue)
{
	char name[128];
	const char *p = ndstr_nstr_end(inputName, name, '(', sizeof(name));
	
	ndstr_to_little(name);
	UserDefData_map_t::iterator it = m_useDefType.find(name);
	if (it==m_useDefType.end()) {
		
		return -1;
	}
#define _FETCH_ARG_TXT(_inText, _outTxt, _startCh,_endCh) \
	if(*_inText == _startCh) { \
		++_inText ;				\
		_inText = ndstr_nstr_end(_inText, _outTxt,_endCh, sizeof(_outTxt)); \
		if(_inText && *_inText==_endCh) { \
			++_inText ;	\
		}				\
	}
	
	//int ret = 0;
	LogicUserDefStruct myType = *(it->second);
	if (p && *p=='(' ) 	{
		// read init function 
		++p;
		char *pend =(char*) strchr(p, ')');
		if (!pend || *pend != ')'){
			return -1;
		}
		*pend = 0;

		int index = 0 ;

		do {
			name[0] = 0;
			_FETCH_ARG_TXT(p, name, '[', ']')
			else _FETCH_ARG_TXT(p, name, '(', ')')
			else _FETCH_ARG_TXT(p, name, '\'', '\'')
			else _FETCH_ARG_TXT(p, name, '\"', '\"')
			else {
				p = ndstr_nstr_end(p, name, ',', sizeof(name));
			}
			if (name[0] )	{
				DBLDataNode *pMember = myType.ref(index);
				if (pMember){
					pMember->InitFromTxt(name);
				}
				else {
					break;
				}
			}
			++index;
			if (p && *p ==','){
				++p;
			}
		} while (p && *p);

		*pend = ')';		
	}

	outValue.InitSet(myType);

	return 0;
}

bool LogicParserEngine::_getValueFromArray(runningStack *stack, const char *inputText, DBLDataNode &outValue)
{
	char name[128];
	const char *p = ndstr_parse_word(inputText, name);
	const LogicUserDefStruct*pUserType = NULL;

	DBL_ELEMENT_TYPE arrayType = DBLDataNode::getTypeFromName(name);
	if (arrayType == OT_USER_DEFINED){
		ndstr_to_little(name);
		pUserType = getUserDataType(name);
		if (!pUserType)	{
			nd_logerror("can not found user define type %s\n", name);
			return false;
		}
	}


	int index = 1;
	p = ndstr_first_valid(p);
	if (*p == _ARRAR_BEGIN_MARK) {
		p = ndstr_fetch_array_index(p, name, sizeof(name));
		if (!p)	{
			return false;
		}
		DBLDataNode subIndex;
		index = -1;
		if (_getVarValue(stack, name, subIndex)) {
			index = subIndex.GetInt();
		}
		if (-1 == index)	{
			nd_logerror("get array size error %s\n", name);
			return false;
		}
	}

	outValue.InitReservedArray(index, arrayType);
	if (pUserType){
		for (int i = 0; i < index; i++){
			outValue.SetArray(*pUserType, i);
		}
	}

	return true;
}

int LogicParserEngine::_read_string(char *pCmdStream, char *outbuf, size_t size)
{
	NDUINT16 size16 = 0;
	char *p = pCmdStream ;
	//size16 = *((*(NDUINT16**)&p)++); 
	p = lp_read_stream(p, size16, m_cmdByteOrder);
	if (size16 >= size){
		//input data is too long
		m_sys_errno = LOGIC_ERR_PARSE_STRING;
		return -1;
	}
	memcpy(outbuf, p, size16);
	outbuf[size16] = 0;
	//strncpy(outbuf, p, size);
	//nd_assert(size16 == (NDUINT16)strlen(p));
	p += size16;
	//nd_assert(*p == 0);
	//++p;

	return (int)(p - pCmdStream);

}
int LogicParserEngine::_readGameDataTable(runningStack *stack, char *pCmdStream, DBLDataNode &outValue)
{
	//	DBLDataNode DBL_FindDataObject(const char *table, const int RowID, const char *ColName);
	//E_OP_READ_TABLE, 	//	read game-static-data  string(tablename) + id + col-name
	char *p = pCmdStream;
	char tablename[128];
	char colname[128];
	
	int readlne = _read_string(p, tablename, sizeof(tablename));
	if (readlne <=0){
		return 0;
	}
	p += readlne;

	readlne = _read_string(p, colname, sizeof(colname));
	if (readlne <= 0){
		return 0;
	}
	p += readlne;

	DBLDataNode dataval;
	readlne = _getValue(stack, p, dataval);
	if (readlne <= 0)	{
		return 0;
	}
	p += readlne;

	LogicEngineRoot *root = LogicEngineRoot::get_Instant();
	
	outValue = DBL_FindDataObject(tablename, dataval.GetInt(), colname,root->GetEncodeType(), root->GetEncodeType());
	return (int)(p - pCmdStream);
}

//外表事件驱动
void LogicParserEngine::onEvent(int event, parse_arg_list_t &params)
{
	//continue function that waiting event
	for (event_list_t::iterator it=m_events.begin(); it!=m_events.end(); ) {
		if (checkEventOk(event,params,it->eventid, it->event_params)) {
			_continueEvent(&(*it));
			it = m_events.erase(it) ;
		}
		else {
			++it ;
		}
	}
	//call local event handler
	if (m_hanlers.size() == 0){
		return;
	}
	
	for (event_handler_list_t::iterator it = m_hanlers.begin(); it != m_hanlers.end();++it) {
		if (it->event_id !=event){
			continue;
		}
		parse_arg_list_t mylist = params;
		mylist.insert(mylist.begin(), DBLDataNode(it->name.c_str()));
		_callFunction(mylist);
	}
}

bool LogicParserEngine::SendEvent(int event_id, parse_arg_list_t &args)
{
	SendEventNode node;
	node.event_id = event_id;
	node.args = args;
		 
	m_needHandleEvents.push_back(node);
	return true;
}

void LogicParserEngine::update(ndtime_t interval)
{
	if (m_timer.size()==0){
		return;
	}
	timer_list_t::iterator it;
	for (it = m_timer.begin(); it != m_timer.end(); ){
		logicParserTimer *ptimer = &*it;
		ptimer->left_val -= interval;
		if (ptimer->left_val <= 0)	{
			//parse_arg_list_t args;
			//args.push_back(DBLDataNode(ptimer->params[0].GetText()));
			ptimer->left_val = ptimer->interval;
			_callFunction(ptimer->params);
			if (ptimer->type){
				it = m_timer.erase(it);
				continue; 
			}
		}
		++it;		
	}
}

bool LogicParserEngine::waitEvent(runningStack *stack, int event, parse_arg_list_t &params)
{
	StackWaitEventNode node ;
	
	node.stack = *stack ;
	
	node.stack.affairHelper = 0;
	node.eventid = event ;	
	node.event_params = params;	
	node.preRegisterVal = m_registerVal;
	node.preFlag = m_registorFlag ;
	node.preCtrl = m_registorCtrl;
	//node.preOnErrorExit = m_OnErrorExit;
	node.preRegCount = m_registerCount;

	m_events.push_back(node) ;
	setErrno(LOGIC_ERR_WOULD_BLOCK);
	return true ;
}

bool LogicParserEngine::_installHandler(int ev, const char *funcName)
{
	event_handler_list_t::iterator it;
	for (it = m_hanlers.begin(); it != m_hanlers.end(); ++it){
		if (it->event_id == ev && 0 == ndstricmp(funcName, it->name.c_str()))	{
			return true;
		}
	}

	m_hanlers.push_back(eventHandler(ev, funcName));
	return true;
}

bool LogicParserEngine::_removeHandler(int ev, const char *funcname)
{
	event_handler_list_t::iterator it;
	for (it =m_hanlers.begin(); it!=m_hanlers.end(); ++it){
		if (it->event_id == ev && 0==ndstricmp(funcname, it->name.c_str()))	{
			m_hanlers.erase(it);
			break;
		}
	}
	return true;
}


bool LogicParserEngine::_CreateUserDefType(runningStack *runstack, parse_arg_list_t &args)
{
	//logcal_variant node;
	//node.name = args[0].GetText();

	LogicUserDefStruct *userDefData = new LogicUserDefStruct();
	for (int i = 1; i < args.size(); ++i) {
		const char *name = args[i].GetText();
		++i;
		if (name) {
			userDefData->push_back(name, args[i]);
		}
	}

	const char *pText = args[0].GetText();
	char name[128];
	strncpy(name, pText, sizeof(name));
	ndstr_to_little(name);

	m_useDefType[name] = userDefData;
	return true;

}


bool LogicParserEngine::_mathOperate(eMathOperate op,const DBLDataNode &var1, const DBLDataNode &var2)
{
	if (var1.GetDataType()==OT_STRING)	{

		float f1 = var1.GetFloat();
		float f2 = var2.GetFloat();

		return _mathOperate(op, DBLDataNode(f1), DBLDataNode(f2));
		
	}
	switch (op) {			
		case E_MATH_ADD:
			m_registerVal = var1 + var2;
			break;
		case E_MATH_SUB:
			m_registerVal = var1 - var2;
			break ;
		case E_MATH_MUL:
			m_registerVal = var1 * var2;
			break ;
		case E_MATH_DIV:
			m_registerVal = var1 / var2;
			break ;
		case E_MATH_SQRT:
		{
			float fv1 = var1.GetFloat();
			//float fv2 = var2.GetFloat();
			float val = sqrt(fv1);
			m_registerVal.InitSet(val);
			break;
		}
		case E_MATH_MAX:
		{
			float fv1 = var1.GetFloat();
			float fv2 = var2.GetFloat();
			float val = NDMAX(fv1, fv2);
			m_registerVal.InitSet(val);
			break;
		}
		case E_MATH_MIN:
		{
			float fv1 = var1.GetFloat();
			float fv2 = var2.GetFloat();
			float val = NDMIN(fv1, fv2);
			m_registerVal.InitSet(val);
			break;
		}
		case E_MATH_RAND:
		{

			float fv1 = var1.GetFloat();
			float fv2 = var2.GetFloat();
			float val = ((float)rand() / ((float)RAND_MAX) * (fv2 - fv1) + fv1);
			m_registerVal.InitSet(val);
			break;
		}
		default:
			m_sys_errno = LOGIC_ERR_INPUT_INSTRUCT;
			return false ;
	}
	return  true ;
}


bool LogicParserEngine::_varAssignin(runningStack *stack,const char *name, DBLDataNode &inputVal)
{
	char varname[128];
	char subName[128];
	const char *p = name;

	p = ndstr_parse_word(p, varname);

	DBLDataNode* pdata = _getLocalVar(stack, varname);
	if (!pdata)	{
		nd_logerror("var %s not exist\n", name);
		return false;
	}
	//DBLDataNode value = *pdata;
	LogicUserDefStruct *pUserStruct =NULL;

	if (pdata->GetDataType() == OT_USER_DEFINED) {
		pUserStruct = (LogicUserDefStruct *)pdata->getUserDef();
		pdata = NULL;
	}

	while (p && *p){
		if (*p == _ARRAR_BEGIN_MARK) {
			if (!pdata)	{
				nd_logerror(" %s is not array\n",name);
				return false;
			}
			p = ndstr_fetch_array_index(p, subName, sizeof(subName));
			if (!p)	{
				nd_logerror("get %s var error\n", name);
				return false;
			}
			DBLDataNode subIndex;
			if (_getVarValue(stack, subName, subIndex)) {
				int index = subIndex.GetInt();				
				if (OT_USER_DEFINED== pdata->GetArrayType()){
					pUserStruct = (LogicUserDefStruct *)pdata->GetarrayUser(index);
					pdata = NULL;
				}
				else {
					pdata->SetArray(inputVal, index);
					return true;

				}
			}
			else {
				return false;
			}
		}
		else if (*p == '.') {
			if ( !pUserStruct ){
				nd_logerror(" %s is not json data\n", name);
				return false;
			}
			++p;
			p = ndstr_parse_word(p, subName);
			pdata = pUserStruct->ref(subName);
			if (!pdata) {
				nd_logerror("can not found member %s\n", subName);
				return false;
			}

			if (pdata->GetDataType() == OT_USER_DEFINED) {
				pUserStruct = (LogicUserDefStruct *)pdata->getUserDef();
				pdata = NULL;
			}
		}
		else {
			break;
		}
	}

	if (pdata ){
		*pdata = inputVal;
	}
	else if (pUserStruct && inputVal.GetDataType() == OT_USER_DEFINED && inputVal.getUserDef()) {
		*pUserStruct = *inputVal.getUserDef();
	}
	else {
		return false;
	}
	return true;
	//return outVarRef.unsafeRefOther(*pdata);
	/*
	DBLDataNode *pData = NULL ;
	
	if (strchr(name, '[')) {
		char arrayName[128] ;
		//char arrayIndex[10] ;
		
		const char *pElement = ndstr_nstr_end(name, arrayName, '[', sizeof(arrayName)) ;
		if (!pElement || *pElement!= '[') {
			nd_logerror("var %s not found ,express-error\n", name);
			m_sys_errno = LOGIC_ERR_VARIANT_NOT_EXIST;
			return false ;
		}
		++pElement ;
		int arrIndex = ndstr_atoi_hex(pElement);
		pData = _getLocalVar(stack, arrayName);
		if (pData->GetArrayType() != OT_ARRAY) {
			nd_logerror("var %s not array\n", name);
			m_sys_errno = LOGIC_ERR_VARIANT_NOT_EXIST;
			return  false ;
		}
		
		pData->SetArray(inputVal, arrIndex) ;
	}
	//get user define data member
	else if(strchr(name, '.')) {
		char varName[128] ;
		
		const char *pMember = ndstr_nstr_end(name, varName, '.', sizeof(varName)) ;
		if (!pMember || *pMember!= '.') {
			nd_logerror("var %s not found ,express-error\n", name);
			m_sys_errno = LOGIC_ERR_VARIANT_NOT_EXIST;
			return false;
		}
		++pMember ;
		
		pData = _getLocalVar(stack, varName);
		if (!pData || pData->GetDataType()!=OT_USER_DEFINED) 	{
			nd_logerror("var %s data type is not json \n", name);
			m_sys_errno = LOGIC_ERR_VARIANT_NOT_EXIST;
			return false;
		}
		
		pData->setUserDefMember(pMember,inputVal) ;
	}
	
	else {
		DBLDataNode *pData = _getLocalVar(stack, name);
		if (pData) 	{
			*pData = inputVal ;
		}
		else {
			logcal_variant node;
			node.name = name;
			node.var = inputVal;
			stack->local_vars.push_back(node);
		}
		
	}
	
	return  true ;
	*/
}

bool LogicParserEngine::_buildUserDefData(runningStack *runstack,parse_arg_list_t &args)
{	
	logcal_variant node;
	node.name = args[0].GetText() ;
	
	LogicUserDefStruct userDefData ;
	for (int i=1; i<args.size(); ++i) {
		const char *name = args[i].GetText() ;
		++i ;
		if (name) {
			userDefData.push_back(name, args[i]) ;
		}
	}
	
	node.var.InitSet(userDefData) ;
	
	runstack->local_vars.push_back(node);
	m_registerVal = node.var ;
	return true ;
	
}

bool LogicParserEngine::_addTimer(parse_arg_list_t &args)
{
	if (args.size() <5)	{
		nd_logerror("timer error need 4 arguments\n");
		m_sys_errno = LOGIC_ERR_INPUT_INSTRUCT;
		return false;
	}
	parse_arg_list_t params;
	for (size_t i = 4; i < args.size(); i++) {
		params.push_back(args[i]);
	}
	if (!args[0].GetBool() ) {

		return _addTimer(args[1].GetInt(), (int)(args[2].GetFloat() * 1000), args[3].GetText(), params);
	}
	else {
		LogicEngineRoot *root = LogicEngineRoot::get_Instant();
		return root->getGlobalParser()._addTimer(args[1].GetInt(), (int)(args[2].GetFloat() * 1000), args[3].GetText(), params);
	}


}

bool LogicParserEngine::_addTimer(int type, int interval, const char *timername, parse_arg_list_t &params)
{
	logicParserTimer lptime;
	lptime.type = type;
	lptime.interval = interval;
	lptime.left_val = interval;
	lptime.name = timername;
	//lptime.functionName = funcName;
	lptime.params = params;

	//lptime.params.insert(lptime.params.begin(), DBLDataNode(funcName));

	m_timer.push_back(lptime);
	return true;
}
bool LogicParserEngine::_delTimer(bool isGlobal , const char *timername)
{
	if (isGlobal){
		LogicEngineRoot *root = LogicEngineRoot::get_Instant();
		return root->getGlobalParser()._delTimer(false, timername);
	}
	timer_list_t::iterator it;
	for (it = m_timer.begin(); it != m_timer.end(); ++it){
		if (0 == ndstricmp(timername, it->name.c_str()))	{
			m_timer.erase(it);
			return true;
		}
	}
	return true;
}



bool LogicParserEngine::handleScriptGenEvent()
{
	if (m_needHandleEvents.size()==0){
		return true;
	}
	send_event_list_t tmpList = m_needHandleEvents;

	m_needHandleEvents.clear();

	LogicEngineRoot *root = LogicEngineRoot::get_Instant();
	nd_assert(root);
	send_event_list_t::iterator it = tmpList.begin();
	for (; it != tmpList.end(); ++it){
		eventNtf(it->event_id, it->args);
	}
	return true;
}


void LogicParserEngine::_pushSKipError(int err)
{
	for (size_t i = 0; i < LOGIC_ERR_NUMBER; i++){
		if (m_curStack->skipErrors[i] == 0 ) {
			m_curStack->skipErrors[i] = err;
			break;
		}
	}


	for (int i = 0; i < LOGIC_ERR_NUMBER; i++)	{
		if (m_skipErrors[i] == 0) {
			m_skipErrors[i] = err;
			break;
		}
	}
}

bool LogicParserEngine::_checkIsSkip(int err)
{
	for (size_t i = 0; i < LOGIC_ERR_NUMBER; i++){
		if (m_skipErrors[i] == err) {
			return true;
		}
	}
	return false;
}

bool LogicParserEngine::_callFunction(parse_arg_list_t &func_args,const char *moduleName)
{
	bool ret = _call(func_args, m_registerVal,moduleName) ;
	if (m_simulate && !ret )	{
		if (_checkIsSkip( getErrno())) {
			DBLDataNode val1;
			char buf[1024];
			snprintf(buf, sizeof(buf), "function %s return val", func_args[0].GetText());
			apollo_logic_out_put("call function :");
			apollo_printf(this, func_args, val1);
			apollo_logic_out_put("success \n");
			m_registerVal.InitSet((const char*)buf);
			ret = true;
		}
			
	}
	return ret;
}

bool LogicParserEngine::_call(parse_arg_list_t &args, DBLDataNode &result, const char *moduleName)
{
	LogicEngineRoot *pRoot = LogicEngineRoot::get_Instant() ;
	nd_assert(pRoot) ;
	
	if (args.size() < 1){
		setErrno(LOGIC_ERR_PARAM_NUMBER_ZERO);
		nd_logerror("function-call args-list is too few\n");
		return false;
	}
	DBL_ELEMENT_TYPE type = args[0].GetDataType();
	if (type != OT_STRING){
		setErrno(LOGIC_ERR_FUNCTION_NAME);
		nd_logerror("function name type must be string\n");
		return false;
	}
	const char *funcName = args[0].GetText();
	if (!funcName || !funcName[0]){
		setErrno(LOGIC_ERR_FUNCTION_NAME);
		nd_logerror("function name is NULL\n");
		return false;
	}
	
	logicParser_func cppfunc = pRoot->getCPPFunc(funcName) ;
	if (cppfunc) {
		bool ret = cppfunc(this, args, result);
		if (m_simulate && !ret)	{
			setErrno(LOGIC_ERR_FUNCTION_NOT_FOUND);
		}
		return ret;
	}
	const char *pModule = moduleName ? moduleName: getCurMoudle();
	const scriptCmdBuf* pScript = pRoot->getScript(funcName, pModule ,&pModule) ;
	if (pScript) {
		return runFunction(pModule, pScript, args, result) ;
	}
	setErrno(LOGIC_ERR_FUNCTION_NOT_FOUND);
	nd_logerror("script function %s not found\n", funcName) ;
	return false;
}


bool LogicParserEngine::opCalc(void *func, int size, float *result)
{
	*result = 0 ;
	return false ;
}

bool LogicParserEngine::_opCmp(DBLDataNode& compValData, DBLDataNode& invalData, eParserCompare op)
{
	if (!compValData.CheckValid() || !invalData.CheckValid()) {
		return false;
	}
	float compVal = compValData.GetFloat();
	float inval = invalData.GetFloat();

	switch (op) {
		case ECMP_EQ :  			// =
			return compVal == inval;
			//return regval == cmpval;
		case ECMP_LT :  			// <
			return  compVal < inval;
		case ECMP_BT :  			// >
			if (compVal == inval)	{
				return false;
			}
			return !(compVal < inval);
		case ECMP_ELT : 			// <=
			if (compVal == inval)	{
				return true;
			}
			return  compVal < inval;
		case ECMP_EBT : 			// >=
			if (compVal == inval)	{
				return true;
			}
			return  !(compVal < inval);
		default:
			break ;
	}
	m_sys_errno = LOGIC_ERR_PARAM_NOT_EXIST;

	return false ;
}

void LogicParserEngine::Reset()
{
	m_registorFlag =false ;				//比较结果
	m_registorCtrl = true;
	m_sys_errno = LOGIC_ERR_SUCCESS;
	m_registerCount = 0;
	
}

void LogicParserEngine::setErrno(int errcode)
{
	m_sys_errno = errcode;
}


void LogicParserEngine::setSimulate(bool flag, LogicObjectBase *owner )
{
	m_simulate = flag;
	if (m_simulate)	{
		if (owner)	{
			m_owner = owner;
		}
		else if (m_owner) {
			m_owner = new TestLogicObject;
			m_ownerIsNew = true;
		}
	}
}
void LogicParserEngine::ResetStep()
{
	
	//m_registorA =0 ;//计算结果
	m_registorFlag = false ;				//比较结果
	//m_sys_errno = 0;
	//m_registorCtrl = false ;

}


bool LogicParserEngine::checkEventOk(int event, parse_arg_list_t &params, int waited_id, parse_arg_list_t &waited_params)
{
	if (event != waited_id)	{
		return false;
	}
	if (waited_params.size() > params.size()){
		return false;
	}
	for (size_t i = 0; i < waited_params.size(); i++)
	{
		if (waited_params[i] != params[i])	{
			return false;
		}
	}
	return true;
}

int LogicParserEngine::_continueEvent(StackWaitEventNode *eventNode)
{
	m_registorCtrl = eventNode->preCtrl;
	m_registerVal = eventNode->preRegisterVal;
	//m_OnErrorExit = eventNode->preOnErrorExit;
	m_registorFlag = eventNode->preFlag;
	m_registerCount = eventNode->preRegCount;
	return _runCmd(&eventNode->stack);
}

//static __ndthread vm_cpu *__cur_vm = NULL;
//find var address
static int place_name_runtime(const char *input, char *buf, int size, void *user_data)
{
	vm_cpu *__cur_vm = (vm_cpu *) user_data;
	if (!__cur_vm){
		return -1;
	}
	LogicParserEngine *le = (LogicParserEngine*)vm_get_param(__cur_vm);
	if (!le){
		return -1;
	}
	runningStack *pstack = le->getCurStack();
	if (!pstack){
		return -1;
	}
	int index = 0;
	LogicData_vct::iterator it = pstack->local_vars.begin();
	for (; it != pstack->local_vars.end(); it++) {
		if (ndstricmp(input, it->name.c_str())==0) {
			snprintf(buf, size, "[%d]",index);
			return 0;
		}
		++index;
	}
	
	nd_logerror("公式解析错误:不能找到变量[%s]\n" , input);
	return -1;
}

vm_value* logic_formula_get_mmaddr(vm_adddress addr, struct vm_cpu *vm)
{
	LogicParserEngine *le = (LogicParserEngine*)vm_get_param(vm);
	if (!le){
		return NULL;
	}
	runningStack *pstack = le->getCurStack();
	if (!pstack){
		return NULL;
	}
	if (addr > pstack->local_vars.size() || addr >= vm->mem_size) {
		nd_logerror("too much vars addr=%d\n", addr);
		return NULL;
	}
	vm->memory[addr] = pstack->local_vars[addr].var.GetFloat();
	return &vm->memory[addr];
}

int LogicParserEngine::_runFormula(const char *text, float *val)
{
	//init vm
	int ret = -1;
	if (!text || !text[0]){
		return 0;
	}

	vm_value mem[VM_STACK_SIZE];
	char cmd_buf[1024];
	vm_machine_init(&m_vmFormula, mem, VM_STACK_SIZE);
#if 0
	vm_set_echo_ins(&m_vmFormula, 1);
	vm_set_echo_result(&m_vmFormula, 1);
	vm_set_outfunc(nd_output);
	vm_set_errfunc(nd_output);
#else 
	vm_set_echo_ins(&m_vmFormula, 0);
	vm_set_echo_result(&m_vmFormula, 0);
	vm_set_outfunc(NULL);
	vm_set_errfunc(NULL);
#endif

	vm_set_param(&m_vmFormula, (void*)this);
	vm_set_mmfunc(&m_vmFormula, logic_formula_get_mmaddr, VM_STACK_SIZE);
	//__cur_vm = &m_vmFormula;

	//parse 
	size_t size = vm_parse_expression((char*)text, cmd_buf, sizeof(cmd_buf), place_name_runtime,&m_vmFormula);
	if (size > 0) {
		if (0 == vm_run_cmd(&m_vmFormula, cmd_buf, size)) {
			*val = vm_return_val(&m_vmFormula);
			ret = 0;
		}
		else {
			nd_logerror("run formula %s error\n", text);
		}
	}
	else {
		nd_logerror("parse formula error %s\n", text);
	}
	
	//__cur_vm = NULL;
	return ret;
}

bool LogicParserEngine::_beginHelper()
{
	 
	if (!m_curStack->affairHelper){
		m_curStack->affairHelper = new LogicObjAffairHelper(m_owner);
	}
	return true;
}

void LogicParserEngine::_commitAffair()
{
	if (m_curStack->affairHelper)	{
		delete m_curStack->affairHelper;
		m_curStack->affairHelper = 0;
	}
}

void LogicParserEngine::_rollbacAffair()
{
	if (m_curStack->affairHelper)	{
		m_curStack->affairHelper->Rollback();
		delete m_curStack->affairHelper;
		m_curStack->affairHelper = 0;
	}
}


const LogicUserDefStruct* LogicParserEngine::getUserDataType(const char *name) const
{
	UserDefData_map_t::const_iterator it = m_useDefType.find(name);
	if (it == m_useDefType.end()) {
		return NULL;
	}
	return it->second;
}


int logic_rand(NDUINT32 val1, NDUINT32 val2)
{
	int val;
	int range_min = (int)val1;
	int range_max = (int)val2;

	if (range_max < range_min) {
		int tmp = range_min;
		range_min = range_max;
		range_max = tmp;
	}
	else if (range_max == range_min)
		return (int)range_max;

	val = (int)((float)rand() / (float)RAND_MAX  * (range_max - range_min) + range_min);

	return val;
}


