#include "startdialog.h"
#include "ui_startdialog.h"
#include "connectdialog.h"
#include "nd_common/nd_common.h"
#include "apoScript/xmldialog.h"
#include "ndlib.h"

#include "logic_parser/dbl_mgr.h"
#include "logic_parser/logic_compile.h"
#include "logic_parser/logicParser.h"
#include "logic_parser/logicEngineRoot.h"
#include "logic_parser/dbl_mgr.h"
#include "attribute/roleattr_help.h"
#include "attribute/stat_machine.h"
#include "ndapplib/ndsingleton.h"

#include "cli_common/dftCliMsgHandler.h"
#include "apoScript/apoEditorSetting.h"

#include "apoScript/editorFrame.h"

#include <QMessageBox>

#include "ndlog_wrapper.h"
ND_LOG_WRAPPER_IMPLEMENTION(startDialog, __glogWrapper);

// static startDialog *__g_loginDlg ;
// static void *__oldFunc ;


#define _LOAD_XML(_xml_name, _filename,_encode,_isexit) \
    ndxml_root _xml_name;				\
    ndxml_initroot(&_xml_name);			\
    if (ndxml_load_ex((char*)_filename, &_xml_name,_encode)) {	\
        nd_logerror("open file %s error", _filename);	\
        if(_isexit) return;							\
    }


startDialog::startDialog(QWidget *parent) :
	QDialog(parent), 
    ui(new Ui::startDialog),
	m_editor_setting(*(apoEditorSetting::getInstant()->getConfig())),
	m_io_setting(*(apoEditorSetting::getInstant()->getIoConfig())),
	editorConfigFile(apoEditorSetting::getInstant()->m_edirotSettingFile),
	ioConfigFile(apoEditorSetting::getInstant()->m_ioConfigFile)
{
    ui->setupUi(this);

	__glogWrapper = ND_LOG_WRAPPER_NEW(startDialog);
}

startDialog::~startDialog()
{
    delete ui;
	ND_LOG_WRAPPER_DELETE(__glogWrapper);
}


void startDialog::ClearLog()
{
    ui->LogText->clear();

	ui->LogText->update();
}

void startDialog::WriteLog(const char *logText)
{
// #ifdef WIN32
// 	char buf[1024];
// 	nd_gbk_to_utf8(logText, buf, sizeof(buf));
// 	logText = buf ;
// #endif
	QTextEdit *pEdit = ui->LogText;
	pEdit->moveCursor(QTextCursor::End);
	pEdit->insertPlainText(QString(logText));
	pEdit->moveCursor(QTextCursor::End, QTextCursor::KeepAnchor);

}


const char *startDialog::getGameDateEncodeType()
{
    int codeType = atoi(_getFromIocfg("game_data_out_encode"));
    return nd_get_encode_name(codeType);
}


const char *startDialog::_getFromIocfg(const char *cfgName)
{
	return	apoEditorSetting::getInstant()->getIoConfigValue(cfgName);

}


const char *startDialog::getScriptSetting(ndxml *scriptXml, const char *settingName)
{
	ndxml *moduleInfo = ndxml_getnode(scriptXml, "moduleInfo");
	if (moduleInfo){
		ndxml *node = ndxml_getnode(moduleInfo, settingName);
		if (node){
			return ndxml_getval(node);
		}
	}
	return NULL;
}

int startDialog::getScriptExpEncodeType(ndxml *scriptXml)
{
	ndxml *moduleInfo = ndxml_getnode(scriptXml, "moduleInfo");
	if (moduleInfo){
		ndxml *node = ndxml_getnode(moduleInfo, "script_out_encode");
		if (node){
			return ndxml_getval_int(node);
		}
	}
	return 0;
}

