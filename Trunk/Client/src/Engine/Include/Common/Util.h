#pragma once
#include <fstream>
#include <iostream>
//#include <Windows.h>
#include <math.h>
#include <string>
#include <stdio.h>
#include <vector>
#include "MemoryAllocatorConfig.h"
#include "AIRTypes.h"

#if AIR_TARGET_PLATFORM == AIR_PLATFORM_WIN32
#define snprintf _snprintf_s
#elif AIR_TARGET_PLATFORM == AIR_PLATFORM_ANDROID
#include <jni.h>
#include <android/log.h>
#define  LOG_TAG    "airengine"
#define  DroidLogD(...)  __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG,__VA_ARGS__)
#define  DroidLogI(...)  __android_log_print(ANDROID_LOG_INFO,  LOG_TAG,__VA_ARGS__)
#define  DroidLogE(...)  __android_log_print(ANDROID_LOG_ERROR, LOG_TAG,__VA_ARGS__)
#define  DroidLogW(...)  __android_log_print(ANDROID_LOG_WARN,  LOG_TAG,__VA_ARGS__)
#endif


//#include "DetectMemLeak.h"

using namespace std;
using namespace AIR;

EngineExport size_t UnicodeStrLen(const wchar_t *pwstr);

inline bool IsNumber(const char code)
{
    return code >= '0' && code <= '9';
}

inline bool IsNumber(const wchar_t code)
{
    return code >= L'0' && code <= L'9';
}

//判断是否16进制字母
inline bool IsLetter16(const char code)
{
    return (code >= 'A' && code <= 'F') || (code >= 'a' && code <= 'f');
}

inline bool IsLetter16(const wchar_t code)
{
    return (code >= L'A' && code <= L'F') || (code >= L'a' && code <= L'f');
}

//---------------------------------------------------------
// 名称:  ATOL
// 功能:  字符串转整数
// 参数:  str - 数字字符串； rule - 进制
// 返回:  数字字符串对应的整数
//---------------------------------------------------------
inline u32 ATOL( const char* str, const u8 &rule = 16 )
{
    u32 result = 0;
    int len = (int)strlen(str);
    int index = 0;
    int p = len-1;
    while( index < len )
    {
        if( str[index] >= '0' && str[index] <= '9' )
            result += (u32)( (str[index] - '0')      * pow( (float)rule, p ) );
        else if( (str[index] >= 'A' && str[index] <= 'F') )
            result += (u32)( (str[index] - 'A' + 10) * pow( (float)rule, p ) );
        else if( (str[index] >= 'a' && str[index] <= 'f') )
            result += (u32)( (str[index] - 'a' + 10) * pow( (float)rule, p ) );
        else 
            return 0;
        ++index;
        --p;
    }
    return result;
}


//---------------------------------------------------------
// 名称:  ATOL
// 功能:  字符串转整数
// 参数:  str - 数字字符串； rule - 进制
// 返回:  数字字符串对应的整数
//---------------------------------------------------------
inline u32 ATOL( const wchar_t* str, const u8 &rule = 16 )
{
    u32 result = 0;
    //int len = (int)wcslen(str);
	int len = UnicodeStrLen(str);
    int index = 0;
    int p = len-1;
    while( index < len )
    {
        if( str[index] >= L'0' && str[index] <= L'9' )
            result += (u32)( (str[index] - L'0')      * pow( (float)rule, p ) );
        else if( (str[index] >= L'A' && str[index] <= L'F') )
            result += (u32)( (str[index] - L'A' + 10) * pow( (float)rule, p ) );
        else if( (str[index] >= L'a' && str[index] <= L'f') )
            result += (u32)( (str[index] - L'a' + 10) * pow( (float)rule, p ) );
        else 
            return 0;
        ++index;
        --p;
    }
    return result;
}


inline u32 StringToARGBColor( const char* szColor )
{
    if ( !szColor )
    {
        return 0xff000000;
    }
    AIRString s(szColor);
    if ( s.size() < 6 )
    {
        for ( size_t i = s.size(); i < 6; ++i )
        {
            s.push_back( char('0') );
        }
    }
    return 0xff000000 + ATOL( s.c_str() );
}


inline u32 StringToARGBColor( const wchar_t* wszColor )
{
    if ( !wszColor )
    {
        return 0xff000000;
    }
    std::wstring s(wszColor);
    if ( s.size() < 6 )
    {
        for ( size_t i = s.size(); i < 6; ++i )
        {
            s.push_back( wchar_t('0') );
        }
    }
    return 0xff000000 + ATOL( s.c_str() );
}


inline float RandRange(float min, float max)
{
    int r;
    float	x;

    r = rand();
    x = (float)(r & 0x7fff) /
        (float)0x7fff;
    return (x * (max - min) + min);
} 


template <typename t>
inline int SafeRelease(t& a)
{
    if (a)
    {
        int nRef = a->Release();
        if (nRef == 0)
        {
            a = NULL;
        }
        return nRef;
    }

    return 0;
}

template<typename t>
inline void SafeDelete(t& a)
{
    if (a)
    {
        delete a;
        a = NULL;
    }
	//SAFE_DELETE_ATOM( a );
}

