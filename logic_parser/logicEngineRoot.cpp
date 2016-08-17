/* file logicEngineRooot.cpp
 *
 * root manager of logic engine 
 *
 * create by duan
 * 2015-5-12
 */

#include "logic_parser/logicEndian.h"
#include "logic_parser/logicEngineRoot.h"
#include <stdarg.h>
#include "nd_net/byte_order.h"
#include "ndapplib/ndsingleton.h"

//bool apollo_log(LogicParserEngine*parser, parse_arg_list_t &args, DBLDataNode &result);
//bool apollo_printf(LogicParserEngine*parser, parse_arg_list_t &args, DBLDataNode &result);

//LogicEngineRoot *LogicEngineRoot::s_root= NULL;
LogicEngineRoot *LogicEngineRoot::get_Instant()
{
	return NDSingleton<LogicEngineRoot>::Get();
}

void LogicEngineRoot::destroy_Instant()
{
	NDSingleton<LogicEngineRoot>::Destroy();
}

LogicEngineRoot::LogicEngineRoot()
{
	m_screen_out_func = 0;
	m_print_file = 0;
	m_scriptEncodeType = E_SRC_CODE_ANSI;
}

LogicEngineRoot::~LogicEngineRoot()
{
}


int LogicEngineRoot::Init()
{
	m_compileTm = time(NULL);
	return init_sys_functions(this);
}
void LogicEngineRoot::Destroy()
{
	unloadScript();
	m_c_funcs.clear();
	m_event_entry.clear();
}


void LogicEngineRoot::update(ndtime_t tminterval)
{
	m_globalParser.update(tminterval);
}

int LogicEngineRoot::LoadScript(const char *scriptStream, LogicParserEngine *loader )
{
	//script come from file , must be utf8
	NDUINT8 byteOrder = -1, encodeType = -1;
	NDUINT16 moduleSize = 0;
	NDUINT32 size;
	NDUINT64 compileTm;
	char moudleName[128] ;
	FILE *pf = fopen(scriptStream, "rb");
	if (0 == pf) {
		return -1;
	}
	//geti file size 
	fseek(pf, 0, SEEK_END);
	size_t fileSize  = ftell(pf);
	fseek(pf, 0, SEEK_SET);


	fread(&byteOrder, sizeof(byteOrder), 1, pf);
	/*if (byteOrder != ND_L_ENDIAN)	{
		nd_logerror(" read file error byte-order error the file is %s current cup is %s\n", byteOrder ? "little-end" : "big-end", nd_byte_order() ? "little-end" : "big-end");
		fclose(pf);
		return -1;
	}*/

	fread(&encodeType, sizeof(encodeType), 1, pf);
	m_scriptEncodeType = (int)encodeType;

	fread(&compileTm, sizeof(compileTm), 1, pf);
	
	m_compileTm = lp_stream2host(compileTm, byteOrder);
	
	
	fread(&moduleSize, sizeof(moduleSize), 1, pf);
	moduleSize = lp_stream2host(moduleSize, byteOrder);

	if (moduleSize > sizeof(moudleName)) {
		nd_logerror("module name is too much, maybe file is demaged!\n") ;
		fclose(pf) ;
		return -1;
	}
	else if (moduleSize > 0) {
		fread(moudleName, moduleSize, 1, pf);
	}
	
	moudleName[moduleSize] = 0 ;
	NDUINT8 isGlobal = 0;
	script_func_map *pscripts = new script_func_map;
	if (!pscripts){
		fclose(pf);
		return -1;
	}
	script_func_map &script = *pscripts;
	scriptCmdBuf *pInitEntry = NULL;

	std::string lastFuncName;
	while (fread(&isGlobal, 1, 1, pf) > 0) {
		int readSzie = fread(&size, 1, sizeof(size), pf);
		size = lp_stream2host(size, byteOrder);

		nd_assert(size < fileSize);

		if (readSzie == sizeof(size) && size < fileSize) {
			scriptCmdBuf *pcmdbuf = (scriptCmdBuf*)malloc(sizeof(scriptCmdBuf) + size);
			if (pcmdbuf) {
				char *p = (char*)(pcmdbuf + 1);
				int read_buflen = (int)fread(p, 1, size, pf);

				pcmdbuf->cmdname = p;
				size_t namesize = strlen(p) + 1;
				pcmdbuf->buf = p + namesize;
				pcmdbuf->size = (int)(size - namesize);
				pcmdbuf->byteOrder = (int)byteOrder;

				lastFuncName = pcmdbuf->cmdname;

				if (isGlobal || !moudleName[0]){
					_pushGlobal(pcmdbuf);
					//script[std::string(pcmdbuf->cmdname)] = pcmdbuf;
				}
				else {
					script[std::string(pcmdbuf->cmdname)] = pcmdbuf;
				}
				
				if (strcmp(DEFAULT_LOAD_INITILIZER_FUNC,pcmdbuf->cmdname)==0) {
					pInitEntry = pcmdbuf;
				}
			}
		}
		else {
			nd_logerror("load script error \n");
			fclose(pf);
			return -1;
		}
	}
	fclose(pf);
	
	if (moudleName[0]) {
		setDftScriptModule(moudleName);
		m_modules[moudleName] = pscripts;
	}
	if (pInitEntry) {
		DBLDataNode result;
		if (!loader)	{
			loader = &m_globalParser;
		}

		if (!loader->_runCmdBuf(moudleName, pInitEntry, 0)) {
			nd_logerror("run %s error %d\n", DEFAULT_LOAD_INITILIZER_FUNC, loader->getErrno());
			return -1;
		}
	}

	return 0;
}

