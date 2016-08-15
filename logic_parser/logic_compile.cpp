/* file logic_compile.cpp
 *
 * comile xml game logic to binary stream
 *
 * create by duan
 *
 * 2015-4-30
 */
#include "logic_compile.h"
#include "logicParser.h"
#include "logicEngineRoot.h"
#include "logicDataType.h"
#include "nd_net/byte_order.h"
#include <vector>
#include <time.h>
//#include ""
#define REVERT_INSTRUCT 0x7f7f7f7f

#include "logic_editor_helper.h"
using namespace LogicEditorHelper;

/////////////////////////////

LogicCompiler::LogicCompiler() :m_bDebugInfo(false), m_compileStep(0), m_curRoot(0)
{
}

LogicCompiler::~LogicCompiler()
{
	ndxml_destroy(&m_configRoot);
	m_enumText.clear();
}


bool LogicCompiler::setConfigFile(const char *config)
{
	ndxml_initroot(&m_configRoot);
	if (ndxml_load(config, &m_configRoot)) {
		return false;
	}
	ndxml *node = ndxml_getnode(&m_configRoot, "compile_setting");
	if (!node)	{
		nd_logerror("open compile setting %s file error please check this setting\n", config);
		return false;
	}
	for (int i = 0; i < ndxml_num(node); i++) {
		const char *p;
		compile_setting setnode;
		ndxml *sub_set = ndxml_getnodei(node, i);
		setnode.name = ndxml_getval(sub_set);
		p = ndxml_getattr_val(sub_set, "instruction");
		if (p)	{
			setnode.ins_type = atoi(p);
		}

		p = ndxml_getattr_val(sub_set, "ins_id");
		if (p)	{
			setnode.ins_id = atoi(p);
		}

		p = ndxml_getattr_val(sub_set, "size");
		if (p)	{
			setnode.size = atoi(p);
		}

		p = ndxml_getattr_val(sub_set, "datatype");
		if (p)	{
			setnode.data_type = atoi(p);
		}
		p = ndxml_getattr_val(sub_set, "record_param_num");
		if (p)	{
			setnode.record_param_num = atoi(p);
		}
		p = ndxml_getattr_val(sub_set, "need_type_stream");
		if (p)	{
			setnode.need_type_stream = atoi(p);
		}
		m_settings.insert(std::make_pair(setnode.name, setnode));
	}
	//get enum text
	node = ndxml_getnode(&m_configRoot, "data_type_define");
	if (node){
		_loadEnumText(node);
	}
	return true;
}


int LogicCompiler::_loadEnumText(ndxml *dataTypeDef)
{
	for (int i = 0; i < ndxml_num(dataTypeDef); i++) {
		const char *p;
		ndxml *node = ndxml_getnodei(dataTypeDef, i);
		nd_assert(node);
		p = ndxml_getattr_val(node, "kinds");
		if (!p || !*p){
			continue;
		}
		if (0!=ndstricmp(p, "enum")){
			continue;
		}
		enumTextVct textvals;
		int num = _getEnumText(node, textvals);
		if (num==0)	{
			continue;
		}
		std::string name = ndxml_getname(node);
		m_enumText.insert(std::make_pair(name, textvals));
	}
	return (int)m_enumText.size();
}

int LogicCompiler::_getEnumText(ndxml * node, enumTextVct &enumText)
{
	const char *pEnumType = (char*)ndxml_getattr_val(node, "enum_type");
	if (!pEnumType || !*pEnumType){
		return 0;
	}
	if (0 != ndstricmp(pEnumType, "text")){
		return 0;
	}

	char *pEnumTexts = (char*)ndxml_getattr_val(node, "enum_text");
	if (!pEnumTexts || !*pEnumTexts) {
		return 0;
	}

	char subtext[128];
	do 	{
		subtext[0] = 0;
		pEnumTexts = (char*)ndstr_nstr_end(pEnumTexts, subtext, ',', sizeof(subtext));
		if (subtext[0])	{
			enumText.enumTextVals.push_back(std::string(subtext));
		}
		if (pEnumTexts && *pEnumTexts == ',') {
			++pEnumTexts;
		}
	} while (pEnumTexts && *pEnumTexts);
	return (int) enumText.enumTextVals.size();
}


bool LogicCompiler::_isFileInfo(ndxml * node)
{
	const char *name = ndxml_getname(node) ;
	if (name && 0==strcmp(name, "moduleInfo") ) {
		return true ;
	}
	return  false ;
}

