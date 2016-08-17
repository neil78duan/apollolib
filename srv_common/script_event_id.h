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

#ifdef _APOLLO_DEFINE_EVENT 
#undef _APOLLO_DEFINE_EVENT
#endif 
#define _APOLLO_DEFINE_EVENT(_id, _comment) _id,
enum eApolloEventID{
	#include "srv_common/_event_def.h"
	APOLLO_EVENT_NUMBER
} ;

#undef _APOLLO_DEFINE_EVENT
#endif
