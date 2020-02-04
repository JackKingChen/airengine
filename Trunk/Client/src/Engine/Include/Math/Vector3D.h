#pragma once 
#include "AIRTypes.h"
#include "MyMath.h"

namespace AIR
{
    class EngineExport Vector3D
    {
    public:
        Vector3D() : x(0), y(0), z(0) {}

        Vector3D(f32 x, f32 y, f32 z) : x(x), y(y), z(z) {}

        explicit Vector3D(f32 n) : x(n), y(n), z(n) {}
        //! Copy constructor
        Vector3D(const Vector3D& other) : x(other.x), y(other.y), z(other.z) {}

		Vector3D(const Vector3D& start, const Vector3D& end) : x(end.x - start.x), y(end.y - start.y), z(end.z - start.z) {}

        // operators

        inline Vector3D operator - () const 
        { 
            return Vector3D(-x, -y, -z); 
        }

        inline Vector3D& operator = (const Vector3D& other) 
        { 
            x = other.x; 
            y = other.y; 
            z = other.z; 
            return *this; 
        }

        inline Vector3D operator+(const Vector3D& other) const { return Vector3D(x + other.x, y + other.y, z + other.z); }
        inline Vector3D& operator+=(const Vector3D& other) { x+=other.x; y+=other.y; z+=other.z; return *this; }
        inline Vector3D operator+(const f32 val) const { return Vector3D(x + val, y + val, z + val); }
        inline Vector3D& operator+=(const f32 val) { x+=val; y+=val; z+=val; return *this; }

        inline Vector3D operator-(const Vector3D& other) const { return Vector3D(x - other.x, y - other.y, z - other.z); }
        inline Vector3D& operator-=(const Vector3D& other) { x-=other.x; y-=other.y; z-=other.z; return *this; }
        inline Vector3D operator-(const f32 val) const { return Vector3D(x - val, y - val, z - val); }
        inline Vector3D& operator-=(const f32 val) { x-=val; y-=val; z-=val; return *this; }

        inline Vector3D operator*(const Vector3D& other) const { return Vector3D(x * other.x, y * other.y, z * other.z); }
        inline Vector3D& operator*=(const Vector3D& other) { x*=other.x; y*=other.y; z*=other.z; return *this; }
        inline Vector3D operator*(const f32 v) const { return Vector3D(x * v, y * v, z * v); }
        inline Vector3D& operator*=(const f32 v) { x*=v; y*=v; z*=v; return *this; }

        friend inline Vector3D operator * (const f32 v, const Vector3D& other)
        {
            return Vector3D(
                v * other.x,
                v * other.y,
                v * other.z);
        }

        friend inline Vector3D operator / (const f32 v, const Vector3D& other)
        {
            return Vector3D(
                v / other.x,
                v / other.y,
                v / other.z);
        }

        friend inline Vector3D operator + (const f32 v, const Vector3D& other)
        {
            return Vector3D(
                v + other.x,
                v + other.y,
                v + other.z);
        }

        friend inline Vector3D operator - (const f32 v, const Vector3D& other)
        {
            return Vector3D(
                v - other.x,
                v - other.y,
                v - other.z);
        }

        inline Vector3D operator/(const Vector3D& other) const { return Vector3D(x / other.x, y / other.y, z / other.z); }
        inline Vector3D& operator/=(const Vector3D& other) { x/=other.x; y/=other.y; z/=other.z; return *this; }
        inline Vector3D operator/(const f32 v) const { f32 i=(f32)1.0/v; return Vector3D(x * i, y * i, z * i); }
        inline Vector3D& operator/=(const f32 v) { f32 i=(f32)1.0/v; x*=i; y*=i; z*=i; return *this; }

        inline bool operator<=(const Vector3D&other) const { return x<=other.x && y<=other.y && z<=other.z;}
        inline bool operator>=(const Vector3D&other) const { return x>=other.x && y>=other.y && z>=other.z;}
        inline bool operator<(const Vector3D&other) const { return x<other.x && y<other.y && z<other.z;}
        inline bool operator>(const Vector3D&other) const { return x>other.x && y>other.y && z>other.z;}

        //! use weak float compare
        inline bool operator == (const Vector3D& other) const
        {
            return this->Equals(other);
        }

