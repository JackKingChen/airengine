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

// �ڴ���Դ
enum MemSource
{
	// δ����
	MS_Unknown							= 0,	
	// ��ϵͳ�������
	MS_System,		
	// ���ڴ���з������
	MS_Pool,		
};

//////////////////////////////////////////////////////////////////////////
// �ڴ�����Լ����ԵĿ��ƿ���
//////////////////////////////////////////////////////////////////////////

// δ����ĵײ��������
#define LLAT_None						0		
// ʹ��nedmalloc��ϵͳ�����ڴ�
#define LLAT_NED						1	
// ʹ��Ĭ�ϵ�malloc��ϵͳ�����ڴ�
#define LLAT_STD						2			
#define TARGET_LLAT						LLAT_NED

// �Ƿ���ڴ��������
#define ENABLE_MEMORY_PROFILER			1
// �Ƿ���������
#define ENABLE_DEEP_PROFILE				1
// �Ƿ��STL���������
#define ENABLE_STL_DEEP_PROFILE			ENABLE_DEEP_PROFILE
// �Ƿ�ʹ���ڴ��
#define USE_MEMORYPOOL					0
// �Ƿ�ʹ���ڴ������
#define USE_MEMORYMANAGER				0
// �Ƿ���ڴ��������ʹ���ռ�
#define ENABLE_MEM_MGR_COLLECT			1
// �Ƿ���ڴ�ص��Զ��ͷ�
#define ENABLE_MEMPOOL_AUTO_RELEASE		1

//////////////////////////////////////////////////////////////////////////
// һЩ����
//////////////////////////////////////////////////////////////////////////

