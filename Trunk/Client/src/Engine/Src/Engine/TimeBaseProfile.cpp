#include "stdafx.h"
#include "TimeBaseProfile.h"

#if AIR_TARGET_PLATFORM == AIR_PLATFORM_WIN32

double				TimeBaseProfile::FlushImpl( void )
{
	LARGE_INTEGER t;
	::QueryPerformanceCounter(&t);
	double dt = double(t.QuadPart - m_StartTime.QuadPart) / double( m_CPUFrequency.QuadPart );
	m_TimeSinceLastFlush = dt - m_CurTime;
	m_CurTime = dt;
	return m_CurTime;
}

double				TimeBaseProfile::GetTimeSinceLastFlush( void )
{
	return m_TimeSinceLastFlush;
}

double				TimeBaseProfile::FlushAndPush( void )
{
	double fCurTime = FlushImpl();
	m_stackOfTime.push( fCurTime );	
	return fCurTime;
}

double				TimeBaseProfile::FlushAndPop( void )
{
	ASSERT_MSG( (!m_stackOfTime.empty()), ("time stack is empty!") );
	double fCurTime = FlushImpl();
	double fLastTime = m_stackOfTime.top();
	m_stackOfTime.pop();
	return fCurTime - fLastTime;
}

#endif



//////////////////////////////////////////////////////////////////////////

#include <assert.h>
#include <stdlib.h>
#include <memory.h>
#include "Timer.h"

void ProfilerClearLog(const char* szLogPath)
{
	std::ofstream fout;
	fout.open( szLogPath, std::ios::out );
	fout<<std::flush;
	fout.close();
}

void ProfilerOutPutToLog( const char* szLogPath, char* buf, ... )
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

CallStackItem::CallStackItem(void* ptr):mPtr(ptr), mLine(-1)
{
	memset( mFile, 0, sizeof(mFile));
	//MemoryProfiler::GetFileAndLine( mPtr, mFile, mLine );
}



//////////////////////////////////////////////////////////////////////////

ProfileEntry::ProfileEntry(const std::string& name, const std::string& func, const std::string& file, int line)
	: mScopeTime(0), mScopeDeltaTime(0), mName(name), mFunc(func), mFile(file), mLine(line), mHitCount(0)
{
	if (!Profiler::Instance().RegisterProfileEntry(this))
	{
		assert(false);
	}		
}

void ProfileEntry::Reset()
{
	mHitCount = 0;
	mScopeTime = 0;
	mScopeDeltaTime = 0;
	assert( mEnterScopeTimeStack.empty() );
}

void ProfileEntry::Begin()
{
	if (Profiler::Instance().GetEnable() == false || Profiler::Instance().GetProfiling() == false)
		return;
	++mHitCount;
	double enterTime = (double)Timer::GetTime();
	mEnterScopeTimeStack.push(enterTime);
}

void ProfileEntry::End()
{
	if (Profiler::Instance().GetEnable() == false || Profiler::Instance().GetProfiling() == false)
		return;
	double enterTime = mEnterScopeTimeStack.top();
	mEnterScopeTimeStack.pop();
	mScopeDeltaTime = (double)Timer::GetTime() - enterTime;
	mScopeTime += mScopeDeltaTime;
}


//////////////////////////////////////////////////////////////////////////

bool ProfileEntryCompare(const ProfileEntry* t1,const ProfileEntry* t2)
{
	return t1->mScopeTime > t2->mScopeTime;  
}

bool CallStackNodeCompare(const CallStackNode* t1,const CallStackNode* t2)
{  
	return t1->mNodeTime > t2->mNodeTime;  
}  

CallStackNode::CallStackNode(ProfileEntry* ent):mParent(NULL), mNodeTime(0.0), mpEntry(ent)
{
	// 获取当前堆栈信息
	mChildNodes.clear();
	{
#if AIR_TARGET_PLATFORM == AIR_PLATFORM_WIN32
		DWORD _ebp, _esp;
		__asm mov _ebp, ebp;
		__asm mov _esp, esp;
		while(1)
		{
			void* cs_ptr = (void*)ULongToPtr(*(((DWORD*)ULongToPtr(_ebp))+1));
			mCallStacks.push_back( CallStackItem(cs_ptr) );
			_ebp = *(DWORD*)ULongToPtr(_ebp);
			if(_ebp == 0 || 0 != (_ebp & 0xFC000000) || _ebp < _esp)
				break;
		}
#endif
	}
}

