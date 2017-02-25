/* file apoUiExeNode.h
 *
 * logic parser editor base exe-node
 *
 * create by duan
 *
 * 2017.2.7
 */

#ifndef _APOUI_EXENODE_H_
#define _APOUI_EXENODE_H_

#include "src/apoBaseExeNode.h"
#include <QWidget>
#include <QVector>
#include <QLabel>
#include <QPushButton>



#ifdef APOUI_NODE_ID
#undef APOUI_NODE_ID
#endif
#define APOUI_NODE_ID(id) EAPO_EXE_NODE_##id,

enum eApoExeNodeType {
#include "src/apouinodedef.h"
	EAPO_EXE_NODE_NUMBER 
};
#undef APOUI_NODE_ID

apoBaseExeNode *g_apoCreateExeNode(int index, QWidget *parent);
apoBaseExeNode *g_apoCreateExeNode(const char *objName, QWidget *parent);

class apoUiExenodeCommon : public apoBaseExeNode
{
public:
	explicit apoUiExenodeCommon(QWidget *parent) :apoBaseExeNode()
	{
		apoBaseExeNode::m_type = EAPO_EXE_NODE_Common;
		disableReturnVar();
		disableNewParam();
		InitCtrl(parent, "Common", "common", 0);
		setTips(QString("common node"));
	}
	virtual ~apoUiExenodeCommon(){}
private:

};



class apoUiExenodeFuncEntry : public apoBaseExeNode
{
public:
	explicit apoUiExenodeFuncEntry(QWidget *parent) :apoBaseExeNode()
	{
		apoBaseExeNode::m_type = EAPO_EXE_NODE_FuncEntry;
		disableReturnVar();
		disableNewParam();
		disableConnectIn();

		InitCtrl(parent, "Begin",NULL, 0);
		setTips(QString("Function entry"));
	}
	virtual ~apoUiExenodeFuncEntry(){}
private:

};


class apoUiExenodeModuleInitEntry : public apoBaseExeNode
{
public:
	explicit apoUiExenodeModuleInitEntry(QWidget *parent) :apoBaseExeNode()
	{
		apoBaseExeNode::m_type = EAPO_EXE_NODE_ModuleInitEntry;
		disableReturnVar();
		disableNewParam();
		disableConnectIn();
		InitCtrl(parent, "Initializer", NULL, 0);
		setTips(QString("Run-in-loaded"));
	}
	virtual ~apoUiExenodeModuleInitEntry(){}
private:

};


class apoUiExenodeExceptionEntry : public apoBaseExeNode
{
public:
	explicit apoUiExenodeExceptionEntry(QWidget *parent) :apoBaseExeNode()
	{
		apoBaseExeNode::m_type = EAPO_EXE_NODE_ExceptionEntry;
		disableReturnVar();
		disableNewParam();
		disableConnectIn();
		InitCtrl(parent, "Exception", NULL, 0);
		setTips(QString("Run-on-except"));
	}
	virtual ~apoUiExenodeExceptionEntry(){}
private:

};

// 
// class apoUiExenodeCallFunc : public apoBaseExeNode
// {
// public:
// 	explicit apoUiExenodeCallFunc(QWidget *parent) :apoBaseExeNode()
// 	{
// 		apoBaseExeNode::m_type = EAPO_EXE_NODE_CALL_FUNCTION;
// 
// 		InitCtrl(parent, "Call", "none", 0);
// 		setTips(QString("call function"));
// 	}
// 	virtual ~apoUiExenodeCallFunc(){}
// 
// 
// 	virtual void paramCtrlDbClicked(apoBaseParam *paramCtrl)
// 	{
// 
// 	}
// 	virtual void nameCtrlDbClicked()
// 	{
// 
// 	}
// private:
// 
// };


class apoUiExenodeNewVar : public apoBaseExeNode
{
public:
	explicit apoUiExenodeNewVar(QWidget *parent) :apoBaseExeNode()
	{
		apoBaseExeNode::m_type = EAPO_EXE_NODE_NewVar;

		disableNewParam();
		InitCtrl(parent, "Variant", "none", 1);
		setTips(QString("Create Var"));
	}
	virtual ~apoUiExenodeNewVar(){}


	virtual void paramCtrlDbClicked(apoBaseParam *paramCtrl)
	{

	}
	virtual void nameCtrlDbClicked()
	{

	}

	virtual void onParamCreated(apoBaseParam *paramCtrl)
	{

	}
private:

};


