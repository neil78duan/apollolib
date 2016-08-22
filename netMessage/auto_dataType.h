/* please do not change this file ,
 * auto create by program 
 * create time 2016-8-18 10:42:51 
 */

#ifndef _AUTO_DATA_TYPE_DEFINE_H_
#define _AUTO_DATA_TYPE_DEFINE_H_
#include "netMessage/auto_macroDefine.h"

#include "netMessage/dataStream.h"

#define DATA_IN_DB_VERSION 1 

namespace NetMessage 
{
// ͨ�ô���ظ�
struct MsgCommonReply 
{
 	MsgCommonReply() 
	{
		memset(this, 0, sizeof(*this) ); 
	}

	NDUINT8  result; 	//��Ϣ�����Ƿ�ɹ�0 success, 1 failed 
	NDUINT32  value; 	//����������ֵ 
};
int ReadStream(NDIStreamMsg &inmsg,NetMessage::MsgCommonReply &data);
int WriteStream(NDOStreamMsg &omsg,const NetMessage::MsgCommonReply &data);



// ͨ�ô��󷵻�,���������͸��ͻ���
struct CommonErrorReply 
{
 	CommonErrorReply() 
	{
		memset(this, 0, sizeof(*this) ); 
	}

	NDUINT8  errtType; 	//��������,�ο�eCommonErrorType 
	NDUINT32  param; 	//������� 
};
int ReadStream(NDIStreamMsg &inmsg,NetMessage::CommonErrorReply &data);
int WriteStream(NDOStreamMsg &omsg,const NetMessage::CommonErrorReply &data);



// ͨ����Ϣ����
struct MsgCommonRequest 
{
 	MsgCommonRequest() 
	{
		memset(this, 0, sizeof(*this) ); 
	}

	NDUINT8  type; 	//���� 
	NDUINT32  value; 	//ֵ 
};
int ReadStream(NDIStreamMsg &inmsg,NetMessage::MsgCommonRequest &data);
int WriteStream(NDOStreamMsg &omsg,const NetMessage::MsgCommonRequest &data);



// װ���ĸ������Խڵ�
struct EquipAttrNode 
{
 	EquipAttrNode() 
	{
		memset(this, 0, sizeof(*this) ); 
	}

	NDUINT8  color; 	//����ԭɫ 
	NDUINT8  attr_id; 	//����id 
	float  attr_val; 	//����ֵ 
};
int ReadStream(NDIStreamMsg &inmsg,NetMessage::EquipAttrNode &data);
int WriteStream(NDOStreamMsg &omsg,const NetMessage::EquipAttrNode &data);



struct ItemInfo 
{
 	ItemInfo() 
	{
		memset(this, 0, sizeof(*this) ); 
	}

	NDUINT32  instantID; 	//��Ʒʵ��id 
	NDUINT32  typeID; 	//��Ʒ����id 
	NDUINT16  slot; 	//λ�� 
	NDUINT16  number; 	//���� 
	NDUINT8  stat; 	//״̬ 
};
int ReadStream(NDIStreamMsg &inmsg,NetMessage::ItemInfo &data);
int WriteStream(NDOStreamMsg &omsg,const NetMessage::ItemInfo &data);



struct EquipInfo 
{
 	EquipInfo() 
	{
		memset(this, 0, sizeof(*this) ); 
	}

	NDUINT32  instantID; 	//��Ʒʵ��id 
	NDUINT32  typeID; 	//��Ʒ����id 
	NDUINT16  slot; 	//λ�� 
	NDUINT8  level; 	//���׵ȼ� 
	NDUINT8  sub_level; 	//ǿ���ȼ� 
	NDUINT8  stat; 	//״̬��λ���� 
	NDUINT16  attrsCount; 	 
	EquipAttrNode  attrs[EQUIP_ATTR_NUMBER]; 	//(null) 
	bool push_attrs(const EquipAttrNode &val)
	{
		if (attrsCount < EQUIP_ATTR_NUMBER)	{
			attrs[attrsCount++] = val;
			return true;
		}
		return false;
	}

};
int ReadStream(NDIStreamMsg &inmsg,NetMessage::EquipInfo &data);
int WriteStream(NDOStreamMsg &omsg,const NetMessage::EquipInfo &data);



struct RoleEquipsData 
{
 	RoleEquipsData() 
	{
		memset(this, 0, sizeof(*this) ); 
	}

