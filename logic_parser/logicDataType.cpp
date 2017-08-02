/* file logicDataType.cpp
 *
 * create by duan 
 * 2015-5-8
 *
 */

#include "logic_parser/logicEndian.h"
#include "logic_parser/logicDataType.h"
#include "logic_parser/objectBaseMgr.h"
#include "logic_parser/logicStruct.hpp"
#include <math.h>


NDUINT8 DBLDataNode::s_bOutHex = 0;
NDUINT8 DBLDataNode::s_bOutLua = 0;



bool DBLDataNode::setOutHex(bool isHex)
{
	bool ret = s_bOutHex ? true : false;
	s_bOutHex = isHex ? 1 : 0;
	return ret;
}

bool DBLDataNode::setOutLua(bool isLua)
{
	bool ret = s_bOutLua ? true : false;
	s_bOutLua = isLua ? 1 : 0;
	return ret;
}
#define IS_OUT_HEX() (DBLDataNode::s_bOutHex==1)

#define IS_OUT_LUA() (DBLDataNode::s_bOutLua==1)

static bool _int_array_init(int *arr, int size, dbl_element_base * eledata)
{
	size_t mem_len = sizeof(dbl_intarray) + (size - 1) * sizeof(int);
	eledata->_data = malloc(mem_len);
	if (!eledata->_data){
		return false;
	}

	memset(eledata->_data, 0, mem_len);
	eledata->_i_arr->capacity = size;

	for (int i = 0; i < size; i++) {
		eledata->_i_arr->data[i] = arr[i];
	}
	eledata->_i_arr->number = size;
	return true;
}

static bool _int64_array_init(NDUINT64 *arr, int size, dbl_element_base * eledata)
{
	size_t mem_len = sizeof(dbl_int64array) + (size - 1) * sizeof(NDUINT64);
	eledata->_data = malloc(mem_len);
	if (!eledata->_data){
		return false;
	}

	memset(eledata->_data, 0, mem_len);
	eledata->_int64_arr->capacity = size;

	for (int i = 0; i < size; i++) {
		eledata->_int64_arr->data[i] = arr[i];
	}
	eledata->_int64_arr->number = size;
	return true;
}

static bool _float_array_init(float *arr, int size, dbl_element_base * eledata)
{
	size_t mem_len = sizeof(dbl_intarray) + (size - 1) * sizeof(float);
	eledata->_data = malloc(mem_len);
	if (!eledata->_data){
		return false;
	}

	memset(eledata->_data, 0, mem_len);
	eledata->_f_arr->capacity = size;

	for (int i = 0; i < size; i++) {
		eledata->_f_arr->data[i] = arr[i];
	}
	eledata->_f_arr->number = size;
	return true;
}

static bool _str_array_init(const char *arr[], int size, dbl_element_base * eledata)
{
	size_t mem_len = sizeof(dbl_strarray) + (size - 1) * sizeof(char*);
	eledata->_data = malloc(mem_len);
	
	if (!eledata->_data){
		return false;
	}

	memset(eledata->_data, 0, mem_len);
	eledata->_str_arr->capacity = size;

	for (int i = 0; i < size; i++) {
		size_t len = strlen(arr[i]);
		if (len > 0)	{
			eledata->_str_arr->data[i] = (char*)malloc(len + 1);
			if (eledata->_str_arr->data[i]) {
				strncpy(eledata->_str_arr->data[eledata->_str_arr->number], arr[i], len + 1);
				eledata->_str_arr->number++;
			}
		}
	}
	return true;
}


static bool _userDef_array_init(const LogicUserDefStruct *arr[], int size, dbl_element_base * eledata)
{
	size_t mem_len = sizeof(dbl_userDefArray) + (size - 1) * sizeof(char*);
	dbl_userDefArray *paddr = (dbl_userDefArray*)malloc(mem_len);
	if (!paddr){
		return false;
	}
	eledata->_data = paddr;

	memset(eledata->_data, 0, mem_len);
	eledata->_arr_user->capacity = size;

	//memset(paddr, 0, mem_len);

	for (int i = 0; i < size; i++) {
		if (arr[i]) {
			paddr->data[paddr->number] = new LogicUserDefStruct(*arr[i]);
			paddr->number++;
		}
	}
	return true;
}

DBL_ELEMENT_TYPE DBLDataNode::getTypeFromName(const char *typeName)
{
	static const char *_s_typeName[] = {
		"int",
		"float",
		"string",
		"int8",
		"int16",
		"int64"
	};
	for (size_t i = 0; i < sizeof(_s_typeName) / sizeof(_s_typeName[0]); i++)	{
		if (ndstricmp(typeName, _s_typeName[i])==0)	{
			return (DBL_ELEMENT_TYPE)i;
		}
	}
	return OT_USER_DEFINED;
}


DBL_ELEMENT_TYPE DBLDataNode::getTypeFromValue(const char *valText)
{
	const char *p = ndstr_first_valid(valText);
	if (!p)	{
		return OT_INT;
	}
	if (ndstr_is_numerals(p) ){
		if (ndstr_is_naturalnumber(p)){
			return OT_INT;
		}
		return OT_FLOAT;
	}

	int subType = OT_STRING;
	return (DBL_ELEMENT_TYPE)DBLDataNode::getCellType(p, subType);
}


int DBLDataNode::getCellType(const char *celltext, int &subType)
{
	bool isFloat = false;
	//bool isArray2d = false;
	subType = 0;
	const char *p = ndstr_first_valid(celltext);
	if (!p || *p == 0) {
		return OT_OBJECT_VOID;
	}

	if (*p == _ARRAR_BEGIN_MARK) {
		int left = 1, right = 0;
		subType = OT_INT;
		++p;
		while (*p){
			char a = *p++;
			if (a == _ARRAR_BEGIN_MARK)	{
				left++;
			}
			else if (a == _ARRAR_END_MARK)	{
				right++;
			}
			else if (IS_NUMERALS(a) || a == ' ' || a == ',' || a == '.' || a == '-') {
				if (a == '.') {
					subType = NDMAX(OT_FLOAT, subType);
				}
			}
			else {
				subType = OT_STRING;
			}
		}
		if (left > 1) {
			subType = OT_STRING;
		}
		if (left != right) {
			nd_logwarn("parse data error ( and ) not match: %s\n", celltext);
		}
		return OT_ARRAY;

	}
	else if (*p == '0' && p[1] && p[1] != '.') {
		return OT_STRING;
	}
	else if (IS_NUMERALS(*p) || *p == '-') {
		//check is number
		int dotNum = 0;

		while (*(++p)) {
			if (IS_NUMERALS(*p) || *p == '.') {
				if (*p == '.') {
					dotNum++;
					isFloat = true;
					if (dotNum > 1)
						return -1;
				}
				continue;
			}
			else {
				return OT_STRING;
			}
		}
		return isFloat ? OT_FLOAT : OT_INT;
	}
	else {
		return OT_STRING;
	}

}
static bool _check_array(const char *src);
//////////////////////////////////////////////////////////////////////////
DBLDataNode::DBLDataNode(dbl_element_base *data, DBL_ELEMENT_TYPE etype, DBL_ELEMENT_TYPE sub_etype) :
m_ele_type(etype), m_sub_type(sub_etype), m_data(data), m_dataOwner(false)
{
	m_dataOwn.isInit = true;
	//m_refParent = false;
	//m_unsafeRef = false;

	//m_unsafeRefType = UNSAFE_REF_ADDR_REF;
}
DBLDataNode::DBLDataNode(const DBLDataNode &r) 
{
	init() ;
	_copy(r);
}

DBLDataNode::DBLDataNode()
{
	init() ;
	m_data = NULL;
	m_dataOwner = false;
	m_dataOwn.isInit = false;
}

DBLDataNode::~DBLDataNode()
{
	Destroy();
}

void DBLDataNode::init()
{
	//m_refParent = false;
	//m_unsafeRef = false;
	//m_unsafeRefType = UNSAFE_REF_ADDR_REF;
	m_ele_type = 0;
	m_sub_type = 0;
	m_dataOwn._data = 0 ;
	m_data = &m_dataOwn;
	m_dataOwn.isInit = true;
	m_dataOwner = true;
}

DBLDataNode &DBLDataNode::operator =(const DBLDataNode &r)
{
	if (this == &r){
		return *this;
	}

	Destroy();
	_copy(r);

	return *this;
}

bool DBLDataNode::operator < (const DBLDataNode &r) const
{
	if (m_data == r.m_data)	{
		return false;
	}
	DBL_ELEMENT_TYPE mytype = (DBL_ELEMENT_TYPE) m_ele_type;
	
	switch (mytype)
	{
	case OT_INT:
	case OT_INT8:
	case OT_INT16:
	case OT_INT64:
	case OT_TIME:
		return GetInt64() < r.GetInt64();
	case OT_FLOAT:
		return GetFloat() < r.GetFloat();

	case OT_STRING:
		if (m_ele_type != r.m_ele_type)	{
			return false;
		}
		return ndstricmp(m_data->str_val, r.m_data->str_val) < 0 ;
	
	case OT_ARRAY:
		if (m_ele_type != r.m_ele_type || m_sub_type != r.m_sub_type){
			return false;
		}

		for (int i = 0; i < GetArraySize(); i++)	{
			if (i >= r.GetArraySize() )	{
				return false;
			}
			if (m_sub_type == OT_INT )	{
				int ret = GetarrayInt(i) - r.GetarrayInt(i);
				if (0==ret)	{
					continue; 
				}
				else {
					return ret < 0;
				}
			}

			if (m_sub_type == OT_INT64)	{
				NDINT64 ret = (NDINT64)( GetarrayInt64(i) - r.GetarrayInt64(i));
				if (0 == ret)	{
					continue;
				}
				else {
					return ret < 0;
				}
			}

			else if (m_sub_type == OT_FLOAT) {
				float ret = GetarrayFloat(i) - r.GetarrayFloat(i);
				if (0 == ret)	{
					continue;
				}
				else {
					return ret < 0;
				}
			}
			else if (m_sub_type == OT_STRING) {
				int ret = ndstricmp(GetarrayText(i), r.GetarrayText(i));
				if (0 == ret)	{
					continue;
				}
				else {
					return ret < 0;
				}
			}
		}
		
		break;
	
	default:
		break;
	}
	return false;
}

bool DBLDataNode::operator != (const DBLDataNode &r) const
{
	return !(*this == r);
}
bool DBLDataNode::operator == (const DBLDataNode &r) const
{
	if (m_data == r.m_data)	{
		return true;
	}
	else if (m_ele_type != r.m_ele_type) {
		return false;
	}
	switch (m_ele_type)
	{
	case OT_INT8:
	case OT_INT16:
	case OT_INT :
	case  OT_BOOL:
		return m_data->i_val == r.m_data->i_val;
	case OT_FLOAT :
		return fabsf(m_data->f_val - r.m_data->f_val) <= 0.001f;
	case OT_STRING:
		if (m_data->str_val==0 && r.m_data->str_val==0)	{
			return true;
		}
		else if (m_data->str_val && r.m_data->str_val) {
			return 0 == ndstricmp(m_data->str_val, r.m_data->str_val);
		}
		return false;
	//case OT_INT64:
	//	return m_data->i64_val == r.m_data->i64_val;
	case  OT_ARRAY:
		if (GetArraySize() != ((DBLDataNode&)r).GetArraySize())	{
			return false;
		}
		for (int i = 0; i < GetArraySize(); i++)	{
			if (m_sub_type == OT_INT)	{
				if (GetarrayInt(i) != ((DBLDataNode&)r).GetarrayInt(i))	{
					return false;
				}
			}
			else if (m_sub_type == OT_INT64)	{
				if (GetarrayInt64(i) != ((DBLDataNode&)r).GetarrayInt64(i))	{
					return false;
				}
			}
			else if (m_sub_type == OT_FLOAT) {
				if (GetarrayFloat(i) != ((DBLDataNode&)r).GetarrayFloat(i))	{
					return false;
				}
			}
			else if (m_sub_type == OT_STRING) {
				const char *p1 = GetarrayText(i);
				const char *p2 = ((DBLDataNode&)r).GetarrayText(i);
				if (p1 == 0 && p2 == 0)	{
					continue;;
				}
				else if (m_data->str_val && r.m_data->str_val) {
					if (ndstricmp((char*)p1, (char*)p2)) {
						return false;
					}
				}
			}
			
		}

		return true;

	default:
		return m_data->_data == m_data->_data;
		break;
	}

	return false;
}

