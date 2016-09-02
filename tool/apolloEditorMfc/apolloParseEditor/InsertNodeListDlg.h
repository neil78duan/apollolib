#pragma once
#include "afxwin.h"
#include <vector>

// InsertNodeListDlg �Ի���

class InsertNodeListDlg : public CDialogEx
{
	DECLARE_DYNAMIC(InsertNodeListDlg)

public:
	
	InsertNodeListDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~InsertNodeListDlg();
	enum {MAX_CREATE_LIST = 60};
// �Ի�������
	enum { IDD = IDD_DIALOG_INSERT_LIST };

	std::vector<CString> m_selList;

	int GetSelect() {
		return m_selIndex;
	}

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	// ѡ��
	int m_selIndex;
};
