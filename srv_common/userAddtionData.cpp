/* file userAddtionData.cpp
 *
 * save player data key-value 
 *
 * create by duan 
 *
 * 2015-5-22
 */

#include "logic_parser/logicEndian.h"
#include "srv_common/userAddtionData.h"
#include "nd_net/nd_netbyte.h"
#include <string>

static int _read_string(const char *pCmdStream, char *outbuf, size_t size, int byteOrder)
{
	NDUINT16 size16 = 0;
	lp_stream_t p = (lp_stream_t)pCmdStream;
	//size16 = *((*(NDUINT16**)&p)++);
	p = lp_read_stream(p, size16, byteOrder);
	size -= 2;

	if (size16 >= size) {
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

int UserAdditionData::FromStream(void *data, size_t size, int byteOrder )
{
	const char *p = (const char*)data;
	const char *pend = p + size;
	char name[256];
	while (p < pend){
		name[0] = 0;
		int len = _read_string(p, name, sizeof(name),byteOrder);
		if (len ==-1){
			return -1;
		}
		p += len;
		size -= len;

		DBLDataNode val;
		len = val.ReadStream(p,size,byteOrder);
		if (-1== len){
			return -1;
		}
		p += len;
		size -= len;

		//parse name 
		//char realName[128];
		const char *pName = name;
		const char *typeName = NULL;
		char *pN = strstr(name,"$name=");
		if (pN) {
			pN += 6;
			pName = pN;
			pN = strchr(pN, '$');
			if (pN) {
				typeName = strchr(pN, '=');
				if (typeName) {
					++typeName;
				}
				*pN = 0;				
			}			
		}

		m_data_map[std::string(pName)] = additionDataNode(typeName, val);
		
	}
	return (int) (p - (const char*)data);
}

int UserAdditionData::ToStream(void *buf, size_t bufsize,int byteOrder, bool withTypeName ) const
{
	char *p = ( char*)buf;
	user_addition_map::const_iterator it = m_data_map.begin();
	for (; it != m_data_map.end(); it++){
		if (!it->second.value.CheckValid()){
			continue;
		}
		size_t len = it->first.size();
		if ((len + 8)>= bufsize)	{
			return -1;
		}

		if (withTypeName && it->second.typeName.size()) {
			//char myName[256];
			char *pSizeAddr = p;
			p += 2;
			bufsize -= 2;
			len = snprintf(p, bufsize, "$name=%s$type=%s", it->first.c_str(), it->second.typeName.c_str());
			p += len;
			bufsize -= len ;
			lp_write_stream(pSizeAddr, (NDUINT16)len, byteOrder);
		}
		else {
			p = lp_write_stream(p, (NDUINT16)len, byteOrder);
			bufsize -= 2;
			strncpy(p, it->first.c_str(), bufsize );
			bufsize -= len ;
			p += len;
		}
		len = it->second.value.WriteStream(p,bufsize,byteOrder);
		if (-1 == len){
			return -1;				 
		}
		p += len;
		bufsize -= len;
	}
	return (int) (p - (char*)buf);
}

bool UserAdditionData::convert2node(DBLDataNode &val, int byteOrder, bool withTypeName ) const
{
	int len ;
	char buf[0x10000] ;
	
	len = ToStream(buf, sizeof(buf),byteOrder) ;
	if (-1==len || len ==0){
		return false;
	}
	
	val.InitSet((void*)buf,len, OT_BINARY_DATA) ;
	
	return  true ;
	
}

bool UserAdditionData::Init(const DBLDataNode *val, int version ,int byteOrder)
{
	if (val->GetDataType() == OT_BINARY_DATA) {
		FromStream(val->GetBinary(),val->GetBinarySize(),byteOrder);
	
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
	return &it->second.value;
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
bool UserAdditionData::setData(const char *name, const  DBLDataNode &val, const char*typeName)
{
	user_addition_map::iterator it = m_data_map.find(std::string(name));
	if (it == m_data_map.end()) {
		m_data_map[name] = additionDataNode(typeName,val);
		myBaseAff::AffairAdd(std::string(name), additionDataNode(typeName, val));
	}
	else {
		myBaseAff::AffairModify(std::string(name), additionDataNode(typeName, val));
		it->second = additionDataNode(typeName, val);;
	}
	return true;
}

bool UserAdditionData::setData(const char *name, void*binData, size_t size, const char*typeName)
{
	return setData(name, DBLDataNode(binData, size),typeName);
}
bool UserAdditionData::setData(const char *name, const userdata_info*data, const char*typeName)
{
	return setData(name, DBLDataNode((void*)data, data->get_stream_len()), typeName);
}

bool UserAdditionData::opRead(const DBLDataNode& id, DBLDataNode &val)
{
	const char *name = id.GetText();
	if (!name || !*name) {
		return false;
	}
	const DBLDataNode *pdata = getData(name);
	if (!pdata)	{
		//m_error = NDSYS_ERR_PROGRAM_OBJ_NOT_FOUND;
		return false;
	}
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

void UserAdditionData::Undo(const std::string &index, const additionDataNode &old_val, int optype)
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
