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

#include "apoScript/apoBaseExeNode.h"
#include <QWidget>
#include <QVector>
#include <QLabel>
#include <QPushButton>



#ifdef APOUI_NODE_ID
#undef APOUI_NODE_ID
#endif
#define APOUI_NODE_ID(id) EAPO_EXE_NODE_##id,

enum eApoExeNodeType {
#include "apoScript/apouinodedef.h"
	EAPO_EXE_NODE_NUMBER 
};
#undef APOUI_NODE_ID

apoBaseExeNode *g_apoCreateExeNode(int index, ndxml *xml,QWidget *parent);
apoBaseExeNode *g_apoCreateExeNode(ndxml *xml, QWidget *parent);

class apoUiExenodeCommon : public apoBaseExeNode
{
public:
	explicit apoUiExenodeCommon(QWidget *parent ,ndxml *exeNodeXml) :apoBaseExeNode()
	{
		apoBaseExeNode::m_type = EAPO_EXE_NODE_Common;
		//disableReturnVar();
		//disableNewParam();

		setNodeInfo(parent, exeNodeXml);
		//InitCtrl(parent, "Common",  0);
		//setTips(QString("common"));
	}
	virtual ~apoUiExenodeCommon(){}
private:

};



class apoUiExenodeFuncEntry : public apoBaseExeNode
{
public:
	explicit apoUiExenodeFuncEntry(QWidget *parent, ndxml *exeNodeXml) :apoBaseExeNode()
	{
		apoBaseExeNode::m_type = EAPO_EXE_NODE_FuncEntry;
		disableReturnVar();
		disableNewParam();
		disableConnectIn();
		enableOutParam();

		setNodeInfo(parent, exeNodeXml,false);
		//InitCtrl(parent, "Begin", 0);
		setTips(QString("Entry"));

		ndxml *toNextAnchor = ndxml_getnode(m_nodeXml, "comment");
		if (toNextAnchor){
			m_toNextNode->setXmlAnchor(toNextAnchor);
		}
		m_toNextNode->setXmlAnchorParent(m_nodeXml);

		m_beDeleted = false;
	}
	virtual ~apoUiExenodeFuncEntry(){}
private:

};


class apoUiExenodeModuleInitEntry : public apoBaseExeNode
{
public:
	explicit apoUiExenodeModuleInitEntry(QWidget *parent, ndxml *exeNodeXml) :apoBaseExeNode()
	{
		apoBaseExeNode::m_type = EAPO_EXE_NODE_ModuleInitEntry;
		disableReturnVar();
		disableNewParam();
		disableConnectIn();

		setNodeInfo(parent, exeNodeXml,false);
		//InitCtrl(parent, "Initializer",  0);
		setTips(QString("Run-in-loaded"));

		ndxml *toNextAnchor = ndxml_getnode(m_nodeXml, "comment");
		if (toNextAnchor){
			m_toNextNode->setXmlAnchor(toNextAnchor);
		}
		m_toNextNode->setXmlAnchorParent(m_nodeXml);
	}
	virtual ~apoUiExenodeModuleInitEntry(){}
private:

};


class apoUiExenodeExceptionEntry : public apoBaseExeNode
{
public:
	explicit apoUiExenodeExceptionEntry(QWidget *parent, ndxml *exeNodeXml) :apoBaseExeNode()
	{
		apoBaseExeNode::m_type = EAPO_EXE_NODE_ExceptionEntry;
		disableReturnVar();
		disableNewParam();
		disableConnectIn();
		setNodeInfo(parent, exeNodeXml,false);
		//InitCtrl(parent, "Exception",  0);
		setTips(QString("Run-on-except"));

		ndxml *toNextAnchor = ndxml_getnode(m_nodeXml, "comment");
		if (toNextAnchor){
			m_toNextNode->setXmlAnchor(toNextAnchor);
		}
		m_toNextNode->setXmlAnchorParent(m_nodeXml);
	}
	virtual ~apoUiExenodeExceptionEntry(){}
private:

};


class apoUiExenodeNewVar : public apoBaseExeNode
{
public:
	explicit apoUiExenodeNewVar(QWidget *parent, ndxml *exeNodeXml);

	virtual ~apoUiExenodeNewVar();
	const char *getVarName();

private:

};



class apoUiExenodeBreak : public apoBaseExeNode
{
public:
	explicit apoUiExenodeBreak(QWidget *parent, ndxml *exeNodeXml) :apoBaseExeNode()
	{
		apoBaseExeNode::m_type = EAPO_EXE_NODE_Break;
		disableReturnVar();
		disableNewParam();
		disableToNext();

		setNodeInfo(parent, exeNodeXml,false);
		//InitCtrl(parent, "Break",  2);
		setTips(QString("Quit loop"));
	}
	virtual ~apoUiExenodeBreak(){}

private:

};


class apoUiExenodeTrueBreak : public apoBaseExeNode
{
public:
	explicit apoUiExenodeTrueBreak(QWidget *parent, ndxml *exeNodeXml) :apoBaseExeNode()
	{
		apoBaseExeNode::m_type = EAPO_EXE_NODE_TrueBreak;
		disableReturnVar();
		disableNewParam();

		setNodeInfo(parent, exeNodeXml,false);
		//InitCtrl(parent, "Break-true",  2);
		setTips(QString("Quit on TRUE"));
	}
	virtual ~apoUiExenodeTrueBreak(){}

private:

};

