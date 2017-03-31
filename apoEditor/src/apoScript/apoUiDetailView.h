/* 
 * file apoUiDetailView.h
 *
 * define detail view 
 *
 * create by duan
 *
 * 2017.2.23
 */
#ifndef _APOUI_DETAIL_VIEW_
#define _APOUI_DETAIL_VIEW_

#include "apoBaseExeNode.h"
#include "myqtitemctrl.h"
#include "apoScript/apoUiXmlTablesWidget.h"

#include <QWidget>
#include <QTableWidget>
#include <QResizeEvent>
#include <QVector>

class apoUiDetailView : public apoUiXmlTableWidget
{
	Q_OBJECT
public:

	explicit apoUiDetailView(QWidget *parent = Q_NULLPTR);
	apoUiDetailView(int rows, int columns, QWidget *parent = Q_NULLPTR);
	~apoUiDetailView();

	bool showDetail(void *data, ndxml_root *xmlfile);
protected:
	bool showExeNode(apoBaseExeNode *node);
	void resizeEvent(QResizeEvent *event) Q_DECL_OVERRIDE;

	virtual bool onChanged(int row, int column, const char *xmlRealValue);
private:
	bool _insertRow(xmlTableItem *nameItem, xmlTableItem* typeItem, xmlTableItem *valItem);
	int _init();
	apoBaseExeNode *m_curNode;
};


class apoUiCommonXmlView :public apoUiXmlTableWidget
{
public:
	apoUiCommonXmlView(QWidget *parent = Q_NULLPTR);
	apoUiCommonXmlView(int rows, int columns, QWidget *parent = Q_NULLPTR);
	~apoUiCommonXmlView();

	bool showDetail(void *data, ndxml_root *xmlfile);
	
private:

	void resizeEvent(QResizeEvent *event);
	int _initShowCtrl();
	void ShowRow(const char *name, const char *val, ndxml* xmlnode, const char *attrName=NULL);
	int ShowXMLValue(ndxml *xml_node, int expand);


};



#endif
