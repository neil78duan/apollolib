/* file dbl_mgr.cpp
 * 
 * 策划静态数据库管理器
 *
 * create by duan 
 *
 * 2012/7/4 18:02:51
 *
 */


#include "game_parser/dbl_mgr.h"
#include "nd_common/nd_common.h"

#include "nd_common/nd_iconv.h"
#include "nd_net/byte_order.h"
//#include <time.h>
//#include <stdlib.h>
// 
// int text_2file(char *buf, size_t size, const char *file)
// {
// 	FILE *pf = fopen(file, "wb") ;
// 	if(pf) {
// 		fwrite(buf,1, size, pf) ;
// 		fclose(pf) ;
// 	}
// 	return 0;
// }

#if 1 //use file crtyp

#include "nd_crypt/nd_cryptfile.h"
#undef CRYPT_FILE
#define CRYPT_FILE NDC_FILE

#undef crypt_fread
#define crypt_fread ndc_fread

#undef crypt_fwrite
#define crypt_fwrite ndc_fwrite

#undef crypt_fclose
#define crypt_fclose ndc_fclose

#undef crypt_fseek
#define crypt_fseek ndc_fseek

#undef crypt_ftell
#define crypt_ftell ndc_ftell

#undef crypt_fflush
#define crypt_fflush ndc_fflush

#undef crypt_fopen_r
#define crypt_fopen_r(_name, _mode) ndc_fopen_r(_name, "apollo123")

#undef crypt_fopen_w
#define crypt_fopen_w(_name, _mode) ndc_fopen_w(_name, "apollo123")

#else 

#undef CRYPT_FILE
#define CRYPT_FILE FILE

#undef crypt_fread
#define crypt_fread fread

#undef crypt_fwrite
#define crypt_fwrite fwrite

#undef crypt_fclose
#define crypt_fclose fclose

#undef crypt_fseek
#define crypt_fseek fseek

#undef crypt_ftell
#define crypt_ftell ftell

#undef crypt_fflush
#define crypt_fflush fflush

#undef crypt_fopen_r
#define crypt_fopen_r(_name, _mode) fopen(_name, _mode)

#undef crypt_fopen_w
#define crypt_fopen_w(_name, _mode) fopen(_name, _mode)

#endif 

#define DBL_MAX_COLS_NUM 1024
//#define USE_MYSQL_CHARACTER_CONVERT 

const char *dbl_resource_type_to_host(const char *inputText, int encodeType);

int create_db_header(FILE *pf);
int check_db_header(FILE *pf, int with_inc) ;

#pragma pack(push, 4)
struct dbfile_header
{
	NDUINT16 header_size ;
	NDUINT16 stuff_len ;
	NDUINT32 used_times ;
	NDUINT32 create_time; //time_t 
	NDUINT32 exceed_time; //time_t
	NDUINT8 sign[16];
};
#pragma pack(pop)

#define DBL_MAX_ARRAY_SIZE 1024
nd_handle DBLDatabase::m_pool ;
char *DBLDatabase::s_convert_buf;
int DBLDatabase::s_data_encodeType = E_SRC_CODE_UTF_8;
int DBLDatabase::s_data_endianOrder = ND_L_ENDIAN;
bool DBLDatabase::s_data_needChangeOrder = false;

#define DBL_DATE_ENCODE_TYPE DBLDatabase::s_data_encodeType
#define DBL_CHECK_CHANGE_BYTE_ORDER() DBLDatabase::s_data_needChangeOrder 
#define DBL_CLEAR_CHANGE_BYTE_ORDER() (DBLDatabase::s_data_needChangeOrder =false )
#define DBL_SET_CHANGE_BYTE_ORDER() (DBLDatabase::s_data_needChangeOrder = true )

static inline void _tryto_change_order_short(NDUINT16 &val)
{
	if (DBL_CHECK_CHANGE_BYTE_ORDER()) {
		val = nd_order_change_short(val);
	}
}

#ifdef malloc 
#undef malloc
#define malloc(size) nd_pool_alloc( DBLDatabase::m_pool,size) 
#endif

#ifdef free 
#undef free
#define free(p) nd_pool_free( DBLDatabase::m_pool, p) 
#endif

static char * getTextLine(char *buf, size_t size, FILE *pf, int encodeType)
{
	if (encodeType == DBL_DATE_ENCODE_TYPE) {
		char *p= fgets(buf, (int)size, pf);
		if (p && *p)	{
			size_t len = strlen(p);
			if (p[len - 1] == '\r' || p[len - 1] == '\n')		{
				p[len - 1] = 0;
			}
		}
		return p;
	}
	else {
		char *p;
		char readbuf[1024 * 4];
		readbuf[0] = 0;
		p = fgets(readbuf, sizeof(readbuf), pf);
		if (!p){
			return NULL;
		}
		size_t len = strlen(p);
		if (p[len - 1] == '\r' || p[len - 1] == '\n')		{
			p[len - 1] = 0;
		}

		if ( DBL_DATE_ENCODE_TYPE ==E_SRC_CODE_UTF_8  )	{
			return nd_gbk_to_utf8(readbuf, buf, (int)size);
		}
		else {
			return nd_utf8_to_gbk(readbuf, buf, (int)size);
		}
		
	}

}
int text2StringVct(char *text, strings_vct &strings_arr,int skip_empty)
{
	//first col not empty
	const char *p = ndstr_first_valid(text);
	char buf[1024];
	while (p && *p) {
		buf[0] = 0;
		p = (char*)ndstr_nstr_end(p, buf, '\t', sizeof(buf));
		if (p && *p=='\t') {
			++p;
		}
		if (skip_empty){
			//skill 
			if (buf[0]) {
				const char *addr = ndstr_first_valid(buf);
				if (addr && *addr){
					strings_arr.push_back(std::string(addr));
				}
			}
			else {
				strings_arr.push_back(std::string(buf));
			}
		}
		else {
			strings_arr.push_back(std::string(buf));
		}
	}
	return (int)strings_arr.size();
}


