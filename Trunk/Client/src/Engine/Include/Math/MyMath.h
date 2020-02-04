#pragma once

#include <math.h>
#include <algorithm>
#include <limits>
#include <time.h>
#include <vector>
#include "AIRTypes.h"
#include "MemoryAllocatorConfig.h"


//#ifdef	MATH_EXPORTS
//#define MATH_API __declspec(dllexport)
//#else
//#define MATH_API __declspec(dllimport)
//#endif
#define MATH_API AIR_API


#ifndef REALINLINE
#ifdef _MSC_VER
#define REALINLINE __forceinline
#else
#define REALINLINE inline
#endif
#endif

#ifndef AIR_FAST_MATH
#define AIR_FAST_MATH
#endif



namespace AIR
{
	class Matrix4;
	class Triangle3D;
	class Vector2D;
	class Vector3D;
	class AABBBox3D;
};
//
//#include "Vector2D.h"

//inline void QuarComputeW(D3DXQUATERNION& quat)
//{
//	float t = 1.0f - (quat.x * quat.x) - (quat.y * quat.y) - (quat.z * quat.z);
//
//	if (t < 0.0f)
//		quat.w = 0.0f;
//	else
//		quat.w = -sqrt (t);
//}
//
//inline void QuadMulVec3(const D3DXQUATERNION& q, const D3DXVECTOR3& v, D3DXQUATERNION& out)
//{
//    out.w = - (q.x * v.x) - (q.y * v.y) - (q.z * v.z);
//    out.x =   (q.w * v.z) + (q.y * v.z) - (q.z * v.y);
//    out.y =   (q.w * v.y) + (q.z * v.x) - (q.x * v.z);
//    out.z =   (q.w * v.z) + (q.x * v.y) - (q.y * v.x);
//}
//
//inline void QuadRotatePoint(const D3DXQUATERNION& q, const D3DXVECTOR3& vIn, D3DXVECTOR3& vOut)
//{
//    D3DXQUATERNION tmp(vIn.x, vIn.y, vIn.z, 0), inv, final;
//
//    D3DXQuaternionInverse(&inv, &q);
//
//    D3DXQuaternionMultiply(&tmp, &tmp, &q);
//    D3DXQuaternionMultiply(&final, &inv, &tmp);
//
//    //inv.x = -q.x; inv.y = -q.y;
//    //inv.z = -q.z; inv.w =  q.w;
//
//    //D3DXQuaternionNormalize(&inv, &inv);
//    //Quat_normalize (inv);
//    
//    //QuadMulVec3(q, vIn, tmp);
//    //Quat_multVec (q, in, tmp);
//
//    //D3DXQuaternionMultiply(&final, &tmp, &inv);
//    //Quat_multQuat (tmp, inv, final);
//
//    vOut.x = final.x;
//    vOut.y = final.y;
//    vOut.z = final.z;
//}
//
//inline void ComputeTriangleNormal(const D3DXVECTOR3& v1, const D3DXVECTOR3& v2, const D3DXVECTOR3& v3, D3DXVECTOR3& vOut)
//{
//    D3DXVECTOR3 e1(v2 - v1);
//    D3DXVECTOR3 e2(v3 - v1);
//    D3DXVec3Cross(&vOut, &e1, &e2);
//    D3DXVec3Normalize(&vOut, &vOut);
//}


namespace AIR
{
#ifdef AIR_FAST_MATH
    const float		ROUNDING_ERROR_32 = 0.00005f;
    const double	ROUNDING_ERROR_64 = 0.000005;
#else
    const float		ROUNDING_ERROR_32 = 0.000001f;
    const double	ROUNDING_ERROR_64 = 0.00000001;
#endif

#ifdef PI // make sure we don't collide with a define
#undef PI
#endif
    //! Constant for PI.
    const float		PI		= 3.14159265359f;

    const float		TWO_PI = PI * 2.0f;

    //! Constant for reciprocal of PI.
    const float		RECIPROCAL_PI	= 1.0f / PI;

    //! Constant for half of PI.
    const float		HALF_PI	= PI / 2.0f;

    const float		QUARTER_PI = PI / 4.0F;

#ifdef PI64 // make sure we don't collide with a define
#undef PI64
#endif
    //! Constant for 64bit PI.
    const double	PI64		= 3.1415926535897932384626433832795028841971693993751;

