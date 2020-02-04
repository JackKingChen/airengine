#pragma once

#include "AIRTypes.h"
#include "MyMath.h"
#include "EngineConfig.h"
#include "Common.h"
#include <assert.h>

//#if AIR_TARGET_PLATFORM == AIR_PLATFORM_WIN32
//#define snprintf _snprintf_s
//#endif

using namespace AIR;

//! Creates a 16 bit A1R5G5B5 color
inline u16 RGBA16(u32 r, u32 g, u32 b, u32 a = 0xFF)
{
    return (u16)((a & 0x80) << 8 |
        (r & 0xF8) << 7 |
        (g & 0xF8) << 2 |
        (b & 0xF8) >> 3);
}


//! Creates a 16 bit A1R5G5B5 color
inline u16 RGB16(u32 r, u32 g, u32 b)
{
    return RGBA16(r,g,b);
}


//! Creates a 16bit A1R5G5B5 color, based on 16bit input values
inline u16 RGB16from16(u16 r, u16 g, u16 b)
{
    return (0x8000 |
        (r & 0x1F) << 10 |
        (g & 0x1F) << 5  |
        (b & 0x1F));
}


//! Converts a 32bit (X8R8G8B8) color to a 16bit A1R5G5B5 color
inline u16 X8R8G8B8toA1R5G5B5(u32 color)
{
    return (u16)(0x8000 |
        ( color & 0x00F80000) >> 9 |
        ( color & 0x0000F800) >> 6 |
        ( color & 0x000000F8) >> 3);
}


//! Converts a 32bit (A8R8G8B8) color to a 16bit A1R5G5B5 color
inline u16 A8R8G8B8toA1R5G5B5(u32 color)
{
    return (u16)(( color & 0x80000000) >> 16|
        ( color & 0x00F80000) >> 9 |
        ( color & 0x0000F800) >> 6 |
        ( color & 0x000000F8) >> 3);
}


//! Converts a 32bit (A8R8G8B8) color to a 16bit R5G6B5 color
inline u16 A8R8G8B8toR5G6B5(u32 color)
{
    return (u16)(( color & 0x00F80000) >> 8 |
        ( color & 0x0000FC00) >> 5 |
        ( color & 0x000000F8) >> 3);
}


//! Convert A8R8G8B8 Color from A1R5G5B5 color
/** build a nicer 32bit Color by extending dest lower bits with source high bits. */
inline u32 A1R5G5B5toA8R8G8B8(u16 color)
{
    return ( (( -( (s32) color & 0x00008000 ) >> (s32) 31 ) & 0xFF000000 ) |
        (( color & 0x00007C00 ) << 9) | (( color & 0x00007000 ) << 4) |
        (( color & 0x000003E0 ) << 6) | (( color & 0x00000380 ) << 1) |
        (( color & 0x0000001F ) << 3) | (( color & 0x0000001C ) >> 2)
        );
}


//! Returns A8R8G8B8 Color from R5G6B5 color
inline u32 R5G6B5toA8R8G8B8(u16 color)
{
    return 0xFF000000 |
        ((color & 0xF800) << 8)|
        ((color & 0x07E0) << 5)|
        ((color & 0x001F) << 3);
}


//! Returns A1R5G5B5 Color from R5G6B5 color
inline u16 R5G6B5toA1R5G5B5(u16 color)
{
    return 0x8000 | (((color & 0xFFC0) >> 1) | (color & 0x1F));
}


//! Returns R5G6B5 Color from A1R5G5B5 color
inline u16 A1R5G5B5toR5G6B5(u16 color)
{
    return (((color & 0x7FE0) << 1) | (color & 0x1F));
}



//! Returns the alpha component from A1R5G5B5 color
/** In Irrlicht, alpha refers to opacity. 
\return The alpha value of the color. 0 is transparent, 1 is opaque. */
inline u32 getAlpha(u16 color)
{
    return ((color >> 15)&0x1);
}


//! Returns the red component from A1R5G5B5 color.
/** Shift left by 3 to get 8 bit value. */
inline u32 getRed(u16 color)
{
    return ((color >> 10)&0x1F);
}


//! Returns the green component from A1R5G5B5 color
/** Shift left by 3 to get 8 bit value. */
inline u32 getGreen(u16 color)
{
    return ((color >> 5)&0x1F);
}


