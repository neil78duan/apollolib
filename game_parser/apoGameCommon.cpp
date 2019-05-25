/* file apoGameCommon.cpp
 * 
 * define apollo plugin for game server
 *
 * create by duan
 *
 * 2018.11.6
 */

#include "game_parser/apoGameCommon.h"
#include "game_parser/dbldata2netstream.h"
#include "game_parser/dbl_mgr.h"

#include "ndapplib/applib.h"
#include "logic_parser/logicEngineRoot.h"
#include "logic_parser/logicParser.h"
#include "logic_parser/logic_compile.h"
#include "logic_parser/logic_function.h"
#include "logic_parser/logic_editor_helper.h"
#include "nd_common/cJSON.h"

int apollo_message_script_entry(void *engine, NDBaseConnector *netconn, nd_usermsgbuf_t *msg, const char *script)
{
	LogicParserEngine *scriptEngine = (LogicParserEngine *)engine;
	if (!scriptEngine) {
//		NDObject *pObj = NDObject::FromHandle(handle);
//		if (!pObj) {
//			return -1;
//		}
		scriptEngine = (LogicParserEngine *)netconn->getScriptEngine();
		if (!scriptEngine) {
			nd_logerror("can not get role script parser\n");
			return -1;
		}
	}

	NDIStreamMsg inmsg(msg);
	//call function 
	parse_arg_list_t params;

	//function name 
	params.push_back(LogicDataObj(script));

	//receive message user
	params.push_back(LogicDataObj(netconn));

	//message object
	params.push_back(LogicDataObj((void*)&inmsg, OT_OBJ_MSGSTREAM));

	LogicDataObj result;
	scriptEngine->runScript(script, params, result);

	return 0;

}


////////////////////////////////////////////////////
///test object manager
apoLogicObject4Game::apoLogicObject4Game()
{

}
apoLogicObject4Game::~apoLogicObject4Game()
{

}

bool apoLogicObject4Game::opCheck(const LogicDataObj& id, const  LogicDataObj &val)
{
	if (val.GetDataType() == OT_STRING) {
		const char *pText = val.GetText();
		if (pText && *pText) {
			if (0 == ndstricmp(pText, "test-error")) {
				m_error = NDERR_SUCCESS;
				return false;
			}
		}
	}
	return true;
}

bool apoLogicObject4Game::getOtherObject(const char*objName, LogicDataObj &val)
{
	if (0 == ndstricmp(objName, "machineInfo")) {
		char buf[256];
		val.InitSet(nd_common_machine_info(buf, sizeof(buf)));
	}
	else if (0 == ndstricmp(objName, "FormatMsgData")) {
		val.InitSet((void*)&getGlobalNetMsgFormat());
	}
	else if (0 == ndstricmp(objName, "self")) {
		val.InitSet((void*)this, OT_OBJ_BASE_OBJ);
	}

	else if (0 == ndstricmp(objName, "settingFile")) {
		const char *pSettingFile = LogicCompiler::get_Instant()->getConfigFileName();
		val.InitSet(pSettingFile);
	}
	else if (0 == ndstricmp(objName, "globalParser")) {
		LogicParserEngine &parser = LogicEngineRoot::get_Instant()->getGlobalParser();
		val.InitSet(&parser);
	}
	else if (0 == ndstricmp(objName, "WritablePath")) {
		val.InitSet(nd_getcwd());
	}
	else if (0 == ndstricmp(objName, "myParser")) {
		LogicParserEngine *parser = getScriptHandler();
		if (parser) {
			val.InitSet(&parser);
		}
		else {
			m_error = LOGIC_ERR_AIM_OBJECT_NOT_FOUND;
			return false;
		}
	}
	else {
		return _myBase::getOtherObject(objName, val);
	}
	return true;
}