int LogicCompiler::_writeFileInfo(ndxml *module, FILE *pf)
{
	ndxml *xmlModule = ndxml_getnode(module, "moduleInfo");
	if (!xmlModule) {
		return -1;
	}
	ndxml *node = ndxml_getnode(xmlModule, "moduleName");
	if (node) {
		const char *p = ndxml_getval(node) ;
		if (p) {
			NDUINT16 size = (NDUINT16) strlen(p) ;
			fwrite(&size, sizeof(size), 1, pf) ;
			fwrite(p, size, 1, pf) ;
			m_moduleName = p;
			return  size;
		}
	}

	NDUINT16 size = 0;
	fwrite(&size, sizeof(size), 1, pf);
	return 0;
}
bool LogicCompiler::_isGlobalFunc(ndxml *funcNode)
{
	//int ret = 0;
	//check function is global
	NDUINT8 bGlobal = 0;
	ndxml *xmlComment = ndxml_getnode(funcNode, "comment");
	if (xmlComment)	{
		ndxml *xmlIsGlobal = ndxml_getnode(xmlComment, "isGlobal");
		if (xmlIsGlobal && ndxml_getval_int(xmlIsGlobal)) {
			bGlobal = 1;
		}
	}
	return bGlobal ? true : false;
}


const char* LogicCompiler::_getNodeText(ndxml *paramNode, char *buf, size_t bufsize)
{
	const char *p = ndxml_getattr_val(paramNode, "kinds");
	if (!p || !*p){
		return getRealValFromStr(ndxml_getval(paramNode), buf, bufsize);
	}

	if (0 == ndstricmp(p, "enum")){

		enumTextVct textvals;
		int num = _getEnumText(paramNode, textvals);
		if (num > 0)	{
			int index = ndxml_getval_int(paramNode);
			if (index < textvals.enumTextVals.size()) {
				strncpy(buf, textvals.enumTextVals[index].c_str(), bufsize);
				return buf;
			}
		}

	}
	else if (0 == ndstricmp(p, "reference")){
		p = ndxml_getattr_val(paramNode, "reference_type");
		if (p && *p){
			enum_textValue_t::iterator it = m_enumText.find(p);
			if (it != m_enumText.end() ) {
				int index = ndxml_getval_int(paramNode);
				if (index < it->second.enumTextVals.size()) {
					strncpy(buf, it->second.enumTextVals[index].c_str(), bufsize);
					return buf;
				}
				return NULL;
			}
		}
	}
	
	return getRealValFromStr(ndxml_getval(paramNode), buf, bufsize);
}

bool LogicCompiler::compileXml(const char *xmlFile, const char *outStreamFile, int outEncodeType,bool withDgbInfo)
{
// 	const char *pOutType = "gbk";
// 	if (encodeType == E_SRC_CODE_UTF_8) {
// 		pOutType = "utf8";
// 	}
	const char *pOutType = nd_get_encode_name(outEncodeType);
	ndxml_root xmlroot;
	char buf[1024*64];
	char init_buf[1024*64];

	m_initBlockSize = sizeof(init_buf);
	m_pInitBlock = init_buf;
	//ndxml_initroot(&xmlroot);

	if (ndxml_load_ex(xmlFile, &xmlroot, pOutType)) {
		nd_logerror("input file %s error\n", xmlFile);
		return false;
	}
	m_bDebugInfo = withDgbInfo;
	m_curRoot = &xmlroot;

	m_cur_file = xmlFile;

	FILE *pf = fopen(outStreamFile, "wb");
	if (!pf){
		nd_logerror("can not open file %s \n", outStreamFile);
		return false;
	}

	NDUINT8 val1;
	//byte order
	val1 = nd_byte_order();
	fwrite(&val1, sizeof(val1), 1, pf);
	//encode type
	val1 = (NDUINT16) outEncodeType;
	fwrite(&val1, sizeof(val1), 1, pf);

	//write compile time 
	NDUINT64 compile_t64 = (NDUINT64)time(NULL);
	fwrite(&compile_t64, sizeof(compile_t64), 1, pf);

	_writeFileInfo(&xmlroot, pf);	

	for (int x = 0; x < ndxml_num(&xmlroot); x++){
		ndxml *funcList = ndxml_getnodei(&xmlroot, x);
		if (_isFileInfo(funcList)) {
			continue ;
		}

		for (int i = 0; i < ndxml_num(funcList); i++) {
			ndxml *node = ndxml_getnodei(funcList, i);
			memset(buf, 0xf3, sizeof(buf));
			int ret = func2Stream(node, buf, sizeof(buf));
			if (-1 == ret || ret > sizeof(buf))	{
				//const char *name = ndxml_getval(node);
				//nd_logerror("compile function %s error \n", name ? name: "unknow");
				return false;
			}
			NDUINT8 isGlobal = _isGlobalFunc(node) ? 1 : 0; 
			fwrite(&isGlobal, 1, 1, pf);
			fwrite(&ret, 1, sizeof(ret), pf);
			if (fwrite(buf, 1, ret, pf) <= 0)	{
				return false;
			}
		}
	}
	//write init block 
	if (m_initBlockSize != sizeof(init_buf)){
		const char *initBlocName = DEFAULT_LOAD_INITILIZER_FUNC;
		int namesize = (int)strlen(initBlocName) + 1;
		NDUINT32 size = namesize + (int)(m_pInitBlock - init_buf);
		NDUINT8 isglobal = 0;
		fwrite(&isglobal, 1, 1, pf);

		fwrite(&size, 1, sizeof(size), pf);
		fwrite(initBlocName, 1, namesize, pf);
		if (fwrite(init_buf, 1, size-namesize, pf) <= 0)	{
			return false;
		}
	}

	m_curRoot = 0;
	ndxml_destroy(&xmlroot);
	fflush(pf);
	fclose(pf);
	return true;
}

