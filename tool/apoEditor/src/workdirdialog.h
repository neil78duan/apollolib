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
    explicit workDirDialog(QWidget *parent = 0);
    ~workDirDialog();

    QString &getWorkingPath() {return m_workPaht;}
private slots:
    void on_okPushButton_clicked();

    void on_selWorkdirButton_clicked();

    void on_ExitButton_clicked();


private:
    Ui::workDirDialog *ui;
    QString m_workPaht ;
    QString m_setting ;
    QString m_scriptEditorCfg ;
};

bool trytoGetSetting(QString &workPath, QWidget *parent=NULL) ;
bool inputSetting(QString &workPath, QWidget *parent=NULL);
#endif // WORKDIRDIALOG_H
