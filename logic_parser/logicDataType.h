/* file logicDataType.h
 *
 * create by duan 
 * 2015-5-8
 *
 */

#ifndef _LOGIC_DATA_TYPE_H_ 
#define _LOGIC_DATA_TYPE_H_

#include "nd_common/nd_common.h"
#include <string>
#include <vector>
#include <map>

#define _ARRAR_BEGIN_MARK '['
#define _ARRAR_END_MARK ']'

class LogicUserDefStruct;

typedef int(*logic_print)(void *pf, const char *stm, ...);

enum DBL_ELEMENT_TYPE
{
	OT_INT = 0x0,
	OT_FLOAT = 0x1,
	OT_STRING = 0x2,
	OT_INT8 = 0x3,
	OT_INT16 = 0x4 ,
	OT_INT64 = 0x5 ,
	OT_BOOL = 0x6,

	OT_ARRAY = 0x7, // 数组     {array_value_type=TEXT|a;b;c}
	//OT_ARRAY_2D = 0x8, // 数组     {array_value_type=TEXT|a;b;c}
	OT_TIME =0x8,
	OT_VARIABLE =0x9,  //variant in game-logic-vm
	OT_PARAM = 0xa ,	// argment/param in call game-logic-script
	OT_LAST_RET = 0xb,	//last step return value 
	OT_USER_DEFINED =0xc ,
	
	OT_COMPILE_TIME=0xd, //compile time 
	OT_FUNCTION_NAME = 0xe, //function name 
	OT_SCRIPT_MODULE_NAME = 0xf, //function name
	OT_BINARY_DATA = 0x10,		//binary-data
	OT_USER_DEFINED_ARRAY =0X11,

	OT_OBJECT_VOID = 0x20,		//game object , c_address of object
	OT_OBJ_MSGSTREAM =0x21,		//input message stream
	OT_OBJ_BASE_OBJ= 0x22,
	OT_OBJ_NDHANDLE = 0x23,
	OT_OBJ_NDOBJECT = 0x24,
	OT_OBJ_OUT_MSGSTREAM = 0x25,	//output message
	OT_ATTR_DATA = 0x26 , //role attribute data
};

#pragma pack(push, 4)
//整形数组/bool
struct dbl_intarray
{
	size_t number;
	size_t capacity;
	int data[1];
};
struct dbl_binary
{
	size_t size;
	size_t capacity;
	char data[1];
};
//浮点数组
struct dbl_floatarray
{
	size_t number;
	size_t capacity;
	float data[1];
};
//文本数组
struct dbl_strarray
{
	size_t number;
	size_t capacity;
	char* data[1];
};
struct  dbl_userDefArray
{
	size_t number;
	size_t capacity;
	LogicUserDefStruct *data[1];
};

// enum { ARRAY_SIZE_1D = 32 };
// struct _dbl_intarr_1d
// {
// 	size_t number;
// 	int data[ARRAY_SIZE_1D];
// };

// struct dbl_intarray2d
// {
// 	size_t number;
// 	_dbl_intarr_1d data[1];
// };
// struct _dbl_float_1d
// {
// 	size_t number;
// 	float data[ARRAY_SIZE_1D];
// };
// 
// struct dbl_float2d
// {
// 	size_t number;
// 	_dbl_float_1d data[1];
// };

// struct _dbl_string_1d
// {
// 	size_t number;
// 	char* data[ARRAY_SIZE_1D];
// };
// struct dbl_string2d
// {
// 	size_t number;
// 	_dbl_string_1d data[1];
// };

struct _attrDataBin {
	int count;
	struct {
		NDUINT8 aid;
		float val;
	}datas[100];
};
struct dbl_attr {
	size_t number;
	_attrDataBin attr;
};

struct dbl_element_base
{
	bool isInit;
	union {
		int i_val;
		float f_val;
		NDUINT64 i64_val;
		char *str_val;
		LogicUserDefStruct *_userDef;
		void *_data;
		dbl_intarray *_i_arr;
		dbl_floatarray *_f_arr;
		dbl_strarray *_str_arr;
		//dbl_intarray2d *_arr_2d;
		//dbl_float2d *_arr_float_2d;
		dbl_binary *_bin;
		dbl_userDefArray *_arr_user;
		dbl_attr *_attr_data;
	};
	
};

#pragma pack(pop)


