//
//  buildCS.cpp
//  protocolBuilder
//
//  Created by duanxiuyun on 15-4-10.
//  Copyright (c) 2015年 duanxiuyun. All rights reserved.
//

#include "nd_common/nd_common.h"
#include <string>

#define _OUT_PUT_TIME(_pf) do { \
fprintf(_pf,"/* please do not change this file ,\n * auto create by program \n * create time %s \n */\n\n", nd_get_datetimestr() ) ; \
}while(0)

struct type_name_info
{
	const char *alias ;
	const char *type ;
};

static type_name_info alias_type[] = {
	{"char", "byte"},
	{"int8_t" , "sbyte"} ,
	{"int16_t" , "short"} ,
	{"int32_t" , "int"} ,
	{"int64_t" , "long"} ,
	{"uint8_t" , "byte"} ,
	{"uint16_t" , "ushort"} ,
	{"uint32_t" , "uint"} ,
	{"uint64_t" , "ulong"},
	{ "float", "float" }
};


static type_name_info alias_operate[] = {
	{"char", "Int8" },
	{"int8_t" , "Int8"} ,
	{"int16_t" , "Int16"} ,
	{"int32_t" , "Int32"} ,
	{"int64_t" , "Int64"} ,
	{"uint8_t" , "Uint8"} ,
	{"uint16_t" , "Uint16"} ,
	{"uint32_t" , "Uint32"} ,
	{"uint64_t" , "Uint64"},
	{"string" , "String"},
	{ "float", "Float" }
};


static const char *get_type_from_alias(const char *in_alias,type_name_info *info, int size)
{
	for (int i=0; i< size; ++i) {
		if (0==ndstricmp((char*) info[i].alias, (char*)in_alias)) {
			return info[i].type ;
		}
	}
	return NULL;
	
}

static const char* getType(const char *in_type)
{
	return get_type_from_alias(in_type,alias_type, sizeof(alias_type)/sizeof(alias_type[0]) ) ;
}

static const char* getOperator(const char *in_type)
{
	return get_type_from_alias(in_type,alias_operate, sizeof(alias_operate)/sizeof(alias_operate[0]) ) ;
}

static bool isNeedBuild(ndxml *xml)
{
	const char *platform = ndxml_getattr_val(xml, "platform");
	if (platform )	{
		if (ndstricmp(platform, "csharp")==0)	{
			return true;
		}
		return false;
	}
	return true;
}

int build_csMessageID(ndxml_root *xmlfile, FILE *pf)
{
	ndxml *xnode = ndxml_getnode(xmlfile, "MessageDefine") ;
	int total = ndxml_getsub_num(xnode) ;
	//NDUINT8 maxID =0,minID =0 ;
	
	fprintf(pf,"#region MessageID \n \tpublic class MessageIDDefine\n\t{\n" ) ;
	
	for (int x = 0; x < total; x++)	{
		ndxml *mainMessage = ndxml_refsubi(xnode, x);
		int subNum = ndxml_getsub_num(mainMessage);

		NDUINT8 maxID = 0, minID = 0;

		const char *p = ndxml_getattr_val(mainMessage, "maxMessageId");
		if (!p || !*p)	{
			continue;
		}
		maxID = atoi(p);
		fprintf(pf, "\n\t\t//%s\n", ndxml_getattr_val(mainMessage, "name"));

		for (int i = 0; i < subNum; ++i) {
			ndxml *sub = ndxml_refsubi(mainMessage, i);

			fprintf(pf, "\t\tpublic const int %s = 0x%x ; //%s\tbody(%s)\n", ndxml_getattr_val(sub, "id"),
				ND_MAKE_WORD(maxID, minID),
				ndxml_getattr_val(sub, "comment"),
				ndxml_getattr_val(sub, "body"));
			++minID;
		}
	}

	
	fprintf(pf,"\t}\n#endregion\n\n") ;
	return 0 ;
}


static int exp_enum(ndxml *node, FILE *pf)
{
	int total = (int)ndxml_getsub_num(node);
	for (int i = 0; i < total; ++i) {
		ndxml *sub = ndxml_getnodei(node, i);
		nd_assert(sub);

		const char *pcomment = ndxml_getattr_val(sub, "comment");
		if (pcomment) {
			fprintf(pf, "// %s \n", pcomment);
		}

		fprintf(pf, "\tpublic enum %s { \n", ndxml_getname(sub));
		for (int x = 0; x < ndxml_getsub_num(sub); ++x) {
			ndxml *element = ndxml_getnodei(sub, x);
			pcomment = ndxml_getattr_val(element, "comment");
			const char *pvalue = ndxml_getattr_val(element, "value");

			if (pvalue && pvalue[0]) {
				fprintf(pf, "\t\t%s =%s,//%s\n", ndxml_getname(element), pvalue, pcomment ? pcomment : "\t ");
			}
			else {
				fprintf(pf, "\t\t%s,//%s\n", ndxml_getname(element), pcomment ? pcomment : "\t ");
			}
		}

		fprintf(pf, "\t};\n\n\n");
	}

	return 0;
}

