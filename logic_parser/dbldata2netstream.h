/* file dbldata2netstream.h
 *
 * convert dbldata to net message stream 
 *
 * create by duan 
 * 2015-5-20
 *
 */

#ifndef _DBLDATA_2_STREAM_H_
#define _DBLDATA_2_STREAM_H_

//#include "ndapplib/applib.h"
#include "nd_common/nd_common.h"
#include "ndapplib/nd_msgpacket.h"
#include "logicDataType.h"
#include "logicStruct.hpp"

#include <map>
#include <string>

int logicDataWrite(DBLDataNode &data, NDOStreamMsg &omsg); //return 0 success ,else return -1
int logicDataRead(DBLDataNode &data, NDIStreamMsg &inmsg); //return 0 success else return -1

typedef std::map<std::string, LogicUserDefStruct> userDefineDataType_map_t;

int loadUserDefFromMsgCfg(const char *msgfile,  userDefineDataType_map_t &userDataRoot);
void destroyUserDefData(userDefineDataType_map_t &userDataRoot);
void dumpMessageData(userDefineDataType_map_t &userDataRoot);
int get_type_from_alias(const char *name);

//int testFormatMsgRead(userDefineDataType_map_t &userDataRoot);

//build data struct from message define-body such as "int16_t: number, [int32:mailid, MailInfo:mail_body]"
//bool buildUserDefFromText(DBLDataNode &dataType, const char *text, userDefineDataType_map_t &userDataRoot);

#endif
