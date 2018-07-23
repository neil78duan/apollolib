/* apoAttrCalcHelper.h
 *
 * define role attribute calc heler
 *
 * create by duan 
 *
 * 2018.7.23
 */

#include "apoAttrCalcHelper.h"

#include "nd_vm/nd_vm.h"
#include "nd_common/nd_common.h"

#include <math.h>

//preparse
static int place_name_entry(char *input, char *buf, int size, void *user_data);
//运行时名字替换
static int apo_place_name_runtime(char *input, char *buf, int size, void *user_data)
{
	int i;
	apoAttrCalcHelper *waHelper = (apoAttrCalcHelper*)user_data;
	role_attr_description  *pwa = waHelper->m_wahelper_bufs;
	for (i = 0; i < waHelper->m_wa_num; i++, pwa++) {
		if (pwa->wa_id == INVALID_ATTR_ID) {
			continue;
		}
		if (0 == strcmp(input, pwa->name.c_str()) || 0 == ndstricmp(input, pwa->real_name.c_str())) {
			snprintf(buf, size, "[%d]", pwa->wa_id);
			return 0;
		}
	}


	nd_logfatal("公式解析错误:不能找到变量[%s]\n" AND input);
	return -1;
}

apoAttrCalcHelper::apoAttrCalcHelper()
{
	Destroy();
}

apoAttrCalcHelper::~apoAttrCalcHelper()
{
}

void apoAttrCalcHelper::Destroy(int)
{
	m_current_parse = 0;
	m_wa_num = 0;	

	for (int i = 0; i<APO_ATTR_CAPACITY; i++) {
		m_wahelper_bufs[i].Reset();
	}
}

int apoAttrCalcHelper::Create(const char*)
{
	return 0;
}


attrid_t apoAttrCalcHelper::GetID(const char *name)
{
	if (!name || !*name) {
		return INVALID_ATTR_ID;
	}
	role_attr_description  *pwa = m_wahelper_bufs;
	for (int i = 0; i<m_wa_num; i++, pwa++) {
		if (pwa->wa_id == INVALID_ATTR_ID) {
			continue;
		}
		else if (0 == strcmp(name, pwa->name.c_str())) {
			return pwa->wa_id;
		}
		else if (0 == ndstricmp(name, pwa->real_name.c_str())) {
			return pwa->wa_id;
		}
	}

	return INVALID_ATTR_ID;
}


const char *apoAttrCalcHelper::Getname(attrid_t wa_id)
{
	if (wa_id >= m_wa_num || m_wahelper_bufs[wa_id].wa_id == INVALID_ATTR_ID) {
		return NULL;
	}
	return m_wahelper_bufs[wa_id].name.c_str();
}

const char *apoAttrCalcHelper::GetRealname(attrid_t wa_id)
{
	if (wa_id >= m_wa_num || m_wahelper_bufs[wa_id].wa_id == INVALID_ATTR_ID) {
		return NULL;
	}
	return m_wahelper_bufs[wa_id].real_name.c_str();
}


role_attr_description *apoAttrCalcHelper::get_wa_desc(attrid_t wa_id)
{
	if (wa_id >= m_wa_num || m_wahelper_bufs[wa_id].wa_id == INVALID_ATTR_ID) {
		return NULL;
	}
	return &m_wahelper_bufs[wa_id];
}


void apoAttrCalcHelper::resetValues()
{
	for (int i = 0; i < APO_ATTR_CAPACITY; i++) {
		m_values[i] = 0;
	}
}

int apoAttrCalcHelper::CalcAll()
{
	//init vm
	int ret = 0;
	vm_cpu	formula;

	vm_machine_init(&formula, m_values, m_wa_num);

	vm_set_echo_ins(&formula, 0);
	vm_set_echo_result(&formula, 0);
	vm_set_outfunc(NULL);
	vm_set_errfunc(NULL);


	for (int i = 0; i < m_run_sort.size(); i++) {
		role_attr_description  &desc = m_wahelper_bufs[m_run_sort[i]];
		if (!desc.cmd_data.cmd_buf || desc.cmd_data.size == 0) {
			continue;
		}
		if (0 == vm_run_cmd(&formula, desc.cmd_data.cmd_buf , desc.cmd_data.size)) {
			float val = vm_return_val(&formula);
			++ret;
			m_values[m_run_sort[i]] = val;
		}
	}

	return ret;
}

bool apoAttrCalcHelper::FormulaRun(const char *text, float *result)
{
	//init vm
	bool ret = false;
	*result = 0;
	if (!text || !text[0]) {
		return true;
	}

	vm_cpu	formula;

	char cmd_buf[1024];
	vm_machine_init(&formula, m_values, m_wa_num);

	vm_set_echo_ins(&formula, 0);
	vm_set_echo_result(&formula, 0);
	vm_set_outfunc(NULL);
	vm_set_errfunc(NULL);


	//parse 
	size_t size = vm_parse_expression((char*)text, cmd_buf, sizeof(cmd_buf), (vm_param_replace_func)apo_place_name_runtime, this);
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

	return ret;
}


