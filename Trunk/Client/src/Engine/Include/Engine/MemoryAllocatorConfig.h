#ifndef __MemoryAllocatorConfig_H__
#define __MemoryAllocatorConfig_H__
#include "MemoryNedAlloc.h"
#include "MemorySTLAllocator.h"
#include <queue>
#include <list>
#include <map>
#include <set>
#include <string>

#if AIR_COMPILER == AIR_COMPILER_GNUC && AIR_COMP_VER >= 310 && !defined(STLPORT)
#	if AIR_TARGET_PLATFORM == AIR_PLATFORM_ANDROID
#		pragma message("AIR_PLATFORM_ANDROID AIR_COMPILER_GNUC")
#       include <backward/hash_map>
#       include <backward/hash_set>
#       define HashMap				::__gnu_cxx::hash_map
#       define HashSet				::__gnu_cxx::hash_set
#		define HashCompare(X)		::__gnu_cxx::hash<X>
#	elif AIR_COMP_VER >= 430
#		pragma message("AIR_COMPILER_GNUC AIR_COMP_VER >= 430")
#       include <tr1/unordered_map>
#       include <tr1/unordered_set> 
#       define HashMap				::std::tr1::unordered_map
#		define HashSet				::std::tr1::unordered_set
#		define HashCompare(X)		::std::tr1::unordered_compare<X>
#    else
#		pragma message("AIR_COMPILER_GNUC AIR_COMP_VER >= 310")
#       include <ext/hash_map>
#       include <ext/hash_set>
#       define HashMap				::__gnu_cxx::hash_map
#       define HashSet				::__gnu_cxx::hash_set
#		define HashCompare(X)		::__gnu_cxx::hash<X>
#    endif
#else
#   if AIR_COMPILER == AIR_COMPILER_MSVC
#       if AIR_COMP_VER >= 1600 // VC++ 10.0
//#			pragma message("AIR_COMPILER_MSVC AIR_COMP_VER >= 1600")
#    		include <unordered_map>
#    		include <unordered_set>
#			define HashMap			::std::tr1::unordered_map
#           define HashSet			::std::tr1::unordered_set
#			define HashCompare(X)	::std::_Hash_compare<X>      //::std::tr1::unordered_compare<X>
#		elif AIR_COMP_VER > 1300 && !defined(_STLP_MSVC)
//#			pragma message("AIR_COMPILER_MSVC AIR_COMP_VER >= 1300")
#   		include <hash_set>
#   		include <hash_map>
#           define HashMap			::stdext::hash_map
#           define HashSet			::stdext::hash_set
#			define HashCompare(X)	::stdext::hash_compare<X, std::less<X>>
#       else
//#			pragma message("AIR_COMPILER_MSVC")
#   		include <hash_set>
#   		include <hash_map>
#           define HashMap			::std::hash_map
#           define HashSet			::std::hash_set
#			define HashCompare(X)	::std::hash_compare<X, std::less<X>>
#       endif
#   else
#   	include <hash_set>
#   	include <hash_map>
#       define HashMap				::std::hash_map
#       define HashSet				::std::hash_set
#		define HashCompare(X)		::std::hash_compare<X, std::less<X>>
#   endif
#endif

