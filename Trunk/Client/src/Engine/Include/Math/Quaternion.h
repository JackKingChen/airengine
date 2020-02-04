#pragma once 

#include "MyMath.h"
#include "Matrix4.h"
#include "Vector3D.h"


#ifndef IN
#define IN
#endif


#ifndef OUT
#define OUT
#endif


namespace AIR
{
    //! Quaternion class for representing rotations.
    /** It provides cheap combinations and avoids gimbal locks.
    Also useful for interpolations. */
    class EngineExport Quaternion
    {
    public:

        //! Default Constructor
        inline Quaternion() : x(0.0f), y(0.0f), z(0.0f), w(1.0f) {}

        //! Constructor
        inline Quaternion(f32 fx, f32 fy, f32 fz, f32 fw) : x(fx), y(fy), z(fz), w(fw) { }

        //! Constructor which converts euler angles (radians) to a Quaternion
        inline Quaternion(f32 fx, f32 fy, f32 fz)
        {
            Set(fx,fy,fz);
        }

        //! Constructor which converts euler angles (radians) to a Quaternion
        inline Quaternion(const Vector3Df& vec)
        {
            Set(vec.x,vec.y,vec.z);
        };


		// 取方向向量
		inline  Vector3Df  v() const
		{
			return Vector3Df(this->x, this->y, this->z); 
		}


        //! Constructor which converts a matrix to a Quaternion
        inline Quaternion(const Matrix4f& mat)
        {
            (*this) = mat;
        }

        //! Equalilty operator
        inline bool operator == (const Quaternion& other) const
        {
            return ((x == other.x) &&
                (y == other.y) &&
                (z == other.z) &&
                (w == other.w));
        }

		inline bool operator != (const Quaternion& other) const
		{
			return ( x !=other.x || y != other.y || z != other.z || w != other.w );
		}

        //! Assignment operator
        inline Quaternion& operator = (const Quaternion& other)
        {
            x = other.x;
            y = other.y;
            z = other.z;
            w = other.w;
            return *this;
        }

		inline Quaternion operator - () const
		{
			return Quaternion(-x,-y,-z,-w);
		}

		inline Quaternion operator - (const Quaternion& rkQ) const
		{
			return Quaternion(x-rkQ.x, y-rkQ.y, z-rkQ.z, w-rkQ.w);
		}

        //! Matrix assignment operator
        inline Quaternion& operator = (const Matrix4f& other)
        {
            const f32 diag = other(0, 0) + other(1, 1) + other(2, 2) + 1;

            if( diag > 0.0f )
            {
                const f32 scale = sqrtf(diag) * 2.0f; // get scale from diagonal

                // TODO: speed this up
                x = ( other(2, 1) - other(1, 2)) / scale;
                y = ( other(0, 2) - other(2, 0)) / scale;
                z = ( other(1, 0) - other(0, 1)) / scale;
                w = 0.25f * scale;
            }
            else
            {
                if ( other(0, 0) > other(1, 1) && other(0, 0) > other(2, 2))
                {
                    // 1st element of diag is greatest value
                    // find scale according to 1st element, and double it
                    const f32 scale = sqrtf( 1.0f + other(0, 0) - other(1, 1) - other(2, 2)) * 2.0f;

                    // TODO: speed this up
                    x = 0.25f * scale;
                    y = (other(0, 1) + other(1, 0)) / scale;
                    z = (other(2, 0) + other(0, 2)) / scale;
                    w = (other(2, 1) - other(1, 2)) / scale;
                }
                else if ( other(1, 1) > other(2, 2))
                {
                    // 2nd element of diag is greatest value
                    // find scale according to 2nd element, and double it
                    const f32 scale = sqrtf( 1.0f + other(1, 1) - other(0, 0) - other(2, 2)) * 2.0f;

                    // TODO: speed this up
                    x = (other(0, 1) + other(1, 0) ) / scale;
                    y = 0.25f * scale;
                    z = (other(1, 2) + other(2, 1) ) / scale;
                    w = (other(0, 2) - other(2, 0) ) / scale;
                }
                else
                {
                    // 3rd element of diag is greatest value
                    // find scale according to 3rd element, and double it
                    const f32 scale = sqrtf( 1.0f + other(2, 2) - other(0, 0) - other(1, 1)) * 2.0f;

                    // TODO: speed this up
                    x = (other(0, 2) + other(2, 0)) / scale;
                    y = (other(1, 2) + other(2, 1)) / scale;
                    z = 0.25f * scale;
                    w = (other(1, 0) - other(0, 1)) / scale;
                }
            }

            return Normalize();
        }

