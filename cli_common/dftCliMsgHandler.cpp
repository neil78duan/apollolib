/* file gmtoolMsgHandler.cpp
 * handler of gmtool message 
 *
 * create by duan 
 * 2015-8-14
 */


#include "ndcli/nd_iconn.h"
#include "nd_msg.h"
#include "msg_def.h"

//#include "srv_define.h"

#ifndef WITHOUT_LOGIC_PARSER
//#include "netMessage/message_inc.h"
#include "logic_parser/dbl_mgr.h"
#include "logic_parser/logicEngineRoot.h"
#include "logic_parser/dbldata2netstream.h"
#endif 

#include "cli_common/dftCliMsgHandler.h"

namespace ClientMsgHandler
{

	int msg_default_handler(NDIConn* pconn, nd_usermsgbuf_t *msg)
	{
		int reLine = 0;
		nd_logmsg("recv (%d,%d) message data-len =%d\n", ND_USERMSG_MAXID(msg), ND_USERMSG_MINID(msg), ND_USERMSG_LEN(msg));

		return 0;
	}
		
#ifdef WITHOUT_LOGIC_PARSER
	
#else

	ApoConnectScriptOwner::ApoConnectScriptOwner()
	{
		Destroy();
	}
	ApoConnectScriptOwner::~ApoConnectScriptOwner()
	{
		Destroy();
	}
	void ApoConnectScriptOwner::Destroy()
	{
		m_conn = 0;
		m_dataType.clear();
		m_msgIdName.clear();
	}
	void ApoConnectScriptOwner::setConn(NDIConn *conn)
	{
		m_conn = conn;
	}
	bool ApoConnectScriptOwner::getOtherObject(const char*objName, DBLDataNode &val)
	{
		if (0==ndstricmp(objName, "connector") || 0 == ndstricmp(objName, "session")){
			if (!m_conn) {
				return false;
			}
			nd_handle h = m_conn->GetHandle();
			val.InitSet((void*)h, OT_OBJ_NDHANDLE);
			return true;
		}
		else if (0==ndstricmp(objName, "FormatMsgData")){

			val.InitSet((void*)&m_dataType);
			return true;
		}
		else if (0 == ndstricmp(objName, "msgIdNameFormat")) {
			val.InitSet((void*)&m_msgIdName);
			return true;
		}

		else if (0 == ndstricmp(objName, "SelfName")) {
			val.InitSet("client");
			return true;
		}
		else if (0 == ndstricmp(objName, "self")) {
			val.InitSet((void*)this, OT_OBJ_BASE_OBJ);
			return true;
		}
		else if (0 == ndstricmp(objName, "machineInfo")) {
			char buf[256];
			val.InitSet(nd_common_machine_info(buf, sizeof(buf)));
			return true;
		}
		return false;
	}

	void ApoConnectScriptOwner::LoadMsgDataTypeFromServer( )
	{
		NDOStreamMsg omsg(ND_MAIN_ID_SYS, ND_MSG_SYS_GET_MESSAGE_BUILD_TIME);
		if (m_conn)	{
			m_conn->SendMsg(omsg);
		}
	}
	const char *ApoConnectScriptOwner::getMsgName(int msgId)
	{
		for (msgIdNameFormat_vct::iterator it = m_msgIdName.begin(); it != m_msgIdName.end(); it++){
			if (it->id == msgId) {
				return it->name.c_str();
			}
		}
		return NULL;
	}
	const char *ApoConnectScriptOwner::getMsgBody(int msgId)
	{
		for (msgIdNameFormat_vct::iterator it = m_msgIdName.begin(); it != m_msgIdName.end(); it++){
			if (it->id == msgId) {
				return  it->format.c_str();
			}
		}
		return NULL;
	}


	bool ApoConnectScriptOwner::loadDataType(const char *file)
	{
		if (-1 == loadUserDefFromMsgCfg(file, m_dataType)){
			return false;
		}
		return true;
	}

#define _GET_OBJ_FROM_MGR(_conn,_name,_val) \
	do{				\
		LogicParserEngine  *parser = (LogicParserEngine  *) _conn->GetUserData();	\
		if (!parser){		\
			return NULL;	\
				}					\
		TestLogicObject * owner = (TestLogicObject *)parser->getOwner();			\
		if (!owner)	{		\
			return NULL;	\
				}					\
		if (!owner->getOtherObject(_name, _val)) {		\
			return NULL;	\
				}					\
		} while (0)