//////////////////////////////////////////////////////////////////////////
DBLRecord::DBLRecord() : m_col(0) ,m_data_buf(0)
{

}
DBLRecord::~DBLRecord() 
{

}
int DBLRecord::Create(int col) 
{
	if (col > 0 ){
		m_data_buf = (dbl_element_base*) malloc(sizeof(dbl_element_base)*col) ;
		if (!m_data_buf){
			return -1 ;
		}
		m_col = col ;
		memset(m_data_buf, 0,sizeof(dbl_element_base)*col) ;
	}
	return 0;
}
void DBLRecord::Destroy() 
{
	if (m_data_buf) {
		free(m_data_buf) ;
		m_data_buf = 0 ;
		m_col = 0;
	}

}
//////////////////////////////////////////////////////////////////////////

DBLTable::DBLTable(nd_handle pool, const char *name) : m_cols(0),m_pcols(0)
{

#ifdef ND_DEBUG
	if (name)
		strncpy(m_raw_name, name, sizeof(m_raw_name)) ;
	else 
		m_raw_name[0] = 0;
#endif 
}
DBLTable::~DBLTable() 
{

}

void DBLTable::ReleaseRecord(DBLRecord *pRecord) 
{
	for(int i=0; i<m_cols; i++) {
		dbl_destroy_data(&pRecord->m_data_buf[i],m_pcols[i].type, m_pcols[i].sub_type) ;
	}
	pRecord->Destroy() ;
}
void DBLTable::Destroy() 
{
	for(table_map_t::iterator it = m_rocords.begin(); it != m_rocords.end(); ++it) {
		if(it->second) {
			DBLRecord *pRecord = it->second;
			ReleaseRecord(pRecord) ;
			delete pRecord ;
			it->second = 0 ;
		}
	}
	if (m_pcols) {
		free(m_pcols) ;
		m_pcols = 0 ;
	}
	m_rocords.clear() ;
}

DBLDataNode DBLTable::FindData(int id, const char *col_name, int encodeType)
{
	int index = GetColIndex(col_name, encodeType);
	if (-1==index){
		return DBLDataNode() ;
	}
	row_iterator it = m_rocords.find((NDUINT32)id) ;
	if (it	== m_rocords.end()) {
		return DBLDataNode() ;
	}
	dbl_element_base *pele = it->second->GetOrgVal( index) ;
	return DBLDataNode(pele, m_pcols[index].type,m_pcols[index].sub_type) ;
}

int DBLTable::GetColIndex(const char *col_name, int encodeType)
{
	const char*name;
	char buf[DBL_NAME_SIZE];
	if (encodeType == DBL_DATE_ENCODE_TYPE || encodeType == E_SRC_CODE_ANSI) {
		name = col_name;
	}
	else {
		name = buf;
		if (DBL_DATE_ENCODE_TYPE == E_SRC_CODE_UTF_8)	{
			nd_gbk_to_utf8(col_name, buf, (int)sizeof(buf));
		}

		else if (encodeType == E_SRC_CODE_UTF_8){
			nd_utf8_to_gbk(col_name, buf, (int)sizeof(buf));
		}

		else {
			name = col_name;
		}
	}

	for(int i=0; i<m_cols; i++){
		if (ndstricmp(name, m_pcols[i].col_alias) == 0){
			return i;
		}
		if (ndstricmp(name, m_pcols[i].col_name) == 0){
			return i;
		}
// 
// 		const char *destname = useAlias ? m_pcols[i].col_alias : m_pcols[i].col_name;
// 		if (ndstricmp(name, destname ) == 0){
// 			return i;
// 		}
	}
	return -1;
}

const char *DBLTable::GetColName(int index )
{
	if (index>=0 && index<m_cols){
		return m_pcols[index].col_name ;
	}
	return NULL;
}

const char *DBLTable::GetColAliaName(int index)
{
	if (index>=0 && index<m_cols){
		return  m_pcols[index].col_alias;
	}
	return NULL;
}


const char *DBLTable::GetColTypeName(int index)
{
	static const char *_col_type_names[] = {
		"int", "float", "string", "NDUINT8", "NDUIN16",  "NDUINT64", "bool", "array", "time"
	};
	
	int type = -1;
	if (index >= 0 && index < m_cols){
		type = m_pcols[index].type;
	}
	if (type == -1 || type >OT_TIME){
		return "unknow";
	}
	return _col_type_names[type];
}

NDUINT32 DBLTable::GetRowIDByindex(int index) 
{
	row_iterator it = m_rocords.begin();
	for (int i = 0; i < index; i++) {
		++it;
		if (it == m_rocords.end()) {
			return 0;
		}
	}
	DBLDataNode data(it->second->GetOrgVal(0), OT_INT, OT_INT);
	return data.GetInt();

	return 0 ;
}

int DBLTable::GetRows() 
{
	return (int)m_rocords.size() ;
}

int DBLTable::GetCols() 
{
	return m_cols ;
}