//! Returns the blue component from A1R5G5B5 color
/** Shift left by 3 to get 8 bit value. */
inline u32 getBlue(u16 color)
{
    return (color & 0x1F);
}


//! Returns the average from a 16 bit A1R5G5B5 color
inline s32 getAverage(s16 color)
{
    return ((getRed(color)<<3) + (getGreen(color)<<3) + (getBlue(color)<<3)) / 3;
}

/*

template < class T >
class CColor
{
public:
    CColor() : R(0), G(0), B(0), A(0) { }
    CColor( T r, T g, T b, T a) : R(r), G(g), B(b), A(a) { }
    CColor( const CColor<T> &other ) : R(other.R), G(other.G), B(other.B), A(other.A) {  }
    CColor( const T* pColor ) {  }
    ~CColor() {}

    CColor<T>& operator = ( T *pColor )
    {
        this->R  = pColor[0];
        this->G  = pColor[1];
        this->B  = pColor[2];
        this->A  = pColor[3];
        return *this;
    }

    CColor<T>& operator = ( const CColor<T>& other )
    {
        this->R  = other.R;
        this->G  = other.G;
        this->B  = other.B;
        this->A  = other.A;
        return *this;
    }

    void Set( T r, T g, T b, T a )
    {
        this->A = a;
        this->B = b;
        this->G = g;
        this->R = r;
    }

    void Set( CColor<T> other )
    {
        this->A = other.A;
        this->B = other.B;
        this->G = other.G;
        this->R = other.R;
    }

    friend CColor<u8> SetColor( const u32 &color ) 
    {
        CColor<u8> c;
        c.R = ( color & 0xff000000 ) >> 24;
        c.G = ( color & 0x00ff0000 ) >> 16;
        c.B = ( color & 0x0000ff00 ) >> 8;
        c.A = ( color & 0x000000ff );
        return c;
    }

    friend CColor<u8> ColorFtoN( const CColor<float> &cf )
    {
        CColor<u8> cn;
        cn.R = cf.R * 255.0;
        cn.G = cf.G * 255.0;
        cn.B = cf.B * 255.0;
        cn.A = cf.A * 255.0;
        return cn;
    };

public:
    T R, G, B, A;
};

typedef CColor<u8>  Color4i;
typedef CColor<float> Color4f;

inline u32 Color4fToSColor(const Color4f& color)
{
    return (u32(color.A * 255.0f) << 24) + (u32(color.R * 255.0f) << 16) + (u32(color.G * 255.0f) << 8) + u32(color.B * 255.0f);
}

*/

class EngineExport SColor
{
public:
	SColor():color(0xffffff) {}
    SColor(u8 a, u8 r, u8 g, u8 b) : color(((a & 0xff)<<24) | ((r & 0xff)<<16) | ((g & 0xff)<<8) | (b & 0xff))
    {

    }
	SColor(const SColor& other) : color(other.color)
	{

	}

    //! Constructs the color from a 32 bit value. Could be another color.
    SColor(u32 clr) : color(clr) {}

    //! Returns the alpha component of the color.
    /** The alpha component defines how opaque a color is. 
    \return The alpha value of the color. 0 is fully transparent, 255 is fully opaque. */
    inline u32 GetAlpha() const { return color>>24; }

    //! Returns the red component of the color.
    /** \return Value between 0 and 255, specifying how red the color is.
    0 means no red, 255 means full red. */
    inline u32 GetRed() const { return (color>>16) & 0xff; }

    //! Returns the green component of the color.
    /** \return Value between 0 and 255, specifying how green the color is.
    0 means no green, 255 means full green. */
    inline u32 GetGreen() const { return (color>>8) & 0xff; }

    //! Returns the blue component of the color.
    /** \return Value between 0 and 255, specifying how blue the color is.
    0 means no blue, 255 means full blue. */
    inline u32 GetBlue() const { return color & 0xff; }

    //! Returns the luminance of the color.
    inline f32 GetLuminance() const
    {
        return 0.3f * GetRed() + 0.59f * GetGreen() + 0.11f * GetBlue();
    }

    //! Returns the average intensity of the color.
    inline u32 GetAverage() const
    {
        return ( GetRed() + GetGreen() + GetBlue() ) / 3;
    }

