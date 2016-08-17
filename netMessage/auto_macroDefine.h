/* please do not change this file ,
 * auto create by program 
 * create time 2016-8-17 16:11:1 
 */

#ifndef _AUTO_MACRO_DEFINE_H_
#define _AUTO_MACRO_DEFINE_H_
namespace NetMessage 
{
#define 	 ROLE_NAME_LENGTH  		40 		//角色名字长度 

#define 	 EQUIP_SLOT_NUMBER  		10 		//装备栏个数 

#define 	 ROLE_ATTRS_NUMBER  		200 		//角色属性个数 

#define 	 ROLE_ATTRS_ON_CREATE  		5 		//角色创建时，客户端发送给服务器的属性个数 

#define 	 PACKAGE_CAPACITY  		500 		//背包容量 

#define 	 EQUIP_PACK_CAPACITY  		100 		//武器背包容量 

#define 	 SKILL_CAPACITY  		20 		//技能容量 

#define 	 MAX_MAP_INSTANCE_NUMBER  		200 		//推图副本个数 

#define 	 CLIENT_SETTING_SIZE  		4096 		//客户端配置缓冲大小 

#define 	 RANK_CAPACITY  		100 		//排行榜容量 

#define 	 PVP_TIME_LIMITED  		180 		//PVP时间限制s 

#define 	 DROP_ATTR_CAPACITY  		8 		//掉落属性奖励个数 

#define 	 DROP_ITEM_CAPACITY  		10 		//一次最多掉落物品数量 

#define 	 PVP_MATCH_ROLE_NUM  		3 		//一次匹配3个 

#define 	 PVP_RANK_PAGE_SIZE  		20 		//每一页的个数 

#define 	 DRAP_WEIGHT_CAPACIT  		1024 		//保存掉落权重的个数 

#define 	 EQUIP_ATTR_NUMBER  		6 		//装备附加属性个数 

#define 	 WEAPON_NUMBER  		2 		//武器个数 

#define 	 DRAGON_EQUIP_SLOT_NUM  		8 		//可以装备龙的个数 

#define 	 DRAGON_SKILL_SLOT_NUM  		5 		//龙的被动技能槽位 

#define 	 DRAGON_PASSIVE_SKILL_TYPE  		4 		//龙影技类型 

#define 	 SKILLS_NUM_PER_DRAGON  		4 		//每个龙有的技术数量-主动技能 

#define 	 WEAPON_MAX_ITEM_ID  		1000 		//武器个数 

#define 	 TABLET_NUMBER  		128 		//石碑节点个数 

#define 	 DRAGON_CAPACITY  		40 		//玩家最多可以拥有的龙的数量 

#define 	 PVP_MAP_DRAGON_CAPACITY  		5 		//PVP地图玩家可以布置的龙的数量 

#define 	 SHOP_ITEMS_CAPACITY  		20 		//商店中一次刷新物品的数量 

#define 	 SHOPS_NUMBER  		20 		//游戏中商店个数 

#define 	 MONSTERS_NUMBER_PER_SCENE  		100 		//每个场景中最多怪物数 

#define 	 MAIL_TITLE_LENGTH  		40 		//邮件标题长度 

#define 	 MAIL_BODY_LENGTH  		256 		//邮件内容长度 

// 普通错误类型 
enum eCommonErrorType { 
	COMMON_ERROR_TYPE_SHORT_ITEM =1,//缺少物品 param 物品id
	COMMON_ERROR_TYPE_ATTR_LOW,//属性太低 param 属性名字
	COMMON_ERROR_TYPE_ATTR_HIGHT,//属性太高 param 属性名字
	COMMON_ERROR_TYPE_IN_CD,//cd中
	COMMON_ERROR_TYPE_IN_TABLE,//返回错误表格中定义的错误 param 错误号
	COMMON_ERROR_TYPE_NEED_PASS_MAP,//需要完成特定副本
};


enum eEquipStatBitsType { 
	EQUIPT_STAT_IN_DRESS =1,//是否穿上
	EQUIPT_STAT_NO_PARSE =2,//禁止分解
	EQUIPT_STAT_NO_SAIL =4,//禁止出售
	EQUIPT_STAT_IN_CALLING =8,//龙召唤中
	EQUIPT_STAT_TAKE_OUT =16,//带龙出战
};


// 物品类型 
enum eItemType { 
	EITEM_TYPE_UNKNOWN =0,//未知类型
	EITEM_TYPE_EQUIP =1,//装备
	EITEM_TYPE_WEAPON,//weapon
	EITEM_TYPE_DEFANCE,//盾
	EITEM_TYPE_DRAGON,//dragon
	EITEM_TYPE_DNA,//dna
	EITEM_TYPE_CONSUME,//消耗品
	EITEM_TYPE_NUMBER,//number
};


// 加速完成任务类型 
enum eAccelerateCD { 
	EACCEL_WEAPON_TABLET =0,//石碑
	EACCEL_NUMBER,//加速类型数量
};


// 任务完成类型 
enum eTaskCompletedType { 
	ETASK_COMPLTET_UNKNOW =0,//未知类型
	ETASK_COMPLTET_TIME,//规定时间
	ETASK_COMPLTET_VIP,//vip登记
	ETASK_COMPLTET_MAKE_WISH_DRAGON,//许愿龙池
	ETASK_COMPLTET_MAP_TYPE,//特定类型副本
	ETASK_COMPLTET_MAP_ID,//特定id副本
	ETASK_COMPLTET_ARENA,//竞技场
	ETASK_COMPLTET_FEED_DRAGON,//喂龙
	ETASK_COMPLTET_GOOD_FINGER,//使用点手
	ETASK_COMPLTET_BUY_IN_STORE,//商店购买
	ETASK_COMPLTET_BUY_IN_MALL,//商场购买
	ETASK_COMPLTET_NUMBER,//number
};


// 成就完成类型 
enum eAchievementCompletedType { 
	EACHIEV_COMPLTET_UNKNOW =0,//未知类型
	EACHIEV_COMPLTET_ATTR,//属性达标
	EACHIEV_COMPLTET_ITEM_TYPE,//收集物品
	EACHIEV_COMPLTET_ITEM_ID,//收集指定类型物品
	EACHIEV_COMPLTET_MAP_TYPE,//特定类型副本
	EACHIEV_COMPLTET_MAP_ID,//特定id副本
	EACHIEV_COMPLTET_DRAGON_UPGRADE,//升级龙
	EACHIEV_COMPLTET_EQUIP_UPGRADE,//升级装备
	EACHIEV_COMPLTET_WEAPON_UPGRADE,//升级武器
	EACHIEV_COMPLTET_BUY_IN_STORE,//商店购买
	EACHIEV_COMPLTET_BUY_IN_MALL,//商场购买
	EACHIEV_COMPLTET_NUMBER,//number
};


};
#endif
