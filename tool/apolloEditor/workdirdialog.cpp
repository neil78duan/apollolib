#include "workdirdialog.h"
#include "ui_workdirdialog.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QSettings>


bool trytoGetSetting(QString &workPath,QString &cfgSetting,QWidget *parent)
{
    QSettings settings("duanxiuyun", "ApolloEditor");
    workPath = settings.value("workdir_path").toString();
    cfgSetting =settings.value("setting_config").toString() ;
    if(workPath.isEmpty() || cfgSetting.isEmpty()) {
        return inputSetting(workPath, cfgSetting, parent) ;
    }
    return true ;
}

bool inputSetting(QString &workPath,QString &cfgSetting,QWidget *parent)
{
    workDirDialog dlg(parent) ;
    int ret = dlg.exec() ;
    if(ret != QDialog::Accepted) {
        return false ;
    }
    workPath = dlg.getWorkingPath() ;
    cfgSetting = dlg.getSetting() ;

    QSettings settings("duanxiuyun", "ApolloEditor");
    settings.setValue("workdir_path", workPath);
    settings.setValue("setting_config",cfgSetting) ;
    return true ;
}

/////////////

workDirDialog::workDirDialog(QWidget *parent, const char *dftWorkPath, const char *dftSettingFile) :
    QDialog(parent),
    ui(new Ui::workDirDialog)
{
    ui->setupUi(this);
    if(dftWorkPath && *dftWorkPath)
        ui->workingEdit->setText(dftWorkPath);

    if(dftSettingFile && *dftSettingFile)
        ui->settingEdit->setText(dftSettingFile);
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
    if(m_setting.isEmpty()) {
        QMessageBox::question(this,tr("Error"), tr("Please select editor setting file !"),QMessageBox::Ok );
        ui->settingEdit->setFocus();
        return ;
    }
    QDialog::accept() ;
}

void workDirDialog::on_selSettingButton_clicked()
{
    m_setting = QFileDialog::getOpenFileName(this, tr("open file"), ".",  tr("Allfile(*.*);;xmlfile(*.xml)"));
    if(!m_setting.isEmpty()) {
        ui->settingEdit->setText(m_setting);
    }

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
