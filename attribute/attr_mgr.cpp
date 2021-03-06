/* file attr_mgr.cpp
 *
 * player attribute manager
 *
 * create by duan 
 *
 * 2015-5-29
 */
#include "attribute/attr_mgr.h"
#include "apollo_errors.h"
#include <math.h>
#include <string>

RoleAttrAsset::RoleAttrAsset(role_attr_data *data) : m_data(data), m_EnableRecalc(false)
{
	m_attrRate = 1.0f ;
	//m_lastErrorID = INVALID_ATTR_ID;
	//m_lastErrorCode = 0;
	m_recordChange = false;
	resetMMAddr();
}
RoleAttrAsset::~RoleAttrAsset()
{

}


bool RoleAttrAsset::Create()
{
    RoleAttrHelper *root = get_attr_helper();
    nd_assert(root) ;
    
    for (int i=0; i<root->GetAttrNum(); i++) {
        role_attr_description *pdesc = root->get_wa_desc((attrid_t)i) ;
        if(pdesc){
            m_data->datas[i].InitType((NDVarType::NDVTYPE_ELEMENT_TYPE)pdesc->data_type);
        }
    }
    return 0;
}
float RoleAttrAsset::setAttrRate(float newVal)
{
	float ret = m_attrRate ;
	m_attrRate = newVal ;
	return ret;
	//return 1.0f;
}

attrid_t RoleAttrAsset::getLastErrorID()
{
	return INVALID_ATTR_ID;
}
int RoleAttrAsset::getLastError()
{
	return 0;
}
void RoleAttrAsset::setLastErrorAttrID(attrid_t )
{
	
}
void RoleAttrAsset::setLastError(int )
{

}


int RoleAttrAsset::getChanged(attrval_node *buf, size_t size)
{
	int num =0;
	for (size_t i = 0; i < m_changed.size() && i<size; i++){
		buf[num++] = m_changed[i];
	}
	return num;
}
void RoleAttrAsset::beginRecordChange()
{
	m_recordChange = true;
	m_changed.clear();
}
void RoleAttrAsset::endRecordChange()
{
	m_recordChange = false;
	m_changed.clear();
}

bool RoleAttrAsset::setVal(attrid_t index, const attrval_t &val)
{
	ND_TRACE_FUNC();
	if (index >= m_data->datasCount){
		return false;
	}

	RoleAttrHelper  *pwah = get_attr_helper();
	if (pwah->check_readonly(index)) {
		char buf_stack[1024];
		if (!nd_get_callstack_desc(buf_stack, sizeof(buf_stack))) {
			strncpy(buf_stack, "unknow stack", sizeof(buf_stack));
		}
		nd_logerror("set [%d] value but is read only {%s}\n", index,buf_stack);
		return false;
	}

	int ret = 0;
	attrval_t *pdata = &m_data->datas[index];
	
    if(pdata->isNumber()) {
    
        if (val != *pdata) {
            ret = _set_val(val, index);
            if (ret > 0  && m_EnableRecalc){
                Recalc();
            }
        }
    }
    else {
        attrval_t oldval = *pdata ;
        *pdata = val ;
        if (CheckInAffair()){
            AffairModify(index, oldval);
        }
        else {
            OnChanged(index, *pdata, oldval);
        }
        SetDataChanged();
    }
	return true;

}

bool RoleAttrAsset::setValRaw(attrid_t index, const attrval_t & val)
{
	RoleAttrHelper  *pwah = get_attr_helper();
	role_attr_description *pdesc = pwah->get_wa_desc(index);
	if (!pdesc) {
		setLastError(NDSYS_ERR_INVALID_INPUT);
		setLastErrorAttrID(index);
		return false;
	}
	m_data->datas[index] = val;
	return true;
}

