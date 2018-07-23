/* file apoFormula.h
 *
 * role attribute formula for client
 *
 * create by duan 
 *
 * 2017.4.18
 */

#ifndef _APO_FORMULA_H_
#define _APO_FORMULA_H_

#include "apoCliApi.h"

APOLLO_CLI_API void apoFormulaSetEncode(const char *encodeName);

APOLLO_CLI_API bool apoFormulaNameInit(int id, const char *attrName, const char *attrAlias);
APOLLO_CLI_API bool apoFormulaSetValue(int id, float value);
APOLLO_CLI_API bool apoFormulaAddValue(int id, float value);
APOLLO_CLI_API bool apoFormulaRun(const char *formulaText, float *result);
APOLLO_CLI_API void apoFromulaResetValue();

APOLLO_CLI_API void *apoCreateAttrForHelper();
APOLLO_CLI_API void apoDestroyAttrForHelper(void *);
APOLLO_CLI_API void apoAttrForInit(void *helper, int aid, const char *name, const char*realName, const char *formula);
APOLLO_CLI_API void apoAttrForPreParse(void *helper);

APOLLO_CLI_API bool apoAttrForSetValue(void *helper,int id, float value);
APOLLO_CLI_API bool apoAttrForAddValue(void *helper, int id, float value);
APOLLO_CLI_API float apoAttrForGetValue(void *helper, int id);
APOLLO_CLI_API void apoAttrForResetValues(void *helper);
APOLLO_CLI_API int apoAttrForGetCount(void *helper);

APOLLO_CLI_API int apoAttrForCalcAll(void *helper);

APOLLO_CLI_API bool apoAttrForRun(void *helper, const char *formulaText, float *result);


#endif
