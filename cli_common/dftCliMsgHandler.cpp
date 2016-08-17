/* file gmtoolMsgHandler.cpp
 * handler of gmtool message 
 *
 * create by duan 
 * 2015-8-14
 */


#include "ndcli/nd_iconn.h"
#include "nd_msg.h"
#include "msg_def.h"

#include "srv_define.h"

#ifndef WITHOUT_LOGIC_PARSER
#include "netMessage/message_inc.h"
#include "logic_parser/dbl_mgr.h"

#include "logic_parser/logicEngineRoot.h"
#include "logic_parser/dbldata2netstream.h"
#endif 

#include "cli_common/dftCliMsgHandler.h"

namespace ClientMsgHandler
{
int client_default_handler(NDIConn* pconn, nd_usermsgbuf_t *msg);

int apollo_cli_msg_script_entry(void *engine, nd_handle  handle, nd_usermsgbuf_t *msg, const char *script);

#ifdef _GUI_TOOL_
#define myfprintf(pf, msg,...) _logmsg(__FUNC__, __FILE__, __LINE__, ND_MSG ,msg,__VA_ARGS__)
#else 
//#define myfprintf(pf, msg,...) fprintf(pf,msg,__VA_ARGS__)
#define myfprintf fprintf
#endif

#ifdef WITHOUT_LOGIC_PARSER
	
int msg_default_handler(NDIConn* pconn, nd_usermsgbuf_t *msg)
{
	int reLine = 0;
	myfprintf(stdout, "recv (%d,%d) message data-len =%d\n", ND_USERMSG_MAXID(msg), ND_USERMSG_MINID(msg), ND_USERMSG_LEN(msg));

	return 0;
}

#else 	
	
class ConnectScriptOwner : public TestLogicObject
{
public:

	bool getOtherObject(const char*objName, DBLDataNode &val)
	{
		if (ndstricmp(objName, "connector") == 0){
			nd_handle h = m_conn->GetHandle();
			val.InitSet((void*)h, OT_OBJ_NDHANDLE);
			return true;
		}
		else if (ndstricmp(objName, "FormatMsgData") == 0){

			val.InitSet((void*)m_dataType);
			return true;
		}
		return false;
	}
	