        inline bool operator != (const Vector3D& other) const
        {
            return !this->Equals(other);
        }

        inline f32 operator [](s32 index)
        {
            switch(index)
            {
            case 0:
                return x;
            case 1:
                return y;
            case 2:
                return z;
            }

            return x;
        }

        // functions

        //! returns if this vector equals the other one, taking floating point rounding errors into account
        inline bool Equals(const Vector3D& other, const f32 tolerance = (f32)ROUNDING_ERROR_32 ) const
        {
            return AIR::Equals(x, other.x, tolerance) &&
                AIR::Equals(y, other.y, tolerance) &&
                AIR::Equals(z, other.z, tolerance);
        }

        inline Vector3D& Set(const f32 nx, const f32 ny, const f32 nz) 
        {
            x=nx; 
            y=ny; 
            z=nz; 
            return *this;
        }

        inline Vector3D& Set(const Vector3D& p) 
        {
            x = p.x; 
            y = p.y; 
            z = p.z;
            return *this;
        }

        //! Get length of the vector.
        inline f32 GetLength() const 
        { 
            return sqrtf( (x*x + y*y + z*z) ); 
        }

        //! Get squared length of the vector.
        /** This is useful because it is much faster than getLength().
        \return Squared length of the vector. */
        inline f32 GetLengthSQ() const { return x*x + y*y + z*z; }

        //! Get the dot product with another vector.
        inline f32 DotProduct(const Vector3D& other) const
        {
            return x*other.x + y*other.y + z*other.z;
        }

		inline bool IsPerpendicular(const Vector3D&other, f32 epsilon) const
		{
			return (x*other.x + y*other.y + z*other.z )<= epsilon;
		}

        //! Get distance from another point.
        /** Here, the vector is interpreted as point in 3 dimensional space. */
        inline f32 GetDistanceFrom(const Vector3D& other) const
        {
            return Vector3D(x - other.x, y - other.y, z - other.z).GetLength();
        }

        //! Returns squared distance from another point.
        /** Here, the vector is interpreted as point in 3 dimensional space. */
        inline f32 GetDistanceFromSQ(const Vector3D& other) const
        {
            return Vector3D(x - other.x, y - other.y, z - other.z).GetLengthSQ();
        }

        //! Calculates the cross product with another vector.
        /** \param p Vector to multiply with.
        \return Crossproduct of this vector with p. */
        inline Vector3D CrossProduct(const Vector3D& p) const
        {
            return Vector3D(y * p.z - z * p.y, z * p.x - x * p.z, x * p.y - y * p.x);
        }

        //! Normalizes the vector.
        /** In case of the 0 vector the result is still 0, otherwise
        the length of the vector will be 1.
        \return Reference to this vector after normalization. */
        inline Vector3D& Normalize()
        {
            //f32 length = (f32)(x*x + y*y + z*z);
            //if (AIR::Equals(length, 0.f))
            //    return *this;
            f32 length = (f32)sqrt((f32)(x*x + y*y + z*z)); //AIR::reciprocal_squareroot ( (f32)(x*x + y*y + z*z)/*(f32)length*/ );
            if (length > 1e-08)
            {
                f32 fInvLength = 1.0 / length;
                x *= fInvLength;
                y *= fInvLength;
                z *= fInvLength;
            }
            //x = (f32)(x * length);
            //y = (f32)(y * length);
            //z = (f32)(z * length);
            return *this;
        }

        //! Sets the length of the vector to a new value
        inline Vector3D& SetLength(f32 newlength)
        {
            Normalize();
            return (*this *= newlength);
        }

        //! Inverts the vector.
        inline Vector3D& Invert()
        {
            x *= -1.0f;
            y *= -1.0f;
            z *= -1.0f;
            return *this;
        }

        //! Rotates the vector by a specified number of degrees around the y axis and the specified center.
        /** \param degrees Number of degrees to rotate around the y axis.
        \param center The center of the rotation. */
        inline void RotateXZBy(f64 degrees, const Vector3D& center = Vector3D())
        {
            degrees *= DEGTORAD64;
            f64 cs = cos(degrees);
            f64 sn = sin(degrees);
            x -= center.x;
            z -= center.z;
            Set((f32)(x*cs - z*sn), y, (f32)(x*sn + z*cs));
            x += center.x;
            z += center.z;
        }