bool startDialog::getScriptExpDebugInfo(ndxml *scriptXml)
{
	ndxml *moduleInfo = ndxml_getnode(scriptXml, "moduleInfo");
	if (moduleInfo){
		ndxml *node = ndxml_getnode(moduleInfo, "script_with_debug");
		if (node){
			return ndxml_getval_int(node) ? true : false;
		}
	}
	return false;
}
void startDialog::on_Setting_clicked()
{
    dialogCloseHelper _helperClose(this) ;
    XMLDialog xmlDlg(NULL);
	
    xmlDlg.showXml( &m_io_setting,"EditorConfig");

    if (xmlDlg.exec() == QDialog::Accepted) {

        //nd_chdir(nd_getcwd());
        ndxml_save(&m_io_setting, ioConfigFile.c_str());
        WriteLog("save script ok\n");
    }
    else {
        WriteLog("script unedited\n");
    }

}

bool startDialog::compile()
{
	const char *script_root = _getFromIocfg("script_root");

	ndxml_root xmlEntry;
	ndxml_initroot(&xmlEntry);
	if (-1 == ndxml_load_ex(script_root, &xmlEntry, apoEditorSetting::getInstant()->m_encodeName.c_str())) {
		return false;
	}

	ndxml_root *xml = ndxml_getnode(&xmlEntry, "script_file_manager");
	if (!xml){
		return false;
	}
	
	bool ret = true;
	int num = ndxml_num(xml);
	for (int i = 0; i < num; i++) {
		ndxml *node = ndxml_getnodei(xml, i);
		if (!node)
			continue;
		std::string scriptPath = ndxml_getval(node);
		if (scriptPath.size() > 0) {
			scriptPath += "/";
		}
		scriptPath += ndxml_getattr_val(node, "main_file");

		if (!compileScript(scriptPath.c_str())) {
			ret = false;
			break;
		}
	}

	ndxml_destroy(&xmlEntry);
	return true;
}

bool startDialog::compileScript(const char *scriptFile)
{
	ndxml_root xmlScript;
	ndxml_initroot(&xmlScript);
	if (-1 == ndxml_load_ex(scriptFile, &xmlScript, apoEditorSetting::getInstant()->m_encodeName.c_str())) {
		return false;
	}
	const char*inFile = scriptFile;

	const char *outFile = getScriptSetting(&xmlScript, "out_file");
	if (!outFile){
		nd_logerror("compile %s error !!!\nMaybe file is destroyed\n", scriptFile);
		return false;
	}
	//get out file absolute path 
	std::string curPath = nd_getcwd();
	char outFilePath[ND_FILE_PATH_SIZE];

	if (!nd_getpath(scriptFile, outFilePath, sizeof(outFilePath))) {
		nd_logerror("get out file path error 1\n");
		return false;
	}
	if (-1 == nd_chdir(outFilePath)) {
		nd_logerror("get script file-path error 2\n");
		return false;
	}

	if (!nd_absolute_filename(outFile, outFilePath, sizeof(outFilePath))) {
		nd_logerror("get out file path error 3\n");
		nd_chdir(curPath.c_str());
		return false;
	}
	nd_chdir(curPath.c_str());
	outFile = outFilePath;
	// ---end get out file absolute path


	int outEncode = getScriptExpEncodeType(&xmlScript);
	bool withDebug = getScriptExpDebugInfo(&xmlScript);
	//std::string outPath = outFile;
	ndxml_destroy(&xmlScript);

	//outFile = outPath.c_str();

	int orderType = ND_L_ENDIAN;
	const char *orderName = _getFromIocfg("bin_data_byte_order");
	if (orderName) {
		orderType = atoi(orderName);
	}

	LogicCompiler lgcompile;

	apoEditorSetting* setting = apoEditorSetting::getInstant();

	if (!lgcompile.setConfigFile(setting->m_edirotSettingFile.c_str())) {
		return false;
	}
	if (!lgcompile.compileXml(inFile, outFile, outEncode, withDebug, orderType)) {

		const char *pFunc = lgcompile.m_cur_function.c_str();
		const char *pStep = lgcompile.m_cur_step.c_str();
		char func_name[256];
		char step_name[256];
		if (outEncode == E_SRC_CODE_GBK) {
			pFunc = nd_gbk_to_utf8(pFunc, func_name, sizeof(func_name));
			pStep = nd_gbk_to_utf8(pStep, step_name, sizeof(step_name));
		}
		nd_logerror("compile error file %s, function %s, step %s , stepindex %d\n",
			lgcompile.m_cur_file.c_str(), pFunc, pStep, lgcompile.m_cur_node_index);

		return false;
	}

	nd_logmsg("!!!!!!!!!!COMPILE %s success !!!!!!!!!!!\n begining run script...\n", scriptFile);

	ClientMsgHandler::ApoConnectScriptOwner apoOwner;
	if (!apoOwner.loadDataType(_getFromIocfg("net_data_def"))) {
		WriteLog("load data type error\n");
		return false;
	}
	
	LogicEngineRoot *scriptRoot = LogicEngineRoot::get_Instant();
	nd_assert(scriptRoot);
	scriptRoot->setOutPutEncode(E_SRC_CODE_UTF_8);

	scriptRoot->setPrint(ND_LOG_WRAPPER_PRINT(startDialog), NULL);
	scriptRoot->getGlobalParser().setSimulate(true,&apoOwner);

	if (0 != scriptRoot->LoadScript(outFile, &scriptRoot->getGlobalParser())){
		WriteLog("load script error n");
		LogicEngineRoot::destroy_Instant();
		return false;
	}

	WriteLog("start run script...\n");
	if (0 != scriptRoot->test()){
		WriteLog("run script error\n");

		const char *curErrNode = scriptRoot->getGlobalParser().getLastErrorNode();
		if (curErrNode && *curErrNode) {
			showScriptError(scriptFile, curErrNode);
		}

		LogicEngineRoot::destroy_Instant();
		return false;
	}

	LogicEngineRoot::destroy_Instant();
	nd_logmsg("!!!!!!!!!!!!!!!!!!!SCRIPT %s SUCCESS!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n",scriptFile);
	return true;
}


