#include "stdafx.h"
#include "MemoryNedAlloc.h"
#include "nedmalloc.h"
#include "DebugTools.h"
#include "_Mutex.h"
#include "Timer.h"
#include <assert.h>
#include "ITexture.h"
#include "IGPUProgram.h"
#include "IVertexBuffer.h"
#include "IIndexBuffer.h"
#include "RenderTarget.h"

// 内存来源
enum MemSource
{
	// 未定义
	MS_Unknown							= 0,	
	// 从系统分配得来
	MS_System,		
	// 从内存池中分配得来
	MS_Pool,		
};

//////////////////////////////////////////////////////////////////////////
// 内存管理以及调试的控制开关
//////////////////////////////////////////////////////////////////////////

// 未定义的底层分配类型
#define LLAT_None						0		
// 使用nedmalloc从系统分配内存
#define LLAT_NED						1	
// 使用默认的malloc从系统分配内存
#define LLAT_STD						2			
#define TARGET_LLAT						LLAT_NED

// 是否打开内存分析功能
#define ENABLE_MEMORY_PROFILER			1
// 是否打开深度剖析
#define ENABLE_DEEP_PROFILE				1
// 是否打开STL的深度剖析
#define ENABLE_STL_DEEP_PROFILE			ENABLE_DEEP_PROFILE
// 是否使用内存池
#define USE_MEMORYPOOL					0
// 是否使用内存管理器
#define USE_MEMORYMANAGER				0
// 是否打开内存管理器的使用收集
#define ENABLE_MEM_MGR_COLLECT			1
// 是否打开内存池的自动释放
#define ENABLE_MEMPOOL_AUTO_RELEASE		1

//////////////////////////////////////////////////////////////////////////
// 一些常量
//////////////////////////////////////////////////////////////////////////

// 是否打开内存池打log调试
#define ENABLE_MEMPOOL_DBG				0
// 调试内存字符串最大长度
#define DBG_MEM_NAME_LENGTH				128
// 是否使用固定堆栈层数
#define USE_FIX_CALLSTACK_COUNT			0
// 对字符串记录是否使用定长字符数组
#define USE_FIX_NAME_LEN				0
// 是否强制STL使用系统内存分配
#define STL_USE_LL_ALLOC				0
// 内存分析器是否使用系统内存分配
#define PROFILER_USE_LL_ALLOC			0


#if (AIR_TARGET_PLATFORM == AIR_PLATFORM_WIN32) && (TARGET_LLAT == LLAT_NED)
typedef InternalAllocatorNed			LowLevelAllocator;
#else
typedef InternalAllocatorStd			LowLevelAllocator;
#endif;


//////////////////////////////////////////////////////////////////////////
#define AUTO_LOCK(lock)					_Mutex_Help autoLock(&lock)

void dbg_print( char* buf, ... )
{
	va_list   args;
	int       nBuf;
	char     szBuffer[4096] = { 0 };
	va_start(args, buf);
	nBuf = vsnprintf( szBuffer, sizeof(szBuffer)*sizeof(char), buf, args );
	assert( nBuf > 0 );
	va_end(args);

#if AIR_TARGET_PLATFORM == AIR_PLATFORM_WIN32
	::OutputDebugStringA( szBuffer );
#elif AIR_TARGET_PLATFORM == AIR_PLATFORM_ANDROID
	DroidLogD( "%s", szBuffer );
#else
	std::cout<<szBuffer<<"\n";
#endif
}

void MemProfilerClearLog(const char* szLogPath)
{
	std::ofstream fout;
	fout.open( szLogPath, std::ios::out );
	fout<<std::flush;
	fout.close();
}

void MemProfilerOutPutToLog( const char* szLogPath, char* buf, ... )
{
	va_list   args;
	int       nBuf;
	char     szBuffer[4096] = { 0 };
	va_start(args, buf);
	nBuf = vsnprintf( szBuffer, sizeof(szBuffer)*sizeof(char), buf, args );
	assert( nBuf > 0 );
	va_end(args);
	std::ofstream fout;
	// 打开文件
	fout.open( szLogPath, std::ios::app );
	// 写入文件
	fout<<szBuffer<<"\n";		
	// 回写缓冲区（回写缓冲区会在保持文件打开的情况下保存文件）
	fout<<std::flush;	
	// 关闭	
	fout.close();	
}

std::string GetFormatMemSizeStr( size_t dataSize )
{
	const size_t KS = 1024 * 1024;
	const size_t MS = KS * 1024;

	char buf[128] = {0};
	if( dataSize < 1024 )
		sprintf( buf, "%d(byte)", dataSize );
	else if( dataSize < KS )
		sprintf( buf, "%.3f(KB)", dataSize / 1024.0f );
	else if( dataSize < MS )
		sprintf( buf, "%.3f(MB)", dataSize / 1024.0f / 1024.0f );
	else 
		sprintf( buf, "%.3f(GB)", dataSize / 1024.0f / 1024.0f / 1024.0f );	
	return std::string(buf);
}

//////////////////////////////////////////////////////////////////////////

template< typename T >
class ProfilerSTLAllocator
{
public:
	typedef size_t    size_type;
	typedef ptrdiff_t difference_type;
	typedef T*        pointer;
	typedef const T*  const_pointer;
	typedef T&        reference;
	typedef const T&  const_reference;
	typedef T         value_type;

	ProfilerSTLAllocator() throw()										{}
	ProfilerSTLAllocator( ProfilerSTLAllocator const&) throw()			{}
	template<typename U>
	ProfilerSTLAllocator( ProfilerSTLAllocator<U> const&) throw()		{}
	template<typename U, typename P>
	ProfilerSTLAllocator( ProfilerSTLAllocator<U> const&) throw()		{}
	~ProfilerSTLAllocator() throw()										{}
	template <class U>
	struct rebind														{	typedef ProfilerSTLAllocator<U> other;	};
	pointer address(reference value) const								{	return &value;	}
	const_pointer address(const_reference value) const					{	return &value;	}
	size_type max_size() const throw()									{	const size_t maxsize = (std::numeric_limits<size_t>::max)(); return maxsize / sizeof(T);	}
    pointer allocate(size_type nNum)									{	pointer p  = static_cast<pointer>(MemoryProfiler::AllocBytes( nNum * sizeof( T ) ));		return p;	}
	void construct (pointer p, const T& value) 							{	new( static_cast<void*>( p ) )T(value);	}
	void destroy (pointer p)											{	p->~T();	}
	void deallocate (pointer p, size_type num)							{	MemoryProfiler::deallocBytes(p);	}
	inline bool operator != (const ProfilerSTLAllocator<T>& other) const{	return false; }
};

/// determine equality, can memory from another allocator
/// be released by this allocator, (ISO C++)
template<typename T, typename T2>
inline bool operator==(ProfilerSTLAllocator<T> const&, ProfilerSTLAllocator<T2> const&)		{	return true; }
/// determine equality, can memory from another allocator
/// be released by this allocator, (ISO C++)
template<typename T, typename P, typename OtherAllocator>
inline bool operator==(ProfilerSTLAllocator<T> const&, OtherAllocator const&)				{	return false; }
/// determine equality, can memory from another allocator
/// be released by this allocator, (ISO C++)
template<typename T, typename T2, typename P>
inline bool operator!=(ProfilerSTLAllocator<T> const&, ProfilerSTLAllocator<T2> const&)		{	return false; }
/// determine equality, can memory from another allocator
/// be released by this allocator, (ISO C++)
template<typename T, typename P, typename OtherAllocator>
inline bool operator!=(ProfilerSTLAllocator<T> const&, OtherAllocator const&)				{	return true; }


#define STRCPY(dst, src)										\
{																\
		size_t len = strlen(src);								\
		dst = (char*)MemoryProfiler::AllocBytes( len + 1 );		\
		memcpy( dst, src, len );								\
		dst[len] = '\0';										\
}

enum GraphicDataType
{
	GDT_Unknown = 0,
	GDT_VertexBuffer,
	GDT_IndexBuffer,
	GDT_Texture,
	GDT_RenderTarget,
	GDT_Shader,
};
const char* C_GRAPHIC_TYPE_STR[] = 
{ 
	"Unknown", 
	"VertexBuffer", 
	"IndexBuffer", 
	"Texture", 
	"RenderTarget", 
	"Shader", 
};
const char* GetRenderPixelFormatStr(RendererPixelFormat pf)
{
	switch( pf )
	{
	case RPF_A8R8G8B8:		return "A8R8G8B8";
	case RPF_X8R8G8B8:		return "X8R8G8B8";
	case RPF_D24S8:			return "D24S8"; 
	case RPF_L8:			return "L8";
	case RPF_L16:			return "L16";
	case RPF_R8G8B8:		return "R8G8B8";
	case RPF_R16F:			return "R16F";
	case RPF_A16B16G16R16F: return "A16B16G16R16F";
	case RPF_R32F:			return "R32F";
	case RPF_G32R32F:		return "G32R32F";
	case RPF_A32B32G32R32F: return "A32B32G32R32F";
	case RPF_D32F:			return "D32F";
	case RPF_A8L8:			return "A8L8";
	case RPF_R24X8:			return "R24X8";
	case RPF_ALPHA:			return "ALPHA";
	}
	return "Unknown";
}
struct GraphicProfileData
{
	GraphicProfileData	*prevData;
	GraphicProfileData	*nextData;
	
	void				*objPtr;
	size_t				dataSize;
	char				*funcName;
	char				*fileName;
	int					line;
	void				**callStack;
	size_t				callStackCount;
	void Init()
	{
		memset( this, 0, sizeof(GraphicProfileData) );
	}
	void Destroy()
	{
		MemoryProfiler::deallocBytes( fileName );
		MemoryProfiler::deallocBytes( funcName );
		MemoryProfiler::deallocBytes( callStack );
		callStack = NULL;
		callStackCount = 0;
	}
};
struct GraphicProfileDataList
{
	GraphicProfileDataList	*prevList;
	GraphicProfileDataList	*nextList;
	GraphicProfileData		*firstData;
	GraphicDataType			dataType;
	size_t					objCount;
	size_t					totalDataSize;

