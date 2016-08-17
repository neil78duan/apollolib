/* file logicengine_api.h
 *
 * interface of game logic engine 
 *
 * create by duan 
 *
 * 2015-5-20
 */

#include "ndapplib/applib.h"
#include "logic_parser/logicEngineRoot.h"
#include "logic_parser/logicParser.h"
#include "logic_parser/logic_function.h"
#include "logic_parser/dbldata2netstream.h"
#include "logic_parser/dbl_mgr.h"
//#include "message_inc.h"
#include <stdarg.h>


//send message api apollo_func_send_msg

static bool apollo_set_message_handler(LogicParserEngine*parser, parse_arg_list_t &args, DBLDataNode &result);
static bool apollo_func_install_event_handler(LogicParserEngine*parser, parse_arg_list_t &args, DBLDataNode &result);
static bool apollo_func_send_msg(LogicParserEngine*parser, parse_arg_list_t &args, DBLDataNode &result);
static bool apollo_func_read_msg(LogicParserEngine*parser, parse_arg_list_t &args, DBLDataNode &result);
static bool apollo_func_change_time(LogicParserEngine*parser, parse_arg_list_t &args, DBLDataNode &result);
static bool apollo_func_exit(LogicParserEngine*parser, parse_arg_list_t &args, DBLDataNode &result);

static bool apollo_func_read_userData_from_msg(LogicParserEngine*parser, parse_arg_list_t &args, DBLDataNode &result);

static bool apollo_load_script_file(LogicParserEngine*parser, parse_arg_list_t &args, DBLDataNode &result);
static bool apollo_export_cpp_api(LogicParserEngine*parser, parse_arg_list_t &args, DBLDataNode &result);
//
//static NDUINT32 _getMsgid(DBLDataNode &data , nd_handle hListen)
//{
//	NDUINT32 msgID = -1;
//
//	msgID = nd_msgentry_get_id(hListen, data.GetText());
//	if ((NDUINT32)-1 == msgID) {
//		msgID = NetMessage::getIDByName(data.GetText()) ;
//		if((NDUINT32)-1 == msgID) {
//			nd_logerror("get message id %s msg not found \n", data.GetText());
//		}
//	}
//	return msgID;
//}



int init_sys_functions(LogicEngineRoot *root)
{
	
	// install c api
	root->installFunc(apollo_set_message_handler, "apollo_set_message_handler", "处理消息(str:listener, str:func, int maxId, int minId, int privilege)");
	root->installFunc(apollo_func_install_event_handler, "apollo_func_install_event_handler", "安装事件处理器(str:function, int:event_id)");
	root->installFunc(apollo_func_send_msg, "apollo_func_send_msg", "发送消息(int:maxID,int:minId, msg_varlist...)");
	root->installFunc(apollo_func_read_msg, "apollo_func_read_msg", "读取消息(int:数据类型)");
	root->installFunc(apollo_func_change_time, "apollo_func_change_time", "修改服务器时间(int:add_hours, int:add_minutes)");
	root->installFunc(apollo_func_read_userData_from_msg, "apollo_func_read_userData_from_msg", "从消息中读取类型(输入消息, 类型名字)");
	
	root->installFunc(apollo_load_script_file, "apollo_load_script_file", "加载脚本(scriptn_name)");
	root->installFunc(apollo_export_cpp_api, "apollo_export_cpp_api", "导出c函数(filename)");

	return 0;
}

bool apollo_load_script_file(LogicParserEngine*parser, parse_arg_list_t &args, DBLDataNode &result)
{
	
	CHECK_ARGS_NUM(args, 2);
	CHECK_DATA_TYPE(args[1], OT_STRING);
	
	LogicEngineRoot *root = LogicEngineRoot::get_Instant() ;
	nd_assert(root);
	if (-1== root->LoadScript(args[1].GetText(), parser ) ) {
		nd_logerror("load %s script error \n", args[1].GetText()) ;
		return  false ;
	}
	return  true ;
}


bool apollo_export_cpp_api(LogicParserEngine*parser, parse_arg_list_t &args, DBLDataNode &result)
{
	
	CHECK_ARGS_NUM(args, 2);
	CHECK_DATA_TYPE(args[1], OT_STRING);
	
	if(-1==common_export_c_api_descript(args[1].GetText()) ) {
		nd_logerror("export cpp api to %s error \n", args[1].GetText()) ;
		return false ;
	}
	
	return  true ;
}


