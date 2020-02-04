#pragma once 

#include "MyMath.h"
#include "Vector3D.h"
#include "AABBBox3D.h"
#include "Triangle2D.h"

namespace AIR
{
	//template<class f32>class AABBBox3D;


    //! 3d triangle template class for doing collision detection and other things.
    class EngineExport Triangle3D
    {
    public:

        //! Constructor for an all 0 triangle
        Triangle3D() {}
        //! Constructor for triangle with given three vertices
        Triangle3D(Vector3Df v1, Vector3Df v2, Vector3Df v3) : pointA(v1), pointB(v2), pointC(v3) {}

        //! Equality operator
        bool operator == (const Triangle3D& other) const
        {
            return other.pointA == pointA && other.pointB == pointB && other.pointC == pointC;
        }

        //! Inequality operator
        bool operator != (const Triangle3D& other) const
        {
            return !(*this == other);
        }

        //! Determines if the triangle is totally inside a bounding box.
        /** \param box Box to check.
        \return True if triangle is within the box, otherwise false. */
		bool IsTotalInsideBox(const AIR::AABBBox3Df& box) const
        {
            return (
				box.IsPointInside(pointA) &&
                box.IsPointInside(pointB) &&
                box.IsPointInside(pointC));
        }

        //! Get the closest point on a triangle to a point on the same plane.
        /** \param p Point which must be on the same plane as the triangle.
        \return The closest point of the triangle */
        Vector3Df ClosestPointOnTriangle(const AIR::Vector3Df& p) const
        {
            const AIR::Vector3Df rab = Line3D(pointA, pointB).GetClosestPoint(p);
            const AIR::Vector3Df rbc = Line3D(pointB, pointC).GetClosestPoint(p);
            const AIR::Vector3Df rca = Line3D(pointC, pointA).GetClosestPoint(p);

            const f32 d1 = rab.GetDistanceFrom(p);
            const f32 d2 = rbc.GetDistanceFrom(p);
            const f32 d3 = rca.GetDistanceFrom(p);

            if (d1 < d2)
                return d1 < d3 ? rab : rca;

            return d2 < d3 ? rbc : rca;
        }

        //! Check if a point is inside the triangle
        /** \param p Point to test. Assumes that this point is already
        on the plane of the triangle.
        \return True if the point is inside the triangle, otherwise false. */
        bool IsPointInside(const Vector3Df& p) const
        {
            return (IsOnSameSide(p, pointA, pointB, pointC) &&
                IsOnSameSide(p, pointB, pointA, pointC) &&
                IsOnSameSide(p, pointC, pointA, pointB));
        }

        //! Check if a point is inside the triangle.
        /** This method is an implementation of the example used in a
        paper by Kasper Fauerby original written by Keidy from
        Mr-Gamemaker.
        \param p Point to test. Assumes that this point is already
        on the plane of the triangle.
        \return True if point is inside the triangle, otherwise false. */
        bool IsPointInsideFast(const Vector3Df& p) const
        {
            const Vector3Df f = pointB - pointA;
            const Vector3Df g = pointC - pointA;

            const f32 a = f.DotProduct(f);
            const f32 b = f.DotProduct(g);
            const f32 c = g.DotProduct(g);

            const Vector3Df vp = p - pointA;
            const f32 d = vp.DotProduct(f);
            const f32 e = vp.DotProduct(g);

            f32 x = (d*c)-(e*b);
            f32 y = (e*a)-(d*b);
            const f32 ac_bb = (a*c)-(b*b);
            f32 z = x+y-ac_bb;

            // return sign(z) && !(sign(x)||sign(y))
            return (( (IR(z)) & ~((IR(x))|(IR(y))) ) & 0x80000000)!=0;
        }


        //! Get an intersection with a 3d line.
        /** \param line Line to intersect with.
        \param outIntersection Place to store the intersection point, if there is one.
        \return True if there was an intersection, false if not. */
        bool GetIntersectionWithLimitedLine(const Line3D& line,
            Vector3Df& outIntersection) const
        {
            return GetIntersectionWithLine(line.start,
                line.GetVector(), outIntersection) &&
                outIntersection.IsBetweenPoints(line.start, line.end);
        }


        //! Get an intersection with a 3d line.
        /** Please note that also points are returned as intersection which
        are on the line, but not between the start and end point of the line.
        If you want the returned point be between start and end
        use getIntersectionWithLimitedLine().
        \param linePoint Point of the line to intersect with.
        \param lineVect Vector of the line to intersect with.
        \param outIntersection Place to store the intersection point, if there is one.
        \return True if there was an intersection, false if there was not. */
        bool GetIntersectionWithLine(const Vector3Df& linePoint,
            const Vector3Df& lineVect, Vector3Df& outIntersection) const
        {
            if (GetIntersectionOfPlaneWithLine(linePoint, lineVect, outIntersection))
                return IsPointInside(outIntersection);

            return false;
        }


