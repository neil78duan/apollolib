/* file pg_dbelement.h
 *
 * define data element of static data
 *
 * create by duan 
 * 
 * 2012/7/4 10:43:25
 * modify from mysql to text file
 */

/* 
 Read data from excel. First you need convert excel to txt by python tool , and list these files in filelst you need load in dbl.
 Then read data into dbl save as DBLNodeData
 */

#ifndef _DBL_MGR_H_
#define _DBL_MGR_H_


#include "logic_parser/logicDataType.h"

#define EXCEL_SECOND_LINE_AS_NAME  //excel second line is realname , first line is alias name 

#define DBL_NAME_SIZE 128 
typedef std::string dbl_name ;

class DBLObjBase
{
public:
// 	void *operator new(size_t size);
// 	void *operator new[](size_t size);
// 	void operator delete(void *p) throw();
// 	void operator delete[](void *p) throw();
};

typedef std::vector<std::string> strings_vct;

class DBLRecord : public DBLObjBase
{
public:
	DBLRecord() ;
	~DBLRecord() ;
	int Create(int col) ;
	void Destroy() ;

	dbl_element_base& operator[] (int index)
	{
		nd_assert(index < m_col);
		return m_data_buf[index] ;
	}
	dbl_element_base *GetOrgVal(int index)
	{

		nd_assert(index < m_col);
		return &m_data_buf[index] ;
	}
protected:
	DBLRecord & operator=(DBLRecord&);
public:
	NDUINT8 m_col ;
	dbl_element_base *m_data_buf ;
};

class DBLTable : public DBLObjBase
{
public:
	DBLTable(nd_handle pool, const char *name=0) ;
	virtual ~DBLTable() ;

	void Destroy() ;
	struct col_info
	{
		DBL_ELEMENT_TYPE type;
		DBL_ELEMENT_TYPE sub_type ;
		char col_alias[DBL_NAME_SIZE]; 	//alias name , excel first line
		char col_name[DBL_NAME_SIZE];	// real name , second line
	};
	friend class DBLCursor ;

	typedef std::map<NDUINT32 , DBLRecord*>table_map_t ;
	typedef table_map_t::iterator row_iterator ;

	DBLDataNode FindData(int id, const char *col_name, int encodeType = ND_ENCODE_TYPE);
	NDUINT32 GetRowIDByindex(int index) ;

	int GetColIndex(const char *col_name, int encodeType = ND_ENCODE_TYPE);

	int GetRows() ;
	int GetCols() ;

	const char *GetColName(int index ) ;
	const char *GetColAliaName(int index) ;	//the second line name 
	const char *GetColTypeName(int index);
	int loadFromText(const char *tablename, int codeType) ;

	int Dump(void *pfile) ;
	int LoadBinStream(void *pfile);

protected:
	int parseColType(const char *colText);
	int parseTableTypeInfo(FILE *pf, int encodeType);
	void ReleaseRecord(DBLRecord *) ;
	int m_cols ;
	col_info *m_pcols ;
	table_map_t m_rocords;
public:
#ifdef ND_DEBUG
	char m_raw_name[32] ;
#endif 
};


class DBLCursor
{
public:
	DBLCursor(DBLTable *ptb); 
	DBLCursor(DBLTable *ptb, const char*col_names[], int col_num, int encodeType = E_SRC_CODE_ANSI);
	DBLCursor();
	~DBLCursor() ;

	int FetchFirst() ;		
	int FetchNext() ;		
	int Fetch(int id) ;		
	void Close() ;			

	const char *GetColName(int index) ;
	int GetCols() ;

	bool GetDataNode(const char* _pszColName, DBLDataNode& _dataNode,  int encodeType = E_SRC_CODE_ANSI);
	DBLDataNode GetDataNode(const char* _pszColName, int encodeType = E_SRC_CODE_ANSI);
	DBLDataNode &operator[](int index);

private:
	int _fetch() ;
	NDUINT8 m_bAllcol:1 ;	
	NDUINT8 m_bOpen:1 ;	
	typedef std::vector<int> select_col_vct ;
	typedef std::vector<DBLDataNode> col_data_vct ;