	void Init()
	{
		memset( this, 0, sizeof(GraphicProfileDataList) );
	}
	void Destroy()
	{
		GraphicProfileData* curData = firstData;
		while ( curData )
		{
			GraphicProfileData* nextData = curData->nextData;
			curData->Destroy();
			MemoryProfiler::deallocBytes( curData );
			curData = nextData;
		}
		firstData = NULL;
		objCount = 0;
		totalDataSize = 0;
	}
	GraphicProfileData* AddData( void* ptr, size_t dataSize, const char* funcName, const char* fileName, int line, void* callStack[DBG_MEM_CALLSTACK_NUM], int callStackCount )
	{
		objCount++;
		totalDataSize += dataSize;
		GraphicProfileData* newData = (GraphicProfileData*)MemoryProfiler::AllocBytes( sizeof(GraphicProfileData) );
		newData->Init();
		newData->objPtr = ptr;
		newData->dataSize = dataSize;
		newData->line = line;
		STRCPY( newData->fileName, fileName );
		STRCPY( newData->funcName, funcName );
		size_t callStackSize = sizeof(void*) * callStackCount;
		newData->callStack = (void**)MemoryProfiler::AllocBytes( callStackSize );
		memcpy( newData->callStack, &callStack[0], callStackSize );
		newData->callStackCount = callStackCount;

		GraphicProfileData* curData = firstData;
		while ( curData && curData->dataSize > dataSize )
		{
			if( curData->nextData == NULL )
			{
				newData->prevData = curData;
				curData->nextData = newData;
				return newData;
			}
			curData = curData->nextData;
		}
		if( curData == NULL )
			firstData = newData;
		else
		{
			if( curData->prevData == NULL )
			{
				newData->nextData = curData;
				curData->prevData = newData;
				firstData = newData;
			}
			else
			{
				GraphicProfileData* parData = curData->prevData;
				parData->nextData = newData;
				newData->prevData = parData;
				newData->nextData = curData;
				curData->prevData = newData;
			}
		}
		return newData;
	}
	bool RemoveData(void* ptr, size_t &totalGraphicSize)
	{
		GraphicProfileData* curData = firstData;
		while( curData )
		{
			if( curData->objPtr == ptr )
			{
				objCount--;
				totalDataSize -= curData->dataSize;
				totalGraphicSize -= curData->dataSize;
				GraphicProfileData* parData = curData->prevData;
				GraphicProfileData* childData = curData->nextData;
				if( parData && childData )
				{
					parData->nextData = childData;
					childData->prevData = parData;
				}
				else if( !parData && childData )
				{
					childData->prevData = NULL;
					firstData = childData;
				}
				else if( parData && !childData )
				{
					parData->nextData = NULL;
				}
				else if( !parData && !childData )
				{
					firstData = NULL;
				}
				curData->Destroy();
				MemoryProfiler::deallocBytes( curData );
				return true;
			}
			else
				curData = curData->nextData;
		}
		return false;
	}
};
struct GraphicPrifilerDataContainer
{
	GraphicProfileDataList	*firstDataLst;
	size_t					totalDataSize;
	GraphicPrifilerDataContainer()
	{
		Init();
	}
	~GraphicPrifilerDataContainer()
	{
		Destroy();
	}
	void Init()
	{
		memset( this, 0, sizeof(GraphicPrifilerDataContainer));
	}
	void Destroy()
	{
		GraphicProfileDataList* curLst = firstDataLst;
		while( curLst )
		{
			GraphicProfileDataList* nextLst = curLst->nextList;
			curLst->Destroy();
			MemoryProfiler::deallocBytes( curLst );
			curLst = nextLst;
		}
		firstDataLst = NULL;
		totalDataSize = 0;
	}
	GraphicProfileData* AddData( GraphicDataType dataType, void* ptr, size_t dataSize, const char* funcName, const char* fileName, int line, void* callStack[DBG_MEM_CALLSTACK_NUM], int callStackCount)
	{
		GraphicProfileDataList* curLst = firstDataLst;
		GraphicProfileDataList* dstLst = NULL;
		if( firstDataLst == NULL )
		{
			dstLst = (GraphicProfileDataList*)MemoryProfiler::AllocBytes( sizeof(GraphicProfileDataList) );
			dstLst->Init();
			dstLst->dataType = dataType;
			firstDataLst = dstLst;
		}
		else
		{
			while( curLst )
			{
				if( curLst->dataType == dataType )
				{
					dstLst = curLst;
					break;
				}
				if( curLst->nextList == NULL )
				{
					dstLst = (GraphicProfileDataList*)MemoryProfiler::AllocBytes( sizeof(GraphicProfileDataList) );
					dstLst->Init();
					dstLst->dataType = dataType;
					dstLst->prevList = curLst;
					curLst->nextList = dstLst;
					break;
				}
				curLst = curLst->nextList;
			}
		}
		assert( dstLst );
		GraphicProfileData* pData = dstLst->AddData( ptr, dataSize, funcName, fileName, line, callStack, callStackCount );
		assert( pData );
		totalDataSize += dataSize;
		return pData;
	}
	bool RemoveData(  GraphicDataType dataType, void* ptr )
	{
		GraphicProfileDataList* curLst = firstDataLst;
		while( curLst )
		{
			if( curLst->dataType == dataType )
			{
				return curLst->RemoveData( ptr, totalDataSize );
			}
			curLst = curLst->nextList;
		}
		//assert( false && "not find GraphicProfileData" );
		return false;
	}
	void PrintDataDetail( const char* memLogFile, GraphicDataType dataType, GraphicProfileData *pdata )
	{
		const char* C_INDEXTYPE_STR[] = { "16BIT", "32BIT", };
		const char* C_TEXTYPE_STR[] = { "Unknown", "2DTexture", "VolumeTexture", "CubeTexture", "Unknown", };
		switch( dataType )
		{
		case GDT_VertexBuffer:
			{
				IVertexBuffer* pVB = (IVertexBuffer*)pdata->objPtr;
				MemProfilerOutPutToLog( memLogFile, "\t\t\t\t<Detail vertexCount=\"%d\" sizePerVertex=\"%d(byte)\"/>", 
					pVB->GetVertexCount(), pVB->GetUnitSize() );
			}
			break;
		case GDT_IndexBuffer:
			{				
				IIndexBuffer* pIB = (IIndexBuffer*)pdata->objPtr;
				MemProfilerOutPutToLog( memLogFile, "\t\t\t\t<Detail indexCount=\"%d\" indexType=\"%s\"/>", 
					pIB->GetTotalIndex(), pIB->GetIndexType() == IT_16BIT ? C_INDEXTYPE_STR[0] : C_INDEXTYPE_STR[1]  );
			}
			break;
		case GDT_Texture:
			{				
				ITexture* pTex = (ITexture*)pdata->objPtr;
				MemProfilerOutPutToLog( memLogFile, "\t\t\t\t<Detail texType=\"%s\" texFile=\"%s\" width=\"%d\" height=\"%d\" depth=\"%d\" pixelFormat=\"%s\"/>", 
					C_TEXTYPE_STR[pTex->GetTextureType()], pTex->GetFileName().c_str(), pTex->GetWidth(), 
					pTex->GetHeight(), pTex->GetDepth(), GetRenderPixelFormatStr(pTex->GetPixelFormat()) );
			}
			break;
		case GDT_RenderTarget:
			{
				TextureRenderTarget* pTex = (TextureRenderTarget*)pdata->objPtr;
				//MemProfilerOutPutToLog( memLogFile, "\t\t\t\t<Detail texType=\"%s\" width=\"%d\" height=\"%d\" depth=\"%d\" pixelFormat=\"%s\"/>", 
				//	C_TEXTYPE_STR[pTex->GetTextureType()], pTex->GetWidth(), pTex->GetHeight(), pTex->GetDepth(), GetRenderPixelFormatStr(pTex->GetPixelFormat()) );
			}
			break;
		case GDT_Shader:
			{
				IGPUProgram* pGrg = (IGPUProgram*)pdata->objPtr;
				MemProfilerOutPutToLog( memLogFile, "\t\t\t\t<Detail shaderFile=\"%s\" shaderType=\"%s\" shaderVersion=\"%s\" entryFunc=\"%s\"/>", 
					pGrg->GetFileName().c_str(), pGrg->GetShaderTypeStr(), pGrg->GetVersion(), pGrg->GetEntryFunction() );
			}
			break;
		}
	}
	void SnapMemShot( const char* memLogFile, bool printCallStack )
	{		
		MemProfilerOutPutToLog( memLogFile, "\t<Graphic totalMemUsage = \"%s\">", GetFormatMemSizeStr(totalDataSize).c_str() );
		GraphicProfileDataList* curLst = firstDataLst;
		while( curLst )
		{
			MemProfilerOutPutToLog( memLogFile, "\t\t<Type name=\"%s\" totalMemUsage=\"%s\" objCount=\"%d\">", C_GRAPHIC_TYPE_STR[curLst->dataType], GetFormatMemSizeStr(curLst->totalDataSize).c_str(), curLst->objCount );
			GraphicProfileData* curData = curLst->firstData;
			while( curData )
			{
				MemProfilerOutPutToLog( memLogFile, "\t\t\t<Obj dataSize=\%s\" funcName=\"%s\" fileName=\"%s\" line=\"%d\">",  GetFormatMemSizeStr(curData->dataSize).c_str(), curData->funcName, curData->fileName, curData->line );
				PrintDataDetail( memLogFile, curLst->dataType, curData );
				if( printCallStack )
				{
#	if AIR_TARGET_PLATFORM == AIR_PLATFORM_WIN32
					MemProfilerOutPutToLog( memLogFile, "\t\t\t\t<CallStacks>" );
					size_t callStackNum = curData->callStackCount;
					for ( int i = 0; i < callStackNum; ++i )
					{
						char file[1024] = {0};
						int line = 0;
						if(curData->callStack[i])
						{
							bool res = MemoryProfiler::GetFileAndLine( curData->callStack[i], file, line );
							if( res )
								MemProfilerOutPutToLog( memLogFile, "\t\t\t\t\t<StackFrame id = \"%d\" ptr = \"%p\" line = \"%d\", file = \"%s\"/>", 
								i, curData->callStack[i], line, file);
							else
								MemProfilerOutPutToLog( memLogFile, "\t\t\t\t\t<StackFrame id = \"%d\" ptr = \"%p\"/>", 
								i, curData->callStack[i]);
						}
						else
							break;
					}
					MemProfilerOutPutToLog( memLogFile, "\t\t\t\t</CallStacks>" );
#	endif
				}
				MemProfilerOutPutToLog( memLogFile, "\t\t\t</Obj>" );
				curData = curData->nextData;
			}
			MemProfilerOutPutToLog( memLogFile, "\t\t</Type>" );
			curLst = curLst->nextList;
		}
		MemProfilerOutPutToLog( memLogFile, "\t</Graphic>" );
	}
};
GraphicPrifilerDataContainer	g_GraphicPrifilerDataContainer;
_Mutex_Body_					g_GraphicProfilerLock;
#define GPU_AUTO_LOCK			//AUTO_LOCK(g_GraphicProfilerLock)

bool GraphicProfiler::OnCreateVB( void* ptr, size_t dataSize, const char* funcName, const char* fileName, int line)
{
	GPU_AUTO_LOCK;
#if AIR_TARGET_PLATFORM == AIR_PLATFORM_WIN32
	#include "CallStack.h"
#else
	size_t validIndexCount = 0;
	void* callStack[DBG_MEM_CALLSTACK_NUM];
#endif
	GraphicProfileData* pData = g_GraphicPrifilerDataContainer.AddData(GDT_VertexBuffer, ptr, dataSize, funcName, fileName, line, callStack, validIndexCount);
	return pData != NULL;
}
bool GraphicProfiler::OnReleaseVB( void* ptr )
{
	GPU_AUTO_LOCK;
	return g_GraphicPrifilerDataContainer.RemoveData( GDT_VertexBuffer, ptr );
}

bool GraphicProfiler::OnCreateIB( void* ptr, size_t dataSize, const char* funcName, const char* fileName, int line)
{
	GPU_AUTO_LOCK;
#if AIR_TARGET_PLATFORM == AIR_PLATFORM_WIN32
	#include "CallStack.h"
#else
	size_t validIndexCount = 0;
	void* callStack[DBG_MEM_CALLSTACK_NUM];
#endif
	GraphicProfileData* pData = g_GraphicPrifilerDataContainer.AddData(GDT_IndexBuffer, ptr, dataSize, funcName, fileName, line, callStack, validIndexCount);
	return pData != NULL;
}
bool GraphicProfiler::OnReleaseIB( void* ptr )
{
	GPU_AUTO_LOCK;
	return g_GraphicPrifilerDataContainer.RemoveData( GDT_IndexBuffer, ptr );
}

bool GraphicProfiler::OnCreateTexture( void* ptr, size_t dataSize, const char* funcName, const char* fileName, int line)
{
	GPU_AUTO_LOCK;
#if AIR_TARGET_PLATFORM == AIR_PLATFORM_WIN32
	#include "CallStack.h"
#else
	size_t validIndexCount = 0;
	void* callStack[DBG_MEM_CALLSTACK_NUM];
#endif
	GraphicProfileData* pData = g_GraphicPrifilerDataContainer.AddData(GDT_Texture, ptr, dataSize, funcName, fileName, line, callStack, validIndexCount);
	return pData != NULL;
}
bool GraphicProfiler::OnReleaseTexture( void* ptr )
{
	GPU_AUTO_LOCK;
	return g_GraphicPrifilerDataContainer.RemoveData( GDT_Texture, ptr );
}

bool GraphicProfiler::OnCreateRenderTarget( void* ptr, size_t dataSize, const char* funcName, const char* fileName, int line)
{
	GPU_AUTO_LOCK;
#if AIR_TARGET_PLATFORM == AIR_PLATFORM_WIN32
	#include "CallStack.h"
#else
	size_t validIndexCount = 0;
	void* callStack[DBG_MEM_CALLSTACK_NUM];
#endif
	GraphicProfileData* pData = g_GraphicPrifilerDataContainer.AddData(GDT_RenderTarget, ptr, dataSize, funcName, fileName, line, callStack, validIndexCount);
	return pData != NULL;
}
bool GraphicProfiler::OnReleaseRenderTarget( void* ptr )
{
	GPU_AUTO_LOCK;
	return g_GraphicPrifilerDataContainer.RemoveData( GDT_RenderTarget, ptr );
}

bool GraphicProfiler::OnCreateShader( void* ptr, size_t dataSize, const char* funcName, const char* fileName, int line )
{
	GPU_AUTO_LOCK;
#if AIR_TARGET_PLATFORM == AIR_PLATFORM_WIN32
#include "CallStack.h"
#else
	size_t validIndexCount = 0;
	void* callStack[DBG_MEM_CALLSTACK_NUM];
#endif
	GraphicProfileData* pData = g_GraphicPrifilerDataContainer.AddData(GDT_Shader, ptr, dataSize, funcName, fileName, line, callStack, validIndexCount);
	return pData != NULL;
}
bool GraphicProfiler::OnReleaseShader( void* ptr )
{
	GPU_AUTO_LOCK;
	return g_GraphicPrifilerDataContainer.RemoveData( GDT_Shader, ptr );
}

//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
namespace AirMemoryPool
{

#define MEMPOOL_SAT(EXP)		EXP
#define MEMPOOL_HAS_SAT
#define UNUSED_ARG(x)			(x)


	// 每个内存池是65536个byte
	// 池里的每个块最大有MAX_POOL_BLOCK_SIZE个字节
	namespace
	{
		enum
		{
			MAX_POOL_BLOCK_SIZE = (65536-4)/2+1
		};
	};

	const size_t MAX_POOL_COUNT = ((MAX_POOL_BLOCK_SIZE-1)+15)/16+1;

	class MemoryPool;

	// 内存池使用状态
	enum PoolStatus
	{
		// 可用于分配
		ePS_CanUse,
		// 耗尽,不可分配了
		ePS_Exausted,
		// 当前池子不在使用状态,可以销毁之,把内存交还给系统
		ePS_NeedDestroy,
	};

	// 内存池信息
	struct PoolInfo
	{
		// 一级索引
		size_t			mFirstLevel;
		// 二级索引
		size_t			mSecondLevel;
		// 子索引
		size_t			mSubID;
		// 当前池子里有多少个子块被分配出去了
		size_t			mBlockTakenCount;
		// 内存池子块的总数
		size_t			mTotalBlockCount;
		// 整个内存池的起始地址
		void			*mRealPoolStartPtr;
		// 标记各个子块的使用情况,有mTotalBlockCount个字节,每个字节对应一个内存子块,0表示可用,1表示被占用
		char			*mFlags;
		// 内存池对象
		MemoryPool		*mpMemPool;
		// 每个块的大小(byte)
		size_t			mSizePerBlock;
		// 整个池的大小(byte)
		size_t			mPoolTotalSize;
		// 池是否已经耗尽
		bool			mIsExausted;
		// 整块内存的起始地址值
		unsigned long	mPtrStartValue;
		// 整块内存的结束地址值
		unsigned long	mPtrEndValue;
		// 上一个内存池子
		PoolInfo		*mPrevPool;
		// 下一个内存池子
		PoolInfo		*mNextPool;

