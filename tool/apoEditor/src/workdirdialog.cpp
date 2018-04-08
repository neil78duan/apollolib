#include "workdirdialog.h"
#include "ui_workdirdialog.h"
#include "nd_common/nd_common.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QSettings>


bool trytoGetSetting(QString &workPath,QWidget *parent)
{
    QSettings settings("duanxiuyun", "ApolloEditor");
    workPath = settings.value("workdir_path").toString();

    if(workPath.isEmpty() ) {
        return inputSetting(workPath, parent);
    }
		
    return true ;
}

bool inputSetting(QString &workPath,QWidget *parent)
{
    workDirDialog dlg(parent) ;
    int ret = dlg.exec() ;
    if(ret != QDialog::Accepted) {
        return false ;
    }
    workPath = dlg.getWorkingPath() ;

    QSettings settings("duanxiuyun", "ApolloEditor");
    settings.setValue("workdir_path", workPath);

    return true ;
}

/////////////

workDirDialog::workDirDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::workDirDialog)
{
    ui->setupUi(this);
	const char *curPath = nd_getcwd() ;
	if(curPath && *curPath)
		ui->workingEdit->setText(curPath);
	
//
//     if(dftSettingFile && *dftSettingFile)
//         ui->settingEdit->setText(dftSettingFile);
}

workDirDialog::~workDirDialog()
{
    delete ui;
}

void workDirDialog::on_okPushButton_clicked()
{
    if(m_workPaht.isEmpty()) {
        QMessageBox::question(this,tr("Error"), tr("Please select set working path !"),QMessageBox::Ok );
        ui->workingEdit->setFocus();
        return ;
    }
//     if(m_setting.isEmpty()) {
//         QMessageBox::question(this,tr("Error"), tr("Please select editor setting file !"),QMessageBox::Ok );
//         ui->settingEdit->setFocus();
//         return ;
//     }
    QDialog::accept() ;
}

void workDirDialog::on_selWorkdirButton_clicked()
{
    m_workPaht = QFileDialog::getExistingDirectory(this,"Select setting file ",".");
    if(!m_workPaht.isEmpty()) {
        ui->workingEdit->setText(m_workPaht);
    }
}

void workDirDialog::on_ExitButton_clicked()
{
    QDialog::reject() ;
}

