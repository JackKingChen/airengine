// ***************************************************************
//  文件: MemoryPool.h
//  日期: 2010-7-21
//  作者: huangjunjie
//  说明: 
//
//	提供一个简单的线程安全的内存池模板
//	其中线程安全部分需要使用WIN32 API
//	警告: TObject_pool只为类设计使用,所以并不需要连续内存块
//	所以严禁一次性new出数组,像这样是被严格禁止的:
//	class TestObject : public Common::TObject_pool<TestObject>
//		new TestObject [xxx]
//	正确使用方式:
//	for (int i=0; i<xxx; ++i)
//	{
//		new TestObject;
//	}
//	而TAllocator则允许一次性分配连续的内存块：
//	Common::TAllocator<TestObject> Allocator;
//	TestObject* p = Allocator(100);
//		等同于:
//	TestObject* p = new TestObject[100];
//	并且启用内存池后new出来的对象是不会引发对象构造函数，并且delete也不会引发对象析构函数
// ***************************************************************

#pragma once

#ifndef __MemoryPool_H__
#define __MemoryPool_H__

#include "BlockStore.h"
#include <vector>

//为多线程使用线程安全
#ifdef _MT
//#include <windows.h>
#endif


namespace AIR
{

    /*********************************************
    参数:
        blocksPerBatch: 每次申请的内存块大小
        blockAlignment: 机器CPU字节长度
    *********************************************/
    template <typename T, size_t blocksPerBatch = 100, size_t blockAlignment = 4>
    class TObject_pool
    {
    public:
        __inline void* operator new(size_t)
        {
            return s_Store.AllocateBlock();
        }

        __inline void operator delete(void* pBlock)
        {
            s_Store.ReleaseBlock((T*)pBlock);
        }
    
    protected:
		//将构造函数设置成protected,使之成为一纯虚基类模板
        TObject_pool() = 0 {}

    private:
        static BlockStore s_Store;
    };



    template<class T, size_t blocksPerBatch, size_t blockAlignment> 
    typename BlockStore TObject_pool<T, blocksPerBatch, blockAlignment>::s_Store(sizeof(T), blocksPerBatch, blockAlignment);




    //仿STL的allocator
    //T:                需要申请内存池的类型
    //maxBlockSize:     内存池最大连续长度
    //blocksPerBatch:   内存池申请内存时一批数量
    //blockAlignment:   字节对齐长度
    template<class T, size_t maxBlockSize=4096, size_t blocksPerBatch=100, size_t blockAlignment=4>
    class TAllocator : public std::_Allocator_base<T>
    {
    public:
        //构造函数
        TAllocator()
        {
            InitMemPool();
        }

        TAllocator(const TAllocator<T>&)
        {
            InitMemPool();
        }

        virtual ~TAllocator()
        {
            DestroyMemPool();
        }

    public:

        typedef std::_Allocator_base<T> _Mybase;
        typedef typename _Mybase::value_type value_type;

        typedef value_type _FARQ *pointer;
        typedef value_type _FARQ& reference;
        typedef const value_type _FARQ *const_pointer;
        typedef const value_type _FARQ& const_reference;

        typedef _SIZT size_type;
        typedef _PDFT difference_type;

        template<class _Other>
        struct rebind
        {
            typedef TAllocator<_Other> other;
        };

        pointer address(reference _Val) const
        {
            return (&_Val);
        }

        const_pointer address(const_reference _Val) const
        {
            return (&_Val);
        }

        template<class _Other>
        TAllocator(const TAllocator<_Other>&)
        {
            InitMemPool();
        }

        template<class _Other>
        TAllocator<T>& operator=(const TAllocator<_Other>&)
        {
            return (*this);
        }

        //释放内存
        static void deallocate(pointer _Ptr, size_type)
        {
            deallocate(_Ptr);
        }
        
        static void deallocate(pointer _Ptr)
        {
            unsigned char* p = (unsigned char*)_Ptr;
            p -= blockAlignment;
            if (*p == 0xFF)
            {
                for (AIRVector<void*>::iterator itr = m_AddrList.begin();
                    itr != m_AddrList.end(); itr++ )
                {
                    if (*itr == p)
                    {
                        delete [] p;
                        m_AddrList.erase(itr);
                        return;
                    }
                }
            }
            m_poollist[*p]->ReleaseBlock(p);
        }