		PoolInfo() : mBlockTakenCount(0), mRealPoolStartPtr(0), mpMemPool(0), mTotalBlockCount(0), mSizePerBlock(0), mPoolTotalSize(0), mIsExausted(false)
		{
			mFirstLevel = -1;
			mSecondLevel = -1;
			mSubID = -1;
			mPtrStartValue = -1;
			mPtrEndValue = -1;
			mPrevPool = mNextPool = NULL;
			mFlags = NULL;
		}
		void				Init()
		{
			mBlockTakenCount = 0;
			mRealPoolStartPtr = 0;
			mpMemPool = 0;
			mTotalBlockCount = 0;
			mSizePerBlock = 0;
			mPoolTotalSize = 0;
			mIsExausted = false;
			mFirstLevel = -1;
			mSecondLevel = -1;
			mSubID = -1;
			mPtrStartValue = -1;
			mPtrEndValue = -1;
			mPrevPool = mNextPool = NULL;
			mFlags = NULL;
		}
		void				Clear();
	};

	// 2级内存池查找表
	// 假设地址是 0xAABBCCDD, 该级索引对应的是 BB 的值
	struct PoolSecondLevelSearchMap
	{
		PoolInfo			*secondLevelSearchMap[256][3];

		PoolSecondLevelSearchMap();
		~PoolSecondLevelSearchMap();
		void				Init();
		void				Clear();
		bool				DestroyPoolInfo( PoolInfo* pPool );
		PoolInfo*			GetPoolInfo( int secLvId, int subId )
		{
			assert( secLvId >= 0 && secLvId < 256 && subId >= 0 && subId < 3 );
			return secondLevelSearchMap[secLvId][subId];
		}
		void				SetPoolInfo( int secLvId, int subId, PoolInfo* pInfo )
		{
			assert( secLvId >= 0 && secLvId < 256 && subId >= 0 && subId < 3 );
			secondLevelSearchMap[secLvId][subId] = pInfo;
		}
	};

	// 1级内存池查找表
	// 假设地址是 0xAABBCCDD, 该级索引对应的是 AA 的值
	struct PoolFirstLevelSearchMap
	{
	public:
		PoolSecondLevelSearchMap		*firstLevelSearchMap[256];	

		PoolFirstLevelSearchMap();
		~PoolFirstLevelSearchMap();
		void				Init();
		void				Clear();
		PoolInfo*			AddPoolInfo( void* ptr );
		PoolInfo*			FindPoolInfo( void* ptr );
		bool				DestroyPoolInfo( PoolInfo* pPool );
	};

	// 内存池
	class MemoryPool
	{
	public:
		MemoryPool(size_t BlockSize);
		void					Init(size_t BlockSize);
		// 更新内存池子使用状态
		void					MarkPoolUsefulOrExausted( PoolInfo* pInfo, PoolStatus status );
		// 分配内存
		void*					Alloc();

		static void				InitMemoryPoolSearchMap();
		static void				ReleaseMemoryPoolSearchMap();
		static PoolFirstLevelSearchMap&	GetMemoryPoolSearchMap();
		static size_t			GetMemSize(void *p);
		static bool				Free(void *p);

	public:
		inline size_t			GetBlockSize() const 
		{ 
			return blocksize;
		}

		inline size_t			GetTotalUseSize() const
		{
			return usesize;
		}

		inline size_t			GetTotalAllocSize() const
		{
			return allocsize;
		}

		inline PoolInfo*		GetFirstPool() const
		{
			return mFirstPool;
		}

	private:
		void					_InnerFree(void *p, PoolInfo *pool);

	public:
		// 内存池索引
		size_t					poolIndex;
		// 内存池中每个子块的大小
		size_t					blocksize;
		// 可用于分配的内存池数量
		size_t					freePoolCount;
		// 首个子内存池
		PoolInfo				*mFirstPool;
		// 是否已经初始化内存池查找表
		static bool				sHasInitPoolSearchMap;
		// 内存池查找表
		static PoolFirstLevelSearchMap* sPoolSearchMap;
		// 内存池使用量
		size_t					usesize;
		// 内存池分配量
		size_t					allocsize;
	};

	// 系统内存池管理器
	class SystemMemoryPools
	{
		MemoryPool				*m_Pools[MAX_POOL_COUNT];

	public:
		SystemMemoryPools();
		~SystemMemoryPools();
		// 根据大小获取内存池
		MemoryPool*				GetPool(size_t blocksize);
		// 分配内存
		void*					Alloc( size_t blocksize, MemSource &ms );
		// 释放内存
		void					Free( void* ptr, MemSource &msType );
		// 把内存池信息打印到log文件里
		void					SnapMemoryPool( const char* szLogPath );
		static SystemMemoryPools& GetSystemPools();
	};

	MemoryPool*			GetMemoryPool(size_t blocksize);
	// 获取内存池分配总量
	size_t      		GetMemoryPoolAllocSize();
	// 获取内存池使用总量
	size_t      		GetMemoryPoolUseSize();
	// 获取内存池内部内存消耗
	size_t				GetMemoryPoolInnerAllocSize();

	// 内存池向系统申请的内存总量
	static size_t g_MemoryPoolSystemAllocSize = 0;
	// 内存池被使用的总量
	static size_t g_MemoryPoolUseSize = 0;
	// 内存池自身消耗的系统内存总量
	static size_t g_MemoryPoolInnerSystemAllocSize = 0;
};
//////////////////////////////////////////////////////////////////////////


_Mutex_Body_					g_ProfilerAllocLock;
_Mutex_Body_					g_HLVEngineAllocLock;
_Mutex_Body_					g_HLVSTLAllocLock;
_Mutex_Body_					g_MemAllocLock;



// 记录每次内存分配的信息
struct EngineExport AllocationData
{
	MemSource		memSourceType;						// 内存来源
	AllocatorType	allocatorType;						// 在哪个内存分配器分配的
	void*			dataPtr;							// 分配得到的指针
	unsigned int	dataSize;							// 分配的数据大小
#if USE_FIX_NAME_LEN
	char			typeName[DBG_MEM_NAME_LENGTH];		// 分配的类型标识(譬如类名)
	char			fileName[DBG_MEM_NAME_LENGTH];		// 在哪个文件发生本次分配
	char			funcName[DBG_MEM_NAME_LENGTH];		// 在哪个函数发生本次分配
#else
	char			*typeName;							// 分配的类型标识(譬如类名)
	char			*fileName;							// 在哪个文件发生本次分配
	char			*funcName;							// 在哪个函数发生本次分配
#endif
	unsigned short	line;								// 在文件的那一行发生本次分配
#if USE_FIX_CALLSTACK_COUNT
	void*			callStack[DBG_MEM_CALLSTACK_NUM];	// 分配的时候的堆栈追溯
#else
	void			**callStack;
	size_t			callStackCount;
#endif

	AllocationData();
	void Init();
	void Destroy();
};

// 内存分配查找节点
struct EngineExport AllocationSearchNode
{
	// 第几层
	unsigned char			levelIndex;
	// 父层
	AllocationSearchNode*   parentLevel;
	// 子层
	AllocationSearchNode*	childLevel[2];
	// 内存分配信息
	AllocationData*			value;

	AllocationSearchNode();
	void Init();
	static void OnDestroyChild( AllocationSearchNode* childLvl );
};


// 内存管理分配器
class MemoryManagerAllocator
{
public:
	static inline void* AllocBytes( size_t size )
	{
#if ENABLE_MEM_MGR_COLLECT
		size_t realAllocSize = sizeof(size_t) + size;
		size_t* ptr = (size_t*)LowLevelAllocator::AllocBytes( realAllocSize );
		assert( ptr );
		*ptr = realAllocSize;
		sTotalAllocSize += realAllocSize;
		++ptr;
		return ptr;
#else
		void* ptr = LowLevelAllocator::AllocBytes( size );
		return ptr;
#endif
	}
	static inline void deallocBytes(void* ptr)
	{
		if (ptr)
		{
#if ENABLE_MEM_MGR_COLLECT
			size_t* oriPtr = (size_t*)ptr;
			--oriPtr;
			size_t realAllocSize = *oriPtr;
			sTotalAllocSize -= realAllocSize;
			LowLevelAllocator::deallocBytes( oriPtr );
#else
			LowLevelAllocator::deallocBytes( ptr );
#endif
		}
	}
	static size_t sTotalAllocSize;
};
size_t MemoryManagerAllocator::sTotalAllocSize = 0;



static inline void* MemoryAllocBytes( size_t dataSize, MemSource &msType )
{
	//AUTO_LOCK( g_MemAllocLock );
#if USE_MEMORYMANAGER
#	if USE_MEMORYPOOL
		void* ptr = AirMemoryPool::SystemMemoryPools::GetSystemPools().Alloc( dataSize, msType );
#	else
		void* ptr = MemoryManagerAllocator::AllocBytes( dataSize );
		msType = MS_System;
#	endif
#else
		void* ptr = LowLevelAllocator::AllocBytes( dataSize );
		msType = MS_System;
#endif
	assert( ptr );
	return ptr;
}

static inline void MemoryDeallocBytes( void* ptr, MemSource &msType )
{
	//AUTO_LOCK( g_MemAllocLock );
	if( ptr )
	{
#if USE_MEMORYMANAGER
#	if USE_MEMORYPOOL
		AirMemoryPool::SystemMemoryPools::GetSystemPools().Free( ptr, msType );
#	else
		MemoryManagerAllocator::deallocBytes( ptr );
		msType = MS_System;
#	endif
#else
		LowLevelAllocator::deallocBytes( ptr );
		msType = MS_System;
#endif
	}
}



//////////////////////////////////////////////////////////////////////////

#if AIR_TARGET_PLATFORM == AIR_PLATFORM_WIN32
	#include <DbgHelp.h>
	#include <tchar.h>

	typedef BOOL
		(WINAPI
		*tFMiniDumpWriteDump)
		(
		IN HANDLE hProcess,
		IN DWORD ProcessId,
		IN HANDLE hFile,
		IN MINIDUMP_TYPE DumpType,
		IN CONST PMINIDUMP_EXCEPTION_INFORMATION ExceptionParam, OPTIONAL
		IN CONST PMINIDUMP_USER_STREAM_INFORMATION UserStreamParam, OPTIONAL
		IN CONST PMINIDUMP_CALLBACK_INFORMATION CallbackParam OPTIONAL
		);

	typedef BOOL
		(WINAPI
		*tFSymInitialize)
		(
		IN HANDLE   hProcess,
		IN PSTR     UserSearchPath,
		IN BOOL     fInvadeProcess
		);

	typedef BOOL
		(WINAPI
		*tFStackWalk64)
		(
		DWORD                             MachineType,
		HANDLE                            hProcess,
		HANDLE                            hThread,
		LPSTACKFRAME64                    StackFrame,
		PVOID                             ContextRecord,
		PREAD_PROCESS_MEMORY_ROUTINE64    ReadMemoryRoutine,
		PFUNCTION_TABLE_ACCESS_ROUTINE64  FunctionTableAccessRoutine,
		PGET_MODULE_BASE_ROUTINE64        GetModuleBaseRoutine,
		PTRANSLATE_ADDRESS_ROUTINE64      TranslateAddress
		);

	typedef BOOL
		(WINAPI
		*tFSymFromAddr)
		(
		IN  HANDLE              hProcess,
		IN  DWORD64             Address,
		OUT PDWORD64            Displacement,
		IN OUT PSYMBOL_INFO     Symbol
		);

	typedef BOOL
		(WINAPI
		*tFSymGetLineFromAddr64)
		(
		IN  HANDLE                  hProcess,
		IN  DWORD64                 qwAddr,
		OUT PDWORD                  pdwDisplacement,
		OUT PIMAGEHLP_LINE64        Line64
		);

	typedef DWORD
		(WINAPI
		*tFSymGetOptions)
		(
		VOID
		);

	typedef DWORD
		(WINAPI
		*tFSymSetOptions)
		(
		IN DWORD   SymOptions
		);

	typedef PVOID
		(WINAPI
		*tFSymFunctionTableAccess64)
		(
		HANDLE  hProcess,
		DWORD64 AddrBase
		);

	typedef DWORD64
		(WINAPI
		*tFSymGetModuleBase64)
		(
		IN  HANDLE              hProcess,
		IN  DWORD64             qwAddr
		);

	static tFMiniDumpWriteDump fnMiniDumpWriteDump = NULL;
	static tFSymInitialize fnSymInitialize = NULL;
	static tFStackWalk64 fnStackWalk64 = NULL;
	static tFSymFromAddr fnSymFromAddr = NULL;
	static tFSymGetLineFromAddr64 fnSymGetLineFromAddr64 = NULL;
	static tFSymGetOptions fnSymGetOptions = NULL;
	static tFSymSetOptions fnSymSetOptions = NULL;
	static tFSymFunctionTableAccess64 fnSymFunctionTableAccess64 = NULL;
	static tFSymGetModuleBase64 fnSymGetModuleBase64 = NULL;

	static HMODULE s_DbgHelpLib = NULL;
	static HANDLE s_Process = NULL;
#endif

//////////////////////////////////////////////////////////////////////////

AllocationData::AllocationData()
{
	memset( this, 0, sizeof(AllocationData) );
}

void AllocationData::Init()
{
	memset( this, 0, sizeof(AllocationData) );
}