int common_export_c_api_descript(const char *outfile)
{
	LogicEngineRoot *logicEngine = LogicEngineRoot::get_Instant();
	nd_assert(logicEngine);
	return logicEngine->dumbCPPfunc(outfile);
}

bool apollo_set_message_handler(LogicParserEngine*parser, parse_arg_list_t &args, DBLDataNode &result)
{
	CHECK_ARGS_NUM(args, 6);

	CHECK_DATA_TYPE(args[1], OT_STRING);
	CHECK_DATA_TYPE(args[2], OT_STRING);
	int maxId = args[3].GetInt();
	int minId = args[4].GetInt();
	const char *pScript = args[2].GetText();

	bool ret = false;
	DBLDataNode netHandle;
	LogicObjectBase *owner = parser->getOwner();

	if (owner && owner->getOtherObject(args[1].GetText(), netHandle)) {
		nd_handle hListen = (nd_handle) netHandle.GetObject();
		if (hListen){

			if (0 == nd_msgentry_script_install(hListen, args[2].GetText(), maxId, minId, args[5].GetInt())) {
				ret = true;
			}
			else {
				nd_logerror("apollo_set_message_handler installl (%d,%d) to %s error \n", maxId, minId, args[1].GetText());
			}
		}
	}

	if (!ret)	{
		parser->setErrno(LOGIC_ERR_AIM_OBJECT_NOT_FOUND);
	}
	return ret;
}

//install event handler
bool apollo_func_install_event_handler(LogicParserEngine*parser, parse_arg_list_t &args, DBLDataNode &result)
{
	CHECK_ARGS_NUM(args, 3);
	CHECK_DATA_TYPE(args[1], OT_STRING);
	CHECK_DATA_TYPE(args[2], OT_INT);

	LogicEngineRoot *logicEngine = LogicEngineRoot::get_Instant();
	logicEngine->installEvent(args[2].GetInt(), args[1].GetText());
	return true;
}
//send message api apollo_func_send_msg
bool apollo_func_send_msg(LogicParserEngine*parser, parse_arg_list_t &args, DBLDataNode &result)
{
	CHECK_ARGS_NUM(args, 3);

	//CHECK_DATA_TYPE(args[2], OT_STRING);
	int maxId =0, minId = 0; 
	maxId = args[1].GetInt();
	minId = args[2].GetInt();

	NDOStreamMsg omsg(maxId, minId);
	for (size_t i = 3; i < args.size(); i++) {
		DBLDataNode &dataMsg = args[i];
		if (-1 == logicDataWrite(dataMsg, omsg)) {
			nd_logerror("apollo_func_send_msg args[%d] write to message-buf error \n", i);
			return false;
		}
	}
	DBLDataNode val;
	if (parser->getOwner()->getOtherObject("session", val)) {
		if (val.GetDataType() ==OT_OBJ_NDHANDLE){
			nd_handle h = (nd_handle) val.GetObject();
			if (h)	{
				nd_connector_send(h, (nd_packhdr_t*)omsg.GetMsgAddr(), 0);
				return true;
			}
		}
		else if (val.GetDataType() == OT_OBJ_NDOBJECT) {
			NDObject *obj = (NDObject*)val.GetObject();
			if (obj){
				nd_handle h = obj->GetHandle();
				if (h)	{
					nd_connector_send(h, (nd_packhdr_t*)omsg.GetMsgAddr(), 0);
					return true;
				}
			}
		}
	}
	
	return false;
}

//send message api
bool apollo_func_read_msg(LogicParserEngine*parser, parse_arg_list_t &args, DBLDataNode &result)
{
	CHECK_ARGS_NUM(args, 3);	
		
	CHECK_DATA_TYPE(args[1], OT_OBJ_MSGSTREAM);
	NDIStreamMsg *pInmsg = (NDIStreamMsg *)args[1].GetObject();

	CHECK_DATA_TYPE(args[2], OT_INT);
	DBL_ELEMENT_TYPE t = (DBL_ELEMENT_TYPE)args[2].GetInt();
		
	DBLDataNode val(NULL, t, (DBL_ELEMENT_TYPE) 0);
	if (0 != logicDataRead(val,*pInmsg) ) {
		nd_logerror("read message error\n");
		return false;
	}
	result = val;
	return true;
}

