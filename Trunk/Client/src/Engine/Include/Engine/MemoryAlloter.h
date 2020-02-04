#ifndef __MemoryAlloter_H__
#define __MemoryAlloter_H__

#include "EngineConfig.h"
#include <limits>
#include "nedmalloc/nedmalloc.h"

//#include "MemoryNedAlloc.h"
//#ifdef new
//#  undef new
//#endif
//#ifdef delete
//#  undef delete
//#endif


//template <class ClsType, class Alloc>
//class EngineExport CMemoryAlloter
//{
//public:
//    explicit CMemoryAlloter()
//    {
//
//    }
//
//    ~CMemoryAlloter()
//    {
//
//    }
//
//    void* operator new(size_t sz)
//    {
//        return Alloc::AllocBytes(sz, typeid(ClsType).name());
//    }
//
//    /// placement operator new
//    void* operator new(size_t sz, void* ptr)
//    {
//        (void) sz;
//        return ptr;
//    }
//
//    void* operator new[] ( size_t sz )
//    {
//        return Alloc::AllocBytes(sz, typeid(ClsType).name());
//    }
//
//    void operator delete( void* ptr )
//    {
//        Alloc::deallocBytes(ptr);
//    }
//
//    // Corresponding operator for placement delete (second param same as the first)
//    void operator delete( void* ptr, void* )
//    {
//        Alloc::deallocBytes(ptr);
//    }
//
//
//    void operator delete[] ( void* ptr )
//    {
//        Alloc::deallocBytes(ptr);
//    }
//
//
//protected:
//
//private:
//
//};


// 底层内存分配器,使用nedmalloc来分配释放
class EngineExport InternalAllocatorNed
{
public:
	static inline void* AllocBytes( size_t size )
	{
		void* ptr = nedalloc::nedmalloc(size); 
		return ptr;
	}

	static inline void deallocBytes(void* ptr)
	{
		if (ptr)
		{
			nedalloc::nedfree(ptr);
		}
	}
};


#include <stdlib.h>
// 底层内存分配器,使用标准C++内存管理
class EngineExport InternalAllocatorStd
{
public:
	static inline void* AllocBytes( size_t size )
	{
		void* ptr = malloc(size); 
		return ptr;
	}

	static inline void deallocBytes(void* ptr)
	{
		if (ptr)
		{
			free(ptr);
		}
	}
};


//#define EngineAllocator(ClsType)	

#endif// endif __MemoryAlloter