template <typename t>
inline void SafeDeleteArray(t& a)
{
    if (a)
    {
        delete [] a;
        a = NULL;
    }
	//SAFE_DELETE_ARRAY( a );
}


/*
inline bool SplitFilename(const string& strSplit, string& strPath, string& strFile)
{
    size_t found;
    found = strSplit.find_last_of("/\\.");
    if(found == AIRString::npos)
        return false;

    strPath = strSplit.substr(0, found);
    strFile = strSplit.substr(found + 1);
    return true;
}
*/



EngineExport u32 WCharToMByte(const wchar_t* lpcwszStr, char* lpszStr, u32 dwSize);
//{
///*    
//#if AIR_TARGET_PLATFORM == AIR_PLATFORM_WIN32
//    u32 dwMinSize;
//    dwMinSize = WideCharToMultiByte(CP_OEMCP,NULL, lpcwszStr,-1,NULL, 0, NULL, FALSE);
//    //if(dwSize < dwMinSize)
//    //{
//    //    return false;
//    //}
//    dwSize = WideCharToMultiByte(CP_OEMCP, NULL, lpcwszStr,-1, lpszStr, dwMinSize, NULL, FALSE);
//#else
//    */
//    setlocale(LC_ALL, "chs");
//    dwSize = wcstombs(lpszStr, lpcwszStr, dwSize);
////#endif
//    return dwSize;
//}

EngineExport u32 MByteToWChar(const char* szIn, wchar_t* wszOut, u32 nSize);
//{
//    /*
//#if AIR_TARGET_PLATFORM == AIR_PLATFORM_WIN32
//    u32 dwMinSize;
//    dwMinSize = MultiByteToWideChar(CP_ACP, NULL, szIn, -1, NULL, 0);
//    if(nSize < dwMinSize)
//    {
//        return false;
//    }
//    MultiByteToWideChar(CP_ACP, NULL, szIn, -1, wszOut, dwMinSize);
//#else
//    */
//    nSize = mbstowcs(wszOut, szIn, nSize);
////#endif
//
//    return nSize;
//}

inline void WriteFile(const char* szFilename, const std::string& str)
{
    ofstream fout;
    fout.open(szFilename, ios::app);//打开文件

    fout<<str<<"\n";//写入文件

    fout<<flush;//回写缓冲区（回写缓冲区会在保持文件打开的情况下保存文件）
    fout.close();//关闭
}

#define RANDRANGE(v) RandRange(-v, v)

inline bool IsFileExist(const char* szFilename)
{
    bool bIsExist = false;

    std::fstream _file;
    _file.open(szFilename, std::ios::in);
    if (_file)
    {
        bIsExist = true;
    }
    _file.close();

    return bIsExist;
}



EngineExport void PrintDebugString( char* buf, ... );


inline char* GetString( char* buf, ... )
{
	static std::string s_resStr;
	/*static*/ char s_Buf[4096];   //去掉static，不然会是线程不安全
	memset( s_Buf, 0, sizeof(s_Buf) );
	int		nBuf;  
	va_list args;  
	va_start( args,buf );  
	nBuf = vsnprintf_s( s_Buf, sizeof(s_Buf), buf, args );
	//nBuf = snprintf( s_Buf, sizeof(s_Buf) - 1, buf, args );  
	va_end(args);
	s_resStr = s_Buf;
	return (char*)s_resStr.c_str();
}

inline std::string GetFormatString( char* buf, ... )
{
	char s_Buf[4096];
	memset( s_Buf, 0, sizeof(s_Buf) );
	int		nBuf;  
	va_list args;  
	va_start( args,buf );  
	nBuf = snprintf( s_Buf, sizeof(s_Buf) - 1, buf, args );  
	va_end(args);
	return std::string(s_Buf);
}

template<typename T>
inline void AnalizeFilePath(const CustomStringT(T)& strSplit, CustomStringT(T)& strPath, CustomStringT(T)& strFile, CustomStringT(T)& strExt)
{
	size_t found;
	found = strSplit.find_last_of("/");
	if(found == CustomStringT(T)::npos)
	{
		found = strSplit.find_last_of("\\");
		if (found == CustomStringT(T)::npos)
		{
			strFile = strSplit;

			found = strFile.find_last_of(".");
			strExt = strSplit.substr(found + 1);
			return;
		}

	}

	strPath = strSplit.substr(0, found);
	strFile = strSplit.substr(found + 1);

	found = strFile.find_last_of(".");
	strExt = strSplit.substr(found + 1);
}

template<typename T>
inline void AnalizeFilePathStd(const StdStringT(T)& strSplit, StdStringT(T)& strPath, StdStringT(T)& strFile, StdStringT(T)& strExt)
{
    size_t found;
    found = strSplit.find_last_of("/");
    if(found == CustomStringT(T)::npos)
    {
        found = strSplit.find_last_of("\\");
        if (found == CustomStringT(T)::npos)
        {
            strFile = strSplit;

            found = strFile.find_last_of(".");
            strExt = strSplit.substr(found + 1);
            return;
        }

    }

    strPath = strSplit.substr(0, found);
    strFile = strSplit.substr(found + 1);

    found = strFile.find_last_of(".");
    strExt = strSplit.substr(found + 1);
}
