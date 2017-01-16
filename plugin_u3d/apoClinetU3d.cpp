/* file apoClientU3d.h
*
* wrapper of apollo-client , implemention of connect to server for unity-3d.
*
* create by duan
* 2016.8.18
*/

#include "apoClientU3d.h"
#include "logic_parser/dbldata2netstream.h"
#include "cli_common/dftCliMsgHandler.h"
//#include "netMessage/message_inc.h"
#include "logic_parser/script_event_id.h"

//#include "RoleDataManager.h"
//#include "NotificationKeyDefine.h"
//#include "BuildingInfoInheritsRef.h"

static void ndnetLog(const char *text)
{
	//cocos2d::log("%s", text);
}

static int apoPrintf(void *pf, const char *stm, ...)
{
	char buf[1024 * 4];
	char *p = buf;
	va_list arg;
	int done;

	va_start(arg, stm);
	done = vsnprintf(p, sizeof(buf), stm, arg);
	va_end(arg);

	return  fprintf((FILE*)pf, "%s", buf);
	//cocos2d::log("%s",buf);
	//return done;
}
//load ios file 
// 
// static bool apollo_load_file_data_cocos(LogicParserEngine*parser, parse_arg_list_t &args, DBLDataNode &result)
// {
// 	//CHECK_ARGS_NUM(args, 2, parser);
// 
// 	size_t size = 0;
// 	const char *fileName = args[1].GetText();
// 	if (!fileName || !*fileName) {
// 		nd_logmsg("input file name error\n");
// 		return false;
// 	}
// 	std::string mypath;
// 	
// 	if (nd_existfile(fileName)) {
// 		mypath = fileName;
// 	}
// 	else {
// 		mypath = cocos2d::FileUtils::getInstance()->fullPathForFilename(fileName);
// 		if (mypath.empty()) {
// 			DBLDataNode val1;
// 			if (parser->getOwner()->getOtherObject("WritablePath", val1)) {
// 				char pathbuf[ND_FILE_PATH_SIZE];
// 				mypath = val1.GetText();
// 				nd_full_path(mypath.c_str(), fileName, pathbuf, sizeof(pathbuf));
// 				mypath = pathbuf;
// 			}
// 		}
// 
// 		if (!nd_existfile(mypath.c_str())) {
// 			nd_logmsg("cant not found file %s\n", fileName);
// 			return false;
// 		}
// 	}
// 	
// 
// 	void *pdata = nd_load_file(mypath.c_str(), &size);
// 	if (!pdata) {
// 		nd_logmsg("file %s not open\n", fileName);
// 		return false;
// 	}
// 	result.InitSet(pdata, size, OT_BINARY_DATA);
// 	nd_unload_file(pdata);
// 	return true;
// }


static void myInitAccCreateInfo(account_base_info &acc, int accType, const char *userName, const char *passwd)
{
	acc.type = accType;
	acc.gender = 0; //default F
	acc.birth_day = 8;
	acc.birth_month = 8;
	acc.birth_year = 1988;
	strncpy((char*)acc.acc_name, userName, sizeof(acc.acc_name));
	strncpy((char*)acc.nick, userName, sizeof(acc.nick));
	strncpy((char*)acc.passwd, passwd, sizeof(acc.passwd));
	strncpy((char*)acc.phone, "100086", sizeof(acc.phone));
	strncpy((char*)acc.email, "webmaster@qq.com", sizeof(acc.email));

}


class ApoScriptOwner : public  ClientMsgHandler::ApoConnectScriptOwner
{
public:

	ApoScriptOwner() : ClientMsgHandler::ApoConnectScriptOwner(), m_netHandler(0)
	{

	}
	bool getOtherObject(const char*objName, DBLDataNode &val)
	{
		bool ret = ClientMsgHandler::ApoConnectScriptOwner::getOtherObject(objName, val);
		if (ret) {
			return ret;
		}

		else if (0 == ndstricmp(objName, "connector") || 0 == ndstricmp(objName, "session")){
			if (!m_netHandler) {
				return false;
			}
			val.InitSet((void*)m_netHandler, OT_OBJ_NDHANDLE);
			return true;
		}

		else if (0==ndstricmp(objName, "LogFunction")) {
			val.InitSet((void*)apoPrintf);
			return true;
		}
		else if (0==ndstricmp(objName, "LogFile")) {
			val.InitSet((void*)NULL);
			return true;
		}

		else if (0==ndstricmp(objName, "LogPath")) {
			//std::string mypath = cocos2d::FileUtils::getInstance()->getWritablePath();
			std::string mypath = ApoClient::getInstant()->getPath();
			val.InitSet(mypath.c_str());
			return true;
		}
		else if (0==ndstricmp(objName, "WritablePath")) {
			//std::string mypath = cocos2d::FileUtils::getInstance()->getWritablePath();
			std::string mypath = ApoClient::getInstant()->getPath();
			val.InitSet(mypath.c_str());
			return true;
		}
		return false;
	}

	const char *getMsgName(int msgId)
	{
		return NULL;
		//return NetMessage::getNameByID(msgId);
	}
	const char *getMsgBody(int msgId)
	{
		return NULL;
		//return NetMessage::getMessageBody(msgId);
	}
	void setConn(nd_handle pconn)
	{
		m_netHandler = pconn;
	}
private:
	nd_handle m_netHandler;
};
static ApoScriptOwner  __scriptObjOwner;


#define _SEND_AND_WAIT(_conn, _omsg, _rmsg_buf,_wait_maxid, _wait_minid,_sendflag) \
	if(0!=ndSendAndWaitMessage(_conn,_omsg.GetMsgAddr(),_rmsg_buf,_wait_maxid, _wait_minid,_sendflag,WAITMSG_TIMEOUT) ) {	\
		nd_logerror( "send and wait data error code =%d\n", nd_object_lasterror(_conn)); \
		return (RESULT_T)nd_object_lasterror(_conn);		\
			}
//////////////////////////////

#define CHECK_CONN_VALID( _conn ) 	\
if (!m_pconn) {						\
	return NDSYS_ERR_NOSOURCE;		\
}

ApoClient::ApoClient() :m_pconn(0), m_login(0), m_accId(0), m_runningUpdate(ERUN_UP_STOP), m_roleId(0)
{
	m_updateIndex = 0;
	time(&m_localTm);
	time(&m_serverTm);
}

ApoClient ::~ApoClient()
{
}

ApoClient *ApoClient::getInstant()
{
	return NDSingleton<ApoClient>::Get();
}
void ApoClient::destroyInstant()
{
	NDSingleton<ApoClient>::Destroy();
}


void ApoClient::setWorkingPath(const char *pathText)
{
	m_path = pathText;
}

void ApoClient::Destroy()
{
	//RoleDataManager::getInstance()->SaveLocalFile();

	if (m_login) {
		delete m_login;
		m_login = 0;
	}

	if (m_pconn && nd_connector_valid((nd_netui_handle)m_pconn) ) {
		ndClostConnect(m_pconn);
		//m_pconn->Close(0);
		//TryLogout(m_pconn->GetHandle());
	}

	nd_object_destroy(m_pconn, 0);
	m_pconn = 0 ;

	LogicEngineRoot::destroy_Instant();
	DBLDatabase::destroy_Instant();

	ndDeinitNet(); 
}


bool ApoClient::IsLoginOk()
{
	if (m_pconn && nd_connector_valid((nd_netui_handle)m_pconn)){
		return nd_connect_level_get(m_pconn) >= EPL_LOGIN;
	}
	return false;
}

time_t ApoClient::getServerTime()
{
	time_t now = time(NULL);
	return m_serverTm + (now - m_localTm);
}

