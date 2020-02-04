// ***************************************************************
//  文件: TimeBaseProfile.h
//  日期: 2010-7-21
//  作者: huangjunjie
//  说明: time base performance profile
// ***************************************************************

#pragma once


#include "EngineConfig.h"

#if AIR_TARGET_PLATFORM == AIR_PLATFORM_WIN32

	#include <Windows.h>
	#include <stdio.h>
	#include <stdlib.h>
	#include <string>
	#include <map>
	#include <list>
	#include <stack>

	#include "DebugTools.h"


	#define _PROFILE_TEST_


	class EngineExport ObjectProfile
	{
		ObjectProfile( void )
		{
			m_bShowInfo = false;
		}

	public:
		void Add( const AIRString& szRTTName )
		{
			MAP_RTT_OBJ_DRAW_INFO::iterator it = m_mapAllInfo.find( szRTTName );
			if ( it == m_mapAllInfo.end() )
			{
				m_mapAllInfo.insert( std::make_pair(szRTTName, m_CounterMap) );
				return;
			}
		}

		void Clear( void )
		{
			m_szInfo.clear();
			m_mapAllInfo.clear();
			m_CounterMap.clear();
		}

		void GrabInfo( void )
		{
			m_szInfo = "=============================\n";
			MAP_RTT_OBJ_DRAW_INFO::iterator it = m_mapAllInfo.begin();
			for ( ; it != m_mapAllInfo.end(); ++it )
			{
				m_szInfo += GetString( "@ RTT [%s] : \n", it->first.c_str() );
				MAP_OBJ_DRAW_COUNT::iterator odc_it = it->second.begin();
				for ( ; odc_it != it->second.end(); ++odc_it )
				{
					m_szInfo += GetString( " object [%s], count [%d]\n", odc_it->first.c_str(), odc_it->second );
				}
			}
			m_szInfo += "=============================\n";
			if ( m_bShowInfo )
			{
				DT_TO_CONSOLE( LT_NONE, (char*)(m_szInfo.c_str()) );
				DT_TO_DBGSTR( LT_NONE, (char*)(m_szInfo.c_str()) );
				m_bShowInfo = false;
			}
		}

		static ObjectProfile* GetSingleton( void )
		{
			static ObjectProfile inst;
			return &inst;
		}

		void AddCount( const AIRString& ot )
		{
			MAP_OBJ_DRAW_COUNT::iterator it = m_CounterMap.find( ot );
			if ( it == m_CounterMap.end() )
			{
				m_CounterMap.insert( std::make_pair( ot, 1 ) );
			}
			else
			{
				it->second = ++(it->second);
			}
		}

		void OutPutObjectProfileInfo( void )
		{
			m_bShowInfo = true;
		}

	protected:
		typedef AIRMap< AIRString, int > MAP_OBJ_DRAW_COUNT;
		typedef AIRMap< AIRString, MAP_OBJ_DRAW_COUNT > MAP_RTT_OBJ_DRAW_INFO;

		MAP_OBJ_DRAW_COUNT		m_CounterMap;
		MAP_RTT_OBJ_DRAW_INFO	m_mapAllInfo;
		AIRString				m_szInfo;
		bool					m_bShowInfo;
	};
	#define OBJECT_PROFILE (ObjectProfile::GetSingleton())	


	class EngineExport TimeBaseProfile
	{

		TimeBaseProfile( void ) : m_CurTime(0), m_TimeSinceLastFlush(0)
		{
			::QueryPerformanceFrequency( &m_CPUFrequency );
			::QueryPerformanceCounter( &m_StartTime );
		}

	public:
		static TimeBaseProfile* GetSingleton( void )
		{
			static TimeBaseProfile inst;
			return &inst;
		}

		~TimeBaseProfile( void )
		{

		}

		// 刷新计时器,返回当前系统时间(单位:秒)
		double				FlushImpl( void );

		// 刷新计时器,返回上一次刷新到当前刷新的时间段长度(单位:秒)
		double				GetTimeSinceLastFlush( void );

		// 刷新计时器,并将当前的时间点压入堆栈
		double				FlushAndPush( void );

		// 刷新计时器,弹出堆栈的栈顶元素,计算并返回栈顶时间点到当前时间点的时间差
		double				FlushAndPop( void );

	private:
		LARGE_INTEGER		m_CPUFrequency;			// cpu频率
		LARGE_INTEGER		m_StartTime;			// 起始时间
		double				m_CurTime;				// 当前时间
		double				m_TimeSinceLastFlush;	// 与上一次刷新的时间差
		std::stack<double>	m_stackOfTime;			// 时间堆栈

	public:
		u32				m_dwVertexCalCount;
		u32				m_dwWeightCount;
		void				AddVertexCal( void )	{ ++m_dwVertexCalCount; }
		void				AddWeightCal( void )	{ ++m_dwWeightCount; }
		void				ResetAll( void )		{ m_dwVertexCalCount = 0; m_dwWeightCount = 0; }


	};//! end class TimeBaseProfile

	#define TIMEBASEPROFILER	(TimeBaseProfile::GetSingleton())


	static void ReportFuncProfile( const char* szMsg, const char* szFile, const char* szFunc, long line, double fTimeElapse )
	{
		AIRString str(szMsg); 
		AIRString szFmtMsg = GetString(
			"------------------------>\n信息: [ %s ]\n文件: [ %s ]\n行号: [ %d ]\n函数: [ %s ]\n时间: [ %f ]\n------------------------>\n", 
			str.c_str(), 
			szFile, 
			line, 
			szFunc, 
			fTimeElapse); 
		DT_TO_DBGSTR( LT_NONE, (char*)szFmtMsg.c_str() );
	}

	#ifdef _PROFILE_TEST_
		#define BEGIN_OBJECT_PROFILE						OBJECT_PROFILE->Clear();
		#define COUNT_OBJECT( szType )						OBJECT_PROFILE->AddCount( szType );
		#define FINISH_CURRENT_COUNT( szName )				OBJECT_PROFILE->Add( szName );
		#define END_OBJECT_PROFILE							OBJECT_PROFILE->GrabInfo();
		#define REPORT_FUNC_PROFILE( szMsg, dTimeElapse )	ReportFuncProfile( szMsg, __FILE__, __FUNCTION__, __LINE__, dTimeElapse );
		#define BEGIN_PROFILE								(TIMEBASEPROFILER->FlushAndPush())
		#define END_PROFILE									(TIMEBASEPROFILER->FlushAndPop())
		#define BEGIN_FUNC_PROFILE							BEGIN_PROFILE;
		#define END_FUNC_PROFILE							END_PROFILE
		#define END_FUNC_PROFILE_WITH_REPORT(lpCTR)			ReportFuncProfile( lpCTR, __FILE__, __FUNCTION__, __LINE__, END_PROFILE );
	#else
		#define BEGIN_OBJECT_PROFILE						
		#define COUNT_OBJECT( szType )						
		#define FINISH_CURRENT_COUNT( szName )				
		#define END_OBJECT_PROFILE							
		#define REPORT_FUNC_PROFILE( szMsg, dTimeElapse )	
		#define BEGIN_PROFILE								
		#define END_PROFILE									
		#define BEGIN_FUNC_PROFILE							
		#define END_FUNC_PROFILE							
		#define END_FUNC_PROFILE_WITH_REPORT(lpCTR)			
	#endif

