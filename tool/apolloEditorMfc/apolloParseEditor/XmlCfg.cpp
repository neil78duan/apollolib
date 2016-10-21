// XmlCfg.cpp : implementation file
//

#include "stdafx.h"
//#include "excel_exp.h"
#include "XmlCfg.h"
//#include "ND_file.h"
#include "resource.h"
#include "InsertNodeListDlg.h"
#include "DlgFunclist.h"
#include "logic_parser/logic_compile.h"
#include "logic_parser/dbl_mgr.h"

#include "ctrls/NewCellTypes/GridURLCell.h"
#include "ctrls/NewCellTypes/GridCellCombo.h"
#include "ctrls/NewCellTypes/GridCellCheck.h"
#include "ctrls/NewCellTypes/GridCellNumeric.h"
#include "ctrls/NewCellTypes/GridCellDateTime.h"

#include "logic_parser/logic_editor_helper.h"
using namespace LogicEditorHelper;

// 得到文件名
static bool _GetFileName(bool bOpen, CString & strFile, const char *default_file)
{
	CString defext = ".xls";
	CString filter;
	if (default_file) {
		filter.Format("default Files (*.%s)| *.%s| All Files (*.*)| *.*||", default_file, default_file);
	}
	else
		filter = " default Files (*.*)| *.*| All Files (*.*)| *.*||";

	CFileDialog myfile(bOpen, _T("*.*"), NULL, NULL, (LPCTSTR)filter, NULL);
	if (myfile.DoModal() == IDOK)	{
		strFile = myfile.GetPathName();
		return true;
	}
	return false;
}

static bool _OpenFilter(CString &strPath, CString &tip, HWND hOwner)
{
	char   szDir[MAX_PATH];
	BROWSEINFO   bi;
	ITEMIDLIST   *pidl;

	bi.hwndOwner = hOwner;
	bi.pidlRoot = NULL;
	bi.pszDisplayName = szDir;
	bi.lpszTitle = (LPCTSTR)tip; //"请选择目录";//strDlgTitle;   
	bi.ulFlags = BIF_RETURNONLYFSDIRS;
	bi.lpfn = NULL;
	bi.lParam = 0;
	bi.iImage = 0;

	pidl = SHBrowseForFolder(&bi);
	if (pidl == NULL)
		return false;
	if (!SHGetPathFromIDList(pidl, szDir))
		return false;
	strPath = szDir;
	return TRUE;
}


//////////////////////////////////////////////////////////////////////////
// CXmlCfg dialog

//IMPLEMENT_DYNAMIC(CXmlCfg, CDialog)

CXmlCfg::CXmlCfg(CWnd* pParent,dlgAccCallBack initfunc, dlgAccCallBack okfunc, dlgAccCallBack cancelfunc )
	: CDialog(CXmlCfg::IDD, pParent,initfunc, okfunc, cancelfunc)
{
	m_config = 0;
	m_root = 0;
	m_pTree = 0;
	m_hRootItem = 0;	//树根
	m_stat =0;			//0 xml is empyt, 1 input xml,  2 changed

	m_grad = 0;
}

CXmlCfg::~CXmlCfg()
{

	m_userDefineList.clear(); 
	if(m_grad) {
		delete m_grad ;
		m_grad = 0 ;
	}
}

void CXmlCfg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE1, m_drawTree);
	
}


BEGIN_MESSAGE_MAP(CXmlCfg, CDialog)
	ON_NOTIFY(NM_DBLCLK, IDC_TREE1, &CXmlCfg::OnNMDblclkTree1)
	ON_NOTIFY(NM_RETURN, IDC_TREE1, &CXmlCfg::OnNMReturnTree1)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE1, &CXmlCfg::OnTvnSelchangedTree1)

	ON_NOTIFY(GVN_BEGINLABELEDIT, IDC_GRID_CTRL, OnGridBeginEdit)
	ON_NOTIFY(GVN_ENDLABELEDIT, IDC_GRID_CTRL, OnGridEndEdit)
	ON_NOTIFY(NM_CLICK, IDC_GRID_CTRL, OnGridClick)

	ON_BN_CLICKED(IDCANCEL, &CXmlCfg::OnBnClickedCancel)
	ON_BN_CLICKED(IDOK, &CXmlCfg::OnBnClickedOk)
	ON_NOTIFY(GVN_SELCHANGED, IDC_GRID_CTRL, OnGridEndSelChange)
	ON_NOTIFY(NM_RCLICK, IDC_TREE1, &CXmlCfg::OnNMRClickTree1)
	ON_COMMAND(ID_POP_INSERT_NODE, &CXmlCfg::OnPopInsertNode)
ON_NOTIFY(TVN_ENDLABELEDIT, IDC_TREE1, &CXmlCfg::OnTvnEndlabeleditTree1)
ON_COMMAND(ID_POP_XML_DEL, &CXmlCfg::OnPopXmlDel)
ON_NOTIFY(TVN_ITEMEXPANDED, IDC_TREE1, &CXmlCfg::OnTvnItemexpandedTree1)
END_MESSAGE_MAP()



