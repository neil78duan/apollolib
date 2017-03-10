/* 
 * file apoEditorSetting. h
 *
 * settings of editor 
 *
 * create by duan 
 *
 * 2017.2.20
 */

#ifndef _APO_EDITOR_SETTING_H_
#define _APO_EDITOR_SETTING_H_


#include "logic_parser/logicDataType.h"
#include "logic_parser/logic_compile.h"
#include "logic_parser/logic_editor_helper.h"

#include <string>

#include <QString>
#include <QWidget>


#define MY_LOAD_XML_AND_NEW(_xml_addr, _filename, _returnval) \
	_xml_addr = new ndxml_root;			\
	ndxml_initroot(_xml_addr);			\
	if (ndxml_load_ex((char*)_filename, _xml_addr,"utf8")) {	\
		nd_logerror("open file %s error", _filename);	\
		_returnval ;							\
	}

#define MY_DESTROY_XML(_xml_addr) \
	do 	{						\
		if(_xml_addr) {			\
			ndxml_destroy(_xml_addr) ;\
			delete _xml_addr ;	\
		}						\
	} while (0)

class apoEditorSetting :public logciCompileSetting
{
	apoEditorSetting();
	~apoEditorSetting();

public:
	static apoEditorSetting*getInstant();
	static void destroyInstant();

	bool Init(const char *ioSetting, const char *editorSetting, int encodeType = E_SRC_CODE_UTF_8);
	
	//bool loadProgramIOSetting(const char *ioSettingFile);//load this program base setting (io_config.xml)
	const char *getIoConfigValue(const char *cfgName);
	virtual bool onLoad(ndxml &cfgRoot);

	const char *getExeNodeName(const char *xmlNodeName);

	bool loadSettingFromScript(ndxml *m_scriptFile); //load enum list from current script
	void destroyScriptSetting();

	LogicEditorHelper::CXMLAlias &getAlias() { return m_alias; }
	ndxml_root *getIoConfig() { return m_ioXmlCfg; }

	typedef std::vector<QString> text_vct_t;
	bool addDisplayNameList(const char *name, apoEditorSetting::text_vct_t &text_list);

	const text_vct_t* getUserdefList(const char *name)const;		//get enum-table from name 

	const char *getDisplayAction(const char *xmlname); //check is display function-blue-print of show detail

	ndxml_root *getConfig() { return &m_configRoot; }

	ndxml* AddNewXmlNode(ndxml *node, QWidget *parentWindow);//create new xml node from template @need show dialog and get user selected

	bool loadUserDefEnum(const char *userDefEnumFile);
	bool loadMessageDefine(const char *messageFile) ;

public:
	std::string m_edirotSettingFile;
	std::string m_ioConfigFile;
private:

	bool loadUserdefDisplayList(ndxml_root &xmlNameList, const char *name);

	const char *_getScriptSetting(ndxml *scriptXml, const char *settingName);

	ndxml_root *m_ioXmlCfg;
	LogicEditorHelper::CXMLAlias m_exeNodeName; //exenode and uinode name table
	LogicEditorHelper::CXMLAlias m_alias;


	typedef std::map<std::string, text_vct_t > text_map_t;
	text_map_t m_userDefineList;		//enum list
	
	//ndxml_root *m_fileRoot; //point to editor_setting.xml

	static apoEditorSetting* s_setting;
};

#endif 
