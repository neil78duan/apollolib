/* file dbldata2netstream.h
 *
 * convert dbldata to net message stream 
 *
 * create by duan 
 * 2015-5-20
 *
 */

#include "logic_parser/dbldata2netstream.h"
#include "logic_parser/logicStruct.hpp"
#include "attribute/roleattr_help.h"

static int _writeMsgToUserDef(const DBLDataNode &data, NDOStreamMsg &omsg)
{
	LogicUserDefStruct *pUserData = (LogicUserDefStruct *) data.getUserDef();
	nd_assert(pUserData);
	if (!pUserData){
		return 0;
	}
	for (size_t i = 0; i < pUserData->count(); i++)	{
		DBLDataNode *p = pUserData->ref(i);
		nd_assert(p);
		if (0!=logicDataWrite(*p,omsg) ){
			return -1;
		}
	}
	return 0;
}

static int _readMsgToUserDef(DBLDataNode &data, NDIStreamMsg &inmsg)
{
	LogicUserDefStruct *pUserData = (LogicUserDefStruct *)data.getUserDef();
	nd_assert(pUserData);
	if (!pUserData){
		return 0;
	}
	for (size_t i = 0; i < pUserData->count(); i++)	{
		DBLDataNode *p = pUserData->ref(i);
		nd_assert(p);
		if (0 != logicDataRead(*p, inmsg)){
			nd_logerror("msgtoUserDef: read %s error\n", pUserData->getName(i));
			return -1;
		}
	}
	return 0;
}

