/* please do not change this file ,
 * auto create by program 
 * create time 2016-8-18 10:42:51 
 */




_APOLLO_MSG_ID_DEFINE(NETMSG_ROLE_GET_LIST_REQ, 0x300) //获得角色请求,data-format:none
_APOLLO_MSG_ID_DEFINE(NETMSG_ROLE_GET_LIST_ACK, 0x301) //获得角色答复,data-format:none
_APOLLO_MSG_ID_DEFINE(NETMSG_ROLE_CREATE_REQ, 0x302) //创建角色请求,data-format:RoleCreateInfo
_APOLLO_MSG_ID_DEFINE(NETMSG_ROLE_CREATE_ACK, 0x303) //创建角色答复,data-format:none
_APOLLO_MSG_ID_DEFINE(NETMSG_ROLE_CHANGE_NAME_REQ, 0x304) //修改角色名字请求,data-format:none
_APOLLO_MSG_ID_DEFINE(NETMSG_ROLE_CHANGE_NAME_ACK, 0x305) //修改角色名字答复,data-format:none
_APOLLO_MSG_ID_DEFINE(NETMSG_ROLE_PACKAGE_INIT_REQ, 0x306) //向服务器请求背包数据,data-format:none
_APOLLO_MSG_ID_DEFINE(NETMSG_ROLE_PACKAGE_INIT_ACK, 0x307) //服务器返回背包数据,data-format:RolePackageData
_APOLLO_MSG_ID_DEFINE(NETMSG_ROLE_ATTR_INIT_REQ, 0x308) //请求玩家属性,data-format:none
_APOLLO_MSG_ID_DEFINE(NETMSG_ROLE_ATTR_INIT_ACK, 0x309) //服务器返回玩家属性,data-format:AttrsBuf
_APOLLO_MSG_ID_DEFINE(NETMSG_ROLE_EQUIP_INIT_REQ, 0x30a) //请求身上装备数据,data-format:none
_APOLLO_MSG_ID_DEFINE(NETMSG_ROLE_EQUIP_INIT_ACK, 0x30b) //服务器返回身上装备,data-format:RoleEquipsData
_APOLLO_MSG_ID_DEFINE(NETMSG_ROLE_ATTR_CHANGE_NTF, 0x30c) //玩家属性变化通知,升级增加经验,data-format:AttrsBuf
_APOLLO_MSG_ID_DEFINE(NETMSG_ROLE_ENTER_SCENE_REQ, 0x30d) //请求进入场景,data-format:int32_t
_APOLLO_MSG_ID_DEFINE(NETMSG_ROLE_ENTER_SCENE_ACK, 0x30e) //答复进入场景,data-format:MsgCommonReply
_APOLLO_MSG_ID_DEFINE(NETMSG_ROLE_SKILL_INIT_REQ, 0x30f) //请求技能数据,data-format:none
_APOLLO_MSG_ID_DEFINE(NETMSG_ROLE_SKILL_INIT_ACK, 0x310) //服务器返回技能数据,data-format:RoleSkillData
_APOLLO_MSG_ID_DEFINE(NETMSG_ROLE_SKILL_ADD_NTF, 0x311) //获得技能通知,data-format:SkillInfo
_APOLLO_MSG_ID_DEFINE(NETMSG_ROLE_SKILL_CHANGE_NTF, 0x312) //技能数据改变通知,data-format:SkillInfo
_APOLLO_MSG_ID_DEFINE(NETMSG_ROLE_SKILL_DEL_NTF, 0x313) //技能删除通知,data-format:int16_t
_APOLLO_MSG_ID_DEFINE(NETMSG_ROLE_CLIENT_SETTING_REQ, 0x314) //客户端相关的配置信息,data-format:none
_APOLLO_MSG_ID_DEFINE(NETMSG_ROLE_CLIENT_SETTING_ACK, 0x315) //返回推客户端给玩家，二进制包,data-format:none
_APOLLO_MSG_ID_DEFINE(NETMSG_ROLE_CHANGE_CLIENT_SETTING_REQ, 0x316) //客户端相关的配置信息改变通知,data-format:none
_APOLLO_MSG_ID_DEFINE(NETMSG_ROLE_MAPS_SCORE_REQ, 0x317) //请求推图数据,data-format:none
_APOLLO_MSG_ID_DEFINE(NETMSG_ROLE_MAPS_SCORE_ACK, 0x318) //返回推图数据给玩家,data-format:MapScoreBuf
_APOLLO_MSG_ID_DEFINE(NETMSG_ROLE_REPORT_SCORE_REQ, 0x319) //上传副本成绩,成功时上传,data-format:MapFinish
_APOLLO_MSG_ID_DEFINE(NETMSG_ROLE_MAP_FAILED_REQ, 0x31a) //通知服务器打副本失败,data-format:int32_t
_APOLLO_MSG_ID_DEFINE(NETMSG_ROLE_DROP_PACKAGE_NTF, 0x31b) //掉落通知,data-format:DropPackage
_APOLLO_MSG_ID_DEFINE(NETMSG_ROLE_MAP_ACHIEV_NTF, 0x31c) //本次打副本获得成就,data-format:MapAchievAwards
_APOLLO_MSG_ID_DEFINE(NETMSG_ROLE_ACCEL_CD_REQ, 0x31d) //使用rmb 加速cd,data-format:MsgCommonRequest
_APOLLO_MSG_ID_DEFINE(NETMSG_ROLE_ACCEL_CD_ACK, 0x31e) //答复使用rmb 加速cd,data-format:MsgCommonReply
_APOLLO_MSG_ID_DEFINE(NETMSG_ROLE_SWEEP_REQ, 0x31f) //请求扫荡,data-format:int32_t
_APOLLO_MSG_ID_DEFINE(NETMSG_ROLE_SWEEP_ACK, 0x320) //返回扫荡结果,data-format:MsgCommonReply



