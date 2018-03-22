/* file expLua.cpp
*
* export lua data struct 
*
* create by duan 
*
* 2017.4.5
*/


#include "nd_common/nd_common.h"
#include <string>


struct type_name_info
{
	const char *alias;
	const char *type;
};

static type_name_info alias_operate[] = {
	{ "char", "Uint8" },
	{ "int8_t", "Int8" },
	{ "int16_t", "Int16" },
	{ "int32_t", "Int32" },
	{ "int64_t", "Int64" },
	{ "uint8_t", "Uint8" },
	{ "uint16_t", "Uint16" },
	{ "uint32_t", "Uint32" },
	{ "uint64_t", "Uint64" },
	{ "string", "String" },
	{"float", "Float"}
};


static const char *get_type_from_alias(const char *in_alias, type_name_info *info, int size)
{
	for (int i = 0; i < size; ++i) {
		if (0 == ndstricmp((char*)info[i].alias, (char*)in_alias)) {
			return info[i].type;
		}
	}
	return NULL;

}
static bool is_base_type(const char *name)
{
	const char *ret = get_type_from_alias(name, alias_operate, sizeof(alias_operate) / sizeof(alias_operate[0]));
	if (ret) {
		return true;
	}
	return false;
}

static const char* getOperator(const char *in_type)
{
	return get_type_from_alias(in_type, alias_operate, sizeof(alias_operate) / sizeof(alias_operate[0]));
}


static bool isNeedBuild(ndxml *xml)
{
	const char *platform = ndxml_getattr_val(xml, "platform");
	if (platform)	{
		if (ndstricmp(platform, "lua")==0)	{
			return true;
		}
		return false;
	}
	return true;
}