int build_csMarco(ndxml_root *xmlfile, FILE *pf)
{
	ndxml *xnode = ndxml_getnode(xmlfile, "MarcoDefine") ;
	
	fprintf(pf,"#region MarcoString \n \tpublic class AutoDataMarco\n\t{\n" ) ;
	
	int total = ndxml_getsub_num(xnode) ;
	for (int i=0; i<total; ++i) {
		ndxml *sub = ndxml_refsubi(xnode, i) ;
		fprintf(pf,"\t\tpublic const int %s = %s ; //%s\t\n", ndxml_getattr_val(sub, "name"),
				ndxml_getattr_val(sub, "value"),
				ndxml_getattr_val(sub, "comment") ) ;
	}
	
	fprintf(pf,"\t}\n#endregion\n\n") ;


	ndxml *pEnumXml = ndxml_getnode(xmlfile, "EnumDefine");
	if (pEnumXml) {
		exp_enum(pEnumXml, pf);
	}
	return 0 ;
}

int build_csDataStruct(ndxml_root *xmlfile, FILE *pf)
{
	ndxml *xnode = ndxml_getnode(xmlfile, "DataType") ;
	
	ndxml *xInfo = ndxml_getnode(xmlfile, "DataDesc");
	if (!xInfo) {
		return -1;
	}

	const char *comm_version = ndxml_getattr_val(xInfo, "comm_version");	
	
	fprintf(pf, "\tpublic struct MessageVersionIndex \n\t{\n");
	fprintf(pf, "\t\tpublic const int NET_COMMUNATION_VERSION = %s;\n\t}\n", comm_version);	

	fprintf(pf,"#region StructFileString \n \tpublic interface MessageDataBase\n\t{\n" ) ;

	fprintf(pf, "\t\tvoid Read(ref NDMsgStream dataStream);\n");
	fprintf(pf, "\t\tint Write(ref NDMsgStream dataStream);\n\t}\n");
	
	
	int total = ndxml_getsub_num(xnode) ;
	
	for (int i=0; i<total; ++i) {
		ndxml *sub = ndxml_refsubi(xnode,i) ;
		const char *pName = ndxml_getname(sub) ;
		if (!pName) {
			continue ;
		}
		if (!isNeedBuild(sub)){
			continue;
		}
		fprintf(pf, "\t[Serializable]\n\tpublic struct %s : MessageDataBase \n\t{\n ", pName) ;		
		char buf_read_func[40960] ;
		char buf_write_func[40960] ;
		char buf_constrct_func[40960] ;		
		char *pReadStream = buf_read_func;
		char *pWriteStream = buf_write_func ;
		char *pConstructStream = buf_constrct_func;
		
		buf_read_func[0] =0;
		buf_write_func[0]=0 ;
		buf_constrct_func[0] =0;
		
		int subNodes = ndxml_getsub_num(sub) ;
		for (int n=0; n<subNodes; ++n) {
			ndxml *node1 = ndxml_refsubi(sub,n) ;
			int bIsString =false ;
			const char *pType = ndxml_getattr_val(node1, "type");
			
			if (0==ndstricmp((char*)pType, (char*)"string")) {
				bIsString = true ;
			}
			
			const char *pArray = ndxml_getattr_val(node1,"isArray") ;
			const char *pValName = ndxml_getattr_val(node1, "name") ;
			
			const char *realType = getType(pType) ;
			const char *realOp = getOperator(pType) ;
			
			if((pArray && pArray[0] && pArray[0]=='1') && !bIsString)  {
				
				fprintf(pf,"\t\tpublic ushort  %sCount; \t\n",pValName ) ;
				fprintf(pf,"\t\tpublic %s  []%s; \t//%s \n",realType?realType:pType,
						pValName,ndxml_getattr_val(node1, "comment") ) ;
				
				int size = 0 ;
				//readstream array
				
				size = snprintf(pReadStream, sizeof(buf_read_func) - (pReadStream - buf_read_func),
									"\t\t\t%sCount = dataStream.ReadUint16();\n"
									"\t\t\t%s = new %s[%sCount];\n" ,
									pValName, pValName, realType ? realType : pType, pValName);
				pReadStream += size;
				
				size = snprintf(pReadStream, sizeof(buf_read_func) - (pReadStream - buf_read_func),
								"\t\t\tfor(int i=0; i<%sCount;++i){\n",pValName ) ;
				pReadStream += size ;
				
				
				
				//write stream
				if (realOp)	{

					size = snprintf(pReadStream, sizeof(buf_read_func) - (pReadStream - buf_read_func),
						"\t\t\t\t%s[i] = dataStream.Read%s();\n\t\t\t}\n"
						, pValName, realOp );
					pReadStream += size;

					size = snprintf(pWriteStream, sizeof(buf_write_func) - (pWriteStream - buf_write_func),
						"\t\t\tsize += dataStream.WriteUint16(%sCount);\n"
						"\t\t\tfor(int i=0; i<%sCount;++i) {\n"
						"\t\t\t\tsize += dataStream.Write%s(%s[i]);\n\t\t\t} \n",
						pValName, pValName, realOp , pValName);
					pWriteStream += size;

				}
				else {

					size = snprintf(pReadStream, sizeof(buf_read_func) - (pReadStream - buf_read_func),
						"\t\t\t\t%s[i] = new %s();\n"
						"\t\t\t\t%s[i].Read(ref dataStream);\n\t\t\t}\n"
						, pValName, realType ? realType : pType, pValName);
					pReadStream += size;

					size = snprintf(pWriteStream, sizeof(buf_write_func) - (pWriteStream - buf_write_func),
						"\t\t\tsize += dataStream.WriteUint16(%sCount);\n"
						"\t\t\tfor(int i=0; i<%sCount;++i) {\n"
						"\t\t\t\tsize += %s[i].Write(ref dataStream);\n\t\t\t} \n",
						pValName, pValName, pValName);
					pWriteStream += size;

				}
				
			}
			
			else {
				int size = 0;
				fprintf(pf,"\t\tpublic %s  %s; \t//%s \n",realType?realType:pType, pValName,ndxml_getattr_val(node1, "comment") ) ;
				//construct				
				if (realOp)	{
					//readstream
					size = snprintf(pReadStream, sizeof(buf_read_func) - (pReadStream - buf_read_func),
						"\t\t\t%s = dataStream.Read%s();\n", pValName, realOp );
					pReadStream += size;
					//write stream
					size = snprintf(pWriteStream, sizeof(buf_write_func) - (pWriteStream - buf_write_func),
						"\t\t\tsize += dataStream.Write%s(%s);\n", realOp , pValName);
					pWriteStream += size;
				}
				else {
					//readstream
					size = snprintf(pReadStream, sizeof(buf_read_func) - (pReadStream - buf_read_func),
						"\t\t\t%s.Read(ref dataStream);\n", pValName);
					pReadStream += size;
					//write stream
					size = snprintf(pWriteStream, sizeof(buf_write_func) - (pWriteStream - buf_write_func),
						"\t\t\tsize += %s.Write(ref dataStream);\n", pValName);
					pWriteStream += size;
				}				
			}			
		}
		
		//output funciont
		//construct
		//fprintf(pf, "\n\t\tpublic %s() \n\t\t{\n\t\t}\n", pName) ;
		
		//read stream
		fprintf(pf, "\n\t\tpublic void Read(ref NDMsgStream dataStream)\n"
				"\t\t{\n%s\n\t\t}\n", buf_read_func ) ;
		
		//write strea
		fprintf(pf, "\n\t\tpublic int Write(ref NDMsgStream dataStream)\n"
				"\t\t{\n\t\t\tint size = 0;\n"
				"%s\n\t\t\treturn size;\n\t\t}\n", buf_write_func ) ;
		
		
		fprintf(pf, "\t}\n\n") ;
		
	}
	fprintf(pf,"#endregion\n\n") ;
	
	return 0 ;
}