bool RoleAttrAsset::setValRaw(const char *name, const attrval_t & newval)
{
	RoleAttrHelper  *pwah = get_attr_helper();
	attrid_t waid = pwah->GetID(name);
	if (waid != INVALID_ATTR_ID){
		m_data->datas[waid] = newval;
		return true;
	}
	setLastError(NDSYS_ERR_INVALID_INPUT);
	return false;
}

bool RoleAttrAsset::setValRaw(const attr_node_buf &attrs)
{
	for (int i = 0; i < attrs.number; i++)	{
		setValRaw(attrs.buf[i].id, attrs.buf[i].val);
	}
	return true;
}

bool RoleAttrAsset::addVal(attrid_t index, const attrval_t & val)
{
    if(!m_data->datas[index].isNumber()) {
        setLastError(ESERVER_ERR_TYPE_NOT_MATCH);
        setLastErrorAttrID(index);
        return  false;
    }
    
	if (checkUnlimitMax(index)) {
		setLastError(ESERVER_ERR_ATTR_TOO_MUCH);
		setLastErrorAttrID(index);
		return false;
	}
	attrval_t addstep = val;
	if (fabsf(m_attrRate - 1.0f) > 0.001f) {
		addstep *= attrval_t(m_attrRate) ;
	}
	if (setVal(index, getVal(index) + addstep)) {

		if (m_recordChange)	{
			attrval_node node;
			node.id = index;
			node.val = addstep;
			m_changed.push_back(node);
		}

		return true;
	}
	return false;
}

bool RoleAttrAsset::subVal(attrid_t index, const attrval_t & val)
{
    if(!m_data->datas[index].isNumber()) {
        setLastError(ESERVER_ERR_TYPE_NOT_MATCH);
        setLastErrorAttrID(index);
        return  false;
    }
    
    
	attrval_t step = val;
	if (fabsf(m_attrRate - 1.0f) > 0.001f) {
		step *= attrval_t(m_attrRate) ;
	}
	attrval_t curVal = getVal(index);
	if (curVal < val) {
		setLastError(ESERVER_ERR_ATTR_NOT_ENOUGH);
		setLastErrorAttrID(index);
		//m_lastErrorCode = ESERVER_ERR_ATTR_NOT_ENOUGH;
		//m_lastErrorID = index;
		return false;
	}
	bool ret = setVal(index, curVal - step);
	if (!ret) {
		setLastErrorAttrID(index);
		//m_lastErrorID = index ;
	}
	else {
		attrid_t errorAttrID = this->getLastErrorID();
		int codeType = this->getLastError();
		if (codeType && errorAttrID != INVALID_ATTR_ID)	{
			ret = false;
		}
	}
	return  ret ;
}

attrval_t  RoleAttrAsset::getVal(attrid_t index, bool withRecalc)
{
	RoleAttrHelper  *pwah = get_attr_helper();
	if (withRecalc && pwah->check_readonly(index) )	{
		role_attr_description *pdesc = pwah->get_wa_desc(index);
		if (pdesc ) {
			if (pdesc->cmd_data.size > 0) {
				init_vm();
				if (-1 != vm_run_cmd(&m_vm, pdesc->cmd_data.cmd_buf, pdesc->cmd_data.size)) {
					float val = vm_return_val(&m_vm);
					_set_val(attrval_t(val), index);
				}
			}
			else {
				_set_val(m_data->datas[index], index);
			}
		}

	}
	if (index < m_data->datasCount){
		return m_data->datas[index];
	}
	return 0;
}

attrval_t  RoleAttrAsset::getVal(const char *name, bool withRecalc )
{
	ND_TRACE_FUNC();
	RoleAttrHelper  *pwah = get_attr_helper();
	attrid_t waid = pwah->GetID(name);
	if (waid != INVALID_ATTR_ID){
		return getVal(waid,withRecalc);
	}
	return 0;
}

