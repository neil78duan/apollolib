#include "startdialog.h"
#include "ui_startdialog.h"
#include "connectdialog.h"
#include "nd_common/nd_common.h"
#include "nd_common/nd_iconv.h"
#include "apoScript/xmldialog.h"
#include "ndlib.h"

#include "game_parser/dbl_mgr.h"

#include "logic_parser/logic_compile.h"
#include "logic_parser/logicParser.h"
#include "logic_parser/logicEngineRoot.h"
#include "attribute/roleattr_help.h"
#include "attribute/stat_machine.h"
#include "ndapplib/ndsingleton.h"

#include "cli_common/dftCliMsgHandler.h"
#include "apoScript/apoEditorSetting.h"

#include "apoScript/editorFrame.h"
#include "logic_parser/logicApi4c.h"

#include <QMessageBox>

#include "ndlog_wrapper.h"
ND_LOG_WRAPPER_IMPLEMENTION(startDialog);

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
	//m_editor_setting(*(apoEditorSetting::getInstant()->getConfig())),
	//m_io_setting(*(apoEditorSetting::getInstant()->getIoConfig())),
	editorConfigFile(apoEditorSetting::getInstant()->getConfigFileName())
	//ioConfigFile(apoEditorSetting::getInstant()->m_projConfigFile)
{
    ui->setupUi(this);

	ND_LOG_WRAPPER_NEW(startDialog);
}

startDialog::~startDialog()
{
    delete ui;
	ND_LOG_WRAPPER_DELETE(startDialog);
}


void startDialog::ClearLog()
{
    ui->LogText->clear();

	ui->LogText->update();
}

void startDialog::WriteLog(const char *logText)
{
	QTextEdit *pEdit = ui->LogText;
	pEdit->moveCursor(QTextCursor::End);
	pEdit->insertPlainText(QString(logText));
	pEdit->moveCursor(QTextCursor::End, QTextCursor::KeepAnchor);

}


void startDialog::setProjectPath(const char *Path)
{
	if (Path) {
		if (nd_path_is_relative(Path)) {
			char buf[ND_FILE_PATH_SIZE];
			if (nd_absolute_path(Path, buf, sizeof(buf))) {
				m_projectPath = buf;
			}
		}
		else {

			m_projectPath = Path;
		}
	}
	else {
		m_projectPath = nd_getcwd();
	}
}

std::string startDialog::getPathFromProject(const char *relativePath)
{
	WorkingPathSwitchHelper __pathHelper(m_projectPath.c_str());
	char buf[ND_FILE_PATH_SIZE];
	if (!nd_absolute_filename(relativePath, buf, sizeof(buf))) {
		return std::string();
	}
	return buf;
}

std::string startDialog::getPathFromConfig(const char *configName)
{
	const char *cfgVal = _getFromIocfg(configName);
	if (!cfgVal) {
		return std::string();
	}
	return getPathFromProject(cfgVal);
}


const char *startDialog::getGameDateEncodeType()
{
    int codeType = atoi(_getFromIocfg("game_data_out_encode"));
    return nd_get_encode_name(codeType);
}


const char *startDialog::_getFromIocfg(const char *cfgName,const char *rootName)
{
	//return	apoEditorSetting::getInstant()->getValueFromSetting(cfgName);
	apoEditorSetting *setting = apoEditorSetting::getInstant();

	if (!rootName) {
		rootName ="project_config";
	}

	ndxml *node = ndxml_getnode(setting->getIoConfig(), rootName);
	if (node) {
		ndxml *cfgxml = ndxml_getnode(node, cfgName);
		if (cfgxml) {
			return ndxml_getval(cfgxml);
		}
	}
	return NULL;


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
	
	ndxml_root *pConfig = apoEditorSetting::getInstant()->getIoConfig();
	if (pConfig) {
		xmlDlg.showXml(pConfig, "EditorConfig");

		if (xmlDlg.exec() == QDialog::Accepted) {

			std::string &ioConfigFile = apoEditorSetting::getInstant()->m_projConfigFile ;
			//nd_chdir(nd_getcwd());
			ndxml_save(pConfig, ioConfigFile.c_str());
			WriteLog("save script ok\n");
		}
		else {
			WriteLog("script unedited\n");
		}

	}

    
}