    //! Sets the alpha component of the Color.
    /** The alpha component defines how transparent a color should be.
    \param a The alpha value of the color. 0 is fully transparent, 255 is fully opaque. */
    inline void SetAlpha(u32 a) { color = ((a & 0xff)<<24) | (color & 0x00ffffff); }

    //! Sets the red component of the Color.
    /** \param r: Has to be a value between 0 and 255.
    0 means no red, 255 means full red. */
    inline void SetRed(u32 r) { color = ((r & 0xff)<<16) | (color & 0xff00ffff); }

    //! Sets the green component of the Color.
    /** \param g: Has to be a value between 0 and 255.
    0 means no green, 255 means full green. */
    inline void SetGreen(u32 g) { color = ((g & 0xff)<<8) | (color & 0xffff00ff); }

    //! Sets the blue component of the Color.
    /** \param b: Has to be a value between 0 and 255.
    0 means no blue, 255 means full blue. */
    inline void SetBlue(u32 b) { color = (b & 0xff) | (color & 0xffffff00); }

    //! Calculates a 16 bit A1R5G5B5 value of this color.
    /** \return 16 bit A1R5G5B5 value of this color. */
    inline u16 ToA1R5G5B5() const { return A8R8G8B8toA1R5G5B5(color); }

    //! Converts color to OpenGL color format
    /** From ARGB to RGBA in 4 byte components for endian aware
    passing to OpenGL
    \param dest: address where the 4x8 bit OpenGL color is stored. */
    inline void ToOpenGLColor(u8* dest) const
    {
        *dest =   (u8)GetRed();
        *++dest = (u8)GetGreen();
        *++dest = (u8)GetBlue();
        *++dest = (u8)GetAlpha();
    }

    //! Sets all four components of the color at once.
    /** Constructs the color from 4 values representing the alpha,
    red, green and blue components of the color. Must be values
    between 0 and 255.
    \param a: Alpha component of the color. The alpha component
    defines how transparent a color should be. Has to be a value
    between 0 and 255. 255 means not transparent (opaque), 0 means
    fully transparent.
    \param r: Sets the red component of the Color. Has to be a
    value between 0 and 255. 0 means no red, 255 means full red.
    \param g: Sets the green component of the Color. Has to be a
    value between 0 and 255. 0 means no green, 255 means full
    green.
    \param b: Sets the blue component of the Color. Has to be a
    value between 0 and 255. 0 means no blue, 255 means full blue. */
    inline void Set(u32 a, u32 r, u32 g, u32 b)
    {
        color = (((a & 0xff)<<24) | ((r & 0xff)<<16) | ((g & 0xff)<<8) | (b & 0xff));
    }

    inline void Set(u32 col) { color = col; }

    //! Compares the color to another color.
    /** \return True if the colors are the same, and false if not. */
    inline bool operator == (const SColor& other) const { return other.color == color; }

    //! Compares the color to another color.
    /** \return True if the colors are different, and false if they are the same. */
    inline bool operator != (const SColor& other) const { return other.color != color; }

    //! comparison operator
    /** \return True if this color is smaller than the other one */
    inline bool operator < (const SColor& other) const { return (color < other.color); }

    inline SColor operator + (const SColor& other) const
    {
        int a = AIR::Clamp(((int)GetAlpha() + (int)other.GetAlpha()), 0, 255);
        int r = AIR::Clamp(((int)GetRed() + (int)other.GetRed()), 0, 255);
        int g = AIR::Clamp(((int)GetGreen() + (int)other.GetGreen()), 0, 255);
        int b = AIR::Clamp(((int)GetBlue() + (int)other.GetBlue()), 0, 255);
        return SColor(a, r, g, b);
    }

    inline SColor operator - (const SColor& other) const
    {
        int a = AIR::Clamp(((int)GetAlpha() - (int)other.GetAlpha()), 0, 255);
        int r = AIR::Clamp(((int)GetRed() - (int)other.GetRed()), 0, 255);
        int g = AIR::Clamp(((int)GetGreen() - (int)other.GetGreen()), 0, 255);
        int b = AIR::Clamp(((int)GetBlue() - (int)other.GetBlue()), 0, 255);
        return SColor(a, r, g, b);
    }

    inline SColor& operator += (const SColor& other)
    {
        int a = AIR::Clamp(((int)GetAlpha() + (int)other.GetAlpha()), 0, 255);
        int r = AIR::Clamp(((int)GetRed() + (int)other.GetRed()), 0, 255);
        int g = AIR::Clamp(((int)GetGreen() + (int)other.GetGreen()), 0, 255);
        int b = AIR::Clamp(((int)GetBlue() + (int)other.GetBlue()), 0, 255);
        Set(a, r, g, b);
        return *this;
    }

