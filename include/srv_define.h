//
//  srv_define.h
//  dbManager
//
//  Created by duanxiuyun on 14-9-29.
//  Copyright (c) 2014 duanxiuyun. All rights reserved.
//

#ifndef dbManager_srv_define_h
#define dbManager_srv_define_h

#include "nd_crypt/nd_crypt.h"
#include "apollo_data.h"
#include "ndapplib/nd_msgpacket.h"

#include <stdlib.h>
#include <string.h>

#define HOST_ID_MAKE(_ip, _port, session_id)  	ND_MAKE_QWORD(_ip, ND_MAKE_DWORD(_port, session_id) )
#define HOST_ID_GET_IP(_host_id) 				ND_HIDWORD(_host_id)
#define HOST_ID_GET_PORT(_host_id) 				((_host_id)>>16 & 0xffff)
#define HOST_ID_GET_SESSION(_host_id) 			((_host_id) & 0xffff)

typedef NDUINT32 thread_msg_id ;

#define RAND_DATA_SIZE_IN_DB 0x20000
//save session info after user login success
//#pragma pack(push , 1)
struct login_token_info
{
	login_token_info() :acc_index(0), session_key(0),create_tm(0),_reserved(0)
	{}
	account_index_t acc_index;
	NDUINT16 session_key;
	NDUINT16 _reserved ;
	time_t create_tm ;
	tea_k sym_key;

};
//struct of transfer when relogin
struct transfer_session_key
{
	transfer_session_key() : acc_index(0), size(0)
	{
		
	}
	account_index_t acc_index ;
	tea_k new_key;
	NDUINT32 size ;
	char session_buf[1024] ;
};
//#pragma pack(pop)


//session info in world server
struct user_sessions {
	user_sessions() :session(0),session_in_game(0),serverID(0)
	{
		
	}
	user_sessions& operator=(const user_sessions &r) {
		serverID = r.serverID;
		session = r.session;          //
		session_in_game = r.session_in_game;	//
		return *this;
	}
	hostid_t serverID ;
	NDUINT16 session ;          //
	NDUINT16 session_in_game ;	//
	//NDUINT16 session_in_room ;	//
	
};

//define account info using in accountlogin server
struct account_data
{
	account_data() {
		memset(this, 0, sizeof(account_data) );
	}
	account_index_t accIndex ;
	int status ; //ref eGameStat
	
	ndtime_t login_tm;
	
	account_base_info base ;
	
	//crypt_info crypt ;
	login_token_info login_token ;
	
};

struct base_role_info
{
	roleid_t rid ;
	char name[USER_NAME_SIZE] ;
};

struct role_in_world
{
	role_in_world() :rid(0),accid(0),isLoad(0),stat(0)
	{
		name[0] = 0 ;
	}
	
	role_in_world& operator=(const role_in_world &r)
	{
		rid = r.rid ;
		accid = r.rid ;
		isLoad = r.isLoad ;
		strncpy(name, r.name, sizeof(name)) ;
		attr_data = r.attr_data ;
		package = r.package ;
		dragon = r.dragon;
		stat = r.stat ;
		level = r.level ;
		pvpPower = r.pvpPower ;
		sessInfo = r.sessInfo;
		return *this;
	}
	roleid_t rid ;
	account_index_t accid ;
	NDUINT8 isLoad ;
	NDUINT8 stat ;
	NDUINT8 level;
	NDUINT32 pvpPower ; // pvp fighting capacity
	char name[USER_NAME_SIZE] ;
	userdata_info attr_data ; // attribute data
	userdata_info package ;
	userdata_info dragon;
	
	user_sessions sessInfo ;
};

//account info using in game/world server
struct account_in_world
{
	account_in_world() {
		memset(this, 0, sizeof(account_in_world) );
	}
	account_index_t accIndex ;
	ndtime_t login_tm;
	roleid_t using_role ;  //current select role
	base_role_info roles[ROLES_PER_ACCOUNT] ;
	
	NDUINT8 accType ;
	NDUINT16 session_key ;
	user_sessions sessInfo;
	tea_k crypt_key ;
	char name[ACCOUNT_NAME_SIZE] ;
};

//struct for load data from db
struct rankPvpBufDb
{
	rankPvpBufDb() {
		rid = 0 ;
		name[0] = 0 ;
	}
	size_t getSize() {
		return sizeof(rankPvpBufDb) - sizeof(data) + data.get_stream_len() ;
	}
	roleid_t rid ;
	NDUINT32 pvePower ; // pvp level
	NDUINT8 level ;
	char name[USER_NAME_SIZE] ;
	userdata_info data ; // attribute data
	
};

