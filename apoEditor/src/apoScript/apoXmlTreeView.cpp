/*
* file apoXmlTreeView.h
*
* apollo editor's xml-tree-view
*
* create by duan
*
* 2017.2.16
*/

#include "apoScript/apoEditorSetting.h"
#include "apoXmlTreeView.h"
#include <QHeaderView>
#include <QContextMenuEvent>
#include <QMenu>
#include <QMessageBox>

using namespace LogicEditorHelper;

apoXmlTreeView::apoXmlTreeView(QWidget *parent) : dragTree(parent), m_alias(0), m_root(0)
{
	QObject::connect(this, SIGNAL(itemChanged(QTreeWidgetItem *, int )),
		this, SLOT(onItemChanged(QTreeWidgetItem *, int )));


	QObject::connect(this, SIGNAL(onMydragItem(QTreeWidgetItem *, QTreeWidgetItem *)), 
		this, SLOT(DragCallBack(QTreeWidgetItem* , QTreeWidgetItem* )));
}


void apoXmlTreeView::setXmlInfo(ndxml *xmldata, int detph,const char *rootName )
{
	m_xmldata = xmldata; 
	m_disp_depth = detph;

	if (rootName && *rootName){
		m_rootName = rootName;
	}
	else {
		m_rootName = "Root";
	}

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

	xmlTreeItem *root = new xmlTreeItem(this, QStringList(m_rootName));

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

	m_root = root;
	return true;
}


void apoXmlTreeView::setRootName(const QString &name)
{
	m_rootName = name;
	if (m_root)	{
		m_root->setText(0, m_rootName);
	}
}

