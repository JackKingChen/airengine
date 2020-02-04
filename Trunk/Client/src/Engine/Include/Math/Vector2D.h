#pragma once
#include "MyMath.h"
//#include "AIRTypes.h"

namespace AIR
{
    class EngineExport Vector2D
    {
    public:
        inline Vector2D() : x(0), y(0) {}

        //! Constructor with two different values
        inline Vector2D(f32 nx, f32 ny) : x(nx), y(ny) {}
        //! Constructor with the same value for both members
        inline explicit Vector2D(f32 n) : x(n), y(n) {}
        //! Copy constructor
        inline Vector2D(const Vector2D& other) : x(other.x), y(other.y) {}

		inline Vector2D( const Vector2D& start, const Vector2D& end ) : x(end.x - start.x), y(end.y - start.y) {}


        inline Vector2D operator - () const 
        { 
            return Vector2D(-x, -y); 
        }

        inline Vector2D& operator = (const Vector2D& other) 
        { 
            x = other.x; 
            y = other.y; 
            return *this; 
        }

        inline Vector2D operator+(const Vector2D& other) const 
        { 
            return Vector2D(x + other.x, y + other.y); 
        }

        inline Vector2D& operator+=(const Vector2D& other) 
        { 
            x += other.x; 
            y += other.y; 
            return *this; 
        }

        inline Vector2D operator+(const f32 v) const 
        { 
            return Vector2D(x + v, y + v); 
        }

        inline Vector2D& operator += (const f32 v) 
        { 
            x += v; 
            y += v; 
            return *this; 
        }

        inline Vector2D operator - (const Vector2D& other) const 
        { 
            return Vector2D(x - other.x, y - other.y); 
        }

        inline Vector2D& operator -= (const Vector2D& other) 
        { 
            x -= other.x; 
            y -= other.y; 
            return *this; 
        }

        inline Vector2D operator - (const f32 v) const 
        { 
            return Vector2D(x - v, y - v); 
        }

        inline Vector2D& operator -= (const f32 v) 
        { 
            x -= v; 
            y -= v; 
            return *this; 
        }

        inline Vector2D operator * (const Vector2D& other) const 
        { 
            return Vector2D(x * other.x, y * other.y); 
        }

        inline Vector2D& operator *= (const Vector2D& other) 
        { 
            x *= other.x; 
            y *= other.y; 
            return *this; 
        }

        inline Vector2D operator * (const f32 v) const 
        { 
            return Vector2D(x * v, y * v); 
        }
        
        inline Vector2D& operator *= (const f32 v) 
        { 
            x*=v; 
            y*=v; 
            return *this; 
        }

        inline Vector2D operator / (const Vector2D& other) const 
        { 
            return Vector2D(x / other.x, y / other.y); 
        }

        inline Vector2D& operator /= (const Vector2D& other) 
        { 
            x /= other.x; 
            y /= other.y; 
            return *this; 
        }

        inline Vector2D operator / (const f32 v) const 
        { 
            return Vector2D(x / v, y / v); 
        }

        inline Vector2D& operator /= (const f32 v) 
        { 
            x /= v; 
            y /= v; 
            return *this; 
        }

        inline bool operator <= (const Vector2D&other) const 
        { 
            return x <= other.x && y <= other.y; 
        }

        inline bool operator >= (const Vector2D&other) const 
        { 
            return x >= other.x && y >= other.y; 
        }

        inline bool operator < (const Vector2D&other) const 
        { 
            return x < other.x && y < other.y; 
        }

        inline bool operator > (const Vector2D&other) const 
        { 
            return x > other.x && y > other.y; 
        }

        inline bool operator == (const Vector2D& other) const 
        { 
            return Equals(other); 
        }

        inline bool operator != (const Vector2D& other) const 
        { 
            return !Equals(other); 
        }

        //! Checks if this vector equals the other one.
        /** Takes floating point rounding errors into account.
        \param other Vector to compare with.
        \return True if the two vector are (almost) equal, else false. */
        inline bool Equals(const Vector2D& other) const
        {
            return AIR::Equals(x, other.x) && AIR::Equals(y, other.y);
        }

