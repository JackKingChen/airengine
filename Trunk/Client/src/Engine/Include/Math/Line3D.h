#pragma once 

#include "MyMath.h"
#include "Vector3D.h"
#include "Rect.h"

namespace AIR
{
    //! 3D line between two points with intersection methods.
    class EngineExport Line3D
    {
    public:

        //! Default constructor
        /** line from (0,0,0) to (1,1,1) */
        inline Line3D() : start(0,0,0), end(1,1,1) {}
        //! Constructor with two points
        inline Line3D(f32 xa, f32 ya, f32 za, f32 xb, f32 yb, f32 zb) : start(xa, ya, za), end(xb, yb, zb) {}
        //! Constructor with two points as vectors
        inline Line3D(const Vector3Df& start, const Vector3Df& end) : start(start), end(end) {}

        // operators

        inline Line3D operator + (const Vector3Df& point) const 
        { 
            return Line3D(start + point, end + point); 
        }

        inline Line3D& operator += (const Vector3Df& point) 
        { 
            start += point; 
            end += point; 
            return *this; 
        }

        inline Line3D operator - (const Vector3Df& point) const 
        { 
            return Line3D(start - point, end - point); 
        }

        Line3D& operator -= (const Vector3Df& point) 
        { 
            start -= point; 
            end -= point; 
            return *this; 
        }

        inline bool operator == (const Line3D& other) const
        { 
            return (start==other.start && end==other.end) || (end==other.start && start==other.end);
        }

        inline bool operator != (const Line3D& other) const
        { 
            return !(start==other.start && end==other.end) || (end==other.start && start==other.end);
        }

        // functions
        //! Set this line to a new line going through the two points.
        inline void SetLine(const f32& xa, const f32& ya, const f32& za, const f32& xb, const f32& yb, const f32& zb)
        {
            start.Set(xa, ya, za); 
            end.Set(xb, yb, zb);
        }

        //! Set this line to a new line going through the two points.
        inline void SetLine(const Vector3Df& nstart, const Vector3Df& nend)
        {
            start.Set(nstart); 
            end.Set(nend);
        }

        //! Set this line to new line given as parameter.
        inline void SetLine(const Line3D& line)
        {
            start.Set(line.start); 
            end.Set(line.end);
        }


		//************************************
		// Method:    SetLineByPosAndDir
		// FullName:  AIR::Line3D::SetLineByPosAndDir
		// Access:    public 
		// Returns:   void
		// Qualifier: 通过一点和方向向量来设置直线
		// Parameter: const Vector3Df & pos 直线经过的点
		// Parameter: const Vector3Df & dir 直线方向向量
		//************************************
		inline void SetLineByPosAndDir( const Vector3Df& pos, const Vector3Df& dir )
		{
			start = pos;
			end = pos + dir;
		}

        //! Get length of line
        /** \return Length of line. */
        inline f32 GetLength() const 
        { 
            return start.GetDistanceFrom(end); 
        }

        //! Get squared length of line
        /** \return Squared length of line. */
        inline f32 GetLengthSQ() const 
        { 
			return 0;//start.GetLengthSQ(end);
        }

        //! Get middle of line
        /** \return Center of line. */
        inline Vector3Df GetMiddle() const
        {
            return (start + end) * (f32)0.5;
        }

        //! Get vector of line
        /** \return vector of line. */
        inline Vector3Df GetVector() const
        {
            return end - start;
        }

        //! Check if the given point is between start and end of the line.
        /** Assumes that the point is already somewhere on the line.
        \param point The point to test.
        \return True if point is on the line between start and end, else false.
        */
        inline bool IsPointBetweenStartAndEnd(const Vector3Df& point) const
        {
            return point.IsBetweenPoints(start, end);
        }

        //! Get the closest point on this line to a point
        /** \param point The point to compare to.
        \return The nearest point which is part of the line. */
        inline Vector3Df GetClosestPoint(const Vector3Df& point) const
        {
            Vector3Df c = point - start;
            Vector3Df v = end - start;
            f32 d = (f32)v.GetLength();
            v /= d;
            f32 t = v.DotProduct(c);

            if (t < (f32)0.0)
                return start;
            if (t > d)
                return end;

            v *= t;
            return start + v;
        }

