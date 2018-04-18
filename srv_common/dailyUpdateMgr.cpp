//
//  dailyUpdateMgr.cpp
//  gameHall
//
//  Created by duanxiuyun on 15/12/14.
//  Copyright 2015 duanxiuyun. All rights reserved.
//

#include "dailyUpdateMgr.hpp"
#include "common_inc.h"
#include "logic_parser/dbl_mgr.h"
#include "srv_common/userAddtionData.h"
#include <algorithm>


AlarmBase::AlarmBase(NDAlarm *pObject, int timezone ) :m_owner(pObject), m_timeZone(timezone), m_bChanged(false)
{
	if (m_timeZone == -1)	{
		m_timeZone = nd_time_zone();
	}
}

AlarmBase::~AlarmBase()
{
}

int AlarmBase::Init(const DBLDataNode *data, int version)
{
	ND_TRACE_FUNC();
	UserAdditionData saveData;

	if (!saveData.Init(data)) {
		nd_logerror("init daily timer from db error \n");
		return -1;
	}

	const user_addition_map& datas = saveData.getDataMap();
	for (user_addition_map::const_iterator it = datas.begin(); it != datas.end(); it++){
		NDUINT64 val = it->second.value.GetInt64();
		NDUINT32 id = (val >> 48);
		time_t t = val & 0xffffffffffff;
		const char *name = getNameFromCfg(id);
		if (name)	{			
			Add(id, name, t);
		}
	}
	return 0;
}

int AlarmBase::toStream(DBLDataNode *outData)
{
	ND_TRACE_FUNC();
	UserAdditionData saveData;

	for (dailyRunInfo_vct::iterator it = m_lastRunInfo.begin(); it != m_lastRunInfo.end(); it++)	{
		NDUINT64 timerId = (it->id);
		NDUINT64 val = timerId << 48 | it->lastRunTm & 0xffffffffffff;
		char buf[128];
		snprintf(buf, sizeof(buf), "%s_%d", it->name.c_str(), it->id);

		saveData.setData(buf, DBLDataNode(val));
	}
	saveData.convert2node(*outData);
	return 0;
}

void AlarmBase::Update()
{

}


const char *AlarmBase::getNameFromCfg(int id)
{
	return NULL;
}

bool AlarmBase::setEvent(int id, time_t lastRun)
{
	const char *name = getNameFromCfg(id); 
	if (name)	{
		return Add(id, name, lastRun);
	}
	return false;
}

bool AlarmBase::Add(int id, const char *name, time_t t)
{
	ND_TRACE_FUNC();
	time_t now = app_inst_time(NULL);
	if (t > now){
		nd_logwarn("alarm %s last run time =%lld now =%lld \n", name, t, now);
	}
	for (dailyRunInfo_vct::iterator it = m_lastRunInfo.begin(); it != m_lastRunInfo.end(); it++)	{
		if (id == it->id)	{
			it->lastRunTm = t;
			return true;
		}
	}

	dailyEventInfo info;
	info.id = id;
	info.name = name;
	info.lastRunTm = t;
	m_lastRunInfo.push_back(info);
	m_bChanged = true;
	return true;
}

dailyEventInfo *AlarmBase::getEventInfo(int id)
{
	ND_TRACE_FUNC();
	for (dailyRunInfo_vct::iterator it = m_lastRunInfo.begin(); it != m_lastRunInfo.end(); it++)	{
		if (id == it->id)	{
			return &(*it);
		}
	}
	return NULL;
}

time_t AlarmBase::getLastTm(int id)
{
	ND_TRACE_FUNC();
	dailyEventInfo *pInfo = getEventInfo(id);
	if (!pInfo) {
		return 0;
	}
	return pInfo->lastRunTm;
}


time_t AlarmBase::getLastRunTime(const char *alarmName)
{
	ND_TRACE_FUNC();
	time_t ret = 0;
	for (dailyRunInfo_vct::iterator it = m_lastRunInfo.begin(); it != m_lastRunInfo.end(); it++)	{
		if (0==ndstricmp(it->name.c_str(), alarmName) )	{
			if (it->lastRunTm > ret ){
				ret = it->lastRunTm;
			}
		}
	}
	return ret;
}


