//
//  login_atlantis.cpp
//  clientDemo
//
//  Created by duanxiuyun on 14-11-17.
//  Copyright (c) 2014 duanxiuyun. All rights reserved.
//

#include "nd_common/nd_common.h"
#include "cli_common/login_apollo.h"
#include "msg_def.h"
//#include "apollo_data.h"
//#include "srv_define.h"
#include "ndcli/nd_api_c.h"
#include "apollo_errors.h"

//#define _DFT_SESSION_INFO_FILE_NAME "./session.tmp"

#if ND_DEBUG 
#define NET_MSG_TIMEOUT (1000 * 600)
#else
#define NET_MSG_TIMEOUT WAITMSG_TIMEOUT

#endif


struct login_session_load{
	NDUINT32 acc_index ;
	NDUINT32 session_size ;
	char session_buf[1024];
	
	char keymd5[16] ;
	R_RSA_PUBLIC_KEY srv_key;
};


struct rsa_key_from_srv{
	char keymd5[16] ;
	R_RSA_PUBLIC_KEY pub_key ;
}   ;


//static account_index_t __g_login_account =0;

static int save_session_info(R_RSA_PUBLIC_KEY *key, login_token_info *session_info ,const char*keymd5, const char *file_name ) ;
static int load_session_info(login_session_load *s_buf, const char * file_name);

static int  session_to_stream(R_RSA_PUBLIC_KEY *key, login_token_info *session_info, const char*keymd5, char *stream_buf, size_t buf_size);
static int stream_to_session(char *stream_buf, size_t size, login_session_load *s_buf);


#define SEND_AND_WAIT(_conn, _omsg, _rmsg_buf,_wait_maxid, _wait_minid,_sendflag) \
if (0 != ndSendAndWaitMessage(_conn, _omsg.GetMsgAddr(), _rmsg_buf, _wait_maxid, _wait_minid, _sendflag,NET_MSG_TIMEOUT)) {	\
		nd_logerror("send and wait data error code =%d\n", nd_object_lasterror(_conn)); \
		return -1;		\
}


//show error
const char *apollo_error(int errcode)
{
	static char errdesc[128] ;
	
	const char *nd_err[] = {
		"UNKNOW_ERROR"
#undef ErrorElement 
#define ErrorElement(a,_err_desc) "game(ESERVER_"#a "):" _err_desc
#include "_error_def.h"
#undef ErrorElement 
	} ;
			
	if (errcode <= NDERR_UNKNOWN) {
		
		nd_error_convert  oldFunc = nd_register_error_convert(NULL);
		const char *perr = nd_error_desc(errcode);
		nd_register_error_convert(oldFunc);
		return perr;
	}
	else if (errcode > ApolloError_Start && errcode < ESERVER_ERR_NUMBER){
		return nd_err[errcode - ApolloError_Start-1];
	}
	else {
		snprintf(errdesc, sizeof(errdesc), "Error code =%d",errcode) ;
		return errdesc ;
	}
}


#ifdef BUILD_AS_THIRD_PARTY
LoginBase *ApolloCreateLoginInst()
{
	nd_register_error_convert(apollo_error);
	return new LoginApollo();
}
void ApolloDestroyLoginInst(LoginBase *pLogin)
{
	delete pLogin;
}
#else 
LoginApollo *ApolloCreateLoginInst()
{
	nd_register_error_convert(apollo_error);
	return new LoginApollo();
}
void ApolloDestroyLoginInst(LoginApollo *pLogin)
{
	delete pLogin;
}

#endif

NDUINT32 LoginApollo::getAccountID() 
{
	return  m_accIndex ;
	//return __g_login_account ;
}

const char *LoginApollo::getAccountName()
{
	return (const char*)m_accName ;
}

LoginApollo::LoginApollo(nd_handle hConn, const char * session_filename, const char*udid ) : m_conn(hConn)
{
	m_accIndex = 0 ;
	m_sessionID = 0 ;
	
	m_accType =0 ;
	m_accName[0] =0 ;

	memset(&m_srv_key, 0, sizeof(m_srv_key)) ;
	m_session_file = 0 ;
	
	ReInit( hConn, session_filename, udid);	
	
}

LoginApollo::LoginApollo()
{
	m_conn = NULL;
	m_accIndex = 0;
	m_sessionID = 0;

	m_accType = 0;
	m_accName[0] = 0;

	//m_nickName[0]=0 ;
	memset(&m_srv_key, 0, sizeof(m_srv_key));
	m_session_file = 0;


	strncpy((char*)m_udid, "unknown", sizeof(m_udid));
}

LoginApollo::~LoginApollo()
{
	memset(&m_srv_key, 0, sizeof(m_srv_key)) ;
	if(m_session_file) {
		free(m_session_file) ;
		m_session_file = NULL ;
	}
}


