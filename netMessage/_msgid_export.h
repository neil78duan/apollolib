/* please do not change this file ,
 * auto create by program 
 * create time 2016-8-18 10:42:51 
 */




_APOLLO_MSG_ID_DEFINE(NETMSG_ROLE_GET_LIST_REQ, 0x300) //��ý�ɫ����,data-format:none
_APOLLO_MSG_ID_DEFINE(NETMSG_ROLE_GET_LIST_ACK, 0x301) //��ý�ɫ��,data-format:none
_APOLLO_MSG_ID_DEFINE(NETMSG_ROLE_CREATE_REQ, 0x302) //������ɫ����,data-format:RoleCreateInfo
_APOLLO_MSG_ID_DEFINE(NETMSG_ROLE_CREATE_ACK, 0x303) //������ɫ��,data-format:none
_APOLLO_MSG_ID_DEFINE(NETMSG_ROLE_CHANGE_NAME_REQ, 0x304) //�޸Ľ�ɫ��������,data-format:none
_APOLLO_MSG_ID_DEFINE(NETMSG_ROLE_CHANGE_NAME_ACK, 0x305) //�޸Ľ�ɫ���ִ�,data-format:none
_APOLLO_MSG_ID_DEFINE(NETMSG_ROLE_PACKAGE_INIT_REQ, 0x306) //����������󱳰�����,data-format:none
_APOLLO_MSG_ID_DEFINE(NETMSG_ROLE_PACKAGE_INIT_ACK, 0x307) //���������ر�������,data-format:RolePackageData
_APOLLO_MSG_ID_DEFINE(NETMSG_ROLE_ATTR_INIT_REQ, 0x308) //�����������,data-format:none
_APOLLO_MSG_ID_DEFINE(NETMSG_ROLE_ATTR_INIT_ACK, 0x309) //�����������������,data-format:AttrsBuf
_APOLLO_MSG_ID_DEFINE(NETMSG_ROLE_EQUIP_INIT_REQ, 0x30a) //��������װ������,data-format:none
_APOLLO_MSG_ID_DEFINE(NETMSG_ROLE_EQUIP_INIT_ACK, 0x30b) //��������������װ��,data-format:RoleEquipsData
_APOLLO_MSG_ID_DEFINE(NETMSG_ROLE_ATTR_CHANGE_NTF, 0x30c) //������Ա仯֪ͨ,�������Ӿ���,data-format:AttrsBuf
_APOLLO_MSG_ID_DEFINE(NETMSG_ROLE_ENTER_SCENE_REQ, 0x30d) //������볡��,data-format:int32_t
_APOLLO_MSG_ID_DEFINE(NETMSG_ROLE_ENTER_SCENE_ACK, 0x30e) //�𸴽��볡��,data-format:MsgCommonReply
_APOLLO_MSG_ID_DEFINE(NETMSG_ROLE_SKILL_INIT_REQ, 0x30f) //����������,data-format:none
_APOLLO_MSG_ID_DEFINE(NETMSG_ROLE_SKILL_INIT_ACK, 0x310) //���������ؼ�������,data-format:RoleSkillData
_APOLLO_MSG_ID_DEFINE(NETMSG_ROLE_SKILL_ADD_NTF, 0x311) //��ü���֪ͨ,data-format:SkillInfo
_APOLLO_MSG_ID_DEFINE(NETMSG_ROLE_SKILL_CHANGE_NTF, 0x312) //�������ݸı�֪ͨ,data-format:SkillInfo
_APOLLO_MSG_ID_DEFINE(NETMSG_ROLE_SKILL_DEL_NTF, 0x313) //����ɾ��֪ͨ,data-format:int16_t
_APOLLO_MSG_ID_DEFINE(NETMSG_ROLE_CLIENT_SETTING_REQ, 0x314) //�ͻ�����ص�������Ϣ,data-format:none
_APOLLO_MSG_ID_DEFINE(NETMSG_ROLE_CLIENT_SETTING_ACK, 0x315) //�����ƿͻ��˸���ң������ư�,data-format:none
_APOLLO_MSG_ID_DEFINE(NETMSG_ROLE_CHANGE_CLIENT_SETTING_REQ, 0x316) //�ͻ�����ص�������Ϣ�ı�֪ͨ,data-format:none
_APOLLO_MSG_ID_DEFINE(NETMSG_ROLE_MAPS_SCORE_REQ, 0x317) //������ͼ����,data-format:none
_APOLLO_MSG_ID_DEFINE(NETMSG_ROLE_MAPS_SCORE_ACK, 0x318) //������ͼ���ݸ����,data-format:MapScoreBuf
_APOLLO_MSG_ID_DEFINE(NETMSG_ROLE_REPORT_SCORE_REQ, 0x319) //�ϴ������ɼ�,�ɹ�ʱ�ϴ�,data-format:MapFinish
_APOLLO_MSG_ID_DEFINE(NETMSG_ROLE_MAP_FAILED_REQ, 0x31a) //֪ͨ�������򸱱�ʧ��,data-format:int32_t
_APOLLO_MSG_ID_DEFINE(NETMSG_ROLE_DROP_PACKAGE_NTF, 0x31b) //����֪ͨ,data-format:DropPackage
_APOLLO_MSG_ID_DEFINE(NETMSG_ROLE_MAP_ACHIEV_NTF, 0x31c) //���δ򸱱���óɾ�,data-format:MapAchievAwards
_APOLLO_MSG_ID_DEFINE(NETMSG_ROLE_ACCEL_CD_REQ, 0x31d) //ʹ��rmb ����cd,data-format:MsgCommonRequest
_APOLLO_MSG_ID_DEFINE(NETMSG_ROLE_ACCEL_CD_ACK, 0x31e) //��ʹ��rmb ����cd,data-format:MsgCommonReply
_APOLLO_MSG_ID_DEFINE(NETMSG_ROLE_SWEEP_REQ, 0x31f) //����ɨ��,data-format:int32_t
_APOLLO_MSG_ID_DEFINE(NETMSG_ROLE_SWEEP_ACK, 0x320) //����ɨ�����,data-format:MsgCommonReply