        //! Rotates the vector by a specified number of degrees around the z axis and the specified center.
        /** \param degrees: Number of degrees to rotate around the z axis.
        \param center: The center of the rotation. */
        inline void RotateXYBy(f64 degrees, const Vector3D& center = Vector3D())
        {
            degrees *= DEGTORAD64;
            f64 cs = cos(degrees);
            f64 sn = sin(degrees);
            x -= center.x;
            y -= center.y;
            Set((f32)(x*cs - y*sn), (f32)(x*sn + y*cs), z);
            x += center.x;
            y += center.y;
        }

        //! Rotates the vector by a specified number of degrees around the x axis and the specified center.
        /** \param degrees: Number of degrees to rotate around the x axis.
        \param center: The center of the rotation. */
        inline void RotateYZBy(f64 degrees, const Vector3D& center = Vector3D())
        {
            degrees *= DEGTORAD64;
            f64 cs = cos(degrees);
            f64 sn = sin(degrees);
            z -= center.z;
            y -= center.y;
            Set(x, (f32)(y * cs - z * sn), (f32)(y * sn + z * cs));
            z += center.z;
            y += center.y;
        }

        //! Creates an interpolated vector between this vector and another vector.
        /** \param other The other vector to interpolate with.
        \param d Interpolation value between 0.0f (all the other vector) and 1.0f (all this vector).
        Note that this is the opposite direction of interpolation to getInterpolated_quadratic()
        \return An interpolated vector.  This vector is not modified. */
        inline Vector3D GetInterpolated(const Vector3D& other, f64 d) const
        {
            const f64 inv = 1.0 - d;
            return Vector3D((f32)(other.x * inv + x * d), (f32)(other.y * inv + y * d), (f32)(other.z * inv + z * d));
        }

        //! Creates a quadratically interpolated vector between this and two other vectors.
        /** \param v2 Second vector to interpolate with.
        \param v3 Third vector to interpolate with (maximum at 1.0f)
        \param d Interpolation value between 0.0f (all this vector) and 1.0f (all the 3rd vector).
        Note that this is the opposite direction of interpolation to getInterpolated() and interpolate()
        \return An interpolated vector. This vector is not modified. */
        inline Vector3D GetInterpolatedQuadratic(const Vector3D& v2, const Vector3D& v3, f64 d) const
        {
            // this*(1-d)*(1-d) + 2 * v2 * (1-d) + v3 * d * d;
            const f64 inv = (f32) 1.0 - d;
            const f64 mul0 = inv * inv;
            const f64 mul1 = (f32) 2.0 * d * inv;
            const f64 mul2 = d * d;

            return Vector3D ((f32)(x * mul0 + v2.x * mul1 + v3.x * mul2),
                (f32)(y * mul0 + v2.y * mul1 + v3.y * mul2),
                (f32)(z * mul0 + v2.z * mul1 + v3.z * mul2));
        }

        //! Sets this vector to the linearly interpolated vector between a and b.
        /** \param a first vector to interpolate with, maximum at 1.0f
        \param b second vector to interpolate with, maximum at 0.0f
        \param d Interpolation value between 0.0f (all vector b) and 1.0f (all vector a)
        Note that this is the opposite direction of interpolation to getInterpolated_quadratic()
        */
        inline Vector3D& Interpolate(const Vector3D& a, const Vector3D& b, f64 d)
        {
            x = (f32)((f64)b.x + ( ( a.x - b.x ) * d ));
            y = (f32)((f64)b.y + ( ( a.y - b.y ) * d ));
            z = (f32)((f64)b.z + ( ( a.z - b.z ) * d ));
            return *this;
        }

        inline bool IsZeroLength(void) const
        {
            f32 sqlen = (x * x) + (y * y) + (z * z);
            return (sqlen < (1e-06 * 1e-06));

        }

        inline Vector3D GetMidPoint(const Vector3D& vec) const
        {
            return Vector3D
                (
                (x + vec.x) / 2,
                (y + vec.y) / 2,
                (z + vec.z) / 2
                );
        }


		//! Returns if this vector interpreted as a point is on a line between two other points.
		/** It is assumed that the point is on the line.
		\param begin Beginning vector to compare between.
		\param end Ending vector to compare between.
		\return True if this vector is between begin and end, false if not. */
		inline bool IsBetweenPoints(const Vector3D& begin, const Vector3D& end) const
		{
			const f32 f = (end - begin).GetLengthSQ();
			return GetDistanceFromSQ(begin) <= f &&	GetDistanceFromSQ(end) <= f;
		}


