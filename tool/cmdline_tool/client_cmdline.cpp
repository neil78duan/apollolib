/* file : client_cmdline.cpp
 *
 * use command line test server 
 * create by duan 
 * 2014-12-15
*/



#include "ndcli/nd_iconn.h"
#include "nd_msg.h"
#include "msg_def.h"
#include "ndcli/nd_api_c.h"
#include "netui_atl.h"
#include "ndapplib/nd_datatransfer.h"
#include "login_apollo.h"

#include "srv_define.h"

#include "atlantis_test.h"
#include "apollo_errors.h"
#include "commonTest.h"
#include "dftCliMsgHandler.h"
//#include "gameMessage.h"
//#include "sanguo_msg.h"
#define _DFT_SESSION_FILE "./test_session.sen"


/*
typedef int (*cmd_func)(int argc, const char *argv[]) ;

struct cmd_info_node 
{
	char name[32] ;
	cmd_func entry ;	
};
*/

NDIConn *__g_conn ;
void exit_tool(int flag);
nd_cmdline_root *__g_root = 0 ;

nd_cmdline_root* _get_cmd_root()
{
	return __g_root;
}

void init_cmdline_msg(NDIConn* pconn)
{	
	
	CONNECT_INSTALL_MSG(pconn, msg_echo_handler,ND_MAIN_ID_SYS,ND_MSG_SYS_ECHO) ;	
	//room message 	
//	CONNECT_INSTALL_MSG(pconn, msg_create_room_ack_handler,NETMSG_MAX_ROOM,ROOM_MSG_CREATE_ACK) ;	
//	CONNECT_INSTALL_MSG(pconn, msg_be_invited_room_handler,NETMSG_MAX_ROOM,ROOM_MSG_INVITED_NTF) ;	
//	CONNECT_INSTALL_MSG(pconn, msg_enter_room_ack_handler,NETMSG_MAX_BATTLE,BATTLE_MSG_ENTER_ACK);
	
}



int _loginServer( const char * host,int port, const char* user, const char *passwd,const char *sessionfile )
{
	if (EPL_LOGIN==getConnectStat(__g_conn)) {
		fprintf(stderr, "already in login\n") ;
		return -1 ;
	}
	if (__g_conn) {
		DestroyConnectorObj(__g_conn) ;
		__g_conn = 0 ;
	}
	__g_conn = LoginServer(  host, port,  user, passwd,sessionfile ) ;
	if (__g_conn) {
		init_cmdline_msg(__g_conn) ;
		return 0 ;
	}
	return -1;
	
}

ND_CMDLINE_FUNC_INSTANCE(Login) 
{
	ND_CMDLINE_CHECK_SHOW_HELP(argc, argv,"login host port [username] [password] [session_file]") ;
	int ret = 0 ;
	if (argc <=2) {
		ret= _loginServer(NULL,0,NULL,NULL,_DFT_SESSION_FILE) ;
	}
	else if(argc > 5 ) {
		ret =_loginServer(argv[1], atoi(argv[2]), argv[3], argv[4], argv[5]) ;
		
	}
	else 	{
		ret =_loginServer(argv[1], atoi(argv[2]), argv[3],argv[4],_DFT_SESSION_FILE) ;
	}
	
	if(0== ret ) {
		root->userdata = __g_conn ;
	}
	return ret ;
}

ND_CMDLINE_FUNC_INSTANCE(Login1) 
{
	ND_CMDLINE_CHECK_SHOW_HELP(argc, argv,"login1 username") ;
	int ret = 0 ;
	if (argc >=2) {
		ret= _loginServer("192.168.8.45",6600, argv[1],"test123",_DFT_SESSION_FILE) ;
	}
	else 	{
		ret =_loginServer("192.168.8.45",6600, "test1","test123",_DFT_SESSION_FILE) ;
	}
	
	if(0== ret ) {
		root->userdata = __g_conn ;
	}
	return ret ;
}

