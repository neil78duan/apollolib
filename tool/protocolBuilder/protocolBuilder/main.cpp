//
//  main.cpp
//  protocolBuilder
//
//  Created by duanxiuyun on 15-4-10.
//  Copyright (c) 2015年 duanxiuyun. All rights reserved.
//

#include "nd_common/nd_common.h"
#include <string>
#include <vector>

struct type_name_info
{
	const char *alias ;
	const char *type ;
};


static type_name_info alias_type[] = {
	{"char" , "char"} ,
	{"int8_t" , "NDINT8"} ,
	{"int16_t" , "NDINT16"} ,
	{"int32_t" , "NDINT32"} ,
	{"int64_t" , "NDINT64"} ,
	{"uint8_t" , "NDUINT8"} ,
	{"uint16_t" , "NDUINT16"} ,
	{"uint32_t" , "NDUINT32"} ,
	{"uint64_t" , "NDUINT64"} ,
	{"float" , "float"}
};


static type_name_info alias_typeUE4[] = {
	{"char" , "ANSICHAR"} ,
	{"int8_t" , "int8"} ,
	{"int16_t" , "int16"} ,
	{"int32_t" , "int32"} ,
	{"int64_t" , "int64"} ,
	{"uint8_t" , "uint8"} ,
	{"uint16_t" , "uint16"} ,
	{"uint32_t" , "uint32"} ,
	{"uint64_t" , "uint64"} ,
	{"float" , "float"}
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
static bool is_base_type(const char *name)
{
	const char *ret = get_type_from_alias(name, alias_type, sizeof(alias_type) / sizeof(alias_type[0]));
	if (ret) {
		return true;
	}
	return false;
}

const char* getTypeForUE4(const char *in_type)
{
	const char *ret = get_type_from_alias(in_type,alias_type, sizeof(alias_type)/sizeof(alias_type[0]) ) ;
	if (ret) {
		return ret ;
	}
	return in_type ;

}
const char* getType(const char *in_type,const char *version)
{
	static char s_name[128] ;
	const char *ret = get_type_from_alias(in_type,alias_type, sizeof(alias_type)/sizeof(alias_type[0]) ) ;
	if (!ret) {
		
		if (version&& version[0]) {
			snprintf(s_name, sizeof(s_name), "%s_v%s", in_type, version) ;
			return s_name ;
		}
		return in_type ;
	}
	return ret ;
}

#define _OUT_PUT_TIME(_pf) do { \
	fprintf(_pf,"/* please do not change this file ,\n * auto create by program \n * create time %s \n */\n\n", nd_get_datetimestr() ) ; \
}while(0) 


struct ref_order_t {
	int is_used ;
	int is_saveDB ;
	std::string name ;
};
typedef std::vector<ref_order_t> name_list_t ;


int get_name_sort(ndxml_root *xmlfile,name_list_t &res_list)
{
	name_list_t org_list ;
	ndxml *xnode = ndxml_getnode(xmlfile, "DataType") ;
	if (!xnode) {
		fprintf(stderr, " build marco error MarcoDefine node unexist\n" ) ;
		return -1 ;
	}
	
	int total = ndxml_getsub_num(xnode) ;
	
	//get original list
	for (int i=0; i<total; ++i) {
		ndxml *sub = ndxml_refsubi(xnode,i) ;
		ref_order_t node ;
		node.name = ndxml_getname(sub) ;
		node.is_used = 0 ;
		const char *saveDB = ndxml_getattr_val(sub, "saveDB") ;
		if (saveDB) {
			node.is_saveDB = atoi(saveDB) ;
		}
		else {
			node.is_saveDB = 0 ;
		}
		org_list.push_back(node) ;
	}
	//check name 
	for (name_list_t::iterator it= org_list.begin(); it!=org_list.end();++it ) {
		for (name_list_t::iterator it_sub= it + 1; it_sub!=org_list.end();++it_sub ) {
			if (it->name == it_sub->name) {
				fprintf(stderr, "%s name duplicate\n", it->name.c_str());
				exit(1) ;
			}
			
		}
	}
	
	//check  is ref not after this
	int free_numb = org_list.size() ;
	while (free_numb > 0) {
		free_numb = 0 ;
		
		for (name_list_t::iterator it= org_list.begin(); it!=org_list.end();++it ) {
			//cheng reference by other
			if (it->is_used) {
				continue ;
			}
			ndxml *subNode1 = ndxml_refsub(xnode,it->name.c_str()) ;
			int sub_num = ndxml_getsub_num(subNode1) ;
			
			bool is_ref = false ;
			
			for (int i=0; i<sub_num; ++i) {
				ndxml *subNode2 = ndxml_refsubi(subNode1,i) ;
				const char *myName = ndxml_getattr_val(subNode2, "type") ;
				
				for (name_list_t::iterator it_sub= org_list.begin(); it_sub!=org_list.end(); ++it_sub) {
					if (it_sub->is_used || it==it_sub) {
						continue ;
					}
					if (0== strcmp(myName, it_sub->name.c_str()) ) {
						is_ref = true ;
						break ;
					}
				}
			}
			
			if (is_ref==false) {
				res_list.push_back(*it) ;
				it->is_used = 1;
			}
			else {
				++free_numb ;
			}
		}
	}
	return 0;
	
}

//get save db list
int get_saveDB_sort(ndxml_root *xmlfile,name_list_t &res_list,name_list_t &save_list)
{
	ndxml *xnode = ndxml_getnode(xmlfile, "DataType") ;
	if (!xnode) {
		fprintf(stderr, " build marco error MarcoDefine node unexist\n" ) ;
		return -1 ;
	}
	name_list_t save_input ;
	
	
	int add_new = 1 ;
	while (add_new > 0) {
		add_new = 0 ;
		for (name_list_t::iterator it= res_list.begin(); it!=res_list.end(); ++it) {
			if (it->is_saveDB && it->is_used==0) {
				it->is_used = 1;
				++add_new ;
			}
			if (!it->is_used) {
				continue ;
			}
			
			ndxml *subNode1 = ndxml_refsub(xnode,it->name.c_str()) ;
			int sub_num = ndxml_getsub_num(subNode1) ;
			
			for (int i=0; i<sub_num; ++i) {
				ndxml *subNode2 = ndxml_refsubi(subNode1,i) ;
				const char *myName = ndxml_getattr_val(subNode2, "type") ;
				
				const char *ret = get_type_from_alias(myName,alias_type, sizeof(alias_type)/sizeof(alias_type[0]) ) ;
				if (ret) {
					continue ;
				}
				
				for (name_list_t::iterator it_sub= res_list.begin(); it_sub!=res_list.end(); ++it_sub) {
					if (it_sub->is_used) {
						continue;
					}
					if (0== strcmp(myName, it_sub->name.c_str()) ) {
						if (it_sub->is_used==0) {
							it_sub->is_used = 1 ;
							++add_new;
							break ;
						}
					}
				}
			}
			
		}
	}
	
	for (name_list_t::iterator it= res_list.begin(); it!=res_list.end(); ++it) {
		if (it->is_used) {
			save_list.push_back(*it) ;
		}
	}
	return 0;
}