#else

	#define BEGIN_OBJECT_PROFILE						
	#define COUNT_OBJECT( szType )						
	#define FINISH_CURRENT_COUNT( szName )				
	#define END_OBJECT_PROFILE							
	#define REPORT_FUNC_PROFILE( szMsg, dTimeElapse )	
	#define BEGIN_PROFILE								
	#define END_PROFILE									
	#define BEGIN_FUNC_PROFILE							
	#define END_FUNC_PROFILE							
	#define END_FUNC_PROFILE_WITH_REPORT(lpCTR)		

#endif


//////////////////////////////////////////////////////////////////////////

class EngineExport CallStackItem
{
public:
	void*	mPtr;
	char	mFile[MAX_PATH];
	int		mLine;

	CallStackItem(void* ptr);
};
typedef std::stack<double>		TimeStack;
typedef std::list<CallStackItem>	CallStackItemList;

class EngineExport ProfileEntry
{
public:
	std::string		mFunc;
	std::string		mFile;
	int				mLine;
	std::string		mName;
	double			mScopeTime;
	double			mScopeDeltaTime;
	int				mHitCount;
	TimeStack		mEnterScopeTimeStack;

	ProfileEntry(){};
	ProfileEntry(const std::string& name, const std::string& func, const std::string& file, int line);
	void Reset();
	void Begin();
	void End();
};