class apoUiExenodeNewJson : public apoBaseExeNode
{
public:
	explicit apoUiExenodeNewJson(QWidget *parent) :apoBaseExeNode()
	{
		apoBaseExeNode::m_type = EAPO_EXE_NODE_NewJson;

		InitCtrl(parent, "JSon", "none", 0);
		setTips(QString("Create Json data"));
	}
	virtual ~apoUiExenodeNewJson(){}


	virtual void paramCtrlDbClicked(apoBaseParam *paramCtrl)
	{

	}
	virtual void nameCtrlDbClicked()
	{

	}
	virtual void onParamCreated(apoBaseParam *paramCtrl)
	{

	}
private:

};

class apoUiExenodeNewStruct : public apoBaseExeNode
{
public:
	explicit apoUiExenodeNewStruct(QWidget *parent) :apoBaseExeNode()
	{
		apoBaseExeNode::m_type = EAPO_EXE_NODE_NewStruct;

		InitCtrl(parent, "Struct", "none", 0);
		setTips(QString("define struct"));
	}
	virtual ~apoUiExenodeNewStruct(){}


	virtual void paramCtrlDbClicked(apoBaseParam *paramCtrl)
	{

	}
	virtual void nameCtrlDbClicked()
	{

	}
	virtual void onParamCreated(apoBaseParam *paramCtrl)
	{

	}
private:

};


class apoUiExenodeAssignment : public apoBaseExeNode
{
public:
	explicit apoUiExenodeAssignment(QWidget *parent) :apoBaseExeNode()
	{
		apoBaseExeNode::m_type = EAPO_EXE_NODE_Assignment;

		disableNewParam();
		InitCtrl(parent, "=", NULL, 2);
		setTips(QString(" (a = b)"));
	}
	virtual ~apoUiExenodeAssignment(){}


	virtual void paramCtrlDbClicked(apoBaseParam *paramCtrl)
	{

	}
private:

};


class apoUiExenodeBreak : public apoBaseExeNode
{
public:
	explicit apoUiExenodeBreak(QWidget *parent) :apoBaseExeNode()
	{
		apoBaseExeNode::m_type = EAPO_EXE_NODE_Break;

		disableNewParam();
		disableToNext();
		InitCtrl(parent, "Break", NULL, 2);
		setTips(QString("Quit loop"));
	}
	virtual ~apoUiExenodeBreak(){}

private:

};


class apoUiExenodeTrueBreak : public apoBaseExeNode
{
public:
	explicit apoUiExenodeTrueBreak(QWidget *parent) :apoBaseExeNode()
	{
		apoBaseExeNode::m_type = EAPO_EXE_NODE_TrueBreak;

		disableNewParam();
		InitCtrl(parent, "Break-true", NULL, 2);
		setTips(QString("Quit on TRUE"));
	}
	virtual ~apoUiExenodeTrueBreak(){}

private:

};

class apoUiExenodeFalseBreak : public apoBaseExeNode
{
public:
	explicit apoUiExenodeFalseBreak(QWidget *parent) :apoBaseExeNode()
	{
		apoBaseExeNode::m_type = EAPO_EXE_NODE_FalseBreak;

		disableNewParam();
		InitCtrl(parent, "Break-false", NULL, 2);
		setTips(QString("Quit on FALSE"));
	}
	virtual ~apoUiExenodeFalseBreak(){}

private:

};

//-------------------


class apoUiExenodeQuit : public apoBaseExeNode
{
public:
	explicit apoUiExenodeQuit(QWidget *parent) :apoBaseExeNode()
	{
		apoBaseExeNode::m_type = EAPO_EXE_NODE_Quit;

		disableNewParam();
		disableToNext();
		InitCtrl(parent, "Return", NULL, 2);
		setTips(QString("Return function"));
	}
	virtual ~apoUiExenodeQuit(){}

private:

};


class apoUiExenodeTrueQuit : public apoBaseExeNode
{
public:
	explicit apoUiExenodeTrueQuit(QWidget *parent) :apoBaseExeNode()
	{
		apoBaseExeNode::m_type = EAPO_EXE_NODE_TrueBreak;

		disableNewParam();
		InitCtrl(parent, "return-true", NULL, 2);
		setTips(QString("return on TRUE"));
	}
	virtual ~apoUiExenodeTrueQuit(){}

private:

};

class apoUiExenodeFalseQuit : public apoBaseExeNode
{
public:
	explicit apoUiExenodeFalseQuit(QWidget *parent) :apoBaseExeNode()
	{
		apoBaseExeNode::m_type = EAPO_EXE_NODE_FalseBreak;

		disableNewParam();
		InitCtrl(parent, "Return-false", NULL, 2);
		setTips(QString("Return on FALSE"));
	}
	virtual ~apoUiExenodeFalseQuit(){}

private:

};
//------------
class apoUiExenodeLoop : public apoBaseExeNode
{
public:
	explicit apoUiExenodeLoop(QWidget *parent) :apoBaseExeNode()
	{
		apoBaseExeNode::m_type = EAPO_EXE_NODE_Loop;

		disableNewParam();
		InitCtrl(parent, "Loop", NULL, 1);
		setTips(QString("loop $count times "));


	}
	virtual ~apoUiExenodeLoop(){}