//scan file to get table info 
int DBLTable::parseTableTypeInfo(FILE *pf, int encodeType)
{
	char *p;
	char readbuf[1024 * 4];
	readbuf[0] = 0;
	NDUINT8 is_setbuf[DBL_MAX_COLS_NUM] = { 0 };
	p = getTextLine(readbuf, sizeof(readbuf), pf,encodeType);
	if (!p){
		nd_logerror("read col-name error\n");
		return -1;
	}
	strings_vct line1;
	if (text2StringVct(readbuf, line1,1) <= 0) {
		nd_logerror("parse cols-name error\n");
		return -1;
	}
	int x = 0;
	for (strings_vct::iterator it = line1.begin(); it != line1.end(); it++){
		++x;
		if ((*it).size() == 0)		{
			nd_logerror("parse name line [2,%d] error, name is NULL\n",x);
			return -1;
		}

	}
	m_cols = (int) line1.size();
	m_pcols = (col_info*)malloc(sizeof(col_info)* m_cols);
	memset(m_pcols, 0, sizeof(col_info)* m_cols);
	
	//remove or skip second line ;
	p = getTextLine(readbuf, sizeof(readbuf), pf, encodeType);
	if (!p){
		//read second line error
		nd_logerror("read second line error row=2\n");
		return -1;
	}

#ifdef EXCEL_SECOND_LINE_AS_NAME
	strings_vct line2;
	if (text2StringVct(readbuf, line2, 1) <= 0) {
		nd_logwarn("parse cols-name error\n");
		//return 0;
	}

	x = 0;
	for (strings_vct::iterator it = line2.begin(); it != line2.end(); it++){
		++x;
		if ((*it).size() == 0)		{
			nd_logerror("parse row %d error name not NULL\n", x);
			return -1;
		}

	}
#endif 
	
	//end second line
	int rowIndex = 2;
	while ((p = getTextLine(readbuf, sizeof(readbuf), pf,encodeType))){
		strings_vct record;
		++rowIndex;
		if (text2StringVct(readbuf, record,0) > 0) {
			int count = NDMIN(m_cols, (int)record.size());
			for (int i = 0; i < count; i++) {
				if (record[i].size() > 0) {
					is_setbuf[i] = 1;
					if (m_pcols[i].type == OT_STRING )	{
						continue;
					}
					int subType = OT_INT ;
					int type = DBLDataNode::getCellType(record[i].c_str(), subType);
					if (OT_OBJECT_VOID == type) {
						continue;
					}
					else if (-1 == type) {
						nd_logerror("parse col %s error at (%d,%d) id= %s text=%s\n", line1[i].c_str(), rowIndex, i, record[0].c_str(),record[i].c_str());
						return -1;
					}
					
					if (m_pcols[i].type == OT_ARRAY ) {
						if (type != OT_ARRAY) {
							nd_logerror("parse col %s error at (%d,%d) id=%s text=%s , need array\n", line1[i].c_str(), rowIndex, i, record[0].c_str(),record[i].c_str());
							return -1;
						}
						m_pcols[i].sub_type = NDMAX(m_pcols[i].sub_type, (DBL_ELEMENT_TYPE)subType); 

					}
					else {
						if (type == OT_ARRAY && m_pcols[i].type == OT_STRING) {
							nd_logerror("parse col %s error row=%d\n", line1[i].c_str(), rowIndex);
							return -1;
						}
						m_pcols[i].type = NDMAX((DBL_ELEMENT_TYPE)type, m_pcols[i].type);
						if (m_pcols[i].type == OT_ARRAY) {
							m_pcols[i].sub_type = NDMAX(m_pcols[i].sub_type, (DBL_ELEMENT_TYPE)subType);
						}
					}

				}
			}
		}
	}
	nd_assert(m_cols < DBL_MAX_COLS_NUM);
	for (int i = 0; i < m_cols; i++) {
		strncpy(m_pcols[i].col_name, line1[i].c_str(), DBL_NAME_SIZE);

#ifdef EXCEL_SECOND_LINE_AS_NAME
		if (line1.size() == line2.size() && line2[i].size() > 0) {
			strncpy(m_pcols[i].col_alias, line2[i].c_str(), DBL_NAME_SIZE);
		}
		else {
			nd_logerror("first line and second line is not size\n");
			return -1;
		}
#endif 
		// force empty is string		
		if (is_setbuf[i] == 0)	{
			m_pcols[i].type = OT_STRING;
		}
	}
	fseek(pf, 0, SEEK_SET);
	return 0;
}

int DBLTable::loadFromText(const char *tablename, int codeType)
{
	char *p;
	char readbuf[0x10000];
	FILE * pf = fopen(tablename, "r");
	if (!pf){
		nd_logerror("openfile %s error %s\n", tablename, nd_last_error());
		return -1;
	}
	if (-1==DBLTable::parseTableTypeInfo(pf,codeType)){
		fclose(pf);
		nd_logerror("Parse file %s error \n", tablename);
		return -1;
	}
	if (m_cols <= 0) {
		fclose(pf);
		nd_logerror("file %s first col not be empty\n", tablename);
		return -1;
	}
	if (m_pcols[0].type == OT_FLOAT) {
		m_pcols[0].type = OT_INT;
	}

	if (m_pcols[0].type != OT_INT ){

		fclose(pf);
		nd_logerror("file %s first col not be int\n", tablename);
		return -1;
	}
	
	int rowIndex = 1;
	// skip first line
	p = fgets(readbuf, sizeof(readbuf), pf);
#ifdef EXCEL_SECOND_LINE_AS_NAME
	// skip second line 
	p = fgets(readbuf, sizeof(readbuf), pf);
	
	rowIndex = 2;
#endif
	
	while ((p = getTextLine(readbuf, sizeof(readbuf), pf, codeType))){
		strings_vct record;
		++rowIndex;
		if (text2StringVct(readbuf, record,0) > 0) {

			NDUINT32 uid = (NDUINT32)atoi(record[0].c_str());

			DBLRecord *pRecord = new DBLRecord;
			if (!pRecord){
				fclose(pf);
				nd_logerror("read %s in row %d error , memory alloc\n", tablename, rowIndex);
				return -1;
			}
			if (-1 == pRecord->Create(m_cols)) {
				fclose(pf);
				nd_logerror("create record error in %s %d \n", tablename, rowIndex);
				return -1;
			}

			int count = NDMIN(m_cols, (int)record.size());
			for (int i = 0; i < count; i++) {
				dbl_element_base *pbase = pRecord->GetOrgVal(i);
				pbase->isInit = false;
				if (record[i].size() > 0) {
					if (-1 == dbl_build_from_text(pbase, (char*)record[i].c_str(), m_pcols[i].type, m_pcols[i].sub_type)) {
						ReleaseRecord(pRecord);
						pRecord->Destroy();
						delete pRecord;
						
						nd_logerror("create cell (%d,%d) error in %s  \n", rowIndex,i, tablename);
						fclose(pf);
						return -1;
					}
					pbase->isInit = true;
				}
			}
			std::pair<table_map_t::iterator, bool> ret = m_rocords.insert(std::make_pair(uid, pRecord));
			if (!ret.second){
				ReleaseRecord(pRecord);
				pRecord->Destroy();
				delete pRecord;
				nd_logerror("read table %s error on %d line : maybe the table index is dup id=%d\n", tablename, rowIndex, uid);
				fclose(pf);
				return -1;
			}
		}
	}

	fclose(pf);
	return 0;
}

