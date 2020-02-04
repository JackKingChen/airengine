#pragma once 

#include "MyMath.h"
#include "Vector2D.h"
#include "Rect.h"

namespace AIR
{
    //! 2D line between two points with intersection methods.
    class EngineExport Line2D
    {
    public:
        //! Default constructor for line going from (0,0) to (1,1).
        inline Line2D() : start(0,0), end(1,1) {}
        //! Constructor for line between the two points.
        inline Line2D(f32 xa, f32 ya, f32 xb, f32 yb) : start(xa, ya), end(xb, yb) {}
        //! Constructor for line between the two points given as vectors.
        inline Line2D(const Vector2D& start, const Vector2D& end) : start(start), end(end) {}
        //! Copy constructor.
        inline Line2D(const Line2D& other) : start(other.start), end(other.end) {}

        // operators

        inline Line2D operator + (const Vector2D& point) const 
        { 
            return Line2D(start + point, end + point); 
        }

        inline Line2D& operator += (const Vector2D& point) 
        { 
            start += point; 
            end += point; 
            return *this; 
        }

        inline Line2D operator-(const Vector2D& point) const 
        { 
            return Line2D(start - point, end - point); 
        }

        inline Line2D& operator -= (const Vector2D& point) 
        { 
            start -= point; 
            end -= point; 
            return *this; 
        }

        inline bool operator == (const Line2D& other) const
        { 
            return (start==other.start && end == other.end) || (end == other.start && start == other.end);
        }
        inline bool operator!=(const Line2D& other) const
        { 
            return !(start == other.start && end == other.end) || (end == other.start && start == other.end);
        }

        // functions
        //! Set this line to new line going through the two points.
        inline void setLine(const f32& xa, const f32& ya, const f32& xb, const f32& yb){start.Set(xa, ya); end.Set(xb, yb);}
        //! Set this line to new line going through the two points.
        inline void setLine(const Vector2D& nstart, const Vector2D& nend){start.Set(nstart); end.Set(nend);}
        //! Set this line to new line given as parameter.
        inline void setLine(const Line2D& line){start.Set(line.start); end.Set(line.end);}

        //! Get length of line
        /** \return Length of the line. */
        inline f64 getLength() const { return start.GetDistanceFrom(end); }

        //! Get squared length of the line
        /** \return Squared length of line. */
        inline f32 getLengthSQ() const { return start.GetDistanceFromSQ(end); }

        //! Get middle of the line
        /** \return center of the line. */
        inline Vector2D getMiddle() const
        {
            return (start + end) * (f32)0.5;
        }

        //! Get the vector of the line.
        /** \return The vector of the line. */
        inline Vector2D getVector() const { return Vector2D(start.x - end.x, start.y - end.y); }

        //! Tests if this line intersects with another line.
        /** \param l: Other line to test intersection with.
        \param out: If there is an intersection, the location of the
        intersection will be stored in this vector.
        \return True if there is an intersection, false if not. */
        inline bool intersectWith(const Line2D& l, Vector2D& out) const
        {
            // Uses the method given at:
            // http://local.wasp.uwa.edu.au/~pbourke/geometry/lineline2d/ 
            const f32 commonDenominator = (l.end.y - l.start.y)*(end.x - start.x) -
                (l.end.x - l.start.x)*(end.y - start.y);

            const f32 numeratorA = (l.end.x - l.start.x)*(start.y - l.start.y) -
                (l.end.y - l.start.y)*(start.x -l.start.x);

            const f32 numeratorB = (end.x - start.x)*(start.y - l.start.y) -
                (end.y - start.y)*(start.x -l.start.x); 

            if(Equals(commonDenominator, 0.f))
            { 
                // The lines are either coincident or parallel
                if(Equals(numeratorA, 0.f) && Equals(numeratorB, 0.f))
                {
                    // Try and find a common endpoint
                    if(l.start == start || l.end == start)
                        out = start;
                    else if(l.end == end || l.start == end)
                        out = end;
                    else
                        // one line is contained in the other, so for lack of a better
                        // answer, pick the average of both lines
                        out = ((start + end + l.start + l.end) * 0.25f);

                    return true; // coincident
                }

                return false; // parallel
            }

            // Get the point of intersection on this line, checking that
            // it is within the line segment.
            const f32 uA = numeratorA / commonDenominator;
            if(uA < 0.f || uA > 1.f)
                return false; // Outside the line segment

            const f32 uB = numeratorB / commonDenominator;
            if(uB < 0.f || uB > 1.f)
                return false; // Outside the line segment

            // Calculate the intersection point.
            out.x = start.x + uA * (end.x - start.x);
            out.y = start.y + uA * (end.y - start.y);
            return true; 
        }