attrval_t  RoleAttrAsset::getVal(attrid_t index)const
{
	
	if (index < m_data->datasCount){
		return m_data->datas[index];
	}
	return 0;
}
attrval_t  RoleAttrAsset::getVal(const char *name)const
{
	ND_TRACE_FUNC();
	RoleAttrHelper  *pwah = get_attr_helper();
	attrid_t waid = pwah->GetID(name);
	if (waid != INVALID_ATTR_ID){
		return getVal(waid);
	}
	return 0;
}

bool RoleAttrAsset::setVal(const char *name, const attrval_t &newval)
{
	ND_TRACE_FUNC();
	RoleAttrHelper  *pwah = get_attr_helper();
	attrid_t waid = pwah->GetID(name);
	if (waid != INVALID_ATTR_ID){
		return setVal(waid, newval);
	}
	setLastError(NDERR_INVALID_INPUT);
	setLastErrorAttrID(INVALID_ATTR_ID);
	//m_lastErrorCode = NDERR_INVALID_INPUT;
	//m_lastErrorID = INVALID_ATTR_ID;
	return false;
}
bool RoleAttrAsset::addVal(const char *name, const attrval_t &addval)
{
	ND_TRACE_FUNC();
	RoleAttrHelper  *pwah = get_attr_helper();
	attrid_t waid = pwah->GetID(name);
	if (waid != INVALID_ATTR_ID){
		return addVal(waid, addval);
	}
    
    if(!m_data->datas[waid].isNumber()) {
        setLastError(ESERVER_ERR_TYPE_NOT_MATCH);
        setLastErrorAttrID(waid);
        return  false;
    }
    
	setLastError(NDERR_INVALID_INPUT);
	setLastErrorAttrID(INVALID_ATTR_ID);
	//m_lastErrorCode = NDERR_INVALID_INPUT;
	//m_lastErrorID = INVALID_ATTR_ID;
	return false;

}
bool RoleAttrAsset::subVal(const char *name, const attrval_t &subval)
{
	ND_TRACE_FUNC();
	if (subval== attrval_t(0)){
		return true;
	}
	RoleAttrHelper  *pwah = get_attr_helper();
	attrid_t waid = pwah->GetID(name);
	if (waid != INVALID_ATTR_ID){
		return  subVal(waid, subval);
	}

    if(!m_data->datas[waid].isNumber()) {
        setLastError(ESERVER_ERR_TYPE_NOT_MATCH);
        setLastErrorAttrID(waid);
        return  false;
    }
    
	setLastError(NDERR_INVALID_INPUT);
	setLastErrorAttrID(INVALID_ATTR_ID);
	//m_lastErrorCode = NDERR_INVALID_INPUT;
	//m_lastErrorID = INVALID_ATTR_ID;
	return false;
}
bool RoleAttrAsset::setVal(const attr_node_buf &attrs)
{
	for (int i = 0; i < attrs.number; i++)	{
		if (!setVal(attrs.buf[i].id, attrs.buf[i].val)) {
			return false;
		}
	}
	return true;
}
bool RoleAttrAsset::addVal(const attr_node_buf &attrs)
{
	//bool ret = false;
	for (int i = 0; i < attrs.number; i++)	{
		if (!addVal(attrs.buf[i].id, attrs.buf[i].val)){
			return false;
		}
	}

	return true;
}
bool RoleAttrAsset::subVal(const attr_node_buf &attrs)
{
	for (int i = 0; i < attrs.number; i++)	{
		if (!subVal(attrs.buf[i].id, attrs.buf[i].val)){
			return false;
		}
	}
	return true;
}


bool RoleAttrAsset::checkUnlimitMax(attrid_t aid)
{
	ND_TRACE_FUNC();
	RoleAttrHelper  *pwah = get_attr_helper();
	role_attr_description *pdesc = pwah->get_wa_desc(aid);
	if (!pdesc){
		return false;
	}

	attrval_t  curVal = m_data->datas[aid];
	
	if (1 == pdesc->isUnlimitedMax){
		if (curVal >= pdesc->unlimitMax){
			return true;
		}
	}
	else if (2 == pdesc->isUnlimitedMax){
		attrid_t idmax = (int)pdesc->unlimitMax;
		if (idmax < pwah->m_wa_num && curVal >= m_data->datas[idmax])	{
			return true;
		}
	}
	return false;
}