    inline SColor& operator -= (const SColor& other)
    {
        int a = AIR::Clamp(((int)GetAlpha() - (int)other.GetAlpha()), 0, 255);
        int r = AIR::Clamp(((int)GetRed() - (int)other.GetRed()), 0, 255);
        int g = AIR::Clamp(((int)GetGreen() - (int)other.GetGreen()), 0, 255);
        int b = AIR::Clamp(((int)GetBlue() - (int)other.GetBlue()), 0, 255);
        Set(a, r, g, b);
        return *this;
    }

    inline SColor operator * (float fFactor) const
    {
        u8 a = (u8)(GetAlpha() * fFactor);
        u8 r = (u8)(GetRed() * fFactor);
        u8 g = (u8)(GetGreen() * fFactor);
        u8 b = (u8)(GetBlue() * fFactor);
        return SColor(a, r, g, b);
    }

    inline SColor operator * (const SColor& other) const
    {
        u8 a = (u8)(GetAlpha() * other.GetAlpha() / 255);
        u8 r = (u8)(GetRed() * other.GetRed() / 255);
        u8 g = (u8)(GetGreen() * other.GetGreen() / 255);
        u8 b = (u8)(GetBlue() * other.GetBlue() / 255);
        return SColor(a, r, g, b);
    }

    inline SColor& operator *= (float fFactor)
    {
        u8 a = (u8)(GetAlpha() * fFactor);
        u8 r = (u8)(GetRed() * fFactor);
        u8 g = (u8)(GetGreen() * fFactor);
        u8 b = (u8)(GetBlue() * fFactor);
        Set(a, r, g, b);
        return *this;
    }

    inline SColor& operator *= (const SColor& other)
    {
        u8 a = (u8)(GetAlpha() * other.GetAlpha() / 255);
        u8 r = (u8)(GetRed() * other.GetRed() / 255);
        u8 g = (u8)(GetGreen() * other.GetGreen() / 255);
        u8 b = (u8)(GetBlue() * other.GetBlue() / 255);
        Set(a, r, g, b);
        return *this;
    }

    inline void ChangeDelta( float a, float r, float g, float b )
    {
        int _a = AIR::Clamp(((int)GetAlpha() + (int)(a * 255.0f)), 0, 255);
        int _r = AIR::Clamp(((int)GetRed()   + (int)(r * 255.0f)), 0, 255);
        int _g = AIR::Clamp(((int)GetGreen() + (int)(g * 255.0f)), 0, 255);
        int _b = AIR::Clamp(((int)GetBlue()  + (int)(b * 255.0f)), 0, 255);
        Set(_a, _r, _g, _b);
    }

	inline AIRString GetAsHexString( void )
	{
		AIRString res("");
		char buf[100];
		snprintf( buf, 100, "%x", GetAlpha() );
		res += buf;
		snprintf( buf, 100, "%x", GetRed() );
		res += buf;
		snprintf( buf, 100, "%x", GetGreen() );
		res += buf;
		snprintf( buf, 100, "%x", GetBlue() );
		res += buf;
		return res;
	}

    u32 color;

	static const SColor White;
	static const SColor Black;
	static const SColor Zero;
	static const SColor Gray;
	static const SColor Red;
	static const SColor Green;
	static const SColor Blue;
	static const SColor Yellow;
	static const SColor Cyan;
	static const SColor Magenta;
};




class EngineExport CColorValue
{
public:
	static const CColorValue White;
	static const CColorValue Black;
	static const CColorValue Zero;
	static const CColorValue Gray;
	static const CColorValue Red;
	static const CColorValue Green;
	static const CColorValue Blue;
	static const CColorValue Yellow;
	static const CColorValue Cyan;
	static const CColorValue Magenta;

    float r, g, b, a;

    explicit CColorValue(float alpha = 1.0f, float red = 1.0f, float green = 1.0f, float blue = 1.0f) : a(alpha),
        r(red), g(green), b(blue)
    {

    }

	CColorValue(const CColorValue& other) : r(other.r), g(other.g), b(other.b), a(other.a)
	{
	}

