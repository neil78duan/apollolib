//
//  gameMessage.cpp
//  clientDemo
//
//  Created by duanxiuyun on 15-2-4.
//  Copyright (c) 2015年 duanxiuyun. All rights reserved.
//

//#include "commonTest.h"
#include "apollo_errors.h"
#include "gameMessage.h"
#include "netui_atl.h"

#include "nd_msg.h"
#include "msg_def.h"
#include "srv_define.h"
#include "message_inc.h"

#include <time.h>

#ifdef _GUI_TOOL_

#ifdef WIN32
#define myfprintf(pf, msg,...) _logmsg(__FUNC__, __FILE__, __LINE__, ND_MSG ,msg, __VA_ARGS__ )
#else
#define myfprintf(pf, msg,arg...) _logmsg(__FUNC__, __FILE__, __LINE__, ND_MSG ,msg, ##arg )

//#define  nd_log_screen(fmt,arg...)  _logmsg_screen(__FILE__, __LINE__, fmt,##arg)
#endif

#else 

//#define myfprintf(stdout,pf, msg,...) fprintf(pf,msg,__VA_ARGS__)
#define myfprintf fprintf

#endif


int msg_init_package_handler(NDIConn* pconn, nd_usermsgbuf_t *msg)
{
	NDIStreamMsg inmsg(msg);
	NetMessage::RolePackageData data;

	myfprintf(stdout,"receivd package init:\n");
	if (0 == NetMessage::ReadStream(inmsg, data)){

		myfprintf(stdout,"equip-package-init number =%d\n", data.equipsCount);
		for (int i = 0; i < data.equipsCount; i++)	{
			myfprintf(stdout,"equip-package-init:%d,inst=%d attr_num=%d \n", data.equips[i].typeID, data.equips[i].instantID, data.equips[i].attrsCount);
		}

		myfprintf(stdout,"item-package-init number =%d\n", data.itemsCount);
		for (int i = 0; i < data.itemsCount; i++)	{
			myfprintf(stdout,"item-init [ %d : %d ] \n", data.items[i].typeID, data.items[i].number);
		}
	}
	else {
		myfprintf(stdout,"received data error \n");
	}

	return 0;
}

int msg_init_attr_handler(NDIConn* pconn, nd_usermsgbuf_t *msg)
{
	NDIStreamMsg inmsg(msg);
	NetMessage::AttrsBuf data;

	myfprintf(stdout,"receivd player attribute init:\n");
	if (0 == NetMessage::ReadStream(inmsg, data)){

		myfprintf(stdout,"attribute-init number =%d\n", data.datasCount);
		for (int i = 0; i < data.datasCount; i++)	{
			myfprintf(stdout,"attr-init [ %d = %f ] \n", data.datas[i].id, data.datas[i].val);
		}
	}
	else {
		myfprintf(stdout,"received data error \n");
	}

	return 0;
}

int msg_init_equips_handler(NDIConn* pconn, nd_usermsgbuf_t *msg)
{
	NDIStreamMsg inmsg(msg);
	NetMessage::RoleEquipsData data;

	myfprintf(stdout,"receivd equips on role :\n");
	if (0 == NetMessage::ReadStream(inmsg, data)){

		myfprintf(stdout,"equips-init number =%d\n", data.equipsCount);

		for (int i = 0; i < data.equipsCount; i++)	{
			myfprintf(stdout,"equip [ slot %d id %d ] \n", i, data.equips[i]);
		}
	}
	else {
		myfprintf(stdout,"received data error \n");
	}

	return 0;
}


int msg_recv_attr_change_handler(NDIConn* pconn, nd_usermsgbuf_t *msg)
{
	NDIStreamMsg inmsg(msg);
	NetMessage::AttrsBuf data;

	myfprintf(stdout,"receivd role attribute changed:\n");
	if (0 == NetMessage::ReadStream(inmsg, data)){
		for (int i = 0; i < data.datasCount; i++)	{
			myfprintf(stdout,"changed attr [ %d = %f ] \n", data.datas[i].id, data.datas[i].val);
		}
	}
	else {
		myfprintf(stdout,"received data error \n");
	}

	return 0;
}

