//
//  atlantis_errors.h
//  dbManager
//
//  Created by duanxiuyun on 14-9-29.
//  Copyright (c) 2014年 duanxiuyun. All rights reserved.
//

#ifndef dbManager_apollo_errors_h
#define dbManager_apollo_errors_h

#if defined(BUILD_AS_THIRD_PARTY)
#include "ndlib.h"
#else 
#include "nd_common/nd_define.h"
#endif

enum eApollo_errors {
	ESERVER_ERR_SUCCESS = 0,

#undef ErrorElement 
#define ErrorElement(a,_err_desc) NDSYS_##a
#include "nd_common/_nderr.h"
#undef ErrorElement
    
	ApolloError_Start = NDERR_USERDEFINE,
	
#undef ErrorElement 
#define ErrorElement(a,_err_desc) a
#include "_error_def.h"		
#undef ErrorElement     
};

typedef eApollo_errors RESULT_T;
 
namespace result {
	
	enum Result
	{
		success 			= 0,
		failed				= 1,
	};
	
};


extern const char *apollo_error(int errcode);
#endif
