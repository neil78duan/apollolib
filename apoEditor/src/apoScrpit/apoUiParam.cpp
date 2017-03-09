/* file apoUiParam.cpp
*
* define param node
*
* create by duan
*
* 2017.2.20
*/

#include "apoScrpit/apoUiParam.h"
#include "logic_parser/logic_editor_helper.h"
#include "apoScrpit/apoEditorSetting.h"

#include "apoScrpit/apoUiExeNode.h"

using namespace LogicEditorHelper;

bool apoBaseSlotCtrl::checkConnectIn()
{
	if (m_slotType == SLOT_RUN_IN || m_slotType == SLOT_NODE_INPUUT_PARAM || m_slotType == SLOT_VAR)	{
		return true;
	}
	return false;

}
bool apoBaseSlotCtrl::onConnectIn(apoBaseSlotCtrl*fromSlot)
{
	return true;
}


bool apoBaseSlotCtrl::onDisconnect()
{
	m_myConnect = 0;
	return true;
}


void apoBaseSlotCtrl::setValid(bool enable)
{ 
	m_valid = enable; 
	if (m_valid){
		setStyleSheet("QLabel{background-color:yellow;}");
	}
	else {
		setStyleSheet("QLabel{background-color:gray;}");

	}
}

bool apoBaseSlotCtrl::event(QEvent *e) 
{
	if (e->type() == QEvent::Enter) {
		m_inDrag = true;
		QWidget *p = (QWidget *) this->parent();
		p->update();
	}
	else if (e->type() == QEvent::Leave) {
		m_inDrag = false;
		QWidget *p = (QWidget *) this->parent();
		p->update();
	}
	return QWidget::event(e);
}

//////////////////////////////////////////////////////////////////////////

apoBaseParam::apoBaseParam(QWidget *parent, Qt::WindowFlags f) :
apoBaseSlotCtrl(parent, f), m_index(0)
{
	m_slotType = apoBaseSlotCtrl::SLOT_NODE_INPUUT_PARAM;
	//m_inDrag = false;
	setParam(NULL, NULL,NULL);

}
apoBaseParam::apoBaseParam(const QString &text, QWidget *parent, Qt::WindowFlags f) :
apoBaseSlotCtrl(text, parent, f), m_index(0)
{
	m_slotType = apoBaseSlotCtrl::SLOT_NODE_INPUUT_PARAM;
	//m_inDrag = false;
	setParam(NULL, NULL,NULL);

}
apoBaseParam::~apoBaseParam()
{

}


QString apoBaseParam::getParamInfo()
{
	QString str1 = getParamName() + ": ";
	str1 += getTypeName();
	str1 += ": ";
	return str1 + getDisplayValue();
}


void apoBaseParam::setParam(ndxml *parent, ndxml *param, ndxml *paramRef, ndxml *varname)
{
	m_parent = parent;
	m_xml = param;
	m_reference = paramRef;
	m_varName = varname;

}


const char *apoBaseParam::getValue()
{
	return ndxml_getval(m_xml);
}
int apoBaseParam::getParamType()
{
	apoEditorSetting* rootSetting = apoEditorSetting::getInstant();
	//ndxml_root *config = rootSetting->getConfig();


	//get type 
	int type = 2;

	if (m_reference) {
		type = ndxml_getval_int(m_reference);
	}
	else {
		const compile_setting*pStepSetting = rootSetting->getStepConfig(ndxml_getname(m_xml));
		if (pStepSetting){
			type = pStepSetting->data_type;
		}
	}

	return type;
}

QString apoBaseParam::getTypeName()
{
	apoEditorSetting* rootSetting = apoEditorSetting::getInstant();
	ndxml_root *config = rootSetting->getConfig();

	
	//const char *val = ndxml_getval(m_xml);
	int type = GetXmlValType(m_xml, config);

	if (type == EDT_BOOL) {
		return QString("Bool");
	}
	else if (type == EDT_ENUM || type == EDT_KEY_VAL) {
		return QString("Enum");
	}

	//get name list from editor_setting.xml data_type_define.type_data_type
	type = getParamType();

	ndxml *typeNameList = getDefinedType(config, "type_data_type");
	nd_assert(typeNameList);
	apoEditorSetting::text_vct_t typeNames;

	if (getEnumList(typeNameList, config, typeNames) > 0) {
		return typeNames[type];
	}
	return QString("string");
}

