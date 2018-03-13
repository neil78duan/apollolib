
// apolloParseEditor.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CapolloParseEditorApp: 
// �йش����ʵ�֣������ apolloParseEditor.cpp
//
int out_log( const char *text);
int out_print(void *pf, const char *stm, ...);

class CapolloParseEditorApp : public CWinApp
{
public:
	CapolloParseEditorApp();

// ��д
public:
	virtual BOOL InitInstance();
	CString getDefaultFile();
	void setDefaultFile(LPCTSTR  filepath);

	CString getOutputFile();
	void setOutputFile(LPCTSTR  filepath);

	void setDefaultSetting(LPCTSTR name, LPCTSTR value);
	CString getDefaultSetting(LPCTSTR name, LPCTSTR dftVal=NULL);


	const char* getGetCPPFunclist();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CapolloParseEditorApp theApp;