int LogicCompiler::func2Stream(ndxml *funcNode, char *buf, size_t bufsize)
{
	char *p = buf;
	
	const char *pFuncName = ndxml_getattr_val(funcNode, "name");
	int len =(int) strlen(pFuncName) + 1;
	strncpy(buf, pFuncName, bufsize);
	p += len;
	bufsize -= len;
	
	m_compileStep = 0;
	
	m_cur_function = pFuncName;
	m_cur_node_index = 0;


	len = trytoCompileExceptionHandler(funcNode, p, bufsize);
	if (len > 0){
		p += len;
		bufsize -= len;
	}

	len = trytoCompileInitilizerBlock(funcNode, m_pInitBlock, m_initBlockSize);
	if (len > 0){
		m_pInitBlock += len;
		m_initBlockSize -= len;
	}


	len = blockSteps2Stream(funcNode, p, bufsize);
	if (-1 == len)	{
		return -1;
	}
	p += len;
	
	return  (int)(p - buf);
}

int LogicCompiler::subEntry2Stream(compile_setting *stepSetting, ndxml *subEntryNode, char *buf, size_t bufsize)
{
	
	std::vector<blockStrem> blocks;

	size_t taotl_size = 0;
	for (int i = 0; i < ndxml_getsub_num(subEntryNode); i++) {
		blockStrem node;
		char *p = node.block_buf;
		NDUINT32 *pLocalJumpOffset=0;
		ndxml *blockXml = ndxml_getnodei(subEntryNode, i);
		
		compile_setting_t::iterator it = m_settings.find(ndxml_getname(blockXml));
		if (it != m_settings.end() && it->second.ins_type == E_INSTRUCT_TYPE_COMMENT){
			continue;
		}

		const char *pCompCondName = ndxml_getattr_val(blockXml, "comp_cond");
		const char *pCompValName = ndxml_getattr_val(blockXml, "comp_value");
		nd_assert(pCompCondName);
		//nd_assert(pCompValName);

		ndxml *cmpNode = ndxml_getnode(blockXml, pCompCondName);
		if (cmpNode){
			const char *pVal = ndxml_getattr_val(cmpNode, "no_comp");
			if(pVal && pVal[0]=='1'){
				node.no_comp = 1;
			}
			node.cmp_instruct = ndxml_getval_int(cmpNode);
		}		
		node.node_index = i;
		//insert jump to current lock end size cmd
		if (node.cmp_instruct){
			if (!node.no_comp)	{
				ndxml *cmpvalXml = ndxml_getnode(blockXml, pCompValName);
				if (!cmpvalXml)	{
					nd_logerror("compile if-else error, perhaps file is damage\n");
					return -1;
				}
				node.cmp_val = ndxml_getval(cmpvalXml);
				//add test instruction
				*((*(NDUINT32**)&p)++) = (NDUINT32)E_OP_COMP;
				*((*(NDUINT32**)&p)++) = (NDUINT32)node.cmp_instruct;
				int len = param2Stream(cmpvalXml, blockXml, p, bufsize);
				if (len == -1)	{
					return -1;
				}
				p += len;

			}

			//test false and jump next block
			*((*(NDUINT32**)&p)++) = (NDUINT32)E_OP_TEST_FALSE_SHORT_JUMP;
			pLocalJumpOffset = (NDUINT32*) p;
			*((*(NDUINT32**)&p)++) = (NDUINT32)REVERT_INSTRUCT; //jump the if{..} end
		}


		int len = blockSteps2Stream(blockXml, p, sizeof(node.block_buf));
		if (-1 == len)	{
			return -1;
		}

		p += len;
		if (node.cmp_instruct){
			//this is not last end block ,need to jump to end 
			*((*(NDUINT32**)&p)++) = E_OP_SHORT_JUMP;
			node.jump_end_offset = (NDUINT32 ) (p - node.block_buf);
			*((*(NDUINT32**)&p)++) = REVERT_INSTRUCT;

			*pLocalJumpOffset = (NDUINT32)(len + 8);
		}

		node.size = (int)(p - node.block_buf);
		blocks.push_back(node);
		taotl_size += node.size;
	}
	if (blocks.size()==0) {
		return 0;
	}
	
	//link if-else{} blocks
	char *p = buf;
	int len = (int)bufsize;
	for (int i = 0; i < blocks.size(); i++) {
		blockStrem &node = blocks[i];
		if (node.size >= len) {
			return -1;
		}
		if (node.jump_end_offset){
			NDUINT32*jump_offset_val = (NDUINT32*)(node.block_buf + node.jump_end_offset);
			*jump_offset_val = (NDUINT32)(taotl_size - node.size);
			taotl_size -= node.size;
		}
		memcpy(p, node.block_buf, node.size);
		p += node.size;
		len = (int)(bufsize - (buf - p));
	}
	return (int)(p - buf);
}