int logicDataWrite(DBLDataNode &data, NDOStreamMsg &omsg)
{
	int ret = 0;
	DBL_ELEMENT_TYPE type = data.GetDataType();
	DBL_ELEMENT_TYPE sub_type;
	switch (type)
	{
	case OT_INT8:
		return omsg.Write((NDUINT8)data.GetInt());
	case OT_INT16:
		return omsg.Write((NDUINT16)data.GetInt());
	case OT_INT:
		return omsg.Write((NDUINT32)data.GetInt());
	case  OT_BOOL:
		return omsg.Write((NDUINT8)data.GetInt());
	case OT_FLOAT:
		return omsg.Write(data.GetFloat());
	case OT_STRING:
		return omsg.Write((NDUINT8*)data.GetText());
	case OT_INT64:
	case OT_TIME:
		return omsg.Write((NDUINT64)data.GetInt64());

	case OT_USER_DEFINED:
		return _writeMsgToUserDef(data, omsg);
	case  OT_ARRAY:
		sub_type = data.GetArrayType();
		ret += omsg.Write((NDUINT16)data.GetArraySize());

		for (int i = 0; i < data.GetArraySize(); i++)	{
			DBLDataNode subdata = data.GetArray(i);
			ret += logicDataWrite(subdata, omsg);
		}
		break;
	case OT_ORG_STREAM:
		return omsg.WriteStream((char*)data.GetBinary(), data.GetBinarySize());
	case OT_BINARY_DATA:
		return omsg.WriteBin(data.GetBinary(), data.GetBinarySize());

	case OT_ATTR_DATA:
	{
		_attrDataBin *data_attr = (_attrDataBin *)data.GetBinary();
		if (!data_attr)	{
			return 0;
		}

		omsg.Write((NDUINT16)data_attr->count);
		for (int i = 0; i < data_attr->count; i++)	{
			omsg.Write(data_attr->datas[i].aid);
			omsg.Write(data_attr->datas[i].val);
		}
		return 0;
	}
	default:
		ret = -1;
		break;
	}
	return ret >=0 ? 0 : -1;
}
int logicDataRead(DBLDataNode &data, NDIStreamMsg &inmsg)
{
	NDUINT16 count = 0;
	int ret = 0;
	DBL_ELEMENT_TYPE type = data.GetDataType();
	DBL_ELEMENT_TYPE sub_type;

	NDUINT8 buf[4096];

#define _READ_FROM_STREAM(_read_type, _msg, _data,_writeType) do { \
	_read_type a = 0;					\
	if (0 == _msg.Read(a))					\
		_data.InitSet(_writeType(a));		\
	else  {							\
		return -1;					\
	}								\
}while (0)

	switch (type)
	{
	case  OT_BOOL:
#define _get_bool(a) (a?true:false )
		_READ_FROM_STREAM(NDUINT8, inmsg, data, _get_bool);
		break;
		
	case OT_INT8:
		_READ_FROM_STREAM(NDUINT8, inmsg, data, NDUINT8);
		break;
	case OT_INT16:

		_READ_FROM_STREAM(NDUINT16, inmsg, data, NDUINT16);
		break;
	case OT_INT:
		_READ_FROM_STREAM(NDUINT32, inmsg, data, int);
		break;
	case OT_FLOAT:
		_READ_FROM_STREAM(float, inmsg, data, float);
		break;
	case OT_INT64:
		_READ_FROM_STREAM(NDUINT64, inmsg, data, NDUINT64);
		break;
	case OT_USER_DEFINED:
		return _readMsgToUserDef(data, inmsg);
	case OT_STRING:
	{
		buf[0] = 0;
		size_t len = inmsg.Read(buf,size_t(buf) );
		if (len > sizeof(buf))   {
			return -1;
		}
		data.InitSet((char*)buf);
	}
		break;
	case OT_ATTR_DATA:
	{
		attr_node_buf bufs;
		NDUINT16 num = 0;
		if (-1 == inmsg.Read(num)) {
			return -1;
		}
		for (int i = 0; i < num; i++)	{
			attrid_t id;
			float val;
			if (-1 == inmsg.Read(id)) {
				return -1;
			}
			if (-1 == inmsg.Read(val)) {
				return -1;
			}
			bufs.push_back(id, val);

		}
		Attrbuf2LogicType(data, &bufs);
	}

		break;

	case OT_ORG_STREAM:
	{
		buf[0] = 0;
		size_t len = inmsg.ReadLeftStream((char*)buf, size_t(buf));
		if (len > sizeof(buf))   {
			return -1;
		}
		data.InitSet((void*)buf, len, type);
	}
	break;

	case OT_BINARY_DATA:
	{
		buf[0] = 0;
		size_t len = inmsg.ReadBin((void*)buf, size_t(buf));
		if (len > sizeof(buf))   {
			return -1;
		}
		data.InitSet((void*)buf, len,type);
	}
	break;
	case  OT_ARRAY:
		sub_type = data.GetArrayType();
		if (0 != inmsg.Read(count)) {
			return -1;
		}
		if (count > 0)	{
			DBLDataNode readDataOrg = data.GetArray(0);

			data.InitReservedArray(count, sub_type);
			for (int i = 0; i < count; i++)	{
				DBLDataNode readData = readDataOrg;
				if (0 == logicDataRead(readData, inmsg)){
					data.pushArray(readData);					
				}
			}
		}
		else {
			LogicUserDefStruct userdata;
			userdata.push_back("number", DBLDataNode((NDUINT16)0));			
			data.InitSet(userdata);
		}
		
		break;
	default:
		ret = -1;
		nd_assert(0);
		break;
	}
	return ret;
#undef  _READ_FROM_STREAM
}

//////////////////////////////////////////////////////////////////////////


struct type_name_info
{
	const char *alias;
	int type;
};

static type_name_info alias_type[] = {
	{ "string", OT_STRING },
	{ "char", OT_INT8 },
	{ "int8_t", OT_INT8 },
	{ "int16_t", OT_INT16 },
	{ "int32_t", OT_INT },
	{ "int64_t", OT_INT64 },
	{ "uint8_t", OT_INT8 },
	{ "uint16_t", OT_INT16 },
	{ "uint32_t", OT_INT  },
	{ "uint64_t", OT_INT64 },
	{ "float", OT_FLOAT },

	{ "int8", OT_INT8 },
	{ "int16", OT_INT16 },
	{ "int32", OT_INT },
	{ "int64", OT_INT64 },
	{ "text", OT_STRING }
};


LogicUserDefStruct* createUserData(ndxml *msgNode, ndxml *msg_root, userDefineDataType_map_t &typeRoot);

int get_type_from_alias(const char *name )
{
	char tmpName[128];
	ndstr_parse_word_n(name, tmpName, 128);
	for (int i = 0; i < 11; ++i) {
		if (0 == ndstricmp((char*)alias_type[i].alias, tmpName)) {
			return alias_type[i].type;
		}
	}
	if (0 == ndstricmp("binaryData", name)) {
		return OT_BINARY_DATA;
	}
	return OT_USER_DEFINED;
}

