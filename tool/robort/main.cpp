//
//  main.cpp
//  httpRobort
//
//  Created by duanxiuyun on 14-8-4.
//  Copyright (c) 2014 duanxiuyun. All rights reserved.
//


#include "ndcli/nd_iconn.h"
#include "ndcli/nd_api_c.h"

#include "testConnector.h"

#define  MAX_CONNECTOR_NUM 1024
int volatile __exit = 0;

char *__host;
int __port ;

char *__stream_file = 0 ;

//NDIConn *__conn_buf[MAX_CONNECTOR_NUM] ;
int __real_conn_num = 1;
int __acc_index_start = 0 ;
int trytoLogin(NDIConn *pconn) ;

int ParseArg(int argc, char *argv[])
{
    if (argc < 5) {
ERROR_EXIT:
        printf("USAGE: %s host port [connects-number] [account_start_index] [message-stream-file]\n" , argv[0]) ;
                      
        exit(1);
    }
	
	
    __host = argv[1] ;
    __port = atoi(argv[2]) ;
	if (argc >= 4) {
		__real_conn_num = atoi(argv[3]) ;
		if (__real_conn_num > MAX_CONNECTOR_NUM) {
			__real_conn_num = MAX_CONNECTOR_NUM;
		}
	}
	if (argc >= 5) {
		__acc_index_start =	 atoi(argv[4]) ;
	}
	if (argc >= 6) {
		__stream_file = argv[5] ;
	}
    return 0;
}


TestconnsMgr *__pconnsMgr ;

int wait_exit()
{
    int ch;
    int index = 0 ;
    while( 0==__exit ){
        if(kbhit()) {
            ch = getch() ;
            if(ND_ESC==ch){
                printf_dbg("you are hit ESC, program eixt\n") ;
                __exit = 1 ;
                break ;
            }
        }
        else {
			if(-1==__pconnsMgr->Update() ) {
				__exit = 1 ;
				fprintf(stdout, "test robort completed !!!");
				break ;
			}
			//__pconnsMgr->UpdateAutoTestRoom() ;
            //updateConnect() ;
            nd_sleep(100) ;
        }
        index++ ;
    }
    return 0;
}


int main(int argc , char *argv[])
{
    
    ParseArg( argc, argv) ;
    
    if(InitNet()){
        ndprintf(_NDT("connect error :%s!"), nd_last_error()) ;
        //getch();
		DeinitNet();
        exit(1);
    }
	TestconnsMgr connsMgr ;
	__pconnsMgr =&connsMgr ;
	
	if (__stream_file && __stream_file[0]) {
		
		if (0!=connsMgr.initStreamMsg(__stream_file) ) {
			fprintf(stderr, "load stream file error \n") ;
			exit(1) ;
		}
	}
	
	connsMgr.init(__host, __port, __real_conn_num,__acc_index_start) ;

    wait_exit() ;
	
	__pconnsMgr->Destroy() ;
    
    DeinitNet();
    
    fprintf(stderr, "client test exit!\n") ;
    //getch();
    return 0;
}