        //! Add operator
        inline Quaternion operator + (const Quaternion& other) const
        {
            return Quaternion(x + other.x, y + other.y, z + other.z, w + other.w);
        }

        //! Multiplication operator
        inline Quaternion operator * (const Quaternion& other) const
        {
            //ogre 计算方式
            return Quaternion
                (
                w * other.x + x * other.w + y * other.z - z * other.y,
                w * other.y + y * other.w + z * other.x - x * other.z,
                w * other.z + z * other.w + x * other.y - y * other.x,
                w * other.w - x * other.x - y * other.y - z * other.z
                );
            
            Quaternion tmp(w * other.x + x * other.w + z * other.y - y * other.z,
                w * other.y + y * other.w + x * other.z - z * other.x,
                w * other.z + z * other.w + y * other.x - x * other.y,
                w * other.w - x * other.x - y * other.y - z * other.z
                );
                

            //tmp.w = (other.w * w) - (other.x * x) - (other.y * y) - (other.z * z);
            //tmp.x = (other.w * x) + (other.x * w) + (other.y * z) - (other.z * y);
            //tmp.y = (other.w * y) + (other.y * w) + (other.z * x) - (other.x * z);
            //tmp.z = (other.w * z) + (other.z * w) + (other.x * y) - (other.y * x);

            return tmp;
        }

        //! Multiplication operator with scalar
        inline Quaternion operator * (f32 s) const
        {
            return Quaternion(s*x, s*y, s*z, s*w);
        }

        //! Multiplication operator with scalar
        inline Quaternion& operator *= (f32 s)
        {
            x*=s;
            y*=s;
            z*=s;
            w*=s;
            return *this;
        }

        //! Multiplication operator
        inline Vector3Df operator * (const Vector3Df& v) const
        {
            // nVidia SDK implementation

            Vector3Df uv, uuv;
            Vector3Df qvec(x, y, z);
            //uv = v.CrossProduct(qvec);
            //uuv = uv.CrossProduct(qvec);
            uv = qvec.CrossProduct(v);
            uuv = qvec.CrossProduct(uv);
            uv *= (2.0f * w);
            uuv *= 2.0f;

            return v + uv + uuv;
        }

		//! add by hjj 2010-5-21
		friend inline Quaternion operator* (float fScalar,const Quaternion& rkQ)
		{
			return Quaternion( rkQ.x * fScalar, rkQ.y * fScalar, rkQ.z * fScalar, rkQ.w * fScalar );
		}

        //! Multiplication operator
        inline Quaternion& operator *= (const Quaternion& other)
        {
            return (*this = (*this) * other);
        }

        //! Calculates the dot product
        inline f32 DotProduct(const Quaternion& other) const
        {
            return (x * other.x) + (y * other.y) + (z * other.z) + (w * other.w);
        }

        //! Sets new Quaternion
        inline Quaternion& Set(f32 fx, f32 fy, f32 fz, f32 fw)
        {
            x = fx;
            y = fy;
            z = fz;
            w = fw;
            return *this;
        }

        //! Sets new Quaternion based on euler angles (radians)
        inline Quaternion& Set(f32 fx, f32 fy, f32 fz)
        {
            f64 angle;

            angle = fx * 0.5;
            const f64 sr = sin(angle);
            const f64 cr = cos(angle);

            angle = fy * 0.5;
            const f64 sp = sin(angle);
            const f64 cp = cos(angle);

            angle = fz * 0.5;
            const f64 sy = sin(angle);
            const f64 cy = cos(angle);

            const f64 cpcy = cp * cy;
            const f64 spcy = sp * cy;
            const f64 cpsy = cp * sy;
            const f64 spsy = sp * sy;

            x = (f32)(sr * cpcy - cr * spsy);
            y = (f32)(cr * spcy + sr * cpsy);
            z = (f32)(cr * cpsy - sr * spcy);
            w = (f32)(cr * cpcy + sr * spsy);

            return Normalize();
        }

