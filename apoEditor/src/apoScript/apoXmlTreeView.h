/* 
 * file apoXmlTreeView.h
 *
 * apollo editor's xml-tree-view
 *
 * create by duan 
 *
 * 2017.2.16
*/

#ifndef APOXMLTREEVIEW_H
#define APOXMLTREEVIEW_H

#include "dragtree.h"
#include "myqtitemctrl.h"
#include "nd_common/nd_common.h"

#include "logic_parser/logic_editor_helper.h"

#include <vector>
#include <QWidget>
#include <QTreeWidgetItem>


typedef std::vector<std::string> strings_vct_t;

class apoXmlTreeView : public dragTree
{
    Q_OBJECT
public:
    explicit apoXmlTreeView(QWidget *parent = 0);

	void setXmlInfo(ndxml *xmldata, int detph,const char *rootName=NULL);
	//void setComfig(ndxml *config);
	void setAlias(LogicEditorHelper::CXMLAlias *alias);

	void hideXmlNodeChildren(const char *xmlName);

	void unCreateNewChild(const char *xmlName);

	void setRootName(const QString &name);
	QString rootName(){ return m_rootName; }

signals:
	void xmlDataChangedSignal();
	void xmlNodeDelSignal(ndxml *delxml);
	void xmlNodeDisplayNameChangeSignal(ndxml *xml);

public slots:
	void DragCallBack(QTreeWidgetItem* from, QTreeWidgetItem* to);
	void onItemChanged(QTreeWidgetItem *item, int column);
	void onItemExpanded(QTreeWidgetItem *item);
protected:
	bool TreeDragCallback(xmlTreeItem*  hFrom, xmlTreeItem* hTo);
	bool _TreeDragInNotSameRoot(xmlTreeItem*  hFrom, xmlTreeItem*  hTo);
	void SetExpand(QTreeWidgetItem* hItem);
	void loadHideNodeInfo(ndxml *xmldata);
	bool isHideChildren(const char *xmlName);
	bool isNotCreateNewChild(const char *xmlName);
	bool createTree();
	bool ExpandTree(QTreeWidgetItem* hItem);
	QTreeWidgetItem* InitTreeNode(int depth, ndxml *xml_node, QTreeWidgetItem *hParent, QTreeWidgetItem *after=NULL);


	void contextMenuEvent(QContextMenuEvent *event);
	void OnPopInsertNode();
	void OnPopXmlDel();
	void onPopMenu(QAction *activeAction);

	bool m_isInitOk;
	int m_disp_depth; //deisplay depth
	ndxml *m_xmldata;
	xmlTreeItem *m_root;
	QString m_rootName;
	//ndxml *m_config;

	LogicEditorHelper::CXMLAlias *m_alias;

	strings_vct_t m_hideChildren;

	strings_vct_t m_unCreateNew;
};

#endif // APOXMLTREEVIEW_H
