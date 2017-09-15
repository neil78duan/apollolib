//#include "mainwindow.h"

#include "workdirdialog.h"
#include "nd_common/nd_common.h"
#include "apoScript/apoEditorSetting.h"
#include "connectdialog.h"

#include <QApplication>
#include "startdialog.h"
#include <QMessageBox>

#include <qtextcodec.h>
#include <QString>

#ifdef WIN32
#pragma comment(lib,"User32.lib")
#pragma comment(lib,"Advapi32.lib")
#endif


int runDevelopTool(int argc, char *argv[])
{
	QApplication a(argc, argv);
	//use utf8 
	ndstr_set_code(APO_QT_SRC_TEXT_ENCODE);

	QString workDir, ioCfgFile, editorCfg;

	if (!trytoGetSetting(workDir, ioCfgFile, editorCfg)) {
		return 1;
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

	startDialog dlg;

	dlg.show();
	return a.exec();
}



#define _LOAD_XML(_xml_name, _filename,_encode,_isexit) \
    ndxml_root _xml_name;				\
    ndxml_initroot(&_xml_name);			\
    if (ndxml_load_ex((char*)_filename, &_xml_name,_encode)) {	\
        nd_logerror("open file %s error", _filename);	\
        if(_isexit) return 1;							\
		    }

int runGm(int argc, char *argv[])
{
	QApplication a(argc, argv);
	//use utf8 
	ndstr_set_code(APO_QT_SRC_TEXT_ENCODE);

	ConnectDialog dlg;

	_LOAD_XML(editorSetting, "../../cfg/editor_setting.xml", "utf8", 0);
	_LOAD_XML(xmlSend, "../../cfg/gm_msg.xml", "utf8", 0);

	dlg.m_editor_setting = &editorSetting;
	dlg.m_gmCfg = &xmlSend;

	dlg.LoadClientScript("../../data/client_script.bin","../../data/cehua_data.dat");


	dlg.exec();

	ndxml_save_encode(&xmlSend, "../../cfg/gm_msg.xml", E_SRC_CODE_UTF_8, E_SRC_CODE_UTF_8);
	ndxml_destroy(&xmlSend);

	//WriteLog("...\n connect server end!");

	dlg.show();
	return a.exec();
}

int main(int argc, char *argv[])
{
	
	const char *nowDir = nd_getcwd();

	for (int i = 1; i < argc ; i++){
		if (0 == ndstricmp(argv[i], "--rungmtool"))	{
			return runGm( argc, argv);
		}
		else if (0 == ndstricmp(argv[i], "--runEditor"))	{
			return runDevelopTool(argc, argv);
		}
	}
#if defined (__ND_MAC__)
    return runDevelopTool(argc, argv);
#endif
	fprintf(stderr, "unknow run mode \n");
	exit(1);
}
