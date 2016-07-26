#ifndef NEWFILEDIALOG_H
#define NEWFILEDIALOG_H

#include <QDialog>
#include <QString>
#include <vector>
#include <map>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QTableWidgetItem>

#include "nd_common/nd_common.h"
#include "myqtitemctrl.h"

namespace Ui {
class NewFileDialog;
}

class NewFileDialog : public QDialog
{
    Q_OBJECT

public:
    explicit NewFileDialog(QWidget *parent = 0);
    ~NewFileDialog();

	bool InitFileRoot(const char *fileRoot, const char *templateFile);
	bool ShowFileList();
	const char *getSelectFile() { return m_selFile; }
	const char *getSelectTitle() { return m_selTitle; }

	const char *m_fileTemplate;
	const char *m_file;
	const char *m_selFile;
	const char *m_selTitle;
	ndxml_root m_xmlRoot;
	void SaveXmlFile(); 
private slots:
    void on_ButtonNew_clicked();

    void on_pushButtonOK_clicked();

    void on_pushButtonCancel_clicked();

    void on_ButtonDel_clicked();

    void on_m_tree_doubleClicked(const QModelIndex &index);
	
    void on_m_tree_itemChanged(QTreeWidgetItem *item, int column);

private:
    Ui::NewFileDialog *ui;
};

#endif // NEWFILEDIALOG_H
