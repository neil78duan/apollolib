/*
* file apoUiDetailView.h
*
* define detail view
*
* create by duan
*
* 2017.2.23
*/


#include "apoScript/apoUiDetailView.h"
#include "logic_parser/logic_editor_helper.h"

#include <QStringList>
#include <QHeaderView>

using namespace LogicEditorHelper;

apoUiDetailView::apoUiDetailView(QWidget *parent ) :
apoUiXmlTableWidget(parent)
{

}

apoUiDetailView::apoUiDetailView(int rows, int columns, QWidget *parent ) :
apoUiXmlTableWidget(rows, columns, parent)
{
	m_curNode = 0;
}

apoUiDetailView::~apoUiDetailView()
{
	m_curNode = 0;
}


bool apoUiDetailView::showDetail(void *data, ndxml_root *xmlfile)
{
	preShow();
	m_rootFile = xmlfile;
	showExeNode((apoBaseExeNode *)data);

	postShow();
	return true;
}
void apoUiDetailView::resizeEvent(QResizeEvent *event)
{
	int length = width();
	setColumnWidth(0, length * 0.3);
	setColumnWidth(1, length * 0.3);
	setColumnWidth(2, length * 0.4);
}

int apoUiDetailView::_init()
{
	clear();


	setColumnCount(3);
	setRowCount(0);

	resizeEvent(NULL);

	QStringList headers;
	headers << "Name" << "Type" << "Value";

	setHorizontalHeaderLabels(headers);

	QHeaderView* headerView = verticalHeader();
	headerView->setHidden(true);

	return 0;
}


static xmlTableItem *createItem(const QString &text, ndxml *refXml = NULL, const char *attrName=NULL)
{
	xmlTableItem *pItem = new xmlTableItem(text );
	if (refXml) {
		pItem->setUserData(refXml);
		pItem->setAttrName(attrName);
	}
	else {
		UNEDIT_CTRL(pItem);
	}

	return pItem;
}

bool apoUiDetailView::showExeNode(apoBaseExeNode *node)
{
	_init();
	m_curNode = node;
	
	//show node name 
	ndxml *xml = (ndxml*)node->getMyUserData();
	if (!xml) {
		return false;
	}
	const char *textName = LogicEditorHelper::_GetXmlName(xml, NULL);
	_insertRow(createItem("Node"), createItem("text"), createItem(textName,xml, "name" ));
	
	//show comment
	if (ndxml_getnode(xml, "comment"))	{
		textName = LogicEditorHelper::_GetNodeComment(xml);
		_insertRow(createItem("Comment"), createItem("text"), createItem(textName, ndxml_getnode(xml, "comment")));
	}

	ndxml *xmlLabel = ndxml_getnode(xml, "internal_label");
	if (xmlLabel){
		_insertRow(createItem("Label"), createItem("text"), createItem(ndxml_getval(xmlLabel), xmlLabel));
	}

	xmlLabel = ndxml_getnode(xml, "internal_goto");
	if (xmlLabel){
		_insertRow(createItem("Goto"), createItem("text"), createItem(ndxml_getval(xmlLabel), xmlLabel));
	}



	int num = node->getParamNum();
	for (int i = 0; i < num; i++){
		apoBaseParam *paramNode= node->getParam(i);
		QString name = paramNode->getParamName();
		QString val = paramNode->getDisplayValue();
		QString typeName = paramNode->getTypeName();


		ndxml *typeXml = paramNode->getTypeXml();
		ndxml *nameXml = paramNode->getNameXml();
		ndxml *valXml = paramNode->getValueXml();


		_insertRow(createItem(name, nameXml),
			createItem(typeName, typeXml),
			createItem(val, valXml));
	}

	return true;
}

bool apoUiDetailView::_insertRow(xmlTableItem *nameItem, xmlTableItem* typeItem, xmlTableItem *valItem)
{
	int rows = rowCount();
	setRowCount(rows + 1);

	//UNEDIT_CTRL(nameItem);
	//UNEDIT_CTRL(typeItem);

	setItem(rows, 0, nameItem);
	setItem(rows, 1, typeItem);
	setItem(rows, 2, valItem);

	return true;
}

