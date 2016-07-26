#include "newfiledialog.h"
#include "ui_newfiledialog.h"

#include <QFileDialog>
#include <QMessageBox>
NewFileDialog::NewFileDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NewFileDialog)
{
    ui->setupUi(this);

	m_selFile = 0;
	m_selTitle = 0;
	ndxml_initroot(&m_xmlRoot);
}

NewFileDialog::~NewFileDialog()
{
	delete ui;
	ndxml_destroy(&m_xmlRoot);
}

bool NewFileDialog::InitFileRoot(const char *fileRoot, const char *templateFile)
{
	m_fileTemplate = templateFile;
	m_file = fileRoot;
	ndxml_initroot(&m_xmlRoot);
	if (ndxml_load_ex((char*)fileRoot, &m_xmlRoot, "utf8")) {		
		QString tipText;
		tipText.sprintf("open file %s error", fileRoot);
		QMessageBox::warning(NULL, "Error", tipText, QMessageBox::Ok);
		return false;
	}
	ShowFileList();
	return true;
}


bool NewFileDialog::ShowFileList()
{
	ui->m_tree->clear();

	xmlTreeItem *root = new xmlTreeItem(ui->m_tree, QStringList(QString("Files")));
	root->setUserData(NULL);

	ndxml_root *xml = ndxml_getnode(&m_xmlRoot, "script_file_manager");
	if (!xml){
		nd_logerror("file error xml\n");
		return false;
	}
	int num = ndxml_num(xml);
	for (int i = 0; i < num; i++) {
		ndxml *node = ndxml_getnodei(xml, i);
		if (!node)
			continue;
		const char *dispName = ndxml_getattr_val(node, "name");
		if (!dispName)	{
			dispName = ndxml_getval(node);
		}
		xmlTreeItem *leaf = new xmlTreeItem(root, QStringList(QString(dispName)));

		leaf->setFlags(leaf->flags() | (Qt::ItemIsEditable ));
		leaf->setUserData(node);
	}
	root->setExpanded(true);
	
	return true;
}

void NewFileDialog::SaveXmlFile()
{
	ndxml_save(&m_xmlRoot, m_file);
}


void NewFileDialog::on_ButtonNew_clicked()
{
	QString filter = QFileDialog::getSaveFileName(this, tr("open file"), ".", tr("Allfile(*.*);;xmlfile(*.xml)"));

	if (filter.isNull()) {
		return ;
	}

	ndxml_root *xml = ndxml_getnode(&m_xmlRoot, "script_file_manager");
	if (!xml){
		QMessageBox::warning(NULL, "Error", "File Error!!!", QMessageBox::Ok);
		return;
	}
	if (-1 == nd_cpfile(m_fileTemplate, filter.toStdString().c_str() )) {
		QMessageBox::warning(NULL, "Error", "SAVE File Error!!!", QMessageBox::Ok);
		return;
	}
	ndxml *newNode = ndxml_addnode(xml, "file", filter.toStdString().c_str());
	if (newNode){
		ndxml_addattrib(newNode, "name", "New-File");
	}
	SaveXmlFile();
	ShowFileList();
}

// 
// void NewFileDialog::on_treeWidget_itemChanged(QTreeWidgetItem *item, int column)
// {
// 	xmlTreeItem* curItem = dynamic_cast<xmlTreeItem*>(item);
// 	if (!curItem)
// 		return;
// 	ndxml *xml = (ndxml *)curItem->getUserData();
// 	if (xml) {
// 		QString text1 = item->text(column);
// 		ndxml_setattrval(xml, "name", text1.toStdString().c_str());
// 		SaveXmlFile();
// 	}
// }

void NewFileDialog::on_pushButtonOK_clicked()
{
	// TODO: Add your control notification handler code here
	m_selFile = 0;
	xmlTreeItem *curItem = (xmlTreeItem*)ui->m_tree->currentItem();
	if (curItem) {
		ndxml *xml = (ndxml *)curItem->getUserData();
		if (xml) {
			m_selFile = ndxml_getval(xml);
			m_selTitle = ndxml_getattr_val(xml, "name");
			QDialog::accept();
			return;
		}
	}
	QMessageBox::warning(NULL, "Error", "Please select the file you need edit!", QMessageBox::Ok);
	
}

void NewFileDialog::on_pushButtonCancel_clicked()
{

	QDialog::reject();
}

void NewFileDialog::on_ButtonDel_clicked()
{
	xmlTreeItem *curItem = (xmlTreeItem*)ui->m_tree->currentItem();
	if (!curItem)
		return;
	ndxml *xml = (ndxml *)curItem->getUserData();
	if (!xml) {
		return;
	}

	ndxml_root *xmlfiles = ndxml_getnode(&m_xmlRoot, "script_file_manager");
	if (!xmlfiles){
		return;
	}
	int num = ndxml_num(xmlfiles);
	for (int i = 0; i < num; i++) {
		ndxml *node = ndxml_getnodei(xmlfiles, i);
		if (node == xml) {
			
			int ret = QMessageBox::question(this, tr("Question"), tr("Do you delete?"),
				QMessageBox::Yes | QMessageBox::No,	QMessageBox::Yes);
			if (QMessageBox::No == ret) {
				return;
			}

			nd_rmfile(ndxml_getval(node));
			ndxml_delxml(node, xmlfiles);

			SaveXmlFile();
			ShowFileList();
			return;
		}
	}

}

void NewFileDialog::on_m_tree_doubleClicked(const QModelIndex &index)
{
	ui->m_tree->setCurrentIndex(index);
	on_pushButtonOK_clicked();
}

void NewFileDialog::on_m_tree_itemChanged(QTreeWidgetItem *item, int column)
{
	xmlTreeItem* curItem = dynamic_cast<xmlTreeItem*>(item);
	if (!curItem)
		return;
	ndxml *xml = (ndxml *)curItem->getUserData();
	if (xml) {
		QString text1 = item->text(column);
		ndxml_setattrval(xml, "name", text1.toStdString().c_str());
		SaveXmlFile();
	}
}
