#pragma once

#include "MyMath.h"
#include "Vector3D.h"

namespace AIR
{
    //! Enumeration for intersection relations of 3d objects
    enum EIntersectionRelation3D
    {
        ISREL3D_FRONT = 0,
        ISREL3D_BACK,
        ISREL3D_PLANAR,
        ISREL3D_SPANNING,
        ISREL3D_CLIPPED
    };
    //! Template plane class with some intersection testing methods.
    class EngineExport Plane3D
    {
    public:

        // Constructors

        Plane3D(): Normal(0,1,0) { RecalculateD(Vector3Df(0,0,0)); }

        Plane3D(const Vector3Df& MPoint, const Vector3Df& Normal) : Normal(Normal) { RecalculateD(MPoint); }

        Plane3D(f32 px, f32 py, f32 pz, f32 nx, f32 ny, f32 nz) : Normal(nx, ny, nz) { RecalculateD(Vector3Df(px, py, pz)); }

        Plane3D(f32 nx, f32 ny, f32 nz, f32 d) : Normal(nx, ny, nz), D(d) {}

        Plane3D(const Vector3Df& point1, const Vector3Df& point2, const Vector3Df& point3)
        { 
            SetPlane(point1, point2, point3); 
        }

        Plane3D(const Vector3Df & normal, const f32 d) : Normal(normal), D(d) { }

        // operators

        inline bool operator == (const Plane3D& other) const 
        { 
            return (Equals(D, other.D) && Normal==other.Normal);
        }

        inline bool operator != (const Plane3D& other) const { return !(*this == other);}

        // functions

        void SetPlane(const Vector3Df& point, const Vector3Df& nvector)
        {
            Normal = nvector;
            RecalculateD(point);
        }

        void SetPlane(const Vector3Df& nvect, f32 d)
        {
            Normal = nvect;
            D = d;
        }

        void SetPlane(const Vector3Df& point1, const Vector3Df& point2, const Vector3Df& point3)
        {
            // creates the plane from 3 memberpoints
            Normal = (point2 - point1).CrossProduct(point3 - point1);
            Normal.Normalize();

            RecalculateD(point1);
        }


        //! Get an intersection with a 3d line.
        /** \param lineVect Vector of the line to intersect with.
        \param linePoint Point of the line to intersect with.
        \param outIntersection Place to store the intersection point, if there is one.
        \return True if there was an intersection, false if there was not.
        */
        bool GetIntersectionWithLine(const Vector3Df& linePoint,
            const Vector3Df& lineVect,
            Vector3Df& outIntersection) const
        {
            f32 t2 = Normal.DotProduct(lineVect);

            if (t2 == 0)
                return false;

            f32 t =- (Normal.DotProduct(linePoint) + D) / t2;
            outIntersection = linePoint + (lineVect * t);
            return true;
        }

        //! Get percentage of line between two points where an intersection with this plane happens.
        /** Only useful if known that there is an intersection.
        \param linePoint1 Point1 of the line to intersect with.
        \param linePoint2 Point2 of the line to intersect with.
        \return Where on a line between two points an intersection with this plane happened.
        For example, 0.5 is returned if the intersection happened exactly in the middle of the two points.
        */
        f32 GetKnownIntersectionWithLine(const Vector3Df& linePoint1,
            const Vector3Df& linePoint2) const
        {
            Vector3Df vect = linePoint2 - linePoint1;
            f32 t2 = (f32)Normal.DotProduct(vect);
            return (f32)-((Normal.DotProduct(linePoint1) + D) / t2);
        }

        //! Get an intersection with a 3d line, limited between two 3d points.
        /** \param linePoint1 Point 1 of the line.
        \param linePoint2 Point 2 of the line.
        \param outIntersection Place to store the intersection point, if there is one.
        \return True if there was an intersection, false if there was not.
        */
        bool GetIntersectionWithLimitedLine(
            const Vector3Df& linePoint1,
            const Vector3Df& linePoint2,
            Vector3Df& outIntersection) const
        {
            return (GetIntersectionWithLine(
				linePoint1, 
				linePoint2 - linePoint1, outIntersection) && outIntersection.IsBetweenPoints(linePoint1, linePoint2));
        }

        //! Classifies the relation of a point to this plane.
        /** \param point Point to classify its relation.
        \return ISREL3D_FRONT if the point is in front of the plane,
        ISREL3D_BACK if the point is behind of the plane, and
        ISREL3D_PLANAR if the point is within the plane. */
        EIntersectionRelation3D ClassifyPointRelation(const Vector3Df& point) const
        {
            const f32 d = Normal.DotProduct(point) + D;

            if (d < -ROUNDING_ERROR_32)
                return ISREL3D_BACK;

            if (d > ROUNDING_ERROR_32)
                return ISREL3D_FRONT;

            return ISREL3D_PLANAR;
        }