	virtual void onInit()
	{
		//create loop ctrl
		
		apoBaseSlotCtrl *ctrl1 = new apoBaseSlotCtrl(QString("{}"), this);	
		ctrl1->resize(PARAM_CTRL_W, PARAM_CTRL_H);
		ctrl1->setStyleSheet("QLabel{background-color:yellow;}");
		ctrl1->move(E_LINE_WIDTH - PARAM_CTRL_W, E_LINE_HEIGHT * 1.5);
		ctrl1->show();
		ctrl1->setAttribute(Qt::WA_DeleteOnClose, true);

		m_loopSlot = ctrl1;
	}

private:
	apoBaseSlotCtrl *m_loopSlot;
};


class apoUiExenodeBool : public apoBaseExeNode
{
public:
	explicit apoUiExenodeBool(QWidget *parent) :apoBaseExeNode()
	{
		apoBaseExeNode::m_type = EAPO_EXE_NODE_Bool;

		disableNewParam();
		InitCtrl(parent, "Bool", NULL, 0);
		setTips(QString("if-else "));


	}
	virtual ~apoUiExenodeBool(){}

	virtual void onInit()
	{
		//create loop ctrl

		apoBaseSlotCtrl *ctrl1 = new apoBaseSlotCtrl(QString("T"), this);
		ctrl1->resize(PARAM_CTRL_W, PARAM_CTRL_H);
		ctrl1->setStyleSheet("QLabel{background-color:green;}");
		ctrl1->move(E_LINE_WIDTH - PARAM_CTRL_W, E_LINE_HEIGHT * 1.5);
		ctrl1->show();
		ctrl1->setAttribute(Qt::WA_DeleteOnClose, true);
		m_trueSlot = ctrl1;


		ctrl1 = new apoBaseSlotCtrl(QString("F"), this);
		ctrl1->resize(PARAM_CTRL_W, PARAM_CTRL_H);
		ctrl1->setStyleSheet("QLabel{background-color:red;}");
		ctrl1->move(E_LINE_WIDTH - PARAM_CTRL_W, E_LINE_HEIGHT * 2.5);
		ctrl1->show();
		ctrl1->setAttribute(Qt::WA_DeleteOnClose, true);
		m_falseSlot = ctrl1;
	}

private:
	apoBaseSlotCtrl *m_trueSlot;
	apoBaseSlotCtrl *m_falseSlot;
};


class apoUiExenodeValueCompare : public apoBaseExeNode
{
public:
	explicit apoUiExenodeValueCompare(QWidget *parent) :apoBaseExeNode()
	{
		apoBaseExeNode::m_type = EAPO_EXE_NODE_ValueCompare;

		disableNewParam();
		InitCtrl(parent, "Compare", NULL, 2);
		setTips(QString("if(a>b)"));


	}
	virtual ~apoUiExenodeValueCompare(){}

	virtual void onInit()
	{
		//create loop ctrl

		apoBaseSlotCtrl *ctrl1 = new apoBaseSlotCtrl(QString("T"), this);
		ctrl1->resize(PARAM_CTRL_W, PARAM_CTRL_H);
		ctrl1->setStyleSheet("QLabel{background-color:green;}");
		ctrl1->move(E_LINE_WIDTH - PARAM_CTRL_W, E_LINE_HEIGHT * 1.5);
		ctrl1->show();
		ctrl1->setAttribute(Qt::WA_DeleteOnClose, true);
		m_trueSlot = ctrl1;

// 
// 		ctrl1 = new apoBaseSlotCtrl(QString("F"), this);
// 		ctrl1->resize(PARAM_CTRL_W, PARAM_CTRL_H);
// 		ctrl1->setStyleSheet("QLabel{background-color:red;}");
// 		ctrl1->move(E_LINE_WIDTH - PARAM_CTRL_W, E_LINE_HEIGHT * 2.5);
// 		ctrl1->show();
// 		ctrl1->setAttribute(Qt::WA_DeleteOnClose, true);
// 		m_falseSlot = ctrl1;
	}

private:
	apoBaseSlotCtrl *m_trueSlot;
	//apoBaseSlotCtrl *m_falseSlot;
};



#endif // _APOUI_EXENODE_H_