int DBLTable::Dump(void *pfile ) 
{
	int ret ;
	NDUINT16 size ;
	size_t length;
	CRYPT_FILE *pf = (CRYPT_FILE*)pfile ;
	//write table header | 16bits col number, + col infos
	size = m_cols ;
	_tryto_change_order_short(size);
	ret = (int)crypt_fwrite(&size, sizeof(size), 1, pf);
	nd_assert(ret ) ;

	for(int i=0; i<m_cols; i++) {
		NDUINT8 type = m_pcols[i].type ;
		ret = (int)crypt_fwrite(&type, sizeof(type), 1, pf);
		nd_assert(ret ) ;

		type = m_pcols[i].sub_type ;
		ret = (int)crypt_fwrite(&type, sizeof(type), 1, pf);
		nd_assert(ret ) ;

		//write real-name
#ifdef EXCEL_SECOND_LINE_AS_NAME
		size = (NDUINT16)strlen(m_pcols[i].col_alias);
		if (size > 0) {
			type = 1;
			crypt_fwrite(&type, sizeof(type), 1, pf);

			length = size;
			_tryto_change_order_short(size);
			ret = (int)crypt_fwrite(&size, sizeof(size), 1, pf);
			nd_assert(ret);

			ret = (int)crypt_fwrite(m_pcols[i].col_alias, length, 1, pf);
			nd_assert(ret);

		}
		else {
			type = 0; 
			crypt_fwrite(&type, sizeof(type), 1, pf);
		}
		
#else 
		type = 0; 
		crypt_fwrite(&type, sizeof(type), 1, pf);
#endif 
		

		size =(NDUINT16) strlen(m_pcols[i].col_name) ;

		length = size;
		_tryto_change_order_short(size);
		ret = (int)crypt_fwrite(&size, sizeof(size), 1, pf);
		nd_assert(ret ) ;
		//col name
		ret = (int)crypt_fwrite(m_pcols[i].col_name, length, 1, pf);
		nd_assert(ret ) ;


	}
	//write row number 
	size =(NDUINT16) m_rocords.size() ;

	length = size;
	_tryto_change_order_short(size);
	ret = (int)crypt_fwrite(&size, sizeof(size), 1, pf);
	nd_assert(ret ) ;
	//write data 
	
	for(table_map_t::iterator it = m_rocords.begin(); it!=m_rocords.end(); ++it) {
		DBLRecord *pRecord = it->second ;
	
		//write this line valid flag 
		for(int i=0; i<m_cols; i++) {
			NDUINT8 isInit = pRecord->m_data_buf[i].isInit ? 1 : 0; 
			ret =(int) crypt_fwrite(&isInit, 1, 1, pf);
			nd_assert(ret);
			if (isInit)	{
				ret = dbl_data_2streamfile(&pRecord->m_data_buf[i], m_pcols[i].type, m_pcols[i].sub_type, (FILE*)pf, (dbl_stream_fwrite)crypt_fwrite, DBLDatabase::s_data_needChangeOrder);
				nd_assert(ret == 0);
			}
		}
	}
	return 0;
}

int DBLTable::LoadBinStream(void *pfile)
{
	int ret ;
	NDUINT16 size ;
	CRYPT_FILE *pf = (CRYPT_FILE*)pfile ;
	ret = (int)crypt_fread(&size, sizeof(size), 1, pf);
	if(ret<=0 )
		return -1;

	_tryto_change_order_short(size);
	m_cols = size ;
	m_pcols = (col_info*) malloc( sizeof(col_info ) * size ) ;
	//memset(m_pcols, 0,  sizeof(col_info ) * size) ;

	if(!m_pcols)
		return -1;

	for(int i=0; i<m_cols; i++) {
		NDUINT8 type ;
		ret = (int)crypt_fread(&type, sizeof(type), 1, pf);
		nd_assert(ret ) ;
		m_pcols[i].type = (DBL_ELEMENT_TYPE) type ;

		ret = (int)crypt_fread(&type, sizeof(type), 1, pf);
		nd_assert(ret ) ;
		m_pcols[i].sub_type = (DBL_ELEMENT_TYPE)type ;

		//write real-name
		type = 0;
		crypt_fread(&type, sizeof(type), 1, pf);
#ifdef EXCEL_SECOND_LINE_AS_NAME
		if (type ==1) {
			ret = (int)crypt_fread(&size, sizeof(size), 1, pf);
			nd_assert(ret);

			_tryto_change_order_short(size);
			//col name
			ret = (int)crypt_fread(m_pcols[i].col_alias, size, 1, pf);
			nd_assert(ret);
			m_pcols[i].col_alias[size] = 0;
		}
#endif 

		ret = (int)crypt_fread(&size, sizeof(size), 1, pf);
		nd_assert(ret ) ;

		_tryto_change_order_short(size);
		//col name
		ret = (int)crypt_fread(m_pcols[i].col_name, size, 1, pf);
		nd_assert(ret ) ;
		m_pcols[i].col_name[size] = 0;
	}
	//read row number 
	ret = (int)crypt_fread(&size, sizeof(size), 1, pf);
	nd_assert(ret ) ;
	_tryto_change_order_short(size);

	for(int x=0; x<size; x++) {
		DBLRecord *pRecord = new DBLRecord ;
		if(-1==pRecord->Create(m_cols) ) {
			delete pRecord;
			return -1 ;
		}

		for(int i=0; i<m_cols; i++) {
			NDUINT8 isInit = 0;
			ret =(int) crypt_fread(&isInit, 1, 1, pf);
			nd_assert(ret);
			if (isInit){
				if (-1 == dbl_read_streamfile(pRecord->GetOrgVal(i), m_pcols[i].type, m_pcols[i].sub_type, (FILE*)pf, (dbl_stream_fread)crypt_fread, DBLDatabase::s_data_needChangeOrder)) {
					delete pRecord;
					return -1;
				}

			}

		}
		NDUINT32 uid = (NDUINT32) (*pRecord)[0].i_val ;
		m_rocords.insert(std::make_pair(uid,pRecord)) ;
	}
	return 0;
}
//////////////////////////////////////////////////////////////////////////
DBLCursor::DBLCursor(DBLTable *ptb) : m_bOpen(0), m_ptable(ptb), m_bAllcol(1)
{
	//m_bUseAlias = useAlias ? 1 : 0;
}
DBLCursor::DBLCursor(DBLTable *ptb, const char*col_names[], int col_num, int encodeType)
	:m_bOpen(0),m_ptable(ptb),m_bAllcol(0)
{
	//m_bUseAlias = useAlias ? 1 : 0;
	if (ptb){
		for(int i=0; i<col_num; i++) {
			m_sel_cols.push_back(ptb->GetColIndex(col_names[i], encodeType));
		}
	}
}