	CColorValue(const SColor& col)
	{
		this->SetAsARGB(col.color);
	}

    /** Clamps colour value to the range [0, 1].
    */
    void Saturate(void)
    {
        if (r < 0)
            r = 0;
        else if (r > 1)
            r = 1;

        if (g < 0)
            g = 0;
        else if (g > 1)
            g = 1;

        if (b < 0)
            b = 0;
        else if (b > 1)
            b = 1;

        if (a < 0)
            a = 0;
        else if (a > 1)
            a = 1;
    }

    inline u32 GetAsARGB() const
    {
        u8 val8;
        u32 val32 = 0;

        // Convert to 32bit pattern
        // (ARGB = 8888)

        // Alpha
        val8 = static_cast<u8>(a * 255);
        val32 = val8 << 24;

        // Red
        val8 = static_cast<u8>(r * 255);
        val32 += val8 << 16;

        // Green
        val8 = static_cast<u8>(g * 255);
        val32 += val8 << 8;

        // Blue
        val8 = static_cast<u8>(b * 255);
        val32 += val8;


        return val32;
    }

    inline u32 GetAsRGBA() const
    {
        u8 val8;
        u32 val32 = 0;

        // Convert to 32bit pattern
        // (RGBA = 8888)

        // Red
        val8 = static_cast<u8>(r * 255);
        val32 = val8 << 24;

        // Green
        val8 = static_cast<u8>(g * 255);
        val32 += val8 << 16;

        // Blue
        val8 = static_cast<u8>(b * 255);
        val32 += val8 << 8;

        // Alpha
        val8 = static_cast<u8>(a * 255);
        val32 += val8;

        return val32;
    }

    inline CColorValue SaturateCopy()
    {
        CColorValue ret = *this;;
        ret.Saturate();
        return ret;
    }

    // arithmetic operations
    inline  CColorValue operator + ( const CColorValue& rkVector ) const
    {
        CColorValue kSum;

        kSum.r = r + rkVector.r;
        kSum.g = g + rkVector.g;
        kSum.b = b + rkVector.b;
        kSum.a = a + rkVector.a;

        return kSum;
    }

    inline  CColorValue operator - ( const CColorValue& rkVector ) const
    {
        CColorValue kDiff;

        kDiff.r = r - rkVector.r;
        kDiff.g = g - rkVector.g;
        kDiff.b = b - rkVector.b;
        kDiff.a = a - rkVector.a;

        return kDiff;
    }

    inline  CColorValue operator * (const float fScalar ) const
    {
        CColorValue kProd;

        kProd.r = fScalar*r;
        kProd.g = fScalar*g;
        kProd.b = fScalar*b;
        kProd.a = fScalar*a;

        return kProd;
    }

    inline  CColorValue operator * ( const CColorValue& rhs) const
    {
        CColorValue kProd;

        kProd.r = rhs.r * r;
        kProd.g = rhs.g * g;
        kProd.b = rhs.b * b;
        kProd.a = rhs.a * a;

        return kProd;
    }

    inline  CColorValue operator / ( const CColorValue& rhs) const
    {
        CColorValue kProd;

        kProd.r = rhs.r / r;
        kProd.g = rhs.g / g;
        kProd.b = rhs.b / b;
        kProd.a = rhs.a / a;

        return kProd;
    }

    inline  CColorValue operator / (const float fScalar ) const
    {
        assert( fScalar != 0.0 );

        CColorValue kDiv;

        float fInv = 1.0 / fScalar;
        kDiv.r = r * fInv;
        kDiv.g = g * fInv;
        kDiv.b = b * fInv;
        kDiv.a = a * fInv;

        return kDiv;
    }

    inline  friend CColorValue operator * (const float fScalar, const CColorValue& rkVector )
    {
        CColorValue kProd;

        kProd.r = fScalar * rkVector.r;
        kProd.g = fScalar * rkVector.g;
        kProd.b = fScalar * rkVector.b;
        kProd.a = fScalar * rkVector.a;

        return kProd;
    }

    // arithmetic updates
    inline  CColorValue& operator += ( const CColorValue& rkVector )
    {
        r += rkVector.r;
        g += rkVector.g;
        b += rkVector.b;
        a += rkVector.a;

        return *this;
    }

    inline  CColorValue& operator -= ( const CColorValue& rkVector )
    {
        r -= rkVector.r;
        g -= rkVector.g;
        b -= rkVector.b;
        a -= rkVector.a;

        return *this;
    }