// �Ƿ���ڴ�ش�log����
#define ENABLE_MEMPOOL_DBG				0
// �����ڴ��ַ�����󳤶�
#define DBG_MEM_NAME_LENGTH				128
// �Ƿ�ʹ�ù̶���ջ����
#define USE_FIX_CALLSTACK_COUNT			0
// ���ַ�����¼�Ƿ�ʹ�ö����ַ�����
#define USE_FIX_NAME_LEN				0
// �Ƿ�ǿ��STLʹ��ϵͳ�ڴ����
#define STL_USE_LL_ALLOC				0
// �ڴ�������Ƿ�ʹ��ϵͳ�ڴ����
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
	// ���ļ�
	fout.open( szLogPath, std::ios::app );
	// д���ļ�
	fout<<szBuffer<<"\n";		
	// ��д����������д���������ڱ����ļ��򿪵�����±����ļ���
	fout<<std::flush;	
	// �ر�	
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


	// ÿ���ڴ����65536��byte
	// �����ÿ���������MAX_POOL_BLOCK_SIZE���ֽ�
	namespace
	{
		enum
		{
			MAX_POOL_BLOCK_SIZE = (65536-4)/2+1
		};
	};

	const size_t MAX_POOL_COUNT = ((MAX_POOL_BLOCK_SIZE-1)+15)/16+1;

	class MemoryPool;

	// �ڴ��ʹ��״̬
	enum PoolStatus
	{
		// �����ڷ���
		ePS_CanUse,
		// �ľ�,���ɷ�����
		ePS_Exausted,
		// ��ǰ���Ӳ���ʹ��״̬,��������֮,���ڴ潻����ϵͳ
		ePS_NeedDestroy,
	};

	// �ڴ����Ϣ
	struct PoolInfo
	{
		// һ������
		size_t			mFirstLevel;
		// ��������
		size_t			mSecondLevel;
		// ������
		size_t			mSubID;
		// ��ǰ�������ж��ٸ��ӿ鱻�����ȥ��
		size_t			mBlockTakenCount;
		// �ڴ���ӿ������
		size_t			mTotalBlockCount;
		// �����ڴ�ص���ʼ��ַ
		void			*mRealPoolStartPtr;
		// ��Ǹ����ӿ��ʹ�����,��mTotalBlockCount���ֽ�,ÿ���ֽڶ�Ӧһ���ڴ��ӿ�,0��ʾ����,1��ʾ��ռ��
		char			*mFlags;
		// �ڴ�ض���
		MemoryPool		*mpMemPool;
		// ÿ����Ĵ�С(byte)
		size_t			mSizePerBlock;
		// �����صĴ�С(byte)
		size_t			mPoolTotalSize;
		// ���Ƿ��Ѿ��ľ�
		bool			mIsExausted;
		// �����ڴ����ʼ��ֵַ
		unsigned long	mPtrStartValue;
		// �����ڴ�Ľ�����ֵַ
		unsigned long	mPtrEndValue;
		// ��һ���ڴ����
		PoolInfo		*mPrevPool;
		// ��һ���ڴ����
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

	// 2���ڴ�ز��ұ�
	// �����ַ�� 0xAABBCCDD, �ü�������Ӧ���� BB ��ֵ
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

	// 1���ڴ�ز��ұ�
	// �����ַ�� 0xAABBCCDD, �ü�������Ӧ���� AA ��ֵ
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

	// �ڴ��
	class MemoryPool
	{
	public:
		MemoryPool(size_t BlockSize);
		void					Init(size_t BlockSize);
		// �����ڴ����ʹ��״̬
		void					MarkPoolUsefulOrExausted( PoolInfo* pInfo, PoolStatus status );
		// �����ڴ�
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
		// �ڴ������
		size_t					poolIndex;
		// �ڴ����ÿ���ӿ�Ĵ�С
		size_t					blocksize;
		// �����ڷ�����ڴ������
		size_t					freePoolCount;
		// �׸����ڴ��
		PoolInfo				*mFirstPool;
		// �Ƿ��Ѿ���ʼ���ڴ�ز��ұ�
		static bool				sHasInitPoolSearchMap;
		// �ڴ�ز��ұ�
		static PoolFirstLevelSearchMap* sPoolSearchMap;
		// �ڴ��ʹ����
		size_t					usesize;
		// �ڴ�ط�����
		size_t					allocsize;
	};

	// ϵͳ�ڴ�ع�����
	class SystemMemoryPools
	{
		MemoryPool				*m_Pools[MAX_POOL_COUNT];

	public:
		SystemMemoryPools();
		~SystemMemoryPools();
		// ���ݴ�С��ȡ�ڴ��
		MemoryPool*				GetPool(size_t blocksize);
		// �����ڴ�
		void*					Alloc( size_t blocksize, MemSource &ms );
		// �ͷ��ڴ�
		void					Free( void* ptr, MemSource &msType );
		// ���ڴ����Ϣ��ӡ��log�ļ���
		void					SnapMemoryPool( const char* szLogPath );
		static SystemMemoryPools& GetSystemPools();
	};

	MemoryPool*			GetMemoryPool(size_t blocksize);
	// ��ȡ�ڴ�ط�������
	size_t      		GetMemoryPoolAllocSize();
	// ��ȡ�ڴ��ʹ������
	size_t      		GetMemoryPoolUseSize();
	// ��ȡ�ڴ���ڲ��ڴ�����
	size_t				GetMemoryPoolInnerAllocSize();

	// �ڴ����ϵͳ������ڴ�����
	static size_t g_MemoryPoolSystemAllocSize = 0;
	// �ڴ�ر�ʹ�õ�����
	static size_t g_MemoryPoolUseSize = 0;
	// �ڴ���������ĵ�ϵͳ�ڴ�����
	static size_t g_MemoryPoolInnerSystemAllocSize = 0;
};
//////////////////////////////////////////////////////////////////////////


_Mutex_Body_					g_ProfilerAllocLock;
_Mutex_Body_					g_HLVEngineAllocLock;
_Mutex_Body_					g_HLVSTLAllocLock;
_Mutex_Body_					g_MemAllocLock;



