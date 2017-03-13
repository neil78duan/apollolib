//#include "mainwindow.h"

#include "workdirdialog.h"
#include "nd_common/nd_common.h"
#include "apoScript/apoEditorSetting.h"

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
	//use utf8 
	ndstr_set_code(APO_QT_SRC_TEXT_ENCODE);
// #ifdef WIN32 
// 	QTextCodec::setCodecForLocale(QTextCodec::codecForName("GB18030"));
// 	ndstr_set_code(E_SRC_CODE_GBK);
// #else 
// 	QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));
// 	ndstr_set_code(E_SRC_CODE_UTF_8);
// #endif 
    //

    QString workDir, ioCfgFile , editorCfg;

    if (!trytoGetSetting(workDir, ioCfgFile, editorCfg)) {
        return 1 ;
    }
	apoEditorSetting* setting;

re_initd:
	nd_chdir(workDir.toStdString().c_str());

	setting = apoEditorSetting::getInstant(); 
	nd_assert(setting);

	if (!setting->Init(ioCfgFile.toStdString().c_str(), editorCfg.toStdString().c_str(), APO_QT_SRC_TEXT_ENCODE)){
		QMessageBox::warning(NULL, "Error", "load config file error, Please Reload", QMessageBox::Yes);
		if (!inputSetting(workDir, ioCfgFile, editorCfg)) {
			exit(1);
		}
		apoEditorSetting::destroyInstant();
		goto re_initd;
	}

    startDialog dlg ;

// 
// 	if (!dlg.initXmlSetting(editorCfg.toStdString().c_str(), ioCfgFile.toStdString().c_str())) {
//         QMessageBox::warning(NULL, "Error","load config file error, Please Reload",QMessageBox::Yes);
//         if(!inputSetting(workDir, ioCfgFile,editorCfg)) {
//             exit(1) ;
//         }
//         goto re_initd;
//     }

    dlg.show();
    return a.exec() ;
}

