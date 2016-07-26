/* file MessageName.cpp
 *
 * define message name 
 *
 * create by duan 
 * 2015-5-210
 */
#include "dataStream.h"


namespace NetMessage
{
	struct MessageNameInfo {
		NDUINT32 id;
		const char *name;
	};
#undef _APOLLO_MSG_ID_DEFINE
#define _APOLLO_MSG_ID_DEFINE(_aName, _aVal) {_aVal, #_aName},
	
	MessageNameInfo _nameBuf[] = {
#include "_msgid_export.h"
	};
#undef _APOLLO_MSG_ID_DEFINE

	
	const char *getNameByID(NDUINT32 id)
	{
		for (size_t i = 0; i < ND_ELEMENTS_NUM(NetMessage::_nameBuf); i++)
		{
			if (NetMessage::_nameBuf[i].id == id)	{
				return NetMessage::_nameBuf[i].name;
			}
		}
		return NULL;
	}
	NDUINT32 getIDByName(const char *name)
	{
		for (size_t i = 0; i < ND_ELEMENTS_NUM(NetMessage::_nameBuf); i++)
		{
			if (0==ndstricmp(NetMessage::_nameBuf[i].name, name))	{
				return NetMessage::_nameBuf[i].id;
			}
		}
		return NULL;
	}

}
