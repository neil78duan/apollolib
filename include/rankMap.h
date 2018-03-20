/* file rankMap.h
 *
 * define rank map templete
 *
 * create by duan
 *
 * 2017.11.15
 */

#ifndef _RANK_MAP_H_
#define _RANK_MAP_H_

#include <list>

template<class Tindex, class Tval, int capacity>
class ApoRankMap
{

	struct rankNode
	{
		rankNode(const Tindex &f, const Tval &v) : first(f), second(v)
		{

		}
		Tindex first;
		Tval second;
	};
	typedef std::list<rankNode> rankDataVct;
public:
	typedef typename rankDataVct::iterator iterator;
	
	ApoRankMap()
	{

	}


	bool trytoAdd(const Tindex &index, const Tval &val)
	{
		bool isOk = false;
		if (m_data.size() == 0)	{
			m_data.push_back(rankNode(index, val));
			return true;
		}
		for (typename rankDataVct::iterator it = m_data.begin(); it != m_data.end(); it++)	{
			if (index >= it->first){
				m_data.insert(it, rankNode(index, val));
				isOk = true;
				break;
			}
		}

		
		if (isOk == false && m_data.size() < capacity) {
			m_data.push_back(rankNode(index, val));
			isOk = true;
		}
		else if (m_data.size() > capacity) {
			m_data.pop_back();
		}
		return isOk;
	}

	iterator begin() {
		return m_data.begin();
	}
	iterator end() {
		return m_data.end();
	}

protected:

	rankDataVct m_data;
};


#endif