        //! Sets new Quaternion based on euler angles (radians)
        inline Quaternion& Set(const Vector3Df& vec)
        {
            return Set(vec.x, vec.y, vec.z);
        }

        //! Normalizes the Quaternion
        inline Quaternion& Normalize()
        {
            const f32 n = x * x + y * y + z * z + w * w;

            if (n == 1)
                return *this;

            //n = 1.0f / sqrtf(n);
            return (*this *= reciprocal_squareroot ( n ));
        }

        //! Creates a matrix from this Quaternion
        inline Matrix4f GetMatrix() const
        {
            Matrix4f m;
            GetMatrix_transposed(m);
            return m;
        }

        //! Creates a matrix from this Quaternion
        inline void GetMatrix( Matrix4f &dest ) const
        {
            /*
            float fTx  = 2.0*x;
            float fTy  = 2.0*y;
            float fTz  = 2.0*z;
            float fTwx = fTx*w;
            float fTwy = fTy*w;
            float fTwz = fTz*w;
            float fTxx = fTx*x;
            float fTxy = fTy*x;
            float fTxz = fTz*x;
            float fTyy = fTy*y;
            float fTyz = fTz*y;
            float fTzz = fTz*z;

            
            dest[0][0] = 1.0-(fTyy+fTzz);
            dest[0][1] = fTxy-fTwz;
            dest[0][2] = fTxz+fTwy;
            dest[1][0] = fTxy+fTwz;
            dest[1][1] = 1.0-(fTxx+fTzz);
            dest[1][2] = fTyz-fTwx;
            dest[2][0] = fTxz-fTwy;
            dest[2][1] = fTyz+fTwx;
            dest[2][2] = 1.0-(fTxx+fTyy);
            */

            /*
            dest[0] = 1.0f - 2.0f * y * y - 2.0f * z * z;
            dest[1] = 2.0f * x * y - 2.0f * z * w;
            dest[2] = 2.0f * x * z + 2.0f * y * w;
            dest[3] = 0.0f;

            dest[4] = 2.0f * x * y + 2.0f * z * w;
            dest[5] = 1.0f - 2.0f * x * x - 2.0f * z * z;
            dest[6] = 2.0f * z * y - 2.0f * x * w;
            dest[7] = 0.0f;

            dest[8] = 2.0f * x * z - 2.0f * y * w;
            dest[9] = 2.0f * z * y + 2.0f * x * w;
            dest[10] = 1.0f - 2.0f * x * x - 2.0f * y * y;
            dest[11] = 0.0f;
            */
            
            dest[0] = 1.0f - 2.0f * y * y - 2.0f * z * z;
            dest[1] = 2.0f * x * y + 2.0f * z * w;
            dest[2] = 2.0f * x * z - 2.0f * y * w;
            dest[3] = 0.0f;

            dest[4] = 2.0f * x * y - 2.0f * z * w;
            dest[5] = 1.0f - 2.0f * x * x - 2.0f * z * z;
            dest[6] = 2.0f * z * y + 2.0f * x * w;
            dest[7] = 0.0f;

            dest[8] = 2.0f * x * z + 2.0f * y * w;
            dest[9] = 2.0f * z * y - 2.0f * x * w;
            dest[10] = 1.0f - 2.0f * x * x - 2.0f * y * y;
            dest[11] = 0.0f;

            dest[12] = 0.f;
            dest[13] = 0.f;
            dest[14] = 0.f;
            dest[15] = 1.f;
        }

