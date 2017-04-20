/* file logicEngineRooot.h
 *
 * root manager of logic engine 
 *
 * create by duan
 * 2015-5-12
 */

#ifndef _LOGIC_ENGINE_ROOT_H_ 
#define _LOGIC_ENGINE_ROOT_H_

#include "logic_parser/logicParser.h"
#include "logic_parser/logic_function.h"


#define  DEFAULT_LOAD_INITILIZER_FUNC "_module_init_entry" 
#define GLOBAL_MODULE_NAME "_global"
#define LOGIC_FUNCTION_LIST_NAME "func_list"
#define LOGIC_EVENT_LIST_NAME "event_list"
#define LOGIC_ERROR_LIST_NAME "error_list"

typedef bool(*logicParser_func)(LogicParserEngine*parser, parse_arg_list_t &args, DBLDataNode &result);
struct func_cpp_info
{
	func_cpp_info() :func(0){}
	std::string comment;
	logicParser_func func;
};

class LogicEngineRoot
{
public:
	LogicEngineRoot();
	~LogicEngineRoot();

	static LogicEngineRoot *get_Instant();
	static void destroy_Instant();

	int LoadScript(const char *scriptStream, LogicParserEngine *initCallLoader);
	int Reload(const char *scriptStream, LogicParserEngine *initCallLoader);

	int Init();
	void Destroy();
	void update(ndtime_t tminterval);
	
	const scriptCmdBuf* getScript(const char *funcName,const char *inModuleName, const char**outModuleName) const;
	const char *getEventFunc(int event_id)const;
	logicParser_func getCPPFunc(const char *funcName) const;

	// set api function . call in c++ . 
	static void installFunc(logicParser_func func, const char *name, const char *dispName);
	void installEvent(int event_id, const char *event_func);
	
	//export c++ function to xml
	int dumbCPPfunc(const char *outXmlfile);// out put cpp function for editor using.
	int test();
	
	const char* _convertFuncName(const char *text, int inputEncodeType);
	
	//int run_init_script( );
	bool getModuleChangedTime(const char *moduleName, DBLDataNode &result);
	int GetEncodeType() {
		return m_scriptEncodeType;
	}
	logic_print setPrint(logic_print func, void *outfile);
	logic_print m_screen_out_func;
	void *m_print_file;
	int getErrno(){ return m_globalParser.getErrno(); }

	LogicParserEngine &getGlobalParser() {
		return m_globalParser;
	}
	const char *getDftScriptModule() const {return m_dftScriptModule.c_str();}
	void setDftScriptModule(const char *script) ;

	void setOutPutEncode(int encode) { m_displayEncodeType = encode; }
	int getOutPutEncode() { return m_displayEncodeType; }


	LogicData_vct* getGlobalVars() { return  &m_global_vars; }

	DBLDataNode *getVar(const char *name);

	UserDefData_map_t &getUserDefType() { return m_useDefType; }
private:

	//static LogicEngineRoot *s_root;
	typedef std::map<std::string, scriptCmdBuf*> script_func_map;	
	typedef std::map<std::string, script_func_map*>script_module_map;
	typedef std::map<std::string, time_t>module_changed_tm_map;
	typedef std::map<std::string, func_cpp_info> cpp_func_map;
	typedef std::map<int, std::string> event_table_map;

	int unloadScript();
	int _unloadModule(script_func_map &module);
	bool _pushGlobal(scriptCmdBuf *pcmdbuf);

	const scriptCmdBuf* _findScript(const char *funcName, const char *moduleName, const char**outModuleName) const;

	int m_displayEncodeType;
	int m_scriptEncodeType;
	//time_t m_compileTm;
	//script_func_map m_scripts;
	script_module_map m_modules;
	static cpp_func_map m_c_funcs;
	event_table_map m_event_entry;
	module_changed_tm_map m_moduleChangedTime;

	LogicParserEngine m_globalParser;
	std::string m_dftScriptModule ; //default run script module if undefine module

	LogicData_vct m_global_vars; 

	UserDefData_map_t m_useDefType;
};


#define CHECK_ARGS_NUM_ONLY(_args, _num, _parser) \
	if (_args.size() < _num){		\
		if(_parser)_parser->setErrno(NDERR_FEW_PARAMS); \
		nd_logerror("%s error need %d args \n",__FUNCTION__, _num);	\
		return false;				\
						}

#ifndef ND_DEBUG
#define CHECK_ARGS_NUM(_args, _num, _parser) \
	if (_args.size() < _num){		\
		if(_parser)_parser->setErrno(NDERR_FEW_PARAMS); \
		nd_logerror("%s error need %d args \n",__FUNCTION__, _num);	\
		return false;				\
				}
#else 
#define CHECK_ARGS_NUM(_args, _num, _parser) \
	if (_args.size() < _num){		\
		if(_parser)_parser->setErrno(NDERR_FEW_PARAMS); \
		nd_logerror("%s error need %d args \n",__FUNCTION__, _num);	\
		return false;				\
	}								\
	else {							\
		for (int _j=1; _j<_num; _j++) {	\
			if(!_args[_j].CheckValid()) {	\
				if(_parser)_parser->setErrno(NDERR_PARAM_INVALID); \
				nd_logerror("%s error arg[%d] is invalid\n",__FUNCTION__, _j);	\
				return false ;			\
			}							\
		}								\
	}

#endif

#define CHECK_DATA_TYPE(_data, _type, _parser) \
	if (_data.GetDataType() != _type) {	\
		if (_parser)_parser->setErrno(NDERR_PARAM_TYPE_NOT_MATCH); \
		nd_logerror(#_data " type error need " #_type " \n");	\
		return false;					\
			}									\
	if (_type >= OT_OBJECT_VOID && NULL==_data.GetObject()) {	\
		if(_parser)_parser->setErrno(NDERR_PARAM_INVALID); \
		nd_logerror(#_data " is NULL \n");	\
		return false;						\
			}



struct logicFuncInstallHelper
{
	logicFuncInstallHelper(logicParser_func expFunction, const char *name, const char *comment)
	{
		LogicEngineRoot::installFunc(expFunction, name, comment);
	}

};

#define APOLLO_SCRIPT_API_DEF(_FUNC_NAME, _COMMENT) \
static bool _FUNC_NAME(LogicParserEngine*parser, parse_arg_list_t &args, DBLDataNode &result) ;		\
static logicFuncInstallHelper __s_installer_helper##_FUNC_NAME(_FUNC_NAME,#_FUNC_NAME, _COMMENT) ;	\
bool _FUNC_NAME(LogicParserEngine*parser, parse_arg_list_t &args, DBLDataNode &result)


#define APOLLO_SCRIPT_API_DEF_GLOBAL(_FUNC_NAME, _COMMENT) \
bool _FUNC_NAME(LogicParserEngine*parser, parse_arg_list_t &args, DBLDataNode &result) ;		\
static logicFuncInstallHelper __s_installer_helper##_FUNC_NAME(_FUNC_NAME,#_FUNC_NAME, _COMMENT) ;	\
bool _FUNC_NAME(LogicParserEngine*parser, parse_arg_list_t &args, DBLDataNode &result)


#endif 
