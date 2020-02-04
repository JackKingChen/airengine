
#ifndef __C_OBJECTPOOL_H__
#define __C_OBJECTPOOL_H__

#include "MemoryAllocatorConfig.h"
#include "Util.h"

namespace AIR
{
	template<typename T>
	class ObjectPool
	{
	public:
		ObjectPool(unsigned int pool_size)
		{
			Resize(pool_size);
		}

        ObjectPool() : m_PoolSize(0), m_nFree(0)
        {
            
        }

		~ObjectPool()
		{

            while (!m_pool.empty())
            {
                typename AIRDeque<T*>::iterator it = m_pool.begin();
                T* pItem = *it;
                m_pool.pop_front();
                SafeDelete(pItem);
            }
		}

        void Resize(unsigned int pool_size)
        {
            while (!m_pool.empty())
            {
                typename AIRDeque<T*>::iterator it = m_pool.begin();
                T* pItem = *it;
                m_pool.pop_front();
                SafeDelete(pItem);
            }
            m_PoolSize = m_nFree = pool_size;

            for (size_t i = 0; i < m_PoolSize; ++i)
            {
                T* pItem = new T;
                m_pool.push_back(pItem);
            }
        }

        T* AllocObject()
        {
            if (m_nFree == 0)
            {
                T* pObject = new T;
                m_PoolSize++;
                return pObject;
            }
            typename AIRDeque<T*>::iterator it = m_pool.begin();
            T* pItem = *it;
            m_pool.pop_front();
            m_nFree--;

            return pItem;
        }

        void FreeObject(T* p)
        {
            m_nFree++;
            m_pool.push_back(p);
        }

        /*
		unsigned int Alloc()
		{
			assert(m_nFree > 0);
			return m_FreeSlots[--m_nFree];
		}

		void Free(unsigned int i)
		{
			assert(m_nFree < m_PoolSize);
			m_FreeSlots[m_nFree++] = i;
		}

        
		T &GetObject(unsigned int i)
		{
			assert(i < m_PoolSize);
			return m_Buffer[i];
		}

		T *GetObjectPtr(unsigned int i)
		{
			assert(i < m_PoolSize);
			return &m_Buffer[i];
		}

		unsigned int Ptr2Index(T *p)
		{
			assert(p>=m_Buffer && p<m_Buffer+m_PoolSize);
			return (unsigned int)(p-m_Buffer);
		}
        */

        void Reset()
        {
            //m_nFree = m_PoolSize;
        }

	private:
		//T *m_Buffer;
		//unsigned int *m_FreeSlots;
		unsigned int m_nFree;
		unsigned int m_PoolSize;

        AIRDeque<T*> m_pool;
	};
}

#endif