	userDefineDataType_map_t *getDataFormat(NDIConn *pconn)
	{
		DBLDataNode val;
		_GET_OBJ_FROM_MGR(pconn, "FormatMsgData", val);
		return (userDefineDataType_map_t *)val.GetObject();
	}
	logic_print getLogFunction(NDIConn *pconn)
	{
		DBLDataNode val;

		_GET_OBJ_FROM_MGR(pconn, "LogFunction", val);
		return (logic_print)val.GetObject();
	}

	std::string getWritablePath(NDIConn *pconn)
	{
		DBLDataNode val;
		_GET_OBJ_FROM_MGR(pconn, "WritablePath", val);
		return val.GetString();
	}
	std::string getLogPath(NDIConn *pconn)
	{
		DBLDataNode val;

		_GET_OBJ_FROM_MGR(pconn, "LogPath", val);
		return val.GetString();
	}

	int OutputMessage2File(NDIConn *pconn, const char *file, const NDIStreamMsg &inmsg)
	{
		char mypath[1024];
		std::string mywritePath = getWritablePath(pconn);
		int ret = inmsg.ToFile(nd_full_path(mywritePath.c_str(), file, mypath, sizeof(mypath)));

		nd_logmsg("write message to file %s %s\n", file, ret == 0 ? "success" : "false");
		return ret;
	}
	int ReadMessageFromFile(NDIConn *pconn, const char *file, NDOStreamMsg &omsg)
	{
		char mypath[1024];		
		std::string mywritePath = getWritablePath(pconn);
		int ret = omsg.FromFile(nd_full_path(mywritePath.c_str(),file, mypath, sizeof(mypath)) );
		nd_logmsg("read message to file %s %s\n", file, ret == 0 ? "success" : "false");
		return ret;
	}


	bool handleMsgFirstFromFileBeforTime(NDIConn *pconn, const char *file, NDUINT64 genTime, NDUINT16 msgId, nd_iconn_func func)
	{
		bool isOK = false;
		NDOStreamMsg omsg;
		//load message id name table 
		if (0 == ReadMessageFromFile(pconn, file, omsg)) {
			nd_usermsgbuf_t *pbufMsg = omsg.GetMsgAddr();
			NDIStreamMsg inmsg(pbufMsg);

			NDUINT64 oldVersionTime = 0;
			inmsg.Read(oldVersionTime);

			if (oldVersionTime >= genTime && 0 == func(pconn, pbufMsg)){
				isOK = true;
				nd_logdebug("laod message %x from %s file success\n", msgId, file) ;
			}
		}

		if (!isOK) {
			NDOStreamMsg msg_send(msgId);
			pconn->SendMsg(msg_send);
			nd_logdebug("load message %x error from %s send to server \n", msgId, file);
			return false;
		}
		return true;
	}

#define APOLLO_DATA_FORMAT "msgformat.dat"
#define APOLLO_MSG_ID_NAME_FORMAT "msgIdName.dat"


	void* getLogFile(NDIConn *pconn)
	{
		DBLDataNode val;

		_GET_OBJ_FROM_MGR(pconn, "LogFile", val);
		return val.GetObject();

	}

	msgIdNameFormat_vct* getMsgIdNameFormat(NDIConn *pconn)
	{
		DBLDataNode val;

		_GET_OBJ_FROM_MGR(pconn, "msgIdNameFormat", val);
		return (msgIdNameFormat_vct*)val.GetObject();

	}


	//get message id-name-and stream format
	int get_id_name_format_handler(NDIConn* pconn, nd_usermsgbuf_t *msg)
	{
		NDIStreamMsg inmsg(msg);

		NDUINT64 build_tm;
		if (inmsg.Read(build_tm) != 0){
			return 0;
		}

		msgIdNameFormat_vct* pIDName = getMsgIdNameFormat(pconn);
		if (!pIDName)	{
			nd_logerror("get id name vector error\n");
			return 0;
		}
		NDUINT16 num;

		if (inmsg.Read(num) != 0){
			return 0;
		}
		pIDName->clear();
		for (int i = 0; i < num; i++)	{
			msgIDNameFormat node;
			node.id = 0;
			char textbuf[1024];
			textbuf[0] = 0;
			inmsg.Read(node.id);
			inmsg.Read(textbuf, sizeof(textbuf));
			node.name = textbuf;
			inmsg.Read(textbuf, sizeof(textbuf));
			node.format = textbuf;
			if (node.id){
				pIDName->push_back(node);
			}
		}

		nd_logdebug("read id-name success !!\n");
		OutputMessage2File(pconn, APOLLO_MSG_ID_NAME_FORMAT, inmsg);

		return 0;
	}

