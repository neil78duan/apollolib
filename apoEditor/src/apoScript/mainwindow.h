#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "nd_common/nd_common.h"
#include "apoScript/apoEditorSetting.h"
#include "logic_parser/logic_compile.h"
#include "logic_parser/logic_debugger.h"

#include "cli_common/dftCliMsgHandler.h"
#include "apoScript/apoUiDebugger.h"

#include <string>

#include <vector>
#include <QMainWindow>
#include <QMouseEvent>
#include <QTreeWidgetItem>
#include <QString>
#include <QTableWidget>


class apoBaseExeNode;
class apoUiMainEditor;



namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:

    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

	bool myInit();
	bool showRuntimeError(const char *scriptFile, const char *errNodeInfo);
	
	void ClearLog();
	void WriteLog(const char *logText);

protected:
	enum eSelNodeStat{ E_SELECTED_NODE_ERROR, E_SELECTED_NODE_DEBUG};
	bool getStackFromName(const char *nodeInfo, stackIndex_vct &stackIndex);
	void showAllView();
	bool setFileRoot(const char *rootFile);
	bool setDefaultFile(const char *lastEditfile);
	bool showCompileError(const char *xmlFile, stackIndex_vct &errorStack);
	bool showDebugNode(const char *nodeInfo);
	void setCurFileSave(bool isSaved=false);

public slots:
	void onBreakPointEdited(const char *function, const char *node, bool isAdd);
	void onShowExeNodeDetail(apoBaseExeNode *exenode);
	void onFileChanged();
	void onXmlNodeDel(ndxml *xmlnode);
	void onFilelistDel(ndxml *xmlnode);
	void onFunctionListChanged(ndxml *xmlnode);
	void onFileModuleNameChanged(ndxml *xmlnode);

	// debugger 
	void onDebugTerminate();
	void onDebugStep(const char *func, const char* node);
	void onDebugCmdAck();
	void onScriptRunOK();

private slots:

    void on_actionViewList_triggered();
    void on_actionViewOutput_triggered();
    void on_actionFunctionView_triggered();
    void on_actionDetail_triggered();
    void on_actionFileNew_triggered();
    void on_actionFileOpen_triggered();
	void on_actionFileClose_triggered();
    void on_actionCompile_triggered();
    void on_actionSave_triggered();
	void on_actionExit_triggered();
	void on_actionCompileAll_triggered();
	void on_actionClearLog_triggered();
	void on_actionRun_triggered();
	void on_actionTest_triggered();
	void on_actionCancel_scale_triggered();

	void on_actionRunDebug_triggered();
    void on_actionStepOver_triggered();
	void on_actionStepIn_triggered();
	void on_actionContinue_triggered();
	void on_actionStepOut_triggered();
	void on_actionAttach_triggered();
	void on_actionDeattch_triggered();

	void onFunctionsTreeCurItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous);
	void onFilesTreeCurItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous);


    void on_actionExit_without_save_triggered();

protected:

	bool openFileView();
	bool openFunctionsView();
	bool openOutputView();
	bool openDetailView();

	bool showCommonDeatil(ndxml *xmldata);
	bool showDebugInfo(ndxml *xmldata);
	void closeDebugInfo();
	void closeDetail();
	bool runFunction(const char *binFile,const char *srcFile, int argc, const char* argv[]);
	bool compileScript(const char *scriptFile, std::string &outFile, bool bWithRun = false, bool bDebug=false,int outEncodeType= -1);
	bool StartDebug(const char *binFile, const char *srcFile, int argc, const char* argv[]);
	void EndDebug(bool bSuccess);
	bool Run(bool bIsDebug);

	bool loadScriptFromModule(const char *moduleName);
	bool loadScriptFile(const char *scriptFile);
	const char *getScriptSetting(ndxml *scriptXml, const char *settingName);
	bool showCurFile();
	bool showFileslist();
	bool showCurFunctions();

	bool saveCurFile();
	bool checkNeedSave();
	void closeCurFile();
	void closeCurFunction();

	bool isDebugging() { return m_debuggerCli != NULL; }

	void onDebugStart();
	void onDebugEnd();

	void onAttached(const char *moduleName);
	void onDeattched();


	//ndxml_root *m_config; //editor cconfig 
	bool m_isChangedCurFile;
	std::string m_fileRootPath;
	ndxml_root *m_fileRoot;

	std::string m_fileMoudleName;	//current edit file moudle name 
	std::string m_filePath;			//current edited file path
	ndxml_root *m_curFile;			//current edited file 
	ndxml *m_currFunction;			//current edited function 
	ndxml *m_debugInfo;
	
	std::string m_fileTemplate;		// new-file template
	std::string m_confgiFile;		// config file-path
private:

    Ui::MainWindow *ui;
	
	QString m_logText;
	apoUiMainEditor *m_editorWindow;
	apoEditorSetting &m_editorSetting;

	DeguggerScriptOwner *m_localDebugOwner;

	ApoDebugger *m_debuggerCli;
};

#endif // MAINWINDOW_H
