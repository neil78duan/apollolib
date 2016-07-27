/* please do not change this file ,
 * auto create by program 
 * create time 2016-7-27 15:15:15 
 */



#include "auto_dataType.h"
namespace NetMessage 
{

	int ReadStream(NDIStreamMsg &inmsg,NetMessage::MsgCommonReply &data)
	{
		READ_STREAM(inmsg, data.result);
		READ_STREAM(inmsg, data.value);

		return 0; 
	}
	int WriteStream(NDOStreamMsg &omsg,const NetMessage::MsgCommonReply &data)
	{
		WRITE_STREAM(omsg,data.result);
		WRITE_STREAM(omsg,data.value);

		return 0; 
	}

	int ReadStream(NDIStreamMsg &inmsg,NetMessage::CommonErrorReply &data)
	{
		READ_STREAM(inmsg, data.errtType);
		READ_STREAM(inmsg, data.param);

		return 0; 
	}
	int WriteStream(NDOStreamMsg &omsg,const NetMessage::CommonErrorReply &data)
	{
		WRITE_STREAM(omsg,data.errtType);
		WRITE_STREAM(omsg,data.param);

		return 0; 
	}

	int ReadStream(NDIStreamMsg &inmsg,NetMessage::MsgCommonRequest &data)
	{
		READ_STREAM(inmsg, data.type);
		READ_STREAM(inmsg, data.value);

		return 0; 
	}
	int WriteStream(NDOStreamMsg &omsg,const NetMessage::MsgCommonRequest &data)
	{
		WRITE_STREAM(omsg,data.type);
		WRITE_STREAM(omsg,data.value);

		return 0; 
	}

	int ReadStream(NDIStreamMsg &inmsg,NetMessage::EquipAttrNode &data)
	{
		READ_STREAM(inmsg, data.color);
		READ_STREAM(inmsg, data.attr_id);
		READ_STREAM(inmsg, data.attr_val);

		return 0; 
	}
	int WriteStream(NDOStreamMsg &omsg,const NetMessage::EquipAttrNode &data)
	{
		WRITE_STREAM(omsg,data.color);
		WRITE_STREAM(omsg,data.attr_id);
		WRITE_STREAM(omsg,data.attr_val);

		return 0; 
	}

	int ReadStream(NDIStreamMsg &inmsg,NetMessage::ItemInfo &data)
	{
		READ_STREAM(inmsg, data.instantID);
		READ_STREAM(inmsg, data.typeID);
		READ_STREAM(inmsg, data.slot);
		READ_STREAM(inmsg, data.number);
		READ_STREAM(inmsg, data.stat);

		return 0; 
	}
	int WriteStream(NDOStreamMsg &omsg,const NetMessage::ItemInfo &data)
	{
		WRITE_STREAM(omsg,data.instantID);
		WRITE_STREAM(omsg,data.typeID);
		WRITE_STREAM(omsg,data.slot);
		WRITE_STREAM(omsg,data.number);
		WRITE_STREAM(omsg,data.stat);

		return 0; 
	}

	int ReadStream(NDIStreamMsg &inmsg,NetMessage::EquipInfo &data)
	{
		READ_STREAM(inmsg, data.instantID);
		READ_STREAM(inmsg, data.typeID);
		READ_STREAM(inmsg, data.slot);
		READ_STREAM(inmsg, data.level);
		READ_STREAM(inmsg, data.sub_level);
		READ_STREAM(inmsg, data.stat);

		READ_STREAM(inmsg,data.attrsCount);
		if( data.attrsCount > EQUIP_ATTR_NUMBER ) {
			return -1;
		}
 		for(int i =0; i<data.attrsCount;++i ){
			READ_STREAM(inmsg,data.attrs[i]);
		}
 
		return 0; 
	}
	int WriteStream(NDOStreamMsg &omsg,const NetMessage::EquipInfo &data)
	{
		WRITE_STREAM(omsg,data.instantID);
		WRITE_STREAM(omsg,data.typeID);
		WRITE_STREAM(omsg,data.slot);
		WRITE_STREAM(omsg,data.level);
		WRITE_STREAM(omsg,data.sub_level);
		WRITE_STREAM(omsg,data.stat);

		if(data.attrsCount > EQUIP_ATTR_NUMBER) {
			return -1; 
		} 
 		WRITE_STREAM(omsg,data.attrsCount);
		for(int i =0 ; i < data.attrsCount;++i){
			WRITE_STREAM(omsg,data.attrs[i]); 
		} 

		return 0; 
	}

	int ReadStream(NDIStreamMsg &inmsg,NetMessage::RoleEquipsData &data)
	{

		READ_STREAM(inmsg,data.equipsCount);
		if( data.equipsCount > EQUIP_SLOT_NUMBER ) {
			return -1;
		}
 		for(int i =0; i<data.equipsCount;++i ){
			READ_STREAM(inmsg,data.equips[i]);
		}
 
		return 0; 
	}
	int WriteStream(NDOStreamMsg &omsg,const NetMessage::RoleEquipsData &data)
	{

		if(data.equipsCount > EQUIP_SLOT_NUMBER) {
			return -1; 
		} 
 		WRITE_STREAM(omsg,data.equipsCount);
		for(int i =0 ; i < data.equipsCount;++i){
			WRITE_STREAM(omsg,data.equips[i]); 
		} 

		return 0; 
	}

