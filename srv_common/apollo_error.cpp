/* file apollo_error.cpp
 * 
 * define apollo error function
 *
 * create by duan 
 *
 * 2018.8.,19
 */

#include "common_inc.h"


 //show error
const char *apollo_error(int errcode)
{
	ND_TRACE_FUNC();
	static char errdesc[256];

	const char *nd_err[] = {
		"UNKNOW_ERROR"
#undef ErrorElement 
#define ErrorElement(a,_err_desc) "game(ESERVER_"#a "):" _err_desc
#include "_error_def.h"
#undef ErrorElement 
	};

	if (errcode <= NDERR_UNKNOWN) {

		nd_error_convert  oldFunc = nd_register_error_convert(NULL);
		const char *perr = nd_error_desc(errcode);
		nd_register_error_convert(oldFunc);
		return perr;
	}
	else if (errcode > ApolloError_Start && errcode < ESERVER_ERR_NUMBER) {
		return nd_err[errcode - ApolloError_Start - 1];
	}
	else {
		snprintf(errdesc, sizeof(errdesc), "Error code =%d", errcode);
		return errdesc;
	}
}

class ApoTmpInitErrorNumber
{
public:
	ApoTmpInitErrorNumber() {
		nd_error_set_user_number(ESERVER_ERR_NUMBER);

		nd_register_error_convert(apollo_error);
	}
};

static ApoTmpInitErrorNumber _s_TmpInitErrorNumber;