_APOLLO_MSG_ID_DEFINE(NETMSG_ITEM_PACKAGE_ITEM_ADD_NTF, 0x400) //背包物品增加通知,data-format:ItemInfo
_APOLLO_MSG_ID_DEFINE(NETMSG_ITEM_PACKAGE_ITEM_CHANGE_NTF, 0x401) //背包物品数据改变通知,data-format:ItemInfo
_APOLLO_MSG_ID_DEFINE(NETMSG_ITEM_PACKAGE_ITEM_DEL_NTF, 0x402) //背包物品删除通知,data-format:int32_t
_APOLLO_MSG_ID_DEFINE(NETMSG_ITEM_PACKAGE_EQUIP_ADD_NTF, 0x403) //背包装备增加,data-format:EquipInfo
_APOLLO_MSG_ID_DEFINE(NETMSG_ITEM_PACKAGE_EQUIP_CHANGE_NTF, 0x404) //背包装备数据变化通知,data-format:EquipInfo
_APOLLO_MSG_ID_DEFINE(NETMSG_ITEM_PACKAGE_EQUIP_DEL_NTF, 0x405) //背包装备删除通知,data-format:int32_t
_APOLLO_MSG_ID_DEFINE(NETMSG_ITEM_EQUIPSLOT_ON_NTF, 0x406) //装备穿上通知,instid,data-format:int32_t
_APOLLO_MSG_ID_DEFINE(NETMSG_ITEM_EQUIPSLOT_OFF_NTF, 0x407) //装备脱下通知,instid,data-format:int32_t
_APOLLO_MSG_ID_DEFINE(NETMSG_ITEM_EQUIPSLOT_PUT_ON_REQ, 0x408) //请求穿装备,instid,data-format:int32_t
_APOLLO_MSG_ID_DEFINE(NETMSG_ITEM_EQUIPSLOT_PUT_OFF_REQ, 0x409) //请求脱装备,instid,data-format:int32_t
_APOLLO_MSG_ID_DEFINE(NETMSG_ITEM_USE_ITEM_REQ, 0x40a) //请求使用物品,typeid,data-format:int16
_APOLLO_MSG_ID_DEFINE(NETMSG_ITEM_RESET_EQUIP_REQ, 0x40b) //请求洗练装备,data-format:ResetEquip
_APOLLO_MSG_ID_DEFINE(NETMSG_ITEM_RESET_EQUIP_ACK, 0x40c) //返回洗练结果，只有成功时才返回,data-format:ResetEquipResult
_APOLLO_MSG_ID_DEFINE(NETMSG_ITEM_RESET_EQUIP_AFFIRM_REQ, 0x40d) //确认洗练结果，0 确认，1回退,data-format:int8_t
_APOLLO_MSG_ID_DEFINE(NETMSG_ITEM_BE_USED_NTF, 0x40e) //物品被使用通知,data-format:int16_t:item_type_id, DropPackage:dropItems