//////////////////////////////////////////////////////////////////////////
////////////////

dailyUpCfg_vct DailyUpdateMgr::s_global_updateCfgInfo ;

int DailyUpdateMgr::LoadDailyUpConfig(const char *tablename)
{
	ND_TRACE_FUNC();
	Destroy();
	
	const char *pfields[] = { "ID", "name", "update_time", "param" };

	DBL_BEGIN_CURSOR(tablename, pfields) 	{
		if (cursor[0].CheckValid() && cursor[1].CheckValid()) {
			dailyUpdateCfg node;

			if (!cursor[1].CheckValid() ||!cursor[2].CheckValid())	{
				continue; 
			}
			if (cursor[3].CheckValid())	{
				node.param = cursor[3];
			}
			if (cursor[2].GetDataType() != OT_ARRAY){
				int offsetSeconds = nd_time_clock_to_seconds(cursor[2].GetText());
				if (-1==offsetSeconds)	{
					nd_logfatal("load alarm time error line =%d update_time=%s\n", cursor[0].GetInt(), cursor[2].GetText());
				}
				else {
					node.timeOffsets.push_back(offsetSeconds);
				}
			}
			else {
				for (int x = 0; x < cursor[2].GetArraySize(); ++x) {
					int offsetSeconds = nd_time_clock_to_seconds(cursor[2].GetarrayText(x));

					if (-1 == offsetSeconds)	{
						nd_logfatal("load alarm time error line =%d update_time=%s\n", cursor[0].GetInt(), cursor[2].GetText());
					}
					else {
						node.timeOffsets.push_back(offsetSeconds);
					}

				}
			}
			
			node.id = cursor[0].GetInt();
			node.name = cursor[1].GetText();
			
			std::sort(node.timeOffsets.begin(), node.timeOffsets.end());
			
			DailyUpdateMgr::s_global_updateCfgInfo.push_back(node);
		}
	}
	return 0;
}

void DailyUpdateMgr::Destroy()
{
	ND_TRACE_FUNC();
	DailyUpdateMgr::s_global_updateCfgInfo.clear();
}

const dailyUpdateCfg *DailyUpdateMgr::GetDailyConfgInfoId(int id)
{
	ND_TRACE_FUNC();
	if (0 == DailyUpdateMgr::s_global_updateCfgInfo.size()) {
		return NULL;
	}

	for (dailyUpCfg_vct::const_iterator it = s_global_updateCfgInfo.begin(); it != s_global_updateCfgInfo.end(); it++) {
		if (it->id == id){
			return &(*it);
		}
	}
	return NULL;
}

int DailyUpdateMgr::GetOffsetSeconds(int id)
{
	ND_TRACE_FUNC();
	const dailyUpdateCfg *pcfg = GetDailyConfgInfoId(id);
	if (pcfg && pcfg->timeOffsets.size()){
		return pcfg->timeOffsets[0];
	}
	return 0;
}

int DailyUpdateMgr::GetOffsetSeconds(const char *name)
{
	ND_TRACE_FUNC();
	if (0 == DailyUpdateMgr::s_global_updateCfgInfo.size()) {
		return NULL;
	}

	for (dailyUpCfg_vct::const_iterator it = s_global_updateCfgInfo.begin(); it != s_global_updateCfgInfo.end(); it++) {
		if (0==ndstricmp(name, it->name.c_str()) ){
			if (it->timeOffsets.size() > 0)	{
				return it->timeOffsets[0];
			}
		}
	}
	return 0;
}

DailyUpdateMgr::DailyUpdateMgr(NDAlarm *pObject, int timezone) :AlarmBase(pObject,timezone)
{

}

DailyUpdateMgr::~DailyUpdateMgr()
{

}

