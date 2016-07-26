//
//  buildCS.cpp
//  protocolBuilder
//
//  Created by duanxiuyun on 15-4-10.
//  Copyright (c) 2015年 duanxiuyun. All rights reserved.
//

#include "nd_common/nd_common.h"
#include <stdlib.h>
#include <stdio.h>
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
	{"uint64_t" , "ulong"}
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
	{"string" , "String"}
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


int build_csMessageID(ndxml_root *xmlfile, FILE *pf)
{
	ndxml *xnode = ndxml_getnode(xmlfile, "MessageDefine") ;
	int total = ndxml_getsub_num(xnode) ;
	NDUINT8 maxID =0,minID =0 ;
	
	fprintf(pf,"#region MessageID \n \tpublic class MessageIDDefine\n\t{\n" ) ;
	
	for (int i=0; i<total; ++i) {
		ndxml *sub = ndxml_refsubi(xnode, i) ;
		const char *p = ndxml_getattr_val(sub, "number") ;
		if (p) {
			int startID = atoi(p) ;
			if (startID > 0 ) {
				maxID = startID ;
				minID = 0 ;
			}
		}
		
		fprintf(pf,"\t\tpublic const int %s = 0x%x ; //%s\t\n", ndxml_getattr_val(sub, "id"),
				ND_MAKE_WORD(maxID, minID),
				ndxml_getattr_val(sub, "comment") ) ;
		++minID ;
	}
	fprintf(pf,"\t}\n#endregion\n\n") ;
	return 0 ;
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
	return 0 ;
}

int build_csDataStruct(ndxml_root *xmlfile, FILE *pf)
{
	ndxml *xnode = ndxml_getnode(xmlfile, "DataType") ;
	
	
	fprintf(pf,"#region StructFileString \n \tpublic class MessageDataBase\n\t{\n" ) ;
	fprintf(pf,"\t\tpublic virtual void Read(DataTranser dataStream){} \n" ) ;
	fprintf(pf,"\t\tpublic virtual int Write(DataTranser dataStream){return 0;}\n\t}\n") ;
	
	
	int total = ndxml_getsub_num(xnode) ;
	
	for (int i=0; i<total; ++i) {
		ndxml *sub = ndxml_refsubi(xnode,i) ;
		const char *pName = ndxml_getname(sub) ;
		if (!pName) {
			continue ;
		}
		fprintf(pf, "\tpublic class %s : MessageDataBase \n\t{\n ", pName) ;
		
		char buf_read_func[4096] ;
		char buf_write_func[4096] ;
		char buf_constrct_func[4096] ;
		
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
				
				size = snprintf(pReadStream, sizeof(buf_read_func),
									"\t\t\t%sCount = dataStream.ReadUint16();\n"
									"\t\t\t%s = new %s[%sCount];\n" ,
									pValName, pValName, realType ? realType : pType, pValName);
				pReadStream += size;
				
				size = snprintf(pReadStream, sizeof(buf_read_func),
								"\t\t\tfor(int i=0; i<%sCount;++i){\n",pValName ) ;
				pReadStream += size ;
				
				
				
				//write stream
				if (realOp)	{

					size = snprintf(pReadStream, sizeof(buf_read_func),
						"\t\t\t\t%s[i] = dataStream.Read%s();\n\t\t\t}\n"
						, pValName, realType ? realType : pType, pValName, realOp );
					pReadStream += size;

					size = snprintf(pWriteStream, sizeof(buf_write_func),
						"\t\t\tsize += dataStream.WriteUint16(%sCount);\n"
						"\t\t\tfor(int i=0; i<%sCount;++i) {\n"
						"\t\t\t\tsize += dataStream.Write%s(%s[i]);\n\t\t\t} \n",
						pValName, pValName, realOp , pValName);
					pWriteStream += size;

				}
				else {

					size = snprintf(pReadStream, sizeof(buf_read_func),
						"\t\t\t\t%s[i] = new %s();\n"
						"\t\t\t\t%s[i].Read(dataStream);\n\t\t\t}\n"
						, pValName, realType ? realType : pType, pValName);
					pReadStream += size;

					size = snprintf(pWriteStream, sizeof(buf_write_func),
						"\t\t\tsize += dataStream.WriteUint16(%sCount);\n"
						"\t\t\tfor(int i=0; i<%sCount;++i) {\n"
						"\t\t\t\tsize += %s[i].Write(dataStream);\n\t\t\t} \n",
						pValName, pValName, pValName);
					pWriteStream += size;

				}
				
			}
			
			else {
				int size = 0;
				fprintf(pf,"\t\tpublic %s  %s; \t//%s \n",realType?realType:pType, pValName,ndxml_getattr_val(node1, "comment") ) ;
				//construct
// 				
// 				if (bIsString) {
// 					size = snprintf(pConstructStream, sizeof(buf_constrct_func),
// 									"\t\t\t%s = string.Empty ;\n",pValName ) ;
// 					pConstructStream += size ;
// 				}
// 				else {
// 					size = snprintf(pConstructStream, sizeof(buf_constrct_func),
// 									"\t\t\t%s = new %s() ;\n",pValName ,realType?realType:pType) ;
// 					pConstructStream += size ;
// 				}
				
				if (realOp)	{
					//readstream
					size = snprintf(pReadStream, sizeof(buf_read_func),
						"\t\t\t%s = dataStream.Read%s();\n", pValName, realOp );
					pReadStream += size;
					//write stream
					size = snprintf(pWriteStream, sizeof(buf_write_func),
						"\t\t\tsize += dataStream.Write%s(%s);\n", realOp , pValName);
					pWriteStream += size;
				}
				else {
					//readstream
					size = snprintf(pReadStream, sizeof(buf_read_func),
						"\t\t\t%s.Read(dataStream);\n", pValName);
					pReadStream += size;
					//write stream
					size = snprintf(pWriteStream, sizeof(buf_write_func),
						"\t\t\tsize += %s.Write(dataStream);\n", pValName);
					pWriteStream += size;
				}
				
			}
			
		}
		
		//output funciont
		//construct
		fprintf(pf, "\n\t\tpublic %s() \n\t\t{\n\t\t}\n", pName) ;
		
		//read stream
		fprintf(pf, "\n\t\tpublic override void Read(DataTranser dataStream)\n"
				"\t\t{\n%s\n\t\t}\n", buf_read_func ) ;
		
		//write strea
		fprintf(pf, "\n\t\tpublic override int Write(DataTranser dataStream)\n"
				"\t\t{\n\t\t\tint size = 0;\n"
				"%s\n\t\t\treturn size;\n\t\t}\n", buf_write_func ) ;
		
		
		fprintf(pf, "\t}\n\n") ;
		
	}
	fprintf(pf,"#endregion\n\n") ;
	
	return 0 ;
}

