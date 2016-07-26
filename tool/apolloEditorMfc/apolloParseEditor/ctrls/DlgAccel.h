
//#include "UI.h"
//////////////////
// Generic dialog-that-uses-accelerators.
//
class CDlgWithAccelerators;
typedef bool(*dlgAccCallBack) (CDlgWithAccelerators *curDlg);

class  CDlgWithAccelerators : public CDialog {
public:
	CDlgWithAccelerators(UINT nIDTemplate, CWnd* pParentWnd = NULL, dlgAccCallBack initfunc = NULL, dlgAccCallBack okfunc = NULL, dlgAccCallBack cancelfunc = NULL);
	CDlgWithAccelerators(LPCTSTR lpszTemplateName, CWnd* pParentWnd = NULL);
	~CDlgWithAccelerators();

protected:
	void NextTabOrder();
	HACCEL m_hAccel; // accelerator table
	ACCEL  m_sAccel ;
	dlgAccCallBack m_myInitfunc;
	dlgAccCallBack m_myOKFunc;
	dlgAccCallBack m_myCancelFunc;

	// MFC overrides
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual LRESULT OnGetDefID(WPARAM wp, LPARAM lp) ;
	virtual void OnAccEnter();
	virtual void OnAccHelp() ;

	virtual void OnOK();
	virtual void OnCancel();
	DECLARE_MESSAGE_MAP()
};