_APOLLO_MSG_ID_DEFINE(NETMSG_MALL_BUY_REQ, 0x500) //购买单个物品请求-只用在开发阶段,data-format:MallBuyItem
_APOLLO_MSG_ID_DEFINE(NETMSG_MALL_BUY_ACK, 0x501) //购买通知，只有失败才会通知,data-format:MsgCommonReply
_APOLLO_MSG_ID_DEFINE(NETMSG_MALL_BUY_PACKAGE_REQ, 0x502) //商店购买包裹,shop_item.xlsx int32 package-id,data-format:int32_t
_APOLLO_MSG_ID_DEFINE(NETMSG_MALL_SALE_INSTANT_REQ, 0x503) //出售物品，通过instant-id出售,data-format:int32_t
_APOLLO_MSG_ID_DEFINE(NETMSG_MALL_SALE_ITEM_REQ, 0x504) //出售道具，通过类型和数量，非装备类型,data-format:MallBuyItem
_APOLLO_MSG_ID_DEFINE(NETMSG_MALL_SALE_ACK, 0x505) //出售道具返回，param 物品实例id,data-format:MsgCommonReply
_APOLLO_MSG_ID_DEFINE(NETMSG_MALL_VIEW_SHOW_REQ, 0x506) //查看商店数据,data-format:int16_t
_APOLLO_MSG_ID_DEFINE(NETMSG_MALL_VIEW_SHOW_ACK, 0x507) //成功,返回商店数据,data-format:ShopInfo
_APOLLO_MSG_ID_DEFINE(NETMSG_MALL_MANUAL_RESET_REQ, 0x508) //手动重置商店,data-format:int16_t
_APOLLO_MSG_ID_DEFINE(NETMSG_MALL_MANUAL_RESET_ACK, 0x509) //成功,返回被重置的商店数据,data-format:ShopInfo
_APOLLO_MSG_ID_DEFINE(NETMSG_MALL_BUY_PACKAGE_ACK, 0x50a) //购买答复,data-format:MsgCommonReply
_APOLLO_MSG_ID_DEFINE(NETMSG_MALL_BAT_SAIL_REQ, 0x50b) //批量卖出,data-format:int16_t:count,[int32_t:instants_id]
_APOLLO_MSG_ID_DEFINE(NETMSG_MALL_BAT_SAIL_ACK, 0x50c) //批量卖出,data-format:MsgCommonReply
_APOLLO_MSG_ID_DEFINE(NETMSG_MALL_ATTR_BUY_REQ, 0x50d) //属性商店购买,data-format:int16_t:attr_item_id
_APOLLO_MSG_ID_DEFINE(NETMSG_MALL_ATTR_BUY_ACK, 0x50e) //属性商店购买回复,data-format:MsgCommonReply
_APOLLO_MSG_ID_DEFINE(NETMSG_MALL_ATTR_LOGS_REQ, 0x50f) //获得属性商店的购买记录,data-format:none
_APOLLO_MSG_ID_DEFINE(NETMSG_MALL_ATTR_LOGS_ACK, 0x510) //返回属性商店的购买记录,data-format:AttrStoreInfo