LogicObjectBase *apoLogicObject4Game::getObjectMgr(const char* destName)
{
	if (0 == ndstricmp(destName, "owner")) {
		return  this;
	}
	m_error = LOGIC_ERR_AIM_OBJECT_NOT_FOUND;
	PARSE_TRACE("logic_engine_test: getObjectMgr(%s) \n", destName);
	return NULL;
}


LogicParserEngine *apoLogicObject4Game::getScriptHandler()
{
	PARSE_TRACE("LogicParserEngine: get my parser not implemention \n");
	return NULL;
}

//////////////////////////////////////////////////////////
APOLLO_SCRIPT_API_DEF(apollo_set_connector_script_parser, "绑定消息处理引擎(netObject , parser)")
{
	CHECK_ARGS_NUM(args, 3, parser);

	nd_handle h = apollo_getNetHanlder(parser->getOwner(), args[1]);

	LogicParserEngine* paimParser = dynamic_cast<LogicParserEngine*>(args[2].GetNDObj());
	if (!paimParser || !h) {
		nd_logerror("input parser or net object is invalid\n");
		parser->setErrno(NDERR_PARAM_INVALID);
		return false;
	}

	if (-1 == nd_message_set_script_engine(h, paimParser,(nd_msg_script_entry) apollo_message_script_entry)) {
		nd_logerror("set script engine for GMConnector error\n");

		parser->setErrno(NDERR_SYSTEM);
		return false;
	}

	return true;
}


APOLLO_SCRIPT_API_DEF(_sys_write_binfile, "sys_BinWriteStream(fileName, var1,var2....)")
{
	CHECK_ARGS_NUM(args, 3, parser);

	std::string filename = args[1].GetString();
	if (filename.size() == 0) {
		parser->setErrno(NDERR_INVALID_INPUT);
		return false;
	}
	FILE *pf = fopen(filename.c_str(), "ab");
	if (!pf) {
		parser->setErrno(NDERR_FILE_NOT_EXIST);
		return false;
	}
	NDOStreamMsg omsg(0, 0);

	for (size_t i = 2; i < args.size(); i++) {
		LogicDataObj &data = args[i];
		if (data.GetDataType() == OT_BINARY_DATA) {
			fwrite(data.GetBinary(), data.GetBinarySize(), 1, pf);
		}
		else {
			if (-1 == logicDataWrite(data, omsg)) {
				parser->setErrno(NDERR_BAD_GAME_OBJECT);
				return false;
			}
			fwrite(omsg.MsgData(), omsg.GetDataLen(), 1, pf);
		}

	}

	fclose(pf);
	return true;
}

