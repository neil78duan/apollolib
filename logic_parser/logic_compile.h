/* file logic_compile.h
 *
 * comile xml game logic to binary stream
 *
 * create by duan
 *
 * 2015-4-30
 */
 
#ifndef _LOGIC_COMPILE_H_
#define _LOGIC_COMPILE_H_

#include "nd_common/nd_common.h"
#include <string>
#include <map>
#include <vector>

enum instructType {
	E_INSTRUCT_TYPE_PARAM = 0,
	E_INSTRUCT_TYPE_CMD,
	E_INSTRUCT_TYPE_SUB_ENTRY,
	E_INSTRUCT_TYPE_LOOP,
	E_INSTRUCT_TYPE_PARAM_COLLECT,
	E_INSTRUCT_TYPE_USER_DEFINE,
	E_INSTRUCT_TYPE_FUNCTION_INFO,
	E_INSTRUCT_TYPE_EXCEPTION_CATCH, //handle error
	E_INSTRUCT_TYPE_INIT_BLOCK, //the block of function init entry
	E_INSTRUCT_TYPE_STEP_BLOCK, // steps block , can use break in this block
	E_INSTRUCT_TYPE_CASE_ENTRY, // SWITCH-CASE sub entry
	E_INSTRUCT_TYPE_FUNCTION ,	//function node
	E_INSTRUCT_TYPE_REF_FROM_PARENT,	//reference from parent node
	E_INSTRUCT_TYPE_DELAY_COMPILE,		//switch of if (default, else entry) 

	E_INSTRUCT_TYPE_COLLOCTION,			//multi-step clooection ,can not use break ;

	E_INSTRUCT_TYPE_COMMENT = 100, // MARRK

};

struct compile_setting
{
	std::string name;
	NDUINT8 ins_type; 
	NDUINT8 ins_id;
	NDUINT8 data_type;
	NDUINT8 record_param_num;
	NDUINT8 need_type_stream;
	NDUINT8 need_refill_jump_len;
	NDUINT32 size;

	compile_setting()
	{
		ins_type =0xff;
		ins_id =0xff;
		data_type = 0;
		record_param_num = 0;
		size = -1;
		need_type_stream = 0;
		need_refill_jump_len = 0;
	}
};

struct blockStrem
{
	int size;
	int node_index;
	int no_comp;
	int cmp_instruct;
	const char* cmp_val;
	NDUINT32 jump_end_offset;
	//char block_buf[1000];
	blockStrem() : size(0), node_index(-1), no_comp(0), cmp_val(0), jump_end_offset(0)
	{}
};

struct enumTextVct
{
	std::vector<std::string>  enumTextVals;
};

//record short jump addr ,after compile ,so it will get the current block size , the short jump need to goto the block-end
struct shortJumpInfo
{
	char *addr;
};

struct streamNode
{
	int size;
	char buf[4089];
};


class logciCompileSetting
{
public:
	logciCompileSetting();
	virtual ~logciCompileSetting();


	bool setConfigFile(const char *config, int encodeType = ND_ENCODE_TYPE);
	ndxml_root *getConfig() { return &m_configRoot; }
	const compile_setting* getStepConfig(const char *stepName)const;
protected:

	const char* _getNodeText(ndxml *paramNode, char *buf, size_t bufsize);
	int _loadEnumText(ndxml *dataTypeDef);
	int _getEnumText(ndxml * node, enumTextVct &enumText);

	virtual bool onLoad(ndxml &cfgRoot);
	//compile init block
	ndxml m_configRoot;

	typedef std::map<std::string, compile_setting> compile_setting_t;
	compile_setting_t m_settings;

	//read enum-text, not enum-int
	typedef std::map<std::string, enumTextVct> enum_textValue_t;
	enum_textValue_t m_enumText;

};


typedef std::vector<shortJumpInfo> shortJumpAddr_vct;

typedef std::map<std::string, streamNode> streamCMD_map_t;