int msg_enter_scene_handler(NDIConn* pconn, nd_usermsgbuf_t *msg)
{
	NDIStreamMsg inmsg(msg);
	NetMessage::MsgCommonReply data;

	myfprintf(stdout,"receivd enter map:\n");
	if (0 == NetMessage::ReadStream(inmsg, data)){
		myfprintf(stdout," player enter scene %s %d\n", data.result == 0 ? "success" : "failed", data.value);
	}
	else {
		myfprintf(stdout,"received data error \n");
	}

	return 0;
}


int msg_init_skill_handler(NDIConn* pconn, nd_usermsgbuf_t *msg)
{
	NDIStreamMsg inmsg(msg);
	NetMessage::RoleSkillData data;

	myfprintf(stdout,"receivd skill data init:\n");
	if (0 == NetMessage::ReadStream(inmsg, data)){

		myfprintf(stdout,"skill-init number =%d\n", data.datasCount);

		for (int i = 0; i < data.datasCount; i++)	{
			myfprintf(stdout,"skill [ %d level %d ] \n", data.datas[i].id, data.datas[i].level);
		}
	}
	else {
		myfprintf(stdout,"received data error \n");
	}

	return 0;
}

int msg_skill_add_handler(NDIConn* pconn, nd_usermsgbuf_t *msg)
{
	NDIStreamMsg inmsg(msg);
	NetMessage::SkillInfo data;

	myfprintf(stdout,"receivd get skill:\n");
	if (0 == NetMessage::ReadStream(inmsg, data)){
		myfprintf(stdout,"get skill : %d \n", data.id);
	}
	else {
		myfprintf(stdout,"received data error \n");
	}

	return 0;
}

int msg_skill_change_handler(NDIConn* pconn, nd_usermsgbuf_t *msg)
{
	NDIStreamMsg inmsg(msg);
	NetMessage::SkillInfo data;

	myfprintf(stdout,"receivd skill changed :\n");
	if (0 == NetMessage::ReadStream(inmsg, data)){
		myfprintf(stdout,"changed skill : %d level=%d\n", data.id, data.level);
	}
	else {
		myfprintf(stdout,"received data error \n");
	}

	return 0;
}


int msg_skill_del_handler(NDIConn* pconn, nd_usermsgbuf_t *msg)
{
	NDIStreamMsg inmsg(msg);
	NDUINT32 skillid;

	myfprintf(stdout,"receivd skill deleted :\n");

	if (0 == inmsg.Read(skillid)){
		myfprintf(stdout,"del skill : %d \n", skillid);
	}
	else {
		myfprintf(stdout,"received data error \n");
	}

	return 0;
}

int msg_package_item_add_handler(NDIConn* pconn, nd_usermsgbuf_t *msg)
{
	NDIStreamMsg inmsg(msg);
	NetMessage::ItemInfo data;

	myfprintf(stdout,"receivd package add item:\n");
	if (0 == NetMessage::ReadStream(inmsg, data)){
		myfprintf(stdout,"add item [ %d, %d ] \n", data.typeID, data.number);
	}
	else {
		myfprintf(stdout,"received data error \n");
	}

	return 0;
}

int msg_recv_item_change_handler(NDIConn* pconn, nd_usermsgbuf_t *msg)
{
	NDIStreamMsg inmsg(msg);
	NetMessage::ItemInfo data;

	myfprintf(stdout,"receivd package data chagned ntf:\n");
	if (0 == NetMessage::ReadStream(inmsg, data)){
		myfprintf(stdout,"item-change [ %d, %d ] \n", data.typeID, data.number);
	}
	else {
		myfprintf(stdout,"received data error \n");
	}

	return 0;
}

int msg_item_del_handler(NDIConn* pconn, nd_usermsgbuf_t *msg)
{
	NDIStreamMsg inmsg(msg);
	NDUINT64 id;

	myfprintf(stdout,"receivd package item deleted:\n");

	if (0 == inmsg.Read(id)){
		myfprintf(stdout,"del item : %lld \n", id);
	}
	else {
		myfprintf(stdout,"received data error \n");
	}

	return 0;
}

int msg_package_equip_add_handler(NDIConn* pconn, nd_usermsgbuf_t *msg)
{
	NDIStreamMsg inmsg(msg);
	NetMessage::EquipInfo data;

	myfprintf(stdout,"receivd package add items:\n");
	if (0 == NetMessage::ReadStream(inmsg, data)){
		myfprintf(stdout,"package equip add [ %d ] \n", data.typeID);
	}
	else {
		myfprintf(stdout,"received data error \n");
	}

	return 0;
}