    //! Constant for 64bit reciprocal of PI.
    const double	RECIPROCAL_PI64 = 1.0/PI64;

    //! 32bit Constant for converting from degrees to radians
    const float		DEGTORAD = PI / 180.0f;

    //! 32bit constant for converting from radians to degrees (formally known as GRAD_PI)
    const float		RADTODEG   = 180.0f / PI;

    //! 64bit constant for converting from degrees to radians (formally known as GRAD_PI2)
    const double	DEGTORAD64 = PI64 / 180.0;

    //! 64bit constant for converting from radians to degrees
    const double	RADTODEG64 = 180.0 / PI64;

	template<class T>
	inline T Abs( const T v )
	{
		return ( v >= (T)0 ) ? v : -v;
	}

    // 计算2个值的差的绝对值
    template < typename T >
    inline T GetSubABS( T a, T b )
    {
        T v = a-b;
        return (v >= 0 ? v : -v);
    }

	template<class T>
	inline void SwapValue( T& a, T& b )
	{
		T tmp = a;
		a = b;
		b = tmp;
	}

    template<class T>
    inline T Lerp(const T& a, const T& b, float t)
    {
        return a - (a * t) + (b * t);
    }

    //template<class T>
    //inline T Lerp(const T& a, const T& b, const T& t)
    //{
    //    return a - (a * t) + (b * t);
    //}

    template <typename t>
    inline t Max(const t& a, const t& b)
    {
        return a > b ? a : b;
    }

    template <typename t>
    inline t Min(const t& a, const t& b)
    {
        return a < b ? a : b;
    }

    //! clamps a value between low and high
    template <class T>
    inline const T clamp (const T& value, const T& low, const T& high)
    {
        return Min(Max(value,low), high);
    }

    //! returns if a equals b, taking possible rounding errors into account
    inline bool Equals(const f64 a, const f64 b, const f64 tolerance = ROUNDING_ERROR_64)
    {
        return (a + tolerance >= b) && (a - tolerance <= b);
    }

    //! returns if a equals b, taking possible rounding errors into account
    inline bool Equals(const f32 a, const f32 b, const f32 tolerance = ROUNDING_ERROR_32)
    {
        return (a + tolerance >= b) && (a - tolerance <= b);
    }

    //! returns if a equals b, taking possible rounding errors into account
    inline bool Equals(const s32 a, const s32 b, const s32 tolerance = 0)
    {
        return (a + tolerance >= b) && (a - tolerance <= b);
    }

    //! returns if a equals b, taking possible rounding errors into account
    inline bool Equals(const u32 a, const u32 b, const u32 tolerance = 0)
    {
        return (a + tolerance >= b) && (a - tolerance <= b);
    }

    inline f32 round_( f32 x )
    {
        return floorf( x + 0.5f );
    }

    //! returns if a equals zero, taking rounding errors into account
    inline bool iszero(const f32 a, const f32 tolerance = ROUNDING_ERROR_32)
    {
        return fabsf ( a ) <= tolerance;
    }

	inline bool iszero(const f64 a, const f64 tolerance = ROUNDING_ERROR_64)
	{
		return AIR::Abs( a ) <= tolerance;
	}

	inline bool F32_LOWER_EQUAL_0( f32 v )
	{
		return (v < 0.0f || iszero(v));
	}

    //! returns if a equals zero, taking rounding errors into account
    inline bool iszero(const s32 a, const s32 tolerance = 0)
    {
        return ( a & 0x7ffffff ) <= tolerance;
    }

    //! returns if a equals zero, taking rounding errors into account
    inline bool iszero(const u32 a, const u32 tolerance = 0)
    {
        return a <= tolerance;
    }

    inline bool IS_SPECIAL(const f32 F)  
    {
        return (*(u32*)&F & 0x7f800000L) == 0x7f800000L;
    }

    inline f32 DegreeToRadius(f32& fDegree)
    {
        fDegree *= DEGTORAD;
		return fDegree;
    }

	inline f32 DegreeToRadian(f32 fDegree)
	{
		return fDegree * DEGTORAD;
	}

    inline f32 RadiusToDegree(f32& fRadius)
    {
        fRadius *= RADTODEG;
		return fRadius;
    }