        //! Check if the line intersects with a shpere
        /** \param sorigin: Origin of the shpere.
        \param sradius: Radius of the sphere.
        \param outdistance: The distance to the first intersection point.
        \return True if there is an intersection.
        If there is one, the distance to the first intersection point
        is stored in outdistance. */
        inline bool GetIntersectionWithSphere(Vector3Df sorigin, f32 sradius, f64& outdistance) const
        {
            const Vector3Df q = sorigin - start;
            f32 c = q.GetLength();
            f32 v = q.DotProduct(GetVector().Normalize());
            f32 d = sradius * sradius - (c*c - v*v);

            if (d < 0.0)
                return false;

            outdistance = v - sqrt((f64)d);
            return true;
        }


		//************************************
		// Method:    GetDistanceFromPointToLine
		// FullName:  AIR::Line3D::GetDistanceFromPointToLine
		// Access:    public 
		// Returns:   boost::f32
		// Qualifier: 计算某点P到直线的距离
		// Parameter: const Vector3Df & point
		//************************************
		inline f32 GetDistanceFromPointToLine( const Vector3Df& point )
		{
			Vector3Df vP2S = point - start;
			Vector3Df vE2S = end - start;
			Vector3Df vP2SxE2S = vP2S.CrossProduct(vE2S);
			f32 fLenP2S = vP2S.GetLength();
			f32 fLenE2S = vE2S.GetLength();
			f32 sinAngle = vP2SxE2S.GetLength() / (fLenP2S * fLenE2S);
			return f32(fLenP2S * sinAngle);
		}


		inline bool IsEqual( const Line3D& other )
		{
			if ( start.Equals(other.start) && end.Equals(other.end) )
			{
				return true;
			}
			return false;
		}

		inline bool IsEqualNotDetectDirection( const Line3D& other )
		{
			if ( start.Equals(other.start) && end.Equals(other.end) ||
				 start.Equals(other.end) && end.Equals(other.start) )
			{
				return true;
			}
			return false;
		}

		//! 获得点与线的关系
		// 线是顺时钟
		// 点在线左边返回 -1
		// 点在线的右边返回 1,
		// 点在线上返回0
		inline int HorzRelationShipOfLineAndPoint( const Vector3Df& point )
		{
			//      S	 vL      E
			//		@------------> 
			//       \ 
			//		  \ vP
			//         @ 
			//         P
			Vector2D vL(Vector2D(start.x, start.z), Vector2D(end.x, end.z));
			Vector2D vP(Vector2D(start.x, start.z), Vector2D(point.x, point.z));

			double re = vL.CrossProduct( vP );

			if ( re < 0 )
			{
				return 1;
			}
			else if ( re > 0 )
			{
				return -1;
			}
			else
			{
				return 0;
			}
		}

		//判断2条线段是否相交
		inline bool HorzIsIntersect( const Line3D& other )
		{
			Rect re1( Vector2D(start.x, start.z), Vector2D(end.x, end.z) );
			Rect re2( Vector2D(other.start.x, other.start.z), Vector2D(other.end.x, other.end.z) );
			re1.Normalize();
			re2.Normalize();

			if( !re1.IsCross(re2) )
			{
				return false;
			}

			// 相互跨立
			Vector2D ve1 ( Vector2D(start.x, start.z), Vector2D(end.x, end.z) );
			Vector2D ve11( Vector2D(start.x, start.z), Vector2D(other.start.x, other.start.z) );
			Vector2D ve12( Vector2D(start.x, start.z), Vector2D(other.end.x, other.end.z) );
			double cro1 = ve1.CrossProduct(ve12) * ve1.CrossProduct(ve11);

			Vector2D ve2 ( Vector2D(other.start.x, other.start.z), Vector2D(other.end.x, other.end.z) );
			Vector2D ve21( Vector2D(other.start.x, other.start.z), Vector2D(start.x, start.z) );
			Vector2D ve22( Vector2D(other.start.x, other.start.z), Vector2D(end.x, end.z) );
			double cro2 = ve2.CrossProduct(ve22) * ve2.CrossProduct(ve21);

			if ( cro1 == 0 && cro2 == 0 )
			{
				return false;
			}
			if ( cro1 <= 0 && cro2 <= 0)
			{
				return true;
			}
			return false;
		}



	public:

        // member variables

        //! Start point of line
        Vector3Df start;
        //! End point of line
        Vector3Df end;
    };

    //! Typedef for an f32 line.
    typedef Line3D Line3Df;


}