#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "apoScrpit/apoUiMainEditor.h"
#include "apoScrpit/apoXmlTreeView.h"
#include "apoScrpit/apoBaseExeNode.h"

#include "logic_parser/dbl_mgr.h"
#include "logic_parser/logic_compile.h"
#include "logic_parser/logicEngineRoot.h"
#include "logic_parser/logic_editor_helper.h"

#include "cli_common/dftCliMsgHandler.h"


#include "nd_common/nd_common.h"
#include "ndlib.h"

#include "ndlog_wrapper.h"
ND_LOG_WRAPPER_IMPLEMENTION(MainWindow, __glogWrapper);


#include <QTextEdit>
#include <QDockWidget>
#include <QTableWidget>
#include <QApplication>


MainWindow::MainWindow(QWidget *parent) :
QMainWindow(parent), m_editorSetting(), m_editorWindow(0),
	m_fileRoot(0), m_curFile(0), m_currFunction(0), 
    ui(new Ui::MainWindow)
{
	m_isChangedCurFile = false;
    ui->setupUi(this);

    setWindowTitle(tr("main editor"));
	__glogWrapper = ND_LOG_WRAPPER_NEW(MainWindow);
}

MainWindow::~MainWindow()
{
	closeCurFile();
	if (m_fileRoot){
		ndxml_save(m_fileRoot, m_fileRootPath.c_str());
		ndxml_destroy(m_fileRoot);
		m_fileRoot = 0;
	}
    delete ui;

	ND_LOG_WRAPPER_DELETE(__glogWrapper);
}


void MainWindow::ClearLog()
{
	//ui->LogText->clear();
	QDockWidget *pDock = this->findChild<QDockWidget*>("outputView");
	if (pDock){
		QTextEdit *pEdit = pDock->findChild<QTextEdit*>("logTextEdit");
		if (pEdit)	{
			pEdit->clear();
		}
	}
	m_logText.clear();
}

void MainWindow::WriteLog(const char *logText)
{
// #ifdef WIN32
// 	char buf[1024];
// 	nd_gbk_to_utf8(logText, buf, sizeof(buf));
// 	logText = buf;
// #endif
	
	QDockWidget *pDock = this->findChild<QDockWidget*>("outputView");
	if (pDock){
		QTextEdit *pEdit = pDock->findChild<QTextEdit*>("logTextEdit");
		if (pEdit)	{
			pEdit->append(QString(logText));
			return;
// 			QTextCursor cursor(pEdit->textCursor());
// 			cursor.movePosition(QTextCursor::End);
// 			pEdit->insertPlainText(QString(logText));
// 			return;				 
		}
	}
	m_logText.append(QString(logText));	
}

void MainWindow::showAllView()
{
	openFileView();
	openFunctionsView();
	openOutputView();
	//openDetailView();
	return;
}
//#include "newfiledialog.h"

