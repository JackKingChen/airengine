//------------------------------------------------------------------------------
/**
    @class	PlatformConfig

	@brief	predefine platform macro values for cross platform

    @date	2013-4-12

	@author HJJ
*/
//------------------------------------------------------------------------------

#pragma once

#ifndef __PlatformConfig_H__
#define __PlatformConfig_H__

//#define SHOW_COMPILE_MESSAGE

//////////////////////////////////////////////////////////////////////////
// pre configure
//////////////////////////////////////////////////////////////////////////

// define supported target platform macro
#define AIR_PLATFORM_UNKNOWN			0
#define AIR_PLATFORM_IOS				1
#define AIR_PLATFORM_ANDROID			2
#define AIR_PLATFORM_WIN32				3
#define AIR_PLATFORM_LINUX				4
#define AIR_PLATFORM_MAC				5

// define compiler macro
#define AIR_COMPILER_NONE				0
#define AIR_COMPILER_MSVC 				1
#define AIR_COMPILER_GNUC 				2
#define AIR_COMPILER_BORL 				3
#define AIR_COMPILER_WINSCW 			4
#define AIR_COMPILER_GCCE 				5

// define architecture macro
#define AIR_ARCHITECTURE_32 			1
#define AIR_ARCHITECTURE_64 			2


//////////////////////////////////////////////////////////////////////////
// Finds the compiler type and version.
//////////////////////////////////////////////////////////////////////////
#define AIR_COMPILER			 		AIR_COMPILER_NONE

#if defined( __GCCE__ )
#	undef AIR_COMPILER
#	define AIR_COMPILER					AIR_COMPILER_GCCE
#	define AIR_COMP_VER					_MSC_VER
#ifdef SHOW_COMPILE_MESSAGE
#	pragma message("AIR_COMPILER = AIR_COMPILER_GCCE")
#endif
//#	include <staticlibinit_gcce.h> // This is a GCCE toolchain workaround needed when compiling with GCCE 
#elif defined( __WINSCW__ )
#	undef AIR_COMPILER
#	define AIR_COMPILER					AIR_COMPILER_WINSCW
#	define AIR_COMP_VER					_MSC_VER
#ifdef SHOW_COMPILE_MESSAGE
#	pragma message("AIR_COMPILER = AIR_COMPILER_WINSCW")
#endif
#elif defined( _MSC_VER )
#	undef AIR_COMPILER
#	define AIR_COMPILER					AIR_COMPILER_MSVC
#	define AIR_COMP_VER					_MSC_VER
#ifdef SHOW_COMPILE_MESSAGE
#	pragma message("AIR_COMPILER = AIR_COMPILER_MSVC")
#endif
#elif defined( __GNUC__ )
#	undef AIR_COMPILER
#	define AIR_COMPILER					AIR_COMPILER_GNUC
#	define AIR_COMP_VER					(((__GNUC__)*100) + (__GNUC_MINOR__*10) + __GNUC_PATCHLEVEL__)		  
#ifdef SHOW_COMPILE_MESSAGE
#	pragma message("AIR_COMPILER = AIR_COMPILER_GNUC")
#endif
#elif defined( __BORLANDC__ )
#	undef AIR_COMPILER
#	define AIR_COMPILER					AIR_COMPILER_BORL
#	define AIR_COMP_VER					__BCPLUSPLUS__
#	define __FUNCTION__					__FUNC__	 
#ifdef SHOW_COMPILE_MESSAGE
#	pragma message("AIR_COMPILER = AIR_COMPILER_BORL")
#endif
#endif



//////////////////////////////////////////////////////////////////////////
// Determine target platform by compile environment macro.
//////////////////////////////////////////////////////////////////////////
#define AIR_TARGET_PLATFORM				AIR_PLATFORM_UNKNOWN

// win32
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(WINDOWS)
#	undef  AIR_TARGET_PLATFORM
#	define AIR_TARGET_PLATFORM			AIR_PLATFORM_WIN32
#ifdef SHOW_COMPILE_MESSAGE
#	pragma message("AIR_TARGET_PLATFORM = AIR_PLATFORM_WIN32")
#endif
#endif

