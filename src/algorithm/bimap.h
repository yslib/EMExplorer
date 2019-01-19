#ifndef BIMAP_H
#define BIMAP_H

#include <unordered_map>

#include <QPersistentModelIndex>



template<typename T>
class KeyTraits;

template<>
class KeyTraits<int> {
public :
	class hash {
	public:
		std::size_t operator()(int key)const
		{
			return std::hash<int>{}(key);
		}
	};
	typedef hash KeyHash;

	typedef int KeyType;

	static KeyType invalid() 
	{
		return -1;

    }
};

template<>
class KeyTraits<QPersistentModelIndex> {
public:
	class hash
	{
	public:
		std::size_t operator()(const QPersistentModelIndex & key)const
		{
			return qHash(key);
		}
	};
	typedef QPersistentModelIndex KeyType;
	typedef hash KeyHash;
    static KeyType invalid()
    {
		return QPersistentModelIndex();
	}
};

template<typename T>
class KeyTraits<T*>{
public:
    class hash
    {
    public:
        std::size_t operator()(const T* key)const
        {
            return std::hash<T*>{}(key);
        }
    };
    typedef T* KeyType;
    typedef hash KeyHash;
    static KeyType invalid() {
        return nullptr;
    }
};

template<typename T1,typename T2,
typename Key1Traits = KeyTraits<T1>,
typename Key2Traits = KeyTraits<T2>>



class BiMap {
    std::unordered_map<T1,T2,typename Key1Traits::KeyHash>	m_index2Id;
    std::unordered_map<T2,T1,typename Key2Traits::KeyHash>  m_id2Index;
public:
	BiMap() {}
	void addQueryPair(const T1 & t1,const T2 & t2) 
	{
		m_index2Id[t1] = t2;
		m_id2Index[t2] = t1;
	}
    void removeQueryPair(const T2 & t2)
    {
		const auto itr = m_id2Index.find(t2);
		if (itr == m_id2Index.end())
			return;
		m_index2Id.erase(*itr);
		m_id2Index.erase(t2);
	}
    void removeQueryPair(const T1 & t1)
    {
		const auto itr = m_index2Id.constFind(t1);
		if (itr == m_index2Id.constEnd())
			return;
		m_id2Index.erase(*itr);
		m_index2Id.erase(t1);
	}
    const T1 & query(const T2 & t2)const
    {
		const auto itr = m_id2Index.find(t2);
		if (itr != m_id2Index.end()) {
			return itr->second;
		}
		return Key1Traits::invalid();
	}
    const T2 & query(const T1 & t1)const
    {
		const auto itr = m_index2Id.find(t1);
		if (itr != m_index2Id.end()) {
			return itr->second;
		}
		return Key2Traits::invalid();
	}

    const T1 & operator()(const T2 & t2)const
    {
        return query(t2);
    }

    const T2 & operator()(const T1 & t1)const
    {
        return query(t1);
    }

	void clear() 
	{
		m_id2Index.clear();
		m_index2Id.clear();
	}
};

#endif // BIMAP_H