void LoginApollo::SetUdid(const char *udid)
{
	if (udid && udid[0]) {
		strncpy((char*)m_udid, udid, sizeof(m_udid))  ;
	}
	else {
		m_udid[0] = 0;
	}
}

void LoginApollo::ReInit(nd_handle hConn, const char * session_filename, const char*udid )
{
	Destroy();

	m_conn = hConn;		
	SetSessionFile(session_filename);
	SetUdid(udid);

}

void LoginApollo::SetSessionFile(const char *session_filepath)
{
	if (session_filepath && *session_filepath){
		size_t size = strlen(session_filepath) + 1;
		if (size > 1024) {
			size = 1024;
		}
		m_session_file = (char*)malloc(size + 1);
		strncpy(m_session_file, session_filepath, size);
	}
	else {
		m_session_file = NULL;
	}
}
size_t LoginApollo::GetSessionSize()
{
	char buf[4096];
	return GetSessionData(buf, sizeof(buf));
}

size_t LoginApollo::GetSessionData(void *out_buf, size_t buf_size)
{
	login_token_info sessionifo;
	sessionifo.acc_index = m_accIndex;
	sessionifo.session_key = m_sessionID;
	sessionifo._reserved = 0;
	int size = sizeof(sessionifo.sym_key);

	time(&sessionifo.create_tm);

	nd_net_ioctl((nd_netui_handle)m_conn, NDIOCTL_GET_CRYPT_KEY, &sessionifo.sym_key, &size);

	rsa_key_from_srv *rsk_md5 = (rsa_key_from_srv *)m_srv_key;

	int ret = session_to_stream(&rsk_md5->pub_key, &sessionifo, rsk_md5->keymd5, (char*)out_buf, buf_size);
	if (-1 == ret ) {
		nd_logerror("convert session to stream error\n");
		return 0;
	}

	return ret;
}
int LoginApollo::ReloginEx(void *session_data, size_t size)
{
	login_session_load session_saved = { 0 };
	
	if (-1 == stream_to_session((char*)session_data, size, &session_saved)){
		nd_logerror("convert stream to session error\n");
		return -1;
	}

	int ret = checkCryptVersion(session_saved.keymd5);
	if (0 != ret) {
		nd_rmfile(m_session_file);
		nd_logmsg("checkCryptVersion Error: %s \n", nd_object_errordesc(m_conn));

		return ret;
	}
	memcpy(&m_srv_key, session_saved.keymd5, sizeof(rsa_key_from_srv));

	if (0 == LoginApollo::relogin(&session_saved, LOGIN_MSG_RELOGIN_REQ, LOGIN_MSG_LOGIN_ACK)) {

		nd_logmsg("RE-login SUCCESS\n");
		return 0;
	}
	else {
		nd_logmsg("RE-login ERROR\n");
		if (ndGetLastError(m_conn) == NDSYS_ERR_USERNAME) {
			return 1;
		}
	}
	return -1;

}


void LoginApollo::Destroy()
{
	m_conn = 0;
	m_accIndex =0 ;
	m_sessionID = 0;
	
	m_accType = 0;
	m_accName[0]=0;
	m_udid[0]=0;

	memset(&m_srv_key, 0, sizeof(m_srv_key));
	if (m_session_file) {
		free(m_session_file);
		m_session_file = NULL;
	}
}

int LoginApollo::Login(const char *userName, const char *passwd, ACCOUNT_TYPE type)
{

	if (TrytoGetCryptKey() == -1) {
		return -1;
	}

	// send login message
	NDOStreamMsg omsg(ND_MAIN_ID_LOGIN,LOGIN_MSG_LOGIN_REQ) ;
	omsg.Write(m_udid) ;
	omsg.Write((NDUINT8)type) ;
	omsg.Write((NDUINT8*)userName) ;
	if (ACC_APOLLO==type && passwd && passwd[0]) {
		omsg.Write((NDUINT8*)passwd) ;
	}

	nd_usermsgbuf_t recv_msg ;

	SEND_AND_WAIT(m_conn, omsg, &recv_msg, ND_MAIN_ID_LOGIN,LOGIN_MSG_LOGIN_ACK,ESF_ENCRYPT|ESF_URGENCY)
	else {
		// login success
		NDIStreamMsg inmsg(&recv_msg) ;
		if(-1==onLogin(inmsg) ) {
			return -1 ;
		}
	}
	nd_logmsg("login SUCCESS \n") ;
	return 0;
}