/*
struct rankData
{
	roleid_t rid ;
	NDUINT16 rankIndex;
	NDUINT16 dataSize ;
	char name[USER_NAME_SIZE];
	char *dragonData ;
	
	rankData()
	{
		rid =0;
		name[0]=0;
		dataSize = 0;
		dragonData = 0;
		rankIndex = 0;
	}
	rankData &operator =(const rankData &r)
	{
		rankIndex = r.rankIndex ;
		rid =r.rid;
		strncpy(name, r.name, sizeof(name)) ;
		
		if(dragonData && dataSize==r.dataSize){
			free(dragonData) ;
			dragonData = NULL;
		}
		dataSize = r.dataSize;
		
		if(dataSize && r.dragonData) {
			if(!dragonData) {
				dragonData = (char*) malloc(dataSize +1);
				if(!dragonData) {
					return *this;
				}
			}
			memcpy(dragonData, r.dragonData, dataSize) ;
		}
		else {
			dataSize = 0 ;
		}
		return *this ;
	}
	bool setData(userdata_info &dragon)
	{
		if(dragonData && dragon.size != dataSize) {
			free(dragonData) ;
			dragonData = NULL;
		}
		
		if(dragon.size > 0) {
			if(!dragonData) {
				dragonData = (char*) malloc(dragon.size +1);
				if(!dragonData) {
					return false;
				}
				
			}
			memcpy(dragonData, dragon.data, dragon.size) ;
		}
		dataSize = dragon.size ;
		return true ;
	}
	int toStream(NDOStreamMsg &omsg)
	{
		omsg.Write(rankIndex) ;
		omsg.Write((NDUINT32)rid) ;
		if(rid) {
			omsg.Write(name) ;
			omsg.WriteBin(dragonData, dataSize) ;
		}
		return 0;
	}
	int fromStream(NDIStreamMsg &inmsg)
	{
		char buf[4096] ;
		inmsg.Read(rankIndex) ;
		inmsg.Read((NDUINT32&)rid) ;
		if(rid) {
			inmsg.Read((NDUINT8*)name,sizeof(name)) ;
			size_t size = inmsg.ReadBin(buf, sizeof(buf)) ;
			if(size > 0) {
				dragonData = (char*) malloc(size +1);
				memcpy(dragonData, buf, size) ;
				dataSize = size ;
			}
		}
		
		return 0;
			
	}
	
	~rankData()
	{
		if(dragonData ) {
			free(dragonData) ;
			dragonData = 0 ;
		}
		
	}
	
};
*/

struct game_ras_key
{
	int pub_size ;
	char pub_key_md5[16] ;
	char pub_key[2048] ;
	R_RSA_PRIVATE_KEY key ;
};

enum eSaveRoleBlockDataField
{
	E_ROLE_ATTR_FIELD ,
	E_ROLE_PACKAGE_FIELD ,
	E_ROLE_DRAGON_FIELD ,
	
	E_ROLE_PVP_POWER,
	E_ROLE_UNKNOW_FIELD
};

//define ROLE DATA NAME
#define ROLE_DATA_VERSION_NAME "version"
#define ROLE_ATTR_NAME "attr"
#define ROLE_EQUIP_NAME "equip"
#define ROLE_PACAKGE_NAME "package"
#define ROLE_SKILL_NAME "skill"
#define ROLE_SKILL_PACKAGE "skill_pack"
#define ROLE_PET_NAME	"pet"
#define ROLE_DATA_MAP_SCORE "map_score"
#define	ROLE_DATA_LOTTERY_LOG "lottery"
#define ROLE_DROP_WEIGHT "drop_weight"
#define ROLE_SHOP_INFO "shop"
#define ROLE_ATTR_SHOP "attrStore"
#define ROLE_DATA_DRAGON "dragon"
#define ROLE_DAILY_UPDATE_INFO "daily_timer" 
#define ROLE_DATA_TASK "task"
#define ROLE_DATA_TASK_LOG "task_log"
#define ROLE_DATA_ACHIEVEMENT "achiev"

#define ROLE_WEAPON_NAME "weapon"
// addition data
#define ROLE_ADDITION_NAME "addition"
#define ADDITION_DATA_SCENE "scene"
#define ADDITION_DATA_LAST_LOGOUT "last_logout"
#define ADDITION_DATA_SIGN_IN_TM "sign_tm"
#define ADDITION_DATA_LOGIN_AWARD_TM "login_awards_tm"

#define ADDITION_DATA_CLIENT_SETTING "cli_info"
#define ADDITION_ITEM_CREATE_INDEX "item_auto_index" //in addition data
//#define ADDITION_PVP_SELF_RANK "pvp_rank" //in addition data

//define daily update object name
#define DAILY_RENEW_SHOP "shop"

#define ROLE_ATTR_LEVEL_ID  (attrid_t)1
#define ROLE_ATTR_EXP_ID  (attrid_t)2
#define ROLE_ATTR_GAME_MONEY_ID  (attrid_t)3
#define ROLE_ATTR_HP_ID  "HP"
#define ROLE_ATTR_MAXHP_ID  "MaxHP"
#define ROLE_ATTR_VIP "VIP"


#endif