class apoUiExenodeFalseBreak : public apoBaseExeNode
{
public:
	explicit apoUiExenodeFalseBreak(QWidget *parent, ndxml *exeNodeXml) :apoBaseExeNode()
	{
		apoBaseExeNode::m_type = EAPO_EXE_NODE_FalseBreak;
		disableReturnVar();
		disableNewParam();
		setNodeInfo(parent, exeNodeXml,false);
		//InitCtrl(parent, "Break-false",  2);
		setTips(QString("Quit on FALSE"));
	}
	virtual ~apoUiExenodeFalseBreak(){}

private:

};

//-------------------


class apoUiExenodeQuit : public apoBaseExeNode
{
public:
	explicit apoUiExenodeQuit(QWidget *parent, ndxml *exeNodeXml) :apoBaseExeNode()
	{
		apoBaseExeNode::m_type = EAPO_EXE_NODE_Quit;
		disableReturnVar();
		disableNewParam();
		disableToNext();

		setNodeInfo(parent, exeNodeXml,true);
		//InitCtrl(parent, "Return",  2);
		setTips(QString("Return function"));
	}
	virtual ~apoUiExenodeQuit(){}

private:

};

class apoUiExenodeThrow : public apoBaseExeNode
{
public:
	explicit apoUiExenodeThrow(QWidget *parent, ndxml *exeNodeXml) :apoBaseExeNode()
	{
		apoBaseExeNode::m_type = EAPO_EXE_NODE_FalseBreak;

		disableNewParam();
		disableReturnVar();
		disableToNext();

		setNodeInfo(parent, exeNodeXml);
		//InitCtrl(parent, "Throw",  1);
		setTips(QString("Throw exception"));

		//m_toNextNode->setValid(false);
	}
	virtual ~apoUiExenodeThrow(){}

private:

};
//------------
class apoUiExenodeLoop : public apoBaseExeNode
{
public:
	explicit apoUiExenodeLoop(QWidget *parent, ndxml *exeNodeXml);
	virtual ~apoUiExenodeLoop();

	virtual void onInit();

	apoBaseSlotCtrl *getLoopSlot() { return m_loopSlot; }
private:
	apoBaseSlotCtrl *m_loopSlot;
};



class apoUiExenodeBool : public apoBaseExeNode
{
public:
	explicit apoUiExenodeBool(QWidget *parent, ndxml *exeNodeXml);
	virtual ~apoUiExenodeBool();
	virtual void onInit();

	apoBaseSlotCtrl *getTrueSlot() { return m_trueSlot; }
	apoBaseSlotCtrl *getFalseSlot() { return m_falseSlot; }


private:
	apoBaseSlotCtrl *m_trueSlot;
	apoBaseSlotCtrl *m_falseSlot;
};

class apoUiExenodeValueComp : public apoBaseExeNode
{
public:
	explicit apoUiExenodeValueComp(QWidget *parent, ndxml *exeNodeXml);
	virtual ~apoUiExenodeValueComp();

	virtual void onInit();

	apoBaseSlotCtrl *getSubSlot() { return m_subSlot; }
private:
	apoBaseSlotCtrl *m_subSlot;
};

class apoUiExenodeSwitch : public apoBaseExeNode
{
	Q_OBJECT
public:
	explicit apoUiExenodeSwitch(QWidget *parent, ndxml *exeNodeXml);
	virtual ~apoUiExenodeSwitch();

	virtual void onInit();

	virtual bool closeParam(apoBaseSlotCtrl *param);

	int getSubBlockNum(){ return m_caseParam.size(); }
	apoBaseParam *getSubSlot(int index);

	apoBaseSlotCtrl *getDefault() { return  m_defaultBlock; }

public slots:
void onAddNewBlockClicked();
	//apoBaseSlotCtrl *getSubSlot() { return m_subSlot; }
private:
	apoBaseParam *createSubBlock(ndxml *subBlockXml);
	//apoBaseSlotCtrl *m_subSlot;
	param_vct_t m_caseParam;

	apoBaseSlotCtrl * m_defaultBlock;
};

//////////////////////////////////////////////////////////////////////////

class apoUiExenodeSelector : public apoBaseExeNode
{
	Q_OBJECT
public:
	explicit apoUiExenodeSelector(QWidget *parent, ndxml *exeNodeXml);
	virtual ~apoUiExenodeSelector();

	virtual void onInit();

	virtual bool closeParam(apoBaseSlotCtrl *param);

	int getSubBlockNum(){ return m_caseParam.size(); }
	apoBaseParam *getSubSlot(int index);

	apoBaseSlotCtrl *getDefault() { return  m_defaultBlock; }

	public slots:
	void onAddNewBlockClicked();
	//apoBaseSlotCtrl *getSubSlot() { return m_subSlot; }
private:
	apoBaseParam *createSubBlock(ndxml *subBlockXml);
	//apoBaseSlotCtrl *m_subSlot;
	param_vct_t m_caseParam;

	apoBaseSlotCtrl * m_defaultBlock;
};

#endif // _APOUI_EXENODE_H_
