// MyPopMenu.cpp: implementation of the CMyPopMenu class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MyPopMenu.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMyPopMenu::CMyPopMenu()
{
	m_menuID = 0 ;
}

CMyPopMenu::CMyPopMenu(UINT menuid):m_menuID(menuid)
{
	
}
CMyPopMenu::~CMyPopMenu()
{

}

//ÏÔÊ¾µ¯³ö²Ëµ¥
int CMyPopMenu::ShowMenu(CWnd *pWnd, CPoint &point,int subMenu)
{
	if (m_menuID == 0 ) {
		TRACE0("No control bar menu defined.\n");
		return -1;
	}

	if (point.x==-1 && point.y == -1)	{
		//keystroke invocation
		CRect rect;
		pWnd->GetClientRect(rect);
		pWnd->ClientToScreen(rect);
		
		point = rect.TopLeft();
		point.Offset(5, 5);
	}
	
	CMenu menu;
	VERIFY(menu.LoadMenu(m_menuID));
	
	//tests
	//menu.GetSubMenu(0)->InsertMenu(0, 1, 2001, "test");
	//menu.AppendMenu(MF_POPUP | MF_STRING, 0, _T("testPop(&t)"));
	
	CMenu* pPopup = menu.GetSubMenu(subMenu);
	ASSERT(pPopup != NULL);
	CWnd* pWndPopupOwner = pWnd;
	
	while (pWndPopupOwner->GetStyle() & WS_CHILD)
		pWndPopupOwner = pWndPopupOwner->GetParent();
	
	pPopup->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y,
		pWndPopupOwner);

	return 0 ;
}

int CMyPopMenu::ShowMenu(CWnd *pWnd)
{
	POINT point ;
	if(!::GetCursorPos(&point) ){
		return -1;
	}
	CPoint cp(point) ;
	return CMyPopMenu::ShowMenu(pWnd , cp) ;
}