int LoginApollo::Relogin()
{
	login_session_load session_saved = {0};
	if (!m_session_file || !nd_existfile(m_session_file) ) {
		nd_logdebug("session file not exist\n");
		return 1 ;
	}
	if(-1==load_session_info( &session_saved, m_session_file) ) {
		nd_rmfile(m_session_file);
		nd_logmsg("load session from %s file error\n", m_session_file);
		return 1 ;
	}
	
	int ret =checkCryptVersion(session_saved.keymd5);
	if ( 0!=ret ) {		
        nd_rmfile(m_session_file) ;
        nd_logmsg("checkCryptVersion Error: %s \n", nd_object_errordesc(m_conn)) ;
        
		return ret ;
	}
	memcpy(&m_srv_key, session_saved.keymd5, sizeof(rsa_key_from_srv)) ;
	
	if(0==LoginApollo::relogin(&session_saved, LOGIN_MSG_RELOGIN_REQ, LOGIN_MSG_LOGIN_ACK) ) {
		
		nd_logmsg("RE-login SUCCESS\n") ;
		return 0 ;	
	}
	else {		
		nd_logmsg("RE-login ERROR\n") ;	
		if (ndGetLastError(m_conn) == NDSYS_ERR_USERNAME) {
			nd_rmfile(m_session_file) ;
			return 1;
		}
	}
	return -1;
}


int LoginApollo::Logout() 
{
	if (nd_connect_level_get(m_conn) < EPL_LOGIN)  {
		nd_object_seterror(m_conn, NDSYS_ERR_NEED_LOGIN);
		nd_connector_set_crypt(m_conn, NULL, 0);
		return -1;
	}
	
	if (nd_connector_valid((nd_netui_handle)m_conn)) {
		NDOStreamMsg omsg(NETMSG_MAX_LOGIN, LOGIN_MSG_LOGOUT_REQ);
		nd_usermsgbuf_t recv_msg;
		if (0 != ndSendAndWaitMessage(m_conn, omsg.GetMsgAddr(), &recv_msg, NETMSG_MAX_LOGIN, LOGIN_MSG_LOGOUT_NTF, ESF_URGENCY, 2000)) {
			nd_object_seterror(m_conn, NDERR_IO);
		}
	}
	
	nd_connect_level_set(m_conn, EPL_CONNECT) ;	
	nd_connector_set_crypt(m_conn, NULL, 0);

	return 0;
}

int LoginApollo::CreateAccount(account_base_info *acc_info)
{
	if (TrytoGetCryptKey() ==-1) {
		return -1;
	}

	NDOStreamMsg omsg(NETMSG_MAX_LOGIN, LOGIN_MSG_CREATE_REQ) ;
	nd_usermsgbuf_t recv_msg ;

	omsg.Write(m_udid) ;
	omsg.Write(acc_info->type) ;
	omsg.Write(acc_info->gender) ;
	
	omsg.Write(acc_info->birth_year) ;
	omsg.Write(acc_info->birth_month) ;
	omsg.Write(acc_info->birth_day) ;
	
	omsg.Write(acc_info->acc_name) ;
	omsg.Write(acc_info->nick) ;
	omsg.Write(acc_info->passwd) ;
	omsg.Write(acc_info->phone) ;
	omsg.Write(acc_info->email) ;

	SEND_AND_WAIT(m_conn, omsg, &recv_msg, NETMSG_MAX_LOGIN, LOGIN_MSG_CREATE_ACK,ESF_ENCRYPT|ESF_URGENCY)
	else {
		NDIStreamMsg inmsg(&recv_msg) ;
		
		if(-1==onLogin(inmsg) ) {
			return -1 ;
		}
	}
	return 0;
}

int LoginApollo::GetServerList(ApolloServerInfo *buf, int size)
{
	
	NDOStreamMsg omsg(NETMSG_MAX_LOGIN, LOGIN_MSG_GET_HOST_LIST) ;
	nd_usermsgbuf_t recv_msg ;
	
	SEND_AND_WAIT(m_conn, omsg, &recv_msg, NETMSG_MAX_LOGIN, LOGIN_MSG_GET_HOST_LIST,ESF_URGENCY)
	else {
		NDUINT16 num =0 ;
		NDIStreamMsg inmsg(&recv_msg) ;
		if (0!=inmsg.Read(num) || num==0) {
			return 0 ;
		}
		
		for (int i=0; i< num; i++) {
			
			host_list_node *it = & (buf[i].host) ;
			//inmsg.Read((NDUINT32&)it->ip) ;
			inmsg.Read(buf[i].ip_addr,sizeof(buf[i].ip_addr) ) ;
			inmsg.Read(it->port) ;
			
			inmsg.Read(it->max_number) ;
			inmsg.Read(it->cur_number) ;
			inmsg.Read(it->version_id) ;
			inmsg.Read(it->name,sizeof(it->name)) ;
			it->ip = nd_inet_aton(buf[i].ip_addr);
		}
		return (int) num ;
	}
	return  0;
	
}


