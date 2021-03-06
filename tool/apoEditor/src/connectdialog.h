#ifndef CONNECTDIALOG_H
#define CONNECTDIALOG_H

#include <QDialog>
#include "nd_common/nd_common.h"
#include <vector>

class NDCliConnector;
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
	int _login(const char *user, const char *passwd, bool skipAuth);
    int _relogin(void *sessionData, size_t session_size);

    int SelOrCreateRole(const char *accountName);
	int ShowServers(void *hostsBuf, int size,const char *tips);
    //int createRole(const char *roleName);
    int getRoleData();

	NDCliConnector *getConnect(){ return m_pConn; }

	bool LoadClientScript(const char *file, const char *dblFile);

	void saveCurrentSendMsg(const QString &msgName);
	const char* m_scriptFile;
	const char* m_dblDataFile;
public:
	typedef std::vector<QString>MsgHistoryVct_t;
	typedef std::vector<ndxml*>XmlVct_t;

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

    void on_pushButtonExit_clicked();

    void on_pushButton_2_clicked();

    void on_pushButtonGetPingVal_clicked();

private:
	void saveHost(const QString &hostName);
	void getSendMsgHistory(MsgHistoryVct_t &historyList);
	void saveSendMsgHistory(MsgHistoryVct_t &historyList);
	void getHistoryNodes(XmlVct_t &xmlHistory);

	void InitHostList();
	void destroyConnect();

    Ui::ConnectDialog *ui;
    QTimer *timer ;

    NDCliConnector *m_pConn;
    LoginApollo *m_login;
	MsgHistoryVct_t m_msgHistory;
};

void initGlobalParser();
#endif // CONNECTDIALOG_H
