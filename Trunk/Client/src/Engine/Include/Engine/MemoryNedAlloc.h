#ifndef __MemoryNedAlloc_H__
#define __MemoryNedAlloc_H__

#include "Singleton.h"
#include "MemoryAlloter.h"

#define MAX_PATH_LEN					1024
// 内存分配的堆栈追溯层数
#define DBG_MEM_CALLSTACK_NUM			32

enum AllocatorType
{
	AT_Unknown = 0,
	AT_HLVENGINE,
	AT_HLVSTL,
};

struct AllocationData;
struct AllocationSearchNode;

// 图形内存占用分析器
class EngineExport GraphicProfiler
{
public:
	static bool OnCreateVB( void* ptr, size_t dataSize, const char* funcName, const char* fileName, int line);
	static bool OnReleaseVB( void* ptr );

	static bool OnCreateIB( void* ptr, size_t dataSize, const char* funcName, const char* fileName, int line);
	static bool OnReleaseIB( void* ptr );

	static bool OnCreateTexture( void* ptr, size_t dataSize, const char* funcName, const char* fileName, int line);
	static bool OnReleaseTexture( void* ptr );

	static bool OnCreateRenderTarget( void* ptr, size_t dataSize, const char* funcName, const char* fileName, int line);
	static bool OnReleaseRenderTarget( void* ptr );

	static bool OnCreateShader( void* ptr, size_t dataSize, const char* funcName, const char* fileName, int line );
	static bool OnReleaseShader( void* ptr );
};
#define GraphicProfilerOnCreateVB(objPtr, dataSize)				GraphicProfiler::OnCreateVB( objPtr, dataSize, __FUNCTION__, __FILE__, __LINE__);
#define GraphicProfilerOnCreateIB(objPtr, dataSize)				GraphicProfiler::OnCreateIB( objPtr, dataSize, __FUNCTION__, __FILE__, __LINE__);
#define GraphicProfilerOnCreateTexture(objPtr, dataSize)		GraphicProfiler::OnCreateTexture( objPtr, dataSize, __FUNCTION__, __FILE__, __LINE__);
#define GraphicProfilerOnCreateRenderTarget(objPtr, dataSize)	GraphicProfiler::OnCreateRenderTarget( objPtr, dataSize, __FUNCTION__, __FILE__, __LINE__);
#define GraphicProfilerOnCreateShader(objPtr, dataSize)			GraphicProfiler::OnCreateShader( objPtr, dataSize, __FUNCTION__, __FILE__, __LINE__);
#define GraphicProfilerOnReleaseVB(objPtr)						GraphicProfiler::OnReleaseVB(objPtr)
#define GraphicProfilerOnReleaseIB(objPtr)						GraphicProfiler::OnReleaseIB(objPtr)
#define GraphicProfilerOnReleaseTexture(objPtr)					GraphicProfiler::OnReleaseTexture(objPtr)
#define GraphicProfilerOnReleaseRenderTarget(objPtr)			GraphicProfiler::OnReleaseRenderTarget(objPtr)
#define GraphicProfilerOnReleaseShader(objPtr)					GraphicProfiler::OnReleaseShader(objPtr)

// 系统内存占用分析器
class EngineExport MemoryProfiler
{
public:
	MemoryProfiler();
	~MemoryProfiler();

	static bool Initialize();
	static bool Destroy();
	static AllocationData* FindMemInfo( void* ptr );	
	static void SnapMemShot(const char* memLogFile, bool printCallStack);

	static void* AllocBytes(size_t count);
	static void deallocBytes(void* ptr);

	static AllocationData* RecMemInfo( void* dataPtr, size_t dataSize, const char* typeName, const char* fileName, const size_t line, const char* funcName, AllocatorType at );
	static bool RemoveMemInfo( void* ptr, size_t &totalSize );

	static bool GetFileAndLine(const void *pAddress, char szFile[MAX_PATH_LEN], int &line);

protected:	
	static AllocationData* AddMemInfo( void* ptr, unsigned int size, const char* typeName, const char* file, const size_t line, const char* funcName, AllocatorType at);	
	static bool InitDbgHelpLib();
	static void FreeDbgHelpLib();

	static AllocationSearchNode*	sRootInfoLevel;
	static unsigned long	sProfilerTotalAllocBytes;
};


// 针对非STL的对象所用
class EngineExport HighLevelEngineAllocator
{
public:
	static void* AllocBytes(size_t count, const char* typeName, const char* file, const size_t line, const char* funcName );

	static void deallocBytes(void* ptr);

	/// Get the maximum size of a single allocation
	static size_t getMaxAllocationSize();

	static size_t sTotalAllocSize;
	static size_t sTotalAllocCount;
	static size_t sTotalUseSize;
	static const AllocatorType ALLOC_TYPE = AT_HLVENGINE;
};

// 针对STL对象内部分配所用
class EngineExport HighLevelSTLAllocator
{
public:
	static void* AllocBytes(size_t count, const char* typeName, const char* file, const size_t line, const char* funcName);

	static void deallocBytes(void* ptr);

	/// Get the maximum size of a single allocation
	static size_t getMaxAllocationSize();

	static size_t sTotalAllocSize;
	static size_t sTotalAllocCount;
	static size_t sTotalUseSize;
	static const AllocatorType ALLOC_TYPE = AT_HLVSTL;
}; 


//class EngineExport CMemoryNedAlloc
//{
//public:
//	static inline void* AllocBytes(size_t count, const char* typeName = "Default")
//    {
//		void* ptr = InternalAllocatorNed::AllocBytes( count );
//		MemInfo* pInfo = ADD_MEM_INFO( ptr, count, typeName, AT_Ned );
//		return ptr;
//    }
//	static inline void deallocBytes(void* ptr)
//	{
//		if (ptr)
//		{
//			REMOVE_MEM_INFO( ptr );
//			nedalloc::nedfree(ptr);
//		}
//	}
//
//	 /// Get the maximum size of a single allocation
//	 static inline size_t getMaxAllocationSize()
//	 {
//		 //不写出这样的原因是 Microsoft定义了min/max两个宏污染到了所以要用括号括起来
//		 //return std::numeric_limits<size_t>::max();
//		 return (std::numeric_limits<size_t>::max)();
//	}
//
//protected:
//private:
//};
//
//#include <stdlib.h>
//
//class EngineExport StdAlloc
//{
//public:
//    static inline void* AllocBytes(size_t count, const char* typeName = "Default")
//    {
//        void* ptr = malloc(count);
//		ADD_MEM_INFO( ptr, count, typeName, AT_Std );
//        return ptr;
//    }
//
//    static inline void deallocBytes(void* ptr)
//    {
//		if( ptr )
//		{
//			REMOVE_MEM_INFO( ptr );
//			free(ptr);
//		}
//    }
//
//    /// Get the maximum size of a single allocation
//    static inline size_t getMaxAllocationSize()
//    {
//        return (std::numeric_limits<size_t>::max)();
//    }
//
//private:
//    // no instantiation
//    StdAlloc()
//    { }
//};









#endif//# __MemoryNedAlloc_H__