DBLCursor::DBLCursor(const char *tableName, const char*col_names[], int col_num, int encodeType )
	: m_bOpen(0),  m_bAllcol(0)
{
	m_ptable = DBL_FindTable(tableName);
	if (m_ptable){
		for (int i = 0; i < col_num; i++) {
			m_sel_cols.push_back(m_ptable->GetColIndex(col_names[i], encodeType));
		}
	}	
}

DBLCursor::DBLCursor() :m_bOpen(0), m_ptable(0), m_bAllcol(1)
{
}
DBLCursor::~DBLCursor() 
{
	if (m_bOpen){
		m_datas.clear();
	}
	m_sel_cols.clear();
}

int DBLCursor::FetchFirst()
{
	if (m_bOpen){
		m_datas.clear();
	}
	m_cur_it = m_ptable->m_rocords.begin() ;
	return _fetch();
}

int DBLCursor::_fetch() 
{
	m_datas.clear();
	m_bOpen = 0;
	if (m_cur_it == m_ptable->m_rocords.end()) {
		return -1;
	}
	m_bOpen = 1;
	DBLRecord &record = *(m_cur_it->second );
	if (m_bAllcol){
		for(int i=0; i<m_ptable->m_cols; i++) {
			m_datas.push_back(DBLDataNode(&record[i],m_ptable->m_pcols[i].type, m_ptable->m_pcols[i].sub_type));
		}
	}
	else {
		for(int i=0; i<m_sel_cols.size(); i++) {
			int index = m_sel_cols[i] ;
			if (-1==index)	{
				m_datas.push_back(DBLDataNode()) ;
			}
			else {
				m_datas.push_back(DBLDataNode(&record[index],m_ptable->m_pcols[index].type, m_ptable->m_pcols[index].sub_type));
			}
		}
	}
	return 0;
}

int DBLCursor::FetchNext()
{
	if (!m_bOpen){
		return -1;
	}
	++m_cur_it ;
	return _fetch();
}

int DBLCursor::Fetch(int id) 
{
	if (!m_ptable)	{
		return -1;
	}
	m_cur_it = m_ptable->m_rocords.find((NDUINT32)id) ;
	return _fetch() ;
}
// 
// const char *DBLCursor::_convertText(const char *inputName, int inuptEncodetype)
// {
// 	const char*name;
// 	char col_name[DBL_NAME_SIZE];
// 	if (inuptEncodetype == DBL_DATE_ENCODE_TYPE) {
// 		name = inputName;
// 	}
// 	else {
// 		if (DBL_DATE_ENCODE_TYPE == E_SRC_CODE_UTF_8)	{
// 			nd_gbk_to_utf8(inputName, m_convertbuf, (int)sizeof(m_convertbuf));
// 		}
// 		else {
// 			nd_utf8_to_gbk(inputName, m_convertbuf, (int)sizeof(m_convertbuf));
// 		}
// 		name = m_convertbuf;
// 	}
// 	return name;
// }

const char *DBLCursor::GetColName(int index)
{
	if (index < 0){
		return NULL;
	}
	if(m_bAllcol) {
		if (index < m_ptable->m_cols){
			return m_ptable->GetColName(index) ;
		}
	}
	else {
		if (index< m_sel_cols.size())	{
			return m_ptable->GetColName(m_sel_cols[index]) ;
		}
	}
	return NULL;
}

const char *DBLCursor::GetAliasName(int index)
{
	if (index < 0){
		return NULL;
	}
	if(m_bAllcol) {
		if (index < m_ptable->m_cols){
			return m_ptable->GetColAliaName(index) ;
		}
	}
	else {
		if (index< m_sel_cols.size())	{
			return m_ptable->GetColAliaName(m_sel_cols[index]) ;
		}
	}
	return NULL;
}
int DBLCursor::GetCols() 
{
	if(m_bAllcol) {
		return m_ptable->m_cols ;
	}
	else {
		return (int) m_sel_cols.size();
	}
}
void DBLCursor::Close()
{
	m_datas.clear() ;
	m_cur_it = m_ptable->m_rocords.end();
	m_bOpen = 0;
}

DBLDataNode &DBLCursor::operator[](int index)
{
	nd_assert(m_bOpen) ;
	nd_assert(index <m_datas.size());
	return m_datas[index] ;
}

bool DBLCursor::GetDataNode(const char* _pszColName, DBLDataNode& _dataNode, int encodeType )
{
	
	int index = m_ptable->GetColIndex(_pszColName, encodeType);
    if (index == -1)
        return false;
    else   {
        _dataNode = m_datas[index];
        return true;
    }
}
DBLDataNode DBLCursor::GetDataNode(const char* _pszColName, int encodeType )
{
	DBLDataNode ret ;
	GetDataNode( _pszColName, ret,encodeType ) ;
	return ret ;
}
//////////////////////////////////////////////////////////////////////////
DBLDatabase::DBLDatabase() : m_bLoaded(false)
{

}
DBLDatabase::~DBLDatabase() 
{

}
int DBLDatabase::LoadFromText(const char *datapath, const char *list_file, const char *textEncode, const char*toEncode)
{
	if (m_bLoaded) {
		return 0;
	}
	int inputEncodeType = nd_get_encode_val(textEncode);
	int toEnhcodeType = nd_get_encode_val(toEncode);
	s_data_encodeType = toEnhcodeType;

	int org_code = ndstr_set_code(toEnhcodeType);
	int ret = _loadText(datapath, list_file, inputEncodeType);
	ndstr_set_code(org_code);
	return ret;

}