BOOL CXmlCfg::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_pTree = &m_drawTree;
// 	m_pTree = (CTreeCtrl *)GetDlgItem(IDC_TREE1) ;
// 
// 	if(!m_pTree)
// 		return FALSE ;
	//m_pTree->set

	CRect rc;	
	GetDlgItem(IDC_GRID_FRAME)->GetWindowRect(rc);
	ScreenToClient(&rc);

	m_grad = new CGradShow ;

	m_grad->Create(rc, this, IDC_GRID_CTRL);
	m_grad->SetEditable(FALSE);
	ShowRow("名字", "值", NULL) ;

	SetGridSize();
	
	m_image.Create(IDB_TREEIMAGE, 16, 1, RGB(255, 255, 255));
	m_drawTree.SetImageList(&m_image, TVSIL_NORMAL);

	CreateXmlTree(m_root) ;


	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

#define BACKUP_NAME "xmlcfgbackup.xml"
void CXmlCfg::SetXML(ndxml_root *config_xml, ndxml_root *data_root)
{
	m_root = data_root ;
	nd_chdir(nd_getcwd());
	ndxml_save(data_root, BACKUP_NAME);

	m_config = config_xml;
	ndxml *alias = ndxml_getnode(m_config, "alias");
	if(alias)
		m_alias.Create(alias) ;
	
}

bool CXmlCfg::loadUserdefDisplayList(ndxml_root &xmlNameList, const char *name)
{
	text_list_t textList;

	ndxml *funcroot = ndxml_getnode(&xmlNameList, name);
	char buf[256];
	if (funcroot) {
		for (int i = 0; i < ndxml_num(funcroot); i++){
			ndxml *fnode = ndxml_getnodei(funcroot, i);
			const char *pDispname = ndxml_getattr_val(fnode, "desc");
			const char *pRealVal = ndxml_getval(fnode);

			const char *p = buildDisplaNameValStr(pRealVal, pDispname, buf, sizeof(buf));
			textList.push_back(CString(p));

		}
		addDisplayNameList(name, textList);
		return true;
	}
	return false;
}


bool CXmlCfg::addDisplayNameList(const char *name, text_list_t &text_list)
{
	std::pair< text_map_t::iterator, bool> ret = m_userDefineList.insert(std::make_pair(std::string(name), text_list));
	return ret.second;
}


bool CXmlCfg::CreateXmlTree(ndxml_root *xml_root) 
{
	m_pTree->DeleteAllItems() ;

	m_hRootItem = m_pTree->InsertItem("XML根节点") ;
	m_pTree->SetItemData(m_hRootItem, (DWORD)0) ;			//root = 0 ;	
	m_grad->Clear();

	int num =ndxml_num(xml_root) ;
	for(int i=0;i<num; i++) {
		ndxml *node = ndxml_getnodei(xml_root,i) ;
		if (!node || CheckHide(node))
			continue ;
		InitTreeNode(node,m_hRootItem) ;
	}
	//expand tree 
	m_pTree->Expand(m_hRootItem, TVE_EXPAND);

	HTREEITEM hChild = m_pTree->GetChildItem(m_hRootItem);
	while (hChild){		
		ExpandTree(hChild);
		hChild = m_pTree->GetNextItem(hChild, TVGN_NEXT);
	}

	return true ;
}

bool CXmlCfg::ExpandTree(HTREEITEM hItem)
{
	if (!hItem){
		return false;
	}
	ndxml* xml = (ndxml*) m_pTree->GetItemData(hItem);
	if (!xml){
		return false;
	}

	bool ret = false;
	const char *pExpVal = ndxml_getattr_val(xml, _GetReverdWord(ERT_EXPAND_STAT));
	if (pExpVal && pExpVal[0]=='1'){
		ret = true;
		m_pTree->Expand(hItem, TVE_EXPAND);
		if (!m_pTree->ItemHasChildren(hItem)) {
			return true;
		}
		HTREEITEM hChild = m_pTree->GetChildItem(hItem);
		while (hChild){
			
			ExpandTree(hChild);
			hChild = m_pTree->GetNextItem(hChild, TVGN_NEXT);
		}

	}

	return ret;
}