int LoginApollo::ReadyGame() 
{
	if (nd_connect_level_get(m_conn) < EPL_LOGIN)  {
		nd_object_seterror(m_conn, NDSYS_ERR_NEED_LOGIN) ;
		return -1;
	}
	nd_connect_level_set(m_conn, EPL_READY) ;
	
	
	return  0;
	
//	if (nd_connect_level_get(m_conn) < EPL_LOGIN)  {
//		nd_object_seterror(m_conn, ESERVER_ERR_NEED_LOGIN) ;
//		return -1;
//	}
//	
//	NDOStreamMsg omsg(NETMSG_MAX_LOGIN, LOGIN_MSG_READY_GAME_REQ) ;
//
//	if(nd_connector_send(m_conn, (nd_packhdr_t*) (omsg.GetMsgAddr()), ESF_URGENCY) <= 0 ) {
//		return -1 ;
//	}
	//return 0;
}

int LoginApollo::GetLastError() {return ndGetLastError(m_conn) ;}


int LoginApollo::EnterServer(ndip_t ip, NDUINT16 port)
{
	return switchServer( ip,  port,LOGIN_MSG_SELECT_SERVER_REQ, LOGIN_MSG_SELECT_SERVER_ACK) ;
}

int LoginApollo::EnterServer(const char *host_name, NDUINT16 port)
{
	return switchServer( host_name,  port,LOGIN_MSG_SELECT_SERVER_REQ, LOGIN_MSG_SELECT_SERVER_ACK) ;
}

int LoginApollo::EnterServer(const char *host_name, NDUINT16 port,const char *session_file)
{
	login_session_load session_saved = {0};
	if (!session_file || !nd_existfile(session_file) ) {
		return -1 ;
	}
	if(-1==load_session_info( &session_saved, session_file) ) {
		nd_logdebug("load session file %s error \n"  AND  session_file);
		return -1 ;
	}
	
	//connect to new server
	if(0!=nd_reconnectex(m_conn, host_name,  port, NULL)  ) {
		nd_object_seterror(m_conn, NDSYS_ERR_HOST_UNAVAILABLE) ;
		nd_logmsg("connect server %s:%d error  \n", host_name,  port) ;
		return -1;
	}
	
	memcpy(&m_srv_key, session_saved.keymd5, sizeof(rsa_key_from_srv)) ;
	
	//SEND session info to server and check
	if(0==LoginApollo::relogin(&session_saved, LOGIN_MSG_SELECT_SERVER_REQ, LOGIN_MSG_SELECT_SERVER_ACK) ) {
		nd_logmsg("Redirect server SUCCESS\n") ;
		return 0 ;
	}
	else {
		nd_logmsg("Redirect server ERROR\n") ;
		return -1;
	}
	
}


int LoginApollo::jumptoGame(NDUINT64 serverid)
{
	char buf[128];
	NDOStreamMsg msgClose(NETMSG_MAX_LOGIN, LOGIN_MSG_JUMPTO_SERVER_NTF) ;
	nd_connector_send(m_conn, (nd_packhdr_t*) (msgClose.GetMsgAddr()), ESF_URGENCY) ;
	
	if(0!=nd_reconnect(m_conn, HOST_ID_GET_IP(serverid), HOST_ID_GET_PORT(serverid), NULL)  ) {
		nd_object_seterror(m_conn, NDSYS_ERR_HOST_UNAVAILABLE) ;
		
		nd_logerror("re-connect to %s : %d error\n",nd_inet_ntoa( HOST_ID_GET_IP(serverid),buf), HOST_ID_GET_PORT(serverid));
		return -1;
	}
	memcpy(buf, &m_sessionID, sizeof(m_sessionID)) ;
	
	tea_k cryptkey ;
	int size = sizeof(cryptkey) ;
	nd_net_ioctl((nd_netui_handle)m_conn, NDIOCTL_GET_CRYPT_KEY, &cryptkey, &size) ;
	
	int len = nd_TEAencrypt((unsigned char*)buf,size, &cryptkey) ;
	NDOStreamMsg omsg(NETMSG_MAX_LOGIN, LOGIN_MSG_JUMPTO_SERVER_REQ) ;
	
	omsg.Write((NDUINT32)m_accIndex);
	omsg.WriteBin(buf, len) ;
	
	nd_usermsgbuf_t recv_msg ;
	
	SEND_AND_WAIT(m_conn, omsg, &recv_msg, NETMSG_MAX_LOGIN, LOGIN_MSG_JUMPTO_SERVER_ACK,ESF_URGENCY)
	else {
		
		int size = sizeof(cryptkey) ;
		nd_net_ioctl((nd_netui_handle)m_conn, NDIOCTL_SET_CRYPT_KEY, &cryptkey, &size) ;
		
		return 0;
	}
	
	return -1;
}

