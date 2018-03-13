#ifndef RUNFUNCDIALOG_H
#define RUNFUNCDIALOG_H

#include "nd_common/nd_common.h"
#include <QDialog>

namespace Ui {
class RunFuncDialog;
}

class RunFuncDialog : public QDialog
{
    Q_OBJECT

public:
    explicit RunFuncDialog(QWidget *parent = 0);
    ~RunFuncDialog();

	QString getArgs();
	QString getFunc();

	bool initFunctionList(ndxml *xmlFile,const char *dftFunc=NULL);

private:
	Ui::RunFuncDialog *ui;
	bool insertFunctions(ndxml *xmlFuncCollection);
};

#endif // RUNFUNCDIALOG_H
