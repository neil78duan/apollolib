/* file nd_winfont.h
 * set windows font
 * neil
 * 2007-4-25 12:54
 * version 1.0
 */

#include "stdafx.h"
#include <tchar.h>
class ND_Cfont
{
public :
	ND_Cfont(LPCTSTR fontname=NULL):m_hFont(0),m_nFontSize(14)
	{
		SetFaceName(fontname);
	}
	virtual ~ND_Cfont() {
		if(m_hFont){
			DeleteObject(m_hFont);
			m_hFont=0;
		}
	}
public:
	HFONT GetFont(void){return m_hFont;}
	LPCTSTR  GetFaceName(void){return m_szFaceName;}
	int GetFontSize(void){return m_nFontSize ;}
	void SetFontSize(int size){m_nFontSize =size;}
	void SetFaceName(LPCTSTR szFaceName);
	bool m_bRedraw ;
private:
	TCHAR  m_szFaceName[LF_FACESIZE];
	int m_nFontSize;
	HFONT m_hFont;

	void createFont(void);
} ;

void ND_Cfont::SetFaceName(LPCTSTR szFaceName)
{
	strcpy_s(m_szFaceName,LF_FACESIZE,szFaceName) ;
	createFont();
}
void ND_Cfont::createFont(void)
{
	LOGFONT lf_default = {14, 0, 0, 0, FW_NORMAL, 0, 0, 0,
			DEFAULT_CHARSET, OUT_CHARACTER_PRECIS, CLIP_DEFAULT_PRECIS,
			PROOF_QUALITY, VARIABLE_PITCH|FF_ROMAN|FF_ROMAN, "ËÎÌו"};
	if(m_nFontSize>0) 
		lf_default.lfHeight = m_nFontSize ;
	if(m_szFaceName[0])
		strcpy_s(lf_default.lfFaceName,LF_FACESIZE,m_szFaceName ) ;
	
	if(m_hFont) {
		DeleteObject(m_hFont) ;
		m_hFont = 0 ;
	}
	m_hFont = ::CreateFontIndirect(&lf_default) ;
}


static  ND_Cfont s_Font(_T("ËÎÌו"));


static BOOL __stdcall SetChildFont(HWND hwnd, LPARAM lparam)
{
	ND_Cfont *pFont = (ND_Cfont*)lparam;
	SendMessage(hwnd, WM_SETFONT, (WPARAM)pFont->GetFont(), (LPARAM)pFont->m_bRedraw);
	return TRUE;
}

BOOL ND_SetChildrenFont(HWND hwnd,bool bRedraw,int nSize, LPCTSTR lpszName) 
{
	if(nSize) 
		s_Font.SetFontSize(nSize) ;
	if(lpszName)
		s_Font.SetFaceName(lpszName);
	s_Font.m_bRedraw = bRedraw ;
	return ::EnumChildWindows(hwnd, ::SetChildFont, (LPARAM)&s_Font);
}