int LoginApollo::switchServer(ndip_t ip, NDUINT16 port,int sendMsg, int waitMsg)
{
	login_session_load session_saved = {0};
	if(-1==getReloginSessionInfo( &session_saved) ) {
		return -1 ;
	}
	
	
	if(0!=nd_reconnect(m_conn,  ip,  port, NULL)  ) {		
		nd_object_seterror(m_conn, NDSYS_ERR_HOST_UNAVAILABLE) ;
		return -1;
	}
	
	//SEND session info to server and check	
	if(0==LoginApollo::relogin(&session_saved, sendMsg, waitMsg) ) {
		nd_logmsg("SWITCH server SUCCESS\n") ;
		return 0 ;	
	}
	else {
		nd_logmsg("SWITCH server ERROR\n") ;		
	}
	
	nd_logmsg("SWITCH server SUCCESS\n") ;
	return 0;
}

int LoginApollo::switchServer(const char *host, NDUINT16 port,int sendMsg, int waitMsg)
{
	login_session_load session_saved = {0};
	if(-1==getReloginSessionInfo( &session_saved) ) {
		return -1 ;
	}
	
	
	if(0!=nd_reconnectex(m_conn,  host,  port, NULL)  ) {
		nd_object_seterror(m_conn, NDSYS_ERR_HOST_UNAVAILABLE) ;
		return -1;
	}
	
	//SEND session info to server and check
	if(0==LoginApollo::relogin(&session_saved, sendMsg, waitMsg) ) {
		nd_logmsg("SWITCH server SUCCESS\n") ;
		return 0 ;
	}
	else {
		nd_logmsg("SWITCH server ERROR\n") ;
	}
	
	nd_logmsg("SWITCH server SUCCESS\n") ;
	return 0;
}


int LoginApollo::relogin(void *token_info, int sendMsgID, int waitMsgID)
{
	login_session_load *saveSession = (login_session_load *) token_info ;
	transfer_session_key trans_key ;
	
	
	tea_k k ;
	
	tea_key(&k) ;
	nd_teaKeyToNetorder(&trans_key.new_key, &k);
	
	//init transfer session key
	trans_key.acc_index =saveSession->acc_index ;
	memcpy(trans_key.session_buf, saveSession->session_buf, saveSession->session_size) ;
	trans_key.size = saveSession->session_size ;
	
	size_t org_size = sizeof(transfer_session_key) - sizeof(trans_key.session_buf) + trans_key.size ;
	
	
	//SEND session info to server and check
	// format : ras_public_crypt(accindex + size + TEA(login_token_info) + teakey)
	NDOStreamMsg omsg(NETMSG_MAX_LOGIN,sendMsgID/*LOGIN_MSG_RELOGIN_REQ*/) ;
	
	char buf[4096] ;
	int size = sizeof(buf) ;
	
	if (0!=rsa_pub_encrypt((char*)&buf, &size,(char*)&trans_key, (int)org_size, &saveSession->srv_key)) {
		nd_logerror("rsa encrypt symm-key error\n");
		return 1;
	}
	
	omsg.WriteBin(buf, size) ;
	nd_log_screen("send relogin data len =%d\n", size) ;
	
	
	size = sizeof(k) ;
	nd_net_ioctl((nd_netui_handle)m_conn,  NDIOCTL_SET_CRYPT_KEY,&k, &size) ;
	
	//nd_log_screen("set new crypt key = { %x, %x, %x, %x} \n", k.k[0],k.k[1],k.k[2],k.k[3] ) ;	
	
	nd_usermsgbuf_t recv_msg ;
	
	/*LOGIN_MSG_LOGIN_ACK*/
	SEND_AND_WAIT(m_conn, omsg, &recv_msg, NETMSG_MAX_LOGIN,waitMsgID,ESF_URGENCY)
	else {
		// login success
		NDIStreamMsg inmsg(&recv_msg) ;
		
		if(-1==onLogin(inmsg) ) {
			return -1 ;
		}
	}
	
	return 0;
}


int LoginApollo::getReloginSessionInfo(void *tokenBuf)
{
	int size ;
	login_session_load *outbuf = (login_session_load *)tokenBuf ;
	
	outbuf->acc_index = m_accIndex ;
	
	login_token_info sessionifo ;
	sessionifo.acc_index = m_accIndex ;
	sessionifo.session_key = m_sessionID ;
	
	size = sizeof(sessionifo.sym_key) ;
	nd_net_ioctl((nd_netui_handle)m_conn, NDIOCTL_GET_CRYPT_KEY, &sessionifo.sym_key, &size) ;
	
	memcpy(outbuf->session_buf, &sessionifo, sizeof(sessionifo)) ;

	memcpy(outbuf->keymd5, &m_srv_key, sizeof(rsa_key_from_srv));
	
	outbuf->session_size = nd_TEAencrypt((unsigned char*)outbuf->session_buf, sizeof(sessionifo), &sessionifo.sym_key) ;
	
	return  0;
}