	int ReadStream(NDIStreamMsg &inmsg,NetMessage::RolePackageData &data)
	{

		READ_STREAM(inmsg,data.equipsCount);
		if( data.equipsCount > EQUIP_PACK_CAPACITY ) {
			return -1;
		}
 		for(int i =0; i<data.equipsCount;++i ){
			READ_STREAM(inmsg,data.equips[i]);
		}
 
		READ_STREAM(inmsg,data.itemsCount);
		if( data.itemsCount > PACKAGE_CAPACITY ) {
			return -1;
		}
 		for(int i =0; i<data.itemsCount;++i ){
			READ_STREAM(inmsg,data.items[i]);
		}
 
		return 0; 
	}
	int WriteStream(NDOStreamMsg &omsg,const NetMessage::RolePackageData &data)
	{

		if(data.equipsCount > EQUIP_PACK_CAPACITY) {
			return -1; 
		} 
 		WRITE_STREAM(omsg,data.equipsCount);
		for(int i =0 ; i < data.equipsCount;++i){
			WRITE_STREAM(omsg,data.equips[i]); 
		} 

		if(data.itemsCount > PACKAGE_CAPACITY) {
			return -1; 
		} 
 		WRITE_STREAM(omsg,data.itemsCount);
		for(int i =0 ; i < data.itemsCount;++i){
			WRITE_STREAM(omsg,data.items[i]); 
		} 

		return 0; 
	}

	int ReadStream(NDIStreamMsg &inmsg,NetMessage::SkillInfo &data)
	{
		READ_STREAM(inmsg, data.id);
		READ_STREAM(inmsg, data.level);

		return 0; 
	}
	int WriteStream(NDOStreamMsg &omsg,const NetMessage::SkillInfo &data)
	{
		WRITE_STREAM(omsg,data.id);
		WRITE_STREAM(omsg,data.level);

		return 0; 
	}

	int ReadStream(NDIStreamMsg &inmsg,NetMessage::RoleSkillData &data)
	{

		READ_STREAM(inmsg,data.datasCount);
		if( data.datasCount > SKILL_CAPACITY ) {
			return -1;
		}
 		for(int i =0; i<data.datasCount;++i ){
			READ_STREAM(inmsg,data.datas[i]);
		}
 
		return 0; 
	}
	int WriteStream(NDOStreamMsg &omsg,const NetMessage::RoleSkillData &data)
	{

		if(data.datasCount > SKILL_CAPACITY) {
			return -1; 
		} 
 		WRITE_STREAM(omsg,data.datasCount);
		for(int i =0 ; i < data.datasCount;++i){
			WRITE_STREAM(omsg,data.datas[i]); 
		} 

		return 0; 
	}

	int ReadStream(NDIStreamMsg &inmsg,NetMessage::AttrNodeVal &data)
	{
		READ_STREAM(inmsg, data.id);
		READ_STREAM(inmsg, data.val);

		return 0; 
	}
	int WriteStream(NDOStreamMsg &omsg,const NetMessage::AttrNodeVal &data)
	{
		WRITE_STREAM(omsg,data.id);
		WRITE_STREAM(omsg,data.val);

		return 0; 
	}

	int ReadStream(NDIStreamMsg &inmsg,NetMessage::AttrsBuf &data)
	{

		READ_STREAM(inmsg,data.datasCount);
		if( data.datasCount > ROLE_ATTRS_NUMBER ) {
			return -1;
		}
 		for(int i =0; i<data.datasCount;++i ){
			READ_STREAM(inmsg,data.datas[i]);
		}
 
		return 0; 
	}
	int WriteStream(NDOStreamMsg &omsg,const NetMessage::AttrsBuf &data)
	{

		if(data.datasCount > ROLE_ATTRS_NUMBER) {
			return -1; 
		} 
 		WRITE_STREAM(omsg,data.datasCount);
		for(int i =0 ; i < data.datasCount;++i){
			WRITE_STREAM(omsg,data.datas[i]); 
		} 

		return 0; 
	}

	int ReadStream(NDIStreamMsg &inmsg,NetMessage::RoleAttrsData &data)
	{

		READ_STREAM(inmsg,data.datasCount);
		if( data.datasCount > ROLE_ATTRS_NUMBER ) {
			return -1;
		}
 		for(int i =0; i<data.datasCount;++i ){
			READ_STREAM(inmsg,data.datas[i]);
		}
 
		return 0; 
	}
	int WriteStream(NDOStreamMsg &omsg,const NetMessage::RoleAttrsData &data)
	{

		if(data.datasCount > ROLE_ATTRS_NUMBER) {
			return -1; 
		} 
 		WRITE_STREAM(omsg,data.datasCount);
		for(int i =0 ; i < data.datasCount;++i){
			WRITE_STREAM(omsg,data.datas[i]); 
		} 

		return 0; 
	}

	int ReadStream(NDIStreamMsg &inmsg,NetMessage::MallBuyItem &data)
	{
		READ_STREAM(inmsg, data.itemid);
		READ_STREAM(inmsg, data.num);

		return 0; 
	}
	int WriteStream(NDOStreamMsg &omsg,const NetMessage::MallBuyItem &data)
	{
		WRITE_STREAM(omsg,data.itemid);
		WRITE_STREAM(omsg,data.num);

		return 0; 
	}

