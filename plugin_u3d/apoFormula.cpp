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

static apoAttrCalcHelper _g_default_helper;
void apoFormulaSetEncode(const char *encodeName)
{
	ndstr_set_code(nd_get_encode_val(encodeName));
}

bool apoFormulaNameInit(int id, const char *attrName, const char *attrAlias, const char *formula)
{
	if (id >= APO_ATTR_CAPACITY){
		nd_logmsg("id=%d is overflow capacity=%d\n", id, APO_ATTR_CAPACITY);
		return false;
	}
	return _g_default_helper.InitAttrNode(id, attrName, attrAlias, formula);
}

void apoFormulaPreParse()
{
	_g_default_helper.preParseFormula();
}

bool apoFormulaSetValue(int id, float value)
{
	return _g_default_helper.setValue(id, value);

}
bool apoFormulaAddValue(int id, float value)
{
	return _g_default_helper.setValue(id, value);

}

void apoFromulaResetValue()
{
	_g_default_helper.resetValues();

}

int apoFromulaGetCalcSort(int outputIds[], int bufsize)
{
	int ret = 0;
	for (int i = 0; i < _g_default_helper.m_run_sort.size() && i<bufsize; i++) {
		outputIds[ret++] = (int)_g_default_helper.m_run_sort[i];
	}
	return ret;
}


bool apoFormulaRun(const char *text, float *result)
{
	return	 _g_default_helper.FormulaRun(text, result);

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

bool apoAttrForSetValues(void *helper, float value[], int count)
{
	apoAttrCalcHelper *pHelper = (apoAttrCalcHelper*)helper;
	nd_assert(pHelper);
	for (int i = 0; i < count && i < APO_ATTR_CAPACITY; i++)
		pHelper->m_values[i] = value[i];
	return true;
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

int apoAttrForGetValues(void *helper, float value[], int count)
{
	apoAttrCalcHelper *pHelper = (apoAttrCalcHelper*)helper;
	nd_assert(pHelper);
	int num = 0;
	for (int i = 0; i < count && i < apoAttrCalcHelper:: m_wa_num; i++) {
		value[i] = pHelper->m_values[i];
		++num;
	}
	return num;
}

float apoAttrForCalcOne(void *helper, int attrId)
{
	apoAttrCalcHelper *pHelper = (apoAttrCalcHelper*)helper;
	nd_assert(pHelper);

	return pHelper->calcOne(attrId);
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


int apoAttrForGetInfections(void *helper, int attrid, int outputIds[], int bufsize)
{
	apoAttrCalcHelper *pHelper = (apoAttrCalcHelper*)helper;
	nd_assert(pHelper);

	int ret = 0;
	attrid_vct_t idvct;
	if (pHelper->getInfections((attrid_t)attrid, idvct)) {

		for (int i = 0; i < idvct.size() && i<bufsize; i++) {
			outputIds[ret++] = (int)idvct[i];
		}
	}
	return ret;
}