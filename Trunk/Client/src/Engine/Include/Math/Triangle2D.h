// ***************************************************************
//  文件: Triangle2D.h
//  日期: 2010-7-6
//  作者: huangjunjie
//  说明: 
// ***************************************************************

#pragma once

#ifndef __Triangle2D_H__
#define __Triangle2D_H__

#include "MyMath.h"
#include "Vector2D.h"
#include "Circle2D.h"


namespace AIR
{
	class EngineExport Triangle2D
	{
	public:
		inline Triangle2D( void )
		{
		}

		inline Triangle2D( const Vector2D& pt0, const Vector2D& pt1, const Vector2D& pt2 ) : m_pt0(pt0), m_pt1(pt1), m_pt2(pt2)
		{
		}

		inline Triangle2D( const Triangle2D& other ) : m_pt0(other.m_pt0), m_pt1(other.m_pt1), m_pt2(other.m_pt2)
		{
		}

		~Triangle2D( void )
		{
		}

		inline Circle2D GetCircumCircle( void )
		{
			double m1   = -(m_pt1.x - m_pt0.x) / (m_pt1.y - m_pt0.y);
			double m2   = -(m_pt2.x - m_pt1.x) / (m_pt2.y - m_pt1.y);
			double mx1  =  (m_pt0.x + m_pt1.x) / 2.0;
			double mx2  =  (m_pt1.x + m_pt2.x) / 2.0;
			double my1  =  (m_pt0.y + m_pt1.y) / 2.0;
			double my2  =  (m_pt1.y + m_pt2.y) / 2.0;

			double xc   =  (m1 * mx1 - m2 * mx2 + my2 - my1) / (m1 - m2);
			double yc   =   m1 * (xc - mx1) + my1;

			double dx   =   m_pt1.x - xc;
			double dy   =   m_pt1.y - yc;
			double rsqr =   dx * dx + dy * dy;
			double r    =   sqrt(rsqr);

			return Circle2D( Vector2D(xc, yc), f32(r) );
		}