	NDUINT16  equipsCount; 	 
	NDUINT32  equips[EQUIP_SLOT_NUMBER]; 	//����������������װ����������IDС��100 
	bool push_equips(const NDUINT32 &val)
	{
		if (equipsCount < EQUIP_SLOT_NUMBER)	{
			equips[equipsCount++] = val;
			return true;
		}
		return false;
	}

};
int ReadStream(NDIStreamMsg &inmsg,NetMessage::RoleEquipsData &data);
int WriteStream(NDOStreamMsg &omsg,const NetMessage::RoleEquipsData &data);



struct RolePackageData 
{
 	RolePackageData() 
	{
		memset(this, 0, sizeof(*this) ); 
	}

	NDUINT16  equipsCount; 	 
	EquipInfo  equips[EQUIP_PACK_CAPACITY]; 	//װ������ 
	bool push_equips(const EquipInfo &val)
	{
		if (equipsCount < EQUIP_PACK_CAPACITY)	{
			equips[equipsCount++] = val;
			return true;
		}
		return false;
	}

	NDUINT16  itemsCount; 	 
	ItemInfo  items[PACKAGE_CAPACITY]; 	//�������� 
	bool push_items(const ItemInfo &val)
	{
		if (itemsCount < PACKAGE_CAPACITY)	{
			items[itemsCount++] = val;
			return true;
		}
		return false;
	}

};
int ReadStream(NDIStreamMsg &inmsg,NetMessage::RolePackageData &data);
int WriteStream(NDOStreamMsg &omsg,const NetMessage::RolePackageData &data);



struct SkillInfo 
{
 	SkillInfo() 
	{
		memset(this, 0, sizeof(*this) ); 
	}

	NDUINT16  id; 	//����id 
	NDUINT8  level; 	//���ܵȼ� 
};
int ReadStream(NDIStreamMsg &inmsg,NetMessage::SkillInfo &data);
int WriteStream(NDOStreamMsg &omsg,const NetMessage::SkillInfo &data);



struct RoleSkillData 
{
 	RoleSkillData() 
	{
		memset(this, 0, sizeof(*this) ); 
	}

	NDUINT16  datasCount; 	 
	SkillInfo  datas[SKILL_CAPACITY]; 	//�������� 
	bool push_datas(const SkillInfo &val)
	{
		if (datasCount < SKILL_CAPACITY)	{
			datas[datasCount++] = val;
			return true;
		}
		return false;
	}

};
int ReadStream(NDIStreamMsg &inmsg,NetMessage::RoleSkillData &data);
int WriteStream(NDOStreamMsg &omsg,const NetMessage::RoleSkillData &data);



struct AttrNodeVal 
{
 	AttrNodeVal() 
	{
		memset(this, 0, sizeof(*this) ); 
	}

	NDUINT8  id; 	//���Ա�� 
	float  val; 	//����ֵ 
};
int ReadStream(NDIStreamMsg &inmsg,NetMessage::AttrNodeVal &data);
int WriteStream(NDOStreamMsg &omsg,const NetMessage::AttrNodeVal &data);



struct AttrsBuf 
{
 	AttrsBuf() 
	{
		memset(this, 0, sizeof(*this) ); 
	}

	NDUINT16  datasCount; 	 
	AttrNodeVal  datas[ROLE_ATTRS_NUMBER]; 	//����ֵ 
	bool push_datas(const AttrNodeVal &val)
	{
		if (datasCount < ROLE_ATTRS_NUMBER)	{
			datas[datasCount++] = val;
			return true;
		}
		return false;
	}

};
int ReadStream(NDIStreamMsg &inmsg,NetMessage::AttrsBuf &data);
int WriteStream(NDOStreamMsg &omsg,const NetMessage::AttrsBuf &data);



struct RoleAttrsData 
{
 	RoleAttrsData() 
	{
		memset(this, 0, sizeof(*this) ); 
	}

	NDUINT16  datasCount; 	 
	float  datas[ROLE_ATTRS_NUMBER]; 	//���� 
	bool push_datas(const float &val)
	{
		if (datasCount < ROLE_ATTRS_NUMBER)	{
			datas[datasCount++] = val;
			return true;
		}
		return false;
	}

};
int ReadStream(NDIStreamMsg &inmsg,NetMessage::RoleAttrsData &data);
int WriteStream(NDOStreamMsg &omsg,const NetMessage::RoleAttrsData &data);



struct MallBuyItem 
{
 	MallBuyItem() 
	{
		memset(this, 0, sizeof(*this) ); 
	}