APOLLO_SCRIPT_API_DEF(_sys_binread_file_stream, "sys_BinReadStream(fileStream, readDataType)")
{
	CHECK_ARGS_NUM(args, 3, parser);
	if (args[1].GetDataType() != OT_FILE_STREAM) {
		parser->setErrno(NDERR_BAD_GAME_OBJECT);
		return false;
	}
	DBL_ELEMENT_TYPE type = args[2].GetDataType();
	FILE *pf = (FILE*)args[1].GetObjectAddr();
	if (!pf) {
		parser->setErrno(NDERR_PARAM_INVALID);
		return false;
	}
	NDUINT8 byteVal = 0;
	char buf[8];

	int res = NDERR_PARAM_INVALID;
	switch (type)
	{
	case OT_INT8:
		if (fread(&byteVal, sizeof(NDUINT8), 1, pf) > 0) {
			result.InitSet(byteVal);
			res = (int)LOGIC_ERR_SUCCESS;
		}
		break;
	case OT_INT16:
		if (fread(buf, sizeof(NDUINT16), 1, pf) > 0) {
			NDUINT16 val = nd_netstream_to_short(buf);
			result.InitSet(val);
			res = LOGIC_ERR_SUCCESS;
		}
		break;
	case OT_INT:
		if (fread(buf, sizeof(NDUINT32), 1, pf) > 0) {
			NDUINT32 val = nd_netstream_to_long(buf);
			result.InitSet((int)val);
			res = (int)LOGIC_ERR_SUCCESS;
		}
		break;
	case  OT_BOOL:
		if (fread(&byteVal, sizeof(NDUINT8), 1, pf) > 0) {
			result.InitSet((int)byteVal);
			res = (int)LOGIC_ERR_SUCCESS;
		}
		break;

	case OT_FLOAT:
	{
		float val;
		if (fread(&val, sizeof(val), 1, pf) > 0) {
			result.InitSet(val);
			res = (int)LOGIC_ERR_SUCCESS;
		}
		break;
	}

	case OT_STRING:
	{
		NDUINT16 size;
		if (fread(buf, sizeof(NDUINT16), 1, pf) > 0) {
			size = nd_netstream_to_short(buf);
			if (size == 0) {
				result.InitSet((int)0);
				res = (int)LOGIC_ERR_SUCCESS;
				break;
			}

			char *p = (char *)malloc(size + 1);
			if (p) {
				fread(p, size, 1, pf);
				p[size] = 0;
				result.InitSet(p);
				free(p);
				res = (int)LOGIC_ERR_SUCCESS;
			}
		}
		break;
	}
	case OT_INT64:
	case OT_TIME:
		if (fwrite(buf, sizeof(NDUINT64), 1, pf) > 0) {
			NDUINT64 val = nd_netstream_to_longlong(buf);
			result.InitSet(val);
			res = (int)LOGIC_ERR_SUCCESS;
		}
		break;

	case OT_BINARY_DATA:
	{
		NDUINT16 size;
		if (fread(buf, sizeof(NDUINT16), 1, pf) > 0) {
			size = nd_netstream_to_short(buf);
			if (size == 0) {
				result.InitSet((int)0);
				res = (int)LOGIC_ERR_SUCCESS;
			}
			char *p = (char *)malloc(size + 1);
			if (p) {
				fread(p, size, 1, pf);
				p[size] = 0;
				result.InitSet((void*)p, (size_t)size);
				free(p);

				res = (int)LOGIC_ERR_SUCCESS;
				break;
			}
		}
		break;
	}
	default:

		break;
	}

	if (res != (int)LOGIC_ERR_SUCCESS) {
		parser->setErrno(res);
		return false;
	}
	return true;
}

//bool apollo_get_dbl_name(LogicParserEngine*parser, parse_arg_list_t &args, LogicDataObj &result)
APOLLO_SCRIPT_API_DEF(apollo_get_dbl_name, "获得dbl信息()")
{
	DBLDatabase *pdbl = DBLDatabase::get_Instant();
	if (!pdbl) {
		nd_logerror("load database library error\n");
		parser->setErrno(NDERR_PROGRAM_OBJ_NOT_FOUND);
		return false;
	}
	result.InitSet(pdbl->getDatabaseName());
	return true;
}
APOLLO_SCRIPT_API_DEF(apollo_get_dbl_encode, "获得dbl编码()")
{
	DBLDatabase *pdbl = DBLDatabase::get_Instant();
	if (!pdbl) {
		nd_logerror("load database library error\n");
		parser->setErrno(NDERR_PROGRAM_OBJ_NOT_FOUND);
		return false;
	}
	const char *pName = nd_get_encode_name(pdbl->GetEncodeType());
	if (pName) {
		result.InitSet(pName);
		return true;
	}
	return false;
}

