/* please do not change this file ,
 * auto create by program 
 * create time 2015-4-23 16:14:13 
 */

#ifndef _AUTO_MESSAGE_ID_H_
#define _AUTO_MESSAGE_ID_H_
//namespace Protocol
//{
#undef _APOLLO_MSG_ID_DEFINE
#define _APOLLO_MSG_ID_DEFINE(_aName, _aVal) _aName = _aVal, //_aMark
	enum MsgIDDefine {
#include "_msgid_export.h"
	};

#undef _APOLLO_MSG_ID_DEFINE

//};

namespace NetMessage
{
	const char *getNameByID(NDUINT32 id);
	NDUINT32 getIDByName(const char *name);
	
};

#endif