	NDUINT32  itemid; 	//��Ʒ��� 
	NDUINT16  num; 	//���� 
};
int ReadStream(NDIStreamMsg &inmsg,NetMessage::MallBuyItem &data);
int WriteStream(NDOStreamMsg &omsg,const NetMessage::MallBuyItem &data);



// ��Ҵ򸱱��ĳɼ�
struct MapScore 
{
 	MapScore() 
	{
		memset(this, 0, sizeof(*this) ); 
	}

	NDUINT16  map_id; 	//��ͼ��� 
	NDUINT16  enter_times; 	//�����Ѿ�������� 
	NDUINT8  achievements; 	//��ɳɾͣ���λ������ÿ��λ����һ���ɾͣ�ֻ�����һ�� 
	NDUINT16  score; 	//������0��ʾû����ɸø��� 
	NDUINT64  time; 	//���ʱ�� 
};
int ReadStream(NDIStreamMsg &inmsg,NetMessage::MapScore &data);
int WriteStream(NDOStreamMsg &omsg,const NetMessage::MapScore &data);



struct SceneMonster 
{
 	SceneMonster() 
	{
		memset(this, 0, sizeof(*this) ); 
	}

	NDUINT16  monsterId; 	//����id 
	NDUINT8  number; 	//�������� 
};
int ReadStream(NDIStreamMsg &inmsg,NetMessage::SceneMonster &data);
int WriteStream(NDOStreamMsg &omsg,const NetMessage::SceneMonster &data);



// ��Ҹ����ɼ���¼
struct MapScoreBuf 
{
 	MapScoreBuf() 
	{
		memset(this, 0, sizeof(*this) ); 
	}

	NDUINT16  dataCount; 	 
	MapScore  data[MAX_MAP_INSTANCE_NUMBER]; 	//(null) 
	bool push_data(const MapScore &val)
	{
		if (dataCount < MAX_MAP_INSTANCE_NUMBER)	{
			data[dataCount++] = val;
			return true;
		}
		return false;
	}

};
int ReadStream(NDIStreamMsg &inmsg,NetMessage::MapScoreBuf &data);
int WriteStream(NDOStreamMsg &omsg,const NetMessage::MapScoreBuf &data);



// �ͻ�����ɸ���
struct MapFinish 
{
 	MapFinish() 
	{
		memset(this, 0, sizeof(*this) ); 
	}

	NDUINT16  map_id; 	//��ͼ��� 
	NDUINT16  score; 	//���� 
	NDUINT8  achievements; 	//����ͨ����ɵĳɾ� 
	NDUINT16  attrsCount; 	 
	AttrNodeVal  attrs[DROP_ATTR_CAPACITY]; 	//������� 
	bool push_attrs(const AttrNodeVal &val)
	{
		if (attrsCount < DROP_ATTR_CAPACITY)	{
			attrs[attrsCount++] = val;
			return true;
		}
		return false;
	}

};
int ReadStream(NDIStreamMsg &inmsg,NetMessage::MapFinish &data);
int WriteStream(NDOStreamMsg &omsg,const NetMessage::MapFinish &data);



// �����ɾͽ���
struct MapAchievAwards 
{
 	MapAchievAwards() 
	{
		memset(this, 0, sizeof(*this) ); 
	}

	NDUINT16  map_id; 	//��ͼ��� 
	NDUINT8  isAllComplete; 	//�Ƿ�ȫ����� 
	NDUINT8  get_achiev; 	//�����»�õĽ��� 
};
int ReadStream(NDIStreamMsg &inmsg,NetMessage::MapAchievAwards &data);
int WriteStream(NDOStreamMsg &omsg,const NetMessage::MapAchievAwards &data);



struct DragonInfo 
{
 	DragonInfo() 
	{
		memset(this, 0, sizeof(*this) ); 
	}