const char *_get_construction_func(char *input,size_t size, ndxml *dataNode)
{
	char *org = input;
	*input = 0;
	int subNodes = ndxml_getsub_num(dataNode);
	for (int n = 0; n < subNodes; ++n) {
		int len = 0;
		ndxml *node1 = ndxml_refsubi(dataNode, n);
		int bIsString = false;
		const char *pType = ndxml_getattr_val(node1, "type");
		if (0 == ndstricmp((char*)pType, (char*)"string") || 0 == ndstricmp((char*)pType, (char*)"char")) {
			bIsString = true;
		}
		const char *pArray = ndxml_getattr_val(node1, "isArray");
		const char *pValName = ndxml_getattr_val(node1, "name");
		const char *pDftVal = ndxml_getattr_val(node1, "default");

		if (pArray && pArray[0] && pArray[0] == '1') {
			if (!bIsString) {
				len = snprintf(input, size, "\t\t%sCount = 0 ;\n", pValName);
			}
			else {
				if (pDftVal) {

					len = snprintf(input, size, "\t\tstrncpy(%s,\"%s\",sizeof(%s));\n", pValName,pDftVal,pValName);
				}
				else {
					len = snprintf(input, size, "\t\t%s[0] = 0 ;\n", pValName);
				}
			}
		}
		else {
			if (is_base_type(pType)) {
				len = snprintf(input, size, "\t\t%s = %s ;\n", pValName, pDftVal ? pDftVal : "0");
			}
		}
		input += len;
		size -= len;
	}
	return  org;
}

