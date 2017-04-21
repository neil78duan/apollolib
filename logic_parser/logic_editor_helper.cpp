/* file logic_editor_helper.cpp
 *
 * editor common function
 *
 * create by duan 
 *
 * 2016-1-14
 */

#include "logic_parser/logic_editor_helper.h"

namespace LogicEditorHelper
{

	int CXMLAlias::Create(ndxml *xmlAlias)
	{
		ndxml *node;
		if (m_created)
			return 0;

		int num = ndxml_getsub_num(xmlAlias);
		if (num <= 0)
			return -1;

		m_pBuf = new sAlias[num];

		for (int i = 0; i < num; i++) {
			node = ndxml_refsubi(xmlAlias, i);
			if (node) {
				m_pBuf[m_num].str_var = ndxml_getname(node);
				m_pBuf[m_num].str_alias = ndxml_getval(node);
				m_num++;
			}
		}
		m_created = 1;
		return m_num;
	}

	const char* CXMLAlias::GetAlia(const char *valname)
	{
		for (int i = 0; i < m_num; i++)
		{
			if (0 == ndstricmp(valname, m_pBuf[i].str_var.c_str()))
				return m_pBuf[i].str_alias.c_str();
		}
		return NULL;
	}

	//////////////////////////////////////////////////////////////////


	const char *_szDataKindsDesc[] =		//数据类型对应的字符串
	{
		"none",		//无类型
		"enum",			//枚举类型
		"bool",		//bool类型
		"numeral",		//数字类型
		"string",		//字符类型
		"password",		//显示密码
		"hide",			//隐藏类型(此node节点不可见
		"in_file",		//输入文件类型
		"out_file",		//输出文件类型
		"dir",			//目录
		"reference",		//使用定义好的类型
		"user_define",		//用户自定输入
		"key_value"
	};
	
	const char *_szReserved[] = {		//保留字
		"kinds",				//制定XML node 的类型 eDataType
		"enum_value",			//枚举值,指定枚举值的内容
		"name",
		"createtime",
		"desc",
		"expand",				//张开所有子项 
		"rw_stat",				//读写状态(只读"read" 读写"write" default )
		"param",				//参数
		"delete",				//是否允许删除
		"create_template",
		"reference_type",
		"data_type_define",
		"expand_list",
		"restrict",			// 数据类型限制
		"comp_cond",			//比较条件
		"comp_value",
		"user_param",			//用户自定参数
		"auto_index",
		"expand_stat",
		"enable_drag",
		"need_confirm",
		"key",
		"value",
		"referenced_only",
		"replace_val",
		"create_label",
		"var_name",
		"connect_in_seq"
	};

	const char *_GetDataTypeName(eDataType dataType)
	{
		return _szDataKindsDesc[dataType];
	}
	const char *_GetReverdWord(eReservedType rType)
	{
		return _szReserved[rType];
	}

	//检查XML 的attribute 是否是保留字
	int CheckReserved(const char *param)
	{
		int num = sizeof(_szReserved) / sizeof(char*);
		for (int i = 0; i < num; i++) {
			if (0 == ndstricmp(param, _szReserved[i]))
				return i;
		}
		return -1;
	}
	bool CheckCanDelete(ndxml *xml)
	{
		char *kinds = (char*)ndxml_getattr_val(xml, _szReserved[ERT_DELETE]);
		if (kinds) {
			return (0 != ndstricmp(kinds, "no"));
		}
		return true;
	}
	//得到数据类型 eDataType
	int CheckDataType(const char *param)
	{
		int num = sizeof(_szDataKindsDesc) / sizeof(char*);
		for (int i = 0; i < num; i++) {
			if (0 == ndstricmp(param, _szDataKindsDesc[i]))
				return i;
		}
		return -1;
	}

	ndxml *getDefinedType(ndxml_root *root, const char *defTypeName)
	{
		ndxml *typeXml = ndxml_getnode(root, _szReserved[ERT_DATA_DEFINE]);
		if (typeXml){
			return ndxml_refsub(typeXml, defTypeName);
		}
		return NULL;
	}
	//得到xml值的类型
	int GetXmlValType(ndxml *xml, ndxml_root *root)
	{
		char *kinds = (char*)ndxml_getattr_val(xml, _szReserved[ERT_KIND]);
		if (kinds) {
			int ret = CheckDataType(kinds);
			if (EDT_REF_DEFINED == ret){
				char *ref_typename = (char*)ndxml_getattr_val(xml, _szReserved[ERT_REF_TYPE]);
				if (!ref_typename)	{
					return EDT_NONE;
				}
				ndxml *defined_type = getDefinedType(root, ref_typename);
				if (defined_type){
					return GetXmlValType(defined_type, root);
				}
			}
			return ret;
		}
		return EDT_NONE;
	}
	//////////////////////