//send message api
//apollo_func_read_userData_from_msg(msg_stream, data_name 
bool apollo_func_read_userData_from_msg(LogicParserEngine*parser, parse_arg_list_t &args, DBLDataNode &result)
{
	CHECK_ARGS_NUM(args, 3);

	CHECK_DATA_TYPE(args[1], OT_OBJ_MSGSTREAM);
	NDIStreamMsg *pInmsg = (NDIStreamMsg *)args[1].GetObject();

	CHECK_DATA_TYPE(args[2], OT_STRING);

	DBLDataNode datatype;
	if (!parser->getOwner()->getOtherObject("FormatMsgData", datatype)){
		nd_logerror("not found datatype define \n");
		return false;
	}

	userDefineDataType_map_t *pDataType = (userDefineDataType_map_t*) datatype.GetObject();
	if (!pDataType)	{
		nd_logerror("not found datatype define \n");
		return false;
	}

	userDefineDataType_map_t::iterator it = pDataType->find(args[2].GetText());
	if (it == pDataType->end())	{
		nd_logerror("not found data %s \n", args[2].GetText());
		return false;
	}
	result.InitSet(it->second);

	if (-1 == logicDataRead(result, *pInmsg)) {
		nd_logerror("read data-type %s error from message  \n", args[2].GetText());
		return false;
	}
	return true;
}

//change server time 
bool apollo_func_change_time(LogicParserEngine*parser, parse_arg_list_t &args, DBLDataNode &result)
{
	CHECK_ARGS_NUM(args, 3);

	int hours = args[1].GetInt();
	int minutes = args[2].GetInt();
	time_t pretm = app_inst_time(NULL);
	app_inst_set_hm(hours, minutes);

	time_t nowtm = app_inst_time(NULL);
	char buf1[64];
	char buf2[64];
	nd_logmsg("change time from %s to %s\n", 
		nd_get_datetimestr_ex(pretm, buf1, 64), nd_get_datetimestr_ex(nowtm, buf2, 64));
	return true;
}

// houst eixt
bool apollo_func_exit(LogicParserEngine*parser, parse_arg_list_t &args, DBLDataNode &result)
{
	NDInstanceBase *pInst= getbase_inst();
	if (pInst){
		nd_logmsg("the host shutdown now\n");
		nd_host_eixt();
		//pInst->Destroy(0);
		//nd_instance_exit(0);
	}
	//inst.End(0);
	return true;
}



//////////////////////////////////////
// public cpp function 

static int _apollo_log(void *pf, const char *stm, ...)
{
	char buf[1024 * 4];
	char *p = buf;
	va_list arg;
	int done;

	va_start(arg, stm);
	done = vsnprintf(p, sizeof(buf), stm, arg);
	va_end(arg);

	nd_logtext(buf);
	return done;
}

static bool _apollo_out(parse_arg_list_t &args, logic_print print_func, void *pf)
{
#ifdef _MSC_VER
	for (int i = 0; i < args.size(); i++){
		if (args[i].GetDataType() == OT_STRING)	{
			DBLDataNode data = args[i];
			data.ConvertEncode(LogicEngineRoot::get_Instant()->GetEncodeType(), ND_ENCODE_TYPE);
			//char buf[4096];
			print_func(pf, "%s ", data.GetText());
		}
		else {
			args[i].Print(print_func, pf);
			print_func(pf, " ");
		}
	}
	print_func(pf, "\n");
#else 
	for (int i = 0; i < args.size(); i++){
		args[i].Print(print_func, pf);
		print_func(pf, " ");
	}
	print_func(pf, "\n");

#endif
	return true;
}

bool apollo_log(LogicParserEngine*parser, parse_arg_list_t &args, DBLDataNode &result)
{
	return _apollo_out(args, _apollo_log, NULL);
}


bool apollo_printf(LogicParserEngine*parser, parse_arg_list_t &args, DBLDataNode &result)
{
//#ifdef ND_OUT_LOG_2CTRL
	LogicEngineRoot *root = LogicEngineRoot::get_Instant();
	nd_assert(root);
	logic_print print_func = root->m_screen_out_func;
	void *pf = (void*)root->m_print_file;
	if (!print_func)	{
		print_func = (logic_print)fprintf;
		pf = (void*)stdout;
	}
	return _apollo_out(args, print_func, pf);
//#else
//	return true;
//#endif

}

