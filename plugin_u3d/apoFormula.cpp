/* file apoFormula.cpp
 *
 * role attribute formula for client
 *
 * create by duan 
 *
 * 2017.4.18
 */

#include "apoFormula.h"
#include "nd_common/nd_common.h"
#include "nd_vm/nd_vm.h"
#include "apoAttrCalcHelper.h"

// 
// #define APO_ATTR_CAPACITY 200
// 
// #define  APO_FORMULA_SIZE 1024 
// #define ATTR_NAME_SIZE 64
// 
// struct apoAttrInfo
// {
// 	char name[ATTR_NAME_SIZE];
// 	char alias[ATTR_NAME_SIZE];
// 
// 	apoAttrInfo()
// 	{
// 		name[0] = 0;
// 		alias[0] = 0;
// 	}
// };
// 
// static int _s_capacity = 1;
// static apoAttrInfo _s_attrName[APO_ATTR_CAPACITY];
// static vm_value apoAttrValue[APO_ATTR_CAPACITY] = { 0 };

static apoAttrCalcHelper _g_default_helper;
void apoFormulaSetEncode(const char *encodeName)
{
	ndstr_set_code(nd_get_encode_val(encodeName));
}

bool apoFormulaNameInit(int id, const char *attrName, const char *attrAlias)
{
	if (id >= APO_ATTR_CAPACITY){
		nd_logmsg("id=%d is overflow capacity=%d\n", id, APO_ATTR_CAPACITY);
		return false;
	}
	return _g_default_helper.InitAttrNode(id, attrName, attrAlias, NULL);

// 	apoAttrInfo *pAttr = &_s_attrName[id]; 
// 
// 	if (attrName){
// 		strncpy(pAttr->name, attrName, sizeof(pAttr->name));
// 	}
// 
// 	if (attrAlias){
// 		strncpy(pAttr->alias, attrAlias, sizeof(pAttr->alias));
// 	}
// 	if (_s_capacity  <= id)	{
// 		_s_capacity = id + 1;
// 	}
	//return true;
}

bool apoFormulaSetValue(int id, float value)
{
	return _g_default_helper.setValue(id, value);
// 	if (id >= APO_ATTR_CAPACITY){
// 		nd_logmsg("id=%d is overflow capacity=%d\n", id, APO_ATTR_CAPACITY);
// 		return false;
// 	}
// 	apoAttrValue[id] = value;
	//return true;
}
bool apoFormulaAddValue(int id, float value)
{
	return _g_default_helper.setValue(id, value);
// 	if (id >= APO_ATTR_CAPACITY) {
// 		nd_logmsg("id=%d is overflow capacity=%d\n", id, APO_ATTR_CAPACITY);
// 		return false;
// 	}
// 	apoAttrValue[id] += value;
// 	return true;
}

void apoFromulaResetValue()
{
	_g_default_helper.resetValues();
// 	for (size_t i = 0; i < APO_ATTR_CAPACITY; i++)	{
// 		apoAttrValue[i] = 0;
// 	}
}
// 
// static int place_name_runtime(const char *input, char *buf, int size, void *user_data)
// {
// 	for (int i = 0; i < _s_capacity; i++)	{
// 		if (ndstricmp(_s_attrName[i].alias, input) == 0 || ndstricmp(_s_attrName[i].name, input) == 0){
// 			snprintf(buf, size, "[%d]", i);
// 			return 0;
// 		}
// 	}
// 
// 	//nd_logerror("parse formula error can not found %s\n", input);
// 	return -1;
// }


bool apoFormulaRun(const char *text, float *result)
{
	return	 _g_default_helper.FormulaRun(text, result);
// 	bool ret = false;
// 	*result = 0;
// 	if (!text || !text[0]){		
// 		return true;
// 	}
// 
// 	vm_cpu	formula;
// 
// 	char cmd_buf[1024];
// 	vm_machine_init(&formula, apoAttrValue, _s_capacity);
// 
// 	vm_set_echo_ins(&formula, 0);
// 	vm_set_echo_result(&formula, 0);
// 	vm_set_outfunc(NULL);
// 	vm_set_errfunc(NULL);
// 
// 
// 	//parse 
// 	size_t size = vm_parse_expression((char*)text, cmd_buf, sizeof(cmd_buf), place_name_runtime, NULL);
// 	if (size > 0) {
// 		if (0 == vm_run_cmd(&formula, cmd_buf, size)) {
// 			*result = vm_return_val(&formula);
// 			ret = true;
// 		}
// 		else {
// 			//nd_logerror("run formula %s error\n", text);
// 		}
// 	}
// 	else {
// 		//nd_logerror("parse formula error %s\n", text);
// 	}
// 
// 	//__cur_vm = NULL;
// 	return ret;

}

//new api

void *apoCreateAttrForHelper()
{
	apoAttrCalcHelper *pHelper = new apoAttrCalcHelper();
	return pHelper;
}
void apoDestroyAttrForHelper(void *p)
{
	apoAttrCalcHelper *pHelper = (apoAttrCalcHelper*)p;
	nd_assert(pHelper);
	delete pHelper;

}
void apoAttrForInit(void *helper, int aid, const char *name, const char*realName, const char *formula)
{
	apoAttrCalcHelper *pHelper = (apoAttrCalcHelper*)helper;
	nd_assert(pHelper);

	if (!pHelper->InitAttrNode(aid, name, realName, formula)) {
		nd_logerror("init attr %d %s error\n", aid, name);
	}

}
void apoAttrForPreParse(void *helper)
{
	apoAttrCalcHelper *pHelper = (apoAttrCalcHelper*)helper;
	nd_assert(pHelper);

	pHelper->preParseFormula();

}

bool apoAttrForSetValue(void *helper, int id, float value)
{
	apoAttrCalcHelper *pHelper = (apoAttrCalcHelper*)helper;
	nd_assert(pHelper);
	return pHelper->setValue(id,value);
}
bool apoAttrForAddValue(void *helper, int id, float value)
{
	apoAttrCalcHelper *pHelper = (apoAttrCalcHelper*)helper;
	nd_assert(pHelper);
	return pHelper->addValue(id,value);
}
float apoAttrForGetValue(void *helper, int id)
{
	apoAttrCalcHelper *pHelper = (apoAttrCalcHelper*)helper;
	nd_assert(pHelper);
	return pHelper->getValue(id);
}

void apoAttrForResetValues(void *helper)
{

	apoAttrCalcHelper *pHelper = (apoAttrCalcHelper*)helper;
	nd_assert(pHelper);
	pHelper->resetValues();
}

int apoAttrForGetCount(void *helper)
{

	apoAttrCalcHelper *pHelper = (apoAttrCalcHelper*)helper;
	nd_assert(pHelper);
	return pHelper->getValueCount();
}
int apoAttrForCalcAll(void *helper)
{
	apoAttrCalcHelper *pHelper = (apoAttrCalcHelper*)helper;
	nd_assert(pHelper);

	return pHelper->CalcAll();
}

bool apoAttrForRun(void *helper, const char *formulaText, float *result)
{
	apoAttrCalcHelper *pHelper = (apoAttrCalcHelper*)helper;
	nd_assert(pHelper);

	return	 pHelper->FormulaRun(formulaText, result);
}