void DBLDataNode::_copy(const DBLDataNode &r)
{
	//Destroy();

	m_ele_type = (r.m_ele_type);
	m_sub_type = (r.m_sub_type);

	if (!r.CheckValid()) {
		m_dataOwn.isInit = r.m_dataOwn.isInit;
		return;
	}

	if (r.m_dataOwner) {
		if (r.GetDataType() == OT_FILE_STREAM)	{
			InitSet(r.GetObject(), OT_FILE_STREAM);
		}
		else {
			m_dataOwner = true;
			m_data = &m_dataOwn;
			dbl_data_copy(r.m_data, m_data, (DBL_ELEMENT_TYPE)m_ele_type, (DBL_ELEMENT_TYPE)m_sub_type);
		}
		
	}
	else {
		m_data = (r.m_data);
		m_dataOwner = false;

	}

}
void DBLDataNode::InitSet(int a, int type)
{
	Destroy();
	m_ele_type = type;

	m_dataOwner = true;
	m_data = &m_dataOwn;
	m_data->i_val = a;
}

void DBLDataNode::InitSet(NDUINT64 a)
{
	Destroy();
	m_ele_type = OT_INT64;

	m_dataOwner = true;
	m_data = &m_dataOwn;
	m_data->i64_val = a;
}


void DBLDataNode::InitSet(time_t t)
{
	Destroy();
	m_ele_type = OT_TIME;

	m_dataOwner = true;
	m_data = &m_dataOwn;
	m_data->i64_val = t;
}

void DBLDataNode::InitSet(NDUINT16 a)
{
	Destroy();
	m_ele_type = OT_INT16;

	m_dataOwner = true;
	m_data = &m_dataOwn;
	m_data->i_val = a;
}
void DBLDataNode::InitSet(NDUINT8 a)
{
	Destroy();
	m_ele_type = OT_INT8;

	m_dataOwner = true;
	m_data = &m_dataOwn;
	m_data->i_val = a;
}

void DBLDataNode::InitSet(bool a)
{
	Destroy();
	m_ele_type = OT_BOOL;
	m_dataOwner = true;
	m_data = &m_dataOwn;
	m_data->i_val = a ? 1 : 0;
}

void DBLDataNode::InitSet(float a)
{
	Destroy();
	m_ele_type = OT_FLOAT;
	m_dataOwner = true;
	m_data = &m_dataOwn;
	m_data->f_val = a;
}

void DBLDataNode::InitSet(char *str1)
{
	InitSet((const char *)str1);
}
void DBLDataNode::InitSet(const char *str1)
{
	Destroy();
	m_ele_type = OT_STRING;
	m_dataOwner = true;
	if (str1 && *str1)	{
		m_data = &m_dataOwn;
		size_t s = strlen(str1) + 1;
		m_data->str_val = (char*)malloc(s);
		strncpy(m_data->str_val, str1, s);

	}
	
}


void DBLDataNode::InitSet(int *arr, int size)
{
	Destroy();	
	if (size > 0) {
		m_ele_type = OT_ARRAY;
		m_sub_type = OT_INT;
		m_dataOwner = true;
		m_data = &m_dataOwn;

		_int_array_init(arr, size, &m_dataOwn);
	}
}

void DBLDataNode::InitSet(float *arr, int size)
{
	Destroy();
	if (size > 0) {
		m_ele_type = OT_ARRAY;
		m_sub_type = OT_FLOAT;
		m_dataOwner = true;
		m_data = &m_dataOwn;
		_float_array_init(arr, size, &m_dataOwn);
	}

}

void DBLDataNode::InitSet(const char *arr[], int size)
{
	Destroy();
	if (size > 0) {
		m_ele_type = OT_ARRAY;
		m_sub_type = OT_STRING;
		m_dataOwner = true;
		m_data = &m_dataOwn;

		_str_array_init(arr, size, &m_dataOwn);
	}
}

void DBLDataNode::InitSet(NDUINT64 *arr, int size)
{
	Destroy();
	if (size > 0) {
		m_ele_type = OT_ARRAY;
		m_sub_type = OT_INT64;
		m_dataOwner = true;
		m_data = &m_dataOwn;
		_int64_array_init(arr, size, &m_dataOwn);
	}
}

void DBLDataNode::InitSet(const LogicUserDefStruct *arr[], int size)
{
	Destroy();
	if (size > 0) {
		m_ele_type = OT_ARRAY;
		m_sub_type = OT_STRING;
		m_dataOwner = true;
		m_data = &m_dataOwn;
		_userDef_array_init(arr, size, &m_dataOwn);
	}
}

void DBLDataNode::InitSet(void *binary, size_t size, DBL_ELEMENT_TYPE eleType)
{

	Destroy();
	m_ele_type = eleType;

	m_dataOwner = true;
	m_data = &m_dataOwn;
	m_data->_data = NULL;

	if (size > 0) {
		dbl_binary *parr = (dbl_binary *)malloc(sizeof(dbl_binary) + size);
		if (parr){
			memcpy(parr->data, binary, size);
			parr->size = size;
			m_data->_bin = parr;
		}
	}

	
}

void DBLDataNode::InitSet(void *object, DBL_ELEMENT_TYPE type)
{
	Destroy();
	m_ele_type = type;

	m_dataOwner = true;
	m_data = &m_dataOwn;
	m_data->_data = object;
}

void DBLDataNode::InitSet(const LogicUserDefStruct &u)
{
	Destroy();
	m_ele_type = OT_USER_DEFINED;
	
	m_dataOwner = true;
	m_data = &m_dataOwn;
	m_data->_data = (void*) new LogicUserDefStruct(u);
}

// 
// void DBLDataNode::InitUserArray(size_t size)
// {
// 	Destroy();
// 	m_ele_type = OT_ARRAY;
// 	m_sub_type = OT_USER_DEFINED;
// 
// 
// 	m_dataOwner = true;
// 	m_data = &m_dataOwn;
// 	m_data->_data = NULL;
// 	dbl_userDefArray *parr = (dbl_userDefArray *)malloc(sizeof(dbl_userDefArray) + size -1);
// 
// 	if (parr){
// 		parr->number = size;
// 		m_data->_data = (void*) parr;
// 		for (size_t i = 0; i < size; i++){
// 			parr->data[i] = 0;
// 		}
// 	}
// }

bool DBLDataNode::InitReservedArray(size_t size, int attay_type)
{
	Destroy();
	m_ele_type = OT_ARRAY;
	m_sub_type = attay_type;
	
	if (m_sub_type == OT_INT || m_sub_type == OT_BOOL || m_sub_type == OT_INT8 || m_sub_type == OT_INT16 )	{
		size_t mem_len = sizeof(dbl_intarray) + (size - 1) * sizeof(int);
		dbl_intarray *paddr = (dbl_intarray*) malloc(mem_len);
		if (!paddr){
			return false;
		}

		for (int i = 0; i < size; i++) {
			paddr->data[i] = 0;
		}
		paddr->number = 0;
		paddr->capacity = size;
		m_dataOwn._data = paddr;

	}

	else if (OT_INT64 == m_sub_type){
		size_t mem_len = sizeof(dbl_int64array) + (size - 1) * sizeof(NDUINT64);
		dbl_int64array *paddr = (dbl_int64array*)malloc(mem_len);
		if (!paddr){
			return false;
		}

		for (int i = 0; i < size; i++) {
			paddr->data[i] = 0;
		}
		paddr->number = 0;
		paddr->capacity = size;
		m_dataOwn._data = paddr;
	}

	else if (OT_FLOAT == m_sub_type){
		size_t mem_len = sizeof(dbl_floatarray) + (size - 1) * sizeof(float);
		dbl_floatarray *paddr = (dbl_floatarray*)malloc(mem_len);
		if (!paddr){
			return false;
		}

		for (int i = 0; i < size; i++) {
			paddr->data[i] = 0;
		}
		paddr->number = 0;
		paddr->capacity = size;
		m_dataOwn._data = paddr;
	}
	else if (OT_STRING == m_sub_type || OT_USER_DEFINED == m_sub_type) {
		size_t mem_len = sizeof(dbl_strarray) + (size - 1) * sizeof(char*);
		dbl_strarray *paddr = (dbl_strarray*)malloc(mem_len);
		if (!paddr){
			return false;
		}
		memset(paddr, 0, mem_len);

		paddr->number = 0;
		paddr->capacity = size;
		m_dataOwn._data = paddr;
	}
	m_dataOwner = true;
	m_data = &m_dataOwn;
	return true;
}

bool DBLDataNode::pushArray(const DBLDataNode &data) 
{
	dbl_intarray *paddr = (dbl_intarray *)m_data->_data;
	if (!paddr || paddr->number>= paddr->capacity ){
		return false;
	}
	return SetArray(data, (int)paddr->number);
}
bool DBLDataNode::SetArray(const DBLDataNode &data, int index)
{
	dbl_intarray *paddr = (dbl_intarray *)m_data->_data;
	if (!paddr || paddr->capacity <= index){
		return false;
	}

	if (m_sub_type == OT_INT || m_sub_type == OT_BOOL || m_sub_type == OT_INT8 || m_sub_type == OT_INT16)	{		
		paddr->data[index] = data.GetInt();

	}
	else if (OT_INT64 == m_sub_type){
		dbl_int64array *paddr = (dbl_int64array *)m_data->_data;
		paddr->data[index] = data.GetInt64();
	}

	else if (OT_FLOAT == m_sub_type){
		dbl_floatarray *paddr = (dbl_floatarray *)m_data->_data;
		paddr->data[index] = data.GetFloat();

	}
	else if (OT_STRING == m_sub_type){
		dbl_strarray *paddr = (dbl_strarray *)m_data->_data;
		if (paddr->data[index])	{
			free(paddr->data[index]);
			paddr->data[index] = NULL;
		}
		std::string str2 = data.GetString();
		size_t size = str2.size() + 1;
		if (str2.size()){			
			paddr->data[index] = (char*)malloc(size);
			strncpy(paddr->data[index], str2.c_str(), size);
		}
		

	}
	else if (OT_USER_DEFINED == m_sub_type) {
		dbl_userDefArray *pU = (dbl_userDefArray *)m_data->_data;
		const LogicUserDefStruct *puserData = data.getUserDef();
		if (puserData)		{
			if (pU->data[index]) {
				delete pU->data[index];
			}
			pU->data[index] = new LogicUserDefStruct(*puserData);
		}
	}
	else {
		return false;
	}
	
	if (index >= paddr->number) {
		paddr->number = index + 1;
	}
	return true;
}

void DBLDataNode::InitFromTxt(const char *valText)
{
	DBL_ELEMENT_TYPE oldType = (DBL_ELEMENT_TYPE) m_ele_type;
	DBL_ELEMENT_TYPE oldSub = (DBL_ELEMENT_TYPE) m_sub_type;

	Destroy();
	dbl_build_from_text(&m_dataOwn, valText, oldType, oldSub);

	m_ele_type = oldType;
	m_sub_type = oldSub;

	m_dataOwner = true;
	m_data = &m_dataOwn;
}


bool DBLDataNode::GetVal(NDUINT64 &a) const
{
	a = this->GetInt64();
	return true;
}
bool DBLDataNode::GetVal(NDUINT16 &a)const
{
	a = GetInt();
	return true;
}
bool DBLDataNode::GetVal(NDUINT8 &a)const
{
	a = GetInt();
	return true;
}
bool DBLDataNode::GetVal(time_t &a)const
{
	a = this->GetInt64();
	return true;
}
bool DBLDataNode::GetVal(float &a)const
{
	a = this->GetFloat();
	return true;
}
bool DBLDataNode::GetVal(int &a)const
{
	a = GetInt();
	return true;
}

