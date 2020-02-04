
///********************************************************************
//	created:	2009/12/22
//	created:	22:12:2009   16:10
//	filename: 	d:\My Codes\client\Current\Engine\Engine\Include\ColorValue.h
//	file path:	d:\My Codes\client\Current\Engine\Engine\Include
//	file base:	ColorValue
//	file ext:	h
//	author:		liangairan
//	
//	purpose:	颜色类，带4个小数
//*********************************************************************/
//
//#pragma once 
//#include "AIRTypes.h"
//#include "EngineConfig.h"
//#include <assert.h>
//
//using namespace AIR;
//
//class EngineExport CColorValue
//{
//public:
//    static const CColorValue ZERO;
//    static const CColorValue Black;
//    static const CColorValue White;
//    static const CColorValue Red;
//    static const CColorValue Green;
//    static const CColorValue Blue;
//    static const CColorValue Yellow;
//
//    float r, g, b, a;
//
//    explicit CColorValue(float alpha = 1.0f, float red = 1.0f, float green = 1.0f, float blue = 1.0f) : a(alpha),
//        r(red), g(green), b(blue)
//    {
//
//    }
//
//    /** Clamps colour value to the range [0, 1].
//    */
//    void Saturate(void)
//    {
//        if (r < 0)
//            r = 0;
//        else if (r > 1)
//            r = 1;
//
//        if (g < 0)
//            g = 0;
//        else if (g > 1)
//            g = 1;
//
//        if (b < 0)
//            b = 0;
//        else if (b > 1)
//            b = 1;
//
//        if (a < 0)
//            a = 0;
//        else if (a > 1)
//            a = 1;
//    }
//
//    u32 GetAsARGB() const
//    {
//        u8 val8;
//        u32 val32 = 0;
//
//        // Convert to 32bit pattern
//        // (ARGB = 8888)
//
//        // Alpha
//        val8 = static_cast<u8>(a * 255);
//        val32 = val8 << 24;
//
//        // Red
//        val8 = static_cast<u8>(r * 255);
//        val32 += val8 << 16;
//
//        // Green
//        val8 = static_cast<u8>(g * 255);
//        val32 += val8 << 8;
//
//        // Blue
//        val8 = static_cast<u8>(b * 255);
//        val32 += val8;
//
//
//        return val32;
//    }
//
//    u32 GetAsRGBA() const
//    {
//        u8 val8;
//        u32 val32 = 0;
//
//        // Convert to 32bit pattern
//        // (RGBA = 8888)
//
//        // Red
//        val8 = static_cast<u8>(r * 255);
//        val32 = val8 << 24;
//
//        // Green
//        val8 = static_cast<u8>(g * 255);
//        val32 += val8 << 16;
//
//        // Blue
//        val8 = static_cast<u8>(b * 255);
//        val32 += val8 << 8;
//
//        // Alpha
//        val8 = static_cast<u8>(a * 255);
//        val32 += val8;
//
//        return val32;
//    }
//
//    CColorValue SaturateCopy()
//    {
//        CColorValue ret = *this;;
//        ret.Saturate();
//        return ret;
//    }
//
//    // arithmetic operations
//    inline CColorValue operator + ( const CColorValue& rkVector ) const
//    {
//        CColorValue kSum;
//
//        kSum.r = r + rkVector.r;
//        kSum.g = g + rkVector.g;
//        kSum.b = b + rkVector.b;
//        kSum.a = a + rkVector.a;
//
//        return kSum;
//    }
//
//    inline CColorValue operator - ( const CColorValue& rkVector ) const
//    {
//        CColorValue kDiff;
//
//        kDiff.r = r - rkVector.r;
//        kDiff.g = g - rkVector.g;
//        kDiff.b = b - rkVector.b;
//        kDiff.a = a - rkVector.a;
//
//        return kDiff;
//    }
//
//    inline CColorValue operator * (const float fScalar ) const
//    {
//        CColorValue kProd;
//
//        kProd.r = fScalar*r;
//        kProd.g = fScalar*g;
//        kProd.b = fScalar*b;
//        kProd.a = fScalar*a;
//
//        return kProd;
//    }
//
//    inline CColorValue operator * ( const CColorValue& rhs) const
//    {
//        CColorValue kProd;
//
//        kProd.r = rhs.r * r;
//        kProd.g = rhs.g * g;
//        kProd.b = rhs.b * b;
//        kProd.a = rhs.a * a;
//
//        return kProd;
//    }
//
//    inline CColorValue operator / ( const CColorValue& rhs) const
//    {
//        CColorValue kProd;
//
//        kProd.r = rhs.r / r;
//        kProd.g = rhs.g / g;
//        kProd.b = rhs.b / b;
//        kProd.a = rhs.a / a;
//
//        return kProd;
//    }
//
//    inline CColorValue operator / (const float fScalar ) const
//    {
//        assert( fScalar != 0.0 );
//
//        CColorValue kDiv;
//
//        float fInv = 1.0 / fScalar;
//        kDiv.r = r * fInv;
//        kDiv.g = g * fInv;
//        kDiv.b = b * fInv;
//        kDiv.a = a * fInv;
//
//        return kDiv;
//    }
//
//    inline friend CColorValue operator * (const float fScalar, const CColorValue& rkVector )
//    {
//        CColorValue kProd;
//
//        kProd.r = fScalar * rkVector.r;
//        kProd.g = fScalar * rkVector.g;
//        kProd.b = fScalar * rkVector.b;
//        kProd.a = fScalar * rkVector.a;
//
//        return kProd;
//    }
//
//    // arithmetic updates
//    inline CColorValue& operator += ( const CColorValue& rkVector )
//    {
//        r += rkVector.r;
//        g += rkVector.g;
//        b += rkVector.b;
//        a += rkVector.a;
//
//        return *this;
//    }
//
//    inline CColorValue& operator -= ( const CColorValue& rkVector )
//    {
//        r -= rkVector.r;
//        g -= rkVector.g;
//        b -= rkVector.b;
//        a -= rkVector.a;
//
//        return *this;
//    }
//
//    inline CColorValue& operator *= (const float fScalar )
//    {
//        r *= fScalar;
//        g *= fScalar;
//        b *= fScalar;
//        a *= fScalar;
//        return *this;
//    }
//
//    inline CColorValue& operator /= (const float fScalar )
//    {
//        assert( fScalar != 0.0 );
//
//        float fInv = 1.0 / fScalar;
//
//        r *= fInv;
//        g *= fInv;
//        b *= fInv;
//        a *= fInv;
//
//        return *this;
//    }
//
//    inline bool operator == (const CColorValue& other) const
//    {
//        return a == other.a && 
//            b == other.b &&
//            r == other.r &&
//            g == other.g;
//    }
//
//    inline bool operator != (const CColorValue& other) const
//    {
//        return !(*this == other);
//    }
//
//	inline void Set( float _a, float _r, float _g, float _b )
//	{
//		a = _a;
//		r = _r;
//		g = _g;
//		b = _b;
//	}
//
//    virtual void SetAsRGBA(const u32 val);
//
//    virtual void SetAsARGB(const u32 val);
//
//    virtual void SetAsBGRA(const u32 val);
//
//    virtual void SetAsABGR(const u32 val);
//
//    virtual void SetAsBGR(const u32 val);
//
//    virtual void SetAsRGB(const u32 val);
//protected:
//private:
//};
//
//
//inline CColorValue Lerp( const CColorValue& c1, const CColorValue& c2, float frac )
//{
//	CColorValue dest;
//	dest.a = c1.a * (1.0f - frac) + c2.a * frac;
//	dest.r = c1.r * (1.0f - frac) + c2.r * frac;
//	dest.g = c1.g * (1.0f - frac) + c2.g * frac;
//	dest.b = c1.b * (1.0f - frac) + c2.b * frac;
//	return dest;
//}