int LoginApollo::onLogin(NDIStreamMsg &inmsg)
{
	//read account info
	if (-1==inmsg.Read(m_sessionID) || m_sessionID==0) {
		NDUINT32 errcode = NDERR_VERSION;
		inmsg.Read(errcode) ;
		nd_object_seterror(m_conn, errcode) ;
		return -1 ;
	}
	if (-1==inmsg.Read((NDUINT32&)m_accIndex) || m_accIndex==0) {
		
		nd_object_seterror(m_conn, NDSYS_ERR_USERNAME) ;
		return -1 ;
	}
	inmsg.Read(m_accType) ;
	inmsg.Read(m_accName,sizeof(m_accName)) ;
	
	
	//jump to new game server
	NDUINT64 redirectHost = 0 ;
	if (0==inmsg.Read(redirectHost) && redirectHost!=0) {
		if (inmsg.MsgMinid() == LOGIN_MSG_SELECT_SERVER_ACK ) {
			if (-1==jumptoGame(redirectHost)) {
				return -1 ;
			}
		}
	}

	int sessionid = m_sessionID ;
	int size = sizeof(int) ;
	nd_net_ioctl((nd_netui_handle)m_conn, NDIOCTL_SET_SESSIONID,&sessionid, &size) ;

	
	int sessionid_test = 0 ;
	size = sizeof(int) ;
	nd_net_ioctl((nd_netui_handle)m_conn, NDIOCTL_GET_SESSIONID,&sessionid_test, &size) ;
	nd_assert(sessionid_test==sessionid) ;
	
	nd_connect_level_set(m_conn, EPL_LOGIN) ;

	//__g_login_account =  m_accIndex ;
	
	//nd_logdebug("login ack accound-id=%d session_id =%d  connector=%p \n" AND m_accIndex  AND  m_sessionID AND  m_conn);

	if ( m_session_file && m_session_file[0]) {		
		login_token_info sessionifo ;
		sessionifo.acc_index = m_accIndex ;
		sessionifo.session_key = m_sessionID ;
		sessionifo._reserved = 0 ;
		size = sizeof(sessionifo.sym_key) ;
		
		time(&sessionifo.create_tm);
		
		nd_net_ioctl((nd_netui_handle)m_conn, NDIOCTL_GET_CRYPT_KEY, &sessionifo.sym_key, &size) ;
		
		rsa_key_from_srv *rsk_md5 = (rsa_key_from_srv *) m_srv_key ;
		if(-1==save_session_info(&rsk_md5->pub_key, &sessionifo , rsk_md5->keymd5 ,m_session_file ) ) {
			nd_logerror("save session ifo error") ;
		}
#if 0
		else {
			login_session_load session_load = {0};
			
			if(-1==load_session_info( &session_load, m_session_file) ) {
				nd_logerror("save session ifo error") ;
				return -1 ;
			}
			
			int size  ;
			char md5[16] ;
			char md5Old[16] ;
			char buf[4096] ;
			
			//memcpy(buf, &sessionifo, sizeof(sessionifo)) ;
			
			tea_k backKey = sessionifo.sym_key ;
			nd_teaKeyToHostorder(&sessionifo.sym_key, &backKey);
			memcpy(buf, &sessionifo, sizeof(sessionifo)) ;
			sessionifo.sym_key = backKey ;
			
			size = nd_TEAencrypt((unsigned char*)buf, sizeof(sessionifo), &sessionifo.sym_key) ;
			MD5Crypt16(buf, size, md5) ;
			MD5Crypt16(session_load.session_buf, session_load.session_size, md5Old) ;
			
			int val = MD5cmp(md5, md5Old) ;
			fprintf(stdout, "save-load file %s\n", val ? "failed" : "success") ;

		}
#endif
		
	}
	
	return  0 ;
}

int LoginApollo::TrytoGetCryptKey()
{
	if (!nd_connector_check_crypt(m_conn)) {
		int ret = nd_exchange_key((netObject)m_conn, (void*)&m_srv_key);
		if (-1 == ret) {
			nd_logerror("exchange key error %s\n", ndGetLastErrorDesc(m_conn));
		}
		return ret;
	}
	return 0;
}