bool DBLDataNode::GetVal(bool &a)const
{
	a = GetBool();
	return true;
}
bool DBLDataNode::GetVal(char *buf, size_t size)const
{
	const char *p = GetText();

	if ( p && *p){
		strncpy(buf, p, size);
		return true;
	}
	return false;
	
}
bool DBLDataNode::GetVal(int *arr, size_t &size)const
{
	if (OT_ARRAY == m_ele_type)	{
		size_t len = NDMIN(this->GetArraySize(), size);
		size = 0;
		for (size_t i = 0; i < len; i++){
			arr[i] = this->GetarrayInt((int)i);
			++size;
		}
		return true;
	}
	return false;
	
}

bool DBLDataNode::GetVal(float *arr, size_t &size)const
{
	if (OT_ARRAY == m_ele_type)	{
		size_t len = NDMIN(this->GetArraySize(), size);
		size = 0;
		for (size_t i = 0; i < len; i++){
			arr[i] = this->GetarrayFloat((int)i);
			++size;
		}
		return true;
	}
	return false;

}

DBLDataNode DBLDataNode::getUserDefMember(const char *name)const
{
	if (m_ele_type != OT_USER_DEFINED || !m_data->_data) {
		return DBLDataNode() ;
	}
	LogicUserDefStruct *pDataUser = m_data->_userDef;
	if (pDataUser){
		return pDataUser->get(name);
	}
	return DBLDataNode();
}
void DBLDataNode::setUserDefMember(const char *name,const DBLDataNode &val)
{
	if (m_ele_type != OT_USER_DEFINED || !m_data->_data) {
		return  ;
	}
	LogicUserDefStruct *pDataUser = m_data->_userDef;
	if (pDataUser){
		pDataUser->set(name, val);
	}

}
const LogicUserDefStruct *DBLDataNode::getUserDef() const
{
	if (m_ele_type != OT_USER_DEFINED || !m_data->_data) {
		return NULL;
	}
	return m_data->_userDef ;	
}
// 
// // unsafe reference 
// DBLDataNode DBLDataNode::unsafeRefArray(int index)
// {
// 	if (m_ele_type != OT_ARRAY || index >= m_data->_i_arr->number)	{
// 		return DBLDataNode();
// 	}
// 	DBLDataNode ret;
// 	ret.init();
// 
// 	ret.m_unsafeRef = true;
// 	ret.m_dataOwner = false;
// 	ret.m_ele_type = m_sub_type;
// 
// 	switch (m_sub_type)
// 	{
// 	case OT_STRING:
// 		ret.m_data->_data = &(m_data->_str_arr->data[index]);
// 		break;
// 	case OT_FLOAT:
// 		ret.m_data->_data = &m_data->_f_arr->data[index];
// 		break;
// 
// 	case OT_INT:
// 	case OT_BOOL:
// 	case OT_INT8:
// 	case OT_INT16:
// 		ret.m_data->_data = &m_data->_i_arr->data[index];
// 		break;
// 
// 	case OT_USER_DEFINED:
// 		ret.m_data->_data = &( m_data->_arr_user->data[index]);
// 		break;
// 	default:
//		return DBLDataNode(); 
//		break;
// 	}
// 	return ret;
// }
// DBLDataNode DBLDataNode::unsafeRefMember(const char *name)
// {
// 	if (m_ele_type != OT_USER_DEFINED || !m_data->_data) {
// 		return DBLDataNode();
// 	}
// 	LogicUserDefStruct *pDataUser = m_data->_userDef;
// 	if (pDataUser){
// 		DBLDataNode *pData = pDataUser->ref(name);
// 		if (!pData) {
// 			return  DBLDataNode();
// 		}
// 		DBLDataNode ret;
// 		ret.unsafeRefOther(*pData);		//unneed use unsafe 
// 		return ret;
// 	}
// 	return DBLDataNode();
// 
// }
// bool DBLDataNode::unsafeRefOther(DBLDataNode &other)
// {
// 	if (!other.CheckValid())	{
// 		return false;
// 	}
// 	m_ele_type = other.m_ele_type;
// 	m_sub_type = other.m_sub_type;
// 	m_data = other.m_data;
// 	m_dataOwner = false;
// 
// 	m_dataOwn.isInit = true;
// 	m_unsafeRef = true;
// 	if (!other.m_unsafeRef)	{
// 		m_unsafeRefType = UNSAFE_REF_ORGINAL;
// 	}
// 	else {
// 		m_unsafeRefType = other.m_unsafeRefType;
// 	}
// 
// 	return true;
// }
// bool DBLDataNode::unsafeSetValue(DBLDataNode &val)
// {
// 	if (!m_unsafeRef){
// 		*this = val;
// 		return true;
// 	}
// 
// 	if (m_unsafeRefType == UNSAFE_REF_ORGINAL)	{
// 		dbl_destroy_data(m_data, (DBL_ELEMENT_TYPE)m_ele_type, (DBL_ELEMENT_TYPE)m_sub_type);
// 		dbl_data_copy(val.m_data, m_data, (DBL_ELEMENT_TYPE)m_ele_type, (DBL_ELEMENT_TYPE)m_sub_type);
// 		return;
// 	}
// 
// 	if (!m_data->_data)	{
// 		return false;
// 	}
// 	
// 	switch (m_sub_type)
// 	{
// 	case OT_STRING:
// 	{
// 		const char *p = val.GetText();
// 		if (p && *p)	{
// 			size_t s = strlen(p) + 1;
// 			char *addr = (char*)malloc(s);
// 			char **refAddr = (char **) m_data->_data;			
// 			strncpy(addr, p, s);
// 			*refAddr = addr;
// 			break;
// 		}
// 		return false;
// 		break;
// 	}
// 	case OT_FLOAT:
// 	{
// 		float *addr = (float*) m_data->_data;
// 		*addr = val.GetFloat();
// 	}
// 		break;
// 
// 	case OT_INT:
// 	case OT_BOOL:
// 	case OT_INT8:
// 	case OT_INT16:
// 	{
// 		int *addr = (int*)m_data->_data;
// 		*addr = val.GetInt();
// 
// 	}
// 		break;
// 
// 	case OT_USER_DEFINED:
// 	{
// 		LogicUserDefStruct **addr = (LogicUserDefStruct **)(m_data->_data);
// 		if (*addr &&val.m_data->_userDef){
// 			**addr = *val.m_data->_userDef;
// 		}
// 		break;
// 	}
// 	default:
//		return false;
// 	}
// 	return true;
// }
// 
// bool DBLDataNode::getValFromUnsafe(DBLDataNode &unsafeVal)
// {
// 	if (!unsafeVal.m_data->_data)	{
// 		return false;
// 	}
// 
// 	switch (m_sub_type)
// 	{
// 	case OT_STRING:
// 		InitSet((char **)m_data->_data);
// 		break;
// 	case OT_FLOAT:
// 		InitSet(*(float*)m_data->_data);
// 		break;
// 
// 	case OT_INT:
// 	case OT_BOOL:
// 	case OT_INT8:
// 	case OT_INT16:
// 		InitSet(*(int*)m_data->_data);
// 		break;
// 	case OT_USER_DEFINED:
// 	{
// 		LogicUserDefStruct **addr = (LogicUserDefStruct **)(m_data->_data);
// 		if (addr && *addr ){
// 			InitSet(**addr);
// 		}
// 		break;
// 	}
// 	default:
//		return false;
// 	}
// 	return true;
// }

//////////////////////////////////////////////////////////////////////////

int DBLDataNode::ReadStream(const char *streamBuf, size_t data_size, int streamByteOrder)
{
	Destroy();
	int read_len = 2;
	m_dataOwn.isInit = false;
	NDUINT16 type;
	data_size -= 2;
	streamBuf = lp_read_stream((lp_stream_t)streamBuf, type , streamByteOrder);
	m_ele_type = (NDUINT8)type;

	if (type == OT_ARRAY ) {
		NDUINT16 subtype;
		streamBuf = lp_read_stream((lp_stream_t)streamBuf, subtype, streamByteOrder);

		m_sub_type = (NDUINT8) subtype;
		read_len += 2;
		data_size -= 2;

		NDUINT16 sub_num = 0;
		lp_read_stream((lp_stream_t)streamBuf, sub_num, streamByteOrder);
		if (0 == sub_num) {
			return read_len + 2;
		}
		if (m_sub_type == OT_USER_DEFINED) {
			streamBuf += 2;
			data_size -= 2;
			read_len += 2;
			InitReservedArray(sub_num, OT_USER_DEFINED);
			for (NDUINT16 i = 0; i < sub_num; i++)	{
				LogicUserDefStruct mystruct;
				int ret = mystruct.FromStream((void*)streamBuf, data_size, streamByteOrder);
				if (ret == -1) {
					return -1;
				}
				read_len += ret;
				data_size -= ret;
				streamBuf += ret;
				SetArray(DBLDataNode(mystruct), i);
			}
			return read_len;
		}
	}
	if (type == OT_USER_DEFINED){
		LogicUserDefStruct mystruct;
		int ret = mystruct.FromStream((void*)streamBuf, data_size, streamByteOrder);
		if (ret == -1) {
			return -1;
		}
		read_len += ret;
		data_size -= ret;
		streamBuf += ret;
		InitSet(mystruct);
	}
	else {

		int ret = dbl_read_buffer(&m_dataOwn, (DBL_ELEMENT_TYPE)m_ele_type, (DBL_ELEMENT_TYPE)m_sub_type, (char *)streamBuf, nd_byte_order() != streamByteOrder);
		if (ret > 0){
			m_data = &m_dataOwn;
			m_dataOwner = true;
			m_dataOwn.isInit = true;
			read_len += ret;
		}
	}
	return read_len;
}
int DBLDataNode::_writeEmptyStream(char *streamBuf,  int streamByteOrder)const
{
	lp_stream_t p = (lp_stream_t)streamBuf;
	switch (m_ele_type){
	case OT_BOOL:
	case OT_INT:
		p = lp_write_stream(p, (NDUINT32)0, streamByteOrder);
		break;
	case OT_FLOAT:
		p = lp_write_stream(p, (float)0, streamByteOrder);
		break;
	case OT_INT8:
		p = lp_write_stream(p, (NDUINT8)0, streamByteOrder);
	break;
	case OT_INT16:
		p = lp_write_stream(p, (NDUINT16)0, streamByteOrder);
	break;
	case OT_INT64:
	case OT_TIME:
		p = lp_write_stream(p, (NDUINT64)0, streamByteOrder);
		break;
	default:
		p = lp_write_stream(p, (NDUINT16)0, streamByteOrder);
		break;
	}
	
	return (int)(p - streamBuf);
}
int DBLDataNode::WriteStream(char *streamBuf, size_t buf_size, int streamByteOrder)const
{
	int write_len = 2;
	streamBuf = lp_write_stream(streamBuf, (NDUINT16)m_ele_type, streamByteOrder);
	buf_size -= 2;
	
	if (m_ele_type == OT_ARRAY ) {
		streamBuf = lp_write_stream(streamBuf, (NDUINT16)m_sub_type, streamByteOrder);
		write_len += 2;
		buf_size -= 2;

		if (m_sub_type == OT_USER_DEFINED) {
			NDUINT16	 num = (NDUINT16)m_data->_i_arr->number;
			streamBuf = lp_write_stream(streamBuf,num, streamByteOrder);
			write_len += 2;
			buf_size -= 2;
			for (NDUINT16 i = 0; i < num; i++)	{
				const LogicUserDefStruct *logicdata = GetarrayUser(i);
				if (logicdata) {
					int size = logicdata->ToStream(streamBuf, buf_size, streamByteOrder);
					if (size == -1) {
						return -1;
					}
					write_len += size;
					streamBuf += size;
					buf_size -= size;
				}
			}
			return write_len;
		}
	}
	
	int len = 0;
	if (m_data) {
		if (m_ele_type == OT_USER_DEFINED) {
			const LogicUserDefStruct * logicdata = getUserDef();
			if (logicdata) {
				int size = logicdata->ToStream(streamBuf, buf_size, streamByteOrder);
				write_len += size;
			}
			return write_len;
		}
		else {
			len = dbl_write_buffer(m_data, (DBL_ELEMENT_TYPE)m_ele_type, (DBL_ELEMENT_TYPE)m_sub_type, (char *)streamBuf, nd_byte_order() != streamByteOrder);
		}
	}
	else {
		len = _writeEmptyStream(streamBuf, streamByteOrder);
	}

	return write_len + len;
}

