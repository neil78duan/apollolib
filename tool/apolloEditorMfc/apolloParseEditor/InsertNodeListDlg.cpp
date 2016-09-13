// InsertNodeListDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "apolloParseEditor.h"
#include "InsertNodeListDlg.h"
#include "afxdialogex.h"


// InsertNodeListDlg 对话框

IMPLEMENT_DYNAMIC(InsertNodeListDlg, CDialogEx)

InsertNodeListDlg::InsertNodeListDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(InsertNodeListDlg::IDD, pParent)
	, m_selIndex(0)
{
}

InsertNodeListDlg::~InsertNodeListDlg()
{
}

void InsertNodeListDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Radio(pDX, IDC_RADIO1, m_selIndex);
}


BEGIN_MESSAGE_MAP(InsertNodeListDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, &InsertNodeListDlg::OnBnClickedOk)
END_MESSAGE_MAP()


BOOL InsertNodeListDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	CButton *pBT = NULL;
	int total = 0;
	for (int i = 0; i < (int) m_selList.size() && i< MAX_CREATE_LIST; i++)
	{
		pBT = (CButton *)GetDlgItem(IDC_RADIO1 + i);
		if (pBT){
			++total;
			pBT->SetWindowText((LPCTSTR)m_selList[i]);
		}
	}
	for (int i = total; i <MAX_CREATE_LIST; i++)
	{
		pBT = (CButton *)GetDlgItem(IDC_RADIO1 + i);
		if (pBT){
			pBT->ShowWindow(SW_HIDE);
			pBT->EnableWindow(FALSE);
		}
	}
	return TRUE;
}

// InsertNodeListDlg 消息处理程序


void InsertNodeListDlg::OnBnClickedOk()
{
	// TODO:  在此添加控件通知处理程序代码
	//m_selected = m_pComboSel.GetCurSel();
	UpdateData(TRUE);
	CDialogEx::OnOK();
}