CallStackNode::~CallStackNode()
{
	CallStackNodeList::iterator itb = mChildNodes.begin(), ite = mChildNodes.end();
	for ( ; itb != ite; ++itb )
	{
		CallStackNode* pCur = *itb;
		AIR_SAFE_DELETE( pCur );
	}
	mChildNodes.clear();
	mCallStacks.clear();
	mpEntry = NULL;
	mParent = NULL;
}

void CallStackNode::ReportLastFrameStatus(const std::string& preStr, const char *szLogFile)
{
	ProfilerOutPutToLog( szLogFile, "%s<EntryCallStackNode Name = \"%s\" TimeLength = \"%.3f(s)\" FuncName = \"%s\" File = \"%s\" Line = \"%d\">",
		preStr.c_str(),
		mpEntry->mName.c_str(),
		mNodeTime,
		mpEntry->mFunc.c_str(),
		mpEntry->mFile.c_str(),
		mpEntry->mLine );
	std::string str = preStr + "\t";

	mChildNodes.sort( CallStackNodeCompare );
	CallStackNodeList::iterator itb = mChildNodes.begin(), ite = mChildNodes.end();
	for ( ; itb != ite; ++itb )
	{
		CallStackNode* node = *itb;
		node->ReportLastFrameStatus( str, szLogFile );
	}
	ProfilerOutPutToLog( szLogFile,  "%s</EntryCallStackNode>", preStr.c_str() );
}



//////////////////////////////////////////////////////////////////////////

CallStackNode* ScopeCollector::sLastOperatingNode = NULL;

ScopeCollector::ScopeCollector(ProfileEntry *entry, bool isFuncEntry):mpEntry(entry), mIsFuncEntry(isFuncEntry)
{
	if( mIsFuncEntry )
		BeginScope();
}

ScopeCollector::~ScopeCollector()
{
	if( mIsFuncEntry )
		EndScope();
}

void ScopeCollector::BeginScope()
{
	// 开始本入口的性能数据收集
	mpEntry->Begin();

	// 创建当前堆栈信息节点
	CallStackNode* pNode = new CallStackNode(mpEntry);
	// 在这个节点进来之前没有其他的节点,意味着这是一个顶层节点
	if ( sLastOperatingNode == NULL )
	{
		Profiler::Instance().AddTopLevelNode( pNode );
		pNode->mParent = NULL;
	}
	// 否则把这个节点加到上一次的节点的子节点列表里
	else
	{
		sLastOperatingNode->mChildNodes.push_back( pNode );
		pNode->mParent = sLastOperatingNode;
	}

	if( mIsFuncEntry )
		sLastOperatingNode = pNode;
}

void ScopeCollector::EndScope()
{
	// 结束本入口的性能数据收集
	mpEntry->End();

	// 返回本入口节点的父节点
	sLastOperatingNode->mNodeTime += mpEntry->mScopeDeltaTime;

	if( mIsFuncEntry )
		sLastOperatingNode = sLastOperatingNode->mParent;
}

//////////////////////////////////////////////////////////////////////////

