#ifndef XMLDIALOG_H
#define XMLDIALOG_H

#include <QDialog>
#include <QString>
#include <vector>
#include <map>
#include <QTreeWidget>

#include "nd_common/nd_common.h"

#include "logic_parser/logic_editor_helper.h"
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

    void showXml( ndxml_root *data_root,const char *title=NULL);
    ndxml *GetCurrentXml();


private slots:

    void on_ButtonOK_clicked();
    void on_ButtonCancel_clicked();

	void CfgChanged();
	void onTreeCurItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous);
	void onXmlNodeDel(ndxml *xmlnode);

private:

    int m_stat:1 ;
    Ui::XMLDialog *ui;

    ndxml_root *m_root ;
    ndxml_root *m_config;


    dlgAccCallBack m_myOKFunc;
    dlgAccCallBack m_myCancelFunc;

};

#endif // XMLDIALOG_H