bool apoAttrCalcHelper::InitAttrNode(attrid_t aid, const char *name, const char*realName, const char *formula)
{
	if (aid >= APO_ATTR_CAPACITY) {
		return false;
	}
	role_attr_description node;
	node.wa_id = aid;
	node.name = name;
	node.real_name = realName;
	node.input_for = formula;

	m_wahelper_bufs[aid] = node;
	int curNum = aid + 1;
	if (m_wa_num < curNum) {
		m_wa_num = curNum;
	}
	return true;
}

int apoAttrCalcHelper::place_param_name(char *input, char *buf, int size)
{
	int i;
	role_attr_description  *pwa = m_wahelper_bufs;
	for (i = 0; i<m_wa_num; i++, pwa++) {
		if (pwa->wa_id == INVALID_ATTR_ID || !pwa->name.c_str()) {
			continue;
		}
		if (0 == strcmp(input, pwa->name.c_str()) || 0 == ndstricmp(input, pwa->real_name.c_str())) {
			snprintf(buf, size, "[%d]", pwa->wa_id);
			if (m_current_parse != i) {
				//pwa->infections[(pwa->infection_num)++] = i;		//记录属性相关
				//m_wahelper_bufs[m_current_parse].need_buf[(m_wahelper_bufs[m_current_parse].need_num)++] = i;		//记录属性相关

				pwa->infections.push_back(i);
				m_wahelper_bufs[m_current_parse].need_buf.push_back(i);
			}
			return 0;
		}
	}

	//
	nd_logfatal("公式解析错误:不能找到变量[%s]\n" AND input);
	return -1;
}


// 
// int apoAttrCalcHelper::parse_form(const char *cmd_txt, ndvm_cmd_node *cmdnode)
// {
// 	if (!cmd_txt || !cmd_txt[0]) {
// 		return -1;
// 	}
// 	cmdnode->size = (int)vm_parse_expression((char*)cmd_txt, cmdnode->cmd_buf, APO_FORMULA_SIZE, (vm_param_replace_func)apo_place_name_runtime, this);
// 	if (cmdnode->size == 0) {
// 		return -1;
// 	}
// 	return 0;
// }

int apoAttrCalcHelper::preParseFormula()
{
	//翻译公式
	role_attr_description  *pwa = m_wahelper_bufs;
	for (int i = 0; i<m_wa_num; i++, pwa++) {
		if (pwa->wa_id == INVALID_ATTR_ID) {
			continue;
		}
		
		if (pwa->input_for[0]) {
			m_current_parse = i;

			char f_buf[APO_FORMULA_SIZE] ;
			size_t size = vm_parse_expression(pwa->input_for.c_str(),f_buf, sizeof(f_buf), (vm_param_replace_func)place_name_entry, this);
			if (size == 0) {
				nd_logfatal("解析公式%s错误 \n row = %d " AND(char*)pwa->input_for AND i + 1);
				return -1;
			}
			pwa->cmd_data.Setcmd(f_buf, (int)size);
		}
	}


	if (parse_run_sort() == -1) {
		return -1;
	}
 
	return 0;

}


int apoAttrCalcHelper::check_canbe_run(role_attr_description *node, attrid_t *buf)
{
	for (int i = 0; i<node->need_buf.size(); i++) {
		if (buf[node->need_buf[i]] == 0) {
			return 1;
		}
	}
	return 0;

}

//解析公式的执行顺序,如果A使用了B,那么应该先运算B
int apoAttrCalcHelper::parse_run_sort()
{
	int i;
	int number = m_wa_num;
	int done = 0;

	attrid_t walked[APO_ATTR_CAPACITY *2];

	memset(walked, 0, sizeof(walked));

	do {
		done = 0;
		for (i = 0; i<m_wa_num; i++) {
			if (m_wahelper_bufs[i].wa_id == INVALID_ATTR_ID) {
				continue;
			}

			if (walked[m_wahelper_bufs[i].wa_id]) {
				continue;
			}
			if (m_wahelper_bufs[i].cmd_data.size > 0) {
				if (0 == check_canbe_run(&m_wahelper_bufs[i], walked)) {
					m_run_sort.push_back(m_wahelper_bufs[i].wa_id);
					//m_run_sort.sort_buf[m_run_sort.num++] = m_wahelper_bufs[i].wa_id;
					walked[m_wahelper_bufs[i].wa_id] = 1;
				}
				else {
					++done;
				}
			}
			else {
				walked[m_wahelper_bufs[i].wa_id] = 1;
			}
		}
	} while (done && (--number > 0));
	if (number <= 0) {

		nd_logfatal("公式解析错误,变量相互嵌套,请检查公式");
		//show_error( "公式嵌套过深,请检查公式")  ;
		return -1;
	}

	return 0;
}


int place_name_entry(char *input, char *buf, int size, void *user_data)
{
	apoAttrCalcHelper *waHelper = (apoAttrCalcHelper*)user_data;
	if (!waHelper) {
		return -1;
	}

	return waHelper->place_param_name(input, buf, size);
}