_APOLLO_MSG_ID_DEFINE(NETMSG_MISC_SYSTEN_NOTICE_NTF, 0x600) //系统公告,data-format:string
_APOLLO_MSG_ID_DEFINE(NETMSG_MISC_COMMON_ERROR_NTF, 0x601) //通用错误消息,data-format:CommonErrorReply
_APOLLO_MSG_ID_DEFINE(NETMSG_MISC_CHAT_NTF, 0x602) //文字聊天,data-format:string
_APOLLO_MSG_ID_DEFINE(NETMSG_MISC_TIMEZONE_LANGUAGE, 0x603) //通知服务器客户端的语言和时区,data-format:int8_t:timezone,string:language
_APOLLO_MSG_ID_DEFINE(NETMSG_MISC_LOTTERY_REQ, 0x604) //宝箱id,data-format:int16_t:lottery_type
_APOLLO_MSG_ID_DEFINE(NETMSG_MISC_LOTTERY_ACK, 0x605) //答复抽奖,失败返回出错消息,data-format:int16_t:packageId, DropPackage:drop_info
_APOLLO_MSG_ID_DEFINE(NETMSG_MISC_GET_LOTTERY_TIME_REQ, 0x606) //获得上次抽奖时间,data-format:NONE
_APOLLO_MSG_ID_DEFINE(NETMSG_MISC_GET_LOTTERY_TIME_ACK, 0x607) //答复上次抽奖,data-format:int64_t:lasttime, int32:CD
_APOLLO_MSG_ID_DEFINE(NETMSG_MISC_GET_LOTTERY_LOG_REQ, 0x608) //获得每个宝箱已经抽奖次数,data-format:NONE
_APOLLO_MSG_ID_DEFINE(NETMSG_MISC_GET_LOTTERY_LOG_ACK, 0x609) //答复上次抽奖,data-format:LotteryLog
_APOLLO_MSG_ID_DEFINE(NETMSG_MISC_GET_SELF_INVIRE_REQ, 0x60a) //获得自己的邀请码,data-format:NONE
_APOLLO_MSG_ID_DEFINE(NETMSG_MISC_GET_SELF_INVIRE_ACK, 0x60b) //返回自己的邀请码,data-format:int8_t:be_invited, int32_t:invite_num,string:code
_APOLLO_MSG_ID_DEFINE(NETMSG_MISC_INPUT_INVIRE_REQ, 0x60c) //输入邀请吗,data-format:string:invite_code
_APOLLO_MSG_ID_DEFINE(NETMSG_MISC_INPUT_INVIRE_ACK, 0x60d) //返回返回邀请结果,data-format:MsgCommonReply
_APOLLO_MSG_ID_DEFINE(NETMSG_MISC_MAIL_RECEIVE_REQ, 0x60e) //收邮件,data-format:NONE
_APOLLO_MSG_ID_DEFINE(NETMSG_MISC_MAIL_RECEIVE_ACK, 0x60f) //返回新邮件,data-format:int16_t: number, [int32:mailid, MailInfo:mail_body]
_APOLLO_MSG_ID_DEFINE(NETMSG_MISC_MAIL_READ_REQ, 0x610) //读邮件,data-format:int32_t: MAILID
_APOLLO_MSG_ID_DEFINE(NETMSG_MISC_MAIL_READ_ACK, 0x611) //邮件已读过,data-format:int32_t: MAILID， DropPackage:addon_items
_APOLLO_MSG_ID_DEFINE(NETMSG_MISC_MAIL_NEW_NTF, 0x612) //有新邮件通知,data-format:none
_APOLLO_MSG_ID_DEFINE(NETMSG_MISC_SENDTO_GM_REQ, 0x613) //给gm发邮件,data-format:string:title, string:body
_APOLLO_MSG_ID_DEFINE(NETMSG_MISC_SENDTO_GM_ACK, 0x614) //回复给gm邮件,data-format:MsgCommonReply