        //! Creates a matrix from this Quaternion
        inline void GetMatrix_transposed( Matrix4f &dest ) const
        {
            dest[0] = 1.0f - 2.0f*y*y - 2.0f*z*z;
            dest[4] = 2.0f*x*y + 2.0f*z*w;
            dest[8] = 2.0f*x*z - 2.0f*y*w;
            dest[12] = 0.0f;

            dest[1] = 2.0f*x*y - 2.0f*z*w;
            dest[5] = 1.0f - 2.0f*x*x - 2.0f*z*z;
            dest[9] = 2.0f*z*y + 2.0f*x*w;
            dest[13] = 0.0f;

            dest[2] = 2.0f*x*z + 2.0f*y*w;
            dest[6] = 2.0f*z*y - 2.0f*x*w;
            dest[10] = 1.0f - 2.0f*x*x - 2.0f*y*y;
            dest[14] = 0.0f;

            dest[3] = 0.f;
            dest[7] = 0.f;
            dest[11] = 0.f;
            dest[15] = 1.f;
        };

        //! Inverts this Quaternion
        inline Quaternion& Inverse()
        {
            x = -x; y = -y; z = -z;
            return *this;
        }

		//! add by hjj 2010-5-21
		Quaternion Log () const
		{
			// If q = cos(A)+sin(A)*(x*i+y*j+z*k) where (x,y,z) is unit length, then
			// log(q) = A*(x*i+y*j+z*k).  If sin(A) is near zero, use log(q) =
			// sin(A)*(x*i+y*j+z*k) since sin(A)/A has limit 1.
			Quaternion kResult;
			kResult.w = 0.0;
			if ( AIR::Abs(w) < 1.0 )
			{
				float fAngle ( AIR::asm_arccos(w) );
				float fSin = AIR::asm_sin(fAngle);
				if ( AIR::Abs(fSin) >= Quaternion::ms_fEpsilon )
				{
					float fCoeff = fAngle/fSin;
					kResult.x = fCoeff*x;
					kResult.y = fCoeff*y;
					kResult.z = fCoeff*z;
					return kResult;
				}
			}
			kResult.x = x;
			kResult.y = y;
			kResult.z = z;
			return kResult;
		}

		//! add by hjj 2010-5-21
		Quaternion Exp () const
		{
			// If q = A*(x*i+y*j+z*k) where (x,y,z) is unit length, then
			// exp(q) = cos(A)+sin(A)*(x*i+y*j+z*k).  If sin(A) is near zero,
			// use exp(q) = cos(A)+A*(x*i+y*j+z*k) since A/sin(A) has limit 1.
			float fAngle ( AIR::asm_sqrt(x*x+y*y+z*z) );
			float fSin = AIR::asm_sin(fAngle);

			Quaternion kResult;
			kResult.w = AIR::asm_cos(fAngle);

			if ( AIR::Abs(fSin) >= ms_fEpsilon )
			{
				float fCoeff = fSin/fAngle;
				kResult.x = fCoeff*x;
				kResult.y = fCoeff*y;
				kResult.z = fCoeff*z;
			}
			else
			{
				kResult.x = x;
				kResult.y = y;
				kResult.z = z;
			}
			return kResult;
		}


        //! Set this Quaternion to the result of the interpolation between two quaternions
        //注意，interpolate是q2的比重，而不是q1的，例如interpolate= 0.8时，q1占0.2， q2占0.8
        Quaternion& Slerp(Quaternion q1, Quaternion q2, f32 interpolate )
        {
            f32 angle = q1.DotProduct(q2);

            if (angle < 0.0f)
            {
                q1 *= -1.0f;
                angle *= -1.0f;
            }

            f32 scale;
            f32 invscale;

            if ((angle + 1.0f) > 0.05f)
            {
                if ((1.0f - angle) >= 0.05f) // spherical interpolation
                {
                    const f32 theta = acosf(angle);
                    const f32 invsintheta = reciprocal(sinf(theta));
                    scale = sinf(theta * (1.0f - interpolate)) * invsintheta;
                    invscale = sinf(theta * interpolate) * invsintheta;
                }
                else // linear interploation
                {
                    scale = 1.0f - interpolate;
                    invscale = interpolate;
                }
            }
            else
            {
                q2.Set(-q1.y, q1.x, -q1.w, q1.z);
                scale = sinf(PI * (0.5f - interpolate));
                invscale = sinf(PI * interpolate);
            }

            return (*this = (q1 *scale) + (q2 * invscale));
        }


