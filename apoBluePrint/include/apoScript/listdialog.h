#ifndef LISTDIALOG_H
#define LISTDIALOG_H

#include <QDialog>
#include <vector>
#include <QString>

namespace Ui {
class ListDialog;
}

class ListDialog : public QDialog
{
    Q_OBJECT

public:
	explicit ListDialog(QWidget *parent = 0, const char *title = NULL);
    ~ListDialog();

    void InitList() ;
    int GetSelect() {
        return m_selIndex;
    }
    bool GetSelectText(QString &val) {

        if(-1==m_selIndex || m_selIndex >= m_selList.size()) {
            return false ;
        }
        val = m_selList[m_selIndex] ;
        return true ;
    }

    typedef std::vector<QString> std_vctstrings_t ;
    std_vctstrings_t m_selList ;
    int m_selIndex ;

private slots:
    void on_mylist_doubleClicked(const QModelIndex &index);

    void on_mylist_clicked(const QModelIndex &index);

    void on_Search_clicked();

private:
    Ui::ListDialog *ui;
};

#endif // LISTDIALOG_H
