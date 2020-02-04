#pragma once 
#include "MyMath.h"
#include "Vector3D.h"
#include "Line3D.h"
#include "Plane3D.h"
//#include "Matrix4.h"

namespace AIR
{
    class Matrix4; 

    //! Axis aligned bounding box in 3d dimensional space.
    /** Has some useful methods used with occlusion culling or clipping.
    */
    class EngineExport AABBBox3D
    {
    public:

        //! Default Constructor.
        inline AABBBox3D(): MinEdge(-1,-1,-1), MaxEdge(1,1,1), bNull(false) {}
        //! Constructor with min edge and max edge.
        inline AABBBox3D(const Vector3Df& min, const Vector3Df& max): MinEdge(min), MaxEdge(max), bNull(false) {}
        //! Constructor with only one point.
        inline AABBBox3D(const Vector3Df& init): MinEdge(init), MaxEdge(init), bNull(false) {}
        //! Constructor with min edge and max edge as single values, not vectors.
        inline AABBBox3D(f32 minx, f32 miny, f32 minz, f32 maxx, f32 maxy, f32 maxz): MinEdge(minx, miny, minz), MaxEdge(maxx, maxy, maxz), bNull(false) {}

        // operators
        //! Equality operator
        /** \param other box to compare with.
        \return True if both boxes are equal, else false. */
        inline bool operator == (const AABBBox3D& other) const 
        { 
            return (MinEdge == other.MinEdge && other.MaxEdge == MaxEdge);
        }

        //! Inequality operator
        /** \param other box to compare with.
        \return True if both boxes are different, else false. */
        inline bool operator != (const AABBBox3D& other) const 
        { 
            return !(MinEdge == other.MinEdge && other.MaxEdge == MaxEdge);
        }

        // functions

        //! Adds a point to the bounding box
        /** The box grows bigger, if point was outside of the box.
        \param p: Point to add into the box. */
        inline void AddInternalPoint(const Vector3Df& p)
        {
            AddInternalPoint(p.x, p.y, p.z);
        }

        //! Adds another bounding box
        /** The box grows bigger, if the new box was outside of the box.
        \param b: Other bounding box to add into this box. */
        inline void AddInternalBox(const AABBBox3D& b)
        {
            if (b.bNull)
            {
                return;
            }
            if (bNull)
            {
                *this = b;
                
            }
            else
            {
                AddInternalPoint(b.MaxEdge);
                AddInternalPoint(b.MinEdge);
            }
            bNull = false;
        }

        //! Resets the bounding box to a one-point box.
        /** \param x x coord of the point.
        \param y y coord of the point.
        \param z z coord of the point. */
        inline void Reset(f32 x, f32 y, f32 z)
        {
            MaxEdge.Set(x,y,z);
            MinEdge = MaxEdge;
            bNull = false;
        }

        //! Resets the bounding box.
        /** \param initValue New box to set this one to. */
        inline void Reset(const AABBBox3D& initValue)
        {
            *this = initValue;
        }

        //! Resets the bounding box to a one-point box.
        /** \param initValue New point. */
        inline void Reset(const Vector3Df& initValue)
        {
            MaxEdge = initValue;
            MinEdge = initValue;
        }

        inline void Reset(const Vector3Df& minValue, const Vector3Df& maxValue)
        {
            MinEdge = minValue;
            MaxEdge = maxValue;
        }

        inline void SetNull(bool bValue = true)
        {
            bNull = bValue;
            //MinEdge = Vector3Df(999999, 999999, 999999);
            //MaxEdge = Vector3Df(-999999, -999999, -999999);
        }

        //! Adds a point to the bounding box
        /** The box grows bigger, if point is outside of the box.
        \param x x coordinate of the point to add to this box.
        \param y y coordinate of the point to add to this box.
        \param z z coordinate of the point to add to this box. */
        inline void AddInternalPoint(f32 x, f32 y, f32 z)
        {
            if (x>MaxEdge.x) 
                MaxEdge.x = x;
            if (y>MaxEdge.y) 
                MaxEdge.y = y;
            if (z>MaxEdge.z) 
                MaxEdge.z = z;

            if (x<MinEdge.x) 
                MinEdge.x = x;
            if (y<MinEdge.y) 
                MinEdge.y = y;
            if (z<MinEdge.z) 
                MinEdge.z = z;
        }