bool startDialog::compile()
{
	std::string script_root = getPathFromConfig("script_root");
	
	/*
	if (!nd_absolute_filename(script_root.c_str(), tmpbuf, sizeof(tmpbuf))) {
		nd_logerror("can not found file %s\n", script_root.c_str());
		return false;
	}
	
	char tmpbuf[ND_FILE_PATH_SIZE];
	std::string absPath = tmpbuf;
	absPath = nd_getpath(absPath.c_str(), tmpbuf, sizeof(tmpbuf));
	*/

	ndxml_root xmlEntry;
	ndxml_initroot(&xmlEntry);
	if (-1 == ndxml_load_ex(script_root.c_str(), &xmlEntry, apoEditorSetting::getInstant()->m_encodeName.c_str())) {
		return false;
	}

	ndxml_root *xml = ndxml_getnode(&xmlEntry, "script_file_manager");
	if (!xml){
		return false;
	}
	

	char projPath[ND_FILE_PATH_SIZE];
	nd_getpath(script_root.c_str(), projPath, sizeof(projPath));

	WorkingPathSwitchHelper __pathHelper(projPath);
	
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

		if (!compileScript(scriptPath.c_str(),__pathHelper.workingPath())) {
			ret = false;
			break;
		}
	}

	ndxml_destroy(&xmlEntry);

	return true;
}