LogicUserDefStruct* _getUserType(const char *name, ndxml *msg_root, userDefineDataType_map_t &typeRoot)
{
	userDefineDataType_map_t::iterator it = typeRoot.find(name);
	if (it != typeRoot.end())	{
		return &(it->second); 
	}
	else {
		ndxml *other = ndxml_getnode(msg_root, name);
		if (other) {
			return createUserData(other, msg_root, typeRoot);
		}
	}
	return NULL;
}

LogicUserDefStruct* createUserData(ndxml *msgNode, ndxml *msg_root, userDefineDataType_map_t &typeRoot)
{
	const char *myName = ndxml_getname(msgNode);
	userDefineDataType_map_t::iterator it = typeRoot.find(myName);
	if (it != typeRoot.end() )	{
		return &(it->second);
	}

	LogicUserDefStruct UserData ;
	
	for (int i = 0; i < ndxml_getsub_num(msgNode); i++)	{
		ndxml *node1 = ndxml_refsubi(msgNode, i);
		int bIsString = false;
		int dataType = 0;
		int subType = 0;
		
		const char *pType = ndxml_getattr_val(node1, "type");
		if (0 == ndstricmp((char*)pType, (char*)"string") || 0 == ndstricmp((char*)pType, (char*)"char")) {
			bIsString = true;
		}


		dataType = get_type_from_alias(pType);

		const char *pArray = ndxml_getattr_val(node1, "isArray");
		const char *pValName = ndxml_getattr_val(node1, "name");
		if (pArray && pArray[0] && pArray[0] == '1') {
			if (bIsString)	{
				dataType = OT_STRING;
			}
			else {
				subType = dataType;
				dataType = OT_ARRAY;
			}
		}

		if (dataType == OT_USER_DEFINED){
			// find from root manager 
			LogicUserDefStruct*pOther = _getUserType(pType, msg_root, typeRoot);
			if (!pOther)	{
				nd_logerror("can not get type %s\n", pType);
				return NULL; //on error
			}

			UserData.push_back(pValName, DBLDataNode(*pOther));

		}
		else {
			if (dataType == OT_ARRAY &&subType == OT_USER_DEFINED) {
				LogicUserDefStruct*pOther = _getUserType(pType, msg_root, typeRoot);
				if (!pOther)	{
					nd_logerror("can not cate type %s\n", pType);
					return NULL; //on error
				}

				DBLDataNode subType;
				subType.InitReservedArray(1,OT_USER_DEFINED);
				subType.SetArray(*pOther,0);
				UserData.push_back(pValName, subType);
			}
			else {
				UserData.push_back(pValName, DBLDataNode(NULL, (DBL_ELEMENT_TYPE)dataType, (DBL_ELEMENT_TYPE)subType));
			}
		}
	}
	//LogicUserDefStruct *pVal = new LogicUserDefStruct(UserData);
	std::pair<userDefineDataType_map_t::iterator, bool> ret = typeRoot.insert(std::make_pair(myName, UserData));
	if (ret.second)	{
		return &(ret.first->second);
	}
	return NULL;
}

int loadUserDefFromMsgCfg(const char *msgfile,  userDefineDataType_map_t &userDataRoot)
{
	int ret = 0;
	ndxml_root xmlfile;
	ndxml_initroot(&xmlfile);
	if (-1 == ndxml_load_ex(msgfile, &xmlfile, nd_get_encode_name(ND_ENCODE_TYPE))) {
		return -1;
	}

	ndxml *dataRoot = ndxml_getnode(&xmlfile, "DataType");
	if (!dataRoot) {
		return -1;
	}
	for (int i = 0; i < ndxml_getsub_num(dataRoot); i++){
		ndxml *node = ndxml_getnodei(dataRoot, i);

		if (!createUserData(node, dataRoot, userDataRoot)) {
			nd_logerror("create node %s error \n", ndxml_getname(node));
			ret = -1;
			break;
		}
	}

	ndxml_destroy(&xmlfile);
	return ret;
}
void destroyUserDefData(userDefineDataType_map_t &userDataRoot)
{
// 	userDefineDataType_map_t::iterator it;
// 	for (it = userDataRoot.begin(); it != userDataRoot.end(); it++) {
// 		if (it->second)	{
// 			LogicUserDefStruct *pData = it->second;
// 			delete pData;
// 			it->second = 0;
// 		}
// 		//delete it->second;
// 	}
	userDataRoot.clear();
}