	int ReadStream(NDIStreamMsg &inmsg,NetMessage::MapScore &data)
	{
		READ_STREAM(inmsg, data.map_id);
		READ_STREAM(inmsg, data.enter_times);
		READ_STREAM(inmsg, data.achievements);
		READ_STREAM(inmsg, data.score);
		READ_STREAM(inmsg, data.time);

		return 0; 
	}
	int WriteStream(NDOStreamMsg &omsg,const NetMessage::MapScore &data)
	{
		WRITE_STREAM(omsg,data.map_id);
		WRITE_STREAM(omsg,data.enter_times);
		WRITE_STREAM(omsg,data.achievements);
		WRITE_STREAM(omsg,data.score);
		WRITE_STREAM(omsg,data.time);

		return 0; 
	}

	int ReadStream(NDIStreamMsg &inmsg,NetMessage::SceneMonster &data)
	{
		READ_STREAM(inmsg, data.monsterId);
		READ_STREAM(inmsg, data.number);

		return 0; 
	}
	int WriteStream(NDOStreamMsg &omsg,const NetMessage::SceneMonster &data)
	{
		WRITE_STREAM(omsg,data.monsterId);
		WRITE_STREAM(omsg,data.number);

		return 0; 
	}

	int ReadStream(NDIStreamMsg &inmsg,NetMessage::MapScoreBuf &data)
	{

		READ_STREAM(inmsg,data.dataCount);
		if( data.dataCount > MAX_MAP_INSTANCE_NUMBER ) {
			return -1;
		}
 		for(int i =0; i<data.dataCount;++i ){
			READ_STREAM(inmsg,data.data[i]);
		}
 
		return 0; 
	}
	int WriteStream(NDOStreamMsg &omsg,const NetMessage::MapScoreBuf &data)
	{

		if(data.dataCount > MAX_MAP_INSTANCE_NUMBER) {
			return -1; 
		} 
 		WRITE_STREAM(omsg,data.dataCount);
		for(int i =0 ; i < data.dataCount;++i){
			WRITE_STREAM(omsg,data.data[i]); 
		} 

		return 0; 
	}

	int ReadStream(NDIStreamMsg &inmsg,NetMessage::MapFinish &data)
	{
		READ_STREAM(inmsg, data.map_id);
		READ_STREAM(inmsg, data.score);
		READ_STREAM(inmsg, data.achievements);

		READ_STREAM(inmsg,data.attrsCount);
		if( data.attrsCount > DROP_ATTR_CAPACITY ) {
			return -1;
		}
 		for(int i =0; i<data.attrsCount;++i ){
			READ_STREAM(inmsg,data.attrs[i]);
		}
 
		return 0; 
	}
	int WriteStream(NDOStreamMsg &omsg,const NetMessage::MapFinish &data)
	{
		WRITE_STREAM(omsg,data.map_id);
		WRITE_STREAM(omsg,data.score);
		WRITE_STREAM(omsg,data.achievements);

		if(data.attrsCount > DROP_ATTR_CAPACITY) {
			return -1; 
		} 
 		WRITE_STREAM(omsg,data.attrsCount);
		for(int i =0 ; i < data.attrsCount;++i){
			WRITE_STREAM(omsg,data.attrs[i]); 
		} 

		return 0; 
	}

	int ReadStream(NDIStreamMsg &inmsg,NetMessage::MapAchievAwards &data)
	{
		READ_STREAM(inmsg, data.map_id);
		READ_STREAM(inmsg, data.isAllComplete);
		READ_STREAM(inmsg, data.get_achiev);

		return 0; 
	}
	int WriteStream(NDOStreamMsg &omsg,const NetMessage::MapAchievAwards &data)
	{
		WRITE_STREAM(omsg,data.map_id);
		WRITE_STREAM(omsg,data.isAllComplete);
		WRITE_STREAM(omsg,data.get_achiev);

		return 0; 
	}

	int ReadStream(NDIStreamMsg &inmsg,NetMessage::DragonInfo &data)
	{
		READ_STREAM(inmsg, data.id);
		READ_STREAM(inmsg, data.Level);
		READ_STREAM(inmsg, data.starLevel);
		READ_STREAM(inmsg, data.grade);
		READ_STREAM(inmsg, data.stat);
		READ_STREAM(inmsg, data.slot);
		READ_STREAM(inmsg, data.favor);
		READ_STREAM(inmsg, data.exp);

		READ_STREAM(inmsg,data.skillsCount);
		if( data.skillsCount > SKILLS_NUM_PER_DRAGON ) {
			return -1;
		}
 		for(int i =0; i<data.skillsCount;++i ){
			READ_STREAM(inmsg,data.skills[i]);
		}
 
		return 0; 
	}
	int WriteStream(NDOStreamMsg &omsg,const NetMessage::DragonInfo &data)
	{
		WRITE_STREAM(omsg,data.id);
		WRITE_STREAM(omsg,data.Level);
		WRITE_STREAM(omsg,data.starLevel);
		WRITE_STREAM(omsg,data.grade);
		WRITE_STREAM(omsg,data.stat);
		WRITE_STREAM(omsg,data.slot);
		WRITE_STREAM(omsg,data.favor);
		WRITE_STREAM(omsg,data.exp);

		if(data.skillsCount > SKILLS_NUM_PER_DRAGON) {
			return -1; 
		} 
 		WRITE_STREAM(omsg,data.skillsCount);
		for(int i =0 ; i < data.skillsCount;++i){
			WRITE_STREAM(omsg,data.skills[i]); 
		} 

		return 0; 
	}