	NDUINT16  id; 	//��id 
	NDUINT16  Level; 	//�ȼ� 
	NDUINT8  starLevel; 	//�Ǽ� 
	NDUINT8  grade; 	//�׼� 
	NDUINT8  stat; 	//״̬,�ο�װ��״̬ 
	NDUINT8  slot; 	//�ڷ�λ�� 
	NDUINT32  favor; 	//�Ѻö� 
	NDUINT32  exp; 	//���� 
	NDUINT16  skillsCount; 	 
	NDUINT32  skills[SKILLS_NUM_PER_DRAGON]; 	//���ļ���-ID�͵ȼ������ 
	bool push_skills(const NDUINT32 &val)
	{
		if (skillsCount < SKILLS_NUM_PER_DRAGON)	{
			skills[skillsCount++] = val;
			return true;
		}
		return false;
	}

};
int ReadStream(NDIStreamMsg &inmsg,NetMessage::DragonInfo &data);
int WriteStream(NDOStreamMsg &omsg,const NetMessage::DragonInfo &data);



struct DragonsBuf 
{
 	DragonsBuf() 
	{
		memset(this, 0, sizeof(*this) ); 
	}

	NDUINT16  dragonsCount; 	 
	DragonInfo  dragons[DRAGON_CAPACITY]; 	//�� 
	bool push_dragons(const DragonInfo &val)
	{
		if (dragonsCount < DRAGON_CAPACITY)	{
			dragons[dragonsCount++] = val;
			return true;
		}
		return false;
	}

};
int ReadStream(NDIStreamMsg &inmsg,NetMessage::DragonsBuf &data);
int WriteStream(NDOStreamMsg &omsg,const NetMessage::DragonsBuf &data);



// ��ɫ������������
struct RoleDragon 
{
 	RoleDragon() 
	{
		memset(this, 0, sizeof(*this) ); 
	}

	DragonInfo  baseInf; 	//��������ֵ 
	NDUINT16  skill; 	//���� 
	NDUINT16  rideSkill; 	//��˼��� 
	AttrsBuf  attrs; 	//���� 
};
int ReadStream(NDIStreamMsg &inmsg,NetMessage::RoleDragon &data);
int WriteStream(NDOStreamMsg &omsg,const NetMessage::RoleDragon &data);



struct DropItem 
{
 	DropItem() 
	{
		memset(this, 0, sizeof(*this) ); 
	}

	NDUINT32  itemId; 	//��Ʒ����id 
	NDUINT16  num; 	//��Ʒ���� 
};
int ReadStream(NDIStreamMsg &inmsg,NetMessage::DropItem &data);
int WriteStream(NDOStreamMsg &omsg,const NetMessage::DropItem &data);



struct DropPackage 
{
 	DropPackage() 
	{
		memset(this, 0, sizeof(*this) ); 
	}

	NDUINT16  attrsCount; 	 
	AttrNodeVal  attrs[DROP_ATTR_CAPACITY]; 	//�����Ǯ 
	bool push_attrs(const AttrNodeVal &val)
	{
		if (attrsCount < DROP_ATTR_CAPACITY)	{
			attrs[attrsCount++] = val;
			return true;
		}
		return false;
	}

	NDUINT16  itemsCount; 	 
	DropItem  items[DROP_ITEM_CAPACITY]; 	//������Ʒ 
	bool push_items(const DropItem &val)
	{
		if (itemsCount < DROP_ITEM_CAPACITY)	{
			items[itemsCount++] = val;
			return true;
		}
		return false;
	}

	NDUINT16  dragonFavor; 	//�����ѺöȽ�����������װ���е��� 
};
int ReadStream(NDIStreamMsg &inmsg,NetMessage::DropPackage &data);
int WriteStream(NDOStreamMsg &omsg,const NetMessage::DropPackage &data);



struct DropWeightNode 
{
 	DropWeightNode() 
	{
		memset(this, 0, sizeof(*this) ); 
	}

	NDUINT16  dropObjId; 	//����ID 
	NDUINT16  weight; 	//����Ȩ�� 
};
int ReadStream(NDIStreamMsg &inmsg,NetMessage::DropWeightNode &data);
int WriteStream(NDOStreamMsg &omsg,const NetMessage::DropWeightNode &data);



struct DropWeightBuf 
{
 	DropWeightBuf() 
	{
		memset(this, 0, sizeof(*this) ); 
	}

	NDUINT16  dataCount; 	 
	DropWeightNode  data[DRAP_WEIGHT_CAPACIT]; 	//������Ȩ�� 
	bool push_data(const DropWeightNode &val)
	{
		if (dataCount < DRAP_WEIGHT_CAPACIT)	{
			data[dataCount++] = val;
			return true;
		}
		return false;
	}

};
int ReadStream(NDIStreamMsg &inmsg,NetMessage::DropWeightBuf &data);
int WriteStream(NDOStreamMsg &omsg,const NetMessage::DropWeightBuf &data);



// ϴ��װ������
struct ResetEquip 
{
 	ResetEquip() 
	{
		memset(this, 0, sizeof(*this) ); 
	}