_APOLLO_MSG_ID_DEFINE(NETMSG_ITEM_PACKAGE_ITEM_ADD_NTF, 0x400) //������Ʒ����֪ͨ,data-format:ItemInfo
_APOLLO_MSG_ID_DEFINE(NETMSG_ITEM_PACKAGE_ITEM_CHANGE_NTF, 0x401) //������Ʒ���ݸı�֪ͨ,data-format:ItemInfo
_APOLLO_MSG_ID_DEFINE(NETMSG_ITEM_PACKAGE_ITEM_DEL_NTF, 0x402) //������Ʒɾ��֪ͨ,data-format:int32_t
_APOLLO_MSG_ID_DEFINE(NETMSG_ITEM_PACKAGE_EQUIP_ADD_NTF, 0x403) //����װ������,data-format:EquipInfo
_APOLLO_MSG_ID_DEFINE(NETMSG_ITEM_PACKAGE_EQUIP_CHANGE_NTF, 0x404) //����װ�����ݱ仯֪ͨ,data-format:EquipInfo
_APOLLO_MSG_ID_DEFINE(NETMSG_ITEM_PACKAGE_EQUIP_DEL_NTF, 0x405) //����װ��ɾ��֪ͨ,data-format:int32_t
_APOLLO_MSG_ID_DEFINE(NETMSG_ITEM_EQUIPSLOT_ON_NTF, 0x406) //װ������֪ͨ,instid,data-format:int32_t
_APOLLO_MSG_ID_DEFINE(NETMSG_ITEM_EQUIPSLOT_OFF_NTF, 0x407) //װ������֪ͨ,instid,data-format:int32_t
_APOLLO_MSG_ID_DEFINE(NETMSG_ITEM_EQUIPSLOT_PUT_ON_REQ, 0x408) //����װ��,instid,data-format:int32_t
_APOLLO_MSG_ID_DEFINE(NETMSG_ITEM_EQUIPSLOT_PUT_OFF_REQ, 0x409) //������װ��,instid,data-format:int32_t
_APOLLO_MSG_ID_DEFINE(NETMSG_ITEM_USE_ITEM_REQ, 0x40a) //����ʹ����Ʒ,typeid,data-format:int16
_APOLLO_MSG_ID_DEFINE(NETMSG_ITEM_RESET_EQUIP_REQ, 0x40b) //����ϴ��װ��,data-format:ResetEquip
_APOLLO_MSG_ID_DEFINE(NETMSG_ITEM_RESET_EQUIP_ACK, 0x40c) //����ϴ�������ֻ�гɹ�ʱ�ŷ���,data-format:ResetEquipResult
_APOLLO_MSG_ID_DEFINE(NETMSG_ITEM_RESET_EQUIP_AFFIRM_REQ, 0x40d) //ȷ��ϴ�������0 ȷ�ϣ�1����,data-format:int8_t
_APOLLO_MSG_ID_DEFINE(NETMSG_ITEM_BE_USED_NTF, 0x40e) //��Ʒ��ʹ��֪ͨ,data-format:int16_t:item_type_id, DropPackage:dropItems