	int ReadStream(NDIStreamMsg &inmsg,NetMessage::DragonsBuf &data)
	{

		READ_STREAM(inmsg,data.dragonsCount);
		if( data.dragonsCount > DRAGON_CAPACITY ) {
			return -1;
		}
 		for(int i =0; i<data.dragonsCount;++i ){
			READ_STREAM(inmsg,data.dragons[i]);
		}
 
		return 0; 
	}
	int WriteStream(NDOStreamMsg &omsg,const NetMessage::DragonsBuf &data)
	{

		if(data.dragonsCount > DRAGON_CAPACITY) {
			return -1; 
		} 
 		WRITE_STREAM(omsg,data.dragonsCount);
		for(int i =0 ; i < data.dragonsCount;++i){
			WRITE_STREAM(omsg,data.dragons[i]); 
		} 

		return 0; 
	}

	int ReadStream(NDIStreamMsg &inmsg,NetMessage::RoleDragon &data)
	{
		READ_STREAM(inmsg, data.baseInf);
		READ_STREAM(inmsg, data.skill);
		READ_STREAM(inmsg, data.rideSkill);
		READ_STREAM(inmsg, data.attrs);

		return 0; 
	}
	int WriteStream(NDOStreamMsg &omsg,const NetMessage::RoleDragon &data)
	{
		WRITE_STREAM(omsg,data.baseInf);
		WRITE_STREAM(omsg,data.skill);
		WRITE_STREAM(omsg,data.rideSkill);
		WRITE_STREAM(omsg,data.attrs);

		return 0; 
	}

	int ReadStream(NDIStreamMsg &inmsg,NetMessage::DropItem &data)
	{
		READ_STREAM(inmsg, data.itemId);
		READ_STREAM(inmsg, data.num);

		return 0; 
	}
	int WriteStream(NDOStreamMsg &omsg,const NetMessage::DropItem &data)
	{
		WRITE_STREAM(omsg,data.itemId);
		WRITE_STREAM(omsg,data.num);

		return 0; 
	}

	int ReadStream(NDIStreamMsg &inmsg,NetMessage::DropPackage &data)
	{

		READ_STREAM(inmsg,data.attrsCount);
		if( data.attrsCount > DROP_ATTR_CAPACITY ) {
			return -1;
		}
 		for(int i =0; i<data.attrsCount;++i ){
			READ_STREAM(inmsg,data.attrs[i]);
		}
 
		READ_STREAM(inmsg,data.itemsCount);
		if( data.itemsCount > DROP_ITEM_CAPACITY ) {
			return -1;
		}
 		for(int i =0; i<data.itemsCount;++i ){
			READ_STREAM(inmsg,data.items[i]);
		}
 		READ_STREAM(inmsg, data.dragonFavor);

		return 0; 
	}
	int WriteStream(NDOStreamMsg &omsg,const NetMessage::DropPackage &data)
	{

		if(data.attrsCount > DROP_ATTR_CAPACITY) {
			return -1; 
		} 
 		WRITE_STREAM(omsg,data.attrsCount);
		for(int i =0 ; i < data.attrsCount;++i){
			WRITE_STREAM(omsg,data.attrs[i]); 
		} 

		if(data.itemsCount > DROP_ITEM_CAPACITY) {
			return -1; 
		} 
 		WRITE_STREAM(omsg,data.itemsCount);
		for(int i =0 ; i < data.itemsCount;++i){
			WRITE_STREAM(omsg,data.items[i]); 
		} 
		WRITE_STREAM(omsg,data.dragonFavor);

		return 0; 
	}

	int ReadStream(NDIStreamMsg &inmsg,NetMessage::DropWeightNode &data)
	{
		READ_STREAM(inmsg, data.dropObjId);
		READ_STREAM(inmsg, data.weight);

		return 0; 
	}
	int WriteStream(NDOStreamMsg &omsg,const NetMessage::DropWeightNode &data)
	{
		WRITE_STREAM(omsg,data.dropObjId);
		WRITE_STREAM(omsg,data.weight);

		return 0; 
	}

	int ReadStream(NDIStreamMsg &inmsg,NetMessage::DropWeightBuf &data)
	{

		READ_STREAM(inmsg,data.dataCount);
		if( data.dataCount > DRAP_WEIGHT_CAPACIT ) {
			return -1;
		}
 		for(int i =0; i<data.dataCount;++i ){
			READ_STREAM(inmsg,data.data[i]);
		}
 
		return 0; 
	}
	int WriteStream(NDOStreamMsg &omsg,const NetMessage::DropWeightBuf &data)
	{

		if(data.dataCount > DRAP_WEIGHT_CAPACIT) {
			return -1; 
		} 
 		WRITE_STREAM(omsg,data.dataCount);
		for(int i =0 ; i < data.dataCount;++i){
			WRITE_STREAM(omsg,data.data[i]); 
		} 

		return 0; 
	}

	int ReadStream(NDIStreamMsg &inmsg,NetMessage::ResetEquip &data)
	{
		READ_STREAM(inmsg, data.instId);
		READ_STREAM(inmsg, data.attrIndex);
		READ_STREAM(inmsg, data.stuffInstId);

		return 0; 
	}
	int WriteStream(NDOStreamMsg &omsg,const NetMessage::ResetEquip &data)
	{
		WRITE_STREAM(omsg,data.instId);
		WRITE_STREAM(omsg,data.attrIndex);
		WRITE_STREAM(omsg,data.stuffInstId);

		return 0; 
	}