void RoleAttrAsset::Undo(const attrid_t &aid, const  attrval_t &old_val, int optype)
{
	ND_TRACE_FUNC();
	if (EAO_MODIFIED == optype){
		//m_data.attrs[aid] = val ;
		m_data->datas[aid] = old_val;
	}

}
void RoleAttrAsset::AffairDo(const attrid_t &aid,const attrval_t &old_val, int optype)
{
	ND_TRACE_FUNC();
	if (EAO_MODIFIED == optype){
		attrval_t newval = getVal(aid);
		OnChanged(aid, newval,old_val);
	}
}

void RoleAttrAsset::OnChanged(attrid_t aid,  attrval_t val,attrval_t old_val)
{
	if (!m_commitNtf){
		return;
	}
}

bool RoleAttrAsset::EnableRecalc(bool bflag)
{
	bool ret = m_EnableRecalc ;
	m_EnableRecalc = bflag;
	return ret;
}


bool RoleAttrAsset::printf()
{
	for (int i = 0; i < m_data->datasCount; ++i) {
		if (isValideAttrVal(m_data->datas[i])) {
			std::string str1 = AttrVal2STDString(m_data->datas[i]);
			fprintf(stderr, "(%d, %s) \n", i, str1.c_str());

		}
	}
	return  true;
}

int RoleAttrAsset::toNodeBuf(attr_node_buf &nodebuf)
{
	int num = 0 ;
	for (int i=0; i<m_data->datasCount; ++i) {
		if(isValideAttrVal(m_data->datas[i])) {
			if(nodebuf.push_back(i, m_data->datas[i]) ) {
				++num ;
			}
		}
	}
	return  num;
	
}



//return 0 normal, 1 need recalc , -1 lower than min-limit -2,
int RoleAttrAsset::_set_val(const attrval_t & val, attrid_t aid)
{
	ND_TRACE_FUNC();
	RoleAttrHelper  *pwah = get_attr_helper();
	role_attr_description *pdesc = pwah->get_wa_desc(aid);
	if (!pdesc){
		return 0;
	}
	
	setLastError(ESERVER_ERR_SUCCESS);
	setLastErrorAttrID(INVALID_ATTR_ID);

	attrval_t  *pdata = &m_data->datas[aid];
	attrval_t oldval = *pdata;
	*pdata = val;
	if (1 == pdesc->ismax){
		if (*pdata > pdesc->maxval){
			*pdata = pdesc->maxval;

			setLastError(ESERVER_ERR_ATTR_TOO_MUCH);
			setLastErrorAttrID(aid);
		}
	}
	else if (2 == pdesc->ismax){
		attrid_t idmax = (int)pdesc->maxval;
		if (idmax < pwah->m_wa_num && *pdata > m_data->datas[idmax])	{
			*pdata = m_data->datas[idmax];

			setLastError(ESERVER_ERR_ATTR_TOO_MUCH);
			setLastErrorAttrID(aid);
		}
	}

	if (1 == pdesc->ismin){
		if (*pdata < pdesc->minval){
			*pdata = pdesc->minval;

			setLastError(ESERVER_ERR_ATTR_NOT_ENOUGH);
			setLastErrorAttrID(aid);
		}
	}
	else if (2 == pdesc->ismin){
		attrid_t idmin = (int)pdesc->minval;
		if (idmin < pwah->m_wa_num && *pdata < m_data->datas[idmin])	{
			*pdata = m_data->datas[idmin];

			setLastError(ESERVER_ERR_ATTR_NOT_ENOUGH);
			setLastErrorAttrID(aid);
		}
	}

	attrval_t val_deta = oldval - *pdata;
	
	if (fabsf(val_deta) >= ATTR_VALUE_DETA){
		//如果是在事务中就调用事务事件，否则直接调用状态变化通知事件
		if (CheckInAffair()){
			AffairModify(aid, oldval);
		}
		else {
			OnChanged(aid, *pdata, oldval);
		}
		SetDataChanged();
		if (pdesc->infection_num > 0) {
			return 1;
		}
		return 0;
	}
	return 0;
}