//#include <QProcess>
bool startDialog::expExcel()
{
    WriteLog("==============Begin export excel===============");

    DBLDatabase::destroy_Instant();
	const char *exp_cmd = _getFromIocfg("game_data_export_cmd");
	const char *excel_path = _getFromIocfg("excel_data_in_path");
	const char *text_path = _getFromIocfg("text_data_out_path");
	const char *package_file = _getFromIocfg("game_data_package_file");
	const char *excel_list = _getFromIocfg("game_data_listfile");

    char exp_cmdbuf[1024];

	if (!exp_cmd || !excel_path || !text_path || !package_file){
		WriteLog("export excel error : on read config file\n");
		return false;
	}

	const char *encodeName = getGameDateEncodeType();
	
#ifdef WIN32
	snprintf(exp_cmdbuf, sizeof(exp_cmdbuf), " %s %s %s %s %s ",  exp_cmd, excel_list, excel_path, text_path, encodeName);
#else
    snprintf(exp_cmdbuf, sizeof(exp_cmdbuf), "sh %s %s %s %s %s ",  exp_cmd, excel_list, excel_path, text_path, encodeName);
	
#endif
	int ret = system(exp_cmdbuf);
	
    if (0 != ret)	{
		nd_logerror("run export shell %s\nerror: %s \n", exp_cmdbuf,nd_last_error());
        return false;
	}

    WriteLog("===============export excel to text file  success =========");


	//get byte order  
	int orderType = ND_L_ENDIAN;
	const char *orderName = _getFromIocfg("bin_data_byte_order");
	if (orderName) {
		orderType = atoi(orderName);
	}

	//DUMP FOR WINDOWS only
#ifdef WIN32
	do 	{
		std::string strWinPack = package_file;
		strWinPack += ".gbk";

		DBLDatabase dbwin;
		if (0 != dbwin.LoadFromText(text_path, excel_list, encodeName, "gbk")) {
			nd_logerror("can not read from text gbk\n");
			return false;
		}
		if (0 != dbwin.Dump(strWinPack.c_str(), "gamedbGBK", orderType)) {
			nd_logmsg("EXPORT game data for windows error\n");
		}
		dbwin.Destroy();

	} while (0);
#endif

	DBLDatabase dbtmp;
	if (0 != dbtmp.LoadFromText(text_path, excel_list, encodeName, encodeName)) {
		nd_logerror("load data from text file error");
		return false;
	}
	if (0 == dbtmp.Dump(package_file, "gamedatadb", orderType)) {
		nd_logmsg("package game data SUCCESS!!\n");
		WriteLog("==========write excel to bin-stream success ===========\n");
	}
	else{
		nd_logerror("write excel to bin-stream FAILED");
	}

	//before run test need load dbl
    DBLDatabase *pdbl = DBLDatabase::get_Instant();
    if (pdbl){
        if (0 != pdbl->LoadBinStream(package_file)) {
            WriteLog("load data from bin-stream error ");
            dbtmp.Destroy();
            DBLDatabase::destroy_Instant();
            return false;
        }
    }
    if (!(*pdbl == dbtmp)){
        nd_logmsg("test data load error , read from text and stream-bin not match!!!!");
        dbtmp.Destroy();
        DBLDatabase::destroy_Instant();
        return false ;
    }

	const char *exp_luapath = _getFromIocfg("lua_data_out_path");
	if (exp_luapath && *exp_luapath ) {
		if (!expLua(exp_luapath, dbtmp)) {
			nd_logmsg("export lua error");
			dbtmp.Destroy();
			DBLDatabase::destroy_Instant();
			return false;
		}
	}

    dbtmp.Destroy();

	std::string strOutTextPath = text_path;
	strOutTextPath += "/test_outputData";
	nd_mkdir(strOutTextPath.c_str());
    if (0 != pdbl->TestOutput(strOutTextPath.c_str())) {
        nd_logmsg("dump from bin-stream to text file error\n");
        DBLDatabase::destroy_Instant();
        return false;

    }


    DBLDatabase::destroy_Instant();
    return true;
}


