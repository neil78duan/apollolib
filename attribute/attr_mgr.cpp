/* file attr_mgr.cpp
 *
 * player attribute manager
 *
 * create by duan 
 *
 * 2015-5-29
 */
#include "attr_mgr.h"
#include <math.h>

RoleAttrAsset::RoleAttrAsset(role_attr_data *data) : m_data(data), m_EnableRecalc(false)
{
	m_attrRate = 1.0f ;
	m_lastErrorID = INVALID_ATTR_ID;
	m_lastErrorCode = 0;
	m_recordChange = false;
}
RoleAttrAsset::~RoleAttrAsset()
{

}


float RoleAttrAsset::setAttrRate(float newVal)
{
	float ret = m_attrRate ;
	m_attrRate = newVal ;
	return ret;
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

attrval_t  RoleAttrAsset::getVal(attrid_t index) const
{
	if (index < m_data->datasCount){
		return m_data->datas[index];
	}
	return 0;
}
bool RoleAttrAsset::setVal(attrid_t index, attrval_t val)
{
	ND_TRACE_FUNC();
	if (index >= m_data->datasCount){
		return false;
	}

	int ret = 0;
	attrval_t *pdata = &m_data->datas[index];
	//attrval_t oldval = *pdata;

	if (val != *pdata) {
		ret = _set_val(val, index);
		if (ret > 0  && m_EnableRecalc){
			Recalc();
		}
	}
	return true;

}

bool RoleAttrAsset::addVal(attrid_t index, attrval_t val)
{
	if (fabsf(m_attrRate - 1.0f) > 0.001f) {
		val *= m_attrRate ;
	}
	if (setVal(index, getVal(index) + val)) {

		if (m_recordChange)	{
			attrval_node node;
			node.id = index;
			node.val = val;
			m_changed.push_back(node);
		}

		return true;
	}
	return false;
}

bool RoleAttrAsset::subVal(attrid_t index, attrval_t val)
{
	if (fabsf(m_attrRate - 1.0f) > 0.001f) {
		val *= m_attrRate ;
	}
	attrval_t curVal = getVal(index);
	if (curVal < val) {
		return false;
	}
	bool ret = setVal(index, curVal - val);
	if (!ret) {
		m_lastErrorID = index ;
	}
	else {
		attrid_t errorAttrID = this->getLastErrorID();
		int codeType = this->getLastError();
		if (codeType && errorAttrID != INVALID_ATTR_ID)	{
			ret = false;
		}
		else {
//			if (m_recordChange)	{
//				attrval_node node;
//				node.id = index;
//				node.val = -val;
//				m_changed.push_back(node);
//			}

		}
	}
	return  ret ;
}

attrval_t  RoleAttrAsset::getVal(const char *name) const
{
	ND_TRACE_FUNC();
	RoleAttrHelper  *pwah = get_attr_helper();
	attrid_t waid = pwah->GetID(name);
	if (waid != INVALID_ATTR_ID){
		return getVal(waid);
	}
	return 0;
}
bool RoleAttrAsset::setVal(const char *name, attrval_t newval)
{
	ND_TRACE_FUNC();
	RoleAttrHelper  *pwah = get_attr_helper();
	attrid_t waid = pwah->GetID(name);
	if (waid != INVALID_ATTR_ID){
		return setVal(waid, newval);
	}
	return false;
}
bool RoleAttrAsset::addVal(const char *name, attrval_t addval)
{
	ND_TRACE_FUNC();
	RoleAttrHelper  *pwah = get_attr_helper();
	attrid_t waid = pwah->GetID(name);
	if (waid != INVALID_ATTR_ID){
		return addVal(waid, addval);
	}
	return false;

}
bool RoleAttrAsset::subVal(const char *name, attrval_t subval)
{
	ND_TRACE_FUNC();
	if (subval==0){
		return true;
	}
	RoleAttrHelper  *pwah = get_attr_helper();
	attrid_t waid = pwah->GetID(name);
	if (waid != INVALID_ATTR_ID){
		return  subVal(waid, subval);
	}
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
	for (int i = 0; i < attrs.number; i++)	{
		addVal(attrs.buf[i].id, attrs.buf[i].val);
	}

	return true;
}
bool RoleAttrAsset::subVal(const attr_node_buf &attrs)
{
	for (int i = 0; i < attrs.number; i++)	{
		subVal(attrs.buf[i].id, attrs.buf[i].val);
	}
	return true;
}


void RoleAttrAsset::Undo(attrid_t &aid, attrval_t &old_val,  int optype)
{
	ND_TRACE_FUNC();
	if (EAO_MODIFIED == optype){
		//m_data.attrs[aid] = val ;
		m_data->datas[aid] = old_val;
	}

}
void RoleAttrAsset::AffairDo(attrid_t &aid, attrval_t &old_val, int optype)
{
	ND_TRACE_FUNC();
	if (EAO_MODIFIED == optype){
		attrval_t newval = getVal(aid);
		OnChanged(aid, newval);
	}
}

void RoleAttrAsset::OnChanged(attrid_t aid, attrval_t val)
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
	for (int i=0; i<m_data->datasCount; ++i) {
		if( m_data->datas[i])
			fprintf(stderr, "(%d, %f) \n",i, m_data->datas[i]) ;
	}
	return  true;
}