typedef std::vector<int> stackIndex_vct;

class LogicCompiler : public logciCompileSetting
{
public:
	LogicCompiler();
	~LogicCompiler();
	bool compileXml(const char *xmlFile, const char *outStreamFile, int outEncodeType = 0, bool withDgbInfo = false, int byteOrder=1);
	stackIndex_vct &getErrorStack() { return m_curCompileStack; }
	//bool setConfigFile(const char *config);
private:
	
	bool compileFuncs(ndxml *funcsCollect, FILE *pf);
	//compile function
	int func2Stream(ndxml *funcNode, char *buf, size_t bufsize);
	//compile if-else
	int subEntry2Stream(compile_setting *setting,ndxml *subEntryNode, char *buf, size_t bufsize);
	//compile step in if{...}
	//int subEntryBlock2Stream(compile_setting *setting, ndxml *stepNode, char *buf, size_t bufsize);
	//compile loop
	int subLoop2Stream(compile_setting *setting, ndxml *stepNode, char *buf, size_t bufsize);


	int blockSteps2Stream(ndxml *blockNode, char *buf, size_t bufsize); //compile block , can use break 
	int stepsCollect2Stream(ndxml *steps, char *buf, size_t bufsize);	//compile steps collections , not use break ;

	//compile-step
	int step2Strem(compile_setting *setting, ndxml *stepNode, char *buf, size_t bufsize);//return stream size ;

	int step_function_info(compile_setting *setting, ndxml *stepNode, char *buf, size_t bufsize);
	
	int param2Stream(ndxml *paramNode, ndxml *parent, char *buf, size_t bufsize, NDUINT32 *param_num = NULL);//return stream size ;

	int writeDebugInfo(ndxml *stepNode, const char*stepName, char *buf, size_t bufsize);
	int writeDebugFileName(const char *fileInfo, char *buf, size_t bufsize);

	int trytoCompileExceptionHandler(ndxml *funcNode, char *buf, size_t bufsize);
	int trytoCompileInitilizerBlock(ndxml *funcNode, char *buf, size_t bufsize);

	bool _isFileInfo(ndxml * node);
	int _writeFileInfo(ndxml *module, FILE *pf) ;
	bool _isGlobalFunc(ndxml *funcNode);

	void _pushReFillJumpAddr(char *addr, shortJumpAddr_vct *jumpAddrList=NULL);
	bool _fillJumpLengthInblock(const char *blockStart, size_t blockSize, shortJumpAddr_vct *jumpAddrList = NULL);


	int _trutoFillPreCmd(ndxml *funcNode, char *buf, size_t bufsize);

	bool _compilePreCmd(ndxml *xmlroot);

	void _pushStack(int stackIndex) ;
	void _popStac();
	void _makeErrorStack(ndxml *xmlError) ;
	bool _getFuncStackInfo(ndxml *curNode,char *buf, size_t size) ;

	ndxml *_getRefNode(ndxml*node);

	bool m_bDebugInfo;
	int m_compileStep;
	int m_aimByteOrder;

	ndxml_root *m_curRoot;

	size_t m_initBlockSize;
	char *m_pInitBlock;

	shortJumpAddr_vct m_reFillJumpStepSize;

	streamCMD_map_t m_preCMDs;
	
	friend class CStackIndexHelper ;
public:
	//current compile info 
	std::string m_cur_file;
	std::string m_cur_function;
	std::string m_moduleName;
	std::string m_cur_step;
	int m_cur_node_index;

	stackIndex_vct m_curCompileStack ;
};


class CStackIndexHelper
{
public:
	CStackIndexHelper(LogicCompiler *host,int index) {
		m_host = host ;
		m_host->_pushStack(index);
	}
	~CStackIndexHelper() {
		if (m_host) {
			m_host->_popStac() ;
		}
	}
private:
	LogicCompiler *m_host ;
};

//bool getTimeFromStr(const char *pInput, size_t *size, time_t *tim);

#endif