        //! Recalculates the distance from origin by applying a new member point to the plane.
        void RecalculateD(const Vector3Df& MPoint)
        {
            D = - MPoint.DotProduct(Normal);
        }

        //! Gets a member point of the plane.
        inline Vector3Df GetMemberPoint() const
        {
            return Normal * -D;
        }

        //! Tests if there is an intersection with the other plane
        /** \return True if there is a intersection. */
        bool ExistsIntersection(const Plane3D& other) const
        {
            Vector3Df cross = other.Normal.CrossProduct(Normal);
            return cross.GetLength() > AIR::ROUNDING_ERROR_32;
        }

        //! Intersects this plane with another.
        /** \param other Other plane to intersect with.
        \param outLinePoint Base point of intersection line.
        \param outLineVect Vector of intersection.
        \return True if there is a intersection, false if not. */
        bool GetIntersectionWithPlane(const Plane3D& other,
            Vector3Df& outLinePoint,
            Vector3Df& outLineVect) const
        {
            const f32 fn00 = Normal.GetLength();
            const f32 fn01 = Normal.DotProduct(other.Normal);
            const f32 fn11 = other.Normal.GetLength();
            const f64 det = fn00*fn11 - fn01*fn01;

            if (AIR::Abs(det) < ROUNDING_ERROR_64 )
                return false;

            const f64 invdet = 1.0 / det;
            const f64 fc0 = (fn11*-D + fn01*other.D) * invdet;
            const f64 fc1 = (fn00*-other.D + fn01*D) * invdet;

            outLineVect = Normal.CrossProduct(other.Normal);
            outLinePoint = Normal*(f32)fc0 + other.Normal*(f32)fc1;
            return true;
        }

        //! Get the intersection point with two other planes if there is one.
        bool GetIntersectionWithPlanes(const Plane3D& o1,
            const Plane3D& o2, Vector3Df& outPoint) const
        {
            Vector3Df linePoint, lineVect;
            if (GetIntersectionWithPlane(o1, linePoint, lineVect))
                return o2.GetIntersectionWithLine(linePoint, lineVect, outPoint);

            return false;
        }

        //! Test if the triangle would be front or backfacing from any point.
        /** Thus, this method assumes a camera position from
        which the triangle is definitely visible when looking into
        the given direction.
        Note that this only works if the normal is Normalized.
        Do not use this method with points as it will give wrong results!
        \param lookDirection: Look direction.
        \return True if the plane is front facing and
        false if it is backfacing. */
        bool IsFrontFacing(const Vector3Df& lookDirection) const
        {
            const f32 d = Normal.DotProduct(lookDirection);
            return AIR::F32_LOWER_EQUAL_0 ( d );
        }

        //! Get the distance to a point.
        /** Note that this only works if the normal is normalized. */
        f32 GetDistanceTo(const Vector3Df& point) const
        {
            return point.DotProduct(Normal) + D;
        }

        Plane3D& Normalize()
        {
            const f32 len = AIR::reciprocal_squareroot(Normal.GetLengthSQ());
            Normal *= len;
            D *= len;
            return *this;
        }

        f32 DotNormal(const Vector3Df& normal)
        {
            return normal.DotProduct(Normal);
        }

        //! Normal vector of the plane.
        Vector3Df Normal;
        //! Distance from origin.
        f32 D;
    };

    //! Typedef for a f32 3d plane.
    typedef Plane3D Plane3Df;


    //  PlaneIntersection
    //    computes the point where three planes intersect
    //    returns whether or not the point exists.
    inline bool PlaneIntersection(Vector3Df& pointOut, const Plane3D& p0, const Plane3D& p1, const Plane3D& p2)
    {
        Vector3Df n1_n2, n2_n0, n0_n1;  
        n1_n2 = p1.Normal.CrossProduct(p2.Normal);
        n2_n0 = p2.Normal.CrossProduct(p0.Normal);
        n0_n1 = p0.Normal.CrossProduct(p1.Normal);

        f32 cosTheta = p0.Normal.DotProduct(n1_n2); //D3DXVec3Dot( &n0, &n1_n2 );

        if ( iszero(cosTheta) || IS_SPECIAL(cosTheta) )
            return false;

        float secTheta = 1.f / cosTheta;

        n1_n2 = n1_n2 * p0.D;
        n2_n0 = n2_n0 * p1.D;
        n0_n1 = n0_n1 * p2.D;

        pointOut = -(n1_n2 + n2_n0 + n0_n1) * secTheta;
        return true;
    }
}