const char* LogicEngineRoot::_convertFuncName(const char *text, int inputEncodeType)
{
	const char*ret;
	static __ndthread  char _s_funcNameBuf[128];
	if (inputEncodeType == m_scriptEncodeType || inputEncodeType == E_SRC_CODE_ANSI) {
		ret = text;
	}
	else {
		ret = _s_funcNameBuf;
		if (m_scriptEncodeType == E_SRC_CODE_UTF_8)	{
			nd_gbk_to_utf8(text, _s_funcNameBuf, (int)sizeof(_s_funcNameBuf));
		}
		else if (inputEncodeType == E_SRC_CODE_UTF_8){
			nd_utf8_to_gbk(text, _s_funcNameBuf, (int)sizeof(_s_funcNameBuf));
		}
		else {
			ret = text;
		}
		
	}
	return ret;
}
int LogicEngineRoot::Reload(const char *scriptStream, LogicParserEngine *loader)
{
	unloadScript();
	return LoadScript(scriptStream,loader);
}

int LogicEngineRoot::unloadScript()
{
	script_module_map::iterator it;
	for (it = m_modules.begin(); it != m_modules.end(); ++it){
		if (it->second)	{
			_unloadModule(*(it->second));
			delete it->second;
			it->second = 0;
		}
	}
	m_modules.clear();
	m_dftScriptModule.clear();
	return 0;
}

int LogicEngineRoot::_unloadModule(script_func_map &module)
{
	script_func_map::iterator it;
	for (it = module.begin(); it != module.end(); ++it){
		if (it->second)	{
			free(it->second);
			it->second = 0;
		}
	}
	return 0;
}

bool LogicEngineRoot::_pushGlobal(scriptCmdBuf *pcmdbuf)
{
	script_module_map::iterator it = m_modules.find(GLOBAL_MODULE_NAME);
	if (it == m_modules.end()){
		script_func_map *pscripts = new script_func_map; 
		(*pscripts)[std::string(pcmdbuf->cmdname)] = pcmdbuf;
		m_modules[GLOBAL_MODULE_NAME] = pscripts;
	}
	else {
		(*(it->second))[std::string(pcmdbuf->cmdname)] = pcmdbuf;
	}
	return true;
}

void LogicEngineRoot::setDftScriptModule(const char *script)
{
	m_dftScriptModule = script ;
}


const char *LogicEngineRoot::getEventFunc(int event_id)const
{
	event_table_map::const_iterator it = m_event_entry.find(event_id);
	if (it == m_event_entry.end()){
		return NULL;
	}
	return it->second.c_str();
}

logicParser_func LogicEngineRoot::getCPPFunc(const char *funcName) const
{
	std::string realName ;
	if (0 != strncmp(funcName, "CPP.",4)) {
		realName = "CPP." ;
		realName += funcName ;
	}
	else {
		realName = funcName ;
	}
	
	//find in c-function-list
	cpp_func_map::const_iterator it =  m_c_funcs.find(realName);
	if (it!=m_c_funcs.end()) {
		
		return it->second.func;
	}
	
	return NULL;
}

void LogicEngineRoot::installFunc(logicParser_func func, const char *name, const char *comment)
{
	if (!name || !name[0]){
		return;
	}
#if (ND_ENCODE_TYPE!=E_SRC_CODE_UTF_8)
	char func_namebuf[128];
	char func_descbuf[128];
	name = nd_gbk_to_utf8(name, func_namebuf, sizeof(func_namebuf));
	comment = nd_gbk_to_utf8(comment, func_descbuf, sizeof(func_descbuf));
#endif
	

	func_cpp_info node;
	node.comment = comment;
	node.func = func;
	
	if (strncmp(name, "CPP.", 4)!=0) {
		std::string realName = "CPP." ;
		realName += name ;
		m_c_funcs[realName] = node;
	}
	else {
		m_c_funcs[std::string(name)] = node;
	}
}

void LogicEngineRoot::installEvent(int event_id, const char *event_func)
{
	if (!event_func || !event_func[0] || !event_id){
		return;
	}
	m_event_entry[event_id] = std::string(event_func);
}


