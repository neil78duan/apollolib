#pragma once

#include "GridCtrl_src/GridCtrl.h"
//show grad control 
// CGradShow

class CGradShow : public CGridCtrl
{
	DECLARE_DYNAMIC(CGradShow)

public:
	CGradShow();
	virtual ~CGradShow();

protected:
	DECLARE_MESSAGE_MAP()
public:
	// //show test in a row
	int ShowText(CString *  text, int number);
	void Clear(void);
};