		//! add by hjj 2010-5-22
		// normalised linear interpolation - faster but less accurate (non-constant rotation velocity)
		static Quaternion nlerp(float fT, const Quaternion& rkP, const Quaternion& rkQ, bool shortestPath = false)
		{
			Quaternion result;
			float fCos = rkP.DotProduct(rkQ);
			if (fCos < 0.0f && shortestPath)
			{
				result = rkP + fT * ((-rkQ) - rkP);
			}
			else
			{
				result = rkP + fT * (rkQ - rkP);
			}
			result.Normalize();
			return result;
		}


		//! add by hjj 2010-5-21
		static Quaternion Slerp( f32 interpolate, const Quaternion& q1, const Quaternion& q2, bool shortestPath = false )
		{
			f32 fCos = q1.DotProduct(q2);

            Quaternion res;

			if (fCos < 0.0f && shortestPath )
			{
				res = -q2;
				fCos = -fCos;
			}
			else
			{
				res = q2;
			}

			//f32 scale;
			//f32 invscale;

            if (AIR::Abs(fCos) < 1.0f - ms_fEpsilon)
			{
				// spherical interpolation
                float fSin = sqrt(1.0f - fCos * fCos);
                float fAngle = atan2(fSin, fCos);//Math::ATan2(fSin, fCos);
                float fInvSin = 1.0f / fSin;
                float fCoeff0 = sinf((1.0f - interpolate) * fAngle) * fInvSin;  //Math::Sin((1.0f - fT) * fAngle) * fInvSin;
                float fCoeff1 = sinf(interpolate * fAngle) * fInvSin;  //Math::Sin(fT * fAngle) * fInvSin;
                return fCoeff0 * q1 + fCoeff1 * res;
			}
			else
			{
				//q2.Set(-q1.y, q1.x, -q1.w, q1.z);
				//scale = sinf(PI * (0.5f - interpolate));
				//invscale = sinf(PI * interpolate);
                Quaternion t = (1.0f - interpolate) * q1 + interpolate * res;
                // taking the complement requires renormalisation
                t.Normalize();
                return t;
			}

			//return ((q1 *scale) + (q2 * invscale));
		}

		//! add by hjj 2010-5-21
		// spherical quadratic interpolation
		static Quaternion Squad ( const Quaternion& rkP,const Quaternion& rkA, const Quaternion& rkB,const Quaternion& rkQ, f32 interpolate, bool shortestPath = false)
		{
			float fSlerpT = 2.0 * interpolate * (1.0 - interpolate);
			Quaternion kSlerpP = Slerp( interpolate, rkP, rkQ, shortestPath);
			Quaternion kSlerpQ = Slerp( interpolate, rkA, rkB, false);
			return Slerp(fSlerpT, kSlerpP ,kSlerpQ, false);
		}

        //! Create Quaternion from rotation angle and rotation axis.
        /** Axis must be unit length. 负数是逆时针，正数是顺时针
        The Quaternion representing the rotation is
        q = cos(A/2)+sin(A/2)*(x*i+y*j+z*k).
        \param angle Rotation Angle in radians.
        \param axis Rotation axis. */
        Quaternion& FromAngleAxis (f32 angle, const Vector3Df& axis)
        {
            const f32 fHalfAngle = 0.5f * angle;
            const f32 fSin = sinf(fHalfAngle);
            w = cosf(fHalfAngle);
            x = fSin * axis.x;
            y = fSin * axis.y;
            z = fSin * axis.z;
            return *this;
        }

        void FromMatrix(const Matrix4f& mat)
        {
            float fTrace = mat(0, 0) + mat(1, 1) + mat(2, 2);
            float fRoot;

            if ( fTrace > 0.0 )
            {
                // |w| > 1/2, may as well choose w > 1/2
                fRoot = sqrt(fTrace + 1.0);  // 2w
                w = 0.5 * fRoot;
                fRoot = 0.5/fRoot;  // 1/(4w)
                x = (mat(2, 1) - mat(1, 2)) * fRoot;
                y = (mat(0, 2) - mat(2, 0)) * fRoot;
                z = (mat(1, 0) - mat(0, 1)) * fRoot;
            }
            else
            {
                // |w| <= 1/2
                static u32 s_iNext[3] = { 1, 2, 0 };
                u32 i = 0;
                if ( mat(1, 1) > mat(0, 0) )
                    i = 1;
                if ( mat(2, 2) > mat(i, i) )
                    i = 2;
                u32 j = s_iNext[i];
                u32 k = s_iNext[j];

                fRoot = sqrt(mat(i, i) - mat(j, j) - mat(k, k) + 1.0);
                float* apkQuat[3] = { &x, &y, &z };
                *apkQuat[i] = 0.5 * fRoot;
                fRoot = 0.5 / fRoot;
                w = (mat(k, j) - mat(j, k)) * fRoot;
                *apkQuat[j] = (mat(j, i) + mat(i, j)) * fRoot;
                *apkQuat[k] = (mat(k, i) + mat(i, k)) * fRoot;
            }
        }

