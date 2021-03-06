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
#include "game_parser/dbl_mgr.h"
#include "game_parser/dbldata2netstream.h"
#include "logic_parser/logicEngineRoot.h"
#include "logic_parser/logic_compile.h"
#endif 

#include "dftCliMsgHandler.h"

namespace ClientMsgHandler
{

	int msg_default_handler(NDIConn* pconn, nd_usermsgbuf_t *msg)
	{
		//int reLine = 0;
		//nd_logmsg("recv (%d,%d) message data-len =%d\n", ND_USERMSG_MAXID(msg), ND_USERMSG_MINID(msg), ND_USERMSG_LEN(msg));
		char buf[0x10000];
		NDIStreamMsg inmsg(msg);
		NDIStreamMsg tmpInm(inmsg.GetMsgAddr());
		int size = tmpInm.dumpText(buf, sizeof(buf));
		if (size > 0)	{
			buf[size] = 0;
			nd_logmsg("%s\n", buf);
		}
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

	LogicParserEngine *ApoConnectScriptOwner::getScriptHandler()
	{
		return &LogicEngineRoot::get_Instant()->getGlobalParser(); 
	}

	void ApoConnectScriptOwner::Destroy(int)
	{
		m_conn = 0;
		//m_dataType.clear();
		m_msgIdName.clear();
	}
	void ApoConnectScriptOwner::setConn(NDIConn *conn)
	{
		m_conn = conn;
	}

	bool ApoConnectScriptOwner::getOtherObject(const char*objName, LogicDataObj &val)
	{
		if (0==ndstricmp(objName, "connector") || 0 == ndstricmp(objName, "session")){
			if (!m_conn) {
				return false;
			}
			nd_handle h = m_conn->GetHandle();
			val.InitSet((void*)h, OT_OBJ_NDHANDLE);
		}
		else if (0 == ndstricmp(objName, "msgIdNameFormat")) {
			val.InitSet((void*)&m_msgIdName);
		}
		else if (0 == ndstricmp(objName, "SelfName")) {
			val.InitSet("client");
		}
		else {
			return apoLogicObject4Game::getOtherObject(objName, val);
		}
		return true;
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



// 	bool ApoConnectScriptOwner::loadDataType(const char *file)
// 	{
// 		if (!InitLoadNetMsgFormat(file)){
// 			return false;
// 		}
// 		return true;
// 	}

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
		LogicDataObj val;
		_GET_OBJ_FROM_MGR(pconn, "FormatMsgData", val);
		return (userDefineDataType_map_t *)val.GetObjectAddr();
	}
	logic_print getLogFunction(NDIConn *pconn)
	{
		LogicDataObj val;

		_GET_OBJ_FROM_MGR(pconn, "LogFunction", val);
		return (logic_print)val.GetObjectAddr();
	}

	std::string getWritablePath(NDIConn *pconn)
	{
		LogicDataObj val;
		_GET_OBJ_FROM_MGR(pconn, "WritablePath", val);
		return val.GetString();
	}
	std::string getLogPath(NDIConn *pconn)
	{
		LogicDataObj val;

		_GET_OBJ_FROM_MGR(pconn, "LogPath", val);
		return val.GetString();
	}

	std::string getDataPath(NDIConn *pconn)
	{
		LogicDataObj val;

		_GET_OBJ_FROM_MGR(pconn, "DataPath", val);
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

	ApoConnectScriptOwner *getScriptOwner(NDIConn *pconn) 
	{
		LogicDataObj val;
		_GET_OBJ_FROM_MGR(pconn, "LogPath", val);
		return (ApoConnectScriptOwner *)val.GetObjectAddr();
	}

	bool handleMsgFirstFromFileBeforTime(NDIConn *pconn, const char *file, NDUINT64 genTime, NDUINT16 msgId, nd_conn_msg_entry func)
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
		LogicDataObj val;

		_GET_OBJ_FROM_MGR(pconn, "LogFile", val);
		return val.GetObjectAddr();

	}

	msgIdNameFormat_vct* getMsgIdNameFormat(NDIConn *pconn)
	{
		LogicDataObj val;

		_GET_OBJ_FROM_MGR(pconn, "msgIdNameFormat", val);
		return (msgIdNameFormat_vct*)val.GetObjectAddr();

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
		handleMsgFirstFromFileBeforTime(pconn, APOLLO_MSG_ID_NAME_FORMAT, newVersionTime, msgId, (nd_conn_msg_entry)get_id_name_format_handler);

		msgId = ND_MAKE_WORD(ND_MAIN_ID_SYS, ND_MSG_SYS_GET_USER_DEFINE_DATA);
		handleMsgFirstFromFileBeforTime(pconn, APOLLO_DATA_FORMAT, newVersionTime, msgId, (nd_conn_msg_entry)get_data_format_handler);

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


	bool outPutMessageFromconfig(NDIConn* pconn, const char *formatText, NDIStreamMsg &inmsg, userDefineDataType_map_t &dataDef)
	{
		logic_print print_func = getLogFunction(pconn);
		if(print_func) {
			inmsg.SetSkipMarker(true);
			void*log_file = getLogFile(pconn);
			return LogicOutputMsgByFormat(print_func, log_file, formatText, inmsg, dataDef);
		}
		return false;
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

		LogicDataObj dataFormat;
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
		return 0;
	}

	int msg_store_file(NDIConn* pconn, nd_usermsgbuf_t *msg)
	{
		NDIStreamMsg inmsg(msg);
		char filename[128];

		if (inmsg.Read((NDUINT8*)filename, sizeof(filename)) <= 0){
			return 0;
		}

		int len = inmsg.PeekBinSize();
		if (-1 == len || 0 == len || len > ND_USERMSG_DATA_CAPACITY){
			return 0;
		}
		len += 7;

		char *_data = (char *)malloc(len);
		if (!_data)	{
			return 0;
		}
		len = (int)(inmsg.ReadBin(_data, len));
		//write file 
		std::string wPath = getWritablePath(pconn);
		wPath += "/";
		wPath += filename;

		FILE *pf = fopen(wPath.c_str(), "wb");
		if (!pf) {
			return -1;
		}
		fwrite(_data, 1, len, pf);
		fclose(pf);

		return 0;
	}

	int msg_install_messageHandler(NDIConn* pconn, nd_usermsgbuf_t *msg)
	{
		NDIStreamMsg inmsg(msg);
		NDUINT16 maxId, minId;
		NDUINT8 byteOrder;
		NDUINT8 name[64];
		char buf[4096];

		name[0] = 0;
		if (0 != inmsg.Read(maxId))	{ return 0; }
		if (0 != inmsg.Read(minId))	{ return 0; }
		if (0 != inmsg.Read(byteOrder))	{ return 0; }

		if (inmsg.Read(name, sizeof(name)) == 0) { return 0; }
		size_t bodySize = inmsg.ReadBin(buf, sizeof(buf));
		if (bodySize){
			if (LogicEngineRoot::get_Instant()->addGlobalFunction(byteOrder, (char*)name, buf, bodySize)) {				
				if (0 == nd_msgentry_script_install(pconn->GetHandle(), (char*)name, (ndmsgid_t)maxId, (ndmsgid_t)minId, 0)) {
					//nd_logdebug("add function %s success for (%d, %d) success \n", name, maxId, minId);
				}
			}
		}
		return 0;
	}


	int msg_reload_file(NDIConn* pconn, nd_usermsgbuf_t *msg)
	{
		std::string myPath = getDataPath(pconn);
		if (myPath.empty())	{
			nd_logdebug("can not get data path\n");
			return 0;
		}
		NDIStreamMsg inmsg(msg);
		char buf[1024];

		if (inmsg.Read(buf, sizeof(buf)) > 0) {
			myPath += "/";
			myPath += buf;

			LogicParserEngine  &parser = LogicEngineRoot::get_Instant()->getGlobalParser();
			LogicEngineRoot::get_Instant()->LoadScript(myPath.c_str(), &parser);

		}
		return 0;
	}
#endif 
	
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

	int msg_show_echo_time(NDIConn* pconn, nd_usermsgbuf_t *msg)
	{
		NDIStreamMsg inmsg(msg);
		NDUINT32 now = nd_time();
		NDUINT32 sendtm = 0;
		if (inmsg.Read(sendtm) == 0){
			nd_logmsg("PING VALUE = %d (ms)\n", (now - sendtm) / 2);
		}
		else {
			nd_logmsg("not Ping message \n");
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

		CONNECT_INSTALL_MSG(pconn, msg_show_msg_name_handler, ND_MAIN_ID_SYS, ND_MSG_SYS_GET_MESSAGE_NAME);
		CONNECT_INSTALL_MSG(pconn, msg_get_version_handler, ND_MAIN_ID_SYS, ND_MSG_SYS_GETVERSION);
		
#ifdef WITHOUT_LOGIC_PARSER
		pconn->SetDftMsgHandler((nd_conn_msg_entry)msg_default_handler);
#else 
		CONNECT_INSTALL_MSG(pconn, get_message_protocol_build_time, ND_MAIN_ID_SYS, ND_MSG_SYS_GET_MESSAGE_BUILD_TIME);
		CONNECT_INSTALL_MSG(pconn, get_id_name_format_handler, ND_MAIN_ID_SYS, ND_MSG_SYS_GET_MESSAGE_FORMAT_LIST);
		CONNECT_INSTALL_MSG(pconn, get_data_format_handler, ND_MAIN_ID_SYS, ND_MSG_SYS_GET_USER_DEFINE_DATA);

		CONNECT_INSTALL_MSG(pconn, msg_store_file, ND_MAIN_ID_SYS, ND_MSG_SYS_REQ_OTHER_STORE_FILE);
		CONNECT_INSTALL_MSG(pconn, msg_reload_file, ND_MAIN_ID_SYS, ND_MSG_SYS_REQ_OTHER_RELOAD_SCRIPT);
		CONNECT_INSTALL_MSG(pconn, msg_install_messageHandler, ND_MAIN_ID_SYS, ND_MSG_SYS_REQ_OTHER_INSTALL_HANDLER);
		
#endif 

	}

};