//compile loop
int LogicCompiler::subLoop2Stream(compile_setting *setting, ndxml *loopSteps, char *buf, size_t bufsize)
{
	int ret = 0;
	char *p = buf;
	int len = (int)bufsize;

	//compile loop count
	compile_setting loop_setting;
	loop_setting.ins_id = E_OP_SET_COUNT_REG;
	loop_setting.ins_type = E_INSTRUCT_TYPE_CMD;

	const char *pCompCondName = ndxml_getattr_val(loopSteps, "comp_cond");
	if (!pCompCondName)	{
		nd_logerror("loop need loop times setting\n");
		return -1;
	}

	ndxml *cmpNode = ndxml_getnode(loopSteps, pCompCondName);
	if (!cmpNode){
		nd_logerror("loop need loop times xmlnode short\n");
		return -1;
	}

	ret = step2Strem(&loop_setting, cmpNode, p, len);
	if (ret == -1) 	{
		nd_logerror(" compile loop times xmlnode error\n");
		return -1;
	}
	p += ret;
	len -= ret;

	//compile cmd in this loop
	ret = blockSteps2Stream(loopSteps, p, len);
	if (ret == -1) 	{
		return -1;
	}
	p += ret;
	len -= ret;

	int offset = ret + 8;
	//add loop jump instruction 
	*((*(NDUINT32**)&p)++) = (NDUINT32)E_OP_TEST_COUNT_JUMP;
	*((*(NDUINT32**)&p)++) = (NDUINT32)(-offset); //jump offset 

	return (int) (p - buf);
}


int LogicCompiler::trytoCompileExceptionHandler(ndxml *funcNode, char *buf, size_t bufsize)
{
	int ret = 0;	
	for (int i = 0; i < ndxml_getsub_num(funcNode); i++) {
		ndxml *xmlStep = ndxml_getnodei(funcNode, i);
		if (!xmlStep) {
			continue;
		}
		const char *stepInsName = ndxml_getname(xmlStep);
		if (!stepInsName){
			continue;
		}

		compile_setting_t::iterator it = m_settings.find(stepInsName);
		if (it == m_settings.end()){
			continue;
		}

		if (it->second.ins_type == E_INSTRUCT_TYPE_EXCEPTION_CATCH) {
			char *p = buf;
			*((*(NDUINT32**)&p)++) = E_OP_EXCEPTION_BLOCK;

			NDUINT16 *size_addr = (NDUINT16 *)p;
			*((*(NDUINT16**)&p)++) = 0;

			int len = blockSteps2Stream(xmlStep, p, bufsize);
			p += len;
			bufsize -= len;
			//write exit instant
			*((*(NDUINT32**)&p)++) = (NDUINT32)E_OP_EXIT;
			*((*(NDUINT32**)&p)++) = (NDUINT32)0; //return false 
			
			ret = p - buf;
			*size_addr = (NDUINT16)(p - (char*)size_addr) - sizeof(NDUINT16);
#ifdef ND_DEBUG
			char *p1 = buf;
			*((*(NDUINT32**)&p1)++);
			NDUINT16 jump_len = *((*(NDUINT16**)&p1)++);
			nd_assert(*size_addr == jump_len);
			p1 += jump_len;
			nd_assert(p1 == p);
#endif
			break ;
		}
	}
	return ret;
}


int LogicCompiler::trytoCompileInitilizerBlock(ndxml *funcNode,char *buf, size_t bufsize)
{
	int ret = 0;
	char *p = buf;
	for (int i = 0; i < ndxml_getsub_num(funcNode); i++) {
		ndxml *xmlStep = ndxml_getnodei(funcNode, i);
		if (!xmlStep) {
			continue;
		}
		const char *stepInsName = ndxml_getname(xmlStep);
		if (!stepInsName){
			continue;
		}

		compile_setting_t::iterator it = m_settings.find(stepInsName);
		if (it == m_settings.end()){
			continue;
		}

		if (it->second.ins_type == E_INSTRUCT_TYPE_INIT_BLOCK) {
			
			int len = blockSteps2Stream(xmlStep, p, bufsize);
			p += len;
			bufsize -= len;
			
			ret = p - buf;
			break;
		}
	}
	return ret;
}

