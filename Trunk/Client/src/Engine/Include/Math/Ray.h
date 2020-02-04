//------------------------------------------------------------------------------
/**
    @class	Ray

	@brief	

    @date	2012-7-12

	@author HJJ
*/
//------------------------------------------------------------------------------

#pragma once

#ifndef __Ray_H__
#define __Ray_H__

#include "MyMath.h"
#include "Matrix4.h"
#include "Vector3D.h"

namespace AIR
{
	enum IntersectFlag
	{
		IF_CW,
		IF_CCW,
		IF_ALL
	};

	class EngineExport Ray
	{
	public:
		Ray():m_Range(AIR::MaxFloat){};
		~Ray(){};

		//和水平面相交
		bool IntersectHorizon(f32 y, f32 *pt = NULL) const; 
		bool IntersectPlane(const Plane3Df *plane, f32 *pt = NULL) const;
		bool IntersectAabbLax(const AABBBox3Df* pAabb, f32 *pt = NULL) const;

		// return <=0: no intersect
		bool IntersectTriangle(const Vector3Df &v1, const Vector3Df &v2, const Vector3Df &v3, f32 *pt = NULL) const;
		bool IntersectTriangle(const Vector3Df &v1, const Vector3Df &v2, const Vector3Df &v3, f32 *pt, f32 *pu, f32 *pv ) const;
		bool IntersectTriangleLax(const Vector3Df &v1, const Vector3Df &v2,	const Vector3Df &v3, f32 *pt = NULL, IntersectFlag eFlag = IF_ALL) const;
		bool IntersectQuad(const Vector3Df &v1, const Vector3Df &v2, const Vector3Df &v3, f32 *pt = NULL) const;
		Vector3Df GetPointAt(f32 dist) const;

	public:
		Vector3Df	m_Origin;
		Vector3Df	m_Dir;
		f32			m_Range;
	};



	//------------------------------------------------------------------------------
	inline bool Ray::IntersectHorizon( f32 y, f32 *pt ) const//和水平面相交
	{
		if( m_Dir.y == 0 ) return false;

		f32 t = (y-m_Origin.y) / m_Dir.y;
		if( t<0 || t>m_Range ) return false;

		if( pt ) *pt = t;
		return true;
	}

	inline bool Ray::IntersectPlane( const Plane3Df *plane, f32 *pt ) const
	{
		const Vector3Df& normal = plane->Normal;

		f32 temp = DotProduct( m_Dir, normal );
		if(iszero(temp))
			return false;

		f32 t= -(DotProduct(m_Origin, normal) + plane->D) / temp;
		if(NULL != pt)
			*pt = t;
		if(t < 0 || t > m_Range)
			return false;

		return true;
	}

	inline bool Ray::IntersectAabbLax(const AABBBox3Df* pAabb, f32 *pt) const
	{
		const AABBBox3Df *pbox = pAabb;

		Vector3Df origin = m_Origin - pbox->GetCenter();
		Vector3Df minpt(-pbox->GetExtent() * 1.01f);
		Vector3Df maxpt = pbox->GetExtent() * 1.01f;

		if( origin.x>=minpt.x && origin.x<=maxpt.x &&origin.y>=minpt.y && origin.y<=maxpt.y &&origin.z>=minpt.z && origin.z<=maxpt.z )
		{
			if( pt ) *pt = 0;
			return true;
		}

		if( (origin.x<minpt.x || origin.x>maxpt.x) && m_Dir.x!=0 )
		{
			f32 x = origin.x<minpt.x ? minpt.x : maxpt.x;
			f32 t = (x - origin.x) / m_Dir.x;
			if( t < 0 ) return false;

			f32 y = origin.y + t*m_Dir.y;
			f32 z = origin.z + t*m_Dir.z;
			if( y>minpt.y && y<maxpt.y && z>minpt.z && z<maxpt.z )
			{
				if( pt ) *pt = t;
				return true;
			}
		}

		if( (origin.y<minpt.y || origin.y>maxpt.y) && m_Dir.y!=0 )
		{
			f32 y = origin.y<minpt.y ? minpt.y : maxpt.y;
			f32 t = (y - origin.y) / m_Dir.y;
			if( t < 0 ) return false;

			f32 x = origin.x + t*m_Dir.x;
			f32 z = origin.z + t*m_Dir.z;
			if( x>minpt.x && x<maxpt.x && z>minpt.z && z<maxpt.z )
			{
				if( pt ) *pt = t;
				return true;
			}
		}

		if( (origin.z<minpt.z || origin.z>maxpt.z) && m_Dir.z!=0 )
		{
			f32 z = origin.z<minpt.z ? minpt.z : maxpt.z;
			f32 t = (z - origin.z) / m_Dir.z;
			if( t < 0 ) return false;

			f32 x = origin.x + t*m_Dir.x;
			f32 y = origin.y + t*m_Dir.y;
			if( x>minpt.x && x<maxpt.x && y>minpt.y && y<maxpt.y )
			{
				if( pt ) *pt = t;
				return true;
			}
		}

		return false;
	}