bool MainWindow::openFileView()
{
	QDockWidget *pDock = this->findChild<QDockWidget*>("FilelistView");
	if (pDock)	{
		return false;
	}

	pDock = new QDockWidget(tr("Files"), this);
	pDock->setObjectName("FilelistView");
	pDock->setAttribute(Qt::WA_DeleteOnClose, true);

	apoXmlTreeView *subwindow = new apoXmlTreeView();
	subwindow->setAttribute(Qt::WA_DeleteOnClose, true);

	QObject::connect(subwindow, SIGNAL(itemDoubleClicked(QTreeWidgetItem *, int)),
		this, SLOT(onFilesTreeItemDBClicked(QTreeWidgetItem *, int)));

	if (m_fileRoot)	{
		//subwindow->setComfig(m_editorSetting.getConfig());
		subwindow->setAlias(&m_editorSetting.getAlias());
		subwindow->setXmlInfo(m_fileRoot, 2);
	}

	pDock->setWidget(subwindow);
	addDockWidget(Qt::LeftDockWidgetArea, pDock);
	return true;

}
bool MainWindow::openFunctionsView()
{

	QDockWidget *pDock = this->findChild<QDockWidget*>("FunctionView");
	if (pDock){
		return false;
	}
	else {
		pDock = new QDockWidget(tr("Functions"), this);
		pDock->setObjectName("FunctionView");
		pDock->setAttribute(Qt::WA_DeleteOnClose, true);

		//function tree list
		apoXmlTreeView *subwindow = new apoXmlTreeView();
		subwindow->setAttribute(Qt::WA_DeleteOnClose, true);
		subwindow->setObjectName("functionsTree");

		//mask functon create
		subwindow->unCreateNewChild("func_node");
		subwindow->unCreateNewChild("msg_handler_node"); 
		subwindow->unCreateNewChild("event_callback_node");


		QObject::connect(subwindow, SIGNAL(itemDoubleClicked(QTreeWidgetItem *, int)),
			this, SLOT(onFunctionsTreeItemDBClicked(QTreeWidgetItem *, int)));

		QObject::connect(subwindow, SIGNAL(xmlNodeDelSignal(ndxml *)),
			this, SLOT(onXmlNodeDel(ndxml *)));

		QObject::connect(subwindow, SIGNAL(xmlDataChangedSignal()),this, SLOT(onFileChanged()));

		if (m_curFile)	{
			//subwindow->setComfig(m_editorSetting.getConfig());

			subwindow->setAlias(&m_editorSetting.getAlias());
			subwindow->setXmlInfo(m_curFile, 5);
		}

		pDock->setWidget(subwindow);
		addDockWidget(Qt::LeftDockWidgetArea, pDock);
	}
	return true;
}
bool MainWindow::openOutputView()
{
	QDockWidget *pDock = this->findChild<QDockWidget*>("outputView");
	if (pDock){
		return false;
	}
	pDock = new QDockWidget(tr("Output"), this);
	pDock->setObjectName("outputView");
	pDock->setAttribute(Qt::WA_DeleteOnClose, true);

	QTextEdit *subwindow = new QTextEdit();

	subwindow->setAttribute(Qt::WA_DeleteOnClose, true);
	subwindow->setObjectName("logTextEdit");
	subwindow->setPlainText(m_logText);

	pDock->setWidget(subwindow);
	addDockWidget(Qt::BottomDockWidgetArea, pDock);
	return true;
}

bool MainWindow::openDetailView()
{
	QDockWidget *pDock = this->findChild<QDockWidget*>("DetailView");
	if (pDock)	{
		return false;
	}

	pDock = new QDockWidget(tr("Detail"), this);
	pDock->setObjectName("DetailView");
	pDock->setAttribute(Qt::WA_DeleteOnClose, true);

	apoUiDetailView *subwindow = new apoUiDetailView();
	subwindow->setAttribute(Qt::WA_DeleteOnClose, true);
	subwindow->setObjectName("xmlDetailTable");

	
	pDock->setWidget(subwindow);
	addDockWidget(Qt::RightDockWidgetArea, pDock);
	return true;
}


// 
// 
// QWidget *MainWindow::getMainEditor()
// {
// 	return m_editorWindow;
// }


const char *MainWindow::getScriptSetting(ndxml *scriptXml, const char *settingName)
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

bool MainWindow::loadScriptFile(const char *scriptFile)
{
	if (!scriptFile)	{
		return false;
	}
	if (checkNeedSave()){
		saveCurFile();
		ndxml_destroy(m_curFile);
		delete m_curFile;
	}

	MY_LOAD_XML_AND_NEW(m_curFile, scriptFile, return true);

	//load user define enum 
	common_export_error_list("./.error_list.xml");
	const char *filename = getScriptSetting(m_curFile, "user_define_enum");
	if (filename){
		m_editorSetting.loadUserDefEnum(filename);
		m_editorSetting.loadMessageDefine(m_messageFile.c_str()) ;
	}

	m_currFunction = NULL;
	if (!openFunctionsView()) {
		showCurFile();
	}
	m_filePath = scriptFile;
	return true;
}
bool MainWindow::showCurFile()
{
	QDockWidget *pDock = this->findChild<QDockWidget*>("FunctionView");
	if (!pDock){
		return false;
	}

	apoXmlTreeView *tree = this->findChild<apoXmlTreeView*>("functionsTree");
	if (!tree)	{
		return false;
	}
	tree->clear();
	tree->setXmlInfo(m_curFile, 5);
	return true;
}