void AllocationData::Destroy()
{
#if USE_FIX_NAME_LEN
#else
	MemoryProfiler::deallocBytes( typeName );
	MemoryProfiler::deallocBytes( fileName );
	MemoryProfiler::deallocBytes( funcName );
#if !USE_FIX_CALLSTACK_COUNT
	MemoryProfiler::deallocBytes( callStack );
	callStack = NULL;
	callStackCount = 0;
#endif
#endif
}

AllocationSearchNode::AllocationSearchNode():levelIndex(-1)
{
	childLevel[0] = childLevel[1] = NULL;
	value = NULL;
}

void AllocationSearchNode::Init()
{
	levelIndex = -1;
	childLevel[0] = childLevel[1] = NULL;
	value = NULL;
	parentLevel = NULL;
}

void AllocationSearchNode::OnDestroyChild( AllocationSearchNode* childLvl )
{
	if( childLvl->childLevel[0] == NULL && childLvl->childLevel[1] == NULL && childLvl->value == NULL )
	{
		AllocationSearchNode* parLvl = childLvl->parentLevel;
		if( parLvl )
		{
			if( parLvl->childLevel[0] == childLvl )
				parLvl->childLevel[0] = NULL;
			else if( parLvl->childLevel[1] == childLvl )
				parLvl->childLevel[1] = NULL;
		}

		MemoryProfiler::deallocBytes( childLvl );
		if( parLvl )
			OnDestroyChild( parLvl );
	}
}


//////////////////////////////////////////////////////////////////////////

unsigned long MemoryProfiler::sProfilerTotalAllocBytes = 0;
AllocationSearchNode* MemoryProfiler::sRootInfoLevel = NULL;

// 分析器占用的系统内存
size_t g_ProfilerSystemAllocSize = 0;
// 引擎占用的系统内存
size_t g_EngineSystemAllocSize = 0;
// STL占用的系统内存
size_t g_STLSystemAllocSize = 0;

#if AIR_ARCH_TYPE == AIR_ARCHITECTURE_32
#define BIT_OFFSET 31
#elif AIR_ARCH_TYPE == AIR_ARCHITECTURE_64
#define BIT_OFFSET 63
#else
#define BIT_OFFSET 31
#endif



bool MemoryProfiler::Initialize()
{
	return InitDbgHelpLib();
}

bool MemoryProfiler::Destroy()
{
	FreeDbgHelpLib();
	return true;
}

AllocationData* MemoryProfiler::RecMemInfo( void* dataPtr, size_t dataSize, const char* typeName, const char* fileName, const size_t line, const char* funcName, AllocatorType at )
{
	AllocationData* pInfo = MemoryProfiler::AddMemInfo( dataPtr, dataSize, typeName, fileName, line, funcName, at );
#if ENABLE_DEEP_PROFILE
#if AIR_TARGET_PLATFORM == AIR_PLATFORM_WIN32	
#include "CallStack.h"							
#	if !USE_FIX_CALLSTACK_COUNT
	pInfo->callStack = (void**)MemoryProfiler::AllocBytes( sizeof(void*) * validIndexCount );
	memcpy( pInfo->callStack, callStack, sizeof(void*) * validIndexCount );
	pInfo->callStackCount = validIndexCount;
#	else
	memcpy( pInfo->callStack, callStack, sizeof(void*) * DBG_MEM_CALLSTACK_NUM );
#	endif
#endif		
#endif
	return pInfo;
}

void* MemoryProfiler::AllocBytes(size_t count)
{
	//AUTO_LOCK(g_ProfilerAllocLock);
	MemSource msType = MS_System;
	const size_t MARK_SIZE = sizeof(size_t);
	size_t totalSize = count + MARK_SIZE;
#if MEMPROFILER_USE_LLALLOC
	size_t* ptr = (size_t*)LowLevelAllocator::AllocBytes( totalSize );
#else
	size_t* ptr = (size_t*)MemoryAllocBytes( totalSize, msType );
#endif
	*ptr = totalSize;
	++ptr;		
	sProfilerTotalAllocBytes += totalSize;
	if( msType == MS_System )
	{
		g_ProfilerSystemAllocSize += totalSize;
	}
	return ptr;
}

void MemoryProfiler::deallocBytes(void* ptr)
{
	//AUTO_LOCK(g_ProfilerAllocLock);
	if( ptr )
	{
		MemSource msType = MS_Unknown;
		size_t* oriPtr = (size_t*)ptr;
		--oriPtr;
		size_t dataSize = *oriPtr;
		sProfilerTotalAllocBytes -= dataSize;
#if MEMPROFILER_USE_LLALLOC
		LowLevelAllocator::deallocBytes(oriPtr);
#else
		MemoryDeallocBytes( oriPtr, msType );
#endif
		if( msType == MS_System )
		{
			g_ProfilerSystemAllocSize -= dataSize;
		}
	}
}

AllocationData* MemoryProfiler::AddMemInfo( void* ptr, unsigned int size, const char* typeName, const char* fileName, const size_t line, const char* funcName, AllocatorType at )
{
	if( sRootInfoLevel == NULL )
	{
		sRootInfoLevel = (AllocationSearchNode*)AllocBytes( sizeof(AllocationSearchNode) );
		sRootInfoLevel->Init();
		sRootInfoLevel->levelIndex = BIT_OFFSET + 1;
	}
	unsigned long ptrValue = (unsigned long)ptr;
	int curByteOff = BIT_OFFSET;
	AllocationSearchNode* curLvl = sRootInfoLevel;
	AllocationSearchNode* childLvl = NULL;
	while( curByteOff > 0 )
	{
		int index = ptrValue>>curByteOff;
		childLvl = curLvl->childLevel[index];
		if( childLvl == NULL )
		{
			childLvl = (AllocationSearchNode*)AllocBytes( sizeof(AllocationSearchNode) );
			childLvl->Init();
			curLvl->childLevel[index] = childLvl;
			childLvl->parentLevel = curLvl;
			childLvl->levelIndex = curByteOff;
		}
		curLvl = childLvl;
		unsigned long seg = 1<<curByteOff;
		if( ptrValue >= seg )
			ptrValue -= seg;
		if( ptrValue == 0 )
			break;
		curByteOff--;
	}
	assert( (curLvl->value == NULL) || (curLvl->value->dataPtr == ptr) );
	if( curLvl->value && curLvl->value->dataPtr == ptr )
	{
		curLvl->value->Destroy();
		deallocBytes( curLvl->value );
		curLvl->value = NULL;
	}
	if( curLvl->value == NULL )
	{
		curLvl->value = (AllocationData*)AllocBytes( sizeof(AllocationData) );
		curLvl->value->Init();
		curLvl->value->dataPtr = ptr;
		curLvl->value->dataSize = size;
#if USE_FIX_NAME_LEN
		size_t len = strlen(typeName); if( len >= DBG_MEM_NAME_LENGTH ) len = DBG_MEM_NAME_LENGTH-1;
		memcpy( curLvl->value->typeName, typeName, len );
		len = strlen(fileName);if( len >= DBG_MEM_NAME_LENGTH ) len = DBG_MEM_NAME_LENGTH-1;
		memcpy( curLvl->value->fileName, fileName, len );
		len = strlen(funcName);if( len >= DBG_MEM_NAME_LENGTH ) len = DBG_MEM_NAME_LENGTH-1;
		memcpy( curLvl->value->funcName, funcName, len );
#else
		size_t len = strlen(typeName);
		curLvl->value->typeName = (char*)MemoryProfiler::AllocBytes( len + 1 );
		memcpy( curLvl->value->typeName, typeName, len );
		curLvl->value->typeName[len] = '\0';
		len = strlen(fileName);
		curLvl->value->fileName = (char*)MemoryProfiler::AllocBytes( len + 1 );
		memcpy( curLvl->value->fileName, fileName, len );
		curLvl->value->fileName[len] = '\0';
		len = strlen(funcName);
		curLvl->value->funcName = (char*)MemoryProfiler::AllocBytes( len + 1 );
		memcpy( curLvl->value->funcName, funcName, len );
		curLvl->value->funcName[len] = '\0';
#endif
		curLvl->value->allocatorType = at;
	}
	return curLvl->value;
}

AllocationData* MemoryProfiler::FindMemInfo( void* ptr )
{
	if( sRootInfoLevel == NULL )
		return false;
	unsigned long ptrValue = (unsigned long)ptr;
	int curByteOff = BIT_OFFSET;
	AllocationSearchNode* curLvl = sRootInfoLevel;
	AllocationSearchNode* childLvl = NULL;
	while( curByteOff > 0 )
	{
		int index = ptrValue>>curByteOff;
		childLvl = curLvl->childLevel[index];
		if( childLvl == NULL )
		{
			return false;
		}
		curLvl = childLvl;
		unsigned long seg = 1<<curByteOff;
		if( ptrValue >= seg )
			ptrValue -= seg;
		if( ptrValue == 0 )
			break;
		curByteOff--;
	}
	if( curLvl->value != NULL && curLvl->value->dataPtr == ptr )
	{
		return curLvl->value;
	}
	dbg_print( "Error : not find AllocationData for %p", ptr );
	return NULL;
}

bool MemoryProfiler::RemoveMemInfo( void* ptr, size_t &totalSize )
{
	if( sRootInfoLevel == NULL )
		return false;
	unsigned long ptrValue = (unsigned long)ptr;
	int curByteOff = BIT_OFFSET;
	AllocationSearchNode* curLvl = sRootInfoLevel;
	AllocationSearchNode* childLvl = NULL;
	while( curByteOff > 0 )
	{
		int index = ptrValue>>curByteOff;
		childLvl = curLvl->childLevel[index];
		if( childLvl == NULL )
		{
			return false;
		}
		curLvl = childLvl;
		unsigned long seg = 1<<curByteOff;
		if( ptrValue >= seg )
			ptrValue -= seg;
		if( ptrValue == 0 )
			break;
		curByteOff--;
	}
	if( curLvl->value != NULL && curLvl->value->dataPtr == ptr )
	{
		totalSize -= curLvl->value->dataSize;
		curLvl->value->Destroy();
		deallocBytes( curLvl->value );
		curLvl->value = NULL;
		AllocationSearchNode::OnDestroyChild( curLvl );
		return true;
	}
	else
	{
		dbg_print( "Error : not find AllocationData for %p", ptr );
	}
	return false;
}

//////////////////////////////////////////////////////////////////////////



class AllocationDataCollectList
{
public:
	struct AllocationDataCollectNode
	{
		AllocationDataCollectNode* prev;
		AllocationDataCollectNode* next;
		AllocationData* value;

		AllocationDataCollectNode():prev(NULL),next(NULL),value(NULL)
		{
		}
		void Init()
		{
			memset(this, 0, sizeof(AllocationDataCollectNode));
		}
	};
	AllocationDataCollectNode* firsNode;
#if USE_FIX_NAME_LEN
	char name[DBG_MEM_NAME_LENGTH];
#else
	char *name;
#endif
	size_t totalAllocSize;
	size_t totalAllocCount;

	AllocationDataCollectList():firsNode(NULL),totalAllocSize(0)
	{
		memset( this, 0, sizeof(AllocationDataCollectList) );
	}
	void Init()
	{
		memset( this, 0, sizeof(AllocationDataCollectList) );
	}
	void AddMemInfo(AllocationData* info)
	{
		AllocationDataCollectList::AllocationDataCollectNode* newNode = (AllocationDataCollectList::AllocationDataCollectNode*)MemoryProfiler::AllocBytes( sizeof(AllocationDataCollectList::AllocationDataCollectNode) );
		newNode->Init();
		newNode->value = info;
		totalAllocSize += info->dataSize;
		totalAllocCount++;

		if( firsNode == NULL )
		{
			firsNode = newNode;
		}
		else
		{
			AllocationDataCollectNode* curNode = firsNode;
			while( curNode && curNode->value->dataSize > info->dataSize )
			{
				if( curNode->next == NULL )
				{
					newNode->prev = curNode;
					curNode->next = newNode;
					return;
				}
				curNode = curNode->next;
			}
			if( curNode->prev == NULL )
			{
				newNode->next = curNode;
				curNode->prev = newNode;
				firsNode = newNode;
			}
			else
			{
				AllocationDataCollectList::AllocationDataCollectNode* parNode = curNode->prev;
				parNode->next = newNode;
				newNode->prev = parNode;
				newNode->next = curNode;
				curNode->prev = newNode;
			}
		}		
	}
	void Destroy()
	{
#if USE_FIX_NAME_LEN
#else
		MemoryProfiler::deallocBytes(name);
#endif
		if( firsNode == NULL )
			return;
		AllocationDataCollectList::AllocationDataCollectNode *curNode = firsNode, *nextNode = NULL;
		while( curNode )
		{
			nextNode = curNode->next;
			MemoryProfiler::deallocBytes( curNode );
			curNode = nextNode;
		}
	}
};

class AllocationDataCollector
{
public:
	struct DataSortNode
	{
		DataSortNode* prev;
		DataSortNode* next;
		AllocationDataCollectList* value;

		void Init()
		{
			prev = next = NULL;
			value = NULL;
		}

		void Destroy()
		{
			value->Destroy();
			MemoryProfiler::deallocBytes( value );
		}
	};
	DataSortNode* firstNode;
	size_t nodeCount;
	size_t totalAllocCount;
	size_t totalAllocSize;

	AllocationDataCollector()
	{
		firstNode = NULL;
		nodeCount = 0;
		totalAllocCount = 0;
		totalAllocSize = 0;
	}

	~AllocationDataCollector()
	{
		Destroy();
	}