        //! Determines if a point is within this box.
        /** \param p: Point to check.
        \return True if the point is within the box and false if not */
        inline bool IsPointInside(const Vector3Df& p) const
        {
            return (p.x >= MinEdge.x && p.x <= MaxEdge.x &&
                p.y >= MinEdge.y && p.y <= MaxEdge.y &&
                p.z >= MinEdge.z && p.z <= MaxEdge.z);
        }

        //! Determines if a point is within this box and its borders.
        /** \param p: Point to check.
        \return True if the point is within the box and false if not. */
        inline bool IsPointTotalInside(const Vector3Df& p) const
        {
            return (p.x > MinEdge.x && p.x < MaxEdge.x &&
                p.y > MinEdge.y && p.y < MaxEdge.y &&
                p.z > MinEdge.z && p.z < MaxEdge.z);
        }

        //! Determines if the box intersects with another box.
        /** \param other: Other box to check a intersection with.
        \return True if there is an intersection with the other box,
        otherwise false. */
        inline bool IntersectsWithBox(const AABBBox3D& other) const
        {
            return (MinEdge <= other.MaxEdge && MaxEdge >= other.MinEdge);
        }

        //! Check if this box is completely inside the 'other' box.
        /** \param other: Other box to check against.
        \return True if this box is completly inside the other box,
        otherwise false. */
        inline bool IsFullInside(const AABBBox3D& other) const
        {
            return MinEdge >= other.MinEdge && MaxEdge <= other.MaxEdge;
        }

        //! Tests if the box intersects with a line
        /** \param line: Line to test intersection with.
        \return True if there is an intersection , else false. */
        inline bool IntersectsWithLine(const Line3D& line) const
        {
            return IntersectsWithLine(line.GetMiddle(), line.GetVector().Normalize(),
                (f32)(line.GetLength() * 0.5));
        }

        //! Tests if the box intersects with a line
        /** \param linemiddle Center of the line.
        \param linevect Vector of the line.
        \param halflength Half length of the line.
        \return True if there is an intersection, else false. */
        inline bool IntersectsWithLine(const Vector3Df& linemiddle,
            const Vector3Df& linevect,
            f32 halflength) const
        {
            const Vector3Df e = GetExtent() * (f32)0.5;
            const Vector3Df t = GetCenter() - linemiddle;

            if ((AIR::Abs(t.x) > e.x + halflength * AIR::Abs(linevect.x)) ||
                (AIR::Abs(t.y) > e.y + halflength * AIR::Abs(linevect.y)) ||
                (AIR::Abs(t.z) > e.z + halflength * AIR::Abs(linevect.z)) )
                return false;

            f32 r = e.y * (f32)AIR::Abs(linevect.z) + e.z * (f32)AIR::Abs(linevect.y);
            if (AIR::Abs(t.y*linevect.z - t.z*linevect.y) > r )
                return false;

            r = e.x * (f32)AIR::Abs(linevect.z) + e.z * (f32)AIR::Abs(linevect.x);
            if (AIR::Abs(t.z*linevect.x - t.x*linevect.z) > r )
                return false;

            r = e.x * (f32)AIR::Abs(linevect.y) + e.y * (f32)AIR::Abs(linevect.x);
            if (AIR::Abs(t.x*linevect.y - t.y*linevect.x) > r)
                return false;

            return true;
        }

        //! Classifies a relation with a plane.
        /** \param plane Plane to classify relation to.
        \return Returns ISREL3D_FRONT if the box is in front of the plane,
        ISREL3D_BACK if the box is behind the plane, and
        ISREL3D_CLIPPED if it is on both sides of the plane. */
        EIntersectionRelation3D ClassifyPlaneRelation(const Plane3D& plane) const
        {
            Vector3Df nearPoint(MaxEdge);
            Vector3Df farPoint(MinEdge);

            if (plane.Normal.x > (f32)0)
            {
                nearPoint.x = MinEdge.x;
                farPoint.x = MaxEdge.x;
            }

            if (plane.Normal.y > (f32)0)
            {
                nearPoint.y = MinEdge.y;
                farPoint.y = MaxEdge.y;
            }

            if (plane.Normal.z > (f32)0)
            {
                nearPoint.z = MinEdge.z;
                farPoint.z = MaxEdge.z;
            }

            if (plane.Normal.DotProduct(nearPoint) + plane.D > (f32)0)
                return ISREL3D_FRONT;

            if (plane.Normal.DotProduct(farPoint) + plane.D > (f32)0)
                return ISREL3D_CLIPPED;

            return ISREL3D_BACK;
        }