        inline bool IsSphereIn(const Vector3D& vPos, f32 radius) const
        {
            return (*this - vPos).GetLength() <= radius;
        }

        //产生垂直于该向量的向量
        inline Vector3D Perpendicular() const
        {
            static const float fSquareZero = (float)(1e-06 * 1e-06);

			// 计算与X轴的叉积
            Vector3D perp = this->CrossProduct( Vector3D(1, 0, 0) );

            // 叉积的长度平方是否接近0
            if( perp.GetLengthSQ() < fSquareZero )
            {
                /* This vector is the Y axis multiplied by a scalar, so we have
                to use another axis.
                */
				// 该向量很接近平行于X轴,计算该向量与Y轴的叉积
                perp = this->CrossProduct( Vector3D(0, 1, 0) );
            }
            perp.Normalize();

            return perp;
        }

		/// Makes the given vectors linearly independent.
		/** This function directly follows the Gram-Schmidt procedure on the input vectors.
		The vector a is kept unmodified, and vector b is modified to be perpendicular to a.
		Finally, if specified, the vector c is adjusted to be perpendicular to a and b.
		@note If any of the input vectors is zero, then the resulting set of vectors cannot be made orthogonal.
		@see AreOrthogonal(), Orthonormalize(), AreOrthonormal(). */

		static inline Vector3D Orthonormalize(Vector3D  &a, Vector3D  &b, Vector3D  &c)
		{
			//assert(!a.IsZeroLength());
			//a.Normalize();
			
			b -= b.ProjectToNorm(a);
			//assert(!b.IsZeroLength());
			//b.Normalize();
			c -= c.ProjectToNorm(a);
			c -= c.ProjectToNorm(b);
			//assert(!c.IsZeroLength());
			c.Normalize();

			return c;
		}

		/// Projects this vector onto the given normalized direction vector.
		/** @param direction The vector to project onto. This vector must be normalized.
		@see ProjectTo(). */
		Vector3D ProjectToNorm(Vector3D &direction) const
		{
			direction.Normalize();
			return direction *  this->DotProduct(direction);
		}

        /** Sets this vector's components to the minimum of its own and the
        ones of the passed in vector.
        @remarks
        'Minimum' in this case means the combination of the lowest
        value of x, y and z from both vectors. Lowest is taken just
        numerically, not magnitude, so -1 < 0.
        */
        inline void MakeFloor(const Vector3D& cmp)
        {
            if( cmp.x < x ) x = cmp.x;
            if( cmp.y < y ) y = cmp.y;
            if( cmp.z < z ) z = cmp.z;
        }

        /** Sets this vector's components to the maximum of its own and the
        ones of the passed in vector.
        @remarks
        'Maximum' in this case means the combination of the highest
        value of x, y and z from both vectors. Highest is taken just
        numerically, not magnitude, so 1 > -3.
        */
        inline void MakeCeil( const Vector3D& cmp )
        {
            if( cmp.x > x ) x = cmp.x;
            if( cmp.y > y ) y = cmp.y;
            if( cmp.z > z ) z = cmp.z;
        }


        inline f64 GetDegreeAngleWith(const Vector3D& b) const
        {
            f64 tmp = DotProduct(b);

            if (tmp == 0.0)
                return 90.0;

            tmp = tmp / sqrt((f64)(GetLengthSQ() * b.GetLengthSQ()));
            if (tmp < 0.0)
                tmp = -tmp;

            return atan(sqrt(1 - tmp*tmp) / tmp) * RADTODEG64;
        }