int DBLDataNode::GetInt() const
{
	if (!m_data){
		return 0;
	}
	if (m_ele_type == OT_INT || m_ele_type == OT_BOOL || m_ele_type == OT_INT8 || m_ele_type == OT_INT16)	{
		return  m_data->i_val;
	}
	else if (OT_INT64 == m_ele_type || OT_TIME == m_ele_type){
		return (int)m_data->i64_val;
	}
	else if (OT_FLOAT == m_ele_type){
		return (int)m_data->f_val;
	}
	else if (OT_STRING == m_ele_type) {
		const char *pText = GetText();
		if (pText && *pText)  {
			DBLDataNode tmpdata;
			if (tmpdata.StringToArrayInt(pText)){
				return tmpdata.GetarrayInt(0);
			}
			return (int)ndstr_atoi_hex(pText);
		}
	}
	return 0;
}

int DBLDataNode::GetRoundInt() const
{
	if (m_ele_type == OT_FLOAT || m_ele_type == OT_STRING)	{
		float fVal = GetFloat();
		return (int)( fVal + 0.5f );
	}
	else {
		return GetInt();
	}
}

NDUINT64 DBLDataNode::GetInt64() const
{
	if (!m_data){
		return 0;
	}
	if (m_ele_type == OT_INT || m_ele_type == OT_BOOL || m_ele_type == OT_INT8 || m_ele_type == OT_INT16)	{
		return  m_data->i_val;
	}
	else if (OT_INT64 == m_ele_type || OT_TIME == m_ele_type){
		return m_data->i64_val;
	}
	else if (OT_FLOAT == m_ele_type){
		return (NDUINT64)m_data->f_val;
	}

	else if (OT_STRING == m_ele_type) {
		const char *pText = GetText();

		if (pText && *pText)  {
			DBLDataNode tmpdata;
			if (tmpdata.StringToArrayInt(pText)){
				return tmpdata.GetarrayInt(0);
			}
			return atoll(pText);
		}
	}
	return 0;

}
bool DBLDataNode::GetBool() const
{
	if (!m_data){
		return false;
	}

	if (m_ele_type == OT_INT || m_ele_type == OT_BOOL || m_ele_type == OT_INT8 || m_ele_type == OT_INT16)	{
		return  m_data->i_val ? true : false;
	}
	else if (OT_INT64 == m_ele_type || OT_TIME == m_ele_type){
		return m_data->i64_val ? true : false;
	}
	else if (OT_FLOAT == m_ele_type){
		return !(m_data->f_val == 0);
	}
	else if (OT_STRING == m_ele_type) {
		if (m_data->str_val && m_data->str_val[0]){
			if (0 == ndstricmp(m_data->str_val, "no") || 0 == ndstricmp(m_data->str_val, "none")
				|| 0 == ndstricmp(m_data->str_val, "false") || 0 == ndstricmp(m_data->str_val, "0")
				|| 0 == ndstricmp(m_data->str_val, "error")) {
				return false; 
			}
			return true;
		}
		//return atoi(m_data->str_val) ? true : false;
	}
	return false;
}
float DBLDataNode::GetFloat() const
{
	if (!m_data){
		return 0;
	}
	if (OT_FLOAT == m_ele_type){
		return m_data->f_val;
	}
	else if (m_ele_type == OT_INT || m_ele_type == OT_BOOL || m_ele_type == OT_INT8 || m_ele_type == OT_INT16)	{
		return (float) m_data->i_val;
	}
	else if (OT_INT64 == m_ele_type || OT_TIME == m_ele_type){
		return (float) m_data->i64_val;
	}
	else if (OT_STRING == m_ele_type) {
		const char *pText = GetText();

		if (pText && *pText)  {
			DBLDataNode tmpdata;
			if (tmpdata.StringToArrayFloat(pText)){
				return tmpdata.GetarrayFloat(0);
			}
			return (float)atof(pText);
		}
		//if (pText) return (float) atof(pText);
	}
	return 0.0f;
}

const char *DBLDataNode::GetText() const
{
	if (!m_data){
		return 0;
	}
	if (OT_STRING == m_ele_type && m_data->str_val &&m_data->str_val[0]){
		if (0 == ndstricmp("none", m_data->str_val)) {
			return NULL;
		}
		else if (0 == ndstricmp("null", m_data->str_val)) {
			return NULL;
		}
		return (const char *)m_data->str_val;
	}
	return NULL;
}

std::string DBLDataNode::GetString() const
{
	if (!CheckValid() ) {
		return std::string("");
	}

	if (OT_STRING == m_ele_type){
		const char *p = GetText();
		if (p) {
			return std::string(p);
		}
		else {
			return std::string("");
		}
	}
	else {
		char buf[4096];
		int size = Sprint(buf, sizeof(buf));
		if (size > 0) {
			return std::string(buf);
		}
		else {
			return std::string("");
		}
	}

}

void *DBLDataNode::GetObject() const
{
	if (m_ele_type >= OT_OBJECT_VOID && m_ele_type!=OT_BINARY_DATA){
		return m_data->_data;
	}
	return NULL;
}

void *DBLDataNode::GetBinary() const
{
	if (  m_data->_bin){
		return m_data->_bin->data;
	}
	return NULL;
}

size_t DBLDataNode::GetBinarySize() const
{
	if ( m_data->_bin){
		return m_data->_bin->size;
	}
	return 0;
}

DBLDataNode DBLDataNode::GetArray(int index)const
{
	if (m_ele_type != OT_ARRAY){
		return *this;
	}
	if (m_sub_type == OT_INT || m_sub_type == OT_BOOL || m_sub_type == OT_INT8 || 
		m_sub_type == OT_INT16 ||  OT_TIME == m_sub_type)	{
		return DBLDataNode(GetarrayInt(index), m_sub_type);
	}
	else if (OT_INT64 == m_sub_type){
		return DBLDataNode(GetarrayInt64(index));
	}

	else if (OT_FLOAT == m_sub_type){
		return DBLDataNode(GetarrayFloat(index));
	}
	else if (OT_STRING == m_sub_type) {
		return DBLDataNode(GetarrayText(index));
	}
	else if (OT_USER_DEFINED == m_sub_type) {
		const LogicUserDefStruct *pData = GetarrayUser(index);
		if (pData){
			return DBLDataNode(*pData);
		}
	}
	return DBLDataNode();

}

int DBLDataNode::GetarrayInt(int index) const
{
	if (!CheckArray(index)){
		if (m_ele_type==OT_STRING)	{
			DBLDataNode tmp;
			if (tmp.StringToArrayInt(GetText())) {
				return tmp.GetarrayInt(index); 
			}
		}
		
		if (index == 0){
			return GetInt();
		}
		return 0;
	}
	if (OT_ARRAY == m_ele_type){
		if (m_sub_type == OT_INT || m_sub_type == OT_BOOL || m_sub_type == OT_INT8 ||
			m_sub_type == OT_INT16 || OT_TIME == m_sub_type){
			return m_data->_i_arr->data[index];
		}

		else if (OT_INT64 == m_sub_type) {
			return (int)m_data->_int64_arr->data[index];
		}

		else if (OT_FLOAT == m_sub_type) {
			return (int)m_data->_f_arr->data[index];
		}
		else if (OT_STRING == m_sub_type) {
			if (m_data->_str_arr->data[index])	{
				const char *pText = m_data->_str_arr->data[index];
				if (pText && *pText)  {
					DBLDataNode tmpdata;
					if (tmpdata.StringToArrayInt(pText)){
						return tmpdata.GetarrayInt(0);
					}
					return (int)ndstr_atoi_hex(pText);
				}

				//return atoi(m_data->_str_arr->data[index]);
			}
			return 0;
		}
	}

	return 0;
}

bool DBLDataNode::GetarrayBool(int index) const
{
	if (!CheckArray(index)){
		if (m_ele_type == OT_STRING)	{
			DBLDataNode tmp;
			if (tmp.StringToArrayString(GetText())) {
				return tmp.GetarrayBool(index);
			}
		}

		if (index == 0){
			return GetBool();
		}
		return false;
	}
	if (OT_ARRAY == m_ele_type){
		if (m_sub_type == OT_INT || m_sub_type == OT_BOOL || m_sub_type == OT_INT8 ||
			m_sub_type == OT_INT16 ||  OT_TIME == m_sub_type)	{
			return m_data->_i_arr->data[index] ? true : false;
		}

		else if (OT_INT64 == m_sub_type) {
			return (int)m_data->_int64_arr->data[index] ? true :false;
		}

		else if (OT_FLOAT == m_sub_type) {
			return m_data->_f_arr->data[index] == 0 ? true : false;
		}
		else if (OT_STRING == m_sub_type) {
			if (m_data->_str_arr->data[index])	{

				const char *pText = m_data->_str_arr->data[index];
				if (pText && *pText)  {
					DBLDataNode tmpdata;
					if (tmpdata.StringToArrayInt(pText)){
						return tmpdata.GetarrayInt(0)?true:false;
					}
					return ndstr_atoi_hex(pText) ? true : false;
				}

			}
			return false;
		}
	}

	return false;
}

float DBLDataNode::GetarrayFloat(int index) const
{
	if (!CheckArray(index)){
		if (m_ele_type == OT_STRING)	{
			DBLDataNode tmp;
			if (tmp.StringToArrayFloat(GetText())) {
				return tmp.GetarrayFloat(index);
			}
		}

		if (index == 0){
			return GetFloat();
		}
		return 0;
	}
	if (OT_ARRAY == m_ele_type){
		if (m_sub_type == OT_INT || m_sub_type == OT_BOOL || m_sub_type == OT_INT8 ||
			m_sub_type == OT_INT16 || OT_TIME == m_sub_type)	{
			return(float)m_data->_i_arr->data[index];
		}

		else if (OT_INT64 == m_sub_type) {
			return (float)m_data->_int64_arr->data[index] ;
		}

		else if (OT_FLOAT == m_sub_type) {
			return m_data->_f_arr->data[index];
		}
		else if (OT_STRING == m_sub_type) {

			if (m_data->_str_arr->data[index])	{
				const char *pText = m_data->_str_arr->data[index];
				if (pText && *pText)  {
					DBLDataNode tmpdata;
					if (tmpdata.StringToArrayFloat(pText)){
						return tmpdata.GetarrayFloat(0);
					}
					return (float)atof(pText);
				}
			}

			return 0;
		}
	}

	return 0;
}

NDUINT64 DBLDataNode::GetarrayInt64(int index) const
{
	if (!CheckArray(index)){
		if (m_ele_type == OT_STRING)	{
			DBLDataNode tmp;
			if (tmp.StringToArrayInt(GetText())) {
				return tmp.GetarrayInt64(index);
			}
		}

		if (index == 0){
			return GetInt64();
		}
		return 0;
	}
	if (OT_ARRAY == m_ele_type){
		if (m_sub_type == OT_INT || m_sub_type == OT_BOOL || m_sub_type == OT_INT8 ||
			m_sub_type == OT_INT16 || OT_TIME == m_sub_type){
			return m_data->_i_arr->data[index];
		}

		else if (OT_INT64 == m_sub_type) {
			return m_data->_int64_arr->data[index];
		}

		else if (OT_FLOAT == m_sub_type) {
			return (NDUINT64)m_data->_f_arr->data[index];
		}
		else if (OT_STRING == m_sub_type) {
			if (m_data->_str_arr->data[index])	{
				const char *pText = m_data->_str_arr->data[index];
				if (pText && *pText)  {
					DBLDataNode tmpdata;
					if (tmpdata.StringToArrayInt(pText)){
						return tmpdata.GetarrayInt(0);
					}
					return ndstr_atoi_hex(pText);
				}

				//return atoi(m_data->_str_arr->data[index]);
			}
			return 0;
		}
	}

	return 0;
}

