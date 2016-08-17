//
//  apollo_msg_entry.h
//  gameHall
//
//  Created by duanxiuyun on 15-4-14.
//  Copyright (c) 2015年 duanxiuyun. All rights reserved.
//

#ifndef gameHall_apollo_msg_entry_h
#define gameHall_apollo_msg_entry_h

#include "netMessage/dataStream.h"

/*
CPPAPI const char *protocol_getNameByID(NDUINT32 id);

CPPAPI NDUINT32 protocol_getIDByName(const char *name);
//CALL_APOLLO_FUNCTION_ENTRY(nethandle,msg,UserLoginRequest);
#define CALL_APOLLO_FUNCTION_ENTRY(_h, _msg, _DataType,_MESSAGE_NAME) \
do {							\
	NDObject *pObj = 0 ;		\
	Protocol::MessageHandlerBase *messageHandler = NULL ;	\
	if(((nd_netui_handle)_h)->is_session) {					\
		pObj =  NDGetSession( (nd_handle)_h) ;				\
		NDListener *plisten = NDGetListener(h_listen) ;		\
		if (plisten) {										\
			messageHandler = (Protocol::MessageHandlerBase*) plisten->GetUserObj() ; \
		}	\
	}else {		\
		pObj = htoConnector((nd_handle) _h);				\
		messageHandler = (Protocol::MessageHandlerBase*) pObj->GetUserObj() ;	\
	}			\
	if (!messageHandler) { \
		nd_logdebug("message function error message handler is null!\n" ) ;\
		return 0;			\
	}	\
	Protocol:: _DataType input_data ;	\
	NDIStreamMsg inmsg(_msg) ;						\
	if(-1== Protocol::ReadStream(inmsg, input_data ) ) {\
		nd_logdebug("read stream data error \n" ) ;\
		return 0 ;\
	}	\
	nd_assert(messageHandler) ;	\
	return messageHandler->Handler##_MESSAGE_NAME(pObj, inmsg, input_data) ;	\
}while(0)


#define CALL_APOLLO_FUNCTION_NO_PARAM_ENTRY(_h, _msg,_MESSAGE_NAME) \
do {							\
	NDObject *pObj = 0 ;		\
	Protocol::MessageHandlerBase *messageHandler = NULL ;	\
	if(((nd_netui_handle)_h)->is_session) {					\
		pObj =  NDGetSession( (nd_handle)_h) ;				\
		NDListener *plisten = NDGetListener(h_listen) ;		\
		if (plisten) {										\
			messageHandler = (Protocol::MessageHandlerBase*) plisten->GetUserObj() ; \
		}		\
	}else {		\
		pObj = htoConnector((nd_handle) _h);				\
		messageHandler = (Protocol::MessageHandlerBase*) pObj->GetUserObj() ;	\
	}						\
	if (!messageHandler) { 	\
		nd_logdebug("message function error message handler is null!\n" ) ;\
		return 0;			\
	}						\
	NDIStreamMsg inmsg(_msg) ;		\
	return messageHandler->Handler##_MESSAGE_NAME(pObj, inmsg) ;	\
}while(0)
*/

//APOLLO_INSTALL_ENTRY(USER_LOGIN_REQUEST,h);
#define APOLLO_INSTALL_ENTRY(_MSG_ID, _h) \
do { \
	int maxid = ((_MSG_ID) >> 8) & 0xff; \
	int minid = _MSG_ID & 0xff; \
	nd_msgentry_install(_h, _MSG_ID##_entry, maxid, minid, EPL_LOGIN, #_MSG_ID); \
}while(0)

#endif