	NDUINT32  instId; 	//װ��ʵ��id 
	NDUINT8  attrIndex; 	//ϴ���������� 
	NDUINT32  stuffInstId; 	//ϴ������id 
};
int ReadStream(NDIStreamMsg &inmsg,NetMessage::ResetEquip &data);
int WriteStream(NDOStreamMsg &omsg,const NetMessage::ResetEquip &data);



// ϴ�����
struct ResetEquipResult 
{
 	ResetEquipResult() 
	{
		memset(this, 0, sizeof(*this) ); 
	}

	NDUINT32  instId; 	//װ��ʵ��id 
	NDUINT32  stuffInstId; 	//ϴ������id 
	NDUINT8  stuffAttrIndex; 	//����������� 
	EquipAttrNode  attrVal; 	//���ϴ�����������ֵ 
};
int ReadStream(NDIStreamMsg &inmsg,NetMessage::ResetEquipResult &data);
int WriteStream(NDOStreamMsg &omsg,const NetMessage::ResetEquipResult &data);



struct WeaponData 
{
 	WeaponData() 
	{
		memset(this, 0, sizeof(*this) ); 
	}

	NDUINT16  id; 	//id 
	NDUINT16  skinId; 	//Ƥ��id 
	NDUINT16  Level; 	//ǿ���ȼ�����ʼ0 
};
int ReadStream(NDIStreamMsg &inmsg,NetMessage::WeaponData &data);
int WriteStream(NDOStreamMsg &omsg,const NetMessage::WeaponData &data);



// ��������
struct WeaponsBuf 
{
 	WeaponsBuf() 
	{
		memset(this, 0, sizeof(*this) ); 
	}

	NDUINT16  dataCount; 	 
	WeaponData  data[WEAPON_NUMBER]; 	//weapon data 
	bool push_data(const WeaponData &val)
	{
		if (dataCount < WEAPON_NUMBER)	{
			data[dataCount++] = val;
			return true;
		}
		return false;
	}

};
int ReadStream(NDIStreamMsg &inmsg,NetMessage::WeaponsBuf &data);
int WriteStream(NDOStreamMsg &omsg,const NetMessage::WeaponsBuf &data);



// ��ʽ����־��Ϣ
struct formatLogInfo 
{
 	formatLogInfo() 
	{
		memset(this, 0, sizeof(*this) ); 
	}

	NDUINT8  logType; 	// log data type  
	NDUINT8  logOp; 	//op type , set , add, sub 
	NDUINT32  roleId; 	//role id  
	NDUINT32  itemId; 	//operate item id  
	NDUINT32  number; 	//item number 
};
int ReadStream(NDIStreamMsg &inmsg,NetMessage::formatLogInfo &data);
int WriteStream(NDOStreamMsg &omsg,const NetMessage::formatLogInfo &data);



// �̵���Ʒ��Ϣ
struct ItemInShop 
{
 	ItemInShop() 
	{
		memset(this, 0, sizeof(*this) ); 
	}

	NDUINT16  itemPackId; 	//��Ʒid 
	NDUINT8  packIndex; 	//��Ʒ�ڰ����е����� 
	NDUINT8  buyNumber; 	//���Թ������,0���ܹ��� 
};
int ReadStream(NDIStreamMsg &inmsg,NetMessage::ItemInShop &data);
int WriteStream(NDOStreamMsg &omsg,const NetMessage::ItemInShop &data);



// �̵������Ʒ
struct ShopInfo 
{
 	ShopInfo() 
	{
		memset(this, 0, sizeof(*this) ); 
	}

	NDUINT16  shopId; 	//id 
	NDUINT16  resetTimes; 	//���ô��� 
	NDUINT64  lastResetTm; 	//�ϴ�����ʱ��time_t 
	NDUINT16  itemsCount; 	 
	ItemInShop  items[SHOP_ITEMS_CAPACITY]; 	//��Ʒ�б� 
	bool push_items(const ItemInShop &val)
	{
		if (itemsCount < SHOP_ITEMS_CAPACITY)	{
			items[itemsCount++] = val;
			return true;
		}
		return false;
	}

};
int ReadStream(NDIStreamMsg &inmsg,NetMessage::ShopInfo &data);
int WriteStream(NDOStreamMsg &omsg,const NetMessage::ShopInfo &data);



struct RoleShopsInfo 
{
 	RoleShopsInfo() 
	{
		memset(this, 0, sizeof(*this) ); 
	}

