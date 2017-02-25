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

	void setXmlInfo(ndxml *xmldata, int detph);
	//void setComfig(ndxml *config);
	void setAlias(LogicEditorHelper::CXMLAlias *alias);

	void hideXmlNodeChildren(const char *xmlName);
signals:

public slots:

protected:
	void loadHideNodeInfo(ndxml *xmldata);
	bool isHideChildren(const char *xmlName);
	bool createTree();
	bool ExpandTree(QTreeWidgetItem* hItem);
	QTreeWidgetItem* InitTreeNode(int depth, ndxml *xml_node, QTreeWidgetItem *hParent, QTreeWidgetItem *after=NULL);

	int m_disp_depth; //deisplay depth
	ndxml *m_xmldata;

	//ndxml *m_config;

	LogicEditorHelper::CXMLAlias *m_alias;

	strings_vct_t m_hideChildren;
};

#endif // APOXMLTREEVIEW_H
