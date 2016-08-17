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
#include <stdarg.h>


static bool _int_array_init(int *arr, int size, dbl_element_base * eledata)
{
	size_t mem_len = sizeof(dbl_intarray) + (size - 1) * sizeof(int);
	eledata->_data = malloc(mem_len);
	if (!eledata->_data){
		return false;
	}

	for (int i = 0; i < size; i++) {
		eledata->_i_arr->data[i] = arr[i];
	}
	eledata->_i_arr->number = size;
	return true;
}

static bool _float_array_init(float *arr, int size, dbl_element_base * eledata)
{
	size_t mem_len = sizeof(dbl_intarray) + (size - 1) * sizeof(float);
	eledata->_data = malloc(mem_len);
	if (!eledata->_data){
		return false;
	}

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

	for (int i = 0; i < size; i++) {
		size_t len = strlen(arr[i]);
		if (len > 0)	{
			eledata->_str_arr->data[i] = (char*)malloc(len + 1);
			if (eledata->_str_arr->data[i])
				strncpy(eledata->_str_arr->data[i], arr[i], len+1);
		}
		else {
			eledata->_str_arr->data[i] = NULL;
		}
	}
	eledata->_str_arr->number = size;
	return true;
}
static bool _check_array(const char *src);
//////////////////////////////////////////////////////////////////////////
DBLDataNode::DBLDataNode(dbl_element_base *data, DBL_ELEMENT_TYPE etype, DBL_ELEMENT_TYPE sub_etype) :
m_ele_type(etype), m_sub_type(sub_etype), m_data(data), m_dataOwner(false)
{

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
			if (m_sub_type == OT_INT)	{
				int ret = GetarrayInt(i) - r.GetarrayInt(i);
				if (0==ret)	{
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
// 		
// 	case OT_ARRAY_2D :
// 		if (Get2DArrSize() != ((DBLDataNode&)r).Get2DArrSize())	{
// 			return false;
// 		}
// 		if (m_sub_type == OT_INT)	{
// 			for (int i = 0; i < Get2DArrSize(); i++)	{
// 				if (Get2DSubArrsize(i) != ((DBLDataNode&)r).Get2DSubArrsize(i))	{
// 					return false;
// 				}
// 				for (int x = 0; x < Get2DSubArrsize(i); x++)	{
// 					if (Get2DArrayInt(i, x) != ((DBLDataNode &)r).Get2DArrayInt(i, x))	{
// 						return false;
// 					}
// 				}
// 			}
// 			return true;
// 		}
// 
// 		break;
	//case OT_OBJECT :
	//	return m_data->_data == m_data->_data;
	//	break;
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

	if (r.m_dataOwner) {
		m_dataOwner = true;
		m_data = &m_dataOwn;
		dbl_data_copy(r.m_data, m_data, (DBL_ELEMENT_TYPE)m_ele_type, (DBL_ELEMENT_TYPE)m_sub_type);
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
	m_data = &m_dataOwn;

	size_t s = strlen(str1) + 1;
	m_data->str_val = (char*)malloc(s);
	strncpy(m_data->str_val, str1, s);

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

void DBLDataNode::InitSet(void *binary, size_t size, DBL_ELEMENT_TYPE eleType)
{

	Destroy();
	m_ele_type = eleType;

	m_dataOwner = true;
	m_data = &m_dataOwn;
	m_data->_data = NULL;
	dbl_binary *parr = (dbl_binary *)malloc(sizeof(dbl_binary)+size);

	if (parr){
		memcpy(parr->data, binary, size);
		parr->size = size;
		m_data->_bin = parr;
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
		paddr->number = size;
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
		paddr->number = size;
		m_dataOwn._data = paddr;
	}
	else if (OT_STRING == m_sub_type || OT_USER_DEFINED == m_sub_type) {
		size_t mem_len = sizeof(dbl_strarray) + (size - 1) * sizeof(char*);
		dbl_strarray *paddr = (dbl_strarray*)malloc(mem_len);
		if (!paddr){
			return false;
		}

		for (int i = 0; i < size; i++) {
			paddr->data[i] = 0;
		}
		paddr->number = size;
		m_dataOwn._data = paddr;
	}
	m_dataOwner = true;
	m_data = &m_dataOwn;
	return true;
}

bool DBLDataNode::SetArray(const DBLDataNode &data, int index)
{
	dbl_intarray *paddr = (dbl_intarray *)m_data->_data;
	if (!paddr || paddr->number <= index){
		return false;
	}

	if (m_sub_type == OT_INT || m_sub_type == OT_BOOL || m_sub_type == OT_INT8 || m_sub_type == OT_INT16)	{		
		paddr->data[index] = data.GetInt();

	}
	else if (OT_FLOAT == m_sub_type){
		dbl_floatarray *paddr = (dbl_floatarray *)m_data->_data;
		paddr->data[index] = data.GetFloat();

	}
	else if (OT_STRING == m_sub_type){
		dbl_strarray *paddr = (dbl_strarray *)m_data->_data;
		if (paddr->data[index])	{
			free(paddr->data[index]);
		}
		size_t size = strlen(data.GetText()) + 1;
		if (size <= 1)	{
			return true;
		}
		paddr->data[index] = (char*)malloc(size);
		strncpy(paddr->data[index], data.GetText(), size);

	}
	if (OT_USER_DEFINED == m_sub_type) {
		dbl_userDefArray *pU = (dbl_userDefArray *)m_data->_data;
		const LogicUserDefStruct *puserData = data.getUserDef();
		if (puserData)		{

			pU->data[index] = new LogicUserDefStruct(*puserData);
		}


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

DBLDataNode DBLDataNode::getUserDefMember(const char *name)
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

int DBLDataNode::ReadStream(const char *streamBuf, int streamByteOrder)
{
	Destroy();
	int read_len = 2;
	m_dataOwn.isInit = false;
	//NDUINT16 type = *((*(NDUINT16**)&streamBuf)++);
	NDUINT16 type;
	streamBuf = lp_read_stream((lp_stream_t)streamBuf, type , streamByteOrder);
	//if (ND_L_ENDIAN != streamByteOrder) {
	//	type = nd_order_change_short(type);
	//}
	

	if (type == OT_ARRAY ) {
		//m_sub_type =(NDUINT8) *((*(NDUINT16**)&streamBuf)++); 

		NDUINT16 subtype;
		streamBuf = lp_read_stream((lp_stream_t)streamBuf, subtype, streamByteOrder);

		m_sub_type = subtype;
		read_len += 2;

	}
	m_ele_type = (NDUINT8) type;

	int ret = dbl_read_buffer(&m_dataOwn, (DBL_ELEMENT_TYPE)m_ele_type, (DBL_ELEMENT_TYPE)m_sub_type, (char *)streamBuf, nd_byte_order() != streamByteOrder);
	if (ret > 0){
		m_data = &m_dataOwn;
		m_dataOwner = true;
		m_dataOwn.isInit = true;
		return ret + read_len;
	}
	return 0;
}
int DBLDataNode::WriteStream(char *streamBuf, int streamByteOrder)const
{
	int write_len = 2;
	//*((*(NDUINT16**)&streamBuf)++) = m_ele_type;
	streamBuf = lp_write_stream(streamBuf, (NDUINT16)m_ele_type, streamByteOrder);
	
	if (m_ele_type == OT_ARRAY ) {
		//m_sub_type = OT_INT;
		//*((*(NDUINT16**)&streamBuf)++) = m_sub_type;
		streamBuf = lp_write_stream(streamBuf, (NDUINT16)m_sub_type, streamByteOrder);
		write_len += 2;
	}


	int len = dbl_write_buffer(m_data, (DBL_ELEMENT_TYPE)m_ele_type, (DBL_ELEMENT_TYPE)m_sub_type, (char *)streamBuf, nd_byte_order() != streamByteOrder);
	if (len > 0){
		return len + write_len;
	}
	return 0;
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
			return atoi(pText);
		}
	}
	return 0;
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
			if (0 == ndstricmp(m_data->str_val, "yes") || 0 == ndstricmp(m_data->str_val, "true")) {
				return true; 
			}
		}
		return false;
		//return atoi(m_data->str_val) ? true : false;
	}
	return 0;
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
	const char *p = GetText();
	if (!p) {
		return std::string("");
	}
	return std::string(p);
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

DBLDataNode DBLDataNode::GetArray(int index)
{
	if (m_ele_type != OT_ARRAY){
		return *this;
	}
	if (m_sub_type == OT_INT || m_sub_type == OT_BOOL || m_sub_type == OT_INT8 || m_sub_type == OT_INT16 || OT_INT64 == m_sub_type)	{
		return DBLDataNode(GetarrayInt(index), m_sub_type);
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
		if (OT_INT == m_sub_type || OT_BOOL == m_sub_type)	{
			return m_data->_i_arr->data[index];
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
					return atoi(pText);
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
		if (OT_INT == m_sub_type || OT_BOOL == m_sub_type)	{
			return m_data->_i_arr->data[index] ? true : false;
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
					return atoi(pText) ? true : false;
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
		if (OT_INT == m_sub_type || OT_BOOL == m_sub_type)	{
			return(float)m_data->_i_arr->data[index];
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

// 
// bool DBLDataNode::PushBack(LogicUserDefStruct &u)
// {
// 	dbl_userDefArray *pU = (dbl_userDefArray *)m_data->_data;
// 	for (size_t i = 0; i < pU->number; i++)	{
// 		if (!pU->data[i]){
// 			pU->data[i] = new LogicUserDefStruct(u);
// 			return true;
// 		}
// 	}
// 	return false;
// }
const LogicUserDefStruct *DBLDataNode::GetarrayUser(int index) const
{
	if (m_ele_type == OT_USER_DEFINED)	{
		return m_data->_userDef;
	}
	else if (m_ele_type == OT_ARRAY && m_sub_type == OT_USER_DEFINED) {
		dbl_userDefArray *pU = m_data->_arr_user;
		if (pU)	{
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
		std::string str1 = GetText();
		str1 += r.GetText();

		return DBLDataNode(str1.c_str());
	}
	default:
		break;
	}
	return DBLDataNode();
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
		int rval = atoi(text);
		int val = GetInt() +rval;
		return DBLDataNode(val);
	}
	case OT_TIME:
	case OT_INT64:
	{
		int rval = atoi(text);
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
	va_end(arg);

	pBuf->addr += done;
	pBuf->bufsize -= done;
	
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
		nd_utf8_to_gbk(text, pBuf, size);
		InitSet(pBuf);
	}
	else if (to == E_SRC_CODE_UTF_8) {
		nd_gbk_to_utf8(text, pBuf, size);
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
		ret = print_func(pf, "%d", GetInt());
		break; 
	case  OT_INT64:
		ret = print_func(pf, "%lld", GetInt64());
		break;
	case OT_TIME:
	{
		char buf[128];
		nd_get_datetimestr_ex((time_t)GetInt64(), buf, 128);
		ret = print_func(pf, "%s",buf);
	}
		break;
	case OT_STRING:
		ret = print_func(pf, "%s", GetText());
		break;
	case  OT_BOOL:
		ret = print_func(pf, "%s", GetInt() ? "true" : "false");
		break;
	case  OT_FLOAT:
		ret = print_func(pf, "%f", GetFloat());
		break;
	case  OT_ARRAY:
		ret += print_func(pf, "%c", _ARRAR_BEGIN_MARK);
		for (int x = 0; x < GetArraySize(); x++) {
			if (m_sub_type==OT_INT) {
				if (x<GetArraySize()-1){
					ret += print_func(pf, "%d,", GetarrayInt(x));
				}
				else {
					ret += print_func(pf, "%d", GetarrayInt(x));
				}
			}
			else if(m_sub_type == OT_FLOAT) {
				if (x<GetArraySize()-1){
					ret += print_func(pf, "%f,", GetarrayFloat(x));
				}
				else {
					ret += print_func(pf, "%f", GetarrayFloat(x));
				}
			}
			else if(m_sub_type == OT_STRING) {
				if (x < GetArraySize() - 1){
					ret += print_func(pf, "%s,", GetarrayText(x));
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
				if (x < GetArraySize() - 1){
					ret += print_func(pf, ",\n");
				}
			}


		}
		ret += print_func(pf, "%c", _ARRAR_END_MARK);
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
		dbl_destroy_data(m_data, (DBL_ELEMENT_TYPE)m_ele_type, (DBL_ELEMENT_TYPE)m_sub_type);
		
	}
	m_dataOwn._data = 0;
	m_ele_type = 0;
	m_sub_type = 0;
	m_data = 0;
	m_dataOwner = false;

	m_dataOwn.isInit = true;
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
			_stream_func(indata->str_val, 1, length, pf);
		}
		break;

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
			_stream_func(indata->_bin->data, 1, length, pf);
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
					_stream_func(indata->_str_arr->data[i], 1, length, pf);
				}
				else {
					size = 0;
					_stream_func(&size, 1, sizeof(size), pf);
				}
			}
		}
		else if (sub_etype == OT_FLOAT) {
			_stream_func(indata->_f_arr->data, 1, sizeof(float)*length, pf);
		}
		else {
			for (size_t i = 0; i < length; i++)	{
				NDUINT32 a = indata->_i_arr->data[i];
				_tryto_change_order_long(a, changeByteOrder);
				_stream_func(&a, 1, sizeof(a), pf);
			}
		}
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


int dbl_data_2streamfile(dbl_element_base *buf, DBL_ELEMENT_TYPE etype, DBL_ELEMENT_TYPE sub_etype, FILE*pf, bool changeByteOrder )
{
	return  _dbl_data_2binStream(buf, etype, sub_etype, pf, fwrite,changeByteOrder);
}
int dbl_read_streamfile(dbl_element_base *indata, DBL_ELEMENT_TYPE etype, DBL_ELEMENT_TYPE sub_etype, FILE*pf, bool changeByteOrder )
{
	return dbl_read_from_binStream(indata, etype, sub_etype, pf, fread,changeByteOrder);
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
			
	}
	return 0;
#undef SAVE_FREE
}

int dbl_data_copy(dbl_element_base *input, dbl_element_base *output, DBL_ELEMENT_TYPE etype, DBL_ELEMENT_TYPE sub_etype)
{
	output->isInit = input->isInit;
	switch (etype){
	
	case OT_STRING:
	{
		size_t size = strlen(input->str_val);
		output->str_val = (char*)malloc(size + 2);
		strncpy(output->str_val, input->str_val, size + 1);
		break;
	}

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
		if (sub_etype == OT_INT) {
			if (input->_i_arr->number) {
				size_t size = sizeof(dbl_intarray) + (input->_i_arr->number - 1) * sizeof(int);
				output->_data = malloc(size);
				memcpy(output->_data, input->_data, size);
			}
		}
		else if (sub_etype == OT_FLOAT) {
			if (input->_f_arr->number) {
				size_t size = sizeof(dbl_floatarray) + (input->_i_arr->number - 1) * sizeof(float);
				output->_data = malloc(size);
				memcpy(output->_data, input->_data, size);
			}
		}
		else if (sub_etype == OT_STRING  ) {
			_str_array_init((const char**)input->_str_arr->data, input->_str_arr->number, output);
		}
		else if (sub_etype == OT_USER_DEFINED) {
			size_t mem_len = sizeof(dbl_userDefArray) + (input->_arr_user->number - 1) * sizeof(char*);
			dbl_userDefArray *paddr = (dbl_userDefArray*)malloc(mem_len);
			if (!paddr){
				return false;
			}

			for (int i = 0; i < input->_arr_user->number; i++) {
				if (input->_arr_user->data[i]) {
					paddr->data[i] = new LogicUserDefStruct(*(input->_arr_user->data[i]) );
				}				
			}
			paddr->number = input->_arr_user->number;
			output->_arr_user = paddr;
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
		buf->i_val = atoi(in_data);
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
		buf->i_val = atoi(in_data);
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