int LoginApollo::checkCryptVersion(char *savedVersion) 
{
	// send login message
	NDOStreamMsg omsg(ND_MAIN_ID_SYS, ND_MSG_SYS_GET_PUBLIC_KEY_MD5) ;
	nd_usermsgbuf_t recv_msg ;
	
	SEND_AND_WAIT(m_conn, omsg, &recv_msg, ND_MAIN_ID_SYS, ND_MSG_SYS_GET_PUBLIC_KEY_MD5,ESF_URGENCY)
	else {
		char srv_md5[16] ;
		NDIStreamMsg inmsg(&recv_msg) ;
		if (16!=inmsg.ReadBin(srv_md5, sizeof(srv_md5))) {
			nd_object_seterror(m_conn,NDERR_BADPACKET) ;
			return 1 ;
		}
		if (MD5cmp(srv_md5, savedVersion)) {
			nd_object_seterror(m_conn,NDERR_VERSION) ;
			return -1 ;
		}		
	}
	return 0;

}

int session_to_stream(R_RSA_PUBLIC_KEY *key, login_token_info *session_info, const char*keymd5, char *stream_buf, size_t buf_size)
{
	char *p = stream_buf;
	int size;
	char md5[16];
	char buf[4096];

	tea_k backKey = session_info->sym_key;
	nd_teaKeyToNetorder(&session_info->sym_key, &backKey);
	memcpy(buf, session_info, sizeof(*session_info));
	session_info->sym_key = backKey;

	size = nd_TEAencrypt((unsigned char*)buf, sizeof(*session_info), &session_info->sym_key);
	MD5Crypt16(buf, size, md5);


#define WRITE_TOFILE(_data,_size)  do {\
	memcpy(p, _data, _size) ;			\
	p += _size ;						\
}while(0) 

	//save md5
	WRITE_TOFILE(md5, sizeof(md5));
	WRITE_TOFILE(&session_info->acc_index, sizeof(account_index_t));
	WRITE_TOFILE(&size, sizeof(size));
	WRITE_TOFILE(buf, size);
	WRITE_TOFILE(keymd5, 16);

	//save public key
	size = nd_rsa_write_key((R_RSA_PRIVATE_KEY*)key, buf, sizeof(buf), 0);
	if (size <= 0) {
		nd_logerror("write rsa_key to file error\n");
		return -1;
	}

	WRITE_TOFILE(&size, sizeof(size));
	WRITE_TOFILE(buf, size);

	return (int)(p - stream_buf);
#undef WRITE_TOFILE
}

int stream_to_session(char *stream_buf, size_t buf_size, login_session_load *s_buf)
{
	char *p = stream_buf;
	char md5[16], calcmd5[16];
	int size = 0;

#define READ_FROMFILE(_buf, _buf_size) do { \
			memcpy(_buf, p, _buf_size) ; \
			p+=_buf_size ;				\
		}while(0)

	//read md5
	READ_FROMFILE(md5, sizeof(md5));
	//read account index
	READ_FROMFILE(&s_buf->acc_index, sizeof(account_index_t));
	//read session_info
	READ_FROMFILE(&size, sizeof(size));

	if (size > sizeof(s_buf->session_buf)) {
		nd_logerror("file session size biger than 1024\n");
		return -1;
	}
	s_buf->session_size = size;

	READ_FROMFILE(s_buf->session_buf, size);

	//cmp md5
	MD5Crypt16(s_buf->session_buf, size, calcmd5);
	if (MD5cmp(md5, calcmd5)) {
		nd_logerror("file md5 check error\n");
		return -1;
	}

	//read public_key
	char buf[4096];
	READ_FROMFILE(s_buf->keymd5, sizeof(s_buf->keymd5));

	//read session_info	
	READ_FROMFILE(&size, sizeof(size));
	if (size > sizeof(buf)) {
		nd_logerror("read buff size error\n");
		return -1;
	}

	READ_FROMFILE(buf, size);

	int ret = nd_rsa_read_key((R_RSA_PRIVATE_KEY*)&s_buf->srv_key, buf, size, 0);
	if (ret != 0){
		nd_logerror("read rsa key from file error\n");
	}
	return ret;

#undef READ_FROMFILE
}

#include "crypt_file.h"

