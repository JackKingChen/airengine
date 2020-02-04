// ***************************************************************
//  �ļ�: MemoryPool.h
//  ����: 2010-7-21
//  ����: huangjunjie
//  ˵��: 
//
//	�ṩһ���򵥵��̰߳�ȫ���ڴ��ģ��
//	�����̰߳�ȫ������Ҫʹ��WIN32 API
//	����: TObject_poolֻΪ�����ʹ��,���Բ�����Ҫ�����ڴ��
//	�����Ͻ�һ����new������,�������Ǳ��ϸ��ֹ��:
//	class TestObject : public Common::TObject_pool<TestObject>
//		new TestObject [xxx]
//	��ȷʹ�÷�ʽ:
//	for (int i=0; i<xxx; ++i)
//	{
//		new TestObject;
//	}
//	��TAllocator������һ���Է����������ڴ�飺
//	Common::TAllocator<TestObject> Allocator;
//	TestObject* p = Allocator(100);
//		��ͬ��:
//	TestObject* p = new TestObject[100];
//	���������ڴ�غ�new�����Ķ����ǲ������������캯��������deleteҲ��������������������
// ***************************************************************

#pragma once

#ifndef __MemoryPool_H__
#define __MemoryPool_H__

#include "BlockStore.h"
#include <vector>

//Ϊ���߳�ʹ���̰߳�ȫ
#ifdef _MT
//#include <windows.h>
#endif


namespace AIR
{

    /*********************************************
    ����:
        blocksPerBatch: ÿ��������ڴ���С
        blockAlignment: ����CPU�ֽڳ���
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
		//�����캯�����ó�protected,ʹ֮��Ϊһ�������ģ��
        TObject_pool() = 0 {}

    private:
        static BlockStore s_Store;
    };



    template<class T, size_t blocksPerBatch, size_t blockAlignment> 
    typename BlockStore TObject_pool<T, blocksPerBatch, blockAlignment>::s_Store(sizeof(T), blocksPerBatch, blockAlignment);




    //��STL��allocator
    //T:                ��Ҫ�����ڴ�ص�����
    //maxBlockSize:     �ڴ�������������
    //blocksPerBatch:   �ڴ�������ڴ�ʱһ������
    //blockAlignment:   �ֽڶ��볤��
    template<class T, size_t maxBlockSize=4096, size_t blocksPerBatch=100, size_t blockAlignment=4>
    class TAllocator : public std::_Allocator_base<T>
    {
    public:
        //���캯��
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

        //�ͷ��ڴ�
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

        //�����ڴ�
        //�ڴ�ش�С�ھ����ڴ��ֽڶ��������ٶ������blockAlignment�ֽ���ά��ÿ��������ڴ����Ϣ
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

            //�����Ҫ������ڴ�鳬��maxBlockSize,����ϵͳ����
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
        // �������������������COPY����
        TAllocator<T>& operator=(const TAllocator<T>&){}

    private:

        // ��ʼ��/�ͷ��ڴ��
        static void InitMemPool()
        {
            ++m_nCount;
            if (m_nCount != 1)
            {
                //���ü��������ڴ���Ѿ����ã����ٱ���ʼ��
                return;
            }
            //����maxBlocksizee�������Ҫ���ٸ�mempool
            size_t currsize = 1;
            BlockStore* pblockstore = NULL;
            while ((currsize <= maxBlockSize) || (currsize/maxBlockSize<2))
            {
                pblockstore = new BlockStore(currsize*sizeof(T)+blockAlignment, blocksPerBatch, blockAlignment);
                m_poollist.push_back(pblockstore);
                currsize *= 2;
            }
        }


		// �����ڴ��
        static void DestroyMemPool()
        {
            --m_nCount;
            if (m_nCount != 0)
            {
                //���ü������������ʹ�ô��ڴ�ص��࣬������ڴ��
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
        static AIRVector<BlockStore*>		m_poollist;         //��ŷ���������������ͬ��С�ڴ���б�
        static AIRVector<void*>			m_AddrList;         //���ϵͳ��������ڴ�ָ����
        static unsigned int					m_nCount;           //��ʼ�����ü���
    };



    template<class T, size_t maxBlockSize, size_t blocksPerBatch, size_t blockAlignment>
        AIRVector<BlockStore*> TAllocator<T, maxBlockSize, blocksPerBatch, blockAlignment>::m_poollist;

    template<class T, size_t maxBlockSize, size_t blocksPerBatch, size_t blockAlignment>
        AIRVector<void*> TAllocator<T, maxBlockSize, blocksPerBatch, blockAlignment>::m_AddrList;

    template<class T, size_t maxBlockSize, size_t blocksPerBatch, size_t blockAlignment>
        unsigned int TAllocator<T, maxBlockSize, blocksPerBatch, blockAlignment>::m_nCount;



};//end namespace AIR

#endif //! __MemoryPool_H__