int DBLDatabase::_loadText(const char *datapath, const char *list_file, int encodeType)
{
	char buf_convert[4096] ;
	buf_convert[0] = 0 ;
	DBLDatabase::s_convert_buf = buf_convert;

	char filename[128];
	char filepath[1024];

	FILE *pf = fopen(list_file, "r");
	if (!pf){
		DBLDatabase::s_convert_buf = 0;
		nd_logerror("load from test, open file-list %s error %s\n" AND list_file AND nd_last_error());
		return -1;
	}
	filename[0] = 0;
	while (fgets(filename, sizeof(filename), pf)) {
		if (filename[0]){

			char tmpf[128];
			tmpf[0] = 0;
			const char *p = ndstr_first_valid(filename);
			if (!p) {
				continue; 
			}
			nd_file_name_without_ext(p,tmpf, sizeof(tmpf));
			snprintf(filepath, sizeof(filepath), "%s/%s.txt", datapath, tmpf);
			NDTRAC("BEGIN IMPORT %s ...\n", tmpf);

			/*const char *p = ndstr_first_valid(filename);
			if (!p || !*p)	{
				continue; 
			}
			char tmpf[128];
			tmpf[0] = 0;
			ndstr_parse_string(p, tmpf);
			if (!tmpf[0])	{
				continue;
			}

			NDTRAC("BEGIN IMPORT %s ...\n", tmpf);
			snprintf(filepath, sizeof(filepath), "%s/%s.txt", datapath, tmpf);
			*/

			DBLTable *ptable = new DBLTable(DBLDatabase::m_pool, tmpf);
			if (!ptable) {
				fclose(pf) ;
				return -1;
			}
			if (-1 == ptable->loadFromText(filepath,encodeType)){
				ptable->Destroy();
				delete ptable;
				fclose(pf) ;
				return -1;
			}

			NDTRAC("IMPORT %s success!!!!!!\n", tmpf);
			std::pair<table_vct_t::iterator, bool> ret = m_tables.insert(std::make_pair(tmpf, ptable));
			if (!ret.second) {
				ptable->Destroy();
				delete ptable;
			}
			else {
				nd_logmsg("load %s from text success\n", tmpf);
			}
		}
	}
	//load version 
	if (m_tables.find("version")==m_tables.end() )	{
		snprintf(filepath, sizeof(filepath), "%s/version.txt", datapath);

		DBLTable *ptable = new DBLTable(DBLDatabase::m_pool, "version");
		if (ptable) {
			if (-1 == ptable->loadFromText(filepath, encodeType)){
				ptable->Destroy();
				delete ptable;
				return -1;
			}

			std::pair<table_vct_t::iterator, bool> ret = m_tables.insert(std::make_pair("version.xlsx", ptable));
			if (!ret.second) {
				ptable->Destroy();
				delete ptable;
			}
			else {
				nd_logmsg("add version.xlsx to table-list success\n");
			}
		}

		NDTRAC("IMPORT version.txt success!!!!!!\n");		
	}

	NDTRAC("!!!!!!!!!!!!LOAD database complete!\n");
	m_bLoaded = true;
	
	fclose(pf) ;
	return 0;
}

int DBLDatabase::Dump(const char *file, const char *dbname, int orderType )
{
	int ret ;
	NDUINT16 size;
	size_t length ;
	CRYPT_FILE *pf = crypt_fopen_w(file, "wb");

	if(!pf) {
		nd_logerror("dump database, open file %s error %s\n" AND file AND nd_last_error()) ;
		return -1 ;
	}
	//write file name  | 16bits size + string
	// Write byte order 
	NDUINT8 val1 = nd_byte_order();

	DBLDatabase::s_data_endianOrder = orderType;
	if (val1 != orderType){
		DBL_SET_CHANGE_BYTE_ORDER();
		val1 = orderType;
	}

	crypt_fwrite(&val1, sizeof(val1), 1, pf);

	val1 = DBL_DATE_ENCODE_TYPE;
	crypt_fwrite(&val1, sizeof(val1), 1, pf);

	//write package user name and time
	char dbname_buf[1024] ;
	size = snprintf(dbname_buf, sizeof(dbname_buf), "%s.%s-%s",dbname?dbname:"dbl",nd_get_sys_username(), nd_get_datetimestr()) ;
	
	length = size ;
	
	_tryto_change_order_short(size);
	ret = (int)crypt_fwrite(&size, sizeof(size), 1, pf);
	nd_assert(ret) ;
	
    ret = (int)crypt_fwrite(dbname_buf, length, 1, pf);
	nd_assert(ret) ;

	
	nd_logdebug("==========begin DUMP database from bin-stream ....\n");
	//write table numbers 16bits
	size =(NDUINT16) m_tables.size() ;
	_tryto_change_order_short(size);
	crypt_fwrite(&size, sizeof(size), 1, pf);

	for(table_vct_t::iterator it = m_tables.begin(); it!= m_tables.end() ;++it) {
		size = (int)it->first.size() ;
		length = size;

		_tryto_change_order_short(size);
		ret = (int)crypt_fwrite(&size, sizeof(size), 1, pf);
		nd_assert(ret) ;
		ret = (int)crypt_fwrite(it->first.c_str(), length, 1, pf);
		nd_assert(ret) ;

		nd_logdebug("DUMPing %s....\n", it->first.c_str());
		if (0 == it->second->Dump((void*)pf)) {
			nd_logdebug("save %s file to stream success\n", it->first.c_str());
		}
	}
	crypt_fclose(pf);

	DBL_CLEAR_CHANGE_BYTE_ORDER();
	return 0 ;
}

bool DBLDatabase::operator == (const DBLDatabase &r)
{
	
	for (table_vct_t::iterator it = m_tables.begin(); it != m_tables.end(); ++it) {

		DBLTable *ptbl2 = ((DBLDatabase &)r).FindTable(it->first.c_str());
		if (!ptbl2)	{
			return false;
		}
		DBLCursor cursor1(it->second);
		DBLCursor cursor2(ptbl2);

		if (cursor1.GetCols()!= cursor2.GetCols())	{
			nd_logerror(" table %s not equal cols not equal\n", it->first.c_str());
			return false;
		}

		int ret1 = cursor1.FetchFirst();
		int ret2 = cursor2.FetchFirst();
		
		int x = 0;
		for (; ret1 == 0 && ret2 == 0; ret1 = cursor1.FetchNext(), ret2 = cursor2.FetchNext())	{
			++x;
			for (int i = 0; i < cursor1.GetCols(); i++)	{
				if (cursor1[i] !=cursor2[i]){
					nd_logerror(" table %s not equal in (%d, %d) ",	it->first.c_str(),x, i);
					return false;
				}
			}
		}
		cursor1.Close();
		cursor2.Close();
	}
	return true;
}


