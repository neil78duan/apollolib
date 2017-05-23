#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "apoScript/apoUiMainEditor.h"
#include "apoScript/apoXmlTreeView.h"
#include "apoScript/apoBaseExeNode.h"

#include "logic_parser/dbl_mgr.h"
#include "logic_parser/logic_compile.h"
#include "logic_parser/logicEngineRoot.h"
#include "logic_parser/logic_editor_helper.h"
#include "logic_parser/script_event_id.h"


#include "nd_common/nd_common.h"
#include "ndlib.h"

#include "ndlog_wrapper.h"
#include "runFuncDialog.h"
ND_LOG_WRAPPER_IMPLEMENTION(MainWindow, __glogWrapper);


#include <QTextEdit>
#include <QDockWidget>
#include <QTableWidget>
#include <QApplication>
#include <QFileDialog>
#include <QMessageBox>



MainWindow::MainWindow(QWidget *parent) :
QMainWindow(parent), m_editorSetting(*apoEditorSetting::getInstant()), m_editorWindow(0),
	m_fileRoot(0), m_curFile(0), m_currFunction(0), m_localDebugOwner(NULL),
    ui(new Ui::MainWindow)
{
	m_debugInfo = NULL;
	m_debuggerCli = NULL;
	m_isChangedCurFile = false;
    ui->setupUi(this);

    setWindowTitle(tr("Script editor"));
	
	m_editorWindow = new apoUiMainEditor(this);
	m_editorWindow->setAttribute(Qt::WA_DeleteOnClose, true);
	m_editorWindow->setObjectName(QString("MainEditor"));

	QObject::connect(m_editorWindow, SIGNAL(showExenodeSignal(apoBaseExeNode*)),
		this, SLOT(onShowExeNodeDetail(apoBaseExeNode *)));

	QObject::connect(m_editorWindow, SIGNAL(breakPointSignal(const char *, const char *, bool)),
		this, SLOT(onBreakPointEdited(const char *, const char *, bool)));

	setCentralWidget(m_editorWindow);

	__glogWrapper = ND_LOG_WRAPPER_NEW(MainWindow);

	onDebugEnd();
}

MainWindow::~MainWindow()
{
	if (m_debuggerCli)	{
		m_debuggerCli->cmdTerminate();
		delete m_debuggerCli;
	}
	closeCurFile();
	if (m_fileRoot){
		ndxml_save(m_fileRoot, m_fileRootPath.c_str());
		ndxml_destroy(m_fileRoot);
		m_fileRoot = 0;
	}
    delete ui;

	ND_LOG_WRAPPER_DELETE(__glogWrapper);
}


bool MainWindow::myInit()
{
	apoEditorSetting *g_seting = apoEditorSetting::getInstant();
	m_fileTemplate = g_seting->getIoConfigValue("new_template");
	//m_messageFile = g_seting->getIoConfigValue("net_protocol");
	m_confgiFile = g_seting->m_edirotSettingFile;

	setFileRoot(g_seting->getIoConfigValue("script_root"));
	showAllView();

	return true;
}

void MainWindow::ClearLog()
{
	//ui->LogText->clear();
	QDockWidget *pDock = this->findChild<QDockWidget*>("outputView");
	if (pDock){
		QTextEdit *pEdit = pDock->findChild<QTextEdit*>("logTextEdit");
		if (pEdit)	{
			pEdit->clear();
			pEdit->update();
		}
	}
	m_logText.clear();
	
}

