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
	
	NETMSG_MAX_ROLE ,							//create role message ...
	
	//NETMSG_MAX_SOCIAL ,							//社交 chat, friend ...
	
	//NETMSG_MAX_ROOM ,							//对战房间消息-send to gameHall
	
	//NETMSG_MAX_BATTLE ,							//对战消息 - send to battleServer
    
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
	SERVER_MSG_NUMBER 
};

//role message
enum APOLLO_ROLE_MSG {
	ROLE_MSG_GET_ROLE_LIST_REQ,
	ROLE_MSG_GET_ROLE_LIST_ACK, // format : (role id,role1: name + attribte + package)
	
	ROLE_MSG_CREATE_ROLE_REQ, //forma: string: rolename
	ROLE_MSG_CREATE_ROLE_ACK , //format : 32bit:role-id + string:name + bin:attribute + package
	
};

//enum ATLANTIS_SOCIAL_MSG {
//	SOCIAL_MSG_PLAYERS_LIST_REQ,
//	SOCIAL_MSG_PLAYERS_LIST_ACK,// format : 16bits:number + friends1 (id + gender+level+birth_year/month/day + name) + friends2...
//	
//	SOCIAL_MSG_CHAT_REQ , // message format : 8bits:type +32BITS  aim-playerID + bin: chat data
//	SOCIAL_MSG_CHAT_NTF , // message format : 8bits:type +32BITS : speaker_id + text:speaker_name + bin: chat data
//	
//	SOCIAL_MSG_GET_FRIENDS_REQ ,	// format : 32bits: friends-id (0 get all friends)
//	SOCIAL_MSG_GET_FRIENDS_ACK , // format : 16bits:number + friends1 (id + gender+level+birth_year/month/day + name) + friends2...
//	
//	SOCIAL_MSG_ADD_FRIENDS_REQ , // message format :32BITS:playerid  + text:request-desc
//	SOCIAL_MSG_REQUEST_FRIEND_NTF , // message format :32BITS: request-player-id +32bit:request-key + text:player-name + text: request-desc
//	SOCIAL_MSG_ADD_FRIEND_REPLY , // message format : 8BITS:YES/NO + 32BITS: request-player-id + 32bit: key
//	SOCIAL_MSG_ADD_FRIEND_ACK , // message format : 8BITS:YES/NO + 32BITS: + text:not agree reason
//	
//	SOCIAL_MSG_MAKE_FRIEND_NTF , // message format :  32BITS: New friend id
//	
//	SOCIAL_MSG_DEL_FRIEND_REQ,	//format : 32bits:player_id
//	SOCIAL_MSG_DEL_FRIEND_ACK, // format : 32bits:player_id + 8bits: yes/no
//	
//	SOCIAL_MSG_BRAODCAST_REQ, 	// Only for test
//	SOCIAL_MSG_BRAODCAST_NTF ,
//	
//	SOCIAL_MSG_NUMBER
//};
//
//
////NETMSG_MAX_ROOM ,							//对战房间消息 
//enum ATLANTIS_ROOM_MSG {
//	ROOM_MSG_CREATE_REQ ,	// format :32bits:mapID + 16bits:player-number +8bits:is-open ＋[text:name+text:password]
//	ROOM_MSG_CREATE_ACK ,	// format :32bits:errorcode + roominfo[32bit:host-ip +16bits:port + 32bits:roomid + 32bit:key +mapid]
//	ROOM_MSG_INVITE_REQ ,	//invite friend ,format :playerid + roomid + key
//	ROOM_MSG_INVITED_NTF ,	//notified aim-player format: 32bits:requestPlayer + text:name+ roominfo[32bit:host-ip +16bits:port + 32bits:roomid + 32bit:key +mapid]
//	ROOM_MSG_INVITED_REPLY,// format: 8bits:yes/no + 32bits:requestPlayer  + roomid
//	ROOM_MSG_INVITED_ACK,// format: 8bits:yes/no + 32bits:aimPlayer  + roomid
//	
//	ROOM_MSG_GET_LIST_REQ,
//	
//	
//	//only send to worldserver
//	ROOM_MSG_READY,
//	ROOM_MSG_START_GAME,
//	ROOM_MSG_REQUEST_ROOM_INFO,
//	ROOM_MSG_GAME_OVER ,	
//	
//	ROOM_MSG_NUMBER
//};
//
////NETMSG_MAX_BATTLE ,							//对战消息
//enum ATLANTIS_BATTLE_MSG {
//
//	BATTLE_MSG_ENTER_REQ ,					// wrapped by BATTLE_MSG_ENTER_REQ :format : userid+ 16bits:session-key + 32bits:room + 32bits:key [+ text:password]
//	BATTLE_MSG_ENTER_ACK,					// format : 32bits:errorcode [ 32bits:roomID + 32bits: mapID + roomName]
//
//	
//	BATTLE_MSG_PLAYER_ENTER_NTF ,			// format : roomID + playerID + text: playername
//	BATTLE_MSG_PLAYER_LEAVE_NTF ,			// format : roomID + playerID 
//	
//	BATTLE_MSG_GET_CUR_ROOM_REQ ,			//get Current RoomInfo
//	BATTLE_MSG_GET_CUR_ROOM_ACK ,			//Back roominfo :errorcode + roomid+ mapid+  16bits:player_num[ player1(id +name) + player2() ]
//	
//	BATTLE_MSG_START_GAME_REQ ,				//start 
//	BATTLE_MSG_START_GAME_NTF ,				// notify game start : roomid
//	
//	BATTLE_MSG_BROAD_NTF , 					//broad cast message to all users in current room
//	
//	BATTLE_MSG_NUMBER
//};


#endif
