
//****************************************************************************//
//*	作者	:  JACK                                                          *//
//*	日期    :   2008-9-17							                         *//
//* 通信    :  ( 280877280 / topjackhjj@126.com )                            *//
//* 标题    :  Vector4.h                                                 *//
//* 功能    :  4d vector template class                                      *//
//* 说明    :                                                                *//
//****************************************************************************//

#ifndef __AURO_AuroVector4_INCLUDE__
#define __AURO_AuroVector4_INCLUDE__

#pragma once
//****************************************************************************//
//* 添加头文件
//****************************************************************************//
//#include "AuroConfig.h"
//#include "MathBase.h"
#include "Vector3D.h"

//****************************************************************************//
//* 添加库连接
//****************************************************************************//

namespace AIR
{


	//********************************************************************//
	//* 添加全局参数, 静态参数, 全局方法...
	//********************************************************************//


	//********************************************************************//
	//* 类名称  :	 Vector4 
	//* 功能    :	 4d vector template class 
	//* 用法    :	  
	//********************************************************************//
	class EngineExport Vector4
	{
	public:
		inline Vector4(f32 X = 0, f32 Y = 0, f32 Z = 0, f32 W = 0)
		{
			x = X;
			y = Y;
			z = Z;
			w = W;
		}

		inline explicit Vector4(f32 XYZW)
		{
			x = y = z = w = XYZW;
		}

		inline Vector4(const Vector4 &source)
		{
			x = source.x;
			y = source.y;
			z = source.z;
			w = source.w;
		}

		inline Vector4(const Vector3D &source, const f32 W = 1)
		{
			x = source.x;
			y = source.y;
			z = source.z;
			w = W;
		}

		inline Vector4( f32 *tArray )
		{
			x = *tArray++;
			y = *tArray++;
			z = *tArray++;
			w = *tArray;
		}

		inline operator const f32*() const { return &x; }
		inline operator f32*() { return &x; }   

		inline const f32  operator[](int i) const { return ((f32*)&x)[i]; }
		inline       f32 &operator[](int i)       { return ((f32*)&x)[i]; }

		inline Vector4 &operator = (const Vector4 &source)
		{
			x = source.x;
			y = source.y;
			z = source.z;
			w = source.w;
			return *this;
		}

		inline Vector4 &operator = ( f32 *tArray )
		{
			x = *tArray++;
			y = *tArray++;
			z = *tArray++;
			w = *tArray;
			return *this;
		}

		inline Vector4 &operator = (const Vector3D &source)
		{
			x = source.x;
			y = source.y;
			z = source.z;
			w = 1.0f;
			return *this;
		}

		inline Vector4 operator + (const Vector4 &right)
		{
			return Vector4(right.x + x, right.y + y, right.z + z, right.w + w );
		}

		inline Vector4 operator + ( f32 *tArray )
		{
			return Vector4( x + *tArray++, y + *tArray++, z + *tArray++, w + *tArray );
		}

		inline Vector4 operator - (const Vector4  &right)
		{
			return Vector4(-right.x + x, -right.y + y, -right.z + z, -right.w + w );
		}

		inline Vector4 operator - ( f32 *tArray )
		{
			return Vector4( x - *tArray++, y - *tArray++, z - *tArray++, w - *tArray );
		}

		inline Vector4 operator * (const f32 scale)
		{
			return Vector4(x*scale, y*scale, z*scale, w*scale);
		}

		inline Vector4 operator / (const f32 scale)
		{
			return ( scale ? Vector4(x/scale, y/scale, z/scale, w/scale) : Vector4(0, 0, 0, 0) );
		}

		inline Vector4 &operator += (const Vector4 &right)
		{
			x +=right.x;
			y +=right.y;
			z +=right.z;
			w +=right.w;
			return *this;
		}

		inline Vector4 &operator += ( f32 *tArray )
		{
			x += *tArray++;
			y += *tArray++;
			z += *tArray++;
			w += *tArray;
			return *this;
		}

