/*
* file apoXmlTreeView.h
*
* apollo editor's xml-tree-view
*
* create by duan
*
* 2017.2.16
*/


#include "apoXmlTreeView.h"
#include <QHeaderView>

using namespace LogicEditorHelper;

apoXmlTreeView::apoXmlTreeView(QWidget *parent) : dragTree(parent), m_alias(0)
{

}


void apoXmlTreeView::setXmlInfo(ndxml *xmldata, int detph)
{
	m_xmldata = xmldata; 
	m_disp_depth = detph;

	loadHideNodeInfo(xmldata);
	createTree();
}


void apoXmlTreeView::setAlias(LogicEditorHelper::CXMLAlias *alias)
{
	m_alias = alias;
}
// 
// void apoXmlTreeView::setComfig(ndxml *config)
// {
// 	m_config = config;
// 
// 	ndxml *alias = ndxml_getnode(m_config, "alias");
// 	if (alias) {
// 		m_alias.Create(alias);
// 	}
// }

bool apoXmlTreeView::createTree()
{
	int depth = 0;
	this->clear();
	QHeaderView *pView = this->header();
	pView->setHidden(true);


	setColumnCount(1);

	xmlTreeItem *root = new xmlTreeItem(this, QStringList(QString("root")));

	root->setUserData(NULL);

	int num = ndxml_num(m_xmldata);
	for (int i = 0; i < num; i++) {
		ndxml *node = ndxml_getnodei(m_xmldata, i);
		if (!node || CheckHide(node))
			continue;
		InitTreeNode(depth, node, root);
	}
	root->setExpanded(true);

	for (int i = 0; i < root->childCount(); i++) {
		QTreeWidgetItem *pChild = root->child(i);
		ExpandTree(pChild);
	}

	return true;
}

void apoXmlTreeView::loadHideNodeInfo(ndxml *xmldata)
{
	m_hideChildren.clear();
	ndxml *xmlSetting = ndxml_getnode(xmldata, "blueprint_setting");
	if (!xmlSetting)	{
		return;
	}
	ndxml *xmlHide = ndxml_getnode(xmlSetting, "hide_children");
	if (!xmlHide){
		return;
	}
	int num = ndxml_getsub_num(xmlHide);
	for (int i = 0; i < num; i++)	{
		ndxml *node = ndxml_getnodei(xmlHide, i);
		if (node){
			hideXmlNodeChildren(ndxml_getval(node));
		}
	}

}

bool apoXmlTreeView::ExpandTree(QTreeWidgetItem* hItem)
{
	if (!hItem){
		return false;
	}
	xmlTreeItem *node = (xmlTreeItem*)hItem;
	ndxml* xml = (ndxml*)node->getUserData();
	if (!xml) {
		return false;
	}

	bool ret = false;
	const char *pExpVal = ndxml_getattr_val(xml, _GetReverdWord(ERT_EXPAND_STAT));
	if (pExpVal && pExpVal[0] == '1'){
		ret = true;
		hItem->setExpanded(true);
		for (int i = 0; i < hItem->childCount(); i++) {
			QTreeWidgetItem *pChild = hItem->child(i);
			ExpandTree(pChild);
		}
	}
	return ret;
}


QTreeWidgetItem* apoXmlTreeView::InitTreeNode(int depth, ndxml *xml_node, QTreeWidgetItem *hParent, QTreeWidgetItem *after)
{
	++depth;
	xmlTreeItem *leaf = NULL;
	if (!after) {
		leaf = new xmlTreeItem(hParent, QStringList(QString(_GetXmlName(xml_node, m_alias))));
	}
	else {
		leaf = new xmlTreeItem(hParent, after);
		leaf->setText(0, QString(_GetXmlName(xml_node, m_alias)));
	}

	leaf->setFlags(leaf->flags() | (Qt::ItemIsEditable | Qt::ItemIsDragEnabled));

	leaf->setUserData(xml_node);
	if (CheckExpand(xml_node)) {
		return leaf;
	}
	
	const char *pName = ndxml_getname(xml_node);

	//reach max depth
	if (depth >= m_disp_depth || isHideChildren(pName))	{
		return leaf;
	}

	text_vect_t exp_list;
	get_string_array((char*)ndxml_getattr_val(xml_node, _GetReverdWord(ERT_EXPAND_LIST)), exp_list);

	int num = ndxml_getsub_num(xml_node);
	for (int i = 0; i < num; i++) {
		ndxml *node = ndxml_refsubi(xml_node, i);
		if (!node)
			continue;
		const char *pName = ndxml_getname(node);
		if (CheckHide(node) || std::find(exp_list.begin(), exp_list.end(), std::string(pName)) != exp_list.end())
			continue;

		InitTreeNode(depth,node, leaf);
	}

	return leaf;
}


void apoXmlTreeView::hideXmlNodeChildren(const char *xmlName)
{
	m_hideChildren.push_back(xmlName);
}
bool apoXmlTreeView::isHideChildren(const char *xmlName)
{
	for (strings_vct_t::iterator it = m_hideChildren.begin(); it !=m_hideChildren.end(); it++)	{
		if (0==ndstricmp(xmlName, it->c_str())) {
			return true;
		}
	}
	return false;
}