const char *_get_copy_function_func(char *input, size_t size, ndxml *dataNode)
{
	char *org = input;
	*input = 0;
	int subNodes = ndxml_getsub_num(dataNode);
	for (int n = 0; n < subNodes; ++n) {
		int len = 0;
		ndxml *node1 = ndxml_refsubi(dataNode, n);
		int bIsString = false;
		const char *pType = ndxml_getattr_val(node1, "type");
		if (0 == ndstricmp((char*)pType, (char*)"string") || 0 == ndstricmp((char*)pType, (char*)"char")) {
			bIsString = true;
		}
		const char *pArray = ndxml_getattr_val(node1, "isArray");
		const char *pValName = ndxml_getattr_val(node1, "name");
		if (pArray && pArray[0] && pArray[0] == '1') {
			if (!bIsString) {
				len = snprintf(input, size, "\t\tfor(int i=0;i<r.%sCount;i++) { \n"
					"\t\t\t%s[i]=r.%s[i];\n\t\t} \n"
					"\t\t%sCount=r.%sCount;\n"
					, pValName, pValName, pValName, pValName, pValName);
			}
			else {
				len = snprintf(input, size, "\t\tstrncpy(%s,r.%s,sizeof(%s));\n", pValName, pValName, pValName);
			}
		}
		else {
			len = snprintf(input, size, "\t\t%s = r.%s ;\n", pValName, pValName);
		}
		input += len;
		size -= len;
	}
	return  org;
}
int _save_dataTypeNode(ndxml *sub, FILE *pf,FILE *pfCpp, const char *version, bool SaveDB)
{
	const char *pName = ndxml_getname(sub) ;
	if (!pName) {
		return-1 ;
	}
	char name_buf[256] ;
	char construct_func[1024*16];
	
	const char *pComment = ndxml_getattr_val(sub, "comment");
	if (pComment) {
		fprintf(pf, "// %s\n", pComment) ;
	}
	
	if (SaveDB) {
		snprintf(name_buf, sizeof(name_buf), "%s_v%s",pName,version) ;
		pName = name_buf ;
		
		//fprintf(pf, "struct %s \n{\n ", pName) ;
		//fprintf(pf, "\t%s() \n\t{\n\t\tmemset(this, 0, sizeof(*this) ); \n\t}\n\n",pName, ) ;
	}
	else {
		//fprintf(pf, "struct %s \n{\n ", pName) ;
		//fprintf(pf, "\t%s() \n\t{\n\t\tmemset(this, 0, sizeof(*this) ); \n\t}\n\n",pName) ;
	}
	fprintf(pf, "struct %s \n{\n ", pName);
	fprintf(pf, "\t%s() \n\t{\n%s\t}\n\n", pName, _get_construction_func(construct_func, sizeof(construct_func), sub));
	fprintf(pf, "\t%s& operator=(const %s &r)\n\t{\n%s\t\treturn *this;\n\t}\n\n",
		pName, pName, _get_copy_function_func(construct_func, sizeof(construct_func), sub));


	char buf_read_func[4096] ;
	char buf_write_func[4096];
	char buf_toUser_func[4096];
	char buf_fromUser_func[4096];

	char buf_toJson_func[4096];

	char *pReadStream = buf_read_func;
	char *pWriteStream = buf_write_func ;
	char *pTouserStream = buf_toUser_func;
	char *pFromuserStream = buf_fromUser_func;

	char *pToJsonStream = buf_toJson_func;

	size_t read_size = sizeof(buf_read_func);
	size_t write_size = sizeof(buf_write_func);

	size_t toUser_size = sizeof(buf_toUser_func);
	size_t fromUser_size = sizeof(buf_fromUser_func);
	size_t toJson_size = sizeof(buf_toJson_func);

	int subNodes = ndxml_getsub_num(sub) ;
	for (int n=0; n<subNodes; ++n) {
		ndxml *node1 = ndxml_refsubi(sub,n) ;
		int bIsString =false ;
		const char *pType = ndxml_getattr_val(node1, "type");
		if (0==ndstricmp((char*)pType, (char*)"string")||0==ndstricmp((char*)pType, (char*)"char") ) {
			pType = "char" ;
			bIsString = true ;
		}
		const char *pArray = ndxml_getattr_val(node1,"isArray") ;
		const char *pValName = ndxml_getattr_val(node1, "name") ;

// 		if (is_base_type(pType)) {
// 			int size = snprintf(pToJsonStream, toJson_size, "\t\tJSON_NODE_NEW_LINE(p, outSize);\n");
// 			pToJsonStream += size;
// 			toJson_size -= size;
// 		}

		if (pArray && pArray[0] && pArray[0]=='1') {
			
			if (bIsString) {
				
				fprintf(pf,"\t%s  %s[%s]; \t//%s \n",getType(pType,version), pValName,
						ndxml_getattr_val(node1, "arrayMarco"),ndxml_getattr_val(node1, "comment") ) ;
				
				//read-string function
				int size = snprintf(pReadStream, read_size, "\t\tREAD_STREAM_STRING(inmsg, data.%s, %s);\n",pValName , ndxml_getattr_val(node1, "arrayMarco")) ;
				pReadStream += size ;
				read_size -= size;
				
				//write string function
				size = snprintf(pWriteStream, write_size, "\t\tWRITE_STREAM(omsg,data.%s);\n",pValName ) ;
				pWriteStream += size ;
				write_size -= size;
				
				//write to userdefine function
				size = snprintf(pTouserStream, toUser_size, "\t\tTO_USER_DEF_TEXT(paramUserData, %s, paramInputData);\n", pValName);
				pTouserStream += size;
				toUser_size -= size;

				size = snprintf(pFromuserStream, fromUser_size, "\t\tFROM_USER_DEF_TEXT(paramUserData, %s, paramInputData, %s);\n", pValName, ndxml_getattr_val(node1, "arrayMarco"));
				pFromuserStream += size;
				fromUser_size -= size;

				//to json char *pToJsonStream = buf_toJson_func; 		size_t PrintJson(char *buf, size_t bufsize, const char *name, NDUINT8 &data);
				size = snprintf(pToJsonStream, toJson_size, "\t\tJSON_BASE_PRINT_TOBUF(p, outSize, \"%s\", (const char*)data.%s);\n", pValName, pValName);
				pToJsonStream += size;
				toJson_size -= size;

			}
			else {
				
				fprintf(pf,"\tNDUINT16  %sCount; \t \n", pValName ) ;
				fprintf(pf,"\t%s  %s[%s]; \t//%s \n",getType(pType,version), pValName,
						ndxml_getattr_val(node1, "arrayMarco"),
						ndxml_getattr_val(node1, "comment") ) ;
				
				//write push function
				fprintf(pf, "\tbool push_%s(const %s &val)\n\t{\n", pValName, getType(pType, version));
				fprintf(pf, "\t\tif (%sCount < %s)	{\n", pValName, ndxml_getattr_val(node1, "arrayMarco"));
				fprintf(pf, "\t\t\t%s[%sCount++] = val;\n", pValName, pValName);
				fprintf(pf, "\t\t\treturn true;\n\t\t}\n\t\treturn false;\n\t}\n\n");

				//read array string fucntion
				int size = snprintf(pReadStream, read_size,
								   "\n\t\tREAD_STREAM(inmsg,data.%sCount);\n"
								   "\t\tif( data.%sCount > %s ) {\n\t\t\treturn -1;\n\t\t}\n ",
								   pValName,pValName, ndxml_getattr_val(node1, "arrayMarco") ) ;
				pReadStream += size ;
				read_size -= size;

				size = snprintf(pReadStream, read_size,
							   "\t\tfor(int i =0; i<data.%sCount;++i )"
							   "{\n\t\t\tREAD_STREAM(inmsg,data.%s[i]);\n\t\t}\n ",
							   pValName, pValName ) ;
				pReadStream += size ;
				read_size -= size;
				
				//write array string function
				size = snprintf(pWriteStream, write_size,
								"\n\t\tif(data.%sCount > %s) {\n\t\t\treturn -1; \n\t\t} \n ",
								pValName, ndxml_getattr_val(node1, "arrayMarco") ) ;
				pWriteStream += size ;
				write_size -= size;
				
				size = snprintf(pWriteStream, write_size,
								"\t\tWRITE_STREAM(omsg,data.%sCount);\n"
								"\t\tfor(int i =0 ; i < data.%sCount;++i){\n"
								"\t\t\tWRITE_STREAM(omsg,data.%s[i]); \n\t\t} \n",
								pValName,pValName,pValName ) ;
				
				pWriteStream += size ;
				write_size -= size;
				
				//usef define function 
				if (is_base_type(pType)) {
					size = snprintf(pTouserStream, toUser_size, "\t\tTO_USER_DEF_BASE_ARR(paramUserData, %s, paramInputData);\n", pValName);
					pTouserStream += size;
					toUser_size -= size;

					size = snprintf(pFromuserStream, fromUser_size, "\t\tFROM_USER_DEF_BASE_ARR(paramUserData, %s, paramInputData, %s);\n", pValName, ndxml_getattr_val(node1, "arrayMarco"));
					pFromuserStream += size;
					fromUser_size -= size;

					size = snprintf(pToJsonStream, toJson_size, "\t\tJSON_BASEARRAY_PRINT_TOBUF(p, outSize, \"%s\", data.%s,data.%sCount);\n", pValName, pValName, pValName);
					pToJsonStream += size;
					toJson_size -= size;
				}
				else {
					size = snprintf(pTouserStream, toUser_size, "\t\tTO_USER_DEF_NODE_ARR(paramUserData, %s, paramInputData);\n", pValName);
					pTouserStream += size;
					toUser_size -= size;

					size = snprintf(pFromuserStream, fromUser_size, "\t\tFROM_USER_DEF_NODE_ARR(paramUserData, %s, paramInputData, %s);\n", pValName, ndxml_getattr_val(node1, "arrayMarco"));
					pFromuserStream += size;
					fromUser_size -= size;

					size = snprintf(pToJsonStream, toJson_size, "\t\tJSON_ARRAY_PRINT_TOBUF(p, outSize, \"%s\", data.%s,data.%sCount);\n", pValName, pValName, pValName);
					pToJsonStream += size;
					toJson_size -= size;
				}

			}
		}
		else {
			
			fprintf(pf,"\t%s  %s; \t//%s \n",getType(pType,version), pValName,
					ndxml_getattr_val(node1, "comment") ) ;
			
			//readstream
			int size = snprintf(pReadStream, read_size, "\t\tREAD_STREAM(inmsg, data.%s);\n", pValName);
			pReadStream += size ;
			read_size -= size;
			
			size = snprintf(pWriteStream, write_size, "\t\tWRITE_STREAM(omsg,data.%s);\n", pValName);
			pWriteStream += size ;
			write_size -= size;

			if (is_base_type(pType) || ndstricmp("binaryData",pType)==0) {
				size = snprintf(pTouserStream, toUser_size, "\t\tTO_USER_DEF_ELE(paramUserData, %s, paramInputData);\n", pValName);
				pTouserStream += size;
				toUser_size -= size;

				size = snprintf(pFromuserStream, fromUser_size, "\t\tFROM_USER_DEF_ELE(paramUserData, %s, paramInputData);\n", pValName);
				pFromuserStream += size;
				fromUser_size -= size;

				size = snprintf(pToJsonStream, toJson_size, "\t\tJSON_BASE_PRINT_TOBUF(p, outSize, \"%s\", data.%s);\n", pValName, pValName);
				pToJsonStream += size;
				toJson_size -= size;
			}
			else {
				size = snprintf(pTouserStream, toUser_size, "\t\tTO_USER_DEF_NODE(paramUserData, %s, paramInputData);\n", pValName);
				pTouserStream += size;
				toUser_size -= size;

				size = snprintf(pFromuserStream, fromUser_size, "\t\tFROM_USER_DEF_NODE(paramUserData, %s, paramInputData);\n", pValName);
				pFromuserStream += size;
				fromUser_size -= size;


				size = snprintf(pToJsonStream, toJson_size, "\t\tJSON_NODE_PRINT_TOBUF(p, outSize, \"%s\", data.%s);\n", pValName, pValName);
				pToJsonStream += size;
				toJson_size -= size;
			}

		}
		
		if (n==subNodes-1) {
			/*int size = snprintf(pToJsonStream, toJson_size, "\t\tJSON_NODE_NEW_LINE(p, outSize);\n");
			pToJsonStream += size;
			toJson_size -= size;*/
		}
		else {
			int size = snprintf(pToJsonStream, toJson_size, "\t\tJSON_NODE_INTERVAL(p, outSize);\n");
			pToJsonStream += size;
			toJson_size -= size;
		}
	}
	
	fprintf(pf, "};\n") ;

	fprintf(pfCpp, "\n\tint ReadStream(NDIStreamMsg &inmsg,NetMessage::%s &data)\n\t{\n", pName) ;
	fprintf(pfCpp, "%s\n\t\treturn 0; \n\t}\n", buf_read_func) ;
	
	fprintf(pfCpp, "\tint WriteStream(NDOStreamMsg &omsg,const NetMessage::%s &data)\n\t{\n", pName) ;
	fprintf(pfCpp, "%s\n\t\treturn 0; \n\t}\n", buf_write_func) ;
	
	fprintf(pf, "int ReadStream(NDIStreamMsg &inmsg,NetMessage::%s &data);\n", pName) ;
	fprintf(pf, "int WriteStream(NDOStreamMsg &omsg,const NetMessage::%s &data);\n", pName) ;
	
	//write json 	size_t PrintJson(char *buf, size_t bufsize, const char *name, const char*data);
	fprintf(pf, "size_t PrintJson(char *buf, size_t bufsize, const char *, %s &data);\n", pName);
	fprintf(pfCpp, "\n\tsize_t PrintJson(char *outBuf, size_t outSize, const char *name, %s &data)\n ", pName);	
	fprintf(pfCpp, "\t{\n\t\tchar *p = outBuf;\n\t\tJSON_NODE_OUT_BEGIN(p,outSize);\n", pName);
	fprintf(pfCpp, "%s\n\t\tJSON_NODE_OUT_END(p,outSize);\n\t\treturn (p-outBuf); \n\t}\n", buf_toJson_func);

	//write to user-define data paramUserData, %s, paramInputData
	fprintf(pfCpp, "#ifndef WITHOUT_LOGIC_PARSER\n");
	fprintf(pfCpp, "\n\tint ToUserDef(LogicUserDefStruct &paramUserData,const NetMessage::%s &paramInputData)\n\t{\n", pName);
	fprintf(pfCpp, "%s\n\t\treturn 0; \n\t}\n", buf_toUser_func);

	fprintf(pfCpp, "\tint  FromUserDef(const LogicUserDefStruct &paramUserData, NetMessage::%s &paramInputData)\n\t{\n", pName);
	fprintf(pfCpp, "%s\n\t\treturn 0; \n\t}\n", buf_fromUser_func);
	fprintf(pfCpp, "#endif\n\n");


	fprintf(pf, "#ifndef WITHOUT_LOGIC_PARSER\n");
	fprintf(pf, "int ToUserDef(LogicUserDefStruct &userData,const NetMessage::%s &data);\n", pName);
	fprintf(pf, "int FromUserDef(const LogicUserDefStruct &userData, NetMessage::%s &data);\n", pName);
	fprintf(pf, "#endif\n\n\n");

	return 0;
}
//build datatype
int build_dataType(ndxml_root *xmlfile, const char *out_file,bool saveDB)
{
	ndxml *xnode = ndxml_getnode(xmlfile, "DataType") ;
	if (!xnode) {
		fprintf(stderr, " build marco error MarcoDefine node unexist\n" ) ;
		return -1 ;
	}
	ndxml *xInfo = ndxml_getnode(xmlfile, "DataDesc") ;
	if (!xInfo) {
		return -1;
	}
	const char *version = ndxml_getattr_val(xInfo, "version");
	if (!version) {
		return -1;
	}
	
	const char *nameSpace = ndxml_getattr_val(xnode, "namespace"); 

	name_list_t res_list ;
	get_name_sort(xmlfile, res_list);
	
	NDUINT64 nowTime = (NDUINT64) time(NULL);
	if (saveDB) {
		
		char buf[128] ;
		snprintf(buf, sizeof(buf), "%s_v%s.h",out_file, version );
		FILE *pf = fopen(buf, "w") ;
		if (!pf){
			fprintf(stderr, " open out put file %s error \n", buf ) ;
			return -1 ;
		}
		snprintf(buf, sizeof(buf), "%s_v%s.cpp",out_file, version  );
		FILE *pfCpp = fopen(buf, "w") ;
		if (!pf){
			fprintf(stderr, " open out put file %s error \n", buf ) ;
			return -1 ;
		}
		
		_OUT_PUT_TIME(pfCpp) ;
		fprintf(pfCpp, "\n\n#include \"netMessage/dataStream.h\"\n");
		fprintf(pfCpp, "\n\n#include \"netMessage/auto_dataTypeDb_v%s.h\"\n", version );
		//fprintf(pfCpp, "namespace NetMessage \n{\n" );
		if (nameSpace) {
			fprintf(pfCpp, "namespace %s \n{\n", nameSpace);
		}
		else {
			fprintf(pfCpp, "/* undefined namespace*/\n{\n");
		}


		
		_OUT_PUT_TIME(pf) ;
		fprintf(pf, "#ifndef _AUTO_DATA_TYPE_DEFINE_DB_%s_H_\n",version );
		fprintf(pf, "#define _AUTO_DATA_TYPE_DEFINE_DB_%s_H_\n",version );
		fprintf(pf, "#include \"netMessage/auto_macroDefine.h\"\n\n") ;
		fprintf(pf, "#include \"netMessage/dataStream.h\"\n\n");
		fprintf(pf, "#include \"netMessage/dataBinary.h\"\n\n");

		if (nameSpace) {
			fprintf(pf, "namespace %s \n{\n", nameSpace);
		}
		else {
			fprintf(pf, "/* undefined namespace*/\n{\n");
		}
		
		
		name_list_t save_list;
		get_saveDB_sort(xmlfile,res_list,save_list);
		
		int total =(int) save_list.size(); //ndxml_getsub_num(xnode) ;
		for (int i=0; i<total; ++i) {
			ndxml *sub = ndxml_refsub(xnode,save_list[i].name.c_str()) ;
			_save_dataTypeNode(sub,  pf,pfCpp ,version, true) ;
			fprintf(pf, "\n\n") ;
		}
		fprintf(pf,"};\n\n#endif\n\n") ;
		fclose(pf) ;
		
		fprintf(pfCpp,"};\n\n") ;
		fclose(pfCpp) ;
		
	}
	else {
		
		char buf[128] ;
		snprintf(buf, sizeof(buf), "%s.h",out_file );
		FILE *pf = fopen(buf, "w") ;
		if (!pf){
			fprintf(stderr, " open out put file %s error \n", buf ) ;
			return -1 ;
		}
		snprintf(buf, sizeof(buf), "%s.cpp",out_file );
		FILE *pfCpp = fopen(buf, "w") ;
		if (!pf){
			fprintf(stderr, " open out put file %s error \n", buf ) ;
			return -1 ;
		}

		_OUT_PUT_TIME(pfCpp) ;
		fprintf(pfCpp, "\n\n#include \"netMessage/auto_dataType.h\"\n" );
		//fprintf(pfCpp, "namespace NetMessage \n{\n" );

		if (nameSpace) {
			fprintf(pfCpp, "namespace %s \n{\n", nameSpace);
		}
		else {
			fprintf(pfCpp, "/* undefined namespace*/\n{\n");
		}

		
		_OUT_PUT_TIME(pf) ;
		fprintf(pf, "#ifndef _AUTO_DATA_TYPE_DEFINE_H_\n" );
		fprintf(pf, "#define _AUTO_DATA_TYPE_DEFINE_H_\n" );;
		fprintf(pf, "#include \"netMessage/auto_macroDefine.h\"\n\n") ;
		fprintf(pf, "#include \"netMessage/dataStream.h\"\n\n") ;
		fprintf(pf, "#include \"netMessage/dataBinary.h\"\n\n");

		fprintf(pf, "#define DATA_IN_DB_VERSION %s \n\n", version);
		fprintf(pf, "#define NET_MESSAGE_EXPORT_TM %lld \n", nowTime);
		

		if (nameSpace) {
			fprintf(pf, "namespace %s \n{\n", nameSpace);
		}
		else {
			fprintf(pf, "/* undefined namespace*/\n{\n");
		}

		int total =(int) res_list.size(); //ndxml_getsub_num(xnode) ;
		for (int i=0; i<total; ++i) {
			ndxml *sub = ndxml_refsub(xnode,res_list[i].name.c_str()) ;
			_save_dataTypeNode(sub,  pf,pfCpp, NULL, false) ;
			fprintf(pf, "\n") ;
		}
		
		fprintf(pf,"};\n\n#endif\n\n") ;
		fclose(pf) ;
		
		fprintf(pfCpp,"};\n\n") ;
		fclose(pfCpp) ;
	}

	return 0 ;
}
/*
 
 
 USTRUCT(BlueprintType)
 struct FRoleAttrNodeVal
 {
	GENERATED_USTRUCT_BODY()
 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RoleAttrNodeVal")
	int32 id;
 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RoleAttrNodeVal")
	float val;
 };
 
 */

