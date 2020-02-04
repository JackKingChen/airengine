#include "stdafx.h"
#include "StringUtil.h"
#include <algorithm>
#include <functional>
#include <bitset>
#include <iostream>
#include "Util.h"

const AIRString CStringUtil::BLANK = "";
const std::string CStringUtil::BLANK_STL = "";
#ifdef _WINDOWS
const AIRString CStringUtil::STR_SLASH = "\\";
const std::string CStringUtil::STR_SLASH_STL = "\\";
#else
const AIRString CStringUtil::STR_SLASH = "/";
const std::string CStringUtil::STR_SLASH_STL = "/";
#endif

//---------------------------------------------------------
// 名称:  CheckIsSplitChar
// 功能:  检查某个字符是否在字符串内
// 参数:  c - 字符;
//        szSplit - 字符串;
//---------------------------------------------------------
inline bool CheckIsCharInStringStl( char c, const AIRString& szSplit )
{
    for ( size_t i = 0; i < szSplit.size(); ++i )
    {
        if ( c == szSplit[i] )
        {
            return true;
        }
    }
    return false;
}

inline bool CheckIsCharInString( char c, const AIRString& szSplit )
{
    for ( size_t i = 0; i < szSplit.size(); ++i )
    {
        if ( c == szSplit[i] )
        {
            return true;
        }
    }
    return false;
}

void CStringUtil::ToLowerCase(AIRString& str)
{
#if AIR_TARGET_PLATFORM == AIR_PLATFORM_WIN32
    std::transform(str.begin(), str.end(), str.begin(), tolower);
#else
	u32 c = str.size();
	for ( int i = 0; i < c; ++i )
	{
		str[i] = tolower(str[i]);
	}
#endif
}

void CStringUtil::ToUpperCase(AIRString& str)
{
#if AIR_TARGET_PLATFORM == AIR_PLATFORM_WIN32
    std::transform(str.begin(), str.end(), str.begin(), toupper);
#else
	u32 c = str.size();
	for ( int i = 0; i < c; ++i )
	{
		str[i] = toupper(str[i]);
	}
#endif
}

void CStringUtil::SplitFilename(const AIRString& strSplit, AIRString& strPath, AIRString& strFile)
{
    size_t found;
    found = strSplit.find_last_of("/");
    if(found == AIRString::npos)
    {
        found = strSplit.find_last_of("\\");
        if (found == AIRString::npos)
        {
            strFile = strSplit;
            return;
        }
        
    }

    strPath = strSplit.substr(0, found);
    strFile = strSplit.substr(found + 1);
}

void CStringUtil::SplitFilename(const AIRString& strSplit, AIRString& strLeft, AIRString& strRight, const AIRString& strSplitChar)
{
	size_t found;
	found = strSplit.find_last_of(strSplitChar);
	if(found == AIRString::npos)
	{
        return;
	}	

	strLeft = strSplit.substr(0, found);
	strRight = strSplit.substr(found + 1);
}

void CStringUtil::Trim(AIRString& str, bool bLeft /* = true */, bool bRight /* = true */)
{
    static const AIRString delims = " \t\r";
    if(bRight)
        str.erase(str.find_last_not_of(delims)+1); // trim right
    if(bLeft)
        str.erase(0, str.find_first_not_of(delims)); // trim left
}


AIRString CStringUtil::ws2s(const WAIRString& ws)
{
	//AIRString curLocale = setlocale(LC_ALL, NULL); // curLocale = "C";
	setlocale(LC_ALL, "chs"); 
	//setlocale( LC_ALL, "zh_CN.UTF-8" );

	const wchar_t* _Source = ws.c_str();
	size_t _Dsize = 2 * ws.size() + 1;
	char *_Dest = new char[_Dsize];
	memset(_Dest,0,_Dsize);
	//wcstombs(_Dest,_Source,_Dsize);
	WCharToMByte(_Source, _Dest, _Dsize);
	AIRString result = _Dest;
	SafeDeleteArray(_Dest);

	//setlocale(LC_ALL, curLocale.c_str());
	return result;
}