class CallStackNode;
typedef std::list<CallStackNode*> CallStackNodeList;

class EngineExport CallStackNode
{
public:
	CallStackItemList	mCallStacks;
	CallStackNodeList	mChildNodes;
	CallStackNode		*mParent;
	double				mNodeTime;
	ProfileEntry*		mpEntry;

	CallStackNode(ProfileEntry* ent);
	~CallStackNode();
	void ReportLastFrameStatus(const std::string& preStr, const char *szLogFile);
};	

class EngineExport ScopeCollector
{
public:
	static CallStackNode	*sLastOperatingNode;
	ProfileEntry		*mpEntry;
	bool mIsFuncEntry;

	ScopeCollector(ProfileEntry *entry, bool isFuncEntry);
	~ScopeCollector();

	void BeginScope();
	void EndScope();
};

class EngineExport Profiler
{
	Profiler();
public:
	friend class ProfileEntry;
	friend class ScopeCollector;

	~Profiler();

	static Profiler& Instance();
	void InitProfiler();
	void ShutdownProfiler();

	void CollectDataAtTheEndOfOneFrame();
	void SetEnable(bool val);
	bool GetEnable()const;
	void StartProfile();
	bool GetProfiling()const;
	double GetTotalTime()const;
	int GetTotalFrame() const;

	void SetProfileLog( const char* szLog );
	void SnapToLog() { mNeedSnapLog = true; }

protected:
	void ReportLastFrameStatus();
	ProfileEntry* FindProfileEntry(const std::string& name);
	ProfileEntry* FirstEntry();
	ProfileEntry* NextEntry();
	void AddTopLevelNode(CallStackNode* node);
	bool RegisterProfileEntry(ProfileEntry* entry);

private:
	typedef std::list<ProfileEntry*> ProfileEntryList;
	typedef std::map<std::string, ProfileEntry*> ProfileEntryMap;
	ProfileEntryMap::iterator mItr;
	// 入口映射表
	ProfileEntryMap mProfileEntryMap;
	// 入口列表
	ProfileEntryList mProfileEntryList;

	bool mEnable;
	bool mIsProfiling;
	double mTotalTime;
	int mTotalFrameCount;

	CallStackNodeList mTopLevelNodes;
	bool mHasInit;
	std::string mLogFile;
	bool mNeedSnapLog;
};

#define ENABLE_PROFILE 1

#if ENABLE_PROFILE

// 初始化性能分析器
#	define PROFILE_INIT	\
		Profiler::Instance().InitProfiler();\
		Profiler::Instance().SetEnable( true );	

// 分析函数
#	define PROFILE_FUNC(name) \
		static ProfileEntry pe_##name( #name, __FUNCTION__, __FILE__, __LINE__ );\
		ScopeCollector scope_collector_##name( & pe_##name, true );

// 开始代码块数据收集
#	define PROFILE_SCOPE_START(name) \
		static ProfileEntry pe_##name( #name, __FUNCTION__, __FILE__, __LINE__ );\
		ScopeCollector scope_collector_##name( & pe_##name, false );\
		scope_collector_##name.BeginScope();

// 结束代码块数据收集
#	define PROFILE_SCOPE_END(name) \
		scope_collector_##name.EndScope();

// 结束一帧的性能数据收集
#	define PROFILE_FRAME_COMPLETED \
		Profiler::Instance().CollectDataAtTheEndOfOneFrame();

// 关闭性能分析器
#	define PROFILE_SHUTDOWN \
		Profiler::Instance().ShutdownProfiler();

#else

#	define PROFILE_INIT
#	define PROFILE_FUNC(name)
#	define PROFILE_SCOPE_START(name)
#	define PROFILE_SCOPE_END(name)
#	define PROFILE_FRAME_COMPLETED
#	define PROFILE_SHUTDOWN

#endif

