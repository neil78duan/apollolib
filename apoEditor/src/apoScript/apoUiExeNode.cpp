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

	const char *runSeq = ndxml_getattr_val(exeNodeXml, "connect_in_seq");
	if (runSeq && (0 == ndstricmp(runSeq, "yes") || 0 == ndstricmp(runSeq, "true")))	{
	}
	else {
		disableToNext();
		disableConnectIn();
	}


	//disableReturnVar();
	apoEditorSetting* p_setting = apoEditorSetting::getInstant();
	if (!LogicEditorHelper::GetCreateTemplate(exeNodeXml, p_setting->getConfig()) ){
		disableNewParam();
	}

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
	const char *varName = ndxml_getattr_val(m_nodeXml, "var_name");
	if (varName){
		node = ndxml_getnode(m_nodeXml, varName);
		if (node){
			return ndxml_getval(node);
		}
	}
	return NULL;
}


//////////////////////////////////////////////////////////////////////////
//loop exenode
apoUiExenodeLoop::apoUiExenodeLoop(QWidget *parent, ndxml *exeNodeXml) :apoBaseExeNode()
{
	apoBaseExeNode::m_type = EAPO_EXE_NODE_Loop;
	disableReturnVar();
	disableNewParam();
	setNodeInfo(parent, exeNodeXml);
	//InitCtrl(parent, "Loop",  1);
	setTips(QString("Loop {}"));
	

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
	ctrl1->resize(PARAM_CTRL_W * 1.5, PARAM_CTRL_H);
	ctrl1->setStyleSheet("QLabel{background-color:yellow;}");
	ctrl1->move(x, y);
	ctrl1->show();
	ctrl1->setAttribute(Qt::WA_DeleteOnClose, true);
	ctrl1->setSlotType(apoBaseSlotCtrl::SLOT_SUB_ENTRY);
	m_loopSlot = ctrl1;

	ndxml *anchorXml = ndxml_getnode(m_nodeXml, "steps_collection");
	if (anchorXml){
		m_loopSlot->setXmlAnchor(anchorXml);
		m_loopSlot->setXmlAnchorParent(anchorXml);
	}
	else {
		m_loopSlot->setXmlAnchorParent(m_nodeXml);
		m_loopSlot->setXmlAnchor(m_nodeXml);
	}

	y += E_LINE_HEIGHT;
	x = E_LINE_WIDTH - PARAM_CTRL_W;
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
	apoEditorSetting* settingRoot = apoEditorSetting::getInstant();

	//create if ctrl
	int x = E_LINE_WIDTH - PARAM_CTRL_W ;
	int y = E_LINE_HEIGHT * 1.5;

	int num = ndxml_getsub_num(m_nodeXml);

	for (int i = 0; i < num; i++){
		ndxml *xml = ndxml_getnodei(m_nodeXml, i);
		if (LogicEditorHelper::CheckHide(xml))	{
			continue;
		}
		const compile_setting* compileSetting = settingRoot->getStepConfig(ndxml_getname(xml));
		if (!compileSetting){
			continue;
		}
		else if (E_INSTRUCT_TYPE_CASE_ENTRY != compileSetting->ins_type)	{
			continue;
		}
		const char *condName = ndxml_getattr_val(xml, "comp_cond");
		if (!condName) {
			continue;
		}
		ndxml *condXml = ndxml_getnode(xml, condName);
		if (!condXml){
			continue;
		}
		apoBaseSlotCtrl *ctrl1 = NULL;
		const char *pval = ndxml_getval(condXml);
		if (pval && *pval !='0'){
			//true 
			ctrl1 = new apoBaseSlotCtrl(QString("True ->"), this);
			ctrl1->setStyleSheet("QLabel{background-color:yellow;}");
			m_trueSlot = ctrl1;
		}

		else {
			//false 
			ctrl1 = new apoBaseSlotCtrl(QString("False->"), this);
			ctrl1->setStyleSheet("QLabel{background-color:gray;}");
			m_falseSlot = ctrl1;
		}

		ctrl1->setSlotType(apoBaseSlotCtrl::SLOT_SUB_ENTRY);
		ctrl1->setXmlAnchor(condXml);
		ctrl1->setXmlAnchorParent(xml);

		ctrl1->resize(PARAM_CTRL_W * 1.5, PARAM_CTRL_H);
		ctrl1->move(x, y);
		ctrl1->show();
		ctrl1->setAttribute(Qt::WA_DeleteOnClose, true);

		y += E_LINE_HEIGHT;
	}

	//y += E_LINE_HEIGHT;
	x = E_LINE_WIDTH - PARAM_CTRL_W;
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


	ndxml *xmlBoolEntry = ndxml_getnode(m_nodeXml, "op_bool_entry");
	if (xmlBoolEntry){
		ndxml *caseEntry = ndxml_getnode(xmlBoolEntry, "op_sub_comp_entry");
		if (caseEntry)	{
			ndxml *xmlCond = ndxml_getnode(caseEntry, "condition");
			
			if (xmlCond){
				const char *pval = ndxml_getval(xmlCond);
				if (pval && *pval != '0'){
					//true 
					apoBaseSlotCtrl *ctrl1 = new apoBaseSlotCtrl(QString("{}->"), this);
					ctrl1->setStyleSheet("QLabel{background-color:yellow;}");
					ctrl1->resize(PARAM_CTRL_W * 1.5, PARAM_CTRL_H);
					ctrl1->move(x, y);
					ctrl1->show();
					ctrl1->setAttribute(Qt::WA_DeleteOnClose, true);
					ctrl1->setSlotType(apoBaseSlotCtrl::SLOT_SUB_ENTRY);

					ctrl1->setXmlAnchor(xmlCond);
					ctrl1->setXmlAnchorParent(caseEntry);
					m_subSlot = ctrl1;
				}
			}
		}
	}


	nd_assert(m_subSlot);

	y += E_LINE_HEIGHT;
	x = E_LINE_WIDTH - PARAM_CTRL_W;
	m_toNextNode->move(x, y);
}

