/* file logic_compile.cpp
 *
 * comile xml game logic to binary stream
 *
 * create by duan
 *
 * 2015-4-30
 */

#include "logic_parser/logicEndian.h"
#include "logic_parser/logic_compile.h"
#include "logic_parser/logicParser.h"
#include "logic_parser/logicEngineRoot.h"
#include "logic_parser/logicDataType.h"
#include "nd_net/byte_order.h"
#include <vector>
#include <time.h>
//#include ""
#define REVERT_INSTRUCT 0x7f7f7f7f

#include "logic_editor_helper.h"
using namespace LogicEditorHelper;



logciCompileSetting::logciCompileSetting()
{
	ndxml_initroot(&m_configRoot);
}

logciCompileSetting::~logciCompileSetting()
{
}


bool logciCompileSetting::setConfigFile(const char *config, int encodeType )
{
	ndxml_initroot(&m_configRoot);

	if (ndxml_load_ex(config, &m_configRoot, nd_get_encode_name(encodeType))) {
		nd_logerror("open compile setting %s file error please check file exist\n", config);
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
			setnode.ins_type = (NDUINT8)ndstr_atoi_hex(p);
		}

		p = ndxml_getattr_val(sub_set, "ins_id");
		if (p)	{
			setnode.ins_id = (NDUINT8)ndstr_atoi_hex(p);
		}

		p = ndxml_getattr_val(sub_set, "size");
		if (p)	{
			setnode.size = ndstr_atoi_hex(p);
		}

		p = ndxml_getattr_val(sub_set, "datatype");
		if (p)	{
			setnode.data_type = (NDUINT8)ndstr_atoi_hex(p);
		}
		p = ndxml_getattr_val(sub_set, "record_param_num");
		if (p)	{
			setnode.record_param_num = (NDUINT8)ndstr_atoi_hex(p);
		}
		p = ndxml_getattr_val(sub_set, "need_type_stream");
		if (p)	{
			setnode.need_type_stream = (NDUINT8)ndstr_atoi_hex(p);
		}

		p = ndxml_getattr_val(sub_set, "need_refill_offset");
		if (p)	{
			setnode.need_refill_jump_len = (NDUINT8)ndstr_atoi_hex(p);
		}

		m_settings.insert(std::make_pair(setnode.name, setnode));
	}
	//get enum text
	node = ndxml_getnode(&m_configRoot, "data_type_define");
	if (node){
		_loadEnumText(node);
	}
	if (!onLoad(m_configRoot))	{
		return false;
	}
	return true;
}


const compile_setting* logciCompileSetting::getStepConfig(const char *stepName)const
{
	compile_setting_t::const_iterator it = m_settings.find(stepName);
	if (it == m_settings.end())	{
		return NULL;
	}
	return &(it->second);
}

int logciCompileSetting::_loadEnumText(ndxml *dataTypeDef)
{
	for (int i = 0; i < ndxml_num(dataTypeDef); i++) {
		const char *p;
		ndxml *node = ndxml_getnodei(dataTypeDef, i);
		nd_assert(node);
		p = ndxml_getattr_val(node, "kinds");
		if (!p || !*p){
			continue;
		}
		if (0 != ndstricmp(p, "enum")){
			continue;
		}
		enumTextVct textvals;
		int num = _getEnumText(node, textvals);
		if (num == 0)	{
			continue;
		}
		std::string name = ndxml_getname(node);
		m_enumText.insert(std::make_pair(name, textvals));
	}
	return (int)m_enumText.size();
}

int logciCompileSetting::_getEnumText(ndxml * node, enumTextVct &enumText)
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
	return (int)enumText.enumTextVals.size();
}



