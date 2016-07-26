// MyPopMenu.h: interface for the CMyPopMenu class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MYPOPMENU_H__CBB20C55_2BF2_40CF_BC94_195AF721CC9A__INCLUDED_)
#define AFX_MYPOPMENU_H__CBB20C55_2BF2_40CF_BC94_195AF721CC9A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class   CMyPopMenu  
{
public:
	int ShowMenu(CWnd *pWnd);
	int ShowMenu(CWnd *pWnd , CPoint &point,int subMenu=0);
	CMyPopMenu();
	CMyPopMenu(UINT);
	virtual ~CMyPopMenu();
	void SetMenuId(UINT id) {m_menuID=id;}
	UINT GetMenuId() {return m_menuID;}
protected :
	UINT m_menuID ;
};

#endif // !defined(AFX_MYPOPMENU_H__CBB20C55_2BF2_40CF_BC94_195AF721CC9A__INCLUDED_)