_APOLLO_MSG_ID_DEFINE(NETMSG_MALL_BUY_REQ, 0x500) //���򵥸���Ʒ����-ֻ���ڿ����׶�,data-format:MallBuyItem
_APOLLO_MSG_ID_DEFINE(NETMSG_MALL_BUY_ACK, 0x501) //����֪ͨ��ֻ��ʧ�ܲŻ�֪ͨ,data-format:MsgCommonReply
_APOLLO_MSG_ID_DEFINE(NETMSG_MALL_BUY_PACKAGE_REQ, 0x502) //�̵깺�����,shop_item.xlsx int32 package-id,data-format:int32_t
_APOLLO_MSG_ID_DEFINE(NETMSG_MALL_SALE_INSTANT_REQ, 0x503) //������Ʒ��ͨ��instant-id����,data-format:int32_t
_APOLLO_MSG_ID_DEFINE(NETMSG_MALL_SALE_ITEM_REQ, 0x504) //���۵��ߣ�ͨ�����ͺ���������װ������,data-format:MallBuyItem
_APOLLO_MSG_ID_DEFINE(NETMSG_MALL_SALE_ACK, 0x505) //���۵��߷��أ�param ��Ʒʵ��id,data-format:MsgCommonReply
_APOLLO_MSG_ID_DEFINE(NETMSG_MALL_VIEW_SHOW_REQ, 0x506) //�鿴�̵�����,data-format:int16_t
_APOLLO_MSG_ID_DEFINE(NETMSG_MALL_VIEW_SHOW_ACK, 0x507) //�ɹ�,�����̵�����,data-format:ShopInfo
_APOLLO_MSG_ID_DEFINE(NETMSG_MALL_MANUAL_RESET_REQ, 0x508) //�ֶ������̵�,data-format:int16_t
_APOLLO_MSG_ID_DEFINE(NETMSG_MALL_MANUAL_RESET_ACK, 0x509) //�ɹ�,���ر����õ��̵�����,data-format:ShopInfo
_APOLLO_MSG_ID_DEFINE(NETMSG_MALL_BUY_PACKAGE_ACK, 0x50a) //�����,data-format:MsgCommonReply
_APOLLO_MSG_ID_DEFINE(NETMSG_MALL_BAT_SAIL_REQ, 0x50b) //��������,data-format:int16_t:count,[int32_t:instants_id]
_APOLLO_MSG_ID_DEFINE(NETMSG_MALL_BAT_SAIL_ACK, 0x50c) //��������,data-format:MsgCommonReply
_APOLLO_MSG_ID_DEFINE(NETMSG_MALL_ATTR_BUY_REQ, 0x50d) //�����̵깺��,data-format:int16_t:attr_item_id
_APOLLO_MSG_ID_DEFINE(NETMSG_MALL_ATTR_BUY_ACK, 0x50e) //�����̵깺��ظ�,data-format:MsgCommonReply
_APOLLO_MSG_ID_DEFINE(NETMSG_MALL_ATTR_LOGS_REQ, 0x50f) //��������̵�Ĺ����¼,data-format:none
_APOLLO_MSG_ID_DEFINE(NETMSG_MALL_ATTR_LOGS_ACK, 0x510) //���������̵�Ĺ����¼,data-format:AttrStoreInfo



