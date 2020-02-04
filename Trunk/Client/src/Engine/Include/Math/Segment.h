//------------------------------------------------------------------------------
/**
    @class	Segment

	@brief	

    @date	2012-8-12

	@author HJJ
*/
//------------------------------------------------------------------------------

#pragma once

#ifndef __Segment_H__
#define __Segment_H__

#include "Vector3D.h"
#include "Ray.h"

namespace AIR
{

	class EngineExport Segment
	{
	public:
		Segment(void){};
		Segment(const AIR::Vector3Df& start, const AIR::Vector3Df& end);
		~Segment(void){};

		AIR::Vector3Df& operator[] (int i);
		const AIR::Vector3Df& operator[](int i) const;

		float Distance(const AIR::Vector3Df& v) const;
		float Distance(const Segment& seg) const;
		float Distance(const AIR::Ray& seg) const;

		void NearestPoint(const Segment& segment, AIR::Vector3Df& outMyPoint, AIR::Vector3Df& outTheirPoint) const;
		AIR::Vector3Df NearestPoint(const AIR::Vector3Df& point) const;

		bool IntersectTriangle(const AIR::Vector3Df& v1, const AIR::Vector3Df& v2, const AIR::Vector3Df& v3, AIR::Vector3Df& outPoint) const;

	private:
		AIR::Vector3Df point[2];
	};


	inline Segment::Segment( const AIR::Vector3Df& start, const AIR::Vector3Df& end )
	{
		point[0] = start;
		point[1] = end;
	}

	inline AIR::Vector3Df& Segment::operator[] (int i)
	{
		return point[i];
	}

	inline const AIR::Vector3Df& Segment::operator[]( int i ) const
	{
		return point[i];
	}

	inline AIR::Vector3Df Segment::NearestPoint( const AIR::Vector3Df& v ) const
	{
		AIR::Vector3Df ab = point[1]-point[0];
		AIR::Vector3Df ac = v-point[0];

		float f = DotProduct(ab,ac);

		if(f<0)
			return point[0];

		float d = ab.GetLength();

		f/=d;

		if(f>d)
			return point[1];

		f/=d;
		AIR::Vector3Df D=point[0]+ab*f;

		return D;
	}

	inline float Segment::Distance( const AIR::Vector3Df& v ) const
	{
		return (v-NearestPoint(v)).GetLength();
	}

	inline void Segment::NearestPoint( const Segment& segment, AIR::Vector3Df& outMyPoint, AIR::Vector3Df& outTheirPoint ) const
	{
		float a,b,c,m,n,Q,t,s,distan;

		a=(point[1].x-point[0].x)*(point[1].x-point[0].x)+(point[1].y-point[0].y)*(point[1].y-point[0].y)+(point[1].z-point[0].z)*(point[1].z-point[0].z);
		b=(point[1].x-point[0].x)*(segment.point[1].x-segment.point[0].x)+(point[1].y-point[0].y)*(segment.point[1].y-segment.point[0].y)+(point[1].z-point[0].z)*(segment.point[1].z-segment.point[0].z);
		c=(point[0].x-point[1].x)*(point[0].x-segment.point[0].x)+(point[0].y-point[1].y)*(point[0].y-segment.point[0].y)+(point[0].z-point[1].z)*(point[0].z-segment.point[0].z);
		m=(point[1].x-point[0].x)*(segment.point[1].x-segment.point[0].x)+(point[1].y-point[0].y)*(segment.point[1].y-segment.point[0].y)+(point[1].z-point[0].z)*(segment.point[1].z-segment.point[0].z);
		n=(segment.point[1].x-segment.point[0].x)*(segment.point[1].x-segment.point[0].x)+(segment.point[1].y-segment.point[0].y)*(segment.point[1].y-segment.point[0].y)+(segment.point[1].z-segment.point[0].z)*(segment.point[1].z-segment.point[0].z);
		Q=(point[0].x-segment.point[0].x)*(segment.point[1].x-segment.point[0].x)+(point[0].y-segment.point[0].y)*(segment.point[1].y-segment.point[0].y)+(point[0].z-segment.point[0].z)*(segment.point[1].z-segment.point[0].z);

		t=(c*m+a*Q)/(a*n-b*m);
		s=(c*n+b*Q)/(a*n-b*m);

		if((t >= 0 && t <= 1)&&(s >= 0 && s <= 1))
		{
			outMyPoint.x=point[0].x+s*(point[1].x-point[0].x);
			outMyPoint.y=point[0].y+s*(point[1].y-point[0].y);
			outMyPoint.z=point[0].z+s*(point[1].z-point[0].z);
			outTheirPoint.x=segment.point[0].x+t*(segment.point[1].x-segment.point[0].x);
			outTheirPoint.y=segment.point[0].y+t*(segment.point[1].y-segment.point[0].y);
			outTheirPoint.z=segment.point[0].z+t*(segment.point[1].z-segment.point[0].z);
		}
		else if(s<0)
		{
			outMyPoint = point[0];
			outTheirPoint = segment.NearestPoint(point[0]);
		}
		else if(s>1)
		{
			outMyPoint = point[1];
			outTheirPoint = segment.NearestPoint(point[1]);
		}
		else if(t<0)
		{
			outTheirPoint = segment.point[0];
			outMyPoint = NearestPoint(segment.point[0]);
		}
		else if(t>1)
		{
			outTheirPoint = segment.point[1];
			outMyPoint = NearestPoint(segment.point[1]);
		}

	}

	inline float Segment::Distance( const Segment& seg ) const
	{
		AIR::Vector3Df myPoint;
		AIR::Vector3Df theirPoint;

		NearestPoint(seg, myPoint, theirPoint);
		return (myPoint-theirPoint).GetLength();
	}

	inline float Segment::Distance( const AIR::Ray& seg ) const
	{
		return 0;
	}

	inline bool Segment::IntersectTriangle( const AIR::Vector3Df& v1, const AIR::Vector3Df& v2, const AIR::Vector3Df& v3, AIR::Vector3Df& outPoint ) const
	{
		AIR::Vector3Df dir = point[1]-point[0];
		float range = dir.GetLength();
		dir/=range;

		AIR::Vector3Df E1, E2, S, P, Q;
		E1 = v2 - v1;
		E2 = v3 - v1;
		S = point[0] - v1;

		P = CrossProduct( dir, E2 );
		Q = CrossProduct( S, E1 );

		float r = DotProduct( P, E1 );
		if( r == 0 ) return false;
		r = 1.0f / r;

		//if( t < 0 ) not intersect ray but intersect line
		float t = DotProduct(Q, E2) * r;
		if( t<0 || t>range ) return false;

		float u = DotProduct(P, S) * r;
		float v = DotProduct(Q, dir) * r;

		if( u>=0 && v>=0 && u+v<=1.0f )
		{
			outPoint = point[0]+dir*t;
			return true;
		}
		return false;
	}

};


#endif //__Segment_H__