int LogicCompiler::blockSteps2Stream(ndxml *blockNode, char *buf, size_t bufsize)
{
	int ret = 0;
	char *p = buf;
	int len = (int)bufsize;

	for (int i = 0; i < ndxml_getsub_num(blockNode); i++) {
		ndxml *xmlStep = ndxml_getnodei(blockNode, i);
		if (!xmlStep) {
			continue;
		}
		const char *stepInsName = ndxml_getname(xmlStep);
		if (!stepInsName){
			continue;
		}

		compile_setting_t::iterator it = m_settings.find(stepInsName);
		if (it == m_settings.end()){
			continue;
		}
		ret = 0;
		if (it->second.ins_type == E_INSTRUCT_TYPE_COMMENT || 
			it->second.ins_type==E_INSTRUCT_TYPE_EXCEPTION_CATCH ||
			it->second.ins_type == E_INSTRUCT_TYPE_INIT_BLOCK) {
			continue; 
		}
		else if (it->second.ins_type == E_INSTRUCT_TYPE_CMD) 	{
			ret = step2Strem(&it->second, xmlStep, p, len);
			if (ret > 0){

			}
			else {
				const char *pName = ndxml_getattr_val(xmlStep, "name");
				if (!pName) {
					nd_logerror("compile node %s error \n", pName);
				}
			}
		}
		else if (it->second.ins_type == E_INSTRUCT_TYPE_SUB_ENTRY) 	{
			ret = subEntry2Stream(&it->second, xmlStep, p, len);
		}
		else if (it->second.ins_type == E_INSTRUCT_TYPE_LOOP) {
			ret = subLoop2Stream(&it->second, xmlStep, p, len);
		}
		else if (it->second.ins_type == E_INSTRUCT_TYPE_USER_DEFINE) {
			ret = user_define_step(&it->second, xmlStep, p, len);
		}
		else if (it->second.ins_type == E_INSTRUCT_TYPE_FUNCTION_INFO) {

		}

		if (-1 == ret) {
			return -1;
		}
		p += ret;
		len -= ret;
		if (len <= 0)	{
			return -1;
		}
	}
	return (int)(bufsize - len);
}