const char *DBLDataNode::GetarrayText(int index) const
{
	if (!CheckArray(index)){
		if (m_ele_type == OT_STRING)	{
			DBLDataNode tmp;
			if (tmp.StringToArrayString(GetText())) {
				return tmp.GetarrayText(index);
			}
		}

		if (index == 0){
			return GetText();
		}
		return 0;
	}
	return (m_data->_str_arr->data[index]);
}

const LogicUserDefStruct *DBLDataNode::GetarrayUser(int index) const
{
	if (m_ele_type == OT_USER_DEFINED)	{
		return m_data->_userDef;
	}
	else if (m_ele_type == OT_ARRAY && m_sub_type == OT_USER_DEFINED) {
		dbl_userDefArray *pU = m_data->_arr_user;
		if (pU && index < pU->number )	{
			return pU->data[index];
		}
	}

	return NULL;
}


DBLDataNode  DBLDataNode::operator + (const DBLDataNode &r) const
{
	if (!CheckValid()){
		return r;
	}
	if (!r.CheckValid())	{
		return *this;
	}
	switch (m_ele_type)
	{
	case OT_INT8:
	case OT_INT16:
	case OT_INT:
	{
		int val = GetInt() + r.GetInt() ;
		return DBLDataNode(val);
	}
	case OT_TIME:
	case OT_INT64:
	{
		NDUINT64 val = GetInt64() + r.GetInt64();
		return DBLDataNode(val);
	}
	case OT_FLOAT:
	{
		float val = GetFloat() + r.GetFloat();
		return DBLDataNode(val);
	}
	case OT_STRING:
	{
		std::string str1 = GetString();
		str1 += r.GetString();

		return DBLDataNode(str1.c_str());
	}
	case OT_ORG_STREAM:
	case OT_BINARY_DATA:
		if (r.m_ele_type == OT_BINARY_DATA || r.m_ele_type == OT_ORG_STREAM){
			size_t s1 = GetBinarySize();
			if (s1 > 0)	{
				size_t s2 = r.GetBinarySize();
				char *pdata = (char*) malloc(s1+s2);
				if (pdata)	{
					memcpy(pdata, GetBinary(), s2);
					memcpy(pdata+s2, r.GetBinary(), s1);
					DBLDataNode tmp((void*)pdata, s1 + s2);
					free(pdata);
					return tmp;
				}
			}
		}
		break;
	case OT_ATTR_DATA:
		return _attrMathAdd(r);
	case  OT_ARRAY:
		return _arrayMathAdd(r);
	default:
		nd_logerror("dbldatanode math op ADD not surport\n");
		break;
	}
	return *this;
}

DBLDataNode  DBLDataNode::operator-(const DBLDataNode &r) const
{
	if (!CheckValid() || !r.CheckValid())	{
		return *this;
	}
	switch (m_ele_type)
	{
	case OT_INT8:
	case OT_INT16:
	case OT_INT:
	{
		int val = GetInt() - r.GetInt();
		return DBLDataNode(val);
	}
	case OT_TIME:
	case OT_INT64:
	{
		NDUINT64 val = GetInt64() - r.GetInt64();
		return DBLDataNode(val);
	}
	case OT_FLOAT:
	{
		float val = GetFloat() - r.GetFloat();
		return DBLDataNode(val);
	}
	case OT_ATTR_DATA:
		return _attrMathSub(r);
	case  OT_ARRAY:
		return _arrayMathSub(r);
	default:
		nd_logerror("dbldatanode math op SUB not surport\n");
		break;
	}
	return *this;
}
DBLDataNode  DBLDataNode::operator*(const DBLDataNode &r) const
{
	if (!CheckValid() || !r.CheckValid())	{
		return *this;
	}
	switch (m_ele_type)
	{
	case OT_INT8:
	case OT_INT16:
	case OT_INT:
	{
		int val = GetInt() * r.GetInt();
		return DBLDataNode(val);
	}
	case OT_TIME:
	case OT_INT64:
	{
		NDUINT64 val = GetInt64() * r.GetInt64();
		return DBLDataNode(val);
	}
	case OT_FLOAT:
	{
		float val = GetFloat() * r.GetFloat();
		return DBLDataNode(val);
	}
	case OT_ATTR_DATA:
		return _attrMathMul(r);
	case  OT_ARRAY:
		return _arrayMathMul(r);
	default:
		nd_logerror("dbldatanode math op MUL not surport\n");
		break;
	}
	return *this;
}

DBLDataNode  DBLDataNode::operator/(const DBLDataNode &r) const
{
	if (!CheckValid() || !r.CheckValid())	{
		return *this;
	}
	float f1 = GetFloat();
	float f2 = r.GetFloat();
	float val = 0;
	if (f2 == 0) {
		val = f1;
	}
	else {
		val = f1 / f2;
	}
	
	switch (m_ele_type)
	{
	case OT_INT8:
		return DBLDataNode((NDUINT8)val);
	case OT_INT16:
		return DBLDataNode((NDUINT16)val);
	case OT_INT:
		return DBLDataNode((int)val);
	case OT_TIME:
		return DBLDataNode((time_t)val);
	case OT_INT64:

		return DBLDataNode((NDUINT64)val);
	case OT_FLOAT:
		return DBLDataNode(val);
	case OT_ATTR_DATA:
		return _attrMathDiv(r);
	case  OT_ARRAY:
		return _arrayMathDiv(r);
	default:
		nd_logerror("dbldatanode math op DIV not surport\n");
		break;
	}
	return *this;
}

DBLDataNode  DBLDataNode::operator+(const char *text) const
{
	if (!CheckValid()){
		return DBLDataNode(text);
	}
	if (!text || !*text) {
		return *this;
	}
	switch (m_ele_type)
	{
	case OT_INT8:
	case OT_INT16:
	case OT_INT:
	{
		int rval = (int)ndstr_atoi_hex(text);
		int val = GetInt() +rval;
		return DBLDataNode(val);
	}
	case OT_TIME:
	case OT_INT64:
	{
		int rval = (int)ndstr_atoi_hex(text);
		NDUINT64 val = GetInt64() + rval;
		return DBLDataNode(val);
	}
	case OT_FLOAT:
	{

		float rval = (float)atof(text);
		float val = GetFloat() +rval;
		return DBLDataNode(val);
	}
	case OT_STRING:
	{
		std::string str1 = GetText();
		str1 += text;

		return DBLDataNode(str1.c_str());
	}
	default:
		break;
	}
	return DBLDataNode();
}

DBLDataNode & DBLDataNode::operator+=(const DBLDataNode &r)
{
	DBLDataNode val = *this;
	*this = val + r;
	return *this;
}
DBLDataNode & DBLDataNode::operator+=(const char *text)
{
	DBLDataNode val = *this;
	*this = val + text;
	return *this;
}

DBLDataNode DBLDataNode::operator[](int n)
{
	return  GetArray(n);
}


int DBLDataNode::GetArraySize() const
{
	if (!m_data || !m_data->_data){
		return 0;
	}
	if (m_ele_type != OT_ARRAY) {
		return 0;
	}
	return (int)m_data->_i_arr->number;
}

bool DBLDataNode::toArray(DBLDataNode &outVal)
{
	if (m_ele_type != OT_STRING){
		return false;
	}
	return outVal.StringToArrayString(this->GetText());
}

bool DBLDataNode::StringToArrayInt(const char *text)
{
	if (!_check_array(text)){
		return false;
	}
	Destroy();
	m_ele_type = OT_ARRAY;
	m_sub_type = OT_INT;
	m_dataOwner = true;
	m_data = &m_dataOwn;

	return 0 == dbl_build_from_text(&m_dataOwn, (char*)text, (DBL_ELEMENT_TYPE)m_ele_type, (DBL_ELEMENT_TYPE)m_sub_type);
}
bool DBLDataNode::StringToArrayFloat(const char *text)
{
	if (!_check_array(text)){
		return false;
	}
	Destroy();
	m_ele_type = OT_ARRAY;
	m_sub_type = OT_FLOAT;
	m_dataOwner = true;
	m_data = &m_dataOwn;

	return 0 == dbl_build_from_text(&m_dataOwn, (char*)text, (DBL_ELEMENT_TYPE)m_ele_type, (DBL_ELEMENT_TYPE)m_sub_type);

}
bool DBLDataNode::StringToArrayString(const char *text)
{
	if (!text || !text[0]) {
		return false;
	}
	if (!_check_array(text)){
		return false;
	}
	Destroy();
	m_ele_type = OT_ARRAY;
	m_sub_type = OT_STRING;
	m_dataOwner = true;
	m_data = &m_dataOwn;

	return 0 == dbl_build_from_text(&m_dataOwn, (char*)text, (DBL_ELEMENT_TYPE)m_ele_type, (DBL_ELEMENT_TYPE)m_sub_type);
}


#define _GET_TIME_VAL(_typeName) do {	\
	time_t tmval = GetTime();			\
	tm _tmptm ;							\
	tm *ptm = localtime_r(&tmval,&_tmptm);	\
	if (ptm) {							\
		return ptm->_typeName;			\
	}									\
} while (0)


time_t DBLDataNode::GetTime() const
{
	if (m_ele_type == OT_TIME || m_ele_type == OT_INT64){
		return  (time_t)m_data->i64_val;
	}
	else if (OT_STRING == m_ele_type) {
		time_t tim;
		time_t tim_val = nd_time_from_str(GetText(), &tim);
		if (tim_val!=(time_t)-1){
			return tim;
		}
	}
	return 0;
}
int DBLDataNode::GetTimeYear()const
{
	_GET_TIME_VAL(tm_year);
	return 0;
}

int DBLDataNode::GetTimeMonth() const
{
	_GET_TIME_VAL(tm_mon);
	return 0;
}
int DBLDataNode::GetTimeDay() const
{
	_GET_TIME_VAL(tm_mday);
	return 0;

}
int DBLDataNode::GetTimeHour() const
{
	_GET_TIME_VAL(tm_hour);
	return 0;

}
int DBLDataNode::GetTimeMinute()  const
{
	_GET_TIME_VAL(tm_min);
	return 0;
}
int DBLDataNode::GetTimeSecond() const
{

	_GET_TIME_VAL(tm_sec);
	return 0;
}

int DBLDataNode::GetTimeWeekDay() const
{
	_GET_TIME_VAL(tm_wday);
	return 0;
}


bool DBLDataNode::TransferType(DBL_ELEMENT_TYPE aimType)
{
	if (m_ele_type == aimType){
		return true;
	}
	switch (aimType)
	{
	case OT_INT:
		InitSet(GetInt());
		break;
	case OT_FLOAT:
		InitSet(GetFloat());
		break;
	case OT_STRING:
	{
		char buf[4096];
		buf[0] = 0;
		size_t s = Sprint(buf, sizeof(buf));
		if (s > 0)	{
			InitSet(buf);
			return true;
		}
		return false;
	}
		break;
	case OT_INT8:
		InitSet((NDUINT8)GetInt());
		break;
	case OT_INT16:
		InitSet((NDUINT16)GetInt());
		break;
	case OT_INT64:

		InitSet((NDUINT64)GetInt());
		break;
	case OT_BOOL:
		InitSet(GetBool());
		break;	
	case OT_TIME:
		InitSet((time_t)GetInt64());
		break;
	
	default:
		return false;
	}
	return true;
}

