//#include "mainwindow.h"

#include "workdirdialog.h"
#include "nd_common/nd_common.h"

#include <QApplication>
#include "startdialog.h"
#include <QMessageBox>

#include <qtextcodec.h>
#include <QString>

#ifdef WIN32
#pragma comment(lib,"User32.lib")
#pragma comment(lib,"Advapi32.lib")
#endif

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
#ifdef WIN32 

	QTextCodec::setCodecForLocale(QTextCodec::codecForName("GB18030"));
#endif 
    //QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));

    QString workDir, ioCfgFile , editorCfg;
    if (!trytoGetSetting(workDir, ioCfgFile, editorCfg)) {
        return 1 ;
    }
	
    startDialog dlg ;
re_initd:
    nd_chdir(workDir.toStdString().c_str()) ;
	if (!dlg.initXmlSetting(editorCfg.toStdString().c_str(), ioCfgFile.toStdString().c_str())) {
        QMessageBox::warning(NULL, "Error","load config file error, Please Reload",QMessageBox::Yes);
        if(!inputSetting(workDir, ioCfgFile,editorCfg)) {
            exit(1) ;
        }
        goto re_initd;
    }
    dlg.show();
    return a.exec() ;
}