void MainWindow::onFileChanged()
{
	m_isChangedCurFile = true;
}

void MainWindow::onXmlNodeDel(ndxml *xmlnode)
{
	m_isChangedCurFile = true;
	if (xmlnode == m_currFunction || ndxml_get_parent(m_currFunction)==xmlnode)	{
		if (m_editorWindow)	{
			m_editorWindow->close();
			m_editorWindow = 0;
		}
		m_currFunction = NULL;
	}
}

bool MainWindow::showCurFunctions()
{
	if (m_editorWindow)	{
		m_editorWindow->close();
	}

	m_editorWindow = new apoUiMainEditor(this);
	m_editorWindow->setAttribute(Qt::WA_DeleteOnClose, true);

	QObject::connect(m_editorWindow, SIGNAL(showExenodeSignal(apoBaseExeNode*)),
		this, SLOT(onShowExeNodeDetail(apoBaseExeNode *)));
	
	setCentralWidget(m_editorWindow);

	m_editorWindow->showFunction(m_currFunction, m_curFile);
	return false;
}

bool MainWindow::saveCurFile()
{ 
	m_isChangedCurFile = false;
	if (m_curFile){
		ndxml_save(m_curFile, m_filePath.c_str());
		nd_logmsg("save %s script ok\n", m_filePath.c_str());
		return true;
	}
	return false;
}

void MainWindow::closeCurFile()
{
	if (m_curFile) {
		ndxml_destroy(m_curFile);
		delete m_curFile;
	}
	m_isChangedCurFile = false;
}

bool MainWindow::checkNeedSave()
{
	return m_isChangedCurFile && m_curFile;
}

bool MainWindow::showCommonDeatil(ndxml *xmldata)
{
	QDockWidget *pDock = this->findChild<QDockWidget*>("DetailView");
	if (pDock)	{
		QWidget *w = pDock->findChild<QWidget*>("xmlDetailTable");
		if (w)	{
			w->close();
		}
	}
	else {
		pDock = new QDockWidget(tr("Detail"), this);
		pDock->setObjectName("DetailView");
		pDock->setAttribute(Qt::WA_DeleteOnClose, true);
	}

	apoUiCommonXmlView *subwindow = new apoUiCommonXmlView();
	subwindow->setAttribute(Qt::WA_DeleteOnClose, true);
	subwindow->setObjectName("xmlDetailTable");

	subwindow->showDetail(xmldata, m_curFile);

	QObject::connect(subwindow, SIGNAL(xmlDataChanged()),
		this, SLOT(onFileChanged()));

	pDock->setWidget(subwindow);
	addDockWidget(Qt::RightDockWidgetArea, pDock);
	
	return true;
}

void MainWindow::onShowExeNodeDetail(apoBaseExeNode *exenode)
{
	QDockWidget *pDock = this->findChild<QDockWidget*>("DetailView");
	if (pDock)	{
		QWidget *w = pDock->findChild<QWidget*>("xmlDetailTable");
		if (w)	{
			w->close();
		}
	}
	else {
		pDock = new QDockWidget(tr("Detail"), this);
		pDock->setObjectName("DetailView");
		pDock->setAttribute(Qt::WA_DeleteOnClose, true);
	}

	apoUiDetailView *subwindow = new apoUiDetailView();
	subwindow->setAttribute(Qt::WA_DeleteOnClose, true);
	subwindow->setObjectName("xmlDetailTable");
	subwindow->showDetail(exenode, m_curFile);

	QObject::connect(subwindow, SIGNAL(xmlDataChanged()),
		this, SLOT(onFileChanged()));

	if (m_editorWindow) {
		QObject::connect(subwindow, SIGNAL(xmlDataChanged()),
			m_editorWindow, SLOT(onCurNodeChanged()));
	}


	pDock->setWidget(subwindow);

	addDockWidget(Qt::RightDockWidgetArea, pDock);

}