int _out_data_forUE(ndxml *sub, FILE *pf,FILE *pfCpp)
{
	const char *pName = ndxml_getname(sub) ;
	if (!pName) {
		return-1 ;
	}
	//char name_buf[256] ;
	
	const char *pComment = ndxml_getattr_val(sub, "comment");
	if (pComment) {
		fprintf(pf, "// %s\n", pComment) ;
	}
	fprintf(pf, "USTRUCT(BlueprintType)\nstruct %s \n{\n\tGENERATED_USTRUCT_BODY();\n", pName) ;
	
	char buf_read_func[4096] ;
	char buf_write_func[4096] ;
	char *pReadStream = buf_read_func;
	char *pWriteStream = buf_write_func ;
	size_t read_size = sizeof(buf_read_func);
	size_t write_size = sizeof(buf_write_func);
	
	int subNodes = ndxml_getsub_num(sub) ;
	for (int n=0; n<subNodes; ++n) {
		ndxml *node1 = ndxml_refsubi(sub,n) ;
		int bIsString =false ;
		const char *pType = ndxml_getattr_val(node1, "type");
		if (0==ndstricmp((char*)pType, (char*)"string")||0==ndstricmp((char*)pType, (char*)"char") ) {
			pType = "char" ;
			bIsString = true ;
		}
		const char *pArray = ndxml_getattr_val(node1,"isArray") ;
		const char *pValName = ndxml_getattr_val(node1, "name") ;
		fprintf(pf,"\tUPROPERTY(EditAnywhere, BlueprintReadWrite, Category = \"%s\")\n ", "NetStream") ;
		if (pArray && pArray[0] && pArray[0]=='1') {
			
			
			if (bIsString) {
				fprintf(pf,"\t%s  %s[%s]; \t//%s \n\n",getTypeForUE4(pType), pValName,
						ndxml_getattr_val(node1, "arrayMarco"),ndxml_getattr_val(node1, "comment") ) ;
				
				//read-string function
				int size = snprintf(pReadStream, read_size, "\tREAD_STREAM_STRING(inmsg, data.%s, %s);\n",pValName , ndxml_getattr_val(node1, "arrayMarco")) ;
				pReadStream += size ;
				read_size -= size;
				
				//write string function
				size = snprintf(pWriteStream, write_size, "\tWRITE_STREAM(omsg,data.%s);\n",pValName ) ;
				pWriteStream += size ;
				write_size -= size;
				
			}
			else {
				
				fprintf(pf,"\tuint16  %sCount; \t \n", pValName ) ;
				fprintf(pf,"\tUPROPERTY(EditAnywhere, BlueprintReadWrite, Category = \"%s\")\n ", "NetStream") ;
				fprintf(pf,"\t%s  %s[%s]; \t//%s \n\n",getTypeForUE4(pType), pValName,
						ndxml_getattr_val(node1, "arrayMarco"),
						ndxml_getattr_val(node1, "comment") ) ;
				
				//write push function
				//
				fprintf(pf, "\tUFUNCTION(BlueprintPure, Category = \"%s\")\n", "NetStream") ;
				fprintf(pf, "\tbool push_%s(const %s &val)\n\t{\n", pValName, getTypeForUE4(pType));
				fprintf(pf, "\t\tif (%sCount < %s)	{\n", pValName, ndxml_getattr_val(node1, "arrayMarco"));
				fprintf(pf, "\t\t\t%s[%sCount++] = val;\n", pValName, pValName);
				fprintf(pf, "\t\t\treturn true;\n\t\t}\n\t\treturn false;\n\t}\n\n");
				
				//read array string fucntion
				int size = snprintf(pReadStream, read_size,
									"\n\tREAD_STREAM(inmsg,data.%sCount);\n"
									"\tif( data.%sCount > %s ) {\n\t\treturn -1;\n\t}\n ",
									pValName,pValName, ndxml_getattr_val(node1, "arrayMarco") ) ;
				pReadStream += size ;
				read_size -= size;
				
				size = snprintf(pReadStream, read_size,
								"\tfor(int i =0; i<data.%sCount;++i )"
								"{\n\t\tREAD_STREAM(inmsg,data.%s[i]);\n\t}\n ",
								pValName, pValName ) ;
				pReadStream += size ;
				read_size -= size;
				
				//write array string function
				size = snprintf(pWriteStream, write_size,
								"\n\tif(data.%sCount > %s) {\n\t\treturn -1; \n\t} \n ",
								pValName, ndxml_getattr_val(node1, "arrayMarco") ) ;
				pWriteStream += size ;
				write_size -= size;
				
				size = snprintf(pWriteStream, write_size,
								"\tWRITE_STREAM(omsg,data.%sCount);\n"
								"\tfor(int i =0 ; i < data.%sCount;++i){\n"
								"\t\tWRITE_STREAM(omsg,data.%s[i]); \n\t} \n",
								pValName,pValName,pValName ) ;
				
				pWriteStream += size ;
				write_size -= size;
				
			}
		}
		else {
			fprintf(pf,"\t%s  %s; \t//%s \n",getTypeForUE4(pType), pValName,
					ndxml_getattr_val(node1, "comment") ) ;
			
			//readstream
			int size = snprintf(pReadStream, read_size, "\tREAD_STREAM(inmsg, data.%s);\n", pValName);
			pReadStream += size ;
			read_size -= size;
			
			size = snprintf(pWriteStream, write_size, "\tWRITE_STREAM(omsg,data.%s);\n", pValName);
			pWriteStream += size ;
			write_size -= size;
		}
		
		fprintf(pf,"\n") ;
	}
	
	fprintf(pf, "\tUFUNCTION(BlueprintPure, Category = \"%s\")\n","NetStream") ;
	fprintf(pf, "\tint ReadStream_%s(NDIStreamMsg &inmsg);\n\n", pName) ;
	
	fprintf(pf, "\tUFUNCTION(BlueprintPure, Category = \"%s\"\n", "NetStream") ;
	fprintf(pf, "\tint WriteStream_%s(NDOStreamMsg &omsg) const;\n\n",pName) ;
	
	//out function body
	fprintf(pfCpp, "int %s::ReadStream_%s(NDIStreamMsg &inmsg)\n{\n", pName, pName) ;
	fprintf(pfCpp, "\treturn ReadStream(inmsg,*this);\n}\n\n") ;
	
	fprintf(pfCpp, "int %s::WriteStream_%s(NDOStreamMsg &omsg) const\n{\n",pName,pName) ;
	fprintf(pfCpp, "\treturn WriteStream(omsg,*this);\n}\n\n") ;
	
	fprintf(pf, "};\n") ;
	
	fprintf(pfCpp, "\nint ReadStream(NDIStreamMsg &inmsg,%s &data)\n{\n", pName) ;
	fprintf(pfCpp, "%s\n\treturn 0; \n}\n\n", buf_read_func) ;
	
	fprintf(pfCpp, "int WriteStream(NDOStreamMsg &omsg,const %s &data)\n{\n",pName) ;
	fprintf(pfCpp, "%s\n\treturn 0; \n}\n\n\n", buf_write_func) ;
	
	fprintf(pf, "int ReadStream(NDIStreamMsg &inmsg,%s &data);\n", pName) ;
	fprintf(pf, "int WriteStream(NDOStreamMsg &omsg,const %s &data);\n",pName) ;
	
	return 0;
}

