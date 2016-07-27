﻿/* file _event_def.h
 *
 * define game event 
 *
 * create by duan 
 *
 * 2015-5-29
 */

_APOLLO_DEFINE_EVENT(APOLLO_EVENT_NONE, "空事件")
_APOLLO_DEFINE_EVENT(APOLLO_EVENT_SERVER_START, "服务器启动成功()")
_APOLLO_DEFINE_EVENT(APOLLO_EVENT_SERVER_STOP, "服务器关闭()")
_APOLLO_DEFINE_EVENT(APOLLO_EVENT_LOGIN, "角色登录(player)")
_APOLLO_DEFINE_EVENT(APOLLO_EVENT_LOGOUT, "角色退出(player)")
_APOLLO_DEFINE_EVENT(APOLLO_EVENT_CREATE_ROLE, "角色创建(player)")
_APOLLO_DEFINE_EVENT(APOLLO_EVENT_DEL_ROLE, "角色删除(player)")
_APOLLO_DEFINE_EVENT(APOLLO_EVENT_ENTER_MAP, "角色进入地图(player, mapid)")
_APOLLO_DEFINE_EVENT(APOLLO_EVENT_ATTR_CHANGED, "属性变化(player,a_id, a_val)")
_APOLLO_DEFINE_EVENT(APOLLO_EVENT_UP_LEVEL, "升级(player, newlevel)")
_APOLLO_DEFINE_EVENT(APOLLO_EVENT_GET_ITEM, "获得物品(player, itemid)")
_APOLLO_DEFINE_EVENT(APOLLO_EVENT_KILL_MONSTER, "杀死怪物(player,monster_id)")
_APOLLO_DEFINE_EVENT(APOLLO_EVENT_GET_SKILL, "获得技能(player,skill_id)")
_APOLLO_DEFINE_EVENT(APOLLO_EVENT_LEAVE_MAP, "角色离开地图(player, mapid)")
_APOLLO_DEFINE_EVENT(APOLLO_EVENT_KILLED, "角色死亡(player)")
_APOLLO_DEFINE_EVENT(APOLLO_EVENT_GET_TASK, "接受任务(player,taskid)")
_APOLLO_DEFINE_EVENT(APOLLO_EVENT_TASK_OK, "完成任务(player,taskid)")
_APOLLO_DEFINE_EVENT(APOLLO_EVENT_DAILY_UPDATE, "每日更新(player)")
_APOLLO_DEFINE_EVENT(APOLLO_EVENT_ROLE_INPUT_INVITE, "成功输入邀请码(player)")