int RoleAttrAsset::toNodeBuf(attr_node_buf &nodebuf)
{
	int num = 0 ;
	for (int i=0; i<m_data->datasCount; ++i) {
		if( m_data->datas[i]) {
			if(nodebuf.push_back(i, m_data->datas[i]) ) {
				++num ;
			}
		}
	}
	return  num;
	
}



//return 0 normal, 1 need recalc , -1 lower than min-limit -2,
int RoleAttrAsset::_set_val(attrval_t val, attrid_t aid)
{
	ND_TRACE_FUNC();
	RoleAttrHelper  *pwah = get_attr_helper();
	role_attr_description *pdesc = pwah->get_wa_desc(aid);
	if (!pdesc){
		return 0;
	}

	m_lastErrorID = INVALID_ATTR_ID;
	m_lastErrorCode = 0;

	attrval_t  *pdata = &m_data->datas[aid];
	attrval_t oldval = *pdata;
	*pdata = val;
	if (1 == pdesc->ismax){
		if (*pdata > pdesc->maxval){
			*pdata = pdesc->maxval;

			m_lastErrorID = aid;
			m_lastErrorCode = 2;
		}
	}
	else if (2 == pdesc->ismax){
		attrid_t idmax = (attrid_t)pdesc->maxval;
		if (idmax < pwah->m_wa_num && *pdata > m_data->datas[idmax])	{
			*pdata = m_data->datas[idmax];

			m_lastErrorID = aid;
			m_lastErrorCode = 2;
		}
	}

	if (1 == pdesc->ismin){
		if (*pdata < pdesc->minval){
			*pdata = pdesc->minval;

			m_lastErrorID = aid;
			m_lastErrorCode = 1;
		}
	}
	else if (2 == pdesc->ismin){
		attrid_t idmin = (attrid_t)pdesc->minval;
		if (idmin < pwah->m_wa_num && *pdata < m_data->datas[idmin])	{
			*pdata = m_data->datas[idmin];

			m_lastErrorID = aid;
			m_lastErrorCode = 2;
		}
	}

	attrval_t val_deta = oldval - *pdata;
	
	if (fabsf(val_deta) >= ATTR_VALUE_DETA){
		//如果是在事务中就调用事务事件，否则直接调用状态变化通知事件
		if (CheckInAffair()){
			AffairModify(aid, oldval);
		}
		else {
			OnChanged(aid, *pdata);
			
			//AttrValChange(aid, m_data.attrs_base[aid]);
		}
		m_dataChanged = 1;
		if (pdesc->infection_num > 0) {
			return 1;
		}
		return 0;
	}
	return 0;
}

void RoleAttrAsset::init_vm()
{
	ND_TRACE_FUNC();
	vm_machine_init(&m_vm, m_data->datas, (size_t)(ROLE_ATTR_CAPACITY));
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
	
	nd_logdebug("start recalc role attribute\n") ;
	
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