int msg_recv_equip_change_handler(NDIConn* pconn, nd_usermsgbuf_t *msg)
{
	NDIStreamMsg inmsg(msg);
	NetMessage::EquipInfo data;

	myfprintf(stdout,"receivd package item changed :\n");
	if (0 == NetMessage::ReadStream(inmsg, data)){
		myfprintf(stdout,"changed equip-inpackage [ %d] \n", data.typeID);
	}
	else {
		myfprintf(stdout,"received data error \n");
	}

	return 0;
}

int msg_item_package_equip_handler(NDIConn* pconn, nd_usermsgbuf_t *msg)
{
	NDIStreamMsg inmsg(msg);
	NDUINT64 id;

	myfprintf(stdout,"receivd equips-del-in-package:\n");

	if (0 == inmsg.Read(id)){
		myfprintf(stdout,"del equip-in-package : %lld \n", id);
	}
	else {
		myfprintf(stdout,"received data error \n");
	}

	return 0;
}

int msg_equip_putton_handler(NDIConn* pconn, nd_usermsgbuf_t *msg)
{
	NDIStreamMsg inmsg(msg);
	NetMessage::EquipInfo data;

	myfprintf(stdout,"receivd equip-on:\n");
	if (0 == NetMessage::ReadStream(inmsg, data)){
		myfprintf(stdout,"equip putton [ %d, slot =%d ] \n", data.typeID, data.slot);
	}
	else {
		myfprintf(stdout,"received data error \n");
	}

	return 0;
}

int msg_equip_puttoff_handler(NDIConn* pconn, nd_usermsgbuf_t *msg)
{
	NDIStreamMsg inmsg(msg);
	NetMessage::EquipInfo data;

	myfprintf(stdout,"receivd equip-off:\n");
	if (0 == NetMessage::ReadStream(inmsg, data)){
		myfprintf(stdout,"equip [ %d, slot =%d ] \n", data.typeID, data.slot);
	}
	else {
		myfprintf(stdout,"received data error \n");
	}

	return 0;
}


int msg_get_client_setting(NDIConn* pconn, nd_usermsgbuf_t *msg)
{
	NDIStreamMsg inmsg(msg);
	myfprintf(stdout,"Receive client setting data len = %d \n", inmsg.GetDataLen() - 2);
	return 0;
}

int msg_get_map_score(NDIConn* pconn, nd_usermsgbuf_t *msg)
{
	NDIStreamMsg inmsg(msg);
	NetMessage::MapScoreBuf data;

	myfprintf(stdout,"map-scores:\n");
	if (0 == NetMessage::ReadStream(inmsg, data)){
		for (size_t i = 0; i < data.dataCount; i++)	{
			myfprintf(stdout,"map:%d star:%d score:%d enter_times:%d \n", data.data[i].map_id, data.data[i].achievements, data.data[i].score, data.data[i].enter_times);
		}
	}

	return 0;
}


int msg_recv_dragon_data_handler(NDIConn* pconn, nd_usermsgbuf_t *msg)
{
	NDIStreamMsg inmsg(msg);
	NetMessage::DragonsBuf data;

	myfprintf(stdout,"receivd :\n");
	if (0 == NetMessage::ReadStream(inmsg, data)){
		for (int i = 0; i < data.dragonsCount; i++)	{
			myfprintf(stdout,"init-get dragon [ %d ] \n", data.dragons[i].id);
		}
	}
	else {
		myfprintf(stdout,"received data error \n");
	}

	return 0;
}

int msg_recv_weapon_data_handler(NDIConn* pconn, nd_usermsgbuf_t *msg)
{
	NDIStreamMsg inmsg(msg);
	NetMessage::WeaponsBuf data;

	myfprintf(stdout,"receivd :\n");
	if (0 == NetMessage::ReadStream(inmsg, data)){
		for (int i = 0; i < data.dataCount; i++)	{
			myfprintf(stdout,"init-get weapon [ %d ] \n", data.data[i].id);
		}
	}
	else {
		myfprintf(stdout,"received weapon data error \n");
	}

	return 0;
}