	NDUINT16  dataCount; 	 
	ShopInfo  data[SHOPS_NUMBER]; 	//�̵깺����Ϣ 
	bool push_data(const ShopInfo &val)
	{
		if (dataCount < SHOPS_NUMBER)	{
			data[dataCount++] = val;
			return true;
		}
		return false;
	}

};
int ReadStream(NDIStreamMsg &inmsg,NetMessage::RoleShopsInfo &data);
int WriteStream(NDOStreamMsg &omsg,const NetMessage::RoleShopsInfo &data);



// ��������
struct TaskInfo 
{
 	TaskInfo() 
	{
		memset(this, 0, sizeof(*this) ); 
	}

	NDUINT16  id; 	//id 
	NDUINT16  progress; 	//���� 
	NDUINT64  taskTM; 	//ʱ�䣺���ʱ�������ȡ������ʱ�� 
};
int ReadStream(NDIStreamMsg &inmsg,NetMessage::TaskInfo &data);
int WriteStream(NDOStreamMsg &omsg,const NetMessage::TaskInfo &data);



// ��������
struct TaskBuf 
{
 	TaskBuf() 
	{
		memset(this, 0, sizeof(*this) ); 
	}

	NDUINT16  dataCount; 	 
	TaskInfo  data[100]; 	//task data 
	bool push_data(const TaskInfo &val)
	{
		if (dataCount < 100)	{
			data[dataCount++] = val;
			return true;
		}
		return false;
	}

};
int ReadStream(NDIStreamMsg &inmsg,NetMessage::TaskBuf &data);
int WriteStream(NDOStreamMsg &omsg,const NetMessage::TaskBuf &data);



// �Ѿ���ɵ���������
struct TaskLog 
{
 	TaskLog() 
	{
		memset(this, 0, sizeof(*this) ); 
	}

	NDUINT16  id; 	//id 
	NDUINT16  number; 	//��ɴ��� 
	NDUINT64  taskTM; 	//ʱ�䣺���ʱ�������ȡ������ʱ�� 
};
int ReadStream(NDIStreamMsg &inmsg,NetMessage::TaskLog &data);
int WriteStream(NDOStreamMsg &omsg,const NetMessage::TaskLog &data);



// ��������
struct TaskLogBuf 
{
 	TaskLogBuf() 
	{
		memset(this, 0, sizeof(*this) ); 
	}

	NDUINT16  dataCount; 	 
	TaskLog  data[100]; 	//task data 
	bool push_data(const TaskLog &val)
	{
		if (dataCount < 100)	{
			data[dataCount++] = val;
			return true;
		}
		return false;
	}

};
int ReadStream(NDIStreamMsg &inmsg,NetMessage::TaskLogBuf &data);
int WriteStream(NDOStreamMsg &omsg,const NetMessage::TaskLogBuf &data);



// ������
struct TaskAwards 
{
 	TaskAwards() 
	{
		memset(this, 0, sizeof(*this) ); 
	}

	NDUINT16  id; 	//id 
	NDUINT16  attrsCount; 	 
	AttrNodeVal  attrs[DROP_ATTR_CAPACITY]; 	//task award attribute 
	bool push_attrs(const AttrNodeVal &val)
	{
		if (attrsCount < DROP_ATTR_CAPACITY)	{
			attrs[attrsCount++] = val;
			return true;
		}
		return false;
	}

	NDUINT16  itemsCount; 	 
	DropItem  items[DROP_ITEM_CAPACITY]; 	//task award item 
	bool push_items(const DropItem &val)
	{
		if (itemsCount < DROP_ITEM_CAPACITY)	{
			items[itemsCount++] = val;
			return true;
		}
		return false;
	}

};
int ReadStream(NDIStreamMsg &inmsg,NetMessage::TaskAwards &data);
int WriteStream(NDOStreamMsg &omsg,const NetMessage::TaskAwards &data);



struct MailInfo 
{
 	MailInfo() 
	{
		memset(this, 0, sizeof(*this) ); 
	}

