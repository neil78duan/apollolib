/* 
 * file apollo_data.h 
 * 
 * data define for server and client 
 *
 * create by duan 
 *
 * 2015-6-18
 */

#ifndef _APOLLO_DATA_H_
#define _APOLLO_DATA_H_


#ifdef BUILD_AS_THIRD_PARTY
#include "ndlib.h"
#else
#include "nd_common/nd_common.h"
#include "nd_crypt/nd_crypt.h"
#endif 


#define USER_PASSWORD_SIZE  128		//password size or token size
#define ACCOUNT_NAME_SIZE   100
#define USER_NAME_SIZE      40
#define INVITE_CODE_SIZE 	33

#define OUTLINE_MSG_DATA_SIZE 1024
#define HOST_NAME_SIZE 64
#define PHONE_NUMBER_SIZE 20
#define EMAIL_SIZE 100
#define USER_DATA_SIZE 0x10000
#define ADDITION_DATA_SIZE 4096
#define DEVICE_UDID_SIZE 64

#define ENTER_ROOM_PASSWORD_SIZE 6

#define MAX_HOST_GROUP_NUMBER 32
#define ROLES_PER_ACCOUNT 1

#define MAIL_TITLE_SIZE 40 
#define MAIL_BODY_SIZE 1024

enum eGameStat {
	EGS_NONE = 0,
	EGS_LOGINOK,
	EGS_INGAME,

};

enum eServerType {
	ESERVER_UNKNOW = 0,
	ESERVER_MANAGER,	// WorldServer
	ESERVER_GAMEHALL,   // 游戏大厅
	ESERVER_GATE,		// 游戏入口
	ESERVER_BATTLE_ROOM,// 对战房间
	ESERVER_SOCIAL,		// SOCIAL SERVER
	ESERVER_ACCOUNT,	//
	ESERVER_GM,			// GM Server节点
};

enum eCommonDataSyncCmd
{
	ECOMMON_DATA_SAVE,
	ECOMMON_DATA_LOAD,
	ECOMMON_DATA_CREATE,
	ECOMMON_DATA_DEL,
};


typedef NDUINT32 account_index_t;
typedef account_index_t  accountid_t;
typedef account_index_t  roleid_t;
typedef NDUINT64 hostid_t;

typedef NDUINT32 itemInst_t ; //ITEM instant id
typedef NDUINT32 itemType_t ; //ITEM instant id
typedef NDUINT16 designTypeId_t;
typedef designTypeId_t skillId_t;
typedef designTypeId_t talentId_t;
//typedef NDUINT16 weaponId_t;
//typedef NDUINT16 dragonId_t;
typedef NDUINT32 taskId_t;

#define HOST_ID_MAKE(_ip, _port, session_id)  	ND_MAKE_QWORD(_ip, ND_MAKE_DWORD(_port, session_id) )
#define HOST_ID_GET_IP(_host_id) 				ND_HIDWORD(_host_id)
#define HOST_ID_GET_PORT(_host_id) 				((_host_id)>>16 & 0xffff)
#define HOST_ID_GET_SESSION(_host_id) 			((_host_id) & 0xffff)


enum ACCOUNT_TYPE{
	ACC_FACEBOOK = 0,
	ACC_UDID,
	ACC_APOLLO,
	ACC_GAME_CENTER,
	ACC_GOOGLE_PLAY,
	ACC_OTHER_3_ACCID,
	ACC_SKIP_AUTH,
	ACC_NUMBER
};

enum eChatType {
	ECHAT_COMMON,
	ECHAT_PRIVATE,
	ECHAT_BROADCAST,
	ECHAT_SYSTEM
};

struct login_token_info
{
	login_token_info() :acc_index(0), session_key(0), create_tm(0), _reserved(0)
	{
		udid[0] = 0;
	}
	account_index_t acc_index;
	NDUINT16 session_key;
	NDUINT16 _reserved;
	time_t create_tm;
	tea_k sym_key;
	NDUINT8 udid[DEVICE_UDID_SIZE];

	int toBuf(char *buf, int bufsize)
	{
		char *src = buf;
		if (bufsize <= sizeof(login_token_info) )	{
			return -1;
		}

		nd_long_to_netstream(buf, acc_index);
		buf += 4;
		nd_short_to_netstream(buf, session_key);
		buf += 2;
		nd_short_to_netstream(buf, _reserved);
		buf += 2;
		nd_longlong_to_netstream(buf, create_tm);
		buf += 8;
		
		nd_teaKeyToNetorder((tea_k*)buf, &sym_key);
		buf += sizeof(sym_key);

		memcpy(buf, udid, sizeof(udid));
		buf += sizeof(udid);

		return (int)(buf - src);
	}