void ApoClient::setServerTime(time_t srvTime)
{
	m_localTm = time(NULL);
	m_serverTm = srvTime;
}

// 
// bool ApoClient::_moveFileToWritable(const char *infileName, const char*outFileName)
// {
// 	cocos2d::FileUtils *fileUtilInst = cocos2d::FileUtils::getInstance();
// 	std::string mypath = fileUtilInst->fullPathForFilename(infileName);
// 
// 	cocos2d::Data data = fileUtilInst->getDataFromFile(mypath.c_str());
// 	if (!data.isNull()) {
// 
// 		std::string outpath = _getWritableFile(outFileName);
// 		FILE *pf = fopen(outpath.c_str(), "wb");
// 		if (pf)	{
// 			fwrite(data.getBytes(), 1, data.getSize(), pf);
// 			fclose(pf);
// 			return true;
// 		}
// 
// 		nd_logerror("open file  %s error %s\n", infileName, nd_last_error()) ;
// 	}
// 	else {
// 		nd_logerror("load file empty %s\n", infileName);
// 
// 	}
// 	return false;
// }
// std::string ApoClient::_getWritableFile(const char *file)
// {
// 	std::string outpath = cocos2d::FileUtils::getInstance()->getWritablePath();
// 	outpath += file;
// 	return outpath;
// }

int ApoClient::Init()
{
	//char logfilePath[ND_FILE_PATH_SIZE] ;
	
// 	//get write path
// 	m_path=cocos2d::FileUtils::getInstance()->getWritablePath();
// 	nd_full_path(m_path.c_str(),APO_LOG_FILE_NAME, logfilePath, sizeof(logfilePath)) ;
// 	nd_log_set_file( logfilePath ) ;
// 
// 	//move file to writable path 
// #if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
// 	_moveFileToWritable(APO_EXCEL_DATA_FILE_ORG, APO_EXCEL_DATA_FILE_TMP);
// 	_moveFileToWritable(APO_SCRIPT_FILE_ORG, APO_SCRIPT_FILE_TMP);
// #endif 
// 	
// 	
// 	cocos2d::FileUtils *fileUtilInst = cocos2d::FileUtils::getInstance();
// 	//load data
// 	std::string mypath = fileUtilInst->fullPathForFilename(APO_EXCEL_DATA_FILE);
// 	if (fileUtilInst->isFileExist(mypath.c_str())) {
// 		if (DBLDatabase::get_Instant()->LoadBinStream(mypath.c_str())) {
// 			cocos2d::log("load excel data error \n");
// 		}
// 	}
// 	else {		
// 		cocos2d::log("cehua_data file %s not exists. current working path is %s\n", mypath.c_str(), nd_getcwd());
// 	}

	//init message script handler
	LogicEngineRoot *scriptRoot = LogicEngineRoot::get_Instant();
	nd_assert(scriptRoot);
	
	LogicParserEngine  &parser = LogicEngineRoot::get_Instant()->getGlobalParser();
	parser.setOwner(&__scriptObjOwner);
	scriptRoot->getGlobalParser().setSimulate(false);
	
	scriptRoot->setPrint(apoPrintf, NULL);
// 	
// 	//load script 
// 	mypath = cocos2d::FileUtils::getInstance()->fullPathForFilename(APO_SCRIPT_FILE);
// 	if (0 != scriptRoot->LoadScript(mypath.c_str())){
// 		cocos2d::log("load script error \n");
// 	}
// 	else {
// 		//install script-export-c function
// 		scriptRoot->installFunc(apollo_load_file_data_cocos, "apollo_load_file_data", "读取整个文件(filename)");
// 	}

	ndInitNet();
	
	m_pconn = nd_object_create("tcp-connector");
	//m_pconn = CreateConnectorObj(NULL);
	//m_pconn->SetUserData(&parser) ;
	__scriptObjOwner.setConn(m_pconn);
	
	parser.eventNtf(APOLLO_EVENT_SERVER_START, 0); //program start up
	
	//onInit();

	return 0;
}

