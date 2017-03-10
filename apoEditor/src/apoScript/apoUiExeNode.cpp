/* file apoUiExeNode.cpp
 *
 * logic parser editor bezier
 *
 * create by duan
 *
 * 2017.2.7
 */

#include "apoUiExeNode.h"
#include "apoScript/apoEditorSetting.h"
#include <QPainter>
#include <QMessageBox>
#include <QMouseEvent>


static int apo_exeNodeGetIndex(const char *name)
{


#ifdef APOUI_NODE_ID
#undef APOUI_NODE_ID
#endif
#define APOUI_NODE_ID(id) #id,

	static const char* exeClassname[] = {
#include "apoScript/apouinodedef.h"
	};
#undef APOUI_NODE_ID
	int number = sizeof(exeClassname) / sizeof(exeClassname[0]);
	for (int i = 0; i < number; i++) {
		if (strcmp(name, exeClassname[i]) == 0) {
			return i;
		}
	}
	return 0;
	
}


apoBaseExeNode *g_apoCreateExeNode(ndxml *xml, QWidget *parent)
{

	apoEditorSetting* p_setting = apoEditorSetting::getInstant();
	const char *ctrlName = p_setting->getExeNodeName(ndxml_getname(xml));
	if (!ctrlName || !*ctrlName) {
		ctrlName = "Common";
	}


	int index = apo_exeNodeGetIndex(ctrlName);
	return g_apoCreateExeNode(index,xml, parent);
}

apoBaseExeNode *g_apoCreateExeNode(int index, ndxml *xml, QWidget *parent)
{

#ifdef APOUI_NODE_ID
#undef APOUI_NODE_ID
#endif
#define APOUI_NODE_ID(id) \
	case EAPO_EXE_NODE_##id :	\
		return new apoUiExenode##id(parent,xml);	\
		break;					\


	switch (index)
	{
#include "apoScript/apouinodedef.h"
	default:
		break;
	}

#undef APOUI_NODE_ID

	return NULL;
}


apoUiExenodeNewVar::apoUiExenodeNewVar(QWidget *parent, ndxml *exeNodeXml) :apoBaseExeNode()
{
	apoBaseExeNode::m_type = EAPO_EXE_NODE_NewVar;
	//disableReturnVar();
	disableNewParam();

	setNodeInfo(parent, exeNodeXml);
	m_returnValue->setSlotType(apoBaseSlotCtrl::SLOT_VAR);
	//InitCtrl(parent, "Common",  0);
	//setTips(QString("common"));
}
apoUiExenodeNewVar::~apoUiExenodeNewVar()
{
}

const char *apoUiExenodeNewVar::getVarName()
{
	if (!m_nodeXml) {
		return NULL;
	}
	ndxml *node = ndxml_getnode(m_nodeXml,"param");
	if (node){
		return ndxml_getval(node);
	}
	return NULL;
}


//////////////////////////////////////////////////////////////////////////
//loop exenode
apoUiExenodeLoop::apoUiExenodeLoop(QWidget *parent, ndxml *exeNodeXml) :apoBaseExeNode()
{
	apoBaseExeNode::m_type = EAPO_EXE_NODE_Loop;

	disableNewParam();
	setNodeInfo(parent, exeNodeXml);
	//InitCtrl(parent, "Loop",  1);
	setTips(QString("loop $count times "));


}
apoUiExenodeLoop::~apoUiExenodeLoop()
{
}

void apoUiExenodeLoop::onInit()
{
	//create loop ctrl
	//create loop ctrl
	int x = E_LINE_WIDTH - PARAM_CTRL_W ;
	int y = E_LINE_HEIGHT * 1.5;

	apoBaseSlotCtrl *ctrl1 = new apoBaseSlotCtrl(QString("{}->"), this);
	ctrl1->resize(PARAM_CTRL_W * 2, PARAM_CTRL_H);
	ctrl1->setStyleSheet("QLabel{background-color:yellow;}");
	ctrl1->move(x, y);
	ctrl1->show();
	ctrl1->setAttribute(Qt::WA_DeleteOnClose, true);
	m_loopSlot = ctrl1;


	y += E_LINE_HEIGHT;
	x = E_LINE_WIDTH;
	m_toNextNode->move(x, y);

}