APOLLO_SCRIPT_API_DEF(apollo_read_excel_line, "读取excel行json(tablename, index)")
{
	CHECK_ARGS_NUM(args, 3, parser);
	const char*tableName = args[1].GetText();
	int index = args[2].GetInt();

	if (!tableName || !*tableName) {
		parser->setErrno(NDERR_PARAM_INVALID);
		return false;
	}
	DBLTable *pTable = DBL_FindTable(tableName);
	if (!pTable) {
		nd_logerror("table %s not exist\n", tableName);
		parser->setErrno(NDERR_PROGRAM_OBJ_NOT_FOUND);
		return false;
	}

	DBLCursor cursor(pTable);
	if (-1 == cursor.Fetch(index)) {
		nd_logerror("fetch %d line from table %s error\n", index, tableName);
		parser->setErrno(NDERR_PROGRAM_OBJ_NOT_FOUND);
		return false;
	}

	LogicUserDefStruct line;
	for (int i = 0; i < cursor.GetCols(); i++) {
		line.push_back(cursor.GetAliasName(i), cursor[i]);
	}

	result.InitSet(line);
	return true;
}

APOLLO_SCRIPT_API_DEF(apollo_read_excel_node, "读取excel单元格(tablename, fieldName, index)")
{
	CHECK_ARGS_NUM(args, 4, parser);
	const char*tableName = args[1].GetText();
	const char*fieldName = args[2].GetText();
	int index = args[3].GetInt();

	if (!tableName || !fieldName) {
		parser->setErrno(NDERR_PARAM_INVALID);
		return false;
	}

	//LogicEngineRoot *root = LogicEngineRoot::get_Instant();
	result = DBL_FindDataObject_ex(tableName, index, fieldName, ND_ENCODE_TYPE);
	return true;
}

APOLLO_SCRIPT_API_DEF(apollo_read_excel_line_text, "读取excel行text(tablename, index)")
{
	CHECK_ARGS_NUM(args, 3, parser);
	const char*tableName = args[1].GetText();
	int index = args[2].GetInt();

	if (!tableName || !*tableName) {
		parser->setErrno(NDERR_PARAM_INVALID);
		return false;
	}
	DBLTable *pTable = DBL_FindTable(tableName);
	if (!pTable) {
		nd_logerror("table %s not exist\n", tableName);
		parser->setErrno(NDERR_PROGRAM_OBJ_NOT_FOUND);
		return false;
	}

	DBLCursor cursor(pTable);
	if (-1 == cursor.Fetch(index)) {
		nd_logerror("fetch %d line from table %s error\n", index, tableName);
		parser->setErrno(NDERR_PROGRAM_OBJ_NOT_FOUND);
		return false;
	}


	char buf[4096];
	char *p = buf;
	int size = (int)sizeof(buf);
	int len = snprintf(p, size, "[");
	p += len;
	size -= len;

	LogicUserDefStruct line;
	for (int i = 0; i < cursor.GetCols(); i++) {
		std::string text = cursor[i].GetString();
		if (i == cursor.GetCols() - 1) {
			len = snprintf(p, size, "[%s]", text.size() ? text.c_str() : "none");
		}
		else {
			len = snprintf(p, size, "[%s],", text.size() ? text.c_str() : "none");
		}
		p += len;
		size -= len;
		if (size <= 0) {
			break;
		}
	}

	snprintf(p, size, "]");
	result.InitSet(buf);
	return true;
}

//bool apollo_run_test(LogicParserEngine*parser, parse_arg_list_t &args, LogicDataObj &result)
APOLLO_SCRIPT_API_DEF(apollo_run_test, "测试数据导出(test_output_path)")
{

	CHECK_ARGS_NUM(args, 2, parser);
	CHECK_DATA_TYPE(args[1], OT_STRING, parser);
	//test database library
	DBLDatabase *pdbl = DBLDatabase::get_Instant();
	if (!pdbl) {
		nd_logerror("load database library error\n");
		parser->setErrno(NDERR_PROGRAM_OBJ_NOT_FOUND);
		return false;
	}
	if (-1 == pdbl->Test(args[1].GetText())) {
		nd_logerror("TEST database library error\n");
		return false;
	}

	return true;
}