ND_CMDLINE_FUNC_INSTANCE(show_msgname) 
{
	
	ND_CMDLINE_CHECK_SHOW_HELP(argc, argv,"msgname maxid minid") ;
	if (!__g_conn || !__g_conn->CheckValid()) {
		fprintf(stderr, "please connect or login first\n") ;
		return 1;
	}
	if (argc <3) {
		fprintf(stderr, "USAGE: msgname maxid minid\n") ;
		return -1;
	}
	
	const char *name = nd_msgentry_get_name(__g_conn->GetHandle(), atoi(argv[1]), atoi(argv[2])) ;
	if (name && name[0]) {
		fprintf(stdout, "(%s , %s) : %s \n",argv[1], argv[2],name ) ;		
	}
	else {
		NDOStreamMsg omsg(ND_MAIN_ID_SYS,ND_MSG_SYS_GET_MESSAGE_NAME) ;
		omsg.Write((NDUINT16) atoi(argv[1])) ;
		omsg.Write((NDUINT16) atoi(argv[2])) ;
		if(__g_conn->SendMsg(omsg)>0) {
			fprintf(stdout, "request name from server .....\n") ;
			return 0 ;
		}
		else {
			fprintf(stderr, "error : cann't get the message name \n") ;
			return -1 ;
		}
	}
	
	fprintf(stderr, "error : cann't get the message name \n") ;
	return -1 ;
}

ND_CMDLINE_FUNC_INSTANCE(Logout)
{
	ND_CMDLINE_CHECK_SHOW_HELP(argc, argv,"logout") ;
	
	if (__g_conn) {
		NDOStreamMsg omsg(NETMSG_MAX_LOGIN, LOGIN_MSG_LOGOUT_NTF) ;		
		__g_conn->SendMsg(omsg, ESF_URGENCY |ESF_ENCRYPT) ;
		__g_conn->Close() ;
		DestroyConnectorObj(__g_conn) ;
		__g_conn = 0 ;
		
	}
	fprintf(stdout, "logout Success!\n") ;
	return 0;
}

ND_CMDLINE_FUNC_INSTANCE(send_message)
{
	ND_CMDLINE_CHECK_SHOW_HELP(argc, argv,"send maxID minID \"message content text\"") ;
	if (argc < 3) {		
		fprintf(stdout, "USAGE:  send maxID minID \"message content text\" \n") ;
		return 1 ;
	}
	
	if (__g_conn) {
		NDOStreamMsg omsg(atoi(argv[1]), atoi(argv[2])) ;
		for (int i=3; i<argc; i++) {
			omsg.Write((NDUINT8*)argv[i]) ;
		}
		int size = __g_conn->SendMsg(omsg, ESF_ENCRYPT) ;
		fprintf(stdout, "send Success data-len =%d\n" , size) ;
		
	}
	return 0;
}


void  send_file_ok(nd_handle nethandle, NDUINT64 param , int error_code)
{
	if (0==param) {
		fprintf(stdout, "send file %s\n", error_code ? "error": "success") ;
	}	
}

ND_CMDLINE_FUNC_INSTANCE(sendfile)
{
	ND_CMDLINE_CHECK_SHOW_HELP(argc, argv,"sendfile file-name-path") ;
	if (argc < 2) {		
		fprintf(stdout, "USAGE:  sendfile file-name-path \n") ;
		return 1 ;
	}
	if (!__g_conn) {
		fprintf(stderr, "Need login\n") ;
	}
	size_t file_size =0 ;
	void* pdata = nd_load_file(argv[1], &file_size); 
	if (!pdata) {
    	fprintf(stderr, "open file %s error \n", pdata) ;
		return -1;
	}
	BigDataAsyncSend(__g_conn->GetHandle(), pdata, file_size, 0, send_file_ok) ;
	
	nd_unload_file(pdata) ;
	
	return 0;
}

ND_CMDLINE_FUNC_INSTANCE(redirect_to_server)
{
	ND_CMDLINE_CHECK_SHOW_HELP(argc, argv,"switch [host] [port]") ;
//	if (argc < 3) {		
//		fprintf(stdout, "USAGE:  switch host port \n") ;
//		return 1 ;
//	}
	
	if (__g_conn) {
		
		if (argc >= 3) {
			int ret = redirectServer(__g_conn->GetHandle(),argv[1],atoi(argv[2]),_DFT_SESSION_FILE)  ;
			if (ret == 0) {
				fprintf(stdout, "redirect server success\n" ) ;
			}
			return ret ;
		}
		else {
			host_list_node bufs[20] ;
			LoginApollo login(__g_conn->GetHandle(), false) ;
			int num = login.GetServerList(bufs, ND_ELEMENTS_NUM(bufs)) ;
			
			if (num == 0) {
				fprintf(stdout, "get host list number=0\n") ;
				return -1 ;
			}
			int ret = redirectServer(__g_conn->GetHandle(),nd_inet_ntoa(bufs[0].ip, NULL),bufs[0].port,_DFT_SESSION_FILE)  ;
			if (ret == 0) {
				fprintf(stdout, "redirect server success\n" ) ;
			}
			return ret ;
		}
		
	}
	else {
		fprintf(stdout, "need login !\n") ;
		return -1;
	}
	
}

