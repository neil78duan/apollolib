//
//  logicParser.h
//  gameHall
//
//  Created by duanxiuyun on 15-3-24.
//  Copyright (c) 2015 duanxiuyun. All rights reserved.
//

#ifndef __gameHall__logicParser__
#define __gameHall__logicParser__


#include "nd_common/nd_common.h"
#include "nd_vm/nd_vm.h"
#include <string>
#include <vector>
#include <list>
#include <map>
#include "logic_parser/logicDataType.h"
#include "logic_parser/objectBaseMgr.h"

#define PARSE_TRACE nd_logdebug

typedef NDUINT32 operator_value_t;

typedef NDUINT32 operator_cmd_t;
class LogicUserDefStruct;

#define _write_float_to_buf(_addr, _f)  \
do {	\
	union {	\
		char buf[4];	\
		float f;		\
	} val;				\
	val.f = _f;			\
	_addr[0] = val.buf[0];	\
	_addr[1] = val.buf[1]; \
	_addr[2] = val.buf[2];	\
	_addr[3] = val.buf[3];	\
	_addr += sizeof(float);	\
} while (0)

#define _read_float_from_buf(_f, _addr)  \
do {	\
	union {	\
		char buf[4];	\
		float f;		\
	} val;				\
	val.buf[0] = _addr[0] ;	\
	val.buf[1] = _addr[1] ; \
	val.buf[2] = _addr[2] ;	\
	val.buf[3] = _addr[3] ;	\
	_addr += sizeof(float);	\
	_f = val.f;			\
} while (0)

#define LOGIC_PARSE_MARK  0x1b2f3f4e
// game logic instruct index
enum eParserOperator{
	E_OP_EXIT,			//	
	E_OP_READ ,			//	 eOperatorDest + DBLDataNode:index
	E_OP_WRITE,			//	 eOperatorDest + DBLDataNode:index + DBLDataNode-stream
	E_OP_ADD ,			//	 eOperatorDest + DBLDataNode:index + DBLDataNode-stream
	E_OP_SUB,			//	 eOperatorDest + DBLDataNode:index + DBLDataNode-stream
	E_OP_OPERATE,		//like  E_OP_ADD : string:obj-name, string:cmd-name, DBLDataNode:index ,DBLDataNode:value
	E_OP_MAKE_VAR ,		//  make local variant	 name + DBLDataNode-stream
	E_OP_READ_TABLE, 	//	read game-static-data  string(tablename)  + col-name + id
	E_OP_COMP,	 		//	 eParserCompare  + DBLDataNode-stream1 + DBLDataNode-stream1
	E_OP_CALC,	 		//	NDUINT32 + string
	E_OP_WAIT_EVENT,	//  eParserEventId + int(argc) + operator_value_t[...]
	E_OP_GET_OTHER_OBJECT, 	//    DBLDataNode-stream
	E_OP_ON_ERROR_CONTINUE,	//set flag continue next instruct when error 
	E_OP_CALL_FUNC,		//  	int(argc) + string(function_name \0)  + string1\0 + string2\0
	E_OP_SHORT_JUMP, 	//  NDUINT32( offset )
	E_OP_LONG_JUMP,		//  jump abs addr
	E_OP_TEST_FALSE_SHORT_JUMP, // test flag register and jump  instruct + address
	E_OP_TEST_FALSE_LONG_JUMP, // 
	E_OP_TEST_SUCCESS_SHORT_JUMP, //not use SET calc-register 
	E_OP_COMMIT_AFFAIR,		//COMMIT AFFAIR
	E_OP_DEBUG_INFO,	// the following info is debug (int16-nodeindex, string node name)
	E_OP_OUT_PUT ,		// out put var info 
	E_OP_LOG,		// out put var info 
	E_OP_RAND,		// GET RANDOM [MIN, MAX] 
	E_OP_SET_COUNT_REG, //SET COUNT REGISTER
	E_OP_TEST_COUNT_JUMP , //JUMP until count register is zero
	E_OP_SEND_EVENT,	//  eParserEventId + int(argc) + operator_value_t[...]
	E_OP_GET_TIME,	//get system time 
	E_OP_HEADER_INFO, //compile function/file header info  16bit size + text
	E_OP_BEGIN_AFFAIR, //  begin affair
	E_OP_GET_COMPILE_TM,
	E_OP_TIME_CMD,		//time function ( op_type, op_unit, src_time:datatype:time, val)
	E_OP_EXCEPTION_BLOCK, //instruct exception block INT32:size
	E_OP_THROW ,		// throw exception error: id 
	E_OP_EVENT_INSTALL, //install function handle event /local handler
	E_OP_EVENT_REMOVE, //remove function handle event /local handler
	E_OP_ADD_TIMER, // FORMAT:area(gloabl/local), type, interval, timer_name, function_name
	E_OP_DEL_TIMER, // timer_name
	E_OP_TEST_LAST_VAL, //test last step result is valid
	E_OP_DATATYPE_TRANSFER, //transfer data type of current value FORMAT: string var-name, aimtype
	E_OP_MAKE_USER_DATA, //build user define data: format:string-type-name, (string member,value)[], like json
	E_OP_CREATE_TYPE, //create user define data type , like c-struct global
	E_OP_MATH_OPERATE, // RUN + - * / ** sqrt max, min rand() format: int-op-type, DBLNodeData-stream1, DBLNodeData-stream2
	E_OP_ASSIGNIN ,  // run = . format: string-var-name, DBLNodeData-stream
	E_OP_CHANGE_DIR, //change current working direct  format: string:new_direct
	E_OP_REMOVE_FILE, //remove file or direct format :string:path_name
	E_OP_MAKE_DIR ,		//create direct : foramt : string 
	E_OP_TEST,			//like op_read /write
	E_OP_SKIP_ERROR,	//Skip some error
	E_OP_GET_ARRAY_SIZE,	//get array size 
	E_OP_SET_LOOP_INDEX, // set loop index, used in list_for_each
	E_OP_GET_LAST_ERROR,	//get last error before exception
	E_OP_CHECK_IS_SIMULATE, // get current simulate status , true run in simulate , false run in real game env.

};

