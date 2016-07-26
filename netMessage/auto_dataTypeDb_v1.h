/* please do not change this file ,
 * auto create by program 
 * create time 2016-7-19 16:36:55 
 */

#ifndef _AUTO_DATA_TYPE_DEFINE_DB_1_H_
#define _AUTO_DATA_TYPE_DEFINE_DB_1_H_
#include "auto_macroDefine.h"

#include "dataStream.h"

namespace NetMessage 
{
// װ���ĸ������Խڵ�
struct EquipAttrNode_v1 
{
 	EquipAttrNode_v1() 
	{
		memset(this, 0, sizeof(*this) ); 
	}

	NDUINT8  color; 	//����ԭɫ 
	NDUINT8  attr_id; 	//����id 
	float  attr_val; 	//����ֵ 
};
int ReadStream(NDIStreamMsg &inmsg,NetMessage::EquipAttrNode_v1 &data);
int WriteStream(NDOStreamMsg &omsg,const NetMessage::EquipAttrNode_v1 &data);




struct ItemInfo_v1 
{
 	ItemInfo_v1() 
	{
		memset(this, 0, sizeof(*this) ); 
	}

	NDUINT32  instantID; 	//��Ʒʵ��id 
	NDUINT32  typeID; 	//��Ʒ����id 
	NDUINT16  slot; 	//λ�� 
	NDUINT16  number; 	//���� 
	NDUINT8  stat; 	//״̬ 
};
int ReadStream(NDIStreamMsg &inmsg,NetMessage::ItemInfo_v1 &data);
int WriteStream(NDOStreamMsg &omsg,const NetMessage::ItemInfo_v1 &data);




struct EquipInfo_v1 
{
 	EquipInfo_v1() 
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
	EquipAttrNode_v1  attrs[EQUIP_ATTR_NUMBER]; 	//(null) 
	bool push_attrs(const EquipAttrNode_v1 &val)
	{
		if (attrsCount < EQUIP_ATTR_NUMBER)	{
			attrs[attrsCount++] = val;
			return true;
		}
		return false;
	}

};
int ReadStream(NDIStreamMsg &inmsg,NetMessage::EquipInfo_v1 &data);
int WriteStream(NDOStreamMsg &omsg,const NetMessage::EquipInfo_v1 &data);




struct SkillInfo_v1 
{
 	SkillInfo_v1() 
	{
		memset(this, 0, sizeof(*this) ); 
	}

	NDUINT16  id; 	//����id 
	NDUINT8  level; 	//���ܵȼ� 
};
int ReadStream(NDIStreamMsg &inmsg,NetMessage::SkillInfo_v1 &data);
int WriteStream(NDOStreamMsg &omsg,const NetMessage::SkillInfo_v1 &data);




// ��Ҵ򸱱��ĳɼ�
struct MapScore_v1 
{
 	MapScore_v1() 
	{
		memset(this, 0, sizeof(*this) ); 
	}

	NDUINT16  map_id; 	//��ͼ��� 
	NDUINT16  enter_times; 	//�����Ѿ�������� 
	NDUINT8  achievements; 	//��ɳɾͣ���λ������ÿ��λ����һ���ɾͣ�ֻ�����һ�� 
	NDUINT16  score; 	//������0��ʾû����ɸø��� 
	NDUINT64  time; 	//���ʱ�� 
};
int ReadStream(NDIStreamMsg &inmsg,NetMessage::MapScore_v1 &data);
int WriteStream(NDOStreamMsg &omsg,const NetMessage::MapScore_v1 &data);




// ��Ҹ����ɼ���¼
struct MapScoreBuf_v1 
{
 	MapScoreBuf_v1() 
	{
		memset(this, 0, sizeof(*this) ); 
	}

	NDUINT16  dataCount; 	 
	MapScore_v1  data[MAX_MAP_INSTANCE_NUMBER]; 	//(null) 
	bool push_data(const MapScore_v1 &val)
	{
		if (dataCount < MAX_MAP_INSTANCE_NUMBER)	{
			data[dataCount++] = val;
			return true;
		}
		return false;
	}

};
int ReadStream(NDIStreamMsg &inmsg,NetMessage::MapScoreBuf_v1 &data);
int WriteStream(NDOStreamMsg &omsg,const NetMessage::MapScoreBuf_v1 &data);




struct DragonInfo_v1 
{
 	DragonInfo_v1() 
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
int ReadStream(NDIStreamMsg &inmsg,NetMessage::DragonInfo_v1 &data);
int WriteStream(NDOStreamMsg &omsg,const NetMessage::DragonInfo_v1 &data);




struct DragonsBuf_v1 
{
 	DragonsBuf_v1() 
	{
		memset(this, 0, sizeof(*this) ); 
	}

