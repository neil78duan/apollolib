#include "connectdialog.h"
#include "ui_connectdialog.h"
#include <QTimer>
#include <QMessageBox>
#include <QString>
#include <QSettings>

#include "apoScript/listdialog.h"

#include "ndlib.h"
#include "cli_common/login_apollo.h"
#include "cli_common/netui_atl.h"
//#include "cli_common/gameMessage.h"
#include "cli_common/login_apollo.h"
#include "cli_common/dftCliMsgHandler.h"

#include "logic_parser/dbldata2netstream.h"
#include "logic_parser/logicEngineRoot.h"
#include "logic_parser/logicDataType.h"
#include "logic_parser/dbl_mgr.h"

#include "apollo_errors.h"
//#include "netMessage/message_inc.h"
#include "logic_parser/script_event_id.h"

#include <string>

#include "ndlog_wrapper.h"
ND_LOG_WRAPPER_IMPLEMENTION(ConnectDialog, __glogWrapper);

NDOStreamMsg __sendMsg;

/////////////////////////////////////

class ConnectScriptOwner :public  ClientMsgHandler::ApoConnectScriptOwner
{
public:
    bool getOtherObject(const char*objName, DBLDataNode &val)
    {
        if (0 == ndstricmp(objName, "LogFunction")) {
			val.InitSet((void*)ND_LOG_WRAPPER_PRINT(ConnectDialog));
            return true;
        }
        else if (0 == ndstricmp(objName, "LogFile")) {
            val.InitSet("ndlog.log");
            return true;
        }
        else if (0 == ndstricmp(objName, "LogPath")) {
            val.InitSet("../../log");
            return true;
        }
        else if (0 == ndstricmp(objName, "WritablePath")) {
            val.InitSet("../../log");
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
	scriptRoot->setOutPutEncode(E_SRC_CODE_UTF_8);
    nd_assert(scriptRoot);
    LogicParserEngine  &parser = LogicEngineRoot::get_Instant()->getGlobalParser();
    nd_message_set_script_engine(pConn->GetHandle(), (void*)&parser, ClientMsgHandler::apollo_cli_msg_script_entry);
    parser.setOwner(&__myScriptOwner);

	scriptRoot->setPrint(ND_LOG_WRAPPER_PRINT(ConnectDialog), NULL);
    scriptRoot->getGlobalParser().setSimulate(false);
    pConn->SetUserData(&parser);

    if (0 != scriptRoot->LoadScript(script_file, &scriptRoot->getGlobalParser())){
        nd_logerror("load script error！\n");
        return false;
    }
    ClientMsgHandler::InstallDftClientHandler(pConn);
    pConn->SetDftMsgHandler(ClientMsgHandler::apollo_dft_message_handler);

    //pConn->InstallMsgFunc(get_data_format_handler, ND_MAIN_ID_SYS, ND_MSG_SYS_GET_USER_DEFINE_DATA);
    return true;
}
//////////////////////////////////////
#define _SESSION_FILE "./gmtoolSession.ses"

static char _s_session_buf[4096];
static size_t _s_session_size = 0;

//static userDefineDataType_map_t m_dataTypeDef;



#define _SEND_AND_WAIT(_conn, _omsg, _rmsg_buf,_wait_maxid, _wait_minid,_sendflag) \
    if(0!=ndSendAndWaitMessage(_conn,_omsg.GetMsgAddr(),_rmsg_buf,_wait_maxid, _wait_minid,_sendflag,WAITMSG_TIMEOUT) ) {	\
        nd_logerror( "send and wait data error code =%d\n", nd_object_lasterror(_conn)); \
        return -1;		\
    }

ConnectDialog::ConnectDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ConnectDialog),
    m_pConn(0),
    m_login(0)
{
    ui->setupUi(this);

    timer = new QTimer() ;
    timer->setInterval(1000) ;
    timer->start() ;

    connect(timer, &QTimer::timeout,this, &ConnectDialog::onTimeOut) ;


    m_editor_setting =0;
    m_gmCfg = 0;
	m_scriptFile = NULL;

    ui->gmMsgButton->setEnabled(false);

	
    InitNet();

	__glogWrapper = ND_LOG_WRAPPER_NEW(ConnectDialog);


    //get init setting
    QSettings settings("duanxiuyun", "ApolloEditor");
#define SET_EDIT_DFTVAL(_name,_dftval)  do { \
    QString var1 = settings.value(#_name, _dftval).toString() ; \
    ui->_name ##Edit->setText(var1) ;      \
}while (0)

    //SET_EDIT_DFTVAL(host, "192.168.8.49") ;
    SET_EDIT_DFTVAL(port, "6600") ;
    SET_EDIT_DFTVAL(name, "tRobort1") ;
    SET_EDIT_DFTVAL(passwd, "test123") ;

#undef SET_EDIT_DFTVAL
	InitHostList();

	QString tmoutText;
	tmoutText.sprintf("%d", WAITMSG_TIMEOUT/1000);
	ui->timeOutEdit->setText(tmoutText);
}

ConnectDialog::~ConnectDialog()
{
    delete ui;
    if(timer)
        delete timer ;
	
    destroy_apollo_object(m_pConn);
   
	if (m_pConn){
		DestroyConnectorObj(m_pConn);
	}
	
	if (m_login){
		ApolloDestroyLoginInst(m_login);
	}
	
	ND_LOG_WRAPPER_DELETE(__glogWrapper);


	DBLDatabase *pdbl = DBLDatabase::get_Instant();
	if (pdbl){
		pdbl->Destroy();
	}

    DeinitNet();

    //ndxml_destroy(&m_message_define);
}

void ConnectDialog::saveHost(const QString &hostName)
{
	QSettings settings("duanxiuyun", "ApolloEditor");

	settings.setValue("lastHost", hostName);

	int size = settings.beginReadArray("hostList");
	for (int i = 0; i < size; ++i) {
		settings.setArrayIndex(i);
		QString ip = settings.value("ip").toString();
		if (ip == hostName)	{
			settings.endArray();
			return;
		}
	}
	settings.endArray();

	settings.beginWriteArray("hostList");	
	settings.setArrayIndex(size);
	settings.setValue("ip", hostName);
	settings.endArray();

}

void ConnectDialog::InitHostList()
{
	QSettings settings("duanxiuyun", "ApolloEditor");
	QString lastHost = settings.value("lastHost", "10.20.20.248").toString();

	ui->hostCombo->addItem(lastHost);

	int size = settings.beginReadArray("hostList");
	if (size == 0) {
		settings.endArray();
		//ui->hostCombo->addItem("10.20.20.248");
		return;
	}

	for (int i = 0; i < size; ++i) {
		settings.setArrayIndex(i);
		QString ip = settings.value("ip").toString();
		if (ip ==lastHost)	{
			continue;
		}
		ui->hostCombo->addItem(ip);
	}

	settings.endArray();
}


void ConnectDialog::onTimeOut()
{
    //WriteLog("timer running...") ;
    if(m_pConn) {
        int ret = m_pConn->Update(0) ;
        if(ret == -1) {
			on_loginButton_clicked();
            //close
            //m_pConn->Close
        }
		else {
			LogicEngineRoot *scriptRoot = LogicEngineRoot::get_Instant();
			if (scriptRoot) {
				parse_arg_list_t arg;
				scriptRoot->getGlobalParser().eventNtf(APOLLO_EVENT_UPDATE, arg);

				//tick
				static ndtime_t lastTick = nd_time();
				ndtime_t now = nd_time();
				scriptRoot->getGlobalParser().update(now - lastTick);
				lastTick = now;
			}
		}
    }

}

void ConnectDialog::ClearLog()
{
    ui->logEdit->clear();
}

void ConnectDialog::WriteLog(const char *logText)
{
	
	QTextEdit *pEdit = ui->logEdit;
	
	pEdit->moveCursor(QTextCursor::End, QTextCursor::KeepAnchor);
	pEdit->insertPlainText(QString(logText));
	
}

bool ConnectDialog::LoadClientScript(const char *file,const char *dblFile)
{
    //if (-1 == loadUserDefFromMsgCfg(file, m_dataTypeDef))	{
    //	nd_logerror("load message datatype error from %s\n", file);
    //	return false;
    //}
	DBLDatabase *pdbl = DBLDatabase::get_Instant();
	if (pdbl){
		pdbl->LoadBinStream(dblFile);
	}


	m_scriptFile = file;
    //if (message_def){
    //	ndxml_load_ex(message_def, &m_message_define, nd_get_encode_name(ND_ENCODE_TYPE));
    //}

	return true;
}

void ConnectDialog::on_loginButton_clicked()
{

    if (m_pConn){

        ui->loginButton->setText(tr("Login"));
        if (m_pConn->CheckValid())	{
            m_pConn->Close();
        }
        DestroyConnectorObj(m_pConn);
        m_pConn = NULL;

        if (m_login){
            ApolloDestroyLoginInst(m_login);
            m_login = 0;
        }

        //ClearLog();
        WriteLog("logout success\n");
        ui->gmMsgButton->setEnabled(false);

    }
    else{
        //get host name
        //std::string strHost = ui->hostEdit->text().toStdString();
		std::string strHost = ui->hostCombo->currentText().toStdString();
        int port = ui->portEdit->text().toInt();
        std::string strName = ui->nameEdit->text().toStdString() ;
        std::string strPasswd = ui->passwdEdit->text().toStdString() ;
		bool skipAuth = ui->skipAuth->isChecked();
		int timeOut = ui->timeOutEdit->text().toInt();

		if (timeOut > 0)		{
			ndSetTimeoutVal(timeOut * 1000);
		}
		//for test 
		//end test 
        __sendMsg.Reset();
        ClearLog();
        //account_index_t aid = 0;

        if (-1 == _connectHost(strHost.c_str() , port)){
            nd_logerror("connect server error\n");
            return;
        }

        nd_logmsg("CONNECT %s:%d SUCCESS\n", strHost.c_str() , port);

		if (-1 == _login(strName.c_str(), strPasswd.c_str(), skipAuth))	{
            nd_logerror("user login error\n");
            return;
        }
        nd_logmsg("ACCOUNT %s login SUCCESS\n", strName.c_str());
		
        if (-1==SelOrCreateRole(strName.c_str()))	{
            nd_logerror("create or select role error\n");

            m_pConn->Close();
            DestroyConnectorObj(m_pConn);
            m_pConn = NULL;
            return;
        }

        // log send data
		if (ui->logSendCheck->isChecked()) {

			const char *msg_stream_file = "./test_robort.data";
			int length = (int)strlen(msg_stream_file);
			if (m_pConn->ioctl(NDIOCTL_LOG_SEND_STRAM_FILE, (void*)msg_stream_file, &length) == -1) {
				nd_logmsg("log net message bin-data errror\n");
			}
		}

		__myScriptOwner.LoadMsgDataTypeFromServer();
		LogicEngineRoot *scriptRoot = LogicEngineRoot::get_Instant();
		if (scriptRoot) {
			parse_arg_list_t arg;
			arg.push_back(DBLDataNode(m_pConn, OT_OBJ_NDOBJECT));
			scriptRoot->getGlobalParser().eventNtf(APOLLO_EVENT_LOGIN, arg);
		}

        //getRoleData();

        ui->loginButton->setText(tr("Logout"));
        ui->gmMsgButton->setEnabled(true);

        //save current  setting value
        QSettings settings("duanxiuyun", "ApolloEditor");
    #define SET_EDIT_DFTVAL(_name,_dftval)  do { \
        QString var1 = ui->_name ##Edit->text() ;      \
        settings.setValue(#_name, var1); \
    }while (0)
        //SET_EDIT_DFTVAL(host, "192.168.8.49") ;
        SET_EDIT_DFTVAL(port, "6600") ;
        SET_EDIT_DFTVAL(name, "tRobort1") ;
        SET_EDIT_DFTVAL(passwd, "test123") ;
#undef  SET_EDIT_DFTVAL
		saveHost(ui->hostCombo->currentText()) ;
    }
}

void ConnectDialog::on_msgWriteButton_clicked()
{
    if (ui->radioButton->isChecked()){
		QString qs = ui->msgDataEdit->text();
		if (qs.size() > 0) {
			int a = ndstr_atoi_hex(qs.toStdString().c_str());//ui->msgDataEdit->text().toInt();
			__sendMsg.Write((NDUINT8)a);
		}
    }
    else if (ui->radioButton_2->isChecked()){
		QString qs = ui->msgDataEdit->text();
		if (qs.size() > 0) {
			int a = ndstr_atoi_hex(qs.toStdString().c_str());//ui->msgDataEdit->text().toInt();
			__sendMsg.Write((NDUINT16)a);
		}
    }
    else if (ui->radioButton_3->isChecked()){

		QString qs = ui->msgDataEdit->text();
		if (qs.size() > 0) {
			int a = ndstr_atoi_hex(qs.toStdString().c_str());//ui->msgDataEdit->text().toInt();
			__sendMsg.Write((NDUINT32)a);
		}
    }
    else if (ui->radioButton_4->isChecked()){

		QString qs = ui->msgDataEdit->text();
		if (qs.size() > 0) {
			NDUINT64 a = ndstr_atoll_hex(qs.toStdString().c_str());//ui->msgDataEdit->text().toInt();
			__sendMsg.Write(a);
		}
    }
    else if (ui->radioButton_5->isChecked()){
        float a = ui->msgDataEdit->text().toFloat();
        __sendMsg.Write(a);
    }
    else if (ui->radioButton_6->isChecked()){
        double a = ui->msgDataEdit->text().toDouble() ;
        __sendMsg.Write(a);
    }
    else{
        std::string data = ui->msgDataEdit->text().toStdString() ;
        __sendMsg.Write((NDUINT8*)data.c_str());
    }

    nd_logmsg("WRITE MESSAGE buf  total size =%d\n", __sendMsg.GetDataLen());
}

void ConnectDialog::on_msgSendButton_clicked()
{
    if (!m_pConn){
        WriteLog("not login Please login first\n");
        return;
    }
    int maxid = ui->msgMaxIdEdit->text().toInt() ;
    int minid = ui->msgMinIdEdit->text().toInt() ;

    __sendMsg.SetID(maxid, minid);
    int ret = m_pConn->SendMsg(__sendMsg);

    nd_logmsg("send message size =%d\n", ret);
}

void ConnectDialog::on_msgReset_clicked()
{

    __sendMsg.Reset();
    nd_logmsg("message buffer size =%d\n", __sendMsg.GetDataLen());
}

int ConnectDialog::_connectHost(const char *host, int port)
{
    if (m_pConn){
        DestroyConnectorObj(m_pConn);
    }

    NDIConn *pConn = CreateConnectorObj(NULL);
    if (!pConn) {
        return 0;
    }

    if (-1 == pConn->Open(host, port, "tcp-connector", NULL)) {
        nd_logmsg( "connect %s:%d ERROR \n", host, port);
        DestroyConnectorObj(pConn);
        return -1;
    }
	nd_logmsg("connect %s:%d success \n", host, port);
    m_pConn = pConn;


    //initApolloGameMessage(m_pConn);
	if (!init_apollo_object(m_pConn, m_scriptFile)) {

		nd_logmsg("Load script %s error\n", m_scriptFile);
		DestroyConnectorObj(pConn);

		return -1;
	}
	//initApolloGameMessage(m_pConn);
    //ClientMsgHandler::initDftClientMsgHandler(m_pConn, m_scriptFile, m_dataTypeDef, &m_message_define, out_print);
    return 0;
}

int ConnectDialog::_login(const char *user, const char *passwd,bool skipAuth)
{
    int ret = 0;
    if (m_login){
        ApolloDestroyLoginInst(m_login);

    }
    m_login = ApolloCreateLoginInst();
    if (!m_login){
        return -1;
    }
    m_login->ReInit(m_pConn->GetHandle(), _SESSION_FILE,"unknow-udid-this-qt-test");

    /*
    if (_s_session_size && _s_session_buf[0]){
        ret = m_login->ReloginEx((void*)_s_session_buf, _s_session_size);
        if (0==ret)	{
            out_print("%s login success \n",user);
            return 0;
        }
    }*/
	if (skipAuth) {

		ret = m_login->Login(user, passwd, ACC_OTHER_3_ACCID, true);
	}
	else {
		ret = m_login->Login(user, passwd, ACC_APOLLO, false);
	}
    if (-1==ret) {
        if (m_login->GetLastError() == NDSYS_ERR_NOUSER && !skipAuth) {
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

int ConnectDialog::_relogin(void *sessionData, size_t session_size)
{

    return 0;
}

int ConnectDialog::ShowServers(void *hostsAddr, int size, const char *tips)
{
	ApolloServerInfo *bufs = (ApolloServerInfo*)hostsAddr;
	int num = size;

	for (int i = 0; i < num; i++)	{
		nd_logmsg("ONLINE-NUMBER >>> host %s:%d : %d / %d \n", (const char*)bufs[i].inet_ip, bufs[i].port,
			bufs[i].cur_number, bufs[i].max_number);
	}

	int selected = 0;
	if (num > 1){
		//dialogCloseHelper _helperClose(this) ;
		ListDialog dlg(this,tips);
		for (int i = 0; i < num; i++)	{
			QString strtips;
			strtips.sprintf("%d: %s %s:%d (%d/%d)", i,
				(const char*)bufs[i].name, (const char*)bufs[i].inet_ip, bufs[i].port,
				bufs[i].cur_number, bufs[i].max_number);

			dlg.m_selList.push_back(strtips);
		}

		dlg.InitList();
		if (dlg.exec() == QDialog::Accepted) {
			WriteLog("selected success \n");
		}
		else {
			WriteLog("user cancel\n");
			return -1;
		}
		selected = dlg.GetSelect();
		nd_assert(selected < num);
	}
	return selected;
}

int ConnectDialog::SelOrCreateRole(const char *accountName)
{
    //JUMP TO server
    ApolloServerInfo bufs[20];
    int num = m_login->GetServerList(bufs, ND_ELEMENTS_NUM(bufs));
    if (num == 0) {
        WriteLog("Get GAME-SERVER-LIST error , Maybe game server start FAILED\n");
        return -1 ;
    }
	bool skipLoadBalance = false;
	int selected = ShowServers((void *)bufs, num, "Group list");
	if (-1==selected)	{
		return -1;
	}
	
	if (ui->showServers->isChecked())	{
		ApolloServerInfo groupEntry = bufs[selected];
		num = m_login->GetSubHostList((char*)groupEntry.inet_ip, groupEntry.port, bufs, ND_ELEMENTS_NUM(bufs));
		if (num ==0){
			WriteLog("Get SUB HOST list error , Maybe game server start FAILED\n");
			return -1;
		}
		selected = ShowServers((void *)bufs, num, "Game Servers");
		if (-1 == selected)	{
			return -1;
		}
		skipLoadBalance = true;
	}

	
    int ret = m_login->EnterServer((char*)bufs[selected].inet_ip, bufs[selected].port);
    if (ret == 0) {
        WriteLog("redirect server success\n");
        _s_session_size = m_login->GetSessionData(_s_session_buf, sizeof(_s_session_buf));
    }
    else {
        WriteLog("enter game server error \n");
        return -1;
    }

    //get role list

	role_base_info role;
	ret = m_login->GetRoleList(&role, 1);
	if (ret == 0){
		return m_login->CreateRole((const char*)accountName, role);
	}

    return 0;
}

int ConnectDialog::getRoleData()
{
#define SEND_MSG16(msgid) m_pConn->Send(ND_HIBYTE(msgid), ND_LOBYTE(msgid), 0, 0)

    /*
    SEND_MSG16(NETMSG_ROLE_PACKAGE_INIT_REQ);
    SEND_MSG16(NETMSG_ROLE_ATTR_INIT_REQ);
    SEND_MSG16(NETMSG_ROLE_EQUIP_INIT_REQ);
    SEND_MSG16(NETMSG_ROLE_SKILL_INIT_REQ);
    SEND_MSG16(NETMSG_ROLE_CLIENT_SETTING_REQ);
    SEND_MSG16(NETMSG_ROLE_MAPS_SCORE_REQ);

    SEND_MSG16(NETMSG_DRAGON_INIT_REQ);
*/
    return 0;
}


#include "apoScript/xmldialog.h"
#include "startdialog.h"

static bool gmdlgInit(QDialog *curDlg)
{

    return true;
}

static bool gmdlgExit(QDialog *curDlg)
{

    return false;
}

static bool gmdlgSend(QDialog *curDlg)
{
    ConnectDialog *dlg = (ConnectDialog*) curDlg->parent();
    if (!dlg){
        QMessageBox::warning(NULL, "Error","Need login!",QMessageBox::Ok);
        return true;
    }
    XMLDialog *xmdlg = (XMLDialog*)curDlg;
    ndxml *xml = xmdlg->GetCurrentXml();
    if (!xml){
        return true;
    }
    NDUINT16 msgid = 0;
    ndxml *msgXML = ndxml_getnode(xml, "msgid");
    if (!msgXML)	{
        QMessageBox::warning(NULL, "Error","select xml node error!",QMessageBox::Ok);
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
                typeId = atoi(typeName);
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
			case OT_BINARY_DATA:
			{
				const char *p = ndxml_getval(node);
				size_t s = (p && *p) ? strlen(p) : 0;
				omsg.WriteBin((void*)p, s);
			}
			break;
            default:
                QMessageBox::warning(NULL, "Error","config error!",QMessageBox::Ok);
                break;
            }

        }
    }
    NDIConn *pconn = dlg->getConnect();
    if (pconn)	{
        pconn->SendMsg(omsg);
    }
    else{

        QMessageBox::warning(NULL, "Error","Need login!",QMessageBox::Ok);
    }
    return false;
}


void ConnectDialog::on_gmMsgButton_clicked()
{
    //dialogCloseHelper _helperClose(this) ;

    XMLDialog xmlDlg(this);
    xmlDlg.showXml( m_gmCfg,"Message");
    xmlDlg.preInit(gmdlgInit,gmdlgSend, gmdlgExit) ;
    xmlDlg.setButtonText("Send", "Exit") ;
    xmlDlg.exec() ;

}

void ConnectDialog::on_pushButton_clicked()
{
    QString qs = ui->msgDataEdit->text();
	if (qs.size() > 0) {
		int a = ndstr_atoi_hex(qs.toStdString().c_str());//ui->msgDataEdit->text().toInt();
		nd_logmsg("error[%d]: %s\n", a, apollo_error(a));
	}

}

void ConnectDialog::on_checkBox16Hex_clicked()
{
	if (ui->checkBox16Hex->checkState() == Qt::Checked)  {
		DBLDataNode::setOutHex(true);
	}
	else {
		DBLDataNode::setOutHex(false);
	}
	//ui->checkBox16Hex
}

void ConnectDialog::on_pushButtonExit_clicked()
{
    accept() ;
}

void ConnectDialog::on_pushButton_2_clicked()
{
    ClearLog();
}
