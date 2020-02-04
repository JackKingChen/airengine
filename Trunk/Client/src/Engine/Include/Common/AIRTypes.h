#pragma once 

namespace AIR
{
    //! 8 bit unsigned variable.
    /** This is a typedef for unsigned char, it ensures portability of the engine. */
#ifdef _MSC_VER
    typedef unsigned __int8		u8;
#else
    typedef unsigned char		u8;
#endif

    //! 8 bit signed variable.
    /** This is a typedef for signed char, it ensures portability of the engine. */
#ifdef _MSC_VER
    typedef __int8				s8;
#else
    typedef signed char			s8;
#endif

    //! 8 bit character variable.
    /** This is a typedef for char, it ensures portability of the engine. */
    typedef char				c8;



    //! 16 bit unsigned variable.
    /** This is a typedef for unsigned short, it ensures portability of the engine. */
#ifdef _MSC_VER
    typedef unsigned __int16	u16;
#else
    typedef unsigned short		u16;
#endif

    //! 16 bit signed variable.
    /** This is a typedef for signed short, it ensures portability of the engine. */
#ifdef _MSC_VER
    typedef __int16				s16;
#else
    typedef signed short		s16;
#endif



    //! 32 bit unsigned variable.
    /** This is a typedef for unsigned int, it ensures portability of the engine. */
#ifdef _MSC_VER
    typedef unsigned __int32	u32;
#else
    typedef unsigned int		u32;
#endif

    //! 32 bit signed variable.
    /** This is a typedef for signed int, it ensures portability of the engine. */
#ifdef _MSC_VER
    typedef __int32				s32;
#else
    typedef signed int			s32;
#endif



    // 64 bit signed variable.
    // This is a typedef for __int64, it ensures portability of the engine.
    // This type is currently not used by the engine and not supported by compilers
    // other than Microsoft Compilers, so it is outcommented.
    //typedef __int64			s64;
#ifdef _MSC_VER
	typedef __int64				s64;
#else
	typedef signed long long	s64;
#endif


#ifdef _MSC_VER
	typedef unsigned __int64	u64;
#else
	typedef unsigned long long  u64;
#endif



    //! 32 bit floating point variable.
    /** This is a typedef for float, it ensures portability of the engine. */
    typedef float				f32;

    //! 64 bit floating point variable.
    /** This is a typedef for double, it ensures portability of the engine. */
    typedef double				f64;

	const s8		MaxInt8		= 127;
	const u8		MaxUInt8	= 255;
	const s16		MaxInt16	= 32767;
	const u16		MaxUInt16	= 0xffff;
	const s32		MaxInt32	= 2147483647;
	const u32		MaxUInt32	= 0xffffffff;
	const f32		MaxFloat	= 3.402823466e+38F;
	const f64		MaxDouble	= 1.7976931348623158e+308;
	const s8		MinInt8		= -127-1;
	const s16		MinInt16	= -32767-1;
	const s32		MinInt32	= -2147483647-1;
	const f32		MinFloat	= 1.175494351e-38F;
	const f64		MinDouble	= 2.2250738585072014e-308;

};