HTREEITEM CXmlCfg::InitTreeNode(ndxml *xml_node,HTREEITEM hParent)
{
	HTREEITEM hNode ;

	hNode = m_pTree->InsertItem(_GetXmlName(xml_node,&m_alias),hParent,TVI_LAST) ;
	m_pTree->SetItemData(hNode,(DWORD)xml_node) ;
	

	if(CheckExpand(xml_node)) {
		return hNode;
	}
	text_vect_t exp_list;
	get_string_array((char*)ndxml_getattr_val(xml_node, _GetReverdWord(ERT_EXPAND_LIST)), exp_list);

	int num =ndxml_getsub_num(xml_node);
	for(int i=0;i<num; i++) {
		ndxml *node = ndxml_refsubi(xml_node,i) ;
		if(!node)
			continue ;
		const char *pName = ndxml_getname(node);
		if (CheckHide(node) || std::find(exp_list.begin(), exp_list.end(),std::string(pName))!=exp_list.end()) 
			continue ;

		InitTreeNode(node,hNode) ;
	}

	return hNode;
}

//显示一行xml
void CXmlCfg::ShowRow(const char *name, const char *val, ndxml* param) 
{
	CString str_buf[E_COL_NUM] ;
	char realval[4096];
	const char *p = getDisplayNameFromStr(val, realval, sizeof(realval));

	str_buf[0] = name ;
	str_buf[1] = p ;
	m_grad->ShowText(str_buf,E_COL_NUM) ;
	int row =m_grad->GetRowCount()-1 ;
	m_grad->SetItemData(row, 1, (LPARAM)param);

	if(param) {	
		m_grad->SetItemState(row, 1, m_grad->GetItemState(row, 1) & ~GVIS_READONLY);
		int type =  GetXmlValType(param, m_config) ;
		if(type==EDT_BOOL) {
			if(m_grad->SetCellType(row,1, RUNTIME_CLASS(CGridCellCheck))) {
				CGridCellCheck *pcell =(CGridCellCheck *) m_grad->GetCell(row,1) ;
				pcell->SetCheck('0'!=val[0]) ;
				m_grad->SetItemText(row,1, NULL);
				m_grad->RedrawCell(row,1);
			}
		}
		else if(type==EDT_ENUM) {
			text_list_t enum_texts;
			if (getEnumList(param, m_config, enum_texts) > 0) {
				int sel = atoi(val);
				if (sel >= 0 && sel < enum_texts.size() )
					m_grad->SetItemText(row, 1, (LPCTSTR)enum_texts[sel]);
			}			
		}
		else if (type == EDT_KEY_VAL) {
			text_list_t key_texts,value_texts;
			if (getKeyList(param, m_config, key_texts) > 0 && getKeyValueList(param, m_config, value_texts)> 0) {
				for (size_t i = 0; i < value_texts.size(); i++) {
					if (val == value_texts[i]) {
						if (i < key_texts.size())	{
							m_grad->SetItemText(row, 1, (LPCTSTR)key_texts[i]);
						}
						break;
					}

				}
			}
		}
	}

}

void CXmlCfg::SetXmlName(ndxml *xml_node, ndxml *xmlParent)
{
	const char *pAutoIndex = ndxml_getattr_val(xmlParent, "auto_index");
	const char *pName = ndxml_getattr_val(xml_node, "name");
	if (pAutoIndex && pName ){
		int a = atoi(pAutoIndex);
		char buf[128];
		snprintf(buf, sizeof(buf), "%s%s", pName, pAutoIndex);
		ndxml_setattrval(xml_node, "name", buf);

		++a;
		snprintf(buf, sizeof(buf), "%d", a);
		ndxml_setattrval(xmlParent, "auto_index", buf);
	}
	
}