// ��¼ÿ���ڴ�������Ϣ
struct EngineExport AllocationData
{
	MemSource		memSourceType;						// �ڴ���Դ
	AllocatorType	allocatorType;						// ���ĸ��ڴ�����������
	void*			dataPtr;							// ����õ���ָ��
	unsigned int	dataSize;							// ��������ݴ�С
#if USE_FIX_NAME_LEN
	char			typeName[DBG_MEM_NAME_LENGTH];		// ��������ͱ�ʶ(Ʃ������)
	char			fileName[DBG_MEM_NAME_LENGTH];		// ���ĸ��ļ��������η���
	char			funcName[DBG_MEM_NAME_LENGTH];		// ���ĸ������������η���
#else
	char			*typeName;							// ��������ͱ�ʶ(Ʃ������)
	char			*fileName;							// ���ĸ��ļ��������η���
	char			*funcName;							// ���ĸ������������η���
#endif
	unsigned short	line;								// ���ļ�����һ�з������η���
#if USE_FIX_CALLSTACK_COUNT
	void*			callStack[DBG_MEM_CALLSTACK_NUM];	// �����ʱ��Ķ�ջ׷��
#else
	void			**callStack;
	size_t			callStackCount;
#endif

	AllocationData();
	void Init();
	void Destroy();
};

// �ڴ������ҽڵ�
struct EngineExport AllocationSearchNode
{
	// �ڼ���
	unsigned char			levelIndex;
	// ����
	AllocationSearchNode*   parentLevel;
	// �Ӳ�
	AllocationSearchNode*	childLevel[2];
	// �ڴ������Ϣ
	AllocationData*			value;

	AllocationSearchNode();
	void Init();
	static void OnDestroyChild( AllocationSearchNode* childLvl );
};


// �ڴ���������
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

