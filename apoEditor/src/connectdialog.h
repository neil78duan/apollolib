#ifndef CONNECTDIALOG_H
#define CONNECTDIALOG_H

#include <QDialog>
#include "nd_common/nd_common.h"

class NDIConn;
class LoginApollo;
class QTimer ;

namespace Ui {
class ConnectDialog;
}

class ConnectDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ConnectDialog(QWidget *parent = 0);
    ~ConnectDialog();


    void ClearLog() ;
    void WriteLog(const char *logText) ;

    void onTimeOut() ;
    int _connectHost(const char *host, int port);
    int _login(const char *user, const char *passwd);
    int _relogin(void *sessionData, size_t session_size);

    int SelOrCreateRole(const char *accountName);
    int createRole(const char *roleName);
    int getRoleData();

	NDIConn *getConnect(){ return m_pConn; }
	bool LoadDataDef(const char *file, const char *script, const char *message_def);
	const char* m_scriptFile;
public:

    ndxml *m_editor_setting;
    ndxml *m_gmCfg ;
    //ndxml_root m_message_define;
private slots:
    void on_loginButton_clicked();

    void on_msgWriteButton_clicked();

    void on_msgSendButton_clicked();

    void on_msgReset_clicked();

    void on_gmMsgButton_clicked();

    void on_pushButton_clicked();

    void on_checkBox16Hex_clicked();

private:
    Ui::ConnectDialog *ui;
    QTimer *timer ;

    NDIConn *m_pConn;
    LoginApollo *m_login;
};

#endif // CONNECTDIALOG_H
