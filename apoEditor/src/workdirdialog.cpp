#include "workdirdialog.h"
#include "ui_workdirdialog.h"
#include "nd_common/nd_common.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QSettings>


bool trytoGetSetting(QString &workPath,QString &cfgSetting,QString &editorCfg,QWidget *parent)
{
    QSettings settings("duanxiuyun", "ApolloEditor");
    workPath = settings.value("workdir_path").toString();
    cfgSetting =settings.value("setting_config").toString() ;
    editorCfg = settings.value("editor_config").toString() ;
    if(workPath.isEmpty() || cfgSetting.isEmpty()) {
		return inputSetting(workPath, cfgSetting, editorCfg, parent);
    }

	std::string config_setting = editorCfg.toStdString();
	const char * pName = nd_filename(config_setting.c_str());
	if (pName && 0==ndstricmp(pName,"editor_setting.xml")) {
		char mypaht[1024];
		if (nd_getpath(config_setting.c_str(), mypaht, sizeof(mypaht))) {
			editorCfg = mypaht;
			editorCfg += "/editor_config_setting.json";

			QSettings settings("duanxiuyun", "ApolloEditor");
			settings.setValue("editor_config", editorCfg);
		}
	}
		
    return true ;
}

bool inputSetting(QString &workPath,QString &cfgSetting,QString &editorCfg,QWidget *parent)
{
    workDirDialog dlg(parent) ;
    int ret = dlg.exec() ;
    if(ret != QDialog::Accepted) {
        return false ;
    }
    workPath = dlg.getWorkingPath() ;
    cfgSetting = dlg.getSetting() ;
	editorCfg = dlg.getEditorCfg();

    QSettings settings("duanxiuyun", "ApolloEditor");
    settings.setValue("workdir_path", workPath);
    settings.setValue("setting_config",cfgSetting) ;
    settings.setValue("editor_config",editorCfg) ;

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

void workDirDialog::on_btEditorFileSel_clicked()
{
    m_scriptEditorCfg = QFileDialog::getOpenFileName(this, tr("open file"), ".",  tr("Allfile(*.*);;xmlfile(*.xml)"));
    if(!m_scriptEditorCfg.isEmpty()) {
        ui->editorCfg->setText(m_scriptEditorCfg);
    }
}
