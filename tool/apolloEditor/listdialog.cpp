#include "listdialog.h"
#include "ui_listdialog.h"
#include <string>
#include "logic_editor_helper.h"

ListDialog::ListDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ListDialog)
{
    ui->setupUi(this);
    m_selIndex = -1 ;
}

ListDialog::~ListDialog()
{
    delete ui;
}

void ListDialog::on_mylist_doubleClicked(const QModelIndex &index)
{
    m_selIndex = index.row() ;
    accepted();
}

void ListDialog::InitList()
{
    for(std_vctstrings_t::iterator it= m_selList.begin(); it!= m_selList.end();++it) {
        std::string str1 = it->toStdString() ;
        char buf[1024];
        const char *p = LogicEditorHelper::getDisplayNameFromStr(str1.c_str(),buf, sizeof(buf)) ;
        ui->mylist->addItem(p);
    }

}

void ListDialog::on_mylist_clicked(const QModelIndex &index)
{
    m_selIndex = index.row() ;
}