nd_handle apollo_getNetHanlder(ILogicObject *owner, LogicDataObj &netObj)
{
	if (netObj.GetDataType() == OT_STRING) {
		LogicDataObj netHandle;
		if (owner  && owner->getOtherObject(netObj.GetText(), netHandle)) {
			if (netHandle.CheckValid() && netHandle.GetDataType() != OT_STRING) {
				return apollo_getNetHanlder(owner, netHandle);
			}
		}
		return NULL;
	}
	else if (netObj.GetDataType() == OT_OBJ_NDHANDLE) {
		return (nd_handle)netObj.GetObjectAddr();
	}
	else if (netObj.GetDataType() == OT_OBJ_NDOBJECT) {
		NDIBaseObj *pBaseObj = (NDIBaseObj*)netObj.GetObjectAddr();
		if (!pBaseObj) {
			return NULL;
		}
		if (dynamic_cast<NDObject*>(pBaseObj)) {
			return dynamic_cast<NDObject*>(pBaseObj)->GetHandle();
		}
	}
	return NULL;
}

//bool apollo_set_message_handler(LogicParserEngine*parser, parse_arg_list_t &args, LogicDataObj &result)
APOLLO_SCRIPT_API_DEF(apollo_set_message_handler, "安装消息处理器(netobject, str:func, int maxId, int minId, int privilege)")
{
	CHECK_ARGS_NUM(args, 6, parser);

	//CHECK_DATA_TYPE(args[1], OT_STRING, parser);
	CHECK_DATA_TYPE(args[2], OT_STRING, parser);
	int maxId = args[3].GetInt();
	int minId = args[4].GetInt();

	bool ret = false;
	LogicDataObj netHandle;

	nd_handle h = apollo_getNetHanlder(parser->getOwner(), args[1]);
	if (h) {
		if (0 == nd_msgentry_script_install(h, args[2].GetText(), maxId, minId, args[5].GetInt())) {
			ret = true;
			nd_logdebug("installl MESSAGE (%d,%d) handler %s success\n", maxId, minId, args[2].GetText());
		}
		else {
			nd_logerror("installl (%d,%d) %s error \n", maxId, minId, args[2].GetText());
		}
	}

	if (!ret) {
		parser->setErrno(LOGIC_ERR_AIM_OBJECT_NOT_FOUND);
	}
	return ret;
}

//call message-handler write by script for test
//bool apollo_call_script_msgHandler_test(LogicParserEngine*parser, parse_arg_list_t &args, LogicDataObj &result)
APOLLO_SCRIPT_API_DEF(apollo_call_script_msgHandler_test, "测试消息处理(script, maxId,minId, data1,data2...)")
{
	CHECK_ARGS_NUM(args, 4, parser);

	CHECK_DATA_TYPE(args[1], OT_STRING, parser);
	const char *pScript = args[1].GetText();
	int maxId = args[2].GetInt();
	int minId = args[3].GetInt();


	//construct input message 
	NDOStreamMsg omsg(maxId, minId);

	for (size_t i = 4; i < args.size(); i++) {
		LogicDataObj &dataMsg = args[i];
		if (-1 == logicDataWrite(dataMsg, omsg)) {
			nd_logerror("apollo_func_send_msg args[%d] write to message-buf error \n", i);

			parser->setErrno(NDERR_BAD_GAME_OBJECT);
			return false;
		}
	}

	NDIStreamMsg inmsg(omsg.GetMsgAddr());

	//call function 
	//LogicDataObj data;
	parse_arg_list_t params;

	//function name 
	//data.InitSet(script);
	params.push_back(LogicDataObj(pScript));

	//receive message user
	//data.InitSet((void*)pSession, OT_OBJ_BASE_OBJ);
	params.push_back(LogicDataObj((void*)NULL, OT_OBJ_BASE_OBJ));

	//message object
	//data.InitSet((void*)&inmsg, OT_OBJ_MSGSTREAM);
	params.push_back(LogicDataObj((void*)&inmsg, OT_OBJ_MSGSTREAM));

	//message function is global
	return parser->callFunc(params, GLOBAL_MODULE_NAME);

}

