/* apoAttrCalcHelper.h
 *
 * define role attribute calc heler
 *
 * create by duan 
 *
 * 2018.7.23
 */

#ifndef _APOATTR_CALC_HELPER_H_
#define _APOATTR_CALC_HELPER_H_


#include "nd_common/nd_common.h"
#include "nd_vm/nd_vm.h"
#include <vector>
#include <string>
 ///-------------------------------------------------------------------

#define APO_ATTR_CAPACITY 512
#define APO_FORMULA_SIZE 1024 

typedef unsigned short attrid_t;
typedef float attrval_t;

#define INVALID_ATTR_ID (attrid_t)-1
#define ATTR_VALUE_DETA 0.0001f

//ndvm instruct stream
struct ndvm_cmd_node
{
	int size;
	char *cmd_buf;
	ndvm_cmd_node() :size(0), cmd_buf(0)
	{

	}
	~ndvm_cmd_node() {
		Reset();
	}

	void Setcmd(void *p, int s) {
		cmd_buf = (char*) malloc(s + 1);
		memcpy(cmd_buf, p, s);
		size = s;
		cmd_buf[s] = 0;
	}
	void Reset() {
		if (cmd_buf) {
			free(cmd_buf);
		}
		cmd_buf = 0;
		size = 0;
	}
};

typedef std::vector<attrid_t> attrid_vct_t;

struct role_attr_description {
	attrid_t wa_id;
	std::string name;
	std::string real_name;
	std::string input_for;

	attrid_vct_t infections;	//the attribute id lists infected by current 
	attrid_vct_t need_buf;		//needed by current
	ndvm_cmd_node cmd_data;
	role_attr_description() : wa_id(0)
	{}
	void Reset() {
		wa_id = 0;
		name.clear();
		real_name.clear();
		input_for.clear();

		infections.clear();	
		need_buf.clear();
		cmd_data.Reset();
	}
};


//attribute calc helper 
class apoAttrCalcHelper
{
public:
	attrid_t GetID(const char *name);
	inline attrid_t GetIDByRealName(const char *name) { return GetID(name); }	
	const char *Getname(attrid_t wa_id);
	const char *GetRealname(attrid_t wa_id);	
	int GetAttrNum() { return m_wa_num; }
	
	int preParseFormula();

	role_attr_description *get_wa_desc(attrid_t wa_id);	

	bool InitAttrNode(attrid_t aid, const char *name, const char*realName, const char *formula);

	bool setValue(attrid_t aid, attrval_t val) { m_values[aid] = val; return true; }
	bool addValue(attrid_t aid, attrval_t val) { m_values[aid] += val; return true; }
	
	attrval_t getValue(attrid_t aid) { return m_values[aid] ; }
	void resetValues();

	int CalcAll();
	float calcOne(attrid_t aid);

	bool FormulaRun(const char *formulaText, float *result);
	int getValueCount() { return m_wa_num; }

	bool getInfections(attrid_t aid, attrid_vct_t &attrids);


	apoAttrCalcHelper();
	~apoAttrCalcHelper();
	int place_param_name(char *input, char *buf, int size);
private:
	void Destroy(int flag = 0);
	int Create(const char*name = NULL);

	int parse_run_sort();
	int check_canbe_run(role_attr_description *node, attrid_t *buf);
public:
	int m_current_parse;
	
	static int m_wa_num;
	static  attrid_vct_t m_run_sort;
	static  role_attr_description m_wahelper_bufs[APO_ATTR_CAPACITY];

	attrval_t m_values[APO_ATTR_CAPACITY];
};

#endif