/************************************************************************/
/* 
��������ͷŵ����������ͻ���ʱ ʹ�� air_new_t �� air_delete_t ģ�庯��
	template< typename T, typename Alloc> 
	T* air_new_t();

	template< typename T, typename Alloc>
	void air_delete_t(T* ptrBase);

����T��Ҫ������ͷŵ����ͣ� Alloc�Ƿ�������Alloc������о�̬����
	static void* allocBytes(size_t count); //����
	static void deallocBytes(void* ptr);   //�ͷ�

��Ҫ������ͷ�������nCount����ֵʱ ʹ��air_new_array_t �� air_delete_array_t ģ�庯��
	template< typename T, typename Alloc>
	T* air_new_array_t(size_t nCount);

	template< typename T, typename Alloc>
	void air_delete_array_t(T* ptrBase, size_t nCount)

����T��Ҫ������ͷŵ����ͣ� nCount��Ҫ��ֵ�Ĵ�С ��Alloc�Ƿ�������Alloc������о�̬����
	static void* allocBytes(size_t count); //����
	static void deallocBytes(void* ptr);   //�ͷ�

��1:
	int* pInt = AIR::air_new_array_t<int, CMemoryNedAlloc>(100);
	//
	//do something
	//
	AIR::air_delete_array_t<int, CMemoryNedAlloc>(pInt, 100);

ע��
		��ʹ�� air_delete_t �� air_delete_array_t ����ʱ�������ģ����� T(�ͷŵ�����) 
		�����봫���ָ������һ�¡�
	��:
		//DeriveA �̳� BaseA;
		BaseA* pBaseA = AIR::air_new_t<DeriveA, CMemoryNedAlloc>();
		//
		//do something
		//
		AIR::air_delete_t<BaseA, CMemoryNedAlloc>(pBaseA);
	���������Ļ����ǻ����BaseA����������Ҫ���麯��������ȷ�ε��õ�����������
*/
/************************************************************************/


//�������ڶ����õ�nedmallco�����Զ��������º귽�����ı�д
//#ifdef new
//#undef new
//#endif
//#ifdef delete
//#undef delete
//#endif

//#if AIR_TARGET_PLATFORM == AIR_PLATFORM_WIN32

#include <typeinfo>

#define EngineAllocator(ClsType)	\
	public:\
	void* operator new       (size_t size){void* ptr = HighLevelEngineAllocator::AllocBytes(size, typeid(ClsType).name(), __FILE__, __LINE__, __FUNCTION__);return ptr;}\
	void* operator new []    (size_t size){void* ptr = HighLevelEngineAllocator::AllocBytes(size, typeid(ClsType).name(), __FILE__, __LINE__, __FUNCTION__);return ptr;}\
	void  operator delete    (void *ptr)  {HighLevelEngineAllocator::deallocBytes(ptr);}\
	void  operator delete [] (void *ptr)  {HighLevelEngineAllocator::deallocBytes(ptr);}\
	void* operator new       (size_t, void *p) { return p; }\
	void* operator new[]     (size_t, void *p) { return p; }\
	void* operator new       (size_t size, const char* typeName, const char* file, const size_t line, const char* funcName) { void* ptr = HighLevelEngineAllocator::AllocBytes(size, typeName, file, line, funcName );return ptr; }\
	void* operator new []    (size_t size, const char* typeName, const char* file, const size_t line, const char* funcName)	{ void* ptr = HighLevelEngineAllocator::AllocBytes(size, typeName, file, line, funcName );return ptr; }\
	void  operator delete    (void *ptr, const char* typeName, const char* file, const size_t line, const char* funcName)	{ HighLevelEngineAllocator::deallocBytes(ptr); }\
	void  operator delete [] (void *ptr, const char* typeName, const char* file, const size_t line, const char* funcName)	{ HighLevelEngineAllocator::deallocBytes(ptr); }


inline void* operator new(size_t size)
{
	void* ptr = HighLevelEngineAllocator::AllocBytes(size, "Unknown", __FILE__, __LINE__, __FUNCTION__);
	return ptr;
}

inline void* operator new [] (size_t size)
{
	void* ptr = HighLevelEngineAllocator::AllocBytes(size, "Unknown", __FILE__, __LINE__, __FUNCTION__);
	return ptr;
}

inline void operator delete(void *ptr)
{
	HighLevelEngineAllocator::deallocBytes(ptr);
}

inline void operator delete [] (void *ptr)
{
	HighLevelEngineAllocator::deallocBytes(ptr);
}

