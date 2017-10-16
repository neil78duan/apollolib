// gmToolDlg.cpp : implementation file
//

#include "stdafx.h"
#include "apolloParseEditor.h"
#include "gmToolDlg.h"
#include "afxdialogex.h"
#include "InsertNodeListDlg.h"

#include "nd_common/nd_common.h"
#include "cli_common/netui_atl.h"
#include "cli_common/login_apollo.h"
#include "apollo_errors.h"
#include "logic_parser/logicDataType.h"
#include "logic_parser/dbl_mgr.h"
// gmToolDlg dialog
#include "logic_parser/dbldata2netstream.h"
#include "logic_parser/logicEngineRoot.h"
#include "cli_common/dftCliMsgHandler.h"
#include "logic_parser/script_event_id.h"


static gmToolDlg *__pMyDlg;
static void*  __oldFunc;
NDOStreamMsg __sendMsg;
//static userDefineDataType_map_t m_dataTypeDef;
//static ndxml_root m_message_define;

extern void gmtoolMsgHandlerInit(NDIConn *pconn);
//show error 

static void out_log(const char *text)
{
	if (__pMyDlg){
		__pMyDlg->LogText(text);
	}
}

static int out_print(void *pf, const char *stm, ...)
{
	char buf[1024 * 4];
	char *p = buf;
	va_list arg;
	int done;

	va_start(arg, stm);
	done = vsnprintf(p, sizeof(buf), stm, arg);
	va_end(arg);

	out_log(buf);
	return done;
}
/*
#include "netMessage/message_inc.h"
static int msgRoleDataInit(NDIConn* pconn, nd_usermsgbuf_t *msg)
{
	NetMessage::RoleInfo data;
	NetMessage::ReadStream(inmsg, data);
	RoleDataManager* roleMgr = RoleDataManager::getInstance();

	NDIStreamMsg inmsg(msg);

	nd_assert(roleMgr);

	if (roleMgr->Init(inmsg) != ESERVER_ERR_SUCCESS) {
		nd_logerror("error received role data message \n");
	}

	//roleMgr->SaveLocalFile();
	return 0;
}
*/
/////////////////////////////////////

class ConnectScriptOwner :public  ClientMsgHandler::ApoConnectScriptOwner
{
public:
	bool getOtherObject(const char*objName, DBLDataNode &val)
	{
		if (0 == ndstricmp(objName, "LogFunction")) {
			val.InitSet((void*)out_print);
			return true;
		}
		else if (0 == ndstricmp(objName, "LogFile")) {
			val.InitSet("ndlog.log");
			return true;
		}
		else if (0 == ndstricmp(objName, "LogPath")) {
			val.InitSet("../../log/");
			return true;
		}
		else if (0 == ndstricmp(objName, "WritablePath")) {
			val.InitSet("../../log/");
			return true;
		}
		else if (0 == ndstricmp(objName, "SelfName")) {
			val.InitSet("gmtool");
			return true;
		}
		else if (0 == ndstricmp(objName, "self")) {
			val.InitSet((void*)this, OT_OBJ_BASE_OBJ);
			return true;
		}

		bool ret = ClientMsgHandler::ApoConnectScriptOwner::getOtherObject(objName, val);
		if (ret) {
			return ret;
		}
		return false;
	}
};

static ConnectScriptOwner  __myScriptOwner;

void destroy_apollo_object(NDIConn *pConn)
{
	__myScriptOwner.Destroy();

	LogicEngineRoot::destroy_Instant();
}