	const char *_getXmlParamVal(ndxml *xml, ndxml_root *root, eReservedType paramName)
	{
		char *pEnumList = (char*)ndxml_getattr_val(xml, _szReserved[paramName]);
		if (pEnumList) {
			return pEnumList;
		}
		else if(root) {
			char *ref_typename = (char*)ndxml_getattr_val(xml, _szReserved[ERT_REF_TYPE]);
			if (ref_typename) {
				ndxml *defined_type = getDefinedType(root, ref_typename);
				if (defined_type)	{
					return  ndxml_getattr_val(defined_type, _szReserved[paramName]);
				}
			}
		}
		return NULL;
	}
	
	const char* GetXmlParam(ndxml *xml)
	{
		return (char*)ndxml_getattr_val(xml, _szReserved[ERT_PARAM]);
	}

	const char *_GetNodeComment(ndxml *xml)
	{
		ndxml *pSub = ndxml_getnode(xml, "comment");
		if (pSub) {
			return ndxml_getval(pSub);
		}
		else {
			const char *p = ndxml_getname(xml);
			const char* ret = strchr(p, '_');
			if (ret){
				return ++ret;
			}
			return p;
		}
	}

	const char *_GetXmlName(ndxml *xml, CXMLAlias *alias)
	{
		//需要获得别名
		const char *name = ndxml_getattr_val(xml, _szReserved[ERT_NAME]);
		if (!name) {
			name = ndxml_getname(xml);
			if (alias)	{
				const char *alia_name = alias->GetAlia(name);
				if (alia_name)	{
					return alia_name;
				}
			}
		}
		return name;
	}

	const char *_GetXmlDesc(ndxml *xml)
	{
		const char *name = ndxml_getattr_val(xml, _szReserved[ERT_DESC]);
		if (!name) {
			name =  "NULL";
		}
		return name;
	}
	//检测节点是否隐藏
	bool CheckHide(ndxml *xml)
	{
		char *kinds = (char*)ndxml_getattr_val(xml, _szReserved[ERT_KIND]);
		if (kinds) {
			if (EDT_HIDE == CheckDataType(kinds))
				return true;
		}
		return false;
	}

	//检测是否是扩展显示所有子节点
	bool CheckExpand(ndxml *xml)
	{
		char *kinds = (char*)ndxml_getattr_val(xml, _szReserved[ERT_EXPAND]);
		if (kinds) {
			if (0 == ndstricmp(kinds, "yes"))
				return true;

		}
		return false;

	}

	int get_string_array(const char *pValue, text_vect_t &texts)
	{
		if (!pValue)
			return 0;
		char subtext[128];
		do 	{
			subtext[0] = 0;
			pValue = (char*)ndstr_nstr_end(pValue, subtext, ',', sizeof(subtext));
			if (pValue && *pValue == ',') {
				++pValue;
			}
			if (subtext[0]){
				texts.push_back(std::string(subtext));
			}
		} while (pValue && *pValue);
		return (int) texts.size();
	}

	//检测是否是扩展显示所有子节点
	bool CheckReadOnly(ndxml *xml)
	{
		char *kinds = (char*)ndxml_getattr_val(xml, _szReserved[ERT_RW]);
		if (kinds) {
			if (0 == ndstricmp(kinds, "read"))
				return true;
		}
		//XML parent ;
		//if(0==xml.Parent(parent) )
		//	return CheckReadOnly(parent) ;
		return false;
	}
	ndxml *GetCreateTemplate(ndxml *xml, ndxml_root *root)
	{
		char*template_name = (char*)ndxml_getattr_val(xml, _szReserved[ERT_TEMPLATE]);
		if (!template_name) {
			return NULL;
		}
		ndxml *template_root = ndxml_getnode(root, _szReserved[ERT_TEMPLATE]);
		if (!template_root)
			return NULL;

		ndxml *create_template = ndxml_refsub(template_root, template_name);
		
		while (create_template) {
			const char *pRef = ndxml_getattr_val(create_template, "ref_from");
			if (pRef && (0 == ndstricmp(pRef, "yes") || 0 == ndstricmp(pRef, "true")))	{
				create_template = LogicEditorHelper::_getRefNode(create_template, ndxml_getval(create_template));
			}
			else {
				break;
			}
		}

		return create_template;
	}
	/////////////////////////////////////////////////////////////////////////////