inline void* operator new(size_t size, const char* typeName, const char* file, const size_t line, const char* funcName)
{
	void* ptr = HighLevelEngineAllocator::AllocBytes(size, typeName, file, line, funcName);
	return ptr;
}

inline void* operator new [] (size_t size, const char* typeName, const char* file, const size_t line, const char* funcName)
{
	void* ptr = HighLevelEngineAllocator::AllocBytes(size, typeName, file, line, funcName);
	return ptr;
}

inline void operator delete(void *ptr, const char* typeName, const char* file, const size_t line, const char* funcName)
{
	HighLevelEngineAllocator::deallocBytes(ptr);
}

inline void operator delete [] (void *ptr, const char* typeName, const char* file, const size_t line, const char* funcName)
{
	HighLevelEngineAllocator::deallocBytes(ptr);
}

#	define  AIR_NEW(T)							(new( typeid(T).name(), __FILE__, __LINE__, __FUNCTION__ ) T)
#	define  AIR_NEW_ARRAY(T, nCount)			(new( typeid(T).name(), __FILE__, __LINE__, __FUNCTION__ ) T[nCount])

#	define  AIR_NEW_T( T )						AIR_NEW( T )
#	define  AIR_DELETE_T(ptr, T)				delete ptr
#	define  AIR_NEW_ARRAY_T(T, nCount)			AIR_NEW_ARRAY(T, nCount)
#	define  AIR_DELETE_ARRAY_T(ptr, T, nCount)	delete [] ptr

//#	define  AIR_NEW_T( T )						AIR::air_new_t<T, CMemoryNedAlloc>(__FILE__, __LINE__, __FUNCTION__)
//#	define  AIR_DELETE_T(ptr, T)				AIR::air_delete_t<T, CMemoryNedAlloc>(ptr)
//#	define  AIR_NEW_ARRAY_T(T, nCount)			AIR::air_new_array_t<T, CMemoryNedAlloc>(nCount,__FILE__, __LINE__, __FUNCTION__)
//#	define  AIR_DELETE_ARRAY_T(ptr, T, nCount)	AIR::air_delete_array_t<T, CMemoryNedAlloc>(ptr, nCount)

//#else
//#	define  AIR_NEW_T( T )						AIR::air_new_t<T, StdAlloc>()
//#	define  AIR_DELETE_T(ptr, T)				AIR::air_delete_t<T, StdAlloc>(ptr)
//#	define  AIR_NEW_ARRAY_T(T, nCount)			AIR::air_new_array_t<T, StdAlloc>(nCount)
//#	define  AIR_DELETE_ARRAY_T(ptr, T, nCount)	AIR::air_delete_array_t<T, StdAlloc>(ptr, nCount)
//
//	static inline void* operator new(size_t size)
//	{
//		void* ptr = StdAlloc::AllocBytes(size);
//		return ptr;
//	}
//
//	static inline void* operator new [] (size_t size)
//	{
//		void* ptr = StdAlloc::AllocBytes(size);
//		return ptr;
//	}
//
//	static inline void operator delete(void *ptr)
//	{
//		CMemoryNedAlloc::StdAlloc(ptr);
//	}
//
//	static inline void operator delete [] (void *ptr)
//	{
//		CMemoryNedAlloc::StdAlloc(ptr);
//	}
//
//#endif

#define  AIR_SAFE_DELETE_T( ptr, T )		if( ptr != NULL ) { AIR_DELETE_T(ptr, T); };
#define  AIR_SAFE_DELETE_ARRAY_T( ptr, T )	if( ptr != NULL ) { AIR_DELETE_ARRAY_T(ptr, T); };
#define  AIR_SAFE_DELETE(ptr)				if( ptr != NULL ) { delete ptr; ptr = NULL; };
#define  AIR_SAFE_DELETE_ARRAY( ptr )	    if( ptr != NULL ) { delete[] ptr; ptr = NULL; };