static bool init_apollo_object(NDIConn *pConn, const char*script_file)
{
	destroy_apollo_object(pConn);

	__myScriptOwner.setConn( pConn);

	// init script 
	LogicEngineRoot *scriptRoot = LogicEngineRoot::get_Instant();
	nd_assert(scriptRoot);
	LogicParserEngine  &parser = LogicEngineRoot::get_Instant()->getGlobalParser();
	nd_message_set_script_engine(pConn->GetHandle(), (void*)&parser, ClientMsgHandler::apollo_cli_msg_script_entry);
	parser.setOwner(&__myScriptOwner);

	scriptRoot->setPrint(out_print, NULL);
	scriptRoot->getGlobalParser().setSimulate(false);
	pConn->SetUserData(&parser);

	if (0 != scriptRoot->LoadScript(script_file, &parser)){
		nd_logerror("加载脚本出错！\n");
		return false;
	}
	ClientMsgHandler::InstallDftClientHandler(pConn);
	pConn->SetDftMsgHandler(ClientMsgHandler::apollo_dft_message_handler);

	//ntf server start
	parser.eventNtf(APOLLO_EVENT_SERVER_START, 0);

	//pConn->InstallMsgFunc(get_data_format_handler, ND_MAIN_ID_SYS, ND_MSG_SYS_GET_USER_DEFINE_DATA);
	return true;
}
//////////////////////////////////////
#define _SESSION_FILE "gmtoolSession.ses"
//nd_setlog_func(out_log);
static char _s_session_buf[4096];
static size_t _s_session_size = 0;

#define _SEND_AND_WAIT(_conn, _omsg, _rmsg_buf,_wait_maxid, _wait_minid,_sendflag) \
	if(0!=ndSendAndWaitMessage(_conn,_omsg.GetMsgAddr(),_rmsg_buf,_wait_maxid, _wait_minid,_sendflag,WAITMSG_TIMEOUT) ) {	\
		nd_logerror( "send and wait data error code =%d\n", nd_object_lasterror(_conn)); \
		return -1;		\
	}


IMPLEMENT_DYNAMIC(gmToolDlg, CDialog)

gmToolDlg::gmToolDlg(CWnd* pParent /*=NULL*/)
	: CDialog(gmToolDlg::IDD, pParent)
	, m_nPort(6600)
	, m_maxId(0)
	, m_minId(0)
	, m_msgData(_T(""))
	, m_selDataType(0)
	, m_bSkipAuth(FALSE)
{
	m_newLine = true;
	m_pConn = NULL;
	m_login = 0;

	m_strHost = theApp.getDefaultSetting(_T("host"), _T("192.168.8.55"));
	m_strUserName = theApp.getDefaultSetting(_T("user"), _T("test1"));
	m_strPasswd = theApp.getDefaultSetting(_T("passwd"), _T("test123"));
	
	//ndxml_initroot(&m_message_define);
}

gmToolDlg::~gmToolDlg()
{

	ndSetLogoutFunc((void*)__oldFunc);
	__pMyDlg = NULL;
	destroy_apollo_object(m_pConn);
	

	if (m_pConn){
		DestroyConnectorObj(m_pConn);
	}
	if (m_login){
		delete m_login;
		m_login = NULL;
		//ApolloDestroyLoginInst(m_login);
	}
	//LogicEngineRoot::destroy_Instant();
	//ndxml_destroy(&m_message_define);

	DeinitNet();
	//destroyUserDefData(m_dataTypeDef);
	DBLDatabase::destroy_Instant();
}

void gmToolDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_HOST, m_strHost);
	DDX_Text(pDX, IDC_EDIT_PORT, m_nPort);
	DDV_MinMaxInt(pDX, m_nPort, 0, 65535);
	DDX_Text(pDX, IDC_EDIT_USER_NAME, m_strUserName);
	DDV_MaxChars(pDX, m_strUserName, 32);
	DDX_Text(pDX, IDC_EDIT_PASSWD, m_strPasswd);
	DDV_MaxChars(pDX, m_strPasswd, 32);
	DDX_Text(pDX, IDC_EDIT_MAX_ID, m_maxId);
	DDV_MinMaxInt(pDX, m_maxId, 0, 255);
	DDX_Text(pDX, IDC_EDIT_MIN_ID, m_minId);
	DDV_MinMaxInt(pDX, m_minId, 0, 255);
	DDX_Text(pDX, IDC_EDIT_MSG_DATA, m_msgData);
	DDX_Radio(pDX, IDC_RADIO1, m_selDataType);
	DDX_Check(pDX, IDC_CHECK2, m_bSkipAuth);
}