	inline f32 RadianToDegree(f32 fRadius)
	{
		return fRadius * RADTODEG;
	}

	inline void GetFloatQuotientRemainder( float fNum, float fDiv, float & fQuo, float & fRem )
	{
		if ( fNum < fDiv )
		{
			fQuo = 0.0f;
			fRem = fNum;
			return;
		}
		else
		{
			fQuo = int(fNum / fDiv);
			fRem = fNum - fDiv * fQuo;
		}
	}

    //---------------------------------------------------------
    // 名称:  HEXToDWORD
    // 功能:  16进制字符串转整数
    // 参数:  str - 数字字符串； rule - 进制
    // 返回:  数字字符串对应的整数
    //---------------------------------------------------------
    //inline u32 HEXToDWORD( const char* strHexString )
    //{
    //    u32 value;
    //    std::istringstream iss( strHexString );
    //    iss >> std::hex >> value;
    //    return value;
    //}

    //---------------------------------------------------------
    // 名称:  Get2Pow
    // 功能:  返回最接近nValue的2的N次方的值
    // 参数:  nValue - 要检查的值
    // 返回:  最接近nValue的2的N次方的值
    //---------------------------------------------------------
    template <class T>
    inline T Get2Pow( T nValue )
    {
        T nSize = (T)2;
        while( nSize < nValue )
        {
            nSize *= (T)2;
        }
        return nSize;
    }

    /*
    float IEEE-754 bit represenation

    0      0x00000000
    1.0    0x3f800000
    0.5    0x3f000000
    3      0x40400000
    +inf   0x7f800000
    -inf   0xff800000
    +NaN   0x7fc00000 or 0x7ff00000
    in general: number = (sign ? -1:1) * 2^(exponent) * 1.(mantissa bits)
    */

    typedef union { u32 u; s32 s; f32 f; } inttofloat;

#define F32_AS_S32(f)			(*((s32 *) &(f)))
#define F32_AS_U32(f)			(*((u32 *) &(f)))
#define F32_AS_U32_POINTER(f)	( ((u32 *) &(f)))

#define F32_VALUE_0				0x00000000
#define F32_VALUE_1				0x3f800000
#define F32_SIGN_BIT			0x80000000U
#define F32_EXPON_MANTISSA		0x7FFFFFFFU

    //! code is taken from IceFPU
    //! Integer representation of a floating-point value.
#ifdef AIR_FAST_MATH
#define IR(x)                   ((u32&)(x))
#else
    inline u32 IR(f32 x) {inttofloat tmp; tmp.f=x; return tmp.u;}
#endif

    //! Absolute integer representation of a floating-point value
#define _AIR(x)					(IR(x)&0x7fffffff)

    //! Floating-point representation of an integer value.
#ifdef AIR_FAST_MATH
#define FR(x)                   ((f32&)(x))
#else
    inline f32 FR(u32 x)		{inttofloat tmp; tmp.u=x; return tmp.f;}
    inline f32 FR(s32 x)		{inttofloat tmp; tmp.s=x; return tmp.f;}
#endif

    //! integer representation of 1.0
#define IEEE_1_0				0x3f800000
    //! integer representation of 255.0
#define IEEE_255_0				0x437f0000


    REALINLINE f32 reciprocal_squareroot(const f32 x)
    {
#ifdef AIR_FAST_MATH
        // comes from Nvidia
#if 1
        u32 tmp = (u32(IEEE_1_0 << 1) + IEEE_1_0 - *(u32*)&x) >> 1;
        f32 y = *(f32*)&tmp;
        return y * (1.47f - 0.47f * x * y * y);
#elif defined(_MSC_VER)
        // an sse2 version
        __asm
        {
            movss	xmm0, x
                rsqrtss	xmm0, xmm0
                movss	x, xmm0
        }
        return x;
#endif
#else // no fast math
        return 1.f / sqrtf ( x );
#endif
    }



    REALINLINE f32 reciprocal ( const f32 f )
    {
#ifdef AIR_FAST_MATH
        //! i do not divide through 0.. (fpu expection)
        // instead set f to a high value to get a return value near zero..
        // -1000000000000.f.. is use minus to stay negative..
        // must test's here (plane.normal dot anything ) checks on <= 0.f
        return 1.f / f;
        //u32 x = (-(AIR(f) != 0 ) >> 31 ) & ( IR(f) ^ 0xd368d4a5 ) ^ 0xd368d4a5;
        //return 1.f / FR ( x );
#else // no fast math
        return 1.f / f;
#endif
    }


