/* file apollo_client.h
 *
 * define net client and login of apollo project 
 *
 * create by duan
 * 2015-6
 */

#ifndef __APOLLO_CLIENT_H__
#define __APOLLO_CLIENT_H__


#if defined(_MSC_VER)
#ifdef NDCLIENT_FOR_UE4
#include "AllowWindowsPlatformTypes.h"
#endif 

#include "ndlib.h"
#include "cli_common/login_apollo.h"
#include "cli_common/netui_atl.h"
#include "apollo_data.h"
#include "nd_msg.h"
#include "msg_def.h"
#include "apollo_errors.h"

#ifdef NDCLIENT_FOR_UE4 
#include "HideWindowsPlatformTypes.h"
#endif 

#else 

#include "ndlib.h"
#include "cli_common/login_apollo.h"
#include "cli_common/netui_atl.h"
#include "apollo_data.h"
#include "nd_msg.h"
#include "msg_def.h"
#include "apollo_errors.h"
	
#endif


#endif