float *RoleAttrAsset::getMMAddr(attrid_t aid)
{
	for (int i=0;i< m_mmnodeNum;i++){
		if (m_runtime_mm[i].aid == aid) {
			return &m_runtime_mm[i].fval;
		}
	}
	if (m_mmnodeNum >= MM_NODE_NUMBER) {
		return NULL;
	}
	attrval_t val = getVal(aid);
	m_runtime_mm[m_mmnodeNum].aid = aid;
	m_runtime_mm[m_mmnodeNum].fval = (float)val;

	return &m_runtime_mm[m_mmnodeNum++].fval;
}
void RoleAttrAsset::resetMMAddr()
{
	m_mmnodeNum = 0;
	memset(m_runtime_mm,0,sizeof(m_runtime_mm));
}
static vm_value* vm_mm_getaddr(vm_adddress addr, struct vm_cpu *vm)
{
	RoleAttrAsset *pAttr =(RoleAttrAsset *) vm->extern_param;
	if (!pAttr) {
		return NULL;
	}
	return (vm_value*)pAttr->getMMAddr((attrid_t)addr);
}

void RoleAttrAsset::init_vm()
{
	ND_TRACE_FUNC();
	//vm_machine_init(&m_vm, m_data->datas, (size_t)(ROLE_ATTR_CAPACITY));
	vm_machine_init(&m_vm, NULL,0);
	vm_set_mmfunc(&m_vm, vm_mm_getaddr, ROLE_ATTR_CAPACITY);
	resetMMAddr();
#if 0
	vm_set_echo_ins(&m_vm, 1);
	vm_set_echo_result(&m_vm, 1);
	vm_set_outfunc(nd_output);
	vm_set_errfunc(nd_output);
#else 
	vm_set_echo_ins(&m_vm, 0);
	vm_set_echo_result(&m_vm, 0);
	vm_set_outfunc(NULL);
	vm_set_errfunc(NULL);
#endif
	vm_set_param(&m_vm, (void*)this);
}


int RoleAttrAsset::RunFormular(const char *text, float *val)
{
	ND_TRACE_FUNC();
	ndvm_cmd_node cmdnode;
	RoleAttrHelper  *pwah = get_attr_helper();
	if (-1 == pwah->parse_form(text, &cmdnode) || !cmdnode.size) {
		return -1;
	}
	init_vm();
	if (-1 == vm_run_cmd(&m_vm, cmdnode.cmd_buf, (size_t)cmdnode.size)) {
		return -1;
	}
	*val = vm_return_val(&m_vm);
	return 0;
}

int RoleAttrAsset::Recalc()
{
	ND_TRACE_FUNC();
	if (!m_EnableRecalc){
		return 0;
	}
	
	//nd_logdebug("start recalc role attribute\n") ;
	
	//int ret = 0;
	RoleAttrHelper  *pwah = get_attr_helper();
	for (int i = 0; i<pwah->m_run_sort.num; i++) {
		attrid_t waid = pwah->m_run_sort.sort_buf[i];
		role_attr_description *pdesc = pwah->get_wa_desc(waid);
		if (pdesc) {
			init_vm();
			if (-1 != vm_run_cmd(&m_vm, pdesc->cmd_data.cmd_buf, pdesc->cmd_data.size)) {
				float val = vm_return_val(&m_vm);
				_set_val((attrval_t)val, waid);
			}
			
		}
	}
	
	return 0;
}