//compare operate
enum eParserCompare{
	ECMP_NONE,
	ECMP_EQ ,  // =
	ECMP_LT ,  // <
	ECMP_BT ,  // >
	ECMP_ELT , // <=
	ECMP_EBT  // >=
};

enum eMathOperate
{
	E_MATH_ADD,
	E_MATH_SUB,
	E_MATH_MUL,
	E_MATH_DIV,
	E_MATH_SQRT,
	E_MATH_MAX,
	E_MATH_MIN,
	E_MATH_RAND,
} ;

enum eLogicSystemError
{
	LOGIC_ERR_SUCCESS = 0,
	LOGIC_ERR_INPUT_INSTRUCT = NDERR_SCRIPT_INSTRUCT, //instruction error
	LOGIC_ERR_AIM_OBJECT_NOT_FOUND = NDERR_PROGRAM_OBJ_NOT_FOUND,	// game object not found
	LOGIC_ERR_EVENT_ERROR = NDERR_EVENT_ERROR,			//evnet error
	LOGIC_ERR_VARIANT_NOT_EXIST = NDERR_VARIANT_NOT_EXIST,	//variant not found 
	LOGIC_ERR_PARSE_STRING = NDERR_PARSE_STRING,		// parse input string error
	LOGIC_ERR_READ_STREAM = NDERR_READ_STREAM,			//read stream error 
	LOGIC_ERR_PARAM_NOT_EXIST = NDERR_PARAM_NOT_EXIST,		//param not found
	LOGIC_ERR_PARAM_NUMBER_ZERO = NDERR_PARAM_NUMBER_ZERO,
	LOGIC_ERR_FUNCTION_NAME = NDERR_FUNCTION_NAME,
	LOGIC_ERR_FUNCTION_NOT_FOUND = NDERR_FUNCTION_NOT_FOUND,
	LOGIC_ERR_WOULD_BLOCK = NDERR_WOULD_BLOCK,		//WAIT EVENT 
	LOGIC_ERR_FILE_NOT_EXIST = NDERR_FILE_NOT_EXIST,
	LOGIC_ERR_NOT_INIT = NDERR_NOT_INIT,

	LOGIC_ERR_UNKNOWN = NDERR_UNKNOWN,
	LOGIC_ERR_USER_DEFINE_ERROR = NDERR_USER_DEFINE_ERROR,

	LOGIC_ERR_NUMBER = 20

// 	LOGIC_ERR_INPUT_INSTRUCT , //instruction error
// 	LOGIC_ERR_AIM_OBJECT_NOT_FOUND,	// game object not found
// 	LOGIC_ERR_EVENT_ERROR,			//evnet error
// 	LOGIC_ERR_VARIANT_NOT_EXIST,	//variant not found 
// 	LOGIC_ERR_PARSE_STRING ,		// parse input string error
// 	LOGIC_ERR_READ_STREAM,			//read stream error 
// 	LOGIC_ERR_PARAM_NOT_EXIST,		//param not found
// 	LOGIC_ERR_PARAM_NUMBER_ZERO,
// 	LOGIC_ERR_FUNCTION_NAME,
// 	LOGIC_ERR_FUNCTION_NOT_FOUND,
// 	LOGIC_ERR_WOULD_BLOCK,		//WAIT EVENT 
// 	LOGCI_ERR_FILE_NOT_EXIST,
// 
// 	LOGIC_ERR_UNKNOW ,
// 	LOGIC_ERR_USER_DEFINE_ERROR,
};