int save_session_info(R_RSA_PUBLIC_KEY *key, login_token_info *session_info , const char*keymd5, const char *file_name )
{
	int size  ;
	char md5[16] ;
	char buf[4096] ;

	tea_k backKey = session_info->sym_key ;
	nd_teaKeyToNetorder(&session_info->sym_key, &backKey);
	memcpy(buf, session_info, sizeof(*session_info)) ;
	session_info->sym_key = backKey ;
	
	size = nd_TEAencrypt((unsigned char*)buf, sizeof(*session_info), &session_info->sym_key) ;
	MD5Crypt16(buf, size, md5) ;
#if 0
	do {
		char tmpbuf[128] ;
		
		tea_k &k = session_info->sym_key ;
		ndprintf("sym-key = { %x, %x, %x, %x} \n", k.k[0],k.k[1],k.k[2],k.k[3] ) ;
		
		nd_logdebug("login success sessionid= %d account_index = %d md5=%s \n", 
				session_info->session_key,
				session_info->acc_index,
				MD5ToString((unsigned char*)md5, (unsigned char*)tmpbuf) ) ;
		
		if (size > 0) {
			fprintf(stdout, "SAVE session key size= %d\n", (int)size) ;
			for (int i=0; i<size; i++) {
				NDUINT8 a = buf[i] ;
				fprintf(stdout, " %x", a) ;
				if ((i & 15) == 15) {
					fprintf(stdout, "\n") ;
				}
			}
			fprintf(stdout, "\n=========end token buf============\n") ;
		}
		
	}while (0);
#endif
	

	FILE *pf = fopen(file_name, "w" ) ;
	if (!pf) {
		nd_logerror("open file %s error:%s \n", file_name, nd_last_error());
		return -1 ;
	}
#define WRITE_TOFILE(_data,_size)  do {\
	if(fwrite(_data, _size, 1, pf) <=0 ) {	\
		fclose(pf) ;					\
		nd_logerror("write %d data to file %s error:%s \n",(int)_size, file_name, nd_last_error());\
		return -1 ;						\
	}	\
}while(0) 
	
	//save md5
	WRITE_TOFILE(md5, sizeof(md5)) ;
	WRITE_TOFILE(&session_info->acc_index, sizeof(account_index_t)) ;
	WRITE_TOFILE(&size, sizeof(size)) ;	
	WRITE_TOFILE(buf, size);
	WRITE_TOFILE(keymd5, 16);

	//save public key
	size = nd_rsa_write_key((R_RSA_PRIVATE_KEY*)key, buf, sizeof(buf), 0) ;
	if (size <= 0) {
		fclose(pf) ;
		nd_logerror("write rsa_key to file error\n");
		return -1 ;
	}
	
	WRITE_TOFILE(&size, sizeof(size));		
	WRITE_TOFILE(buf, size);

	fclose(pf) ;
	return 0 ;
	
#undef WRITE_TOFILE
}

int load_session_info(login_session_load *s_buf, const char * file_name)
{
	char md5[16],calcmd5[16] ;
	int size = 0;
	FILE *pf = fopen(file_name, "r" ) ;
	if (!pf) {
		nd_logerror("open file %s error: %s\n", file_name, nd_last_error());
		return -1 ;
	}
#define READ_FROMFILE(_buf, _buf_size) do { \
	if(fread(_buf, _buf_size, 1, pf) <= 0) {	\
		nd_logerror("read data from session file error: %s\n", nd_last_error()); \
		fclose(pf) ;		\
		return -1 ;			\
	}						\
}while(0) 
	
	//read md5
	READ_FROMFILE(md5, sizeof(md5)) ;
	//read account index
	READ_FROMFILE(&s_buf->acc_index, sizeof(account_index_t)) ;
	//read session_info
	READ_FROMFILE(&size, sizeof(size)) ;
	
	if (size > sizeof(s_buf->session_buf)) {
		fclose(pf) ;
		nd_logerror("file session size biger than 1024\n");
		return -1;
	}
	s_buf->session_size = size ;
	
	READ_FROMFILE(s_buf->session_buf, size) ;

	//cmp md5
	MD5Crypt16(s_buf->session_buf, size,  calcmd5) ;
	if (MD5cmp(md5,calcmd5)) {
		fclose(pf);
		nd_logerror("file md5 check error\n");
		return -1 ;
	}

	//read public_key
	char buf[4096] ;	
	READ_FROMFILE(s_buf->keymd5, sizeof(s_buf->keymd5)) ;

	//read session_info	
	READ_FROMFILE(&size, sizeof(size)) ;
	if (size > sizeof(buf)) {
		fclose(pf);
		nd_logerror("read buff size error\n");
		return -1;
	}
	
	READ_FROMFILE(buf,size) ;
	
	fclose(pf) ;
	
#if 0
	if (s_buf->session_size > 0) {
		fprintf(stdout, "load session key  size= %d\n", (int)s_buf->session_size) ;
		for (int i=0; i<s_buf->session_size; i++) {
			NDUINT8 a = s_buf->session_buf[i] ;
			fprintf(stdout, " %x", a) ;
			if ((i & 15) == 15) {
				fprintf(stdout, "\n") ;
			}
		}
		fprintf(stdout, "\n=========end token buf============\n") ;
	}
#endif
	int ret = nd_rsa_read_key((R_RSA_PRIVATE_KEY*)&s_buf->srv_key, buf, size, 0) ;
	if (ret != 0){
		nd_logerror("read rsa key from file error\n");
	}
	return ret;

#undef READ_FROMFILE
}


