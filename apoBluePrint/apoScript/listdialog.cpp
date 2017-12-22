#include "listdialog.h"
#include "ui_listdialog.h"
#include <string>
#include "logic_parser/logic_editor_helper.h"

ListDialog::ListDialog(QWidget *parent, const char *title) :
    QDialog(parent),
    ui(new Ui::ListDialog)
{
    ui->setupUi(this);
    m_selIndex = -1 ;

//     if(title && *title) {
//         ui->tipsLab->setText(title);
//     }
}

ListDialog::~ListDialog()
{
    delete ui;
}

void ListDialog::on_mylist_doubleClicked(const QModelIndex &index)
{
    m_selIndex = index.row() ;
    //emit accepted();
	accept();
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

void ListDialog::on_Search_clicked()
{
	QString search = ui->searchText->text();
	if (search.size() == 0)	{
		m_selIndex = -1;
		return;
	}


	int i = 0;
	std_vctstrings_t::iterator searchBegin = m_selList.begin();
	if (m_selIndex != -1){
		if (m_selIndex >= m_selList.size())	{
			m_selIndex = 0;
		}
		searchBegin += m_selIndex;
		i = m_selIndex;
	}

	for (std_vctstrings_t::iterator it = m_selList.begin(); it != m_selList.end(); ++it) {
		if (-1 != it->indexOf(search)) {
			m_selIndex = i;
			break;
		}
		++i;
	}
	ui->mylist->clearSelection();
	if (m_selIndex >=0 ){
		ui->mylist->setCurrentRow(m_selIndex);
	}
}
