/* file userAddtionData.cpp
 *
 * save player data key-value 
 *
 * create by duan 
 *
 * 2015-5-22
 */

#include "userAddtionData.h"
#include "nd_net/nd_netbyte.h"
#include <string>

static int _read_string(const char *pCmdStream, char *outbuf, size_t size)
{
	NDUINT16 size16 = 0;
	const char *p = pCmdStream;
	//size16 = *((*(NDUINT16**)&p)++);
	size16 = nd_netstream_to_short(p) ;
	p+=sizeof(NDUINT16) ;
	
	if (size16 >= size){
		return -1;
	}
	memcpy(outbuf, p, size16);
	outbuf[size16] = 0;
	p += size16;
	return (int)(p - pCmdStream);
}

UserAdditionData::UserAdditionData()
{
}

UserAdditionData ::~UserAdditionData()
{
}

void UserAdditionData::Destroy()
{
	m_data_map.clear();
}

int UserAdditionData::FromStream(void *data, size_t size)
{
	const char *p = (const char*)data;
	const char *pend = p + size;
	char name[256];
	while (p < pend){
		name[0] = 0;
		int len = _read_string(p, name, sizeof(name));
		if (len ==-1){
			return -1;
		}
		p += len;

		DBLDataNode val;
		len = val.ReadStream(p);
		if (-1== len){
			return -1;
		}
		p += len;
		m_data_map[std::string(name)] = val;
		
	}
	return (int) (p - (const char*)data);
}

int UserAdditionData::ToStream(void *buf, size_t bufsize) const
{
	char *p = ( char*)buf;
	user_addition_map::const_iterator it = m_data_map.begin();
	for (; it != m_data_map.end(); it++){
		size_t len = it->first.size();
		if (len >= bufsize)	{
			return -1;
		}
		if (!it->second.CheckValid()){
			continue;
		}

		//*((*(NDUINT16**)&p)++) = len;
		nd_short_to_netstream(p, (NDUINT16)len) ;
		p+=sizeof(NDUINT16) ;
		
		strncpy(p, it->first.c_str(), bufsize - 2);
		bufsize -= len+2;
		p += len;
		len = it->second.WriteStream(p);
		if (-1 == len){
			return -1;				 
		}
		p += len;
		bufsize -= len;
	}
	return (int) (p - (char*)buf);
}

bool UserAdditionData::convert2node( DBLDataNode &val) const
{
	int len ;
	char buf[0x10000] ;
	
	len = ToStream(buf, sizeof(buf)) ;
	if (-1==len || len ==0){
		return false;
	}
	
	val.InitSet((void*)buf,len, OT_BINARY_DATA) ;
	
	return  true ;
	
}

bool UserAdditionData::Init(const DBLDataNode *val, int )
{
	if (val->GetDataType() == OT_BINARY_DATA) {
		FromStream(val->GetBinary(),val->GetBinarySize());
	
		return  true ;
	}
	return false ;
}


const DBLDataNode *UserAdditionData::getData(const char *name) const
{
	user_addition_map::const_iterator it = m_data_map.find(std::string(name));
	if (it == m_data_map.end()) {
		return NULL;
	}
	return &it->second;
}

bool UserAdditionData::removeData(const char *name)
{
	user_addition_map::iterator it = m_data_map.find(std::string(name));
	if (it!=m_data_map.end()){
		myBaseAff::AffairDel(std::string(name), it->second);
		m_data_map.erase(it);
	}
	return true;
}
bool UserAdditionData::setData(const char *name, const  DBLDataNode &val)
{
	user_addition_map::iterator it = m_data_map.find(std::string(name));
	if (it == m_data_map.end()) {
		m_data_map[name] = val;
		myBaseAff::AffairAdd(std::string(name), (DBLDataNode &)val);
	}
	else {
		myBaseAff::AffairModify(std::string(name), (DBLDataNode &)val);
		it->second = val;
	}
	return true;
}

bool UserAdditionData::setData(const char *name, void*binData, size_t size)
{
	return setData(name, DBLDataNode(binData, size));
// 	m_data_map[name] = DBLDataNode(binData,size) ;
// 	return true;
}
bool UserAdditionData::setData(const char *name, const userdata_info*data)
{
	return setData(name, DBLDataNode((void*)data, data->get_stream_len()));
// 	size_t size = data->get_stream_len() ;
// 	m_data_map[name] = DBLDataNode((void*)data,size) ;
// 	return true;
}

bool UserAdditionData::opRead(const DBLDataNode& id, DBLDataNode &val)
{
	const char *name = id.GetText();
	if (!name || !*name) {
		return false;
	}
	const DBLDataNode *pdata = getData(name);
	val = *pdata;
	return true;
}
bool UserAdditionData::opWrite(const DBLDataNode& id, const DBLDataNode &val)
{
	const char *name = id.GetText();
	if (!name || !*name) {
		return false;
	}
	return setData(name, val);
}

bool UserAdditionData::opAdd(const DBLDataNode& id, const  DBLDataNode &val)
{
	const char *name = id.GetText();
	if (!name || !*name) {
		return false;
	}
	return setData(name, val);
}
bool UserAdditionData::opSub(const DBLDataNode& id, const DBLDataNode &val)
{
	const char *name = id.GetText();
	if (!name || !*name) {
		return false;
	}
	return removeData(name);
}
bool UserAdditionData::opClear(const DBLDataNode& id)
{
	const char *name = id.GetText();
	if (!name || !*name) {
		return false;
	}
	return removeData(name);
}

void UserAdditionData::Undo(std::string &index, DBLDataNode &old_val, int optype)
{
	if (myBaseAff::EAO_ADD == optype || myBaseAff::EAO_MODIFIED == optype) {
		m_data_map[index] = old_val;
	}
	else if (myBaseAff::EAO_DEL == optype){		
		m_data_map.erase(index);
	}
}

bool UserAdditionData::CheckInAffair()
{
	return myBaseAff::GetAffairStat() ? true : false;
}

bool UserAdditionData::BeginAffair()
{
	myBaseAff::Begin();
	return true;
}
bool UserAdditionData::CommitAffair()
{
	myBaseAff::Commit();
	return true;

}
bool UserAdditionData::RollbackAffair()
{
	myBaseAff::Rollback();
	return true;
}
