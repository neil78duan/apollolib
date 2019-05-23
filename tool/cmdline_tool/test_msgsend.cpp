//
//  test_msgsend.cpp
//  nddebugger
//
//  Created by duanxiuyun on 14-12-16.
//  Copyright (c) 2014年 duanxiuyun. All rights reserved.
//



#include "ndcli/nd_iconn.h"
#include "nd_common/nd_cmdline.h"
#include "nd_msg.h"
#include "msg_def.h"
//#include "ndcli/nd_api_c.h"


NDOStreamMsg _g_omsg ;


ND_CMDLINE_FUNC_INSTANCE( ResetMsg)
{
	ND_CMDLINE_CHECK_SHOW_HELP(argc, argv,"reset [maxID] [minID]") ;
	if (argc >=3) {
		_g_omsg.Reset() ;
		_g_omsg.SetID(atoi(argv[1]), atoi(argv[2])) ;
	}
	else {
		_g_omsg.Reset() ;
	}
	return 0 ;
}

ND_CMDLINE_FUNC_INSTANCE(SendMsg)
{
	ND_CMDLINE_CHECK_SHOW_HELP(argc, argv,"send (send the message you builded)") ;
	
	
	NDIConn *pConn = (NDIConn *) root->userdata ;
	if (!pConn) {
		fprintf(stderr, "error not login \n") ;
		return -1 ;
	}
	int ret = pConn->SendMsg(_g_omsg, ESF_URGENCY) ;	
	
	fprintf(stdout, "send data len =%d \n", ret) ;
	
	return 0 ;
}

ND_CMDLINE_FUNC_INSTANCE(LeaveOff) 
{
	ND_CMDLINE_CHECK_SHOW_HELP(argc, argv,"end :(exit message build and send)") ;	
	_g_omsg.Reset() ;
	nd_cmdline_quit(root) ;
	
	//fprintf(stdout, "\n") ;
	return 0 ;
}

ND_CMDLINE_FUNC_INSTANCE(WriteInt8)
{
	ND_CMDLINE_CHECK_SHOW_HELP(argc, argv,"int8 number") ;
	if (argc >=2) {
		_g_omsg.Write((NDUINT8)atoi(argv[1])) ;		
		fprintf(stdout, "Message Data-len=%d\n", (int) _g_omsg.GetDataLen()) ;
	}
	else {
		fprintf(stdout, "usage : int8 number\n") ;		
	}
	return 0 ;
}

ND_CMDLINE_FUNC_INSTANCE( WriteInt16)
{
	ND_CMDLINE_CHECK_SHOW_HELP(argc, argv,"int16 number") ;
	if (argc >=2) {
		_g_omsg.Write((NDUINT16)atoi(argv[1])) ;		
		fprintf(stdout, "Message Data-len=%d\n", (int) _g_omsg.GetDataLen()) ;
	}
	else {
		fprintf(stdout, "usage : int16 number\n") ;		
	}
	return 0 ;
}

ND_CMDLINE_FUNC_INSTANCE( WriteInt32)
{
	ND_CMDLINE_CHECK_SHOW_HELP(argc, argv,"int32 number") ;
	if (argc >=2) {
		_g_omsg.Write((NDUINT32)atoi(argv[1])) ;		
		fprintf(stdout, "Message Data-len=%d\n", (int) _g_omsg.GetDataLen()) ;
	}
	else {
		fprintf(stdout, "usage : int32 number\n") ;		
	}
	return 0 ;
}

ND_CMDLINE_FUNC_INSTANCE( WriteInt64)
{
	ND_CMDLINE_CHECK_SHOW_HELP(argc, argv,"int64 number") ;
	if (argc >=2) {
		_g_omsg.Write((NDUINT64)atoi(argv[1])) ;		
		fprintf(stdout, "Message Data-len=%d\n", (int) _g_omsg.GetDataLen()) ;
	}
	else {
		fprintf(stdout, "usage : int64 number\n") ;		
	}
	return 0 ;
}

ND_CMDLINE_FUNC_INSTANCE( WriteFloat)
{
	ND_CMDLINE_CHECK_SHOW_HELP(argc, argv,"float  number") ;
	if (argc >=2) {
		_g_omsg.Write((float)atof(argv[1])) ;		
		fprintf(stdout, "Message Data-len=%d\n", (int) _g_omsg.GetDataLen()) ;
	}
	else {
		fprintf(stdout, "usage : float number\n") ;		
	}
	return 0 ;
}

ND_CMDLINE_FUNC_INSTANCE(WriteText)
{
	ND_CMDLINE_CHECK_SHOW_HELP(argc, argv,"text input-text-you-need-send") ;
	if (argc >=2) {
		_g_omsg.Write((NDUINT8*)argv[1]) ;		
		fprintf(stdout, "Message Data-len=%d\n", (int) _g_omsg.GetDataLen()) ;
	}
	else {
		fprintf(stdout, "usage : text input-text-you-need-send \n") ;		
	}
	return 0 ;
}

ND_CMDLINE_FUNC_INSTANCE(SetMsgId)
{
	ND_CMDLINE_CHECK_SHOW_HELP(argc, argv,"setid maxID minID") ;
	if (argc >=3) {
		_g_omsg.SetID(atoi(argv[1]), atoi(argv[2])) ;
	}
	else {
		fprintf(stdout, "usage : setid maxID minID\n") ;	
	}
	return 0 ;
}

ND_CMDLINE_FUNC_INSTANCE(MsgInfo)
{
	ND_CMDLINE_CHECK_SHOW_HELP(argc, argv,"info : show message data length") ;
	fprintf(stdout, "message data len =%d\n", (int) _g_omsg.GetDataLen()) ;
	return 0 ;
}


ND_CMDLINE_FUNC_INSTANCE(net_msg_test)
{
	
	ND_CMDLINE_CHECK_SHOW_HELP(argc, argv,"message") ;
	
	nd_cmdline_root myroot = { 0, 0,"message" } ;
	
	nd_cmdline_node cmdlist[] = {
		{"help", nd_cmdline_help}
		,{"reset", ResetMsg} 
		,{"send", SendMsg} 
		,{"end" , LeaveOff} 
		,{"int8", WriteInt8}
		,{"int16", WriteInt16}
		,{"int32", WriteInt32}
		,{"int64", WriteInt64}
		,{"float", WriteFloat}
		,{"text", WriteText}
		,{"setid", SetMsgId}
		,{"info", MsgInfo}
	};
	
	
	myroot.update_func = root->update_func ;
	myroot.userdata = root->userdata ;
	
	
	myroot.number = ND_ELEMENTS_NUM(cmdlist) ;
	myroot.entries = cmdlist ;
	
	_g_omsg.Reset() ;
	
	return nd_run_cmdline(&myroot,argc,argv) ;
}