BEGIN_MESSAGE_MAP(gmToolDlg, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_LOGIN, &gmToolDlg::OnBnClickedButtonLogin)
	ON_BN_CLICKED(IDC_BUTTON_WRITE_DATA, &gmToolDlg::OnBnClickedButtonWriteData)
	ON_BN_CLICKED(IDC_BUTTON_SEND, &gmToolDlg::OnBnClickedButtonSend)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BUTTON_MSG_RESET, &gmToolDlg::OnBnClickedButtonMsgReset)
	ON_BN_CLICKED(IDC_BUTTON_SERVER_RELOAD, &gmToolDlg::OnBnClickedButtonServerReload)
	ON_BN_CLICKED(IDC_BUTTON_CLEAR, &gmToolDlg::OnBnClickedButtonClear)
	ON_BN_CLICKED(IDC_BT_GMMSG, &gmToolDlg::OnBnClickedBtGmmsg)
	ON_BN_CLICKED(IDC_BUTTON_SHOW_ERROR, &gmToolDlg::OnBnClickedButtonShowError)
	ON_BN_CLICKED(IDC_CHECK_SHOW_HEX, &gmToolDlg::OnBnClickedCheckShowHex)
//	ON_BN_CLICKED(IDC_CHECK2, &gmToolDlg::OnBnClickedCheck2)
END_MESSAGE_MAP()


BOOL gmToolDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	InitNet();
	__pMyDlg = this;
	__oldFunc = ndSetLogoutFunc(out_log);


	return TRUE;
}

// gmToolDlg message handlers


void gmToolDlg::OnBnClickedButtonLogin()
{
	CButton *pBt = (CButton *)GetDlgItem(IDC_BUTTON_LOGIN);
	if (!pBt)	{
		return;
	}

	CButton *pBTgm = (CButton *)GetDlgItem(IDC_BT_GMMSG);
	UpdateData(TRUE);

	if (m_pConn){

		KillTimer(1);
		KillTimer(2);
		pBt->SetWindowText(_T("登录"));
		if (m_pConn->CheckValid())	{
			m_pConn->Close();
		}
		DestroyConnectorObj(m_pConn);
		m_pConn = NULL;
		
		if (m_login){
			//ApolloDestroyLoginInst(m_login);
			delete m_login;
			m_login = 0;
		}

		clearLog();
		out_log("logout success\n");
		pBTgm->EnableWindow(false);
		
	}
	else{
		__sendMsg.Reset();
		clearLog();
		account_index_t aid = 0;
		char userName[128];
		nd_gbk_to_utf8((LPCTSTR)m_strUserName, userName, sizeof(userName));

		if (-1 == _connectHost((LPCTSTR)m_strHost, m_nPort)){
			AfxMessageBox("connect server error");
			return;
		}

		out_print("CONNECT %s:%d SUCCESS\n", (LPCTSTR)m_strHost, m_nPort);

		if (-1 == _login(userName, (LPCTSTR)m_strPasswd,(bool)m_bSkipAuth))	{
			AfxMessageBox("user login error");
			return;
		}
		out_print("ACCOUNT %s login SUCCESS\n", (LPCTSTR)m_strUserName);

		//打开消息日志输出
		/*
		int val = 1;
		int size = (int) sizeof(val);
		m_pConn->ioctl(NDIOCTL_LOG_SEND_MSG, &val, &size);

		val = 1;
		size = (int) sizeof(val);
		m_pConn->ioctl(NDIOCTL_LOG_RECV_MSG, &val, &size);
		*/


		nd_logmsg("登录成功账号ID=%d\n", aid);
		
		
		if (-1==SelOrCreateRole())	{
			AfxMessageBox("create or select role error");

			m_pConn->Close();
			DestroyConnectorObj(m_pConn);
			m_pConn = NULL;
			return;
		}
				
		
		SetTimer(1, 100, 0);
		SetTimer(2, 1000, 0);
		pBt->SetWindowText(_T("登出"));

		pBTgm->EnableWindow(TRUE);
		
		theApp.setDefaultSetting(_T("host"), (LPCTSTR)m_strHost);
		theApp.setDefaultSetting(_T("user"), (LPCTSTR)m_strUserName);
		theApp.setDefaultSetting(_T("passwd"), (LPCTSTR)m_strPasswd);

		__myScriptOwner.LoadMsgDataTypeFromServer();

		LogicEngineRoot *scriptRoot = LogicEngineRoot::get_Instant();
		if (scriptRoot ) {
			parse_arg_list_t arg;
			arg.push_back(DBLDataNode(m_pConn, OT_OBJ_NDOBJECT));
			scriptRoot->getGlobalParser().eventNtf(APOLLO_EVENT_LOGIN, arg);
		}
		//get and init role data
		//getRoleData();
	}
}