bool CXmlCfg::_GetUserDefinData(const char *pUserParam, CString &val)
{
	if (0==ndstricmp(pUserParam,"func_list") ) {
		//m_root
		CDlgFunclist dlg;
		ndxml *node = ndxml_getnode(m_root, "baseFunction");
		if (node)	{
			for (int i = 0; i < ndxml_num(node); i++){
				ndxml *funcNode = ndxml_getnodei(node,i);
				const char *name = (char*)ndxml_getattr_val(funcNode, _GetReverdWord(ERT_NAME));
				if (!name) {
					name = (char*)ndxml_getname(funcNode);
				}
				dlg.m_funcList.push_back(CString(name));
			}
		}
		text_map_t::const_iterator it = m_userDefineList.find(pUserParam);
		if (it != m_userDefineList.end()) {
			const text_list_t &exterFunclist = it->second;
			for (int i = 0; i < exterFunclist.size(); i++) {
				dlg.m_funcList.push_back(exterFunclist[i]);
			}
		}

		
		if (IDOK==dlg.DoModal()) {
			CString *inputName = dlg.getSelectFunc();
			if (inputName)	{
				val = *inputName;
				return true;
			}
		}
	}
// 	else if (0 == ndstricmp(pUserParam, "error_list")) {
// 
// 	}
	/*else if (0 == ndstricmp((char*)pUserParam, "msg_list")) {
		//m_root
		return _GetUserSelEnumVal(m_messageList, val);

	}
	else if (0 == ndstricmp((char*)pUserParam, "event_list")) {
		return _GetUserSelEnumVal(m_eventsList, val);
	}
	*/
	else if (0 == ndstricmp((char*)pUserParam, "dbl_excel")) {
		DBLDatabase *pdbl = DBLDatabase::get_Instant();
		if (!pdbl )	{
			AfxMessageBox("不能加载excel数据,请先导出excel数据");
			return false;
		}
		text_list_t texts;
		DBLDatabase::table_vct_t::iterator it = pdbl->m_tables.begin();
		for (; it!=pdbl->m_tables.end() ;it++)	{

			texts.push_back(CString(it->first.c_str()));
		}
		return _GetUserSelEnumVal(texts, val);
	}
	else if (0 == ndstricmp((char*)pUserParam, "dbl_excel_col")) {
		DBLDatabase *pdbl = DBLDatabase::get_Instant();
		if (!pdbl)	{
			AfxMessageBox("不能加载excel数据,请先导出excel数据");
			return false;
		}

		ndxml *xml = GetSelXml(m_hCurItem);
		if (!xml)	{
			return false;
		}
		ndxml *tablexml = ndxml_getnode(xml, "tablename");
		if (!tablexml)	{
			AfxMessageBox("请先选则表格");
			return false;
		}
		const char *pname = ndxml_getval(tablexml);
		if (0==ndstricmp((char*)pname,(char*)"none"))	{
			AfxMessageBox("请先选则表格");
			return false;
		}
		DBLTable *ptable = pdbl->FindTable(pname);
		if (!ptable){
			AfxMessageBox("表格不存在");
			return false;
		}

		text_list_t texts;
		int total = ptable->GetCols();
		for (int i = 0; i < total; i++)	{
			if (pdbl->GetEncodeType() == E_SRC_CODE_UTF_8) {
				char buf[128];
				nd_utf8_to_gbk(ptable->GetColName(i), buf, sizeof(buf));
				texts.push_back(CString(buf));
			}
			else {
				texts.push_back(CString(ptable->GetColName(i)));
			}
		}

		return _GetUserSelEnumVal(texts, val);
	}
	else {
		text_map_t::const_iterator it = m_userDefineList.find(pUserParam);
		if (it != m_userDefineList.end()) {
			return _GetUserSelEnumVal(it->second, val);
		}
	}


	return false;
}

bool CXmlCfg::_GetUserSelEnumVal(const text_list_t &enum_vals, CString &val)
{
	CDlgFunclist dlg;
	for (int i = 0; i < enum_vals.size(); i++) {
		dlg.m_funcList.push_back(enum_vals[i]);
	}
	if (IDOK == dlg.DoModal()) {
		CString *inputName = dlg.getSelectFunc();
		if (inputName)	{
			val = *inputName;
			return true;
		}
	}
	return false;
}

bool CXmlCfg::_beginEdit(int nRow, int nCol)
{
	ndxml *xml = (ndxml *)m_grad->GetItemData(nRow, nCol);
	if (!xml)	{
		return false;
	}
	bool ret = false;
	CString  str1;
	int type = GetXmlValType(xml, m_config);
	if (type == EDT_IN_FILE || type == EDT_OUT_FILE) {
		
		if (_GetFileName(type == EDT_IN_FILE, str1, GetXmlParam(xml))) {
			if (0 == ndxml_setval(xml, (char*)(LPCTSTR)str1)) {
				ret = true;
			}
		}
	}
	else if (EDT_DIR == type) {
		if (_OpenFilter(str1, CString(_T("先择目录")), GetSafeHwnd())){
			if (str1.Right(1) != _T("\\")) {
				str1 += _T("\\");
			}
			if (0 == ndxml_setval(xml, (char*)(LPCTSTR)str1)) {
				ret = true;
			}
		}
	}
	else if (EDT_USER_DEFINE == type) {
		CString str2;
		const char *pUserparam = ndxml_getattr_val(xml, _GetReverdWord(ERT_USER_DEFINE_PARAM));
		if (_GetUserDefinData(pUserparam, str2)) {
			if (0 == ndxml_setval(xml, (char*)(LPCTSTR)str2)) {

				char realval[256];
				const char *p = getDisplayNameFromStr((LPCTSTR)str2, realval, sizeof(realval));
				str1 = (LPCTSTR)p;
				ret = true;
			}
		}

	}
	else if (EDT_ENUM == type)	{
		text_list_t enum_texts;
		if (getEnumList(xml, m_config, enum_texts) > 0) {
			if (_GetUserSelEnumVal(enum_texts, str1)){
				int sel = -1;
				for (int i = 0; i < enum_texts.size(); i++)	{
					if (str1 == enum_texts[i]) {
						sel = i;
						break;
					}
				}
				if (-1 != sel) {
					CString str3;
					str3.Format("%d", sel);
					ndxml_setval(xml, (char*)(LPCTSTR)str3);
					ret = true;
				}

			}
		}
	}
	else if (EDT_KEY_VAL == type)	{
		ret = false;
		text_list_t key_texts;
		if (getKeyList(xml, m_config, key_texts) > 0) {
			if (_GetUserSelEnumVal(key_texts, str1)){
				int sel = -1;
				for (int i = 0; i < key_texts.size(); i++)	{
					if (str1 == key_texts[i]) {
						sel = i;
						break;
					}
				}
				if (-1 != sel) {
					text_list_t value_texts;
					getKeyValueList(xml, m_config, value_texts);
					if (sel < value_texts.size()){
						ndxml_setval(xml, (char*)(LPCTSTR)value_texts[sel]);
						ret = true;
					}
					else {
						AfxMessageBox("配置文件错误,key和value长度不一致");
					}
				}
			}
		}
	}
	if (ret) {
		m_grad->SetItemText(nRow, nCol, (LPCTSTR)str1);
		m_grad->RedrawCell(nRow, nCol);
		m_grad->Invalidate();

		CfgChanged();
	}
	return ret;
	
}