        //! Get unit vector of the line.
        /** \return Unit vector of this line. */
        inline Vector2D GetUnitVector() const
        {
            f32 len = (f32)(1.0 / getLength());
            return Vector2D((end.x - start.x) * len, (end.y - start.y) * len);
        }

        //! Get angle between this line and given line.
        /** \param l Other line for test.
        \return Angle in degrees. */
        inline f64 GetAngleWith(const Line2D& l) const
        {
            Vector2D vect = getVector();
            Vector2D vect2 = l.getVector();
            return vect.GetAngleWith(vect2);
        }

        //! Tells us if the given point lies to the left, right, or on the line.
        /** \return 0 if the point is on the line
        <0 if to the left, or >0 if to the right. */
        inline f32 GetPointOrientation(const Vector2D& point) const
        {
            return ( (end.x - start.x) * (point.y - start.y) -
                (point.x - start.x) * (end.y - start.y) );
        }

        //! Check if the given point is a member of the line
        /** \return True if point is between start and end, else false. */
        inline bool IsPointOnLine(const Vector2D& point) const
        {
            f32 d = GetPointOrientation(point);
            return (d == 0 && point.IsBetweenPoints(start, end));
        }

        //! Check if the given point is between start and end of the line.
        /** Assumes that the point is already somewhere on the line. */
        inline bool IsPointBetweenStartAndEnd(const Vector2D& point) const
        {
            return point.IsBetweenPoints(start, end);
        }

        //! Get the closest point on this line to a point
        inline Vector2D GetClosestPoint(const Vector2D& point) const
        {
            Vector2D c = point - start;
            Vector2D v = end - start;
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

        inline f32 GetDistanceFromPoint(const Vector2D& point)
        {
            Vector2Df closestPoint = GetClosestPoint(point);
            return (point - closestPoint).GetLength();
        }


		//! 获得点与线的关系
		// 线是顺时钟
		// 点在线左边返回 -1
		// 点在线的右边返回 1,
		// 点在线上返回0
		inline int RelationShipOfLineAndPoint( const Vector2D& point )
		{
			Vector2D vL(start, end);
			Vector2D vP(start, point);

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


		inline const Vector2D& GetLineStart( void ) const
		{
			return start;
		}

		inline const Vector2D& GetLineEnd( void ) const
		{
			return end;
		}

		inline bool IsIntersect( const Line2D& other )
		{
			Rect re1( start, end );
			Rect re2( other.start, other.end );
			re1.Normalize();
			re2.Normalize();

			if( !re1.IsCross(re2) )
			{
				return false;
			}

			// 相互跨立
			Vector2D ve1 (start, end);
			Vector2D ve11(start, other.start);
			Vector2D ve12(start, other.end);
			double cro1 = ve1.CrossProduct(ve12) * ve1.CrossProduct(ve11);

			Vector2D ve2 (other.start, other.end);
			Vector2D ve21(other.start, start);
			Vector2D ve22(other.start, end );
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

		inline bool IsEqual( const Line2D& other )
		{
			if ( start.Equals(other.start) && end.Equals(other.end) )
			{
				return true;
			}
			return false;
		}

		inline bool IsEqualNotDetectDirection( const Line2D& other )
		{
			if ( start.Equals(other.start) && end.Equals(other.end) ||
				start.Equals(other.end) && end.Equals(other.start) )
			{
				return true;
			}
			return false;
		}



	public:

        //! Start point of the line.
        Vector2D start;
        //! End point of the line.
        Vector2D end;
    };

    //! Typedef for an f32 line.
    typedef Line2D Line2Df;

}