DBLDatabase *ApoClient::getExcelDatabase()
{
	return DBLDatabase::get_Instant();
}
LogicParserEngine *ApoClient::getScriptParser()
{
	return &(LogicEngineRoot::get_Instant()->getGlobalParser());
}
LogicEngineRoot *ApoClient::getScriptRoot()
{
	return LogicEngineRoot::get_Instant();
}



RESULT_T ApoClient::ReloginBackground(const char *host, int port, const char *dev_udid)
{
	CHECK_CONN_VALID(m_pconn) ;
	
	nd_connector_close(m_pconn, 1);
	//m_pconn->Close(1);
	RESULT_T res = Open(host, port,dev_udid);
	if (res != ESERVER_ERR_SUCCESS ){
		return res;
	}
	res = TrytoRelogin();
	if (res != ESERVER_ERR_SUCCESS) {
		nd_connector_close(m_pconn, 0);
		//m_pconn->Close(0);
	}
	return res;
}

RESULT_T ApoClient::Open(const char *host, int port, const char *dev_udid)
{
	//connect host
	
	CHECK_CONN_VALID(m_pconn) ;
	if (m_pconn) {
		nd_connector_set_crypt(m_pconn, NULL, 0);
	}
	
	m_host = host;
	m_port = port;
	m_udid = dev_udid ;
	
	return ESERVER_ERR_SUCCESS ;
	
}

RESULT_T ApoClient::_connectHost(const char *host, int port)
{
	
	CHECK_CONN_VALID(m_pconn) ;
	
	//if (-1 == m_pconn->Open(host, port, "tcp-connector", NULL)) {
	if (-1==nd_connector_open(m_pconn,host,port, NULL)){
		return (RESULT_T)NDSYS_ERR_HOST_UNAVAILABLE;
	}
	
	if (m_login) {
		delete m_login;
		m_login = 0;
	}
	
	if (m_path.empty()){
		m_login = new LoginApollo(m_pconn,  NULL, m_udid.c_str());
	}
	else {
		char sessionFile[ND_FILE_PATH_SIZE] ;
		nd_full_path(m_path.c_str(),APO_LOGIN_SESSION_FILE, sessionFile, sizeof(sessionFile)) ;
		
		m_login = new LoginApollo(m_pconn,  sessionFile, m_udid.c_str());
	}
	nd_assert(m_login);
	
	if (-1 == m_login->TrytoGetCryptKey()) {
		return (RESULT_T) NDERR_VERSION;
	}
	m_runningUpdate = ERUN_UP_NORMAL;
	onInit();
	return ESERVER_ERR_SUCCESS;
}

RESULT_T ApoClient::CreateAccount(const char *userName, const char *passwd, const char *phone, const char *email)
{
	// in login 
	if (IsLoginOk()){
		return NDSYS_ERR_ALREADY_LOGIN;
	}

	if (-1 == _trytoOpen()) {
		return NDSYS_ERR_HOST_UNAVAILABLE;
	}
	account_base_info acc;

	m_runningUpdate = ERUN_UP_STOP;
	myInitAccCreateInfo(acc, ACC_APOLLO, userName, passwd);
	strncpy((char*)acc.email, email, sizeof(acc.email));
	strncpy((char*)acc.phone, phone, sizeof(acc.phone));

	int ret = m_login->CreateAccount(&acc);
	if (-1 == ret) {
		m_runningUpdate = ERUN_UP_NORMAL;
		return (RESULT_T)m_login->GetLastError();
	}

	RESULT_T res = _enterGame(NULL, 0,userName);
	m_runningUpdate = ERUN_UP_NORMAL;
	return res;
}