void CXmlCfg::SetGridSize()
{
	enum {min_col_width = 150};
	DWORD wid = m_grad->GetColumnWidth(0);
	if (wid < min_col_width)	{
		m_grad->SetColumnWidth(0,min_col_width);
	} 
	wid = m_grad->GetColumnWidth(1);
	if (wid < min_col_width)	{
		m_grad->SetColumnWidth(1,min_col_width);
	}
}
//显示xml的内容
int CXmlCfg::ShowXMLValue(ndxml *xml_node,CGradShow *show_ctrl, int expand ) 
{
	int i;
	const char *pval , *name;
	//size_t param ;
	pval = ndxml_getval(xml_node) ;
	if(pval) {
		const char *desc =_GetXmlDesc(xml_node);
		name = _GetXmlName(xml_node, &m_alias) ;

		const char *alias = m_alias.GetAlia(name) ;
		if(alias)
			name = alias ;

		ShowRow(name, pval, xml_node) ;

	}
	for(i=0; i<ndxml_getattr_num(xml_node); i++) {

		name = (char*)ndxml_getattr_name(xml_node,i) ;
		if(!name || CheckReserved(name)>=0)
			continue;
		pval = (char*)ndxml_getattr_vali(xml_node, i);
		if(pval) {
			const char *alias = m_alias.GetAlia(name) ;
			if(alias)
				name = alias ;

			ShowRow(name, pval, xml_node) ;
		}
	}

	if(expand) {
		for(i=0; i<ndxml_getsub_num(xml_node); i++) {
			ndxml *sub_node = ndxml_refsubi(xml_node,i) ;
			if(sub_node && !CheckHide(sub_node) ){
				ShowXMLValue(sub_node,show_ctrl,expand ) ;
			}
		}
	}
	else {
		text_vect_t exp_list;
		if (get_string_array((char*)ndxml_getattr_val(xml_node, _GetReverdWord(ERT_EXPAND_LIST)), exp_list)) {
			for (text_vect_t::iterator it = exp_list.begin(); it != exp_list.end(); it++) {
				ndxml *sub_node = ndxml_refsub(xml_node, (*it).c_str());
				if (sub_node && !CheckHide(sub_node)){
					ShowXMLValue(sub_node, show_ctrl, 1);
				}
			}
		}
	}	
	return 0;

}
void CXmlCfg::OnNMDblclkTree1(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: Add your control notification handler code here

	DisplaySelXml() ;
	*pResult = 0;
}

void CXmlCfg::OnNMReturnTree1(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: Add your control notification handler code here

	DisplaySelXml() ;
	*pResult = 0;
}

void CXmlCfg::OnTvnSelchangedTree1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);

	DisplaySelXml() ;
	m_hCurItem = m_pTree->GetSelectedItem();
	*pResult = 0;
}

ndxml* CXmlCfg::GetCurrentXml()
{
	HTREEITEM hItem = m_pTree->GetSelectedItem();
	if (!hItem)
		return 0;
	ndxml* xml = GetSelXml(hItem);
	if (!xml)
		return 0;
	return xml;
}

int CXmlCfg::DisplaySelXml()
{
	HTREEITEM hItem = m_pTree->GetSelectedItem();
	if(!hItem) 
		return -1;
	ndxml* xml = GetSelXml(hItem) ;
	if(!xml)
		return -1;

	m_grad->Clear();

	ShowXMLValue(xml,m_grad, (int)CheckExpand(xml)) ; 

	ND_SetChildrenFont(m_grad->GetSafeHwnd(),false,12, _T("宋体"))  ;
	
	SetGridSize();

	return 0;
}

//通过选中的TREE item来得到XML 节点
ndxml* CXmlCfg::GetSelXml(HTREEITEM hItem)
{
	if(hItem==m_hRootItem) {
		return NULL ;
	}
	return (ndxml*) (m_pTree->GetItemData(hItem) );
}

