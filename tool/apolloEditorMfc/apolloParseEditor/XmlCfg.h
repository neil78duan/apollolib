#pragma once


#include "nd_common/nd_common.h"
#include "ctrls/GridCtrl_src/GridCtrl.h"
#include "ctrls/GradShow.h"
#include "resource.h"
#include "DrawTreeCtrl.h"
#include "logic_parser/logic_editor_helper.h"

BOOL ND_SetChildrenFont(HWND hwnd,bool bRedraw,int nSize, LPCTSTR lpszName)  ;

#include <string>
#include <vector>
#include <map>
using namespace std ;

using namespace LogicEditorHelper;

// CXMLParserWnd window

typedef std::vector<CString> text_list_t;
// CXmlCfg dialog
#include "ctrls/mydlg.h"
class CXmlCfg : public CDialog
{
//	DECLARE_DYNAMIC(CXmlCfg)

	enum {E_COL_NUM=2};
public:
	CXmlCfg(CWnd* pParent = NULL, dlgAccCallBack initfunc = NULL, dlgAccCallBack okfunc = NULL, dlgAccCallBack cancelfunc = NULL);   // standard constructor
	virtual ~CXmlCfg();

	void SetXML(ndxml_root *config_xml, ndxml_root *data_root);
	bool TreeDragCallback(HTREEITEM hFrom, HTREEITEM hTo);
	ndxml* GetCurrentXml();

	bool loadUserdefDisplayList(ndxml_root &xmlNameList, const char *name);
	bool addDisplayNameList(const char *name, text_list_t &text_list);
// Dialog Data
	enum { IDD = IDD_CFG_EDIT };

	ndxml_root *m_root ;
	ndxml_root *m_config;

	CImageList m_image;
	CDrawTreeCtrl m_drawTree;
	CXMLAlias m_alias ;
	CTreeCtrl *m_pTree ;
	HTREEITEM m_hRootItem ;	//树根
	HTREEITEM m_hCurItem;
	int m_stat ;			//0 xml is nothing, 1   2 changed

	CGradShow *m_grad ;
protected:
	bool _TreeDragInNotSameRoot(HTREEITEM hFrom, HTREEITEM hTo);
	bool _GetUserDefinData(const char *pUserParam, CString &val);
	bool _GetUserSelEnumVal(const text_list_t &enum_vals, CString &val);
	bool _beginEdit(int nRow, int nCol);
	bool CreateXmlTree(ndxml_root *xml_root) ;
	bool ExpandTree(HTREEITEM hItem);
	void SetExpand(HTREEITEM hItem);
	HTREEITEM InitTreeNode(ndxml *xml_node, HTREEITEM hParent, HTREEITEM hiAfter = TVI_LAST);
	int ShowXMLValue(ndxml *xml_node,CGradShow *show_ctrl, int expand =0) ;	//显示xml的内容
	int DisplaySelXml() ;
	ndxml* GetSelXml(HTREEITEM hItem);
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	void CfgChanged() {m_stat = 1;}
	void ShowRow(const char *name, const char *val, ndxml* param) ;
	void SetXmlName(ndxml *xml_node, ndxml *xmlParent);
	void SetGridSize();

	void OnAccEnter();
	ndxml* GetClickTreeXml();

	typedef std::map<std::string, text_list_t > text_map_t;
	text_map_t m_userDefineList;

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnNMDblclkTree1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMReturnTree1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTvnSelchangedTree1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnGridBeginEdit(NMHDR *pNotifyStruct, LRESULT* pResult);
	afx_msg void OnGridEndEdit(NMHDR *pNotifyStruct, LRESULT* pResult);
	afx_msg void OnGridClick(NMHDR *pNotifyStruct, LRESULT* pResult);
	afx_msg void OnGridEndSelChange(NMHDR *pNotifyStruct, LRESULT* pResult);
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedOk();
	afx_msg void OnNMRClickTree1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnPopInsertNode();
	afx_msg void OnTvnEndlabeleditTree1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnPopXmlDel();
	afx_msg void OnTvnItemexpandedTree1(NMHDR *pNMHDR, LRESULT *pResult);
};
