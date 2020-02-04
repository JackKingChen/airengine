#include "stdafx.h"
#include "Util.h"
#include "xstring.h"

#if AIR_TARGET_PLATFORM == AIR_PLATFORM_ANDROID || AIR_TARGET_PLATFORM == AIR_PLATFORM_IOS
#include "iconv.h"
#endif

size_t UnicodeStrLen(const wchar_t *pwstr)
{
#if AIR_TARGET_PLATFORM == AIR_PLATFORM_ANDROID || AIR_TARGET_PLATFORM == AIR_PLATFORM_IOS
	size_t i = 0;
	while (*(pwstr+i)) 
	{
		i++;
	}
	return i;
#else
	return wcslen( pwstr );
#endif
}

//
//int UnicodeToAnsi( char *pdestbuf, int maxchar, const wchar_t *psrc, int nsrcchar )
//{
//#if AIR_TARGET_PLATFORM == AIR_PLATFORM_WIN32
//	int nchar;
//	nchar = WideCharToMultiByte(
//		CP_ACP,
//		WC_SEPCHARS | WC_COMPOSITECHECK,
//		psrc,
//		nsrcchar,
//		pdestbuf,
//		maxchar,
//		NULL,
//		NULL );
//	return nchar;
//#else
//	iconv_t cd = iconv_open ("UTF-8", "UTF-16LE");
//	if (cd == (iconv_t)-1)
//	{
//		return 0;
//	}
//
//	if (nsrcchar==-1)
//	{
//		nsrcchar = (int)UnicodeStrLen(psrc);
//	}
//
//	size_t inbytesleft = nsrcchar*sizeof(wchar_t);
//	if (inbytesleft <= 0)
//	{
//		iconv_close(cd);
//		return 0;
//	}
//
//	unsigned long outleft = maxchar;
//	size_t ee = iconv(cd, (char**)&psrc, &inbytesleft, (char**)&pdestbuf, &outleft);
//	if (ee == (size_t)(-1))
//	{
//		ee = 0;
//	}
//	iconv_close(cd);
//	return maxchar-(int)outleft;
//#endif
//}
//
//int AnsiToUnicode( wchar_t *pdestbuf, int maxchar, const char *psrc, int nsrcchar )
//{
//#if AIR_TARGET_PLATFORM == AIR_PLATFORM_WIN32
//	int nchar;
//	nchar = MultiByteToWideChar(
//		CP_ACP,
//		MB_PRECOMPOSED,
//		psrc,
//		nsrcchar,
//		pdestbuf,
//		maxchar );
//	return nchar;
//#else
//	iconv_t cd = iconv_open ("UTF-16LE", "UTF-8");
//	if (cd == (iconv_t)-1)
//	{
//		return 0;
//	}
//
//	size_t inbytesleft = nsrcchar;
//	if (nsrcchar==-1)
//	{
//		inbytesleft = strlen(psrc);
//	}
//
//	if (inbytesleft <= 0)
//	{
//		iconv_close(cd);
//		return 0;
//	}
//
//	unsigned long outleft = maxchar*sizeof(wchar_t);
//	size_t ee = iconv(cd, (char**)&psrc, &inbytesleft, (char**)&pdestbuf, &outleft);
//	if (ee == (size_t)(-1))
//	{
//		ee = 0;
//	}
//	iconv_close(cd);
//	return maxchar-(int)outleft/2;
//#endif
//}
//
//
//int UnicodeToUTF8( char *pdestbuf, int maxchar, const wchar_t *psrc, int nsrcchar )
//{
//	u8 *pdst = (u8 *)pdestbuf;
//	const wchar_t *pcur = psrc;
//	int src_count = nsrcchar<0 ? MaxInt32 : nsrcchar;
//	while(*pcur && src_count>0 )
//	{
//		src_count--;
//		wchar_t c = *pcur++;
//		if( c < 0x80 )
//		{
//			*pdst++ = (u8)c;
//		}
//		else if( c < 0x800 )
//		{
//			*pdst++ = (u8)((c>>6) | 0xC0);
//			*pdst++ = (c&0x3F) | 0x80;
//		}
//		else
//		{
//			*pdst++ = (c>>12) | 0xE0;
//			*pdst++ = ((c>>6)&0x3F) | 0x80;
//			*pdst++ = (c&0x3F) | 0x80;
//		}
//	}
//	*pdst++ = 0;
//	int nchar = int((char *)pdst - (char *)pdestbuf - 1);
//	//assert(nchar < maxchar);
//	return nchar;
//}
//
//int UTF8ToUnicode( wchar_t *pdestbuf, int maxchar, const char *psrc, int nsrcchar )
//{
//	wchar_t *pdst = pdestbuf;
//	const u8 *pcur = (u8 *)psrc;
//	int src_count = nsrcchar<0 ? MaxInt32 : nsrcchar;
//	while( *pcur && src_count>0 )
//	{
//		src_count--;
//		u8 head = *pcur++;
//		u8 mask = 0x80;
//		wchar_t  c = 0;
//		int bitshift = 0;
//		if( head&mask )
//		{
//			mask >>= 1;
//			while( head&mask )
//			{
//				//				assert( *pcur!=0 && src_count>0 );
//				src_count--;
//				mask >>= 1;
//				c = (c<<6) | (*pcur++ & 0x3F);
//				bitshift += 6;
//			}
//		}
//		c |= (head&(mask-1)) << bitshift;
//		*pdst++ = c;
//	}
//	*pdst++ = 0;
//	int nchar = int((char *)pdst - (char *)pdestbuf - 1);
//	//assert(nchar < maxchar);
//	return nchar;
//}
//
//const size_t MAX_CONVERT = 4096*2;
//static char s_DBCSBuffer[MAX_CONVERT];
//static wchar_t s_UnicodeBuffer[MAX_CONVERT];
//
//int UTF8ToAnsi( char* pdestbuf, int maxchar, const char *psrc, int nsrcchar )
//{
//	UTF8ToUnicode(s_UnicodeBuffer , 1024 , psrc, -1);
//	return UnicodeToAnsi(pdestbuf , 1024 , s_UnicodeBuffer, -1);				
//}
//
//const char* UnicodeToAnsi( const wchar_t *psrc, int nsrcchar = -1 )
//{
//	UnicodeToAnsi(s_DBCSBuffer, MAX_CONVERT, psrc, nsrcchar);
//	return s_DBCSBuffer;
//}
//
//const wchar_t* AnsiToUnicode( const char *psrc, int nsrcchar = -1 )
//{
//	AnsiToUnicode(s_UnicodeBuffer, MAX_CONVERT, psrc, nsrcchar);
//	return s_UnicodeBuffer;
//}
//
//const char* UnicodeToUTF8( const wchar_t *psrc, int nsrcchar = -1 )
//{
//	UnicodeToUTF8(s_DBCSBuffer, MAX_CONVERT, psrc, nsrcchar);
//	return s_DBCSBuffer;
//}
//
//const wchar_t* UTF8ToUnicode( const char *psrc, int nsrcchar = -1 )
//{
//	UTF8ToUnicode(s_UnicodeBuffer, MAX_CONVERT, psrc, nsrcchar);
//	return s_UnicodeBuffer;
//}
//
//const char* UTF8ToAnsi( const char *psrc, int nsrcchar = -1 )
//{
//	UTF8ToAnsi(s_DBCSBuffer, MAX_CONVERT, psrc, nsrcchar);
//	return s_DBCSBuffer;
//}
//
//const char* AnsiToUTF8(const char* psrc, int length = -1)
//{
//	int wCharLength = AnsiToUnicode(s_UnicodeBuffer, MAX_CONVERT, psrc, length);
//	return UnicodeToUTF8(s_UnicodeBuffer, wCharLength);
//}