// linux / android
#if defined(LINUX) || defined(linux) || defined(__linux__)
#	undef  AIR_TARGET_PLATFORM
#	if defined(ANDROID) || defined(__ANDROID__)
#		define AIR_TARGET_PLATFORM		AIR_PLATFORM_ANDROID
#ifdef SHOW_COMPILE_MESSAGE
#		pragma message("AIR_TARGET_PLATFORM = AIR_PLATFORM_ANDROID")
#endif
#	else
#		define AIR_TARGET_PLATFORM		AIR_PLATFORM_LINUX
#ifdef SHOW_COMPILE_MESSAGE
#		pragma message("AIR_TARGET_PLATFORM = AIR_PLATFORM_LINUX")
#endif
#	endif
#endif

// mac / ios
#if defined(__APPLE_CC__) || defined(APPLE) || defined(__MACH__)
#	undef  AIR_TARGET_PLATFORM
#	if (__ENVIRONMENT_IPHONE_OS_VERSION_MIN_REQUIRED__ >= 40000 || __IPHONE_OS_VERSION_MIN_REQUIRED >= 40000)
#		define AIR_TARGET_PLATFORM		AIR_PLATFORM_IOS
#ifdef SHOW_COMPILE_MESSAGE
#		pragma message("AIR_TARGET_PLATFORM = AIR_PLATFORM_IOS")
#endif
#	else 
#		define AIR_TARGET_PLATFORM		AIR_PLATFORM_MAC
#ifdef SHOW_COMPILE_MESSAGE
#		pragma message("AIR_TARGET_PLATFORM = AIR_PLATFORM_MAC")
#endif
#	endif
#endif



//////////////////////////////////////////////////////////////////////////
// Find the architecture type
//////////////////////////////////////////////////////////////////////////
#if defined(__x86_64__) || defined(_M_X64) || defined(__powerpc64__) || defined(__alpha__) || defined(__ia64__) || defined(__s390__) || defined(__s390x__)
#	define AIR_ARCH_TYPE				AIR_ARCHITECTURE_64
#ifdef SHOW_COMPILE_MESSAGE
#	pragma message("AIR_ARCH_TYPE = AIR_ARCHITECTURE_64")
#endif
#else
#	define AIR_ARCH_TYPE				AIR_ARCHITECTURE_32
#ifdef SHOW_COMPILE_MESSAGE
#	pragma message("AIR_ARCH_TYPE = AIR_ARCHITECTURE_32")
#endif
#endif



//////////////////////////////////////////////////////////////////////////
// post configure
//////////////////////////////////////////////////////////////////////////

// check user set platform
#if (AIR_TARGET_PLATFORM == AIR_PLATFORM_UNKNOWN)
#error  "Cannot recognize the target platform, Abort!"
#endif

// check user set compiler
#if (AIR_COMPILER == AIR_COMPILER_NONE)
#error  "Cannot recognize the compiler, Abort!"
#endif

#if (AIR_TARGET_PLATFORM == AIR_PLATFORM_WIN32)
#	pragma warning (disable:4127)  
#endif

#if (AIR_TARGET_PLATFORM == AIR_PLATFORM_WIN32)
#	if defined(AIR_STATIC_LIB)
#		define		AIR_API
#ifdef SHOW_COMPILE_MESSAGE
#		pragma message("win platform, static lib")
#endif
#	else
#		if defined(AIR_EXPORT) || defined(_SHARED_)
#			define	AIR_API				__declspec( dllexport )
#ifdef SHOW_COMPILE_MESSAGE
#			pragma message("win platform, export dynamic lib")
#endif
#		else
#			define	AIR_API				__declspec( dllimport )
#ifdef SHOW_COMPILE_MESSAGE
#			pragma message("win platform, import dynamic lib")
#endif
#		endif
#	endif
#else
#   if defined(AIR_STATIC_LIB)
#       define		AIR_API
#   else
#       if defined(AIR_EXPORT) || defined(_SHARED_)
#           define  AIR_API             __attribute__((visibility("default")))
#		else
#			define	AIR_API				
#		endif
#	endif
#endif

#endif //__PlatformConfig_H__