        //! Fills an angle (radians) around an axis (unit vector)
        void ToAngleAxis (f32 &angle, Vector3Df& axis) const
        {
            const f32 scale = sqrtf(x * x + y * y + z * z);

            if (AIR::iszero(scale) || w > 1.0f || w < -1.0f)
            {
                angle = 0.0f;
                axis.x = 0.0f;
                axis.y = 1.0f;
                axis.z = 0.0f;
            }
            else
            {
                const f32 invscale = reciprocal(scale);
                angle = 2.0f * acosf(w);
                axis.x = x * invscale;
                axis.y = y * invscale;
                axis.z = z * invscale;
            }
        }

        //! Output this Quaternion to an euler angle (radians)
        void ToEuler(Vector3Df& euler) const
        {
            const f64 sqw = w * w;
            const f64 sqx = x * x;
            const f64 sqy = y * y;
            const f64 sqz = z * z;

            // heading = rotation about z-axis
            euler.z = (f32) (atan2(2.0 * (x*y +z*w),(sqx - sqy - sqz + sqw)));

            // bank = rotation about x-axis
            euler.x = (f32) (atan2(2.0 * (y*z +x*w),(-sqx - sqy + sqz + sqw)));

            // attitude = rotation about y-axis
            euler.y = asinf( clamp(-2.0f * (x*z - y*w), -1.0f, 1.0f) );
        };

        //! Set Quaternion to identity
        inline Quaternion& Identity()
        {
            w = 1.f;
            x = 0.f;
            y = 0.f;
            z = 0.f;
            return *this;
        }

        //! Set Quaternion to represent a rotation from one vector to another.
        Quaternion& RotationFromTo(const Vector3Df& from, const Vector3Df& to)
        {
            // Based on Stan Melax's article in Game Programming Gems
            // Copy, since cannot modify local
            Vector3Df v0 = from;
            Vector3Df v1 = to;
            v0.Normalize();
            v1.Normalize();

            const f32 d = v0.DotProduct(v1);
            if (d >= 1.0f) // If dot == 1, vectors are the same
            {
                return Identity();
            }
            if (d < (1e-6f - 1.0f))
            {
                    // Generate an axis
                Vector3Df axis = Vector3Df(1, 0, 0).CrossProduct(from);
                if (axis.IsZeroLength() ) // pick another if colinear
                    axis = Vector3Df(0, 1, 0).CrossProduct(from);
                axis.Normalize();
                FromAngleAxis(AIR::PI, axis);
                return *this;
            }

            const f32 s = sqrtf( (1+d)*2 ); // optimize inv_sqrt
            const f32 invs = 1.f / s;
            const Vector3Df c = v0.CrossProduct(v1)*invs;

            x = c.x;
            y = c.y;
            z = c.z;
            w = s * 0.5f;

            return *this;
        }

        inline void CaculateW()
        {
            float t = 1.0f - (x * x) - (y * y) - (z * z);

            if (t < 0.0f)
                w = 0.0f;
            else
                w = -sqrt (t);
        }