namespace AIR
{
	////placement new ���������construct
	//template< typename T > 
	//T* constructN(T* ptrBase, size_t n)
	//{
	//	for (size_t i = 0 ; i < n ; i++)
	//	{
	//		new ( ( void* )( ptrBase + i ) ) T();
	//	}
	//	return ptrBase;
	//}

	////placement new ��Ӧ����������
	//template < typename T >
	//void destructorN(T* ptrBase, size_t n)
	//{
	//	if(ptrBase)
	//	{
	//		for (size_t i = 0; i < n; i++)
	//		{
	//			(ptrBase)[i].~T();
	//		}
	//	}
	//}


	////���䵥���࣬��������Ϳռ�
	//template< typename T, typename Alloc> 
	//T* air_new_t(const char* file, const size_t line, const char* funcName)
	//{
	//	return	new ( Alloc::AllocBytes( sizeof( T ), typeid(T).name() ) ) T;
	//}

	////��Ӧ�� air_new_t �������ͷ�
	//template< typename T, typename Alloc>
	//void air_delete_t(T* ptrBase)
	//{
	//	if(ptrBase)
	//	{
	//		ptrBase->~T();
	//		Alloc::deallocBytes( ( void* ) ptrBase );
	//	}
	//}

	////����nCount����С������ռ�
	//template< typename T, typename Alloc>
	//T* air_new_array_t(size_t nCount, const char* file, const size_t line, const char* funcName)
	//{		
	//	return AIR::constructN<T> ( static_cast<T*>( Alloc::AllocBytes( sizeof( T ) * nCount, typeid(T).name() )), nCount);
	//}

	////��Ӧair_new_array_t�Ŀռ��ͷ�
	//template< typename T, typename Alloc>
	//void air_delete_array_t(T* ptrBase, size_t nCount)
	//{
	//	if ( ptrBase )
	//	{
	//		destructorN<T>(ptrBase, nCount);
	//		Alloc::deallocBytes( (void*) ptrBase );
	//	}
	//}
	////-------------------------------

};


/************************************************************************
	��׼����ʹ���Լ����ڴ������
	CURRENT_ALLOC ����ʹ�õ��ڴ��Ƶ��
	CUSTOM_ALLOC  �綨������������ʾstl����ʹ���Զ�����ڴ��������
	����ʹ��Ĭ�ϵ��ڴ������

	����ʹ�� map
	AIR::map<AIRString, int>::type mapName;
	mapName["ogre"] = 3;
	mapName["u3"] = 4;
	AIR::map<AIRString, int>::type::iterator it = mapName.begin();
	for ( ; it != mapName.end() ; it++ )
	{
		cout<< "key: "<<it->first<<"  value: "<<it->second<<endl;
	}
************************************************************************/
//for stl(����) containter 

//#define CURRENT_ALLOC CMemoryNedAlloc
//#define CUSTOM_ALLOC 1