u32 WCharToMByte(const wchar_t* lpcwszStr, char* lpszStr, u32 dwSize)
{
#if AIR_TARGET_PLATFORM == AIR_PLATFORM_WIN32

    setlocale(LC_ALL, "chs");
    dwSize = wcstombs(lpszStr, lpcwszStr, dwSize);

#elif AIR_TARGET_PLATFORM == AIR_PLATFORM_ANDROID || AIR_TARGET_PLATFORM == AIR_PLATFORM_IOS

	AIR::XEvol_UnicodeToUtf8( lpcwszStr, lpszStr, dwSize );

#endif
    return dwSize;
}

u32 MByteToWChar(const char* szIn, wchar_t* wszOut, u32 nSize)
{
#if AIR_TARGET_PLATFORM == AIR_PLATFORM_WIN32

	setlocale(LC_ALL, "chs"); 
    nSize = mbstowcs(wszOut, szIn, nSize);
	
#elif AIR_TARGET_PLATFORM == AIR_PLATFORM_ANDROID || AIR_TARGET_PLATFORM == AIR_PLATFORM_IOS

	AIR::XEvol_Utf8ToUnicode( szIn, wszOut, nSize );

#endif

	return nSize;
}



//
//u32 WCharToMByte(const wchar_t* lpcwszStr, char* lpszStr, u32 dwSize)
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
//
//u32 MByteToWChar(const char* szIn, wchar_t* wszOut, u32 nSize)
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

void PrintDebugString( char* buf, ... )
{
	char s_CSPrintBuf[4096];
	memset( s_CSPrintBuf, 0, sizeof(s_CSPrintBuf) );
	int		nBuf;
	va_list args;
	va_start( args, buf );
#if AIR_TARGET_PLATFORM == AIR_PLATFORM_WIN32
	nBuf = _vsnprintf_s( s_CSPrintBuf, sizeof( s_CSPrintBuf ), sizeof( s_CSPrintBuf ) - 1, buf, args ); 
#else
	nBuf = vsnprintf( s_CSPrintBuf, sizeof(s_CSPrintBuf)-1, buf, args );
	//nBuf = snprintf( s_CSPrintBuf, sizeof(s_CSPrintBuf) - 1, buf, args );
#endif
	va_end(args);

#if AIR_TARGET_PLATFORM == AIR_PLATFORM_WIN32
	OutputDebugStringA( s_CSPrintBuf );
#elif AIR_TARGET_PLATFORM == AIR_PLATFORM_ANDROID
	DroidLogD( "%s", s_CSPrintBuf );
#else
	std::cout<<s_CSPrintBuf<<"\n";
#endif


}
