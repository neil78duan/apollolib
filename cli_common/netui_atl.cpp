//
//  netui_atl.cpp
//  clientDemo
//
//  Created by duanxiuyun on 14-12-2.
//  Copyright (c) 2014 duanxiuyun. All rights reserved.
//

#include "nd_common/nd_common.h"
#include "cli_common/login_apollo.h"
#include "apollo_errors.h"
#include "cli_common/netui_atl.h"
#include "ndcli/nd_api_c.h"
#include "msg_def.h"


void initAccCreateInfo(account_base_info &acc,int accType,const char *userName, const char *passwd)
{
	acc.type = accType ;
	acc.gender = 0 ; //default F
	acc.birth_day = 8 ;
	acc.birth_month = 8 ;
	acc.birth_year = 1988 ;
	strncpy((char*) acc.acc_name, userName, sizeof(acc.acc_name)) ;
	strncpy((char*) acc.nick, userName,sizeof(acc.nick)) ;
	strncpy((char*) acc.passwd, passwd,sizeof(acc.passwd)) ;
	strncpy((char*) acc.phone, "100086",sizeof(acc.phone)) ;
	strncpy((char*) acc.email, "webmaster@qq.com", sizeof(acc.email)) ;
	
}

int RloginTrylogin(const char*udid,nd_handle h,int accType, const char *userName, const char *passwd,const char *save_session_file, NDUINT32 *accID)
{
	int ret = 0;
	LoginApollo login((nd_handle)h,true, save_session_file) ;
	
	login.InitUdid(udid) ;
	
	ret = login.Relogin() ;

	if (0==ret) {
		nd_logmsg("login complete SUCCESS!\n") ;
		if (accID) {
			*accID = login.getAccountID() ;
		}
		return  0 ;
	}

	//need input use-name and password
	ret = login.Login(userName, passwd, (ACCOUNT_TYPE)accType) ;
	if (-1==ret ) {
		//Create account
		if ( ndGetLastError(h)==ESERVER_ERR_NOUSER) {
			account_base_info acc ;
			initAccCreateInfo(acc, accType,userName, passwd);

			ret = login.CreateAccount(&acc) ;
		}	
	}
	
	
	if (ret == 0 ) {		
		nd_logmsg("login complete SUCCESS!\n") ;
		
		if (accID) {
			*accID = login.getAccountID() ;
		}
	}
	else {
		nd_logerror("login error %s \n", ndGetLastErrorDesc(h)) ;
		return -1;
	}	

	return 0;

}


int loginAndCreate(const char*udid,nd_handle h,int accType, const char *userName, const char *passwd,const char *save_session_file, NDUINT32 *accID) 
{
	int ret = 0;
	bool bSaveSession = (save_session_file && save_session_file[0]) ? true : false;
	LoginApollo login((nd_handle)h, bSaveSession, save_session_file); //for debug tool ,maybe it would open too many connectors
	
	login.InitUdid(udid) ;
	
	ret = login.Login(userName, passwd, (ACCOUNT_TYPE)accType) ;
	if (-1==ret ) {
		//Create account
		if ( ndGetLastError(h)==ESERVER_ERR_NOUSER) {
			account_base_info acc ;
			
			initAccCreateInfo(acc, accType,userName, passwd);
			
			ret = login.CreateAccount(&acc) ;
		}
		
	}
	
	if (ret == 0 ) {
		nd_logmsg("login complete SUCCESS!\n") ;
		if (accID) {
			*accID = login.getAccountID() ;
		}
	}else {
		nd_logerror("login error %s \n", ndGetLastErrorDesc(h)) ;
		return -1;
	}
	
	
	return 0;
	
}

int redirectServer(nd_handle h,const char *host, int port,const char*session_file)
{
	int ret = 0;
	LoginApollo login((nd_handle)h) ;
	
	ret = login.EnterServer(host, port,session_file) ;
	if (-1==ret ) {		
		nd_logerror("redirectServer error %s \n", ndGetLastErrorDesc(h)) ;
	}
	else {
		nd_logmsg("login complete SUCCESS!\n") ;
	}
	
	return ret;

}

int TryLogin(const char*udid,nd_handle h,int accType, const char *userName, const char *passwd,const char *save_session_file)
{
    int ret = -1;
    LoginApollo login((nd_handle)h,true, save_session_file) ;
	
	login.InitUdid(udid) ;
	
    //need input use-name and password
    ret = login.Login(userName, passwd, (ACCOUNT_TYPE)accType) ;
    
    if (ret == 0 ) {
        nd_logmsg("login complete SUCCESS!\n") ;
    }
    else {
        nd_logerror("login error %s \n", ndGetLastErrorDesc(h)) ;
    }
    return ret;
}

int TryRegister(const char*udid,nd_handle h,int accType, const char *userName, const char *passwd,const char *save_session_file)
{
    int ret = -1;
    LoginApollo login((nd_handle)h,true, save_session_file) ;
	
	login.InitUdid(udid) ;
	
    account_base_info acc ;
	
	initAccCreateInfo(acc, accType,userName, passwd);
	
    ret = login.CreateAccount(&acc) ;
    if (ret == 0 ) {
        nd_logmsg("login complete SUCCESS!\n") ;
    }
    else {
        nd_logerror("login error %s \n", ndGetLastErrorDesc(h)) ;
    }
    return ret;
}

int TryLogout(nd_handle h)
{
    int ret = -1;
    LoginApollo login((nd_handle)h,false, NULL) ;
    ret = login.Logout();
    return ret;
}

int TryReLogin(nd_handle h, const char *save_session_file)
{
    int ret = -1;
    LoginApollo login((nd_handle)h,true, save_session_file) ;
    ret = login.Relogin() ;
    
    if (0==ret) {
        nd_logmsg("login complete SUCCESS!\n") ;
        return  0 ;
    }
    
    return ret;
}


int getServerList(nd_handle h, ApolloServerInfo *buf, int size)
{
	
	LoginApollo login((nd_handle)h,false, NULL) ;
	
	return login.GetServerList(buf, size) ;
}

