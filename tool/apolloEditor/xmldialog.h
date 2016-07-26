#ifndef XMLDIALOG_H
#define XMLDIALOG_H

#include <QDialog>
#include <QString>
#include <vector>
#include <map>
#include <QTreeWidget>

#include "nd_common/nd_common.h"

#include "logic_editor_helper.h"
#include "myqtitemctrl.h"

using namespace LogicEditorHelper;

namespace Ui {
class XMLDialog;
}


typedef bool(*dlgAccCallBack) (QDialog *curDlg);
typedef std::vector<QString> text_list_t;

class XMLDialog : public QDialog
{
    Q_OBJECT

public:
    explicit XMLDialog(QWidget *parent = 0);
    ~XMLDialog();


    bool preInit(dlgAccCallBack initF, dlgAccCallBack acceptF, dlgAccCallBack rejectF) ;
    void setButtonText(const char *okText, const char *cancelText) ;

    void SetXML(ndxml_root *config_xml, ndxml_root *data_root,const char *title=NULL);
    //void SetMessageID(ndxml_root *funclist_xml);
    //void SetEventsID(ndxml_root *funclist_xml);
    ndxml *GetCurrentXml();

	bool loadUserdefDisplayList(ndxml_root &xmlNameList, const char *name);
	bool addDisplayNameList(const char *name, text_list_t &text_list);

private slots:
    void on_xmlTree_itemClicked(QTreeWidgetItem *item, int column);

    void on_xmlTable_cellClicked(int row, int column);

    void on_xmlTable_itemChanged(QTableWidgetItem *item);

    void on_xmlTree_customContextMenuRequested(const QPoint &pos);

    void on_xmlTree_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous);
	
	void on_xmlTree_itemCollapsed(QTreeWidgetItem *item);

    void on_xmlTree_itemExpanded(QTreeWidgetItem *item);

    void on_xmlTree_itemChanged(QTreeWidgetItem *item, int column);

    //void on_xmlTree_itemEntered(QTreeWidgetItem *item, int column);

    void on_ButtonOK_clicked();

    void on_ButtonCancel_clicked();

private:

    //void mouseReleaseEvent(QMouseEvent *event) ;
    ndxml* GetSelXml(QTreeWidgetItem* hItem);
    void DragCallBack(QTreeWidgetItem* from, QTreeWidgetItem* to) ;
    bool TreeDragCallback(xmlTreeItem*  hFrom, xmlTreeItem* hTo) ;

    void CfgChanged() { m_stat = 1;}
    int DisplaySelXml();
    int _initShowCtrl(QTableWidget *show_ctrl );
    int ShowXMLValue(ndxml *xml_node,QTableWidget *show_ctrl, int expand );
    void ShowRow(const char *name, const char *val, ndxml* param,QTableWidget *show_ctrl);

    bool CreateXmlTree(ndxml_root *xml_root)  ;
    QTreeWidgetItem *InitTreeNode(ndxml *xml_node,QTreeWidgetItem *hParent);

    bool ExpandTree(QTreeWidgetItem* hItem) ;
    bool _GetUserDefinData(const char *pUserParam, QString &val);
    bool _GetUserSelEnumVal(const text_list_t &enum_vals, QString &val);
    bool _GetFileName(bool bOpen, QString & strFile, const char *default_file) ;
    bool _OpenFilter(QString &strPath,const QString &tip) ;

    bool _beginEdit(int nRow, int nCol) ;
    void OnPopInsertNode();
    void OnPopXmlDel();

    int m_stat:1 ;
    int m_inEditing:1;
    int m_inInit:1 ;
    int m_treeInInit:1;
    Ui::XMLDialog *ui;

    xmlTreeItem *m_curItem ; //current edit item
    xmlTreeItem *m_dragItem ;

    ndxml_root *m_root ;
    ndxml_root *m_config;

    text_list_t m_cPPfunclist;
    text_list_t m_eventsList;
    text_list_t m_messageList;
    CXMLAlias m_alias ;

    dlgAccCallBack m_myOKFunc;
    dlgAccCallBack m_myCancelFunc;

	std::string m_title;
	typedef std::map<std::string, text_list_t > text_map_t;
	text_map_t m_userDefineList;
};

#endif // XMLDIALOG_H