//GVN_BEGINLABELEDIT
void CXmlCfg::OnGridBeginEdit(NMHDR *pNotifyStruct, LRESULT* pResult)
{
	NM_GRIDVIEW* pItem = (NM_GRIDVIEW*)pNotifyStruct;
	_beginEdit(pItem->iRow, pItem->iColumn);
	*pResult = 0;
}

// GVN_ENDLABELEDIT
void CXmlCfg::OnGridEndEdit(NMHDR *pNotifyStruct, LRESULT* pResult)
{
	NM_GRIDVIEW* pItem = (NM_GRIDVIEW*) pNotifyStruct;

	ndxml *xml =(ndxml *) m_grad->GetItemData(pItem->iRow, pItem->iColumn) ;
	int ret = -1 ;
	if(xml){
		CString str1 = m_grad->GetItemText(pItem->iRow, pItem->iColumn) ;
		if (GetXmlValType(xml, m_config) == EDT_ENUM) {
			int sel = -1;
			text_list_t enum_texts;
			if (getEnumList(xml, m_config, enum_texts) > 0) {
				for (int i = 0; i < enum_texts.size(); i++)	{
					if (str1 == enum_texts[i] ) {
						sel = i;
						break;
					}
				}
			}
			if (-1 != sel) {
				str1.Format("%d", sel);
				ret = ndxml_setval(xml, (char*)(LPCTSTR)str1);
			}
			
		}
		else if (GetXmlValType(xml, m_config) == EDT_KEY_VAL) {
			int sel = -1;
			text_list_t enum_texts;
			if (getEnumList(xml, m_config, enum_texts) > 0) {
				for (int i = 0; i < enum_texts.size(); i++)	{
					if (str1 == enum_texts[i]) {
						sel = i;
						break;
					}
				}
			}
			if (-1 != sel) {
				text_list_t value_texts;
				getKeyValueList(xml, m_config, value_texts);
				if (sel < value_texts.size()){
					ndxml_setval(xml, (char*)(LPCTSTR)value_texts[sel]);
					ret = true;
				}
				else {
					AfxMessageBox("配置文件错误,key和value长度不一致");
				}
			}

		}
		else if(!str1.IsEmpty()) {
			ret = ndxml_setval(xml, (char*)(LPCTSTR)str1) ;
			m_grad->Invalidate();
			CfgChanged() ;
			ret = 0;
		}
	}
	*pResult = ret;
}

// GVN_SELCHANGED
void CXmlCfg::OnGridEndSelChange(NMHDR *pNotifyStruct, LRESULT* /*pResult*/)
{
	NM_GRIDVIEW* pItem = (NM_GRIDVIEW*) pNotifyStruct;
	//Trace(_T("End Selection Change on row %d, col %d (%d Selected)\n"), 
	//	pItem->iRow, pItem->iColumn, m_Grid.GetSelectedCount());
	ndxml *xml =(ndxml *) m_grad->GetItemData(pItem->iRow, pItem->iColumn) ;
	if(xml){
		if (GetXmlValType(xml, m_config) == EDT_BOOL) {
			CGridCellCheck *pcell =(CGridCellCheck *) m_grad->GetCell(pItem->iRow, pItem->iColumn) ;
			if(pcell) {
				char *val = pcell->GetCheck() ? "0":"1" ;
				ndxml_setval(xml, val) ;
				CfgChanged() ;
			}
		}
	}

}


void CXmlCfg::OnGridClick(NMHDR *pNotifyStruct, LRESULT* /*pResult*/)
{
	NM_GRIDVIEW* pItem = (NM_GRIDVIEW*) pNotifyStruct;
	//Trace(_T("Clicked on row %d, col %d\n"), pItem->iRow, pItem->iColumn);
	_beginEdit(pItem->iRow, pItem->iColumn);
}

void CXmlCfg::OnBnClickedCancel()
{
	// TODO: Add your control notification handler code here
	if(m_stat) {
		nd_chdir( nd_getcwd()) ;
		ndxml_destroy(m_root) ;
		ndxml_load(BACKUP_NAME,m_root) ;
	}
	nd_rmfile(BACKUP_NAME) ;
	OnCancel();
}

void CXmlCfg::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	nd_rmfile(BACKUP_NAME) ;
	OnOK();
// 	if(m_stat)
// 		OnOK();
// 	else
// 		OnCancel();
}

#include "ctrls/MyPopMenu.h"
void CXmlCfg::OnNMRClickTree1(NMHDR *pNMHDR, LRESULT *pResult)
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	

	DWORD dwPos = GetMessagePos() ;
	CPoint point(LOWORD(dwPos), HIWORD(dwPos));
	m_pTree->ScreenToClient(&point);
	HTREEITEM hItem = m_pTree->HitTest( point );
	if (! hItem )	{
		return ;
	}
	// Select item clicked on
	m_pTree->SelectItem( hItem );
	m_hCurItem = hItem ;

	ndxml *xml = (ndxml*) m_pTree->GetItemData(m_hCurItem) ;
	if(!xml) {
		return ;
	}

	CMyPopMenu menu ;
	ndxml *create_template = GetCreateTemplate(xml, m_config) ;
	if(!create_template) {
		if(CheckCanDelete(xml)) {
			menu.SetMenuId(IDR_MENU_XML_DEL) ;
		}
		else 
			return ;
	}
	else {
		menu.SetMenuId(IDR_MENU_INSERT) ;
	}

	CPoint pi ;
	GetCursorPos(&pi) ;
	menu.ShowMenu(this,pi) ;

	*pResult = 0;
}