	void AddMemInfo(AllocationData* info)
	{
		totalAllocCount++;
		totalAllocSize += info->dataSize;

		if( firstNode == NULL )
		{
			firstNode = (DataSortNode*)MemoryProfiler::AllocBytes( sizeof(AllocationDataCollector::DataSortNode) );
			firstNode->Init();
			firstNode->value = (AllocationDataCollectList*)MemoryProfiler::AllocBytes(sizeof(AllocationDataCollectList));
			firstNode->value->Init();
			int len = strlen( info->typeName );
#if USE_FIX_NAME_LEN
			if( len >= DBG_MEM_NAME_LENGTH ) len = DBG_MEM_NAME_LENGTH - 1;
			memcpy( firstNode->value->name, info->typeName, len );
#else
			firstNode->value->name = (char*)MemoryProfiler::AllocBytes( len + 1 );
			memcpy( firstNode->value->name, info->typeName, len );
			firstNode->value->name[len] = '\0';
#endif
			firstNode->value->AddMemInfo( info );
			++nodeCount;
			return;
		}
		DataSortNode* curNode = firstNode;
		while( curNode )
		{
			if( strcmp(curNode->value->name, info->typeName) == 0 )
			{
				curNode->value->AddMemInfo( info );
				return;
			}
			if( curNode->next == NULL )
			{
				DataSortNode* newNode = (DataSortNode*)MemoryProfiler::AllocBytes( sizeof(AllocationDataCollector::DataSortNode) );
				newNode->Init();
				newNode->prev = curNode;
				curNode->next = newNode;
				newNode->value = (AllocationDataCollectList*)MemoryProfiler::AllocBytes(sizeof(AllocationDataCollectList));
				newNode->value->Init();
				int len = strlen( info->typeName );
#if USE_FIX_NAME_LEN
				if( len >= DBG_MEM_NAME_LENGTH ) len = DBG_MEM_NAME_LENGTH - 1;
				memcpy( newNode->value->name, info->typeName, len );
#else
				newNode->value->name = (char*)MemoryProfiler::AllocBytes( len + 1 );
				memcpy( newNode->value->name, info->typeName, len );
				newNode->value->name[len] = '\0';
#endif
				newNode->value->AddMemInfo( info );
				++nodeCount;
				return;
			}
			else
				curNode = curNode->next;
		}
	}

	void Destroy()
	{
		if( firstNode == NULL )
			return;
		AllocationDataCollector::DataSortNode *curNode = firstNode, *nextNode = NULL;
		while( curNode )
		{
			nextNode = curNode->next;
			curNode->value->Destroy();
			MemoryProfiler::deallocBytes( curNode );
			curNode = nextNode;
		}
		firstNode = NULL;
		nodeCount = 0;
	} 

	void Sort()
	{
		AllocationDataCollector::DataSortNode* tmpMaxNode = firstNode;
		AllocationDataCollector::DataSortNode* curCmpNode = firstNode;
		AllocationDataCollector::DataSortNode* maxNode = firstNode;
		while( tmpMaxNode )
		{
			while( curCmpNode )
			{
				if( curCmpNode->value->totalAllocSize > maxNode->value->totalAllocSize )
					maxNode = curCmpNode;
				curCmpNode = curCmpNode->next;
			}

			if( maxNode != tmpMaxNode )
			{
				AllocationDataCollectList* tmpVal = maxNode->value;
				maxNode->value = tmpMaxNode->value;
				tmpMaxNode->value = tmpVal;
			}

			tmpMaxNode = tmpMaxNode->next;
			maxNode = curCmpNode = tmpMaxNode;
		}
	}

};

//////////////////////////////////////////////////////////////////////////

void CollectMemInfo(AllocationSearchNode* pCurLvl, AllocationDataCollector &collector)
{
	if( pCurLvl == NULL )
		return;
	if( pCurLvl )
	{
		if( pCurLvl->value != NULL )
		{
			collector.AddMemInfo(pCurLvl->value);
		}
	}
	AllocationSearchNode* childLvl = NULL;
	for ( int i = 0; i < 2; ++i )
	{
		childLvl = pCurLvl->childLevel[i];
		if( childLvl != NULL )
			CollectMemInfo( childLvl, collector );
	}
}


void MemoryProfiler::SnapMemShot(const char* memLogFile, bool printCallStack)
{
	MemoryProfiler::Initialize();

	MemProfilerClearLog(memLogFile);

	const char* C_ALLOC_NAME[] = { "Unknown", "EngineAllocator", "STLAllocator", }; 
	const char* C_MEM_SOURCE[] = { "Unknown", "System",          "MemPool",      };

	AllocationDataCollector collector;
	CollectMemInfo( sRootInfoLevel, collector );
	collector.Sort();

	size_t engineTotalAllocSize = HighLevelEngineAllocator::sTotalAllocSize + HighLevelSTLAllocator::sTotalAllocSize;
	size_t engineTotalAllocCount = HighLevelEngineAllocator::sTotalAllocCount + HighLevelSTLAllocator::sTotalAllocCount;
	size_t engineTotalUseSize = HighLevelEngineAllocator::sTotalUseSize + HighLevelSTLAllocator::sTotalUseSize;
	size_t systemTotalAllocSize = engineTotalAllocSize + sProfilerTotalAllocBytes;
	float engineTotalAllocUseRate = (float)engineTotalUseSize / (float)engineTotalAllocSize;
	float engineAllocUseRate = (float)HighLevelEngineAllocator::sTotalUseSize / (float)HighLevelEngineAllocator::sTotalAllocSize;
	float stlAllocUseRate = (float)HighLevelSTLAllocator::sTotalUseSize / (float)HighLevelSTLAllocator::sTotalAllocSize;

	// 根节点
	MemProfilerOutPutToLog( memLogFile, "<MemorySnapShot Time=\"%.3f(s)\">", (float)Timer::GetTime() );
	// 内存管理器信息打印(记录程序当前向系统申请的内存总量)
	MemProfilerOutPutToLog( memLogFile, "\t<MemoryManager TotalSystemAllocSize=\"%s\"/>",  GetFormatMemSizeStr(MemoryManagerAllocator::sTotalAllocSize).c_str() );
	// 所有内存分配器系统内存分布信息简报
	MemProfilerOutPutToLog( memLogFile, "\t<SystemMemoryBrief TotalSystemAllocSize=\"%s\">", 
		 GetFormatMemSizeStr(g_ProfilerSystemAllocSize + g_EngineSystemAllocSize + g_STLSystemAllocSize + AirMemoryPool::g_MemoryPoolInnerSystemAllocSize + AirMemoryPool::g_MemoryPoolSystemAllocSize).c_str() );
	// 内存分析器所占的系统内存
	MemProfilerOutPutToLog( memLogFile, "\t\t<MemoryProfiler SystemAllocSize=\"%s\"/>",  GetFormatMemSizeStr(g_ProfilerSystemAllocSize).c_str() );
	// 引擎内存分配器所占的系统内存
	MemProfilerOutPutToLog( memLogFile, "\t\t<HighLevelEngineAllocator SystemAllocSize=\"%s\"/>",  GetFormatMemSizeStr(g_EngineSystemAllocSize).c_str() );
	// STL内存分配器所占的系统内存
	MemProfilerOutPutToLog( memLogFile, "\t\t<HighLevelSTLAllocator SystemAllocSize=\"%s\"/>",  GetFormatMemSizeStr(g_STLSystemAllocSize).c_str() );
	// 内存池管理器所占的系统内存
	MemProfilerOutPutToLog( memLogFile, "\t\t<MemoryPool SystemAllocSize=\"%s\">",  GetFormatMemSizeStr(AirMemoryPool::g_MemoryPoolSystemAllocSize+AirMemoryPool::g_MemoryPoolInnerSystemAllocSize).c_str() );
	MemProfilerOutPutToLog( memLogFile, "\t\t\t<Pool SystemAllocSize=\"%s\"/>",  GetFormatMemSizeStr(AirMemoryPool::g_MemoryPoolSystemAllocSize).c_str() );
	MemProfilerOutPutToLog( memLogFile, "\t\t\t<Inner SystemAllocSize=\"%s\"/>",  GetFormatMemSizeStr(AirMemoryPool::g_MemoryPoolInnerSystemAllocSize).c_str() );
	MemProfilerOutPutToLog( memLogFile, "\t\t</MemoryPool>" );
	MemProfilerOutPutToLog( memLogFile, "\t</SystemMemoryBrief>" );

	MemProfilerOutPutToLog( memLogFile, "\t<AllocatorBrief>" );
	// MemoryProfiler的内存占用情况
	MemProfilerOutPutToLog( memLogFile, "\t\t<MemoryProfiler totalAllocSize=\"%s\" allocSizeFromSystem=\"%s\" allocSizeFromPool=\"%s\"/>",
		 GetFormatMemSizeStr(sProfilerTotalAllocBytes).c_str(),  GetFormatMemSizeStr(g_ProfilerSystemAllocSize).c_str(),  GetFormatMemSizeStr(sProfilerTotalAllocBytes-g_ProfilerSystemAllocSize).c_str() );
	// HighLevelEngineAllocator的内存占用情况
	MemProfilerOutPutToLog( memLogFile, "\t\t<HighLevelEngineAllocator allocSize = \"%s\" allocCount = \"%d\" allocSizeFromSystem=\"%s\" allocSizeFromPool=\"%s\"/>", 
		 GetFormatMemSizeStr(HighLevelEngineAllocator::sTotalAllocSize).c_str(), HighLevelEngineAllocator::sTotalAllocCount,  GetFormatMemSizeStr(g_EngineSystemAllocSize).c_str(),  GetFormatMemSizeStr(HighLevelEngineAllocator::sTotalAllocSize-g_EngineSystemAllocSize).c_str());
	// HighLevelSTLAllocator的内存占用情况
	MemProfilerOutPutToLog( memLogFile, "\t\t<HighLevelSTLAllocator allocSize = \"%s\" allocCount = \"%d\" allocSizeFromSystem=\"%s\" allocSizeFromPool=\"%s\"/>", 
		 GetFormatMemSizeStr(HighLevelSTLAllocator::sTotalAllocSize).c_str(), HighLevelSTLAllocator::sTotalAllocCount,  GetFormatMemSizeStr(g_STLSystemAllocSize).c_str(),  GetFormatMemSizeStr(HighLevelSTLAllocator::sTotalAllocSize-g_STLSystemAllocSize).c_str());
	// MemoryPool的内存占用情况
	AirMemoryPool::SystemMemoryPools::GetSystemPools().SnapMemoryPool( memLogFile );
	MemProfilerOutPutToLog( memLogFile, "\t</AllocatorBrief>" );

	// 记录图形内存使用情况
	g_GraphicPrifilerDataContainer.SnapMemShot( memLogFile, printCallStack );


#if ENABLE_DEEP_PROFILE
	MemProfilerOutPutToLog( memLogFile, "\t<AllocationDetail allocSize = \"%s\" allocCount = \"%d\">",  GetFormatMemSizeStr(collector.totalAllocSize).c_str(), collector.totalAllocCount );
	bool newTag = false;
	bool firstTag = true;
	AllocationDataCollector::DataSortNode* pCurNode = collector.firstNode;
	while( pCurNode )
	{
		MemProfilerOutPutToLog( memLogFile, "\t\t<Type typeName = \"%s\" allocSize = \"%s\" allocCount = \"%d\">", 
			pCurNode->value->name,  GetFormatMemSizeStr(pCurNode->value->totalAllocSize).c_str(), pCurNode->value->totalAllocCount );

		AllocationDataCollectList::AllocationDataCollectNode* pBkNode = pCurNode->value->firsNode;
		while( pBkNode )
		{
			MemProfilerOutPutToLog( memLogFile, "\t\t\t<Object typeName=\"%s\" size=\"%s\" func=\"%s\" file=\"%s\" line=\"%d\" allocatorType=\"%s\" memSource=\"%s\">", 
				pBkNode->value->typeName,  GetFormatMemSizeStr(pBkNode->value->dataSize).c_str(), pBkNode->value->funcName, pBkNode->value->fileName, pBkNode->value->line, C_ALLOC_NAME[pBkNode->value->allocatorType], C_MEM_SOURCE[pBkNode->value->memSourceType] );
			if( printCallStack )
			{
#	if AIR_TARGET_PLATFORM == AIR_PLATFORM_WIN32
#		if USE_FIX_CALLSTACK_COUNT
				size_t callStackNum = DBG_MEM_CALLSTACK_NUM;
#		else
				size_t callStackNum = pBkNode->value->callStackCount;
#		endif
				MemProfilerOutPutToLog( memLogFile, "\t\t\t\t<CallStacks>" );
				for ( int i = 0; i < callStackNum; ++i )
				{
					char file[1024] = {0};
					int line = 0;
					if(pBkNode->value->callStack[i])
					{
						bool res = GetFileAndLine( pBkNode->value->callStack[i], file, line );
						if( res )
							MemProfilerOutPutToLog( memLogFile, "\t\t\t\t\t<StackFrame id = \"%d\" ptr = \"%p\" line = \"%d\", file = \"%s\"/>", 
							i, pBkNode->value->callStack[i], line, file);
						else
							MemProfilerOutPutToLog( memLogFile, "\t\t\t\t\t<StackFrame id = \"%d\" ptr = \"%p\"/>", 
							i, pBkNode->value->callStack[i]);
					}
					else
						break;
				}
				MemProfilerOutPutToLog( memLogFile, "\t\t\t\t</CallStacks>" );
#	endif
			}
			MemProfilerOutPutToLog( memLogFile, "\t\t\t</Object>" );
			pBkNode = pBkNode->next;
		}

		MemProfilerOutPutToLog( memLogFile, "\t\t</Type>" );
		pCurNode = pCurNode->next;
	}
	MemProfilerOutPutToLog( memLogFile, "\t</AllocationDetail>" );
#endif

	
	MemProfilerOutPutToLog( memLogFile, "</MemorySnapShot>" );
}