    inline  CColorValue& operator *= (const float fScalar )
    {
        r *= fScalar;
        g *= fScalar;
        b *= fScalar;
        a *= fScalar;
        return *this;
    }

    inline  CColorValue& operator /= (const float fScalar )
    {
        assert( fScalar != 0.0 );

        float fInv = 1.0 / fScalar;

        r *= fInv;
        g *= fInv;
        b *= fInv;
        a *= fInv;

        return *this;
    }

	inline CColorValue& operator = (const SColor& Color)
	{
		this->SetAsARGB(Color.color);
		return *this;
	}

    inline  bool operator == (const CColorValue& other) const
    {
        return a == other.a && 
            b == other.b &&
            r == other.r &&
            g == other.g;
    }

    inline  bool operator != (const CColorValue& other) const
    {
        return !(*this == other);
    }

	inline  void Set( float _a, float _r, float _g, float _b )
	{
		a = _a;
		r = _r;
		g = _g;
		b = _b;
	}


	inline void SetAsARGB(const u32 val)
	{
		u32 val32 = val;
		// Convert from 32bit pattern
		// (ARGB = 8888)
		// Alpha
		a = ((val32 >> 24) & 0xFF) / 255.0f;
		// Red
		r = ((val32 >> 16) & 0xFF) / 255.0f;
		// Green
		g = ((val32 >> 8) & 0xFF) / 255.0f;
		// Blue
		b = (val32 & 0xFF) / 255.0f;
	}

	inline void SetAsRGBA(const u32 val)
	{
		u32 val32 = val;
		// Convert from 32bit pattern
		// (RGBA = 8888)
		// Red
		r = ((val32 >> 24) & 0xFF) / 255.0f;
		// Green
		g = ((val32 >> 16) & 0xFF) / 255.0f;
		// Blue
		b = ((val32 >> 8) & 0xFF) / 255.0f;
		// Alpha
		a = (val32 & 0xFF) / 255.0f;
	}

	inline void SetAsBGRA(const u32 val)
	{
		u32 val32 = val;
		// Convert from 32bit pattern
		// (ARGB = 8888)
		// Blue
		b = ((val32 >> 24) & 0xFF) / 255.0f;
		// Green
		g = ((val32 >> 16) & 0xFF) / 255.0f;
		// Red
		r = ((val32 >> 8) & 0xFF) / 255.0f;
		// Alpha
		a = (val32 & 0xFF) / 255.0f;
	}

	inline void SetAsBGR(const u32 val)
	{
		u32 val32 = val;
		// Blue
		a = 1.0f;
		// Green
		b = ((val32 >> 16) & 0xFF) / 255.0f;
		// Red
		g = ((val32 >> 8) & 0xFF) / 255.0f;
		// Alpha
		r = (val32 & 0xFF) / 255.0f;
	}

	inline void SetAsABGR(const u32 val)
	{
		u32 val32 = val;
		// Convert from 32bit pattern
		// (ABGR = 8888)
		// Alpha
		a = ((val32 >> 24) & 0xFF) / 255.0f;
		// Blue
		b = ((val32 >> 16) & 0xFF) / 255.0f;
		// Green
		g = ((val32 >> 8) & 0xFF) / 255.0f;
		// Red
		r = (val32 & 0xFF) / 255.0f;
	}

	inline void SetAsRGB(const u32 val)
	{
		u32 val32 = val;
		// Convert from 32bit pattern
		// (ARGB = 8888)
		// Alpha
		//a = ((val32 >> 24) & 0xFF) / 255.0f;
		a = 1.0f;
		// Red
		r = ((val32 >> 16) & 0xFF) / 255.0f;
		// Green
		g = ((val32 >> 8) & 0xFF) / 255.0f;
		// Blue
		b = (val32 & 0xFF) / 255.0f;
	}

};


inline CColorValue Lerp( const CColorValue& c1, const CColorValue& c2, float frac )
{
	CColorValue dest;
	dest.a = c1.a * (1.0f - frac) + c2.a * frac;
	dest.r = c1.r * (1.0f - frac) + c2.r * frac;
	dest.g = c1.g * (1.0f - frac) + c2.g * frac;
	dest.b = c1.b * (1.0f - frac) + c2.b * frac;
	return dest;
}



