#pragma once


// CDlgFunclist �Ի���
#include <vector>

class CDlgFunclist : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgFunclist)

public:
	CDlgFunclist(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDlgFunclist();

	std::vector<CString> m_funcList;
// �Ի�������
	enum { IDD = IDD_DIALOG_FUNCLIST };
	CString* getSelectFunc();

protected:
	virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	int m_selIndex;
	afx_msg void OnBnClickedOk();
	afx_msg void OnLbnDblclkList1();
};