bool MemoryProfiler::GetFileAndLine(const void *pAddress, char szFile[MAX_PATH_LEN], int &line)
{
#if AIR_TARGET_PLATFORM == AIR_PLATFORM_WIN32
	IMAGEHLP_LINE64 Line;
	Line.SizeOfStruct = sizeof(Line);
	memset(&Line, 0, sizeof(Line));
	DWORD Offset = 0;
	if(fnSymGetLineFromAddr64(s_Process, (DWORD64)pAddress, &Offset, &Line))
	{
		strcpy(szFile, Line.FileName);
		line = Line.LineNumber;
		return true;
	}
#endif
	return false;
}

bool MemoryProfiler::InitDbgHelpLib()
{
#if AIR_TARGET_PLATFORM == AIR_PLATFORM_WIN32
	if( s_DbgHelpLib != NULL )
		return true;
	TCHAR szDbgName[MAX_PATH_LEN];
	GetModuleFileName(NULL, szDbgName, MAX_PATH_LEN);
	TCHAR *p = _tcsrchr(szDbgName, _T('\\'));
	if(p)
		*p = 0;
	_tcscat(szDbgName, _T("\\dbghelp.dll"));

	// 查找当前目录的DLL
	s_DbgHelpLib = LoadLibrary(szDbgName);
	if(s_DbgHelpLib == NULL)
	{
		// 使用系统的DLL
		s_DbgHelpLib = LoadLibrary(_T("dbghelp.dll"));
		if(s_DbgHelpLib == NULL)
			return false;
	}

	fnMiniDumpWriteDump = (tFMiniDumpWriteDump)GetProcAddress(s_DbgHelpLib, "MiniDumpWriteDump");
	fnSymInitialize = (tFSymInitialize)GetProcAddress(s_DbgHelpLib, "SymInitialize");
	fnStackWalk64 = (tFStackWalk64)GetProcAddress(s_DbgHelpLib, "StackWalk64");
	fnSymFromAddr = (tFSymFromAddr)GetProcAddress(s_DbgHelpLib, "SymFromAddr");
	fnSymGetLineFromAddr64 = (tFSymGetLineFromAddr64)GetProcAddress(s_DbgHelpLib, "SymGetLineFromAddr64");
	fnSymGetOptions = (tFSymGetOptions)GetProcAddress(s_DbgHelpLib, "SymGetOptions");
	fnSymSetOptions = (tFSymSetOptions)GetProcAddress(s_DbgHelpLib, "SymSetOptions");
	fnSymFunctionTableAccess64 = (tFSymFunctionTableAccess64)GetProcAddress(s_DbgHelpLib, "SymFunctionTableAccess64");
	fnSymGetModuleBase64 = (tFSymGetModuleBase64)GetProcAddress(s_DbgHelpLib, "SymGetModuleBase64");

	if(fnMiniDumpWriteDump &&
		fnSymInitialize &&
		fnStackWalk64 &&
		fnSymFromAddr &&
		fnSymGetLineFromAddr64 &&
		fnSymGetOptions &&
		fnSymSetOptions &&
		fnSymFunctionTableAccess64 &&
		fnSymGetModuleBase64)
	{
		DWORD ProcessID = GetCurrentProcessId();
		s_Process = OpenProcess(PROCESS_ALL_ACCESS, FALSE, ProcessID);
		fnSymInitialize(s_Process, ".", TRUE);
		return true;
	}
#endif
	return false;
}

void MemoryProfiler::FreeDbgHelpLib()
{
#if AIR_TARGET_PLATFORM == AIR_PLATFORM_WIN32
	if(s_DbgHelpLib != NULL)
	{
		FreeLibrary(s_DbgHelpLib);
		CloseHandle(s_Process);
		s_Process = NULL;
		s_DbgHelpLib = NULL;
	}

	fnMiniDumpWriteDump = NULL;
	fnSymInitialize = NULL;
	fnStackWalk64 = NULL;
	fnSymFromAddr = NULL;
	fnSymGetLineFromAddr64 = NULL;
	fnSymGetOptions = NULL;
	fnSymSetOptions = NULL;
	fnSymFunctionTableAccess64 = NULL;
	fnSymGetModuleBase64 = NULL;
#endif
}


//////////////////////////////////////////////////////////////////////////

#define ENABLE_HLV_ALLOCATOR_COLLECT_SIZE	0

size_t HighLevelEngineAllocator::sTotalAllocSize = 0;
size_t HighLevelEngineAllocator::sTotalAllocCount = 0;
size_t HighLevelEngineAllocator::sTotalUseSize = 0;
size_t HighLevelSTLAllocator::sTotalAllocSize = 0;
size_t HighLevelSTLAllocator::sTotalAllocCount = 0;
size_t HighLevelSTLAllocator::sTotalUseSize = 0;

void* HighLevelEngineAllocator::AllocBytes(size_t count, const char* typeName, const char* file, const size_t line, const char* funcName )
{
	AUTO_LOCK(g_HLVEngineAllocLock);
	MemSource msType = MS_System;

#if ENABLE_HLV_ALLOCATOR_COLLECT_SIZE

#	if ENABLE_MEMORY_PROFILER
		const size_t MARK_SIZE = sizeof(size_t);
		size_t totalSize = count + MARK_SIZE;
		//size_t* ptr = (size_t*)LowLevelAllocator::AllocBytes( totalSize );
		size_t* ptr = (size_t*)MemoryAllocBytes( totalSize, msType );
		*ptr = totalSize;
		++ptr;
		sTotalAllocSize += totalSize;
		sTotalUseSize += count;
		sTotalAllocCount += 1;
		AllocationData* pMemInfo = MemoryProfiler::RecMemInfo(ptr, count, typeName, file, line, funcName, ALLOC_TYPE);
		pMemInfo->memSourceType = msType;
		if( msType == MS_System )
			g_EngineSystemAllocSize += totalSize; 
#	else
		//void* ptr = LowLevelAllocator::AllocBytes( count );
		void* ptr = MemoryAllocBytes( count, msType );
#	endif
		return ptr;

#else

#	if ENABLE_MEMORY_PROFILER
		//void* ptr = LowLevelAllocator::AllocBytes( count );
		void* ptr = MemoryAllocBytes( count, msType );
		sTotalAllocSize += count;
		sTotalUseSize += count;
		sTotalAllocCount += 1;
		AllocationData* pMemInfo = MemoryProfiler::RecMemInfo(ptr, count, typeName, file, line, funcName, ALLOC_TYPE);		
		pMemInfo->memSourceType = msType;
		if( msType == MS_System )
			g_EngineSystemAllocSize += count; 
#	else
		//void* ptr = LowLevelAllocator::AllocBytes( count );
		void* ptr = MemoryAllocBytes( count, msType );
#	endif
		return ptr;

#endif
}

void HighLevelEngineAllocator::deallocBytes(void* ptr)
{
	AUTO_LOCK(g_HLVEngineAllocLock);
	if (ptr)
	{
		MemSource msType = MS_Unknown;
#if ENABLE_HLV_ALLOCATOR_COLLECT_SIZE
				
#	if ENABLE_MEMORY_PROFILER
		size_t* oriPtr = (size_t*)ptr;
		--oriPtr;
		size_t totalSize = *oriPtr;
		sTotalAllocSize -= totalSize;
		sTotalAllocCount -= 1;
		MemoryProfiler::RemoveMemInfo( ptr, sTotalUseSize );
		//LowLevelAllocator::deallocBytes(oriPtr);
		MemoryDeallocBytes( oriPtr, msType );
		if( msType == MS_System )
			g_EngineSystemAllocSize -= totalSize; 
#	else
		//LowLevelAllocator::deallocBytes(ptr);
		MemoryDeallocBytes( ptr, msType );
#	endif

#else

#	if ENABLE_MEMORY_PROFILER
		size_t oriAllocSize = sTotalAllocSize;
		MemoryProfiler::RemoveMemInfo( ptr, sTotalAllocSize );
		--sTotalAllocCount;		
		sTotalUseSize = sTotalAllocSize;
		//LowLevelAllocator::deallocBytes(ptr);
		MemoryDeallocBytes( ptr, msType );
		if( msType == MS_System )
			g_EngineSystemAllocSize -= (oriAllocSize - sTotalAllocSize); 
#	else
		//LowLevelAllocator::deallocBytes(ptr);
		MemoryDeallocBytes( ptr, msType );
#	endif		

#endif
	}
}

/// Get the maximum size of a single allocation
size_t HighLevelEngineAllocator::getMaxAllocationSize()
{
	return (std::numeric_limits<size_t>::max)();
}



void* HighLevelSTLAllocator::AllocBytes(size_t count, const char* typeName, const char* file, const size_t line, const char* funcName)
{
	AUTO_LOCK(g_HLVSTLAllocLock);
	MemSource msType = MS_System;

#if ENABLE_HLV_ALLOCATOR_COLLECT_SIZE

#	if ENABLE_MEMORY_PROFILER
		const size_t MARK_SIZE = sizeof(size_t);
		size_t totalSize = count + MARK_SIZE;
		size_t* ptr = (size_t*)MemoryAllocBytes( totalSize, msType );
		*ptr = totalSize;
		++ptr;
		sTotalAllocSize += totalSize;
		sTotalUseSize += count;
		sTotalAllocCount += 1;
		AllocationData* pMemInfo = MemoryProfiler::RecMemInfo(ptr, count, typeName, file, line, funcName, ALLOC_TYPE);
		pMemInfo->memSourceType = msType;
		if( msType == MS_System )
			g_STLSystemAllocSize += totalSize; 
#	else
		void* ptr = MemoryAllocBytes( count, msType );
#	endif
		return ptr;

#else

#	if ENABLE_MEMORY_PROFILER
		void* ptr = MemoryAllocBytes( count, msType );
		sTotalAllocSize += count;
		sTotalUseSize += count;
		sTotalAllocCount += 1;
		AllocationData* pMemInfo = MemoryProfiler::RecMemInfo(ptr, count, typeName, file, line, funcName, ALLOC_TYPE);	
		pMemInfo->memSourceType = msType;
		if( msType == MS_System )
			g_STLSystemAllocSize += count; 
#	else
		void* ptr = MemoryAllocBytes( count, msType );
#	endif

		return ptr;

#endif
}

void HighLevelSTLAllocator::deallocBytes(void* ptr)
{
	AUTO_LOCK(g_HLVSTLAllocLock);
	MemSource msType = MS_System;
	if (ptr)
	{
#if ENABLE_HLV_ALLOCATOR_COLLECT_SIZE

#	if ENABLE_MEMORY_PROFILER
		size_t* oriPtr = (size_t*)ptr;
		--oriPtr;
		size_t totalSize = *oriPtr;
		sTotalAllocSize -= totalSize;
		sTotalAllocCount -= 1;
		MemoryProfiler::RemoveMemInfo( ptr, sTotalUseSize );
		MemoryDeallocBytes( oriPtr, msType );
		if( msType == MS_System )
			g_STLSystemAllocSize -= totalSize;
#	else
		MemoryDeallocBytes( ptr, msType );
#	endif

#else

#	if ENABLE_MEMORY_PROFILER
		size_t oriAllocSize = sTotalAllocSize;
		MemoryProfiler::RemoveMemInfo( ptr, sTotalAllocSize );
		--sTotalAllocCount;
		sTotalUseSize = sTotalAllocSize;
		MemoryDeallocBytes( ptr, msType );
		if( msType == MS_System )
			g_STLSystemAllocSize -= (oriAllocSize - sTotalAllocSize);
#	else
		MemoryDeallocBytes( ptr, msType );
#	endif		

#endif			
	}
}

/// Get the maximum size of a single allocation
size_t HighLevelSTLAllocator::getMaxAllocationSize()
{
	return (std::numeric_limits<size_t>::max)();
}


//////////////////////////////////////////////////////////////////////////














namespace AirMemoryPool
{


	inline void AddPoolInnerAllocSize(size_t size)
	{
		g_MemoryPoolInnerSystemAllocSize += size;
	}

	inline void RemovePoolInnerAllocSize(size_t size)
	{
		g_MemoryPoolInnerSystemAllocSize -= size;
	}

	inline void AddAllocSize(size_t size)
	{
		g_MemoryPoolSystemAllocSize += size;
	}

	inline void RemoveAllocSize(size_t size)
	{
		g_MemoryPoolSystemAllocSize -= size;
	}

	inline void AddUseSize(size_t size)
	{
		g_MemoryPoolUseSize += size;
	}

	inline void RemoveUseSize(size_t size)
	{
		g_MemoryPoolUseSize -= size;
	}

	//////////////////////////////////////////////////////////////////////////

