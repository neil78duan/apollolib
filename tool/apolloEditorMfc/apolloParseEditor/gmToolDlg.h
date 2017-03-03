#pragma once


class NDIConn;
class LoginApollo;
#include "nd_common/nd_common.h"
// gmToolDlg dialog


#include "ctrls/mydlg.h"
class gmToolDlg : public CDialog
{
	DECLARE_DYNAMIC(gmToolDlg)

public:
	gmToolDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~gmToolDlg();

// Dialog Data
	enum { IDD = IDD_GM_DLG };
	void clearLog();

	void LogText(const char* text);

	ndxml_root *m_editor_setting;
	ndxml_root *m_gmCfg;

	NDIConn *getConnect() { return m_pConn; };
	bool LoadDataDef(const char *file,const char *script,const char *message_def);
protected:
	bool m_newLine;
	NDIConn *m_pConn;
	LoginApollo *m_login;
	const char* m_scriptFile;

	int _connectHost(const char *host, int port);
	int _login(const char *user, const char *passwd);
	int _relogin(void *sessionData, size_t session_size);

	int SelOrCreateRole();
	int createRole(const char *roleName);
	int getRoleData();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonLogin();
	afx_msg void OnBnClickedButtonWriteData();
	afx_msg void OnBnClickedButtonSend();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
protected:
	CString m_strHost;
public:
	int m_nPort;
	CString m_strUserName;
	CString m_strPasswd;
	afx_msg void OnBnClickedButtonMsgReset();
	int m_maxId;
	int m_minId;
	CString m_msgData;
	int m_selDataType;
	afx_msg void OnBnClickedButtonServerReload();
	afx_msg void OnBnClickedButtonClear();
	afx_msg void OnBnClickedBtGmmsg();
public:
	afx_msg void OnBnClickedButtonShowError();
	afx_msg void OnBnClickedCheckShowHex();
};
