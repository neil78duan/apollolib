//
//  msg_def.h
//  dbManager
//
//  Created by duanxiuyun on 14-9-30.
//  Copyright (c) 2014年 duanxiuyun. All rights reserved.
//

#ifndef dbManager_msg_def_h
#define dbManager_msg_def_h

#include "nd_msg.h"

//主消息号
enum APOLLO_MAX_MSG {
    NETMSG_MAX_LOGIN = ND_MAIN_ID_LOGIN ,       //1 登录消息
    
    NETMSG_MAX_SERVER,                          //服务器之间通信
		
    NETMSG_MAX_NUMBER
};

// 登录消息
enum APOLLO_LOGIN_MSG {
    LOGIN_MSG_LOGIN_REQ ,   //请求登录
    LOGIN_MSG_LOGIN_ACK ,   //登录回复
	LOGIN_MSG_RELOGIN_REQ , // format : ras_public_crypt(accindex + size + TEA(login_token_info) + teakey)
	LOGIN_MSG_CREATE_REQ ,	//创建账号
	LOGIN_MSG_CREATE_ACK ,	//创建账号
	
	LOGIN_MSG_SELECT_SERVER_REQ, //from account enter gameserver, format : ref relogin
	LOGIN_MSG_SELECT_SERVER_ACK,
	LOGIN_MSG_JUMPTO_SERVER_NTF, //notify client jump-to gameserver from current-gameserver
	LOGIN_MSG_JUMPTO_SERVER_REQ,
	LOGIN_MSG_JUMPTO_SERVER_ACK,
	
	
	LOGIN_MSG_LOGOUT_REQ , //登出请求(client request logout)
	LOGIN_MSG_LOGOUT_NTF , //登出通知
	
	LOGIN_MSG_GET_HOST_LIST, //get host list , return 16bits:number + host info
	
	LOGIN_MSG_GAMESERVER_TO_ACCOUNT_LOGOUT , // 

	LOGIN_MSG_GETACCOUNT_OF_THIS_DEVICE_REQ, //查找这个设备的账号 format: udid-string
	LOGIN_MSG_GETACCOUNT_OF_THIS_DEVICE_ACK, //返回这个设备的账号 32bits(0success: error)+8bit:account_type + text:account_name
	LOGIN_MSG_ACCOUNT_BIND_REQ,			//绑定账号,输入用户名和密码 32bit: account_id + text:name + text:password 
	LOGIN_MSG_ACCOUNT_BIND_ACK,			//绑定返回: 32bits:accountid + 32bits:(0 success :error_code) + text:name
	LOGIN_MSG_DEL_ACCOUNT_REQ,		//删除游客账号 32bits:accountid 
	LOGIN_MSG_DEL_ACCOUNT_ACK,		//删除游客账号 32bits:accountid + 32bits:(0 success :error_code)

	LOGIN_MSG_CLIENT_LANGUAGE_INFO, //发送客户的端的时区和语言信息 8bits:timezone + text:language 
	

	LOGIN_MSG_GET_ROLE_LIST_REQ,//Get role list request : format :8BITS:isRelogin
	LOGIN_MSG_GET_ROLE_LIST_ACK, // format : (role id,role1: name + attribte + package)

	LOGIN_MSG_CREATE_ROLE_REQ, //forma: string: rolename
	LOGIN_MSG_CREATE_ROLE_ACK, //format : 32bit:role-id + string:name + bin:attribute + package

	LOGIN_MSG_ROLE_ON_READY,

	LOGIN_MSG_GET_HOST_LIST_IN_GROUP, //get host list , return 16bits:number + host info 
    LOGIN_MSG_NUMBER 
};


enum ATLANTIS_SERVER_MSG {
    SERVER_MSG_REGISTER_REQ, 
    SERVER_MSG_REGISTER_ACK,
	SERVER_MSG_UNREGISTER,

	SERVER_MSG_ADD_FRIENDS,	
	SERVER_MSG_REQUEST_RUN_SQL, 
	
	SERVER_MSG_GETVERSION , //get server format: 8bit servertype (eServerType)
	
	SERVER_MSG_USERDATA_SYNC,
	
	SERVER_MSG_GET_ONLINE_NUMBER, //get total online player numbers 
	SERVER_MSG_GET_NUMBER_PER_HOST, //get online number per-host
	
	//SERVER_MSG_SYNC_ADDITION_DATA, //sync player addition data format: 32bit playerid + bin data
	
	SERVER_MSG_SYNC_COMMON_DATA, //8bit(cmd 0save,1 load,2 create)+ 64bit user-param, 32bit id, 32bit type, bin data
	SERVER_MSG_LOAD_COMMON_DATA_BYTYPE, //32BIT type
	
	SERVER_MSG_FORMAT_LOG ,
	SERVER_MSG_TEXT_LOG,
	SERVER_MSG_GET_INVITE_CODE , // get invite code format: 32bits:accountid + 32bit: roleid
	SERVER_MSG_INPUT_INVITE_CODE ,// input invite code format: 32bits:accountid + 32bit: roleid + text: invitecode
	
	SERVER_MSG_UPDATE_PVP_DATA, //NetMessage::RolePvpDetail 

	SERVER_MSG_GET_OTHER_ROLE_DATA, //int32:role-id, int8:field-type, int16:callback-msg, int64:callback_param
	SERVER_MSG_CLIENT_SESSION_SYNC,//notify worldserver or social server client session changed after the connector re-connect

	SERVER_MSG_TEST_DISCONNECT1 , //test disconnect between game and world 
	SERVER_MSG_TEST_DISCONNECT2,  //test disconnect between game and social
	SERVER_MSG_GM_ADD_NOTICE_REQ,	  //gm send notice msg
	SERVER_MSG_GM_ADD_NOTICE_ACK,	  //gm send notice msg ack

	SERVER_MSG_GM_FORBIDLIST_REQ,	  //gm get forbidlist req
	SERVER_MSG_GM_FORBIDLIST_ACK,	  //gm get forbidlist ack
	SERVER_MSG_GM_FORBIDINFO_SYC,	  //gm forbidinfo syc
	SERVER_MSG_GM_SENDMAIL,			  //gm send mail
	SERVER_MSG_GM_SENDCHAT,		      //gm send chat
	SERVER_MSG_GM_GETSERVERSTATUS_REQ,    //gm get server status req
	SERVER_MSG_GM_GETSERVERSTATUS_ACK,    //gm get server status ack
	SERVER_MSG_GM_SETSERVERSTATUS_REQ,    //gm set server status req
	SERVER_MSG_GM_SETSERVERSTATUS_ACK,    //gm set server status ack
	SERVER_MSG_GM_OPER_ACK,			      //gm common aper ack
	SERVER_MSG_SERVER_INFO_RENEW,		//RENEW the game server info (capacit and online number)
	//SERVER_MSG_ERROR_DIRECT_TO_CLIENT,	// from account or world send error code to client 
	SERVER_MSG_NUMBER
};
// 
// //role message
// enum APOLLO_ROLE_MSG {
// 	ROLE_MSG_GET_ROLE_LIST_REQ,
// 	ROLE_MSG_GET_ROLE_LIST_ACK, // format : (role id,role1: name + attribte + package)
// 	
// 	ROLE_MSG_CREATE_ROLE_REQ, //forma: string: rolename
// 	ROLE_MSG_CREATE_ROLE_ACK , //format : 32bit:role-id + string:name + bin:attribute + package
// 	
// };


#endif