        //! Get center of the bounding box
        /** \return Center of the bounding box. */
        inline Vector3Df GetCenter() const
        {
            return (MinEdge + MaxEdge) / 2;
        }


        inline Vector3Df GetHalfSize() const
        {
            return (MaxEdge - MinEdge) * 0.5;
        }

        inline f32 GetRadius() const
        {
            return GetHalfSize().GetLength();
        }

        //! Get extent of the box
        /** \return Extent of the bounding box. */
        inline Vector3Df GetExtent() const
        {
            return MaxEdge - MinEdge;
        }


        //! Stores all 8 edges of the box into an array
        /** \param edges: Pointer to array of 8 edges. */
        inline void GetEdges(Vector3Df *edges) const
        {
            const AIR::Vector3Df middle = GetCenter();
            const AIR::Vector3Df diag = middle - MaxEdge;

            /*
            Edges are stored in this way:
            Hey, am I an ascii artist, or what? :) niko.
              /3--------/7
             /  |      / |
            /   |     /  |
            1---------5  |
            |   2- - -| -6
            |  /      |  /
            |/        | /
            0---------4/
            */

            edges[0].Set(middle.x + diag.x, middle.y + diag.y, middle.z + diag.z);
            edges[1].Set(middle.x + diag.x, middle.y - diag.y, middle.z + diag.z);
            edges[2].Set(middle.x + diag.x, middle.y + diag.y, middle.z - diag.z);
            edges[3].Set(middle.x + diag.x, middle.y - diag.y, middle.z - diag.z);
            edges[4].Set(middle.x - diag.x, middle.y + diag.y, middle.z + diag.z);
            edges[5].Set(middle.x - diag.x, middle.y - diag.y, middle.z + diag.z);
            edges[6].Set(middle.x - diag.x, middle.y + diag.y, middle.z - diag.z);
            edges[7].Set(middle.x - diag.x, middle.y - diag.y, middle.z - diag.z);
        }


        //! Check if the box is empty.
        /** This means that there is no space between the min and max
        edge.
        \return True if box is empty, else false. */
        inline bool IsEmpty() const
        {
            return MinEdge.Equals ( MaxEdge );
        }


        //! Repairs the box.
        /** Necessary if for example MinEdge and MaxEdge are swapped. */
        inline void Repair()
        {
            f32 t;

            if (MinEdge.x > MaxEdge.x)
            { t=MinEdge.x; MinEdge.x = MaxEdge.x; MaxEdge.x=t; }
            if (MinEdge.y > MaxEdge.y)
            { t=MinEdge.y; MinEdge.y = MaxEdge.y; MaxEdge.y=t; }
            if (MinEdge.z > MaxEdge.z)
            { t=MinEdge.z; MinEdge.z = MaxEdge.z; MaxEdge.z=t; }
        }

        //! Calculates a new interpolated bounding box.
        /** \param other: other box to interpolate between
        \param d: value between 0.0f and 1.0f.
        \return Interpolated box. */
        inline AABBBox3D GetInterpolated(const AABBBox3D& other, f32 d) const
        {
            f32 inv = 1.0f - d;
            return AABBBox3D((other.MinEdge*inv) + (MinEdge*d),
                (other.MaxEdge*inv) + (MaxEdge*d));
        }

        //! Get the volume enclosed by the box in cubed units
        inline f32 GetVolume() const
        {
            const Vector3Df e = GetExtent();
            return e.x * e.y * e.z;
        }

        //! Get the surface area of the box in squared units
        inline f32 GetArea() const
        {
            const Vector3Df e = GetExtent();
            return 2*(e.x * e.y + e.x * e.z + e.y * e.z);
        }

