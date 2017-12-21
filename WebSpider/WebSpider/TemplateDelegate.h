#ifndef __TEMPLATE_DELEGATE_H__
#define __TEMPLATE_DELEGATE_H__
#include"COMMONALLOC.h"
#include<list>
class IDelegate
{
public:
	virtual void Invoke(int param) = 0;
};

template<typename T>
class Delegate : public IDelegate
{
	friend class EventPool;
public:
	Delegate(T* _instance, void (T::*Functions)(int)) : m_Instance(_instance), m_pFun(Functions) {}

	virtual ~Delegate() {}
	virtual void Invoke(int a)
	{
		(m_Instance->*m_pFun)(a);
	}
private:
	T*          m_Instance;
	void        (T::*m_pFun)(int);
};

class EventPool
{
public:
	template<typename T>
	void Attach(T* _instance, void (T::*Funtions)(int))
	{
		IDelegate *pNewFunctions = new Delegate<T>(_instance, Funtions);
		m_Pool.push_back(pNewFunctions);
	}

	template<typename T>
	void Detach(T* _instance, void (T::*Funtions)(int))
	{
		pooliter iter = m_Pool.begin();
		for (;iter != m_Pool.end();++iter)
		{
			if ((*iter)->m_Instance == _instance && *iter->m_pFun == Funtions)
			{
				SAFE_DELETE_PTR(*iter);
				iter = m_Pool.erase(iter);
			}
		}
	}

	void Invoke(int a)
	{
		pooliter iter = m_Pool.begin();
		for (;iter != m_Pool.end();++iter)
			(*iter)->Invoke(a);
	}
private:
	typedef std::list<IDelegate*>         listpool;
	typedef listpool::iterator            pooliter;
	listpool                              m_Pool;
};

#endif