	int fromBuf(char *buf, int bufsize)
	{
		char *src = buf;

		acc_index = nd_netstream_to_long(buf);
		buf += 4;
		session_key =nd_netstream_to_short(buf);
		buf += 2;
		_reserved =nd_netstream_to_short(buf);
		buf += 2;
		create_tm = nd_netstream_to_longlong(buf);
		buf += 8;

		nd_teaKeyToHostorder(&sym_key, (tea_k*)buf);
		buf += sizeof(sym_key);

		memcpy(udid, buf, sizeof(udid));
		buf += sizeof(udid);

		return (int)(buf - src);
	}
	
};
//struct of transfer when relogin
struct transfer_session_key
{
	transfer_session_key() : acc_index(0), size(0)
	{

	}
	account_index_t acc_index;
	NDUINT8 udid[DEVICE_UDID_SIZE];
	tea_k new_key;
	NDUINT32 size;
	char session_buf[1024];
	int getLength()
	{
		return sizeof(transfer_session_key) - sizeof(session_buf) + size;
	}
	int toBuf(char *buf, size_t bufSize)
	{
		if (bufSize < getLength())	{
			return -1;
		}
		char *src = buf;
		nd_long_to_netstream(buf, acc_index);
		buf += 4;
		memcpy(buf, udid, sizeof(udid));
		buf += sizeof(udid);
		nd_teaKeyToNetorder((tea_k*)buf, &new_key);
		buf += sizeof(new_key);
		nd_long_to_netstream(buf, size);
		buf += 4;
		memcpy(buf, session_buf, size);
		buf += size;

		return (int)(buf - src);
	}

	int fromBuf(char *buf, int bufSize)
	{
		char *src = buf;
		size = 0;
		bufSize -= getLength();
		if (bufSize<0)		{
			return -1;
		}

		acc_index= nd_netstream_to_long(buf);
		buf += 4;
		memcpy( udid,buf, sizeof(udid));
		buf += sizeof(udid);
		nd_teaKeyToHostorder(&new_key, (tea_k*)buf);
		buf += sizeof(new_key);
		size =nd_netstream_to_long(buf);
		buf += 4;
		if (size > bufSize)		{
			return -1;
		}
		memcpy(session_buf,buf, size);
		buf += size;

		return (int)(buf - src);
	}
};

struct account_base_info
{
	NDUINT8 type;
	NDUINT8 gender;
	NDUINT16  birth_year, birth_month, birth_day;
	NDUINT8 acc_name[ACCOUNT_NAME_SIZE];
	NDUINT8 nick[USER_NAME_SIZE];
	NDUINT8 passwd[USER_PASSWORD_SIZE];
	NDUINT8 phone[PHONE_NUMBER_SIZE];
	NDUINT8 email[EMAIL_SIZE];
	NDUINT8 udid[DEVICE_UDID_SIZE] ;
};

struct invite_info
{
	NDUINT8 invited ; // had been invited
	NDUINT16 invite_num ; //invited players number
	NDUINT8 my_invite_code[INVITE_CODE_SIZE] ;
};

struct host_list_node {
	ndip_t ip ;
	NDUINT16 port ;
	NDUINT16 max_number ;
	NDUINT16 cur_number ;
	NDUINT32 version_id ;
	NDUINT8 name[HOST_NAME_SIZE] ;
};

//#define CURRENT_DATA_VERSION 1
struct userdata_info
{
	userdata_info() {
		//version = CURRENT_DATA_VERSION;
		size = 0;
		data[0] = 0;

	}
	userdata_info &operator =(const userdata_info &r)
	{
		//version = r.version ;
		size = NDMIN( r.size , sizeof(data));
		memcpy(data, r.data, size) ;
		return  *this ;
	}
	size_t capacity() {
		return USER_DATA_SIZE;
	}
	void setData(void *indata, size_t len)
	{
		size =(NDUINT32) NDMIN(len, sizeof(data));
		memcpy(data, indata, size);
	}
	size_t get_stream_len() const
	{
		return sizeof(userdata_info) - sizeof(data) + size ;
	}
	
	NDUINT32 size;
	char data[USER_DATA_SIZE];
};

struct addition_data_info
{
	addition_data_info() {
		size = 0;
		data[0] = 0;

	}
	addition_data_info &operator =(const addition_data_info &r)
	{
		size = NDMIN(r.size, sizeof(data));
		memcpy(data, r.data, size);
		return  *this;
	}

	addition_data_info &operator =(const userdata_info &r)
	{
		size = NDMIN(r.size, sizeof(data));
		memcpy(data, r.data, size);
		return  *this;
	}

	void setData(void *indata, size_t len)
	{
		size = (NDUINT32) NDMIN(len, sizeof(data));
		memcpy(data, indata, size);
	}

	size_t get_stream_len() const
	{
		return sizeof(addition_data_info) - sizeof(data) + size;
	}

	NDUINT32 size;
	char data[ADDITION_DATA_SIZE];
};



#endif