int LogicCompiler::step2Strem(compile_setting *stepSetting, ndxml *stepNode, char *buf, size_t bufsize)
{
	char *p = buf;
	int len = (int)bufsize;
	NDUINT32 *cur_step_size = 0; //record this step size in debug block 
	NDUINT32 _reverd = 0;

	//get current step name 
	m_cur_node_index++;
	const char *pStepName = ndxml_getattr_val(stepNode, "name");
	if (!pStepName) {
		pStepName = ndxml_getname(stepNode);
		if (!pStepName)	{
			return	-1;
		}
	}
	m_cur_step = pStepName;

	if (m_bDebugInfo) {
		int ret = writeDebugInfo(stepNode, pStepName, p, len);
		if (ret > 0){
			len -= ret;
			p += ret;
			cur_step_size = (NDUINT32*) p;
			*((*(NDUINT32**)&p)++) = 0;
			len -= sizeof(NDUINT32);

		}
		else {
			return -1;
		}
	}

	*((*(NDUINT32**)&p)++) = (NDUINT32) stepSetting->ins_id;

	NDUINT32 *pArgNum;
	if (stepSetting->record_param_num){
		pArgNum = (NDUINT32*)p;
		p += sizeof(NDUINT32);
		len -= sizeof(NDUINT32);
		*pArgNum = 0;
	}
	else {
		pArgNum = &_reverd;
	}

	for (int i = 0; i < ndxml_getsub_num(stepNode); i++) {
		ndxml *xmlParam = ndxml_getnodei(stepNode, i);
		if (!xmlParam) {
			continue;
		}
		int ret = param2Stream(xmlParam, stepNode, p, len, pArgNum);
		if (-1==ret)	{
			return -1;
		}
		p += ret;
		len -= ret;

		//++*pArgNum;
	}

	if (cur_step_size) {
		*cur_step_size = (NDUINT32)(p - (char*)cur_step_size) - 4;
	}
	return (int)(p - buf);
}
#define WRITE_STRING_TOSTREAM(_p,_text)  do {\
	if (!_text || !_text[0]) {					\
		nd_logerror("text is  none or null\n");	\
		return -1;								\
	}											\
	if (0 == ndstricmp(_text, "none") || 0 == ndstricmp(_text, "null")) {	\
		nd_logerror("text is  none or null\n");	\
		return -1;	\
	}				\
	NDUINT16 size =(NDUINT16) strlen(_text);			\
	*((*(NDUINT16**)&_p)++) = size;			\
	if (size > 0)							\
		memcpy(_p, _text, size );			\
	_p += size ;							\
}while (0)
//user define step
int LogicCompiler::user_define_step(compile_setting *setting, ndxml *stepNode, char *buf, size_t bufsize)
{
	return 0;
// 	int ret = 0;
// 	const char *type = ndxml_getattr_val(stepNode, "name");
// 	if (!type || !type[0]){
// 		return 0;
// 	}
// 
// 	char *p = buf;
// 	int len = (int)bufsize;
// 
// 	if (0 == ndstricmp((char*)type, (char*)"init_msg_func") ) {
// 		ret = user_define_msg_init(stepNode, p, len);
// 	}
// 	else if (0 == ndstricmp((char*)type, (char*)"init_event_func")) {
// 		ret = user_define_event_init(stepNode, p, len);
// 	}
// 
// 	if (ret != -1){
// 		p += ret;
// 		len -= ret;
// 	}
// 	return (int)(p - buf);
}
int LogicCompiler::step_function_info(compile_setting *setting, ndxml *stepNode, char *buf, size_t bufsize)
{
	char *p = buf;
	int len = (int)bufsize;
	const char *pStepName = ndxml_getname(stepNode);
	const char *pValOrg = ndxml_getval(stepNode);
	if (!pStepName || !pValOrg || *pValOrg)	{
		return	0;
	}
	char infotext[1024];

	char tmpbuf[4096];
	const char *pVal = getRealValFromStr(pValOrg, infotext, sizeof(infotext));

	snprintf(tmpbuf, sizeof(tmpbuf), "%s=%s;", pStepName, pVal);

	*((*(NDUINT32**)&p)++) = E_OP_HEADER_INFO;

	*((*(NDUINT16**)&p)++) = OT_STRING;
	WRITE_STRING_TOSTREAM(p, tmpbuf); //function name 

	return (int)(p - buf);
}

int LogicCompiler::user_define_msg_init(ndxml *stepNode, char *buf, size_t bufsize)
{
	char *p = buf;
	int len = (int)bufsize;

	m_cur_function = "install_message_handler";
	const char *pMsgFuncName = ndxml_getattr_val(stepNode, "install_root");
	const char *callFuncName = ndxml_getattr_val(stepNode, "action");
	if (!pMsgFuncName || !pMsgFuncName[0] || !callFuncName)	{
		return 0;
	}
	ndxml *msg_entry = ndxml_getnode(m_curRoot, pMsgFuncName);
	if (!msg_entry)	{
		return 0;
	}
	m_cur_node_index = 0;
	for (int i = 0; i < ndxml_num(msg_entry); i++){
		++m_cur_node_index;
		ndxml *node = ndxml_getnodei(msg_entry, i);
		ndxml *msgID = ndxml_getnode(node, "install_msg_id");
		ndxml *msgPrivilege = ndxml_getnode(node, "msg_privilege");
		if (!msgID)	{
			continue;
		}
		const char *pScriptName = ndxml_getattr_val(node, "name");
		const char *pMsgName = ndxml_getval(msgID);
		m_cur_step = pScriptName;
		if (!pScriptName || !pScriptName[0] || !pMsgName || 0 == ndstricmp((char*)pMsgName, (char*)"none")){
			nd_logerror("install message %s handler error msg_id not define \n", pScriptName);
			return -1;
		}
		nd_assert(node);
		char msg_name_buf[128];
		pMsgName = getRealValFromStr(pMsgName, msg_name_buf, sizeof(msg_name_buf));
		//call function
		*((*(NDUINT32**)&p)++) = E_OP_CALL_FUNC;
		*((*(NDUINT32**)&p)++) = 4;

		*((*(NDUINT16**)&p)++) = OT_STRING;
		WRITE_STRING_TOSTREAM(p, callFuncName); //function name 

		*((*(NDUINT16**)&p)++) = OT_STRING;
		WRITE_STRING_TOSTREAM(p, pScriptName); //arg1 -script name 

		*((*(NDUINT16**)&p)++) = OT_STRING;
		WRITE_STRING_TOSTREAM(p, pMsgName); //arg2 - msg_id_name

		*((*(NDUINT16**)&p)++) = OT_INT;
		int valPrivilege = 0;
		if (msgPrivilege) {
			const char *pVal = ndxml_getval(msgPrivilege);
			if (pVal&& *pVal) {
				valPrivilege = atoi(pVal);
			}
		}
		*((*(NDUINT32**)&p)++) = valPrivilege;
	}
	return (int) (p - buf);
}
int LogicCompiler::user_define_event_init(ndxml *stepNode, char *buf, size_t bufsize)
{
	char *p = buf;
	int len = (int)bufsize;

	m_cur_function = "install_message_handler";
	const char *pMsgFuncName = ndxml_getattr_val(stepNode, "install_root");
	const char *callFuncName = ndxml_getattr_val(stepNode, "action");
	if (!pMsgFuncName || !pMsgFuncName[0] || !callFuncName)	{
		return 0;
	}
	ndxml *msg_entry = ndxml_getnode(m_curRoot, pMsgFuncName);
	if (!msg_entry)	{
		return 0;
	}
	m_cur_node_index = 0;
	for (int i = 0; i < ndxml_num(msg_entry); i++){
		++m_cur_node_index;
		ndxml *node = ndxml_getnodei(msg_entry, i);
		ndxml *eventID = ndxml_getnode(node, "event_id");
		if (!eventID)	{
			continue;
		}
		const char *pScriptName = ndxml_getattr_val(node, "name");
		const char *pMsgName = ndxml_getval(eventID);
		m_cur_step = pScriptName;
		if (!pScriptName || !pScriptName[0] || !pMsgName || 0 == ndstricmp((char*)pMsgName, (char*)"none")){
			nd_logerror("install message %s handler error msg_id not define \n", pScriptName);
			return -1;
		}
		nd_assert(node);
		char msg_name_buf[128];
		pMsgName = getRealValFromStr(pMsgName, msg_name_buf, sizeof(msg_name_buf));
		//call function
		*((*(NDUINT32**)&p)++) = E_OP_CALL_FUNC;
		*((*(NDUINT32**)&p)++) = 3;

		*((*(NDUINT16**)&p)++) = OT_STRING;
		WRITE_STRING_TOSTREAM(p, callFuncName); //function name 

		*((*(NDUINT16**)&p)++) = OT_STRING;
		WRITE_STRING_TOSTREAM(p, pScriptName); //arg1 -script name 

		*((*(NDUINT16**)&p)++) = OT_INT;		//event id
		*((*(NDUINT32**)&p)++) = atoi(pMsgName);

	}
	return (int)(p - buf);
}