	NDUINT32  from_id; 	//������id��0ϵͳ 
	NDUINT64  send_tm; 	//����ʱ�� 
	char  title[MAIL_TITLE_LENGTH]; 	//�ʼ����� 
	char  context[MAIL_BODY_LENGTH]; 	//�ʼ����� 
	NDUINT16  attrsCount; 	 
	AttrNodeVal  attrs[3]; 	//���Ը��� 
	bool push_attrs(const AttrNodeVal &val)
	{
		if (attrsCount < 3)	{
			attrs[attrsCount++] = val;
			return true;
		}
		return false;
	}

	NDUINT16  itemsCount; 	 
	DropItem  items[3]; 	//��Ʒ���� 
	bool push_items(const DropItem &val)
	{
		if (itemsCount < 3)	{
			items[itemsCount++] = val;
			return true;
		}
		return false;
	}

};
int ReadStream(NDIStreamMsg &inmsg,NetMessage::MailInfo &data);
int WriteStream(NDOStreamMsg &omsg,const NetMessage::MailInfo &data);



// ���ı�������װ����Ϣ
struct DragonSkill 
{
 	DragonSkill() 
	{
		memset(this, 0, sizeof(*this) ); 
	}

	NDUINT16  skillId; 	//id&level 
	NDUINT8  slot; 	//skill equip pos 
};
int ReadStream(NDIStreamMsg &inmsg,NetMessage::DragonSkill &data);
int WriteStream(NDOStreamMsg &omsg,const NetMessage::DragonSkill &data);



// pvp������Ϣ
struct RolePvpBaseInfo 
{
 	RolePvpBaseInfo() 
	{
		memset(this, 0, sizeof(*this) ); 
	}

	NDUINT32  roleId; 	//id 
	NDUINT16  rankIndex; 	//���� 
	char  name[ROLE_NAME_LENGTH]; 	//���� 
	NDUINT16  attrsCount; 	 
	AttrNodeVal  attrs[ROLE_ATTRS_NUMBER]; 	//���� 
	bool push_attrs(const AttrNodeVal &val)
	{
		if (attrsCount < ROLE_ATTRS_NUMBER)	{
			attrs[attrsCount++] = val;
			return true;
		}
		return false;
	}

};
int ReadStream(NDIStreamMsg &inmsg,NetMessage::RolePvpBaseInfo &data);
int WriteStream(NDOStreamMsg &omsg,const NetMessage::RolePvpBaseInfo &data);



// pvp��ϸ��Ϣ
struct RolePvpDetail 
{
 	RolePvpDetail() 
	{
		memset(this, 0, sizeof(*this) ); 
	}

	NDUINT32  roleId; 	//id 
	NDUINT16  rankIndex; 	//���� 
	char  name[ROLE_NAME_LENGTH]; 	//���� 
	NDUINT16  attrsCount; 	 
	AttrNodeVal  attrs[ROLE_ATTRS_NUMBER]; 	//���� 
	bool push_attrs(const AttrNodeVal &val)
	{
		if (attrsCount < ROLE_ATTRS_NUMBER)	{
			attrs[attrsCount++] = val;
			return true;
		}
		return false;
	}

	NDUINT16  skillsCount; 	 
	SkillInfo  skills[SKILL_CAPACITY]; 	//���� 
	bool push_skills(const SkillInfo &val)
	{
		if (skillsCount < SKILL_CAPACITY)	{
			skills[skillsCount++] = val;
			return true;
		}
		return false;
	}

	NDUINT16  equipsCount; 	 
	EquipInfo  equips[EQUIP_ATTR_NUMBER]; 	//װ�� 
	bool push_equips(const EquipInfo &val)
	{
		if (equipsCount < EQUIP_ATTR_NUMBER)	{
			equips[equipsCount++] = val;
			return true;
		}
		return false;
	}

	NDUINT16  weaponsCount; 	 
	WeaponData  weapons[WEAPON_NUMBER]; 	//���� 
	bool push_weapons(const WeaponData &val)
	{
		if (weaponsCount < WEAPON_NUMBER)	{
			weapons[weaponsCount++] = val;
			return true;
		}
		return false;
	}

	NDUINT16  dragonCount; 	 
	DragonInfo  dragon[DRAGON_EQUIP_SLOT_NUM]; 	//װ������ 
	bool push_dragon(const DragonInfo &val)
	{
		if (dragonCount < DRAGON_EQUIP_SLOT_NUM)	{
			dragon[dragonCount++] = val;
			return true;
		}
		return false;
	}