//数据单元
class DBLDataNode
{
public:
	DBLDataNode(dbl_element_base *data, DBL_ELEMENT_TYPE etype, DBL_ELEMENT_TYPE sub_etype);
	DBLDataNode(const DBLDataNode &r);
	DBLDataNode();
	~DBLDataNode();
	//init
	//init set vale
	DBLDataNode(int a, int type=OT_INT) {init() ;InitSet(a,type);}
	DBLDataNode(bool a) {init() ;InitSet(a);}
	DBLDataNode(float a) {init() ;InitSet(a);}
	DBLDataNode(char *str1) {init() ;InitSet(str1);}
	DBLDataNode(const char *str1)  {init() ;InitSet(str1);}
	DBLDataNode(int *arr, int size) {init() ;InitSet(arr, size);}
	DBLDataNode(float *arr, int size) {init() ;InitSet(arr, size);}
	DBLDataNode(const char *arr[], int size) { init(); InitSet(arr, size); }

	DBLDataNode(void *object, DBL_ELEMENT_TYPE type = OT_OBJECT_VOID)  {init() ;InitSet(object, type);}
	DBLDataNode(void *binary, size_t size, DBL_ELEMENT_TYPE eleType = OT_BINARY_DATA) {init() ;InitSet(binary,size,eleType);}
	DBLDataNode(NDUINT64 a) {init() ;InitSet(a);}
	DBLDataNode(NDUINT16 a) {init() ;InitSet(a);}
	DBLDataNode(NDUINT8 a) {init() ;InitSet(a);}
	DBLDataNode(time_t t) {init() ;InitSet(t);}
	DBLDataNode(const LogicUserDefStruct &u) {init() ;InitSet(u);}
	
	static DBL_ELEMENT_TYPE getTypeFromName(const char *typeName);
	
	DBLDataNode &operator =(const DBLDataNode &r);
	bool operator == (const DBLDataNode &r) const;
	bool operator != (const DBLDataNode &r) const;
	bool operator < (const DBLDataNode &r) const ;

	//init set vale
	void InitSet(int a, int type= OT_INT);
	void InitSet(bool a);
	void InitSet(float a);
	void InitSet(char *str1);
	void InitSet(const char *str1);
	void InitSet(int *arr, int size);
	void InitSet(float *arr, int size);
	void InitSet(const char *arr[], int size);
	void InitSet(const LogicUserDefStruct *arr[], int size);
	void InitSet(void *object, DBL_ELEMENT_TYPE type = OT_OBJECT_VOID);
	void InitSet(void *binary, size_t size, DBL_ELEMENT_TYPE eleType = OT_BINARY_DATA);
	void InitSet(NDUINT64 a);
	void InitSet(NDUINT16 a);
	void InitSet(NDUINT8 a);
	void InitSet(time_t t);
	void InitSet(const LogicUserDefStruct &u);
	bool InitReservedArray(size_t size, int attay_type = OT_INT);
	bool SetArray(const DBLDataNode &data, int index);	
	bool pushArray(const DBLDataNode &data);
	void InitFromTxt(const char *valText);
	

	bool GetVal(NDUINT64 &a) const ;
	bool GetVal(NDUINT16 &a)const;
	bool GetVal(NDUINT8 &a)const;
	bool GetVal(time_t &t)const;
	bool GetVal(float &a)const;
	bool GetVal(int &a)const;
	bool GetVal(bool &a)const;
	bool GetVal(char *buf, size_t size)const;
	bool GetVal(int *arr, size_t &size)const;
	bool GetVal(float *arr, size_t &size)const;

	int ReadStream(const char *streamBuf, size_t data_len, int streamByteOrder);
	int WriteStream(char *streamBuf, size_t buf_size, int streamByteOrder )const;
	int GetInt() const;
	int GetRoundInt() const;
	NDUINT64 GetInt64() const;
	bool GetBool() const;
	float GetFloat() const;
	const char *GetText() const;
	std::string GetString() const;
	void *GetObject() const;
	void *GetBinary() const;
	size_t GetBinarySize() const;

	DBLDataNode GetArray(int index)const;
	int GetarrayInt(int index) const;
	bool GetarrayBool(int index) const;
	float GetarrayFloat(int index) const;
	const char *GetarrayText(int index) const;
	const LogicUserDefStruct *GetarrayUser(int index) const;

	bool toArray(DBLDataNode &outVal);		//convert this(string) to array
	bool StringToArrayInt(const char *text); 
	bool StringToArrayFloat(const char *text);
	bool StringToArrayString(const char *text);
		
	time_t GetTime() const;
	int GetTimeYear()const;
	int GetTimeMonth()const;
	int GetTimeDay()const;
	int GetTimeHour()const;
	int GetTimeMinute()const;
	int GetTimeSecond()const;
	int GetTimeWeekDay()const;
	
	DBLDataNode getUserDefMember(const char *name) const;
	void setUserDefMember(const char *name,const DBLDataNode &val) ;
	const LogicUserDefStruct *getUserDef() const;