_APOLLO_MSG_ID_DEFINE(NETMSG_MISC_SYSTEN_NOTICE_NTF, 0x600) //ϵͳ����,data-format:string
_APOLLO_MSG_ID_DEFINE(NETMSG_MISC_COMMON_ERROR_NTF, 0x601) //ͨ�ô�����Ϣ,data-format:CommonErrorReply
_APOLLO_MSG_ID_DEFINE(NETMSG_MISC_CHAT_NTF, 0x602) //��������,data-format:string
_APOLLO_MSG_ID_DEFINE(NETMSG_MISC_TIMEZONE_LANGUAGE, 0x603) //֪ͨ�������ͻ��˵����Ժ�ʱ��,data-format:int8_t:timezone,string:language
_APOLLO_MSG_ID_DEFINE(NETMSG_MISC_LOTTERY_REQ, 0x604) //����id,data-format:int16_t:lottery_type
_APOLLO_MSG_ID_DEFINE(NETMSG_MISC_LOTTERY_ACK, 0x605) //�𸴳齱,ʧ�ܷ��س�����Ϣ,data-format:int16_t:packageId, DropPackage:drop_info
_APOLLO_MSG_ID_DEFINE(NETMSG_MISC_GET_LOTTERY_TIME_REQ, 0x606) //����ϴγ齱ʱ��,data-format:NONE
_APOLLO_MSG_ID_DEFINE(NETMSG_MISC_GET_LOTTERY_TIME_ACK, 0x607) //���ϴγ齱,data-format:int64_t:lasttime, int32:CD
_APOLLO_MSG_ID_DEFINE(NETMSG_MISC_GET_LOTTERY_LOG_REQ, 0x608) //���ÿ�������Ѿ��齱����,data-format:NONE
_APOLLO_MSG_ID_DEFINE(NETMSG_MISC_GET_LOTTERY_LOG_ACK, 0x609) //���ϴγ齱,data-format:LotteryLog
_APOLLO_MSG_ID_DEFINE(NETMSG_MISC_GET_SELF_INVIRE_REQ, 0x60a) //����Լ���������,data-format:NONE
_APOLLO_MSG_ID_DEFINE(NETMSG_MISC_GET_SELF_INVIRE_ACK, 0x60b) //�����Լ���������,data-format:int8_t:be_invited, int32_t:invite_num,string:code
_APOLLO_MSG_ID_DEFINE(NETMSG_MISC_INPUT_INVIRE_REQ, 0x60c) //����������,data-format:string:invite_code
_APOLLO_MSG_ID_DEFINE(NETMSG_MISC_INPUT_INVIRE_ACK, 0x60d) //���ط���������,data-format:MsgCommonReply
_APOLLO_MSG_ID_DEFINE(NETMSG_MISC_MAIL_RECEIVE_REQ, 0x60e) //���ʼ�,data-format:NONE
_APOLLO_MSG_ID_DEFINE(NETMSG_MISC_MAIL_RECEIVE_ACK, 0x60f) //�������ʼ�,data-format:int16_t: number, [int32:mailid, MailInfo:mail_body]
_APOLLO_MSG_ID_DEFINE(NETMSG_MISC_MAIL_READ_REQ, 0x610) //���ʼ�,data-format:int32_t: MAILID
_APOLLO_MSG_ID_DEFINE(NETMSG_MISC_MAIL_READ_ACK, 0x611) //�ʼ��Ѷ���,data-format:int32_t: MAILID�� DropPackage:addon_items
_APOLLO_MSG_ID_DEFINE(NETMSG_MISC_MAIL_NEW_NTF, 0x612) //�����ʼ�֪ͨ,data-format:none
_APOLLO_MSG_ID_DEFINE(NETMSG_MISC_SENDTO_GM_REQ, 0x613) //��gm���ʼ�,data-format:string:title, string:body
_APOLLO_MSG_ID_DEFINE(NETMSG_MISC_SENDTO_GM_ACK, 0x614) //�ظ���gm�ʼ�,data-format:MsgCommonReply



