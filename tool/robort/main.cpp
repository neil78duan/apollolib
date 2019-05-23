//
//  main.cpp
//  httpRobort
//
//  Created by duanxiuyun on 14-8-4.
//  Copyright (c) 2014 duanxiuyun. All rights reserved.
//


#include "ndcli/nd_iconn.h"
//#include "ndcli/nd_api_c.h"

#include "testConnector.h"
#include <vector>

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

// 
// TestconnsMgr *__pconnsMgr ;
// 
// int wait_exit()
// {
//     int ch;
//     int index = 0 ;
//     while( 0==__exit ){
//         if(kbhit()) {
//             ch = getch() ;
//             if(ND_ESC==ch){
//                 printf_dbg("you are hit ESC, program eixt\n") ;
//                 __exit = 1 ;
//                 break ;
//             }
//         }
//         else {
// 			if(-1==__pconnsMgr->Update() ) {
// 				__exit = 1 ;
// 				fprintf(stdout, "test robort completed !!!");
// 				break ;
// 			}
// 			//__pconnsMgr->UpdateAutoTestRoom() ;
//             //updateConnect() ;
//             nd_sleep(100) ;
//         }
//         index++ ;
//     }
//     return 0;
// }

struct robortHostInfo
{
	int acc_start_index;
	int roborts_num;
	const char *userPreName;
};

void *robort_update_entry(void* param)
{
	robortHostInfo *pRobortInfo = (robortHostInfo*)param;
	nd_assert(pRobortInfo);

	TestconnsMgr connsMgr;
	if (0 != connsMgr.initStreamMsg(__stream_file)) {
		fprintf(stderr, "load stream file error \n");
		return (void*)1;
	}

	connsMgr.setAccountPrefixName(pRobortInfo->userPreName);
	connsMgr.init(__host, __port, pRobortInfo->roborts_num, pRobortInfo->acc_start_index);
	
	while (0 == __exit){
		if (-1 == connsMgr.Update()) {
			fprintf(stdout, "thread test roborts completed !!!");
			break;
		}
		nd_sleep(50);
	}
	connsMgr.Destroy();
	
	delete pRobortInfo;
	return 0;
}

#define NUMBER_PER_THREAD 10

int main(int argc , char *argv[])
{
    ParseArg( argc, argv) ;
    
    if(InitNet()){
        ndprintf(_NDT("connect error :%s!"), nd_last_error()) ;
        //getch();
		DeinitNet();
        exit(1);
    }
	const char *preName = nd_filename(argv[0]);
	std::vector<ndth_handle> thvct;

	if (__real_conn_num >NUMBER_PER_THREAD)	{
		int thNumber = (__real_conn_num + NUMBER_PER_THREAD - 1) / NUMBER_PER_THREAD;
		for (int i = 0; i < thNumber; i++)	{

			robortHostInfo *pRobortInfo = new robortHostInfo();
			pRobortInfo->acc_start_index = __acc_index_start;
			if (i == thNumber -1)	{
				pRobortInfo->roborts_num = __real_conn_num - (thNumber - 1) *NUMBER_PER_THREAD;
			}
			else {
				pRobortInfo->roborts_num = NUMBER_PER_THREAD;
			}
			pRobortInfo->userPreName = preName;

			__acc_index_start += NUMBER_PER_THREAD;

			ndthread_t thid;
			ndth_handle thHandler = nd_createthread(robort_update_entry, pRobortInfo, &thid, 0);
			if (thHandler) {
				thvct.push_back(thHandler);
			}
		}

		//wait thread 
		for (size_t i = 0; i < thvct.size(); i++){
			nd_waitthread(thvct[i]);
		}
	}
	else {
		robortHostInfo *pRobortInfo = new robortHostInfo();
		pRobortInfo->acc_start_index = __acc_index_start;
		pRobortInfo->roborts_num = __real_conn_num;
		pRobortInfo->userPreName = preName;

		robort_update_entry((void*)pRobortInfo);

	}

    DeinitNet();
    
    fprintf(stderr, "client test exit!\n") ;
    //getch();
    return 0;
}