	int GetArraySize() const;

	DBLDataNode  operator+(const DBLDataNode &r) const;
	DBLDataNode  operator-(const DBLDataNode &r) const;
	DBLDataNode  operator*(const DBLDataNode &r) const;
	DBLDataNode  operator/(const DBLDataNode &r) const;

	DBLDataNode  operator+(const char *text) const ;

	DBLDataNode & operator+=(const DBLDataNode &r);
	DBLDataNode & operator+=(const char *text);

	DBLDataNode operator[](int n);
	
	bool TransferType(DBL_ELEMENT_TYPE aimType);
	bool CheckValid() const;
	bool CheckArray(int index) const;

	void ConvertEncode(int from, int to);
	int Print(logic_print, void *pf) const;
	int Sprint(char *buf, size_t size) const;

	DBL_ELEMENT_TYPE GetDataType() const;
	DBL_ELEMENT_TYPE GetArrayType() const;

	static bool setOutHex(bool isHex = true);
	static bool setOutLua(bool isLua);

// 	DBLDataNode unsafeRefArray(int index) ;
// 	DBLDataNode unsafeRefMember(const char *name);
// 	bool unsafeRefOther(DBLDataNode &other);
// 	bool unsafeSetValue(DBLDataNode &val);
// 	bool getValFromUnsafe(DBLDataNode &unsafeVal);
// 	bool isUnsafe() { return m_unsafeRef; }
protected:
	DBLDataNode _attrMathAdd(const DBLDataNode &leftval)const ;
	DBLDataNode _attrMathSub(const DBLDataNode &leftval)const ;
	DBLDataNode _attrMathMul(const DBLDataNode &leftval)const ;
	DBLDataNode _attrMathDiv(const DBLDataNode &leftval)const ;

	DBLDataNode _arrayMathAdd(const DBLDataNode &leftval)const;
	DBLDataNode _arrayMathSub(const DBLDataNode &leftval)const;
	DBLDataNode _arrayMathMul(const DBLDataNode &leftval)const;
	DBLDataNode _arrayMathDiv(const DBLDataNode &leftval)const;

	void Destroy();
	void init() ;
	void _copy(const DBLDataNode &r);
	int _writeEmptyStream(char *streamBuf, int streamByteOrder)const;
	bool m_dataOwner; // false needn't release data 
	
// 	bool m_unsafeRef; // array element or userdefdata reference parent ,  which _data is 
// 
// 	enum eUnsafeType{ UNSAFE_REF_ADDR_REF, UNSAFE_REF_ORGINAL};
// 	NDUINT8 m_unsafeRefType;

	NDUINT8 m_ele_type;
	NDUINT8 m_sub_type;
	dbl_element_base *m_data;
	dbl_element_base m_dataOwn;

	static NDUINT8 s_bOutHex;
	static NDUINT8 s_bOutLua;
};


CPPAPI int dbl_destroy_data(dbl_element_base *buf, DBL_ELEMENT_TYPE etype, DBL_ELEMENT_TYPE sub_etype);
CPPAPI int dbl_build_from_text(dbl_element_base *buf, const char *in_data, DBL_ELEMENT_TYPE etype, DBL_ELEMENT_TYPE sub_etype);
CPPAPI int dbl_data_copy(const dbl_element_base *input, dbl_element_base *output, DBL_ELEMENT_TYPE etype, DBL_ELEMENT_TYPE sub_etype);

typedef size_t (*dbl_stream_fread)( void * _DstBuf,  size_t _ElementSize,  size_t _Count,  FILE * _File);
typedef size_t(*dbl_stream_fwrite)(const void * _DstBuf, size_t _ElementSize, size_t _Count, FILE * _File);

CPPAPI int dbl_data_2streamfile(dbl_element_base *buf, DBL_ELEMENT_TYPE etype, DBL_ELEMENT_TYPE sub_etype, FILE*pf, dbl_stream_fwrite writefunc,  bool changeByteOrder = false);
CPPAPI int dbl_read_streamfile(dbl_element_base *indata, DBL_ELEMENT_TYPE etype, DBL_ELEMENT_TYPE sub_etype, FILE*pf, dbl_stream_fread readfunc, bool changeByteOrder = false);

CPPAPI int dbl_read_buffer(dbl_element_base *data, DBL_ELEMENT_TYPE etype, DBL_ELEMENT_TYPE sub_etype, char *buf, bool changeByteOrder=false);
CPPAPI int dbl_write_buffer(dbl_element_base *data, DBL_ELEMENT_TYPE etype, DBL_ELEMENT_TYPE sub_etype, char *buf, bool changeByteOrder = false);


#endif