RESULT_T ApoClient::TrytoRelogin()
{
	// in login 
	if (IsLoginOk()){
		return ESERVER_ERR_SUCCESS ;
	}
	
	if (-1 == _trytoOpen()) {
		return NDSYS_ERR_HOST_UNAVAILABLE;
	}

	//login server 

	m_runningUpdate = ERUN_UP_STOP;
	int ret = m_login->Relogin();

	if (0 == ret) {
		//login success 
		//return  ESERVER_ERR_SUCCESS;
		RESULT_T res = EnterGame();

		m_runningUpdate = ERUN_UP_NORMAL;
		return res;
	}
	return (RESULT_T)NDERR_LIMITED;
}
RESULT_T ApoClient::LoginAccount(const char *account, const char *passwd)
{
	// in login 
	if (IsLoginOk()){
		return NDSYS_ERR_ALREADY_LOGIN;
	}

	m_runningUpdate = ERUN_UP_STOP;
	if (-1 == _trytoOpen()) {
		return NDSYS_ERR_HOST_UNAVAILABLE;
	}
	int ret = m_login->Login(account, passwd, (ACCOUNT_TYPE)ACC_APOLLO);
	if (-1 == ret) {

		m_runningUpdate = ERUN_UP_NORMAL;
		return(RESULT_T)m_login->GetLastError();
	}

	RESULT_T res = _enterGame(NULL,0,account);
	m_runningUpdate = ERUN_UP_NORMAL;
	return res;
}


int ApoClient::_trytoOpen()
{
	if (m_pconn && nd_connector_valid((nd_netui_handle)m_pconn)) {
		return 0;
	}
	
	if (ESERVER_ERR_SUCCESS!=_connectHost(m_host.c_str(), m_port)) {
		return -1;
	}

	return	0;
}

int ApoClient::GetGameAreaList(ApolloServerInfo bufs[], size_t number)
{
	return m_login->GetServerList(bufs, (int) number);
}

RESULT_T ApoClient::_enterGame(const char *host, int port, const char *roleName)
{
	int logResult = 0;
	if (host && *host) {
		logResult = m_login->EnterServer(host, port);
	}
	else {
		ApolloServerInfo bufs[10];
		int num = m_login->GetServerList(bufs, ND_ELEMENTS_NUM(bufs));
		if (num == 0) {
			return NDSYS_ERR_HOST_UNAVAILABLE;
		}
		logResult = m_login->EnterServer(bufs[0].ip_addr, bufs[0].host.port);
	}
	
	if (logResult != 0) {
		return NDSYS_ERR_HOST_UNAVAILABLE;
	}

	//get role list
	NDOStreamMsg omsg(NETMSG_MAX_LOGIN,LOGIN_MSG_GET_ROLE_LIST_REQ);
	nd_handle h = m_pconn;
	nd_usermsgbuf_t recv_msg;

	_SEND_AND_WAIT(h, omsg, &recv_msg, NETMSG_MAX_LOGIN, LOGIN_MSG_GET_ROLE_LIST_ACK, 0)
	else {
		NDUINT32 roleid = 0;
		NDUINT32 error_code = 0;

		NDIStreamMsg inmsg(&recv_msg);
		NDUINT8 name[USER_NAME_SIZE];
		inmsg.Read(roleid);

		if (roleid == 0) {
			inmsg.Read(error_code);
			if (error_code) {
				nd_logerror("get role list error : %d\n", error_code);
				return (RESULT_T)error_code;
			}
			if (roleName && *roleName) {
				return createRole(roleName);
			}
		}
		else {
			m_roleId = roleid;
			inmsg.Read(name, sizeof(name));

			//read attribute
			NDUINT16 num = 0;
			if (0 == inmsg.Read(num)) {
				for (int i = 0; i < num; ++i) {
					NDUINT8 aid;
					float val;
					inmsg.Read(aid); inmsg.Read(val);
					nd_logmsg("load role attribute id = %d val =%f \n", aid, val);
				}
			}
			onLogin();
		}
	}
	
	return ESERVER_ERR_SUCCESS;
}