void gmToolDlg::OnBnClickedButtonWriteData()
{
	UpdateData(TRUE);
	if (m_selDataType==0){
		int a = ndstr_atoi_hex((LPCTSTR)m_msgData);
		__sendMsg.Write((NDUINT8)a);
	}
	else if (m_selDataType == 1){
		int a = ndstr_atoi_hex((LPCTSTR)m_msgData);
		__sendMsg.Write((NDUINT16)a);
	}
	else if (m_selDataType == 2){
		int a = ndstr_atoi_hex((LPCTSTR)m_msgData);
		__sendMsg.Write((NDUINT32)a);
	}
	else if (m_selDataType == 3){
		NDUINT64 a = ndstr_atoll_hex((LPCTSTR)m_msgData);
		__sendMsg.Write(a);
	}
	else if (m_selDataType == 4){
		float a = (float)atof((LPCTSTR)m_msgData);
		__sendMsg.Write(a);
	}
	else if (m_selDataType == 5){
		double a = atof((LPCTSTR)m_msgData);
		__sendMsg.Write(a);
	}
	else{
		__sendMsg.Write((NDUINT8*)(LPCTSTR)m_msgData);
	}

	nd_logmsg("WRITE MESSAGE buf  total size =%d\n", __sendMsg.GetDataLen());
}


void gmToolDlg::OnBnClickedButtonSend()
{
	if (!m_pConn){
		out_log("没有登录\n");
		return;
	}
	UpdateData(TRUE);
	__sendMsg.SetID(m_maxId, m_minId);
	int ret = m_pConn->SendMsg(__sendMsg);

	nd_logmsg("send message size =%d\n", ret);
}

int gmToolDlg::createRole(const char *roleName) 
{

	NDOStreamMsg omsg(NETMSG_MAX_LOGIN, LOGIN_MSG_CREATE_ROLE_REQ);
	omsg.Write((NDUINT8*)roleName);

	omsg.Write((NDUINT16)1);
	omsg.Write((NDUINT8)20);
	omsg.Write((float)0);

	nd_handle h = m_pConn->GetHandle();
	nd_usermsgbuf_t recv_msg;
	_SEND_AND_WAIT(h, omsg, &recv_msg, NETMSG_MAX_LOGIN, LOGIN_MSG_CREATE_ROLE_ACK, 0)
	else {
		NDUINT32 roleid = 0;
		NDUINT32 error_code = 0;
		NDUINT8 name[USER_NAME_SIZE];

		NDIStreamMsg inmsg(&recv_msg);
		inmsg.Read(roleid);

		if (roleid == 0) {
			inmsg.Read(error_code);
			if (error_code) {
				nd_logerror( "create role list error : %d\n", error_code);
			}
			return -1 ;
		}
		else {
			inmsg.Read(name, sizeof(name));

			char roleName[128];
			nd_utf8_to_gbk((const char*)name, roleName, sizeof(roleName));

			nd_logmsg("create role %s success \n", roleName);
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
		}
	}
	return 0 ;
}


