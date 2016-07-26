#include "startdialog.h"
#include "ui_startdialog.h"
#include "connectdialog.h"
#include "nd_common/nd_common.h"
#include "xmldialog.h"
#include "dbl_mgr.h"
#include "newfiledialog.h"
#include "ndlib.h"

#include "logic_compile.h"
#include "logicParser.h"
#include "logicEngineRoot.h"
#include "roleattr_help.h"
#include "stat_machine.h"
#include "dbl_mgr.h"
#include "ndapplib/ndsingleton.h"

#include <QMessageBox>
static startDialog *__g_loginDlg ;
static void *__oldFunc ;


#define _LOAD_XML(_xml_name, _filename,_encode,_isexit) \
    ndxml_root _xml_name;				\
    ndxml_initroot(&_xml_name);			\
    if (ndxml_load_ex((char*)_filename, &_xml_name,_encode)) {	\
        nd_logerror("open file %s error", _filename);	\
        if(_isexit) return;							\
    }

static void out_log(const char *text)
{
    if (__g_loginDlg){
        __g_loginDlg->WriteLog(text);
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

startDialog::startDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::startDialog)
{
    ui->setupUi(this);

    ndxml_initroot(&m_editor_setting);
    ndxml_initroot(&m_io_setting);

    __g_loginDlg = this ;
    __oldFunc = ndSetLogoutFunc((void*)out_log);

	nd_log_no_file(1);
	nd_log_no_time(1);
}

startDialog::~startDialog()
{
    delete ui;

    ndSetLogoutFunc((void*)__oldFunc);
    __g_loginDlg = NULL;
}

bool startDialog::initXmlSetting(const char *editorCfg, const char *ioCfg)
{
	if (0 != ndxml_load_ex(editorCfg, &m_editor_setting, "utf8")){
        nd_logerror("load editor setting error \n") ;
        return false;
    }
	if (0 != ndxml_load_ex(ioCfg, &m_io_setting, "utf8")){
        nd_logerror("load io setting error \n") ;

        return false;
    }
    editorConfigFile = editorCfg;
    ioConfigFile = ioCfg;
    return true ;
}

void startDialog::ClearLog()
{
    ui->LogText->clear();
}

void startDialog::WriteLog(const char *logText)
{
#ifdef WIN32
	char buf[1024];
	nd_gbk_to_utf8(logText, buf, sizeof(buf));
	logText = buf ;
#endif

	QTextCursor cursor(ui->LogText->textCursor());//
	cursor.movePosition(QTextCursor::End);
	ui->LogText->insertPlainText(QString(logText));
}


const char *startDialog::getNetProtocol()
{
    return _getFromIocfg("net_protocol");
}
const char *startDialog::getGameDataInPath()
{
    return _getFromIocfg("game_data_path");
}
const char *startDialog::getGameDataList()
{
    return _getFromIocfg("game_data_listfile");
}
const char *startDialog::getGameDataOutfile()
{
    return _getFromIocfg("game_data_outfile");
}

const char *startDialog::getExcelExportEncodeType()
{
    int codeType = atoi( _getFromIocfg("game_data_in_encode") );
    return nd_get_encode_name(codeType);
}
const char *startDialog::getGameDateEncodeType()
{
    int codeType = atoi(_getFromIocfg("game_data_out_encode"));
    return nd_get_encode_name(codeType);
}


const char *startDialog::_getFromIocfg(const char *cfgName)
{
    ndxml *node = ndxml_getnode(&m_io_setting, "setting_config");
    if (node){
        ndxml *cfgxml = ndxml_getnode(node, cfgName);
        if (cfgxml)	{
            return ndxml_getval(cfgxml);
        }
    }
    return NULL;
}

void startDialog::_beginEdit(const char *script_file, const char *title)
{
	const char *filename;
	ClearLog();

	WriteLog("begin editor.....\n");
	//filename = getGameScript();

	filename = script_file;
	_LOAD_XML(xml_script, filename, "utf8", 1);

	filename = getScriptSetting(&xml_script, "c_func_api");
	_LOAD_XML(xml_cpp_func, filename, "utf8", 0);

	filename = getScriptSetting(&xml_script, "events_id");
	_LOAD_XML(xml_events_id, filename, "utf8", 0);


	filename = getNetProtocol();
	_LOAD_XML(xml_net_protocol, filename, "utf8", 0);

	XMLDialog xmlDlg(this);
	xmlDlg.SetXML(&m_editor_setting, &xml_script,title);
	//xmlDlg.SetMessageID(&xml_net_protocol);
	//xmlDlg.SetEventsID(&xml_events_id);

	ndxml *funcroot = ndxml_getnode(&xml_net_protocol, "MessageDefine");
	if (funcroot) {
		char buf[256];
		text_list_t messageList;
		for (int i = 0; i < ndxml_num(funcroot); i++){
			ndxml *fnode = ndxml_getnodei(funcroot, i);
			const char *pDispname = ndxml_getattr_val(fnode, "comment");
			const char *pRealVal = ndxml_getattr_val(fnode, "id");
			const char *p = buildDisplaNameValStr(pRealVal, pDispname, buf, sizeof(buf));
			messageList.push_back(QString(p));
		}
		xmlDlg.addDisplayNameList("msg_list", messageList);
	}
	xmlDlg.loadUserdefDisplayList(xml_cpp_func, LOGIC_FUNCTION_LIST_NAME);
	xmlDlg.loadUserdefDisplayList(xml_events_id, LOGIC_EVENT_LIST_NAME);

	DBLDatabase::get_Instant()->LoadBinStream(getGameDataOutfile());

	if (xmlDlg.exec() == QDialog::Accepted) {
		nd_chdir(nd_getcwd());
		ndxml_save(&xml_script, script_file);
		WriteLog("save script ok\n");
	}
	else {
		WriteLog("script unedited\n");
	}

	ndxml_destroy(&xml_script);
	ndxml_destroy(&xml_cpp_func);
	ndxml_destroy(&xml_net_protocol);
	ndxml_destroy(&xml_events_id);
	DBLDatabase::destroy_Instant();
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
    XMLDialog xmlDlg(this);
    xmlDlg.SetXML(&m_editor_setting, &m_io_setting,"EditorConfig");

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
	if (-1 == ndxml_load_ex(script_root, &xmlEntry,nd_get_encode_name(ND_ENCODE_TYPE))) {
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
		if (!compileScript(ndxml_getval(node))) {
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
	if (-1 == ndxml_load_ex(scriptFile, &xmlScript, nd_get_encode_name(ND_ENCODE_TYPE))) {
		return false;
	}
	const char*inFile = scriptFile;

	std::string outFile = getScriptSetting(&xmlScript, "out_file");
	int outEncode = getScriptExpEncodeType(&xmlScript);
	bool withDebug = getScriptExpDebugInfo(&xmlScript);
	ndxml_destroy(&xmlScript);

	LogicCompiler lgcompile;

	lgcompile.setConfigFile(CONFIG_FILE_PATH);
	if (!lgcompile.compileXml(inFile, outFile.c_str(), outEncode, withDebug)) {

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

	WriteLog("!!!!!!!!!!COMPILE script success !!!!!!!!!!!\n begining run script...\n");

	LogicEngineRoot *scriptRoot = LogicEngineRoot::get_Instant();
	nd_assert(scriptRoot);

	scriptRoot->setPrint(out_print, NULL);
	scriptRoot->getGlobalParser().setSimulate(true);

	if (0 != scriptRoot->LoadScript(outFile.c_str())){
		WriteLog("load script error n");
		LogicEngineRoot::destroy_Instant();
		return false;
	}

	WriteLog("start run script...\n");
	if (0 != scriptRoot->test()){
		WriteLog("run script error\n");
		LogicEngineRoot::destroy_Instant();
		return false;
	}

	LogicEngineRoot::destroy_Instant();
	WriteLog("!!!!!!!!!!!!!!!!!!!SCRIPT COMPILE SUCCESS!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
	return true;
}
// bool startDialog::compileScript()
// {
//     LogicCompiler lgcompile;
// 
//     int outEncode = E_SRC_CODE_UTF_8;
//     bool withDebug = false;
//     const char *p = _getFromIocfg("script_with_debug");
//     if (p && *p != '0'){
//         withDebug = true;
//     }
// 
//     p = _getFromIocfg("script_out_encode");
//     if (p && *p){
//         outEncode = atoi(p);
//     }
// 
//     lgcompile.setConfigFile(editorConfigFile);
//     if (!lgcompile.compileXml(getGameScript(), getGameScriptOut(), outEncode, withDebug)) {
// 
//         const char *pFunc = lgcompile.m_cur_function.c_str();
//         const char *pStep = lgcompile.m_cur_step.c_str();
// 
//         nd_logerror("compile error file %s, function %s, step %s , stepindex %d\n",
//             lgcompile.m_cur_file.c_str(), pFunc, pStep, lgcompile.m_cur_node_index);
// 
//         return false ;
//     }
// 
//     WriteLog("!!!!!!!!!!COMPILE script success !!!!!!!!!!!\n begining run script...\n");
// 
//     LogicEngineRoot *scriptRoot = LogicEngineRoot::get_Instant();
//     nd_assert(scriptRoot);
// 
//     scriptRoot->setPrint(out_print, NULL);
// 
//     if (0 != scriptRoot->LoadScript(getGameScriptOut())){
//         WriteLog("load script error n");
//         LogicEngineRoot::destroy_Instant();
//         return false;
//     }
// 
//     WriteLog("start run script...\n");
//     if (0 != scriptRoot->test()){
//         WriteLog("run script error\n");
//         LogicEngineRoot::destroy_Instant();
//         return false;
//     }
// 
//     LogicEngineRoot::destroy_Instant();
//     WriteLog("!!!!!!!!!!!!!!!!!!!SCRIPT COMPILE SUCCESS!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
//     return true;
// }

#include <QProcess>
bool startDialog::expExcel()
{
    char org_dir[1024];
    char path[1024];

    WriteLog("==============Begin export excel===============");

    const char *exp_cmd = _getFromIocfg("game_data_export_cmd");
    if (!exp_cmd){
        WriteLog("get export shell error");
        return false ;
    }
    const char *cur_dir = nd_getcwd();
    strncpy(org_dir, cur_dir, sizeof(org_dir));

    nd_getpath(exp_cmd, path, sizeof(path));
    if (0 != nd_chdir(path)) {
        nd_logerror("change workding path error: %s", nd_last_error());
        return false ;
    }
    const char *encodeName = getExcelExportEncodeType();

#ifdef WIN32
	snprintf(path, sizeof(path), "cmd.exe /c %s %s", exp_cmd, encodeName);
#else 
	snprintf(path, sizeof(path), "sh  %s %s", exp_cmd, encodeName);
#endif

	int ret = system(path);


    if (0 != ret)	{
		nd_logerror("run export shell %s\nerror: %s \n", path,nd_last_error());
		nd_chdir(org_dir);
        return false;
	}
	nd_chdir(org_dir);

    WriteLog("===============export excel to text file  success =========");

    DBLDatabase dbtmp;
    if (0 != dbtmp.LoadFromText(getGameDataInPath(), getGameDataList(), getExcelExportEncodeType(), getGameDateEncodeType())) {
        nd_logerror("load data from text file error");
        return false;
    }
    if (0 == dbtmp.Dump(getGameDataOutfile(), "gamedatadb")) {
        nd_logmsg("package game data SUCCESS!!\n");
        WriteLog("==========write excel to bin-stream success ===========\n");
    }
    else{
        nd_logerror("write excel to bin-stream FAILED");
    }

    //before run test need load dbl
    DBLDatabase *pdbl = DBLDatabase::get_Instant();
    if (pdbl){
        if (0 != pdbl->LoadBinStream(getGameDataOutfile())) {
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

    dbtmp.Destroy();

    std::string strOutTextPath = getGameDataInPath();
    strOutTextPath  += "/test_outputData";
    if (0 != pdbl->TestOutput(strOutTextPath.c_str())) {
        nd_logmsg("dump from bin-stream to text file error\n");
        DBLDatabase::destroy_Instant();
        return false;

    }

    DBLDatabase::destroy_Instant();
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

    if (0!= DBLDatabase::get_Instant()->LoadBinStream(getGameDataOutfile())) {
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
	const char *dataTypeFile = _getFromIocfg("net_data_def");
	const char *client_script = _getFromIocfg("connect_script");
	const char *message_file = _getFromIocfg("net_protocol");

	_LOAD_XML(xmlSend, filename, "utf8", 0);


    ConnectDialog dlg(this) ;

    dlg.m_editor_setting =&m_editor_setting;
    dlg.m_gmCfg = &xmlSend;
	
	if (!dlg.LoadDataDef(dataTypeFile, client_script, message_file))	{
		//AfxMessageBox("load message data type error");
		QMessageBox::warning(this, "Error", "load message data type error!", QMessageBox::Ok);
	}

    dlg.exec();

    ndxml_save_encode(&xmlSend, filename, E_SRC_CODE_UTF_8, E_SRC_CODE_UTF_8);
    ndxml_destroy(&xmlSend) ;

    WriteLog("...\n connect server end!");
}

void startDialog::on_ScriptEdit_clicked()
{
	NewFileDialog dlg(this);
	const char *scriptRoot = _getFromIocfg("script_root");
	const char *newTmplFile = _getFromIocfg("new_template");

	if (!dlg.InitFileRoot(scriptRoot, newTmplFile)) {
		QMessageBox::warning(NULL, "Error", "Init script-edit config error!", QMessageBox::Ok);
		return;
	}
	if (dlg.exec() == QDialog::Accepted) {
		const char *file = dlg.getSelectFile();
		const char *title = dlg.getSelectTitle();
		if (file && *file)	{
			_beginEdit(file,title);
		}
		else {
			//QMessageBox::warning(NULL, "Error", "Please select the file you need edit!", QMessageBox::Ok);
			//AfxMessageBox(_T("请选择要编辑的文件!"));
		}

	}
	else {
		WriteLog("script unedited\n");
	}

// 	dlg.InitFileRoot(scriptRoot, newTmplFile);
// 	if (IDOK == dlg.DoModal()){
// 		const char *file = dlg.getSelectFile();
// 		if (file && *file)	{
// 			_beginEdit(file);
// 		}
// 		else {
// 			AfxMessageBox(_T("请选择要编辑的文件!"));
// 		}
// 	
	/*
    dialogCloseHelper _helperClose(this) ;
    const char *filename;
    const char *script_file;
    ClearLog();

    WriteLog("Begin edit script...");
    filename = getGameScript();
    script_file = filename;
    _LOAD_XML(xml_script, filename,"utf8",1);

    filename = getCppFuncList();
    _LOAD_XML(xml_cpp_func, filename, "utf8",0);

    filename = getNetProtocol();
    _LOAD_XML(xml_net_protocol, filename, "utf8",0);

    filename = getEventList();
    _LOAD_XML(xml_events_id, filename, "utf8",0);


    XMLDialog xmlDlg(this);
    xmlDlg.SetXML(&m_editor_setting, &xml_script);
    //xmlDlg.SetMessageID(&xml_net_protocol);
    //xmlDlg.SetEventsID(&xml_events_id);

	ndxml *funcroot = ndxml_getnode(&xml_net_protocol, "MessageDefine");
	if (funcroot) {
		char buf[256];
		text_list_t messageList;
		for (int i = 0; i < ndxml_num(funcroot); i++){
			ndxml *fnode = ndxml_getnodei(funcroot, i);
			const char *pDispname = ndxml_getattr_val(fnode, "comment");
			const char *pRealVal = ndxml_getattr_val(fnode, "id");
			const char *p = buildDisplaNameValStr(pRealVal, pDispname, buf, sizeof(buf));
			messageList.push_back(QString(p));
		}
		xmlDlg.addDisplayNameList("msg_list", messageList);
	}
	xmlDlg.loadUserdefDisplayList(xml_cpp_func, LOGIC_FUNCTION_LIST_NAME);
	xmlDlg.loadUserdefDisplayList(xml_events_id, LOGIC_EVENT_LIST_NAME);

    DBLDatabase::get_Instant()->LoadBinStream(getGameDataOutfile());

    if (xmlDlg.exec() == QDialog::Accepted) {
        nd_chdir(nd_getcwd());
        ndxml_save(&xml_script, script_file);
        WriteLog("save script ok\n");
    }
    else {
        WriteLog("script unedited\n");
    }
    ndxml_destroy(&xml_script);
    ndxml_destroy(&xml_cpp_func);
    ndxml_destroy(&xml_net_protocol);
    ndxml_destroy(&xml_events_id);
    DBLDatabase::destroy_Instant();
	*/
}

void startDialog::on_Compile_clicked()
{
    ClearLog();
    if (false == compile()){
        WriteLog("compile script error");
        return;
    }
    WriteLog("=========compile script success===========\n");
}

void startDialog::on_ExportExcel_clicked()
{
    ClearLog();

    if (false == expExcel()) {
        WriteLog("Export excel error");
        return;
    }
    WriteLog("==========excel export success==========\n");
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

void startDialog::on_Test_clicked()
{

    ClearLog();
    if (false == runTest()){
        WriteLog("Run test error!!!!!!!!!");
    }
}