//////////////////////////////////////////////////////////////////////////
// bool exenode

apoUiExenodeBool::apoUiExenodeBool(QWidget *parent, ndxml *exeNodeXml) :apoBaseExeNode()
{
	apoBaseExeNode::m_type = EAPO_EXE_NODE_Bool;

	disableNewParam();
	disableReturnVar();

	//parse param 
	const char *paramName = ndxml_getattr_val(exeNodeXml, "comp_cond");
	if (paramName && *paramName){
		ndxml *param1 = ndxml_getnode(exeNodeXml, paramName);
		if (param1)	{
			if (!_parseParam(param1)) {
				nd_logerror("parse param error\n");
			}
		}
	}
	
	setNodeInfo(parent, exeNodeXml, false);

	//InitCtrl(parent, "Bool",  0);
	setTips(QString("if-else "));


}
apoUiExenodeBool::~apoUiExenodeBool(){}

void apoUiExenodeBool::onInit()
{
	//create if ctrl
	int x = E_LINE_WIDTH - PARAM_CTRL_W*2;
	int y = E_LINE_HEIGHT * 1.5;

	apoBaseSlotCtrl *ctrl1 = new apoBaseSlotCtrl(QString("True ->"), this);
	ctrl1->resize(PARAM_CTRL_W*3, PARAM_CTRL_H);
	ctrl1->setStyleSheet("QLabel{background-color:yellow;}");
	ctrl1->move(x,y);
	ctrl1->show();
	ctrl1->setAttribute(Qt::WA_DeleteOnClose, true);
	m_trueSlot = ctrl1;

	y += E_LINE_HEIGHT;
	ctrl1 = new apoBaseSlotCtrl(QString("False->"), this);
	ctrl1->resize(PARAM_CTRL_W*3, PARAM_CTRL_H);
	ctrl1->setStyleSheet("QLabel{background-color:gray;}");
	ctrl1->move(x, y);
	ctrl1->show();
	ctrl1->setAttribute(Qt::WA_DeleteOnClose, true);
	m_falseSlot = ctrl1;


	y += E_LINE_HEIGHT;
	x = E_LINE_WIDTH ;
	m_toNextNode->move(x, y);
}

//////////////////////////////////////////////////////////////////////////
//
apoUiExenodeValueComp::apoUiExenodeValueComp(QWidget *parent, ndxml *exeNodeXml) : m_subSlot(0)
{
	apoBaseExeNode::m_type = EAPO_EXE_NODE_ValueComp;

	disableNewParam();
	disableReturnVar();

	//parse param 
	ndxml *cmpNode = ndxml_getnode(exeNodeXml, "op_comp");
	if (cmpNode){
		if (!_parseParam(cmpNode)) {
			nd_logerror("parse param error\n");
		}
	}

	setNodeInfo(parent, exeNodeXml, false);

	setTips(QString("if(a>b)"));

}

apoUiExenodeValueComp::~apoUiExenodeValueComp()
{

}

void apoUiExenodeValueComp::onInit()
{
	//create case ctrl
	int x = E_LINE_WIDTH - PARAM_CTRL_W;
	int y = E_LINE_HEIGHT * 1.5;

	apoBaseSlotCtrl *ctrl1 = new apoBaseSlotCtrl(QString("{}->"), this);
	ctrl1->resize(PARAM_CTRL_W * 2, PARAM_CTRL_H);
	ctrl1->setStyleSheet("QLabel{background-color:yellow;}");
	ctrl1->move(x, y);
	ctrl1->show();
	ctrl1->setAttribute(Qt::WA_DeleteOnClose, true);
	m_subSlot = ctrl1;


	y += E_LINE_HEIGHT;
	x = E_LINE_WIDTH;
	m_toNextNode->move(x, y);
}

