//
//  commonTest.cpp
//  clientDemo
//
//  Created by duanxiuyun on 15-2-4.
//  Copyright (c) 2015年 duanxiuyun. All rights reserved.
//

#include "commonTest.h"
#include "apollo_errors.h"
//#include "gameMessage.h"
#include "netui_atl.h"

#include "nd_msg.h"
#include "msg_def.h"
//#include "apollo_data.h"
#include "login_apollo.h"

#include "dftCliMsgHandler.h"

int getConnectStat(NDIConn *pConn)
{
	if (pConn && pConn->CheckValid()) {		
		int priv_level = 0 ;
		int size = sizeof(priv_level) ;
		
		pConn->ioctl(NDIOCTL_GET_LEVEL, &priv_level, &size) ;
		return  priv_level ;
	}
	return 0;
}



NDIConn* LoginServer( const char * host,int port, const char* user, const char *passwd, const char*sessionFile,NDUINT32 *accID ) 
{
	NDIConn *pConn = CreateConnectorObj(NULL) ;
	if (!pConn) {
		return 0 ;
	}
	//initCommonMessageHandle(pConn) ;
	if (!host || !host[0]) {
		host = "localhost" ;
		port = 6600 ;
	}
	
	if(-1==pConn->Open(host, port, "tcp-connector", NULL) ) {		
		fprintf(stderr, "connect %s:%d ERROR \n",  host, port) ;		
		DestroyConnectorObj(pConn) ;
		return 0 ;
	}
	
	fprintf(stdout, "connect %s:%d success \n",host, port ) ;
	
	if (!user || !user[0]) {
		user = "testTooluser0" ;
		passwd = "testUser123" ;
	}
	
	int bSuccess = 0;
	if (sessionFile && sessionFile[0]) {
		bSuccess =	RloginTrylogin(pConn->GetHandle(),ACC_APOLLO, user, passwd,sessionFile, accID) ;
	}
	else {
		bSuccess = loginAndCreate(pConn->GetHandle(),ACC_APOLLO,user, passwd,NULL,accID);
	}
	
	if (0== bSuccess) {
		fprintf(stdout,"%s Login Success \n",user) ;
	}
	else {
		fprintf(stderr,"%s Login Failed errorcode = %d\n", user, pConn->LastError()) ;
		pConn->Close(1) ; 
		DestroyConnectorObj(pConn) ;
		return 0;
	}
	return pConn;
}