		inline Vector4 operator -= (const Vector4 &right)
		{
			x-=right.x;
			y-=right.y;
			z-=right.z;
			w-=right.w;
			return *this;
		}

		inline Vector4 operator -= ( f32 *tArray )
		{
			x-=*tArray++;
			y-=*tArray++;
			z-=*tArray++;
			w-=*tArray;
			return *this;
		}

		inline Vector4 Clamp(const f32 min, const f32 max)
		{
			x = x < min ? min : x > max ? max : x;
			y = y < min ? min : y > max ? max : y;
			z = z < min ? min : z > max ? max : z;
			w = w < min ? min : w > max ? max : w;
			return *this;
		}

		inline Vector4 operator *= (const f32 scale)
		{
			x*=scale;
			y*=scale;
			z*=scale;
			w*=scale;
			return *this;
		} 

		inline Vector4 operator /= (const f32 scale)
		{
			if(scale)
			{
				x/=scale;
				y/=scale;
				z/=scale;
				w/=scale;
			}
			return *this;
		}

		inline bool operator == (const Vector4 &right)
		{
			return (x == right.x &&
				y == right.y &&
				z == right.z &&
				w == right.w	);
		}

		inline bool operator == ( f32 *tArray )
		{
			return ( x == *tArray++ &&
				y == *tArray++ &&
				z == *tArray++ &&
				w == *tArray     );
		};

		inline bool operator != (const Vector4 &right)
		{
			return !(	x == right.x &&
				y == right.y &&
				z == right.z &&
				w == right.w		);
		}

		inline bool operator != ( f32 *tArray )
		{
			return ( x != *tArray++ &&
				y != *tArray++ &&
				z != *tArray++ &&
				w != *tArray      );
		}

		inline void Set(f32 xyzw)
		{
			x = y = z = w = xyzw;
		}

		inline void Set(f32 nx, f32 ny, f32 nz, f32 nw)
		{
			x = nx;
			y = ny;
			z = nz;
			w = nw;
		}

		inline void Set(const Vector4  & vec)
		{
			x = vec.x;
			y = vec.y;
			z = vec.z;
			w = vec.w;   
		}

		inline void Set( f32 *tArray )
		{
			x = *tArray++;
			y = *tArray++;
			z = *tArray++;
			w = *tArray;
		}

		inline void Set(const Vector3D  & vec, const f32 W = 1)
		{
			x = vec.x;
			y = vec.y;
			z = vec.z;
			w = W;   
		}

		inline Vector3D XYZ(void)
		{
			return Vector3D(x,y,z);
		}

		//friend inline std::ostream & operator<< ( std::ostream & out, const Vector4 & right)
		//{
		//	return out << "Vector4( " << right.x
		//		<< ", "
		//		<< right.y
		//		<< ", "
		//		<< right.z
		//		<< ", "
		//		<< right.w
		//		<< " )\n";
		//}

		friend inline Vector4 operator +  (const Vector4 &u,	const Vector4 &v	)
		{
			return Vector4 ( u.x+v.x, u.y+v.y, u.z+v.z, u.w+v.w );
		}

		friend inline Vector4 operator +  (const Vector4 &v,	const f32 s				)
		{
			return Vector4 ( s+v.x, s+v.y, s+v.z, s+v.w );
		}

		friend inline Vector4 operator +  (const f32 s,				const Vector4 &v	)
		{
			return Vector4 ( s+v.x, s+v.y, s+v.z, s+v.w );
		}

		friend inline Vector4 operator -  (const Vector4 &u,	const Vector4 &v	)
		{
			return Vector4 ( u.x-v.x, u.y-v.y, u.z-v.z, u.w-v.w );
		}

		friend inline Vector4 operator -  (const Vector4 &v,	const f32 s				)
		{
			return Vector4 ( -s+v.x, -s+v.y, -s+v.z, -s+v.w );
		}

		friend inline Vector4 operator -  (const f32 s,				const Vector4 &v	)
		{
			return Vector4 ( s-v.x, s-v.y, s-v.z, s-v.w );
		}