_APOLLO_MSG_ID_DEFINE(NETMSG_GM_BROADCAST, 0x700) //发送公告,data-format:none
_APOLLO_MSG_ID_DEFINE(NETMSG_GM_KICK_PLAYER, 0x701) //t人,data-format:int32_t:player_id
_APOLLO_MSG_ID_DEFINE(NETMSG_GM_SHUTDOWN_SERVER, 0x702) //关服,data-format:int32_t:exit_code
_APOLLO_MSG_ID_DEFINE(NETMSG_GM_ADD_EXP_REQ, 0x703) //GM加经验,data-format:int32_t:exp_val
_APOLLO_MSG_ID_DEFINE(NETMSG_GM_SET_LEVEL_REQ, 0x704) //改等级,data-format:int32_t:level
_APOLLO_MSG_ID_DEFINE(NETMSG_GM_CHANGE_SERVER_TIME_REQ, 0x705) //修改服务器时间,小时,分钟,data-format:int32_t:add_hours,int32_t:add_minutes
_APOLLO_MSG_ID_DEFINE(NETMSG_GM_GIVE_ITEM_REQ, 0x706) //发放物品32bit:id + 32bits:number,data-format:int32_t:item_id,int32_t:number
_APOLLO_MSG_ID_DEFINE(NETMSG_GM_GIVE_DRAGON_REQ, 0x707) //发放龙:dragonid,data-format:int32_t:dragonId
_APOLLO_MSG_ID_DEFINE(NETMSG_GM_ADD_ATTR_REQ, 0x708) //增加属性,data-format:int8_t:id,int32_t:value
_APOLLO_MSG_ID_DEFINE(NETMSG_GM_ADD_ATTR_ACK, 0x709) //回复增加属性,data-format:int8_t:id,int32_t:current_value
_APOLLO_MSG_ID_DEFINE(NETMSG_GM_SEND_SYSTEM_MAIL_REQ, 0x70a) //发送系统邮件,data-format:string:title, string:body
_APOLLO_MSG_ID_DEFINE(NETMSG_GM_SEND_MAIL_ITEM_REQ, 0x70b) //发送补偿邮件,data-format:int32_t:useId, MailInfo:BODY
_APOLLO_MSG_ID_DEFINE(NETMSG_GM_PVP_NPC_INIT_REQ, 0x70c) //初始化pvp npc,data-format:none
_APOLLO_MSG_ID_DEFINE(NETMSG_GM_PVP_RANK_LIST_RESET_REQ, 0x70d) //清空排行榜,data-format:none