int msg_recv_drop_message_handler(NDIConn* pconn, nd_usermsgbuf_t *msg)
{
	NDIStreamMsg inmsg(msg);
	NetMessage::DropPackage data;

	myfprintf(stdout,"Drop-items-info:\n");
	if (0 == NetMessage::ReadStream(inmsg, data)){
		for (int i = 0; i < data.itemsCount; i++)	{
			myfprintf(stdout,"drop item-id=%d num=%d \n", data.items[i].itemId, data.items[i].num);
		}

		for (int i = 0; i < data.attrsCount; i++)	{
			myfprintf(stdout,"get attribute-id=%d value=%f \n", data.attrs[i].id, data.attrs[i].val);
		}
	}
	else {
		myfprintf(stdout,"received data error \n");
	}

	return 0;
}

void initApolloGameMessage(NDIConn* pconn) 
{

	CONNECT_INSTALL_MSG_INT16(pconn, msg_equip_puttoff_handler, NETMSG_ITEM_EQUIPSLOT_OFF_NTF);
	CONNECT_INSTALL_MSG_INT16(pconn, msg_equip_putton_handler, NETMSG_ITEM_EQUIPSLOT_ON_NTF);
	CONNECT_INSTALL_MSG_INT16(pconn, msg_item_package_equip_handler, NETMSG_ITEM_PACKAGE_EQUIP_DEL_NTF);
	CONNECT_INSTALL_MSG_INT16(pconn, msg_recv_equip_change_handler, NETMSG_ITEM_PACKAGE_EQUIP_CHANGE_NTF);
	CONNECT_INSTALL_MSG_INT16(pconn, msg_package_equip_add_handler, NETMSG_ITEM_PACKAGE_EQUIP_ADD_NTF);
	CONNECT_INSTALL_MSG_INT16(pconn, msg_item_del_handler, NETMSG_ITEM_PACKAGE_ITEM_DEL_NTF);
	CONNECT_INSTALL_MSG_INT16(pconn, msg_recv_item_change_handler, NETMSG_ITEM_PACKAGE_ITEM_CHANGE_NTF);
	CONNECT_INSTALL_MSG_INT16(pconn, msg_package_item_add_handler, NETMSG_ITEM_PACKAGE_ITEM_ADD_NTF);
	CONNECT_INSTALL_MSG_INT16(pconn, msg_skill_del_handler, NETMSG_ROLE_SKILL_DEL_NTF);
	CONNECT_INSTALL_MSG_INT16(pconn, msg_skill_change_handler, NETMSG_ROLE_SKILL_CHANGE_NTF);
	CONNECT_INSTALL_MSG_INT16(pconn, msg_skill_add_handler, NETMSG_ROLE_SKILL_ADD_NTF);
	CONNECT_INSTALL_MSG_INT16(pconn, msg_init_skill_handler, NETMSG_ROLE_SKILL_INIT_ACK);
	CONNECT_INSTALL_MSG_INT16(pconn, msg_enter_scene_handler, NETMSG_ROLE_ENTER_SCENE_ACK);
	CONNECT_INSTALL_MSG_INT16(pconn, msg_recv_attr_change_handler, NETMSG_ROLE_ATTR_CHANGE_NTF);
	CONNECT_INSTALL_MSG_INT16(pconn, msg_init_equips_handler, NETMSG_ROLE_EQUIP_INIT_ACK);
	CONNECT_INSTALL_MSG_INT16(pconn, msg_init_attr_handler, NETMSG_ROLE_ATTR_INIT_ACK);
	CONNECT_INSTALL_MSG_INT16(pconn, msg_init_package_handler, NETMSG_ROLE_PACKAGE_INIT_ACK);

	CONNECT_INSTALL_MSG_INT16(pconn, msg_get_client_setting, NETMSG_ROLE_CLIENT_SETTING_ACK);
	CONNECT_INSTALL_MSG_INT16(pconn, msg_get_map_score, NETMSG_ROLE_MAPS_SCORE_ACK);
	CONNECT_INSTALL_MSG_INT16(pconn, msg_recv_dragon_data_handler, NETMSG_DRAGON_INIT_ACK);
	CONNECT_INSTALL_MSG_INT16(pconn, msg_recv_weapon_data_handler, NETMSG_WEAPON_INIT_WEAPON_DATA_ACK);
	CONNECT_INSTALL_MSG_INT16(pconn, msg_recv_drop_message_handler, NETMSG_ROLE_DROP_PACKAGE_NTF);

}