WAIRString CStringUtil::s2ws(const AIRString& s)
{
	setlocale(LC_ALL, "chs"); 
	//setlocale( LC_ALL, "zh_CN.UTF-8" );

	const char* _Source = s.c_str();
	size_t _Dsize = s.size() + 1;
	wchar_t *_Dest = new wchar_t[_Dsize];
	size_t _dist_bytes = _Dsize * sizeof(wchar_t);
	memset( _Dest, 0, _dist_bytes );
	//wmemset(_Dest, 0, _Dsize);
	//mbstowcs(_Dest,_Source,_Dsize);
	MByteToWChar(_Source,_Dest, /*_Dsize*/_dist_bytes);
	WAIRString result = _Dest;
	SafeDeleteArray(_Dest);

	//setlocale(LC_ALL, "C");
	return result;
}

const char* CStringUtil::GetFilenameNoExt(const char* szFilename, char* szOut)
{
	char szTmp[256] = { 0 };
	strcpy_s(szTmp, 255, GetFilename((szFilename)));

	char* p = strrchr(szTmp, '.');
	if (p != NULL)
	{
		p[0] = '\0';
	}

	strcpy_s(szOut, 255, szTmp);
	return szOut;
}

const char* CStringUtil::GetFilenameExt(const char* szFilename)
{
    return strrchr(szFilename, '.');
}

const char* CStringUtil::GetFilename(const char* szFilename)
{
    const char* p = strrchr(szFilename, '\\');
    if (p == NULL)
    {
        p = strrchr(szFilename, '/');
    }

    if (p != NULL)
    {
        return p + 1;
    }

    return szFilename;
}

void DecodeFloat( u8 pByte[4] )
{
	printf( "原始（十进制）：%d  %d  %d  %d\n" , 
		(int)pByte[0],
		(int)pByte[1], 
		(int)pByte[2], 
		(int)pByte[3] );

	printf( "翻转（十进制）：%d  %d  %d  %d\n" , 
		(int)pByte[3],
		(int)pByte[2], 
		(int)pByte[1], 
		(int)pByte[0] );

	std::bitset<32> bitAll( (u64)*(unsigned long*)pByte );
	AIRString strBinary = bitAll.to_string<char, char_traits<char>, allocator<char> >().c_str();
	strBinary.insert( 9, "  " );
	strBinary.insert( 1, "  " );
	std::cout << "二进制：" << strBinary.c_str() << std::endl;
	std::cout << "符号：" << ( bitAll[31] ? "-" : "+" ) << std::endl;
	std::bitset<32> bitTemp;
	bitTemp = bitAll;
	bitTemp <<= 1;
	s32 ulExponent = 0;
	for ( int i = 0; i < 8; i++ )
	{
		ulExponent |= ( bitTemp[ 31 - i ] << ( 7 - i ) );
	}
	ulExponent -= 127;
	std::cout << "指数（十进制）：" << ulExponent << std::endl;
	bitTemp = bitAll;
	bitTemp <<= 9;
	float fMantissa = 1.0f;
	for ( int i = 0; i < 23; i++ )
	{
		bool b = bitTemp[ 31 - i ];
		fMantissa += ( (float)bitTemp[ 31 - i ] / (float)( 2 << i ) );
	}
	std::cout << "尾数（十进制）："  << fMantissa << std::endl;
	float fPow;
	if ( ulExponent >= 0 )
	{
		fPow = (float)( 2 << ( ulExponent - 1 ) );
	}
	else
	{
		fPow = 1.0f / (float)( 2 << ( -1 - ulExponent ) );
	}
	std::cout << "运算结果：" << fMantissa * fPow << std::endl;
}

//下面是stl操作
void CStringUtil::ToLowerCaseStl(std::string& str)
{
#if AIR_TARGET_PLATFORM == AIR_PLATFORM_WIN32
    std::transform(str.begin(), str.end(), str.begin(), tolower);
#else
	u32 c = str.size();
	for ( int i = 0; i < c; ++i )
	{
		str[i] = tolower(str[i]);
	}
#endif
}

void CStringUtil::ToUpperCaseStl(std::string& str)
{
#if AIR_TARGET_PLATFORM == AIR_PLATFORM_WIN32
    std::transform(str.begin(), str.end(), str.begin(), toupper);
#else
	u32 c = str.size();
	for ( int i = 0; i < c; ++i )
	{
		str[i] = toupper(str[i]);
	}
#endif
}