bool DBLDataNode::CheckValid() const
{
	if (!m_data || !m_data->isInit){
		return false;
	}

	if (m_ele_type == OT_STRING || m_ele_type == OT_ARRAY 
		|| m_ele_type >= OT_OBJECT_VOID)	{
		return m_data->_data ? true : false;
 	}
	return true;
}

bool DBLDataNode::CheckArray(int index) const
{
	if (!m_data || !m_data->_data){
		return false;
	}
	if (index < 0 || index >= m_data->_i_arr->number){
		return false;
	}
	return true;
}

struct _mysprintfAddr{
	size_t bufsize ;
	char *addr ;
};

static  int mysprintf(void *pf, const char *stm, ...)
{
	_mysprintfAddr *pBuf = (_mysprintfAddr*) pf ;
	char *pAddr = pBuf->addr ;
	
	va_list arg;
	int done =0;
	
	va_start(arg, stm);
	done = vsnprintf(pAddr, pBuf->bufsize, stm, arg);
	pBuf->addr += done;
	pBuf->bufsize -= done;
	va_end(arg);

	
	return  done ;
	
}

int DBLDataNode::Sprint(char *buf, size_t size) const
{
	//char buf[4096] ;
	_mysprintfAddr spAddr ;
	spAddr.bufsize = size ;
	spAddr.addr = buf ;
	
	return Print(mysprintf, (void*)&spAddr) ;	
}

void DBLDataNode::ConvertEncode(int from, int to)
{
	if (GetDataType() != OT_STRING)	{
		return;
	}
	if (from == E_SRC_CODE_ANSI || from == to){
		return;
	}

	const char *text = GetText();
	if (!text || !text[0])
		return;

	size_t size  = strlen(text) * 2 ;
	char *pBuf = (char*) malloc(size +1);
	if (!pBuf)
		return;
	if (from == E_SRC_CODE_UTF_8){
		nd_utf8_to_gbk(text, pBuf, (int)size);
		InitSet(pBuf);
	}
	else if (to == E_SRC_CODE_UTF_8) {
		nd_gbk_to_utf8(text, pBuf, (int)size);
		InitSet(pBuf);
	}
	else {
		//nothing to be done
	}
	free(pBuf);
}

int DBLDataNode::Print(logic_print print_func, void *pf) const
{
	int ret = 0;
	switch (GetDataType()){
	case  OT_INT:
	case  OT_INT8:
	case  OT_INT16:
		if (IS_OUT_HEX()) {
			ret = print_func(pf, "0x%x", GetInt());
		}
		else{
			ret = print_func(pf, "%d", GetInt());
		}
		break; 
	case  OT_INT64:
		if (IS_OUT_HEX()) {
			ret = print_func(pf, "0x%llx", GetInt64());
		}
		else{
			ret = print_func(pf, "%lld", GetInt64());
		}
		break;
	case OT_TIME:
	{
		char buf[128];
		nd_get_datetimestr_ex((time_t)GetInt64(), buf, 128);
		ret = print_func(pf, "%s",buf);
	}
		break;
	case OT_STRING:
		if (IS_OUT_LUA()) {
			const char *ptext = GetText();
			if (ptext && *ptext) {
				if ( *ptext == '\"') {
					ret = print_func(pf, "%s", ptext);
					break; 
				}
				else if (ndstricmp(ptext, "true") == 0 ) {
					ret = print_func(pf, "true", GetBool());
					break;
				}
				else if (0 == ndstricmp(ptext, "false")) {
					ret = print_func(pf, "false", GetBool());
					break;
				}
			}
			ret = print_func(pf, "\"%s\"", CheckValid()? GetText():"nil");
		}
		else {
			ret = print_func(pf, "%s", GetText());
		}
		break;
	case  OT_BOOL:
		ret = print_func(pf, "%s", GetInt() ? "true" : "false");
		break;
	case  OT_FLOAT:
		ret = print_func(pf, "%f", GetFloat());
		break;
	case  OT_ARRAY:
		ret += print_func(pf, "%c", IS_OUT_LUA()?'{': _ARRAR_BEGIN_MARK);
		for (int x = 0; x < GetArraySize(); x++) {
			if (m_sub_type == OT_INT || m_sub_type == OT_INT8 || m_sub_type == OT_INT16 || m_sub_type == OT_BOOL||
				m_sub_type == OT_TIME) {
				if (IS_OUT_HEX()) {
					ret += print_func(pf, "%x", GetarrayInt(x));
				}
				else{
					ret += print_func(pf, "%d", GetarrayInt(x));
				}
			}

			else if (OT_INT64 == m_sub_type) {
				if (IS_OUT_HEX()) {
					ret += print_func(pf, "0x%llx", GetarrayInt64(x));
				}
				else {
					ret += print_func(pf, "%lld", GetarrayInt64(x));
				}
			}

			else if(m_sub_type == OT_FLOAT) {
				ret += print_func(pf, "%f", GetarrayFloat(x));
			}
			else if(m_sub_type == OT_STRING) {
				if (IS_OUT_LUA()) {
					DBLDataNode node;
					if (node.StringToArrayString(GetarrayText(x))) {
						ret += node.Print(print_func, pf);
					}
					else {
						const char *ptext = GetarrayText(x);
						if (*ptext == '\"') {
							ret += print_func(pf, "%s", ptext);
						}
						else {
							ret += print_func(pf, "\"%s\"", ptext);
						}
					}
				}
				else {
					ret += print_func(pf, "%s", GetarrayText(x));
				}
				
			}
			else if (m_sub_type == OT_USER_DEFINED) {
				const LogicUserDefStruct *pUser = GetarrayUser(x);
				if (pUser)	{
					ret = pUser->Print(print_func, pf);
				}
			}

			//out ,
			if (x < GetArraySize() - 1){
				ret += print_func(pf, ",");
			}
					
		}
		ret += print_func(pf, "%c", IS_OUT_LUA() ? '}' : _ARRAR_END_MARK);
		break;
		
	case OT_OBJ_BASE_OBJ:
	{
		LogicObjectBase *pObj = (LogicObjectBase *)GetObject();
		if (pObj)  {
			ret = pObj->Print(print_func, pf);
		}
	}
		break;
	case OT_USER_DEFINED:
		{
			LogicUserDefStruct *pUser = m_data->_userDef ;
			if (pUser) {
				ret = pUser->Print(print_func, pf) ;
			}
		}
		break;
			
	case OT_ATTR_DATA:
		{
			struct otPack {
				int number;
				struct {
					NDUINT8 aid ;
					float val ;
				}datas[100] ;
			} ;
			otPack *pdata = (otPack *) GetBinary() ;
			int len =print_func(pf, "attr size =%d(", pdata->number);
			ret += len ;
			for (int i=0; i<pdata->number; ++i) {
				len =print_func(pf, "(%d,%f)" , pdata->datas[i].aid, pdata->datas[i].val);
				ret += len ;
			}
			len = print_func(pf, ")");
			ret += len ;
			break ;

		}
	default:
		ret = print_func(pf, "(addr=%p)", GetObject());
		break;
	}
	return ret;
}

DBL_ELEMENT_TYPE DBLDataNode::GetDataType() const
{
	return (DBL_ELEMENT_TYPE)m_ele_type;
}
DBL_ELEMENT_TYPE DBLDataNode::GetArrayType() const
{
	return (DBL_ELEMENT_TYPE)m_sub_type;
}


void DBLDataNode::Destroy()
{
	if (m_dataOwner && m_dataOwn._data) {
		if (m_ele_type == OT_FILE_STREAM)	{
			if (m_data->_data)	{
				fclose((FILE*)m_data->_data);
			}
		}
		else {
			dbl_destroy_data(m_data, (DBL_ELEMENT_TYPE)m_ele_type, (DBL_ELEMENT_TYPE)m_sub_type);
		}
		
	}
	m_dataOwn._data = 0;
	m_ele_type = 0;
	m_sub_type = 0;
	m_data = 0;
	m_dataOwner = false;
	//m_unsafeRef = false;
	m_dataOwn.isInit = true;
}

//math operate 

static float getValAttr(_attrDataBin *pattr, NDUINT8 id, float defVal )
{
	for (int i = 0; i < pattr->count; i++)	{
		if (id == pattr->datas[i].aid) {
			return pattr->datas[i].val;
		}
	}
	return defVal;
}

DBLDataNode DBLDataNode::_attrMathAdd(const DBLDataNode &leftval)const
{
	DBLDataNode val = *this;

	_attrDataBin *pattr = (_attrDataBin *) val.GetBinary();
	if (!pattr){
		return DBLDataNode();
	}

	if (leftval.GetDataType() == OT_ATTR_DATA){
		_attrDataBin *left_attr = (_attrDataBin *)leftval.GetBinary();
		if (!left_attr)	{
			return *this;
		}

		for (int i = 0; i < pattr->count; i++)	{
			pattr->datas[i].val += getValAttr(left_attr, pattr->datas[i].aid, 0);
		}

	}
	else {
		float ldata = leftval.GetFloat();
		if (ldata != 0)	{
			for (int i = 0; i < pattr->count; i++)	{
				pattr->datas[i].val += ldata;
			}
		}

	}
	return val;
}
DBLDataNode DBLDataNode::_attrMathSub(const DBLDataNode &leftval)const
{
	DBLDataNode val = *this;

	_attrDataBin *pattr = (_attrDataBin *)val.GetBinary();
	if (!pattr){
		return DBLDataNode();
	}

	if (leftval.GetDataType() == OT_ATTR_DATA){
		_attrDataBin *left_attr = (_attrDataBin *)leftval.GetBinary();
		if (!left_attr)	{
			return *this;
		}

		for (int i = 0; i < pattr->count; i++)	{
			pattr->datas[i].val -= getValAttr(left_attr, pattr->datas[i].aid, 0);
		}

	}
	else {
		float ldata = leftval.GetFloat();
		if (ldata != 0)	{
			for (int i = 0; i < pattr->count; i++)	{
				pattr->datas[i].val -= ldata;
			}
		}

	}
	return val;
}
DBLDataNode DBLDataNode::_attrMathMul(const DBLDataNode &leftval)const
{
	DBLDataNode val = *this;

	_attrDataBin *pattr = (_attrDataBin *)val.GetBinary();
	if (!pattr){
		return DBLDataNode();
	}

	if (leftval.GetDataType() == OT_ATTR_DATA){
		_attrDataBin *left_attr = (_attrDataBin *)leftval.GetBinary();
		if (!left_attr)	{
			return *this;
		}

		for (int i = 0; i < pattr->count; i++)	{
			pattr->datas[i].val *= getValAttr(left_attr, pattr->datas[i].aid, 1.0);
		}

	}
	else {
		float ldata = leftval.GetFloat();
		for (int i = 0; i < pattr->count; i++)	{
			pattr->datas[i].val *= ldata;
		}
	}
	return val;
}
DBLDataNode DBLDataNode::_attrMathDiv(const DBLDataNode &leftval)const
{
	DBLDataNode val = *this;

	_attrDataBin *pattr = (_attrDataBin *)val.GetBinary();
	if (!pattr){
		return DBLDataNode();
	}

	if (leftval.GetDataType() == OT_ATTR_DATA){
		_attrDataBin *left_attr = (_attrDataBin *)leftval.GetBinary();
		if (!left_attr)	{
			return *this;
		}

		for (int i = 0; i < pattr->count; i++)	{
			float divval = getValAttr(left_attr, pattr->datas[i].aid, 1.0);
			if (divval != 0)	{
				pattr->datas[i].val /= divval;
			}
		}

	}
	else {
		float ldata = leftval.GetFloat();
		if (ldata != 0)	{
			for (int i = 0; i < pattr->count; i++)	{
				pattr->datas[i].val /= ldata;
			}
		}

	}
	return val;
}

/// array math operate