        bool Intersect(f32& hitDist, const Vector3Df& origPt, const Vector3Df& dir)
        {
            Plane3Df sides[6] = { Plane3Df( 1, 0, 0,-MinEdge.x), Plane3Df(-1, 0, 0, MaxEdge.x),
                Plane3Df( 0, 1, 0,-MinEdge.y), Plane3Df( 0,-1, 0, MaxEdge.y),
                Plane3Df( 0, 0, 1,-MinEdge.z), Plane3Df( 0, 0,-1, MaxEdge.z) };

            hitDist = 0.f;  // safe initial value
            Vector3Df hitPt = origPt;

            bool inside = false;

            for ( int i=0; (i<6) && !inside; i++ )
            {
                float cosTheta = sides[i].DotNormal(dir); //D3DXPlaneDotNormal( &sides[i], dir );
                float dist = sides[i].GetDistanceTo(origPt); //D3DXPlaneDotCoord ( &sides[i], origPt );

                //  if we're nearly intersecting, just punt and call it an intersection
                if ( iszero(dist) ) 
                {
                    return true;
                }
                //  skip nearly (&actually) parallel rays
                if ( iszero(cosTheta) )
                    continue;
                //  only interested in intersections along the ray, not before it.
                hitDist = -dist / cosTheta;
                if (hitDist < 0.f ) 
                    continue;

                hitPt = hitDist * dir + origPt;

                inside = true;

                for ( int j=0; (j < 6) && inside; j++ )
                {
                    if ( j==i )
                        continue;
                    float d = sides[j].GetDistanceTo(hitPt); //D3DXPlaneDotCoord( &sides[j], &hitPt );

                    inside = ((d + 0.00015) >= 0.f);
                }
            }

            return inside;        
        }

        bool IsRayIntersect(const Vector3Df& vRayStart, const Vector3Df& vRayDir, f32& distance)
        {
            float lowt = 0.0f;
            float t;
            bool hit = false;
            Vector3Df hitpoint;
            //Vector3Df min = box.minimum();
            //Vector3Df max = box.maximum();
            //Vector3Df rayorig = ra.start();
            //Vector3Df raydir = ra.direction();

            //先检查在盒子内
            if(IsPointInside(vRayStart))
            {
                distance = 0;
                return true;
            }

            //依次检查各面的相交情况
            if(vRayStart.x < MinEdge.x && vRayDir.x > 0)
            {
                t = (MinEdge.x - vRayStart.x) / vRayDir.x;
                if(t > 0)
                {
                    hitpoint = vRayStart + vRayDir * t;
                    if(hitpoint.y >= MinEdge.y && hitpoint.y <= MaxEdge.y && hitpoint.z >= MinEdge.z && hitpoint.z <= MaxEdge.z && (!hit || t < lowt))
                    {
                        hit = true;
                        lowt = t;
                    }
                }
            }

            if(vRayStart.x > MaxEdge.x && vRayDir.x < 0)
            {
                t = (MaxEdge.x - vRayStart.x) / vRayDir.x;
                if(t>0)
                {
                    hitpoint = vRayStart + vRayDir * t;
                    if(hitpoint.y > MinEdge.y && hitpoint.y <= MaxEdge.y &&
                        hitpoint.z >= MinEdge.z && hitpoint.z <= MaxEdge.z &&
                        (!hit || t < lowt))
                    {
                        hit = true;
                        lowt = t;
                    }
                }
            }

            if(vRayStart.y < MinEdge.y && vRayDir.y > 0)
            {
                t = (MinEdge.y - vRayStart.y) / vRayDir.y;
                if(t>0)
                {
                    hitpoint = vRayStart + vRayDir * t;
                    if (hitpoint.x >= MinEdge.x && hitpoint.x <= MaxEdge.x &&
                        hitpoint.z >= MinEdge.z && hitpoint.z <= MaxEdge.z &&
                        (!hit || t < lowt))
                    {
                        hit = true;
                        lowt = t;
                    }
                }
            }

            if (vRayStart.y > MaxEdge.y && vRayDir.y < 0)
            {
                t = (MaxEdge.y - vRayStart.y) / vRayDir.y;
                if (t > 0)
                {
                    hitpoint = vRayStart + vRayDir * t;
                    if (hitpoint.x >= MinEdge.x && hitpoint.x <= MaxEdge.x &&
                        hitpoint.z >= MinEdge.z && hitpoint.z <= MaxEdge.z &&
                        (!hit || t < lowt))
                    {
                        hit = true;
                        lowt = t;
                    }
                }
            }

            if (vRayStart.z < MinEdge.z && vRayDir.z > 0)
            {
                t = (MinEdge.z - vRayStart.z) / vRayDir.z;
                if (t > 0)
                {
                    hitpoint = vRayStart + vRayDir * t;
                    if (hitpoint.x >= MinEdge.x && hitpoint.x <= MaxEdge.x &&
                        hitpoint.y >= MinEdge.y && hitpoint.y <= MaxEdge.y &&
                        (!hit || t < lowt))
                    {
                        hit = true;
                        lowt = t;
                    }
                }
            }

            if (vRayStart.z > MaxEdge.z && vRayDir.z < 0)
            {
                t = (MaxEdge.z - vRayStart.z) / vRayDir.z;
                if (t > 0)
                {
                    hitpoint = vRayStart + vRayDir * t;
                    if (hitpoint.x >= MinEdge.x && hitpoint.x <= MaxEdge.x &&
                        hitpoint.y >= MinEdge.y && hitpoint.y <= MaxEdge.y &&
                        (!hit || t < lowt))
                    {
                        hit = true;
                        lowt = t;
                    }
                }
            } 

            distance = lowt;
            return hit;

        }