void CStringUtil::SplitFilenameStl(const std::string& strSplit, std::string& strPath, std::string& strFile)
{
    size_t found;
    found = strSplit.find_last_of("/");
    if(found == std::string::npos)
    {
        found = strSplit.find_last_of("\\");
        if (found == std::string::npos)
        {
            strFile = strSplit;
            return;
        }

    }

    strPath = strSplit.substr(0, found);
    strFile = strSplit.substr(found + 1);
}

void CStringUtil::SplitFilenameStl(const std::string& strSplit, std::string& strLeft, std::string& strRight, const std::string& strSplitChar)
{
    size_t found;
    found = strSplit.find_last_of(strSplitChar);
    if(found == std::string::npos)
    {
        return;
    }	

    strLeft = strSplit.substr(0, found);
    strRight = strSplit.substr(found + 1);
}

void CStringUtil::TrimStl(std::string& str, bool bLeft /* = true */, bool bRight /* = true */)
{
    static const std::string delims = " \t\r";
    if(bRight)
        str.erase(str.find_last_not_of(delims)+1); // trim right
    if(bLeft)
        str.erase(0, str.find_first_not_of(delims)); // trim left
}


std::string CStringUtil::ws2sStl(const std::wstring& ws)
{
    std::string curLocale = setlocale(LC_ALL, NULL); // curLocale = "C";
    setlocale(LC_ALL, "chs"); 
    const wchar_t* _Source = ws.c_str();
    size_t _Dsize = 2 * ws.size() + 1;
    char *_Dest = new char[_Dsize];
    memset(_Dest,0,_Dsize);
    wcstombs(_Dest,_Source,_Dsize);
    std::string result = _Dest;
    SafeDeleteArray(_Dest);
    setlocale(LC_ALL, curLocale.c_str());
    return result;
}


std::wstring CStringUtil::s2wsStl(const std::string& s)
{
    setlocale(LC_ALL, "chs"); 
    const char* _Source = s.c_str();
    size_t _Dsize = s.size() + 1;
    wchar_t *_Dest = new wchar_t[_Dsize];
    wmemset(_Dest, 0, _Dsize);
    mbstowcs(_Dest,_Source,_Dsize);
    std::wstring result = _Dest;
    SafeDeleteArray(_Dest);
    setlocale(LC_ALL, "C");
    return result;
}

void CStringUtil::SplitString(const AIRString& szSrc, const AIRString& szSplit, AIRVector<AIRString>& vtRes )
{
    vtRes.clear();
    AIRString tmp("");
    for ( size_t i = 0; i < szSrc.size(); ++i )
    {
        if ( CheckIsCharInString( szSrc[i], szSplit ) )
        {
            if ( tmp.size() > 0 )
            {
                vtRes.push_back( tmp );
                tmp.clear();
            }
        }
        else
        {
            tmp.push_back( szSrc[i] );
        }
    }
    if ( tmp.size() > 0 )
    {
        vtRes.push_back( tmp );
        tmp.clear();
    }
}

void CStringUtil::SplitStringStl(const AIRString& szSrc, const AIRString& szSplit, AIRVector<AIRString>& vtRes)
{
    vtRes.clear();
    AIRString tmp("");
    for ( size_t i = 0; i < szSrc.size(); ++i )
    {
        if ( CheckIsCharInStringStl( szSrc[i], szSplit ) )
        {
            if ( tmp.size() > 0 )
            {
                vtRes.push_back( tmp );
                tmp.clear();
            }
        }
        else
        {
            tmp.push_back( szSrc[i] );
        }
    }
    if ( tmp.size() > 0 )
    {
        vtRes.push_back( tmp );
        tmp.clear();
    }
}

bool CStringUtil::IsSameFilePath(const AIRString& l, const AIRString& r)
{
	AIRString strLeft = l;
	AIRString strRight = r;
    Replace(strLeft, "\\", "/");
	Replace(strRight, "\\", "/");
	return strRight == strLeft;
}