typedef int (*xml_export_func)(ndxml_root *xmlfile, FILE *pf);

int xml2Sharp(ndxml_root *xml, const char *file, xml_export_func func)
{
	FILE *pf = fopen(file, "w");
	if (!pf){
		fprintf(stderr, " open out put file %s error \n", file);
		return -1;
	}

	_OUT_PUT_TIME(pf);

	fprintf(pf, "using NetMessage;\n");
	fprintf(pf, "using System;\n");
	fprintf(pf, "using System.IO;\n");
	fprintf(pf, "using System.Text;\n");
	fprintf(pf, "using System.Collections.Generic;\n");
	fprintf(pf, "namespace NetMessage\n{\n\n");

	func(xml, pf);

	fprintf(pf, "\n}\n");
	fclose(pf);
	return 0;
}
int build_CSharp(ndxml_root *xmlID, ndxml_root *xmlMarco, ndxml_root *xmlData, const char *outfile)
{

	if (-1 == xml2Sharp(xmlID, "./csharp/MessageID.cs",build_csMessageID)) {
		return -1;
	}


	if (-1 == xml2Sharp(xmlMarco, "./csharp/MacroDefind.cs", build_csMarco)) {
		return -1;
	}

	if (-1 == xml2Sharp(xmlData, "./csharp/MessageDataStruct.cs", build_csDataStruct)) {
		return -1;
	}

	return 0 ;
	
}