#if AIR_TARGET_PLATFORM == AIR_PLATFORM_WIN32
#else
template <size_t size>
char* temp_strcpy_s(char dest[size], const char* src)
{
	//assert(src!=NULL);
	if( src == NULL )
		return dest;
	char* temp = dest;
	int i = 0;
	while(i++ < size  && (*temp++ = *src++)!='\0')
	{}
	if(*(temp)!='\0')
		*temp='\0';
	return dest;
}
#define strcpy_s temp_strcpy_s<512>
#endif
/*
namespace AIR
{ 
	template <typename T, typename A = CMemorySTLAllocator<T, CURRENT_ALLOC> > 
	struct deque 
	{ 
#if CUSTOM_ALLOC
		typedef typename std::deque<T, A> type;    
#else
		typedef typename std::deque<T> type;    
#endif
	}; 

	template <typename T, typename A =  CMemorySTLAllocator<T, CURRENT_ALLOC> > 
	struct vector 
	{ 
#if CUSTOM_ALLOC
		typedef typename std::vector<T, A> type;    
#else
		typedef typename std::vector<T> type;    
#endif
	}; 

	template <typename T, typename A = CMemorySTLAllocator<T, CURRENT_ALLOC> > 
	struct list 
	{ 
#if CUSTOM_ALLOC
		typedef typename std::list<T, A> type;    
#else
		typedef typename std::list<T> type;    
#endif
	}; 

	template <typename T, typename P = std::less<T>, typename A = CMemorySTLAllocator<T, CURRENT_ALLOC> > 
	struct set 
	{ 
#if CUSTOM_ALLOC
		typedef typename std::set<T, P, A> type;    
#else
		typedef typename std::set<T, P> type;    
#endif
	}; 

	template <typename K, typename V, typename P = std::less<K>, typename A = CMemorySTLAllocator<std::pair<const K, V>, CURRENT_ALLOC> > 
	struct map 
	{ 
#if CUSTOM_ALLOC
		typedef typename std::map<K, V, P, A> type; 
#else
		typedef typename std::map<K, V, P> type; 
#endif
	}; 
}; // AIR
*/
#if AIR_TARGET_PLATFORM == AIR_PLATFORM_WIN32
#	define STRING_ALLOCATOR
#endif

#ifdef STRING_ALLOCATOR
	typedef std::basic_string<char, std::char_traits<char>, CMemorySTLAllocator<char> >				AIRString;
	typedef std::basic_string<wchar_t, std::char_traits<wchar_t>, CMemorySTLAllocator<wchar_t> >	WAIRString;

	#define StdStringT(T)		std::basic_string<T, std::char_traits<T>, std::allocator<T> >   
	#define CustomStringT(T)	std::basic_string<T, std::char_traits<T>, CMemorySTLAllocator<T> >

	template<typename T>
	bool operator <(const CustomStringT(T)& l, const StdStringT(T)& o)
	{
		return l.compare(0, l.length(), o.c_str(), o.length())<0;
	}


	template<typename T>
	bool operator < (const StdStringT(T)& l, const CustomStringT(T)& o)
	{
		return l.compare(0,l.length(),o.c_str(),o.length())<0;
	}

	template<typename T>
	bool operator <= (const CustomStringT(T)& l, const StdStringT(T)& o)
	{
		return l.compare(0, l.length(), o.c_str(), o.length())<=0;
	}

	template<typename T>
	bool operator <= (const StdStringT(T)& l, const CustomStringT(T)& o)
	{
		return l.compare(0, l.length(), o.c_str(), o.length())<=0;
	}

	template<typename T>
	bool operator > (const CustomStringT(T)& l, const StdStringT(T)& o)
	{
		return l.compare(0, l.length(), o.c_str(), o.length())>0;
	}

	template<typename T>
	bool operator > (const StdStringT(T)& l, const CustomStringT(T)& o)
	{
		return l.compare(0, l.length(),o.c_str(), o.length())>0;
	}

	template<typename T>
	bool operator >= (const CustomStringT(T)& l, const StdStringT(T)& o)
	{
		return l.compare(0, l.length(), o.c_str(), o.length())>=0;
	}

	template<typename T>
	bool operator >= (const StdStringT(T)& l, const CustomStringT(T)& o)
	{
		return l.compare(0, l.length(), o.c_str(), o.length())>=0;
	}

	template<typename T>
	bool operator == (const CustomStringT(T)& l,const StdStringT(T)& o)
	{
		return l.compare(0, l.length(), o.c_str(), o.length())==0;
	}

	template<typename T>
	bool operator == (const StdStringT(T)& l, const CustomStringT(T)& o)
	{
		return l.compare(0, l.length(), o.c_str(), o.length())==0;
	}

	template<typename T>
	bool operator != (const CustomStringT(T)& l, const StdStringT(T)& o)
	{
		return l.compare(0, l.length(), o.c_str(), o.length())!=0;
	}

	template<typename T>
	bool operator !=(const StdStringT(T)& l, const CustomStringT(T)& o)
	{
		return l.compare(0, l.length(), o.c_str(), o.length())!=0;
	}