	//get data-type format
	int get_data_format_handler(NDIConn* pconn, nd_usermsgbuf_t *msg)
	{
		NDIStreamMsg inmsg(msg);

		NDUINT64 build_tm;
		if (inmsg.Read(build_tm) != 0){
			return 0;
		}
		userDefineDataType_map_t *pdatatype = getDataFormat(pconn);
		if (!pdatatype)	{
			return 0;
		}

		pdatatype->clear();
		if (-1 == UserDefFormatFromMessage(*pdatatype, inmsg)) {
			nd_logerror("read data type format error\n");
			pdatatype->clear();
			return 0;
		}
		else {
			nd_logdebug("read data format success !!\n");
		}
		OutputMessage2File(pconn, APOLLO_DATA_FORMAT, inmsg);
		
#ifdef ND_DEBUG
		char mypath[1024];
		std::string mywritePath = getWritablePath(pconn);
		nd_full_path(mywritePath.c_str(), "client_msg_foramt.txt", mypath, sizeof(mypath));
		dumpMessageData(*pdatatype, mypath);

#endif
		return 0;
	}


	static bool trytoLoadMessageFormat(NDIConn* pconn, NDUINT64 newVersionTime)
	{

		NDUINT16 msgId = ND_MAKE_WORD(ND_MAIN_ID_SYS, ND_MSG_SYS_GET_MESSAGE_FORMAT_LIST);
		handleMsgFirstFromFileBeforTime(pconn, APOLLO_MSG_ID_NAME_FORMAT, newVersionTime, msgId, get_id_name_format_handler);

		msgId = ND_MAKE_WORD(ND_MAIN_ID_SYS, ND_MSG_SYS_GET_USER_DEFINE_DATA);
		handleMsgFirstFromFileBeforTime(pconn, APOLLO_DATA_FORMAT, newVersionTime, msgId, get_data_format_handler);

		return true;

	}