int LogicCompiler::writeDebugInfo(ndxml *stepNode, const char*stepName, char *buf, size_t bufsize)
{
	char *p = buf;
// 	const char *pName = ndxml_getattr_val(stepNode, "name");
// 	if (!pName) {
// 		pName = ndxml_getname(stepNode);
// 		if (!pName)	{
// 			return	-1;
// 		}
// 	}
	*((*(NDUINT32**)&p)++) = E_OP_DEBUG_INFO ;
	*((*(NDUINT16**)&p)++) = m_compileStep++;

	WRITE_STRING_TOSTREAM(p, stepName);
// 	int _tmpsize = strlen(pName) ;
// 	if (_tmpsize > (bufsize - 8))	{
// 		return -1;
// 	}
// 	*((*(NDUINT16**)&p)++) = (NDUINT16)_tmpsize ;
// 	memcpy(p, pName, _tmpsize);
// 	p += _tmpsize;

	return (int)(p - buf);
}

int LogicCompiler::param2Stream(ndxml *xmlParam, ndxml *parent, char *buf, size_t bufsize, NDUINT32 *param_num)
{
	char *p = buf;
	NDUINT16 *paramType = NULL;
	int len = (int)bufsize;

	const char *paramName = ndxml_getname(xmlParam);
	if (!paramName){
		return 0;
	}
	compile_setting_t::iterator it = m_settings.find(paramName);
	if (it == m_settings.end()){
		return 0;
	}
	if (it->second.ins_type == E_INSTRUCT_TYPE_COMMENT) {
		return 0;
	}

	compile_setting *paramSetting = &it->second;
	if (paramSetting->ins_type == E_INSTRUCT_TYPE_PARAM_COLLECT){
		for (int i = 0; i < ndxml_num(xmlParam); i++) {
			ndxml *node1 = ndxml_getnodei(xmlParam, i);
			int ret = param2Stream(node1,xmlParam,  p, len, param_num);
			if (-1 == ret)	{
				return -1;
			}
			p += ret;
			len -= ret;
		}
		return (int)(p - buf);
	}
	//step 
	else if (paramSetting->ins_type == E_INSTRUCT_TYPE_CMD) {
		return step2Strem(paramSetting, xmlParam, p, len);
	}

	//get type 
	int type = -1;
	const char *restrictType = ndxml_getattr_val(xmlParam, "restrict");
	if (restrictType) {
		ndxml *xmlRestrict = ndxml_getnode(parent, restrictType);
		if (xmlRestrict) {
			type = ndxml_getval_int(xmlRestrict);
		}
	}
	if (-1 == type) {
		type = paramSetting->data_type;
	}

	//get realvalue

	char tmpbuf[4096];
	const char *pVal = _getNodeText(xmlParam, tmpbuf, sizeof(tmpbuf));
	if (!pVal || !*pVal){
		nd_logerror("paser %s error string value is empty\n", paramName);
		return -1;
	}

	//const char *pValOrg = ndxml_getval(xmlParam);	
	//const char *pVal = getRealValFromStr(pValOrg, tmpbuf, sizeof(tmpbuf));
	
	if (type < OT_LAST_RET || type == OT_TIME)	{
		if (0 == ndstricmp(pVal, "none") || 0 == ndstricmp(pVal, "null")) {
			nd_logerror("%s string value not none or null\n", paramName);
			return -1;
		}
	}

	if (paramSetting->need_type_stream){
		paramType = (NDUINT16*)p;
		*((*(NDUINT16**)&p)++) = type;
	}
	switch (type)
	{
	case  OT_INT :
	case OT_BOOL :
	case  OT_PARAM:
		*((*(NDUINT32**)&p)++) = pVal ? atoi(pVal) : 0;
		break;
	case OT_FLOAT: {
		float fv = pVal ? (float)atof(pVal) : 0;
		_write_float_to_buf(p, fv);
	}
		break;
	case OT_INT8:
		*((*(NDUINT8**)&p)++) = (NDUINT8)(pVal ? atoi(pVal) : 0);
		break;
	case OT_INT16:
		*((*(NDUINT16**)&p)++) = (NDUINT16)(pVal ? atoi(pVal) : 0);
		break;
	case OT_INT64:
		*((*(NDUINT64**)&p)++) = (NDUINT64)(pVal ? nd_atoi64(pVal) : 0);
		break; 

	case OT_TIME:
	{
		time_t timval=0;
		if ((time_t)-1==nd_time_from_str(pVal, &timval))	{
			nd_logerror("time %s format error in %s\n", pVal, paramName);
			return -1;
		}
		*((*(NDUINT64**)&p)++) = timval;
	}	
		break;
	case OT_USER_DEFINED:
	case OT_STRING:
	case OT_VARIABLE:
		if (!pVal || !pVal[0]){
			*((*(NDUINT16**)&p)++) = (NDUINT16)0;
		}
		else {
			WRITE_STRING_TOSTREAM(p, pVal);
		}
		break;

	case OT_LAST_RET:
		//nothing to write
		break;
	case OT_COMPILE_TIME:
		if (paramType)	{
			*paramType = OT_TIME;
		}
		*((*(NDUINT64**)&p)++) = (NDUINT64) time(NULL);
		break;
	case OT_FUNCTION_NAME:
	{
		if (paramType)	{
			*paramType = OT_STRING;
		}
		const char *Name = m_cur_function.c_str();
		WRITE_STRING_TOSTREAM(p, Name);
	}
		break;
	case OT_SCRIPT_MODULE_NAME:
	{
		if (paramType)	{
			*paramType = OT_STRING;
		}
		const char *Name = m_moduleName.c_str();
		WRITE_STRING_TOSTREAM(p, Name);
	}
		break;

	case OT_ARRAY:
		if (pVal){
			DBLDataNode convertData ;
			if (convertData.StringToArrayString(pVal)) {
				if (paramType){
					p -= sizeof(NDUINT16);
				}
				int size = convertData.WriteStream(p) ;
				//////////////////////////////////////////////////////////////////////////
				//DBLDataNode test1;
				//int size1 = test1.ReadStream(p);
				//nd_assert(test1 == convertData);
				//////////////////////////////////////////////////////////////////////////
				p += size ;
			}
			else {
				*((*(NDUINT16**)&p)++) = (NDUINT16)0;
			}
			/*int arr_size = 0;
			int input_array[100];
			
			char *text =(char*) ndstr_first_valid(pVal);
			while (text && *text){
				input_array[arr_size] = strtol(text, &text, 0);
				if (text && *text) {
					text = strchr(text, ',');
					if (text && *text == ',') {
						++text;
					}
				}
			}

			*((*(NDUINT16**)&p)++) = (NDUINT16)arr_size;
			if (arr_size){
				for (int x = 0; x < arr_size; x++) {
					*((*(NDUINT32**)&p)++) = input_array[x];
				}
			}*/
		}
		else {
			*((*(NDUINT16**)&p)++) = (NDUINT16)0;
		}
		break;
	default:
		nd_logerror("param %s type %d not suport \n", paramName,type);
		return -1;
		break;
	}

	if (param_num){
		++(*param_num);
	}
	return (int) (p - buf);
}
