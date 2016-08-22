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
// 通用处理回复
struct MsgCommonReply 
{
 	MsgCommonReply() 
	{
		memset(this, 0, sizeof(*this) ); 
	}

	NDUINT8  result; 	//消息返回是否成功0 success, 1 failed 
	NDUINT32  value; 	//服务器返回值 
};
int ReadStream(NDIStreamMsg &inmsg,NetMessage::MsgCommonReply &data);
int WriteStream(NDOStreamMsg &omsg,const NetMessage::MsgCommonReply &data);



// 通用错误返回,服务器发送给客户的
struct CommonErrorReply 
{
 	CommonErrorReply() 
	{
		memset(this, 0, sizeof(*this) ); 
	}

	NDUINT8  errtType; 	//错误类型,参考eCommonErrorType 
	NDUINT32  param; 	//错误参数 
};
int ReadStream(NDIStreamMsg &inmsg,NetMessage::CommonErrorReply &data);
int WriteStream(NDOStreamMsg &omsg,const NetMessage::CommonErrorReply &data);



// 通用消息请求
struct MsgCommonRequest 
{
 	MsgCommonRequest() 
	{
		memset(this, 0, sizeof(*this) ); 
	}

	NDUINT8  type; 	//类型 
	NDUINT32  value; 	//值 
};
int ReadStream(NDIStreamMsg &inmsg,NetMessage::MsgCommonRequest &data);
int WriteStream(NDOStreamMsg &omsg,const NetMessage::MsgCommonRequest &data);



// 装备的附加属性节点
struct EquipAttrNode 
{
 	EquipAttrNode() 
	{
		memset(this, 0, sizeof(*this) ); 
	}

	NDUINT8  color; 	//属性原色 
	NDUINT8  attr_id; 	//属性id 
	float  attr_val; 	//属性值 
};
int ReadStream(NDIStreamMsg &inmsg,NetMessage::EquipAttrNode &data);
int WriteStream(NDOStreamMsg &omsg,const NetMessage::EquipAttrNode &data);



struct ItemInfo 
{
 	ItemInfo() 
	{
		memset(this, 0, sizeof(*this) ); 
	}

	NDUINT32  instantID; 	//物品实例id 
	NDUINT32  typeID; 	//物品类型id 
	NDUINT16  slot; 	//位置 
	NDUINT16  number; 	//数量 
	NDUINT8  stat; 	//状态 
};
int ReadStream(NDIStreamMsg &inmsg,NetMessage::ItemInfo &data);
int WriteStream(NDOStreamMsg &omsg,const NetMessage::ItemInfo &data);



struct EquipInfo 
{
 	EquipInfo() 
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
	NDUINT32  equips[EQUIP_SLOT_NUMBER]; 	//穿身上武器和武器装备数据武器ID小于100 
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
	EquipInfo  equips[EQUIP_PACK_CAPACITY]; 	//装备数据 
	bool push_equips(const EquipInfo &val)
	{
		if (equipsCount < EQUIP_PACK_CAPACITY)	{
			equips[equipsCount++] = val;
			return true;
		}
		return false;
	}

	NDUINT16  itemsCount; 	 
	ItemInfo  items[PACKAGE_CAPACITY]; 	//背包数据 
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

	NDUINT16  id; 	//技能id 
	NDUINT8  level; 	//技能等级 
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
	SkillInfo  datas[SKILL_CAPACITY]; 	//技能数据 
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

	NDUINT8  id; 	//属性编号 
	float  val; 	//属性值 
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
	AttrNodeVal  datas[ROLE_ATTRS_NUMBER]; 	//属性值 
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
	float  datas[ROLE_ATTRS_NUMBER]; 	//属性 
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

	NDUINT32  itemid; 	//物品编号 
	NDUINT16  num; 	//数量 
};
int ReadStream(NDIStreamMsg &inmsg,NetMessage::MallBuyItem &data);
int WriteStream(NDOStreamMsg &omsg,const NetMessage::MallBuyItem &data);



// 玩家打副本的成绩
struct MapScore 
{
 	MapScore() 
	{
		memset(this, 0, sizeof(*this) ); 
	}

	NDUINT16  map_id; 	//地图编号 
	NDUINT16  enter_times; 	//今天已经进入次数 
	NDUINT8  achievements; 	//完成成就，按位操作，每个位代表一个成就，只能完成一次 
	NDUINT16  score; 	//分数，0表示没有完成该副本 
	NDUINT64  time; 	//完成时间 
};
int ReadStream(NDIStreamMsg &inmsg,NetMessage::MapScore &data);
int WriteStream(NDOStreamMsg &omsg,const NetMessage::MapScore &data);