int DBLDatabase::LoadBinStream(const char *file)
{
	if (m_bLoaded)	{
		return 0;
	}

	int ret ;
	NDUINT16 size ;
	char dbname[1024] ;
	CRYPT_FILE *pf = crypt_fopen_r(file, "rb") ;

	NDUINT8 byteOrder = -1, encodeType = -1;
	if(!pf) {
		nd_logerror("open file %s error %s \n" AND file AND nd_last_error())  ;
		return -1 ;
	}
	
	crypt_fread(&byteOrder, sizeof(byteOrder), 1, pf);
	if (byteOrder != nd_byte_order())	{
		//nd_logerror(" read file error byte-order error the file is %s current cup is %s\n", byteOrder ? "little-end" : "big-end", nd_byte_order() ? "little-end" : "big-end");
		//fclose(pf);
		//return -1;
		DBL_SET_CHANGE_BYTE_ORDER();
	}
	DBLDatabase::s_data_endianOrder = byteOrder;

	crypt_fread(&encodeType, sizeof(encodeType), 1, pf);
	s_data_encodeType = (int)encodeType;
	
	
	ret = (int)crypt_fread(&size, sizeof(size), 1, pf);
	_tryto_change_order_short(size);

	if (ret <= 0 || size > 1024){
		crypt_fclose(pf);

		DBL_CLEAR_CHANGE_BYTE_ORDER();

		nd_logerror("database name size is too long\n");
		return -1;
	}

	if (size > 0){
		ret = (int)crypt_fread(dbname, size, 1, pf);
		nd_assert(ret) ;

		dbname[size]= 0 ;
		m_dbName = dbname ;
	}
	else {
		m_dbName = "dbl-database" ;
	}

	//nd_logmsg("begin load %s from %s ....\n", m_dbName.c_str(), file) ;

	//NDTRAC("==========begin READing database from bin-stream ....\n");
	//write table numbers 16bits
	ret = (int)crypt_fread(&size, sizeof(size), 1, pf);
	nd_assert(ret) ;
	_tryto_change_order_short(size);

	for(int i=0; i<size; i++) {
		NDUINT16 name_len = 0 ;
		char tablename[128] ;
		tablename[0] = 0;
		ret = (int)crypt_fread(&name_len, sizeof(name_len), 1, pf);
		nd_assert(ret) ;
		_tryto_change_order_short(name_len);

		if (name_len){
			ret = (int)crypt_fread(tablename, name_len, 1, pf);
			nd_assert(ret) ;
			tablename[name_len] = 0 ;
		}

		//NDTRAC("loading %s....\n", tablename);

		DBLTable *ptable = new DBLTable(DBLDatabase::m_pool, tablename);
		if(!ptable) {
			return -1;
		}
		if (-1 == ptable->LoadBinStream((void*)pf)){
			delete ptable ;
			nd_logerror("load table %s error\n", tablename);
			return -1;
		}

		m_tables.insert(std::make_pair(dbl_name(tablename), ptable)) ;

		//nd_logdebug("Load %s file from stream success\n", tablename);
	}
	crypt_fclose(pf);
	m_bLoaded = true;
	DBL_CLEAR_CHANGE_BYTE_ORDER();
	
	
	nd_logmsg("load %s from %s SUCCESS!!!\n", m_dbName.c_str(), file) ;
	return 0 ;
}

int DBLDatabase::ReloadTable(const char *table) 
{
	/*
#ifdef ND_DEBUG
	if (m_dbinfos[0].empty()|| m_dbinfos[1].empty() || m_dbinfos[2].empty() || m_dbinfos[3].empty()){
		return -1;
	}
	char buf_convert[4096] ;
	buf_convert[0] = 0 ;
	DBLDatabase::s_convert_buf = buf_convert;

	table_vct_t ::iterator it = m_tables.find(dbl_name(table)) ;
	if (it == m_tables.end() ){
		return -1 ;
	}
	CMyDatabase mydb ;
	int ret = mydb.open_database(m_dbinfos[0].c_str(), m_dbinfos[1].c_str(),m_dbinfos[2].c_str(), m_dbinfos[3].c_str());
	if (-1==ret) {
		return -1;
	}
	if ( mysql_set_character_set( mydb.get_mysql_handle(), "utf8" )!=0 ){
		return -1;
	}
	
	DBLTable *ptable = new DBLTable(DBLDatabase::m_pool,it->second->m_raw_name) ;
	if(!ptable) {
		return -1;
	}
	if (-1== ptable->loadtable(it->second->m_raw_name,&mydb )){
		delete ptable ;
		return -1;
	}
	delete it->second ;
	it->second = ptable ;

	DBLDatabase::s_convert_buf = 0;
	mydb.close_database() ;
#endif
	*/
	return 0;
}

DBLTable * DBLDatabase::FindTable(const char *tablename) 
{
	table_vct_t :: iterator it = m_tables.find(dbl_name(tablename)) ;
	if (it == m_tables.end()){
		return 0 ;
	}
	return it->second ;
}

//////////////////////////////////////////////////////////////////////////

#include "ndapplib/ndsingleton.h"

DBLDataNode DBL_FindDataObject_ex(const char *table, const int RowID, const char *ColName, int inputEncodeType, int outputEncode)
{
	//ND_TRACE_FUNC() ;
	//DBLDatabase*pdbl = DBLDatabase::get_Instant();
	DBLDatabase*pdbl = NDSingleton<DBLDatabase>::Get();
	if (pdbl){
		DBLTable *pTable = pdbl->FindTable(table);
		if (pTable){
			if (outputEncode == DBL_DATE_ENCODE_TYPE)	{
				return pTable->FindData((int)RowID, ColName, inputEncodeType);
			}
			else {
				DBLDataNode val = pTable->FindData((int)RowID, ColName, inputEncodeType);
				if (!val.CheckValid())
					return val;

				if (val.GetDataType() == OT_STRING || val.GetArrayType() == OT_STRING)	{
					val.ConvertEncode(DBL_DATE_ENCODE_TYPE, outputEncode);
				}
				return val;
			}
		}
	}

	return DBLDataNode();
}