_APOLLO_MSG_ID_DEFINE(NETMSG_GM_BROADCAST, 0x700) //���͹���,data-format:none
_APOLLO_MSG_ID_DEFINE(NETMSG_GM_KICK_PLAYER, 0x701) //t��,data-format:int32_t:player_id
_APOLLO_MSG_ID_DEFINE(NETMSG_GM_SHUTDOWN_SERVER, 0x702) //�ط�,data-format:int32_t:exit_code
_APOLLO_MSG_ID_DEFINE(NETMSG_GM_ADD_EXP_REQ, 0x703) //GM�Ӿ���,data-format:int32_t:exp_val
_APOLLO_MSG_ID_DEFINE(NETMSG_GM_SET_LEVEL_REQ, 0x704) //�ĵȼ�,data-format:int32_t:level
_APOLLO_MSG_ID_DEFINE(NETMSG_GM_CHANGE_SERVER_TIME_REQ, 0x705) //�޸ķ�����ʱ��,Сʱ,����,data-format:int32_t:add_hours,int32_t:add_minutes
_APOLLO_MSG_ID_DEFINE(NETMSG_GM_GIVE_ITEM_REQ, 0x706) //������Ʒ32bit:id + 32bits:number,data-format:int32_t:item_id,int32_t:number
_APOLLO_MSG_ID_DEFINE(NETMSG_GM_GIVE_DRAGON_REQ, 0x707) //������:dragonid,data-format:int32_t:dragonId
_APOLLO_MSG_ID_DEFINE(NETMSG_GM_ADD_ATTR_REQ, 0x708) //��������,data-format:int8_t:id,int32_t:value
_APOLLO_MSG_ID_DEFINE(NETMSG_GM_ADD_ATTR_ACK, 0x709) //�ظ���������,data-format:int8_t:id,int32_t:current_value
_APOLLO_MSG_ID_DEFINE(NETMSG_GM_SEND_SYSTEM_MAIL_REQ, 0x70a) //����ϵͳ�ʼ�,data-format:string:title, string:body
_APOLLO_MSG_ID_DEFINE(NETMSG_GM_SEND_MAIL_ITEM_REQ, 0x70b) //���Ͳ����ʼ�,data-format:int32_t:useId, MailInfo:BODY
_APOLLO_MSG_ID_DEFINE(NETMSG_GM_PVP_NPC_INIT_REQ, 0x70c) //��ʼ��pvp npc,data-format:none
_APOLLO_MSG_ID_DEFINE(NETMSG_GM_PVP_RANK_LIST_RESET_REQ, 0x70d) //������а�,data-format:none