//send message api apollo_func_send_msg
//bool apollo_func_send_msg(LogicParserEngine*parser, parse_arg_list_t &args, LogicDataObj &result)
APOLLO_SCRIPT_API_DEF(apollo_func_send_msg, "发送消息(int:maxID,int:minId, msg_varlist...)")
{
	CHECK_ARGS_NUM(args, 3, parser);

	int maxId = 0, minId = 0;
	maxId = args[1].GetInt();
	minId = args[2].GetInt();

	NDOStreamMsg omsg(maxId, minId);

	omsg.SkipStructEndMark();
	for (size_t i = 3; i < args.size(); i++) {
		LogicDataObj &dataMsg = args[i];
		if (-1 == logicDataWrite(dataMsg, omsg)) {
			nd_logerror("apollo_func_send_msg args[%d] write to message-buf error \n", i);

			parser->setErrno(NDERR_BAD_GAME_OBJECT);
			return false;
		}
	}


	LogicDataObj val;
	if (parser->getOwner()->getOtherObject("session", val)) {
		if (val.GetDataType() == OT_OBJ_NDHANDLE) {
			nd_handle h = (nd_handle)val.GetObjectAddr();
			if (h) {
				nd_connector_send(h, (nd_packhdr_t*)omsg.GetMsgAddr(), 0);
				return true;
			}
		}
		else if (val.GetDataType() == OT_OBJ_NDOBJECT) {
			NDObject *obj = (NDObject*)val.GetObjectAddr();
			if (obj) {
				nd_handle h = obj->GetHandle();
				if (h) {
					nd_connector_send(h, (nd_packhdr_t*)omsg.GetMsgAddr(), 0);
					return true;
				}
			}
		}
	}

	parser->setErrno(NDERR_BAD_GAME_OBJECT);
	return false;
}
APOLLO_SCRIPT_API_DEF(apollo_func_send_msg_by_netobject, "发送消息通过特定接口(connector,int:maxID,int:minId, msg_varlist...)")
{
	CHECK_ARGS_NUM(args, 4, parser);
	int maxId = 0, minId = 0;
	maxId = args[2].GetInt();
	minId = args[3].GetInt();

	NDOStreamMsg omsg(maxId, minId);
	omsg.SkipStructEndMark();
	for (size_t i = 4; i < args.size(); i++) {
		LogicDataObj &dataMsg = args[i];
		if (-1 == logicDataWrite(dataMsg, omsg)) {
			nd_logerror("apollo_func_send_msg args[%d] write to message-buf error \n", i);

			parser->setErrno(NDERR_BAD_GAME_OBJECT);
			return false;
		}
	}

	nd_handle h = apollo_getNetHanlder(parser->getOwner(), args[1]);
	if (h) {
		nd_connector_send(h, (nd_packhdr_t*)omsg.GetMsgAddr(), 0);
		return true;
	}

	parser->setErrno(NDERR_BAD_GAME_OBJECT);
	return false;
}

//send message api
//bool apollo_func_read_msg(LogicParserEngine*parser, parse_arg_list_t &args, LogicDataObj &result)
APOLLO_SCRIPT_API_DEF(apollo_func_read_msg, "读取消息(int:数据类型)")
{
	CHECK_ARGS_NUM(args, 3, parser);

	CHECK_DATA_TYPE(args[1], OT_OBJ_MSGSTREAM, parser);

	NDIStreamMsg *pInmsg = (NDIStreamMsg *)args[1].GetObjectAddr();
	bool bSkipMarker = pInmsg->SetSkipMarker(true);

	CHECK_DATA_TYPE(args[2], OT_INT, parser);
	DBL_ELEMENT_TYPE t = (DBL_ELEMENT_TYPE)args[2].GetInt();

	LogicDataObj val(NULL, t, (DBL_ELEMENT_TYPE)0);
	if (0 != logicDataRead(val, *pInmsg)) {
		nd_logerror("read message error\n");
		parser->setErrno(NDERR_NETMSG_FORMAT);
		pInmsg->SetSkipMarker(bSkipMarker);
		return false;
	}
	pInmsg->SetSkipMarker(bSkipMarker);
	result = val;
	return true;
}