	const char *getRealValFromStr(const char *pInput, char *outpub, size_t bufsize)
	{
		const char *p = strstr(pInput, "_realval=");
		if (p){
			p += 9;
			outpub[0] = 0;
			ndstr_nstr_end((char*)p, outpub, '&', (int)bufsize);
			if (outpub[0])	{
				return outpub;
			}
		}
		return pInput;
	}
	const char *getDisplayNameFromStr(const char *pInput, char *outpub, size_t bufsize)
	{
		const char *p = strstr(pInput, "_dispname=");
		if (p){
			p += 10;
			outpub[0] = 0;
			ndstr_nstr_end((char*)p, outpub, '&', (int)bufsize);
			if (outpub[0])	{
				return outpub;
			}
		}
		return pInput;
	}
	const char *buildDisplaNameValStr(const char *val, const char*dispName, char *outpub, size_t bufsize)
	{
		snprintf(outpub, bufsize, "_realval=%s&_dispname=%s", val, dispName);
		return outpub;
	}


	ndxml *_getRefNode(ndxml*node, const char *xmlPath)
	{
		return ndxml_recursive_ref(node, xmlPath);
		/*
		const char *p = xmlPath;
		char nodeName[128];

		ndxml *retXml = NULL;
		if (!p || !*p){
			return NULL;
		}
		while (p && *p && node)	{
			if (*p == '/') {
				++p;
			}

			nodeName[0] = 0;
			p = ndstr_nstr_ansi(p, nodeName, '/', 128);
			if (strcmp(nodeName, "..") == 0){
				retXml = ndxml_get_parent(node);
			}
			else if (nodeName[0]) {
				//skip node.attrName
				char *attrNameStart = strchr(nodeName, '.');
				if (attrNameStart)	{
					*attrNameStart = 0;
					if (!*nodeName)	{
						return node;
					}
					else {
						return ndxml_getnode(node, nodeName);
					}
				}
				else {
					retXml = ndxml_getnode(node, nodeName);
				}
			}
			else {
				break;
			}
			node = retXml;
		}

		return retXml;
		*/
	}


	bool _CheckIsChild(ndxml *childXml, const char *parentName)
	{
		ndxml *parent = ndxml_get_parent(childXml);

		while (parent)	{
			const char *name =ndxml_getname(parent);
			if (0==ndstricmp(name,parentName) )	{
				return true;
			}
			parent = ndxml_get_parent(parent);
		}
		return false;
	}

	const char *_getRefNodeAttrName(/*ndxml *node,*/ const char *xmlPath)
	{
		return nd_file_ext_name(xmlPath);
		/*
		const char *start = xmlPath;
		size_t size = strlen(start);
		const char *p = start + size;

		bool ret = false;

		while (p-- > start){
			if (*p == '.'){
				ret = true;
				break;
			}
			else if (*p == '/')	{
				ret = false;
				break;
			}
		}

		if (ret){
			return p + 1;
		}
		return NULL;
		*/
	}


	ndtime_t getScriptChangedTime(ndxml_root *xmlFile)
	{
		//char buf[64];
		ndxml *xmlModule = ndxml_getnode(xmlFile, "moduleInfo");
		if (xmlModule)	{
			ndxml *valxml = ndxml_getnode(xmlModule, "LastMod");
			if (valxml) {
				//snprintf(buf, sizeof(buf), "%lld", )
				const char *pVal = ndxml_getval(valxml);
				if (pVal){
					return (ndtime_t) atoll(pVal);
				}
			}
		}
		return time(NULL);
		
	}
	void setScriptChangedTime(ndxml_root *xmlFile, ndtime_t changedTime)
	{
		char buf[64];
		ndxml *xmlModule = ndxml_getnode(xmlFile, "moduleInfo");
		if (xmlModule)	{
			snprintf(buf, sizeof(buf), "%lld", (NDUINT64)changedTime);
			ndxml *valxml = ndxml_getnode(xmlModule, "LastMod");
			if (valxml) {
				ndxml_setval(valxml, buf);
			}
			else {
				ndxml_addnode(xmlModule,"LastMod",buf);
			}
		}
	}
}