        inline Vector2D& Set(f32 nx, f32 ny) 
        {
            x = nx; 
            y = ny; 
            return *this; 
        }

        inline Vector2D& Set(const Vector2D& p) 
        { 
            x = p.x; 
            y = p.y; 
            return *this; 
        }

        //! Gets the length of the vector.
        /** \return The length of the vector. */
        inline f32 GetLength() const 
        { 
            return (f32)sqrt((f64)(x * x + y * y)); 
        }

        //! Get the squared length of this vector
        /** This is useful because it is much faster than getLength().
        \return The squared length of the vector. */
        inline f32 GetLengthSQ() const 
        { 
            return x * x + y * y; 
        }

        //! Get the dot product of this vector with another.
        /** \param other Other vector to take dot product with.
        \return The dot product of the two vectors. */
        inline f32 DotProduct(const Vector2D& other) const
        {
            return x * other.x + y * other.y;
        }

        //! Gets distance from another point.
        /** Here, the vector is interpreted as a point in 2-dimensional space.
        \param other Other vector to measure from.
        \return Distance from other point. */
        inline f32 GetDistanceFrom(const Vector2D& other) const
        {
            return Vector2D(x - other.x, y - other.y).GetLength();
        }

        //! Returns squared distance from another point.
        /** Here, the vector is interpreted as a point in 2-dimensional space.
        \param other Other vector to measure from.
        \return Squared distance from other point. */
        inline f32 GetDistanceFromSQ(const Vector2D& other) const
        {
            return Vector2D(x - other.x, y - other.y).GetLengthSQ();
        }

        //! rotates the point anticlockwise around a center by an amount of degrees.
        /** \param degrees Amount of degrees to rotate by, anticlockwise.
        \param center Rotation center.
        \return This vector after transformation. */
        inline Vector2D& RotateBy(f64 degrees, const Vector2D& center = Vector2D())
        {
            degrees *= DEGTORAD64;
            const f64 cs = cos(degrees);
            const f64 sn = sin(degrees);

            x -= center.x;
            y -= center.y;

            Set((f32)(x*cs - y*sn), (f32)(x*sn + y*cs));

            x += center.x;
            y += center.y;
            return *this;
        }

        //! Normalize the vector.
        /** The null vector is left untouched.
        \return Reference to this vector, after normalization. */
        inline Vector2D& Normalize()
        {
			//f64 length = (f64)(x*x + y*y);
			//if ( AIR::Equals(length, AIR::ROUNDING_ERROR_64) )
			//	return *this;
			//length = sqrt( length );
			//x /= length;
			//y /= length;
			//return *this;
			f32 length = (f32)sqrt((f32)(x*x + y*y));
			if (length > 1e-08)
			{
				f32 fInvLength = 1.0 / length;
				x *= fInvLength;
				y *= fInvLength;
			}
			return *this;
		}

        //! Calculates the angle of this vector in degrees in the trigonometric sense.
        /** 0 is to the left (9 o'clock), values increase clockwise.
        This method has been suggested by Pr3t3nd3r.
        \return Returns a value between 0 and 360. */
        inline f64 GetAngleTrig() const
        {
            if (x == 0)
                return y < 0 ? 270 : 90;
            else
                if (y == 0)
                    return x < 0 ? 180 : 0;

            if ( y > 0)
                if (x > 0)
                    return atan(y/x) * RADTODEG64;
                else
                    return 180.0 - atan(y / -x) * RADTODEG64;
            else
                if (x > 0)
                    return 360.0 - atan(-y / x) * RADTODEG64;
                else
                    return 180.0+atan(-y / -x) * RADTODEG64;
        }

        //! Calculates the angle of this vector in degrees in the counter trigonometric sense.
        /** 0 is to the right (3 o'clock), values increase counter-clockwise.
        \return Returns a value between 0 and 360. */
        inline f64 GetAngle() const
        {
            if (y == 0) // corrected thanks to a suggestion by Jox
                return x < 0 ? 180 : 0;
            else if (x == 0)
                return y < 0 ? 90 : 270;

            f64 tmp = y / GetLength();
            tmp = atan(sqrt(1 - tmp*tmp) / tmp) * RADTODEG64;

            if (x>0 && y>0)
                return tmp + 270;
            else
                if (x>0 && y<0)
                    return tmp + 90;
                else
                    if (x<0 && y<0)
                        return 90 - tmp;
                    else
                        if (x<0 && y>0)
                            return 270 - tmp;

            return tmp;
        }

