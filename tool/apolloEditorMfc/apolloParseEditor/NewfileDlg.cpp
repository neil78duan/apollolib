// NewfileDlg.cpp : implementation file
//

#include "stdafx.h"
#include "apolloParseEditor.h"
#include "NewfileDlg.h"
#include "afxdialogex.h"


// CNewfileDlg dialog

IMPLEMENT_DYNAMIC(CNewfileDlg, CDialog)

CNewfileDlg::CNewfileDlg(CWnd* pParent /*=NULL*/)
: CDialog(CNewfileDlg::IDD, pParent)
{
	m_selFile = 0;
	ndxml_initroot(&m_xmlRoot);
}

CNewfileDlg::~CNewfileDlg()
{
	ndxml_destroy(&m_xmlRoot);
}

void CNewfileDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE_FILES, m_tree);
}


BOOL CNewfileDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	ShowFileList();

	return TRUE;
}

BEGIN_MESSAGE_MAP(CNewfileDlg, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_DELTET, &CNewfileDlg::OnBnClickedButtonDeltet)
	ON_BN_CLICKED(IDC_BUTTON_NEW, &CNewfileDlg::OnBnClickedButtonNew)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_FILES, &CNewfileDlg::OnTvnSelchangedTreeFiles)
	ON_NOTIFY(TVN_ENDLABELEDIT, IDC_TREE_FILES, &CNewfileDlg::OnTvnEndlabeleditTreeFiles)
	ON_BN_CLICKED(IDOK, &CNewfileDlg::OnBnClickedOk)
	ON_NOTIFY(NM_DBLCLK, IDC_TREE_FILES, &CNewfileDlg::OnNMDblclkTreeFiles)
END_MESSAGE_MAP()



bool CNewfileDlg::InitFileRoot(const char *fileRoot, const char *templateFile)
{
	m_fileTemplate = templateFile;
	m_file = fileRoot;
	ndxml_initroot(&m_xmlRoot);
	if (ndxml_load_ex((char*)fileRoot, &m_xmlRoot, "gbk")) {
		CString tipText;
		tipText.Format("open file %s error", fileRoot);
		AfxMessageBox((LPCTSTR)tipText);
		return false;
	}
	return true;
}


void CNewfileDlg::SaveXmlFile()
{
	ndxml_save_encode(&m_xmlRoot, m_file, E_SRC_CODE_GBK, E_SRC_CODE_UTF_8);
}

// CNewfileDlg message handlers


void CNewfileDlg::OnBnClickedButtonDeltet()
{
	// TODO: Add your control notification handler code here
	HTREEITEM hCurItem = m_tree.GetSelectedItem();
	if (hCurItem){
		ndxml *xml = (ndxml*)(m_tree.GetItemData(hCurItem));
		if (!xml) {
			return;			
		}
		ndxml_root *xmlfiles = ndxml_getnode(&m_xmlRoot, "script_file_manager");
		if (!xmlfiles){
			return;
		}
		int num = ndxml_num(xmlfiles);
		for (int i = 0; i < num; i++) {
			ndxml *node = ndxml_getnodei(xmlfiles, i);
			if (node == xml) {
				if (IDYES != AfxMessageBox("是否要删除?", MB_YESNO))	{
					return;
				}
				nd_rmfile(ndxml_getval(node));
				ndxml_delxml(node, xmlfiles);

				SaveXmlFile();
				ShowFileList();
				return;
			}
		}

	}
}

bool CNewfileDlg::ShowFileList()
{
	m_tree.DeleteAllItems();
	HTREEITEM hRootItem = m_tree.InsertItem(_T("文件"));


	ndxml_root *xml = ndxml_getnode(&m_xmlRoot, "script_file_manager");
	if (!xml){
		AfxMessageBox(_T("文件损坏"));
		return false;
	}
	int num = ndxml_num(xml);
	for (int i = 0; i < num; i++) {
		ndxml *node = ndxml_getnodei(xml, i);
		if (!node )
			continue;
		const char *dispName = ndxml_getattr_val(node, "name");
		if (!dispName)	{
			dispName = ndxml_getval(node);
		}
		HTREEITEM 	hNode = m_tree.InsertItem(dispName, hRootItem, TVI_LAST);

		m_tree.SetItemData(hNode, (DWORD_PTR)node);
	}
	m_tree.Expand(hRootItem, TVE_EXPAND);

	return true;
}


void CNewfileDlg::OnBnClickedButtonNew()
{
	CString filter;
	filter = " default Files (*.xml)| *.xml||";
	CFileDialog myfile(false, _T("*.xml"),NULL, OFN_OVERWRITEPROMPT, (LPCTSTR)filter, NULL);
	if (myfile.DoModal() != IDOK)	{
		return ;
	}

	CString strFile = myfile.GetPathName();
	if (strFile.IsEmpty())	{
		return;
	}
	ndxml_root *xml = ndxml_getnode(&m_xmlRoot, "script_file_manager");
	if (!xml){
		AfxMessageBox(_T("文件损坏"));
		return;
	}
	if (-1 == nd_cpfile(m_fileTemplate, (LPCTSTR)strFile)) {
		AfxMessageBox(_T("不能保存文件"));
		return;
	}
	ndxml *newNode = ndxml_addnode(xml, "file", (LPCTSTR)strFile);
	if (newNode){
		CString cName = myfile.GetFileTitle();
		ndxml_addattrib(newNode, "name", (LPCTSTR)cName);
	}
	SaveXmlFile();
	ShowFileList();
}


void CNewfileDlg::OnTvnSelchangedTreeFiles(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;
}


void CNewfileDlg::OnTvnEndlabeleditTreeFiles(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTVDISPINFO pTVDispInfo = reinterpret_cast<LPNMTVDISPINFO>(pNMHDR);

	HTREEITEM hItem = m_tree.GetSelectedItem();
	if (!hItem) {
		*pResult = -1;
		return;
	}
	//CString str1 = m_pTree->GetItemText(hItem) ;
	CEdit* pedit = m_tree.GetEditControl();
	if (!pedit) {
		*pResult = -1;
		return;
	}
	CString str1;
	pedit->GetWindowText(str1);

	if (str1.IsEmpty()) {
		*pResult = -1;
		return;
	}
	ndxml *xml = (ndxml *)m_tree.GetItemData(hItem);
	if (!xml) {
		*pResult = -1;
		return;
	}

	if (0 == ndxml_setattrval(xml, "name", (char*)(LPCTSTR)str1)) {
		m_tree.SetItemText(hItem, (LPCTSTR)str1);
	}
	*pResult = 0;

}


void CNewfileDlg::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	m_selFile = 0;
	HTREEITEM hCurItem = m_tree.GetSelectedItem();
	if (hCurItem){
		ndxml *xml = (ndxml*)(m_tree.GetItemData(hCurItem));
		if (xml) {
			m_selFile = ndxml_getval(xml);
		}
	}
	if (!m_selFile){
		AfxMessageBox(_T("请选择要编辑的文件!"));
		return;
	}
	CDialog::OnOK();
}


void CNewfileDlg::OnNMDblclkTreeFiles(NMHDR *pNMHDR, LRESULT *pResult)
{
	OnBnClickedOk();
	// TODO: Add your control notification handler code here
	*pResult = 0;
}
