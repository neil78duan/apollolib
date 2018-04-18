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
#include "ndapplib/nd_affair.h"
#include "apollo_data.h"
#include <map>
#include <string>
#include "logic_parser/objectBaseMgr.h"

struct additionDataNode
{
	std::string typeName;
	DBLDataNode value;

	additionDataNode() 
	{}
	additionDataNode(const char *name, const DBLDataNode &val) :  value(val)
	{
		if (name && *name) {
			typeName = name;
		}
	}

	additionDataNode& operator = (const additionDataNode &r)
	{
		typeName = r.typeName;
		value = r.value;
		return *this;
	}
};

typedef std::map<std::string, additionDataNode> user_addition_map;
typedef NDAffair<std::string, additionDataNode> AdditionDataAffair;

#define USER_ADDITION_DATA_TYPE "UserAdditionData"

class UserAdditionData :public LogicObjectBase, public AdditionDataAffair
{
	typedef AdditionDataAffair myBaseAff;
public:
	UserAdditionData() ;
	~UserAdditionData ();

	int FromStream(void *data, size_t size, int byteOrder = 1);
	int ToStream(void *buf, size_t bufsize, int byteOrder = 1, bool withTypeName = true) const;
	
	const DBLDataNode *getData(const char *name) const;
	bool setData(const char *name, const DBLDataNode &val, const char*typeName=NULL);
	bool removeData(const char *name);
	bool setData(const char *name, void*binData, size_t size, const char*typeName = NULL);
	bool setData(const char *name, const userdata_info* data, const char*typeName = NULL);
	
	
	bool convert2node(DBLDataNode &val, int byteOrder = 1,bool withTypeName= true) const;
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

	void Undo(const std::string &index, const  additionDataNode &old_val, int optype);
private:
	user_addition_map m_data_map;
};


#endif
