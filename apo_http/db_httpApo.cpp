/* file db_httpApo.h
 *
 * mysql database interface for aposcript
 *
 * create by duan
 *
 * 2018.4.17
 */

#include "game_parser/dbldata2netstream.h"
#include "apo_http/httpScriptApi.h"
#include "apo_http/db_httpApo.h"
#include "db_common/mydatabase.h"

APOLLO_SCRIPT_API_DEF(apoDb_mysql_open, "db_连接数据库(host,port,user, passwd, dbInst)")
{
	ND_TRACE_FUNC();
	CHECK_ARGS_NUM(args, 5, parser);
	const char *dbInst = NULL;
	CMyDatabase *pDBconn = new CMyDatabase;
	nd_assert(pDBconn);
	if (args.size() >= 6 && args[5].CheckValid()) {
		dbInst = args[5].GetText();
	}

	int ret = pDBconn->open_database(args[1].GetText(), args[2].GetInt(), args[3].GetText(), args[4].GetText(), dbInst);
	if (-1 == ret) {
		nd_logerror("connect to mysql %s user=%s \n", args[1].GetText(), args[3].GetText());
		parser->setErrno(NDERR_HOST_UNAVAILABLE);
		return false;
	}
	pDBconn->set_thread(nd_thread_self());
	result.InitSet((void*)pDBconn);

	return true;
}


APOLLO_SCRIPT_API_DEF(apoDb_mysql_close, "db_关闭连接(db_connector)")
{
	ND_TRACE_FUNC();
	CHECK_ARGS_NUM(args, 2, parser);
	CHECK_DATA_TYPE(args[1], OT_OBJECT_VOID, parser);

	CMyDatabase *pDBconn = (CMyDatabase *)args[1].GetObjectAddr();
	if (pDBconn) {
		pDBconn->close_database();
	}
	else {
		parser->setErrno(NDERR_PARAM_INVALID);
		return false;
	}

	return true;
}


APOLLO_SCRIPT_API_DEF(apoDb_mysql_runsql, "db_执行sql(db_connector, sql_text)")
{
	ND_TRACE_FUNC();
	CHECK_ARGS_NUM(args, 3, parser);
	CHECK_DATA_TYPE(args[1], OT_OBJECT_VOID, parser);
	CHECK_DATA_TYPE(args[2], OT_STRING, parser);

	CMyDatabase *pDBconn = (CMyDatabase *)args[1].GetObjectAddr();
	if (!pDBconn) {		
		parser->setErrno(NDERR_PARAM_INVALID);
		return false;
	}
	const char *pInput = args[2].GetText();
	if (!pInput){

		parser->setErrno(NDERR_PARAM_INVALID);
		return false;
	}

	char sql_buf[4096];
	char *pOutaddr = sql_buf;
	size_t size = sizeof(sql_buf);

	do {
		const char *pVarName = strchr(pInput, '$');
		if (pVarName) {
			//find var name 
			char myName[128];
			int len = ndstr_parse_variant_n(pVarName, myName,sizeof(myName));
			if (len == 0) {
				nd_logerror("parse %s error\n", pVarName);
				parser->setErrno(NDERR_PARAM_INVALID);
				return false;
			}
			int offset = 1;
			if (IS_NUMERALS(myName[1]) || 0 == ndstricmp(&myName[1], "value")) {
				offset = 0;
			}
			LogicDataObj var1;
			if (!parser->getVarValue(&myName[offset], var1)) {
				nd_logerror("can not found %s \n", pVarName);
				parser->setErrno(NDERR_PARAM_INVALID);
				return false;
			}
			if (!var1.CheckValid()) {
				nd_logerror("var %s is invalid\n", pVarName);
				parser->setErrno(NDERR_PARAM_INVALID);
				return false;
			}
			
			//copy before this var
			size_t datalen = pVarName - pInput;
			memcpy(pOutaddr, pInput, datalen);			
			pOutaddr += datalen;
			size -= datalen;
			pInput = pVarName + len;

			//copy this var text-value
			if (var1.GetDataType() == OT_BINARY_DATA) {
				datalen =(size_t) pDBconn->escape_sql_string(pOutaddr, (char*)var1.GetBinary(), var1.GetBinarySize());
			}
			else {
				std::string varString = var1.GetString();
				if (varString.empty()) {
					nd_logerror("var %s is invalid\n", pVarName);
					parser->setErrno(NDERR_PARAM_INVALID);
					return false;
				}
				datalen =(size_t) pDBconn->escape_sql_string(pOutaddr, (char*)varString.c_str(), varString.size());
			}
			pOutaddr += datalen;
			size -= datalen;
		}
		else {
			strncpy(pOutaddr, pInput, size);
			break;
		}
	} while (pInput);

	int ret = pDBconn->sql_execute(sql_buf);
	if (-1 == ret) {
		nd_logerror("run %s \b", sql_buf);
		parser->setErrno(NDERR_MYSQL);
		return false;
	}

	return true;
}