bool gmToolDlg::LoadDataDef(const char *file, const char *script, const char *message_def)
{
	/*if (-1 == loadUserDefFromMsgCfg(file, m_dataTypeDef))	{
		nd_logerror("load message datatype error from %s\n", file);
		return false;
	}*/

	m_scriptFile = script ;
	/*if (message_def){
		ndxml_load_ex(message_def, &m_message_define, nd_get_encode_name(ND_ENCODE_TYPE));
	}*/

	//testFormatMsgRead(m_dataTypeDef);
	
	return true;
}
int gmToolDlg::_connectHost(const char *host, int port)
{
	if (m_pConn){
		DestroyConnectorObj(m_pConn);
		LogicEngineRoot::destroy_Instant();
	}

	NDIConn *pConn = CreateConnectorObj(NULL);
	if (!pConn) {
		return 0;
	}


	// log send data 
	const char *msg_stream_file = "./test_robort.data";
	int length = strlen(msg_stream_file);
	if (pConn->ioctl(NDIOCTL_LOG_SEND_STRAM_FILE, (void*)msg_stream_file, &length) == -1) {
		nd_logmsg("log net message bin-data errror\n");
	}

	if (!init_apollo_object(pConn, m_scriptFile)) {
		out_print("load script %s error \n", m_scriptFile);
		DestroyConnectorObj(pConn);
		return -1;
	}

	if (-1 == pConn->Open(host, port, "tcp-connector", NULL)) {
		out_print( "connect %s:%d ERROR \n", host, port);
		DestroyConnectorObj(pConn);
		return -1;
	}
	out_print("connect %s:%d success \n", host, port);
	m_pConn = pConn;

	//initApolloGameMessage(m_pConn);	
	//ClientMsgHandler::initDftClientMsgHandler(m_pConn, m_scriptFile, out_print);


	return 0;
}
int gmToolDlg::_login(const char *user, const char *passwd,bool bSkipAuth)
{
	int ret = 0;
	if (m_login){
		//ApolloDestroyLoginInst(m_login);
		delete m_login;
	}
	m_login = new LoginApollo(m_pConn->GetHandle(), _SESSION_FILE, "unknow-udid-this-mfc-test");
	if (!m_login){
		return -1;
	}
	
// 	if (_s_session_size && _s_session_buf[0]){
// 		ret = m_login->ReloginEx((void*)_s_session_buf, _s_session_size);
// 		if (0==ret)	{
// 			out_print("%s login success \n",user);
// 			return 0;
// 		}
// 	}

	if (bSkipAuth) {

		ret = m_login->Login(user, passwd, ACC_OTHER_3_ACCID, true);
	}
	else {
		ret = m_login->Login(user, passwd, ACC_APOLLO, false);
	}
	if (-1 == ret) {
		if (m_login->GetLastError() == NDSYS_ERR_NOUSER && !bSkipAuth) {
			account_base_info acc;
			initAccCreateInfo(acc, ACC_APOLLO, user, passwd);

			ret = m_login->CreateAccount(&acc);
		}
	}

	if (ret == 0) {
		nd_logmsg("login complete SUCCESS!\n", m_login->getAccountID() );
		m_login->ReadyGame();
		_s_session_size = m_login->GetSessionData(_s_session_buf, sizeof(_s_session_buf));
	}
	else {
		nd_logerror("login error %d \n", m_login->GetLastError());
		return -1;
	}

	return 0;
}