	select_col_vct m_sel_cols ;
	col_data_vct m_datas;

	DBLTable *m_ptable;
	DBLTable::row_iterator m_cur_it ;
	//char m_convertbuf[DBL_NAME_SIZE];
};

class DBLDatabase
{
public:
	DBLDatabase() ;

	virtual ~DBLDatabase() ;

	static DBLDatabase *get_Instant();
	static void destroy_Instant();

	int Init();
	void Destroy();

	bool operator == (const DBLDatabase &r);

	//load from text file
	int LoadFromText(const char *datapath, const char *list_file, const char *textEncode, const char*toEncode);
	int ReloadTable(const char *table) ;
	DBLTable * FindTable(const char *tablename) ;

	int Dump(const char *file, const char *dbname, int orderType =1) ;//save 2 file 
	int LoadBinStream(const char *file) ;			//load from bin file
	

	int Test(const char *outPath); //test database is ok 
	int TestOutput(const char *path);
	int OutputLua(const char *path)const;
	int GetEncodeType()const {
		return s_data_encodeType;
	}
	const char *getDatabaseName() { return m_dbName.c_str() ;}

	typedef std::map<dbl_name, DBLTable*> table_vct_t;
	table_vct_t m_tables ;
	static nd_handle m_pool ;
	static char *s_convert_buf;
	static int s_data_encodeType;
	static int s_data_endianOrder;
	static bool s_data_needChangeOrder;

#ifdef ND_DEBUG
	std::string m_dbinfos[4] ;
#endif 
protected:
	int _loadText(const char *datapath, const char *list_file, int encodeType);
	bool m_bLoaded;
	std::string m_dbName ;
};

DBLDataNode DBL_FindDataObject_ex(const char *table, const int RowID, const char *ColName, int inputEncodeType = E_SRC_CODE_ANSI, int outputEncode = ND_ENCODE_TYPE);

#define DBL_FindDataObject DBL_FindDataObject_ex
// inline DBLDataNode DBL_FindDataObject(const char *table, const int RowID, const char *ColName, int inputEncodeType = E_SRC_CODE_ANSI, int outputEncode = ND_ENCODE_TYPE)
// {
// 	return DBL_FindDataObject_ex(table,RowID,ColName,inputEncodeType, outputEncode);
// }
DBLTable * DBL_FindTable(const char *table) ;
//int add_dbfile_useinfo(const char *file) ;

#define GET_DBL_TABLE(_tablename)	\
	DBLTable * pTable = DBL_FindTable(_tablename) ;	\
	if (!pTable){									\
		nd_logfatal("open table %serror\n" AND _tablename) ;	\
		return -1;									\
		}

#define DBL_BEGIN_CURSOR(_tablename, _fieldslist)	\
	GET_DBL_TABLE(_tablename) ;					\
	DBLCursor cursor(pTable,_fieldslist, ND_ELEMENTS_NUM(_fieldslist),ND_ENCODE_TYPE) ;		\
	for(int  nResult =cursor.FetchFirst(); nResult!=-1;  nResult=cursor.FetchNext())	
#define DBL_END_CURSOR()		}


#define DBL_FOR_EACH(_tablename, _fieldslist,_retval)	\
	DBLTable * pTable = DBL_FindTable(_tablename) ;	\
	if (!pTable){									\
		nd_logfatal("open table %serror\n" AND _tablename) ;	\
		return _retval;									\
	}													\
	DBLCursor cursor(pTable,_fieldslist, ND_ELEMENTS_NUM(_fieldslist),ND_ENCODE_TYPE) ;		\
	for(int  nResult =cursor.FetchFirst(); nResult!=-1;  nResult=cursor.FetchNext())


#define DBL_BEGIN_CURSOR_EX(_tablename, _fieldslist, _numbers)	\
	GET_DBL_TABLE(_tablename) ;					\
	DBLCursor cursor(pTable,_fieldslist, _numbers,ND_ENCODE_TYPE) ;		\
	for(int  nResult =cursor.FetchFirst(); nResult!=-1;  nResult=cursor.FetchNext())	
#endif