struct SceneMonster 
{
 	SceneMonster() 
	{
		memset(this, 0, sizeof(*this) ); 
	}

	NDUINT16  monsterId; 	//怪物id 
	NDUINT8  number; 	//怪物数量 
};
int ReadStream(NDIStreamMsg &inmsg,NetMessage::SceneMonster &data);
int WriteStream(NDOStreamMsg &omsg,const NetMessage::SceneMonster &data);



// 玩家副本成绩记录
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



// 客户端完成副本
struct MapFinish 
{
 	MapFinish() 
	{
		memset(this, 0, sizeof(*this) ); 
	}

	NDUINT16  map_id; 	//地图编号 
	NDUINT16  score; 	//分数 
	NDUINT8  achievements; 	//本次通关完成的成就 
	NDUINT16  attrsCount; 	 
	AttrNodeVal  attrs[DROP_ATTR_CAPACITY]; 	//打怪收益 
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



// 副本成就奖励
struct MapAchievAwards 
{
 	MapAchievAwards() 
	{
		memset(this, 0, sizeof(*this) ); 
	}

	NDUINT16  map_id; 	//地图编号 
	NDUINT8  isAllComplete; 	//是否全部完成 
	NDUINT8  get_achiev; 	//本次新获得的奖励 
};
int ReadStream(NDIStreamMsg &inmsg,NetMessage::MapAchievAwards &data);
int WriteStream(NDOStreamMsg &omsg,const NetMessage::MapAchievAwards &data);



struct DragonInfo 
{
 	DragonInfo() 
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
int ReadStream(NDIStreamMsg &inmsg,NetMessage::DragonInfo &data);
int WriteStream(NDOStreamMsg &omsg,const NetMessage::DragonInfo &data);



struct DragonsBuf 
{
 	DragonsBuf() 
	{
		memset(this, 0, sizeof(*this) ); 
	}

	NDUINT16  dragonsCount; 	 
	DragonInfo  dragons[DRAGON_CAPACITY]; 	//龙 
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



// 角色养的龙的数据
struct RoleDragon 
{
 	RoleDragon() 
	{
		memset(this, 0, sizeof(*this) ); 
	}

	DragonInfo  baseInf; 	//龙基础数值 
	NDUINT16  skill; 	//技能 
	NDUINT16  rideSkill; 	//骑乘技能 
	AttrsBuf  attrs; 	//属性 
};
int ReadStream(NDIStreamMsg &inmsg,NetMessage::RoleDragon &data);
int WriteStream(NDOStreamMsg &omsg,const NetMessage::RoleDragon &data);



struct DropItem 
{
 	DropItem() 
	{
		memset(this, 0, sizeof(*this) ); 
	}

	NDUINT32  itemId; 	//物品类型id 
	NDUINT16  num; 	//物品数量 
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
	AttrNodeVal  attrs[DROP_ATTR_CAPACITY]; 	//掉落金钱 
	bool push_attrs(const AttrNodeVal &val)
	{
		if (attrsCount < DROP_ATTR_CAPACITY)	{
			attrs[attrsCount++] = val;
			return true;
		}
		return false;
	}

	NDUINT16  itemsCount; 	 
	DropItem  items[DROP_ITEM_CAPACITY]; 	//掉落物品 
	bool push_items(const DropItem &val)
	{
		if (itemsCount < DROP_ITEM_CAPACITY)	{
			items[itemsCount++] = val;
			return true;
		}
		return false;
	}

	NDUINT16  dragonFavor; 	//龙的友好度奖励，给所有装备中的龙 
};
int ReadStream(NDIStreamMsg &inmsg,NetMessage::DropPackage &data);
int WriteStream(NDOStreamMsg &omsg,const NetMessage::DropPackage &data);



struct DropWeightNode 
{
 	DropWeightNode() 
	{
		memset(this, 0, sizeof(*this) ); 
	}

	NDUINT16  dropObjId; 	//掉落ID 
	NDUINT16  weight; 	//掉落权重 
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
	DropWeightNode  data[DRAP_WEIGHT_CAPACIT]; 	//掉落物权重 
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



// 洗炼装备请求
struct ResetEquip 
{
 	ResetEquip() 
	{
		memset(this, 0, sizeof(*this) ); 
	}

