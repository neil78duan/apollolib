/* file apollo_client.h
 *
 * define net client and login of apollo project 
 *
 * create by duan
 * 2015-6
 */

#ifndef __APOLLO_CLIENT_H__
#define __APOLLO_CLIENT_H__


#if defined(_MSC_VER) && defined(NDCLIENT_FOR_UE4)
#include "AllowWindowsPlatformTypes.h"
#endif 

#include "ndlib.h"
#include "login_apollo.h"
#include "apollo_data.h"
#include "nd_msg.h"
#include "msg_def.h"
#include "apollo_errors.h"
#include "apoClientU3d.h"


#ifdef BUILD_AS_THIRD_PARTY
#include "loginBase.h"
#else 
#include "login_apollo.h"
#endif


#if defined(_MSC_VER) && defined(NDCLIENT_FOR_UE4)
#include "HideWindowsPlatformTypes.h"
#endif 


#endif