	/*
	O + t*D = (1-u-v)*V1 + u*V2 + v*V3
	E1 = V2-V1, E2 = V3-V1, S = O-V1, P = D^E2, Q = S^E1
	t      1   Q*E2
	u  = ----  P*S
	v    P*E1  Q*D
	*/
	inline bool Ray::IntersectTriangle( const Vector3Df &v1, const Vector3Df &v2, const Vector3Df &v3, f32 *pt ) const
	{
		Vector3Df E1, E2, S, P, Q;
		E1 = v2 - v1;
		E2 = v3 - v1;
		S = m_Origin - v1;

		P = CrossProduct( m_Dir, E2 );
		Q = CrossProduct( S, E1 );

		f32 r = DotProduct( P, E1 );
		if( r == 0 ) return false;
		r = 1.0f / r;

		//if( t < 0 ) not intersect ray but intersect line
		f32 t = DotProduct(Q, E2) * r;
		if( t<0 || t>m_Range ) return false;

		f32 u = DotProduct(P, S) * r;
		f32 v = DotProduct(Q, m_Dir) * r;

		if( u>=0 && v>=0 && u+v<=1.0f )
		{
			if( pt ) *pt = t;
			return true;
		}
		return false;
	}

	inline bool Ray::IntersectTriangle( const Vector3Df &v1, const Vector3Df &v2, const Vector3Df &v3, f32 *pt, f32 *pu, f32 *pv ) const
	{
		Vector3Df E1, E2, S, P, Q;
		E1 = v2 - v1;
		E2 = v3 - v1;
		S = m_Origin - v1;

		P = CrossProduct( m_Dir, E2 );
		Q = CrossProduct( S, E1 );

		f32 r = DotProduct( P, E1 );
		if( r == 0 ) return false;
		r = 1.0f / r;

		//if( t < 0 ) not intersect ray but intersect line
		f32 t = DotProduct(Q, E2) * r;
		if( t<0 || t>m_Range ) return false;

		f32 u = DotProduct(P, S) * r;
		f32 v = DotProduct(Q, m_Dir) * r;

		if( u>=0 && v>=0 && u+v<=1.0f )
		{
			if( pt ) *pt = t;
			if( pu ) *pu = u;
			if( pv ) *pv = v;
			return true;
		}
		return false;
	}

	inline bool Ray::IntersectTriangleLax( const Vector3Df &v1, const Vector3Df &v2, 
		const Vector3Df &v3, f32 *pt, IntersectFlag eFlag) const
	{
		Vector3Df E1, E2, S, P, Q;
		E1 = v2 - v1;
		E2 = v3 - v1;
		S = m_Origin - v1;

		P = CrossProduct( m_Dir, E2 );
		Q = CrossProduct( S, E1 );

		f32 r = DotProduct( P, E1 );

		if( eFlag == IF_ALL )
		{
			if( r == 0 ) return false;
		}
		else if( eFlag == IF_CW )
		{
			if( r >= 0 ) return false;
		}
		else
		{
			if( r <= 0 ) return false;
		}

		r = 1.0f / r;

		//if( t < 0 ) not intersect ray but intersect line
		f32 t = DotProduct(Q, E2) * r;
		if( t<0 || t>m_Range ) return false;

		f32 u = DotProduct(P, S) * r;
		f32 v = DotProduct(Q, m_Dir) * r;

		if( u>=-0.002 && v>=-0.002 && u+v<=1.002f )
		{
			if( pt ) *pt = t;
			return true;
		}
		return false;
	}

	//  v1--------v3
	//   |        |
	//  v2--------v4
	inline bool Ray::IntersectQuad( const Vector3Df &v1, const Vector3Df &v2, const Vector3Df &v3, f32 *pt ) const
	{
		Vector3Df E1, E2, S, P, Q;
		E1 = v2-v1;
		E2 = v3-v1;
		S = m_Origin-v1;

		P = CrossProduct( m_Dir, E2 );
		Q = CrossProduct( S, E1 );

		f32 r = DotProduct( P, E1 );
		if( r == 0 ) return false;
		r = 1.0f / r;

		//if( t < 0 ) not intersect ray but intersect line
		f32 t = DotProduct(Q, E2) * r;
		if( t<0 && t>m_Range ) return false;

		f32 u = DotProduct(P, S) * r;
		f32 v = DotProduct(Q, m_Dir) * r;

		if( u>=0 && v>=0 && u<=1.0f && v<=1.0f )
		{
			if( pt ) *pt = t;
			return true;
		}
		return false;
	}

	inline Vector3Df Ray::GetPointAt(f32 dist) const
	{
		return m_Origin + m_Dir * dist;
	}
};




#endif //__Ray_H__