	void*	InnerMalloc(size_t size)
	{
		return MemoryManagerAllocator::AllocBytes( size );
	}
	void	InnerFree(void *ptr)
	{
#if ENABLE_MEMPOOL_DBG
		dbg_print( "Free mem at [%p], lowlevel free\n", ptr );
#endif
		return MemoryManagerAllocator::deallocBytes( ptr );
	}

#if AIR_TARGET_PLATFORM != AIR_PLATFORM_WIN32
#	define PtrToUlong( p )			((unsigned long)(unsigned long*) (p) )
#endif

#	define PoolRealAlloc(S)			InnerMalloc( S )
#	define PoolRealFree(P)			InnerFree(P)
	//#ifdef USE_ALIGNED_MALLOC_AS_POOL_REAL_ALLOC
	//#	define PoolRealAlloc(S)			aligned_malloc( S, 16 )
	//#	define PoolRealFree(P)			aligned_free(P)
	//#else
	//#	if AIR_TARGET_PLATFORM == AIR_PLATFORM_WIN32
	//#		define PoolRealAlloc(S)		VirtualAlloc(NULL, S, MEM_COMMIT, PAGE_READWRITE)
	//#		define PoolRealFree(P)		VirtualFree(P, 0, MEM_RELEASE)
	//#	else
	//#		include <sys/mman.h>
	//void* mmapAlloc(size_t size)
	//{
	//	size_t realSize = size + sizeof(size_t);
	//	void* ptrStart = mmap((void*)NULL, realSize, PROT_NONE, MAP_PRIVATE | MAP_ANON, -1, 0);
	//	if( ptrStart )
	//	{
	//		int result2 = mprotect(ptrStart, realSize, PROT_READ | PROT_WRITE);
	//		size_t* ptrUser = (size_t*)ptrStart;
	//		*ptrUser = realSize;
	//		++ptrUser;
	//		return ptrUser;
	//	}		
	//	return NULL;
	//}
	//void mmapFree(void* ptr)
	//{
	//	if( ptr )
	//	{
	//		size_t* ptrStart = (size_t*)ptr;
	//		--ptrStart;
	//		size_t realSize = *ptrStart;
	//		int result4 = munmap(ptrStart, realSize);
	//	}
	//}
	//#		define PoolRealAlloc(S) mmapAlloc(S)
	//#		define PoolRealFree(P)  mmapFree(P)
	//#	endif
	//#endif

#define ROUNDNUM2(value, num)	(((value)+(num-1)) & (~(num-1)))

	//////////////////////////////////////////////////////////////////////////
	
	void PoolInfo::Clear()
	{
#if ENABLE_MEMPOOL_DBG
		dbg_print( "destroy subpool [%p] [%d] [%d] [%d]\n", this, this->mFirstLevel, this->mSecondLevel, this->mSubID );
#endif
		if( mFlags )
		{
			RemovePoolInnerAllocSize( mTotalBlockCount );
			PoolRealFree( mFlags );
		}
		mFlags = NULL;
	}

	//////////////////////////////////////////////////////////////////////////
	const size_t C_POOLINFO_SIZE = sizeof(PoolInfo);

	PoolSecondLevelSearchMap::PoolSecondLevelSearchMap()
	{
		memset( secondLevelSearchMap, 0, sizeof(secondLevelSearchMap) );
	}
	PoolSecondLevelSearchMap::~PoolSecondLevelSearchMap()
	{
		Clear();
	}

	void PoolSecondLevelSearchMap::Init()
	{
		memset( secondLevelSearchMap, 0, sizeof(secondLevelSearchMap) );
	}

	void PoolSecondLevelSearchMap::Clear()
	{
		for ( int i = 0; i < 256; ++i )
		{
			for( int j = 0; j < 3; ++j )
			{
				PoolInfo* pSubPool = secondLevelSearchMap[i][j];
				if( pSubPool != NULL )
				{
					pSubPool->Clear();
					RemovePoolInnerAllocSize( C_POOLINFO_SIZE );
					PoolRealFree( pSubPool );
					pSubPool = NULL;
				}
			}
		}
	}

	bool PoolSecondLevelSearchMap::DestroyPoolInfo( PoolInfo* pPool )
	{
		PoolInfo* oriPool = secondLevelSearchMap[pPool->mSecondLevel][pPool->mSubID];
		assert( oriPool == pPool );
		secondLevelSearchMap[pPool->mSecondLevel][pPool->mSubID] = NULL;
		pPool->Clear();
		RemovePoolInnerAllocSize( C_POOLINFO_SIZE );
		PoolRealFree( pPool );
		return true;
	}

	//////////////////////////////////////////////////////////////////////////

	const size_t C_SEC_LEVEL_SEARCH_MAP_SIZE = sizeof(PoolSecondLevelSearchMap);

	bool GetLevelInfoFromPtr( void* ptr, size_t &firstLV, size_t& secondLV )
	{
		if( !ptr )
			return false;
		unsigned long ptrValue = PtrToUlong(ptr);
		firstLV  = size_t((ptrValue & 0xFF000000)>>24);
		secondLV = size_t((ptrValue & 0x00FF0000)>>16);
		return true;
	}

	PoolFirstLevelSearchMap::PoolFirstLevelSearchMap()
	{
		memset( firstLevelSearchMap, 0, sizeof(firstLevelSearchMap) );
	}

	void PoolFirstLevelSearchMap::Init()
	{
		memset( firstLevelSearchMap, 0, sizeof(firstLevelSearchMap) );
	}

	void PoolFirstLevelSearchMap::Clear()
	{
		for ( int i = 0; i < 256; ++i )
		{
			if( firstLevelSearchMap[i] != NULL )
			{
				RemovePoolInnerAllocSize( C_SEC_LEVEL_SEARCH_MAP_SIZE );
				firstLevelSearchMap[i]->Clear();
				PoolRealFree( firstLevelSearchMap[i] );
				firstLevelSearchMap[i] = NULL;
			}
		}
	}

	PoolFirstLevelSearchMap::~PoolFirstLevelSearchMap()
	{
		Clear();
	}

	PoolInfo* PoolFirstLevelSearchMap::AddPoolInfo( void* ptr )
	{
		size_t firstLevelIndex = -1, secondLevelIndex = -1;
		if( GetLevelInfoFromPtr( ptr, firstLevelIndex, secondLevelIndex ) == false )
			return NULL;
		PoolSecondLevelSearchMap* secMap = firstLevelSearchMap[firstLevelIndex];
		if( !secMap )
		{			
			secMap = (PoolSecondLevelSearchMap*)PoolRealAlloc( C_SEC_LEVEL_SEARCH_MAP_SIZE );
			assert( secMap );
			secMap->Init();
			AddPoolInnerAllocSize( C_SEC_LEVEL_SEARCH_MAP_SIZE );
			firstLevelSearchMap[firstLevelIndex] = secMap;
		}
		unsigned long ptrValue = PtrToUlong(ptr);
		int emptyJ = -1;
		for ( int j = 0; j < 3; ++j )
		{
			PoolInfo* pi = secMap->GetPoolInfo(secondLevelIndex, j);
			if( pi )
			{
				if( pi->mPtrStartValue <= ptrValue && ptrValue <= pi->mPtrEndValue )
				{
					assert( false && "Find Exist PoolInfo!" );
					return pi;
				}
			}
			else if( emptyJ == -1 )
				emptyJ = j;
		}
		assert( emptyJ >= 0 && emptyJ < 3 );
		PoolInfo* pi = (PoolInfo*)PoolRealAlloc( C_POOLINFO_SIZE );
		assert( pi );
		AddPoolInnerAllocSize( C_POOLINFO_SIZE );		
		pi->Init();
		secMap->SetPoolInfo(secondLevelIndex, emptyJ, pi);
		pi->mSecondLevel = secondLevelIndex;
		pi->mFirstLevel = firstLevelIndex;
		pi->mSubID = emptyJ;
		return pi;
	}

	PoolInfo* PoolFirstLevelSearchMap::FindPoolInfo( void* ptr )
	{
		size_t firstLevelIndex = -1, secondLevelIndex = -1;
		if( GetLevelInfoFromPtr( ptr, firstLevelIndex, secondLevelIndex ) == false )
			return NULL;
		int startIndex = 0, endIndex = 2;
		int flAR[3] = { firstLevelIndex, firstLevelIndex, firstLevelIndex, };
		int slAR[3] = { secondLevelIndex-1, secondLevelIndex, secondLevelIndex+1, };
		if( firstLevelIndex == 0 && secondLevelIndex == 0 )
			startIndex = 1;
		else if( firstLevelIndex == 255 && secondLevelIndex == 255 )
			endIndex = 1;
		else
		{
			if( secondLevelIndex == 0 )
			{
				flAR[0] = firstLevelIndex-1;
				slAR[0] = 255;
			}
			if( secondLevelIndex == 255 )
			{
				flAR[2] = firstLevelIndex+1;
				slAR[2] = 0;
			}
		}
		unsigned long ptrValue = PtrToUlong(ptr);
		for ( int lid = startIndex; lid <= endIndex; ++lid )
		{
			PoolSecondLevelSearchMap* secMap = firstLevelSearchMap[flAR[lid]];
			if( !secMap )
				continue;		
			for ( int j = 0; j < 3; ++j )
			{
				PoolInfo* pi = secMap->GetPoolInfo(slAR[lid],j);
				if( pi )
				{
					if( pi->mPtrStartValue <= ptrValue && ptrValue <= pi->mPtrEndValue )
						return pi;
				}
			}
		}
		return NULL;
	}

	bool PoolFirstLevelSearchMap::DestroyPoolInfo( PoolInfo* pPool )
	{
		if( !pPool ) return false;
		PoolSecondLevelSearchMap* secMap = firstLevelSearchMap[ pPool->mFirstLevel ];
		assert( secMap );
		return secMap->DestroyPoolInfo( pPool );
	}

	//////////////////////////////////////////////////////////////////////////

	PoolFirstLevelSearchMap*	MemoryPool::sPoolSearchMap = NULL;
	bool						MemoryPool::sHasInitPoolSearchMap = false;

	MemoryPool::MemoryPool(size_t BlockSize)
	{
		blocksize = BlockSize;
		freePoolCount = 0;
		mFirstPool = NULL;
		poolIndex = -1;

		MEMPOOL_SAT( usesize = 0);
		MEMPOOL_SAT( allocsize = 0);
	}

	void MemoryPool::Init(size_t BlockSize)
	{
		blocksize = BlockSize;
		freePoolCount = 0;
		mFirstPool = NULL;
		poolIndex = -1;

		MEMPOOL_SAT( usesize = 0);
		MEMPOOL_SAT( allocsize = 0);
	}

	void MemoryPool::InitMemoryPoolSearchMap()
	{
		const size_t C_FIRST_LEVEL_SM_SIZE = sizeof(PoolFirstLevelSearchMap);
		if( sPoolSearchMap == NULL )
		{
			sPoolSearchMap = (PoolFirstLevelSearchMap*)InnerMalloc( C_FIRST_LEVEL_SM_SIZE );
			assert( sPoolSearchMap );
			AddPoolInnerAllocSize( C_FIRST_LEVEL_SM_SIZE );
			sPoolSearchMap->Init();
		}
	}

	void MemoryPool::ReleaseMemoryPoolSearchMap()
	{
		const size_t C_FIRST_LEVEL_SM_SIZE = sizeof(PoolFirstLevelSearchMap);
		if( sPoolSearchMap )
		{
			RemovePoolInnerAllocSize( C_FIRST_LEVEL_SM_SIZE );
			sPoolSearchMap->Clear();
			InnerFree( sPoolSearchMap );
			sPoolSearchMap = NULL;
		}
	}

	PoolFirstLevelSearchMap& MemoryPool::GetMemoryPoolSearchMap()
	{
		if(!sHasInitPoolSearchMap)
		{
			sHasInitPoolSearchMap = true;
			InitMemoryPoolSearchMap();
		}
		return *sPoolSearchMap;
	}

	void MemoryPool::MarkPoolUsefulOrExausted( PoolInfo* Pool, PoolStatus status )
	{
		if( !Pool ) return;

		PoolInfo *curPrevPool = Pool->mPrevPool, *curNextPool = Pool->mNextPool;
		// 池子可用,将池子从耗尽队列摘除并放到可用队列的头部
		if( status == ePS_CanUse )
		{
			Pool->mIsExausted = false;
		}
		// 池子耗尽, 将池子从可用队列摘除并放到耗尽队列的头部
		else if( status == ePS_Exausted )
		{
			Pool->mIsExausted = true;
		}
		// 池子需要销毁,从可用队列和耗尽队列摘除
		else if( status == ePS_NeedDestroy )
		{
			if( curPrevPool && curNextPool )
			{
				curPrevPool->mNextPool = curNextPool;
				curNextPool->mPrevPool = curPrevPool;
			}
			else if( curPrevPool && !curNextPool )
			{
				curPrevPool->mNextPool = NULL;
			}
			else if( !curPrevPool && curNextPool )
			{
				curNextPool->mPrevPool = NULL;
				mFirstPool = curNextPool;
			}
			else if( !curPrevPool && !curNextPool )
			{
				mFirstPool = NULL;
			}
		}
	}

	#define POOL_LEVEL_CHECK

