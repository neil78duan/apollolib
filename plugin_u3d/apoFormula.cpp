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

#define APO_ATTR_CAPACITY 200

#define  APO_FORMULA_SIZE 1024 
#define ATTR_NAME_SIZE 64

struct apoAttrInfo
{
	char name[ATTR_NAME_SIZE];
	char alias[ATTR_NAME_SIZE];

	apoAttrInfo()
	{
		name[0] = 0;
		alias[0] = 0;
	}
};

static int _s_capacity = 1;
static apoAttrInfo _s_attrName[APO_ATTR_CAPACITY];
static vm_value apoAttrValue[APO_ATTR_CAPACITY] = { 0 };


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
	apoAttrInfo *pAttr = &_s_attrName[id]; 

	if (attrName){
		strncpy(pAttr->name, attrName, sizeof(pAttr->name));
	}

	if (attrAlias){
		strncpy(pAttr->alias, attrAlias, sizeof(pAttr->alias));
	}
	if (_s_capacity  <= id)	{
		_s_capacity = id + 1;
	}
	return true;
}

bool apoFormulaSetValue(int id, float value)
{
	if (id >= APO_ATTR_CAPACITY){
		nd_logmsg("id=%d is overflow capacity=%d\n", id, APO_ATTR_CAPACITY);
		return false;
	}
	apoAttrValue[id] = value;
	return true;

}

void apoFromulaResetValue()
{
	for (size_t i = 0; i < APO_ATTR_CAPACITY; i++)	{
		apoAttrValue[i] = 0;
	}
}
//static __ndthread vm_cpu *__cur_vm = NULL;
//find var address
static int place_name_runtime(const char *input, char *buf, int size, void *user_data)
{
	for (int i = 0; i < _s_capacity; i++)	{
		if (ndstricmp(_s_attrName[i].alias, input) == 0 || ndstricmp(_s_attrName[i].name, input) == 0){
			snprintf(buf, size, "[%d]", i);
			return 0;
		}
	}

	nd_logerror("parse formula error can not found %s\n", input);
	return -1;
}


bool apoFormulaRun(const char *text, float *result)
{
	//init vm
	bool ret = false;
	*result = 0;
	if (!text || !text[0]){		
		return true;
	}

	vm_cpu	formula;

	char cmd_buf[1024];
	vm_machine_init(&formula, apoAttrValue, _s_capacity);

	vm_set_echo_ins(&formula, 0);
	vm_set_echo_result(&formula, 0);
	vm_set_outfunc(NULL);
	vm_set_errfunc(NULL);


	//parse 
	size_t size = vm_parse_expression((char*)text, cmd_buf, sizeof(cmd_buf), place_name_runtime, NULL);
	if (size > 0) {
		if (0 == vm_run_cmd(&formula, cmd_buf, size)) {
			*result = vm_return_val(&formula);
			ret = true;
		}
		else {
			nd_logerror("run formula %s error\n", text);
		}
	}
	else {
		nd_logerror("parse formula error %s\n", text);
	}

	//__cur_vm = NULL;
	return ret;

}