DBLTable * DBL_FindTable(const char *table) 
{
	DBLDatabase*pdbl = NDSingleton<DBLDatabase>::Get();
	nd_assert(pdbl) ;
	return pdbl->FindTable(table) ;
}


int DBLDatabase::Test(const char *outPath)
{

	std::string outPathText = outPath;
	outPathText += "/_testdb_tmp1.dat";
	
	if (0!= this->Dump(outPathText.c_str(), this->getDatabaseName())) {
		nd_logerror("export database  error : %s\n", outPathText.c_str());
		return -1;
	}
	
	
	//load from text 
	//int type = DBLDatabase::GetEncodeType();
	//const char *encode = nd_get_encode_name(type);

	DBLDatabase dbtmp;
	
	if (-1== dbtmp.LoadBinStream(outPathText.c_str())) {
		nd_logerror("load from data stream error\n");
		return -1;
	}
	
	if (!(*this == dbtmp)){
		nd_logerror("comp data error \n");
		return -1;
	}

	dbtmp.Destroy();
	return 0;


}

int DBLDatabase::OutputLua(const char *path)const
{
	char filename[1024];
	std::string listFile = path;
	listFile += "/_filelist.lua";
	FILE *plistFile = fopen(listFile.c_str(), "w");
	if (!plistFile) {
		return -1;
	}

	const char *timestr = nd_get_datetimestr();
	fprintf(plistFile, "-- file: filelist.lua \n-- export time : %s \n", timestr);
	fprintf(plistFile, "-- export author: %s\n\n", nd_get_sys_username());	
	//fprintf(plistFile, "module(\"design_filelist\", package.seeall)\n\n");

	fprintf(plistFile, "filelist = {\n");
	

	for (table_vct_t::const_iterator it = m_tables.begin(); it != m_tables.end(); it++) {
		char tablename[128];
		tablename[0] = 0;
		ndstr_parse_word(it->first.c_str(), tablename);

		snprintf(filename, sizeof(filename), "%s/%s.lua", path, tablename);

		FILE *pf = fopen(filename, "w");
		if (!pf)	{
			fclose(plistFile);
			return -1;
		}

		fprintf(pf, "-- file: %s.lua \n-- export time : %s \n", tablename, timestr);

		fprintf(pf, "-- encode type : %s\n", nd_get_encode_name( GetEncodeType() ));
		fprintf(pf, "-- export author: %s\n\n", nd_get_sys_username());
		

		DBLTable *ptable = it->second;
		DBLCursor cursor(ptable);

		fprintf(pf, "\ndesign_%s = {}\n", tablename);
		
		for (int ret = cursor.FetchFirst(); ret == 0; ret = cursor.FetchNext())	{
			//fprintf(pf, "\tid%d = {",cursor[0].GetInt());
			fprintf(pf, "design_%s[%d]={", tablename, cursor[0].GetInt());
			for (int i = 0; i < cursor.GetCols(); i++)	{
				fprintf(pf, " %s=", ptable->GetColAliaName(i));
				cursor[i].Print((logic_print)fprintf, pf);
				if (i < cursor.GetCols() - 1) {
					fprintf(pf, ",");
				}
			}
			fprintf(pf, "}\n");
		}
		cursor.Close();


		fclose(pf);

		fprintf(plistFile, "\t\"%s\",\n", it->first.c_str());
	}

	fprintf(plistFile, "}\n");

	fclose(plistFile);
	return 0;
}

int DBLDatabase::TestOutput(const char *path)
{
	char filename[1024];
	std::string listFile = path;
	listFile += "/_listfile.txt";
	FILE *plistFile = fopen(listFile.c_str(), "w");
	if (!plistFile) {
		return -1;
	}
	for (table_vct_t::iterator it = m_tables.begin(); it != m_tables.end(); it++) {
		snprintf(filename, sizeof(filename), "%s/%s.txt", path, it->first.c_str());
		FILE *pf = fopen(filename, "w");
		if (!pf)	{
			fclose(plistFile);
			return -1;
		}

		DBLTable *ptable = it->second;
		DBLCursor cursor(ptable);
		for (int i = 0; i < cursor.GetCols(); i++)	{
			fprintf(pf, "%s(%s)\t", cursor.GetColName(i),ptable->GetColTypeName(i));
		}
		fprintf(pf, "\n");

		for (int ret = cursor.FetchFirst(); ret == 0; ret = cursor.FetchNext())	{
			for (int i = 0; i < cursor.GetCols(); i++)	{
				cursor[i].Print((logic_print)fprintf, pf);
				fprintf(pf, "\t");
			}
			fprintf(pf, "\n");
		}
		cursor.Close();
		fclose(pf);

		fprintf(plistFile, "%s\n", it->first.c_str());
	}

	fclose(plistFile);
	return 0;
}



DBLDatabase *DBLDatabase::get_Instant()
{
	return NDSingleton<DBLDatabase>::Get();
}
void DBLDatabase::destroy_Instant()
{
	NDSingleton<DBLDatabase>::Destroy();
}


int DBLDatabase::Create(const char *)
{
	return 0;
}
void DBLDatabase::Destroy(int )
{
	//typedef pgmap<dbl_name, DBLTable*> table_vct_t; 
	for (table_vct_t::iterator it = m_tables.begin(); it != m_tables.end(); ++it) {
		if (it->second) {
			it->second->Destroy();
			delete (it->second);
			it->second = 0;
		}
	}
	m_tables.clear();
	nd_pool_destroy(DBLDatabase::m_pool, 0);
	DBLDatabase::m_pool = 0;
	m_bLoaded = false;


#ifdef ND_DEBUG
	for (int i = 0; i < 4; i++){
		m_dbinfos[0].clear();
	}
#endif 
	m_dbName.clear();
}


const char *dbl_resource_type_to_host(const char *inputText, int encodeType)
{
	static char s_buf[4096];
#ifdef WIN32 
	if (encodeType == E_SRC_CODE_UTF_8)	{
		return nd_utf8_to_gbk(inputText, s_buf,  sizeof(s_buf) );
	}
	else {
		return inputText;
	}
#else
	if (encodeType != E_SRC_CODE_UTF_8)	{
		return nd_gbk_to_utf8(inputText, s_buf, sizeof(s_buf));
	}
	else {
		return inputText;
	}
#endif
}

//////////////////////////////////////////////////////////////////////////




