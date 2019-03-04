//#include "mainwindow.h"

#include "apoScript/apoEditorSetting.h"
#include "apoScript/editorFrame.h"
#include "apoScript/workdirdialog.h"
#include "connectdialog.h"


#include <QApplication>
#include <QCoreApplication>
#include "startdialog.h"
#include <QMessageBox>

#include <qtextcodec.h>
#include <QString>
#include <QDir>

#include "nd_common/nd_common.h"
#include "logic_parser/logicEngineRoot.h"
#include "game_parser/dbl_mgr.h"

#ifdef WIN32
#pragma comment(lib,"User32.lib")
#pragma comment(lib,"Advapi32.lib")
#endif

#define CONFIG_FILE_PATH "../cfg/editor_config_setting.json"
#ifdef __MAC_OS__
#define CONFIG_IO_SETTING "../cfg/io_config_mac.xml"
#else
#define CONFIG_IO_SETTING "../cfg/io_config.xml"
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

static const char *__in_working_path = NULL;
static const char *__in_config = NULL;
static const char *__in_proj_path = NULL;
static int __run_type = 0;		// 0 develop tool 1 gmtool 2 base-editor

static bool workingConfigInit(QString &init_working);

static bool initWorkingPath()
{
	if (__in_working_path && *__in_working_path) {
		char tmpbuf[ND_FILE_PATH_SIZE];
		
		if (!nd_absolute_filename(__in_working_path, tmpbuf, sizeof(tmpbuf))) {

			QMessageBox::critical(NULL, "Error", "Get wroking path error !");
			return false;
		}

		if (-1 == nd_chdir(tmpbuf)) {
			QMessageBox::critical(NULL, "Error", "Switch wroking path error !");
			return false;
		}

	}

	if (__in_config && *__in_config) {		
		if (!nd_existfile(__in_config)) {
			QMessageBox::critical(NULL, "Error", "can not found config file !");
			return false;
		}

		if (!apoEditorSetting::getInstant()->Init(__in_config, APO_QT_SRC_TEXT_ENCODE)) {
			QMessageBox::critical(NULL, "Error", "Set editor config error");
			return false;
		}
	}
	else {
		__in_config = "../cfg/config.xml"; //set default config file

		if (__in_working_path) {
			if (!apoEditorSetting::getInstant()->Init(__in_config, APO_QT_SRC_TEXT_ENCODE)) {
				QMessageBox::critical(NULL, "Error", "Set editor config error");
				return false;
			}
		}
		else {
			QString workingPath;
			while (!workingConfigInit(workingPath)) {
				if (trytoGetSetting(workingPath) && workingConfigInit(workingPath)) {
					break;
				}

				//GET working path
				if (!inputSetting(workingPath, NULL)) {
					exit(1);
				}
				QDir::setCurrent(workingPath);
			}
			return true;
		}

	}
	
	return true;
}

static bool workingConfigInit(QString &init_working)
{
	if (init_working.size()) {
		if (-1 == nd_chdir(init_working.toStdString().c_str())) {
			return false;
		}
	}
	else {
		const char *pCurPath = nd_getcwd();
		if (ndstrstr(pCurPath, "editor")) {
			nd_chdir("..");
		}
	}


	if (!nd_existfile(__in_config)) {
		return false;
	}

	if (!apoEditorSetting::getInstant()->Init(__in_config, APO_QT_SRC_TEXT_ENCODE)) {
		return false;
	}

	return true;
}

int runEditor(int argc,  char *argv[])
{
	QApplication a(argc, argv);
	//workingConfigInit();
	initWorkingPath() ;

	if (__in_proj_path) {
		nd_chdir(__in_proj_path);
	}
	//load dbl data
// 	const char *package_file = apoEditorSetting::getInstant()->getValueFromSetting("game_data_package_file");
// 	if (package_file) {
// 		DBLDatabase::get_Instant()->LoadBinStream(package_file);
// 	}

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

	startDialog dlg;
	if (__in_proj_path) {
		char tmpbuf[ND_FILE_PATH_SIZE];
		if (!nd_absolute_filename(__in_proj_path, tmpbuf, sizeof(tmpbuf))) {
			QMessageBox::critical(NULL, "Error", "init project path error!");
			return 1;
		}
		dlg.setProjectPath(tmpbuf);
	}

	initWorkingPath();

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

int runGm(int argc,  char *argv[])
{
    QApplication a(argc, argv);
	//use utf8 
	ndstr_set_code(APO_QT_SRC_TEXT_ENCODE);
	initWorkingPath();

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


int parser_args(int argc, char *argv[])
{
	for (int i = 1; i < argc; i++) {
		if (0 == ndstricmp(argv[i], "--runEditor")) {
			__run_type = 0;
		}
		else if (0 == ndstricmp(argv[i], "--rungmtool")) {
			__run_type = 1;
		}
		else if (0 == ndstricmp(argv[i], "--runBaseEditor")) {
			__run_type = 2;
		}
		else if (0 == ndstricmp(argv[i], "--workdir")) {
			__in_working_path = argv[++i];
		}
		else if (0 == ndstricmp(argv[i], "--porjdir")) {
			__in_proj_path = argv[++i];
		}
		else if (0 == ndstricmp(argv[i], "-f")) {
			__in_config = argv[++i];
		}
	}
	return 0;
}



int main(int argc, char *argv[])
{
	initGlobalParser();

	setRegisterRootName("apoEditor");

	if (argc == 1) {
		return runDevelopTool(argc, argv);
	}

	parser_args(argc, argv);
 
	if (__run_type == 1) {
		return runGm(argc, argv);
	}
	else if (__run_type == 2) {
		return runEditor(argc, argv);
	}

	else  {
		return runDevelopTool(argc, argv);
	}


}
