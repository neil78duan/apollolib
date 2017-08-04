/* file role_attr_mgr.h
 *
 * player attribute manager
 *
 * create by duan 
 *
 * 2015-5-29
 */

#ifndef _ROLE_ATTR_MGR_H_
#define _ROLE_ATTR_MGR_H_

#include "ndapplib/applib.h"
#include "ndapplib/nd_affair.h"
//#include "protocol_def.h"
#include "logic_parser/logicEngineRoot.h"

#include "attribute/roleattr_help.h"
#include <vector>
//#include "message_inc.h"


struct role_attr_data
{
	NDUINT16 datasCount;
	attrval_t datas[ROLE_ATTR_CAPACITY];

	role_attr_data() :datasCount(0)
	{
		for (int i = 0; i < ROLE_ATTR_CAPACITY; i++)
		{
			datas[i] = 0;
		}
	}
	
};
/*
class Player; 

typedef unsigned int attrid_t;
typedef NDINT32 attrval_t;

#define MAX_ATTRIBUTE_COUNT 100
struct UserBaseAttribute
{
	UserBaseAttribute()
	{
		memset(this, 0, sizeof(*this) );
	}
	
	NDUINT16  datasCount;
	NDINT32  datas[MAX_ATTRIBUTE_COUNT]; 	//角色属性，用数组保持，下表参考宏定义 ATTR_INDEX_...
};
*/


typedef NDAffair<attrid_t, attrval_t> RoleAttrAffair;

class RoleAttrAsset : /*public LogicObjectBase,*/ public RoleAttrAffair
{
	typedef RoleAttrAffair myBaseAff;
public:
	RoleAttrAsset(role_attr_data *data);
	virtual ~RoleAttrAsset();

	float setAttrRate(float newVal) ;
	
	int getChanged(attrval_node *buf, size_t size);
	void beginRecordChange();
	void endRecordChange();
public:
	attrval_t getVal(attrid_t index) const;
	bool setVal(attrid_t index, attrval_t val);
	bool addVal(attrid_t index, attrval_t val);
	bool subVal(attrid_t index, attrval_t val);

	attrval_t   getVal(const char *name) const;
	bool        setVal(const char *name, attrval_t newval);
	bool		addVal(const char *name, attrval_t addval);
	bool		subVal(const char *name, attrval_t subval);

	bool setVal(const attr_node_buf &attrs);
	bool addVal(const attr_node_buf &attrs);
	bool subVal(const attr_node_buf &attrs);

	bool printf() ;
	int toNodeBuf(attr_node_buf &nodebuf) ;
	
	//////////////////////////////////////////////////////////////////////////
	
	int         RunFormular(const char *text, float *val);
	int         Recalc();
	bool		EnableRecalc(bool bflag = true);

	class FlagRecalcHelper
	{
	public:
		FlagRecalcHelper(RoleAttrAsset *pa, bool flag) :m_pa(pa)
		{
			m_oldflag = pa->EnableRecalc(flag);
		}
		~FlagRecalcHelper()
		{
			if (m_pa)	{
				m_pa->EnableRecalc(m_oldflag);
			}
		}

	private:
		RoleAttrAsset *m_pa;
		bool m_oldflag;
	};

	attrid_t getLastErrorID() { return m_lastErrorID; }
	int getLastError(){ return m_lastErrorCode; }; //return 0 success ,1 too-lower ,2 too hight

protected:

	
	int _set_val(attrval_t val, attrid_t aid);

	void init_vm();

	void AffairDo(const attrid_t &aid, const attrval_t &old_val, int optype);
	void Undo(const attrid_t &aid, const  attrval_t &old_val, int optype);
	virtual void OnChanged(attrid_t aid, attrval_t new_val); //value changed notify client and save db
	
	bool m_EnableRecalc;
	attrid_t m_lastErrorID;
	int m_lastErrorCode;
	float m_attrRate ;
private:
	role_attr_data *m_data;
	vm_cpu	m_vm;

	typedef std::vector<attrval_node>  attrValVct_t;
	bool m_recordChange;
	attrValVct_t m_changed;
};
// 
// //for test
// class RoleAttrsTest :public RoleAttrAsset
// {
// public:
// 	RoleAttrsTest();
// 	virtual ~RoleAttrsTest();
// 
// private:
// 	role_attr_data m_data;
// };
// 
// RoleAttrsTest::RoleAttrsTest() : RoleAttrAsset(&m_data)
// {
// }
// 
// RoleAttrsTest::~RoleAttrsTest()
// {
// }

#endif
