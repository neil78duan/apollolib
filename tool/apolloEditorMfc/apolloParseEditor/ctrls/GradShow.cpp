// GradShow.cpp : implementation file
//

#include "stdafx.h"
#include "GradShow.h"


// CGradShow

IMPLEMENT_DYNAMIC(CGradShow, CGridCtrl)

CGradShow::CGradShow()
{

}

CGradShow::~CGradShow()
{
}


BEGIN_MESSAGE_MAP(CGradShow, CGridCtrl)
END_MESSAGE_MAP()



// //show test in a row



int CGradShow::ShowText(CString *  text, int number)
{
	LOGFONT lf_default = {12, 0, 0, 0, FW_NORMAL, 0, 0, 0,
		DEFAULT_CHARSET, OUT_CHARACTER_PRECIS, CLIP_DEFAULT_PRECIS,
		PROOF_QUALITY, VARIABLE_PITCH|FF_ROMAN|FF_ROMAN, "宋体"};


	//int row = number ; //显示名字说明,编号
	int row = GetRowCount() + 1;
	int col = number ; 
	try {
		if(row >= 1)
			SetRowCount(row);
		SetColumnCount(col);
	}
	catch (CMemoryException* e)
	{
		e->ReportError();
		e->Delete();
		return -1;
	}

	GV_ITEM Item;

	//显示编号
	Item.mask = GVIF_TEXT|GVIF_FORMAT;
	Item.nFormat = DT_LEFT|DT_WORDBREAK;
	for (int i=0; i<number; i++){
		Item.lParam =0 ;
		Item.row = row -1;
		Item.col = i;
		Item.strText =text[i] ;
		SetItem(&Item);	
		SetItemState(Item.row,Item.col, GetItemState(Item.row,Item.col) |GVIS_READONLY);
		
		SetItemFont(Item.row ,Item.col,&lf_default);
		
		
	}
	AutoSize(GVS_BOTH) ;
	return 0;
}

void CGradShow::Clear(void)
{
	EndEditing();
	
	SetRowCount(1);
	//SetColumnCount(0);

	SetEditable(TRUE);
	EnableSelection(FALSE) ;

	SetFixedRowCount();
	SetFixedColumnCount();
}