//send message api
//apollo_func_read_userData_from_msg(msg_stream, data_name )
static bool _get_format_type(LogicParserEngine*parser, const char *typeName, LogicDataObj &type)
{
	DBL_ELEMENT_TYPE t = (DBL_ELEMENT_TYPE)get_type_from_alias(typeName);
	if (t != OT_USER_DEFINED) {
		type = LogicDataObj(NULL, t, (DBL_ELEMENT_TYPE)0);
	}
	else {
		LogicDataObj datatype;
		if (!parser->getOwner()->getOtherObject("FormatMsgData", datatype)) {
			nd_logerror("not found datatype define \n");

			parser->setErrno(NDERR_PROGRAM_OBJ_NOT_FOUND);
			return false;
		}

		userDefineDataType_map_t *pDataType = (userDefineDataType_map_t*)datatype.GetObjectAddr();
		if (!pDataType) {
			nd_logerror("not found datatype define \n");
			parser->setErrno(NDERR_PROGRAM_OBJ_NOT_FOUND);
			return false;
		}

		userDefineDataType_map_t::iterator it = pDataType->find(typeName);
		if (it == pDataType->end()) {
			parser->setErrno(NDERR_INVALID_INPUT);
			nd_logerror("not found data %s \n", typeName);
			return false;
		}
		type.InitSet(it->second);
	}
	return true;
}

//get data type
//bool apollo_func_get_userDataType(LogicParserEngine*parser, parse_arg_list_t &args, LogicDataObj &result)
APOLLO_SCRIPT_API_DEF(apollo_func_get_userDataType, "获得消息数据类型(类型名字)")
{
	CHECK_ARGS_NUM(args, 2, parser);
	CHECK_DATA_TYPE(args[1], OT_STRING, parser);
	if (!args[1].CheckValid()) {
		nd_logerror("data type name is NULL\n");

		parser->setErrno(NDERR_PARAM_INVALID);
		return false;
	}

	if (!_get_format_type(parser, args[1].GetText(), result)) {
		nd_logerror("data type name %s not found\n", args[1].GetText());
		parser->setErrno(NDERR_INVALID_INPUT);
		return false;
	}
	return true;
}

//bool apollo_func_read_userData_from_msg(LogicParserEngine*parser, parse_arg_list_t &args, LogicDataObj &result)
APOLLO_SCRIPT_API_DEF(apollo_func_read_userData_from_msg, "从消息中读UserDef类型(输入消息, 类型名字)")
{
	CHECK_ARGS_NUM(args, 3, parser);
	CHECK_DATA_TYPE(args[2], OT_STRING, parser);
	if (!args[2].CheckValid()) {
		nd_logerror("data type name is NULL\n");
		parser->setErrno(NDERR_PARAM_INVALID);
		return false;
	}

	if (!_get_format_type(parser, args[2].GetText(), result)) {
		nd_logerror("data type name %s not found\n", args[2].GetText());
		parser->setErrno(NDERR_INVALID_INPUT);
		return false;
	}

	if (parser->checkSimulate()) {
		return true;
	}

	CHECK_DATA_TYPE(args[1], OT_OBJ_MSGSTREAM, parser);
	NDIStreamMsg *pInmsg = (NDIStreamMsg *)args[1].GetObjectAddr();
	bool bSkipMarker = pInmsg->SetSkipMarker(true);

	if (-1 == logicDataRead(result, *pInmsg)) {
		nd_logerror("read data-type %s error from message  \n", args[2].GetText());
		parser->setErrno(NDERR_NETMSG_FORMAT);
		pInmsg->SetSkipMarker(bSkipMarker);
		return false;
	}
	pInmsg->SetSkipMarker(bSkipMarker);
	return true;

}