        //! Calculates the intersection between a 3d line and the plane the triangle is on.
        /** \param lineVect Vector of the line to intersect with.
        \param linePoint Point of the line to intersect with.
        \param outIntersection Place to store the intersection point, if there is one.
        \return True if there was an intersection, else false. */
        bool GetIntersectionOfPlaneWithLine(const Vector3Df& linePoint,
            const Vector3Df& lineVect, Vector3Df& outIntersection) const
        {
            const Vector3Df normal = GetNormal().Normalize();
            f32 t2;

            if ( AIR::iszero ( t2 = normal.DotProduct(lineVect) ) )
                return false;

            f32 d = pointA.DotProduct(normal);
            f32 t = -(normal.DotProduct(linePoint) - d) / t2;
            outIntersection = linePoint + (lineVect * t);
            return true;
        }


        //! Get the normal of the triangle.
        /** Please note: The normal is not always normalized. */
        Vector3Df GetNormal() const
        {
            return (pointB - pointA).CrossProduct(pointC - pointA);
        }

        //! Test if the triangle would be front or backfacing from any point.
        /** Thus, this method assumes a camera position from which the
        triangle is definitely visible when looking at the given direction.
        Do not use this method with points as it will give wrong results!
        \param lookDirection Look direction.
        \return True if the plane is front facing and false if it is backfacing. */
        bool IsFrontFacing(const Vector3Df& lookDirection) const
        {
            const Vector3Df n = GetNormal().Normalize();
            const f32 d = (f32)n.DotProduct(lookDirection);
            return F32_LOWER_EQUAL_0(d);
        }

        //! Get the plane of this triangle.
        Plane3D GetPlane() const
        {
            return Plane3D(pointA, pointB, pointC);
        }

        //! Get the area of the triangle
        f32 GetArea() const
        {
            return (pointB - pointA).CrossProduct(pointC - pointA).GetLength() * 0.5;

        }

        //! sets the triangle's points
        void Set(const AIR::Vector3Df& a, const AIR::Vector3Df& b, const AIR::Vector3Df& c)
        {
            pointA = a;
            pointB = b;
            pointC = c;
        }

		Triangle2D GetHorzProjTriangle2D( void )
		{
			return Triangle2D( Vector2D(pointA.x, pointA.z), Vector2D(pointB.x, pointB.z), Vector2D(pointC.x, pointC.z) );
		}


		inline int SharingSide( const Triangle3D& other )
		{
			int n = 0;
			if (pointA.Equals(other.pointA)||
				pointA.Equals(other.pointB)||
				pointA.Equals(other.pointC))
			{
				n = n|1;
			}

			if (pointB.Equals(other.pointA)||
				pointB.Equals(other.pointB)||
				pointB.Equals(other.pointC))
			{
				n = n|2;
			}

			if (pointC.Equals(other.pointA)||
				pointC.Equals(other.pointB)||
				pointC.Equals(other.pointC))
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


		Vector3Df GetCenterOfTriangle( void ) const
		{
			double x = (pointA.x + pointB.x + pointC.x)/3;
			double y = (pointA.y + pointB.y + pointC.y)/3;
			double z = (pointA.z + pointB.z + pointC.z)/3;
			return Vector3Df(x, y, z);
		}

		bool Is2DPointInHorzProjTriangle( f32 x, f32 z, double fDisErrTor = 0.0 )
		{
			//Vector2D v0 (pointB.x - pointA.x, pointB.z - pointA.z); 
			//Vector2D v0p(x - pointA.x, z - pointA.z );
			//Vector2D v1 (pointC.x - pointB.x, pointC.z - pointB.z);
			//Vector2D v1p(x - pointB.x, z - pointB.z );
			//Vector2D v2 (pointA.x - pointC.x, pointA.z - pointC.z);
			//Vector2D v2p(x - pointC.x, z - pointC.z );

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
			//return false;

			Vector2D v0 (pointB.x - pointA.x, pointB.z - pointA.z); 
			Vector2D v0p(x - pointA.x, z - pointA.z );
			Vector2D v1 (pointC.x - pointB.x, pointC.z - pointB.z);
			Vector2D v1p(x - pointB.x, z - pointB.z );
			Vector2D v2 (pointA.x - pointC.x, pointA.z - pointC.z);
			Vector2D v2p(x - pointC.x, z - pointC.z );

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



	public:

        //! the three points of the triangle
        Vector3Df pointA;
        Vector3Df pointB;
        Vector3Df pointC;

    private:
        bool IsOnSameSide(
			const Vector3Df& p1, 
			const Vector3Df& p2,
            const Vector3Df& a, 
			const Vector3Df& b) const
        {
            Vector3Df bminusa = b - a;
            Vector3Df cp1 = bminusa.CrossProduct(p1 - a);
            Vector3Df cp2 = bminusa.CrossProduct(p2 - a);
            return (cp1.DotProduct(cp2) >= 0.0f);
        }
    };


    //! Typedef for a f32 3d triangle.
    typedef Triangle3D Triangle3Df;

}