//double click function list
void MainWindow::onFunctionsTreeItemDBClicked(QTreeWidgetItem *item, int column)
{
	xmlTreeItem *pXmlItem = dynamic_cast<xmlTreeItem*>(item);
	if (!pXmlItem)	{
		return;
	}
	ndxml *pxml = (ndxml *)pXmlItem->getUserData();
	if (!pxml)	{
		return;
	}

	const char *pAction = m_editorSetting.getDisplayAction( ndxml_getname(pxml) );
    if(!pAction) {
        return ;
    }

	if (0 == ndstricmp(pAction, "function"))	{
		m_currFunction = pxml;
		showCurFunctions();

	}
	else if (0 == ndstricmp(pAction, "detail")) {
		showCommonDeatil(pxml);
		if (m_editorWindow) {
			m_editorWindow->curDetailChanged(NULL);
		}
	}
}

// double click file list
void MainWindow::onFilesTreeItemDBClicked(QTreeWidgetItem *item, int column)
{
	xmlTreeItem *pXmlItem = dynamic_cast<xmlTreeItem*>(item);
	if (!pXmlItem)	{
		return;
	}
	ndxml *pxml = (ndxml *)pXmlItem->getUserData();
	if (!pxml)	{
		return;
	}
	if (loadScriptFile(ndxml_getval(pxml))) {
		setDefaultFile(ndxml_getval(pxml));
	}
}

//////////////////////////////////////////////////////////////////////////


bool MainWindow::setConfig(const char *cfgFile, const char *messageFile)
{
	m_messageFile = messageFile;
	m_confgiFile = cfgFile;
	if (m_editorSetting.setConfigFile(cfgFile, E_SRC_CODE_UTF_8)) {
		if (m_editorWindow) {
			m_editorWindow->setSettingConfig(&m_editorSetting);
		}
		return true;
	}
	return false;
}
bool MainWindow::setFileRoot(const char *rootFile)
{
	MY_LOAD_XML_AND_NEW(m_fileRoot, rootFile, return false);

	ndxml *fileroot = ndxml_getnode(m_fileRoot, "script_file_manager");
	if (!fileroot){
		return true;
	}
	m_fileRootPath = rootFile;
	const char *lastOpen = ndxml_getattr_val(fileroot, "last_edited");
	if (!lastOpen)	{
		return true;
	}

	for (int i = 0; i < ndxml_getsub_num(fileroot); i++) {
		ndxml *node = ndxml_getnodei(fileroot, i);
		const char *fileName = ndxml_getattr_val(node, "name");
		if (0 == ndstricmp(fileName, lastOpen))	{
			loadScriptFile(ndxml_getval(node));
			break;
		}
	}
	return true;
}


bool MainWindow::setDefaultFile(const char *lastEditfile)
{
	ndxml *fileroot = ndxml_getnode(m_fileRoot, "script_file_manager");
	if (!fileroot){
		return false;
	}

	for (int i = 0; i < ndxml_getsub_num(fileroot); i++) {
		ndxml *node = ndxml_getnodei(fileroot, i);

		const char *fileName = ndxml_getval(node);
		if (0 == ndstricmp(fileName, lastEditfile))	{
			ndxml_setattrval(fileroot, "last_edited", ndxml_getattr_val(node, "name"));
			break;
		}
	}
	return true;
}