APOLLO_SCRIPT_API_DEF(apoDb_mysql_store, "db_保存结果(db_connector)return rows")
{
	ND_TRACE_FUNC();
	CHECK_ARGS_NUM(args, 2, parser);
	CHECK_DATA_TYPE(args[1], OT_OBJECT_VOID, parser);

	CMyDatabase *pDBconn = (CMyDatabase *)args[1].GetObjectAddr();
	if (!pDBconn) {
		parser->setErrno(NDERR_PARAM_INVALID);
		return false;
	}

	pDBconn->sql_store_result();

	int num = pDBconn->sql_num_rows();
	result.InitSet(num);

	return true;
}

APOLLO_SCRIPT_API_DEF(apoDb_mysql_release, "db_释放结果(db_connector)")
{
	ND_TRACE_FUNC();
	CHECK_ARGS_NUM(args, 2, parser);
	CHECK_DATA_TYPE(args[1], OT_OBJECT_VOID, parser);

	CMyDatabase *pDBconn = (CMyDatabase *)args[1].GetObjectAddr();
	if (!pDBconn) {
		parser->setErrno(NDERR_PARAM_INVALID);
		return false;
	}

	pDBconn->sql_free_result();
	return true;
}

APOLLO_SCRIPT_API_DEF(apoDb_mysql_num_rows, "db_结果行数(db_connector)")
{
	ND_TRACE_FUNC();
	CHECK_ARGS_NUM(args, 2, parser);
	CHECK_DATA_TYPE(args[1], OT_OBJECT_VOID, parser);

	CMyDatabase *pDBconn = (CMyDatabase *)args[1].GetObjectAddr();
	if (!pDBconn) {
		parser->setErrno(NDERR_PARAM_INVALID);
		return false;
	}

	int num = pDBconn->sql_num_rows();
	result.InitSet(num);

	return true;
}


APOLLO_SCRIPT_API_DEF(apoDb_mysql_num_cols, "db_结果列数(db_connector)")
{
	ND_TRACE_FUNC();
	CHECK_ARGS_NUM(args, 2, parser);
	CHECK_DATA_TYPE(args[1], OT_OBJECT_VOID, parser);

	CMyDatabase *pDBconn = (CMyDatabase *)args[1].GetObjectAddr();
	if (!pDBconn) {
		parser->setErrno(NDERR_PARAM_INVALID);
		return false;
	}

	int num = pDBconn->sql_num_fields();
	result.InitSet(num);

	return true;
}


APOLLO_SCRIPT_API_DEF(apoDb_mysql_insert_id, "db_当前插入ID(db_connector)")
{
	ND_TRACE_FUNC();

	CHECK_ARGS_NUM(args, 2, parser);
	CHECK_DATA_TYPE(args[1], OT_OBJECT_VOID, parser);

	CMyDatabase *pDBconn = (CMyDatabase *)args[1].GetObjectAddr();
	if (!pDBconn) {
		parser->setErrno(NDERR_PARAM_INVALID);
		return false;
	}

	NDUINT32 num = pDBconn->sql_insert_id();
	result.InitSet((int)num);

	return true;
}

APOLLO_SCRIPT_API_DEF(apoDb_mysql_affected_rows, "db_受影响记录数(db_connector)")
{
	ND_TRACE_FUNC();
	CHECK_ARGS_NUM(args, 2, parser);
	CHECK_DATA_TYPE(args[1], OT_OBJECT_VOID, parser);

	CMyDatabase *pDBconn = (CMyDatabase *)args[1].GetObjectAddr();
	if (!pDBconn) {
		parser->setErrno(NDERR_PARAM_INVALID);
		return false;
	}

	int num = pDBconn->sql_affected_rows();
	result.InitSet(num);

	return true;
}

APOLLO_SCRIPT_API_DEF(apoDb_mysql_fetch, "db_提取当前行(db_connector)")
{
	ND_TRACE_FUNC();
	CHECK_ARGS_NUM(args, 2, parser);
	CHECK_DATA_TYPE(args[1], OT_OBJECT_VOID, parser);

	CMyDatabase *pDBconn = (CMyDatabase *)args[1].GetObjectAddr();
	if (!pDBconn) {
		parser->setErrno(NDERR_PARAM_INVALID);
		return false;
	}
	ROW_SQL rows;

	if ((rows = pDBconn->sql_fetch()) != NULL) {
		LogicUserDefStruct usrData;
		int nCol = pDBconn->sql_num_fields();
		unsigned long*pLength = pDBconn->sql_fetch_lengths();

		for (int i = 0; i < nCol; i++){
			FIELD_INFO pField = pDBconn->sql_field_info(i);
			if (!pField) {
				continue;
			}

			if (pField->type == MYSQL_TYPE_TINY_BLOB,
				MYSQL_TYPE_MEDIUM_BLOB == pField->type,
				MYSQL_TYPE_LONG_BLOB == pField->type,
				MYSQL_TYPE_BLOB == pField->type) {

				usrData.push_back(pField->name, LogicDataObj((void*)rows[i], (size_t)pLength[i]));
			}
			else {
				usrData.push_back(pField->name, LogicDataObj((const char*)rows[i]));
			}
		}
		result.InitSet(usrData);
		return true;
	}
	return false;
}

int apoMysqlDBInit()
{
	ND_TRACE_FUNC();
	nd_logmsg("mysql api for apollo init success!\n");
	return 0;
}