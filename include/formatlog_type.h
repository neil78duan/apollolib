/* file  formatlog_type.h
 *
 * define format log data type 
 *
 * create by duan
 *
 * 2015-11-30
 */

#ifndef _FORMATLOG_TYPE_H_
#define _FORMATLOG_TYPE_H_

enum eFormatLogType 
{
	ELOGTYPE_ITEM ,
	ELOGTYPE_ATTR ,
	ELOGTYPE_LOGIN ,
	ELOGTYPE_ENTER_MAP,
	ELOGTYPE_UNKNOWN
};

enum eFormatLogOperate
{
	ELOG_OP_NONE,
	ELOG_OP_ADD,
	ELOG_OP_SUB,
	ELOG_OP_SET,
	ELOG_OP_UNKNOWN
};
#endif // !_FORMATLOG_TYPE_H_