int build_luaDataStruct(ndxml_root *xmlfile, const char *outFileName)
{
	FILE *pf = fopen(outFileName, "w");
	if (!pf){
		fprintf(stderr, " open out put file %s error \n", outFileName);
		return -1;
	}
	fprintf(pf, "-- PLEASE do not change this file ,\n"
		"-- auto create by program \n-- create time %s \n\n\n", nd_get_datetimestr()); 

	ndxml *xnode = ndxml_getnode(xmlfile, "DataType");

	fprintf(pf, "datastruct = {}\n\nlocal this = datastruct\n");

	int total = ndxml_getsub_num(xnode);

	for (int i = 0; i < total; ++i) {
		ndxml *sub = ndxml_refsubi(xnode, i);
		const char *pName = ndxml_getname(sub);
		if (!pName) {
			continue;
		}
		if (!isNeedBuild(sub)){
			continue;
		}

		char buf_read_func[40960];
		char buf_write_func[40960];
		char buf_constrct_func[40960];
		char *pReadStream = buf_read_func;
		char *pWriteStream = buf_write_func;
		char *pConstructStream = buf_constrct_func;

		buf_read_func[0] = 0;
		buf_write_func[0] = 0;
		buf_constrct_func[0] = 0;

		if (ndxml_getattr_val(sub, "comment")) {
			fprintf(pf, "\n-- %s\n%s = {\n", ndxml_getattr_val(sub, "comment"), pName);
		}
		else {
			fprintf(pf, "\n%s = {\n", pName);
		}

		int subNodes = ndxml_getsub_num(sub);
		for (int n = 0; n < subNodes; ++n) {
			bool isLastField = (n == subNodes - 1);
			ndxml *node1 = ndxml_refsubi(sub, n);
			int bIsString = false;
			const char *pType = ndxml_getattr_val(node1, "type");

			if (0 == ndstricmp((char*)pType, (char*)"string")) {
				bIsString = true;
			}

			const char *pArray = ndxml_getattr_val(node1, "isArray");
			const char *pValName = ndxml_getattr_val(node1, "name");
			const char *realOp = getOperator(pType);

			if ((pArray && pArray[0] && pArray[0] == '1') && !bIsString)  {

				fprintf(pf, "\t%sCount, \t\n", pValName);
				fprintf(pf, "\t%s%c -- %s\n", pValName, isLastField?' ':',' , ndxml_getattr_val(node1, "comment"));

				int size = 0;
				//construct function 

				size = snprintf(pConstructStream, sizeof(buf_constrct_func) - (pConstructStream - buf_constrct_func),
					"\to.%sCount = 0\n"
					"\to.%s = {}\n",
					pValName, pValName);
				pConstructStream += size;


				if (realOp)	{
					//readstream array
					size = snprintf(pReadStream, sizeof(buf_read_func) - (pReadStream - buf_read_func),
						"\tself.%sCount = dataStream:ReadUint16()\n"
						"\tif true == dataStream.IsStructEnd() then\n\t\treturn\n\tend\n"
						"\tfor i = 1, self.%sCount do\n"
						"\t\tlocal val = dataStream:Read%s()\n"
						"\t\tif true == dataStream.IsStructEnd() then\n\t\t\treturn\n\t\tend\n"
						"\t\ttable.insert(self.%s, i, val)\n\tend\n\n",
						pValName, pValName,realOp,pValName);
					pReadStream += size;


					//write stream
					size = snprintf(pWriteStream, sizeof(buf_write_func) - (pWriteStream - buf_write_func),
						"\tsize = size + dataStream:WriteUint16(self.%sCount)\n"
						"\tfor i = 1, self.%sCount do\n"
						"\t\tsize = size + dataStream : Write%s(self.%s[i])\n\tend\n",
						pValName, pValName, realOp, pValName);
					pWriteStream += size;

				}
				else {
					//readstream array
					size = snprintf(pReadStream, sizeof(buf_read_func) - (pReadStream - buf_read_func),
						"\tself.%sCount = dataStream:ReadUint16()\n"
						"\tif true == dataStream.IsStructEnd() then\n\t\treturn\n\tend\n"
						"\tfor i = 1, self.%sCount do\n"
						"\t\tlocal val = %s:New(nil)\n"
						"\t\tval:Read(dataStream)\n"
						"\t\tif true == dataStream.IsStructEnd() then\n\t\t\treturn\n\t\tend\n"
						"\t\ttable.insert(self.%s, i, val)\n\tend\n\n",
						pValName, pValName, pType,pValName);
					pReadStream += size;


					//write stream
					size = snprintf(pWriteStream, sizeof(buf_write_func) - (pWriteStream - buf_write_func),
						"\tsize = size + dataStream:WriteUint16(self.%sCount)\n"
						"\tfor i = 1, self.%sCount do\n"
						"\t\tsize = size + self.%s[i]:Write(dataStream)\n\tend\n",
						pValName, pValName, pValName);
					pWriteStream += size;
					
				}

			}

			else {
				int size = 0;
				fprintf(pf, "\t%s%c -- %s\n", pValName, isLastField ? ' ' : ',', ndxml_getattr_val(node1, "comment"));
								
				if (realOp)	{
					//construct
					size = snprintf(pConstructStream, sizeof(buf_constrct_func) - (pConstructStream - buf_constrct_func),
						"\to.%s = %s\n",
						pValName, bIsString ? "\"\"" : "0" );
					pConstructStream += size;

					//readstream
					size = snprintf(pReadStream, sizeof(buf_read_func) - (pReadStream - buf_read_func),
						"\tself.%s = dataStream:Read%s()\n\tif true == dataStream.IsStructEnd() then\n\t\treturn\n\tend\n", pValName, realOp);
					pReadStream += size;

					//write stream
					size = snprintf(pWriteStream, sizeof(buf_write_func) - (pWriteStream - buf_write_func),
						"\tsize = size + dataStream:Write%s(self.%s)\n", realOp, pValName);
					pWriteStream += size;
				}
				else {
					//construct
					size = snprintf(pConstructStream, sizeof(buf_constrct_func) - (pConstructStream - buf_constrct_func),
						"\to.%s = %s:New(nil)\n",	pValName, pType);
					pConstructStream += size;

					//readstream
					size = snprintf(pReadStream, sizeof(buf_read_func) - (pReadStream - buf_read_func),
						"\tself.%s:Read(dataStream)\n\tif true == dataStream.IsStructEnd() then\n\t\treturn\n\tend\n", pValName);
					pReadStream += size;

					//write stream
					size = snprintf(pWriteStream, sizeof(buf_write_func) - (pWriteStream - buf_write_func),
						"\tsize = size + self.%s:Write(dataStream)\n",  pValName);
					pWriteStream += size;
				}
			}
		}

		fprintf(pf, "}\n");

		//output funciont
		//construct
		fprintf(pf, "\nfunction %s:New(o)\n"
			"\to = o or{}\n"
			"\tsetmetatable(o, self)\n"
			"\tself.__index = self\n"
			"%s\treturn o\nend\n"
			, pName,buf_constrct_func);

		//read stream
		fprintf(pf, "\nfunction %s:Read(dataStream)\n"
			"%s\n\tdataStream.TrytoMoveStructEnd()\nend\n", pName, buf_read_func);

		//write stream
		fprintf(pf, "\nfunction %s:Write(dataStream)\n"
			"\tlocal size = 1 \n%s\tdataStream.WriteStructEnd()\n\t"
			"return size\nend\n\n",pName, buf_write_func);

	}

	return 0;
}


