#pragma once


//#include "Common.h"
#include "EngineConfig.h"
#include <string>
#include <locale.h>
#include "MemoryAllocatorConfig.h"

#ifdef _WINDOWS
#define SLASH '\\'
#else
#define SLASH '/'
#endif


enum eStringType
{
	eST_INT = 0,
	eST_FLOAT = 1,
	eST_STRING = 2,
	eST_EMPTY = 3
};

class EngineExport CStringUtil
{
public:
    CStringUtil() {};
    virtual ~CStringUtil() {};

    /** Upper-cases all the characters in the string.
    */
    static void ToLowerCase( AIRString& str );

    /** Lower-cases all the characters in the string.
    */
    static void ToUpperCase( AIRString& str );

    static void SplitFilename(const AIRString& strSplit, AIRString& strPath, AIRString& strFile);

	static void SplitFilename(const AIRString& strSplit, AIRString& strLeft, AIRString& strRight, const AIRString& strSplitChar);

    static void Trim(AIRString& str, bool bLeft = true, bool bRight = true);

	static AIRString ws2s(const WAIRString& ws);

	static WAIRString s2ws(const AIRString& s);

	static eStringType GetStringType( const AIRString& str)
	{
		if( str.empty() )   
		{
			return eST_EMPTY;
		}

		int nDot = 0;   
		//数值只能是0到9及小数点组成   
		for(size_t i = 0; i < str.length(); i++ )   
		{   
			char ch = str[i];   
			if( '.'== ch )//小数点   
			{   
				nDot++;   
				if ( nDot > 1 )
				{
					return eST_STRING;
				}
				continue;   
			}   
			else if( ch >= '0' && ch <= '9' )//数字  
			{
				continue;   
			}
			else if ( ch == '-' && i != 0 )
			{
				return eST_STRING;
			}
			else
			{
				return eST_STRING;
			}
		}   
		if ( nDot == 0 )
		{
			return eST_INT;
		}
		return eST_FLOAT;
	}

    //---------------------------------------------------------
    // 名称:  SplitString
    // 功能:  字符串分解
    // 参数:  szSrc - 源字符串;
    //        szSplit - 分解字符串;
    //        vtRes - 分解出来的子串容器
    //---------------------------------------------------------
    static void SplitString( const AIRString& szSrc, const AIRString& szSplit, AIRVector<AIRString>& vtRes );

	//获得没有后缀名的文件名
	static const char* GetFilenameNoExt(const char* szFilename, char* szOut);

    //获得文件后缀名
    static const char* GetFilenameExt(const char* szFilename);

    //获得整个文件的文件名
    static const char* GetFilename(const char* szFilename);

    //下面是std::string的操作
    /** Upper-cases all the characters in the string.
    */
    static void ToLowerCaseStl( std::string& str );

    /** Lower-cases all the characters in the string.
    */
    static void ToUpperCaseStl( std::string& str );

    static void SplitFilenameStl(const std::string& strSplit, std::string& strPath, std::string& strFile);

    static void SplitFilenameStl(const std::string& strSplit, std::string& strLeft, std::string& strRight, const std::string& strSplitChar);

    static void TrimStl(std::string& str, bool bLeft = true, bool bRight = true);

    static std::string ws2sStl(const std::wstring& ws);

    static std::wstring s2wsStl(const std::string& s);

    static eStringType GetStringTypeStl( const std::string& str)
    {
        if( str.empty() )   
        {
            return eST_EMPTY;
        }

        int nDot = 0;   
        //数值只能是0到9及小数点组成   
        for(size_t i = 0; i < str.length(); i++ )   
        {   
            char ch = str[i];   
            if( '.'== ch )//小数点   
            {   
                nDot++;   
                if ( nDot > 1 )
                {
                    return eST_STRING;
                }
                continue;   
            }   
            else if( ch >= '0' && ch <= '9' )//数字  
            {
                continue;   
            }
            else if ( ch == '-' && i != 0 )
            {
                return eST_STRING;
            }
            else
            {
                return eST_STRING;
            }
        }   
        if ( nDot == 0 )
        {
            return eST_INT;
        }
        return eST_FLOAT;
    }

    static void SplitStringStl( const AIRString& szSrc, const AIRString& szSplit, AIRVector<AIRString>& vtRes );

	static AIRString Replace(AIRString& str, const char *string_to_replace, const char *new_string) 
	{ 
		// Find the first string to replace 
		int index = str.find(string_to_replace); 
		// while there is one 
		while(index != AIRString::npos) 
		{ 
			// Replace it 
			str.replace(index, strlen(string_to_replace), new_string); 
			//   Find   the   next   one 
			index = str.find(string_to_replace, index + strlen(new_string)); 
		} 
		return str;
	} 

	static bool IsSameFilePath(const AIRString& l, const AIRString& r);

	static AIRString CombineFilePath(const AIRString& strPath, const AIRString& strFile);

    static AIRString Format(char* buf, ...);

    static AIRString BeforeFirst(const AIRString& strSrc, char ch);
    // get all characters before the last occurence of ch
    // (returns empty string if ch not found)
    static AIRString BeforeLast(const AIRString& strSrc, char ch);
    // get all characters after the first occurence of ch
    // (returns empty string if ch not found)
    static AIRString AfterFirst(const AIRString& strSrc, char ch);
    // get all characters after the last occurence of ch
    // (returns the whole string if ch not found)
    static AIRString AfterLast(const AIRString& strSrc, char ch);

    static bool StartsWith(const AIRString& strSrc, const char* strStart);

    static bool EndsWith(const AIRString& strSrc, const char* strEnd);

    static AIRString Lower(const AIRString& strSrc);

    static AIRString Upper(const AIRString& strSrc);

    //返回字符的
    static AIRString Mid(const AIRString& strSrc, size_t begin, size_t end = AIRString::npos);

    static const AIRString BLANK;
    static const std::string BLANK_STL;
    static const AIRString STR_SLASH;
    static const std::string STR_SLASH_STL;
protected:
private:
};



