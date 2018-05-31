/* file apoClientU3d.h
*
* wrapper of apollo-client , implemention of connect to server for unity-3d.
*
* create by duan
* 2016.8.18
*/

#include "apoClientU3d.h"
#include "cli_common/dftCliMsgHandler.h"
//#include "netMessage/message_inc.h"
#include "script_event_id.h"

#ifndef WITHOUT_LOGIC_PARSER
#include "logic_parser/dbldata2netstream.h"

class ApoScriptOwner : public  ClientMsgHandler::ApoConnectScriptOwner
{
public:

	ApoScriptOwner() : ClientMsgHandler::ApoConnectScriptOwner()
	{

	}
	bool getOtherObject(const char*objName, DBLDataNode &val)
	{
		bool ret = ClientMsgHandler::ApoConnectScriptOwner::getOtherObject(objName, val);
		if (ret) {
			return ret;
		}

// 		else if (0 == ndstricmp(objName, "connector") || 0 == ndstricmp(objName, "session")){
// 			if (!m_netHandler) {
// 				return false;
// 			}
// 			val.InitSet((void*)m_netHandler, OT_OBJ_NDHANDLE);
// 			return true;
// 		}

// 		else if (0==ndstricmp(objName, "LogFunction")) {
// 			val.InitSet((void*)apoPrintf);
// 			return true;
// 		}
// 		else if (0==ndstricmp(objName, "LogFile")) {
// 			val.InitSet((void*)NULL);
// 			return true;
// 		}

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

		else if (0 == ndstricmp(objName, "DataPath")) {
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

private:
	//nd_handle m_netHandler;
};
static ApoScriptOwner  __scriptObjOwner;

static NDIConn*  _create_connector_object()
{
	NDIConn* p =CreateConnectorObj("tcp-connector");
	__scriptObjOwner.setConn(p);

	LogicParserEngine  &parser = LogicEngineRoot::get_Instant()->getGlobalParser();
	nd_message_set_script_engine(p->GetHandle(), (void*)&parser, apollo_message_script_entry);
	
	ClientMsgHandler::InstallDftClientHandler(p);
	return p;
}
#else 

static NDIConn* _create_connector_object()
{
// 	nd_handle pconn = nd_object_create("tcp-connector");
// 	if (-1 == nd_msgtable_create(pconn, ND_MAIN_MSG_CAPACITY, ND_MSG_BASE_ID)) {
// 		nd_object_destroy(pconn, 0);
// 		return NULL;
// 	}
// 	return pconn;
	return CreateConnectorObj("tcp-connector");
}
#endif

#define _SEND_AND_WAIT(_conn, _omsg, _rmsg_buf,_wait_maxid, _wait_minid,_sendflag) \
	if(0!=ndSendAndWaitMessage(_conn,_omsg.GetMsgAddr(),_rmsg_buf,_wait_maxid, _wait_minid,_sendflag,WAITMSG_TIMEOUT) ) {	\
		nd_logerror( "send and wait data error code =%d\n", nd_object_lasterror(_conn)); \
		return (RESULT_T)nd_object_lasterror(_conn);		\
			}
//////////////////////////////

#define CHECK_CONN_VALID( _conn ) 	\
if (!_conn) {	\
	return NDSYS_ERR_INVALID_HANDLE;		\
}

ApoClient::ApoClient() :m_pconn(0), m_login(0), m_accId(0), m_runningUpdate(ERUN_UP_STOP), m_roleId(0)
{
	m_isRelogin = 0;
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


void ApoClient::setLogPath(const char *logPath)
{
	m_logPath = logPath;
}

void ApoClient::Destroy(int)
{
	if (m_login) {
		delete m_login;
		m_login = 0;
	}

	Close();

#ifndef WITHOUT_LOGIC_PARSER
	LogicEngineRoot::destroy_Instant();
	DBLDatabase::destroy_Instant();
#endif
	ndDeinitNet(); 
}


bool ApoClient::IsLoginOk()
{
// 	if (m_pconn && nd_connector_valid((nd_netui_handle)m_pconn)){
// 		return nd_connect_level_get(m_pconn) >= EPL_LOGIN;
// 	}
	if (m_pconn && m_pconn->CheckValid()){
		return nd_connect_level_get(m_pconn->GetHandle()) >= EPL_LOGIN;
	}
	return false;
}

bool ApoClient::CheckValid()
{
	if (m_pconn && m_pconn->CheckValid()){
		return true;
	}
	return false;
}

bool ApoClient::IsInConnect()
{
	if (m_pconn && m_pconn->CheckValid()){
		return nd_connect_level_get(m_pconn->GetHandle()) >= EPL_CONNECT;
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

void ApoClient::trytoHandle(nd_usermsgbuf_t *msg)
{
	if (m_pconn->TestMsgIsHandle(ND_USERMSG_MAXID(msg), ND_USERMSG_MINID(msg)))	{
		m_pconn->CallMsgHandle(msg);
	}
}

int ApoClient::Create(const char*)
{
	
#ifndef WITHOUT_LOGIC_PARSER
	//init message script handler
	LogicEngineRoot *scriptRoot = LogicEngineRoot::get_Instant();
	nd_assert(scriptRoot);
	
	LogicParserEngine  &parser = LogicEngineRoot::get_Instant()->getGlobalParser();
	parser.setOwner(&__scriptObjOwner);
	scriptRoot->getGlobalParser().setSimulate(false);	
	//scriptRoot->setPrint(NULL, NULL);	

	ndInitNet();	
	//m_pconn = nd_object_create("tcp-connector");
	m_pconn = _create_connector_object() ;
	parser.eventNtf(APOLLO_EVENT_SERVER_START, 0); //program start up
#else 
	ndInitNet();
#endif 
	
	return 0;
}

#ifndef WITHOUT_LOGIC_PARSER
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
#endif 


nd_handle ApoClient::getConn()
{
	if (m_pconn){
		return m_pconn->GetHandle();
	}
	return NULL;
}


void ApoClient::SetEnableStreamRecord(bool bEnable)
{ 
	m_bEnableStreamRecord = bEnable; 
	if (m_bEnableStreamRecord)	{
		openSendStreamLog();
	}
}

RESULT_T ApoClient::ReloginBackground()
{
	CHECK_CONN_VALID(m_pconn) ;
	
	m_isRelogin = 1;
	RESULT_T res = TrytoRelogin();
	if (res != ESERVER_ERR_SUCCESS) {
		m_pconn->Close(0);
	}

	m_isRelogin = 0;
	return res;
}

RESULT_T ApoClient::ReloginEx(void *session, size_t sessionSize, bool bReloginOffline)
{
	CHECK_CONN_VALID(m_pconn);

	m_isRelogin = bReloginOffline?1:0;
	
	RESULT_T res = TrytoReloginEx(session, sessionSize);
	if (res != ESERVER_ERR_SUCCESS) {
		m_pconn->Close(0);
	}
	m_isRelogin = 0;
	return res;
}

RESULT_T ApoClient::Open(const char *host, int port)
{
	//connect host
	Close();
	m_pconn = _create_connector_object();
	
	if (!m_pconn) {
		return NDSYS_ERR_NOSOURCE;
	}
	
	if (host && *host)	{
		m_host = host;
		m_port = port;
	}
	
	return ESERVER_ERR_SUCCESS ;
	
}

void ApoClient::Close()
{
	if (m_pconn)	{
		nd_logmsg(" apoClient::close() net colsed error = %d\n", m_pconn->LastError());
		m_pconn->Close(0);
	}
	DestroyConnectorObj(m_pconn);
	m_pconn = 0;
}

void ApoClient::ResetConnect()
{
	if (m_pconn) {
		m_pconn->Close(1);
		nd_connector_set_crypt(m_pconn->GetHandle(), NULL, 0);
	}

}

RESULT_T ApoClient::_connectHost(const char *host, int port)
{
	if (!host || !*host){
		nd_logerror("connect to host error, input host name is NULL\n");
		return NDSYS_ERR_INVALID_INPUT;
	}	
	CHECK_CONN_VALID(m_pconn) ;
	
	if (-1 == m_pconn->Open(host, port, "tcp-connector")){
		return (RESULT_T)NDSYS_ERR_HOST_UNAVAILABLE;
	}


	int level = ndGetTimeoutVal();
	int size = sizeof(int);
	m_pconn->ioctl(NDIOCTL_SET_TIMEOUT, &level, &size);
		
	if (m_login) {
		delete m_login;
		m_login = 0;
	}
	
	if (m_sessionFile.empty()){
		m_login = new LoginApollo(m_pconn->GetHandle(),  NULL);
	}
	else {
		m_login = new LoginApollo(m_pconn->GetHandle(), m_sessionFile.c_str());
	}
	nd_assert(m_login);
	
	if (-1 == m_login->TrytoGetCryptKey()) {
		return (RESULT_T) NDERR_VERSION;
	}
	m_runningUpdate = ERUN_UP_NORMAL;
	onInit();
	nd_logmsg("connect host:%s port:%d ok\n", host, port);
	return ESERVER_ERR_SUCCESS;
}


void ApoClient::_closeConnect()
{
	if (m_pconn) {
		nd_logmsg("net colsed error = %d\n", m_pconn->LastError());
		m_pconn->Close(0);
	}
}

RESULT_T ApoClient::CreateAccount(const char *userName, const char *passwd,int channel)
{
	RESULT_T res = CreateOnly(userName, passwd,channel);
	if (res != ESERVER_ERR_SUCCESS)	{
		return res;
	}
	m_runningUpdate = ERUN_UP_STOP;
	res = _enterGame(NULL, 0, userName);
	m_runningUpdate = ERUN_UP_NORMAL;
	return res;
}
RESULT_T ApoClient::CreateOnly(const char *userName, const char *passwd, int channel)
{
	m_isRelogin = 0;
	if (!userName || !*userName){
		nd_logerror("create account error, input account name is NULL\n");
		return NDSYS_ERR_INVALID_INPUT;
	}

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

	acc.channel = channel;

	int ret = m_login->CreateAccount(&acc);

	m_runningUpdate = ERUN_UP_NORMAL;

	if (-1 == ret) {
		return (RESULT_T)m_login->GetLastError();
	}
	return ESERVER_ERR_SUCCESS;
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
	if (!m_login)	{
		nd_logerror("can not login the program is not init-success or connector-to-server\n");
		return NDSYS_ERR_NOT_INIT;
	}

	//login server 
	m_runningUpdate = ERUN_UP_STOP;
	int ret = m_login->Relogin();

	if (0 == ret) {
		//login success 
		RESULT_T res = EnterGame();
		m_runningUpdate = ERUN_UP_NORMAL;
		return res;
	}
	return (RESULT_T)NDERR_LIMITED;
}

RESULT_T ApoClient::TrytoReloginEx(void *session, size_t sessionSize)
{
	// in login 
	if (IsLoginOk()){
		nd_logerror("already in login\n");
		return ESERVER_ERR_SUCCESS;
	}

	if (-1 == _trytoOpen()) {
		return NDSYS_ERR_HOST_UNAVAILABLE;
	}

	//login server 
	if (!m_login)	{
		nd_logerror("can not login the program is not init-success or connector-to-server\n");
		return NDSYS_ERR_NOT_INIT;
	}

	m_runningUpdate = ERUN_UP_STOP;
	int ret = m_login->ReloginEx(session,sessionSize);
	if (0 == ret) {
		RESULT_T res = EnterGame();
		m_runningUpdate = ERUN_UP_NORMAL;
		return res;
	}
	return (RESULT_T)NDERR_LIMITED;
}

RESULT_T ApoClient::LoginAccountOneKey(const char *account, const char *passwd, int accType,int channel, bool skipAuth)
{
	RESULT_T res = LoginOnly(account, passwd, accType,channel, skipAuth);
	if (res != ESERVER_ERR_SUCCESS) {
		return res;
	}

	m_runningUpdate = ERUN_UP_STOP;

	res = _enterGame(NULL,0,account,true);
	m_runningUpdate = ERUN_UP_NORMAL;
	return res;
}

RESULT_T ApoClient::LoginOnly(const char *account, const char *passwd, int accType,int channel, bool skipAuth)
{
	m_isRelogin = 0;
	int reTrytimes = 3;
	// in login 
	if (IsLoginOk()){
		return NDSYS_ERR_ALREADY_LOGIN;
	}

	m_runningUpdate = ERUN_UP_STOP;

RE_LOGIN:
	if (-1 == _trytoOpen()) {
		return NDSYS_ERR_HOST_UNAVAILABLE;
	}
	int ret = m_login->Login(account, passwd, (ACCOUNT_TYPE)accType,channel, skipAuth);
	if (-1 == ret) {
		int errCode = m_login->GetLastError();
		if (errCode == NDERR_CLOSED || errCode == NDERR_RESET) {
			if (--reTrytimes > 0){
				ResetConnect();
				goto RE_LOGIN;
			}
		}

		m_runningUpdate = ERUN_UP_NORMAL;
		return (RESULT_T)errCode;
	}

	m_runningUpdate = ERUN_UP_NORMAL;
	return ESERVER_ERR_SUCCESS;
}

int ApoClient::_trytoOpen()
{
	if (m_host.empty() || m_port == 0){
		nd_logerror("connect host error adress is NULL\n");
		return -1;
	}

	if (!m_pconn){
		if (ESERVER_ERR_SUCCESS != Open()) {
			return -1;
		}
	}
	if (m_pconn->CheckValid()) {
		if (!m_login){
			m_pconn->Close(1);
			RESULT_T res = _connectHost(m_host.c_str(), m_port);
			if (res == ESERVER_ERR_SUCCESS)	{
				return 0;
			}
			m_pconn->SetLastError(res);
			return -1;
		}
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

RESULT_T ApoClient::_enterGame(const char *host, int port, const char *roleName,bool bWithoutLoadBalance)
{
	RESULT_T ret = SelectServer(host, port);
	if (ret!=ESERVER_ERR_SUCCESS)	{
		return ret;
	}
	role_base_info roleBaseInfo;
	int num = m_login->GetRoleList(&roleBaseInfo,1);
	if (-1==num){
		return (RESULT_T)m_login->GetLastError();
	}
	if (num ==0){
		if (-1 == m_login->CreateRole(roleName, roleBaseInfo)) {
			return (RESULT_T)m_login->GetLastError();
		}
	}
	m_roleId = roleBaseInfo.rid;
	onLogin();
	return ESERVER_ERR_SUCCESS;
	
}

RESULT_T ApoClient::SelectServer(const char *host , int port )
{
	int logResult = 0;
	if (host && *host) {
		logResult = m_login->EnterServer(host, port);
	}
	else {
		int serverId = m_login->getServerId();

		ApolloServerInfo bufs[10];
		int num = m_login->GetServerList(bufs, ND_ELEMENTS_NUM(bufs));
		if (num == 0) {
			return NDSYS_ERR_HOST_UNAVAILABLE;
		}
		if (num == 1)		{
			host = (const char*)bufs[0].inet_ip;
			port = bufs[0].port;
		}
		else {
			if (serverId == 0) {
				int maxNum = 0x10000;
				for (int i = 0; i < num; i++){
					if (maxNum > bufs[i].cur_number) {
						maxNum = bufs[i].cur_number;
						host = (const char*)bufs[i].inet_ip;
						port = bufs[i].port;
					}
				}
			}
			else {
				for (int i = 0; i < num; i++){
					if (bufs[i].logic_group_id == serverId) {
						host = (const char*)bufs[i].inet_ip;
						port = bufs[i].port;
						break;
					}
				}
			}
		}
		if (!host || !host[0]){
			return NDSYS_ERR_CANNOT_ENTER_SERVER;
		}
		logResult = m_login->EnterServer(host, port, true);

	}

	if (logResult != 0) {
		return (RESULT_T)m_login->GetLastError();
	}
	return	ESERVER_ERR_SUCCESS;

}


void ApoClient::Logout()
{
	if (m_login && m_pconn)	{
		m_login->Logout();
	}

	if (m_pconn) {
		m_pconn->Close(0);
	}

#ifndef WITHOUT_LOGIC_PARSER
	LogicParserEngine  &parser = LogicEngineRoot::get_Instant()->getGlobalParser();
	parser.eventNtf(APOLLO_EVENT_LOGOUT, 0); //program start up
#endif
}


void ApoClient::ClearLoginHistory()
{
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
	if (m_pconn && m_pconn->CheckValid() ){
		++m_updateIndex;
		int ret = m_pconn->Update(0);
		if (-1 == ret) {
			m_login->Logout() ;
			m_pconn->Close() ;
		}
		if ((m_updateIndex & 255) == 255){
			m_pconn->Send(ND_MAIN_ID_SYS, ND_MSG_SYS_GAME_TIME, NULL, 0);
		}
	}

	return true;
}


//////////////////////////////////////////////////////////////////////////


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
	if (m_bEnableStreamRecord) {
		openSendStreamLog();
	}
}


bool ApoClient::openSendStreamLog()
{
	const char *pFielName = "./apoGameNetStream.data";
	int length = (int)strlen(pFielName);
	if (m_pconn->ioctl(NDIOCTL_LOG_SEND_STRAM_FILE, (void*)pFielName, &length) == -1){
		nd_logmsg("log net message bin-data errror\n");
		return false;
	}
	else {
		//const char *p = nd_getcwd();
		//nd_logmsg("out put net stream file %s/apoGameNetStream.data\n", p);
		return true;
	}
	
}