int build_dataTypeForUE4(ndxml_root *xmlfile, const char *out_file)
{
	ndxml *xnode = ndxml_getnode(xmlfile, "DataType") ;
	if (!xnode) {
		fprintf(stderr, " build marco error MarcoDefine node unexist\n" ) ;
		return -1 ;
	}
	ndxml *xInfo = ndxml_getnode(xmlfile, "DataDesc") ;
	if (!xInfo) {
		return -1;
	}
	
	name_list_t res_list ;
	get_name_sort(xmlfile, res_list);
	
	char buf[128] ;
	snprintf(buf, sizeof(buf), "%s.h",out_file );
	FILE *pf = fopen(buf, "w") ;
	if (!pf){
		fprintf(stderr, " open out put file %s error \n", buf ) ;
		return -1 ;
	}
	snprintf(buf, sizeof(buf), "%s.cpp",out_file );
	FILE *pfCpp = fopen(buf, "w") ;
	if (!pf){
		fprintf(stderr, " open out put file %s error \n", buf ) ;
		return -1 ;
	}
	
	_OUT_PUT_TIME(pfCpp) ;
	fprintf(pfCpp, "\n\n#include \"netMessage/dataStream.h\"\n" );
	fprintf(pfCpp, "#include \"netMessage/netStreamAutoDefine.h\"\n\n\n" );
	//fprintf(pfCpp, "namespace NetMessage \n{\n" );
	
	_OUT_PUT_TIME(pf) ;
	fprintf(pf, "#ifndef _NET_STREAM_AUTO_DEFINE_H_\n" );
	fprintf(pf, "#define _NET_STREAM_AUTO_DEFINE_H_\n" );;
	fprintf(pf, "#include \"Object.h\" \n\nclass NDIStreamMsg;\nclass  NDOStreamMsg;\n\n") ;
	//fprintf(pf, "#include \"dataStream.h\"\n\n") ;
	//fprintf(pf, "#define DATA_IN_DB_VERSION %s \n\n", version );
	//fprintf(pf, "namespace NetMessage \n{\n" );
	
	int total =(int) res_list.size(); //ndxml_getsub_num(xnode) ;
	for (int i=0; i<total; ++i) {
		ndxml *sub = ndxml_refsub(xnode,res_list[i].name.c_str()) ;
		_out_data_forUE(sub,  pf,pfCpp) ;
		fprintf(pf, "\n") ;
	}
	
	fprintf(pf,"\n#endif\n\n") ;
	fclose(pf) ;
	
	//fprintf(pfCpp,"};\n\n") ;
	fclose(pfCpp) ;

	return 0 ;
}