_APOLLO_MSG_ID_DEFINE(NETMSG_DRAGON_INIT_REQ, 0x800) //����������Ϣ,data-format:NONE
_APOLLO_MSG_ID_DEFINE(NETMSG_DRAGON_INIT_ACK, 0x801) //������Ϣ,data-format:DragonsBuf:dragonInfo
_APOLLO_MSG_ID_DEFINE(NETMSG_DRAGON_DATA_CHANGE_NTF, 0x802) //�����������ӣ��仯֪ͨ,data-format:DragonInfo:changed_dragon_info
_APOLLO_MSG_ID_DEFINE(NETMSG_DRAGON_DEL_NTF, 0x803) //����ʧ֪ͨ,data-format:int16_t:dragonId
_APOLLO_MSG_ID_DEFINE(NETMSG_DRAGON_BUILD_REQ, 0x804) //�ϳ�������,data-format:int16_t
_APOLLO_MSG_ID_DEFINE(NETMSG_DRAGON_BUILD_ACK, 0x805) //�ظ��ϳ���,data-format:CommonErrorReply
_APOLLO_MSG_ID_DEFINE(NETMSG_DRAGON_GETDATA_REQ, 0x806) //������������������,data-format:int16_t
_APOLLO_MSG_ID_DEFINE(NETMSG_DRAGON_GETDATA_ACK, 0x807) //�ظ���������,data-format:RoleDragon
_APOLLO_MSG_ID_DEFINE(NETMSG_DRAGON_UP_STAR_REQ, 0x808) //����������,data-format:int16_t
_APOLLO_MSG_ID_DEFINE(NETMSG_DRAGON_UP_STAR_ACK, 0x809) //�ظ�����,data-format:CommonErrorReply
_APOLLO_MSG_ID_DEFINE(NETMSG_DRAGON_UP_GRADE_REQ, 0x80a) //������,data-format:int16_t
_APOLLO_MSG_ID_DEFINE(NETMSG_DRAGON_UP_GRADE_ACK, 0x80b) //�ظ�����,data-format:CommonErrorReply
_APOLLO_MSG_ID_DEFINE(NETMSG_DRAGON_ADD_FAVOR_REQ, 0x80c) //�ͻ����������ܶ�[��id�����ܶ�],data-format:int16_t:dragon_id,int32_t:favor
_APOLLO_MSG_ID_DEFINE(NETMSG_DRAGON_ADD_FAVOR_NTF, 0x80d) //���ܶȱ仯֪ͨ,���ս��,data-format:int16_t:dragon_id, int32_t:new_favor
_APOLLO_MSG_ID_DEFINE(NETMSG_DRAGON_TAKE_OUT_REQ, 0x80e) //������ս,data-format:int16_t
_APOLLO_MSG_ID_DEFINE(NETMSG_DRAGON_TAKE_OUT_ACK, 0x80f) //�𸴳�ս,data-format:CommonErrorReply
_APOLLO_MSG_ID_DEFINE(NETMSG_DRAGON_TAKE_IN_REQ, 0x810) //�ջس�ս�������Żزֿ�,data-format:int16_t
_APOLLO_MSG_ID_DEFINE(NETMSG_DRAGON_TAKE_IN_ACK, 0x811) //�����ջس�ս����,data-format:CommonErrorReply
_APOLLO_MSG_ID_DEFINE(NETMSG_DRAGON_FEED_REQ, 0x812) //ι����,��id ����Ʒid,data-format:int16_t, int16_t
_APOLLO_MSG_ID_DEFINE(NETMSG_DRAGON_FEED_ACK, 0x813) //ι����,data-format:CommonErrorReply
_APOLLO_MSG_ID_DEFINE(NETMSG_DRAGON_TO_FRAGMENT_NTF, 0x814) //�������Ƭ֪ͨ,data-format:int16_t:item_type_id
_APOLLO_MSG_ID_DEFINE(NETMSG_DRAGON_EQUIP_REQ, 0x815) //װ��������,data-format:int16_t:DRAGON_ID, int8_t:slot
_APOLLO_MSG_ID_DEFINE(NETMSG_DRAGON_EQUIP_ACK, 0x816) //װ������,data-format:MsgCommonReply
_APOLLO_MSG_ID_DEFINE(NETMSG_DRAGON_UNEQUIP_REQ, 0x817) //ж��������,data-format:int16_t:DRAGON_ID
_APOLLO_MSG_ID_DEFINE(NETMSG_DRAGON_UNEQUIP_ACK, 0x818) //ж������,data-format:MsgCommonReply
_APOLLO_MSG_ID_DEFINE(NETMSG_DRAGON_GET_PASSIVE_SKILL_LIST_REQ, 0x819) //������ı��������б�,data-format:none
_APOLLO_MSG_ID_DEFINE(NETMSG_DRAGON_GET_PASSIVE_SKILL_LIST_ACK, 0x81a) //�������ı𶯼����б�,data-format:RoleSkillData
_APOLLO_MSG_ID_DEFINE(NETMSG_DRAGON_GET_SKILL_POS_LIST_REQ, 0x81b) //���ı�������װ��λ��,data-format:none
_APOLLO_MSG_ID_DEFINE(NETMSG_DRAGON_GET_SKILL_POS_LIST_ACK, 0x81c) //�������ı�������װ��λ��,data-format:int16_t:number,[int16_t]
_APOLLO_MSG_ID_DEFINE(NETMSG_DRAGON_EQUIP_DRAGON_SKILL_REQ, 0x81d) //���ı�������װ��,data-format:int8_t:off_on, int16_t:skill, int16:pos
_APOLLO_MSG_ID_DEFINE(NETMSG_DRAGON_EQUIP_DRAGON_SKILL_ACK, 0x81e) //���ؼ���װ��,data-format:int16_t:number , [int16_t]