	int ReadStream(NDIStreamMsg &inmsg,NetMessage::ResetEquipResult &data)
	{
		READ_STREAM(inmsg, data.instId);
		READ_STREAM(inmsg, data.stuffInstId);
		READ_STREAM(inmsg, data.stuffAttrIndex);
		READ_STREAM(inmsg, data.attrVal);

		return 0; 
	}
	int WriteStream(NDOStreamMsg &omsg,const NetMessage::ResetEquipResult &data)
	{
		WRITE_STREAM(omsg,data.instId);
		WRITE_STREAM(omsg,data.stuffInstId);
		WRITE_STREAM(omsg,data.stuffAttrIndex);
		WRITE_STREAM(omsg,data.attrVal);

		return 0; 
	}

	int ReadStream(NDIStreamMsg &inmsg,NetMessage::WeaponData &data)
	{
		READ_STREAM(inmsg, data.id);
		READ_STREAM(inmsg, data.skinId);
		READ_STREAM(inmsg, data.Level);

		return 0; 
	}
	int WriteStream(NDOStreamMsg &omsg,const NetMessage::WeaponData &data)
	{
		WRITE_STREAM(omsg,data.id);
		WRITE_STREAM(omsg,data.skinId);
		WRITE_STREAM(omsg,data.Level);

		return 0; 
	}

	int ReadStream(NDIStreamMsg &inmsg,NetMessage::WeaponsBuf &data)
	{

		READ_STREAM(inmsg,data.dataCount);
		if( data.dataCount > WEAPON_NUMBER ) {
			return -1;
		}
 		for(int i =0; i<data.dataCount;++i ){
			READ_STREAM(inmsg,data.data[i]);
		}
 
		return 0; 
	}
	int WriteStream(NDOStreamMsg &omsg,const NetMessage::WeaponsBuf &data)
	{

		if(data.dataCount > WEAPON_NUMBER) {
			return -1; 
		} 
 		WRITE_STREAM(omsg,data.dataCount);
		for(int i =0 ; i < data.dataCount;++i){
			WRITE_STREAM(omsg,data.data[i]); 
		} 

		return 0; 
	}

	int ReadStream(NDIStreamMsg &inmsg,NetMessage::formatLogInfo &data)
	{
		READ_STREAM(inmsg, data.logType);
		READ_STREAM(inmsg, data.logOp);
		READ_STREAM(inmsg, data.roleId);
		READ_STREAM(inmsg, data.itemId);
		READ_STREAM(inmsg, data.number);

		return 0; 
	}
	int WriteStream(NDOStreamMsg &omsg,const NetMessage::formatLogInfo &data)
	{
		WRITE_STREAM(omsg,data.logType);
		WRITE_STREAM(omsg,data.logOp);
		WRITE_STREAM(omsg,data.roleId);
		WRITE_STREAM(omsg,data.itemId);
		WRITE_STREAM(omsg,data.number);

		return 0; 
	}

	int ReadStream(NDIStreamMsg &inmsg,NetMessage::ItemInShop &data)
	{
		READ_STREAM(inmsg, data.itemPackId);
		READ_STREAM(inmsg, data.packIndex);
		READ_STREAM(inmsg, data.buyNumber);

		return 0; 
	}
	int WriteStream(NDOStreamMsg &omsg,const NetMessage::ItemInShop &data)
	{
		WRITE_STREAM(omsg,data.itemPackId);
		WRITE_STREAM(omsg,data.packIndex);
		WRITE_STREAM(omsg,data.buyNumber);

		return 0; 
	}

	int ReadStream(NDIStreamMsg &inmsg,NetMessage::ShopInfo &data)
	{
		READ_STREAM(inmsg, data.shopId);
		READ_STREAM(inmsg, data.resetTimes);
		READ_STREAM(inmsg, data.lastResetTm);

		READ_STREAM(inmsg,data.itemsCount);
		if( data.itemsCount > SHOP_ITEMS_CAPACITY ) {
			return -1;
		}
 		for(int i =0; i<data.itemsCount;++i ){
			READ_STREAM(inmsg,data.items[i]);
		}
 
		return 0; 
	}
	int WriteStream(NDOStreamMsg &omsg,const NetMessage::ShopInfo &data)
	{
		WRITE_STREAM(omsg,data.shopId);
		WRITE_STREAM(omsg,data.resetTimes);
		WRITE_STREAM(omsg,data.lastResetTm);

		if(data.itemsCount > SHOP_ITEMS_CAPACITY) {
			return -1; 
		} 
 		WRITE_STREAM(omsg,data.itemsCount);
		for(int i =0 ; i < data.itemsCount;++i){
			WRITE_STREAM(omsg,data.items[i]); 
		} 

		return 0; 
	}

	int ReadStream(NDIStreamMsg &inmsg,NetMessage::RoleShopsInfo &data)
	{

		READ_STREAM(inmsg,data.dataCount);
		if( data.dataCount > SHOPS_NUMBER ) {
			return -1;
		}
 		for(int i =0; i<data.dataCount;++i ){
			READ_STREAM(inmsg,data.data[i]);
		}
 
		return 0; 
	}
	int WriteStream(NDOStreamMsg &omsg,const NetMessage::RoleShopsInfo &data)
	{

		if(data.dataCount > SHOPS_NUMBER) {
			return -1; 
		} 
 		WRITE_STREAM(omsg,data.dataCount);
		for(int i =0 ; i < data.dataCount;++i){
			WRITE_STREAM(omsg,data.data[i]); 
		} 

		return 0; 
	}