int build_messageID(ndxml_root *xmlfile, const char *out_file)
{
	ndxml *xnode = ndxml_getnode(xmlfile, "MessageDefine") ;
	if (!xnode) {
		fprintf(stderr, " build marco error MarcoDefine node unexist\n" ) ;
		return -1 ;
	}
	FILE *pf = fopen(out_file, "w") ;
	if (!pf){
		fprintf(stderr, " open out put file %s error \n", out_file ) ;
		
		return -1 ;
	}
	
	_OUT_PUT_TIME(pf) ;
	
	int total = ndxml_getsub_num(xnode) ;
	
	for (int x = 0; x < total; x++)	{
		ndxml *mainMessage = ndxml_refsubi(xnode, x);
		int subNum = ndxml_getsub_num(mainMessage);

		NDUINT8 maxID = 0, minID = 0;

		const char *p = ndxml_getattr_val(mainMessage, "maxMessageId");
		if (!p || !*p)	{
			continue;
		}
		maxID = atoi(p);
		fprintf(pf, "\n\n\n// %s\n", ndxml_getattr_val(mainMessage,"name"));


		for (int i = 0; i < subNum; ++i) {
			ndxml *sub = ndxml_refsubi(mainMessage, i);
			
			const char *dataInfo = ndxml_getattr_val(sub, "body");
			if (!dataInfo || !*dataInfo) {
				dataInfo = "none";
			}
			fprintf(pf, "_APOLLO_MSG_ID_DEFINE(%s, 0x%x , \"%s\") //(%d,%d)%s,data-format:%s\n", 
				ndxml_getattr_val(sub, "id"),ND_MAKE_WORD(maxID, minID), 
				dataInfo, maxID, minID,ndxml_getattr_val(sub, "comment"), dataInfo);
			++minID;
		}

	}
	
	//fprintf(pf,"};\n#endif\n") ;
	
	fclose(pf) ;
	return 0 ;
}

