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
#include "nd_net/nd_netlib.h"
#include "nd_crypt/nd_crypt.h"
#endif 


#define USER_PASSWORD_SIZE  128		//password size or token size
#define ACCOUNT_NAME_SIZE   100
#define USER_NAME_SIZE      40
#define INVITE_CODE_SIZE 	33
#define COMMON_DATA_NAME_SIZE 60

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
	ESERVER_WORLD,	// WorldServer
	ESERVER_GAMEHALL,   // 游戏大厅
	ESERVER_GATE,		// 游戏入口
	ESERVER_BATTLE_ROOM,// 对战房间
	ESERVER_SOCIAL,		// SOCIAL SERVER
	ESERVER_ACCOUNT,	//
	ESERVER_GM,			// GM Server节点
	ESERVER_NATION,			// 国家工会

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
	ACC_ANYSDK,		//any sdk
	
	ACC_NUMBER ,
	ACC_UC =255,
	ACC_SKIP_AUTH = 0xff,
};

enum eChatType {
	ECHAT_COMMON,
	ECHAT_PRIVATE,
	ECHAT_BROADCAST,
	ECHAT_NATION,
	ECHAT_GUILD,
	ECHAT_SYSTEM_ROLL,
	ECHAT_SYSTEM
};

struct login_token_info
{
        login_token_info() :acc_index(0), session_key(0), _reserved(0), create_tm(0)
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
                if (bufsize <= (int)sizeof(login_token_info) )	{
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
                if(bufsize <= 16) {
                    return 0 ;
                }

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
                if (bufSize <(size_t) getLength())	{
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
                if (size >(NDUINT32) bufSize)		{
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
	//NDUINT8 gender;
	NDUINT8 isAdult;
	NDUINT16 serverGroupId;
	NDUINT32 channel;				//渠道id
	NDUINT16 reserved;

	//NDUINT16  birth_year, birth_month, birth_day;
	NDUINT8 acc_name[ACCOUNT_NAME_SIZE];
	NDUINT8 nick[USER_NAME_SIZE];
	NDUINT8 passwd[USER_PASSWORD_SIZE];
	//NDUINT8 phone[PHONE_NUMBER_SIZE];
	//NDUINT8 email[EMAIL_SIZE];
	NDUINT8 udid[DEVICE_UDID_SIZE] ;
	NDUINT8 devDesc[DEVICE_UDID_SIZE];

	account_base_info()
	{
		type = 2 ;
		isAdult = 0 ;
		serverGroupId = 1;
		channel = 0;				//渠道id
		reserved = 0;

		acc_name[0] = 0;
		nick[0] = 0;
		passwd[0] = 0;
		udid[0] = 0;
		devDesc[0] = 0;
	}
	int WriteStream(NDOStreamMsg &omsg)
	{
		omsg.Write(udid);
		omsg.Write(type );
		omsg.Write(isAdult);

		omsg.Write(serverGroupId );
		omsg.Write(channel );				//渠道id
		omsg.Write(reserved);				//渠道id

		omsg.Write(acc_name);
		omsg.Write(nick);
		omsg.Write(passwd);
		omsg.Write(devDesc);
		omsg.Write("");	//email . for old version
		omsg.Write(serverGroupId); // for old version

		
		return 0;
	}

	int ReadStream(NDIStreamMsg &inmsg)
	{
		char tmp[128];
		if (-1 == inmsg.Read(udid, sizeof(udid))) { return -1; }
		if (-1 == inmsg.Read(type)) { return -1; }
		if (-1 == inmsg.Read(isAdult)) { return -1; }

		if (-1 == inmsg.Read(serverGroupId)) { return -1; }
		//if (-1 == inmsg.Read(channel)) { return -1; }				//渠道id
		eNDnetStreamMarker channelDataType = inmsg.PeekDataType();
		if (channelDataType == ENDSTREAM_MARKER_INT32){
			inmsg.Read(channel);
		}
		else if (channelDataType == ENDSTREAM_MARKER_INT16) {
			NDUINT16 tmpch = 0;
			inmsg.Read(tmpch);
			channel = tmpch;
		}
		else {
			return -1;
		}
		///end channel

		if (-1 == inmsg.Read(reserved)) { return -1; }				//reserved data 

		if (-1 == inmsg.Read(acc_name, sizeof(acc_name))) { return -1; }
		if (-1 == inmsg.Read(nick, sizeof(nick))) { return -1; }
		if (-1 == inmsg.Read(passwd, sizeof(passwd))) { return -1; }
		if (-1 == inmsg.Read(devDesc, sizeof(devDesc))) { return -1; }
		if (-1 == inmsg.Read(tmp, sizeof(tmp))) { return -1; }	//email . for old version
		if (-1 == inmsg.Read(serverGroupId)) { return -1; } // for old version

		return 0;
	}
};

#define BASE_ROLE_ATTR_NUM 20
struct role_base_info 
{
	roleid_t rid;
	char name[USER_NAME_SIZE];
	NDUINT16 num;
	struct attrs{
		NDUINT8 aid;
		float val;
	}attrs[BASE_ROLE_ATTR_NUM];

	role_base_info() : rid(0), num(0)
	{
		name[0] = 0;
	}
	void pushAttr(NDUINT8 id, float val)
	{
		if (num < BASE_ROLE_ATTR_NUM)	{
			attrs[num].aid = id;
			attrs[num].val = val;
		}
	}
};

struct invite_info
{
	NDUINT8 invited ; // had been invited
	NDUINT16 invite_num ; //invited players number
	NDUINT8 my_invite_code[INVITE_CODE_SIZE] ;
};

struct host_list_node {
	NDUINT16 port ;
	NDUINT16 max_number ;
	NDUINT16 cur_number ;
	NDUINT16 logic_group_id;
	NDUINT8 isdefault_entry;
	NDUINT8 isDebug;
	NDUINT32 version_id ;
	NDUINT8 inet_ip[20];
	NDUINT8 name[HOST_NAME_SIZE] ;

	host_list_node() 
	{
		port =0;
		max_number =0;
		cur_number=0;
		logic_group_id=0;
		isdefault_entry=0;
		isDebug=0;
		version_id=0;
		inet_ip[0] = 0;
		name[0]=0;
	}
	int WriteStream(NDOStreamMsg &omsg)
	{
		omsg.Write(port);
		omsg.Write(max_number);
		omsg.Write(cur_number);
		omsg.Write(logic_group_id);
		omsg.Write(isdefault_entry);
		omsg.Write(isDebug);
		omsg.Write(version_id);
		omsg.Write(inet_ip);
		omsg.Write(name);
		return 0;
	}

	int ReadStream(NDIStreamMsg &inmsg)
	{
		
		if (-1 == inmsg.Read(port)) {
			return -1;
		}

		if (-1 == inmsg.Read(max_number)) {
			return -1;
		}

		if (-1 == inmsg.Read(cur_number)) {
			return -1;
		}
		if (-1 == inmsg.Read(logic_group_id)) {
			return -1;
		}
		if (-1 == inmsg.Read(isdefault_entry)) {
			return -1;
		}
		if (-1 == inmsg.Read(isDebug)) {
			return -1;
		}
		if (-1 == inmsg.Read(version_id)) {
			return -1;
		}
                if (0 == inmsg.Read(inet_ip, sizeof(inet_ip))) {
			return -1;
		}
                if (0 == inmsg.Read(name, sizeof(name))) {
			return -1;
		}
		return 0;
	}

	int Stream2Data(const char *StreamBuf, size_t stream_size)
	{
		nd_usermsgbuf_t msgbuf;
		nd_usermsghdr_init(&msgbuf.msg_hdr);

		memcpy(msgbuf.data, StreamBuf, stream_size);
		ND_USERMSG_LEN(&msgbuf) += (NDUINT16)stream_size;
		NDIStreamMsg inmsg(&msgbuf);

		return ReadStream(inmsg);
	}

	int Data2Stream(char *StreamBuf, size_t size)
	{
		NDOStreamMsg omsg(0, 0);

		if (WriteStream(omsg) == -1) {
			return -1;
		}
		if (omsg.GetDataLen() > size) {
			return -1;
		}
		int data_len = (int)omsg.GetDataLen();
		memcpy(StreamBuf, omsg.MsgData(), data_len);

		return (int)data_len;
	}
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

typedef userdata_info addition_data_info;
// 
// struct addition_data_info
// {
// 	addition_data_info() {
// 		size = 0;
// 		data[0] = 0;
// 
// 	}
// 	addition_data_info &operator =(const addition_data_info &r)
// 	{
// 		size = NDMIN(r.size, sizeof(data));
// 		memcpy(data, r.data, size);
// 		return  *this;
// 	}
// 
// 	addition_data_info &operator =(const userdata_info &r)
// 	{
// 		size = NDMIN(r.size, sizeof(data));
// 		memcpy(data, r.data, size);
// 		return  *this;
// 	}
// 
// 	void setData(void *indata, size_t len)
// 	{
// 		size = (NDUINT32) NDMIN(len, sizeof(data));
// 		memcpy(data, indata, size);
// 	}
// 
// 	size_t get_stream_len() const
// 	{
// 		return sizeof(addition_data_info) - sizeof(data) + size;
// 	}
// 
// 	NDUINT32 size;
// 	char data[ADDITION_DATA_SIZE];
// };



#endif