DBLDataNode DBLDataNode::_arrayMathAdd(const DBLDataNode &leftval)const
{

	DBLDataNode val = *this;
	switch (m_sub_type)
	{
	case OT_INT8:
	case OT_INT16:
	case OT_INT:
	{
		dbl_intarray *parray = val.m_data->_i_arr;
		for (int i = 0; i < parray->number; i++){
			parray->data[i] += leftval.GetarrayInt(i);
		}
	}
	break;
	case OT_INT64:
	{

		dbl_int64array *parray = val.m_data->_int64_arr;
		for (int i = 0; i < parray->number; i++){
			parray->data[i] += leftval.GetarrayInt64(i);
		}
	}
	break;
	case OT_FLOAT:
	{
		dbl_floatarray *parray = val.m_data->_f_arr;
		for (int i = 0; i < parray->number; i++){
			parray->data[i] += leftval.GetarrayFloat(i);
		}
	}
	break;
	}
	
	return val;
}
DBLDataNode DBLDataNode::_arrayMathSub(const DBLDataNode &leftval)const
{
	DBLDataNode val = *this;
	switch (m_sub_type)
	{
	case OT_INT8:
	case OT_INT16:
	case OT_INT:
	{
		dbl_intarray *parray = val.m_data->_i_arr;
		for (int i = 0; i < parray->number; i++){
			parray->data[i] -= leftval.GetarrayInt(i);
		}
	}
	break;
	case OT_INT64:
	{
		dbl_int64array *parray = val.m_data->_int64_arr;
		for (int i = 0; i < parray->number; i++){
			parray->data[i] -= leftval.GetarrayInt64(i);
		}
	}
	break;
	case OT_FLOAT:
	{
		dbl_floatarray *parray = val.m_data->_f_arr;
		for (int i = 0; i < parray->number; i++){
			parray->data[i] -= leftval.GetarrayFloat(i);
		}
	}
	break;
	}

	return val;

}

DBLDataNode DBLDataNode::_arrayMathMul(const DBLDataNode &leftval)const
{
	DBLDataNode val = *this;
	switch (m_sub_type)
	{
	case OT_INT8:
	case OT_INT16:
	case OT_INT:
	{
		dbl_intarray *parray = val.m_data->_i_arr;
		for (int i = 0; i < parray->number; i++){
			parray->data[i] *= leftval.GetarrayInt(i);
		}
	}
	break;

	case OT_INT64:
	{

		dbl_int64array *parray = val.m_data->_int64_arr;
		for (int i = 0; i < parray->number; i++){
			parray->data[i] *= leftval.GetarrayInt64(i);
		}
	}
	break;
	case OT_FLOAT:
	{
		dbl_floatarray *parray = val.m_data->_f_arr;
		for (int i = 0; i < parray->number; i++){
			parray->data[i] *= leftval.GetarrayFloat(i);
		}
	}
	break;
	}

	return val;
}
DBLDataNode DBLDataNode::_arrayMathDiv(const DBLDataNode &leftval)const
{
	DBLDataNode val = *this;
	switch (m_sub_type)
	{
	case OT_INT8:
	case OT_INT16:
	case OT_INT:
	{
		dbl_intarray *parray = val.m_data->_i_arr;
		for (int i = 0; i < parray->number; i++){
			int divval = leftval.GetarrayInt(i); 
			if (divval != 0) {
				parray->data[i] /= divval;
			}
		}
	}
	break;

	case OT_INT64:
	{

		dbl_int64array *parray = val.m_data->_int64_arr;
		for (int i = 0; i < parray->number; i++){
			NDUINT64 divval = leftval.GetarrayInt64(i);
			if (divval != 0) {
				parray->data[i] /= divval;
			}
		}

	}
	break;

	case OT_FLOAT:
	{
		dbl_floatarray *parray = val.m_data->_f_arr;
		for (int i = 0; i < parray->number; i++){

			float divval = leftval.GetarrayFloat(i);
			if (divval != 0) {
				parray->data[i] /= divval;
			}

		}
	}
	break;
	}

	return val;
}

///////////////////////////////////////////////
#include "nd_net/byte_order.h"
static inline void _tryto_change_order_short(NDUINT16 &val, bool needChange)
{
	if (needChange) {
		val = nd_order_change_short(val);
	}
}

static inline void _tryto_change_order_long(NDUINT32 &val, bool needChange)
{
	if (needChange) {
		val = nd_order_change_long(val);
	}
}

static inline void _tryto_change_order_long64(NDUINT64 &val, bool needChange)
{
	if (needChange) {
		val = nd_order_change_longlong(val);
	}
}

template <class TStreamRW, class TStreamHandle>
int _dbl_data_2binStream(dbl_element_base *indata, DBL_ELEMENT_TYPE etype, DBL_ELEMENT_TYPE sub_etype, TStreamHandle &stream_h, TStreamRW &wf, bool changeByteOrder )
{
#define _stream_func(_addr, _s, _c, _stream) wf(_addr, _s, _c,stream_h)
	NDUINT16 size;
	size_t length;
	switch (etype){
	case OT_BOOL:
	case OT_INT:
	{
		NDUINT32 val = indata->i_val;
		_tryto_change_order_long(val, changeByteOrder);
		_stream_func(&val, 1, sizeof(NDUINT32), pf);
		break;
	}
	case OT_FLOAT:
		_stream_func(&(indata->f_val), 1, sizeof(float), pf);
		break;
	case OT_INT8:
	{
		NDUINT8 a = (NDUINT8)indata->i_val;
		_stream_func(&a, 1, sizeof(a), pf);
	}
		break;
	case OT_INT16:
	{
		NDUINT16 a = (NDUINT16)indata->i_val;
		_tryto_change_order_short(a, changeByteOrder);
		_stream_func(&a, 1, sizeof(a), pf);
	}
		break;
	case OT_TIME:
	case OT_INT64:
	{
		NDUINT64 a = indata->i64_val;
		_tryto_change_order_long64(a, changeByteOrder);
		_stream_func(&a, 1, sizeof(a), pf);
	}
		break;
	case OT_STRING:
		if (!indata->str_val || 0 == indata->str_val[0])	{
			size = 0;
			_stream_func(&size, sizeof(size), 1, pf);
		}
		else {
			size = (NDUINT16)strlen(indata->str_val);

			length = size;
			_tryto_change_order_short(size, changeByteOrder);
			_stream_func(&size, 1, sizeof(size), pf);
			_stream_func(indata->str_val, 1, (int)length, pf);
		}
		break;

	case OT_ORG_STREAM:
	case OT_ATTR_DATA:
	case OT_BINARY_DATA :
		if (!indata->_bin || 0==indata->_bin->size) {
			size = 0;
			_stream_func(&size, sizeof(size), 1, pf);
		}
		else {
			size = (NDUINT16)indata->_bin->size;

			length = size;
			_tryto_change_order_short(size, changeByteOrder);

			_stream_func(&size, 1, sizeof(size), pf);
			_stream_func(indata->_bin->data, 1, (int)length, pf);
		}
		break;
	case OT_ARRAY:
		if (!indata->_i_arr) {
			size = 0;
			_stream_func(&size, 1, sizeof(size), pf);
			break;
		}
		size = (NDUINT16)indata->_i_arr->number;
		length = size;
		_tryto_change_order_short(size, changeByteOrder);
		_stream_func(&size, 1, sizeof(size), pf);
		if (sub_etype == OT_STRING){
			for (int i = 0; i < indata->_str_arr->number; i++) {
				if (indata->_str_arr->data[i] && indata->_str_arr->data[i][0])	{
					size = (NDUINT16)strlen(indata->_str_arr->data[i]);
					length = size;
					_tryto_change_order_short(size, changeByteOrder);
					_stream_func(&size, 1, sizeof(size), pf);
					_stream_func(indata->_str_arr->data[i], 1, (int)length, pf);
				}
				else {
					size = 0;
					_stream_func(&size, 1, sizeof(size), pf);
				}
			}
		}
		else if (sub_etype == OT_FLOAT) {
			_stream_func(indata->_f_arr->data, 1, (int)(sizeof(float)*length), pf);
		}
		else if (sub_etype == OT_USER_DEFINED) {

		}
		else if (sub_etype == OT_INT64) {
			for (size_t i = 0; i < length; i++)	{
				NDUINT64 a = indata->_int64_arr->data[i];
				_tryto_change_order_long64(a, changeByteOrder);
				_stream_func(&a, 1, sizeof(a), pf);
			}
		}
		else {
			for (size_t i = 0; i < length; i++)	{
				NDUINT32 a = indata->_i_arr->data[i];
				_tryto_change_order_long(a, changeByteOrder);
				_stream_func(&a, 1, sizeof(a), pf);
			}
		}
		break;
	default:
			break;
	}
	return 0;
#undef _stream_func
}

//read from file 
template <class TStreamRW, class TStreamHandle>
int dbl_read_from_binStream(dbl_element_base *indata, DBL_ELEMENT_TYPE etype, DBL_ELEMENT_TYPE sub_etype, TStreamHandle &stream_h, TStreamRW& rf, bool changeByteOrder )
{
#define _stream_read(_addr, _s, _c, _stream) rf(_addr, _s, _c,stream_h)
#define CHECK_RETVAL(ret)	if(ret <= 0)	return -1
	int ret;
	NDUINT16 size;
	indata->_data = 0;
	indata->isInit = false;
	switch (etype){
	case OT_BOOL:
	case OT_INT:
		ret = (int)_stream_read(&(indata->i_val), 1, sizeof(int), pf);
		CHECK_RETVAL(ret);

		_tryto_change_order_long((NDUINT32&)(indata->i_val), changeByteOrder);
		break;
	case OT_FLOAT:
		ret = (int)_stream_read(&(indata->f_val), 1, sizeof(float), pf);
		CHECK_RETVAL(ret);
		//nd_assert(ret == sizeof(float));
		break;
	case OT_INT8:
	{
		NDUINT8 a = 0;
		ret = (int)_stream_read(&a, 1, sizeof(a), pf);
		CHECK_RETVAL(ret);
		indata->i_val = a;
	}
		break;
	case OT_INT16:
	{
		NDUINT16 a = 0;
		ret = (int)_stream_read(&a, 1, sizeof(a), pf);
		CHECK_RETVAL(ret);

		_tryto_change_order_short(a, changeByteOrder);
		indata->i_val = a;
	}
		break;
	case  OT_TIME:
	case OT_INT64:
	{
		NDUINT64 a = 0;
		ret = (int)_stream_read(&a, 1, sizeof(a), pf);
		CHECK_RETVAL(ret);

		_tryto_change_order_long64(a, changeByteOrder);
		indata->i64_val = a;
	}
		break;

	case OT_STRING:
		size = 0;
		indata->str_val = 0;
		ret = (int)_stream_read(&size, 1, sizeof(size), pf);
		CHECK_RETVAL(ret);
		_tryto_change_order_short(size, changeByteOrder);

		if (size > 0)	{
			indata->str_val = (char*)malloc(size + 1);
			_stream_read(indata->str_val, 1, size, pf);
			indata->str_val[size] = 0;
		}
		break;
	case OT_ORG_STREAM:
	case OT_ATTR_DATA:
	case OT_BINARY_DATA:
		size = 0;
		indata->_bin = 0;
		ret = (int)_stream_read(&size, 1, sizeof(size), pf);
		CHECK_RETVAL(ret);

		_tryto_change_order_short(size, changeByteOrder);
		if (size > 0)	{
			dbl_binary *parr = (dbl_binary *)malloc(sizeof(dbl_binary)+size);

			_stream_read(parr->data, 1, size, pf);
			parr->size = size;
			indata->_bin = parr;
		}
		
		break;
	case OT_ARRAY:
		ret = (int)_stream_read(&size, 1, sizeof(size), pf);
		CHECK_RETVAL(ret);

		_tryto_change_order_short(size, changeByteOrder);
		if (size == 0){
			indata->_i_arr = 0;
			break;
		}

		if (sub_etype == OT_STRING){

			dbl_strarray *parr = (dbl_strarray *)malloc(sizeof(dbl_strarray)+sizeof(char*)* (size - 1));
			nd_assert(parr);
			parr->number = size;

			for (int i = 0; i<parr->number; i++) {
				_stream_read(&size, 1, sizeof(size), pf);
				_tryto_change_order_short(size, changeByteOrder);
				if (size > 0) {
					parr->data[i] = (char*)malloc(size + 1);
					_stream_read(parr->data[i], 1, size, pf);
					parr->data[i][size] = 0;
				}
				else {
					parr->data[i] = 0;
				}
			}
			indata->_str_arr = parr;

		}
		else if (sub_etype == OT_FLOAT) {

			dbl_floatarray *parr = (dbl_floatarray *)malloc(sizeof(dbl_floatarray)+sizeof(float)* (size - 1));
			nd_assert(parr);
			parr->number = size;
			_stream_read(parr->data, 1, sizeof(float)* size, pf);
			indata->_f_arr = parr;
		}
		else if (sub_etype == OT_INT64) {
			dbl_int64array *parr = (dbl_int64array *)malloc(sizeof(dbl_int64array) + sizeof(NDUINT64)* (size - 1));
			nd_assert(parr);
			parr->number = size;

			for (int i = 0; i < size; i++){
				NDUINT64 a;
				_stream_read(&a, 1, sizeof(a), pf);
				_tryto_change_order_long64(a, changeByteOrder);
				parr->data[i] = a;
			}
			indata->_int64_arr = parr;
		}
		else {
			dbl_intarray *parr = (dbl_intarray *)malloc(sizeof(dbl_intarray)+sizeof(int)* (size - 1));
			nd_assert(parr);
			parr->number = size;

			for (int i = 0; i < size; i++){
				NDUINT32 a;
				_stream_read(&a, 1, sizeof(a), pf);
				_tryto_change_order_long(a, changeByteOrder);
				parr->data[i] = a;
			}
			//_stream_read(parr->data, 1, sizeof(int)* size, pf);
			indata->_i_arr = parr;
		}
		break;

	default:
		return -1;
	}
	indata->isInit = true;
	return 0;
}

