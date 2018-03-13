#include "xmldialog.h"
#include "ui_xmldialog.h"
#include "logic_parser/dbl_mgr.h"
#include "apoScript/apoEditorSetting.h"
#include "apoScript/apoUiXmlTablesWidget.h"
#include "apoScript/apoUiDetailView.h"

#include "listdialog.h"
#include <QFileDialog>
#include <QMenu>
#include <QMessageBox>
#include <QMouseEvent>
#include <QTreeWidget>
#include "dragtree.h"

void _xml_errorout(const char *errdesc)
{
    nd_logerror("parse xml error %s", errdesc) ;
}


XMLDialog::XMLDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::XMLDialog)
{
    ui->setupUi(this);
	m_config = apoEditorSetting::getInstant()->getConfig();



    m_myOKFunc = 0;
    m_myCancelFunc=0;

	QObject::connect(ui->xmlTree, SIGNAL(currentItemChanged(QTreeWidgetItem *, QTreeWidgetItem *)),
		this, SLOT(onTreeCurItemChanged(QTreeWidgetItem *, QTreeWidgetItem *)));

	QObject::connect(ui->xmlTree, SIGNAL(xmlNodeDelSignal(ndxml *)),
		this, SLOT(onXmlNodeDel(ndxml *)));

	QObject::connect(ui->xmlTree, SIGNAL(xmlDataChangedSignal()), this, SLOT(CfgChanged()));
	QObject::connect(ui->xmlTable, SIGNAL(xmlDataChanged()), this, SLOT(CfgChanged()));
}

XMLDialog::~XMLDialog()
{
    delete ui;
}


void XMLDialog::onTreeCurItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous)
{
	xmlTreeItem *pXmlItem = dynamic_cast<xmlTreeItem*>(current);
	if (!pXmlItem)	{
		return;
	}
	ndxml *pxml = (ndxml *)pXmlItem->getUserData();
	if (!pxml)	{
		return;
	}

	apoUiXmlTableWidget* table = (apoUiXmlTableWidget* ) ui->xmlTable;

	table->showDetail(pxml, m_root);
}


void XMLDialog::onXmlNodeDel(ndxml *xmlnode)
{
	ndxml_delxml(xmlnode, NULL);
}

void XMLDialog::CfgChanged() 
{ 
	m_stat = 1; 
}

void XMLDialog::setButtonText(const char *okText, const char *cancelText)
{
    ui->ButtonCancel->setText(cancelText);
    ui->ButtonOK->setText(okText );
}

bool XMLDialog::preInit(dlgAccCallBack initF, dlgAccCallBack acceptF, dlgAccCallBack rejectF)
{
    if(initF) {
        initF(this) ;
    }

    m_myOKFunc = acceptF;
    m_myCancelFunc = rejectF;
    return true ;
}


#define BACKUP_NAME "xmlcfgbackup.xml"
void XMLDialog::showXml(ndxml_root *data_root, const char *title)
{
    m_root = data_root ;
    nd_chdir(nd_getcwd());
    ndxml_save(data_root, BACKUP_NAME);

	apoXmlTreeView *xmlTree = (apoXmlTreeView *) ui->xmlTree;
	xmlTree->setXmlInfo(data_root, 100, title);

}



ndxml *XMLDialog::GetCurrentXml()
{
    xmlTreeItem *curItem = (xmlTreeItem*) ui->xmlTree->currentItem() ;
    if(!curItem)
        return NULL;
    return (ndxml *) curItem->getUserData();
}



void XMLDialog::on_ButtonOK_clicked()
{
    if(m_myOKFunc) {
        bool ret = m_myOKFunc(this) ;
        if(ret) {
            return ;
        }
    }
    QDialog::accept() ;
}

void XMLDialog::on_ButtonCancel_clicked()
{
    if(m_myCancelFunc) {
        bool ret = m_myCancelFunc(this) ;
        if(ret) {
            return ;
        }
    }
    QDialog::reject() ;

}