#else
	typedef std::string			AIRString;
	typedef std::wstring		WAIRString;

	#define StdStringT(T)		std::basic_string<T, std::char_traits<T>, std::allocator<T> >   
	#define CustomStringT(T)	std::basic_string<T, std::char_traits<T>, std::allocator<T> >
#endif


#if AIR_TARGET_PLATFORM == AIR_PLATFORM_WIN32
template <class _Ty>
class AIRVector
		: public std::vector<
		  _Ty,
		  CMemorySTLAllocator<_Ty> >
{};

template <class _Ty>
class AIRList
		: public std::list<
		  _Ty,
		  CMemorySTLAllocator<_Ty> >
{};

template <typename K, typename V>
class AIRMap
		: public std::map<
		  K,
		  V,
		  std::less<K>,
		  CMemorySTLAllocator<std::pair<const K, V>> >
{};

template <typename _Ty>
class AIRSet
		: public std::set<
		  _Ty,
		  CMemorySTLAllocator<_Ty> >
{};

template <typename _Ty>
class AIRDeque
		: public std::deque<
		  _Ty,
		  CMemorySTLAllocator<_Ty> >
{};
#else
#define AIRVector std::vector
#define AIRList   std::list
#define AIRMap    std::map
#define AIRSet    std::set
#define AIRDeque  std::deque
#endif

//template <typename K, typename V> class AIRHashMap : public stdext::hash_map<K, V, stdext::hash_compare<K, std::less<K>>, CMemorySTLAllocator<std::pair<const K, V>, CMemoryNedAlloc>> {};
#if AIR_TARGET_PLATFORM == AIR_PLATFORM_WIN32
#if AIR_COMP_VER >= 1600 // VC++ 10.0
template<>
class std::hash<AIRString>
    : public std::unary_function<AIRString, size_t>
{	// hash functor
public:
    typedef AIRString _Kty;

    size_t operator()(const _Kty& _Keyval) const
    {	// hash _Keyval to size_t value by pseudorandomizing transform
        size_t _Val = 2166136261U;
        size_t _First = 0;
        size_t _Last = _Keyval.size();
        size_t _Stride = 1 + _Last / 10;

        for(; _First < _Last; _First += _Stride)
            _Val = 16777619U * _Val ^ (size_t)_Keyval[_First];
        return (_Val);
    }
};

template<>
class std::hash<WAIRString>
    : public std::unary_function<WAIRString, size_t>
{	// hash functor
public:
    typedef WAIRString _Kty;

    size_t operator()(const _Kty& _Keyval) const
    {	// hash _Keyval to size_t value by pseudorandomizing transform
        size_t _Val = 2166136261U;
        size_t _First = 0;
        size_t _Last = _Keyval.size();
        size_t _Stride = 1 + _Last / 10;

        for(; _First < _Last; _First += _Stride)
            _Val = 16777619U * _Val ^ (size_t)_Keyval[_First];
        return (_Val);
    }
};
template <typename K, typename V, class _Hasher = _STD tr1::hash<K>>
class AIRHashMap
    : public HashMap<
    K,
    V,
    std::tr1::hash<K>,
    std::equal_to<K>,
    CMemorySTLAllocator<std::pair<const K, V>/*, CMemoryNedAlloc*/> > {};
#else
template <typename K, typename V>
class AIRHashMap
    : public HashMap<
    K,
    V,
    HashCompare(K),
    CMemorySTLAllocator<std::pair<const K, V>> >{};
#endif
#else
template <typename K, typename V>
class AIRHashMap
	: public HashMap<
	K,
	V,
	HashCompare(K),
	CMemorySTLAllocator< std::pair<const K, V> > >{};
//typedef HashMap AIRHashMap 
#endif



#endif// __MemoryAllocatorConfig_H__