	NDUINT32  instId; 	//装备实例id 
	NDUINT8  attrIndex; 	//洗练属性索引 
	NDUINT32  stuffInstId; 	//洗炼消化id 
};
int ReadStream(NDIStreamMsg &inmsg,NetMessage::ResetEquip &data);
int WriteStream(NDOStreamMsg &omsg,const NetMessage::ResetEquip &data);



// 洗炼结果
struct ResetEquipResult 
{
 	ResetEquipResult() 
	{
		memset(this, 0, sizeof(*this) ); 
	}

	NDUINT32  instId; 	//装备实例id 
	NDUINT32  stuffInstId; 	//洗炼消化id 
	NDUINT8  stuffAttrIndex; 	//获得属性索引 
	EquipAttrNode  attrVal; 	//获得洗练结果，属性值 
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
	NDUINT16  skinId; 	//皮肤id 
	NDUINT16  Level; 	//强化等级，初始0 
};
int ReadStream(NDIStreamMsg &inmsg,NetMessage::WeaponData &data);
int WriteStream(NDOStreamMsg &omsg,const NetMessage::WeaponData &data);



// 武器数据
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



// 格式化日志信息
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



// 商店物品信息
struct ItemInShop 
{
 	ItemInShop() 
	{
		memset(this, 0, sizeof(*this) ); 
	}

	NDUINT16  itemPackId; 	//商品id 
	NDUINT8  packIndex; 	//物品在包裹中的索引 
	NDUINT8  buyNumber; 	//可以购买次数,0不能购买 
};
int ReadStream(NDIStreamMsg &inmsg,NetMessage::ItemInShop &data);
int WriteStream(NDOStreamMsg &omsg,const NetMessage::ItemInShop &data);



// 商店出售物品
struct ShopInfo 
{
 	ShopInfo() 
	{
		memset(this, 0, sizeof(*this) ); 
	}

	NDUINT16  shopId; 	//id 
	NDUINT16  resetTimes; 	//重置次数 
	NDUINT64  lastResetTm; 	//上次重置时间time_t 
	NDUINT16  itemsCount; 	 
	ItemInShop  items[SHOP_ITEMS_CAPACITY]; 	//物品列表 
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
	ShopInfo  data[SHOPS_NUMBER]; 	//商店购买信息 
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



// 任务数据
struct TaskInfo 
{
 	TaskInfo() 
	{
		memset(this, 0, sizeof(*this) ); 
	}

	NDUINT16  id; 	//id 
	NDUINT16  progress; 	//进度 
	NDUINT64  taskTM; 	//时间：完成时间或者领取奖励的时间 
};
int ReadStream(NDIStreamMsg &inmsg,NetMessage::TaskInfo &data);
int WriteStream(NDOStreamMsg &omsg,const NetMessage::TaskInfo &data);



// 任务数组
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



// 已经完成的任务数据
struct TaskLog 
{
 	TaskLog() 
	{
		memset(this, 0, sizeof(*this) ); 
	}

	NDUINT16  id; 	//id 
	NDUINT16  number; 	//完成次数 
	NDUINT64  taskTM; 	//时间：完成时间或者领取奖励的时间 
};
int ReadStream(NDIStreamMsg &inmsg,NetMessage::TaskLog &data);
int WriteStream(NDOStreamMsg &omsg,const NetMessage::TaskLog &data);



// 任务数组
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



// 任务奖励
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

	NDUINT32  from_id; 	//发送者id，0系统 
	NDUINT64  send_tm; 	//发送时间 
	char  title[MAIL_TITLE_LENGTH]; 	//邮件标题 
	char  context[MAIL_BODY_LENGTH]; 	//邮件内容 
	NDUINT16  attrsCount; 	 
	AttrNodeVal  attrs[3]; 	//属性附件 
	bool push_attrs(const AttrNodeVal &val)
	{
		if (attrsCount < 3)	{
			attrs[attrsCount++] = val;
			return true;
		}
		return false;
	}

	NDUINT16  itemsCount; 	 
	DropItem  items[3]; 	//物品附件 
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



// 龙的被动技能装备信息
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



// pvp基础信息
struct RolePvpBaseInfo 
{
 	RolePvpBaseInfo() 
	{
		memset(this, 0, sizeof(*this) ); 
	}

