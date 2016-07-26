#ifndef WORKDIRDIALOG_H
#define WORKDIRDIALOG_H

#include <QDialog>
#include <QString>

namespace Ui {
class workDirDialog;
}

class workDirDialog : public QDialog
{
    Q_OBJECT

public:
    explicit workDirDialog(QWidget *parent = 0, const char *dftWorkPath=NULL, const char *dftSettingFile= NULL);
    ~workDirDialog();

    QString &getWorkingPath() {return m_workPaht;}
    QString &getSetting() {return m_setting;}
private slots:
    void on_okPushButton_clicked();

    void on_selSettingButton_clicked();

    void on_selWorkdirButton_clicked();

    void on_ExitButton_clicked();

private:
    Ui::workDirDialog *ui;
    QString m_workPaht ;
    QString m_setting ;
};

bool trytoGetSetting(QString &workPath,QString &cfgSetting,QWidget *parent=NULL) ;
bool inputSetting(QString &workPath,QString &cfgSetting,QWidget *parent=NULL);
#endif // WORKDIRDIALOG_H
