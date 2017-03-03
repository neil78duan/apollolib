// DlgFunclist.cpp : 实现文件
//

#include "stdafx.h"
#include "apolloParseEditor.h"
#include "DlgFunclist.h"
#include "afxdialogex.h"

#include "logic_parser/logic_compile.h"

#include "logic_parser/logic_editor_helper.h"
using namespace LogicEditorHelper;
// CDlgFunclist 对话框

IMPLEMENT_DYNAMIC(CDlgFunclist, CDialogEx)

CDlgFunclist::CDlgFunclist(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDlgFunclist::IDD, pParent)
	, m_selIndex(-1)
{

}

CDlgFunclist::~CDlgFunclist()
{
}

void CDlgFunclist::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BOOL CDlgFunclist::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	CListBox *pBT = (CListBox *)GetDlgItem(IDC_LIST1);
	if (pBT) {
		for (int i = 0; i < m_funcList.size(); i++)
		{
			char buf[256];
			const char *p = getDisplayNameFromStr((LPCTSTR)m_funcList[i],buf, sizeof(buf));

			pBT->AddString(p);
		}
		pBT->SetSel(-1);
	}
	return TRUE;
}

BEGIN_MESSAGE_MAP(CDlgFunclist, CDialogEx)
	ON_BN_CLICKED(IDOK, &CDlgFunclist::OnBnClickedOk)
	ON_LBN_DBLCLK(IDC_LIST1, &CDlgFunclist::OnLbnDblclkList1)
END_MESSAGE_MAP()


// CDlgFunclist 消息处理程序


void CDlgFunclist::OnBnClickedOk()
{
	//UpdateData();

	CListBox *pBT = (CListBox *)GetDlgItem(IDC_LIST1);
	if (pBT){
		m_selIndex = pBT->GetCurSel();
	}
	CDialogEx::OnOK();
}

CString* CDlgFunclist::getSelectFunc()
{
	if (m_funcList.size() > 0 && m_selIndex!=-1 && m_selIndex < m_funcList.size())
	{
		return &m_funcList[m_selIndex];
	}
	return NULL;
}

void CDlgFunclist::OnLbnDblclkList1()
{
	// TODO:  在此添加控件通知处理程序代码
	OnBnClickedOk();
}
