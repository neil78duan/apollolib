#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "nd_common/nd_common.h"
#include "apoScript/apoEditorSetting.h"
#include "logic_parser/logic_compile.h"

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
	void showAllView();
	bool setFileRoot(const char *rootFile);
	bool setDefaultFile(const char *lastEditfile);
	bool showCompileError(const char *xmlFile, stackIndex_vct &errorStack);
	void setCurFileSave(bool isSaved=false);

public slots:
	void onShowExeNodeDetail(apoBaseExeNode *exenode);
	void onFileChanged();
	void onXmlNodeDel(ndxml *xmlnode);
	void onFilelistDel(ndxml *xmlnode);
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


	void onFunctionsTreeCurItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous);
	void onFilesTreeCurItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous);

	void on_actionCompileAll_triggered();

protected:

	bool openFileView();
	bool openFunctionsView();
	bool openOutputView();
	bool openDetailView();

	bool showCommonDeatil(ndxml *xmldata);
	void closeDetail();
	bool compileScript(const char *scriptFile);

	bool loadScriptFile(const char *scriptFile);
	const char *getScriptSetting(ndxml *scriptXml, const char *settingName);
	bool showCurFile();
	bool showFileslist();
	bool showCurFunctions();

	bool saveCurFile();
	bool checkNeedSave();
	void closeCurFile();

	//QWidget *getMainEditor();

	//bool loadUserdefDisplayList(ndxml_root &xmlNameList, const char *name);


	//ndxml_root *m_config; //editor cconfig 
	bool m_isChangedCurFile;
	std::string m_fileRootPath;
	ndxml_root *m_fileRoot;

	std::string m_fileMoudleName;	//current edit file moudle name 
	std::string m_filePath;			//current edited file path
	ndxml_root *m_curFile;			//current edited file 
	ndxml *m_currFunction;			//current edited function 
	
	std::string m_fileTemplate;		// new-file template
	//std::string m_messageFile ;		// message file-path
	std::string m_confgiFile;		// config file-path
private:

    Ui::MainWindow *ui;
	
	QString m_logText;
	apoUiMainEditor *m_editorWindow;
	apoEditorSetting &m_editorSetting;
};

#endif // MAINWINDOW_H