AIRString CStringUtil::CombineFilePath(const AIRString& strPath, const AIRString& strFile)
{
	AIRString strOutPath = strPath;
	AIRString strOutFile = strFile;
	Replace(strOutPath, "\\", "/");
	Replace(strOutFile, "\\", "/");

	int fid = -1;
	if (strOutFile.find_first_of("./") == 0)
	{
		fid = strOutFile.find_first_of(".");
		if ( fid >= 0 )
			strOutFile.erase(fid, 1);
	}

	if (strOutFile.find_last_of("/") == strOutFile.size() - 1)
	{
		fid = strOutFile.find_last_of("/");
		if ( fid >= 0 )
			strOutFile.erase(fid, 1);
	}

	if (strOutPath.find_last_of("/") != strOutPath.size() - 1 && strOutFile.find_first_of("/") != 0)
	{
		return strOutPath + AIRString("/") + strOutFile;
	}

	if (strOutPath.find_last_of("/") == strOutPath.size() - 1 && strOutFile.find_first_of("/") == 0)
	{
		fid = strOutFile.find_first_of("/");
		if ( fid >= 0 )
			strOutFile.erase(fid, 1);
	}


	return strOutPath + strOutFile;
}

AIRString CStringUtil::Format(char* buf, ...)
{
    static std::string s_resStr;
    static char s_Buf[4096];   //去掉static，不然会是线程不安全
    memset( s_Buf, 0, sizeof(s_Buf) );
    int		nBuf;  
    va_list args;  
    va_start( args,buf );  
    nBuf = vsnprintf( s_Buf, sizeof(s_Buf), buf, args );
    //nBuf = snprintf( s_Buf, sizeof(s_Buf) - 1, buf, args );  
    va_end(args);
    s_resStr = s_Buf;
    return (char*)s_resStr.c_str();
}

AIRString CStringUtil::BeforeFirst(const AIRString& strSrc, char ch)
{
    size_t found = strSrc.find_first_of(ch);

    AIRString strOut;

    if (found != AIRString::npos)
    {
        strOut = strSrc.substr(0, found);
    }

    return strOut;
    
}
// get all characters before the last occurence of ch
// (returns empty string if ch not found)
AIRString CStringUtil::BeforeLast(const AIRString& strSrc, char ch)
{
    size_t found = strSrc.find_last_of(ch);

    AIRString strOut;

    if (found != AIRString::npos)
    {
        strOut = strSrc.substr(0, found);
    }

    return strOut;
}
// get all characters after the first occurence of ch
// (returns empty string if ch not found)
AIRString CStringUtil::AfterFirst(const AIRString& strSrc, char ch)
{
    size_t found = strSrc.find_first_of(ch);

    AIRString strOut;

    if (found != AIRString::npos)
    {
        strOut = strSrc.substr(found + 1);
    }

    return strOut;
}
// get all characters after the last occurence of ch
// (returns the whole string if ch not found)
AIRString CStringUtil::AfterLast(const AIRString& strSrc, char ch)
{
    size_t found = strSrc.find_last_of(ch);

    AIRString strOut;

    if (found != AIRString::npos)
    {
        strOut = strSrc.substr(found + 1);
    }

    return strOut;
}

bool CStringUtil::StartsWith(const AIRString& strSrc, const char* strStart)
{
    return strSrc.find_first_of(strStart) == 0;
}

bool CStringUtil::EndsWith(const AIRString& strSrc, const char* strEnd)
{
    size_t nPos = strSrc.find_last_of(strEnd);

    return strSrc.size() - nPos == strlen(strEnd) - 1;
}

AIRString CStringUtil::Lower(const AIRString& strSrc)
{
    AIRString strDest = strSrc;
#if AIR_TARGET_PLATFORM == AIR_PLATFORM_WIN32
    std::transform(strDest.begin(), strDest.end(), strDest.begin(), tolower);
#else
    u32 c = strDest.size();
    for ( int i = 0; i < c; ++i )
    {
        strDest[i] = toupper(strDest[i]);
    }
#endif

    return strDest;
}

AIRString CStringUtil::Upper(const AIRString& strSrc)
{
    AIRString strDest = strSrc;
#if AIR_TARGET_PLATFORM == AIR_PLATFORM_WIN32
    std::transform(strDest.begin(), strDest.end(), strDest.begin(), toupper);
#else
    u32 c = strDest.size();
    for ( int i = 0; i < c; ++i )
    {
        strDest[i] = toupper(strDest[i]);
    }
#endif

    return strDest;
}

AIRString CStringUtil::Mid(const AIRString& strSrc, size_t begin, size_t end /* = AIRString::npos */)
{
    if (end == AIRString::npos)
    {
        return strSrc.substr(begin);
    }
    else
    {
        return strSrc.substr(begin, end);
    }
}