const scriptCmdBuf* LogicEngineRoot::_findScript(const char *funcName, const char *moduleName, const char**outModuleName) const
{
	script_module_map::const_iterator module_it = m_modules.find(moduleName);
	if (module_it == m_modules.end()){
		return NULL;
	}
	script_func_map *pfuncMap = module_it->second;
	if (!pfuncMap){
		return NULL;
	}

	script_func_map::const_iterator itscript = pfuncMap->find(std::string(funcName));
	if (itscript != pfuncMap->end()) {
		if (outModuleName ) {
			*outModuleName = module_it->first.c_str();
		}
		return (itscript->second);
	}
	else {
		return  NULL;
	}
}
const scriptCmdBuf* LogicEngineRoot::getScript(const char *funcName, const char *moduleName, const char**outModuleName)const
{
	const char *p = strchr(funcName, '.');
	if (p){
		const char *realname = ++p;
		const char *realModule;
		char module[128];

		realModule = ndstr_nstr_end(funcName, module, '.', sizeof(module));
		return _findScript(realname, realModule, outModuleName);
	}
	else if (moduleName && *moduleName) {
		return _findScript(funcName, moduleName, outModuleName);
	}
	else {
		const scriptCmdBuf*pCmd = _findScript(funcName, GLOBAL_MODULE_NAME,NULL);
		if (pCmd){
			if (outModuleName) { *outModuleName = NULL; }
			return pCmd;
		}
		if (m_dftScriptModule.size()){
			return _findScript(funcName, m_dftScriptModule.c_str(), outModuleName);
		}
	}
	return NULL;

}


int LogicEngineRoot::dumbCPPfunc(const char *outXmlfile)
{
	ndxml_root xmlroot;
	ndxml_initroot(&xmlroot);
	ndxml_addattrib(&xmlroot, "version", "1.0");
	ndxml_addattrib(&xmlroot, "encoding", "utf8");
	ndxml *xml_funcs = ndxml_addnode(&xmlroot, LOGIC_FUNCTION_LIST_NAME, NULL);
	nd_assert(xml_funcs);
	if (!xml_funcs)	{
		return -1;
	}
	for (cpp_func_map::iterator it = m_c_funcs.begin(); it != m_c_funcs.end(); it++) {

		ndxml *xmlsub = ndxml_addnode(xml_funcs, "node", it->first.c_str());
		if (xmlsub)	{
			ndxml_addattrib(xmlsub, "desc", it->second.comment.c_str());
		}
	}
	 
	return ndxml_save(&xmlroot, outXmlfile);
}
// int LogicEngineRoot::run_init_script( )
// {
// 	DBLDataNode result;
// 
// 	if (!m_globalParser.runScript(E_SRC_CODE_ANSI, "script_system_init", result, 0)) {
// 		nd_logerror("run script_system_init error %d\n", m_globalParser.getErrno());
// 		return -1;
// 	}
// 	return 0;
// }
int LogicEngineRoot::test()
{
	LogicParserEngine &tmpEngine = m_globalParser;
	tmpEngine.setSimulate(true);

// 	if (-1==run_init_script()) {
// 		nd_logerror("run script_system_init error %d\n", tmpEngine.getErrno());
// 	}
	
	script_module_map::const_iterator it;
	tmpEngine.eventNtf(1, 0);
	for (it = m_modules.begin(); it != m_modules.end(); ++it){

		const script_func_map &funcs = *(it->second);
		for (script_func_map::const_iterator it_script = funcs.begin(); it_script != funcs.end(); it_script++){
			bool ret = tmpEngine._runCmdBuf(it->first.c_str(), it_script->second, 0);
			if (!ret)	{
				int runerror = tmpEngine.getErrno();
				if (runerror == LOGIC_ERR_AIM_OBJECT_NOT_FOUND || runerror == LOGIC_ERR_WOULD_BLOCK || runerror >= LOGIC_ERR_USER_DEFINE_ERROR) {
					continue;
				}
				nd_logerror("test script func %s error \n", it_script->second->cmdname);
				return -1;
			}
		}
	}

	for (int i = 3; i < 100; i++){
		tmpEngine.eventNtf(i, 0);
	}
	
	tmpEngine.eventNtf(2, 0);
	tmpEngine.setSimulate(false);
	return 0;
}

const char *LogicEngineRoot::getCompileTime(DBLDataNode &result)
{
	char buf[128];	
	nd_get_datetimestr_ex(m_compileTm, buf, 128);
	result.InitSet(buf);
	return result.GetText();
}

logic_print LogicEngineRoot::setPrint(logic_print func, void *outfile)
{
	logic_print ret = m_screen_out_func;
	m_screen_out_func = func;
	m_print_file = outfile;
	return ret;
}