        /** Transforms the box according to the affine matrix supplied.
        @remarks
        By calling this method you get the axis-aligned box which
        surrounds the transformed version of this box. Therefore each
        corner of the box is transformed by the matrix, then the
        extents are mapped back onto the axes to produce another
        AABB. Useful when you have a local AABB for an object which
        is then transformed.
        @note
        The matrix must be an affine matrix. @see Matrix4::isAffine.
        */
		void TransformAffine(const AIR::Matrix4& m);


        inline bool IsNull() const
        {
            return bNull;
        }

		/*
            Edges are stored in this way:
            Hey, am I an ascii artist, or what? :) niko.
              /1--------/0
             /  |      / |
            /   |     /  |
            5---------4  |
            |   3- - -| -2
            |  /      |  /
            |/        | /
            7---------6/
        */
        Vector3Df Point(u32 i) const
        {
            return Vector3Df((i & 1) ? MinEdge.x : MaxEdge.x, (i & 2) ? MinEdge.y : MaxEdge.y, (i & 4) ? MinEdge.z:MaxEdge.z);
        }

        void Intersection(AABBBox3D& outBox, const AABBBox3D& other)
        {
            Vector3Df min = Vector3Df::Maximize(MinEdge, other.MinEdge);
            Vector3Df max = Vector3Df::Minimize(MaxEdge, other.MaxEdge);

            if (max.x > min.x && max.y > min.y && max.z > min.z)
            {
                outBox.MinEdge = min;
                outBox.MaxEdge = max;
            }
        }

        void GetCornerPoints(Vector3Df* pPoints) const
        {
            Vector3Df center = GetCenter();
            Vector3Df vExtents = GetExtent();

            pPoints[0] = Vector3Df(center.x - vExtents.x, center.y + vExtents.y, center.z - vExtents.z);
            pPoints[1] = Vector3Df(center.x + vExtents.x, center.y + vExtents.y, center.z - vExtents.z);
            pPoints[2] = Vector3Df(center.x + vExtents.x, center.y + vExtents.y, center.z + vExtents.z);
            pPoints[3] = Vector3Df(center.x - vExtents.x, center.y + vExtents.y, center.z + vExtents.z);
            pPoints[4] = Vector3Df(center.x - vExtents.x, center.y - vExtents.y, center.z - vExtents.z);
            pPoints[5] = Vector3Df(center.x + vExtents.x, center.y - vExtents.y, center.z - vExtents.z);
            pPoints[6] = Vector3Df(center.x + vExtents.x, center.y - vExtents.y, center.z + vExtents.z);
            pPoints[7] = Vector3Df(center.x - vExtents.x, center.y - vExtents.y, center.z + vExtents.z);
        }

        //! The near edge
        Vector3Df MinEdge;
        //! The far edge
        Vector3Df MaxEdge;

        bool        bNull;   //是否为空
    };

    //! Typedef for a f32 3d bounding box.
    typedef AABBBox3D AABBBox3Df;
}