/*
MessageIDDefine = {
NETMSG_ROLE_LAST_CHANGE_TM_REQ = 0x300,  --获得上次角色数据更新时间
NETMSG_ROLE_LAST_CHANGE_TM_ACK = 0x301, --返回上次角色数据更新时间
NETMSG_ROLE_CHANGE_NAME_REQ = 0x302 ,
NETMSG_ROLE_CHANGE_NAME_ACK = 0x303,
NETMSG_ROLE_DATA_INIT_REQ = 0x304 ,
NETMSG_ROLE_DATA_INIT_ACK = 0x305


}

*/

int build_luaMessageID(ndxml_root *xmlfile, const char *out_file)
{
	ndxml *xnode = ndxml_getnode(xmlfile, "MessageDefine");
	if (!xnode) {
		fprintf(stderr, " build marco error MarcoDefine node unexist\n");
		return -1;
	}
	FILE *pf = fopen(out_file, "w");
	if (!pf){
		fprintf(stderr, " open out put file %s error \n", out_file);
		return -1;
	}

	fprintf(pf, "-- PLEASE do not change this file ,\n"
		"-- auto create by program \n-- create time %s \n\n\n", nd_get_datetimestr());


	fprintf(pf, "MessageIDDefine = {\n");

	int total = ndxml_getsub_num(xnode);

	for (int x = 0; x < total; x++)	{
		ndxml *mainMessage = ndxml_refsubi(xnode, x);
		int subNum = ndxml_getsub_num(mainMessage);
		NDUINT8 maxID = 0, minID = 0;

		const char *p = ndxml_getattr_val(mainMessage, "maxMessageId");
		if (!p || !*p)	{
			continue;
		}
		maxID = atoi(p);
		fprintf(pf, "\n-- %s\n", ndxml_getattr_val(mainMessage, "name"));


		for (int i = 0; i < subNum; ++i) {
			ndxml *sub = ndxml_refsubi(mainMessage, i);

			const char *dataInfo = ndxml_getattr_val(sub, "body");
			if (!dataInfo || !*dataInfo) {
				dataInfo = "none";
			}
			fprintf(pf, "\t%s= 0x%x , -- (%d,%d)%s,data-format:%s\n",
				ndxml_getattr_val(sub, "id"), ND_MAKE_WORD(maxID, minID),
				 maxID, minID, ndxml_getattr_val(sub, "comment"), dataInfo);
			++minID;
		}

	}

	fprintf(pf,"\n}\n") ;

	fclose(pf);
	return 0;
}