//MessageReceiveSendClassString
int build_csMessageRecvSend(ndxml_root *xmlfile, FILE *pf)
{
	ndxml *xnode = ndxml_getnode(xmlfile, "MessageDefine") ;
	int total = ndxml_getsub_num(xnode) ;
	
	fprintf(pf,"#region MessageReceiveSendClassString \n") ;
 	fprintf(pf,"\tpublic class MessageReceiveSend\n\t{\n" ) ;
	fprintf(pf,"\t\tDataTranser m_readDataTranser;\n\t\tDataTranser m_sendDataTranser;\n") ;
	fprintf(pf,"\t\tpublic MessageReceiveSend()\n\t\t{\n") ;
	fprintf(pf,"\t\t\tm_readDataTranser = new DataTranser();\n") ;
	fprintf(pf,"\t\t\tm_sendDataTranser = new DataTranser();\n\t\t}\n") ;
	fprintf(pf,"#region EventString\n\n") ;
	
	for (int i=0; i<total; ++i) {
		ndxml *sub = ndxml_refsubi(xnode, i) ;
		const char *body = ndxml_getattr_val(sub, "body") ;
		
		if (body && body[0]) {
			fprintf(pf,"\t\t//%s\n\t\tpublic delegate void Handle%s(MessageHead head,%s body);\n",
					ndxml_getattr_val(sub, "comment") , ndxml_getattr_val(sub, "id"),body) ;
		}
		else {
			fprintf(pf,"\t\t//%s\n\t\tpublic delegate void Handle%s(MessageHead head);\n",
					ndxml_getattr_val(sub, "comment") , ndxml_getattr_val(sub, "id")	) ;
		}
		
		
		fprintf(pf,"\t\tpublic event Handle%s OnHandle%s;\n\n",
				ndxml_getattr_val(sub, "id") , ndxml_getattr_val(sub, "id")) ;
		
	}
	fprintf(pf,"#endregion\n\n") ;
	fprintf(pf,"#region PackMsgString\n") ;
	//message packeage to byte
	
	
	for (int i=0; i<total; ++i) {
		ndxml *sub = ndxml_refsubi(xnode, i) ;
		const char *body = ndxml_getattr_val(sub, "body") ;
		
		if (body && body[0]) {
			fprintf(pf,"\tpublic byte[] Pack%s(MessageHead head,%s body)\n",
				ndxml_getattr_val(sub, "id"),body);
		}else {
			
			fprintf(pf,"\tpublic byte[] Pack%s(MessageHead head)\n",
					ndxml_getattr_val(sub, "id"));
		}
		
		fprintf(pf,"\t{\n\t\thead.id = MessageIDDefine.%s;\n",ndxml_getattr_val(sub, "id")) ;
		fprintf(pf,"\t\tMemoryStream sendMemStream = new MemoryStream();\n") ;
		fprintf(pf, "\t\tm_sendDataTranser.SetStream(sendMemStream);\n") ;
		fprintf(pf, "\t\tint headSize = m_sendDataTranser.WriteMessageHead(head);\n") ;
		if (body && body[0]) {
			fprintf(pf, "\t\tint bodySize = body.Write(m_sendDataTranser);\n") ;
		}
		
		fprintf(pf, "\t\thead.lenth = (ushort)(headSize+bodySize);\n") ;
		fprintf(pf, "\t\tsendMemStream.Seek(0, SeekOrigin.Begin);\n") ;
		fprintf(pf, "\t\tm_sendDataTranser.WriteMessageHead(head);\n") ;
		fprintf(pf, "\t\treturn sendMemStream.ToArray();\n\t}\n") ;
		
	}
	fprintf(pf,"#endregion\n\n") ;
	fprintf(pf,"#region HandleAction\n") ;
	
	//handle action
	fprintf(pf,"\t\tprivate delegate void HandleActionDelegate(MessageHead head);\n") ;
	fprintf(pf,"\t\tprivate Dictionary<ushort, HandleActionDelegate> HandleDic = new Dictionary<ushort, HandleActionDelegate>(); \n") ;
	
	
	for (int i=0; i<total; ++i) {
		ndxml *sub = ndxml_refsubi(xnode, i) ;
		const char *body = ndxml_getattr_val(sub, "body") ;
		
		fprintf(pf,"\t\tpublic void Action%s( MessageHead head ) \n\t\t{\n",ndxml_getattr_val(sub, "id")) ;
		fprintf(pf,"\t\t\tif( OnHandle%s != null ) {\n",ndxml_getattr_val(sub, "id")) ;
		
		if (body && body[0]) {
			
			fprintf(pf,"\t\t\t\t%s body = new %s();\n",body,body) ;
			fprintf(pf,"\t\t\t\tbody.Read(m_readDataTranser); \n") ;
			fprintf(pf,"\t\t\t\tOnHandle%s(head, body);\n\t\t\t}\n", ndxml_getattr_val(sub, "id")) ;
		}
		else {
			fprintf(pf,"\t\t\t\tOnHandle%s(head);\n\t\t\t}\n", ndxml_getattr_val(sub, "id")) ;
		}
		fprintf(pf,"\t\t} \n") ;
		
	}
	fprintf(pf,"#endregion\n\n") ;
	
	//receiveInit
	
	fprintf(pf,"#region ReceiveInit \n") ;
	fprintf(pf,"\t\tpublic void ReceiveInit() \n\t\t{\n") ;
	
	for (int i=0; i<total; ++i) {
		ndxml *sub = ndxml_refsubi(xnode, i) ;
		fprintf(pf,"\t\t\tHandleDic.Add( MessageIDDefine.%s,new HandleActionDelegate( Action%s ) );\n",
				ndxml_getattr_val(sub, "id"),ndxml_getattr_val(sub, "id")) ;
		
	}
	fprintf(pf,"\t\t} \n") ;
	fprintf(pf,"#endregion\n\n") ;

	
	//end
	fprintf(pf,"\t\tpublic void Recieve( byte[] msgData) \n\t\t{\n") ;
	
	fprintf(pf,"\t\t\tMemoryStream memStream = new MemoryStream(msgData); \n") ;
	fprintf(pf,"\t\t\tm_readDataTranser.SetStream(memStream); \n") ;
	fprintf(pf,"\t\t\tMessageHead head = m_readDataTranser.ReadMessageHead(); \n") ;
	fprintf(pf,"\t\t\tHandleDic[head.id].Invoke(head); \n\t\t}\n") ;
	fprintf(pf, "\t}\n\n#endregion\n");
	
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

	fprintf(pf, "using AutoMessageProtocol;\n");
	fprintf(pf, "using System.IO;\n");
	fprintf(pf, "using System.Text;\n");
	fprintf(pf, "using System.Collections.Generic;\n");
	fprintf(pf, "namespace AutoMessageProtocol\n{\n\n");

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

	if (-1 == xml2Sharp(xmlID, "./csharp/MessageRecvSend.cs", build_csMessageRecvSend)) {
		return -1;
	}
	return 0 ;
	/*
	FILE *pf = fopen(outfile, "w") ;
	if (!pf){
		fprintf(stderr, " open out put file %s error \n", outfile ) ;
		return -1 ;
	}
	
	_OUT_PUT_TIME(pf) ;
	
	fprintf(pf, "using AutoMessageProtocol;\n");
	fprintf(pf, "using System.IO;\n");
	fprintf(pf, "using System.Text;\n");
	fprintf(pf, "using System.Collections.Generic;\n");
	fprintf(pf, "namespace AutoMessageProtocol\n{\n\n");
	
	if(-1==build_csMessageID(xmlID, pf)) {
		return -1 ;
	}
	
	
	if(-1==build_csMarco(xmlMarco, pf)) {
		return -1 ;
	}
	
	if(-1==build_csDataStruct(xmlData, pf)) {
		return -1 ;
	}
	
	if(-1==build_csMessageRecvSend(xmlID, pf)) {
		return -1 ;
	}
	fprintf(pf, "#endregion\n}\n");
	
	fclose(pf);
	return 0;
	*/
}
