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

#include "nd_common/nd_common.h"
#include "ndapplib/nd_msgpacket.h"
#include "logic_parser/logicDataType.h"
#include "logic_parser/logicStruct.hpp"
#include "logic_parser/logicEngineRoot.h"

#include <map>
#include <string>
#include <vector>

 //user define data type , load from datatype.xml
typedef std::map<std::string, LogicUserDefStruct> userDefineDataType_map_t;

bool InitLoadNetMsgFormat(const char* msgDataTypeXml);
userDefineDataType_map_t &getGlobalNetMsgFormat();
void DestroyNetMsgFormat();


int logicDataWrite(LogicDataObj &data, NDOStreamMsg &omsg); //return 0 success ,else return -1
int logicDataRead(LogicDataObj &data, NDIStreamMsg &inmsg); //return 0 success else return -1

void dumpMessageData(userDefineDataType_map_t &userDataRoot,const char *outFile=NULL);
int get_type_from_alias(const char *name);

int UserDefFormatToMessage(userDefineDataType_map_t &userDataRoot, NDOStreamMsg &omsg);
int UserDefFormatFromMessage(userDefineDataType_map_t &userDataRoot, NDIStreamMsg &inmsg);


bool LogicOutputMsgByFormat(logic_print print_func, void *log_file, const char *formatText, NDIStreamMsg &inmsg, userDefineDataType_map_t &dataDef);


#endif