ND_CMDLINE_FUNC_INSTANCE(get_server_list)
{
	ND_CMDLINE_CHECK_SHOW_HELP(argc, argv,"server_list ") ;
	
	
	if (__g_conn) {
		host_list_node host_buf[20] ;
		LoginApollo login(__g_conn->GetHandle(), false) ;
		int num = login.GetServerList(host_buf, ND_ELEMENTS_NUM(host_buf)) ;
		for (int i=0; i<num; ++i) {
			fprintf(stdout, "server : %s : %d [%s]\n", nd_inet_ntoa(host_buf[i].ip, NULL),host_buf[i].port, host_buf[i].name) ;
		}
		if (num == 0) {
			fprintf(stdout, "get host list number=0\n") ;
		}
		
	
	}
	else {
		fprintf(stdout, "need login !\n") ;
		return -1;
	}
	return 0;
}

ND_CMDLINE_FUNC_INSTANCE(get_server_version)
{
	ND_CMDLINE_CHECK_SHOW_HELP(argc, argv,"srvinfo") ;
	if (__g_conn) {
		NDOStreamMsg omsg(ND_MAIN_ID_SYS,ND_MSG_SYS_GETVERSION) ;		
		
		nd_handle h = __g_conn->GetHandle() ;		
		nd_usermsgbuf_t recv_msg ;
		
		TEST_SEND_AND_WAIT(h, omsg, &recv_msg, ND_MAIN_ID_SYS, ND_MSG_SYS_GETVERSION,0)
		else {
			if (argc==2 ) {
				NDUINT8 isDebug =0 ;
				NDIStreamMsg inmsg(&recv_msg) ;
				NDUINT32 ver_id = 0;				
				inmsg.Read(ver_id) ;	
				
				inmsg.Read(isDebug) ;
				NDUINT32 input_id = atoi(argv[1]) ;
				if (input_id != ver_id) {
					fprintf(stderr, "Need version =%d server version is %d %s\n",input_id, ver_id, isDebug?"debug":"release");
					return  -1 ;
				}
				
				fprintf(stdout, "check version id success %d %s\n", ver_id, isDebug?"debug":"release");
				return 0;
			}
			else {
				return ClientMsgHandler::msg_get_version_handler(__g_conn, &recv_msg) ;
			}
		}
		
	}
	else {
		fprintf(stdout, "need login !\n") ;
		return -1;
	}
	
}
////