	NDUINT16  dragonSkillsCount; 	 
	DragonSkill  dragonSkills[DRAGON_SKILL_SLOT_NUM]; 	//װ���õ����ļ��� 
	bool push_dragonSkills(const DragonSkill &val)
	{
		if (dragonSkillsCount < DRAGON_SKILL_SLOT_NUM)	{
			dragonSkills[dragonSkillsCount++] = val;
			return true;
		}
		return false;
	}

};
int ReadStream(NDIStreamMsg &inmsg,NetMessage::RolePvpDetail &data);
int WriteStream(NDOStreamMsg &omsg,const NetMessage::RolePvpDetail &data);



// �����̵깺�����
struct AttrStoreNode 
{
 	AttrStoreNode() 
	{
		memset(this, 0, sizeof(*this) ); 
	}

	NDUINT16  itemId; 	//��ƷID 
	NDUINT16  buyTimes; 	//������� 
};
int ReadStream(NDIStreamMsg &inmsg,NetMessage::AttrStoreNode &data);
int WriteStream(NDOStreamMsg &omsg,const NetMessage::AttrStoreNode &data);



// ���Թ����¼
struct AttrStoreInfo 
{
 	AttrStoreInfo() 
	{
		memset(this, 0, sizeof(*this) ); 
	}

	NDUINT16  itemsCount; 	 
	AttrStoreNode  items[100]; 	//�����̵깺���¼ 
	bool push_items(const AttrStoreNode &val)
	{
		if (itemsCount < 100)	{
			items[itemsCount++] = val;
			return true;
		}
		return false;
	}

};
int ReadStream(NDIStreamMsg &inmsg,NetMessage::AttrStoreInfo &data);
int WriteStream(NDOStreamMsg &omsg,const NetMessage::AttrStoreInfo &data);



// �齱����
struct LotteryNode 
{
 	LotteryNode() 
	{
		memset(this, 0, sizeof(*this) ); 
	}

	NDUINT8  lotteryId; 	//����ID 
	NDUINT8  times; 	//�齱���� 
};
int ReadStream(NDIStreamMsg &inmsg,NetMessage::LotteryNode &data);
int WriteStream(NDOStreamMsg &omsg,const NetMessage::LotteryNode &data);



// �齱��Ϣ
struct LotteryLog 
{
 	LotteryLog() 
	{
		memset(this, 0, sizeof(*this) ); 
	}

	NDUINT16  lotteryCount; 	 
	LotteryNode  lottery[20]; 	//�齱 
	bool push_lottery(const LotteryNode &val)
	{
		if (lotteryCount < 20)	{
			lottery[lotteryCount++] = val;
			return true;
		}
		return false;
	}

};
int ReadStream(NDIStreamMsg &inmsg,NetMessage::LotteryLog &data);
int WriteStream(NDOStreamMsg &omsg,const NetMessage::LotteryLog &data);



struct RoleCreateInfo 
{
 	RoleCreateInfo() 
	{
		memset(this, 0, sizeof(*this) ); 
	}

	char  roleName[ROLE_NAME_LENGTH]; 	//��ɫ���� 
	NDUINT16  roleAttrsCount; 	 
	AttrNodeVal  roleAttrs[ROLE_ATTRS_ON_CREATE]; 	//����ʱ��ʼ�������� 
	bool push_roleAttrs(const AttrNodeVal &val)
	{
		if (roleAttrsCount < ROLE_ATTRS_ON_CREATE)	{
			roleAttrs[roleAttrsCount++] = val;
			return true;
		}
		return false;
	}

};
int ReadStream(NDIStreamMsg &inmsg,NetMessage::RoleCreateInfo &data);
int WriteStream(NDOStreamMsg &omsg,const NetMessage::RoleCreateInfo &data);



struct DragonToFragment 
{
 	DragonToFragment() 
	{
		memset(this, 0, sizeof(*this) ); 
	}

	NDUINT16  dragonId; 	//��id 
	NDUINT16  itemsCount; 	 
	DropItem  items[10]; 	//��Ƭ��Ʒ 
	bool push_items(const DropItem &val)
	{
		if (itemsCount < 10)	{
			items[itemsCount++] = val;
			return true;
		}
		return false;
	}

};
int ReadStream(NDIStreamMsg &inmsg,NetMessage::DragonToFragment &data);
int WriteStream(NDOStreamMsg &omsg,const NetMessage::DragonToFragment &data);



};

#endif