		friend inline Vector4 operator -  (const Vector4 &v							)
		{
			return Vector4 ( -v.x, -v.y, -v.z, -v.w );
		}

		friend inline Vector4 operator *  (const Vector4 &u,	const Vector4 &v	)
		{
			return Vector4 ( u.x*v.x, u.y*v.y, u.z+v.z, u.w+v.w );
		}

		friend inline Vector4 operator *  (const f32 s,				const Vector4 &v	)
		{
			return Vector4 ( s*v.x, s*v.y, s*v.z, s*v.w );
		}

		friend inline Vector4 operator *  (const Vector4 &v,	const f32 s				)
		{
			return Vector4 ( s*v.x, s*v.y, s*v.z, s*v.w );
		}

		friend inline Vector4 operator /  (const Vector4 &u,	const Vector4 &v	)
		{
			return Vector4 (	v.x > 0 ? u.x/v.x : 0, 
				v.y > 0 ? u.y/v.y : 0, 
				v.z > 0 ? u.z/v.z : 0,
				v.w > 0 ? u.w/v.w : 0  );
		}

		friend inline Vector4 operator /  (const Vector4 &v,	const f32 s				)
		{
			if( s ) return Vector4 ( v.x/s, v.y/s, v.z/s, v.w/s );
			else	return Vector4 (0,0,0,0);
		}

		friend inline Vector4 operator /  (const f32 s,				const Vector4 &v	)
		{
			if( s ) return Vector4 ( s/v.x, s/v.y, s/v.z, s/v.w );
			else	return Vector4 (0,0,0,0);
		}

		friend inline bool operator  == (const Vector4 &u,	const Vector4 &v	)
		{
			return u.x == v.x && u.y == v.y && u.z == v.z && u.w == v.w;
		}

		friend inline bool operator  != (const Vector4 &u,  const Vector4 &v	)
		{
			return !( u.x == v.x && u.y == v.y && u.z == v.z && u.w == v.w );
		}

	public:
		f32 x, y, z, w;


	public:
		static const Vector4 ZERO;
		static const Vector4 UNIT_X;
		static const Vector4 UNIT_Y;
		static const Vector4 UNIT_Z;
		static const Vector4 NEGATIVE_UNIT_X;
		static const Vector4 NEGATIVE_UNIT_Y;
		static const Vector4 NEGATIVE_UNIT_Z;
		static const Vector4 UNIT_SCALE;

	};//! end class Vector4


	typedef Vector4	   Vector4f;


	//const Vector4f Vector4f::ZERO(0, 0, 0, 0);
	//const Vector4f Vector4f::UNIT_X(1, 0, 0, 0);
	//const Vector4f Vector4f::UNIT_Y(0, 1, 0, 0);
	//const Vector4f Vector4f::UNIT_Z(0, 0, 1, 0);
	//const Vector4f Vector4f::NEGATIVE_UNIT_X(-1, 0, 0, 0);
	//const Vector4f Vector4f::NEGATIVE_UNIT_Y(0, -1, 0, 0);
	//const Vector4f Vector4f::NEGATIVE_UNIT_Z(0, 0, -1, 0);
	//const Vector4f Vector4f::UNIT_SCALE(1, 1, 1, 1);

	//const Vector4i Vector4i::ZERO(0, 0, 0, 0);
	//const Vector4i Vector4i::UNIT_X(1, 0, 0, 0);
	//const Vector4i Vector4i::UNIT_Y(0, 1, 0, 0);
	//const Vector4i Vector4i::UNIT_Z(0, 0, 1, 0);
	//const Vector4i Vector4i::NEGATIVE_UNIT_X(-1, 0, 0, 0);
	//const Vector4i Vector4i::NEGATIVE_UNIT_Y(0, -1, 0, 0);
	//const Vector4i Vector4i::NEGATIVE_UNIT_Z(0, 0, -1, 0);
	//const Vector4i Vector4i::UNIT_SCALE(1, 1, 1, 1);

	


};//! end namespace AURO

#endif  //! __AURO_AuroVector4_INCLUDE__