static int exp_enum(ndxml *node, FILE *pf)
{
	int total =(int) ndxml_getsub_num(node) ;
	for (int i=0; i<total; ++i) {
		ndxml *sub = ndxml_getnodei(node, i);
		nd_assert(sub) ;
		
		const char *pcomment = ndxml_getattr_val(sub, "comment") ;
		if (pcomment) {
			fprintf(pf, "// %s \n", pcomment) ;
		}
		
		fprintf(pf, "enum %s { \n", ndxml_getname(sub)) ;
		for (int x=0; x< ndxml_getsub_num(sub); ++x) {
			ndxml *element = ndxml_getnodei(sub, x) ;
			pcomment = ndxml_getattr_val(element, "comment") ;
			const char *pvalue = ndxml_getattr_val(element, "value") ;
			
			if (pvalue && pvalue[0] ) {
				fprintf(pf, "\t%s =%s,//%s\n", ndxml_getname(element),pvalue, pcomment?pcomment:"\t " ) ;
			}
			else {
				fprintf(pf, "\t%s,//%s\n", ndxml_getname(element),  pcomment?pcomment:"\t ");
			}
		}
		
		fprintf(pf, "};\n\n\n") ;
	}
	
	return 0;
}
int build_marco(ndxml_root *xmlfile, const char *out_file)
{
	ndxml *xnode = ndxml_getnode(xmlfile, "MarcoDefine") ;
	if (!xnode) {
		fprintf(stderr, " build marco error MarcoDefine node unexist\n" ) ;
		return -1 ;
	}
	FILE *pf = fopen(out_file, "w") ;
	if (!pf){
		fprintf(stderr, " open out put file %s error \n", out_file ) ;

		return -1 ;
	}
	
	_OUT_PUT_TIME(pf) ;
	fprintf(pf, "#ifndef _AUTO_MACRO_DEFINE_H_\n" );
	fprintf(pf, "#define _AUTO_MACRO_DEFINE_H_\n" );
	fprintf(pf, "namespace NetMessage \n{\n" );
			
	int total = ndxml_getsub_num(xnode) ;
	
	for (int i=0; i<total; ++i) {
		ndxml *sub = ndxml_refsubi(xnode, i) ;
		fprintf(pf,"#define \t %s  \t\t%s \t\t//%s \n\n", ndxml_getattr_val(sub, "name"),
				ndxml_getattr_val(sub, "value"),
				ndxml_getattr_val(sub, "comment") ) ;
	}
	//enum define
	ndxml *pEnumXml = ndxml_getnode(xmlfile, "EnumDefine") ;
	if (pEnumXml) {
		exp_enum(pEnumXml, pf) ;
	}
	fprintf(pf,"};\n#endif\n") ;
	
	fclose(pf) ;
	return 0 ;
}

//build message handler function
/*
int build_messageHandler(ndxml_root *xmlfile, const char *out_file, const char *install_entry)
{
	ndxml *xnode = ndxml_getnode(xmlfile, "MessageDefine") ;
	if (!xnode) {
		fprintf(stderr, " build marco error MarcoDefine node unexist\n" ) ;
		return -1 ;
	}
	//OUTPUT HEADFILE
	do {
		FILE *pf = fopen(out_file, "w") ;
		if (!pf){
			fprintf(stderr, " open out put file %s error \n", out_file ) ;
			return -1 ;
		}
		
		_OUT_PUT_TIME(pf) ;
		fprintf(pf, "#ifndef _AUTO_MESSAGE_HANDLER_H_\n" );
		fprintf(pf, "#define _AUTO_MESSAGE_HANDLER_H_\n" );
		
		fprintf(pf, "\n#include \"msgHandlerDft.h\"\n\n") ;
		
		fprintf(pf, "extern void install_message_handler(nd_handle h); \n\n" );
		fprintf(pf, "namespace NetMessage \n { \n" );
		
		fprintf(pf, "class MessageHandlerBase : public NDObject \n{\npublic:\n\n" );
		
		int total = ndxml_getsub_num(xnode) ;
		
		for (int i=0; i<total; ++i) {
			ndxml *sub = ndxml_refsubi(xnode, i) ;
			const char *body = ndxml_getattr_val(sub, "body") ;
			const char *msgName = ndxml_getattr_val(sub, "id") ;
			
			if (body && body[0]) {
				
				fprintf(pf,"\tvirtual int  Handler%s(NDObject *h,NDIStreamMsg &inmsg,const NetMessage::%s& data)\n"
						"\t{\n\t\tDEFAULT_MESSAGE_HANDLE(%s) ;\n\t}\n",msgName, body,msgName ) ;
			}
			else {
				
				fprintf(pf,"\tvirtual int  Handler%s(NDObject *h,NDIStreamMsg &inmsg)\n"
						"\t{\n\t\tDEFAULT_MESSAGE_HANDLE(%s);\n\t}\n",msgName,msgName ) ;
			}
			
		}
				
		fprintf(pf,"};\n};\n#endif\n") ;
		fclose(pf) ;
	}while(0) ;
	
	//output message entry
	do {
		//create install function
		FILE *pf = fopen(install_entry, "w") ;
		if (!pf){
			fprintf(stderr, " open out put file %s error \n", out_file ) ;
			return -1 ;
		}
		
		_OUT_PUT_TIME(pf) ;
		fprintf(pf, "//create by auto tool \n//message handler function \n\n" );
		
		fprintf(pf, "#include \"ndapplib/applib.h\" \n\n" );
		
		fprintf(pf, "#include \"apollo_msg_entry.h\" \n\n" );
		
		int total = ndxml_getsub_num(xnode) ;
		//function entry
		for (int i=0; i<total; ++i) {
			ndxml *sub = ndxml_refsubi(xnode, i) ;
			const char *body = ndxml_getattr_val(sub, "body") ;
			
			if (body && body[0]) {
				
				fprintf(pf,"MSG_ENTRY_INSTANCE(%s_entry)\n"
						"{\n\tCALL_APOLLO_FUNCTION_ENTRY(nethandle,msg,%s,%s);\n}\n",
						ndxml_getattr_val(sub, "id"),body,ndxml_getattr_val(sub, "id")) ;
			}
			else {
				fprintf(pf,"MSG_ENTRY_INSTANCE(%s_entry)\n"
						"{\n\tCALL_APOLLO_FUNCTION_NO_PARAM_ENTRY(nethandle,msg,%s);\n}\n",
						ndxml_getattr_val(sub, "id"),ndxml_getattr_val(sub, "id")) ;
			}
			
		}
		//function install
		fprintf(pf, "void install_message_handler(nd_handle h)\n{\n\n" );

		
		for (int i=0; i<total; ++i) {
			ndxml *sub = ndxml_refsubi(xnode, i) ;
			fprintf(pf,"\t APOLLO_INSTALL_ENTRY(%s,h);\n",ndxml_getattr_val(sub, "id")) ;
			
		}
		fprintf(pf, "}//end install_message_handler function  \n\n" );

		
		fclose(pf) ;

	}while (0);
	
	return 0 ;
}
*/