//////////////////////////////////////////////////////////////////////////
//通过模板创建
int GetCreateName(ndxml *xml_template, char *buf, size_t size)
{
	char *p;
	p = (char*)ndxml_getattr_val(xml_template, "name");
	if (!p) {
		return -1;
	}
	else {
		strncpy(buf, p, size);
	}
	return 0;
	
}

ndxml * CreateByTemplate(ndxml *root, ndxml *xml_template)
{
	char *val,*p ;
	char name[128] ;
	if (0!=GetCreateName(xml_template, name, sizeof(name))){
		return NULL;
	}
	val = (char*)ndxml_getattr_val(xml_template, "value");

	ndxml *new_xml = ndxml_addsubnode(root,name, val) ;
	if(!new_xml) 
		return NULL ;

	//创建属性
	ndxml *attr_xml =ndxml_refsub(xml_template, "attribute") ;
	if(attr_xml){
		for(int i=0; i<ndxml_getsub_num(attr_xml); i++) {
			ndxml *sub_node = ndxml_refsubi(attr_xml,i) ;
			if(sub_node) {
				p = (char*)ndxml_getattr_val(sub_node, "name");
				val = (char*)ndxml_getattr_val(sub_node, "value");
				if(p && val) {
					ndxml_addattrib(new_xml,p,val);
				}
			}
		}
	}
	//创建子节点
	ndxml *child_node = ndxml_refsub(xml_template,"sub_node") ;
	if(child_node) {
		for(int i=0; i<ndxml_getsub_num(child_node); i++) {
			ndxml *sub_template = ndxml_refsubi(child_node,i) ;
			if(sub_template) 
				CreateByTemplate(new_xml, sub_template) ;
		}
	}
	return new_xml ;
}
//////////////////////////////////////////////////////////////////////////
void CXmlCfg::OnPopInsertNode()
{
	if(!m_hCurItem)
		return ;

	ndxml *xml = (ndxml*) m_pTree->GetItemData(m_hCurItem) ;
	if(!xml) {
		return ;
	}
	
	ndxml *create_template = GetCreateTemplate(xml, m_config) ;
	if(!create_template) {
		return ;
	}
	//获得模板类型
	int tempType = 0;
	char *p = (char*)ndxml_getattr_val(create_template, "create_type");
	if (p)	{
		tempType = atoi(p);
	}
	//显示创建模板
	if (tempType == TEMPLATE_LIST){
		ndxml *temp_root = ndxml_getnode(m_config, _GetReverdWord(ERT_TEMPLATE));
		if (!temp_root)
			return;
		std::vector<const char*> creat_temp_list;
		InsertNodeListDlg dlg;
		for (int i = 0; i < ndxml_getsub_num(create_template); ++i){
			//获得被创建模板的显示名字
			ndxml *subnode = ndxml_refsubi(create_template,i);
			const char *temp_real_name = ndxml_getval(subnode);
			ndxml *temp_node = ndxml_refsub(temp_root, temp_real_name);
			CString str1;
			if (temp_node){
				str1 = (LPCTSTR)_GetXmlName(temp_node, &m_alias);
			}
			else{
				str1 = temp_real_name;
			}
			
			dlg.m_selList.push_back(str1);
			creat_temp_list.push_back(temp_real_name);
		}
		if (IDCANCEL==dlg.DoModal()){
			return;
		}
		int seled = dlg.GetSelect();
		if (seled==-1) {
			return;
		}
		create_template = ndxml_refsub(temp_root, creat_temp_list[seled]);
		if (!create_template){
			return;
		}
		p = (char*)ndxml_getattr_val(create_template, "create_type");
		if (p)	{
			tempType = atoi(p);
		}
	}


	ndxml *new_xml=NULL;
	if (tempType == TEMPLATE_CREAT){
		new_xml = CreateByTemplate(xml, create_template);
		if (new_xml) {
			SetXmlName(new_xml, xml);
			CfgChanged();
			HTREEITEM hNodeNew = InitTreeNode(new_xml, m_hCurItem);
			if (hNodeNew) {
				m_pTree->SelectItem(hNodeNew);
			}
			m_pTree->Invalidate(true);
		}
	}
	else if (tempType == TEMPLATE_DIRECT) {
		for (int i = 0; i< ndxml_num(create_template); i++)	{
			ndxml *sub1 = ndxml_refsubi(create_template, i);
			new_xml = ndxml_copy(sub1);
			
			if (new_xml) {
				ndxml_insert(xml, new_xml);
				SetXmlName(new_xml, xml);

				CfgChanged();
				HTREEITEM hNodeNew = InitTreeNode(new_xml, m_hCurItem);
				if (hNodeNew) {
					m_pTree->SelectItem(hNodeNew);
				}
				m_pTree->Invalidate(true);
			}
		}
	}
	
	

}


