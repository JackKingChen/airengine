// ***************************************************************
//  文件: BlockStore.h
//  日期: 2010-7-21
//  作者: huangjunjie
//  说明: 内存池模板核心分配组件
// ***************************************************************

#pragma once

#ifndef __BlockStore_H__
#define __BlockStore_H__



#include <vector>
#include <iostream>

#ifdef _MT
#include <windows.h>
#endif

namespace AIR
{
	class BlockStore
	{
	public:


		explicit BlockStore(size_t blockSize, size_t blockPerBatch=100, size_t blockAlignment=4)
			: m_ppNextBlock(NULL), 
			m_blockSize((blockSize+blockAlignment-1)&(~(blockAlignment-1))), 
			m_blockPerBatch(blockPerBatch),
			m_blockAlignment(blockAlignment)
		{
#ifdef _MT
			//如果定义成多线程,则启用
			InitializeCriticalSection( &m_cs );
#endif
		}




		virtual ~BlockStore()
		{
			//清空内存池中所有申请的内存
			size_t iNum = m_batches.size();
			for (size_t i=0; i<iNum; ++i)
			{
				unsigned char* p = (unsigned char*)m_batches[i];
				delete [] p;
				//GlobalFree(p);
			}

#ifdef _MT        
			//如果定义成多线程,则启用
			DeleteCriticalSection( &m_cs );
#endif
		}





#ifdef _MT
		//如果定义成多线程,则启用
		void Lock() { EnterCriticalSection( &m_cs ); }
		void Unlock() { LeaveCriticalSection( &m_cs ); }
#endif




		void* AllocateBlock()
		{  
#ifdef _MT
			//如果定义成多线程,则启用
			Lock();
#endif

			//如果空余内存没有则新开辟一块内存
			if (m_ppNextBlock == NULL || *m_ppNextBlock == NULL)
			{
				//确定T内存大小,再根据内存对齐得出需要申请的单个T内存块大小
				//               static const size_t blockSize = (m_blockSize+m_blockAlignment-1)&(~(m_blockAlignment-1));

				//新开一批内存(申请的内存比原有内存多15字节,用于下面的字节对齐)
				unsigned char* pNewBatch = new unsigned char[ m_blockPerBatch * m_blockSize + 15 ];
				//unsigned char* pNewBatch = (unsigned char*)GlobalAlloc(NULL, m_blockPerBatch*blockSize+15);

				//因为新开的内存与原来的内存并非连续的,因此需要由vector管理零散的内存块指针
				m_batches.push_back(pNewBatch);

				//暂时将警告消除
#pragma warning(disable : 4311 4312)
				//16字节对齐
				unsigned char* pAlignedPtr =(unsigned char*)((unsigned int)(pNewBatch+15)&(~15));

				//将各个元素可以放置的地址确定
				m_ppNextBlock = (unsigned char**)pAlignedPtr;
				for (size_t i=0; i<m_blockPerBatch-1; ++i)
				{
					*((unsigned int*)(pAlignedPtr + i*m_blockSize)) = (unsigned int)(pAlignedPtr + (i+1)*m_blockSize);
				}
				*((unsigned int*)(pAlignedPtr + (m_blockPerBatch-1)*m_blockSize)) = (unsigned int)0;
			}

			unsigned char* pBlock = (unsigned char*)m_ppNextBlock;
			m_ppNextBlock = (unsigned char**)*m_ppNextBlock;
#ifdef _MT
			//如果定义成多线程,则启用
			Unlock();
#endif
			return (void*)pBlock;
		}




		void ReleaseBlock(void* pBlock)
		{
#ifdef _MT
			//如果定义成多线程,则启用
			Lock();
#endif
			if(pBlock)
			{
				//暂时将警告消除
#pragma warning(disable : 4311)
				*((unsigned int*)pBlock) = (unsigned int)m_ppNextBlock;
				m_ppNextBlock = (unsigned char**)((unsigned char*)pBlock);
			}
#ifdef _MT
			//如果定义成多线程,则启用
			Unlock();
#endif
		}



	private:
		explicit BlockStore()
			: m_ppNextBlock(NULL), 
			m_blockSize(1), 
			m_blockPerBatch(100),
			m_blockAlignment(4)
		{
#ifdef _MT
			//如果定义成多线程,则启用
			InitializeCriticalSection( &m_cs );
#endif              
		}



		explicit BlockStore(const BlockStore&)
			: m_ppNextBlock(NULL), 
			m_blockSize(1), 
			m_blockPerBatch(100),
			m_blockAlignment(4)
		{
#ifdef _MT
			//如果定义成多线程,则启用
			InitializeCriticalSection( &m_cs );
#endif
		}


		BlockStore& operator=(const BlockStore&){}


	public:

		typedef AIRVector<unsigned char*> BatchPtrVector;

		unsigned char** m_ppNextBlock;          //指向内存块元素下一个位置的指针
		BatchPtrVector m_batches;               //管理零散的内存块指针
		const size_t m_blockSize;               //每个单体结构的内存块字节数
		const size_t m_blockPerBatch;           //每批申请的内存块数量
		const size_t m_blockAlignment;          //内存对齐字节数

#ifdef _MT
		//如果定义成多线程,则启用
		CRITICAL_SECTION    m_cs;
#endif

	};// end class BlockStore

};// end namespace AIR



#endif //! __BlockStore_H__