	NDUINT32  roleId; 	//id 
	NDUINT16  rankIndex; 	//排名 
	char  name[ROLE_NAME_LENGTH]; 	//名字 
	NDUINT16  attrsCount; 	 
	AttrNodeVal  attrs[ROLE_ATTRS_NUMBER]; 	//属性 
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



// pvp详细信息
struct RolePvpDetail 
{
 	RolePvpDetail() 
	{
		memset(this, 0, sizeof(*this) ); 
	}

	NDUINT32  roleId; 	//id 
	NDUINT16  rankIndex; 	//排名 
	char  name[ROLE_NAME_LENGTH]; 	//名字 
	NDUINT16  attrsCount; 	 
	AttrNodeVal  attrs[ROLE_ATTRS_NUMBER]; 	//属性 
	bool push_attrs(const AttrNodeVal &val)
	{
		if (attrsCount < ROLE_ATTRS_NUMBER)	{
			attrs[attrsCount++] = val;
			return true;
		}
		return false;
	}

	NDUINT16  skillsCount; 	 
	SkillInfo  skills[SKILL_CAPACITY]; 	//技能 
	bool push_skills(const SkillInfo &val)
	{
		if (skillsCount < SKILL_CAPACITY)	{
			skills[skillsCount++] = val;
			return true;
		}
		return false;
	}

	NDUINT16  equipsCount; 	 
	EquipInfo  equips[EQUIP_ATTR_NUMBER]; 	//装备 
	bool push_equips(const EquipInfo &val)
	{
		if (equipsCount < EQUIP_ATTR_NUMBER)	{
			equips[equipsCount++] = val;
			return true;
		}
		return false;
	}

	NDUINT16  weaponsCount; 	 
	WeaponData  weapons[WEAPON_NUMBER]; 	//武器 
	bool push_weapons(const WeaponData &val)
	{
		if (weaponsCount < WEAPON_NUMBER)	{
			weapons[weaponsCount++] = val;
			return true;
		}
		return false;
	}

	NDUINT16  dragonCount; 	 
	DragonInfo  dragon[DRAGON_EQUIP_SLOT_NUM]; 	//装备的龙 
	bool push_dragon(const DragonInfo &val)
	{
		if (dragonCount < DRAGON_EQUIP_SLOT_NUM)	{
			dragon[dragonCount++] = val;
			return true;
		}
		return false;
	}

	NDUINT16  dragonSkillsCount; 	 
	DragonSkill  dragonSkills[DRAGON_SKILL_SLOT_NUM]; 	//装备好的龙的技能 
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



// 属性商店购买次数
struct AttrStoreNode 
{
 	AttrStoreNode() 
	{
		memset(this, 0, sizeof(*this) ); 
	}

	NDUINT16  itemId; 	//商品ID 
	NDUINT16  buyTimes; 	//购买次数 
};
int ReadStream(NDIStreamMsg &inmsg,NetMessage::AttrStoreNode &data);
int WriteStream(NDOStreamMsg &omsg,const NetMessage::AttrStoreNode &data);



// 属性购买记录
struct AttrStoreInfo 
{
 	AttrStoreInfo() 
	{
		memset(this, 0, sizeof(*this) ); 
	}

	NDUINT16  itemsCount; 	 
	AttrStoreNode  items[100]; 	//属性商店购买记录 
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



// 抽奖次数
struct LotteryNode 
{
 	LotteryNode() 
	{
		memset(this, 0, sizeof(*this) ); 
	}

	NDUINT8  lotteryId; 	//宝箱ID 
	NDUINT8  times; 	//抽奖次数 
};
int ReadStream(NDIStreamMsg &inmsg,NetMessage::LotteryNode &data);
int WriteStream(NDOStreamMsg &omsg,const NetMessage::LotteryNode &data);



// 抽奖信息
struct LotteryLog 
{
 	LotteryLog() 
	{
		memset(this, 0, sizeof(*this) ); 
	}

	NDUINT16  lotteryCount; 	 
	LotteryNode  lottery[20]; 	//抽奖 
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

	char  roleName[ROLE_NAME_LENGTH]; 	//角色名字 
	NDUINT16  roleAttrsCount; 	 
	AttrNodeVal  roleAttrs[ROLE_ATTRS_ON_CREATE]; 	//创建时初始化的属性 
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

	NDUINT16  dragonId; 	//龙id 
	NDUINT16  itemsCount; 	 
	DropItem  items[10]; 	//碎片物品 
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