int dbl_data_2streamfile(dbl_element_base *buf, DBL_ELEMENT_TYPE etype, DBL_ELEMENT_TYPE sub_etype, FILE*pf, dbl_stream_fwrite writefunc, bool changeByteOrder)
{
	return  _dbl_data_2binStream(buf, etype, sub_etype, pf, writefunc,changeByteOrder);
}
int dbl_read_streamfile(dbl_element_base *indata, DBL_ELEMENT_TYPE etype, DBL_ELEMENT_TYPE sub_etype, FILE*pf, dbl_stream_fread readfunc, bool changeByteOrder)
{
	return dbl_read_from_binStream(indata, etype, sub_etype, pf, readfunc,changeByteOrder);
}

static int _readbuf(void *data, size_t size, int count, char *&buf)
{
	size_t total = size * count;
	memcpy(data, buf, total);
	buf += total;
	return count;
}

static int _writebuf(void *data, size_t size, int count, char *&buf)
{
	size_t total = size * count;
	memcpy(buf, data, total);
	buf += total;
	return count;
}

int dbl_read_buffer(dbl_element_base *data, DBL_ELEMENT_TYPE etype, DBL_ELEMENT_TYPE sub_etype, char *buf, bool changeByteOrder)
{
	char *p = buf;
	int ret = dbl_read_from_binStream(data, etype, sub_etype, p, _readbuf, changeByteOrder);
	if (ret == 0) {
		return  (int)(p - buf);
	}
	return 0;
}

int dbl_write_buffer(dbl_element_base *data, DBL_ELEMENT_TYPE etype, DBL_ELEMENT_TYPE sub_etype, char *buf, bool changeByteOrder)
{

	char *p = buf;
	int ret = _dbl_data_2binStream(data, etype, sub_etype, p, _writebuf, changeByteOrder);
	if (ret == 0) {
		return  (int)(p - buf);
	}
	return 0;
}
int dbl_destroy_data(dbl_element_base *buf, DBL_ELEMENT_TYPE etype, DBL_ELEMENT_TYPE sub_etype)
{
#define SAVE_FREE(a) do { if(a) {free(a); (a)=0;}} while (0);

	switch (etype){
	case OT_ORG_STREAM:
	case OT_ATTR_DATA:
	case OT_BINARY_DATA:
	case OT_STRING:
		SAVE_FREE(buf->str_val);
		break;
	case OT_ARRAY:
		if (!buf->_data) {
			break;
		}
		if (sub_etype == OT_USER_DEFINED ) {
			for (int i = 0; i < buf->_arr_user->number; i++)	{
				if (buf->_arr_user->data[i])	{
					delete buf->_arr_user->data[i];
					buf->_arr_user->data[i]=0;
				}
			}
		}
		else if (sub_etype == OT_STRING ) {
			for (int i = 0; i < buf->_str_arr->number; i++)	{
				if (buf->_str_arr->data[i])	{
					SAVE_FREE(buf->_str_arr->data[i]);
				}
			}
		}

		SAVE_FREE(buf->_data);
		break;
	case OT_USER_DEFINED:
		if (buf->_data) {
			delete buf->_userDef ;
		}
		break;
	default:
			break ;
	}
	return 0;
#undef SAVE_FREE
}

int dbl_data_copy(const dbl_element_base *input, dbl_element_base *output, DBL_ELEMENT_TYPE etype, DBL_ELEMENT_TYPE sub_etype)
{
	output->isInit = input->isInit;
	switch (etype){
	
	case OT_STRING:
	{
		if (!input->str_val || !*(input->str_val)) {
			break;
		}
		size_t size = strlen(input->str_val);
		output->str_val = (char*)malloc(size + 2);
		strncpy(output->str_val, input->str_val, size + 1);
		break;
	}

	case OT_ORG_STREAM:
	case OT_ATTR_DATA:
	case OT_BINARY_DATA:
		if (input->_bin && input->_bin->size) {
			dbl_binary *src = input->_bin;
			dbl_binary *parr = (dbl_binary *)malloc(sizeof(dbl_binary)+src->size);
			memcpy(parr->data, src->data, src->size);
			parr->size = src->size;
			output->_bin = parr;
		}
		break;
	case OT_ARRAY:
		if (!input->_data) {
			break;
		}
		if (sub_etype == OT_INT || sub_etype == OT_INT8 || sub_etype == OT_INT16 || sub_etype == OT_BOOL) {
			if (input->_i_arr->number) {
				_int_array_init(input->_i_arr->data, (int)input->_i_arr->number, output);
			}
		}
		else if (sub_etype == OT_FLOAT) {
			if (input->_f_arr->number) {
				_float_array_init(input->_f_arr->data, (int)input->_f_arr->number, output);
			}
		}
		else if (sub_etype == OT_INT64) {
			if (input->_int64_arr->number) {
				_int64_array_init(input->_int64_arr->data, (int)input->_int64_arr->number, output);
			}
		}
		else if (sub_etype == OT_STRING  ) {
			_str_array_init((const char**)input->_str_arr->data, (int)input->_str_arr->number, output);
		}
		else if (sub_etype == OT_USER_DEFINED) {
			_userDef_array_init((const LogicUserDefStruct **)input->_arr_user->data, (int)input->_arr_user->number, output);

		}

		break;

	case OT_USER_DEFINED :
		if (input->_data)	{
			LogicUserDefStruct *p = (LogicUserDefStruct *)input->_data;
			output->_data = new LogicUserDefStruct(*p);
		}
		break; 
	default:
		output->i64_val = input->i64_val;
		return -1;
	}
	return 0;
}

bool _check_array(const char *src)
{
	int left = 0, right = 0;

	src = ndstr_first_valid(src);
	if (*src != _ARRAR_BEGIN_MARK)	{
		return false;
	}

	while (*src){
		char a = *src++;
		if (a == _ARRAR_BEGIN_MARK)	{
			++left;
		}
		else if (a == _ARRAR_END_MARK) {
			++right;
		}
	}
	if (left == 0){
		return false;
	}
	return left == right;
}

int dbl_build_from_text(dbl_element_base *buf, const char *in_data, DBL_ELEMENT_TYPE etype, DBL_ELEMENT_TYPE sub_etype)
{
	buf->_data = 0;
	in_data = ndstr_first_valid(in_data);
	buf->isInit = false;
	if (!in_data){
		return 0;
	}
	switch (etype){
	case OT_INT:
		buf->i_val =(int) ndstr_atoi_hex(in_data);
		break;
	case OT_FLOAT:
		buf->f_val = (float)atof(in_data);
		break;

	case OT_STRING:
	{
		NDUINT16 size = (NDUINT16)strlen(in_data);
		buf->str_val = (char*)malloc(size + 2);
		strncpy(buf->str_val, in_data, size + 1);
		break;
	}
		break;
	case OT_BOOL:
		buf->i_val = (int)ndstr_atoi_hex(in_data);
		break;

	case OT_ARRAY:
		if (!_check_array(in_data) ){
			return -1;
		}
		if (sub_etype == OT_STRING){
			const char *p = (char*)strchr(in_data, _ARRAR_BEGIN_MARK);
			nd_assert(p);
			++p;
		
			char *myarr[1024];
			int _num = 0;
			while (p && *p && *p != _ARRAR_END_MARK){
				size_t _array_size = 0;
				char _tmpbuf[4096];
				char *dest = _tmpbuf ;
				_tmpbuf[0] = 0;
				int markNum = 0 ;
				while (*p) {
					
					if (*p == ',')	{
						if (markNum == 0)	{
							++p;
							break;
						}
					}
					if (*p == _ARRAR_BEGIN_MARK) {
						++markNum ;
					}
					else if(*p == _ARRAR_END_MARK) {
						if (markNum==0)	{
							break;
						}
						--markNum ;
					}					
					_array_size += ndstr_read_utf8char( (char**) &p,  &dest) ;
					
				}
				if (_tmpbuf[0] && _array_size > 0)	{
					_tmpbuf[_array_size++] = 0;

					myarr[_num] = (char*)malloc(_array_size + 1);
					strncpy(myarr[_num], _tmpbuf, _array_size+1);
					++_num;
					if (_num >= 1024)
						break;
				}

			}
			if (_num > 0) {
				size_t mem_len = sizeof(dbl_strarray) + (_num - 1) * sizeof(char*);
				dbl_strarray *pstrarr = (dbl_strarray *) malloc(mem_len);
				for (int i = 0; i < _num; i++)	{
					pstrarr->data[i] = myarr[i];
				}
				pstrarr->number = _num;
				buf->_data = pstrarr;
			}
			break;

		}
		else {
			char *p = (char*)strchr(in_data, _ARRAR_BEGIN_MARK);
			char *pend = strchr(p, _ARRAR_END_MARK);
			if (!p || !pend) {
				return -1;
			}
			++p;
			struct {
				dbl_intarray hdr;
				char _buff[4096];
			} tmp_buf;
			dbl_intarray *tmparr = &tmp_buf.hdr;
			tmparr->number = 0;
			while (p && p < pend){
				if (sub_etype == OT_INT) {
					tmparr->data[tmparr->number++] = (int)strtol(p, &p, 0);
				}
				else {
					((dbl_floatarray*)tmparr)->data[tmparr->number++] = (float)strtof(p, &p);
				}

				if (p && *p && *p != _ARRAR_END_MARK) {
					p = strchr(p, ',');
					if (p && *p == ',') {
						++p;
					}
				}
			}
			if (tmparr->number) {
				size_t size;
				if (sub_etype == OT_INT) {
					size = sizeof(dbl_intarray) + (tmparr->number - 1) * sizeof(int);
				}
				else {
					size = sizeof(dbl_intarray) + (tmparr->number - 1) * sizeof(float);

				}

				buf->_data = malloc(size);
				memcpy(buf->_data, tmparr, size);
			}
		} //end int float
		break;
	
	default:
		buf->_data = 0;
		return -1;
	}
	buf->isInit = true;
	return 0;
}