bool startDialog::loadDataBase()
{

	const char *package_file = _getFromIocfg("game_data_package_file");
	DBLDatabase *pdbl = DBLDatabase::get_Instant();
	if (pdbl){
		if (0 == pdbl->LoadBinStream(package_file)) {
			return true;
		}
	}
	return false;
}

bool startDialog::expLua(const char *outPath, const DBLDatabase &db)
{
	nd_mkdir(outPath);
	bool outflag = DBLDataNode::setOutLua(true);
	if (-1 == db.OutputLua(outPath)) {
		DBLDataNode::setOutLua(outflag);
		return false;
	}
	DBLDataNode::setOutLua(outflag);
	return true;
}

bool startDialog::runTest()
{
    if (expExcel()==false ){
        return false;
    }
    WriteLog("begin run test ........");

    if (compile() == false ) {
        return false;
    }

    bool ret = true;

	const char *package_file = _getFromIocfg("game_data_package_file");

	if (0 != DBLDatabase::get_Instant()->LoadBinStream(package_file)) {
        WriteLog("load script from bin-stream file error");
        return false;;
    }

    do 	{
        const char*attr_table = _getFromIocfg("role_attr_table");
        //const char*state_table = _getFromIocfg("role_state_table");
        //const char*forbid_table = _getFromIocfg("role_forbid_table");
        //const char*operate_table = _getFromIocfg("role_operate_check_table");
        NDSingleton<RoleAttrHelper>::Destroy();
        StatMachine::Destroy();

        if (0 != NDSingleton<RoleAttrHelper>::Get()->Load(attr_table,"role_level_attr.xlsx")) {
            WriteLog("load role attribute helper error ");
            ret = false;
            goto ERROR_EXIT;
        }

        NDSingleton<RoleAttrHelper>::Destroy();

        WriteLog("test role attribute success");

    } while (0);
    //test role attribute

    WriteLog("Run test success!!!!!!!!!!!\n");


ERROR_EXIT:

    DBLDatabase::destroy_Instant();
    return ret;

}


bool startDialog::showScriptError(const char *scriptFile, const char *nodeDescript)
{
	EditorFrame *pMain = new EditorFrame();
	pMain->setHostWidget(this);
	pMain->setAttribute(Qt::WA_DeleteOnClose, true);


	if (pMain->myInit()) {
		this->setVisible(false);

		if (!pMain->showRuntimeError(scriptFile, nodeDescript)) {
			return false;
		}
		pMain->showMaximized();
	}
	else {
		delete pMain;
		return false;
	}
	return true;
}

void startDialog::on_Exit_clicked()
{
    accept() ;
}

