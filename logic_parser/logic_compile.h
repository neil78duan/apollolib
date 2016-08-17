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
	NDUINT32 size;

	compile_setting()
	{
		ins_type =0xff;
		ins_id =0xff;
		data_type = 0;
		record_param_num = 0;
		size = -1;
		need_type_stream = 0;
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
	char block_buf[1000];
	blockStrem() : size(0), node_index(-1), no_comp(0), cmp_val(0), jump_end_offset(0)
	{}
};

struct enumTextVct
{
	std::vector<std::string>  enumTextVals;
};

class LogicCompiler
{
public:
	LogicCompiler();
	~LogicCompiler();
	bool compileXml(const char *xmlFile, const char *outStreamFile, int outEncodeType = 0, bool withDgbInfo = false, int byteOrder=1);
	bool setConfigFile(const char *config);
private:
	
	//compile function
	int func2Stream(ndxml *funcNode, char *buf, size_t bufsize);
	//compile if-else
	int subEntry2Stream(compile_setting *setting,ndxml *subEntryNode, char *buf, size_t bufsize);
	//compile step in if{...}
	int subEntryBlock2Stream(compile_setting *setting, ndxml *stepNode, char *buf, size_t bufsize);
	//compile loop
	int subLoop2Stream(compile_setting *setting, ndxml *stepNode, char *buf, size_t bufsize);


	int blockSteps2Stream(ndxml *blockNode, char *buf, size_t bufsize);
	//compile-step
	int step2Strem(compile_setting *setting, ndxml *stepNode, char *buf, size_t bufsize);//return stream size ;

	int step_function_info(compile_setting *setting, ndxml *stepNode, char *buf, size_t bufsize);
	//user define step
	int user_define_step(compile_setting *setting, ndxml *stepNode, char *buf, size_t bufsize);
	//int user_define_msg_init( ndxml *stepNode, char *buf, size_t bufsize);
	//int user_define_event_init(ndxml *stepNode, char *buf, size_t bufsize);

	int param2Stream(ndxml *paramNode, ndxml *parent, char *buf, size_t bufsize, NDUINT32 *param_num = NULL);//return stream size ;

	int writeDebugInfo(ndxml *stepNode,const char*stepName, char *buf, size_t bufsize);

	int trytoCompileExceptionHandler(ndxml *funcNode, char *buf, size_t bufsize);
	int trytoCompileInitilizerBlock(ndxml *funcNode, char *buf, size_t bufsize);

	const char* _getNodeText(ndxml *paramNode, char *buf, size_t bufsize);
	int _loadEnumText(ndxml *dataTypeDef);
	int _getEnumText(ndxml * node, enumTextVct &enumText);
	
	bool _isFileInfo(ndxml * node);
	int _writeFileInfo(ndxml *module, FILE *pf) ;
	bool _isGlobalFunc(ndxml *funcNode);

	//compile init block
	ndxml m_configRoot;

	typedef std::map<std::string, compile_setting> compile_setting_t;
	compile_setting_t m_settings;

	//read enum-text, not enum-int
	typedef std::map<std::string, enumTextVct> enum_textValue_t;
	enum_textValue_t m_enumText;

	bool m_bDebugInfo;
	int m_compileStep;
	int m_aimByteOrder;

	ndxml_root *m_curRoot;

	size_t m_initBlockSize;
	char *m_pInitBlock;
public:
	//current compile info 
	std::string m_cur_file;
	std::string m_cur_function;
	std::string m_moduleName;
	std::string m_cur_step;
	int m_cur_node_index;

	
};

//bool getTimeFromStr(const char *pInput, size_t *size, time_t *tim);

#endif