_APOLLO_MSG_ID_DEFINE(NETMSG_PVP_GET_LIST_REQ, 0x900) //�鿴��,data-format:int16_t:startIndex,int16_t:numbers 
_APOLLO_MSG_ID_DEFINE(NETMSG_PVP_GET_LIST_ACK, 0x901) //���������б�,data-format:int16_t:number,[RolePvpBaseInfo]
_APOLLO_MSG_ID_DEFINE(NETMSG_PVP_GET_SELF_INDEX_REQ, 0x902) //�����Լ�����,data-format:null
_APOLLO_MSG_ID_DEFINE(NETMSG_PVP_GET_SELF_INDEX_ACK, 0x903) //�����Լ�����,data-format:int16_t:total_number,int16_t:current_num int16_t:selfIndex
_APOLLO_MSG_ID_DEFINE(NETMSG_PVP_MATCH_REQ, 0x904) //�������,data-format:null
_APOLLO_MSG_ID_DEFINE(NETMSG_PVP_MATCH_ACK, 0x905) //�������,data-format:int16_t:number, [RolePvpDetail]
_APOLLO_MSG_ID_DEFINE(NETMSG_PVP_START_BATTLE_REQ, 0x906) //��ʼս��,data-format:int32_t:aim_index,int16_t:map_id
_APOLLO_MSG_ID_DEFINE(NETMSG_PVP_START_BATTLE_ACK, 0x907) //�𸴿�ʼս��,data-format:int32_t:errcode,int16_t:mapId, RolePvpDetail:aimInfo
_APOLLO_MSG_ID_DEFINE(NETMSG_PVP_COMPLETED_REQ, 0x908) //����ս�����,data-format:int8_t:isSuccess
_APOLLO_MSG_ID_DEFINE(NETMSG_PVP_COMPLETED_ACK, 0x909) //��ս�����,ʧ�ܸ�ʽ: int8_t:isSuccess,int32:errorCode,data-format:int8_t:isSuccess,int16_t:oldRankIndex,int16_t:rankIndex, DropPackage
_APOLLO_MSG_ID_DEFINE(NETMSG_PVP_GET_ACHIEVMENT_REQ, 0x90a) //pvp��ȡ��������,data-format:int16_t:awardIndex
_APOLLO_MSG_ID_DEFINE(NETMSG_PVP_GET_ACHIEVMENT_ACK, 0x90b) //pvp��ȡ��������,data-format:int16_t:awardIndex,DropPackage
_APOLLO_MSG_ID_DEFINE(NETMSG_PVP_ADDTO_RANK_REQ, 0x90c) //���Լ��ҵ����а�������а��п�λ�Ļ�,data-format:none
_APOLLO_MSG_ID_DEFINE(NETMSG_PVP_ADDTO_RANK_ACK, 0x90d) //���ؽ������а�,data-format:MsgCommonReply
_APOLLO_MSG_ID_DEFINE(NETMSG_PVP_VIEW_DETAIL_REQ, 0x90e) //�鿴�����ϸ��Ϣ,data-format:int32_t:roleId
_APOLLO_MSG_ID_DEFINE(NETMSG_PVP_VIEW_DETAIL_ACK, 0x90f) //���������ϸ��Ϣ,data-format:RolePvpDetail
_APOLLO_MSG_ID_DEFINE(NETMSG_PVP_CANCEL_REQ, 0x910) //ȡ��PVPս��,data-format:int32_t:reason



_APOLLO_MSG_ID_DEFINE(NETMSG_WEAPON_INIT_WEAPON_DATA_REQ, 0xa00) //������������,data-format:none
_APOLLO_MSG_ID_DEFINE(NETMSG_WEAPON_INIT_WEAPON_DATA_ACK, 0xa01) //������������,data-format:WeaponsBuf
_APOLLO_MSG_ID_DEFINE(NETMSG_WEAPON_WEAPON_NODE_CHANGE_NTF, 0xa02) //�����������ӣ��仯֪ͨ,data-format:WeaponData
_APOLLO_MSG_ID_DEFINE(NETMSG_WEAPON_UPLEVEL_REQ, 0xa03) //������������,data-format:int16_t
_APOLLO_MSG_ID_DEFINE(NETMSG_WEAPON_UPLEVEL_ACK, 0xa04) //����������,data-format:CommonErrorReply
_APOLLO_MSG_ID_DEFINE(NETMSG_WEAPON_SET_SKIN_REQ, 0xa05) //����Ƥ��,data-format:int16_t,skin_id
_APOLLO_MSG_ID_DEFINE(NETMSG_WEAPON_SET_SKIN_ACK, 0xa06) //���ظ���Ƥ��,data-format:CommonErrorReply



