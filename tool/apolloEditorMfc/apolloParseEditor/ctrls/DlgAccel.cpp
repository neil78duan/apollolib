////////////////////////////////////////////////////////////////
// VCKBASE -- July 2000
// Compiles with Visual C++ 6.0, runs on Windows 98 and probably NT too.
//
// CDlgWithAccelerators is a general-purpose class that adds
// accelerators to CDialog.
//
#include "stdafx.h"
#include "dlgaccel.h"
#include "../Resource.h"
//#include "MyHelp.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BEGIN_MESSAGE_MAP(CDlgWithAccelerators,CDialog)
//	ON_MESSAGE(DM_GETDEFID, OnGetDefID)
//#else
	ON_COMMAND(ID_MY_ENTER, OnAccEnter)
	ON_COMMAND(IDHELP, OnAccHelp)
//#endif 
END_MESSAGE_MAP()

////////////////
// do-nothing ctors and dtor
//
CDlgWithAccelerators::CDlgWithAccelerators(LPCTSTR lpszTemplateName,
	CWnd* pParentWnd) : CDialog(lpszTemplateName, pParentWnd)
{

	m_myInitfunc = 0;
	m_myOKFunc = 0;
	m_myCancelFunc=0;

	m_myInitfunc = 0;
	m_myOKFunc = 0;
	m_myCancelFunc = 0;
}

CDlgWithAccelerators::CDlgWithAccelerators(UINT nIDTemplate,
	CWnd* pParentWnd,
	dlgAccCallBack initfunc , dlgAccCallBack okfunc , dlgAccCallBack cancelfunc ) 
	: CDialog(nIDTemplate, pParentWnd)
{
	m_myInitfunc = 0;
	m_myOKFunc = 0;
	m_myCancelFunc = 0;


	m_myInitfunc = initfunc;
	m_myOKFunc = okfunc;
	m_myCancelFunc = cancelfunc ;
}

CDlgWithAccelerators::~CDlgWithAccelerators()
{
	//if(m_hAccel) {
	//	DestroyAcceleratorTable(m_hAccel) ;
	//	m_hAccel = 0 ;
	//}
}

//////////////////
// Pre-translate message: translate keystrokes using acclerator table. 
//
BOOL CDlgWithAccelerators::PreTranslateMessage(MSG* pMsg)
{
	if (WM_KEYFIRST <= pMsg->message && pMsg->message <= WM_KEYLAST) {
		HACCEL hAccel = m_hAccel;
		if (hAccel && ::TranslateAccelerator(m_hWnd, hAccel, pMsg))
			return TRUE;
	}
	return CDialog::PreTranslateMessage(pMsg);
}
LRESULT CDlgWithAccelerators::OnGetDefID(WPARAM wp, LPARAM lp) 
{
    return MAKELONG(0,DC_HASDEFID); 
} 

//得到实例的句柄(主要是资源句柄)
extern HINSTANCE GetMfxExHandle() ;
//////////////////
// Initialize dialog: load accelerators
//
BOOL CDlgWithAccelerators::OnInitDialog()
{
	BOOL bRet = CDialog::OnInitDialog();

	// Load dialog's accelerators
	//CString stracc ;
	//BOOL bret = stracc.LoadString(IDR_ACCELERATOR1) ;
	m_hAccel = ::LoadAccelerators(AfxGetInstanceHandle(),
		MAKEINTRESOURCE(IDR_ACCELERATOR1) );

	if (m_myInitfunc) {
		m_myInitfunc(this);
	}
	return bRet;
}



void CDlgWithAccelerators::OnOK()
{
	if (m_myOKFunc) {
		bool ret = m_myOKFunc(this);
		if (ret){
			return;
		}
	}
	CDialog::OnOK();
}
void CDlgWithAccelerators::OnCancel()
{
	if (m_myCancelFunc)	{
		bool ret = m_myCancelFunc(this);
		if (ret )	{
			return;
		}
	}
	CDialog::OnCancel();
}

void CDlgWithAccelerators::NextTabOrder()
{
	CWnd* pWndNext = GetNextDlgTabItem(GetFocus());
	if (pWndNext) {
		pWndNext->SetFocus();
	}
}

void CDlgWithAccelerators::OnAccEnter()
{
	CWnd *pWnd = GetFocus() ;
	if(pWnd) {
		DWORD dwCtrlID =pWnd->GetDlgCtrlID() ;
		if(IDOK==dwCtrlID)
			SendMessage(WM_COMMAND,IDOK) ;
		else if(IDCANCEL==dwCtrlID)
			SendMessage(WM_COMMAND,IDCANCEL) ;
	}
	NextTabOrder() ;
}
//显示帮助
void CDlgWithAccelerators::OnAccHelp() 
{
	//RUN_HELP() ;
}