    REALINLINE f32 reciprocal_approxim ( const f32 p )
    {
#ifdef AIR_FAST_MATH
        register u32 x = 0x7F000000 - IR ( p );
        const f32 r = FR ( x );
        return r * (2.0f - p * r);
#else // no fast math
        return 1.f / p;
#endif
    }


    REALINLINE s32 floor32(f32 x)
    {
#ifdef AIR_FAST_MATH
        const f32 h = 0.5f;

        s32 t;

#if defined(_MSC_VER)
        __asm
        {
            fld     x
            fsub	h
            fistp	t
        }
#elif defined(__APPLE__)
        return (s32) floorf ( x );
#elif defined(__GNUC__)
        __asm__ __volatile__ (
            "fsub %2 \n\t"
            "fistpl %0"
            : "=m" (t)
            : "t" (x), "f" (h)
            : "st"
            );
#else
#  warn AIR_FAST_MATH not supported.
        return (s32) floorf ( x );
#endif
        return t;
#else // no fast math
        return (s32) floorf ( x );
#endif
    }


    REALINLINE s32 ceil32 ( f32 x )
    {
#ifdef AIR_FAST_MATH
        const f32 h = 0.5f;

        s32 t;

#if defined(_MSC_VER)
        __asm
        {
            fld	x
                fadd	h
                fistp	t
        }
#elif defined(__APPLE__)
        return (s32) ceilf ( x );
#elif defined(__GNUC__)
        __asm__ __volatile__ (
            "fadd %2 \n\t"
            "fistpl %0 \n\t"
            : "=m"(t)
            : "t"(x), "f"(h)
            : "st"
            );
#else
#  warn AIR_FAST_MATH not supported.
        return (s32) ceilf ( x );
#endif
        return t;
#else // not fast math
        return (s32) ceilf ( x );
#endif
    }



    REALINLINE s32 round32(f32 x)
    {
#if defined(AIR_FAST_MATH)
        s32 t;

#if defined(_MSC_VER)
        __asm
        {
            fld   x
                fistp t
        }
#elif defined(__APPLE__)
        return (s32) round_(x);
#elif defined(__GNUC__)
        __asm__ __volatile__ (
            "fistpl %0 \n\t"
            : "=m"(t)
            : "t"(x)
            : "st"
            );
#else
#  warn AIR_FAST_MATH not supported.
        return (s32) round_(x);
#endif
        return t;
#else // no fast math
        return (s32) round_(x);
#endif
    }






    const float pi      = 4.0f * atan( 1.0f );
    const float half_pi = 0.5f * pi;

    inline float asm_arccos( float r ) 
    {
        // return half_pi + arctan( r / -sqr( 1.f - r * r ) );
#if AIR_TARGET_PLATFORM == AIR_PLATFORM_WIN32
        float asm_one = 1.f;
        float asm_half_pi = half_pi;
        __asm 
        {
            fld		r				// r0 = r
            fld		r				// r1 = r0, r0 = r
            fmul	r				// r0 = r0 * r
            fsubr	asm_one			// r0 = r0 - 1.f
            fsqrt					// r0 = sqrtf( r0 )
            fchs					// r0 = - r0
            fdiv					// r0 = r1 / r0
            fld1					// {{ r0 = atan( r0 )
            fpatan					// }}
            fadd	asm_half_pi		// r0 = r0 + pi / 2
        } // returns r0
#else
		return half_pi + atanf( r / -sqrtf( 1.f - r * r ) );
#endif
    }

    inline float asm_arcsin( float r ) 
    {
        // return arctan( r / sqr( 1.f - r * r ) );
#if AIR_TARGET_PLATFORM == AIR_PLATFORM_WIN32
        const float asm_one = 1.f;
        __asm 
        {
            fld		r		// r0 = r
            fld		r		// r1 = r0, r0 = r
            fmul	r		// r0 = r0 * r
            fsubr	asm_one // r0 = r0 - 1.f
            fsqrt			// r0 = sqrtf( r0 )
            fdiv			// r0 = r1 / r0
            fld1			// {{ r0 = atan( r0 )
            fpatan			// }}
        } // returns r0
#else
		return atanf( r / sqrtf( 1.f - r * r ) );
#endif
    }