RESULT_T ApoClient::createRole(const char *roleName)
{
	NDUINT32 error_code = NDSYS_ERR_UNKNOWN;
	NDOStreamMsg omsg(NETMSG_MAX_LOGIN, LOGIN_MSG_CREATE_ROLE_REQ);
	omsg.Write((NDUINT8*)roleName);

	omsg.Write((NDUINT16)1);	//role attribute
	omsg.Write((NDUINT8)20);
	omsg.Write((float)0);

	nd_usermsgbuf_t recv_msg;


	_SEND_AND_WAIT(m_pconn, omsg, &recv_msg, NETMSG_MAX_LOGIN, LOGIN_MSG_CREATE_ROLE_ACK, 0)
	else {
		NDUINT32 roleid = 0;
		NDUINT32 error_code = 0;
		NDUINT8 name[USER_NAME_SIZE];

		NDIStreamMsg inmsg(&recv_msg);
		inmsg.Read(roleid);

		if (roleid == 0) {
			inmsg.Read(error_code);
			return (RESULT_T)error_code;
		}
		else {
			m_roleId = roleid;
			inmsg.Read(name, sizeof(name));
			//char roleName[128];
			//nd_utf8_to_gbk((const char*)name, roleName, sizeof(roleName));
			//nd_logerror("create role %s success \n", roleName);
			//read attribute
			NDUINT16 num = 0;
			if (0 == inmsg.Read(num)) {
				for (int i = 0; i < num; ++i) {
					NDUINT8 aid;
					float val;
					inmsg.Read(aid); inmsg.Read(val);
					nd_logmsg("create role attribute id = %d val =%f \n", aid, val);
				}
			}
			onLogin();
		}
	}
	return ESERVER_ERR_SUCCESS ;
}


RESULT_T ApoClient::testOneKeyLogin(const char *host, int port, const char *user, const char *passwd)
{
	RESULT_T res ;
	res = Open(host, port, "unknow-device") ;
	
	if(res) {
		return res ;
	}
	
	res = LoginAccount(user, passwd) ;
	if(res == ESERVER_ERR_SUCCESS) {
		return res ;
	}
	else if(res == NDSYS_ERR_NOUSER) {
		return CreateAccount(user, passwd, "10001", "web@qq.com") ;
	}
	
	return res ;
}

void ApoClient::Logout()
{
	m_login->Logout();
	//m_pconn->Close(0);
	nd_connector_close(m_pconn, 0);

	LogicParserEngine  &parser = LogicEngineRoot::get_Instant()->getGlobalParser();
	parser.eventNtf(APOLLO_EVENT_LOGOUT, 0); //program start up
}


void ApoClient::ClearLoginHistory()
{
	//std::string mypath = m_path + "/";
	//mypath += APO_LOGIN_SESSION_FILE;
	
	char sessionFile[ND_FILE_PATH_SIZE] ;
	nd_full_path(m_path.c_str(),APO_LOGIN_SESSION_FILE, sessionFile, sizeof(sessionFile)) ;
	
	nd_rmfile(sessionFile) ;
}


void ApoClient::EnterBackground()
{
	//RoleDataManager::getInstance()->SaveLocalFile();
}

void ApoClient::LeaveBackground()
{

}

bool ApoClient::Update()
{
	if (ERUN_UP_NORMAL != m_runningUpdate)	{
		return true;
	}
	if (m_pconn && nd_connector_valid((nd_netui_handle)m_pconn)){
		++m_updateIndex;
		int ret =ndUpdateConnect((netObject)m_pconn,0) ;
		if (-1 == ret) {
			//return false;
			int err =nd_object_lasterror(m_pconn) ;
			
			m_login->Logout() ;
			//m_pconn->Close() ;

			nd_connector_close(m_pconn, 0);

// 			ErrorDesignDataNotify* notifyInfo = ErrorDesignDataNotify::create();
// 			notifyInfo->setDesignID(err);
// 			
// 			cocos2d::NotificationCenter::getInstance()->postNotification(RELOGIN_NTF, notifyInfo);
		}
		if ((m_updateIndex & 255) == 255){
			//m_pconn->Send(ND_MAIN_ID_SYS, ND_MSG_SYS_GAME_TIME, NULL, 0);
			ndSend(m_pconn, ND_MAIN_ID_SYS, ND_MSG_SYS_GAME_TIME, NULL, 0);
		}
	}

	return true;
}