// ������ռ�õ�ϵͳ�ڴ�
size_t g_ProfilerSystemAllocSize = 0;
// ����ռ�õ�ϵͳ�ڴ�
size_t g_EngineSystemAllocSize = 0;
// STLռ�õ�ϵͳ�ڴ�
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

	// ���ڵ�
	MemProfilerOutPutToLog( memLogFile, "<MemorySnapShot Time=\"%.3f(s)\">", (float)Timer::GetTime() );
	// �ڴ��������Ϣ��ӡ(��¼����ǰ��ϵͳ������ڴ�����)
	MemProfilerOutPutToLog( memLogFile, "\t<MemoryManager TotalSystemAllocSize=\"%s\"/>",  GetFormatMemSizeStr(MemoryManagerAllocator::sTotalAllocSize).c_str() );
	// �����ڴ������ϵͳ�ڴ�ֲ���Ϣ��
	MemProfilerOutPutToLog( memLogFile, "\t<SystemMemoryBrief TotalSystemAllocSize=\"%s\">", 
		 GetFormatMemSizeStr(g_ProfilerSystemAllocSize + g_EngineSystemAllocSize + g_STLSystemAllocSize + AirMemoryPool::g_MemoryPoolInnerSystemAllocSize + AirMemoryPool::g_MemoryPoolSystemAllocSize).c_str() );
	// �ڴ��������ռ��ϵͳ�ڴ�
	MemProfilerOutPutToLog( memLogFile, "\t\t<MemoryProfiler SystemAllocSize=\"%s\"/>",  GetFormatMemSizeStr(g_ProfilerSystemAllocSize).c_str() );
	// �����ڴ��������ռ��ϵͳ�ڴ�
	MemProfilerOutPutToLog( memLogFile, "\t\t<HighLevelEngineAllocator SystemAllocSize=\"%s\"/>",  GetFormatMemSizeStr(g_EngineSystemAllocSize).c_str() );
	// STL�ڴ��������ռ��ϵͳ�ڴ�
	MemProfilerOutPutToLog( memLogFile, "\t\t<HighLevelSTLAllocator SystemAllocSize=\"%s\"/>",  GetFormatMemSizeStr(g_STLSystemAllocSize).c_str() );
	// �ڴ�ع�������ռ��ϵͳ�ڴ�
	MemProfilerOutPutToLog( memLogFile, "\t\t<MemoryPool SystemAllocSize=\"%s\">",  GetFormatMemSizeStr(AirMemoryPool::g_MemoryPoolSystemAllocSize+AirMemoryPool::g_MemoryPoolInnerSystemAllocSize).c_str() );
	MemProfilerOutPutToLog( memLogFile, "\t\t\t<Pool SystemAllocSize=\"%s\"/>",  GetFormatMemSizeStr(AirMemoryPool::g_MemoryPoolSystemAllocSize).c_str() );
	MemProfilerOutPutToLog( memLogFile, "\t\t\t<Inner SystemAllocSize=\"%s\"/>",  GetFormatMemSizeStr(AirMemoryPool::g_MemoryPoolInnerSystemAllocSize).c_str() );
	MemProfilerOutPutToLog( memLogFile, "\t\t</MemoryPool>" );
	MemProfilerOutPutToLog( memLogFile, "\t</SystemMemoryBrief>" );

	MemProfilerOutPutToLog( memLogFile, "\t<AllocatorBrief>" );
	// MemoryProfiler���ڴ�ռ�����
	MemProfilerOutPutToLog( memLogFile, "\t\t<MemoryProfiler totalAllocSize=\"%s\" allocSizeFromSystem=\"%s\" allocSizeFromPool=\"%s\"/>",
		 GetFormatMemSizeStr(sProfilerTotalAllocBytes).c_str(),  GetFormatMemSizeStr(g_ProfilerSystemAllocSize).c_str(),  GetFormatMemSizeStr(sProfilerTotalAllocBytes-g_ProfilerSystemAllocSize).c_str() );
	// HighLevelEngineAllocator���ڴ�ռ�����
	MemProfilerOutPutToLog( memLogFile, "\t\t<HighLevelEngineAllocator allocSize = \"%s\" allocCount = \"%d\" allocSizeFromSystem=\"%s\" allocSizeFromPool=\"%s\"/>", 
		 GetFormatMemSizeStr(HighLevelEngineAllocator::sTotalAllocSize).c_str(), HighLevelEngineAllocator::sTotalAllocCount,  GetFormatMemSizeStr(g_EngineSystemAllocSize).c_str(),  GetFormatMemSizeStr(HighLevelEngineAllocator::sTotalAllocSize-g_EngineSystemAllocSize).c_str());
	// HighLevelSTLAllocator���ڴ�ռ�����
	MemProfilerOutPutToLog( memLogFile, "\t\t<HighLevelSTLAllocator allocSize = \"%s\" allocCount = \"%d\" allocSizeFromSystem=\"%s\" allocSizeFromPool=\"%s\"/>", 
		 GetFormatMemSizeStr(HighLevelSTLAllocator::sTotalAllocSize).c_str(), HighLevelSTLAllocator::sTotalAllocCount,  GetFormatMemSizeStr(g_STLSystemAllocSize).c_str(),  GetFormatMemSizeStr(HighLevelSTLAllocator::sTotalAllocSize-g_STLSystemAllocSize).c_str());
	// MemoryPool���ڴ�ռ�����
	AirMemoryPool::SystemMemoryPools::GetSystemPools().SnapMemoryPool( memLogFile );
	MemProfilerOutPutToLog( memLogFile, "\t</AllocatorBrief>" );

	// ��¼ͼ���ڴ�ʹ�����
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

	// ���ҵ�ǰĿ¼��DLL
	s_DbgHelpLib = LoadLibrary(szDbgName);
	if(s_DbgHelpLib == NULL)
	{
		// ʹ��ϵͳ��DLL
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
		// ���ӿ���,�����ӴӺľ�����ժ�����ŵ����ö��е�ͷ��
		if( status == ePS_CanUse )
		{
			Pool->mIsExausted = false;
		}
		// ���Ӻľ�, �����Ӵӿ��ö���ժ�����ŵ��ľ����е�ͷ��
		else if( status == ePS_Exausted )
		{
			Pool->mIsExausted = true;
		}
		// ������Ҫ����,�ӿ��ö��кͺľ�����ժ��
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

		// �ҵ�ǰ�ɷ�����ڴ��
		PoolInfo *freePool = mFirstPool;
		PoolInfo *Pool = NULL;
		while( freePool && freePool->mIsExausted )
		{
			freePool = freePool->mNextPool;
		}
		if(!freePool || freePool->mIsExausted )
		{
			const size_t POOL_MAX_SIZE = 0xFFFF;
			// ��������ڴ���Ի��ֶ��ٸ���
			size_t Blocks = (POOL_MAX_SIZE - 4) / blocksize;
			// �����ڴ�صĴ�С
			size_t Bytes = Blocks * blocksize;

			// ����һ�����е��ڴ��,���ڷָ���û�ʹ��
			void *pRealPool = PoolRealAlloc(Bytes);
			assert( pRealPool );
			// ����Blocks���ֽ�,��¼ÿ���ڴ���ʹ�����(0:δʹ��;1:�Ѿ���ʹ��)
			char *pFlags = (char*)PoolRealAlloc(Blocks);
			assert( pFlags );
			// ȫ�����Ϊ0,�������еĿ鶼����ʹ��
			memset( pFlags, 0, Blocks );

			MEMPOOL_SAT(AddAllocSize(Bytes));
			MEMPOOL_SAT(allocsize += Bytes);
			UNUSED_ARG(Bytes);
			AddPoolInnerAllocSize(Blocks);			

			PoolFirstLevelSearchMap& PIndirect = GetMemoryPoolSearchMap();
			// �����ڴ����Ϣ
			Pool = PIndirect.AddPoolInfo( pRealPool );
			assert( Pool );
			// �����ڴ����ռ�õ���ʼ�ͽ�����ַ��ֵ
			Pool->mPtrStartValue = PtrToUlong(pRealPool);
			Pool->mPtrEndValue = Pool->mPtrStartValue + Bytes;
			// ��ռ�õĿ���
			Pool->mBlockTakenCount = 0;
			// �ڴ�ص���ʼ��ַ
			Pool->mRealPoolStartPtr = pRealPool;
			// �ڴ�ض���
			Pool->mpMemPool = this;
			// ÿ�����Ĵ�С
			Pool->mSizePerBlock = blocksize;
			// �����ڴ����ռ���ֽ���
			Pool->mPoolTotalSize = Bytes;
			// �ܵ��������
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

		// ռ�ÿ�����һ
		Pool->mBlockTakenCount++;
		assert( Pool->mTotalBlockCount >= Pool->mBlockTakenCount );
		unsigned char *retFree = NULL;
		unsigned char *realStartPtr = (unsigned char*)(Pool->mRealPoolStartPtr);
		// ���ҿ��еĿ�
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
		// ����ʣ��Ŀ��п���
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

		// ������п���Ϊ0,�����������ȫ������ռ����,���Ϊ�ľ�
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

		// �ҵ������ĸ��ڴ������
		unsigned long ptrValue = PtrToUlong(p);
		assert( pool->mPtrStartValue <= ptrValue && ptrValue < pool->mPtrEndValue );
		int blockID = (ptrValue - pool->mPtrStartValue) / pool->mSizePerBlock;
		assert( pool->mFlags[blockID] == 1 );
		// ���ÿ���Ϊ0
		pool->mFlags[blockID] = 0;

		// �����ڴ��ʹ�����
		MEMPOOL_SAT(RemoveUseSize(blocksize));
		MEMPOOL_SAT(usesize -= blocksize);
		// ռ�ÿ�����1
		--(pool->mBlockTakenCount);

		// ����ڴ���ӿ������ڷ���
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
		// �����ǰ�ڴ����û�б�����,���һ���2�����ϵĿ��ó���,�Ǹ�������ӾͿ������ٽ�����ϵͳ
		if(pool->mBlockTakenCount == 0 && freePoolCount > 2)
		{
			// ��ǳ��Ӵ�����
			MarkPoolUsefulOrExausted( pool, ePS_NeedDestroy );
			pool->mpMemPool = NULL;
			MEMPOOL_SAT(RemoveAllocSize(pool->mPoolTotalSize));
			MEMPOOL_SAT(allocsize -= pool->mPoolTotalSize);
			UNUSED_ARG(pool->mPoolTotalSize);
			// ���ٳ���
			PoolRealFree(pool->mRealPoolStartPtr);
			pool->mRealPoolStartPtr = NULL;
			// �ͷų�����Ϣ
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