        void FromMatrixEx(const Matrix4f& mat)
        {
            float fourWSquaredMinus1 = mat(0, 0) + mat(1, 1) + mat(2, 2);
            float fourXSquaredMinus1 = mat(0, 0) - mat(1, 1) - mat(2, 2);
            float fourYSquaredMinus1 = mat(1, 1) - mat(0, 0) - mat(2, 2);
            float fourZSquaredMinus1 = mat(2, 2) - mat(0, 0) - mat(1, 1);
            int biggestIndex = 0;
            float fourBiggestSquaredMinus1 = fourWSquaredMinus1;
            if (fourXSquaredMinus1 > fourBiggestSquaredMinus1)
            {
                fourBiggestSquaredMinus1 = fourXSquaredMinus1;
                biggestIndex = 1;
            }
            
            if (fourYSquaredMinus1 > fourBiggestSquaredMinus1)
            {
                fourBiggestSquaredMinus1 = fourYSquaredMinus1;
                biggestIndex = 2;
            }

            if (fourZSquaredMinus1 > fourBiggestSquaredMinus1)
            {
                fourBiggestSquaredMinus1 = fourZSquaredMinus1;
                biggestIndex = 3;
            }

            float biggestVal = sqrt(fourBiggestSquaredMinus1 + 1.0f) * 0.5f;
            float mult = 0.25f / biggestVal;

            switch (biggestIndex)
            {
            case 0:
                w = biggestVal;
                x = (mat(1, 2) - mat(2, 1)) * mult;
                y = (mat(2, 0) - mat(0, 2)) * mult;
                z = (mat(0, 1) - mat(1, 0)) * mult;
                break;
            case 1:
                x = biggestVal;
                w = (mat(1, 2) - mat(2, 1)) * mult;
                y = (mat(0, 1) + mat(1, 0)) * mult;
                z = (mat(2, 0) + mat(0, 2)) * mult;
                break;
            case 2:
                y = biggestVal;
                w = (mat(2, 0) - mat(0, 2)) * mult;
                x = (mat(0, 1) + mat(1, 0)) * mult;
                z = (mat(1, 2) + mat(2, 1)) * mult;
                break;
            case 3:
                z = biggestVal;
                w = (mat(0, 1) - mat(1, 0)) * mult;
                x = (mat(2, 0) + mat(0, 2)) * mult;
                y = (mat(1, 2) + mat(2, 1)) * mult;
                break;
            }
        }

		inline bool Equals(const Quaternion& other, const f32 tolerance = (f32)ROUNDING_ERROR_32 ) const
		{
			return(	AIR::Equals(x, other.x, tolerance) &&
					AIR::Equals(y, other.y, tolerance) &&
					AIR::Equals(z, other.z, tolerance) &&
					AIR::Equals(w, other.w, tolerance) );
		}

		inline Vector3Df GetAxisX() const
		{
			float ys = y * 2.f;
			float zs = z * 2.f;

			float wy = w * ys;
			float wz = w * zs;

			float yy = y * ys;
			float zz = z * zs;
			float xy = x * ys;
			float xz = x * zs;
			return Vector3Df(1 - yy - zz,     xy + wz,     xz - wy);
		}

		inline Vector3Df GetAxisY() const
		{
			float xs = x * 2.f;
			float ys = y * 2.f;
			float zs = z * 2.f;

			float wx = w * xs;
			float wz = w * zs;

			float xx = x * xs;
			float zz = z * zs;
			float xy = x * ys;
			float yz = y * zs;
			return Vector3Df(xy - wz, 1 - xx - zz,     yz + wx);
		}

		inline Vector3Df GetAxisZ() const
		{
			float xs = x * 2.f;
			float ys = y * 2.f;
			float zs = z * 2.f;

			float wx = w * xs;
			float wy = w * ys;

			float xx = x * xs;
			float yy = y * ys;
			float xz = x * zs;
			float yz = y * zs;
			return Vector3Df(xz + wy,     yz - wx, 1 - xx - yy);
		}

		void ToRotationMatrix(Matrix3& mat) const;
	public:
        //! Quaternion elements.
        f32 x, y, z, w;
    public:
		static const float			ms_fEpsilon;
        static const Quaternion		ZERO;
        static const Quaternion		IDENEITY;
    };


	inline void MultiplyQuaternion( const Quaternion& q1, const Quaternion& q2, Quaternion& qOut )
	{
		qOut.x = q1.w * q2.x + q1.x * q2.w + q1.y * q2.z - q1.z * q2.y;
		qOut.y = q1.w * q2.y + q1.y * q2.w + q1.z * q2.x - q1.x * q2.z;
		qOut.z = q1.w * q2.z + q1.z * q2.w + q1.x * q2.y - q1.y * q2.x;
		qOut.w = q1.w * q2.w - q1.x * q2.x - q1.y * q2.y - q1.z * q2.z;
	}