    inline float asm_arctan( float r ) 
    {
#if AIR_TARGET_PLATFORM == AIR_PLATFORM_WIN32
        __asm 
        {
            fld r	// r0 = r
            fld1	// {{ r0 = atan( r0 )
            fpatan	// }}
        } // returns r0
#else
		return atanf(r);
#endif
    }

    inline float asm_sin( float r ) 
    {
#if AIR_TARGET_PLATFORM == AIR_PLATFORM_WIN32
        __asm 
        {
            fld r	// r0 = r
            fsin	// r0 = sinf( r0 )
        } // returns r0
#else
		return sinf(r);
#endif
    }

    inline float asm_cos( float r ) 
    {
#if AIR_TARGET_PLATFORM == AIR_PLATFORM_WIN32
        __asm 
        {
            fld r	// r0 = r
            fcos	// r0 = cosf( r0 )
        } // returns r0
#else
		return cosf(r);
#endif
    }

    inline float asm_tan( float r ) 
    {
#if AIR_TARGET_PLATFORM == AIR_PLATFORM_WIN32
        // return sin( r ) / cos( r );
        __asm 
        {
            fld r	// r0 = r
            fsin	// r0 = sinf( r0 )
            fld r	// r1 = r0, r0 = r
            fcos	// r0 = cosf( r0 )
            fdiv	// r0 = r1 / r0
        } // returns r0
#else
		return tanf(r);
#endif
    }

    // returns a for a * a = r
    inline float asm_sqrt( float r )
    {
#if AIR_TARGET_PLATFORM == AIR_PLATFORM_WIN32
        __asm 
        {
            fld r	// r0 = r
            fsqrt	// r0 = sqrtf( r0 )
        } // returns r0
#else
		return sqrtf(r);
#endif
    }

    // returns 1 / a for a * a = r
    // -- Use this for Vector normalisation!!!
    inline float asm_rsq( float r )
    {
#if AIR_TARGET_PLATFORM == AIR_PLATFORM_WIN32
        __asm 
        {
            fld1	// r0 = 1.f
            fld r	// r1 = r0, r0 = r
            fsqrt	// r0 = sqrtf( r0 )
            fdiv	// r0 = r1 / r0
        } // returns r0
#else
		return 1.0f / sqrtf(r);
#endif
    }

    // returns 1 / a for a * a = r
    // Another version
    inline float apx_rsq( float r ) 
    {
#if AIR_TARGET_PLATFORM == AIR_PLATFORM_WIN32
        const float asm_dot5 = 0.5f;
        const float asm_1dot5 = 1.5f;

        __asm 
        {
            fld r // r0 = r
            fmul asm_dot5 // r0 = r0 * .5f
            mov eax, r // eax = r
            shr eax, 0x1 // eax = eax >> 1
            neg eax // eax = -eax
            add eax, 0x5F400000 // eax = eax & MAGICAL NUMBER
            mov r, eax // r = eax
            fmul r // r0 = r0 * r
            fmul r // r0 = r0 * r
            fsubr asm_1dot5 // r0 = 1.5f - r0
            fmul r // r0 = r0 * r
        } // returns r0
#else
		return 1.0f / sqrtf(r);
#endif
    }


#if AIR_TARGET_PLATFORM == AIR_PLATFORM_WIN32
    /* very MS-specific, commented out for now
    Finally the best InvSqrt implementation?
    Use for vector normalisation instead of 1/length() * x,y,z
    */
    __declspec(naked) inline float __fastcall InvSqrt(float fValue)
    {						  
        __asm
        {
            mov        eax, 0be6eb508h
            mov        DWORD ptr[esp-12],03fc00000h
            sub        eax, DWORD ptr[esp + 4]
            sub        DWORD ptr[esp+4], 800000h
            shr        eax, 1
            mov        DWORD ptr[esp -  8], eax

            fld        DWORD ptr[esp -  8]
            fmul    st, st
            fld        DWORD ptr[esp -  8]
            fxch    st(1)
            fmul    DWORD ptr[esp +  4]
            fld        DWORD ptr[esp - 12]
            fld        st(0)
            fsub    st,st(2)

            fld        st(1)
            fxch    st(1)
            fmul    st(3),st
            fmul    st(3),st
            fmulp    st(4),st
            fsub    st,st(2)

            fmul    st(2),st
            fmul    st(3),st
            fmulp    st(2),st
            fxch    st(1)
            fsubp    st(1),st

            fmulp    st(1), st
            ret 4
        }		

    }
#endif