	NDIConn *m_conn;
	ndxml *m_message_define;
	userDefineDataType_map_t *m_dataType;
	logic_print m_output;
};
static ConnectScriptOwner  __myScriptOwner;

void destroyDftClientMsgHandler(NDIConn *pConn)
{

	__myScriptOwner.m_conn = 0;
	__myScriptOwner.m_message_define = 0;
	__myScriptOwner.m_dataType = 0;
	__myScriptOwner.m_output = 0;

	LogicEngineRoot::destroy_Instant();
}

bool initDftClientMsgHandler(NDIConn *pConn, const char*script_file, userDefineDataType_map_t &dataConfig, ndxml *msg_def, logic_print outfunc)
{
	destroyDftClientMsgHandler(pConn);

	__myScriptOwner.m_conn = pConn;
	__myScriptOwner.m_message_define = msg_def;
	__myScriptOwner.m_dataType = &dataConfig;
	__myScriptOwner.m_output = outfunc;


	// init script 
	LogicEngineRoot *scriptRoot = LogicEngineRoot::get_Instant();
	nd_assert(scriptRoot);
	LogicParserEngine  &parser = LogicEngineRoot::get_Instant()->getGlobalParser();
	nd_message_set_script_engine(pConn->GetHandle(), (void*)&parser, apollo_cli_msg_script_entry);
	parser.setOwner(&__myScriptOwner);

	scriptRoot->setPrint(outfunc, NULL);
	scriptRoot->getGlobalParser().setSimulate(false);
	pConn->SetUserData(&parser);

	if (0 != scriptRoot->LoadScript(script_file)){
		nd_logerror("加载脚本出错！\n");
		return false;
	}
	defaultCliMsgHandlerInit(pConn);
	pConn->SetDftMsgHandler(client_default_handler);
	return true;
}

int apollo_cli_msg_script_entry(void *engine, nd_handle  handle, nd_usermsgbuf_t *msg, const char *script)
{
	NDIConn *pconn = (NDIConn *)handle;
	nd_assert(pconn);

	NDIStreamMsg inmsg(msg);

	LogicParserEngine *scriptEngine = (LogicParserEngine *)pconn->GetUserData();
	nd_assert(scriptEngine);

	//call function 
	//DBLDataNode data;
	parse_arg_list_t params;

	//function name 
	//data.InitSet(script);
	params.push_back(DBLDataNode(script));

	//receive message user
	//data.InitSet((void*)pSession, OT_OBJ_BASE_OBJ);
	params.push_back(DBLDataNode(handle, OT_OBJ_NDHANDLE));

	//message object
	//data.InitSet((void*)&inmsg, OT_OBJ_MSGSTREAM);
	params.push_back(DBLDataNode((void*)&inmsg, OT_OBJ_MSGSTREAM));

	DBLDataNode result;
	scriptEngine->runScript(script, params, result);
	//bool ret = LogicEngineRoot::get_Instant()->runScript(script, scriptEngine, params, result);
	return 0;
}


bool outPutMessageFromconfig(const char *formatText, NDIStreamMsg &inmsg)
{
	userDefineDataType_map_t &m_dataTypeDef = *__myScriptOwner.m_dataType;
	int index = 0;
	char name[128];
	const char *p = ndstr_first_valid(formatText);
	if (!p || !*p)	{
		return false;
	}
	if (0 == ndstricmp(p, "none") || 0 == ndstricmp(p, "null"))	{
		return false;
	}

	int array_size = 0;
	do {
		char memberName[128];
		bool isArray = 0;
		name[0] = 0;
		if (*p == '[') {
			++p;
			p = ndstr_nstr_end(p, name, ']', sizeof(name));
			if (p && *p == ']') {
				++p;
			}
		}
		else {
			p = ndstr_nstr_end(p, name, ',', sizeof(name));
		}
		if (!name[0]){
			break;
		}

		if (isArray)	{
			for (int i = 0; i < array_size; i++) {
				if (!outPutMessageFromconfig(name, inmsg)) {
					return false;
				}
			}
		}
		else {
			const char *pmember = strchr(name, ':');
			if (pmember && *pmember == ':')	{
				++pmember;
				ndstr_parse_word_n(ndstr_first_valid(pmember), memberName, 128);
			}
			else {
				snprintf(memberName, sizeof(memberName), "member%d", index);
			}
			char typeName[128];
			typeName[0] = 0;
			ndstr_parse_word_n(ndstr_first_valid(name), typeName, 128);

			DBLDataNode dataFormat;
			int type_index = get_type_from_alias(typeName);
			if (type_index == OT_USER_DEFINED)	{
				userDefineDataType_map_t::const_iterator it = m_dataTypeDef.find(typeName);
				if (it == m_dataTypeDef.end()){
					return false;
				}
				dataFormat.InitSet(it->second);
			}
			else {
				dataFormat.InitSet((void*)0, (DBL_ELEMENT_TYPE)type_index);
			}

			if (-1 != logicDataRead(dataFormat, inmsg)){
				dataFormat.Print(__myScriptOwner.m_output, NULL);
				if (0 == ndstricmp(memberName, "number") || 0 == ndstricmp(memberName, "count")){
					if (dataFormat.GetDataType() == OT_INT16)	{
						array_size = dataFormat.GetInt();
					}
				}
			}
			else {
				return false;
			}

		}
		if (p && *p == ','){
			++p;
		}
		++index;
	} while (p && *p);

	return true;
}

static ndxml *_getMsgNode(ndxml_root *xmlFile, const char *messageName)
{
	ndxml *root = ndxml_getnode(xmlFile, "MessageDefine");
	if (!root)	{
		return 0;
	}
	for (int i = 0; i < ndxml_getsub_num(root); i++)	{
		ndxml *node = ndxml_getnodei(root, i);
		if (0 == ndstricmp(messageName, ndxml_getattr_val(node, "id"))) {
			return node;
		}
	}
	return NULL;
}
char *convert_msg_name(const char *inname, char *buf, int size)
{
	char *ret = buf;
	while (*inname && size-->0)	{
		if (*inname == '_')	{
			*buf = ' ';
		}
		else {
			*buf = tolower(*inname);
		}
		++buf;
		++inname;
	}
	*buf = 0;
	return ret;
}

int client_default_handler(NDIConn* pconn, nd_usermsgbuf_t *msg)
{
	userDefineDataType_map_t &m_dataTypeDef = *__myScriptOwner.m_dataType;
	NDIStreamMsg inmsg(msg);
	NDUINT16 msgID = ND_MAKE_WORD(inmsg.MsgMaxid(), inmsg.MsgMinid());

	const char *pMsgname = NetMessage::getNameByID(msgID);
	if (!pMsgname || !*pMsgname){
		return 0;
	}
	ndxml *mesageInfo = _getMsgNode(__myScriptOwner.m_message_define, pMsgname);
	if (!mesageInfo){
		return 0;
	}
	const char *body = ndxml_getattr_val(mesageInfo, "body");
	if (!body || !*body)	{
		return 0;
	}
	DBLDataNode dataFormat;
	char namebuf[128];
	//__myScriptOwner.m_output(NULL, "received message %s =======\n", pMsgname);
	__myScriptOwner.m_output(NULL, "%s (%d,%d): ", convert_msg_name(pMsgname,namebuf,128), inmsg.MsgMaxid(), inmsg.MsgMinid());
	outPutMessageFromconfig(body, inmsg);

	__myScriptOwner.m_output(NULL, "\n");

	return 0;
}

//common error handler
int msg_common_error_handler(NDIConn* pconn, nd_usermsgbuf_t *msg)
{
	NDIStreamMsg inmsg(msg);
	NetMessage::CommonErrorReply reply;
	NetMessage::ReadStream(inmsg, reply);

	if (reply.errtType == NetMessage::COMMON_ERROR_TYPE_SHORT_ITEM){
		DBLDataNode data = DBL_FindDataObjectByname("item.xlsx", reply.param, "name");
		if (!data.CheckValid())	{
			data = DBL_FindDataObjectByname("equip_attr.xlsx", reply.param, "name");
			if (!data.CheckValid())		{
				nd_logmsg("COMMON-ERROR-MESSAGE: short item [ %d ] \n",reply.param);
				return 0;
			}
		}
		nd_logmsg("COMMON-ERROR-MESSAGE:short item [ %s ] \n", data.GetText());
	}
	else if (reply.errtType == NetMessage::COMMON_ERROR_TYPE_ATTR_LOW) {
		DBLDataNode data = DBL_FindDataObjectByname("role_attr_config.xlsx", reply.param, "name");
		if (!data.CheckValid())	{
			nd_logmsg("COMMON-ERROR-MESSAGE:[ %d ] 不够\n", reply.param);
		}
		else {
			nd_logmsg("COMMON-ERROR-MESSAGE:[ %s ] 不够\n", data.GetText());
		}

	}
	else if (reply.errtType == NetMessage::COMMON_ERROR_TYPE_IN_CD) {
		nd_logmsg("COMMON-ERROR-MESSAGE: skill [ %s ] IN CD\n",reply.param);
	}
	else if (reply.errtType == NetMessage::COMMON_ERROR_TYPE_IN_TABLE) {

		DBLDataNode data = DBL_FindDataObjectByname("error.xlsx", reply.param, "desc");
		if (!data.CheckValid())	{
			nd_logmsg("COMMON-ERROR-MESSAGE: error-id =%d\n", reply.param);
		}
		else {
			nd_logmsg("COMMON-ERROR-MESSAGE:[ %s ]\n", data.GetText());
		}
	}

	return 0;
}
#endif 

int msg_logout_handler(NDIConn* pconn, nd_usermsgbuf_t *msg)
{
	NDIStreamMsg inmsg(msg);
	NDUINT32 errorCode = 0;

	inmsg.Read(errorCode);

	myfprintf(stdout, "Received logout Message errorcode = %d \n", errorCode);
	return 0;
}

int msg_get_version_handler(NDIConn* pconn, nd_usermsgbuf_t *msg)
{
	NDIStreamMsg inmsg(msg);
	NDUINT32 ver_id = 0;
	NDUINT8 isDebug = 0;
	NDUINT8 buf[1024];
	buf[0] = 0;

	inmsg.Read(ver_id);
	inmsg.Read(isDebug);
	inmsg.Read(buf, sizeof(buf));

	myfprintf(stdout, "Get Server Version ver-id=%d %s\n Version description=%s\n", ver_id, isDebug ? "debug" : "release", buf[0] == 0 ? "unknow" : (const char*)buf);

	return 0;
}

int msg_get_rlimit_handler(NDIConn* pconn, nd_usermsgbuf_t *msg)
{
	NDIStreamMsg inmsg(msg);
	NDUINT8 buf[1024];
	buf[0] = 0;

	inmsg.Read(buf, sizeof(buf));

	myfprintf(stdout, "server rlimit info :\n%s\n", buf[0] == 0 ? "unknow" : (const char*)buf);

	return 0;
}


int msg_show_msg_name_handler(NDIConn* pconn, nd_usermsgbuf_t *msg)
{
	NDIStreamMsg inmsg(msg);
	NDUINT16 maxID, minID;
	NDUINT8 buf[1024];
	inmsg.Read(maxID);

	inmsg.Read(minID);
	inmsg.Read(buf, sizeof(buf));

	myfprintf(stdout, "message (%d, %d)  name=%s \n", maxID, minID, buf);
	return 0;
}


int msg_show_server_time_handler(NDIConn* pconn, nd_usermsgbuf_t *msg)
{
	NDIStreamMsg inmsg(msg);
	NDUINT64 tm1;
	if (0 == inmsg.Read(tm1)) {
		time_t tm2 = (time_t)tm1;

		char str[128];
		myfprintf(stdout, "%s\n", nd_get_datetimestr_ex(tm2, str, 128));
	}


	return 0;
}

int msg_show_game_time_handler(NDIConn* pconn, nd_usermsgbuf_t *msg)
{
	NDIStreamMsg inmsg(msg);
	NDUINT64 tm1;
	if (0 == inmsg.Read(tm1)) {
		time_t tm2 = (time_t)tm1;

		char str[128];
		myfprintf(stdout, "%s\n", nd_get_datetimestr_ex(tm2, str, 128));
	}


	return 0;
}
int msg_broadcast_handler(NDIConn* pconn, nd_usermsgbuf_t *msg)
{
	NDIStreamMsg inmsg(msg);
	char buf[1024];
	buf[0] = 0;
	if (inmsg.Read((NDUINT8*)buf, sizeof(buf))){
		nd_logmsg("BROAD-CASE: %s\n", buf);
	}
	return 0;
}
int msg_chat_handler(NDIConn* pconn, nd_usermsgbuf_t *msg)
{
	NDIStreamMsg inmsg(msg);
	char buf[1024];
	buf[0] = 0;
	if (inmsg.Read((NDUINT8*)buf, sizeof(buf))){
		nd_logmsg("CHAT-MSG: %s\n", buf);
	}
	return 0;
}



void defaultCliMsgHandlerInit(NDIConn *pconn)
{
	CONNECT_INSTALL_MSG(pconn, msg_show_server_time_handler, ND_MAIN_ID_SYS, ND_MSG_SYS_TIME);
	CONNECT_INSTALL_MSG(pconn, msg_show_game_time_handler, ND_MAIN_ID_SYS, ND_MSG_SYS_GAME_TIME);
	CONNECT_INSTALL_MSG(pconn, msg_logout_handler, NETMSG_MAX_LOGIN, LOGIN_MSG_LOGOUT_NTF);

	CONNECT_INSTALL_MSG(pconn, msg_show_msg_name_handler, ND_MAIN_ID_SYS, ND_MSG_SYS_GET_MESSAGE_NAME);
	CONNECT_INSTALL_MSG(pconn, msg_get_version_handler, ND_MAIN_ID_SYS, ND_MSG_SYS_GETVERSION);
	CONNECT_INSTALL_MSG(pconn, msg_get_rlimit_handler, ND_MAIN_ID_SYS, ND_MSG_SYS_GET_RLIMIT);


#ifdef WITHOUT_LOGIC_PARSER
	pconn->SetDftMsgHandler(msg_default_handler);
#endif 
		
}
};