        //申请内存
        //内存池大小在经过内存字节对齐计算后，再额外添加blockAlignment字节以维护每个申请的内存块信息
        static pointer allocate(size_type _Count)
        {
            unsigned char* p = NULL;
            if (_Count == 1 || _Count == 0)
            {
                p = (unsigned char*)m_poollist[0]->AllocateBlock();
                *p = 0x0;
                return (pointer)(p+blockAlignment);
            }

            size_t nIndex = 0;
            size_t nCount = 1;
            while (nCount < _Count)
            {
                ++nIndex;
                nCount *= 2;
            }

            if (nIndex < m_poollist.size())
            {
                p = (unsigned char*)m_poollist[nIndex]->AllocateBlock();
                *p = (unsigned char)nIndex;
                return (pointer)(p+blockAlignment);
            }

            //如果需要申请的内存块超出maxBlockSize,则交由系统分配
            p = new unsigned char[sizeof(T)*_Count+blockAlignment];
            m_AddrList.push_back(p);
            *p = 0xFF;
            return (pointer)(p+blockAlignment);
        }

        static pointer allocate(size_type _Count, const void _FARQ*)
        {
            return (allocate(_Count));
        }

        void construct(pointer _Ptr, const T& _Val)
        {
            _Construct(_Ptr, _Val);
        }

        void destroy(pointer _Ptr)
        {
            _Destroy(_Ptr);
        }

        _SIZT max_size() const
        {
            _SIZT _Count = (_SIZT)(-1) / sizeof (T);
            return (0 < _Count ? _Count : 1);
        }

    private:
        // 不允许从其他函数进行COPY构造
        TAllocator<T>& operator=(const TAllocator<T>&){}

    private:

        // 初始化/释放内存池
        static void InitMemPool()
        {
            ++m_nCount;
            if (m_nCount != 1)
            {
                //引用计数，此内存池已经启用，则不再被初始化
                return;
            }
            //根据maxBlocksizee计算出需要多少个mempool
            size_t currsize = 1;
            BlockStore* pblockstore = NULL;
            while ((currsize <= maxBlockSize) || (currsize/maxBlockSize<2))
            {
                pblockstore = new BlockStore(currsize*sizeof(T)+blockAlignment, blocksPerBatch, blockAlignment);
                m_poollist.push_back(pblockstore);
                currsize *= 2;
            }
        }


		// 销毁内存池
        static void DestroyMemPool()
        {
            --m_nCount;
            if (m_nCount != 0)
            {
                //引用计数，如果还有使用此内存池的类，则不清除内存池
                return;
            }
            size_t nCnt = m_poollist.size();
            for (size_t i=0; i<nCnt; ++i)
            {
                delete m_poollist[i];
            }
            nCnt = m_AddrList.size();
            for (size_t i=0; i<nCnt; ++i)
            {
                delete [] m_AddrList[i];
            }
        }

    private:
        static AIRVector<BlockStore*>		m_poollist;         //存放分配器中所包含不同大小内存池列表
        static AIRVector<void*>			m_AddrList;         //存放系统所分配的内存指针存放
        static unsigned int					m_nCount;           //初始化引用计数
    };



    template<class T, size_t maxBlockSize, size_t blocksPerBatch, size_t blockAlignment>
        AIRVector<BlockStore*> TAllocator<T, maxBlockSize, blocksPerBatch, blockAlignment>::m_poollist;

    template<class T, size_t maxBlockSize, size_t blocksPerBatch, size_t blockAlignment>
        AIRVector<void*> TAllocator<T, maxBlockSize, blocksPerBatch, blockAlignment>::m_AddrList;

    template<class T, size_t maxBlockSize, size_t blocksPerBatch, size_t blockAlignment>
        unsigned int TAllocator<T, maxBlockSize, blocksPerBatch, blockAlignment>::m_nCount;



};//end namespace AIR

#endif //! __MemoryPool_H__