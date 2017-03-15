/* file logic_editor_helper.h
 *
 * editor common function
 *
 * create by duan 
 *
 * 2016-1-14
 */

#ifndef _LOGIC_EDITOR_HELPER_H_
#define _LOGIC_EDITOR_HELPER_H_


#include <string>
#include <vector>
#include <set>
#include "nd_common/nd_common.h"

namespace LogicEditorHelper
{
	typedef std::vector<std::string> text_vect_t;

	enum CreateTemplate
	{
		TEMPLATE_CREAT,		//ͨ��ģ�崴��
		TEMPLATE_DIRECT,	//ֱ�Ӵ���
		TEMPLATE_LIST		//ѡ���б�
	};

	enum eDataType{			//��������
		EDT_NONE = 0,		//������
		EDT_ENUM,			//ö������
		EDT_BOOL,			//bool����
		EDT_NUMERAL,		//��������
		EDT_STRING,		//�ַ�����
		EDT_PASSWORD,		//��������
		EDT_HIDE,			//���ؽڵ�
		EDT_IN_FILE,		//�����ļ�
		EDT_OUT_FILE,		//����ļ�
		EDT_DIR,			//Ŀ¼
		EDT_REF_DEFINED,
		EDT_USER_DEFINE,
		EDT_KEY_VAL,		//����key-valueֵ,��ʾkey,ʵ��ֵ��value
	};
	enum eReservedType {
		ERT_KIND = 0,
		ERT_VALUE,
		ERT_NAME,
		ERT_CREATETIME,
		ERT_DESC,
		ERT_EXPAND,
		ERT_RW,
		ERT_PARAM,
		ERT_DELETE,
		ERT_TEMPLATE,
		ERT_REF_TYPE,
		ERT_DATA_DEFINE,
		ERT_EXPAND_LIST,
		ERT_RESTRICT,
		ERT_COMP_COND,
		ERT_COMP_VALUE,
		ERT_USER_DEFINE_PARAM,
		ERT_AUTO_INDEX,
		ERT_EXPAND_STAT,
		ERT_ENABLE_DRAG,
		ERT_NEED_CONFIRM,
		ERT_KEY,
		ERT_KEY_VALUE,
		ERT_BE_REF_ONLY,
	};

	const char *_GetDataTypeName(eDataType dataType);
	const char *_GetReverdWord(eReservedType rType);

	class CXMLAlias {
	public:
		struct sAlias {
			std::string str_var;
			std::string str_alias;
		};
		CXMLAlias() : m_num(0), m_pBuf(0), m_created(0){}
		virtual ~CXMLAlias() {
			if (m_pBuf) delete[] m_pBuf;
		}

		sAlias &operator[] (int n){ return m_pBuf[n]; }
		int Create(ndxml *xmlAlias);
		const char *GetAlia(const char *valname);
		int m_num;
		int m_created;
		sAlias *m_pBuf;

	};

	//���XML ��attribute �Ƿ��Ǳ�����
	int CheckReserved(const char *param);
	bool CheckCanDelete(ndxml *xml);
	//�õ��������� eDataType
	int CheckDataType(const char *param);

	ndxml *getDefinedType(ndxml_root *root, const char *defTypeName);
	//�õ�xmlֵ������
	int GetXmlValType(ndxml *xml, ndxml_root *root);

	const char *_getXmlParamVal(ndxml *xml, ndxml_root *root, eReservedType paramName);

	inline const char *GetEnumValText(ndxml *xml, ndxml_root *root)
	{
		return _getXmlParamVal(xml, root, ERT_VALUE);
	}

	inline const char *GetKeyValueText(ndxml *xml, ndxml_root *root)
	{
		return _getXmlParamVal(xml, root, ERT_KEY);
	}
	inline const char *GetKeyValue(ndxml *xml, ndxml_root *root)
	{
		return _getXmlParamVal(xml, root, ERT_KEY_VALUE);
	}

	template<class TTextList>
	int _getEnumMembers(ndxml *xml, ndxml_root *config_root, TTextList& texts, eReservedType typeName)
	{
		const char *pValue = _getXmlParamVal(xml, config_root, typeName);
		if (!pValue)
			return 0;
		char subtext[128];

		do 	{
			subtext[0] = 0;
			pValue = ndstr_nstr_end(pValue, subtext, ',', sizeof(subtext));
			if (subtext[0])	{
				texts.push_back((subtext));
			}
			if (pValue && *pValue == ',') {
				++pValue;
			}
		} while (pValue && *pValue);
		return(int) texts.size();
	}


	template<class TTextList>
	int getEnumList(ndxml *xml, ndxml_root *config_root, TTextList& texts)
	{
		return _getEnumMembers(xml, config_root, texts, ERT_VALUE);
// 		const char *pValue = GetEnumValText(xml, config_root);
// 		if (!pValue)
// 			return 0;
// 		char subtext[128];
// 
// 		do 	{
// 			subtext[0] = 0;
// 			pValue = ndstr_nstr_end(pValue, subtext, ',', sizeof(subtext));
// 			if (subtext[0])	{
// 				texts.push_back((subtext));
// 			}
// 			if (pValue && *pValue == ',') {
// 				++pValue;
// 			}
// 		} while (pValue && *pValue);
// 		return texts.size();
	}


	template<class TTextList>
	int getKeyList(ndxml *xml, ndxml_root *config_root, TTextList& texts)
	{
		return _getEnumMembers(xml, config_root, texts, ERT_KEY);
	}

	template<class TTextList>
	int getKeyValueList(ndxml *xml, ndxml_root *config_root, TTextList& texts)
	{
		return _getEnumMembers(xml, config_root, texts, ERT_KEY_VALUE);
	}

	const char* GetXmlParam(ndxml *xml);
	const char *_GetXmlName(ndxml *xml, CXMLAlias *alias);
	const char *_GetNodeComment(ndxml *xml);

	const char *_GetXmlDesc(ndxml *xml);


	//���ڵ��Ƿ�����
	bool CheckHide(ndxml *xml);

	//����Ƿ�����չ��ʾ�����ӽڵ�
	bool CheckExpand(ndxml *xml);

	int get_string_array(const char *pValue, text_vect_t &texts);

	//����Ƿ�����չ��ʾ�����ӽڵ�
	bool CheckReadOnly(ndxml *xml);
	ndxml *GetCreateTemplate(ndxml *xml, ndxml_root *root);

	const char *getRealValFromStr(const char *pInput, char *outpub, size_t bufsize);
	const char *getDisplayNameFromStr(const char *pInput, char *outpub, size_t bufsize);
	const char *buildDisplaNameValStr(const char *val, const char*dispName, char *outpub, size_t bufsize);
}
#endif
