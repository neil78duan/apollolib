#ifndef STARTDIALOG_H
#define STARTDIALOG_H

#include <QDialog>
#include <string>

#include "nd_common/nd_common.h"

namespace Ui {
class startDialog;
}

class DBLDatabase;
class dialogCloseHelper
{
public:
    dialogCloseHelper(QDialog *dlg) {
        dlg->setHidden(true);
        m_dlg = dlg ;
    }
    ~dialogCloseHelper()
    {
        if(m_dlg) {

            m_dlg->setVisible(true);
            m_dlg->activateWindow();
        }
    }
private:
    QDialog *m_dlg ;
    //bool m_oldStat ;
};

class startDialog : public QDialog
{
    Q_OBJECT

public:
    explicit startDialog(QWidget *parent = 0);
    ~startDialog();

    void ClearLog() ;
    void WriteLog(const char *logText) ;
	void setProjectPath(const char *Path);
	//bool InitConfigFile(const char *ioConfig, const char *scriptSetting);
	
private slots:
    void on_Exit_clicked();

    void on_Connect_clicked();

    void on_ScriptEdit_clicked();

    void on_Setting_clicked();

    void on_Compile_clicked();

    void on_ExportExcel_clicked();

    void on_CompleteAll_clicked();

    void on_WorkingPath_clicked();

private:

	bool showScriptError(const char *scriptFile, const char *nodeDescript, const char *editorWorkingPath);
	const char *getScriptSetting(ndxml *scriptXml, const char *settingName);
	int getScriptExpEncodeType(ndxml *scriptXml);
	bool getScriptExpDebugInfo(ndxml *scriptXml);
	
    const char *getGameDateEncodeType();

    const char *_getFromIocfg(const char *cfgName);
	bool compileScript(const char *scriptFile,const char *editorWorkingPath);
	bool compile();

    bool runTest();
    bool expExcel();
	bool loadDataBase();

	bool expLua(const char *outPath, const DBLDatabase &db);

	std::string getPathFromProject(const char *relativePath);
	std::string getPathFromConfig(const char *configName);
    Ui::startDialog *ui;

    //ndxml_root &m_editor_setting;
    //ndxml_root &m_io_setting;

	std::string editorConfigFile ;
	std::string ioConfigFile;
	std::string m_projectPath;

};

#endif // STARTDIALOG_H
