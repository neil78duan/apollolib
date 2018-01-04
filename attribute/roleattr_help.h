/* file warattr_help.h
 *
 * war attribute calcutor helper
 *
 * create by duan 
 * 2011/3/14 17:21:26
 */
 
#ifndef _ROLE_ATTR_HELPER_
#define _ROLE_ATTR_HELPER_

#include "nd_common/nd_common.h"
#include <vector>

#define ROLE_ATTR_CAPACITY 200

#define  NDVM_CMD_SIZE 1024 
#define ATTR_NAME_SIZE 64
#define MAX_PLAYER_LEVEL 300


typedef unsigned char attrid_t;
typedef float attrval_t;

#define INVALID_ATTR_ID 0xff

#define ATTR_VALUE_DETA 0.0001f
//�����ָ��
struct ndvm_cmd_node
{
	int size ;
	char cmd_buf[NDVM_CMD_SIZE] ;
};
struct attr_name_t
{
	char attrname[ATTR_NAME_SIZE];
};

struct attrval_node
{
	attrid_t id;
	attrval_t val;

};

struct attr_node_buf
{
	int number;
	attrval_node buf[ROLE_ATTR_CAPACITY];
	attr_node_buf() : number(0)
	{

	}
	bool push_back(attrid_t id, attrval_t val)
	{
		if (number >= ROLE_ATTR_CAPACITY)
		{
			return false;
		}
		buf[number].val = val;
		buf[number].id = id;
		++number;
		return true;
	}
	void addval(attrid_t id, attrval_t val)
	{
		for (int i = 0; i < number; i++) {
			if (id == buf[i].id) {
				buf[i].val += val;
				return;
			}
		}
		push_back(id, val);
	}
	attr_node_buf & operator = (const attr_node_buf &r)
	{
		for (int i = 0; i < r.number; i++) {
			buf[i].id = r.buf[i].id;
			buf[i].val = r.buf[i].val;
		}
		number = r.number;
		return *this;
	}
	void operator *= (float deta)
	{
		for (int i = 0; i < number; i++) {
			buf[i].val = deta;
		}
	}
	size_t getBinSize()
	{

		return sizeof(attr_node_buf) - sizeof(this->buf) + this->number * sizeof(this->buf[0]);
	}
};
// 
// struct base_attr_t
// {
// 	attrid_t wa_id ;
// 	NDUINT8 m_is_sync :1;
// 	NDUINT8 m_is_set_byclient:1;
// 	NDUINT8 m_is_create_role_set:1;
// 	char attrname[ATTR_NAME_SIZE];
// };
//���Լ��㹫ʽ
//����ÿ�����ԵĹ�ʽ����ص�����˳��
struct role_attr_description{
	attrid_t wa_id ;
	unsigned char issave, issync,isChangeByclient,iscallEvent,islog ;	//�Ƿ�浵���Ƿ�ͬ��  
	unsigned char isSyncBuilding, forRole, forBuilding; //��������Ƿ�Ӱ����ǽ���� ������ԣ�����������
	unsigned char isAffectCard, forCard; 
	unsigned char showInGuild;
	unsigned char isLostOnAttacked;
	int infection_num ;				//�ܵ�ǰ����Ӱ�������
	int need_num ;					//��ǰ���Լ�����Ҫ�����Ը���
	attr_name_t name;
	attr_name_t real_name;
	char input_for[NDVM_CMD_SIZE];
	char ismin, ismax, isUnlimitedMax ;				//�Ƿ������ֵ0û��,1 ������ֵ, 2 ������ĳ������
	attrval_t minval ,maxval, unlimitMax ;		//�����Сֵ
	attrid_t infections[ROLE_ATTR_CAPACITY];
	attrid_t need_buf[ROLE_ATTR_CAPACITY];
	ndvm_cmd_node cmd_data;
};

//��ʽִ��˳��
struct formula_run_sort
{
	int num ;
	attrid_t sort_buf[ROLE_ATTR_CAPACITY];
};

