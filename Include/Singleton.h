//
// singleton.h
//
//  Created on: 2016-05-05
//      Author: zjg
//

#ifndef SINGLETON_H_
#define SINGLETON_H_

#define  TOTAL_OBJECT_RELEASE   0

#if TOTAL_OBJECT_RELEASE
#include <typeinfo>
#include "TotalObject.h"

#pragma comment(lib, "TotalObject.lib")
#endif

template<class T>
class Singleton
{
public:
	static T& Instance()
	{ 
		if(NULL == m_pInstance)
		{
#if TOTAL_OBJECT_RELEASE
			AddClass(typeid(T).name());
#endif
			m_pInstance = new T;
		}
		return *m_pInstance;
	}
	static void UnInstance()
	{
		if (NULL != m_pInstance)
		{
#if TOTAL_OBJECT_RELEASE
			RemoveClass(typeid(T).name());
#endif
			delete m_pInstance;
			m_pInstance = NULL;
		}
	}
protected:
	Singleton(){};
private:
	static T* m_pInstance;
 };

template<class T>
T*	Singleton<T>::m_pInstance	=	NULL;

#endif // SINGLETON_H_
