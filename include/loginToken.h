/* file loginToken.h
 *
 * define token for login
 *
 * create by duan 
 *
 * 2019.1.29
 */

#ifndef _APOLLO_LOGIN_TOKER_H_
#define _APOLLO_LOGIN_TOKER_H_

#include "apollo_data.h"
#include "nd_net/nd_netlib.h"

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
		if (bufsize <= (int)sizeof(login_token_info)) {
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
		if (bufsize <= 16) {
			return 0;
		}

		acc_index = nd_netstream_to_long(buf);
		buf += 4;
		session_key = nd_netstream_to_short(buf);
		buf += 2;
		_reserved = nd_netstream_to_short(buf);
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
		if (bufSize < (size_t)getLength()) {
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
		if (bufSize < 0) {
			return -1;
		}

		acc_index = nd_netstream_to_long(buf);
		buf += 4;
		memcpy(udid, buf, sizeof(udid));
		buf += sizeof(udid);
		nd_teaKeyToHostorder(&new_key, (tea_k*)buf);
		buf += sizeof(new_key);
		size = nd_netstream_to_long(buf);
		buf += 4;
		if (size > (NDUINT32)bufSize) {
			return -1;
		}
		memcpy(session_buf, buf, size);
		buf += size;

		return (int)(buf - src);
	}
};


#endif