const char* logciCompileSetting::_getNodeText(ndxml *paramNode, char *buf, size_t bufsize)
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
			if (it != m_enumText.end()) {
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


bool logciCompileSetting::onLoad(ndxml &cfgRoot)
{
	return true;
}

/////////////////////////////


void LabelMgr::clear()
{
	m_labels.clear() ;
}
void LabelMgr::pushLabel(const char *name , void *pAimAddr)
{
	if (!name || !pAimAddr) {
		return ;
	}
	labelNode_map_t::iterator it = m_labels.find(name) ;
	if (it != m_labels.end()) {
		it->second.aimAddr = pAimAddr ;
	}
	else {
		m_labels[name] = labelAddrs(pAimAddr) ;
	}
	
	//nd_logmsg("push label %s addr =%p\n",name, pAimAddr) ;
}

void LabelMgr::pushJump(const char *name, void *recordSizeAddr)
{
	if (!name || !recordSizeAddr) {
		return ;
	}
	labelNode_map_t::iterator it = m_labels.find(name) ;
	if (it != m_labels.end()) {
		it->second.fromAddr.push_back(recordSizeAddr) ;
	}
	else {
		labelAddrs tmpLabel(0) ;
		tmpLabel.fromAddr.push_back(recordSizeAddr) ;
		
		m_labels[name] = tmpLabel ;
	}
	
	//nd_logmsg("push jump addr =%p\n", recordSizeAddr) ;
}

void LabelMgr::fillJumpAddr(int byteOrder)
{
	if (0==m_labels.size())	{
		return;
	}
	
	for (labelNode_map_t::iterator it = m_labels.begin(); it != m_labels.end(); ++it){
		if (!it->second.aimAddr || it->second.fromAddr.size()==0) {
			continue;
		}
		shortJumpAddr_vct &jumpInfo = it->second.fromAddr ;
		char *toAddr = (char*)it->second.aimAddr ;
		
		for (shortJumpAddr_vct::iterator jump_it= jumpInfo.begin(); jump_it != jumpInfo.end(); ++jump_it) {
			char *fromAddr = jump_it->addr ;
			int offset = toAddr - (fromAddr + sizeof(NDUINT32) );
			lp_write_stream(fromAddr,offset, byteOrder);
			
			
			//nd_logmsg("jump from %p to %p offset =%d\n", fromAddr, toAddr, offset) ;
			
		}
	}
	
}


/////////////////////////////

LogicCompiler::LogicCompiler() :m_bDebugInfo(false), m_compileStep(0), m_curRoot(0), m_aimByteOrder(ND_L_ENDIAN)
{
}

LogicCompiler::~LogicCompiler()
{
	ndxml_destroy(&m_configRoot);
	m_enumText.clear();
}



bool LogicCompiler::_isFileInfo(ndxml * node)
{
	const char *p = ndxml_getattr_val(node, "is_file_info");
	if (p && *p){
		if (0 == ndstricmp(p, "true") || 0 == ndstricmp(p, "yes") || 0 == ndstricmp(p, "ok") ) {
			return true;
		}
	}
// 	const char *name = ndxml_getname(node) ;
// 	if (name && 0==strcmp(name, "moduleInfo") ) {
// 		return true ;
// 	}
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
			NDUINT16 outval = lp_host2stream(size, m_aimByteOrder);

			fwrite(&outval, sizeof(outval), 1, pf);
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

bool LogicCompiler::compileXml(const char *xmlFile, const char *outStreamFile, int outEncodeType, bool withDgbInfo, int byteOrder)
{
	const char *pOutType = nd_get_encode_name(outEncodeType);
	ndxml_root xmlroot;
	char init_buf[1024*64];

	m_aimByteOrder = byteOrder;

	m_initBlockSize = sizeof(init_buf);
	m_pInitBlock = init_buf;
	//ndxml_initroot(&xmlroot);

	if (ndxml_load_ex(xmlFile, &xmlroot, pOutType)) {
		nd_logerror("input file %s error\n", xmlFile);
		return false;
	}

	if (!_compilePreCmd(&xmlroot)) {
		nd_logerror("compile pre-command error %s \n", xmlFile);
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

	NDUINT8 valbit1;
	//byte order
	//valbit1 = nd_byte_order();
	valbit1 = m_aimByteOrder;
	fwrite(&valbit1, sizeof(valbit1), 1, pf);
	//encode type
	valbit1 = (NDUINT8)outEncodeType;
	fwrite(&valbit1, sizeof(valbit1), 1, pf);

	//write compile time 
	NDUINT64 compile_t64 = (NDUINT64)time(NULL);
	compile_t64 = lp_host2stream(compile_t64, m_aimByteOrder);

	fwrite(&compile_t64, sizeof(compile_t64), 1, pf);

	_writeFileInfo(&xmlroot, pf);	

	for (int x = 0; x < ndxml_num(&xmlroot); x++){
		ndxml *funcList = ndxml_getnodei(&xmlroot, x);
		if (_isFileInfo(funcList)) {
			continue;
		}
		if (!compileFuncs(funcList, pf)) {
			return false;
		}
	}

	//write init block 
	if (m_initBlockSize != sizeof(init_buf)){
		const char *initBlocName = DEFAULT_LOAD_INITILIZER_FUNC;
		int namesize = (int)strlen(initBlocName) + 1;
		NDUINT32 size = namesize + (int)(m_pInitBlock - init_buf);
		NDUINT8 isglobal = 0;
		fwrite(&isglobal, 1, 1, pf);

		NDUINT32 outval = lp_host2stream(size, m_aimByteOrder);
		fwrite(&outval, 1, sizeof(outval), pf);
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

bool LogicCompiler::compileFuncs(ndxml *funcsCollect, FILE *pf)
{
	char buf[1024 * 32];
	for (int i = 0; i < ndxml_num(funcsCollect); i++) {
		ndxml *node = ndxml_getnodei(funcsCollect, i);
		memset(buf, 0xf3, sizeof(buf));

		//////////////////////////////////////////////////////////////////////////		
		const char *stepInsName = ndxml_getname(node);
		if (!stepInsName){
			continue;
		}

		compile_setting_t::iterator it = m_settings.find(stepInsName);
		if(it == m_settings.end()) {
			continue;
		}
		if ( it->second.ins_type == E_INSTRUCT_TYPE_STEP_BLOCK){
			if (!compileFuncs(node, pf)){
				return false;
			}
		}
		else if(E_INSTRUCT_TYPE_FUNCTION == it->second.ins_type) {
			int ret = func2Stream(node, buf, sizeof(buf));
			if (-1 == ret || (size_t)ret > sizeof(buf))	{
				return false;
			}
			NDUINT8 isGlobal = _isGlobalFunc(node) ? 1 : 0;
			fwrite(&isGlobal, 1, 1, pf);

			int outval = lp_host2stream(ret, m_aimByteOrder);
			fwrite(&outval, 1, sizeof(outval), pf);
			if (fwrite(buf, 1, ret, pf) <= 0)	{
				return false;
			}
		}

	}

	return true;
}

int LogicCompiler::func2Stream(ndxml *funcNode, char *buf, size_t bufsize)
{
	m_reFillJumpStepSize.clear();
	m_labelAddr.clear();

	char *p = buf;	
	const char *pFuncName = ndxml_getattr_val(funcNode, "name");
	int len =(int) strlen(pFuncName) + 1;
	strncpy(buf, pFuncName, bufsize);
	p += len;
	bufsize -= len;
	
	m_compileStep = 0;
	
	m_cur_function = pFuncName;
	m_cur_node_index = 0;

	if (m_bDebugInfo) {
		len = writeDebugFileName(m_cur_file.c_str(), p, bufsize);
		if (len > 0){
			p += len;
			bufsize -= len;
		}
	}

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

	//add pre compile cmd to function 
	len = _trutoFillPreCmd(funcNode, p, bufsize);
	if (len > 0){
		p += len;
		bufsize -= len;
	}

	len = blockSteps2Stream(funcNode, p, bufsize);
	if (-1 == len)	{
		return -1;
	}
	p += len;
	
	m_labelAddr.fillJumpAddr(m_aimByteOrder) ;
	return  (int)(p - buf);
}

int LogicCompiler::subEntry2Stream(compile_setting *stepSetting, ndxml *subEntryNode, char *buf, size_t bufsize)
{
	char *p = buf;
	//int len = (int)bufsize;
	size_t size = bufsize;
	
	shortJumpAddr_vct jumpLeave;
	//void LogicCompiler::_pushReFillJumpAddr(char *addr, shortJumpAddr_vct *jumpAddrList);

	for (int i = 0; i < ndxml_getsub_num(subEntryNode); i++) {
		int no_comp = 0;
		int cmp_instruct = 0;
		//blockStrem node;
		NDUINT32 *pLocalJumpOffset = 0;
		ndxml *blockXml = ndxml_getnodei(subEntryNode, i);

		compile_setting_t::iterator it = m_settings.find(ndxml_getname(blockXml));
		if (it != m_settings.end() && it->second.ins_type == E_INSTRUCT_TYPE_COMMENT){
			continue;
		}

		const char *pCompCondName = ndxml_getattr_val(blockXml, "comp_cond");
		nd_assert(pCompCondName);

		ndxml *cmpNode = ndxml_getnode(blockXml, pCompCondName);
		if (cmpNode){
			const char *pVal = ndxml_getattr_val(cmpNode, "no_comp");
			if (pVal && pVal[0] == '1'){
				no_comp = 1;
			}
			cmp_instruct = ndxml_getval_int(cmpNode);
		}

		//insert jump to current lock end size cmd
		if (cmp_instruct){
			//test false and jump next block
			p = lp_write_stream(p, (NDUINT32)E_OP_TEST_FALSE_SHORT_JUMP, m_aimByteOrder);

			pLocalJumpOffset = (NDUINT32*)p;
			p = lp_write_stream(p, (NDUINT32)REVERT_INSTRUCT, m_aimByteOrder);
		}
		
		int ret = stepsCollect2Stream(blockXml, p, size);
		if (-1 == ret)	{
			return -1;
		}
		p += ret;

		//this is not last end block ,need to jump to end 
		if (cmp_instruct){
			lp_write_stream((lp_stream_t)pLocalJumpOffset, (NDUINT32)(ret + 8), m_aimByteOrder);
			
			p = lp_write_stream(p, (NDUINT32)E_OP_SHORT_JUMP, m_aimByteOrder);
			_pushReFillJumpAddr(p, &jumpLeave);

			p = lp_write_stream(p, (NDUINT32)REVERT_INSTRUCT, m_aimByteOrder);
		}

		size = bufsize - (p - buf);
	}

	_fillJumpLengthInblock(buf, (p-buf),&jumpLeave);
	return (int) (p - buf);
	
}

//compile loop
int LogicCompiler::subLoop2Stream(compile_setting *setting, ndxml *loopSteps, char *buf, size_t bufsize)
{
	int ret = 0;
	char *p = buf;
	int len = (int)bufsize;


	const char *pCompCondName = ndxml_getattr_val(loopSteps, "comp_cond");
	if (!pCompCondName)	{
		nd_logerror("loop need loop times setting\n");

		_makeErrorStack(loopSteps);
		return -1;
	}

	ndxml *cmpNode = ndxml_getnode(loopSteps, pCompCondName);
	if (!cmpNode){
		nd_logerror("loop need loop times xmlnode short\n");

		_makeErrorStack(loopSteps);
		return -1;
	}
	//write index count
	p = lp_write_stream(p, (NDUINT32)E_OP_SET_LOOP_INDEX, m_aimByteOrder);
	len -= sizeof(NDUINT32);
	p = lp_write_stream(p, (NDUINT32)0, m_aimByteOrder);
	len -= sizeof(NDUINT32);

	////

	//compile loop count
	compile_setting loop_setting;
	loop_setting.ins_id = E_OP_SET_COUNT_REG;
	loop_setting.ins_type = E_INSTRUCT_TYPE_CMD;
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
	p = lp_write_stream(p, (NDUINT32)E_OP_TEST_COUNT_JUMP, m_aimByteOrder);
	p = lp_write_stream(p, (NDUINT32)(-offset), m_aimByteOrder);

	return (int) (p - buf);
}


int LogicCompiler::trytoCompileExceptionHandler(ndxml *funcNode, char *buf, size_t bufsize)
{

	m_reFillJumpStepSize.clear();
	m_labelAddr.clear();

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
			//*((*(NDUINT32**)&p)++) = E_OP_EXCEPTION_BLOCK;
			p = lp_write_stream(p, (NDUINT32)E_OP_EXCEPTION_BLOCK, m_aimByteOrder);

			NDUINT16 *size_addr = (NDUINT16 *)p;
			//*((*(NDUINT16**)&p)++) = 0;
			p = lp_write_stream(p, (NDUINT16)0, m_aimByteOrder);

			int len = blockSteps2Stream(xmlStep, p, bufsize);
			p += len;
			bufsize -= len;
			//write exit instant
			//*((*(NDUINT32**)&p)++) = (NDUINT32)E_OP_EXIT;
			//*((*(NDUINT32**)&p)++) = (NDUINT32)0; //return false 
			p = lp_write_stream(p, (NDUINT32)E_OP_EXIT, m_aimByteOrder);
			p = lp_write_stream(p, (NDUINT32)0, m_aimByteOrder);
			
			ret = (int)( p - buf);
			//*size_addr = (NDUINT16)(p - (char*)size_addr) - sizeof(NDUINT16);
			NDUINT16 blockSize =(NDUINT16)(p - (char*)size_addr) - sizeof(NDUINT16) ;
			lp_write_stream((lp_stream_t)size_addr, blockSize,m_aimByteOrder) ;
			break ;
		}
	}
	
	m_labelAddr.fillJumpAddr(m_aimByteOrder) ;
	m_labelAddr.clear() ;
	return ret;
}


int LogicCompiler::trytoCompileInitilizerBlock(ndxml *funcNode,char *buf, size_t bufsize)
{

	m_reFillJumpStepSize.clear();
	m_labelAddr.clear();

	int ret = 0;
	char *p = buf;

	if (m_bDebugInfo) {
		int len = writeDebugFileName(m_cur_file.c_str(), p, bufsize);
		if (len > 0){
			p += len;
			bufsize -= len;
		}
	}

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
			
			ret = (int)( p - buf);
			break;
		}
	}
	
	m_labelAddr.fillJumpAddr(m_aimByteOrder) ;
	m_labelAddr.clear() ;
	return ret;
}

int LogicCompiler::blockSteps2Stream(ndxml *blockNode, char *buf, size_t bufsize)
{
	shortJumpAddr_vct  jumpAddrBack = m_reFillJumpStepSize;
	int ret = stepsCollect2Stream(blockNode, buf,bufsize);
	if (-1==ret){
		return ret;
	}

	//try to refill jump address 
	if (!_fillJumpLengthInblock(buf, ret)) {
		nd_logerror("refill jump address error\n");
		return -1;
	}

	if (jumpAddrBack.size() > 0){
		m_reFillJumpStepSize = jumpAddrBack;
	}

	return ret;
}


int LogicCompiler::stepsCollect2Stream(ndxml *stepsCollection, char *buf, size_t bufsize)
{
	int ret = 0;
	char *p = buf;
	int len = (int)bufsize;
	ndxml *defaultEntry = NULL;

	for (int i = 0; i < ndxml_getsub_num(stepsCollection); i++) {
		ndxml *xmlStep = ndxml_getnodei(stepsCollection, i);
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
		if (it->second.ins_type == E_INSTRUCT_TYPE_CMD) 	{
			ret = step2Strem(&it->second, xmlStep, p, len);
			if (ret ==-1){
				const char *pName = ndxml_getattr_val(xmlStep, "name");
				if (!pName) {
					nd_logerror("compile node %s error \n", pName);
				}
			}
		}
		else if (it->second.ins_type == E_INSTRUCT_TYPE_DELAY_COMPILE) {
			defaultEntry = xmlStep;
			continue;
		}
		else if (it->second.ins_type == E_INSTRUCT_TYPE_COLLOCTION) {
			ret = stepsCollect2Stream (xmlStep, p, len);
		}
		else if (it->second.ins_type == E_INSTRUCT_TYPE_STEP_BLOCK) {
			ret = blockSteps2Stream(xmlStep, p, len);
		}
		else if (it->second.ins_type == E_INSTRUCT_TYPE_SUB_ENTRY) 	{
			ret = subEntry2Stream(&it->second, xmlStep, p, len);
		}
		else if (it->second.ins_type == E_INSTRUCT_TYPE_LOOP) {
			ret = subLoop2Stream(&it->second, xmlStep, p, len);
		}
		else if(it->second.ins_type == E_INSTRUCT_TYPE_LABEL) {
			m_labelAddr.pushLabel(ndxml_getval(xmlStep), p);

// 			ndxml *LableXml = GetCompCond(xmlStep) ;
// 			if (LableXml) {
// 				m_labelAddr.pushLabel(ndxml_getval(LableXml), p) ;
// 			}
			ret = 0 ;
		}
		else if(it->second.ins_type ==E_INSTRUCT_TYPE_GOTO) {
			char *pStream = p;
			pStream = lp_write_stream(pStream, (NDUINT32)E_OP_SHORT_JUMP, m_aimByteOrder);
			m_labelAddr.pushJump(ndxml_getval(xmlStep), pStream);
			pStream = lp_write_stream(pStream, (NDUINT32)0, m_aimByteOrder);

// 			ndxml *LableXml = GetCompCond(xmlStep) ;
// 			char *pStream = p ;
// 			if (LableXml) {
// 				pStream = lp_write_stream(pStream, (NDUINT32)E_OP_SHORT_JUMP, m_aimByteOrder);
// 				m_labelAddr.pushJump(ndxml_getval(LableXml), pStream) ;
// 				pStream = lp_write_stream(pStream, (NDUINT32)0, m_aimByteOrder);
// 			}
			ret = pStream - p ;
		}
		
		else {
			continue;
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

	if (defaultEntry){
		ret = stepsCollect2Stream(defaultEntry, p, len);
		if (-1 == ret){
			return -1;
		}
		p += ret;
		len -= ret;
	}
	return (int)(bufsize - len);
}

int LogicCompiler::step2Strem(compile_setting *stepSetting, ndxml *stepNode, char *buf, size_t bufsize)
{
	char *p = buf;
	int len = (int)bufsize;
	lp_stream_t cur_step_size = 0; //record this step size in debug block 


	//get current step name 
	m_cur_node_index++;
	const char *pStepName = ndxml_getattr_val(stepNode, "name");
	if (!pStepName) {
		pStepName = ndxml_getname(stepNode);
		if (!pStepName)	{

			_makeErrorStack(stepNode);
			return	-1;
		}
	}
	m_cur_step = pStepName;

	if (m_bDebugInfo) {
		int ret = writeDebugInfo(stepNode, pStepName, p, len);
		if (ret > 0){
			len -= ret;
			p += ret;
			cur_step_size = p;
			//*((*(NDUINT32**)&p)++) = 0;
			p = lp_write_stream(p, (NDUINT32)0, m_aimByteOrder);

			len -= sizeof(NDUINT32);

		}
		else {
			nd_logerror("wirte debug info error :%s\n", pStepName);
			_makeErrorStack(stepNode);
			return -1;
		}
	}

	//*((*(NDUINT32**)&p)++) = (NDUINT32)stepSetting->ins_id;
	p = lp_write_stream(p, (NDUINT32)stepSetting->ins_id, m_aimByteOrder);

	lp_stream_t pArgNum = NULL;
	NDUINT32 args_num = 0;
	if (stepSetting->record_param_num){
		pArgNum = p;
		p += sizeof(NDUINT32);
		len -= sizeof(NDUINT32);
	}

	ndxml *xlmLabel = NULL, *xmlGoto = NULL; 
	for (int i = 0; i < ndxml_getsub_num(stepNode); i++) {
		ndxml *xmlParam = ndxml_getnodei(stepNode, i);
		if (!xmlParam) {
			continue;
		}
		compile_setting_t::iterator itparam = m_settings.find(ndxml_getname(xmlParam));
		if (itparam == m_settings.end()){
			continue;
		}
		compile_setting *paramSetting = &itparam->second;

		const char *pRefOnly = ndxml_getattr_val(xmlParam, "referenced_only");
		if (pRefOnly && *pRefOnly){
			if (0 == ndstricmp(pRefOnly, "yes") || 0 == ndstricmp(pRefOnly, "true")){
				continue;
			}
		}

		//add jump ---
		else if (paramSetting->ins_type == E_INSTRUCT_TYPE_LABEL) {
			xlmLabel = xmlParam;
			continue;
		}
		else if (paramSetting->ins_type == E_INSTRUCT_TYPE_GOTO) {
			xmlGoto = xmlParam;
			continue;
		}
		//end jump --

		int ret = param2Stream(xmlParam, stepNode, p, len, &args_num);
		if (-1==ret)	{
			//_makeErrorStack(xmlParam);
			return -1;
		}
		p += ret;
		len -= ret;
	}
	if (pArgNum) {
		lp_write_stream(pArgNum, args_num, m_aimByteOrder);
	}


	if (cur_step_size) {
		//*cur_step_size = (NDUINT32)(p - (char*)cur_step_size) - 4;
		lp_write_stream((lp_stream_t)cur_step_size , (NDUINT32)((p - (char*)cur_step_size) - 4), m_aimByteOrder);
	}

	//add jump 
	if (xlmLabel) {
		m_labelAddr.pushLabel(ndxml_getval(xlmLabel), buf);
	}
	if (xmlGoto) {
		p = lp_write_stream(p, (NDUINT32)E_OP_SHORT_JUMP, m_aimByteOrder);
		m_labelAddr.pushJump(ndxml_getval(xmlGoto), p);
		p = lp_write_stream(p, (NDUINT32)0, m_aimByteOrder);
	}
	//end jump

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
	NDUINT16 _size =(NDUINT16) strlen(_text);			\
	_p = lp_write_stream(_p, _size, m_aimByteOrder);		\
	if (_size > 0)							\
		memcpy(_p, _text, _size );			\
	_p += _size ;							\
}while (0)

int LogicCompiler::step_function_info(compile_setting *setting, ndxml *stepNode, char *buf, size_t bufsize)
{
	char *p = buf;
	//int len = (int)bufsize;
	const char *pStepName = ndxml_getname(stepNode);
	const char *pValOrg = ndxml_getval(stepNode);
	if (!pStepName || !pValOrg || *pValOrg)	{
		return	0;
	}
	char infotext[1024];

	char tmpbuf[4096];
	const char *pVal = getRealValFromStr(pValOrg, infotext, sizeof(infotext));

	snprintf(tmpbuf, sizeof(tmpbuf), "%s=%s;", pStepName, pVal);

	//*((*(NDUINT32**)&p)++) = E_OP_HEADER_INFO;
	p = lp_write_stream(p, (NDUINT32)E_OP_HEADER_INFO, m_aimByteOrder);

	//*((*(NDUINT16**)&p)++) = OT_STRING;
	p = lp_write_stream(p, (NDUINT16)OT_STRING, m_aimByteOrder);

	WRITE_STRING_TOSTREAM(p, tmpbuf); //function name 

	return (int)(p - buf);
}

int LogicCompiler::writeDebugInfo(ndxml *stepNode, const char*stepName, char *buf, size_t bufsize)
{
	char *p = buf;
	
	char debugInfo[1024] ;
	char *start = debugInfo ;
	int len = snprintf(debugInfo, sizeof(debugInfo), "%s", stepName) ;
	start += len;
	
	_getFuncStackInfo(stepNode,start, sizeof(debugInfo) - len) ;
	stepName = debugInfo ;

	p = lp_write_stream(p, (NDUINT32)E_OP_DEBUG_INFO, m_aimByteOrder);
	p = lp_write_stream(p, (NDUINT16)m_compileStep, m_aimByteOrder);
	++m_compileStep;
	WRITE_STRING_TOSTREAM(p, stepName);

	return (int)(p - buf);
}


int LogicCompiler::writeDebugFileName(const char *fileInfo, char *buf, size_t bufsize)
{
	char *p = buf;
	p = lp_write_stream(p, (NDUINT32)E_OP_FILE_INFO, m_aimByteOrder);
	WRITE_STRING_TOSTREAM(p, fileInfo);
	return (int)(p - buf);
}

int LogicCompiler::param2Stream(ndxml *xmlParam, ndxml *parent, char *buf, size_t bufsize, NDUINT32 *param_num)
{
	char *p = buf;
	lp_stream_t paramType = NULL;
	int len = (int)bufsize;

	const char *paramName = ndxml_getname(xmlParam);
	if (!paramName){
		return 0;
	}
	compile_setting_t::iterator it = m_settings.find(paramName);
	if (it == m_settings.end()){
		return 0;
	}
	compile_setting *paramSetting = &it->second;

	if (paramSetting->ins_type == E_INSTRUCT_TYPE_COMMENT) {
		return 0;
	}
	else if (paramSetting->ins_type == E_INSTRUCT_TYPE_REF_FROM_PARENT) {
		ndxml *refedXml = _getRefNode(xmlParam);
		if (!refedXml)	{
			return -1;
		}
		parent = ndxml_get_parent(refedXml);
		return param2Stream(refedXml, parent, p, len, param_num);
	}

	else if (paramSetting->ins_type == E_INSTRUCT_TYPE_PARAM_COLLECT){
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

		_makeErrorStack(xmlParam);
		return -1;
	}

	//const char *pValOrg = ndxml_getval(xmlParam);	
	//const char *pVal = getRealValFromStr(pValOrg, tmpbuf, sizeof(tmpbuf));
	
	if (type < OT_LAST_RET || type == OT_TIME)	{
		if (0 == ndstricmp(pVal, "none") || 0 == ndstricmp(pVal, "null")) {
			nd_logerror("%s string value not none or null\n", paramName);

			_makeErrorStack(xmlParam);
			return -1;
		}
	}

	if (paramSetting->need_type_stream){
		paramType = p;
		//*((*(NDUINT16**)&p)++) = type;
		p = lp_write_stream(p, (NDUINT16) type, m_aimByteOrder);
		len -= 2;
	}
	if (paramSetting->need_refill_jump_len)	{
		_pushReFillJumpAddr(p);
	}

#define CHECK_IS_NUMERAL(_text) do {	\
		if (_text) {		\
			if (!ndstr_is_numerals(_text)) {		\
				nd_logerror("value is number: %s\n", _text);	\
				return -1;						\
			}									\
		}										\
	}while (0)

	switch (type)
	{
	case  OT_INT:
	case  OT_PARAM:
		CHECK_IS_NUMERAL(pVal);
	case OT_BOOL :
		//*((*(NDUINT32**)&p)++) = pVal ? atoi(pVal) : 0;
		p = lp_write_stream(p, (int)(pVal ? ndstr_atoi_hex(pVal) : 0), m_aimByteOrder);
		break;
	case OT_FLOAT: 
	{
		CHECK_IS_NUMERAL(pVal);
		float fv = pVal ? (float)atof(pVal) : 0;
		_write_float_to_buf(p, fv);
	}
		break;
	case OT_INT8:
		CHECK_IS_NUMERAL(pVal);
		*((*(NDUINT8**)&p)++) = (NDUINT8)(pVal ? ndstr_atoi_hex(pVal) : 0);
		break;
	case OT_INT16:
		CHECK_IS_NUMERAL(pVal);
		//*((*(NDUINT16**)&p)++) = (NDUINT16)(pVal ? atoi(pVal) : 0);
		p = lp_write_stream(p, (NDUINT16)(pVal ? ndstr_atoi_hex(pVal) : 0), m_aimByteOrder);
		break;
	case OT_INT64:
		CHECK_IS_NUMERAL(pVal);
		//*((*(NDUINT64**)&p)++) = (NDUINT64)(pVal ? nd_atoi64(pVal) : 0);
		p = lp_write_stream(p, (NDUINT64)(pVal ? ndstr_atoll_hex(pVal) : 0), m_aimByteOrder);

		break; 

	case OT_TIME:
	{
		time_t timval=0;
		if ((time_t)-1==nd_time_from_str(pVal, &timval))	{
			nd_logerror("time %s format error in %s\n", pVal, paramName);
			return -1;
		}
		//*((*(NDUINT64**)&p)++) = timval;
		p = lp_write_stream(p, (NDUINT64)timval, m_aimByteOrder);
	}	
		break;
	case OT_USER_DEFINED:
	case OT_STRING:
	case OT_VARIABLE:
		if (!pVal || !pVal[0]){
			//*((*(NDUINT16**)&p)++) = (NDUINT16)0;
			p = lp_write_stream(p, (NDUINT16)0, m_aimByteOrder);
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
			//*paramType = OT_TIME;
			lp_write_stream(paramType, (NDUINT16)OT_TIME, m_aimByteOrder);
		}
		//*((*(NDUINT64**)&p)++) = (NDUINT64) time(NULL);
		p = lp_write_stream(p, (NDUINT64)time(NULL), m_aimByteOrder);
		break;
	case OT_FUNCTION_NAME:
	{
		if (paramType)	{
			//*paramType = OT_STRING;
			lp_write_stream((lp_stream_t)paramType, (NDUINT16)OT_STRING, m_aimByteOrder);
		}
		const char *Name = m_cur_function.c_str();
		WRITE_STRING_TOSTREAM(p, Name);
	}
		break;
	case OT_SCRIPT_MODULE_NAME:
	{
		if (paramType)	{
			//*paramType = OT_STRING;
			lp_write_stream((lp_stream_t)paramType, (NDUINT16)OT_STRING, m_aimByteOrder);
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
				int size = convertData.WriteStream(p,len, m_aimByteOrder);
				//////////////////////////////////////////////////////////////////////////
				//DBLDataNode test1;
				//int size1 = test1.ReadStream(p);
				//nd_assert(test1 == convertData);
				//////////////////////////////////////////////////////////////////////////
				p += size ;
			}
			else {
				//*((*(NDUINT16**)&p)++) = (NDUINT16)0;
				p = lp_write_stream(p, (NDUINT16)0, m_aimByteOrder);
			}
			
		}
		else {
			//*((*(NDUINT16**)&p)++) = (NDUINT16)0;
			p = lp_write_stream(p, (NDUINT16)0, m_aimByteOrder);
		}
		break;
	default:
		nd_logerror("param %s type %d not suport \n", paramName, type);
		_makeErrorStack(xmlParam);
		return -1;
		break;
	}

	if (param_num){
		++(*param_num);
	}
	return (int) (p - buf);
}


void LogicCompiler::_pushReFillJumpAddr(char *addr, shortJumpAddr_vct *jumpAddrList)
{
	shortJumpInfo jinfo;
	jinfo.addr = addr;
	if (!jumpAddrList)	{
		jumpAddrList = &m_reFillJumpStepSize;
	}
	jumpAddrList->push_back(jinfo);
}

bool LogicCompiler::_fillJumpLengthInblock(const char *blockStart, size_t blockSize, shortJumpAddr_vct *jumpAddrList)
{
	if (!jumpAddrList)	{
		jumpAddrList = &m_reFillJumpStepSize;
	}

	if (jumpAddrList->size() == 0){
		return true;			 
	}

	for (shortJumpAddr_vct::iterator it = jumpAddrList->begin(); it != jumpAddrList->end(); it++){
		char *paddr = it->addr;
		if (paddr < blockStart || paddr >= (blockStart + blockSize)) {
			nd_logerror("bin code is to big\n");
			return false;
		}
		NDUINT32 offset =(NDUINT32)( blockSize - (paddr + sizeof(NDUINT32) - blockStart) );
		lp_write_stream(paddr,offset, m_aimByteOrder);
	}

	jumpAddrList->clear();
	return true;
}

int LogicCompiler::_trutoFillPreCmd(ndxml *funcNode, char *buf, size_t bufsize)
{
	const char *pFuncName = ndxml_getattr_val(funcNode, "preFillCmd");
	if (pFuncName && *pFuncName){
		streamCMD_map_t::iterator it = m_preCMDs.find(pFuncName);
		if (it!=m_preCMDs.end() && it->second.size > 0 && bufsize >= it->second.size) {
			memcpy(buf, it->second.buf, it->second.size);
			return it->second.size;
		}
		return -1;
	}
	return 0;
}

bool LogicCompiler::_compilePreCmd(ndxml *xmlroot)
{
	m_preCMDs.clear();
	ndxml *xmlModule = ndxml_getnode(xmlroot, "moduleInfo");
	if (!xmlModule) {
		return true;
	}
	ndxml *preCMDxml = ndxml_getnode(xmlModule, "PreCmd");
	if (!preCMDxml)	{
		return true;
	}

	for (int i = 0; i < ndxml_num(preCMDxml); i++) {
		ndxml *cmdNode = ndxml_getnodei(preCMDxml, i);

		const char *pFuncName = ndxml_getattr_val(cmdNode, "name");
		if (!pFuncName || !*pFuncName) {
			continue;
		}
		//std::string name = pFuncName;
		streamNode nodeBuf;

		int ret = blockSteps2Stream(cmdNode, nodeBuf.buf, sizeof(nodeBuf.buf));
		if (-1 == ret )	{
			nd_logerror("compile pre-cmd instruct  %s error \n", pFuncName);
			return false;
		}
		nodeBuf.size = ret;
		m_preCMDs[pFuncName] = nodeBuf;
	}

	return true;
}
void LogicCompiler::_pushStack(int stackIndex)
{
	m_curCompileStack.push_back(stackIndex);
}
void LogicCompiler::_popStac()
{
	if(m_curCompileStack.size() >0) {
		m_curCompileStack.pop_back();
	}
}

void LogicCompiler::_makeErrorStack(ndxml *xmlError)
{
	stackIndex_vct stackIndex ;
	ndxml *parent = ndxml_get_parent(xmlError) ;
	ndxml *node = xmlError ;
	
	while (parent ) {
		int index = ndxml_get_index(parent, node) ;
		stackIndex.push_back(index) ;
		
		node = parent ;
		parent = ndxml_get_parent(node) ;
	}
	m_curCompileStack.clear() ;
	
	m_curCompileStack.insert(m_curCompileStack.begin(), stackIndex.rbegin(), stackIndex.rend()) ;
}

bool LogicCompiler::_getFuncStackInfo(ndxml *curNode,char *buf, size_t size)
{
	stackIndex_vct stackIndex ;
	ndxml *parent = ndxml_get_parent(curNode) ;
	ndxml *node = curNode ;
	
	while (parent ) {
		
		int index = ndxml_get_index(parent, node) ;
		stackIndex.push_back(index) ;
		
		node = parent ;
		parent = ndxml_get_parent(node) ;
	}
	
	if (stackIndex.size()==0) {
		return false ;
	}
	char *p = buf ;
	*p = 0 ;
	for (stackIndex_vct::reverse_iterator it =stackIndex.rbegin() ; it!=stackIndex.rend(); ++it) {
		int len = snprintf(p, size, ".%d", (*it)) ;
		p+= len ;
		size -= len ;
	}
	
	return true;
}


ndxml *LogicCompiler::_getRefNode(ndxml*node)
{ 
	return LogicEditorHelper::_getRefNode(node,ndxml_getval(node));
}
