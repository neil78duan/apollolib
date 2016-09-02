#pragma once
#include "afxwin.h"
#include <vector>

// InsertNodeListDlg 对话框

class InsertNodeListDlg : public CDialogEx
{
	DECLARE_DYNAMIC(InsertNodeListDlg)

public:
	
	InsertNodeListDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~InsertNodeListDlg();
	enum {MAX_CREATE_LIST = 60};
// 对话框数据
	enum { IDD = IDD_DIALOG_INSERT_LIST };

	std::vector<CString> m_selList;

	int GetSelect() {
		return m_selIndex;
	}

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	// 选中
	int m_selIndex;
};