_APOLLO_MSG_ID_DEFINE(NETMSG_TASK_INIT_CURRENT_TASK_REQ, 0xb00) //��ȡ����ִ���е�����,data-format:none
_APOLLO_MSG_ID_DEFINE(NETMSG_TASK_INIT_CURRENT_TASK_ACK, 0xb01) //��������ִ���е�����,data-format:TaskBuf
_APOLLO_MSG_ID_DEFINE(NETMSG_TASK_INIT_COMPLTETD_TASK_REQ, 0xb02) //��ȡ��ɵ�����,data-format:none
_APOLLO_MSG_ID_DEFINE(NETMSG_TASK_INIT_COMPLTETD_TASK_ACK, 0xb03) //������ɵ�����,data-format:TaskLogBuf
_APOLLO_MSG_ID_DEFINE(NETMSG_TASK_GET_REQ, 0xb04) //��ȡ����,data-format:int16_t
_APOLLO_MSG_ID_DEFINE(NETMSG_TASK_GET_ACK, 0xb05) //������ȡ����,data-format:MsgCommonReply
_APOLLO_MSG_ID_DEFINE(NETMSG_TASK_COMPLTET_REQ, 0xb06) //�����������,data-format:int16_t
_APOLLO_MSG_ID_DEFINE(NETMSG_TASK_COMPLTET_NTF, 0xb07) //֪ͨ�������,data-format:TaskAwards
_APOLLO_MSG_ID_DEFINE(NETMSG_TASK_GIVEUP_REQ, 0xb08) //��������,data-format:int16_t
_APOLLO_MSG_ID_DEFINE(NETMSG_TASK_GIVEUP_ACK, 0xb09) //���������,data-format:MsgCommonReply
_APOLLO_MSG_ID_DEFINE(NETMSG_TASK_SIGN_IN_REQ, 0xb0a) //ÿ��ǩ��,data-format:none
_APOLLO_MSG_ID_DEFINE(NETMSG_TASK_SIGN_IN_ACK, 0xb0b) //����ǩ��,data-format:MsgCommonReply
_APOLLO_MSG_ID_DEFINE(NETMSG_TASK_SIGN_IN_CHECK_REQ, 0xb0c) //�������Ƿ��Ѿ�ǩ��,data-format:none
_APOLLO_MSG_ID_DEFINE(NETMSG_TASK_SIGN_IN_CHECK_ACK, 0xb0d) //�����Ƿ��Ѿ�ǩ����1���Ѿ�ǩ����0û��,data-format:int8_t
_APOLLO_MSG_ID_DEFINE(NETMSG_TASK_GET_LOGIN_AWARDS_REQ, 0xb0e) //��õ�¼����,data-format:none
_APOLLO_MSG_ID_DEFINE(NETMSG_TASK_GET_LOGIN_AWARDS_ACK, 0xb0f) //���ص�¼�������,data-format:MsgCommonReply
_APOLLO_MSG_ID_DEFINE(NETMSG_TASK_CHECK_LOGIN_AWARDS_REQ, 0xb10) //����¼�����Ƿ��Ѿ���ȡ,data-format:none
_APOLLO_MSG_ID_DEFINE(NETMSG_TASK_CHECK_LOGIN_AWARDS_ACK, 0xb11) //���ؼ����,data-format:int8_t
_APOLLO_MSG_ID_DEFINE(NETMSG_TASK_GET_ACHIEVEMENT_REQ, 0xb12) //��óɾ��б����û�г��־���û�����,data-format:none
_APOLLO_MSG_ID_DEFINE(NETMSG_TASK_GET_ACHIEVEMENT_ACK, 0xb13) //���سɾ��б�,data-format:TaskBuf
_APOLLO_MSG_ID_DEFINE(NETMSG_TASK_GET_ACHIEVEMENT_AWARDS_REQ, 0xb14) //��ȡ�ɾͽ���,data-format:int16_t
_APOLLO_MSG_ID_DEFINE(NETMSG_TASK_GET_ACHIEVEMENT_AWARDS_ACK, 0xb15) //���سɾͽ���,data-format:TaskAwards
_APOLLO_MSG_ID_DEFINE(NETMSG_TASK_ACHIEVEMENT_COMPLETE_NTF, 0xb16) //�ɾʹ��֪ͨ,data-format:int16_t
_APOLLO_MSG_ID_DEFINE(NETMSG_TASK_PROGRESS_CHANGED_NTF, 0xb17) //������ȱ仯֪ͨ,data-format:TaskInfo
_APOLLO_MSG_ID_DEFINE(NETMSG_TASK_BAT_COMPLTET_REQ, 0xb18) //���������������,data-format:int16_t:number, [int16_t:tasks-id]
_APOLLO_MSG_ID_DEFINE(NETMSG_TASK_GET_ACTIVE_TASK_REQ, 0xb19) //��ȡ���Խ��ܵ�����,data-format:NONE
_APOLLO_MSG_ID_DEFINE(NETMSG_TASK_GET_ACTIVE_TASK_ACK, 0xb1a) //���ؿ��Խ��ܵ�����,data-format:int16_t:number, [int16_t:tasks-id]
