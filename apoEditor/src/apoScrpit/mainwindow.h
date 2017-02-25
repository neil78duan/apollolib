#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "nd_common/nd_common.h"
#include "apoScrpit/apoEditorSetting.h"

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

	void showAllView();
	//bool loadUserDefEnum(const char *userDefEnumFile);
	//bool addDisplayNameList(const char *name, text_vct_t &text_list);

	void setNewFileTempl(const char *newfiletempl) { m_fileTemplate = newfiletempl; }
	bool setConfig(const char *cfgFile);
	bool setFileRoot(const char *rootFile);
	

	void ClearLog();
	void WriteLog(const char *logText);

	QTableWidget *getDetailWidget();

public slots:
	void onShowExeNodeDetail(apoBaseExeNode *exenode);
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

	void onFilesTreeItemDBClicked(QTreeWidgetItem *item, int column);
	void onFunctionsTreeItemDBClicked(QTreeWidgetItem *item, int column);
protected:

	bool openFileView();
	bool openFunctionsView();
	bool openOutputView();
	bool openDetailView();

	bool showCommonDeatil(ndxml *xmldata);

	bool loadScriptFile(const char *scriptFile);
	const char *getScriptSetting(ndxml *scriptXml, const char *settingName);
	bool showCurFile();
	bool showCurFunctions();

	bool saveCurFile(){ return true; }
	bool checkNeedSave(){ return true; }

	QWidget *getMainEditor();

	//bool loadUserdefDisplayList(ndxml_root &xmlNameList, const char *name);


	//ndxml_root *m_config; //editor cconfig 
	ndxml_root *m_fileRoot;

	ndxml_root *m_curFile;
	ndxml *m_currFunction;
	
	std::string m_fileTemplate;
private:

    Ui::MainWindow *ui;
	
	QString m_logText;
	apoUiMainEditor *m_editorWindow;

	apoEditorSetting m_editorSetting;
};

#endif // MAINWINDOW_H