ND_CMDLINE_FUNC_INSTANCE(check_server_program_update)
{
	ND_CMDLINE_CHECK_SHOW_HELP(argc, argv,"check_srv new-version [world room game all] ") ;
	if (argc < 2) {
		fprintf(stderr, "usage: check_srv new-version [world account game all] \n") ;
		return 1;
	}
	if (__g_conn) {
		NDUINT32 input_id = atoi(argv[1]) ;
		NDUINT8 serverType = ESERVER_UNKNOW ;
		
		if (argc >= 3) {
			if (0==ndstricmp((char*)argv[2],(char*) "game")) {
				serverType = ESERVER_UNKNOW ;
			}
			else if (0==ndstricmp((char*)argv[2], (char*)"world")) {
				serverType = ESERVER_MANAGER ;
			}
			else if (0==ndstricmp((char*)argv[2], (char*)"account")) {
				serverType = ESERVER_BATTLE_ROOM ;
			}
		}
		
		NDOStreamMsg omsg(NETMSG_MAX_SERVER,SERVER_MSG_GETVERSION) ;	
		omsg.Write(serverType) ;
		
		nd_handle h = __g_conn->GetHandle() ;		
		nd_usermsgbuf_t recv_msg ;
		
		TEST_SEND_AND_WAIT(h, omsg, &recv_msg, NETMSG_MAX_SERVER,SERVER_MSG_GETVERSION,0)
		else {
			NDUINT8 isDebug =0 , aimType=0;
			NDUINT32 ver_id = 0;			
			NDIStreamMsg inmsg(&recv_msg) ;	
			
			inmsg.Read(aimType);
			inmsg.Read(ver_id) ;
			inmsg.Read(isDebug) ;
			
			const char *pHost = "server" ;
			if (aimType==ESERVER_MANAGER) {
				pHost = "world-server" ;
			}
			else if (aimType==ESERVER_ACCOUNT) {
				pHost = "account-server" ;
			}
			else if (aimType==ESERVER_GAMEHALL) {
				pHost = "game-server" ;
			}
			
			if (input_id != ver_id) {
				fprintf(stderr, "The %s version not match current=%d server=%d %s\n",pHost, input_id, ver_id, isDebug?"debug":"release");
				return  -1 ;
			}
			
			fprintf(stdout, "check server version success %d %s\n", ver_id, isDebug?"debug":"release");
			return 0;
		}
		
	}
	else {
		fprintf(stdout, "need login !\n") ;
		return -1;
	}
	
}
ND_CMDLINE_FUNC_INSTANCE(get_server_rlimit)
{
	ND_CMDLINE_CHECK_SHOW_HELP(argc, argv,"srvlimit") ;
	
	if (__g_conn) {
		
		NDOStreamMsg omsg(ND_MAIN_ID_SYS,ND_MSG_SYS_GET_RLIMIT) ;
		
		nd_handle h = __g_conn->GetHandle() ;		
		nd_usermsgbuf_t recv_msg ;
		
		TEST_SEND_AND_WAIT(h, omsg, &recv_msg, ND_MAIN_ID_SYS, ND_MSG_SYS_GET_RLIMIT,0)
		else {
			return ClientMsgHandler::msg_get_rlimit_handler(__g_conn, &recv_msg) ;
		}
		
	}
	else {
		fprintf(stdout, "need login !\n") ;
		return -1;
	}
	
}
//request server log specified message
ND_CMDLINE_FUNC_INSTANCE(srv_log_msg)
{
    ND_CMDLINE_CHECK_SHOW_HELP(argc, argv,"srvmsglog maxID minID [is-open?0:1]") ;
    if (argc < 3 ) {
        fprintf(stdout, "usage: srvmsglog maxID minID [is-open?0:1] ,argv[i]\n" ) ;
        return 1;
    }
    
    if (__g_conn) {
        NDOStreamMsg omsg(ND_MAIN_ID_SYS,ND_MSG_SYS_SET_MSGLOG) ;
        omsg.Write((NDUINT16)atoi(argv[1]) ) ;
        omsg.Write((NDUINT16)atoi(argv[2]) ) ;
        if (argc >= 4) {
            omsg.Write((NDUINT8)atoi(argv[3]) ) ;
        }
        else {
            omsg.Write((NDUINT8)1 ) ;
        }
        
        nd_handle h = __g_conn->GetHandle() ;
        nd_usermsgbuf_t recv_msg ;
        
        TEST_SEND_AND_WAIT(h, omsg, &recv_msg, ND_MAIN_ID_SYS, ND_MSG_SYS_SET_MSGLOG,0)
        else {
            NDUINT16 maxID, minID ;
            NDUINT8 isOpen = 0xff;
            NDIStreamMsg inmsg(&recv_msg) ;
            if (-1==inmsg.Read(maxID)) {
                fprintf(stderr, "read message error\n") ;
                return -1 ;
            }
            if (-1==inmsg.Read(minID)) {
                fprintf(stderr, "read message error\n") ;
                return -1 ;
            }
            if (-1==inmsg.Read(isOpen)) {
                fprintf(stderr, "read message error\n") ;
                return -1 ;
            }
            const char *p = "error" ;
            if (0==isOpen) {
                p = "open" ;
            }
            else if(1==isOpen) {
                p = "close";
            }
            fprintf(stdout, "set message (%d, %d) log %s\n", maxID, minID, p) ;
        }
        return 0;
    }
    else {
        fprintf(stdout, "need login !\n") ;
        return -1;
    }
    
}

ND_CMDLINE_FUNC_INSTANCE(Status)
{
	ND_CMDLINE_CHECK_SHOW_HELP(argc, argv,"status") ;
	int stat =  getConnectStat(__g_conn);
	
	if (stat >= EPL_LOGIN) {
		fprintf(stdout, "Login ok!\n") ;
	}
	else if(stat > 0) {
		fprintf(stdout, "Connect ok!\n") ;
	}
	else{
		fprintf(stdout, "Unconnected \n") ;
	}	
	return 0;
}

ND_CMDLINE_FUNC_INSTANCE(exit_tool)
{
	ND_CMDLINE_CHECK_SHOW_HELP(argc, argv,"quit") ;
	
	Logout(root, 0, NULL) ;
	DeinitNet();
	if (argc >=2) {
		int flag = atoi( argv[1] );
		exit(flag) ;
	}
	else {
		exit(0) ;
	}
	return 0;
}