//for gmtool

static bool _binData2Json(LogicParserEngine*parser, void *data, size_t size, const char*typeName, LogicDataObj &val)
{
	if (!_get_format_type(parser, typeName, val)) {
		nd_logerror("data type name %s not found\n", typeName);
		parser->setErrno(NDERR_INVALID_INPUT);
		return false;
	}

	nd_usermsgbuf_t msgbuf;
	nd_usermsghdr_init(&msgbuf.msg_hdr);

	memcpy(msgbuf.data, data, size);

	ND_USERMSG_LEN(&msgbuf) += (NDUINT16)size;
	NDIStreamMsg inmsg(&msgbuf);
	inmsg.SetSkipMarker(true);

	if (-1 == logicDataRead(val, inmsg)) {
		nd_logerror("read data-type %s error from message  \n", typeName);
		parser->setErrno(NDERR_READ_STREAM);
		return false;
	}
	return true;
}

//bool apollo_func_binary_to_userData(LogicParserEngine*parser, parse_arg_list_t &args, LogicDataObj &result)
APOLLO_SCRIPT_API_DEF(apollo_func_binary_to_userData, "二进制转dataType(binary,类型名字)")
{
	CHECK_ARGS_NUM(args, 3, parser);
	CHECK_DATA_TYPE(args[1], OT_BINARY_DATA, parser);
	CHECK_DATA_TYPE(args[2], OT_STRING, parser);

	return _binData2Json(parser, args[1].GetBinary(), args[1].GetBinarySize(), args[2].GetText(), result);

}

static bool _additon2Json(LogicParserEngine*parser, void *data, size_t size, LogicUserDefStruct &outJsonData)
{
	LogicUserDefStruct val;
	const char*pRet = val._readFromBin((const char*)data, size);
	if (!pRet) {
		nd_logerror("read stream to json error \n");
		parser->setErrno(NDERR_READ_STREAM);
		return false;
	}
	for (size_t i = 0; i < val.count(); i++) {
		char name[128];
		const char *pName = val.getName((int)i);
		LogicDataObj *pData = val.ref(i);
		nd_assert(pData);

		bool isResetData = false;;
		do {
			if (pData->GetDataType() != OT_BINARY_DATA) {
				break;
			}
			const char *pN = strstr(pName, "$name=");
			if (!pN) {
				break;
			}
			pN += 6;
			pN = ndstr_nstr_end(pN, name, '$', sizeof(name));
			pName = name;

			if (!pN || *pN != '$') {
				break;
			}
			const char* typeName = strchr(pN, '=');
			if (typeName) {
				++typeName;
			}

			if (0 == ndstricmp(typeName, "unknown")) {
				break;
			}
			else if (0 == ndstricmp(typeName, "UserAdditionData")) {
				isResetData = true;
				LogicUserDefStruct subJson;
				if (_additon2Json(parser, pData->GetBinary(), pData->GetBinarySize(), subJson)) {
					outJsonData.push_back(pName, LogicDataObj(subJson));
				}
			}
			else {
				isResetData = true;
				LogicDataObj subData;
				if (_binData2Json(parser, pData->GetBinary(), pData->GetBinarySize(), typeName, subData)) {
					outJsonData.push_back(pName, subData);
				}
			}
		} while (0);

		if (!isResetData) {
			outJsonData.push_back(pName, *pData);
		}

	}
	return true;
}

APOLLO_SCRIPT_API_DEF(apollo_func_addition_to_json, "addition转dataType(binary)")
{
	CHECK_ARGS_NUM(args, 2, parser);
	CHECK_DATA_TYPE(args[1], OT_BINARY_DATA, parser);

	LogicUserDefStruct jsonData;
	if (_additon2Json(parser, args[1].GetBinary(), args[1].GetBinarySize(), jsonData)) {
		result.InitSet(jsonData);
		return true;
	}
	return false;
}
