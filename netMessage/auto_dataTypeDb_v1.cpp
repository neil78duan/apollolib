/* please do not change this file ,
 * auto create by program 
 * create time 2016-8-18 10:42:51 
 */



#include "netMessage/dataStream.h"


#include "netMessage/auto_dataTypeDb_v1.h"
namespace NetMessage 
{

	int ReadStream(NDIStreamMsg &inmsg,NetMessage::EquipAttrNode_v1 &data)
	{
		READ_STREAM(inmsg, data.color);
		READ_STREAM(inmsg, data.attr_id);
		READ_STREAM(inmsg, data.attr_val);

		return 0; 
	}
	int WriteStream(NDOStreamMsg &omsg,const NetMessage::EquipAttrNode_v1 &data)
	{
		WRITE_STREAM(omsg,data.color);
		WRITE_STREAM(omsg,data.attr_id);
		WRITE_STREAM(omsg,data.attr_val);

		return 0; 
	}

	int ReadStream(NDIStreamMsg &inmsg,NetMessage::ItemInfo_v1 &data)
	{
		READ_STREAM(inmsg, data.instantID);
		READ_STREAM(inmsg, data.typeID);
		READ_STREAM(inmsg, data.slot);
		READ_STREAM(inmsg, data.number);
		READ_STREAM(inmsg, data.stat);

		return 0; 
	}
	int WriteStream(NDOStreamMsg &omsg,const NetMessage::ItemInfo_v1 &data)
	{
		WRITE_STREAM(omsg,data.instantID);
		WRITE_STREAM(omsg,data.typeID);
		WRITE_STREAM(omsg,data.slot);
		WRITE_STREAM(omsg,data.number);
		WRITE_STREAM(omsg,data.stat);

		return 0; 
	}

	int ReadStream(NDIStreamMsg &inmsg,NetMessage::EquipInfo_v1 &data)
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
	int WriteStream(NDOStreamMsg &omsg,const NetMessage::EquipInfo_v1 &data)
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

	int ReadStream(NDIStreamMsg &inmsg,NetMessage::SkillInfo_v1 &data)
	{
		READ_STREAM(inmsg, data.id);
		READ_STREAM(inmsg, data.level);

		return 0; 
	}
	int WriteStream(NDOStreamMsg &omsg,const NetMessage::SkillInfo_v1 &data)
	{
		WRITE_STREAM(omsg,data.id);
		WRITE_STREAM(omsg,data.level);

		return 0; 
	}

	int ReadStream(NDIStreamMsg &inmsg,NetMessage::MapScore_v1 &data)
	{
		READ_STREAM(inmsg, data.map_id);
		READ_STREAM(inmsg, data.enter_times);
		READ_STREAM(inmsg, data.achievements);
		READ_STREAM(inmsg, data.score);
		READ_STREAM(inmsg, data.time);

		return 0; 
	}
	int WriteStream(NDOStreamMsg &omsg,const NetMessage::MapScore_v1 &data)
	{
		WRITE_STREAM(omsg,data.map_id);
		WRITE_STREAM(omsg,data.enter_times);
		WRITE_STREAM(omsg,data.achievements);
		WRITE_STREAM(omsg,data.score);
		WRITE_STREAM(omsg,data.time);

		return 0; 
	}

	int ReadStream(NDIStreamMsg &inmsg,NetMessage::MapScoreBuf_v1 &data)
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
	int WriteStream(NDOStreamMsg &omsg,const NetMessage::MapScoreBuf_v1 &data)
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

	int ReadStream(NDIStreamMsg &inmsg,NetMessage::DragonInfo_v1 &data)
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
	int WriteStream(NDOStreamMsg &omsg,const NetMessage::DragonInfo_v1 &data)
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

	int ReadStream(NDIStreamMsg &inmsg,NetMessage::DragonsBuf_v1 &data)
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
	int WriteStream(NDOStreamMsg &omsg,const NetMessage::DragonsBuf_v1 &data)
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

	int ReadStream(NDIStreamMsg &inmsg,NetMessage::DropWeightNode_v1 &data)
	{
		READ_STREAM(inmsg, data.dropObjId);
		READ_STREAM(inmsg, data.weight);

		return 0; 
	}
	int WriteStream(NDOStreamMsg &omsg,const NetMessage::DropWeightNode_v1 &data)
	{
		WRITE_STREAM(omsg,data.dropObjId);
		WRITE_STREAM(omsg,data.weight);

		return 0; 
	}

	int ReadStream(NDIStreamMsg &inmsg,NetMessage::DropWeightBuf_v1 &data)
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
	int WriteStream(NDOStreamMsg &omsg,const NetMessage::DropWeightBuf_v1 &data)
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

	int ReadStream(NDIStreamMsg &inmsg,NetMessage::ItemInShop_v1 &data)
	{
		READ_STREAM(inmsg, data.itemPackId);
		READ_STREAM(inmsg, data.packIndex);
		READ_STREAM(inmsg, data.buyNumber);

		return 0; 
	}
	int WriteStream(NDOStreamMsg &omsg,const NetMessage::ItemInShop_v1 &data)
	{
		WRITE_STREAM(omsg,data.itemPackId);
		WRITE_STREAM(omsg,data.packIndex);
		WRITE_STREAM(omsg,data.buyNumber);

		return 0; 
	}

	int ReadStream(NDIStreamMsg &inmsg,NetMessage::ShopInfo_v1 &data)
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
	int WriteStream(NDOStreamMsg &omsg,const NetMessage::ShopInfo_v1 &data)
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

	int ReadStream(NDIStreamMsg &inmsg,NetMessage::RoleShopsInfo_v1 &data)
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
	int WriteStream(NDOStreamMsg &omsg,const NetMessage::RoleShopsInfo_v1 &data)
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
};

