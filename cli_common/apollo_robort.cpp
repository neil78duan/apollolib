/* file apollo_robort.cpp
 *
 * define auto-test-robort 
 *
 * create by duan 
 *
 * 2015-11-25
 *
 */


#include "nd_common/nd_common.h"
#include "nd_common/nd_iconv.h"
//#include "cli_common/netui_atl.h"
//#include "commonTest.h"
//#include "cli_common/gameMessage.h"
#include "apollo_errors.h"
//#include "message_inc.h"

#include "apollo_robort.h"
#include "login_apollo.h"

#define _robort_SEND_AND_WAIT(_conn, _omsg, _rmsg_buf,_wait_maxid, _wait_minid,_sendflag) \
	if(0!=ndSendAndWaitMessage(_conn,_omsg.GetMsgAddr(),_rmsg_buf,_wait_maxid, _wait_minid,_sendflag,WAITMSG_TIMEOUT) ) {	\
		nd_logerror( "send and wait data error code =%d\n", nd_object_lasterror(_conn)); \
		return -1;		\
			}


static int my_msg_default_handler(NDIConn* , nd_usermsgbuf_t *msg)
{
	if (ND_USERMSG_MAXID(msg) > 16 || ND_USERMSG_MINID(msg)> 64){
		nd_logerror("get error message (%d,%d)\n", ND_USERMSG_MAXID(msg) , ND_USERMSG_MINID(msg));
		return -1;
	}
	nd_logmsg("recv (%d,%d) message data: length = %d \n", ND_USERMSG_MAXID(msg), ND_USERMSG_MINID(msg), ND_USERMSG_DATALEN(msg));
	return 0;
}

ApolloRobort::ApolloRobort()
{
	m_pConn = 0;
	m_login = 0;
}

ApolloRobort::~ApolloRobort()
{
}

int ApolloRobort::Create(const char *host, int port, const char *accountName, const char *passwd, int iAccType )
{
	if (-1 == _connectHost(host,port)){
		nd_logerror("connect host error\n");
		return -1;
	}

	nd_logmsg("CONNECT %s:%d SUCCESS\n", host, port);

	if (-1 == _login(accountName, passwd, iAccType))	{
		nd_logerror("user login error\n");
		Close(1);
		return -1;
	}
	nd_logmsg("ACCOUNT %s login SUCCESS\n", accountName);

	m_pConn->SetDftMsgHandler(my_msg_default_handler);
	//gmtoolMsgHandlerInit(m_pConn);
	char roleName[100];
	snprintf(roleName, sizeof(roleName), "%s_role", accountName);

	if (-1 == _selOrCreateRole(roleName))	{
		nd_logerror("create or select role error\n");

		m_pConn->Close();
		DestroyConnectorObj(m_pConn);
		m_pConn = NULL;
		return -1;
	}
	return 0;
}

int ApolloRobort::Close(int flag)
{
	if (m_pConn) {
		if (m_pConn->CheckValid())	{
			m_pConn->Close(flag);
		}
		DestroyConnectorObj(m_pConn);
		m_pConn = NULL;
	}
	

	if (m_login){
		delete m_login;
		m_login = 0;
	}

	return 0;

}

bool ApolloRobort::checkLogin()
{
	if (m_pConn && m_pConn->CheckValid()) {
		int priv_level = 0 ;
		int size = sizeof(priv_level) ;
		
		m_pConn->ioctl(NDIOCTL_GET_LEVEL, &priv_level, &size) ;
		return  priv_level >= EPL_LOGIN ;
	}
	return false;
}

bool ApolloRobort::CheckValid()
{
	if (m_pConn && m_pConn->CheckValid()) {
		return true;
	}
	return false ;
}

int ApolloRobort::_connectHost(const char *host, int port)
{
	if (m_pConn){
		DestroyConnectorObj(m_pConn);
	}

	NDIConn *pConn = CreateConnectorObj(NULL);
	if (!pConn) {
		return 0;
	}
	//initCommonMessageHandle(pConn);

	if (-1 == pConn->Open(host, port, "tcp-connector", NULL)) {
		nd_logerror("connect %s:%d ERROR \n", host, port);
		DestroyConnectorObj(pConn);
		return -1;
	}
	nd_logmsg("connect %s:%d success \n", host, port);
	m_pConn = pConn;
	return 0;
}