QString apoBaseParam::getParamName()
{
	if (m_varName){
		return QString(ndxml_getval(m_varName));
	}
	else {
		const char *pRootName = ndxml_getname(m_parent);
		if (0 == ndstricmp(pRootName, "param_collect") || 0 == ndstricmp(pRootName, "condition"))	{
			return QString(_GetXmlName(m_parent, NULL));

		}
		else {
			return QString(_GetXmlName(m_xml, NULL));

		}
		
	}
}

QString apoBaseParam::getDisplayValue()
{
	apoEditorSetting* rootSetting = apoEditorSetting::getInstant();
	ndxml_root *pConfig = rootSetting->getConfig();

	const char *val = ndxml_getval(m_xml);
	int type = GetXmlValType(m_xml, pConfig);

	if (type == EDT_BOOL) {
		return QString(val[0] == '0' ? "FALSE" : "TRUE");
	}
	else if (type == EDT_ENUM) {
		apoEditorSetting::text_vct_t enum_texts;

		if (getEnumList(m_xml, pConfig, enum_texts) > 0) {
			int sel = atoi(val);
			if (sel >= 0 && sel < (int)enum_texts.size()) {
				return QString(enum_texts[sel]);
			}
		}

	}
	else if (type == EDT_KEY_VAL) {
		apoEditorSetting::text_vct_t key_texts, value_texts;
		if (getKeyList(m_xml, pConfig, key_texts) > 0 && getKeyValueList(m_xml, pConfig, value_texts) > 0) {
			for (int i = 0; i < (int)value_texts.size(); i++) {
				if (val == value_texts[i]) {
					if (i < key_texts.size())	{
						return QString(key_texts[i]);
					}
					break;
				}

			}
		}
	}

	char realval[4096];
	const char *p = getDisplayNameFromStr(val, realval, sizeof(realval));
	return QString(p);
}


bool apoBaseParam::checkConnectIn()
{
	if (m_reference) {
		return true;
	}
	return false;

}
bool apoBaseParam::onConnectIn(apoBaseSlotCtrl*fromSlot)
{
	if (!checkConnectIn()){
		return false;
	}
	apoBaseExeNode *parentFrom = dynamic_cast<apoBaseExeNode *>(fromSlot->parent());
	if (!parentFrom){
		return false;
	}
	eBaseSlotType type = fromSlot->slotType();
	if (type == SLOT_FUNCTION_PARAM){
		int paramIndex = parentFrom->getFunctionParamIndex(fromSlot);
		if (-1==paramIndex)	{
			return false;
		}

		ndxml_setval_int(m_reference, (int)OT_PARAM);
		ndxml_setval_int(m_xml, paramIndex);

	}
	else if (type == SLOT_VAR){
		apoUiExenodeNewVar *varNode = dynamic_cast<apoUiExenodeNewVar *>(parentFrom);
		if (!varNode){
			return false;
		}
		const char *varName = varNode->getVarName();
		if (!varName){
			return false;
		}

		ndxml_setval_int(m_reference, (int)OT_VARIABLE);
		ndxml_setval(m_xml, varName);
	}
	else if (type == SLOT_RETURN_VALUE)	{

		ndxml_setval_int(m_reference, (int)OT_LAST_RET);
		ndxml_setval(m_xml, "0");
	}
	return true;
}


bool apoBaseParam::onDisconnect()
{
	if (m_reference){
		ndxml_setval_int(m_reference, (int)OT_INT);
	}
	ndxml_setval(m_xml, "0");
	m_myConnect = 0;
	return true;
}