	int ReadStream(NDIStreamMsg &inmsg,NetMessage::TaskInfo &data)
	{
		READ_STREAM(inmsg, data.id);
		READ_STREAM(inmsg, data.progress);
		READ_STREAM(inmsg, data.taskTM);

		return 0; 
	}
	int WriteStream(NDOStreamMsg &omsg,const NetMessage::TaskInfo &data)
	{
		WRITE_STREAM(omsg,data.id);
		WRITE_STREAM(omsg,data.progress);
		WRITE_STREAM(omsg,data.taskTM);

		return 0; 
	}

	int ReadStream(NDIStreamMsg &inmsg,NetMessage::TaskBuf &data)
	{

		READ_STREAM(inmsg,data.dataCount);
		if( data.dataCount > 100 ) {
			return -1;
		}
 		for(int i =0; i<data.dataCount;++i ){
			READ_STREAM(inmsg,data.data[i]);
		}
 
		return 0; 
	}
	int WriteStream(NDOStreamMsg &omsg,const NetMessage::TaskBuf &data)
	{

		if(data.dataCount > 100) {
			return -1; 
		} 
 		WRITE_STREAM(omsg,data.dataCount);
		for(int i =0 ; i < data.dataCount;++i){
			WRITE_STREAM(omsg,data.data[i]); 
		} 

		return 0; 
	}

	int ReadStream(NDIStreamMsg &inmsg,NetMessage::TaskLog &data)
	{
		READ_STREAM(inmsg, data.id);
		READ_STREAM(inmsg, data.number);
		READ_STREAM(inmsg, data.taskTM);

		return 0; 
	}
	int WriteStream(NDOStreamMsg &omsg,const NetMessage::TaskLog &data)
	{
		WRITE_STREAM(omsg,data.id);
		WRITE_STREAM(omsg,data.number);
		WRITE_STREAM(omsg,data.taskTM);

		return 0; 
	}

	int ReadStream(NDIStreamMsg &inmsg,NetMessage::TaskLogBuf &data)
	{

		READ_STREAM(inmsg,data.dataCount);
		if( data.dataCount > 100 ) {
			return -1;
		}
 		for(int i =0; i<data.dataCount;++i ){
			READ_STREAM(inmsg,data.data[i]);
		}
 
		return 0; 
	}
	int WriteStream(NDOStreamMsg &omsg,const NetMessage::TaskLogBuf &data)
	{

		if(data.dataCount > 100) {
			return -1; 
		} 
 		WRITE_STREAM(omsg,data.dataCount);
		for(int i =0 ; i < data.dataCount;++i){
			WRITE_STREAM(omsg,data.data[i]); 
		} 

		return 0; 
	}

	int ReadStream(NDIStreamMsg &inmsg,NetMessage::TaskAwards &data)
	{
		READ_STREAM(inmsg, data.id);

		READ_STREAM(inmsg,data.attrsCount);
		if( data.attrsCount > DROP_ATTR_CAPACITY ) {
			return -1;
		}
 		for(int i =0; i<data.attrsCount;++i ){
			READ_STREAM(inmsg,data.attrs[i]);
		}
 
		READ_STREAM(inmsg,data.itemsCount);
		if( data.itemsCount > DROP_ITEM_CAPACITY ) {
			return -1;
		}
 		for(int i =0; i<data.itemsCount;++i ){
			READ_STREAM(inmsg,data.items[i]);
		}
 
		return 0; 
	}
	int WriteStream(NDOStreamMsg &omsg,const NetMessage::TaskAwards &data)
	{
		WRITE_STREAM(omsg,data.id);

		if(data.attrsCount > DROP_ATTR_CAPACITY) {
			return -1; 
		} 
 		WRITE_STREAM(omsg,data.attrsCount);
		for(int i =0 ; i < data.attrsCount;++i){
			WRITE_STREAM(omsg,data.attrs[i]); 
		} 

		if(data.itemsCount > DROP_ITEM_CAPACITY) {
			return -1; 
		} 
 		WRITE_STREAM(omsg,data.itemsCount);
		for(int i =0 ; i < data.itemsCount;++i){
			WRITE_STREAM(omsg,data.items[i]); 
		} 

		return 0; 
	}

	int ReadStream(NDIStreamMsg &inmsg,NetMessage::MailInfo &data)
	{
		READ_STREAM(inmsg, data.from_id);
		READ_STREAM(inmsg, data.send_tm);
		READ_STREAM_STRING(inmsg, data.title, MAIL_TITLE_LENGTH);
		READ_STREAM_STRING(inmsg, data.context, MAIL_BODY_LENGTH);

		READ_STREAM(inmsg,data.attrsCount);
		if( data.attrsCount > 3 ) {
			return -1;
		}
 		for(int i =0; i<data.attrsCount;++i ){
			READ_STREAM(inmsg,data.attrs[i]);
		}
 
		READ_STREAM(inmsg,data.itemsCount);
		if( data.itemsCount > 3 ) {
			return -1;
		}
 		for(int i =0; i<data.itemsCount;++i ){
			READ_STREAM(inmsg,data.items[i]);
		}
 
		return 0; 
	}
	int WriteStream(NDOStreamMsg &omsg,const NetMessage::MailInfo &data)
	{
		WRITE_STREAM(omsg,data.from_id);
		WRITE_STREAM(omsg,data.send_tm);
		WRITE_STREAM(omsg,data.title);
		WRITE_STREAM(omsg,data.context);

		if(data.attrsCount > 3) {
			return -1; 
		} 
 		WRITE_STREAM(omsg,data.attrsCount);
		for(int i =0 ; i < data.attrsCount;++i){
			WRITE_STREAM(omsg,data.attrs[i]); 
		} 

		if(data.itemsCount > 3) {
			return -1; 
		} 
 		WRITE_STREAM(omsg,data.itemsCount);
		for(int i =0 ; i < data.itemsCount;++i){
			WRITE_STREAM(omsg,data.items[i]); 
		} 

		return 0; 
	}