	void* MemoryPool::Alloc()
	{
#ifndef USE_MEMORYPOOL
		return InnerMalloc(blocksize);
#endif

		if(blocksize > MAX_POOL_BLOCK_SIZE)
			return InnerMalloc(blocksize);

		MEMPOOL_SAT(AddUseSize(blocksize));
		MEMPOOL_SAT(usesize += blocksize);

		// 找当前可分配的内存池
		PoolInfo *freePool = mFirstPool;
		PoolInfo *Pool = NULL;
		while( freePool && freePool->mIsExausted )
		{
			freePool = freePool->mNextPool;
		}
		if(!freePool || freePool->mIsExausted )
		{
			const size_t POOL_MAX_SIZE = 0xFFFF;
			// 计算这块内存可以划分多少个块
			size_t Blocks = (POOL_MAX_SIZE - 4) / blocksize;
			// 整个内存池的大小
			size_t Bytes = Blocks * blocksize;

			// 分配一大块空闲的内存块,用于分割给用户使用
			void *pRealPool = PoolRealAlloc(Bytes);
			assert( pRealPool );
			// 分配Blocks个字节,记录每个内存块的使用情况(0:未使用;1:已经被使用)
			char *pFlags = (char*)PoolRealAlloc(Blocks);
			assert( pFlags );
			// 全部填充为0,表明所有的块都可以使用
			memset( pFlags, 0, Blocks );

			MEMPOOL_SAT(AddAllocSize(Bytes));
			MEMPOOL_SAT(allocsize += Bytes);
			UNUSED_ARG(Bytes);
			AddPoolInnerAllocSize(Blocks);			

			PoolFirstLevelSearchMap& PIndirect = GetMemoryPoolSearchMap();
			// 建立内存池信息
			Pool = PIndirect.AddPoolInfo( pRealPool );
			assert( Pool );
			// 计算内存池所占用的起始和结束地址的值
			Pool->mPtrStartValue = PtrToUlong(pRealPool);
			Pool->mPtrEndValue = Pool->mPtrStartValue + Bytes;
			// 被占用的块数
			Pool->mBlockTakenCount = 0;
			// 内存池的起始地址
			Pool->mRealPoolStartPtr = pRealPool;
			// 内存池对象
			Pool->mpMemPool = this;
			// 每个碎块的大小
			Pool->mSizePerBlock = blocksize;
			// 整个内存池所占的字节数
			Pool->mPoolTotalSize = Bytes;
			// 总的碎块数量
			Pool->mTotalBlockCount = Blocks;
			Pool->mFlags = pFlags;
			++freePoolCount;

			if( mFirstPool == NULL )
				mFirstPool = Pool;
			else
			{
				Pool->mNextPool = mFirstPool;
				mFirstPool->mPrevPool = Pool;
				mFirstPool = Pool;
			}

#if ENABLE_MEMPOOL_DBG
			dbg_print( "create subpool [%p] [%d] [%d] [%d] at mempool [%d]\n", Pool, Pool->mFirstLevel, Pool->mSecondLevel, Pool->mSubID, Pool->mpMemPool->poolIndex );
#endif
		}
		else
			Pool = freePool;

		// 占用块数加一
		Pool->mBlockTakenCount++;
		assert( Pool->mTotalBlockCount >= Pool->mBlockTakenCount );
		unsigned char *retFree = NULL;
		unsigned char *realStartPtr = (unsigned char*)(Pool->mRealPoolStartPtr);
		// 查找空闲的块
		for ( int i = 0; i < Pool->mTotalBlockCount; ++i )
		{
			if( Pool->mFlags[i] == 0 )
			{
				Pool->mFlags[i] = 1;
				retFree = (unsigned char*)(realStartPtr + i * blocksize);
				memset( retFree, 0, blocksize );
				break;
			}
		}
		assert( retFree );
		// 计算剩余的空闲块数
		int freeCount = 0;
		if( Pool->mTotalBlockCount > Pool->mBlockTakenCount )
			freeCount = Pool->mTotalBlockCount - Pool->mBlockTakenCount;

#ifdef POOL_LEVEL_CHECK
		PoolFirstLevelSearchMap& PIndirect = GetMemoryPoolSearchMap();
		PoolInfo *pCur = PIndirect.FindPoolInfo( retFree );
		if( pCur != Pool )
		{
		int a = 0;
		}
		assert( pCur == Pool );
#endif

		// 如果空闲块数为0,表明这个池子全部都被占用了,标记为耗尽
		if(freeCount == 0)
		{
			MarkPoolUsefulOrExausted( Pool, ePS_Exausted );
			--freePoolCount;
		}

#if ENABLE_MEMPOOL_DBG
		dbg_print( "Alloc mem at [%p], size [%d], create in pool [%d] subpool [%p][%d][%d][%d]\n", 
			retFree, blocksize, poolIndex, Pool, Pool->mFirstLevel, Pool->mSecondLevel, Pool->mSubID );
#endif

		return retFree;
	}

	size_t MemoryPool::GetMemSize(void *p)
	{
		PoolFirstLevelSearchMap &PIndirect = GetMemoryPoolSearchMap();
		PoolInfo* Pool = PIndirect.FindPoolInfo( p );
		if( Pool == NULL )
			return 0;
		if(!Pool->mpMemPool)
			return 0;
		return Pool->mpMemPool->blocksize;
	}

	bool MemoryPool::Free(void *p)
	{
#ifndef USE_MEMORYPOOL
		InnerFree(p);
		return true;
#endif

		PoolFirstLevelSearchMap &PIndirect = GetMemoryPoolSearchMap();
		PoolInfo* Pool = PIndirect.FindPoolInfo( p );
		if( Pool )
		{
			assert( Pool->mSizePerBlock <= MAX_POOL_BLOCK_SIZE );
			Pool->mpMemPool->_InnerFree(p, Pool);
			return true;
		}
		return false;
	}

	void MemoryPool::_InnerFree(void *p, PoolInfo *pool)
	{
		if(p == NULL)
			return;

		if(blocksize >= MAX_POOL_BLOCK_SIZE)
		{
			InnerFree(p);
			return;
		}

		// 找到是在哪个内存块分配的
		unsigned long ptrValue = PtrToUlong(p);
		assert( pool->mPtrStartValue <= ptrValue && ptrValue < pool->mPtrEndValue );
		int blockID = (ptrValue - pool->mPtrStartValue) / pool->mSizePerBlock;
		assert( pool->mFlags[blockID] == 1 );
		// 将该块标记为0
		pool->mFlags[blockID] = 0;

		// 更新内存池使用情况
		MEMPOOL_SAT(RemoveUseSize(blocksize));
		MEMPOOL_SAT(usesize -= blocksize);
		// 占用块数减1
		--(pool->mBlockTakenCount);

		// 标记内存池子可以用于分配
		if( pool->mIsExausted )
		{
			MarkPoolUsefulOrExausted( pool, ePS_CanUse );
			++freePoolCount;
		}

#if ENABLE_MEMPOOL_DBG
		dbg_print( "Free mem at [%p], size [%d], create in pool [%d] subpool [%p][%d][%d][%d]\n", 
			p, blocksize, poolIndex, pool, pool->mFirstLevel, pool->mSecondLevel, pool->mSubID );
#endif

#if ENABLE_MEMPOOL_AUTO_RELEASE
		// 如果当前内存池子没有被引用,并且还有2个以上的可用池子,那个这个池子就可以销毁交还给系统
		if(pool->mBlockTakenCount == 0 && freePoolCount > 2)
		{
			// 标记池子待销毁
			MarkPoolUsefulOrExausted( pool, ePS_NeedDestroy );
			pool->mpMemPool = NULL;
			MEMPOOL_SAT(RemoveAllocSize(pool->mPoolTotalSize));
			MEMPOOL_SAT(allocsize -= pool->mPoolTotalSize);
			UNUSED_ARG(pool->mPoolTotalSize);
			// 销毁池子
			PoolRealFree(pool->mRealPoolStartPtr);
			pool->mRealPoolStartPtr = NULL;
			// 释放池子信息
			GetMemoryPoolSearchMap().DestroyPoolInfo( pool );
			--freePoolCount;
		}
#endif
	}

	//////////////////////////////////////////////////////////////////////////

#define ARRAY_ELEMENTS(array)			(sizeof(array)/sizeof(array[0]))

	const size_t MEM_POOL_SIZE =			ROUNDNUM2(sizeof(MemoryPool), 16);

	SystemMemoryPools::SystemMemoryPools()
	{
		memset(m_Pools, 0, sizeof(m_Pools));
	}

	SystemMemoryPools::~SystemMemoryPools()
	{
		size_t pool_count = ARRAY_ELEMENTS(m_Pools);
		for(size_t i=0; i<pool_count; i++)
		{
			if(m_Pools[i])
			{
				RemovePoolInnerAllocSize( MEM_POOL_SIZE );
				// Note: don't use delete, it may be over write.
				//	delete m_Pools[i];
				m_Pools[i]->MemoryPool::~MemoryPool();
				//aligned_free(m_Pools[i]);
				InnerFree( m_Pools[i] );
			}
		}
	}

	MemoryPool* SystemMemoryPools::GetPool(size_t blocksize)
	{
		size_t index;
		if(blocksize + 4 >= MAX_POOL_BLOCK_SIZE)
			index = ARRAY_ELEMENTS(m_Pools) - 1;
		else
			index = ROUNDNUM2(blocksize, 16) / 16 - 1;
		assert(index < ARRAY_ELEMENTS(m_Pools));

		if(m_Pools[index] == NULL)
		{		
			// Note: don't use new, it may be over write.
			//m_Pools[index] = (MemoryPool*)aligned_malloc(MEM_POOL_SIZE, 16);
			m_Pools[index] = (MemoryPool*)InnerMalloc( MEM_POOL_SIZE );
			assert( m_Pools[index] );
			AddPoolInnerAllocSize( MEM_POOL_SIZE );

#if AIR_TARGET_PLATFORM == AIR_PLATFORM_WIN32
			m_Pools[index]->MemoryPool::MemoryPool(blocksize);
#else
			m_Pools[index]->Init(blocksize);
#endif
			m_Pools[index]->poolIndex = index;
		}

		MemoryPool* pool = m_Pools[index];
		size_t poolBlockSize = pool->GetBlockSize();
		assert( (poolBlockSize >= blocksize) && ((poolBlockSize - 16) <= blocksize) );

		return pool;
	}

	SystemMemoryPools& SystemMemoryPools::GetSystemPools()
	{
		static SystemMemoryPools sPools;
		return sPools;
	}

	void* SystemMemoryPools::Alloc( size_t blocksize, MemSource &ms )
	{
		void *ptr = NULL;
		bool innerCreate = false;
		MemoryPool* pool = NULL;
		if( blocksize > MAX_POOL_BLOCK_SIZE )
		{
			ptr = (void*)InnerMalloc( blocksize );
			innerCreate = true;
			ms = MS_System;
		}
		else
		{
			pool = GetMemoryPool( blocksize );
			if( !pool )
			{
				ptr = (void*)InnerMalloc( blocksize );
				innerCreate = true;
				ms = MS_System;
			}
			else
			{
				assert( pool->GetBlockSize() < MAX_POOL_BLOCK_SIZE );
				ptr = (void*)pool->Alloc();
				if( !ptr )
				{
					ptr = (void*)InnerMalloc( blocksize );
					innerCreate = true;
					ms = MS_System;
				}
				else
				{
					innerCreate = false;
					ms = MS_Pool;
				}
			}
		}
#if ENABLE_MEMPOOL_DBG
		if( innerCreate )
			dbg_print( "Alloc mem at [%p], size [%d], lowlevel create\n", ptr, blocksize );
#endif
		assert( ptr );
		return ptr;
	}

	void SystemMemoryPools::Free( void* ptr, MemSource &msType )
	{		
		if( MemoryPool::Free( ptr ) == false )
		{
			InnerFree( ptr );
			msType = MS_System;
		}
		else
			msType = MS_Pool;
	}

	void SystemMemoryPools::SnapMemoryPool( const char* szLogPath )
	{
		MemProfilerOutPutToLog( szLogPath, "\t\t<MemoryPool TotalSystemAllocSize=\"%s\" PoolSystemAllocSize=\"%s\" PoolInnerSystemAllocSize=\"%s\" PoolUsageSize=\"%s\">", 
			 GetFormatMemSizeStr(g_MemoryPoolSystemAllocSize+g_MemoryPoolInnerSystemAllocSize).c_str(),  GetFormatMemSizeStr(g_MemoryPoolSystemAllocSize).c_str(),  GetFormatMemSizeStr(g_MemoryPoolInnerSystemAllocSize).c_str(),  GetFormatMemSizeStr(g_MemoryPoolUseSize).c_str()  );
		for ( int i = 0; i < MAX_POOL_COUNT; ++i )
		{
			MemoryPool* pool = m_Pools[i];
			if( !pool )
				continue;
			MemProfilerOutPutToLog( szLogPath, "\t\t\t<Pool blockSize=\"%d(byte)\" allocSize=\"%d(byte)\" useSize=\"%d(byte)\" UtilizationRatio=\"%.3f\">",
				pool->GetBlockSize(), pool->GetTotalAllocSize(), pool->GetTotalUseSize(), (float)pool->GetTotalUseSize()/(float)pool->GetTotalAllocSize() );
			PoolInfo* curPool = pool->GetFirstPool();
			while( curPool )
			{
				MemProfilerOutPutToLog( szLogPath, "\t\t\t\t<SubPool blockSize=\"%d(byte)\" blockTaken=\"%d\" blockTotal=\"%d\" UtilizationRatio=\"%.3f\"/>", 
					curPool->mSizePerBlock, curPool->mBlockTakenCount, curPool->mTotalBlockCount, (float)curPool->mBlockTakenCount/(float)curPool->mTotalBlockCount );
				curPool = curPool->mNextPool;
			}
			MemProfilerOutPutToLog( szLogPath, "\t\t\t</Pool>" );
		}
		MemProfilerOutPutToLog( szLogPath, "\t\t</MemoryPool>" );
	}

	MemoryPool* GetMemoryPool(size_t blocksize)
	{
		size_t _blocksize = ((blocksize+0xF) & ~0xF);
		return SystemMemoryPools::GetSystemPools().GetPool(_blocksize);
	}

	size_t GetMemoryPoolAllocSize()
	{
		return g_MemoryPoolSystemAllocSize;
	}

	size_t GetMemoryPoolUseSize()
	{
		return g_MemoryPoolUseSize;
	}

	size_t GetMemoryPoolInnerAllocSize()
	{
		return g_MemoryPoolInnerSystemAllocSize;
	}
}