ND_CMDLINE_FUNC_INSTANCE(cmdline_echo)
{
	ND_CMDLINE_CHECK_SHOW_HELP(argc, argv,"echo [echo-text-content]") ;
	
	if (argc >=2) {
		for (int i=1; i<argc; i++) {
			fprintf(stdout, "%s " ,argv[i] ) ;			
		}
	}	
	fprintf(stdout, "\n"  ) ;
	return 0;
}


ND_CMDLINE_FUNC_INSTANCE(update_tool)
{
	int ret = 0 ;
	if (__g_conn) {
		ret = __g_conn->Update(50);
		if(-1== ret && __g_conn){
			__g_conn->Close() ;
			DestroyConnectorObj(__g_conn) ;
			__g_conn = 0 ;
			//gloable_connector_on_error(__g_conn->GetHandle(), 0,0) ;
		}
	}
	return ret;
}


ND_CMDLINE_FUNC_INSTANCE(init_tool)
{
	if(InitNet()){
		fprintf(stderr,"nd-net init error %s \n", nd_last_error()) ;
		DeinitNet();
		return -1;
	}
		
	return 0;
}


ND_CMDLINE_FUNC_INSTANCE(convert_error)
{
	nd_register_error_convert(atlantis_error);
	ND_CMDLINE_CHECK_SHOW_HELP(argc, argv,"error error-code : convert error to text") ;
	
	if (argc < 2) {		
		fprintf(stdout, "USAGE: error error-code  \n") ;
		return 1 ;
	}
	fprintf(stdout, "%s error = %s \n", argv[1], nd_error_desc(atoi(argv[1])) ) ;
	return 0;
}

ND_CMDLINE_FUNC_INSTANCE(current_dir)
{
	ND_CMDLINE_CHECK_SHOW_HELP(argc, argv,"pwd : show current dir") ;
	
	fprintf(stdout, "%s\n",nd_getcwd() ) ;
	
	
	return 0;
}


ND_CMDLINE_FUNC_INSTANCE(change_dir)
{
	ND_CMDLINE_CHECK_SHOW_HELP(argc, argv,"cd [aim-dir]") ;
	
	if (argc < 2) {
		nd_chdir(nd_get_init_dir()) ;
	}
	else {
		if(-1==nd_chdir(argv[1]) ) {
			fprintf(stderr, "change dir to %s error \n",nd_getcwd() ) ;
		}
	}
	return 0;
}

ND_CMDLINE_FUNC_INSTANCE(list_file)
{
    ND_CMDLINE_CHECK_SHOW_HELP(argc, argv,"ls [aim-dir]") ;
    DIR *dirp ;
    struct dirent * dp ;
    if (argc >= 2) {
        dirp = opendir(argv[1]);
    }
    else {
        dirp = opendir(".");
    }
    
    if (dirp == NULL) {
        fprintf(stderr, "open dir error\n") ;
        return -1;
    }
    
    while ((dp = readdir(dirp)) != NULL) {
        fprintf(stdout, "%s\n", dp->d_name) ;
    }
    closedir(dirp);
    return 0;

}

ND_CMDLINE_FUNC_INSTANCE(run_shell)
{
	char shell_param[1024] ;
	char *p = shell_param ;
	int size = 0 ;
	ND_CMDLINE_CHECK_SHOW_HELP(argc, argv,"shell [shell command ] [shell-param]") ;
	
	for (int i=1; i<argc; ++i) {
		size = snprintf(p, sizeof(shell_param) -(p-shell_param),"%s ", argv[i]) ;
		p += size ;
	}
	system(shell_param) ;
	return 0;
}


extern ND_CMDLINE_FUNC_INSTANCE(net_msg_test);

ND_CMDLINE_FUNC_INSTANCE(trace_mmap) ;

