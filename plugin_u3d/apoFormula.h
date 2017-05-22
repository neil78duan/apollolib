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
APOLLO_CLI_API bool apoFormulaRun(const char *formulaText, float *result);
APOLLO_CLI_API void apoFromulaResetValue();

#endif
