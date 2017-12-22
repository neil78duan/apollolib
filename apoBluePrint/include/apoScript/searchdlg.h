#ifndef SEARCHDLG_H
#define SEARCHDLG_H

#include "nd_common/nd_common.h"
#include "logic_parser/logic_editor_helper.h"

#include <QDialog>

namespace Ui {
class SearchDlg;
}

class SearchDlg : public QDialog
{
    Q_OBJECT

public:
    explicit SearchDlg(ndxml *xmlFile, QWidget *parent = 0);
    ~SearchDlg();

	ndxml *getResult();
private slots:
    void on_SearchButton_clicked();
    void on_CancelButton_clicked();

private:
	int searchText(const char *aimText);
	int searchOnlyCurrent(ndxml *xml, const char *aimText);
	int searchFromXml(ndxml *xml, const char *aimText);
	void storeSearchValue(ndxml *aimXml);
    Ui::SearchDlg *ui;
	ndxml *m_xmlFile;
	ndxml_root m_xmlResult;
};

#endif // SEARCHDLG_H