#define LOAD_XML_FROM_FILE(_xmlRoot,_pathDir, _fileName,_encode) \
do { 			\
	std::string fileName = _pathDir + _fileName ;				\
	if(-1==ndxml_load_ex(fileName.c_str(), _xmlRoot,_encode) ) {			\
		fprintf(stderr, "load file %s error \n", _fileName) ;	\
		exit(1) ;		\
	}					\
}while(0)

extern int build_CSharp(ndxml_root *xmlID, ndxml_root *xmlMarco, ndxml_root *xmlData, const char *outfile);
extern int build_luaDataStruct(ndxml_root *xmlfile, const char *outFileName);
extern int build_luaMessageID(ndxml_root *xmlfile, const char *out_file);
int main(int argc, char *argv[])
{
	int i ;
	const char *encode = "gbk" ;
	fprintf(stderr, "current path =%s\n", nd_getcwd());
	std::string _input_dir ;
	//get config file
	for (i=1; i<argc; i++){
		if(0 == strcmp(argv[i],"-d" ) && i< argc-1) {
			_input_dir = argv[++i] ;
		}
		if (0==strcmp("-encode", argv[i])) {
			encode = argv[++i] ;
		}
	}
	
	if (_input_dir.length() <= 0 ) {
		
		fprintf(stderr, "usage:  -d input-config-direct\n") ;
		exit(-1) ;
	}
	
	//int ret ;
	ndxml_root xmlMarco , xmlDatatype , xmlMessage ;
	
	LOAD_XML_FROM_FILE(&xmlMarco, _input_dir ,"/marco.xml",encode) ;
	LOAD_XML_FROM_FILE(&xmlMessage, _input_dir ,"/message.xml",encode) ;
	LOAD_XML_FROM_FILE(&xmlDatatype, _input_dir ,"/datatype.xml",encode) ;
	
	
	nd_rmdir("./cpp") ;
	nd_mkdir("./cpp") ;
	
	//nd_rmdir("./cpp_ue") ;
	//nd_mkdir("./cpp_ue") ;
	
// 	if(-1==build_dataTypeForUE4(&xmlDatatype, "./cpp_ue/netStreamAutoDefine" ) ) {
// 		fprintf(stderr, "export datatype error \n")  ;
// 		exit(1) ;
// 	}
	
	
	if(-1==build_dataType(&xmlDatatype, "./cpp/auto_dataType",false ) ) {
		fprintf(stderr, "export datatype error \n")  ;
		exit(1) ;
	}
	
	if(-1==build_dataType(&xmlDatatype, "./cpp/auto_dataTypeDb",true ) ) {
		fprintf(stderr, "export datatype error \n")  ;
		exit(1) ;
	}
	
	if(build_messageID(&xmlMessage, "./cpp/_msgid_export.h")){
		fprintf(stderr, "export MessageID error \n")  ;
		exit(1) ;
	}

	
	if(build_marco(&xmlMarco, "./cpp/auto_macroDefine.h")) {
		fprintf(stderr, "export Macro error \n")  ;
		exit(1) ;
	}

	
//	if(build_messageHandler(&xmlMessage, "./cpp/UserMsgHandler.h","./cpp/UserMsgInstall.cpp")) {
//		fprintf(stderr, "export Macro error \n")  ;
//		exit(1) ;
//	}
	//export cs
	
	nd_rmdir("./csharp");
	nd_mkdir("./csharp");


	nd_rmdir("./luaOut");
	nd_mkdir("./luaOut");

	//load for utf8
	ndxml_destroy(&xmlMarco);
	ndxml_destroy(&xmlDatatype);
	ndxml_destroy(&xmlMessage);


	ndstr_set_code(E_SRC_CODE_UTF_8);
	LOAD_XML_FROM_FILE(&xmlMarco, _input_dir, "/marco.xml", "utf8");
	LOAD_XML_FROM_FILE(&xmlMessage, _input_dir, "/message.xml", "utf8");
	LOAD_XML_FROM_FILE(&xmlDatatype, _input_dir, "/datatype.xml", "utf8");


	if(build_CSharp(&xmlMessage,&xmlMarco , &xmlDatatype, "./csharp/ProtocolMessage.cs") ) {
		fprintf(stderr, "export CSharp error \n")  ;
		exit(1) ;
	}
	
	if ( build_luaDataStruct(&xmlDatatype,"./luaOut/NetDataStruct.lua"))	{
		fprintf(stderr, "export lua error \n");
		exit(1);
	}


	if (build_luaMessageID(&xmlMessage, "./luaOut/NetMessageIDDefine.lua")) {
		fprintf(stderr, "export lua mesage id error \n");
		exit(1);
	}
	
	ndxml_destroy(&xmlMarco); ndxml_destroy(&xmlDatatype); ndxml_destroy(&xmlMessage) ;
	
	exit(0) ;
}