//int gmToolDlg::_relogin(void *sessionData, size_t session_size);

int gmToolDlg::SelOrCreateRole()
{
	//JUMP TO server 
	ApolloServerInfo bufs[20];
	//LoginApollo login(m_pConn->GetHandle(), false);
	int num = m_login->GetServerList(bufs, ND_ELEMENTS_NUM(bufs));

	if (num == 0) {
		out_log("get host list number=0\n");
		return -1 ;
	}

	for (int i = 0; i < num; i++)	{
		nd_logmsg("ONLINE-NUMBER >>> host %s:%d : %d / %d \n", (const char*)bufs[i].inet_ip, bufs[i].port,
			bufs[i].cur_number, bufs[i].max_number);
	}

	int selected = 0;
	if (num > 1) {
		InsertNodeListDlg dlg;
		for (int i = 0; i < num; i++)	{
			dlg.m_selList.push_back(CString((LPCTSTR)bufs[i].name));
		}
		if (IDOK != dlg.DoModal())  {

			out_log("you cancel login\n");
			return -1;
		}
		selected = dlg.GetSelect();
		nd_assert(selected < num);
	}

	int ret = m_login->EnterServer((char*)bufs[selected].inet_ip, bufs[selected].port);
	//int ret = redirectServer(m_pConn->GetHandle(), nd_inet_ntoa(bufs[0].ip, NULL), bufs[0].port, _SESSION_FILE);
	if (ret == 0) {
		out_log("redirect server success\n");
		_s_session_size = m_login->GetSessionData(_s_session_buf, sizeof(_s_session_buf));
	}
	else {
		out_log("enter game server error \n");
		return -1;
	}
	//get role list

	NDOStreamMsg omsg(NETMSG_MAX_LOGIN, LOGIN_MSG_GET_ROLE_LIST_REQ);
	omsg.Write((NDUINT8)0);
	nd_handle h = m_pConn->GetHandle();
	nd_usermsgbuf_t recv_msg;

	_SEND_AND_WAIT(h, omsg, &recv_msg, NETMSG_MAX_LOGIN, LOGIN_MSG_GET_ROLE_LIST_ACK, 0)
	else {
		NDUINT32 roleid = 0;
		NDUINT32 error_code = 0;
		NDUINT8 name[USER_NAME_SIZE];

		NDIStreamMsg inmsg(&recv_msg);
		inmsg.Read(roleid);

		if (roleid == 0) {
			inmsg.Read(error_code);
			if (error_code) {
				nd_logerror("get role list error : %d\n", error_code);
				return -1 ;
			}
			CString roleName =m_strUserName + "role";

			nd_gbk_to_utf8((LPCTSTR)roleName,(char*) name, sizeof(name));

			return createRole((LPCTSTR)name);

		}
		else {
			inmsg.Read(name, sizeof(name));

			char roleName[128];
			nd_utf8_to_gbk((const char*)name, roleName, sizeof(roleName));

			nd_logmsg("get role %s id=%d success \n", roleName, roleid);

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
		}
	}
	return 0;
}

//init role data
int gmToolDlg::getRoleData()
{
#define SEND_MSG16(msgid) m_pConn->Send(ND_HIBYTE(msgid), ND_LOBYTE(msgid), 0, 0)
// 	
// 	SEND_MSG16(NETMSG_ROLE_PACKAGE_INIT_REQ);
// 	SEND_MSG16(NETMSG_ROLE_ATTR_INIT_REQ);
// 	SEND_MSG16(NETMSG_ROLE_EQUIP_INIT_REQ);
// 	SEND_MSG16(NETMSG_ROLE_SKILL_INIT_REQ);
// 	SEND_MSG16(NETMSG_ROLE_CLIENT_SETTING_REQ);
// 	SEND_MSG16(NETMSG_ROLE_MAPS_SCORE_REQ);
// 
// 	SEND_MSG16(NETMSG_DRAGON_INIT_REQ);
// 	SEND_MSG16(NETMSG_TASK_GET_ACHIEVEMENT_REQ);
// 	SEND_MSG16(NETMSG_TASK_CHECK_LOGIN_AWARDS_REQ);
// 	SEND_MSG16(NETMSG_TASK_INIT_CURRENT_TASK_REQ);
// 	SEND_MSG16(NETMSG_TASK_GET_LOGIN_AWARDS_REQ);
// 	SEND_MSG16(NETMSG_TASK_SIGN_IN_CHECK_REQ);
// 	SEND_MSG16(NETMSG_TASK_INIT_COMPLTETD_TASK_REQ);
	return 0; 
}