_APOLLO_MSG_ID_DEFINE(NETMSG_DRAGON_INIT_REQ, 0x800) //请求龙的信息,data-format:NONE
_APOLLO_MSG_ID_DEFINE(NETMSG_DRAGON_INIT_ACK, 0x801) //龙的信息,data-format:DragonsBuf:dragonInfo
_APOLLO_MSG_ID_DEFINE(NETMSG_DRAGON_DATA_CHANGE_NTF, 0x802) //龙的数据增加／变化通知,data-format:DragonInfo:changed_dragon_info
_APOLLO_MSG_ID_DEFINE(NETMSG_DRAGON_DEL_NTF, 0x803) //龙消失通知,data-format:int16_t:dragonId
_APOLLO_MSG_ID_DEFINE(NETMSG_DRAGON_BUILD_REQ, 0x804) //合成龙请求,data-format:int16_t
_APOLLO_MSG_ID_DEFINE(NETMSG_DRAGON_BUILD_ACK, 0x805) //回复合成龙,data-format:CommonErrorReply
_APOLLO_MSG_ID_DEFINE(NETMSG_DRAGON_GETDATA_REQ, 0x806) //获得玩家养的龙的数据,data-format:int16_t
_APOLLO_MSG_ID_DEFINE(NETMSG_DRAGON_GETDATA_ACK, 0x807) //回复龙的数据,data-format:RoleDragon
_APOLLO_MSG_ID_DEFINE(NETMSG_DRAGON_UP_STAR_REQ, 0x808) //龙升星请求,data-format:int16_t
_APOLLO_MSG_ID_DEFINE(NETMSG_DRAGON_UP_STAR_ACK, 0x809) //回复升星,data-format:CommonErrorReply
_APOLLO_MSG_ID_DEFINE(NETMSG_DRAGON_UP_GRADE_REQ, 0x80a) //龙升阶,data-format:int16_t
_APOLLO_MSG_ID_DEFINE(NETMSG_DRAGON_UP_GRADE_ACK, 0x80b) //回复升阶,data-format:CommonErrorReply
_APOLLO_MSG_ID_DEFINE(NETMSG_DRAGON_ADD_FAVOR_REQ, 0x80c) //客户端增加亲密度[龙id和亲密度],data-format:int16_t:dragon_id,int32_t:favor
_APOLLO_MSG_ID_DEFINE(NETMSG_DRAGON_ADD_FAVOR_NTF, 0x80d) //亲密度变化通知,最终结果,data-format:int16_t:dragon_id, int32_t:new_favor
_APOLLO_MSG_ID_DEFINE(NETMSG_DRAGON_TAKE_OUT_REQ, 0x80e) //带龙出战,data-format:int16_t
_APOLLO_MSG_ID_DEFINE(NETMSG_DRAGON_TAKE_OUT_ACK, 0x80f) //答复出战,data-format:CommonErrorReply
_APOLLO_MSG_ID_DEFINE(NETMSG_DRAGON_TAKE_IN_REQ, 0x810) //收回出战的龙，放回仓库,data-format:int16_t
_APOLLO_MSG_ID_DEFINE(NETMSG_DRAGON_TAKE_IN_ACK, 0x811) //返回收回出战的龙,data-format:CommonErrorReply
_APOLLO_MSG_ID_DEFINE(NETMSG_DRAGON_FEED_REQ, 0x812) //喂养龙,龙id ，物品id,data-format:int16_t, int16_t
_APOLLO_MSG_ID_DEFINE(NETMSG_DRAGON_FEED_ACK, 0x813) //喂养龙,data-format:CommonErrorReply
_APOLLO_MSG_ID_DEFINE(NETMSG_DRAGON_TO_FRAGMENT_NTF, 0x814) //龙变成碎片通知,data-format:int16_t:item_type_id
_APOLLO_MSG_ID_DEFINE(NETMSG_DRAGON_EQUIP_REQ, 0x815) //装备龙请求,data-format:int16_t:DRAGON_ID, int8_t:slot
_APOLLO_MSG_ID_DEFINE(NETMSG_DRAGON_EQUIP_ACK, 0x816) //装备龙答复,data-format:MsgCommonReply
_APOLLO_MSG_ID_DEFINE(NETMSG_DRAGON_UNEQUIP_REQ, 0x817) //卸下龙请求,data-format:int16_t:DRAGON_ID
_APOLLO_MSG_ID_DEFINE(NETMSG_DRAGON_UNEQUIP_ACK, 0x818) //卸下龙答复,data-format:MsgCommonReply
_APOLLO_MSG_ID_DEFINE(NETMSG_DRAGON_GET_PASSIVE_SKILL_LIST_REQ, 0x819) //获得龙的被动技能列表,data-format:none
_APOLLO_MSG_ID_DEFINE(NETMSG_DRAGON_GET_PASSIVE_SKILL_LIST_ACK, 0x81a) //返回龙的别动技能列表,data-format:RoleSkillData
_APOLLO_MSG_ID_DEFINE(NETMSG_DRAGON_GET_SKILL_POS_LIST_REQ, 0x81b) //龙的被动技能装备位置,data-format:none
_APOLLO_MSG_ID_DEFINE(NETMSG_DRAGON_GET_SKILL_POS_LIST_ACK, 0x81c) //返回龙的被动技能装备位置,data-format:int16_t:number,[int16_t]
_APOLLO_MSG_ID_DEFINE(NETMSG_DRAGON_EQUIP_DRAGON_SKILL_REQ, 0x81d) //龙的被动技能装备,data-format:int8_t:off_on, int16_t:skill, int16:pos
_APOLLO_MSG_ID_DEFINE(NETMSG_DRAGON_EQUIP_DRAGON_SKILL_ACK, 0x81e) //返回技能装备,data-format:int16_t:number , [int16_t]