void startDialog::on_Connect_clicked()
{
    //ui->LogText->clear();
    dialogCloseHelper _helperClose(this) ;
	
    WriteLog("begin connect to server....");

    const char*filename = _getFromIocfg("gm_send_msg");
	//const char *dataTypeFile = _getFromIocfg("net_data_def");
	const char *client_script = _getFromIocfg("connect_script");
	//const char *message_file = _getFromIocfg("net_protocol");
	const char *package_file = _getFromIocfg("game_data_package_file");

	_LOAD_XML(xmlSend, filename, "utf8", 0);


    ConnectDialog dlg(NULL) ;
	
	//dlg.setWindowFlags(Qt::Popup);
	//dlg.setWindowFlags(Qt::Window);

    dlg.m_editor_setting =&m_editor_setting;
    dlg.m_gmCfg = &xmlSend;
	
	if (!dlg.LoadClientScript(client_script, package_file))	{
		//AfxMessageBox("load message data type error");
		QMessageBox::warning(this, "Error", "load message data type error!", QMessageBox::Ok);
	}

    dlg.exec();

	ndxml *msgRoot = ndxml_getnode(&xmlSend, "send_msg_list");
	if (msgRoot){
		ndxml_delnode(msgRoot, "history");
	}

    ndxml_save_encode(&xmlSend, filename, E_SRC_CODE_UTF_8, E_SRC_CODE_UTF_8);
    ndxml_destroy(&xmlSend) ;

    WriteLog("...\n connect server end!");
}

void startDialog::on_ScriptEdit_clicked()
{
	const char *toExeCmd = _getFromIocfg("compile_to_exe_cmd");

	EditorFrame *pMain = new EditorFrame();
	pMain->setHostWidget(this);
	pMain->setAttribute(Qt::WA_DeleteOnClose, true);
	if (toExeCmd) {
		pMain->SetCompileExeCmd(toExeCmd);
	}

	if (!loadDataBase()) {
		nd_logerror("load database error\n");
	}
	if (pMain->myInit()) {
		this->setVisible(false);
		pMain->showMaximized();
	}
	else {
		delete pMain;
	}

}

void startDialog::on_Compile_clicked()
{
    ClearLog();
    if (false == compile()){
        WriteLog("compile script error");
        return;
    }
    WriteLog("=========compile script success===========\n");

    const char *packaged_cmd = _getFromIocfg("compiled_rum_cmd");

    if (packaged_cmd && *packaged_cmd)	{
        int ret = system(packaged_cmd);

        if (0 != ret)	{
            nd_logerror("run command error %s %d\n", packaged_cmd, ret);
        }
    }

}

void startDialog::on_ExportExcel_clicked()
{
    ClearLog();

    if (false == expExcel()) {
        WriteLog("Export excel error");
        return;
    }
    WriteLog("==========excel export success==========\n");

    const char *packaged_cmd = _getFromIocfg("packeaged_rum_cmd");
    if (packaged_cmd && *packaged_cmd)	{
		char cmdbuf[1024];
		const char *p = packaged_cmd;
		while (p && *p) {
			cmdbuf[0] = 0;
			p = ndstr_nstr_end(p, cmdbuf, ';', sizeof(cmdbuf));
			if (p && *p == ';')	{
				++p;
			}

			if (cmdbuf[0])	{
				int ret = system(cmdbuf);
				if (0 != ret)	{
                    nd_logerror("Run command by system %s %d\n", cmdbuf, ret);
					break;
				}
			}
		}
    }
}

void startDialog::on_CompleteAll_clicked()
{
    ClearLog();
    if (false == compile()){
        WriteLog("compile script error");
        return;
    }
    if (false == expExcel()) {
        WriteLog("Export excel error");
        return;
    }
    WriteLog("==========one key complete success===========\n");
}

#include "workdirdialog.h"
void startDialog::on_WorkingPath_clicked()
{
	QString workPath;

	if (inputSetting(workPath, this)) {
		QMessageBox::warning(NULL, "Warning", "Please restart program!");
		exit(0);
	}
	else {
		QMessageBox::critical(NULL, "Warning", "Set work path error!");
	}
}