	inline void MultiplyMatrix4( const Matrix4f& m1, const Matrix4f& m2, Matrix4f& mOut )
	{
		mOut[0]  = m1[0] * m2[0] + m1[1] * m2[4] + m1[2] * m2[8] + m1[3] * m2[12];
		mOut[1]  = m1[0] * m2[1] + m1[1] * m2[5] + m1[2] * m2[9] + m1[3] * m2[13];
		mOut[2]  = m1[0] * m2[2] + m1[1] * m2[6] + m1[2] * m2[10] + m1[3] * m2[14];
		mOut[3]  = m1[0] * m2[3] + m1[1] * m2[7] + m1[2] * m2[11] + m1[3] * m2[15];

		mOut[4]  = m1[4] * m2[0] + m1[5] * m2[4] + m1[6] * m2[8] + m1[7] * m2[12];
		mOut[5]  = m1[4] * m2[1] + m1[5] * m2[5] + m1[6] * m2[9] + m1[7] * m2[13];
		mOut[6]  = m1[4] * m2[2] + m1[5] * m2[6] + m1[6] * m2[10] + m1[7] * m2[14];
		mOut[7]  = m1[4] * m2[3] + m1[5] * m2[7] + m1[6] * m2[11] + m1[7] * m2[15];

		mOut[8]  = m1[8] * m2[0] + m1[9] * m2[4] + m1[10] * m2[8] + m1[11] * m2[12];
		mOut[9]  = m1[8] * m2[1] + m1[9] * m2[5] + m1[10] * m2[9] + m1[11] * m2[13];
		mOut[10] = m1[8] * m2[2] + m1[9] * m2[6] + m1[10] * m2[10] + m1[11] * m2[14];
		mOut[11] = m1[8] * m2[3] + m1[9] * m2[7] + m1[10] * m2[11] + m1[11] * m2[15];

		mOut[12] = m1[12] * m2[0] + m1[13]*m2[4] + m1[14]*m2[8] + m1[15]*m2[12];
		mOut[13] = m1[12] * m2[1] + m1[13]*m2[5] + m1[14]*m2[9] + m1[15]*m2[13];
		mOut[14] = m1[12] * m2[2] + m1[13]*m2[6] + m1[14]*m2[10] + m1[15]*m2[14];
		mOut[15] = m1[12] * m2[3] + m1[13]*m2[7] + m1[14]*m2[11] + m1[15]*m2[15];
	}

	inline void MultiplyVector3( const Vector3Df& v1, const Vector3Df& v2, Vector3Df& vOut )
	{
		vOut.x = v1.x * v2.x;
		vOut.y = v1.y * v2.y;
		vOut.z = v1.z * v2.z;
	}

	inline void QuaternionMultiplyVector( const Quaternion& q, const Vector3Df& v, Vector3Df& vOut )
	{
		Vector3Df uv, uuv;
		Vector3Df qvec(q.x, q.y, q.z);
		uv  = qvec.CrossProduct(v);
		uuv = qvec.CrossProduct(uv);
		uv  *= (2.0f * q.w);
		uuv *=  2.0f;
		vOut.x = v.x + uv.x + uuv.x;
		vOut.y = v.y + uv.y + uuv.y;
		vOut.z = v.z + uv.z + uuv.z;
	}

	inline void QuaternionInverse( const Quaternion& q, Quaternion& qOut )
	{
		qOut.x = -q.x; 
		qOut.y = -q.y; 
		qOut.z = -q.z; 
		qOut.w =  q.w;
	}

	inline void FastNormalize( f32* pVec )
	{
		f32 len = AIR::asm_sqrt( pVec[0] * pVec[0] + pVec[1] * pVec[1] + pVec[2] * pVec[2] );
		if (len > 1e-08)
		{
			f32 fInvLength = 1.0 / len;
			pVec[0] *= fInvLength;
			pVec[1] *= fInvLength;
			pVec[2] *= fInvLength;
		}
	}

};