_APOLLO_MSG_ID_DEFINE(NETMSG_PVP_GET_LIST_REQ, 0x900) //查看排,data-format:int16_t:startIndex,int16_t:numbers 
_APOLLO_MSG_ID_DEFINE(NETMSG_PVP_GET_LIST_ACK, 0x901) //返回排名列表,data-format:int16_t:number,[RolePvpBaseInfo]
_APOLLO_MSG_ID_DEFINE(NETMSG_PVP_GET_SELF_INDEX_REQ, 0x902) //请求自己排名,data-format:null
_APOLLO_MSG_ID_DEFINE(NETMSG_PVP_GET_SELF_INDEX_ACK, 0x903) //返回自己排名,data-format:int16_t:total_number,int16_t:current_num int16_t:selfIndex
_APOLLO_MSG_ID_DEFINE(NETMSG_PVP_MATCH_REQ, 0x904) //请求配对,data-format:null
_APOLLO_MSG_ID_DEFINE(NETMSG_PVP_MATCH_ACK, 0x905) //返回配对,data-format:int16_t:number, [RolePvpDetail]
_APOLLO_MSG_ID_DEFINE(NETMSG_PVP_START_BATTLE_REQ, 0x906) //开始战斗,data-format:int32_t:aim_index,int16_t:map_id
_APOLLO_MSG_ID_DEFINE(NETMSG_PVP_START_BATTLE_ACK, 0x907) //答复开始战斗,data-format:int32_t:errcode,int16_t:mapId, RolePvpDetail:aimInfo
_APOLLO_MSG_ID_DEFINE(NETMSG_PVP_COMPLETED_REQ, 0x908) //发送战斗结果,data-format:int8_t:isSuccess
_APOLLO_MSG_ID_DEFINE(NETMSG_PVP_COMPLETED_ACK, 0x909) //答复战斗结果,失败格式: int8_t:isSuccess,int32:errorCode,data-format:int8_t:isSuccess,int16_t:oldRankIndex,int16_t:rankIndex, DropPackage
_APOLLO_MSG_ID_DEFINE(NETMSG_PVP_GET_ACHIEVMENT_REQ, 0x90a) //pvp领取排名奖励,data-format:int16_t:awardIndex
_APOLLO_MSG_ID_DEFINE(NETMSG_PVP_GET_ACHIEVMENT_ACK, 0x90b) //pvp领取排名奖励,data-format:int16_t:awardIndex,DropPackage
_APOLLO_MSG_ID_DEFINE(NETMSG_PVP_ADDTO_RANK_REQ, 0x90c) //把自己家到排行榜，如果排行榜有空位的话,data-format:none
_APOLLO_MSG_ID_DEFINE(NETMSG_PVP_ADDTO_RANK_ACK, 0x90d) //返回进入排行榜,data-format:MsgCommonReply
_APOLLO_MSG_ID_DEFINE(NETMSG_PVP_VIEW_DETAIL_REQ, 0x90e) //查看玩家详细信息,data-format:int32_t:roleId
_APOLLO_MSG_ID_DEFINE(NETMSG_PVP_VIEW_DETAIL_ACK, 0x90f) //返回玩家详细信息,data-format:RolePvpDetail
_APOLLO_MSG_ID_DEFINE(NETMSG_PVP_CANCEL_REQ, 0x910) //取消PVP战斗,data-format:int32_t:reason



_APOLLO_MSG_ID_DEFINE(NETMSG_WEAPON_INIT_WEAPON_DATA_REQ, 0xa00) //请求武器数据,data-format:none
_APOLLO_MSG_ID_DEFINE(NETMSG_WEAPON_INIT_WEAPON_DATA_ACK, 0xa01) //返回武器数据,data-format:WeaponsBuf
_APOLLO_MSG_ID_DEFINE(NETMSG_WEAPON_WEAPON_NODE_CHANGE_NTF, 0xa02) //武器数据增加／变化通知,data-format:WeaponData
_APOLLO_MSG_ID_DEFINE(NETMSG_WEAPON_UPLEVEL_REQ, 0xa03) //武器升级请求,data-format:int16_t
_APOLLO_MSG_ID_DEFINE(NETMSG_WEAPON_UPLEVEL_ACK, 0xa04) //答复武器升级,data-format:CommonErrorReply
_APOLLO_MSG_ID_DEFINE(NETMSG_WEAPON_SET_SKIN_REQ, 0xa05) //更换皮肤,data-format:int16_t,skin_id
_APOLLO_MSG_ID_DEFINE(NETMSG_WEAPON_SET_SKIN_ACK, 0xa06) //返回更换皮肤,data-format:CommonErrorReply