void gmToolDlg::LogText(const char* text)
{
	CListBox *pList = (CListBox *)GetDlgItem(IDC_LIST_LOG);
	if (!pList)	{
		return;
	}
	char buf[4096];
	char *p = (char*)text;
	do	{
		buf[0] = 0;
		p = (char*)ndstr_nstr_end(p, buf, '\n', sizeof(buf));
		if (buf[0]){
			if (m_newLine)	{
				pList->AddString((LPCTSTR)text);
			}
			else {
				int total = pList->GetCount();
				if (total > 0)	{
					CString str1;
					int index = total - 1;
					pList->GetText(index, str1);
					pList->DeleteString(index);
					str1 += (LPCTSTR)buf;
					pList->AddString((LPCTSTR)str1);
				}
			}
		}

		if (p && *p == '\n') {
			++p;
			m_newLine = true;
		}
		else {
			m_newLine = false;
		}

	} while (p && *p);

	pList->SetCurSel(pList->GetCount() - 1);
}

void gmToolDlg::clearLog()
{
	CListBox *pList = (CListBox *)GetDlgItem(IDC_LIST_LOG);
	if (pList) {
		pList->ResetContent();
		UpdateData(FALSE);
		pList->SetHorizontalExtent(4096);
	}
	m_newLine = true;

}


void gmToolDlg::OnTimer(UINT_PTR nIDEvent)
{
	static ndtime_t lastTick = nd_time();
	if (1==nIDEvent){
		if (m_pConn)	{
			int ret = m_pConn->Update(0);
			if (-1==ret){
				OnBnClickedButtonLogin();
			}

			ndtime_t now = nd_time();
			LogicParserEngine  &parser = LogicEngineRoot::get_Instant()->getGlobalParser();
			parser.update(now - lastTick);
			lastTick = now;
		}
	}
	else if (2 == nIDEvent) {
		LogicEngineRoot *scriptRoot = LogicEngineRoot::get_Instant();
		if (scriptRoot) {
			parse_arg_list_t arg;
			scriptRoot->getGlobalParser().eventNtf(APOLLO_EVENT_UPDATE, arg);
		}
	}
	CDialog::OnTimer(nIDEvent);
}


void gmToolDlg::OnBnClickedButtonMsgReset()
{
	// TODO: Add your control notification handler code here
	__sendMsg.Reset();
	nd_logmsg("message buffer size =%d\n", __sendMsg.GetDataLen());
}


void gmToolDlg::OnBnClickedButtonServerReload()
{
	if (!m_pConn) {
		nd_logerror("没有登录\n");
		return;
	}

	NDOStreamMsg omsg(ND_MAIN_ID_SYS, ND_MSG_SYS_RELOAD_DATA);
	if (m_pConn->SendMsg(omsg) <= 0) {
		nd_logerror("发送请求失败\n");
		return;
	}

}


void gmToolDlg::OnBnClickedButtonClear()
{
	clearLog();
}