//////////////////////////////////////////////////////////////////////////

apoUiExenodeSwitch::apoUiExenodeSwitch(QWidget *parent, ndxml *exeNodeXml) 
{
	apoBaseExeNode::m_type = EAPO_EXE_NODE_Switch;
	m_defaultBlock = NULL;
	disableNewParam();
	disableReturnVar();

	setNodeInfo(parent, exeNodeXml, true);
	setTips(QString("Switch"));
}

apoUiExenodeSwitch::~apoUiExenodeSwitch()
{

}


apoBaseParam *apoUiExenodeSwitch::getSubSlot(int index)
{
	if (index >= 0 && index<m_caseParam.size())	{
		return m_caseParam[index];
	}
	return NULL;
}

void apoUiExenodeSwitch::onAddNewBlockClicked()
{
	apoEditorSetting*pSetting = apoEditorSetting::getInstant();
	ndxml *newxml = pSetting->AddNewXmlNode(m_nodeXml, (QWidget*) this->parent());
	if (!newxml){
		nd_logerror("create new param error\n");
		return ;
	}

	apoBaseParam *pNewSub = createSubBlock(newxml);
	if (!pNewSub){
		return;
	}
	QPoint pos = m_outParamAddNew->pos();
	pNewSub->move(pos);

	pos.setY(pos.y() + E_LINE_HEIGHT);

	m_outParamAddNew->move(pos);

	m_caseParam.push_back(pNewSub);

	m_size.setHeight(m_size.height() + E_LINE_HEIGHT);
	//emit NodeAddParamSignal(this); 

	trytoMoveGotoNodeTail();
	return;

}

bool apoUiExenodeSwitch::closeParam(apoBaseSlotCtrl *slot)
{
	apoBaseParam *paramSlot = dynamic_cast<apoBaseParam*>(slot);

	if (paramSlot == m_defaultBlock || paramSlot == NULL)	{
		return false;
	}
	ndxml *delNode = paramSlot->getParentXml();
	if (!delNode){
		return false;
	}
	
	param_vct_t::iterator delit = m_caseParam.end();
	for (param_vct_t::iterator it = m_caseParam.begin(); it != m_caseParam.end(); ++it) {
		if ((*it) == paramSlot)	{
			delit = it;
		}
		else if (delit != m_caseParam.end())	{
			QPoint pos = (*it)->pos();
			pos.setY(pos.y() - E_LINE_HEIGHT);
			(*it)->move(pos);
		}
	}

	
	if (delit != m_caseParam.end()) {
		m_caseParam.erase(delit);
		ndxml_delxml(delNode, NULL);
		slot->close();

		m_size.setHeight(m_size.height() - E_LINE_HEIGHT);

		QPoint pos = m_outParamAddNew->pos();
		pos.setY(pos.y() - E_LINE_HEIGHT);
		m_outParamAddNew->move(pos);

		for (param_vct_t::iterator it = m_paramVct.begin(); it != m_paramVct.end(); ++it) {
			if ((*it) == paramSlot)	{
				m_paramVct.erase(it);
				break;
			}
		}
	}

return true;
}