typedef std::vector<attr_name_t> baseattr_vct ;
//ս����������������
class RoleAttrHelper
{

public:
	attrid_t GetID(const char *name); 
	inline attrid_t GetIDByRealName(const char *name) { return GetID(name); }
	//attrid_t GetIDByRealName(const char *name);

	attrid_t GetIDEx(const char *name,int &MinOrMax);		//�õ�ս������MinOrMax =1 min , 2max , 0 none (ref ����min	����max)
	const char *Getname(attrid_t wa_id);
	const char *GetRealname(attrid_t wa_id);
	attrid_t GetwaIDBase() ;
	int GetAttrNum(){ return m_wa_num; }
	
	int FetchAttr2Others(attrval_t *inbufs,int bufsize, attrval_node *out_nodes, int out_size ) ;

	int ParseFormula() ;
	int place_param_name(char *input, char *buf, int size) ;
	int parse_minmax(const char *maxval, const char *minval, const char*unlimitedMax, role_attr_description *pwa_desc);
	bool check_save(attrid_t wa_id) ;
	bool check_log(attrid_t wa_id);
	bool check_sync(attrid_t wa_id) ;
	bool check_client_change(attrid_t wa_id) ;
	bool check_gen_event(attrid_t wa_id);
	role_attr_description *get_wa_desc(attrid_t wa_id);
    //const up_grade_info* GetUpgradeInfo(const int _grade);

	int parse_form(const char *cmd_txt, ndvm_cmd_node *cmdnode) ;
	
	void Destroy() ;
	int Init() ;
	int Load(const char *attr_file, const char *up_level_file);

	RoleAttrHelper() ;
	~RoleAttrHelper() ;
private:
	int loadUplevelExp(const char *file);
	int parse_run_sort();
	int load_rank() ;
	int check_canbe_run(role_attr_description *node, attrid_t *buf);
public:
	int m_current_parse ;
	int m_wa_num;
	
	formula_run_sort m_run_sort;
	formula_run_sort m_save_attrs ;
	formula_run_sort m_sync_attrs;
	//formula_run_sort m_change_byclient;
	role_attr_description m_wahelper_bufs[ROLE_ATTR_CAPACITY] ;

	int m_maxLevel;
	NDUINT32 m_upLevelExp[MAX_PLAYER_LEVEL]; //the cost exp that update next level 
	

};

extern RoleAttrHelper *get_attr_helper();

#include "logic_parser/dbl_mgr.h"
#include "logic_parser/logicDataType.h"

//ͨ�����ѯ����ID������ return value ,number of attributes , return -1 on error
int DBL_GetAttrIDName(const char *table, attrid_t ids[], const char *[], int bufsize) ;


//�ӱ��ж�ȡ��ɫ���� return value ,number of attributes , return -1 on error
int DBL_ReadAttrList(const char *table, int index, attr_node_buf &attrbuf);

static inline void LogicType2Attrbuf(attr_node_buf *to_buf,DBLDataNode &fromData )
{
	//size_t size = fromData.GetBinarySize();
	attr_node_buf *p = (attr_node_buf*)fromData.GetBinary();
	nd_assert(p);
	if (p){
		*to_buf = *p;
	}
}

//void InitSet(void *binary, size_t size, DBL_ELEMENT_TYPE eleType = OT_BINARY_DATA);
static inline void Attrbuf2LogicType(DBLDataNode &toData, attr_node_buf *from_buf )
{
	nd_assert(from_buf);
	size_t size = sizeof(attr_node_buf) - sizeof(from_buf->buf) + from_buf->number * sizeof(attrval_node);
	toData.InitSet((void*)from_buf, size, OT_ATTR_DATA);
}

//dataString read from excel-table
int Dbl_TableStringToAttr(const DBLDataNode &dataString, attr_node_buf &attrbuf);

#endif