        inline f64 GetRadianAngleWith(const Vector3D& b) const
        {
            //f64 tmp = DotProduct(b);
            //if (tmp == 0.0)
            //    return 90.0f;
            //tmp = tmp / sqrt((f64)(GetLengthSQ() * b.GetLengthSQ()));
            //if (tmp < 0.0)
            //    tmp = -tmp;
            //return atan(sqrt(1 - tmp*tmp) / tmp);

			////f64 tmp = DotProduct(b);
			////if (iszero(tmp))
			////	return HALF_PI;
			////tmp = tmp / sqrt((f64)(GetLengthSQ() * b.GetLengthSQ()));
			////f64 angle = atan(sqrt(1 - tmp*tmp) / tmp);
			////if ( tmp < 0.0f )
			////{
			////	angle = PI+angle;
			////}
			////return angle;

			//--------------------------------------------------
			//! modify by hjj 2010-7-5
			f64 lenProduct = GetLength() * b.GetLength();

			// Divide by zero check
			if(lenProduct < 1e-6f)
				lenProduct = 1e-6f;

			f64 f = DotProduct(b) / lenProduct;

			f = AIR::Clamp(f, (f64)-1.0, (f64)1.0);
			return AIR::asm_arccos(f);
			//--------------------------------------------------
        }

        static Vector3D Maximize(const Vector3D& v1, const Vector3D& v2)
        {
            return Vector3D(Max(v1.x, v2.x), Max(v1.y, v2.y), Max(v1.z, v2.z));
        }

        static Vector3D Minimize(const Vector3D& v1, const Vector3D& v2)
        {
            return Vector3D(Min(v1.x, v2.x), Min(v1.y, v2.y), Min(v1.z, v2.z));
        }

		f32 GetElement(int index) const
		{
			switch (index)
			{
			case 0:
				return x;
			case 1:
				return y;
			case 2:
				return z;
			}
			return 0;
		}

	public:
        //! x coordinate of the vector
        f32 x;
        //! y coordinate of the vector
		f32 y;
        //! z coordinate of the vector
		f32 z;

    public:
        static const Vector3D ZERO;
        static const Vector3D UNIT_X;
        static const Vector3D UNIT_Y;
        static const Vector3D UNIT_Z;
        static const Vector3D NEGATIVE_UNIT_X;
        static const Vector3D NEGATIVE_UNIT_Y;
        static const Vector3D NEGATIVE_UNIT_Z;
        static const Vector3D UNIT_SCALE;

    };

    //template class EngineExport Vector3D<s32>;
    //template class EngineExport Vector3D<f32>;

    //typedef Vector3D<int>	Vector3Di;
    typedef Vector3D Vector3Df;
    
    //const Vector3Df Vector3Df::ZERO(0, 0, 0);
    //const Vector3Df Vector3Df::UNIT_X(1, 0, 0);
    //const Vector3Df Vector3Df::UNIT_Y(0, 1, 0);
    //const Vector3Df Vector3Df::UNIT_Z(0, 0, 1);
    //const Vector3Df Vector3Df::NEGATIVE_UNIT_X(-1, 0, 0);
    //const Vector3Df Vector3Df::NEGATIVE_UNIT_Y(0, -1, 0);
    //const Vector3Df Vector3Df::NEGATIVE_UNIT_Z(0, 0, -1);
    //const Vector3Df Vector3Df::UNIT_SCALE(1, 1, 1);

    //const Vector3Di Vector3Di::ZERO(0, 0, 0);
    //const Vector3Di Vector3Di::UNIT_X(1, 0, 0);
    //const Vector3Di Vector3Di::UNIT_Y(0, 1, 0);
    //const Vector3Di Vector3Di::UNIT_Z(0, 0, 1);
    //const Vector3Di Vector3Di::NEGATIVE_UNIT_X(-1, 0, 0);
    //const Vector3Di Vector3Di::NEGATIVE_UNIT_Y(0, -1, 0);
    //const Vector3Di Vector3Di::NEGATIVE_UNIT_Z(0, 0, -1);
    //const Vector3Di Vector3Di::UNIT_SCALE(1, 1, 1);

	

	//------------------------------------------------------------------------------

	inline f32 DotProduct(const Vector3D &v1, const Vector3D &v2)
	{
		return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
	}

	//------------------------------------------------------------------------------
	inline Vector3D CrossProduct(const Vector3D& p1, const Vector3D& p2)
	{
		return Vector3D(
			p1.y * p2.z - p1.z * p2.y, 
			p1.z * p2.x - p1.x * p2.z, 
			p1.x * p2.y - p1.y * p2.x);
	}

	//------------------------------------------------------------------------------
	inline Vector3D Normalize(const Vector3D& p1)
	{
		Vector3D v = p1;
		v.Normalize();
		return v;
	}

};


