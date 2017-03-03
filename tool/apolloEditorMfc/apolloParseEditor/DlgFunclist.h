#pragma once


// CDlgFunclist 对话框
#include <vector>

class CDlgFunclist : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgFunclist)

public:
	CDlgFunclist(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgFunclist();

	std::vector<CString> m_funcList;
// 对话框数据
	enum { IDD = IDD_DIALOG_FUNCLIST };
	CString* getSelectFunc();

protected:
	virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	int m_selIndex;
	afx_msg void OnBnClickedOk();
	afx_msg void OnLbnDblclkList1();
};