void CXmlCfg::OnTvnEndlabeleditTree1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTVDISPINFO pTVDispInfo = reinterpret_cast<LPNMTVDISPINFO>(pNMHDR);

	HTREEITEM hItem = m_pTree->GetSelectedItem() ;
	if(!hItem) {
		*pResult = -1;
		return ;
	}
	//CString str1 = m_pTree->GetItemText(hItem) ;
	CEdit* pedit = m_pTree->GetEditControl() ;
	if(!pedit) {
		*pResult = -1;
		return ;
	}
	CString str1 ;
	pedit->GetWindowText(str1);

	if(str1.IsEmpty()) {
		*pResult = -1;
		return ;
	}
	ndxml *xml =(ndxml *) m_pTree->GetItemData(hItem) ;
	if(!xml) {
		*pResult = -1;
		return ;		
	}
	if(0==ndxml_setattrval(xml, "name", (char*)(LPCTSTR)str1) ) {
		m_pTree->SetItemText(hItem, (LPCTSTR)str1) ;
		CfgChanged();
	}
	*pResult = 0 ;
}

void CXmlCfg::OnPopXmlDel()
{
	if(!m_hCurItem)
		return ;
	ndxml *xml = (ndxml*) m_pTree->GetItemData(m_hCurItem) ;

	if(xml) {
		if(! CheckCanDelete(xml) ) {
			AfxMessageBox("当前节点不允许删除!") ;
			return ;
		}
	}
	else {
		return ;
	}

	HTREEITEM hParent = m_pTree->GetParentItem(m_hCurItem) ;
	if(!hParent ) {
		return ;
	}
	if(IDYES!=AfxMessageBox("确实要删除吗?",MB_YESNO) ) {
		return ;
	}

	ndxml *xml_parent = (ndxml *)m_pTree->GetItemData(hParent);
	if (xml_parent) {
		ndxml_delxml(xml,xml_parent);
		CreateXmlTree(m_root);
		CfgChanged();
	}

}

ndxml* CXmlCfg::GetClickTreeXml()
{
	DWORD dwPos = GetMessagePos();
	CPoint point(LOWORD(dwPos), HIWORD(dwPos));
	m_pTree->ScreenToClient(&point);
	HTREEITEM hItem = m_pTree->HitTest(point);
	if (hItem)	{
		m_hCurItem = hItem;
		return (ndxml *)m_pTree->GetItemData(hItem);
	}
	return NULL;
}

void CXmlCfg::OnAccEnter()
{
	NextTabOrder();
}

void CXmlCfg::OnTvnItemexpandedTree1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);

	
	ndxml *xml = GetClickTreeXml();
	if (xml) {
		if (pNMTreeView->action == TVE_COLLAPSE)	{
			ndxml_setattrval(xml, "expand_stat", "0");
		}
		else if (pNMTreeView->action == TVE_EXPAND)	{
			ndxml_setattrval(xml, "expand_stat", "1");
		}
		CfgChanged();
	}

	*pResult = 0;
}

bool CXmlCfg::TreeDragCallback(HTREEITEM  hFrom, HTREEITEM hTo)
{
	HTREEITEM parent1 = m_pTree->GetParentItem(hFrom);
	HTREEITEM parent2 = m_pTree->GetParentItem(hTo);
	if (parent2!=parent1){
		return false;
	}
	ndxml *xmlparent = GetSelXml(parent1);
	ndxml*xmlFrom = GetSelXml(hFrom);
	ndxml*xmlTo = GetSelXml(hTo);
	if (!xmlparent|| !xmlFrom || !xmlTo)	{
		return false;
	}
	const char *pEnableDrag = ndxml_getattr_val(xmlparent, "enable_drag");
	if (!pEnableDrag){
		return false;
	}
	if (0!=ndstricmp(pEnableDrag,"yes")){
		return false;
	}

	int index = ndxml_get_index(xmlparent, xmlTo);
	int src_index = ndxml_get_index(xmlparent, xmlFrom);
	if (-1==index || -1==src_index){
		return false;
	}
	if (-1==ndxml_remove(xmlFrom, xmlparent)){
		return false;
	}
	if (src_index > index) {
		index -= 1;
	}
	if (-1 == ndxml_insert_ex(xmlparent, xmlFrom, index)) {
		return false;
	}

	CreateXmlTree(m_root);
	CfgChanged();
	return true;
}
