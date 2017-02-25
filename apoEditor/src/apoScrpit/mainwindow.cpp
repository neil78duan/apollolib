#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "apoScrpit/apoUiMainEditor.h"
#include "apoScrpit/apoXmlTreeView.h"
#include "apoScrpit/apoBaseExeNode.h"

#include "logic_parser/dbl_mgr.h"
#include "logic_parser/logic_compile.h"
#include "logic_parser/logicEngineRoot.h"
#include "logic_parser/logic_editor_helper.h"

#include "nd_common/nd_common.h"
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
    ui->setupUi(this);

    setWindowTitle(tr("main editor"));
	__glogWrapper = ND_LOG_WRAPPER_NEW(MainWindow);
}

MainWindow::~MainWindow()
{
// 	if (m_config){
// 		ndxml_destroy(m_config);
// 		m_config = 0;
// 	}
	if (m_fileRoot){
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
#ifdef WIN32
	char buf[1024];
	nd_gbk_to_utf8(logText, buf, sizeof(buf));
	logText = buf;
#endif
	
	QDockWidget *pDock = this->findChild<QDockWidget*>("outputView");
	if (pDock){
		QTextEdit *pEdit = pDock->findChild<QTextEdit*>("logTextEdit");
		if (pEdit)	{
			QTextCursor cursor(pEdit->textCursor());
			cursor.movePosition(QTextCursor::End);
			pEdit->insertPlainText(QString(logText));
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
		QObject::connect(subwindow, SIGNAL(itemDoubleClicked(QTreeWidgetItem *, int)),
			this, SLOT(onFunctionsTreeItemDBClicked(QTreeWidgetItem *, int)));

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


bool MainWindow::showCommonDeatil(ndxml *xmldata)
{
	QDockWidget *pDock = this->findChild<QDockWidget*>("DetailView");
	if (pDock)	{
		QWidget *w = pDock->findChild<QWidget*>("xmlDetailTable");
				
		if (dynamic_cast<apoUiCommonXmlView*>(w)) {
			apoUiCommonXmlView *subwindow = (apoUiCommonXmlView*)w;
			subwindow->showDetail(xmldata, m_curFile);
		}
		else {
			w->close();

			apoUiCommonXmlView *subwindow = new apoUiCommonXmlView();
			subwindow->setAttribute(Qt::WA_DeleteOnClose, true);
			subwindow->setObjectName("xmlDetailTable");
			subwindow->showDetail(xmldata, m_curFile);
			pDock->setWidget(subwindow);
		}
	}
	else {
		pDock = new QDockWidget(tr("Detail"), this);
		pDock->setObjectName("DetailView");
		pDock->setAttribute(Qt::WA_DeleteOnClose, true);

		apoUiCommonXmlView *subwindow = new apoUiCommonXmlView();
		subwindow->setAttribute(Qt::WA_DeleteOnClose, true);
		subwindow->setObjectName("xmlDetailTable");

		subwindow->showDetail(xmldata, m_curFile);

		pDock->setWidget(subwindow);
		addDockWidget(Qt::RightDockWidgetArea, pDock);
		return true;
	}
	return true;
}


QWidget *MainWindow::getMainEditor()
{
	return m_editorWindow;
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

bool MainWindow::loadScriptFile(const char *scriptFile)
{
	if (!scriptFile)	{
		return false;
	}

	MY_LOAD_XML_AND_NEW(m_curFile, scriptFile, return true);

	//load user define enum 
	common_export_error_list("./.error_list.xml");
	const char *filename = getScriptSetting(m_curFile, "user_define_enum");
	if (filename){
		m_editorSetting.loadUserDefEnum(filename);
	}
// 
// 	ndxml *funcroot = ndxml_getnode(&xml_net_protocol, "MessageDefine");
// 	if (funcroot) {
// 		char buf[256];
// 		text_list_t messageList;
// 		for (int i = 0; i < ndxml_num(funcroot); i++){
// 			ndxml *fnode = ndxml_getnodei(funcroot, i);
// 			const char *pDispname = ndxml_getattr_val(fnode, "comment");
// 			const char *pRealVal = ndxml_getattr_val(fnode, "id");
// 			const char *p = buildDisplaNameValStr(pRealVal, pDispname, buf, sizeof(buf));
// 			messageList.push_back(QString(p));
// 		}
// 		xmlDlg.addDisplayNameList("msg_list", messageList);
// 	}


	m_currFunction = NULL;
	if (!openFunctionsView()) {
		showCurFile();
	}
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

bool MainWindow::showCurFunctions()
{
	if (m_editorWindow)	{
		m_editorWindow->close();
	}

	m_editorWindow = new apoUiMainEditor(this);
	m_editorWindow->setAttribute(Qt::WA_DeleteOnClose, true);

	//QObject::connect(m_editorWindow, SIGNAL(showExenodeDetail(apoBaseExeNode *exeNode)),
		//this, SLOT(onShowExeNodeDetail(apoBaseExeNode *)));

	
	setCentralWidget(m_editorWindow);

	m_editorWindow->showFunction(m_currFunction, m_curFile);
	return false;
}

void MainWindow::onShowExeNodeDetail(apoBaseExeNode *exenode)
{
	QDockWidget *pDock = this->findChild<QDockWidget*>("DetailView");
	if (pDock)	{
		QWidget *w = pDock->findChild<QWidget*>("xmlDetailTable");

		if (dynamic_cast<apoUiDetailView*>(w)) {
			apoUiDetailView *subwindow = (apoUiDetailView*)w;
			subwindow->showDetail(exenode, m_curFile);
		}
		else {
			w->close();

			apoUiDetailView *subwindow = new apoUiDetailView();
			subwindow->setAttribute(Qt::WA_DeleteOnClose, true);
			subwindow->setObjectName("xmlDetailTable");
			subwindow->showDetail(exenode, m_curFile);
			pDock->setWidget(subwindow);
		}
	}
	else {
		pDock = new QDockWidget(tr("Detail"), this);
		pDock->setObjectName("DetailView");
		pDock->setAttribute(Qt::WA_DeleteOnClose, true);

		apoUiDetailView *subwindow = new apoUiDetailView();
		subwindow->setAttribute(Qt::WA_DeleteOnClose, true);
		subwindow->setObjectName("xmlDetailTable");
		subwindow->showDetail(exenode, m_curFile);
		pDock->setWidget(subwindow);

		pDock->setWidget(subwindow);
		addDockWidget(Qt::RightDockWidgetArea, pDock);
	}
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
		m_editorWindow->curDetailChanged(NULL);
		//m_currFunction = pxml;
	}
	//nd_logmsg("show functions !");
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
	loadScriptFile(ndxml_getval(pxml));
// 
// 	const char *fileName = ndxml_getval(pxml);
// 	if (fileName && *fileName)	{
// 		MY_LOAD_XML_AND_NEW(m_curFile, fileName, return);
// 
// 		m_currFunction = NULL;
// 		if (!openFunctionsView()) {
// 			showCurFile();
// 		}
// 
// 	}
	//nd_logmsg("load %s\n", fileName);
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


//////////////////////////////////////////////////////////////////////////


// 
// bool MainWindow::setConfig(const char *cfgFile)
// {
// 	MY_LOAD_XML_AND_NEW(m_config, cfgFile, return false);
// 	return true;
// }


bool MainWindow::setConfig(const char *cfgFile)
{
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
	const char *lastOpen = ndxml_getattr_val(fileroot, "last_edited");
	if (!lastOpen )	{
		return true;
	}

	for (int i = 0; i < ndxml_getsub_num(fileroot); i++) {
		ndxml *node = ndxml_getnodei(fileroot, i);
		const char *fileName = ndxml_getattr_val(node, "name");
		if (0 == ndstricmp(fileName, lastOpen))	{
			loadScriptFile(ndxml_getval(node));
// 			const char *filePath = ndxml_getval(node);
// 			if (filePath)	{
// 				MY_LOAD_XML_AND_NEW(m_curFile, filePath, return true);
// 				m_currFunction = NULL;
// 				if (!openFunctionsView()) {
// 					showCurFile();
// 				}
// 			}
			break;
		}

	}

	return true;

}