	int ReadStream(NDIStreamMsg &inmsg,NetMessage::DragonSkill &data)
	{
		READ_STREAM(inmsg, data.skillId);
		READ_STREAM(inmsg, data.slot);

		return 0; 
	}
	int WriteStream(NDOStreamMsg &omsg,const NetMessage::DragonSkill &data)
	{
		WRITE_STREAM(omsg,data.skillId);
		WRITE_STREAM(omsg,data.slot);

		return 0; 
	}

	int ReadStream(NDIStreamMsg &inmsg,NetMessage::RolePvpBaseInfo &data)
	{
		READ_STREAM(inmsg, data.roleId);
		READ_STREAM(inmsg, data.rankIndex);
		READ_STREAM_STRING(inmsg, data.name, ROLE_NAME_LENGTH);

		READ_STREAM(inmsg,data.attrsCount);
		if( data.attrsCount > ROLE_ATTRS_NUMBER ) {
			return -1;
		}
 		for(int i =0; i<data.attrsCount;++i ){
			READ_STREAM(inmsg,data.attrs[i]);
		}
 
		return 0; 
	}
	int WriteStream(NDOStreamMsg &omsg,const NetMessage::RolePvpBaseInfo &data)
	{
		WRITE_STREAM(omsg,data.roleId);
		WRITE_STREAM(omsg,data.rankIndex);
		WRITE_STREAM(omsg,data.name);

		if(data.attrsCount > ROLE_ATTRS_NUMBER) {
			return -1; 
		} 
 		WRITE_STREAM(omsg,data.attrsCount);
		for(int i =0 ; i < data.attrsCount;++i){
			WRITE_STREAM(omsg,data.attrs[i]); 
		} 

		return 0; 
	}

	int ReadStream(NDIStreamMsg &inmsg,NetMessage::RolePvpDetail &data)
	{
		READ_STREAM(inmsg, data.roleId);
		READ_STREAM(inmsg, data.rankIndex);
		READ_STREAM_STRING(inmsg, data.name, ROLE_NAME_LENGTH);

		READ_STREAM(inmsg,data.attrsCount);
		if( data.attrsCount > ROLE_ATTRS_NUMBER ) {
			return -1;
		}
 		for(int i =0; i<data.attrsCount;++i ){
			READ_STREAM(inmsg,data.attrs[i]);
		}
 
		READ_STREAM(inmsg,data.skillsCount);
		if( data.skillsCount > SKILL_CAPACITY ) {
			return -1;
		}
 		for(int i =0; i<data.skillsCount;++i ){
			READ_STREAM(inmsg,data.skills[i]);
		}
 
		READ_STREAM(inmsg,data.equipsCount);
		if( data.equipsCount > EQUIP_ATTR_NUMBER ) {
			return -1;
		}
 		for(int i =0; i<data.equipsCount;++i ){
			READ_STREAM(inmsg,data.equips[i]);
		}
 
		READ_STREAM(inmsg,data.weaponsCount);
		if( data.weaponsCount > WEAPON_NUMBER ) {
			return -1;
		}
 		for(int i =0; i<data.weaponsCount;++i ){
			READ_STREAM(inmsg,data.weapons[i]);
		}
 
		READ_STREAM(inmsg,data.dragonCount);
		if( data.dragonCount > DRAGON_EQUIP_SLOT_NUM ) {
			return -1;
		}
 		for(int i =0; i<data.dragonCount;++i ){
			READ_STREAM(inmsg,data.dragon[i]);
		}
 
		READ_STREAM(inmsg,data.dragonSkillsCount);
		if( data.dragonSkillsCount > DRAGON_SKILL_SLOT_NUM ) {
			return -1;
		}
 		for(int i =0; i<data.dragonSkillsCount;++i ){
			READ_STREAM(inmsg,data.dragonSkills[i]);
		}
 
		return 0; 
	}
	int WriteStream(NDOStreamMsg &omsg,const NetMessage::RolePvpDetail &data)
	{
		WRITE_STREAM(omsg,data.roleId);
		WRITE_STREAM(omsg,data.rankIndex);
		WRITE_STREAM(omsg,data.name);

		if(data.attrsCount > ROLE_ATTRS_NUMBER) {
			return -1; 
		} 
 		WRITE_STREAM(omsg,data.attrsCount);
		for(int i =0 ; i < data.attrsCount;++i){
			WRITE_STREAM(omsg,data.attrs[i]); 
		} 

		if(data.skillsCount > SKILL_CAPACITY) {
			return -1; 
		} 
 		WRITE_STREAM(omsg,data.skillsCount);
		for(int i =0 ; i < data.skillsCount;++i){
			WRITE_STREAM(omsg,data.skills[i]); 
		} 

		if(data.equipsCount > EQUIP_ATTR_NUMBER) {
			return -1; 
		} 
 		WRITE_STREAM(omsg,data.equipsCount);
		for(int i =0 ; i < data.equipsCount;++i){
			WRITE_STREAM(omsg,data.equips[i]); 
		} 

		if(data.weaponsCount > WEAPON_NUMBER) {
			return -1; 
		} 
 		WRITE_STREAM(omsg,data.weaponsCount);
		for(int i =0 ; i < data.weaponsCount;++i){
			WRITE_STREAM(omsg,data.weapons[i]); 
		} 

		if(data.dragonCount > DRAGON_EQUIP_SLOT_NUM) {
			return -1; 
		} 
 		WRITE_STREAM(omsg,data.dragonCount);
		for(int i =0 ; i < data.dragonCount;++i){
			WRITE_STREAM(omsg,data.dragon[i]); 
		} 

		if(data.dragonSkillsCount > DRAGON_SKILL_SLOT_NUM) {
			return -1; 
		} 
 		WRITE_STREAM(omsg,data.dragonSkillsCount);
		for(int i =0 ; i < data.dragonSkillsCount;++i){
			WRITE_STREAM(omsg,data.dragonSkills[i]); 
		} 

		return 0; 
	}