#include "XmlCfg.h"
static bool gmdlgInit(CDlgWithAccelerators *curDlg)
{
	curDlg->GetDlgItem(IDOK)->SetWindowTextA(_T("Send"));

	curDlg->GetDlgItem(IDCANCEL)->SetWindowTextA(_T("Exit"));

	return true;
}
static bool gmdlgSend(CDlgWithAccelerators *curDlg)
{
	gmToolDlg *dlg = (gmToolDlg*) curDlg->GetParent();
	if (!dlg){
		AfxMessageBox("没有登录");
		return true;
	}
	CXmlCfg *xmdlg = (CXmlCfg*)curDlg;
	ndxml *xml = xmdlg->GetCurrentXml();
	if (!xml){
		AfxMessageBox("No function selected");
		return true;
	}
	const char *isCheck = ndxml_getattr_val(xml, "need_confirm");
	if (isCheck && *isCheck){
		if (0==ndstricmp(isCheck,"yes")){
			char buf[128];
			snprintf(buf, sizeof(buf), "确实要%s吗?", ndxml_getattr_val(xml, "name"));
			if (IDYES != AfxMessageBox(buf, MB_YESNO)) {
				return true;
			}
		}
	}
	NDUINT16 msgid = 0;
	ndxml *msgXML = ndxml_getnode(xml, "msgid");
	if (!msgXML)	{
		AfxMessageBox("select xml node error");
		return true;
	}
	else {
		const char *maxId = ndxml_getattr_val(msgXML, "maxId");
		const char *minId = ndxml_getattr_val(msgXML, "minId");
		if (maxId && minId)	{
			msgid = ND_MAKE_WORD(ndstr_atoi_hex(maxId), ndstr_atoi_hex(minId));
		}
		else {
			msgid = ndxml_getval_int(msgXML);
		}
	}

	NDOStreamMsg omsg(msgid);

	for (int i = 1; i < ndxml_num(xml); ++i){
		ndxml *node = ndxml_getnodei(xml, i);
		if (node){
			int typeId = 0;
			const char *typeName = ndxml_getattr_val(node, "param");
			if (typeName)
				typeId = ndstr_atoi_hex(typeName);
			switch (typeId)
			{
			case OT_INT :
				omsg.Write((NDUINT32)ndxml_getval_int(node));
				break;
			case OT_FLOAT:
				omsg.Write(ndxml_getval_float(node));
				break;
			case OT_STRING:
				omsg.Write((NDUINT8*)ndxml_getval(node));
				break;
			case OT_INT8:
				omsg.Write((NDUINT8)ndxml_getval_int(node));
				break;
			case OT_INT16:
				omsg.Write((NDUINT16)ndxml_getval_int(node));
				break;
			case OT_INT64:
				omsg.Write((NDUINT64)ndxml_getval_int(node));
				break;
			default:
				AfxMessageBox("config error ");
				break;
			}

		}
	}
	NDIConn *pconn = dlg->getConnect();
	if (pconn)	{
		pconn->SendMsg(omsg);
	}
	else{
		AfxMessageBox("need login");
	}
	return false;
}


void gmToolDlg::OnBnClickedBtGmmsg()
{

	CXmlCfg xmlDlg(this,gmdlgInit, gmdlgSend) ;
	xmlDlg.SetXML(m_editor_setting, m_gmCfg);
	xmlDlg.DoModal();

}


void gmToolDlg::OnBnClickedButtonShowError()
{
	// TODO: Add your control notification handler code here
	UpdateData();
	int a = ndstr_atoi_hex((LPCTSTR)m_msgData);

	out_print(NULL, "error[%d] : %s\n", a, apollo_error(a));
	
}


void gmToolDlg::OnBnClickedCheckShowHex()
{
	// TODO: Add your control notification handler code here
	CButton *pbt = (CButton *) GetDlgItem(IDC_CHECK_SHOW_HEX);
	if (pbt){
		DBLDataNode::setOutHex(pbt->GetCheck() ? true : false);
	}
}


//void gmToolDlg::OnBnClickedCheck2()
//{
//	// TODO: Add your control notification handler code here
//}