int main(int argc, char *argv[])
{
	int ret ;
	nd_cmdline_root root = { 0, 0,"connector test" } ;
	root.init_func = init_tool ;	
	
		
	nd_cmdline_node cmdlist[] = {
		{"login", Login} 
		,{"login1", Login1} 
		,{"logout", Logout}
		,{"quit" , exit_tool}
		
		,{"role_list" , getRoleList}
		,{"role_create" , createRole}
		
		,{"help", nd_cmdline_help}
		,{"checkexit",nd_cmdline_check_and_eixt}
		,{"status", Status}
		,{"message", net_msg_test}
		,{"server_list", get_server_list }
		,{"switch" , redirect_to_server }		
		,{"msgname" , show_msgname }
		//,{"sendfile", sendfile }
		,{"cd" , change_dir }
		,{"pwd" , current_dir }
		,{"shell" , run_shell }
        ,{"ls" , list_file }
		,{"echo",cmdline_echo}
        
		//ATLANTIS function
		//,{"chat",atlantisChat}
		//,{"pchat",privateChat}
		,{"srvinfo",get_server_version}
		,{"check_srv", check_server_program_update}
		,{"srvlimit",get_server_rlimit}
		,{"online_number",get_total_online}
		,{"per_host_number",get_per_hosts_online_number}

		//,{"room",room_test}
		
		//,{"remotecall",atlantisBraodcast}
		//,{"addfriend",atlantisAddfriend}
		,{"error",convert_error}
        ,{"trace", trace_mmap}
		,{"triptime",atlantis_round_trip_time}
        ,{"srvmsglog",srv_log_msg} 
		
		,{"rsakeycpp",gen_rsa_key_cpp } 
		
		,{"rsakeybin",gen_rsa_key_bin } 
		
	};
	
	root.update_func = update_tool;
	root.number = ND_ELEMENTS_NUM(cmdlist) ;
	root.entries = cmdlist ;

	
	ret = nd_run_cmdline(&root,argc,(const char**)argv) ;
	if (0==ret) {
		exit_tool(&root, 0,0) ;
	}
	else {
		exit(1) ;
	}
	
}

////////////////////////////////
//trace mmap
ND_CMDLINE_FUNC_INSTANCE(open_mmap)
{
    ND_CMDLINE_CHECK_SHOW_HELP(argc, argv,"open mmap-file") ;
    if (argc < 2) {
        fprintf(stderr, "USAGE : open mmap-file \n") ;
        return  1;
    }
    nd_callstack_monitor_end() ;
    if(nd_callstack_monitor_init(argv[1]) ) {
        fprintf(stderr, "error in open %s memory_map_file\n", argv[1]) ;
        return -1;
    }
    return 0 ;
}


ND_CMDLINE_FUNC_INSTANCE(close_mmap)
{
    ND_CMDLINE_CHECK_SHOW_HELP(argc, argv,"close : close-mmapfile") ;
    
    nd_callstack_monitor_end() ;
    return 0 ;
}

ND_CMDLINE_FUNC_INSTANCE(dump_mmap)
{
    ND_CMDLINE_CHECK_SHOW_HELP(argc, argv,"dump [to-output-file]") ;
    
    if (argc >=2 ) {
        FILE *fp = fopen(argv[1], "w") ;
        if (fp) {
            nd_callstack_monitor_dump(fprintf, fp);
            fclose(fp) ;
        }
        else {
            fprintf(stderr, "dump to %s file error\n", argv[1]);
            return -1;
        }
        
    }
    else {
        nd_callstack_monitor_dump(fprintf, stdout);
    }
    return 0 ;
}


ND_CMDLINE_FUNC_INSTANCE(trace_end)
{
    ND_CMDLINE_CHECK_SHOW_HELP(argc, argv,"end") ;
    
    nd_callstack_monitor_end() ;
    nd_cmdline_quit(root) ;
    return 0 ;
}


ND_CMDLINE_FUNC_INSTANCE(trace_mmap)
{
    
    ND_CMDLINE_CHECK_SHOW_HELP(argc, argv,"trace [mmap-file]") ;
    
    nd_cmdline_root myroot = { 0, 0,"trace" } ;
    
    nd_cmdline_node cmdlist[] = {
        {"help", nd_cmdline_help}
        ,{"open", open_mmap}
        ,{"close", close_mmap}
        ,{"end" , trace_end}
        ,{"dump", dump_mmap}    };
    
    
    //myroot.update_func = root->update_func ;
    myroot.userdata = root->userdata ;
    
    
    myroot.number = ND_ELEMENTS_NUM(cmdlist) ;
    myroot.entries = cmdlist ;
    
    
    nd_common_init() ;
    if (argc >= 2) {
        if(nd_callstack_monitor_init(argv[1]) ) {
            fprintf(stderr, "error in open %s memory_map_file\n", argv[1]) ;
        }
    }
    
	__g_root = &myroot;
    return nd_run_cmdline(&myroot,argc,argv) ;
}


