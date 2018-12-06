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


CPPAPI  DLL_EXPORT void nodeflow_init_dbl_plugin()
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
CPPAPI  DLL_EXPORT void nodeflow_destroy_dbl_plugin()
{
	LogicParserEngine &parser = LogicEngineRoot::get_Instant()->getGlobalParser();
	LogicObjectBase *pobjOwner = (LogicObjectBase *)parser.getOwner();
	pobjOwner->removeOperateCmdFunction(getDblTableName);
	DBLDatabase *pdbl = DBLDatabase::get_Instant();
	if (pdbl) {
		pdbl->Destroy();
	}
}
