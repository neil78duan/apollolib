
// apolloParseEditorDlg.h : ͷ�ļ�
//

#pragma once

#define CONFIG_FILE_PATH "../cfg/editor_config_setting.json"
#define CONFIG_IO_SETTING "../cfg/io_config.xml"

#include "nd_common/nd_common.h"
#include "logic_parser/logic_compile.h"
class DBLDatabase;
// CapolloParseEditorDlg �Ի���
class CapolloParseEditorDlg : public CDialogEx
{
// ����
public:
	CapolloParseEditorDlg(CWnd* pParent = NULL);	// ��׼���캯��

	void LogText(const char* text);
	void clearLog();
// �Ի�������
	enum { IDD = IDD_APOLLOPARSEEDITOR_DIALOG };

	//CString m_filePath;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��
	
	const char *getScriptSetting(ndxml *scriptXml, const char *settingName);
	int getScriptExpEncodeType(ndxml *scriptXml);
	bool getScriptExpDebugInfo(ndxml *scriptXml);
	const char *getScriptEntry();

	//get file 
	//const char *getGameScript();
	//const char *getGameScriptOut();
	//const char *getCppFuncList();
	//const char *getEventList();
	const char *getNetProtocol();

	//const char *getGameDataInPath();
	//const char *getGameDataList();
	//const char *getGameDataOutfile();
	//const char *getExcelExportEncodeType();
	const char *getGameDateEncodeType();
	
	const char *_getFromIocfg(const char *cfgName);
	bool _loadUserDefEnum(const char *userDefEnumFile, void *pDlg);
	bool compile();
	bool compileScript(const char *scriptFile);
	bool expLua(const char *outPath, const DBLDatabase &db);
	bool expExcel();
	bool runTest();
	void _beginEdit(const char *filename);
	//bool GetFile(bool bOpen, CString & strFile);
	logciCompileSetting m_compileSetting;
	ndxml_root &m_editor_setting;
	//ndxml_root &m_io_setting;

// ʵ��
protected:
	HICON m_hIcon;
	bool m_newLine;
	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	//afx_msg void OnBnClickedBtFile();
	afx_msg void OnBnClickedBtEdit();
	afx_msg void OnBnClickedBtCompile();
	//afx_msg void OnBnClickedBtOuttFile();
	afx_msg void OnBnClickedBtLoaderTest();
	//afx_msg void OnBnClickedBtClearLog();
	afx_msg void OnBnClickedBtExpData();
	afx_msg void OnBnClickedBtSetting();
	afx_msg void OnDestroy();
//	afx_msg void OnBnClickedBtPackageData();
	afx_msg void OnBnClickedBtAllExport();
	afx_msg void OnBnClickedBtRunGm();
};