Profiler& Profiler::Instance()
{
	static Profiler sProfiler;
	return sProfiler;
}
Profiler::Profiler() : mEnable(true), mIsProfiling(false), mTotalTime(0), mTotalFrameCount(0)
{
	mHasInit = false;
	mNeedSnapLog = false;
}
Profiler::~Profiler()
{
}
void Profiler::InitProfiler()
{
	mLogFile = "";
	mHasInit = true;
	mTotalTime = Timer::GetTime();
}
void Profiler::ShutdownProfiler()
{
	mHasInit = false;
}
void Profiler::CollectDataAtTheEndOfOneFrame()
{
	ReportLastFrameStatus();
	mTotalTime = Timer::GetTime();
	mTotalFrameCount++;
	StartProfile();
}
ProfileEntry* Profiler::FindProfileEntry(const std::string& name)
{
	ProfileEntry* pEntry = NULL;
	ProfileEntryMap::iterator itr = mProfileEntryMap.find(name);
	if (itr != mProfileEntryMap.end())
	{
		pEntry = itr->second;
	}
	return pEntry;
}
ProfileEntry* Profiler::FirstEntry()
{
	mItr = mProfileEntryMap.begin();
	if (mItr == mProfileEntryMap.end())
		return NULL;
	return mItr->second;
}
ProfileEntry* Profiler::NextEntry()
{
	++mItr;
	if (mItr == mProfileEntryMap.end())
		return NULL;
	return mItr->second;
}
bool Profiler::RegisterProfileEntry(ProfileEntry* entry)
{
	ProfileEntryMap::iterator itr = mProfileEntryMap.find(entry->mName);
	if (itr != mProfileEntryMap.end())
		return false;
	mProfileEntryMap.insert(ProfileEntryMap::value_type(entry->mName, entry));
	mProfileEntryList.push_back( entry );
	return true;
}
void Profiler::SetEnable(bool val)
{
	mEnable = val;
}
bool Profiler::GetEnable()const
{
	return mEnable;
}
void Profiler::StartProfile()
{
	mIsProfiling = true;
	for (ProfileEntryMap::iterator itr = mProfileEntryMap.begin(); itr != mProfileEntryMap.end(); ++itr)
	{
		itr->second->Reset();
	}
	ScopeCollector::sLastOperatingNode = NULL;
	CallStackNodeList::iterator itb = mTopLevelNodes.begin(), ite = mTopLevelNodes.end();
	for ( ; itb != ite; ++itb )
	{
		AIR_SAFE_DELETE( *itb );
	}
	mTopLevelNodes.clear();
}
bool Profiler::GetProfiling()const
{
	return mIsProfiling;
}
double Profiler::GetTotalTime()const
{
	return mTotalTime;
}
int Profiler::GetTotalFrame() const
{
	return mTotalFrameCount;
}

void Profiler::ReportLastFrameStatus()
{
	if( mNeedSnapLog == false )
		return;
	mNeedSnapLog = false;
	if( mLogFile.length() == 0 )
		return;

	ProfilerOutPutToLog( mLogFile.c_str(), "<Frame Index = \"%d\" TimeStamp = \"%.3f(s)\">", mTotalFrameCount, mTotalTime );
	ProfilerOutPutToLog( mLogFile.c_str(), "\t<Entrys>" );

	// 按照占用时间长度从大到小排列,输出每个入口的性能信息
	mProfileEntryList.sort( ProfileEntryCompare );
	ProfileEntryList::iterator pitb = mProfileEntryList.begin(), pite = mProfileEntryList.end();
	for (; pitb != pite; ++pitb)
	{
		ProfileEntry* pe = *pitb;

		ProfilerOutPutToLog( mLogFile.c_str(), "\t\t<Entry Name = \"%s\" TimeLength = \"%.3f(s)\" RunCount = \"%d\" FuncName = \"%s\" File = \"%s\" Line = \"%d\"/>",
			pe->mName.c_str(),
			pe->mScopeTime,
			pe->mHitCount,
			pe->mFunc.c_str(),
			pe->mFile.c_str(),
			pe->mLine);
	}

	ProfilerOutPutToLog( mLogFile.c_str(), "\t</Entrys>" );
	ProfilerOutPutToLog( mLogFile.c_str(), "\t<EntryCallStackInfos>");

	// 按照占用时间长度从大到小排列, 输出调用节点的性能信息
	mTopLevelNodes.sort( CallStackNodeCompare );
	CallStackNodeList::iterator itb = mTopLevelNodes.begin(), ite = mTopLevelNodes.end();
	for ( ; itb != ite; ++itb )
	{
		CallStackNode* pNode = *itb;
		pNode->ReportLastFrameStatus( "\t\t", mLogFile.c_str() );
	}

	ProfilerOutPutToLog( mLogFile.c_str(), "\t</EntryCallStackInfos>");
	ProfilerOutPutToLog( mLogFile.c_str(), "</Frame>" );
}

void Profiler::AddTopLevelNode(CallStackNode* node)
{
	CallStackNodeList::iterator it = std::find( mTopLevelNodes.begin(), mTopLevelNodes.end(), node );
	if( it == mTopLevelNodes.end() )
		mTopLevelNodes.push_back( node );
}

void Profiler::SetProfileLog( const char* szLog )
{
	mLogFile = szLog;
	if( mLogFile.length() > 0 )
		ProfilerClearLog( szLog );
}