void dumpMessageData(userDefineDataType_map_t &userDataRoot, const char *outFile )
{
	FILE *pf;
	if (outFile == NULL) {
		pf = fopen("./dumpData.txt", "w");
	}
	else {
		pf = fopen(outFile, "w");
	}

	if (pf)	{
		userDefineDataType_map_t::const_iterator it;
		for (it = userDataRoot.begin(); it != userDataRoot.end(); it++) {
			fprintf(pf, "%s : ", it->first.c_str());
			it->second.Print((logic_print)fprintf, pf);
			fprintf(pf, "\n");
		}
		fclose(pf);
	}
}


int UserDefFormatToMessage(userDefineDataType_map_t &userDataRoot, NDOStreamMsg &omsg)
{
	int size = 0;
	int num = 0;

	int len=0;
	char buf[0x10000];

	userDefineDataType_map_t::const_iterator it;
	for (it = userDataRoot.begin(); it != userDataRoot.end(); it++) {
		if (it->first.empty())
			continue;
		len = it->second.ToStream(buf, sizeof(buf));
		if (len == -1) {
			nd_logerror("write user define error\n");
			return -1;
		}
		omsg.Write(it->first.c_str());
		omsg.WriteBin(buf, len);
		num++;
		size += len;
		//nd_logmsg("format index =%d len =%d\n", num, size);
	}
	return num;
}

int UserDefFormatFromMessage(userDefineDataType_map_t &userDataRoot, NDIStreamMsg &inmsg)
{
	int len;
	char name[USER_DEF_VAR_NAME_SIZE];
	char buf[0x10000];
	while (inmsg.LeftData() > 0){
		size_t s = inmsg.Read(name, sizeof(name));
		if (s == 0) {
			break;
		}
		len = (int)inmsg.ReadBin(buf, sizeof(buf));
		if (len > 0) {
			LogicUserDefStruct val;
			if (-1 == val.FromStream(buf, len)) {
				return -1;
			}
			std::pair<userDefineDataType_map_t::iterator, bool> ret = userDataRoot.insert(std::make_pair(name, val));
			if (!ret.second)	{
				return -1;
			}		
		}
	}
	return 0;
}

/*
#include "message_inc.h"
int testFormatMsgRead(userDefineDataType_map_t &userDataRoot)
{
	NetMessage::EquipAttrNode attrNode;
	NetMessage::EquipInfo equip;
	NetMessage::RolePackageData package;

	equip.instantID = 0x10000 + 1 ;
	equip.typeID = 1001 ;
	equip.slot = 1;
	equip.level = 2;
	equip.sub_level = 1;
	equip.stat = 1; 

	attrNode.attr_id = 30;
	attrNode.attr_val = 2.0;
	attrNode.color = 1;

	equip.push_attrs(attrNode);
	package.push_equips(equip);
	
	equip.typeID += 1;
	equip.instantID += 1;
	package.push_equips(equip);



	NetMessage::ItemInfo item;
	item.instantID = equip.instantID + 1;
	item.number = 2;
	item.slot = 0;
	item.typeID = 2002;
	item.stat = 0;

	package.push_items(item);

	NDOStreamMsg omsg(0);
	NetMessage::WriteStream(omsg, package);

	NDIStreamMsg inmsg(omsg.GetMsgAddr());

	userDefineDataType_map_t::iterator it = userDataRoot.find("RolePackageData");
	nd_assert(it != userDataRoot.end());
	
	DBLDataNode val(it->second);

	int ret = logicDataRead(val, inmsg);
	if (ret == -1){
		nd_logerror("test logic data read error \n");
	}
	char buf[1024];
	val.Sprint(buf, 1024);
	nd_logmsg("RolePackageData format message read :\n %s \n", buf);
	return ret;


}
*/