#ifdef ND_OUT_LOG_2CTRL
int _apollo_script_printf(const char *stm, ...)
{
	char buf[1024 * 4];
	char *p = buf;
	va_list arg;
	int done;

	va_start(arg, stm);
	done = vsnprintf(p, sizeof(buf), stm, arg);
	va_end(arg);

	apollo_logic_out_put(buf);
	return done;
}
#endif

int apollo_logic_out_put(const char *text)
{
#ifdef ND_OUT_LOG_2CTRL
	LogicEngineRoot *root = LogicEngineRoot::get_Instant();
	nd_assert(root);
	logic_print print_func = root->m_screen_out_func;
	void *pf = (void*)root->m_print_file;
	if (!print_func)	{
		print_func = (logic_print)fprintf;
		pf = (void*)stdout;
	}
	return print_func(pf, "%s", text);
#else 
	return 0;
#endif
}

//////////////////////////////////////////////////////////////////////////

enum eApolloOperateCmd
{
	E_APOLLO_TIME_OP_GET,
	E_APOLLO_TIME_OP_ADD,
	E_APOLLO_TIME_OP_SUB,
	E_APOLLO_TIME_OP_INTERVAL
};

enum eApolloTimeUnit
{
	E_APOLLO_TIME_UNIT_SECOND,
	E_APOLLO_TIME_UNIT_MINUTE,
	E_APOLLO_TIME_UNIT_HOUR,
	E_APOLLO_TIME_UNIT_DAY,
	E_APOLLO_TIME_UNIT_MONTH,
	E_APOLLO_TIME_UNIT_YEAR,
	E_APOLLO_TIME_UNIT_WEEKDAY,

};
//time function 
bool apollo_time_func(LogicParserEngine*parser, parse_arg_list_t &args, DBLDataNode &result)
{

	CHECK_ARGS_NUM(args, 4);

	CHECK_DATA_TYPE(args[1], OT_INT);
	CHECK_DATA_TYPE(args[2], OT_INT);
	//CHECK_DATA_TYPE(args[3], OT_TIME);
	int opType = args[1].GetInt();
	int opUnit = args[2].GetInt();
	if (E_APOLLO_TIME_OP_GET == opType) {
		int val = -1;
		switch (opUnit)
		{
		case E_APOLLO_TIME_UNIT_SECOND:
			val = args[3].GetTimeSecond();
			break;
		case E_APOLLO_TIME_UNIT_MINUTE:
			val = args[3].GetTimeMinute();
			break;
		case E_APOLLO_TIME_UNIT_HOUR:
			val = args[3].GetTimeHour();
			break;
		case E_APOLLO_TIME_UNIT_DAY:
			val = args[3].GetTimeDay();
			break;
		case E_APOLLO_TIME_UNIT_MONTH:
			val = args[3].GetTimeMonth();
			break;

		case E_APOLLO_TIME_UNIT_YEAR:
			val = args[3].GetTimeYear();
			break;
		case E_APOLLO_TIME_UNIT_WEEKDAY:
			val = args[3].GetTimeWeekDay();
			break;
		default:
			return false;
		}
		result.InitSet(val);
	}
	else if (E_APOLLO_TIME_OP_ADD == opType || E_APOLLO_TIME_OP_SUB == opType) {
		if (args.size() < 4){
			nd_logerror("apollo_time_func error need 4 args on ADD/sub\n");
			return false;
		}
		CHECK_DATA_TYPE(args[4], OT_INT);
		NDUINT64 deta = args[4].GetInt();
		time_t orgtime = args[3].GetTime();

		switch (opUnit)
		{
		case E_APOLLO_TIME_UNIT_SECOND:
			break;
		case E_APOLLO_TIME_UNIT_MINUTE:
			deta *= 60;
			break;
		case E_APOLLO_TIME_UNIT_HOUR:
			deta *= 3600;
			break;
		case E_APOLLO_TIME_UNIT_DAY:
			deta *= (3600 * 24);
			break;
		case E_APOLLO_TIME_UNIT_MONTH:
		{
			tm _tm1;
			if (localtime_r(&orgtime, &_tm1)) {
				int year = (int)(deta / 12);
				int month = deta % 12;

				if (E_APOLLO_TIME_OP_ADD) {
					_tm1.tm_mon += month;
					_tm1.tm_year += year;
				}
				else {
					_tm1.tm_mon -= month;
					_tm1.tm_year -= year;
				}

				//roundup month
				if (_tm1.tm_mon >= 12)	{
					_tm1.tm_year += 1;
					_tm1.tm_mon -= 12;
				}
				else if (_tm1.tm_mon < 0)	{
					_tm1.tm_year -= 1;
					_tm1.tm_mon += 12;
				}

				//round up year
				if (_tm1.tm_year < 0){
					orgtime = 0;
				}
				else {
					orgtime = mktime(&_tm1);
				}
				result.InitSet(orgtime);
				return true;
			}

			return false;
		}
		break;

		case E_APOLLO_TIME_UNIT_YEAR:
		{
			tm _tm1;
			if (localtime_r(&orgtime, &_tm1)) {
				if (E_APOLLO_TIME_OP_ADD) {
					_tm1.tm_year += (int)deta;
				}
				else {
					_tm1.tm_year -= (int)deta;
				}

				if (_tm1.tm_year < 0){
					orgtime = 0;
				}
				else {
					orgtime = mktime(&_tm1);
				}
				result.InitSet(orgtime);
				return true;
			}
			return false;
		}
		break;
		case E_APOLLO_TIME_UNIT_WEEKDAY:
			deta *= 7 * 3600 * 24;
			break;
		default:
			return false;
		}
		if (opType == E_APOLLO_TIME_OP_ADD)	{
			orgtime += deta;
		}
		else {
			orgtime += deta;
		}
		result.InitSet(orgtime);
	}
	else if (E_APOLLO_TIME_OP_INTERVAL) {
		CHECK_DATA_TYPE(args[4], OT_TIME);
		time_t tim1 = args[3].GetTime(), tim2 = args[4].GetTime();
		if (tim1 < tim2){
			time_t _tmp = tim1;
			tim1 = tim2;
			tim2 = _tmp;
		}

		int val = 0;
		switch (opUnit)
		{
		case E_APOLLO_TIME_UNIT_SECOND:
			val = (int)(tim1 - tim2);
			break;
		case E_APOLLO_TIME_UNIT_MINUTE:
			val = (int)(tim1 / 60 - tim2 / 60);
			break;
		case E_APOLLO_TIME_UNIT_HOUR:
			val = (int)(tim1 / 3600 - tim2 / 3600);
			break;
		case E_APOLLO_TIME_UNIT_DAY:
			val = nd_time_day_interval(tim1, tim2);
			break;

		case E_APOLLO_TIME_UNIT_MONTH:
		{
			tm _tm1, _tm2;
			if (localtime_r(&tim1, &_tm1) && localtime_r(&tim2, &_tm2)) {
				NDUINT64 month1 = _tm1.tm_year * 12 + _tm1.tm_mon;
				NDUINT64 month2 = _tm2.tm_year * 12 + _tm2.tm_mon;
				val = (int)(month1 - month2);
			}
			else {
				return false;
			}
		}
		break;

		case E_APOLLO_TIME_UNIT_YEAR:
		{
			tm _tm1, _tm2;
			if (localtime_r(&tim1, &_tm1) && localtime_r(&tim2, &_tm2)) {
				NDUINT64 month1 = _tm1.tm_year * 12 + _tm1.tm_mon;
				NDUINT64 month2 = _tm2.tm_year * 12 + _tm2.tm_mon;
				val = _tm1.tm_year - _tm2.tm_year;
			}
			else {
				return false;
			}
		}
		break;

		case E_APOLLO_TIME_UNIT_WEEKDAY:
		{
			int timezone = nd_time_zone();
			tim1 += timezone * 3600;
			tim2 += timezone * 3600; // set localtime as gmtime

			tim1 += 4 * 3600 * 24;
			tim2 += 4 * 3600 * 24;  // 1970.1.1 is Thursday
			val = (int)(tim1 / (3600 * 24 * 7) - tim2 / (3600 * 24 * 7)); //get the week index of time1 and time2
		}

		break;
		default:
			return false;
		}
		result.InitSet(val);
	}
	else {
		return false;
	}
	return true;
}