	int ReadStream(NDIStreamMsg &inmsg,NetMessage::AttrStoreNode &data)
	{
		READ_STREAM(inmsg, data.itemId);
		READ_STREAM(inmsg, data.buyTimes);

		return 0; 
	}
	int WriteStream(NDOStreamMsg &omsg,const NetMessage::AttrStoreNode &data)
	{
		WRITE_STREAM(omsg,data.itemId);
		WRITE_STREAM(omsg,data.buyTimes);

		return 0; 
	}

	int ReadStream(NDIStreamMsg &inmsg,NetMessage::AttrStoreInfo &data)
	{

		READ_STREAM(inmsg,data.itemsCount);
		if( data.itemsCount > 100 ) {
			return -1;
		}
 		for(int i =0; i<data.itemsCount;++i ){
			READ_STREAM(inmsg,data.items[i]);
		}
 
		return 0; 
	}
	int WriteStream(NDOStreamMsg &omsg,const NetMessage::AttrStoreInfo &data)
	{

		if(data.itemsCount > 100) {
			return -1; 
		} 
 		WRITE_STREAM(omsg,data.itemsCount);
		for(int i =0 ; i < data.itemsCount;++i){
			WRITE_STREAM(omsg,data.items[i]); 
		} 

		return 0; 
	}

	int ReadStream(NDIStreamMsg &inmsg,NetMessage::LotteryNode &data)
	{
		READ_STREAM(inmsg, data.lotteryId);
		READ_STREAM(inmsg, data.times);

		return 0; 
	}
	int WriteStream(NDOStreamMsg &omsg,const NetMessage::LotteryNode &data)
	{
		WRITE_STREAM(omsg,data.lotteryId);
		WRITE_STREAM(omsg,data.times);

		return 0; 
	}

	int ReadStream(NDIStreamMsg &inmsg,NetMessage::LotteryLog &data)
	{

		READ_STREAM(inmsg,data.lotteryCount);
		if( data.lotteryCount > 20 ) {
			return -1;
		}
 		for(int i =0; i<data.lotteryCount;++i ){
			READ_STREAM(inmsg,data.lottery[i]);
		}
 
		return 0; 
	}
	int WriteStream(NDOStreamMsg &omsg,const NetMessage::LotteryLog &data)
	{

		if(data.lotteryCount > 20) {
			return -1; 
		} 
 		WRITE_STREAM(omsg,data.lotteryCount);
		for(int i =0 ; i < data.lotteryCount;++i){
			WRITE_STREAM(omsg,data.lottery[i]); 
		} 

		return 0; 
	}

	int ReadStream(NDIStreamMsg &inmsg,NetMessage::RoleCreateInfo &data)
	{
		READ_STREAM_STRING(inmsg, data.roleName, ROLE_NAME_LENGTH);

		READ_STREAM(inmsg,data.roleAttrsCount);
		if( data.roleAttrsCount > ROLE_ATTRS_ON_CREATE ) {
			return -1;
		}
 		for(int i =0; i<data.roleAttrsCount;++i ){
			READ_STREAM(inmsg,data.roleAttrs[i]);
		}
 
		return 0; 
	}
	int WriteStream(NDOStreamMsg &omsg,const NetMessage::RoleCreateInfo &data)
	{
		WRITE_STREAM(omsg,data.roleName);

		if(data.roleAttrsCount > ROLE_ATTRS_ON_CREATE) {
			return -1; 
		} 
 		WRITE_STREAM(omsg,data.roleAttrsCount);
		for(int i =0 ; i < data.roleAttrsCount;++i){
			WRITE_STREAM(omsg,data.roleAttrs[i]); 
		} 

		return 0; 
	}

	int ReadStream(NDIStreamMsg &inmsg,NetMessage::DragonToFragment &data)
	{
		READ_STREAM(inmsg, data.dragonId);

		READ_STREAM(inmsg,data.itemsCount);
		if( data.itemsCount > 10 ) {
			return -1;
		}
 		for(int i =0; i<data.itemsCount;++i ){
			READ_STREAM(inmsg,data.items[i]);
		}
 
		return 0; 
	}
	int WriteStream(NDOStreamMsg &omsg,const NetMessage::DragonToFragment &data)
	{
		WRITE_STREAM(omsg,data.dragonId);

		if(data.itemsCount > 10) {
			return -1; 
		} 
 		WRITE_STREAM(omsg,data.itemsCount);
		for(int i =0 ; i < data.itemsCount;++i){
			WRITE_STREAM(omsg,data.items[i]); 
		} 

		return 0; 
	}
};

