/* file script_event_id.h
 *
 * define event id for script
 *
 * create by duan 
 *
 * 2015-5-29
 */

#ifndef _SCRIPT_EVENT_H_
#define _SCRIPT_EVENT_H_


#ifdef _APOLLO_DEFINE_EVENT_VAL 
#undef _APOLLO_DEFINE_EVENT_VAL
#endif 
#define _APOLLO_DEFINE_EVENT_VAL(enum_name, _id, _desc) APOLLO_##enum_name = _id ,


#ifdef _APOLLO_DEFINE_EVENT 
#undef _APOLLO_DEFINE_EVENT
#endif 
#define _APOLLO_DEFINE_EVENT(_id, _comment) _id,
enum eApolloEventID {
	#include "logic_parser/_logicEventId.h"
	#include "_event_def.h"
	APOLLO_EVENT_NUMBER
} ;

#undef _APOLLO_DEFINE_EVENT_VAL

#undef _APOLLO_DEFINE_EVENT
/*

static inline const char **apo_get_events_name() 
{

#ifdef _APOLLO_DEFINE_EVENT 
#undef _APOLLO_DEFINE_EVENT
#endif 
#define _APOLLO_DEFINE_EVENT(_id, _comment) _comment,

	static const char *event_name[] = {
#include "logic_parser/_event_def.h"
	};
#undef _APOLLO_DEFINE_EVENT
	return event_name;
}*/

#endif
