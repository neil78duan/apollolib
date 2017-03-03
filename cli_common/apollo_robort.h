/* file apollo_robort.h
 *
 * define auto-test-robort 
 *
 * create by duan 
 *
 * 2015-11-25
 *
 */

#ifndef _APOLLO_ROBORT_H
#define _APOLLO_ROBORT_H


class NDIConn;
class LoginApollo;

class ApolloRobort
{
public:
	ApolloRobort();
	virtual~ApolloRobort();

	int Create(const char *host, int port, const char *accountName, const char *passwd);
	int Close(int flag);
	bool checkLogin();
	bool CheckValid() ;
	
	NDIConn *getConn() { return m_pConn; }
	LoginApollo *getLogin() { return m_login; };
protected:
	int _connectHost(const char *host, int port);
	int _login(const char *acc, const char *passwd);
	int _selOrCreateRole(const char *roleName);
	int _createRole(const char *roleName);
	NDIConn *m_pConn;
	LoginApollo *m_login;
	char m_account[100];
};
#endif

 