const char *DailyUpdateMgr::getNameFromCfg(int id)
{
	ND_TRACE_FUNC();
	const dailyUpdateCfg *pcfg =GetDailyConfgInfoId( id);
	if (pcfg)	{
		return pcfg->name.c_str();
	}
	return 0;
}

void DailyUpdateMgr::Update()
{
	ND_TRACE_FUNC();
	dailyUpCfg_vct::const_iterator it = s_global_updateCfgInfo.begin();

	for (; it != s_global_updateCfgInfo.end(); it++) {

		dailyEventInfo *pEventInfo = getEventInfo(it->id);
		if (!pEventInfo){
			time_t now = app_inst_time(NULL);
			dailyEventInfo eventInfo;
			eventInfo.lastRunTm = now - 24 * 3600;

			if (_UpdateNode(&(*it),&eventInfo, m_timeZone)){
				Add(it->id, it->name.c_str(), now);
				m_bChanged = true;
			}
		}
		else {
			_UpdateNode(&(*it),pEventInfo, m_timeZone);
		}

	}
}

bool DailyUpdateMgr::_UpdateNode(const dailyUpdateCfg *pCfg , dailyEventInfo *eventInfo, int timezone)
{

	ND_TRACE_FUNC();
	time_t now = app_inst_time(NULL);
	std::vector<time_t> clock_seconds;

	time_t lastRenewTm = eventInfo->lastRunTm;

	if (lastRenewTm > now){
		eventInfo->lastRunTm = now;
		m_bChanged = true;
		return false;
	}

	int renewTimes = 0;

	int intervalDays = nd_time_day_interval_ex(now, eventInfo->lastRunTm, timezone);
	if (intervalDays > 0){
		renewTimes += intervalDays * pCfg->timeOffsets.size();
		lastRenewTm += intervalDays * 24 * 3600;
	}

	for (size_t i = 0; i < pCfg->timeOffsets.size(); i++){
		time_t t = nd_time_getgm_from_offset(pCfg->timeOffsets[i], now, timezone);
		clock_seconds.push_back(t);
	}


	time_t val1 = clock_seconds[clock_seconds.size() - 1] - 3600 * 24;

	clock_seconds.insert(clock_seconds.begin(), val1);
	int lastIndex = -1;
	int nowtimeIndex = -1;
	for (int i = 0; i < clock_seconds.size(); ++i) {
		if (now >= clock_seconds[i]) {
			nowtimeIndex++;
		}

		if (lastRenewTm >= clock_seconds[i]) {
			lastIndex++;
		}
	}
	renewTimes += nowtimeIndex - lastIndex;

	if (renewTimes > 0) {
		m_bChanged = true;
		m_owner->UpdateDaily(pCfg->name.c_str(), (void*)&pCfg->param, renewTimes);
		eventInfo->lastRunTm = now;

		nd_logdebug("daily alarm %d %s \n", pCfg->id, pCfg->name.c_str());
		return true;
	}
	return false;
}






//////////////////////////////////////////////////////////////////////////
//weekly alarm

weeklyUpCfg_vct WeekAlarmMgr::s_global_weeklyCfgInfo;

WeekAlarmMgr::WeekAlarmMgr(NDAlarm *pObject, int timezone) :AlarmBase(pObject,timezone)
{
}

WeekAlarmMgr::~WeekAlarmMgr()
{
}


int WeekAlarmMgr::LoadWeeklyConfig(const char *tablename)
{
	ND_TRACE_FUNC();
	Destroy();

	const char *pfields[] = { "ID", "name", "update_time", "param", "update_week"};

	DBL_BEGIN_CURSOR(tablename, pfields) 	{
		if (cursor[0].CheckValid() && cursor[1].CheckValid()) {
			WeeklyUpdateCfg node;

			if (!cursor[1].CheckValid() || !cursor[2].CheckValid())	{
				continue;
			}
			if (cursor[3].CheckValid())	{
				node.param = cursor[3];
			}

			node.weekDay = cursor[4].GetInt();

			node.timeOffset = nd_time_clock_to_seconds(cursor[2].GetarrayText(0));

			if (-1 == node.timeOffset)	{
				nd_logfatal("load alarm time error line =%d update_time=%s\n", cursor[0].GetInt(), cursor[2].GetText());
			}
			else {
				node.id = cursor[0].GetInt();
				node.name = cursor[1].GetText();
				s_global_weeklyCfgInfo.push_back(node);
			}


		}
	}
	return 0;
}
void WeekAlarmMgr::Destroy()
{
	ND_TRACE_FUNC();
	s_global_weeklyCfgInfo.clear();
}