    // returns a random number
    inline float asm_rand()
    {				 
#if AIR_TARGET_PLATFORM == AIR_PLATFORM_WIN32
		//static unsigned __int64 q = time( NULL );
		static u64 q = time( NULL );
        _asm 
        {
            movq mm0, q

            // do the magic MMX thing
            pshufw mm1, mm0, 0x1E
            paddd mm0, mm1

            // move to integer memory location and free MMX
            movq q, mm0
            emms
        }
        return float( q );
#else
		return float( rand() );
#endif
    }


    // returns the maximum random number
    inline float asm_rand_max()
    {
#if AIR_TARGET_PLATFORM == AIR_PLATFORM_WIN32
        return (std::numeric_limits< u64 >::max)();
        //return 9223372036854775807.0f;	
#else
		return float( RAND_MAX );
#endif
    }


    // returns log2( r ) / log2( e )
    inline float asm_ln( float r ) 
    {    
        const float asm_e = 2.71828182846f;
        const float asm_1_div_log2_e = .693147180559f;
        const float asm_neg1_div_3 = -.33333333333333333333333333333f;
        const float asm_neg2_div_3 = -.66666666666666666666666666667f;
        const float asm_2 = 2.f;

        int log_2 = 0;
#if AIR_TARGET_PLATFORM == AIR_PLATFORM_WIN32
        __asm 
        {
            // log_2 = ( ( r >> 0x17 ) & 0xFF ) - 0x80;
            mov eax, r
            sar eax, 0x17
            and eax, 0xFF
            sub eax, 0x80
            mov log_2, eax

            // r = ( r & 0x807fffff ) + 0x3f800000;
            mov ebx, r
            and ebx, 0x807FFFFF
            add ebx, 0x3F800000
            mov r, ebx

            // r = ( asm_neg1_div_3 * r + asm_2 ) * r + asm_neg2_div_3;   // (1)
            fld r
            fmul asm_neg1_div_3
            fadd asm_2
            fmul r
            fadd asm_neg2_div_3
            fild log_2
            fadd
            fmul asm_1_div_log2_e
        }
#else
		return logf(r);
#endif
    }


    inline float UnitRandom()
    {
        return asm_rand() / asm_rand_max();
    }


    inline float RangeRandom(float fLow, float fHigh)
    {
        return (fHigh-fLow)*UnitRandom() + fLow;
    }


    inline float SymmetricRandom()
    {
        return 2.0f * UnitRandom() - 1.0f;
    }


    /** Clamp a value within an inclusive range. */
    template <typename T>
    T Clamp(T val, T minval, T maxval)
    {
#undef max
#undef min
        //assert (minval < maxval && "Invalid clamp range");
        return std::max(std::min(val, maxval), minval);
    }


    //高斯函数
    inline float GetGaussianDistribution( float cullWidth, float radius, float height = 1.0f ) 
    {
        //if (radius == 0.0f)
        //{
        //    return height;
        //}
        float g = 1.0f / sqrt( 2.0f * PI * cullWidth );
        return height * g * exp( -(radius * radius) / (2 * cullWidth * cullWidth) );
    }

    inline float GetGaussianDistribution( float x, float y, float rho, float height)
    {
        float g = 1.0f / sqrt( 2.0f * 3.141592654f * rho * rho );
        return height * g * exp( -(x * x + y * y) / (2 * rho * rho) );
    }

    //2维高斯函数
    inline float GetGaussianDistribution(float x, float y, float centerX, float centerY, float cullWidth, float height = 1.0f)
    {
        float radius = sqrtf(AIR::Abs(x - centerX) * AIR::Abs(x - centerX) + AIR::Abs(y - centerY) * AIR::Abs(y - centerY));
        return GetGaussianDistribution(cullWidth, radius, height);
    }

    inline float Frac(float fVal)
    {
        return fVal - (int)fVal;
    }


};
//#include "Obb.h"
