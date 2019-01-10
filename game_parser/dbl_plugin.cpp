/* file dbl_plugin.cpp
 *
 * plugin api from dbl (load excel data) 
 *
 * create by duan
 *
 * all rights reserved 
 *
 * 2018.12.4
 */

#include "dbl_mgr.h"
#include "logic_parser/logicEngineRoot.h"
#include "logic_parser/objectBaseMgr.h"

static bool getDblTableName(LogicObjectBase *pObj, const char* cmd, const LogicDataObj& id, LogicDataObj &val)
{
	if (0 == ndstricmp(cmd, "dbl_excel")) {
		DBLDatabase *pdbl = DBLDatabase::get_Instant();
		if (!pdbl) {
			nd_logerror("need load excel data");
			return false;
		}
		int encode = pdbl->GetEncodeType();
		val.InitReservedArray(pdbl->m_tables.size(), OT_STRING);

		DBLDatabase::table_vct_t::iterator it = pdbl->m_tables.begin();
		for (; it != pdbl->m_tables.end(); it++) {

			if (encode != E_SRC_CODE_UTF_8) {
				LogicDataObj data;
				data.InitSet(it->first.c_str());
				data.ConvertEncode(encode, E_SRC_CODE_UTF_8);

				val.pushArray(data);
			}
			else {
				val.pushArray(LogicDataObj(it->first.c_str()));
			}
		}
		return true;
	}
	else if (0 == ndstricmp(cmd, "dbl_excel_col")) {
		DBLDatabase *pdbl = DBLDatabase::get_Instant();
		if (!pdbl) {
			nd_logerror("need load excel data");
			return false;
		}

		int encode = pdbl->GetEncodeType();
		const char *pname = id.GetText();

		if (!pname || 0 == ndstricmp((char*)pname, (char*)"none")) {
			nd_logerror("param not be none");
			return false;
		}
		DBLTable *ptable = pdbl->FindTable(pname);
		if (!ptable) {
			nd_logerror("table not found");
			return false;
		}

		int total = ptable->GetCols();
		val.InitReservedArray(total, OT_STRING);

		for (int i = 0; i < total; i++) {
			if (encode != E_SRC_CODE_UTF_8) {
				LogicDataObj data;
				data.InitSet(ptable->GetColName(i));
				data.ConvertEncode(encode, E_SRC_CODE_UTF_8);

				val.pushArray(data);
			}
			else {
				val.pushArray(LogicDataObj(ptable->GetColName(i)));
			}

		}
		return true;
	}
	return false;
}


CPPAPI  DLL_EXPORT void nodeflow_plugin_init()
{
	DBLDatabase *pdbl = DBLDatabase::get_Instant();
	if (!pdbl) {
		nd_logerror("need load excel data");
		return;
	}
	if (-1 == pdbl->LoadBinStream("../../../data/cehua_data.dat")) {
		nd_logerror("dbl init : can not open excel data file\n");
		return;
	}

	LogicParserEngine &parser = LogicEngineRoot::get_Instant()->getGlobalParser();
	LogicObjectBase *pobjOwner = (LogicObjectBase *)parser.getOwner();
	pobjOwner->setOperateCmdFunction(getDblTableName);

	
}

CPPAPI  DLL_EXPORT void nodeflow_plugin_destroy()
{
	LogicParserEngine &parser = LogicEngineRoot::get_Instant()->getGlobalParser();
	LogicObjectBase *pobjOwner = (LogicObjectBase *)parser.getOwner();
	pobjOwner->removeOperateCmdFunction(getDblTableName);
	DBLDatabase *pdbl = DBLDatabase::get_Instant();
	if (pdbl) {
		pdbl->Destroy();
	}
}
CPPAPI  DLL_EXPORT void nodeflow_plugin_run()
{
	nd_logmsg("plugin dbl not need this function\n") ;
}
CPPAPI  DLL_EXPORT const char* nodeflow_plugin_name()
{
	return "dbl" ;
}