bool startDialog::compileScript(const char *scriptFile, const char *editorWorkingPath)
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
	const char *orderName = apoEditorSetting::getInstant()->getValueFromSetting("bin_data_byte_order");
	if (orderName) {
		orderType = atoi(orderName);
	}

	LogicCompiler &lgcompile= *LogicCompiler::get_Instant();

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
	
	LogicEngineRoot *scriptRoot = LogicEngineRoot::get_Instant();
	nd_assert(scriptRoot);
	scriptRoot->setOutPutEncode(E_SRC_CODE_UTF_8);

	scriptRoot->setPrint(ND_LOG_WRAPPER_PRINT(startDialog), NULL);
	scriptRoot->getGlobalParser().setSimulate(true);
	//scriptRoot->getGlobalParser().setOwner(&apoOwner);

	if (0 != scriptRoot->LoadScript(outFile, &scriptRoot->getGlobalParser())){
		WriteLog("load script error n");
		LogicEngineRoot::destroy_Instant();
		return false;
	}

	WriteLog("start run script...\n");
	scriptRoot->setDftScriptModule(scriptRoot->getMainModuleName());
	if (0 != scriptRoot->test()){
		WriteLog("run script error\n");

		const char *curErrNode = scriptRoot->getGlobalParser().getLastErrorNode();
		if (curErrNode && *curErrNode) {
			showScriptError(scriptFile, curErrNode,editorWorkingPath);
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
	
	std::string stdstr_excel_path = _getFromIocfg("excel_data_in_path");
	std::string stdstr_text_path = _getFromIocfg("text_data_out_path");
	std::string stdstr_package_file = _getFromIocfg("game_data_package_file");
	std::string stdstr_excel_list = _getFromIocfg("game_data_listfile");

	const char *excel_path = stdstr_excel_path.c_str();
	const char *text_path = stdstr_text_path.c_str();
	const char *package_file = stdstr_package_file.c_str();
	const char *excel_list = stdstr_excel_list.c_str(); 

    char exp_cmdbuf[1024];

	if (!exp_cmd || !excel_path || !text_path || !package_file){
		WriteLog("export excel error : on read config file\n");
		return false;
	}

	WorkingPathSwitchHelper __pathHelper(m_projectPath.c_str());


	const char *encodeName = getGameDateEncodeType();
	const char *inputCode = encodeName;
#ifdef WIN32
	snprintf(exp_cmdbuf, sizeof(exp_cmdbuf), " %s %s %s %s %s ", exp_cmd, excel_list, excel_path, text_path, encodeName);
#else
	snprintf(exp_cmdbuf, sizeof(exp_cmdbuf), "sh %s %s %s %s %s ", exp_cmd, excel_list, excel_path, text_path, encodeName);
#endif
	
	int ret = ::system(exp_cmdbuf);	
    if (0 != ret)	{
		nd_logerror("[%s] run export shell %s\nerror: %s \n",nd_getcwd(), exp_cmdbuf,nd_last_error());
        return false;
	}

    WriteLog("===============export excel to text file  success =========");


	//get byte order  
	int orderType = ND_L_ENDIAN;
	const char *orderName = apoEditorSetting::getInstant()->getValueFromSetting("bin_data_byte_order");
	if (orderName) {
		orderType = atoi(orderName);
	}

	//DUMP FOR WINDOWS only
#ifdef WIN32
	do 	{
		std::string strWinPack = package_file;
		strWinPack += ".gbk";

		DBLDatabase dbwin;
		if (0 != dbwin.LoadFromText(text_path, excel_list, inputCode, "gbk")) {
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
	if (0 != dbtmp.LoadFromText(text_path, excel_list, inputCode, encodeName)) {
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

	//const char *package_file = _getFromIocfg("game_data_package_file");
	std::string package_file = getPathFromConfig("game_data_package_file");

	DBLDatabase *pdbl = DBLDatabase::get_Instant();
	if (pdbl){
		if (0 == pdbl->LoadBinStream(package_file.c_str())) {
			return true;
		}
	}
	return false;
}

bool startDialog::expLua(const char *outPath, const DBLDatabase &db)
{
	nd_mkdir(outPath);
	bool outflag = LogicDataObj::setOutLua(true);
	if (-1 == db.OutputLua(outPath)) {
		LogicDataObj::setOutLua(outflag);
		return false;
	}
	LogicDataObj::setOutLua(outflag);
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


bool startDialog::showScriptError(const char *scriptFile, const char *nodeDescript, const char *editorWorkingPath)
{
	nd_chdir(editorWorkingPath);

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

    std::string filename = getPathFromConfig("gm_send_msg");
	std::string client_script = getPathFromConfig("connect_script");
	std::string package_file = getPathFromConfig("game_data_package_file");

	_LOAD_XML(xmlSend, filename.c_str(), "utf8", 0);


    ConnectDialog dlg(NULL) ;
	
    dlg.m_editor_setting = apoEditorSetting::getInstant()->getConfig() ;
    dlg.m_gmCfg = &xmlSend;
	
	if (!dlg.LoadClientScript(client_script.c_str(), package_file.c_str()))	{
		QMessageBox::warning(this, "Error", "load message data type error!", QMessageBox::Ok);
	}

    dlg.exec();

	ndxml *msgRoot = ndxml_getnode(&xmlSend, "send_msg_list");
	if (msgRoot){
		ndxml_delnode(msgRoot, "history");
	}

    ndxml_save_encode(&xmlSend, filename.c_str(), E_SRC_CODE_UTF_8, E_SRC_CODE_UTF_8);
    ndxml_destroy(&xmlSend) ;

    WriteLog("...\n connect server end!");
}

void startDialog::on_ScriptEdit_clicked()
{
	EditorFrame *pMain = new EditorFrame();
	pMain->setHostWidget(this);
	pMain->setAttribute(Qt::WA_DeleteOnClose, true);

	if (!loadDataBase()) {
		nd_logerror("load database error\n");
	}

	std::string defProj = getPathFromConfig("script_root");

	if (!defProj.empty() && pMain->myInit(defProj.c_str() )) {
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

	if (!loadDataBase()) {
		nd_logerror("load database error\n");
	}

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

#include "apoScript/workdirdialog.h"
void startDialog::on_WorkingPath_clicked()
{
	QString workPath;

	if (inputSetting(workPath, this)) {
		QMessageBox::warning(NULL, "Warning", "Please restart program!");
		exit(0);
	}
	else {
		//QMessageBox::critical(NULL, "Warning", "Set work path error!");
	}
}