//<type_msg_data_type kinds = "enum" enum_value = "8bits,16bits,32bits,64bits,float, string" / >
enum eSendMsgDataThype{
	E_MSG_DATA_INT8,
	E_MSG_DATA_INT16,
	E_MSG_DATA_INT32 ,
	E_MSG_DATA_INT64,
	E_MSG_DATA_FLOAT,
	E_MSG_DATA_STRING
};

#define PARSER_EVENT_PARAM_NUM 4


typedef std::vector<DBLDataNode> parse_arg_list_t;

struct scriptCmdBuf
{
	scriptCmdBuf() :size(0), cmdname(0), buf(0), byteOrder(1)
	{
		
	}
	int byteOrder;
	int size ;
	const char *cmdname ;
	const char *buf ;
};

struct logcal_variant {
	std::string name;
	DBLDataNode var;
};
typedef std::vector <logcal_variant> LogicData_vct; //local variant list
struct runningStack
{
	runningStack() :cmd(0), cur_point(0), dbg_cur_node_index(0), exception_addr(0)
	{
		dbg_node[0] = 0;
		affairHelper = 0;
		for (int i = 0; i < LOGIC_ERR_NUMBER; i++)	{
			skipErrors[i] = 0;
		}
		skipErrors[0] = LOGIC_ERR_AIM_OBJECT_NOT_FOUND;
		skipErrors[1] = LOGIC_ERR_FUNCTION_NOT_FOUND;
		//skipErrors[2] = LOGIC_ERR_VARIANT_NOT_EXIST;
		//skipErrors[3] = LOGIC_ERR_VARIANT_NOT_EXIST;
		loopIndex = 0;
		error_continue = false;
	}
	bool error_continue;
	const scriptCmdBuf *cmd ;
	const char *cur_point ;
	const char *exception_addr;
	parse_arg_list_t params ;
	LogicData_vct local_vars;
	LogicObjAffairHelper *affairHelper;

	NDUINT16 dbg_cur_node_index;
	char dbg_node[128];
	
	std::string curModuleName ;		//current script file name
	std::string workingPath;		//the host work direct

	int loopIndex;
	int skipErrors[LOGIC_ERR_NUMBER];
};

//script wait event and continue run
struct StackWaitEventNode
{
	StackWaitEventNode() :  eventid(0), preFlag(false)
	{
		
	}
	
	runningStack stack ;
	int eventid ;
	
	bool preFlag;				
	bool preCtrl;
	bool preOnErrorExit;
	int preRegCount;
	DBLDataNode preRegisterVal;

	parse_arg_list_t event_params;

};

//send event to system, the effect same with call function ,but run after current function return 
struct SendEventNode
{
	int event_id;
	parse_arg_list_t args;
};

// local
struct eventHandler
{
	int event_id;
	std::string name;
	eventHandler()
	{

	}
	eventHandler(int id, const char *funcname) : event_id(id), name(funcname)
	{

	}
};

struct logicParserTimer
{
	NDUINT8 type; // 0 loop time ,1 run once when timeout
	NDUINT32 interval;
	int left_val;
	std::string name;
	parse_arg_list_t params;
};

class LogicParserEngine
{
public:
	LogicParserEngine() ;
	LogicParserEngine(LogicObjectBase *owner);	
	virtual ~LogicParserEngine() ;
	
	//call in c++
	bool runScript(int encodeType ,const char *scriptName, DBLDataNode &result, int num, ...);
	//CALL IN c++ ARGS must be included args
	bool runScript(const char *scriptName,  parse_arg_list_t &args, DBLDataNode &result, int encodeType = ND_ENCODE_TYPE);
	
	// generate game event in c++
	bool eventNtf(int event_id, int num, ...);
	bool eventNtf(int event_id, parse_arg_list_t &args);
	
	void update(ndtime_t interval) ;
	void Reset() ;
	void setErrno(int errcode);
	int getErrno() { return m_sys_errno; }
	void setSimulate(bool flag = false, LogicObjectBase *owner=NULL);
	bool checkSimulate() { return m_simulate; }
	DBLDataNode &getValue() { return m_registerVal; }
	LogicObjectBase *getOwner() { return m_owner; }
	void setOwner(LogicObjectBase *owner) { m_owner = owner; }
	runningStack *getCurStack() { return m_curStack; }


private:
	friend class LogicEngineRoot;
	//do not call directly, only call in loader
	bool _runCmdBuf(const char *moduleName, const scriptCmdBuf *buf, int param_num, ...);
	bool _runCmdBuf(const char *moduleName, const scriptCmdBuf *buf, parse_arg_list_t &params);