        //! Calculates the angle between this vector and another one in degree.
        /** \param b Other vector to test with.
        \return Returns a value between 0 and 90. */
        inline f64 GetAngleWith(const Vector2D& b) const
        {
            f64 tmp = x*b.x + y*b.y;

            if (tmp == 0.0)
                return 90.0;

            tmp = tmp / sqrt((f64)((x*x + y*y) * (b.x*b.x + b.y*b.y)));
            if (tmp < 0.0)
                tmp = -tmp;

            return atan(sqrt(1 - tmp*tmp) / tmp) * RADTODEG64;
        }

        //! Returns if this vector interpreted as a point is on a line between two other points.
        /** It is assumed that the point is on the line.
        \param begin Beginning vector to compare between.
        \param end Ending vector to compare between.
        \return True if this vector is between begin and end, false if not. */
        inline bool IsBetweenPoints(const Vector2D& begin, const Vector2D& end) const
        {
            if (begin.x != end.x)
            {
                return ((begin.x <= x && x <= end.x) ||
                    (begin.x >= x && x >= end.x));
            }
            else
            {
                return ((begin.y <= y && y <= end.y) ||
                    (begin.y >= y && y >= end.y));
            }
        }

        //! Creates an interpolated vector between this vector and another vector.
        /** \param other The other vector to interpolate with.
        \param d Interpolation value between 0.0f (all the other vector) and 1.0f (all this vector).
        Note that this is the opposite direction of interpolation to getInterpolated_quadratic()
        \return An interpolated vector.  This vector is not modified. */
        inline Vector2D GetInterpolated(const Vector2D& other, f64 d) const
        {
            f64 inv = 1.0f - d;
            return Vector2D((f32)(other.x*inv + x*d), (f32)(other.y*inv + y*d));
        }

        //! Creates a quadratically interpolated vector between this and two other vectors.
        /** \param v2 Second vector to interpolate with.
        \param v3 Third vector to interpolate with (maximum at 1.0f)
        \param d Interpolation value between 0.0f (all this vector) and 1.0f (all the 3rd vector).
        Note that this is the opposite direction of interpolation to getInterpolated() and interpolate()
        \return An interpolated vector. This vector is not modified. */
        inline Vector2D GetInterpolated_Quadratic(const Vector2D& v2, const Vector2D& v3, f64 d) const
        {
            // this*(1-d)*(1-d) + 2 * v2 * (1-d) + v3 * d * d;
            const f64 inv = 1.0f - d;
            const f64 mul0 = inv * inv;
            const f64 mul1 = 2.0f * d * inv;
            const f64 mul2 = d * d;

            return Vector2D ( (f32)(x * mul0 + v2.x * mul1 + v3.x * mul2),
                (f32)(y * mul0 + v2.y * mul1 + v3.y * mul2));
        }

        //! Sets this vector to the linearly interpolated vector between a and b.
        /** \param a first vector to interpolate with, maximum at 1.0f
        \param b second vector to interpolate with, maximum at 0.0f
        \param d Interpolation value between 0.0f (all vector b) and 1.0f (all vector a)
        Note that this is the opposite direction of interpolation to getInterpolated_quadratic()
        */
        inline Vector2D& Interpolate(const Vector2D& a, const Vector2D& b, f64 d)
        {
            x = (f32)((f64)b.x + ( ( a.x - b.x ) * d ));
            y = (f32)((f64)b.y + ( ( a.y - b.y ) * d ));
            return *this;
        }


		inline f32 CrossProduct( const Vector2D& other )
		{
			return f32( x * other.y - y * other.x );
		}


	public:

        //! x coordinate of vector.
        f32 x;
        //! y coordinate of vector.
        f32 y;
    };

    typedef Vector2D Vector2Df;

}