//////////////////////////////////////////////////////////////////////////


//#include "MessageHandler/msgHandler.h"

//install message handler
void ApoClient::onInit()
{
	//ClientMsgHandler::InstallDftClientHandler(m_pconn);
	//NetMessageHandler::MessageHandlerInit(m_pconn);	
	
	//m_pconn->SetDftMsgHandler(ClientMsgHandler::apollo_dft_message_handler);
	
	//LogicParserEngine  &parser = LogicEngineRoot::get_Instant()->getGlobalParser();
	//nd_message_set_script_engine(m_pconn->GetHandle(), (void*)&parser, ClientMsgHandler::apollo_cli_msg_script_entry);
	
}

void ApoClient::onLogin()
{
	//get message format
	__scriptObjOwner.LoadMsgDataTypeFromServer() ;//get program define data
	ndSend(m_pconn, ND_MAIN_ID_SYS, ND_MSG_SYS_GAME_TIME, NULL, 0);

	
	//RoleDataManager::getInstance()->LoadData(); //load role data from local-file or remote-host

	//LogicParserEngine  &parser = LogicEngineRoot::get_Instant()->getGlobalParser();
	//parser.eventNtf(APOLLO_EVENT_LOGIN, 0); //program start up
}

///////////////////////////////
//read excel test and demo
// int readExcelLine()
// {
// 
// 	const char *pfields[] = { "times", "type", "attr_award", "attr_rate", "item_award", "item_rate", "awarded", "awarded_rate" };
// 	DBLTable * pTable = DBL_FindTable("lottery_items.xlsx");
// 	nd_assert(pTable);
// 
// 	//read list
// 	DBLCursor cursor(pTable, pfields, ND_ELEMENTS_NUM(pfields), E_SRC_CODE_ANSI);
// 	if (-1 == cursor.Fetch(100)) {
// 		nd_logerror("game data config error\n");
// 		return ESERVER_ERR_GAME_CONFIG;
// 	}
// 	if (cursor[1].CheckValid() ) {
// 		//do some..
// 		int a = cursor[1].GetInt();
// 	}
// 
// 	if (cursor[2].CheckValid()) {
// 		//do some...
// 	}
// 	return 0;
// }
// 
// int listExcel()
// {
// 	const char *pfields[] = { "name", "update_time" };
// 
// 	DBL_BEGIN_CURSOR("daily_renew.xlsx", pfields) 	{
// 		if (cursor[0].CheckValid() && cursor[1].CheckValid()) {
// 			const char *name = cursor[0].GetText();
// 		}
// 	}
// 
// 	//list all 
// 	DBLTable * pListTable = DBL_FindTable("daily_renew.xlsx");
// 	if (pListTable)	{
// 		DBLCursor cursor(pTable);
// 		for (int result = cursor.FetchFirst(); result != -1; result = cursor.FetchNext()) {
// 			DBLDataNode data = cursor.GetDataNode("name");
// 			if (data.CheckValid()) 	{
// 			}
// 		}
// 	}
// 
// 
// 	
// 	return 0;
// }
// int searchCell()
// {
// 
// 	DBLDataNode node = DBL_FindDataObject("item.xlsx", 100, "sale_price");
// 	if (!node.CheckValid()){
// 		int price = node.GetInt();
// 	}
// 	return 0;
// }