void apoUiExenodeSwitch::onInit()
{
	apoEditorSetting* settingRoot = apoEditorSetting::getInstant();
	//create case ctrl
	int x = E_LINE_WIDTH - PARAM_CTRL_W*1.5;
	int y = E_LINE_HEIGHT * 4;

	ndxml *defaultBlock = NULL;
	int num = ndxml_getsub_num(m_nodeXml);
	for (int i = 0; i < num; i++){
		ndxml *xml = ndxml_getnodei(m_nodeXml, i);
		const compile_setting* compileSetting = settingRoot->getStepConfig(ndxml_getname(xml));
		if (!compileSetting){
			continue;
		}
		else if (E_INSTRUCT_TYPE_DELAY_COMPILE == compileSetting->ins_type)	{
			defaultBlock = xml;
			continue;
		}
		else if (E_INSTRUCT_TYPE_COLLOCTION == compileSetting->ins_type){
			apoBaseParam *caseCtrl = createSubBlock(xml);
			if (!caseCtrl)	{
				continue;
			}			
			caseCtrl->move(x, y);
			m_caseParam.push_back(caseCtrl);
			y += E_LINE_HEIGHT;
		}
	}

	CREATE_CTRL_OBJECT(QPushButton, "+", green, m_outParamAddNew);
	connect(m_outParamAddNew, SIGNAL(clicked()), this, SLOT(onAddNewBlockClicked()));

	//resize exenode size 
	m_size.setHeight(y + E_LINE_HEIGHT);

	if (defaultBlock)	{
		y = E_LINE_HEIGHT * 3;

		apoBaseSlotCtrl *ctrl1 = new apoBaseSlotCtrl(QString("default"), this);
		ctrl1->resize(PARAM_CTRL_W * 2, PARAM_CTRL_H);
		ctrl1->setStyleSheet("QLabel{background-color:yellow;}");
		ctrl1->move(x, y);
		ctrl1->show();
		ctrl1->setAttribute(Qt::WA_DeleteOnClose, true);
		ctrl1->setXmlAnchor(defaultBlock);
		ctrl1->setXmlAnchorParent(defaultBlock);

		m_defaultBlock = ctrl1;

	}
	
}

apoBaseParam *apoUiExenodeSwitch::createSubBlock(ndxml *subBlockXml)
{
	ndxml *cmpNode = ndxml_getnode(subBlockXml, "op_comp");
	if (!cmpNode) {
		return NULL;
	}
	ndxml *paramColl = ndxml_getnode(cmpNode, "param_collect");
	if (!paramColl)	{
		return NULL;
	}
	apoBaseParam *ctrl1 = createParam(paramColl, cmpNode);
	if (!ctrl1)	{
		return NULL;
	}

	ndxml *typexml = ctrl1->getTypeXml();
	ndxml *varxml = ctrl1->getValueXml();
	ctrl1->setParam(subBlockXml, varxml, typexml, subBlockXml);

	const char *tips = ndxml_getattr_val(subBlockXml, "name");
	if (tips){
		ctrl1->setText(QString(tips));
	}

	ctrl1->resize(PARAM_CTRL_W * 2, PARAM_CTRL_H);
	ctrl1->show();
	ctrl1->setSlotType(apoBaseSlotCtrl::SLOT_SUB_ENTRY);
	
	//get anchor 
	ndxml *sublEntry = ndxml_getnode(subBlockXml, "op_bool_entry");
	if (!sublEntry)	{
		ctrl1->close();
		return NULL;
	}
	ndxml *exeBlock = ndxml_getnode(sublEntry, "op_sub_comp_entry");
	if (!exeBlock)	{
		ctrl1->close();
		return NULL;
	}
	ndxml *anchor = ndxml_getnode(exeBlock, "condition");
	if (anchor)	{
		ctrl1->setXmlAnchor(anchor);
	}
	else {
		ctrl1->setXmlAnchor(exeBlock);
	}
	ctrl1->setXmlAnchorParent(exeBlock);

	m_paramVct.push_back(ctrl1);
	return ctrl1;
}
