
// apolloParseEditor.h : PROJECT_NAME 应用程序的主头文件
//

#pragma once

#ifndef __AFXWIN_H__
	#error "在包含此文件之前包含“stdafx.h”以生成 PCH 文件"
#endif

#include "resource.h"		// 主符号


// CapolloParseEditorApp: 
// 有关此类的实现，请参阅 apolloParseEditor.cpp
//
int out_log( const char *text);
int out_print(void *pf, const char *stm, ...);

class CapolloParseEditorApp : public CWinApp
{
public:
	CapolloParseEditorApp();

// 重写
public:
	virtual BOOL InitInstance();
	CString getDefaultFile();
	void setDefaultFile(LPCTSTR  filepath);

	CString getOutputFile();
	void setOutputFile(LPCTSTR  filepath);

	void setDefaultSetting(LPCTSTR name, LPCTSTR value);
	CString getDefaultSetting(LPCTSTR name, LPCTSTR dftVal=NULL);


	const char* getGetCPPFunclist();

// 实现

	DECLARE_MESSAGE_MAP()
};

extern CapolloParseEditorApp theApp;