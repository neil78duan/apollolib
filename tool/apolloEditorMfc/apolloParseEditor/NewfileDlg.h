#pragma once


#include "nd_common/nd_common.h"
#include "afxcmn.h"
// CNewfileDlg dialog

#include "ctrls/mydlg.h"
class CNewfileDlg : public CDialog
{
	DECLARE_DYNAMIC(CNewfileDlg)

public:
	CNewfileDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CNewfileDlg();
	bool InitFileRoot(const char *fileRoot, const char *templateFile);
	bool ShowFileList();
	const char *getSelectFile() { return m_selFile; }

// Dialog Data
	enum { IDD = IDD_DIALOG_NEW_FILE };
	const char *m_fileTemplate;
	const char *m_file;
	const char *m_selFile;
	ndxml_root m_xmlRoot;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	void SaveXmlFile();

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonDeltet();
	afx_msg void OnBnClickedButtonNew();
	CTreeCtrl m_tree;
	afx_msg void OnTvnSelchangedTreeFiles(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTvnEndlabeleditTreeFiles(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedOk();
	afx_msg void OnNMDblclkTreeFiles(NMHDR *pNMHDR, LRESULT *pResult);
};