void apoXmlTreeView::onItemChanged(QTreeWidgetItem *item, int column)
{
	xmlTreeItem* curItem = dynamic_cast<xmlTreeItem*>(item);
	if (!curItem)
		return;
	ndxml *xml = (ndxml *)curItem->getUserData();
	if (xml) {
		QString text1 = item->text(column);
		ndxml_setattrval(xml, "name", text1.toStdString().c_str());
		nd_logmsg("itemchange %s : %s", _GetXmlName(xml,NULL), text1.toStdString().c_str());

		emit xmlDataChangedSignal();
	}
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



void apoXmlTreeView::unCreateNewChild(const char *xmlName)
{
	m_unCreateNew.push_back(xmlName);
}
bool apoXmlTreeView::isNotCreateNewChild(const char *xmlName)
{
	for (strings_vct_t::iterator it = m_unCreateNew.begin(); it != m_unCreateNew.end(); it++)	{
		if (0 == ndstricmp(xmlName, it->c_str())) {
			return true;
		}
	}
	return false;
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


void apoXmlTreeView::contextMenuEvent(QContextMenuEvent *event)
{
	xmlTreeItem* curItem = (xmlTreeItem*)itemAt(event->pos());
	if (!curItem)
		return;

	setCurrentItem(curItem);
	//m_curItem = curItem ;

	ndxml *xml = (ndxml *)curItem->getUserData();
	if (!xml ) {
		return;
	}


	apoEditorSetting *setting = apoEditorSetting::getInstant();
	QMenu *pop_menu = new QMenu(this);

	ndxml *create_template = GetCreateTemplate(xml, setting->getConfig());
	bool addOk = false;

	if (create_template && !isNotCreateNewChild(ndxml_getname(xml)) ) {
		QAction *add_node = new QAction(this);
		add_node->setText(tr("add"));
		pop_menu->addAction(add_node);

		connect(add_node, &QAction::triggered, this, &apoXmlTreeView::OnPopInsertNode);
		addOk = true;
	}
	if (CheckCanDelete(xml)) {

		QAction *del_node = new QAction(this);
		del_node->setText(tr("del"));
		pop_menu->addAction(del_node);
		connect(del_node, &QAction::triggered, this, &apoXmlTreeView::OnPopXmlDel);
		addOk = true;
	}

	if (addOk) {
		pop_menu->exec(QCursor::pos());
	}
	else {
		delete pop_menu;
	}

	//nd_logdebug("test");
}

void apoXmlTreeView::OnPopInsertNode()
{
	xmlTreeItem *curItem = (xmlTreeItem *)currentItem();
	if (!curItem)
		return;
	ndxml *xml = (ndxml *)curItem->getUserData();
	if (!xml) {
		return;
	}

	const char *pName = ndxml_getname(xml);
	if (isNotCreateNewChild(pName))	{
		return;
	}


	apoEditorSetting *setting = apoEditorSetting::getInstant();

	ndxml *newxml = setting->AddNewXmlNode(xml, this);
	if (!newxml) {
		nd_logerror("can not create new node\n");
		return;
	}

	QTreeWidgetItem* hNodeNew = InitTreeNode(1,newxml, curItem);
	if (hNodeNew) {
		setCurrentItem(hNodeNew);
	}
	emit xmlDataChangedSignal();
}

void apoXmlTreeView::OnPopXmlDel()
{
	xmlTreeItem *curItem = (xmlTreeItem*)currentItem();
	if (!curItem)
		return;

	//apoEditorSetting *setting = apoEditorSetting::getInstant();

	ndxml *xml = (ndxml *)curItem->getUserData();
	if (xml) {
		if (!CheckCanDelete(xml)) {
			nd_logerror("this item not be deleted");
			return;
		}
	}
	else {
		return;
	}

	xmlTreeItem *hParent = (xmlTreeItem*)curItem->parent();
	if (!hParent) {
		return;
	}
	int ret = QMessageBox::question(this, tr("Question"), tr("Do you delete?"),
		QMessageBox::Yes | QMessageBox::No,
		QMessageBox::Yes);
	if (QMessageBox::No == ret) {
		return;
	}

	hParent->removeChild(curItem);
	emit  xmlNodeDelSignal(xml);

}

//////////////////////////////////////////////////////////////////////////
//drag

static __INLINE__ ndxml* GetSelXml(QTreeWidgetItem* hItem)
{
	xmlTreeItem *node = (xmlTreeItem*)hItem;
	return (ndxml*)node->getUserData();
	//return (ndxml*) (m_pTree->GetItemData(hItem) );
}

void apoXmlTreeView::SetExpand(QTreeWidgetItem* hItem)
{
	if (!hItem){
		return;
	}
	ndxml* xml = GetSelXml(hItem);
	if (!xml){
		return;
	}
	ndxml_setattrval(xml, _GetReverdWord(ERT_EXPAND_STAT), "1");
	hItem->setExpanded(true);

}

void apoXmlTreeView::DragCallBack(QTreeWidgetItem* from, QTreeWidgetItem* to)
{
	TreeDragCallback((xmlTreeItem*)from, (xmlTreeItem*)to);
}

bool apoXmlTreeView::TreeDragCallback(xmlTreeItem*  hFrom, xmlTreeItem* hTo)
{
	xmlTreeItem *parent1 = (xmlTreeItem *)hFrom->parent();
	xmlTreeItem *parent2 = (xmlTreeItem *)hTo->parent();
	if (parent2 != parent1){
		return _TreeDragInNotSameRoot(hFrom, hTo);
	}
	ndxml *xmlparent = GetSelXml(parent1);
	ndxml*xmlFrom = GetSelXml(hFrom);
	ndxml*xmlTo = GetSelXml(hTo);
	if (!xmlparent || !xmlFrom || !xmlTo)	{
		return false;
	}

	nd_logmsg("drag  item %s to %s", _GetXmlName(xmlFrom,NULL), _GetXmlName(xmlTo, NULL));

	const char *pEnableDrag = ndxml_getattr_val(xmlparent, "enable_drag");
	if (pEnableDrag && 0 == ndstricmp(pEnableDrag, "no")){
		return false;
	}

	int index = ndxml_get_index(xmlparent, xmlTo);
	int src_index = ndxml_get_index(xmlparent, xmlFrom);
	if (-1 == index || -1 == src_index){
		return false;
	}
	if (-1 == ndxml_remove(xmlFrom, xmlparent)){
		return false;
	}
	if (-1 == ndxml_insert_ex(xmlparent, xmlFrom, index - 1)) {
		return false;
	}

	//recreate tree node 

	xmlTreeItem *gp = (xmlTreeItem*)parent1->parent();
	xmlTreeItem* hNode = (xmlTreeItem*)InitTreeNode(1,xmlparent, gp, parent1);
	gp->removeChild(parent1);

	ExpandTree(hNode);

	emit xmlDataChangedSignal();
	return true;
}


bool apoXmlTreeView::_TreeDragInNotSameRoot(xmlTreeItem*  hFrom, xmlTreeItem*  hTo)
{

	xmlTreeItem *parentFrom = (xmlTreeItem *)hFrom->parent();

	ndxml *xmlparent = GetSelXml(parentFrom);

	ndxml*xmlFrom = GetSelXml(hFrom);
	ndxml*xmlTo = GetSelXml(hTo);
	if (!xmlparent || !xmlFrom || !xmlTo)	{
		return false;
	}
	const char *fromName = ndxml_getname(xmlFrom);
	const char *acceptName = ndxml_getattr_val(xmlTo, "accept_drag_in");
	if (!fromName || !acceptName){
		return false;
	}
	if (0 != ndstricmp(fromName, acceptName))	{
		return false;
	}

	//move
	if (-1 == ndxml_remove(xmlFrom, xmlparent)){
		return false;
	}

	if (-1 == ndxml_insert(xmlTo, xmlFrom)) {
		return false;
	}


	//rebuild tree node 
	parentFrom->removeChild(hFrom);
	InitTreeNode(1,xmlFrom, hTo);
	SetExpand(hTo);
	emit xmlDataChangedSignal();
	return true;
}