//
///********************************************************************
//	created:	2009/12/22
//	created:	22:12:2009   16:10
//	filename: 	d:\My Codes\client\Current\Engine\Engine\Include\ColorValue.h
//	file path:	d:\My Codes\client\Current\Engine\Engine\Include
//	file base:	ColorValue
//	file ext:	h
//	author:		liangairan
//	
//	purpose:	颜色类，带4个小数
//*********************************************************************/
//
//#pragma once 
//#include "AIRTypes.h"
//#include "EngineConfig.h"
//#include <assert.h>
//
//using namespace AIR;
//
//class EngineExport CColorValue
//{
//public:
//    static const CColorValue ZERO;
//    static const CColorValue Black;
//    static const CColorValue White;
//    static const CColorValue Red;
//    static const CColorValue Green;
//    static const CColorValue Blue;
//    static const CColorValue Yellow;
//
//    float r, g, b, a;
//
//    explicit CColorValue(float alpha = 1.0f, float red = 1.0f, float green = 1.0f, float blue = 1.0f) : a(alpha),
//        r(red), g(green), b(blue)
//    {
//
//    }
//
//    /** Clamps colour value to the range [0, 1].
//    */
//    void Saturate(void)
//    {
//        if (r < 0)
//            r = 0;
//        else if (r > 1)
//            r = 1;
//
//        if (g < 0)
//            g = 0;
//        else if (g > 1)
//            g = 1;
//
//        if (b < 0)
//            b = 0;
//        else if (b > 1)
//            b = 1;
//
//        if (a < 0)
//            a = 0;
//        else if (a > 1)
//            a = 1;
//    }
//
//    u32 GetAsARGB() const
//    {
//        u8 val8;
//        u32 val32 = 0;
//
//        // Convert to 32bit pattern
//        // (ARGB = 8888)
//
//        // Alpha
//        val8 = static_cast<u8>(a * 255);
//        val32 = val8 << 24;
//
//        // Red
//        val8 = static_cast<u8>(r * 255);
//        val32 += val8 << 16;
//
//        // Green
//        val8 = static_cast<u8>(g * 255);
//        val32 += val8 << 8;
//
//        // Blue
//        val8 = static_cast<u8>(b * 255);
//        val32 += val8;
//
//
//        return val32;
//    }
//
//    u32 GetAsRGBA() const
//    {
//        u8 val8;
//        u32 val32 = 0;
//
//        // Convert to 32bit pattern
//        // (RGBA = 8888)
//
//        // Red
//        val8 = static_cast<u8>(r * 255);
//        val32 = val8 << 24;
//
//        // Green
//        val8 = static_cast<u8>(g * 255);
//        val32 += val8 << 16;
//
//        // Blue
//        val8 = static_cast<u8>(b * 255);
//        val32 += val8 << 8;
//
//        // Alpha
//        val8 = static_cast<u8>(a * 255);
//        val32 += val8;
//
//        return val32;
//    }
//
//    CColorValue SaturateCopy()
//    {
//        CColorValue ret = *this;;
//        ret.Saturate();
//        return ret;
//    }
//
//    // arithmetic operations
//    inline CColorValue operator + ( const CColorValue& rkVector ) const
//    {
//        CColorValue kSum;
//
//        kSum.r = r + rkVector.r;
//        kSum.g = g + rkVector.g;
//        kSum.b = b + rkVector.b;
//        kSum.a = a + rkVector.a;
//
//        return kSum;
//    }
//
//    inline CColorValue operator - ( const CColorValue& rkVector ) const
//    {
//        CColorValue kDiff;
//
//        kDiff.r = r - rkVector.r;
//        kDiff.g = g - rkVector.g;
//        kDiff.b = b - rkVector.b;
//        kDiff.a = a - rkVector.a;
//
//        return kDiff;
//    }
//
//    inline CColorValue operator * (const float fScalar ) const
//    {
//        CColorValue kProd;
//
//        kProd.r = fScalar*r;
//        kProd.g = fScalar*g;
//        kProd.b = fScalar*b;
//        kProd.a = fScalar*a;
//
//        return kProd;
//    }
//
//    inline CColorValue operator * ( const CColorValue& rhs) const
//    {
//        CColorValue kProd;
//
//        kProd.r = rhs.r * r;
//        kProd.g = rhs.g * g;
//        kProd.b = rhs.b * b;
//        kProd.a = rhs.a * a;
//
//        return kProd;
//    }
//
//    inline CColorValue operator / ( const CColorValue& rhs) const
//    {
//        CColorValue kProd;
//
//        kProd.r = rhs.r / r;
//        kProd.g = rhs.g / g;
//        kProd.b = rhs.b / b;
//        kProd.a = rhs.a / a;
//
//        return kProd;
//    }
//
//    inline CColorValue operator / (const float fScalar ) const
//    {
//        assert( fScalar != 0.0 );
//
//        CColorValue kDiv;
//
//        float fInv = 1.0 / fScalar;
//        kDiv.r = r * fInv;
//        kDiv.g = g * fInv;
//        kDiv.b = b * fInv;
//        kDiv.a = a * fInv;
//
//        return kDiv;
//    }
//
//    inline friend CColorValue operator * (const float fScalar, const CColorValue& rkVector )
//    {
//        CColorValue kProd;
//
//        kProd.r = fScalar * rkVector.r;
//        kProd.g = fScalar * rkVector.g;
//        kProd.b = fScalar * rkVector.b;
//        kProd.a = fScalar * rkVector.a;
//
//        return kProd;
//    }
//
//    // arithmetic updates
//    inline CColorValue& operator += ( const CColorValue& rkVector )
//    {
//        r += rkVector.r;
//        g += rkVector.g;
//        b += rkVector.b;
//        a += rkVector.a;
//
//        return *this;
//    }
//
//    inline CColorValue& operator -= ( const CColorValue& rkVector )
//    {
//        r -= rkVector.r;
//        g -= rkVector.g;
//        b -= rkVector.b;
//        a -= rkVector.a;
//
//        return *this;
//    }
//
//    inline CColorValue& operator *= (const float fScalar )
//    {
//        r *= fScalar;
//        g *= fScalar;
//        b *= fScalar;
//        a *= fScalar;
//        return *this;
//    }
//
//    inline CColorValue& operator /= (const float fScalar )
//    {
//        assert( fScalar != 0.0 );
//
//        float fInv = 1.0 / fScalar;
//
//        r *= fInv;
//        g *= fInv;
//        b *= fInv;
//        a *= fInv;
//
//        return *this;
//    }
//
//    inline bool operator == (const CColorValue& other) const
//    {
//        return a == other.a && 
//            b == other.b &&
//            r == other.r &&
//            g == other.g;
//    }
//
//    inline bool operator != (const CColorValue& other) const
//    {
//        return !(*this == other);
//    }
//
//	inline void Set( float _a, float _r, float _g, float _b )
//	{
//		a = _a;
//		r = _r;
//		g = _g;
//		b = _b;
//	}
//
//    void SetAsRGBA(const u32 val);
//
//    void SetAsARGB(const u32 val);
//
//    void SetAsBGRA(const u32 val);
//
//    void SetAsABGR(const u32 val);
//
//    void SetAsBGR(const u32 val);
//
//    void SetAsRGB(const u32 val);
//protected:
//private:
//};
//
//
//inline CColorValue Lerp( const CColorValue& c1, const CColorValue& c2, float frac )
//{
//	CColorValue dest;
//	dest.a = c1.a * (1.0f - frac) + c2.a * frac;
//	dest.r = c1.r * (1.0f - frac) + c2.r * frac;
//	dest.g = c1.g * (1.0f - frac) + c2.g * frac;
//	dest.b = c1.b * (1.0f - frac) + c2.b * frac;
//	return dest;
//}