	//bool ParseFuncName(const char *inName, std::string &outName) ;
	//void fetchModuleName(const char * fullName,std::string &moduleName) ;
	const char *getCurMoudle() ;
	bool getFuncHeader(const scriptCmdBuf *cmd, char *buf, size_t size);
	
	// call function in script
	bool runFunction(const char *moduleName, const scriptCmdBuf *buf, parse_arg_list_t &args, DBLDataNode &result);
	// called when external event
	void onEvent(int event, parse_arg_list_t &params);
	
	//sent event to system
	bool SendEvent(int event, parse_arg_list_t &args);
protected:
	//add running script to event waiting list
	bool waitEvent(runningStack *stack, int event, parse_arg_list_t &params );
	bool handleScriptGenEvent();
	bool _installHandler(int ev, const char *funcName); //install local event handler
	bool _removeHandler(int ev, const char *funcname);
	
	bool _buildUserDefData(runningStack *runstack,parse_arg_list_t &args) ;
	
	bool _mathOperate(eMathOperate op,const DBLDataNode &var1,const DBLDataNode &var2);
	bool _varAssignin(runningStack *runstack,const char *varName, DBLDataNode &inputVal);

	bool _CreateUserDefType(runningStack *runstack, parse_arg_list_t &args);
	bool _addTimer(parse_arg_list_t &args);
	bool _addTimer(int type, int interval, const char *timername,  parse_arg_list_t &params);
	bool _delTimer(bool isGlobal,const char *timername);

	int _runCmd(runningStack *stack) ;
	int _makeVar(runningStack *stack, char *pCmdStream); //make variant from instruction 
	bool _getArg(runningStack *stack, int index, DBLDataNode &outValue);
	int _storeReg2Var(runningStack *stack, char *pCmdStream);
	//DBLDataNode* _refVariant(runningStack *stack, char *&pCmdStream);
	int _getValueFromUserDef(const char *name, DBLDataNode &outValue);
	int _getValue(runningStack *stack, char *pCmdStream, DBLDataNode &outValue); //get value from instruction
	int _readGameDataTable(runningStack *stack, char *pCmdStream, DBLDataNode &outValue);
	int _read_string(char *pCmdStream, char *outbuf, size_t size);
	DBLDataNode* _getLocalVar(runningStack *stack, const char *varname);
	bool _chdir(const char *curDir);
	bool _rmfile(const char *filename); 
	bool _mkdir(const char *curDir);

	virtual LogicObjectBase *getLogicObjectMgr(const char *objName);

	virtual bool opCalc(void *func, int size, float *result) ;// run formulate 
	bool _callFunction(parse_arg_list_t &func_args) ;
	// call external function in running script
	bool _call( parse_arg_list_t &args, DBLDataNode &result);
	
	bool _opCmp(DBLDataNode& compVal, DBLDataNode& inval, eParserCompare op);
	
	void ResetStep() ;
	bool checkEventOk(int event, parse_arg_list_t &params, int waited_id, parse_arg_list_t &waited_params);
	int _continueEvent(StackWaitEventNode *eventNode) ;
	
	
	int _runFormula(const char *text, float *val);
	bool _beginHelper();
	void _commitAffair();
	void _rollbacAffair();

	void _pushSKipError(int err);
	bool _checkIsSkip(int err);
	bool _checkIsSystemError(int err) { return ( err < NDERR_USERDEFINE && err > 0); }
	
	bool m_ownerIsNew;
	bool m_registorFlag;				// result of step or function return value,false exit
	bool m_registorCtrl;				// compare and jump registor
	bool m_OnErrorExit;
	bool m_simulate;					//run simulate or test
	int m_sys_errno;
	int m_registerCount;
	int m_cmdByteOrder;

	DBLDataNode m_registerVal; //this is common register 

	typedef std::list<StackWaitEventNode> event_list_t ;
	typedef std::vector<SendEventNode> send_event_list_t;
	typedef std::list<eventHandler> event_handler_list_t;
	typedef std::list<logicParserTimer> timer_list_t;
	typedef std::map<std::string, LogicUserDefStruct*> UserDefData_map_t;
		
	event_list_t m_events ;
	send_event_list_t m_needHandleEvents;
	event_handler_list_t m_hanlers;
	timer_list_t m_timer;

	UserDefData_map_t m_useDefType;

	runningStack *m_curStack;
	LogicObjectBase *m_owner;
	vm_cpu	m_vmFormula;


	int m_skipErrors[LOGIC_ERR_NUMBER];
};

int logic_rand(NDUINT32 val1, NDUINT32 val2);

#endif /* defined(__gameHall__logicParser__) */