int ApolloRobort::_login(const char *accName, const char *passwd,int iAccType )
{
	int ret = 0;
	if (m_login){
		//ApolloDestroyLoginInst(m_login);
		delete m_login;
	}
	
	m_login = new LoginApollo(m_pConn->GetHandle(), NULL);
	if (!m_login){
		return -1;
	}
	
	ret = m_login->Login(accName, passwd, (ACCOUNT_TYPE)iAccType,0, false);
	if (-1 == ret) {
		if (m_login->GetLastError() == NDSYS_ERR_NOUSER) {
			account_base_info acc;
			myInitAccCreateInfo(acc, (ACCOUNT_TYPE)iAccType, accName, passwd);

			ret = m_login->CreateAccount(&acc);
		}
	}

	if (ret == 0) {
		nd_logmsg("login complete SUCCESS!\n", m_login->getAccountID());
		m_login->ReadyGame();
	}
	else {
		nd_logerror("login error %d \n", m_login->GetLastError());
		return -1;
	}

	return 0;
}


int ApolloRobort::_selOrCreateRole(const char *roleName)
{
	ApolloServerInfo bufs[20];
	int num = m_login->GetServerList(bufs, ND_ELEMENTS_NUM(bufs));
	if (num == 0) {
		nd_logerror("get host list number=0\n");
		return -1;
	}
	int ret = m_login->EnterServer((const char*)bufs[0].inet_ip, bufs[0].port);
	if (ret == 0) {
		nd_logmsg("redirect server success\n");
	}
	else {
		nd_logerror("enter game server error \n");
		return -1;
	}
	//get role list
	role_base_info role;
	ret = m_login->GetRoleList(&role, 1);
	if (ret == 0){
		return m_login->CreateRole(roleName, role);
	}

// 
// 	NDOStreamMsg omsg(NETMSG_MAX_LOGIN, LOGIN_MSG_GET_ROLE_LIST_REQ);
// 	omsg.Write((NDUINT8)0);
// 	nd_handle h = m_pConn->GetHandle();
// 	nd_usermsgbuf_t recv_msg;
// 
// 	_robort_SEND_AND_WAIT(h, omsg, &recv_msg, NETMSG_MAX_LOGIN, LOGIN_MSG_GET_ROLE_LIST_ACK, 0)
// 	else {
// 		NDUINT32 roleid = 0;
// 		NDUINT32 error_code = 0;
// 		NDUINT8 name[USER_NAME_SIZE];
// 
// 		NDIStreamMsg inmsg(&recv_msg);
// 		inmsg.Read(roleid);
// 
// 		if (roleid == 0) {
// 			inmsg.Read(error_code);
// 			if (error_code) {
// 				nd_logerror("get role list error : %d\n", error_code);
// 				return -1;
// 			}
// 			return _createRole(roleName);
// 		}
// 		else {
// 			inmsg.Read(name, sizeof(name));
// 
// 			//char roleName[128];
// 			//nd_utf8_to_gbk((const char*)name, roleName, sizeof(roleName));
// 
// 			nd_logmsg("get role %s success \n", roleName);
// 
// 			//read attribute
// 			NDUINT16 num = 0;
// 			if (0 == inmsg.Read(num)) {
// 				for (int i = 0; i < num; ++i) {
// 					NDUINT8 aid;
// 					float val;
// 					inmsg.Read(aid); inmsg.Read(val);
// 					//nd_logmsg("load role attribute id = %d val =%f \n", aid, val);
// 				}
// 			}
// 		}
// 	}
	return 0;
}

int ApolloRobort::_createRole(const char *roleName)
{
	NDOStreamMsg omsg(NETMSG_MAX_LOGIN, LOGIN_MSG_CREATE_ROLE_REQ);
	omsg.Write((NDUINT8*)roleName);

	omsg.Write((NDUINT16)1);
	omsg.Write((NDUINT8)20);
	omsg.Write((float)0);

	nd_handle h = m_pConn->GetHandle();
	nd_usermsgbuf_t recv_msg;
	_robort_SEND_AND_WAIT(h, omsg, &recv_msg, NETMSG_MAX_LOGIN, LOGIN_MSG_CREATE_ROLE_ACK, 0)
	else {
		NDUINT32 roleid = 0;
		NDUINT32 error_code = 0;
		NDUINT8 name[USER_NAME_SIZE];

		NDIStreamMsg inmsg(&recv_msg);
		inmsg.Read(roleid);

		if (roleid == 0) {
			inmsg.Read(error_code);
			if (error_code) {
				nd_logerror("create role list error : %d\n", error_code);
			}
			return -1;
		}
		else {
			inmsg.Read(name, sizeof(name));

			char roleName[128];
			nd_utf8_to_gbk((const char*)name, roleName, sizeof(roleName));

			nd_logerror("create role %s success \n", roleName);
			//read attribute
			NDUINT16 num = 0;
			if (0 == inmsg.Read(num)) {
				for (int i = 0; i < num; ++i) {
					NDUINT8 aid;
					float val;
					inmsg.Read(aid); inmsg.Read(val);
					nd_logmsg("create role attribute id = %d val =%f \n", aid, val);
				}
			}
		}
	}
	return 0;
}