const WeeklyUpdateCfg *WeekAlarmMgr::GetWeeklyConfgInfo(int id)
{
	ND_TRACE_FUNC();
	if (0 == s_global_weeklyCfgInfo.size()) {
		return NULL;
	}

	for (weeklyUpCfg_vct::const_iterator it = s_global_weeklyCfgInfo.begin(); it != s_global_weeklyCfgInfo.end(); it++) {
		if (it->id == id){
			return &(*it);
		}
	}
	return NULL;
}

int WeekAlarmMgr::GetOffsetSeconds(int id)
{
	ND_TRACE_FUNC();
	const WeeklyUpdateCfg *pcfg = GetWeeklyConfgInfo(id);
	if (pcfg ){
		return pcfg->timeOffset;
	}
	return 0;
}


bool WeekAlarmMgr::_UpdateNode(const WeeklyUpdateCfg *pCfg, dailyEventInfo *eventInfo, int timezone)
{
	ND_TRACE_FUNC();
	time_t now = app_inst_time(NULL);	
	time_t lastRenewTm = eventInfo->lastRunTm;

	if (lastRenewTm > now){
		eventInfo->lastRunTm = now;
		m_bChanged = true;
		return false;
	}

	int nowWeekIndex = nd_time_week_index(now, timezone);
	int lastWeekIndex = nd_time_week_index(lastRenewTm, timezone);

	if (nowWeekIndex == lastWeekIndex)	{
		return false;
	}

	time_t eventTm = nd_time_from_week(pCfg->weekDay, pCfg->timeOffset, now, timezone);
	if (eventTm > now  )	{
		return false;
	}

// 
// 	struct  tm ltm = { 0 };
// 	time_t localTm = now - 3600 * timezone;	
// 	gmtime_r(&localTm, &ltm);
// 
// 	if (pCfg->weekDay > ltm.tm_wday){
// 		return false;
// 	}
// 	else if(pCfg->weekDay == ltm.tm_wday){
// 		int secondIndex = localTm % (24 * 3600);
// 		if (secondIndex < pCfg->timeOffset)	{
// 			return false;
// 		}
// 	}
	//on alarm 
	m_owner->UpdateWeek(pCfg->name.c_str(),(void*) &pCfg->param);
	eventInfo->lastRunTm = now;
	m_bChanged = true;
	nd_logdebug("weekly alarm %d %s \n", pCfg->id, pCfg->name.c_str());
	return true;
}


const char *WeekAlarmMgr::getNameFromCfg(int id)
{
	ND_TRACE_FUNC();
	const WeeklyUpdateCfg *pcfg = WeekAlarmMgr::GetWeeklyConfgInfo(id);

	if (pcfg)	{
		return pcfg->name.c_str();
	}
	return 0;
}

void WeekAlarmMgr::Update()
{
	ND_TRACE_FUNC();
	time_t now = app_inst_time(NULL);
	weeklyUpCfg_vct::const_iterator it = s_global_weeklyCfgInfo.begin();

	for (; it != s_global_weeklyCfgInfo.end(); it++) {

		dailyEventInfo *pEventInfo = getEventInfo(it->id);
		if (!pEventInfo){
			
			dailyEventInfo eventInfo;
			eventInfo.lastRunTm = now - 7 * 24 * 3600;
			if (_UpdateNode(&(*it), &eventInfo, m_timeZone)){
				Add(it->id, it->name.c_str(), now);
				m_bChanged = true;
			}
		}
		else {
			_UpdateNode(&(*it),pEventInfo, m_timeZone);
		}
	}
}
