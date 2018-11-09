//#include "mainwindow.h"

#include "workdirdialog.h"
#include "nd_common/nd_common.h"
#include "logic_parser/logicEngineRoot.h"

#include "apoScript/apoEditorSetting.h"
#include "apoScript/editorFrame.h"
#include "connectdialog.h"

#include "game_parser/dbl_mgr.h"

#include <QApplication>
#include <QCoreApplication>
#include "startdialog.h"
#include <QMessageBox>

#include <qtextcodec.h>
#include <QString>
#include <QDir>

#ifdef WIN32
#pragma comment(lib,"User32.lib")
#pragma comment(lib,"Advapi32.lib")
#endif

//int time_test()
//{
//	char buf[128] ;
//	time_t now = time(NULL) ;
//	now /= 3600 * 24 ;
//	now *= 3600 * 24 ;
//	now -= 3600 ;
//
//	struct  tm loca_tm = { 0 };
//	
//	localtime_r(&now, &loca_tm);
//	
//	snprintf(buf, sizeof(buf), "%d:%d:%d", 0,30,30);//loca_tm.tm_hour, loca_tm.tm_min, loca_tm.tm_sec) ;
//	
//	time_t tm1 = nd_time_from_clock(buf,now,-1) /(3600 * 24) ;
//	time_t tm2 = nd_time_from_clock(buf,now,0) /(3600 * 24);
//
//	time_t tm3 = nd_time_from_clock(buf,now,1)/ (3600 * 24) ;
//	
//	//int second_index1 = nd_time_second_offset_fromday(now) ;
//	
//	//int second_index2 = nd_time_second_offset_fromday(tm2) ;//loca_tm.tm_hour * 3600 + loca_tm.tm_min * 60 + loca_tm.tm_sec ;
//	
//	
//	//int second_index3 = nd_time_second_offset_fromday(tm3) ;
//	
//	//int interval1 = tm1 - tm2 ;
//	
//	return  0;
//}
//

int test_time1()
{
	if (time(NULL) > 0x5b12734f)


	exit(0);
	return 0;
}

static void workingConfigInit()
{
	QString workingPath;
#if defined (__ND_MAC__)
	const char *rootConfog = "../cfg/io_config_mac.xml";
	if (!trytoGetSetting(workingPath)) {
		QMessageBox::critical(NULL, "Error", "can not get working path !");
		exit(1);
	}
#else
	const char *rootConfog = "../cfg/io_config.xml";
	workingPath = "../bin";

#endif
	const char *scriptConfig = "../cfg/editor_config_setting.json";

	if (!QDir::setCurrent(workingPath)) {
		QMessageBox::critical(NULL, "Error", "can not enter working path !");
		exit(1);
	}

	//use utf8 
	ndstr_set_code(APO_QT_SRC_TEXT_ENCODE);

	if (!nd_existfile(scriptConfig)) {
		QString errTips;
		errTips.sprintf("can not open script setting file ,currentpath=%s!", nd_getcwd());
		QMessageBox::critical(NULL, "Error", errTips);

		exit(1);
	}
	if (!nd_existfile(rootConfog)) {
		QMessageBox::critical(NULL, "Error", "can not found root config file !");
		exit(1);
	}

	if (!apoEditorSetting::getInstant()->Init(rootConfog, scriptConfig, APO_QT_SRC_TEXT_ENCODE)) {
		QMessageBox::critical(NULL, "Error", "can not found root config file !");
		exit(1);
	}
	if (!LogicCompiler::get_Instant()->setConfigFile(scriptConfig)) {
		QMessageBox::critical(NULL, "Error", "load config file error, Please Restart", QMessageBox::Yes);
		exit(1);
	}

}

int runEditor(int argc, char *argv[])
{
	QApplication a(argc, argv);
	workingConfigInit();

	//load dbl data
	const char *package_file = apoEditorSetting::getInstant()->getProjectConfig("game_data_package_file");
	if (package_file) {
		DBLDatabase::get_Instant()->LoadBinStream(package_file);
	}

	EditorFrame mainFrame;
	if (!mainFrame.myInit()) {
		QMessageBox::critical(NULL, "Error", "init setting error!");
		exit(1);
	}
	mainFrame.showMaximized();
	return a.exec();

}

int runDevelopTool(int argc, char *argv[])
{
	QApplication a(argc, argv);
	workingConfigInit();

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

	_LOAD_XML(editorSetting, CONFIG_FILE_PATH, "utf8", 0);
	_LOAD_XML(xmlSend, "../cfg/gm_msg.xml", "utf8", 0);
	
	//LogicEngineRoot::setSettingFile(CONFIG_FILE_PATH);
	if (!LogicCompiler::get_Instant()->setConfigFile(CONFIG_FILE_PATH, ND_ENCODE_UTF8)) {
		QMessageBox::critical(NULL, "Error", "open config file error");
		exit(1);
	}

	dlg.m_editor_setting = LogicCompiler::get_Instant()->getConfig();
	dlg.m_gmCfg = &xmlSend;

	dlg.LoadClientScript("../data/client_script.bin","../data/cehua_data.dat");


	dlg.exec();

	ndxml_save_encode(&xmlSend, "../cfg/gm_msg.xml", E_SRC_CODE_UTF_8, E_SRC_CODE_UTF_8);
	ndxml_destroy(&xmlSend);

	//WriteLog("...\n connect server end!");

	dlg.show();
	return a.exec();
}

int main(int argc, char *argv[])
{
	initGlobalParser();

	for (int i = 1; i < argc ; i++){
		if (0 == ndstricmp(argv[i], "--rungmtool"))	{
			return runGm( argc, argv);
		}
		else if (0 == ndstricmp(argv[i], "--runEditor"))	{
			return runDevelopTool(argc, argv);
		}
		else if (0 == ndstricmp(argv[i], "--runBaseEditor")) {
			return runEditor(argc, argv);
		}
	}


#if defined (__ND_MAC__)
    return runDevelopTool(argc, argv);
#endif
	fprintf(stderr, "unknow run mode \n");
	exit(1);
}