		inline bool IsInTriangle( const Vector2D& pt, double fDisErrTor = 0 )
		{
			//Vector2D v0 ( m_pt0, m_pt1 );
			//Vector2D v0p( m_pt0, pt    );
			//Vector2D v1 ( m_pt1, m_pt2 );
			//Vector2D v1p( m_pt1, pt    );
			//Vector2D v2 ( m_pt2, m_pt0 );
			//Vector2D v2p( m_pt2, pt    );

			//double c1 = v0.CrossProduct(v0p);
			//double c2 = v1.CrossProduct(v1p);
			//double c3 = v2.CrossProduct(v2p);
			//if ( c1 <= AIR::ROUNDING_ERROR_64 && c2 <= AIR::ROUNDING_ERROR_64 && c3 <= AIR::ROUNDING_ERROR_64 )
			//{
			//	return true;
			//}
			//if ( c1 >= AIR::ROUNDING_ERROR_64 && c2 >= AIR::ROUNDING_ERROR_64 && c3 >= AIR::ROUNDING_ERROR_64 )
			//{
			//	return true;
			//}

			
			Vector2D v0 ( m_pt0, m_pt1 );	// p0->p1
			Vector2D v0p( m_pt0, pt    );	// p0->p
			Vector2D v1 ( m_pt1, m_pt2 );	// p1->p2
			Vector2D v1p( m_pt1, pt    );	// p1->p
			Vector2D v2 ( m_pt2, m_pt0 );	// p2->p0
			Vector2D v2p( m_pt2, pt    );	// p2->p

			double c1 = v0.CrossProduct(v0p);
			double c2 = v1.CrossProduct(v1p);
			double c3 = v2.CrossProduct(v2p);
			if ( c1 <= AIR::ROUNDING_ERROR_64 && c2 <= AIR::ROUNDING_ERROR_64 && c3 <= AIR::ROUNDING_ERROR_64 )
			{
				return true;
			}
			if ( c1 >= AIR::ROUNDING_ERROR_64 && c2 >= AIR::ROUNDING_ERROR_64 && c3 >= AIR::ROUNDING_ERROR_64 )
			{
				return true;
			}

			if ( fDisErrTor == 0.0 )
			{
				return false;
			}

			// 如果点p到点p0的距离小于误差值, 点P可当作在三角形之内
			double distP2A = v0p.GetLength();
			if ( distP2A <= fDisErrTor )
			{
				return true;
			}

			// 如果点p到点p1的距离小于误差值, 点P可当作在三角形之内
			double distP2B = v1p.GetLength();
			if ( distP2B <= fDisErrTor )
			{
				return true;
			}

			// 如果点p到点p2的距离小于误差值, 点P可当作在三角形之内
			double distP2C = v2p.GetLength();
			if ( distP2C <= fDisErrTor )
			{
				return true;
			}

			// 求线段p0p1的长度
			double l0 = v0.GetLength();
			double V0PDotV0 = v0p.DotProduct( v0 );
			double dCosAngle = V0PDotV0 / l0 / distP2A;
			dCosAngle = AIR::Clamp<double>( dCosAngle, -1.0, 1.0 );
			// 计算点p到线段p0p1的距离
			double distP2V0 = distP2A * AIR::asm_sin( AIR::asm_arccos( dCosAngle ) );
			if ( V0PDotV0 >= 0 && distP2V0 <= fDisErrTor && distP2A <= l0 )
			{
				return true;
			}

			// 求线段p1p2的长度
			double l1 = v1.GetLength();
			double V1PDotV1 = v1p.DotProduct( v1 );
			dCosAngle = V1PDotV1 / l1 / distP2B;
			dCosAngle = AIR::Clamp<double>( dCosAngle, -1.0, 1.0 );
			// 计算点p到线段p1p2的距离
			double distP2V1 = distP2B * AIR::asm_sin( AIR::asm_arccos( dCosAngle ) );
			if ( V1PDotV1 >= 0 && distP2V1 <= fDisErrTor && distP2B <= l1 )
			{
				return true;
			}

			// 求线段p2p0的长度
			double l2 = v2.GetLength();
			double V2PDotV2 = v2p.DotProduct( v2 );
			dCosAngle = V2PDotV2 / l2 / distP2C;
			dCosAngle = AIR::Clamp<double>( dCosAngle, -1.0, 1.0 );
			// 计算点p到线段p2p0的距离
			double distP2V2 = distP2C * AIR::asm_sin( AIR::asm_arccos( dCosAngle ) );
			if ( V2PDotV2 >= 0 && distP2V2 <= fDisErrTor && distP2C <= l2 )
			{
				return true;
			}

			return false;
		}


		inline int SharingSide( const Triangle2D& other )
		{
			int n = 0;
			if (m_pt0.Equals(other.m_pt0)||
				m_pt0.Equals(other.m_pt1)||
				m_pt0.Equals(other.m_pt2))
			{
				n = n|1;
			}

			if (m_pt1.Equals(other.m_pt0)||
				m_pt1.Equals(other.m_pt1)||
				m_pt1.Equals(other.m_pt2))
			{
				n = n|2;
			}

			if (m_pt2.Equals(other.m_pt0)||
				m_pt2.Equals(other.m_pt1)||
				m_pt2.Equals(other.m_pt2))
			{
				n = n|4;
			}

			if ( n == 3)
			{
				return 0;
			}
			else if ( n == 5)
			{
				return 2;
			}
			else if ( n == 6)
			{
				return 1;
			}
			return -1;
		}



		inline Vector2D GetCenterOfTriangle( void )
		{
			double x = (m_pt0.x + m_pt1.x + m_pt2.x)/3;
			double y = (m_pt0.y + m_pt1.y + m_pt2.y)/3;
			return Vector2D(x,y);
		}


	public:
		Vector2D m_pt0;
		Vector2D m_pt1;
		Vector2D m_pt2;

	};//! end class Triangle2D

	typedef Triangle2D	Triangle2Df;


};



#endif //! __Triangle2D_H__