bool apoUiDetailView::onChanged(int row, int column, const char *xmlRealValue)
{
	if (column != 1){
		apoUiXmlTableWidget::onChanged(row, column, xmlRealValue);
		return true;
	}
	ndxml *xmlval = _getXml(row, column+1);
	if (!xmlval){
		return false;
	}
	
	ndxml *xmlType = _getXml(row, column );
	if (!xmlType){
		return false;
	}

	//get type 
	int type = -1;
	const char *restrictType = ndxml_getattr_val(xmlval, "restrict");
	ndxml *parent = ndxml_get_parent(xmlval);
	if (!parent)	{
		return true;
	}
	ndxml *node = ndxml_getnode(parent, restrictType);
	if (!node || node != xmlType )	{
		return true;
	}
	type = ndxml_getval_int(xmlType);
	if (type ==-1)	{
		return true;
	}
	
	xmlTableItem *cell = (xmlTableItem*)item(row, column + 1);
	if (!cell){
		return false;
	}

	if (type == OT_PARAM){
		cell->setText(QString("1"));
	}
	else if (type == OT_VARIABLE) {
		cell->setText(QString("var1"));
	}
	else if (type == OT_LAST_RET) {
		cell->setText(QString("0"));
	}
	else if (type == OT_INT || type == OT_FLOAT || type == OT_INT64 || type == OT_INT8 || type == OT_INT16) {
		cell->setText(QString("0"));
	}

	apoUiXmlTableWidget::onChanged(row, column, xmlRealValue);
	return true;
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////


apoUiCommonXmlView::apoUiCommonXmlView(QWidget *parent):
apoUiXmlTableWidget(parent)
{
}

apoUiCommonXmlView::apoUiCommonXmlView(int rows, int columns, QWidget *parent):
apoUiXmlTableWidget(rows, columns, parent)
{
}

apoUiCommonXmlView::~apoUiCommonXmlView()
{

}


bool apoUiCommonXmlView::showDetail(void *data, ndxml_root *xmlfile)
{

	preShow();

	_initShowCtrl();
	m_rootFile = xmlfile;
	m_editedNode = (ndxml *)data;

	int num = ndxml_getsub_num(m_editedNode);
	for (int i = 0; i < num; i++){
		ndxml *subNode = ndxml_getnodei(m_editedNode, i);
		if (!CheckHide(subNode)) {
			ShowXMLValue(subNode, CheckExpand(subNode));
		}
	}
	//

	postShow();

	return true;
}


int apoUiCommonXmlView::_initShowCtrl()
{
	clear();


	setColumnCount(2);
	setRowCount(0);

	resizeEvent(NULL);

	QStringList headers;
	headers << "Name" << "Value";
	setHorizontalHeaderLabels(headers);


	QHeaderView* headerView = verticalHeader();
	headerView->setHidden(true);

	return 0;
}

void apoUiCommonXmlView::resizeEvent(QResizeEvent *event)
{
	int length = width();
	setColumnWidth(0, length * 0.28);
	setColumnWidth(1, length * 0.68);
}

int apoUiCommonXmlView::ShowXMLValue(ndxml *xml_node, int expand)
{
	int i;
	const char *pval, *name;
	
	//size_t param ;
	pval = ndxml_getval(xml_node);
	if (pval) {
		name = _GetXmlName(xml_node, m_alias);
		ShowRow(name, pval, xml_node);

	}
	for (i = 0; i < ndxml_getattr_num(xml_node); i++) {

		name = (char*)ndxml_getattr_name(xml_node, i);
		if (!name || CheckReserved(name) >= 0)
			continue;
		pval = (char*)ndxml_getattr_vali(xml_node, i);
		if (pval) {
			const char *alias = m_alias->GetAlia(name);
			if (alias) {
				ShowRow(alias, pval, xml_node, name);
			}
			else {
				ShowRow(name, pval, xml_node, name);
			}
		}
	}

	if (expand) {
		for (i = 0; i < ndxml_getsub_num(xml_node); i++) {
			ndxml *sub_node = ndxml_refsubi(xml_node, i);
			if (sub_node && !CheckHide(sub_node)){
				ShowXMLValue(sub_node, expand);
			}
		}
	}
	else {
		text_vect_t exp_list;
		if (get_string_array((char*)ndxml_getattr_val(xml_node, _GetReverdWord(ERT_EXPAND_LIST)), exp_list)) {
			for (text_vect_t::iterator it = exp_list.begin(); it != exp_list.end(); it++) {
				ndxml *sub_node = ndxml_refsub(xml_node, (*it).c_str());
				if (sub_node && !CheckHide(sub_node)){
					ShowXMLValue(sub_node, 1);
				}
			}
		}
	}
	return 0;

}

void apoUiCommonXmlView::ShowRow(const char *name, const char *val, ndxml* param,const char *attrName)
{
	int rows = rowCount();
	setRowCount(rows + 1);

	char realval[4096];
	const char *p = getDisplayNameFromStr(val, realval, sizeof(realval));

	xmlTableItem *tableNode = new xmlTableItem(QString(p));
	tableNode->setUserData(param);


	if (attrName && *attrName){
		tableNode->setAttrName(attrName);
	}
	else if (param) {
		apoEditorSetting*pRootSetting = apoEditorSetting::getInstant();
		ndxml *configRoot = pRootSetting->getConfig();

		int type = GetXmlValType(param, configRoot);
		if (type == EDT_BOOL) {
			tableNode->setText(QString(val[0] == '0' ? "FALSE" : "TRUE"));
		}
		else if (type == EDT_ENUM) {
			apoEditorSetting::text_vct_t enum_texts;
			if (getEnumList(param, configRoot, enum_texts) > 0) {
				int sel = atoi(val);
				if (sel >= 0 && sel < (int)enum_texts.size())
					tableNode->setText(enum_texts[sel]);
			}

		}
		else if (type == EDT_KEY_VAL) {
			apoEditorSetting::text_vct_t key_texts, value_texts;
			if (getKeyList(param, configRoot, key_texts) > 0 && getKeyValueList(param, configRoot, value_texts) > 0) {
				for (int i = 0; i < (int)value_texts.size(); i++) {
					if (val == value_texts[i]) {
						if (i < key_texts.size())	{
							tableNode->setText(key_texts[i]);
							//m_grad->SetItemText(row, 1, (LPCTSTR)key_texts[i]);
						}
						break;
					}

				}
			}
		}
	}

#define UNEDIT_CTRL(itemCtrl)	 itemCtrl->setFlags(itemCtrl->flags() & (~Qt::ItemIsEditable))

	xmlTableItem *pXmlItem = new xmlTableItem(QString(name));
	UNEDIT_CTRL(pXmlItem);
	setItem(rows, 0,pXmlItem);
	setItem(rows, 1, tableNode);
	//nd_logdebug("show %s : %s ", name, p);
}
