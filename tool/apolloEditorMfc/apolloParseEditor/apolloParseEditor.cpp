
// apolloParseEditor.cpp : ����Ӧ�ó��������Ϊ��
//

#include "stdafx.h"
#include "apolloParseEditor.h"
#include "apolloParseEditorDlg.h"
#include "nd_common/nd_common.h"
#include "cli_common/login_apollo.h"
#include "apollo_errors.h"

//#include <vld.h>
#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CapolloParseEditorApp

BEGIN_MESSAGE_MAP(CapolloParseEditorApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CapolloParseEditorApp ����

CapolloParseEditorApp::CapolloParseEditorApp()
{
	// ֧����������������
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_RESTART;

	// TODO:  �ڴ˴���ӹ�����룬
	// ��������Ҫ�ĳ�ʼ�������� InitInstance ��
}


// Ψһ��һ�� CapolloParseEditorApp ����

CapolloParseEditorApp theApp;


void _xml_error_output(const char *errdesc)
{
	AfxMessageBox((LPCTSTR)errdesc);
}

void out_log(const char *text)
{
	CapolloParseEditorDlg *pWnd = (CapolloParseEditorDlg *)AfxGetMainWnd();
	if (pWnd){
		pWnd->LogText(text);
	}
}

int out_print(void *pf, const char *stm, ...)
{
	char buf[1024 * 4];
	char *p = buf;
	va_list arg;
	int done;

	va_start(arg, stm);
	done = vsnprintf(p, sizeof(buf), stm, arg);
	va_end(arg);

	out_log(buf);
	return done;
}
// CapolloParseEditorApp ��ʼ��

BOOL CapolloParseEditorApp::InitInstance()
{
	// ���һ�������� Windows XP �ϵ�Ӧ�ó����嵥ָ��Ҫ
	// ʹ�� ComCtl32.dll �汾 6 ����߰汾�����ÿ��ӻ���ʽ��
	//����Ҫ InitCommonControlsEx()��  ���򣬽��޷��������ڡ�
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// ��������Ϊ��������Ҫ��Ӧ�ó�����ʹ�õ�
	// �����ؼ��ࡣ
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();

	nd_common_init();
	nd_setxml_log(_xml_error_output);
	nd_log_no_file(1);
	nd_log_no_time(1);
	ndstr_set_code(E_SRC_CODE_GBK);
	// ���� shell ���������Է��Ի������
	// �κ� shell ����ͼ�ؼ��� shell �б���ͼ�ؼ���
	CShellManager *pShellManager = new CShellManager;

	// ���Windows Native���Ӿ����������Ա��� MFC �ؼ�����������
	CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows));

	// ��׼��ʼ��
	// ���δʹ����Щ���ܲ�ϣ����С
	// ���տ�ִ���ļ��Ĵ�С����Ӧ�Ƴ�����
	// ����Ҫ���ض���ʼ������
	// �������ڴ洢���õ�ע�����
	// TODO:  Ӧ�ʵ��޸ĸ��ַ�����
	// �����޸�Ϊ��˾����֯��
	SetRegistryKey(_T("apolloEditor"));

	CapolloParseEditorDlg dlg;
	m_pMainWnd = &dlg;

	nd_setlog_func(out_log);
	nd_register_error_convert(apollo_error);

	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO:  �ڴ˷��ô����ʱ��
		//  ��ȷ�������رնԻ���Ĵ���
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO:  �ڴ˷��ô����ʱ��
		//  ��ȡ�������رնԻ���Ĵ���
	}
	else if (nResponse == -1)
	{
		TRACE(traceAppMsg, 0, "����: �Ի��򴴽�ʧ�ܣ�Ӧ�ó���������ֹ��\n");
		TRACE(traceAppMsg, 0, "����: ������ڶԻ�����ʹ�� MFC �ؼ������޷� #define _AFX_NO_MFC_CONTROLS_IN_DIALOGS��\n");
	}

	// ɾ�����洴���� shell ��������
	if (pShellManager != NULL)
	{
		delete pShellManager;
	}

	// ���ڶԻ����ѹرգ����Խ����� FALSE �Ա��˳�Ӧ�ó���
	//  ����������Ӧ�ó������Ϣ�á�

	nd_common_release();
	return FALSE;
}


CString CapolloParseEditorApp::getDefaultFile()
{
	return GetProfileString(_T("setting"), _T("defaultFilePath"), "gamedata.xml");
}
void CapolloParseEditorApp::setDefaultFile(LPCTSTR  filepath)
{
	WriteProfileString(_T("setting"), _T("defaultFilePath"), filepath);
}

CString CapolloParseEditorApp::getOutputFile()
{
	return GetProfileString(_T("setting"), _T("defaultOutputFile"), "gamedata.data");
}
void CapolloParseEditorApp::setOutputFile(LPCTSTR  filepath)
{
	WriteProfileString(_T("setting"), _T("defaultOutputFile"), filepath);
}

void CapolloParseEditorApp::setDefaultSetting(LPCTSTR name, LPCTSTR value)
{
	WriteProfileString(_T("setting"),name, value);
}

CString CapolloParseEditorApp::getDefaultSetting(LPCTSTR name, LPCTSTR dftVal)
{
	return GetProfileString(_T("setting"), name,dftVal);

}

const char* CapolloParseEditorApp::getGetCPPFunclist()
{
	return NULL;
}