//menu
void MainWindow::on_actionViewList_triggered()
{
	QDockWidget *pDock = this->findChild<QDockWidget*>("FilelistView");
	if (pDock){
		pDock->close();
		return;
	}
	else {
		openFileView();	
	}
	
}



void MainWindow::on_actionViewOutput_triggered()
{
	QDockWidget *pDock = this->findChild<QDockWidget*>("outputView");
	if (pDock){

		QTextEdit *pEdit = this->findChild<QTextEdit*>("logTextEdit");
		if (pEdit)	{
			m_logText = pEdit->toPlainText();
		}
		pDock->close();
		return;
	}
	else {        
		openOutputView();
	}

}


void MainWindow::on_actionFunctionView_triggered()
{

    QDockWidget *pDock = this->findChild<QDockWidget*>("FunctionView");
    if (pDock){
        pDock->close();
        return;
    }
    else {
		openFunctionsView();
    }
}

void MainWindow::on_actionDetail_triggered()
{
    QDockWidget *pDock = this->findChild<QDockWidget*>("DetailView");
    if (pDock){
        pDock->close();
        return;
    }
    else {
		openDetailView();
    }
}

void MainWindow::on_actionFileNew_triggered()
{

}

void MainWindow::on_actionFileOpen_triggered()
{

}

void MainWindow::on_actionFileClose_triggered()
{

}

void MainWindow::on_actionCompile_triggered()
{

}

void MainWindow::on_actionSave_triggered()
{
	saveCurFile();
}

void MainWindow::on_actionExit_triggered()
{
	close();
}


void MainWindow::on_actionCompileAll_triggered()
{
	saveCurFile();
	ndxml_root *xml = ndxml_getnode(m_fileRoot, "script_file_manager");
	if (!xml){
		return ;
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

}



static int getScriptExpEncodeType(ndxml *scriptXml)
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

static bool getScriptExpDebugInfo(ndxml *scriptXml)
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
bool MainWindow::compileScript(const char *scriptFile)
{
	ndxml_root xmlScript;
	ndxml_initroot(&xmlScript);
	//nd_get_encode_name(ND_ENCODE_TYPE)
	if (-1 == ndxml_load_ex(scriptFile, &xmlScript,"utf8" )) {
		return false;
	}
	const char*inFile = scriptFile;

	const char *outFile = getScriptSetting(&xmlScript, "out_file");
	if (!outFile){
		nd_logerror("compile %s error !!!\nMaybe file is destroyed\n", scriptFile);
		return false;
	}
	int outEncode = getScriptExpEncodeType(&xmlScript);
	bool withDebug = getScriptExpDebugInfo(&xmlScript);
	std::string outPath = outFile;
	ndxml_destroy(&xmlScript);

	outFile = outPath.c_str();

	int orderType = ND_L_ENDIAN;
// 	const char *orderName = _getFromIocfg("bin_data_byte_order");
// 	if (orderName) {
// 		orderType = atoi(orderName);
// 	}

	LogicCompiler lgcompile;

	if (!lgcompile.setConfigFile(m_confgiFile.c_str())) {
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

	/*
	WriteLog("!!!!!!!!!!COMPILE script success !!!!!!!!!!!\n begining run script...\n");

	ClientMsgHandler::ApoConnectScriptOwner apoOwner;
	if (!apoOwner.loadDataType(_getFromIocfg("net_data_def"))) {
		WriteLog("load data type error\n");
		return false;
	}

	LogicEngineRoot *scriptRoot = LogicEngineRoot::get_Instant();
	nd_assert(scriptRoot);

	scriptRoot->setPrint(ND_LOG_WRAPPER_PRINT(MainWindow), NULL);
	scriptRoot->getGlobalParser().setSimulate(true, &apoOwner);

	if (0 != scriptRoot->LoadScript(outFile)){
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

	LogicEngineRoot::destroy_Instant();*/
	WriteLog("!!!!!!!!!!!!!!!!!!!SCRIPT COMPILE SUCCESS!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
	return true;
}