	NDUINT16  dragonsCount; 	 
	DragonInfo_v1  dragons[DRAGON_CAPACITY]; 	//�� 
	bool push_dragons(const DragonInfo_v1 &val)
	{
		if (dragonsCount < DRAGON_CAPACITY)	{
			dragons[dragonsCount++] = val;
			return true;
		}
		return false;
	}

};
int ReadStream(NDIStreamMsg &inmsg,NetMessage::DragonsBuf_v1 &data);
int WriteStream(NDOStreamMsg &omsg,const NetMessage::DragonsBuf_v1 &data);




struct DropWeightNode_v1 
{
 	DropWeightNode_v1() 
	{
		memset(this, 0, sizeof(*this) ); 
	}

	NDUINT16  dropObjId; 	//����ID 
	NDUINT16  weight; 	//����Ȩ�� 
};
int ReadStream(NDIStreamMsg &inmsg,NetMessage::DropWeightNode_v1 &data);
int WriteStream(NDOStreamMsg &omsg,const NetMessage::DropWeightNode_v1 &data);




struct DropWeightBuf_v1 
{
 	DropWeightBuf_v1() 
	{
		memset(this, 0, sizeof(*this) ); 
	}

	NDUINT16  dataCount; 	 
	DropWeightNode_v1  data[DRAP_WEIGHT_CAPACIT]; 	//������Ȩ�� 
	bool push_data(const DropWeightNode_v1 &val)
	{
		if (dataCount < DRAP_WEIGHT_CAPACIT)	{
			data[dataCount++] = val;
			return true;
		}
		return false;
	}

};
int ReadStream(NDIStreamMsg &inmsg,NetMessage::DropWeightBuf_v1 &data);
int WriteStream(NDOStreamMsg &omsg,const NetMessage::DropWeightBuf_v1 &data);




// �̵���Ʒ��Ϣ
struct ItemInShop_v1 
{
 	ItemInShop_v1() 
	{
		memset(this, 0, sizeof(*this) ); 
	}

	NDUINT16  itemPackId; 	//��Ʒid 
	NDUINT8  packIndex; 	//��Ʒ�ڰ����е����� 
	NDUINT8  buyNumber; 	//���Թ������,0���ܹ��� 
};
int ReadStream(NDIStreamMsg &inmsg,NetMessage::ItemInShop_v1 &data);
int WriteStream(NDOStreamMsg &omsg,const NetMessage::ItemInShop_v1 &data);




// �̵������Ʒ
struct ShopInfo_v1 
{
 	ShopInfo_v1() 
	{
		memset(this, 0, sizeof(*this) ); 
	}

	NDUINT16  shopId; 	//id 
	NDUINT16  resetTimes; 	//���ô��� 
	NDUINT64  lastResetTm; 	//�ϴ�����ʱ��time_t 
	NDUINT16  itemsCount; 	 
	ItemInShop_v1  items[SHOP_ITEMS_CAPACITY]; 	//��Ʒ�б� 
	bool push_items(const ItemInShop_v1 &val)
	{
		if (itemsCount < SHOP_ITEMS_CAPACITY)	{
			items[itemsCount++] = val;
			return true;
		}
		return false;
	}

};
int ReadStream(NDIStreamMsg &inmsg,NetMessage::ShopInfo_v1 &data);
int WriteStream(NDOStreamMsg &omsg,const NetMessage::ShopInfo_v1 &data);




struct RoleShopsInfo_v1 
{
 	RoleShopsInfo_v1() 
	{
		memset(this, 0, sizeof(*this) ); 
	}

	NDUINT16  dataCount; 	 
	ShopInfo_v1  data[SHOPS_NUMBER]; 	//�̵깺����Ϣ 
	bool push_data(const ShopInfo_v1 &val)
	{
		if (dataCount < SHOPS_NUMBER)	{
			data[dataCount++] = val;
			return true;
		}
		return false;
	}

};
int ReadStream(NDIStreamMsg &inmsg,NetMessage::RoleShopsInfo_v1 &data);
int WriteStream(NDOStreamMsg &omsg,const NetMessage::RoleShopsInfo_v1 &data);




};

#endif