void MainWindow::WriteLog(const char *logText)
{	
	QDockWidget *pDock = this->findChild<QDockWidget*>("outputView");
	if (pDock){
		QTextEdit *pEdit = pDock->findChild<QTextEdit*>("logTextEdit");
		if (pEdit)	{			
			pEdit->moveCursor(QTextCursor::End);
			pEdit->insertPlainText(QString(logText));
			pEdit->moveCursor(QTextCursor::End, QTextCursor::KeepAnchor);
			pEdit->update();
			return;		 
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
	subwindow->setObjectName("FileListTree");

	QObject::connect(subwindow, SIGNAL(currentItemChanged(QTreeWidgetItem *, QTreeWidgetItem *)),
		this, SLOT(onFilesTreeCurItemChanged(QTreeWidgetItem *, QTreeWidgetItem *)));

	QObject::connect(subwindow, SIGNAL(xmlNodeDelSignal(ndxml *)),
		this, SLOT(onFilelistDel(ndxml *)));

	QObject::connect(subwindow, SIGNAL(xmlNodeDisplayNameChangeSignal(ndxml *)), this, SLOT(onFileModuleNameChanged(ndxml*)));

	if (m_fileRoot)	{
		subwindow->setAlias(&m_editorSetting.getAlias());
		ndxml *xmlFiles = ndxml_getnode(m_fileRoot, "script_file_manager");
		if (xmlFiles){
			subwindow->setXmlInfo(xmlFiles, 2, "Files");
		}
		
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


		QObject::connect(subwindow, SIGNAL(currentItemChanged(QTreeWidgetItem *, QTreeWidgetItem *)),
			this, SLOT(onFunctionsTreeCurItemChanged(QTreeWidgetItem *, QTreeWidgetItem *)));

		QObject::connect(subwindow, SIGNAL(xmlNodeDelSignal(ndxml *)),
			this, SLOT(onXmlNodeDel(ndxml *)));

		QObject::connect(subwindow, SIGNAL(xmlNodeDisplayNameChangeSignal(ndxml *)), this, SLOT(onFunctionListChanged(ndxml*)));

		if (m_curFile)	{
			subwindow->setAlias(&m_editorSetting.getAlias());
			subwindow->setXmlInfo(m_curFile, 5, m_fileMoudleName.c_str());
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


bool MainWindow::loadScriptFromModule(const char *moduleName)
{
	if (!moduleName || !*moduleName)	{
		return false;
	}
	ndxml *fileroot = ndxml_getnode(m_fileRoot, "script_file_manager");
	if (!fileroot){
		return false;		
	}

	for (int i = 0; i < ndxml_getsub_num(fileroot); i++) {
		ndxml *node = ndxml_getnodei(fileroot, i);
		const char *fileName = ndxml_getattr_val(node, "name");
		if (0 == ndstricmp(fileName, moduleName))	{
			m_fileMoudleName = ndxml_getattr_val(node, "name");
			return loadScriptFile(ndxml_getval(node));
		}
	}
	return false;
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

	LogicEngineRoot::get_Instant()->getGlobalDebugger().clearBreakpoint();

	MY_LOAD_XML_AND_NEW(m_curFile, scriptFile, m_editorSetting.m_encodeName.c_str(), return true);

	//load user define enum 
	common_export_error_list("./.error_list.xml");
	const char *filename = getScriptSetting(m_curFile, "user_define_enum");
	if (filename){
		m_editorSetting.loadUserDefEnum(filename);
		//m_editorSetting.loadMessageDefine(m_messageFile.c_str());
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
	if (m_curFile) {
		tree->setXmlInfo(m_curFile, 5, m_fileMoudleName.c_str());
	}
	return true;
}

bool MainWindow::showFileslist()
{
	QDockWidget *pDock = this->findChild<QDockWidget*>("FilelistView");
	if (!pDock){
		return false;
	}

	apoXmlTreeView *tree = this->findChild<apoXmlTreeView*>("FileListTree");
	if (!tree)	{
		return false;
	}
	
	ndxml *xmlFiles = ndxml_getnode(m_fileRoot, "script_file_manager");
	if (xmlFiles){
		tree->clear();
		tree->setXmlInfo(xmlFiles, 2, "Files");
	}

	return true;
}


void MainWindow::onFileChanged()
{
	m_isChangedCurFile = true;
	setCurFileSave(false);

	LogicEditorHelper::setScriptChangedTime(m_curFile, time(NULL));
}


void MainWindow::onFilelistDel(ndxml *xmlnode)
{
	const char *name = ndxml_getname(xmlnode);
	if (0==ndstricmp(name, "file") ){
		const char *path = ndxml_getval(xmlnode);
		if (path){
			nd_rmfile(path);
		}
	}
	if (xmlnode == m_curFile) {
		m_editorWindow->clearFunction();
		m_editorWindow->update();
		m_curFile = NULL;
	}
	ndxml_delxml(xmlnode, NULL);

	ndxml_save(m_fileRoot, m_fileRootPath.c_str());
}

void MainWindow::onXmlNodeDel(ndxml *xmlnode)
{
	m_isChangedCurFile = true;
	if (xmlnode == m_currFunction || (m_currFunction&&ndxml_get_parent(m_currFunction) == xmlnode))	{
		m_editorWindow->clearFunction();
	}
	ndxml_delxml(xmlnode, NULL);
	onFileChanged();
}


void MainWindow::onFunctionListChanged(ndxml *xmlnode)
{
	if (m_currFunction == xmlnode && m_editorWindow){
		m_editorWindow->onFuncNameChanged(xmlnode);
	}
}


void MainWindow::onFileModuleNameChanged(ndxml *xmlnode)
{
	const char *fileName = ndxml_getval(xmlnode);
	const char *moduleName = ndxml_getattr_val(xmlnode, "name");
	if (moduleName && *moduleName && fileName){

		if (0 == ndstricmp(m_filePath.c_str(), fileName)) {
			m_fileMoudleName = moduleName;
			ndxml *xml = ndxml_getnode(m_fileRoot, "script_file_manager");
			if (xml){
				ndxml_setattrval(xml, "last_edited", moduleName);
			}
			if (m_curFile){
				LogicEditorHelper::setModuleName(m_curFile, moduleName);
			}
			onFileChanged();
		}
		else {
			ndxml_root scriptFile;
			if (0==ndxml_load_ex(fileName,&scriptFile,"utf8") )	{
				LogicEditorHelper::setModuleName(&scriptFile, moduleName);
				ndxml_destroy(&scriptFile);
			}
		}

	}
}


void MainWindow::onDebugTerminate()
{
	EndDebug(true);
	this->update();
}
void MainWindow::onDebugStep(const char *func, const char* node)
{
	nd_logdebug("recv step in %s : %s\n", func, node);
	showDebugNode(node);

	if (m_debugInfo){
		delete m_debugInfo;
		m_debugInfo = NULL;
	}
	if (m_debuggerCli)	{
		m_debugInfo = m_debuggerCli->getParserInfo();
		if (m_debugInfo)		{
			showDebugInfo(m_debugInfo);
		}
	}
	this->update();
}
void MainWindow::onDebugCmdAck()
{
	if (m_debugInfo){
		delete m_debugInfo;
		m_debugInfo = NULL;
	}
	if (m_debuggerCli)	{
		m_debugInfo = m_debuggerCli->getParserInfo();
		if (m_debugInfo)		{
			showDebugInfo(m_debugInfo);
		}
	}
	this->update();
}

void MainWindow::onScriptRunOK()
{
	nd_logdebug("script run completed\n");
	if (m_debuggerCli)	{
		m_debugInfo = m_debuggerCli->getParserInfo();
		if (m_debugInfo)		{
			showDebugInfo(m_debugInfo);
		}
	}
	m_editorWindow->setDebugNode(NULL);
	this->update();
}


bool MainWindow::showCurFunctions()
{	
	bool ret = m_editorWindow->showFunction(m_currFunction, m_curFile);
	if (ret == false){
		m_editorWindow->clearFunction();
		QMessageBox::warning(NULL, "Error", "Can not show current Function!", QMessageBox::Ok);
		m_currFunction = NULL;
	}
	return ret ;
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

void MainWindow::closeCurFunction()
{
	if (m_editorWindow) {
		m_editorWindow->clearFunction();
		m_editorWindow->update();
	}
	m_currFunction = NULL;
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


bool MainWindow::showDebugInfo(ndxml *xmldata)
{
	QDockWidget *pDock = this->findChild<QDockWidget*>("DebugView");
	if (pDock)	{
		QWidget *w = pDock->findChild<QWidget*>("xmlDebugTable");
		if (w)	{
			w->close();
		}
	}
	else {
		pDock = new QDockWidget(tr("Debug"), this);
		pDock->setObjectName("DebugView");
		pDock->setAttribute(Qt::WA_DeleteOnClose, true);
	}

	apoUiCommonXmlView *subwindow = new apoUiCommonXmlView();
	subwindow->setAttribute(Qt::WA_DeleteOnClose, true);
	subwindow->setObjectName("xmlDebugTable");

	subwindow->showDetail(xmldata, m_curFile);

	QObject::connect(subwindow, SIGNAL(xmlDataChanged()),
		this, SLOT(onFileChanged()));

	pDock->setWidget(subwindow);
	addDockWidget(Qt::RightDockWidgetArea, pDock);

	return true;
}

void MainWindow::closeDebugInfo()
{
	QDockWidget *pDock = this->findChild<QDockWidget*>("DebugView");
	if (pDock)	{
		pDock->close();
	}
}

void MainWindow::closeDetail()
{
	QDockWidget *pDock = this->findChild<QDockWidget*>("DetailView");
	if (pDock)	{
		pDock->close();
	}
}

void MainWindow::onBreakPointEdited(const char *func, const char *node, bool isAdd)
{
	if (m_debuggerCli){
		if (isAdd)	{
			m_debuggerCli->cmdAddBreakPoint(func, node);
		}
		else{
			m_debuggerCli->cmdDelBreakPoint(func, node);
		}
	}
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

	//QObject::connect(subwindow, SIGNAL(xmlDataChanged()),
	//	this, SLOT(onFileChanged()));

	if (m_editorWindow) {
		QObject::connect(subwindow, SIGNAL(xmlDataChanged()),
			m_editorWindow, SLOT(onCurNodeChanged()));
	}


	pDock->setWidget(subwindow);

	addDockWidget(Qt::RightDockWidgetArea, pDock);

}

//double click function list
void MainWindow::onFunctionsTreeCurItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous)
{
	xmlTreeItem *pXmlItem = dynamic_cast<xmlTreeItem*>(current);
	if (!pXmlItem)	{
		return;
	}
	ndxml *pxml = (ndxml *)pXmlItem->getUserData();
	if (!pxml)	{
		return;
	}
	if (pxml == m_currFunction){
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
			m_editorWindow->showNodeDetail(NULL);
		}
	}
}

// double click file list
void MainWindow::onFilesTreeCurItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous)
{

	xmlTreeItem *pXmlItem = dynamic_cast<xmlTreeItem*>(current);
	if (!pXmlItem)	{
		return;
	}
	ndxml *pxml = (ndxml *)pXmlItem->getUserData();
	if (!pxml)	{
		return;
	}

	closeDetail();
	m_editorWindow->clearFunction();
	m_currFunction = NULL;

	m_fileMoudleName = ndxml_getattr_val(pxml, "name");
	if (loadScriptFile(ndxml_getval(pxml))) {
		setDefaultFile(ndxml_getval(pxml));
	}
}

//////////////////////////////////////////////////////////////////////////

// 
// bool MainWindow::setConfig(const char *cfgFile, const char *messageFile)
// {
// 	m_messageFile = messageFile;
// 	m_confgiFile = cfgFile;
// 	if (m_editorSetting.setConfigFile(cfgFile, E_SRC_CODE_UTF_8)) {
// 		if (m_editorWindow) {
// 			m_editorWindow->setSettingConfig(&m_editorSetting);
// 		}
// 		return true;
// 	}
// 	return false;
// }
bool MainWindow::setFileRoot(const char *rootFile)
{
	MY_LOAD_XML_AND_NEW(m_fileRoot, rootFile,m_editorSetting.m_encodeName.c_str(), return false);

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

			m_fileMoudleName = ndxml_getattr_val(node, "name");
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
	QString filter = QFileDialog::getSaveFileName(this, tr("New file"), ".", tr("Allfile(*.*);;xmlfile(*.xml)"));

	if (filter.isNull()) {
		return;
	}

	char buf[1024];
	nd_relative_path(filter.toStdString().c_str(), nd_getcwd(), buf, sizeof(buf));
	
	ndxml_root *xml = ndxml_getnode(m_fileRoot, "script_file_manager");
	if (!xml){
		QMessageBox::warning(NULL, "Error", "File Error!!!", QMessageBox::Ok);
		return;
	}
	if (-1 == nd_cpfile(m_fileTemplate.c_str(), buf)) {
		QMessageBox::warning(NULL, "Error", "SAVE File Error!!!", QMessageBox::Ok);
		return;
	}
	ndxml_root newFileXml;
	if (0 == ndxml_load(buf, &newFileXml)) {
		LogicEditorHelper::setScriptChangedTime(&newFileXml,time(NULL)) ;
		ndxml_destroy(&newFileXml);
	}

	ndxml *newNode = ndxml_addnode(xml, "file", buf);
	if (newNode){
		const char *fileName = nd_filename(buf);
		char moduleName[128];
		ndstr_nstr_end(fileName, moduleName, '.', 128);
		ndxml_addattrib(newNode, "name", moduleName);

	}

	ndxml_save(m_fileRoot, m_fileRootPath.c_str());
	showFileslist();
}

void MainWindow::on_actionFileOpen_triggered()
{
	char mypath[1024];
	QString fullPath = QFileDialog::getOpenFileName(this, tr("open file"), ".", tr("Allfile(*.*);;xmlfile(*.xml)"));
	if (fullPath.isNull()) {
		return;
	}
	if (!nd_relative_path(fullPath.toStdString().c_str(), nd_getcwd(), mypath, sizeof(mypath))) {
		return;
	}

	ndxml_root xmlScript;
	ndxml_initroot(&xmlScript);
	//nd_get_encode_name(ND_ENCODE_TYPE)
	if (-1 == ndxml_load_ex(mypath, &xmlScript, "utf8")) {
		nd_logerror("can not open file %s", fullPath.toStdString().c_str());
		return;
	}
#define CHECK_FILE(_xml, _nodeName) \
	do 	{	\
		ndxml *xml = ndxml_getnode((_xml), _nodeName) ;	\
		if(!xml) {	\
			nd_logerror("can not load file %s Maybe file is destroyed\n", fullPath.toStdString().c_str());	\
			return ;	\
		}				\
	} while (0)

	CHECK_FILE(&xmlScript, "blueprint_setting");
	CHECK_FILE(&xmlScript, "moduleInfo");
	CHECK_FILE(&xmlScript, "baseFunction");

	if (!loadScriptFile(mypath)) {
		nd_logerror("can not load file %s\n", fullPath.toStdString().c_str());	
		return;
	}
	//////////////////////////////////////////////////////////////////////////
	ndxml *fileroot = ndxml_getnode(m_fileRoot, "script_file_manager");
	if (fileroot){
		bool isFound = false;
		for (int i = 0; i < ndxml_getsub_num(fileroot); i++) {
			ndxml *node = ndxml_getnodei(fileroot, i);
			const char *filePath = ndxml_getval(node);
#ifdef WIN32
			if (0 == ndstricmp(filePath, mypath))	{
#else 
			if (0 == strcmp(filePath, mypath))	{
#endif

				m_fileMoudleName = ndxml_getattr_val(node, "name");
				loadScriptFile(ndxml_getval(node));
				isFound = true;
				break;
			}
		}
		if (!isFound) {

			const char *fileName = nd_filename(mypath);
			char moduleName[128];
			ndstr_nstr_end(fileName, moduleName, '.', 128);
			ndxml *newxml = ndxml_addnode(fileroot, "file", mypath);
			if (newxml)		{
				ndxml_addattrib(newxml, "name", moduleName);
			}

			ndxml_save(m_fileRoot, m_fileRootPath.c_str());
			showFileslist();
		}
	}
	

}

void MainWindow::on_actionFileClose_triggered()
{
	if (checkNeedSave()){
		saveCurFile();
		ndxml_destroy(m_curFile);
		delete m_curFile;
	}
	m_curFile = NULL;
	m_editorWindow->clearFunction();
	showCurFile();

}

void MainWindow::on_actionCompile_triggered()
{
	//ClearLog();
	on_actionSave_triggered();
	if (m_filePath.size()) {
		std::string outFile;
		compileScript(m_filePath.c_str(), outFile);
	}
}

void MainWindow::on_actionSave_triggered()
{
	if (checkNeedSave()){
		saveCurFile();
	}
	setCurFileSave(true);
}

void MainWindow::on_actionExit_triggered()
{
	close();
}

void MainWindow::on_actionExit_without_save_triggered()
{
	setCurFileSave(true);
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
		std::string outFile;
		ndxml *node = ndxml_getnodei(xml, i);
		if (!node)
			continue;
		if (!compileScript(ndxml_getval(node), outFile)) {
			ret = false;
			break;
		}
	}

}

void MainWindow::on_actionClearLog_triggered()
{
	m_logText.clear();

	QDockWidget *pDock = this->findChild<QDockWidget*>("outputView");
	if (pDock){
		QTextEdit *pEdit = pDock->findChild<QTextEdit*>("logTextEdit");
		if (pEdit)	{
			pEdit->clear();
		}
	}
}

void MainWindow::on_actionRun_triggered()
{
	on_actionSave_triggered();
	ClearLog();

	Run(false);

}

void MainWindow::on_actionTest_triggered()
{
	on_actionSave_triggered();
	ndxml_root *xml = ndxml_getnode(m_fileRoot, "script_file_manager");
	if (!xml){
		return;
	}

	bool ret = true;
	int num = ndxml_num(xml);
	for (int i = 0; i < num; i++) {
		std::string outFile;
		ndxml *node = ndxml_getnodei(xml, i);
		if (!node)
			continue;
		if (!compileScript(ndxml_getval(node), outFile,true)) {
			ret = false;
			break;
		}
	}
}


void MainWindow::on_actionCancel_scale_triggered()
{
	if (m_editorWindow && m_currFunction)	{
		showCurFunctions();
	}
}

void MainWindow::on_actionRunDebug_triggered()
{
	on_actionSave_triggered();
	ClearLog();
	closeDetail();

	Run(true);
}

void MainWindow::on_actionStepOver_triggered()
{
	if (m_debuggerCli)	{
		bool bExeed = false;
		int ret = -1;
		//run to next node 
		apoBaseExeNode *pDbgNode = m_editorWindow->getCurDebugNode();
		if (pDbgNode)	{
			apoBaseExeNode *aimNode = pDbgNode->getMyNextNode();
			if (aimNode)	{
				ndxml *xmlnode = aimNode->getBreakPointAnchor();
				if (xmlnode){
					char buf[1024];
					if (LogicCompiler::getFuncStackInfo(xmlnode, buf, sizeof(buf))) {

						bExeed = true;
						ret = m_debuggerCli->cmdRunTo(LogicEditorHelper::_GetXmlName(m_currFunction,NULL), buf);
					}
				}
			}
		}


		if (!bExeed ) {
			ret = m_debuggerCli->cmdStep();
		}

		if (ret == -1)	{
			EndDebug(false);
		}

	}
}


void MainWindow::on_actionStepIn_triggered()
{
	if (m_debuggerCli)	{
		if (m_debuggerCli->cmdStep() == -1) {
			EndDebug(false);
		}
	}
}

void MainWindow::on_actionContinue_triggered()
{
	if (m_debuggerCli)	{
		if (m_debuggerCli->cmdContinue() == -1) {
			EndDebug(false);
		}
	}
}

void MainWindow::on_actionStepOut_triggered()
{

	if (m_debuggerCli)	{
		if (m_debuggerCli->cmdRunOut() == -1) {
			EndDebug(false);
		}
	}
}

void MainWindow::on_actionAttach_triggered()
{
	Process_vct_t processes;
	if (!ApoDebugger::getRunningProcess(processes)) {
		nd_logerror("no process could be attached\n");
		return;
	}

	ListDialog dlg(this);

	for (Process_vct_t::iterator it = processes.begin(); it != processes.end() ; it++)	{		
		dlg.m_selList.push_back(QString(it->name.c_str()));
	}

	dlg.InitList();
	if (dlg.exec() != QDialog::Accepted) {
		return ;
	}

	int seled = dlg.GetSelect();
	if (seled == -1) {
		return ;
	}

	if (seled >= processes.size()) {
		return;
	}
	

	NDUINT32 pId = processes[seled].pId;

	m_debuggerCli = new ApoDebugger;


	QObject::connect(&m_debuggerCli->m_obj, SIGNAL(stepSignal(const char *, const char *)),
		this, SLOT(onDebugStep(const char *, const char*)));

	QObject::connect(&m_debuggerCli->m_obj, SIGNAL(terminateSignal()),
		this, SLOT(onDebugTerminate()));

	QObject::connect(&m_debuggerCli->m_obj, SIGNAL(commondOkSignal()),
		this, SLOT(onDebugCmdAck()));


	QObject::connect(&m_debuggerCli->m_obj, SIGNAL(scriptRunOKSignal()),
		this, SLOT(onScriptRunOK()));

	closeCurFunction();

	m_debuggerCli->Attach(pId,E_SRC_CODE_UTF_8);
	onAttached(processes[seled].scriptModule.c_str());

}

void MainWindow::on_actionDeattch_triggered()
{
	onDeattched();
	if (m_debuggerCli)	{
		m_debuggerCli->Deattach();		
		delete m_debuggerCli;
		m_debuggerCli = NULL;
	}

	nd_logmsg("----------Deattached----------\n");
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


bool MainWindow::runFunction(const char *binFile, const char *srcFile, int argc, const char* argv[])
{
	bool ret = true;
	DeguggerScriptOwner apoOwner;
	if (!apoOwner.loadDataType(m_editorSetting.getIoConfigValue("net_data_def"))) {
		WriteLog("load data type error\n");
		return false;
	}

	LogicEngineRoot *scriptRoot = LogicEngineRoot::get_Instant();
	nd_assert(scriptRoot);
	scriptRoot->setPrint(ND_LOG_WRAPPER_PRINT(MainWindow), NULL);
	scriptRoot->setOutPutEncode(E_SRC_CODE_UTF_8);

	if (argc ==0 )	{
		scriptRoot->getGlobalParser().setSimulate(true, &apoOwner);
		if (0 != scriptRoot->LoadScript(binFile, &scriptRoot->getGlobalParser() )){
			WriteLog("load script error n");
			LogicEngineRoot::destroy_Instant();
			return false;
		}

		LogicParserEngine &parser = scriptRoot->getGlobalParser();
		parser.eventNtf(APOLLO_EVENT_SERVER_START, 0) ;
		WriteLog("start run script...\n");
		if (0 != scriptRoot->test()){
			WriteLog("run script error\n");
			ret = false;
		}
		else {
			nd_logmsg("!!!!!!!!!!!!!!!!!!!RUN SCRIPT %s  SUCCESS!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n", binFile);
		}
		parser.eventNtf(APOLLO_EVENT_SERVER_STOP, 0);
	}
	else {
		LogicParserEngine &parser = scriptRoot->getGlobalParser();
		parser.setSimulate(true, &apoOwner);

		if (0 != scriptRoot->LoadScript(binFile,NULL)){
			WriteLog("load script error n");
			LogicEngineRoot::destroy_Instant();
			return false;
		}

		parser.eventNtf(APOLLO_EVENT_SERVER_START, 0);
		int ret = parser.runCmdline(argc, argv, E_SRC_CODE_UTF_8);
		if (ret && parser.getErrno() != NDERR_WOULD_BLOCK)	{
			nd_logmsg("run function %s error : %d \n", argv[0], ret);
			ret = false;
		}
		else {
			//nd_logmsg("run function %s SUCCESS \n", argv[0]);
		}
		//parser.eventNtf(APOLLO_EVENT_UPDATE, 0);
		parser.eventNtf(APOLLO_EVENT_SERVER_STOP, 0);
	}
	LogicEngineRoot::destroy_Instant();

	if (ret == false)	{
		LogicParserEngine &parser = scriptRoot->getGlobalParser();
		const char *lastError = parser.getLastErrorNode();
		if (lastError)	{
			nd_logerror("run in %s\n", lastError);
			showRuntimeError(srcFile, lastError);

		}
	}

	return ret;
}

bool MainWindow::StartDebug(const char *binFile, const char *srcFile, int argc, const char* argv[])
{
	//bool ret = true;	
	m_localDebugOwner = new DeguggerScriptOwner();
	m_debuggerCli = new ApoDebugger();

	DeguggerScriptOwner &apoOwner = *m_localDebugOwner;

	if (!apoOwner.loadDataType(m_editorSetting.getIoConfigValue("net_data_def"))) {
		WriteLog("load data type error\n");
		return false;
	}

	LogicEngineRoot *scriptRoot = LogicEngineRoot::get_Instant();
	nd_assert(scriptRoot);
	scriptRoot->setPrint(ND_LOG_WRAPPER_PRINT(MainWindow), NULL);
	scriptRoot->setOutPutEncode(E_SRC_CODE_UTF_8);

	LogicParserEngine &parser = scriptRoot->getGlobalParser();
	parser.setSimulate(true, m_localDebugOwner);


	if (0 != scriptRoot->LoadScript(binFile, NULL)){
		WriteLog("load script error n");
		EndDebug(false);
		return false;
	}


	QObject::connect(&m_debuggerCli->m_obj, SIGNAL(stepSignal(const char *, const char *)),
		this, SLOT(onDebugStep(const char *, const char* )));

	QObject::connect(&m_debuggerCli->m_obj, SIGNAL(terminateSignal()),
		this, SLOT(onDebugTerminate()));
	QObject::connect(&m_debuggerCli->m_obj, SIGNAL(commondOkSignal()),
		this, SLOT(onDebugCmdAck()));

	QObject::connect(&m_debuggerCli->m_obj, SIGNAL(scriptRunOKSignal()),
		this, SLOT(onScriptRunOK()));


	onDebugStart();
	parser.eventNtf(APOLLO_EVENT_SERVER_START, 0);


	nd_logmsg("======== Begin deubg %s ...==========\n", argv[0]);
	int ret = m_debuggerCli->localStrat(&parser,argc, argv, E_SRC_CODE_UTF_8);
	if (-1==ret) {
		EndDebug(false);
	}


	return true;
}

void MainWindow::EndDebug(bool bSuccess)
{
	onDebugEnd();
	if (m_debuggerCli)	{
		LogicEngineRoot *scriptRoot = LogicEngineRoot::get_Instant();
		nd_assert(scriptRoot);
		LogicParserEngine &parser = scriptRoot->getGlobalParser();

		if (bSuccess){
			parser.eventNtf(APOLLO_EVENT_SERVER_STOP, 0);
		}
		else {
			const char *lastError = parser.getLastErrorNode();
			if (lastError)	{
				nd_logerror("run in %s\n", lastError);
				showRuntimeError(m_filePath.c_str(), lastError);
			}
		}

		m_debuggerCli->postEndDebug();

		delete m_localDebugOwner;
		m_localDebugOwner = NULL;

		delete m_debuggerCli;
		m_debuggerCli = NULL;

		LogicEngineRoot::destroy_Instant();
	}

	nd_logmsg("----------Debug end----------\n");
}

void MainWindow::onDebugStart()
{

	ui->actionRunDebug->setDisabled(true);
	ui->actionAttach->setDisabled(true);

	ui->actionStepIn->setDisabled(false);
	ui->actionStepOver->setDisabled(false);
	ui->actionContinue->setDisabled(false);
	ui->actionStepOut->setDisabled(false);
	ui->actionDeattch->setDisabled(false);

	closeDetail();
}

void MainWindow::onDebugEnd()
{
	closeDebugInfo();
	if (m_editorWindow)	{
		m_editorWindow->setDebugNode(NULL);
	}

	
	ui->actionRunDebug->setDisabled(false);
	ui->actionAttach->setDisabled(false);

	ui->actionStepIn->setDisabled(true);
	ui->actionStepOver->setDisabled(true);
	ui->actionContinue->setDisabled(true);
	ui->actionStepOut->setDisabled(true);
	ui->actionDeattch->setDisabled(true);

}
void MainWindow::onAttached(const char *moduleName)
{
	bool ret = loadScriptFromModule(moduleName);
	if (ret){
		std::string outFile;
		LocalDebugger & debuggerHost = LogicEngineRoot::get_Instant()->getGlobalDebugger();
		debuggerHost.clearBreakpoint();

		if (compileScript(m_filePath.c_str(), outFile,false,true, E_SRC_CODE_UTF_8)) {
			breakPoint_vct  breakpoints = debuggerHost.getBreakPoints();
			for (breakPoint_vct::iterator it = breakpoints.begin(); it != breakpoints.end(); it++){
				if (it->tempBreak)	{
					continue;
				}
				m_debuggerCli->cmdAddBreakPoint(it->functionName.c_str(), it->nodeName.c_str());
			}
		}
	}
	onDebugStart();
}

void MainWindow::onDeattched()
{
	onDebugEnd();
}


bool MainWindow::Run(bool bIsDebug)
{

	const char *curFunc = m_editorWindow ? m_editorWindow->getEditedFunc() : NULL;
	RunFuncDialog dlg(this);
	dlg.initFunctionList(m_curFile, curFunc);

	if (dlg.exec() != QDialog::Accepted) {
		return false;
	}
	QString qfuncName = dlg.getFunc();
	QString qargs = dlg.getArgs();
	if (qfuncName.isEmpty()) {
		nd_logerror("run function name is NULL\n");
		return false;
	}

	int argc = 0;
	char argbuf[10][64];
	char *argv[10];

	for (int i = 0; i < 10; i++){
		argv[i] = argbuf[i];
	}

	//get func 
	std::string funcName = qfuncName.toStdString();
	std::string input_arg = qargs.toStdString();

	strncpy(argbuf[0], funcName.c_str(), sizeof(argbuf[0]));

	if (input_arg.size() > 0)	{
		nd_logmsg("begin run %s %s\n", funcName.c_str(), input_arg.c_str());
		argc = ndstr_parse_command(input_arg.c_str(), &argv[1], 64, 9);
	}
	else {
		nd_logmsg("begin run %s\n", funcName.c_str());
	}
	++argc;

	std::string outFile;
	if (!compileScript(m_filePath.c_str(), outFile,false, bIsDebug)) {
		return false;
	}

	if (bIsDebug) {
		return StartDebug(outFile.c_str(), m_filePath.c_str(), argc, (const char**)argv);		
	}
	else {
		return runFunction(outFile.c_str(), m_filePath.c_str(), argc, (const char**)argv);
	}

}

bool MainWindow::compileScript(const char *scriptFile, std::string &outputFile, bool bWithRun, bool bDebug, int outEncodeType)
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
	outputFile = outFile;

	int outEncode = outEncodeType;
	if (outEncode == -1){
		getScriptExpEncodeType(&xmlScript);
	}
	bool withDebug = bDebug;
	if (!withDebug)	{
		withDebug = getScriptExpDebugInfo(&xmlScript);
	}
	std::string outPath = outFile;
	ndxml_destroy(&xmlScript);

	outFile = outPath.c_str();

	int orderType = ND_L_ENDIAN;
	
	const char *orderName = m_editorSetting.getIoConfigValue("bin_data_byte_order");
	if (orderName) {
		orderType = atoi(orderName);
 	}

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


		showCompileError(inFile, lgcompile.getErrorStack()) ;
		return false;
	}

	WriteLog("!!!!!!!!!!COMPILE script success !!!!!!!!!!!\n ");
	if (!bWithRun)	{
		return true;
	}
	return runFunction(outFile,scriptFile, 0, NULL);

}


bool MainWindow::showCompileError(const char *xmlFile, stackIndex_vct &errorStack)
{
	if (!loadScriptFile(xmlFile)) {
		nd_logerror("can not load %s file\n", xmlFile);
		return false;
	}

	ndxml *xmlroot = m_curFile;
	for (size_t i = 0; i < errorStack.size(); i++){
		int index = errorStack[i];
		ndxml *node = ndxml_getnodei(xmlroot, index);
		xmlroot = node;

		const char *pAction = m_editorSetting.getDisplayAction(ndxml_getname(node));
		if (pAction && 0 == ndstricmp(pAction, "function")) {
			m_currFunction = node;
			showCurFunctions();
		}


		const compile_setting* compileSetting = m_editorSetting.getStepConfig(ndxml_getname(node));
		if (!compileSetting) {
			continue;
		}

		if (compileSetting->ins_type == E_INSTRUCT_TYPE_CMD){
			if (!m_editorWindow->setCurDetail(node, true)) {
				nd_logerror("can not show current detail node \n");
				return false;
			}
		}
		else if (compileSetting->ins_type == E_INSTRUCT_TYPE_PARAM) {
			if (!m_editorWindow->setCurNodeSlotSelected(node, true))	{
				nd_logerror("can not show current param\n");
				return false;
			}
		}
	}
	return true;
}


bool MainWindow::showDebugNode(const char *nodeInfo)
{
	stackIndex_vct debugStack;
	getStackFromName(nodeInfo, debugStack);

	ndxml *xmlroot = m_curFile;
	for (size_t i = 0; i < debugStack.size(); i++){
		int index = debugStack[i];
		ndxml *node = ndxml_getnodei(xmlroot, index);
		xmlroot = node;

		const char *pAction = m_editorSetting.getDisplayAction(ndxml_getname(node));
		if (pAction && 0 == ndstricmp(pAction, "function")) {
			if (m_currFunction != node)	{
				closeCurFunction();
				m_currFunction = node;
				showCurFunctions();
			}
		}


		const compile_setting* compileSetting = m_editorSetting.getStepConfig(ndxml_getname(node));
		if (!compileSetting) {
			continue;
		}

		if (compileSetting->ins_type == E_INSTRUCT_TYPE_CMD){
			if (!m_editorWindow->setDebugNode(node)) {
				nd_logerror("can not show current debug node \n");
				return false;
			}
		}
	}
	return true;
}

bool  MainWindow::showRuntimeError(const char *scriptFile, const char *errNodeInfo)
{
	stackIndex_vct stackIndex;
	getStackFromName(errNodeInfo, stackIndex);
// 	const char *p = strchr(errNodeInfo, '.');
// 
// 	while (p && *p) {
// 		if (*p == '.') {
// 			++p;
// 		}
// 		char buf[10];
// 		buf[0] = 0;
// 		p = ndstr_nstr_ansi(p, buf,'.', 10);
// 		if (buf[0])	{
// 			stackIndex.push_back(atoi(buf));
// 		}
// 	}
	return showCompileError(scriptFile, stackIndex);
}

bool MainWindow::getStackFromName(const char *nodeInfo, stackIndex_vct &stackIndex)
{
	const char *p = strchr(nodeInfo, '.');

	while (p && *p) {
		if (*p == '.') {
			++p;
		}
		char buf[10];
		buf[0] = 0;
		p = ndstr_nstr_ansi(p, buf, '.', 10);
		if (buf[0])	{
			stackIndex.push_back(atoi(buf));
		}
	}

	return true;
}

void MainWindow::setCurFileSave(bool isSaved)
{
	QDockWidget *pDock = this->findChild<QDockWidget*>("FunctionView");
	if (!pDock){
		return;
	}

	apoXmlTreeView *pEdit = this->findChild<apoXmlTreeView*>("functionsTree");
	if (!pEdit)	{
		return;
	}

	QString rootName = pEdit->rootName();

	if (isSaved){
		if (rootName[0] == '*'){
			rootName.remove(0, 1);
			pEdit->setRootName(rootName);
		}
	}
	else{
		if (rootName[0] != '*'){
			rootName.insert(0,"*");
			pEdit->setRootName(rootName);
		}
	}

}