_APOLLO_MSG_ID_DEFINE(NETMSG_TASK_INIT_CURRENT_TASK_REQ, 0xb00) //获取正在执行中的任务,data-format:none
_APOLLO_MSG_ID_DEFINE(NETMSG_TASK_INIT_CURRENT_TASK_ACK, 0xb01) //返回正在执行中的任务,data-format:TaskBuf
_APOLLO_MSG_ID_DEFINE(NETMSG_TASK_INIT_COMPLTETD_TASK_REQ, 0xb02) //获取完成的任务,data-format:none
_APOLLO_MSG_ID_DEFINE(NETMSG_TASK_INIT_COMPLTETD_TASK_ACK, 0xb03) //返回完成的任务,data-format:TaskLogBuf
_APOLLO_MSG_ID_DEFINE(NETMSG_TASK_GET_REQ, 0xb04) //领取任务,data-format:int16_t
_APOLLO_MSG_ID_DEFINE(NETMSG_TASK_GET_ACK, 0xb05) //返回领取任务,data-format:MsgCommonReply
_APOLLO_MSG_ID_DEFINE(NETMSG_TASK_COMPLTET_REQ, 0xb06) //请求完成任务,data-format:int16_t
_APOLLO_MSG_ID_DEFINE(NETMSG_TASK_COMPLTET_NTF, 0xb07) //通知任务完成,data-format:TaskAwards
_APOLLO_MSG_ID_DEFINE(NETMSG_TASK_GIVEUP_REQ, 0xb08) //放弃任务,data-format:int16_t
_APOLLO_MSG_ID_DEFINE(NETMSG_TASK_GIVEUP_ACK, 0xb09) //放弃任务答复,data-format:MsgCommonReply
_APOLLO_MSG_ID_DEFINE(NETMSG_TASK_SIGN_IN_REQ, 0xb0a) //每日签到,data-format:none
_APOLLO_MSG_ID_DEFINE(NETMSG_TASK_SIGN_IN_ACK, 0xb0b) //返回签到,data-format:MsgCommonReply
_APOLLO_MSG_ID_DEFINE(NETMSG_TASK_SIGN_IN_CHECK_REQ, 0xb0c) //检测今天是否已经签到,data-format:none
_APOLLO_MSG_ID_DEFINE(NETMSG_TASK_SIGN_IN_CHECK_ACK, 0xb0d) //返回是否已经签到，1，已经签到，0没有,data-format:int8_t
_APOLLO_MSG_ID_DEFINE(NETMSG_TASK_GET_LOGIN_AWARDS_REQ, 0xb0e) //获得登录奖励,data-format:none
_APOLLO_MSG_ID_DEFINE(NETMSG_TASK_GET_LOGIN_AWARDS_ACK, 0xb0f) //返回登录奖励结果,data-format:MsgCommonReply
_APOLLO_MSG_ID_DEFINE(NETMSG_TASK_CHECK_LOGIN_AWARDS_REQ, 0xb10) //检测登录奖励是否已经领取,data-format:none
_APOLLO_MSG_ID_DEFINE(NETMSG_TASK_CHECK_LOGIN_AWARDS_ACK, 0xb11) //返回检测结果,data-format:int8_t
_APOLLO_MSG_ID_DEFINE(NETMSG_TASK_GET_ACHIEVEMENT_REQ, 0xb12) //获得成就列表，如果没有出现就是没有完成,data-format:none
_APOLLO_MSG_ID_DEFINE(NETMSG_TASK_GET_ACHIEVEMENT_ACK, 0xb13) //返回成就列表,data-format:TaskBuf
_APOLLO_MSG_ID_DEFINE(NETMSG_TASK_GET_ACHIEVEMENT_AWARDS_REQ, 0xb14) //领取成就奖励,data-format:int16_t
_APOLLO_MSG_ID_DEFINE(NETMSG_TASK_GET_ACHIEVEMENT_AWARDS_ACK, 0xb15) //返回成就奖励,data-format:TaskAwards
_APOLLO_MSG_ID_DEFINE(NETMSG_TASK_ACHIEVEMENT_COMPLETE_NTF, 0xb16) //成就达成通知,data-format:int16_t
_APOLLO_MSG_ID_DEFINE(NETMSG_TASK_PROGRESS_CHANGED_NTF, 0xb17) //任务进度变化通知,data-format:TaskInfo
_APOLLO_MSG_ID_DEFINE(NETMSG_TASK_BAT_COMPLTET_REQ, 0xb18) //请求批量完成任务,data-format:int16_t:number, [int16_t:tasks-id]
_APOLLO_MSG_ID_DEFINE(NETMSG_TASK_GET_ACTIVE_TASK_REQ, 0xb19) //获取可以接受的任务,data-format:NONE
_APOLLO_MSG_ID_DEFINE(NETMSG_TASK_GET_ACTIVE_TASK_ACK, 0xb1a) //返回可以接受的任务,data-format:int16_t:number, [int16_t:tasks-id]
