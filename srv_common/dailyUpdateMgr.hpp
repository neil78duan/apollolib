//
//  dailyUpdateMgr.hpp
//  gameHall
//
//  Created by duanxiuyun on 15/12/14.
//  Copyright2015 duanxiuyun. All rights reserved.
//

#ifndef dailyUpdateMgr_hpp
#define dailyUpdateMgr_hpp

#include "ndapplib/applib.h"
#include <map>
#include <vector>

#include <string>
#include "logic_parser/logicDataType.h"


// udpate config , read from excel table
struct dailyUpdateCfg
{
	int id;
	//int timeoffset;
	std::vector<int> timeOffsets;
	std::string name;
	DBLDataNode param;
	dailyUpdateCfg() :id(0)
	{

	}
	dailyUpdateCfg &operator= (const dailyUpdateCfg &r)
	{
		id = r.id;
		timeOffsets = r.timeOffsets;
		name = r.name;
		param = r.param;
		return *this;
	}
};

typedef std::vector<dailyUpdateCfg> dailyUpCfg_vct;
struct dailyEventInfo
{
	dailyEventInfo() :id(0), lastRunTm(0)
	{}
	int id;
	std::string name;
	time_t lastRunTm;
};
typedef std::vector<dailyEventInfo> dailyRunInfo_vct;


class AlarmBase
{
public:
	AlarmBase(NDAlarm *pObject, int timezone = -1);
	virtual~AlarmBase();

	int Init(const DBLDataNode *data, int version);
	int toStream(DBLDataNode *outData);

	//void UpdateMinute(int timezone = 0xff) ;
	virtual void Update();
	virtual const char *getNameFromCfg(int id);

	int Count() { return (int)m_lastRunInfo.size(); }
	dailyEventInfo *getEventInfo(int id);
	bool setEvent(int id, time_t lastRun);
	time_t getLastTm(int id);

	bool checkDataChange() { return m_bChanged; }
	void ClearDataChange() { m_bChanged = false; }
	void setTimeZone(int timezone) { m_timeZone = timezone; }

	time_t getLastRunTime(const char *alarmName);
	
protected:
	bool Add(int id, const char *name, time_t lastRunTM = 0);	

	bool m_bChanged;
	int m_timeZone;
	NDAlarm *m_owner;

	dailyRunInfo_vct m_lastRunInfo;


};

class DailyUpdateMgr :public AlarmBase
{	
public:
	//typedef std::map<std::string, DailyUpdateObj> dailyUpdate_vct ;
	
	DailyUpdateMgr(NDAlarm *pObject, int timezone=-1);
	virtual ~DailyUpdateMgr() ;
	
	void Update();
	virtual const char *getNameFromCfg(int id);
	
	static int LoadDailyUpConfig(const char *tablename);
	static void Destroy();
	//static const dailyUpdateCfg *GetDailyConfgInfo(const char *name);
	static const dailyUpdateCfg *GetDailyConfgInfoId(int id);
	static int GetOffsetSeconds(int id) ;
	static int GetOffsetSeconds(const char *name);
	
protected:


	bool _UpdateNode(const dailyUpdateCfg *pCfg, dailyEventInfo *eventInfo, int timezone);

	static dailyUpCfg_vct s_global_updateCfgInfo;

};

//////////////////////////////////////////////////////////////////////////

struct WeeklyUpdateCfg
{
	int id;
	int dayIndex;
	int timeOffset;
	std::string name;
	DBLDataNode param;
	WeeklyUpdateCfg() :id(0), dayIndex(0), timeOffset(0)
	{

	}
	WeeklyUpdateCfg &operator= (const WeeklyUpdateCfg &r)
	{
		dayIndex =r.dayIndex;
		id = r.id;
		timeOffset = r.timeOffset;
		name = r.name;
		param = r.param;
		return *this;
	}
};


typedef std::vector<WeeklyUpdateCfg> weeklyUpCfg_vct;

class WeekAlarmMgr : public AlarmBase 
{
public:
	WeekAlarmMgr(NDAlarm *pObject, int timezone = 0);
	virtual~WeekAlarmMgr();

	void Update();
	const char *getNameFromCfg(int id);

	static int LoadWeeklyConfig(const char *tablename);
	static void Destroy();
	static const WeeklyUpdateCfg *GetWeeklyConfgInfo(int id);
	static int GetOffsetSeconds(int id);

protected:
	static const WeeklyUpdateCfg *_getWeeklyConfgInfo(weeklyUpCfg_vct &cfgVct, int id);
	static bool _loadConfigInfo(weeklyUpCfg_vct &cfgVct, const char *table);
	bool _UpdateNode(const WeeklyUpdateCfg *weeklyCfg, dailyEventInfo *eventInfo, int timezone);

private:	
	static weeklyUpCfg_vct s_global_weeklyCfgInfo;
};


class MonthAlarmMgr : public WeekAlarmMgr
{
public:
	MonthAlarmMgr(NDAlarm *pObject, int timezone = 0);
	virtual~MonthAlarmMgr();

	void Update();
	const char *getNameFromCfg(int id);

	static int LoadMonthlyConfig(const char *tablename);
	static void Destroy();
	static const WeeklyUpdateCfg *GetMonthlyConfgInfo(int id);
	static int GetOffsetSeconds(int id);

	time_t _getGmTime(int dayOfMon, int timeOffset, int timezone);
protected:
	bool _UpdateNode(const WeeklyUpdateCfg *weeklyCfg, dailyEventInfo *eventInfo, int timezone);

private:
	static weeklyUpCfg_vct s_global_monthlyCfgInfo;
};

#endif /* dailyUpdateMgr_hpp */
