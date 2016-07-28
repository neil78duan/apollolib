/* please do not change this file ,
 * auto create by program 
 * create time 2016-7-28 12:18:12 
 */

#ifndef _AUTO_DATA_TYPE_DEFINE_DB_1_H_
#define _AUTO_DATA_TYPE_DEFINE_DB_1_H_
#include "auto_macroDefine.h"

#include "dataStream.h"

namespace NetMessage 
{
// 装备的附加属性节点
struct EquipAttrNode_v1 
{
 	EquipAttrNode_v1() 
	{
		memset(this, 0, sizeof(*this) ); 
	}

	NDUINT8  color; 	//属性原色 
	NDUINT8  attr_id; 	//属性id 
	float  attr_val; 	//属性值 
};
int ReadStream(NDIStreamMsg &inmsg,NetMessage::EquipAttrNode_v1 &data);
int WriteStream(NDOStreamMsg &omsg,const NetMessage::EquipAttrNode_v1 &data);




struct ItemInfo_v1 
{
 	ItemInfo_v1() 
	{
		memset(this, 0, sizeof(*this) ); 
	}

	NDUINT32  instantID; 	//物品实例id 
	NDUINT32  typeID; 	//物品类型id 
	NDUINT16  slot; 	//位置 
	NDUINT16  number; 	//数量 
	NDUINT8  stat; 	//状态 
};
int ReadStream(NDIStreamMsg &inmsg,NetMessage::ItemInfo_v1 &data);
int WriteStream(NDOStreamMsg &omsg,const NetMessage::ItemInfo_v1 &data);




struct EquipInfo_v1 
{
 	EquipInfo_v1() 
	{
		memset(this, 0, sizeof(*this) ); 
	}

	NDUINT32  instantID; 	//物品实例id 
	NDUINT32  typeID; 	//物品类型id 
	NDUINT16  slot; 	//位置 
	NDUINT8  level; 	//进阶等级 
	NDUINT8  sub_level; 	//强化等级 
	NDUINT8  stat; 	//状态按位操作 
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

	NDUINT16  id; 	//技能id 
	NDUINT8  level; 	//技能等级 
};
int ReadStream(NDIStreamMsg &inmsg,NetMessage::SkillInfo_v1 &data);
int WriteStream(NDOStreamMsg &omsg,const NetMessage::SkillInfo_v1 &data);




// 玩家打副本的成绩
struct MapScore_v1 
{
 	MapScore_v1() 
	{
		memset(this, 0, sizeof(*this) ); 
	}

	NDUINT16  map_id; 	//地图编号 
	NDUINT16  enter_times; 	//今天已经进入次数 
	NDUINT8  achievements; 	//完成成就，按位操作，每个位代表一个成就，只能完成一次 
	NDUINT16  score; 	//分数，0表示没有完成该副本 
	NDUINT64  time; 	//完成时间 
};
int ReadStream(NDIStreamMsg &inmsg,NetMessage::MapScore_v1 &data);
int WriteStream(NDOStreamMsg &omsg,const NetMessage::MapScore_v1 &data);




// 玩家副本成绩记录
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

	NDUINT16  id; 	//龙id 
	NDUINT16  Level; 	//等级 
	NDUINT8  starLevel; 	//星级 
	NDUINT8  grade; 	//阶级 
	NDUINT8  stat; 	//状态,参考装备状态 
	NDUINT8  slot; 	//摆放位置 
	NDUINT32  favor; 	//友好度 
	NDUINT32  exp; 	//经验 
	NDUINT16  skillsCount; 	 
	NDUINT32  skills[SKILLS_NUM_PER_DRAGON]; 	//龙的技能-ID和等级的组合 
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
	DragonInfo_v1  dragons[DRAGON_CAPACITY]; 	//龙 
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

	NDUINT16  dropObjId; 	//掉落ID 
	NDUINT16  weight; 	//掉落权重 
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
	DropWeightNode_v1  data[DRAP_WEIGHT_CAPACIT]; 	//掉落物权重 
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




// 商店物品信息
struct ItemInShop_v1 
{
 	ItemInShop_v1() 
	{
		memset(this, 0, sizeof(*this) ); 
	}

	NDUINT16  itemPackId; 	//商品id 
	NDUINT8  packIndex; 	//物品在包裹中的索引 
	NDUINT8  buyNumber; 	//可以购买次数,0不能购买 
};
int ReadStream(NDIStreamMsg &inmsg,NetMessage::ItemInShop_v1 &data);
int WriteStream(NDOStreamMsg &omsg,const NetMessage::ItemInShop_v1 &data);




// 商店出售物品
struct ShopInfo_v1 
{
 	ShopInfo_v1() 
	{
		memset(this, 0, sizeof(*this) ); 
	}

	NDUINT16  shopId; 	//id 
	NDUINT16  resetTimes; 	//重置次数 
	NDUINT64  lastResetTm; 	//上次重置时间time_t 
	NDUINT16  itemsCount; 	 
	ItemInShop_v1  items[SHOP_ITEMS_CAPACITY]; 	//物品列表 
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
	ShopInfo_v1  data[SHOPS_NUMBER]; 	//商店购买信息 
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

