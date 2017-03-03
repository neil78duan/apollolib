/* file userAddtionData.h
 *
 * save player data key-value 
 *
 * create by duan 
 *
 * 2015-5-22
 */

#ifndef _USER_ADDTION_DATA_H_
#define _USER_ADDTION_DATA_H_

#include "logic_parser/logicDataType.h"
#include "apollo_data.h"
#include "ndapplib/nd_affair.h"
#include <map>
#include <string>
#include "logic_parser/objectBaseMgr.h"


typedef std::map<std::string, DBLDataNode> user_addition_map;

typedef NDAffair<std::string, DBLDataNode> AdditionDataAffair;
// store player addition data key-value (string, DBLDataNode)
class UserAdditionData :public LogicObjectBase, public AdditionDataAffair
{
	typedef AdditionDataAffair myBaseAff;
public:
	UserAdditionData() ;
	~UserAdditionData ();

	int FromStream(void *data, size_t size, int byteOrder = 1);
	int ToStream(void *buf, size_t bufsize, int byteOrder = 1) const;
	
	const DBLDataNode *getData(const char *name) const;
	bool setData(const char *name, const DBLDataNode &val);
	bool removeData(const char *name);
	bool setData(const char *name, void*binData, size_t size);
	bool setData(const char *name, const userdata_info* data);
	
	
	bool convert2node(DBLDataNode &val, int byteOrder = 1) const;
	bool Init(const DBLDataNode *val, int version = 0, int byteOrder = 1);
	int toStream(DBLDataNode *outData, int byteOrder = 1) const
	{
		return convert2node(*outData,byteOrder)?0:-1;
	}

	int Count() { return (int)m_data_map.size(); }

	const user_addition_map& getDataMap() const { return m_data_map; }
public:
	void Destroy();
	bool opRead(const DBLDataNode& id, DBLDataNode &val);
	bool opWrite(const DBLDataNode& id, const DBLDataNode &val);
	bool opAdd(const DBLDataNode& id, const  DBLDataNode &val);
	bool opSub(const DBLDataNode& id, const DBLDataNode &val);
	//bool opClear(const DBLDataNode& id, const DBLDataNode &val);
	
	bool CheckInAffair();
	bool BeginAffair();
	bool CommitAffair();
	bool RollbackAffair();
protected:

	void Undo(std::string &index, DBLDataNode &old_val, int optype);
private:
	user_addition_map m_data_map;
};


#endif