	int get_message_protocol_build_time(NDIConn* pconn, nd_usermsgbuf_t *msg)
	{
		NDIStreamMsg inmsg(msg);

		NDUINT64 build_tm = 0;
		if (inmsg.Read(build_tm) != 0){
			return 0;
		}

		trytoLoadMessageFormat(pconn, build_tm);

		nd_logmsg("get message protocol build time %lld\n", build_tm);
		return 0;
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


	bool outPutMessageFromconfig(NDIConn* pconn, const char *formatText, NDIStreamMsg &inmsg, userDefineDataType_map_t &dataDef)
	{
		userDefineDataType_map_t &m_dataTypeDef = dataDef;
		int index = 0;
		char name[128];
		const char *p = ndstr_first_valid(formatText);
		if (!p || !*p)	{
			return false;
		}
		if (0 == ndstricmp(p, "none") || 0 == ndstricmp(p, "null"))	{
			return false;
		}

		logic_print print_func = getLogFunction(pconn);
		void*log_file = getLogFile(pconn);

		print_func(log_file, "{");

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
				isArray = true;
			}
			else {
				p = ndstr_nstr_end(p, name, ',', sizeof(name));
			}
			if (!name[0]){
				break;
			}

			if (isArray)	{
				for (int i = 0; i < array_size; i++) {
					if (!outPutMessageFromconfig(pconn, name, inmsg, dataDef)) {
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

					if (0 == ndstricmp(memberName, "number") || 0 == ndstricmp(memberName, "count")){
						if (dataFormat.GetDataType() == OT_INT16)	{
							array_size = dataFormat.GetInt();
						}
					}
					else {

						print_func(log_file, "%s=", pmember?pmember:typeName);
						dataFormat.Print(print_func, log_file);
						print_func(log_file, ", ");
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
		print_func(log_file, "}\n");

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
		while (*inname && size-- > 0)	{
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

	int apollo_dft_message_handler(NDIConn* pconn, nd_usermsgbuf_t *msg)
	{
		NDIStreamMsg inmsg(msg);
		NDUINT16 msgID = ND_MAKE_WORD(inmsg.MsgMaxid(), inmsg.MsgMinid());

		LogicParserEngine  *parser = (LogicParserEngine  *)pconn->GetUserData();
		if (!parser){
			return 0;
		}
		ApoConnectScriptOwner * owner = (ApoConnectScriptOwner *)parser->getOwner();
		if (!owner)	{
			return 0;
		}
		const char *pName = owner->getMsgName(msgID);
		const char *body = owner->getMsgBody(msgID);

		logic_print outfunc = (logic_print)getLogFunction(pconn);

		userDefineDataType_map_t *pFormat = getDataFormat(pconn);
		if (!pFormat || pFormat->size() == 0 || !outfunc) {
			return msg_default_handler(pconn, msg);
		}


		if (!pName)	{
			return msg_default_handler(pconn, msg);
		}

		if (!body || 0 == ndstricmp(body, "none") || 0 == ndstricmp(body, "null")) {
			return msg_default_handler(pconn, msg);
		}

		DBLDataNode dataFormat;
		char namebuf[128];
		outfunc(NULL, "%s (%d,%d): ", convert_msg_name(pName, namebuf, 128), inmsg.MsgMaxid(), inmsg.MsgMinid());
		outPutMessageFromconfig(pconn, body, inmsg, *pFormat);

		outfunc(NULL, "\n");

		return 0;
	}

	//common error handler
	int msg_common_error_handler(NDIConn* pconn, nd_usermsgbuf_t *msg)
	{
		NDIStreamMsg inmsg(msg);
		NDUINT8 errtype = 0;
		NDUINT32 errParam = 0;
		inmsg.Read(errtype);
		inmsg.Read(errParam);
		nd_logerror("received error message %d %d\n", errtype, errParam);

		/*
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
		*/
		return 0;
	}
#endif 
	// 
	// int msg_logout_handler(NDIConn* pconn, nd_usermsgbuf_t *msg)
	// {
	// 	NDIStreamMsg inmsg(msg);
	// 	NDUINT32 errorCode = 0;
	// 
	// 	inmsg.Read(errorCode);
	// 
	// 	myfprintf(stdout, "Received logout Message errorcode = %d \n", errorCode);
	// 	return 0;
	// }

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

		nd_logmsg("Get Server Version ver-id=%d %s\n Version description=%s\n", ver_id, isDebug ? "debug" : "release", buf[0] == 0 ? "unknow" : (const char*)buf);

		return 0;
	}

	int msg_get_rlimit_handler(NDIConn* pconn, nd_usermsgbuf_t *msg)
	{
		NDIStreamMsg inmsg(msg);
		NDUINT8 buf[1024];
		buf[0] = 0;

		inmsg.Read(buf, sizeof(buf));

		nd_logmsg("server rlimit info :\n%s\n", buf[0] == 0 ? "unknow" : (const char*)buf);

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

		nd_logmsg("message (%d, %d)  name=%s \n", maxID, minID, buf);
		return 0;
	}


	int msg_show_server_time_handler(NDIConn* pconn, nd_usermsgbuf_t *msg)
	{
		NDIStreamMsg inmsg(msg);
		NDUINT64 tm1;
		if (0 == inmsg.Read(tm1)) {
			time_t tm2 = (time_t)tm1;

			char str[128];
			nd_logmsg("%s\n", nd_get_datetimestr_ex(tm2, str, 128));
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
			nd_logmsg("%s\n", nd_get_datetimestr_ex(tm2, str, 128));
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


	void InstallDftClientHandler(NDIConn *pconn)
	{
		CONNECT_INSTALL_MSG(pconn, msg_show_server_time_handler, ND_MAIN_ID_SYS, ND_MSG_SYS_TIME);
		CONNECT_INSTALL_MSG(pconn, msg_show_game_time_handler, ND_MAIN_ID_SYS, ND_MSG_SYS_GAME_TIME);
		//CONNECT_INSTALL_MSG(pconn, msg_logout_handler, NETMSG_MAX_LOGIN, LOGIN_MSG_LOGOUT_NTF);

		CONNECT_INSTALL_MSG(pconn, msg_show_msg_name_handler, ND_MAIN_ID_SYS, ND_MSG_SYS_GET_MESSAGE_NAME);
		CONNECT_INSTALL_MSG(pconn, msg_get_version_handler, ND_MAIN_ID_SYS, ND_MSG_SYS_GETVERSION);
		CONNECT_INSTALL_MSG(pconn, msg_get_rlimit_handler, ND_MAIN_ID_SYS, ND_MSG_SYS_GET_RLIMIT);


#ifdef WITHOUT_LOGIC_PARSER
		pconn->SetDftMsgHandler(msg_default_handler);
#else 

		CONNECT_INSTALL_MSG(pconn, get_message_protocol_build_time, ND_MAIN_ID_SYS, ND_MSG_SYS_GET_MESSAGE_BUILD_TIME);
		CONNECT_INSTALL_MSG(pconn, get_id_name_format_handler, ND_MAIN_ID_SYS, ND_MSG_SYS_GET_MESSAGE_FORMAT_LIST);
		CONNECT_INSTALL_MSG(pconn, get_data_format_handler, ND_MAIN_ID_SYS, ND_MSG_SYS_GET_USER_DEFINE_DATA);
#endif 

	}

};
