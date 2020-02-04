#pragma once 
#include "MyMath.h"

#include "Plane3D.h"
#include "Rect.h"
#include "AABBBox3D.h"
#include "Vector2D.h"

#include "Vector3D.h"
#include "Vector4.h"
#include "Triangle3D.h"
//#include "SIMD.h"

namespace AIR
{
 //   template<class f32>class AABBBox3D;
	//template<class f32>class Vector2D;
	//template<class f32>
	class Quaternion;
	class EngineExport Matrix3
	{
	public:
		Vector3D GetRow3(size_t i) const
		{
			return Vector3D(m[i][0], m[i][1], m[i][2]);
		}
		void Identity()
		{
			_11 = 1.0f, _12 = 0.0f, _13 = 0.0f;
			_21 = 0.0f, _22 = 1.0f, _23 = 0.0f;
			_31 = 0.0f, _32 = 0.0f, _33 = 1.0f;
		}
		void SetRow(size_t i, f32 x, f32 y, f32 z)
		{
			m[i][0] = x;
			m[i][1] = y;
			m[i][2] = z;
		}

		Vector3D GetColumn(size_t i) const
		{
			return Vector3D(m[0][i], m[1][i], m[2][i]);
		}

		f32 Det() const
		{
			return (_11 * _22 - _12 * _21) * _33
				- (_11 * _23 - _13 * _21) * _32
				+ (_11 * _23 - _13 * _22) * _31;
		}
		void Inverse(Matrix3 &output) const
		{
			//assert(this != &output);

			float s = Det();
			if (s == 0) return;

			s = 1.0f / s;
			output._11 = s * ((_22 * _33) - (_23 * _32));
			output._12 = s * ((_32 * _13) - (_33 * _12));
			output._13 = s * ((_12 * _23) - (_13 * _22));
			output._21 = s * ((_23 * _31) - (_21 * _33));
			output._22 = s * ((_33 * _11) - (_31 * _13));
			output._23 = s * ((_13 * _21) - (_11 * _23));
			output._31 = s * ((_21 * _32) - (_22 * _31));
			output._32 = s * ((_31 * _12) - (_32 * _11));
			output._33 = s * ((_11 * _22) - (_12 * _21));
		}
		void Inverse()
		{
			Matrix3 tmp = *this;
			tmp.Inverse(*this);
		}
		void MakeScaleMatrix(const Vector3D &scale)
		{
			Identity();
			_11 = scale.x;
			_22 = scale.y;
			_33 = scale.z;
		}
		void MakeScaleMatrix(f32 s)
		{
			MakeScaleMatrix(Vector3D(s, s, s));
		}

		void Transform(Vector3D &output, const Vector3D &input) const
		{
			float x = input.x * _11 + input.y * _21 + input.z * _31;
			float y = input.x * _12 + input.y * _22 + input.z * _32;
			float z = input.x * _13 + input.y * _23 + input.z * _33;

			output.x = x;
			output.y = y;
			output.z = z;
		}

		inline const f32* operator[] (size_t iRow) const
		{
			return m[iRow];
		}

		inline f32* operator[] (size_t iRow)
		{
			return m[iRow];
		}

	public:
		union
		{
			f32 m[3][3];
			struct
			{
				f32 _11, _12, _13;
				f32 _21, _22, _23;
				f32 _31, _32, _33;
			};
		};
	};

	typedef Matrix3 Matrix3f;

	inline Vector3D operator *(const Vector3D &vec, const Matrix3 &mat)
	{
		return Vector3D(vec.x * mat._11 + vec.y * mat._21 + vec.z * mat._31,
			vec.x * mat._12 + vec.y * mat._22 + vec.z * mat._32,
			vec.x * mat._13 + vec.y * mat._23 + vec.z * mat._33);
	}

	inline Matrix3 operator *(const Matrix3 &m1, const Matrix3 &m2)
	{
		Matrix3 retvalue;
		for (int i = 0; i < 3; i++)
		{
			for (int j = 0; j < 3; j++)
			{
				retvalue.m[i][j] = m1.m[i][0] * m2.m[0][j] + m1.m[i][1] * m2.m[1][j] + m1.m[i][2] * m2.m[2][j];
			}
		}
		return retvalue;
	}

    class EngineExport Matrix4
    {
    public:
        Matrix4(/*bool bIdentity = false*/)// : m_bIdentity(bIdentity)
        {
            //if (bIdentity)
            //{
            //    Identity();
            //}
            //else
            //{
            //    m_bIdentity = false;
                memset(M, 0, 16 * sizeof(f32));
            //}
        }

        Matrix4(f32 m00, f32 m01, f32 m02, f32 m03, 
                       f32 m10, f32 m11, f32 m12, f32 m13,
                       f32 m20, f32 m21, f32 m22, f32 m23, 
                       f32 m30, f32 m31, f32 m32, f32 m33)
        {
            M[0] = m00;
            M[1] = m01;
            M[2] = m02;
            M[3] = m03;
            M[4] = m10;
            M[5] = m11;
            M[6] = m12;
            M[7] = m13;
            M[8] = m20;
            M[9] = m21;
            M[10] = m22;
            M[11] = m23;
            M[12] = m30;
            M[13] = m31;
            M[14] = m32;
            M[15] = m33;
        }

        Matrix4(const Matrix4& other)// : m_bIdentity(false)
        {
            //if (other.m_bIdentity)
            //{
            //    Identity();
            //}
            //else
            //    *this = other;
			//this->m_bIdentity = other.m_bIdentity;
			memcpy( &(this->M), &(other.M), sizeof(f32) * 16 );
        }

        Matrix4& Identity()
        {
            memset(M, 0, 16 * sizeof(f32));
            M[0] = M[5] = M[10] = M[15] = (f32)1;
            //m_bIdentity = true;
            //m_bIdentity=true;
            return *this;
        }

        bool IsIdentity() const
        {
            //if (m_bIdentity)
            //{
            //    return m_bIdentity;
            //}

            if (!AIR::Equals( M[ 0], (f32)1 ) ||
                !AIR::Equals( M[ 5], (f32)1 ) ||
                !AIR::Equals( M[10], (f32)1 ) ||
                !AIR::Equals( M[15], (f32)1 ))
                return false;

            for (s32 i = 0; i < 4; ++i)
                for (s32 j = 0; j < 4; ++j)
                    if ((j != i) && (!AIR::iszero((*this)(i, j))))
                        return false;
            return false;
        }

        //! Simple operator for directly accessing every element of the matrix.
        inline f32& operator()(const u32 row, const u32 col) 
        { 
            //m_bIdentity = false; 
            return M[ row * 4 + col ]; 
        }

        //! Simple operator for directly accessing every element of the matrix.
        inline const f32& operator()(const u32 row, const u32 col) const 
        { 
            return M[row * 4 + col];
        }

        //! Simple operator for linearly accessing every element of the matrix.
        f32& operator[](u32 index) 
        { 
            //m_bIdentity = false; 
            return M[index]; 
        }

        //! Simple operator for linearly accessing every element of the matrix.
        inline const f32& operator[](u32 index) const 
        { 
            return M[index]; 
        }

        //! Sets this matrix equal to the other matrix.
        inline Matrix4& operator = (const Matrix4 &other)
        {
            if (this == &other)
                return *this;
            memcpy( &(M), &(other.M), 16 * sizeof(f32) );
            //m_bIdentity = other.m_bIdentity;
            return *this;
        }

        //! Sets all elements of this matrix to the value.
        inline Matrix4& operator = (const f32& scalar)
        {
            for (s32 i = 0; i < 16; ++i)
                M[i] = scalar;
            //m_bIdentity = false;
            return *this;
        }

        //! Returns pointer to internal array
        const f32* Pointer() const 
        { 
            return M; 
        }

        f32* Pointer() 
        { 
            //m_bIdentity = false; 
            return M; 
        }

        //! Returns true if other matrix is equal to this matrix.
        inline bool operator == (const Matrix4 &other) const
        {
            //if (m_bIdentity && other.m_bIdentity)
            //    return true;
            for (s32 i = 0; i < 16; ++i)
                if (M[i] != other.M[i])
                    return false;

            return true;
        }

        //! Returns true if other matrix is not equal to this matrix.
        inline bool operator != (const Matrix4 &other) const
        {
            return !(*this == other);
        }

        //! Add another matrix.
        Matrix4 operator + (const Matrix4& other) const
        {
            Matrix4 temp;

            temp[0] = M[0] + other[0];
            temp[1] = M[1] + other[1];
            temp[2] = M[2] + other[2];
            temp[3] = M[3] + other[3];
            temp[4] = M[4] + other[4];
            temp[5] = M[5] + other[5];
            temp[6] = M[6] + other[6];
            temp[7] = M[7] + other[7];
            temp[8] = M[8] + other[8];
            temp[9] = M[9] + other[9];
            temp[10] = M[10] + other[10];
            temp[11] = M[11] + other[11];
            temp[12] = M[12] + other[12];
            temp[13] = M[13] + other[13];
            temp[14] = M[14] + other[14];
            temp[15] = M[15] + other[15];

            return temp;
        }



        //! Add another matrix.
        Matrix4& operator += (const Matrix4& other)
        {
            M[0] += other[0];
            M[1] += other[1];
            M[2] += other[2];
            M[3] += other[3];
            M[4] += other[4];
            M[5] += other[5];
            M[6] += other[6];
            M[7] += other[7];
            M[8] += other[8];
            M[9] += other[9];
            M[10] += other[10];
            M[11] += other[11];
            M[12] += other[12];
            M[13] += other[13];
            M[14] += other[14];
            M[15] += other[15];

            return *this;
        }

        //! Subtract another matrix.
        Matrix4 operator - (const Matrix4& other) const
        {
            Matrix4 temp; // ( EM4CONST_NOTHING );

            temp[0] = M[0] - other[0];
            temp[1] = M[1] - other[1];
            temp[2] = M[2] - other[2];
            temp[3] = M[3] - other[3];
            temp[4] = M[4] - other[4];
            temp[5] = M[5] - other[5];
            temp[6] = M[6] - other[6];
            temp[7] = M[7] - other[7];
            temp[8] = M[8] - other[8];
            temp[9] = M[9] - other[9];
            temp[10] = M[10] - other[10];
            temp[11] = M[11] - other[11];
            temp[12] = M[12] - other[12];
            temp[13] = M[13] - other[13];
            temp[14] = M[14] - other[14];
            temp[15] = M[15] - other[15];

            return temp;
        }

        //! Subtract another matrix.
        Matrix4& operator -= (const Matrix4& other)
        {
            M[0] -= other[0];
            M[1] -= other[1];
            M[2] -= other[2];
            M[3] -= other[3];
            M[4] -= other[4];
            M[5] -= other[5];
            M[6] -= other[6];
            M[7] -= other[7];
            M[8] -= other[8];
            M[9] -= other[9];
            M[10] -= other[10];
            M[11] -= other[11];
            M[12] -= other[12];
            M[13] -= other[13];
            M[14] -= other[14];
            M[15] -= other[15];

            return *this;
        }

        //! Multiply by another matrix.
        Matrix4 operator * (const Matrix4& m2) const
        {
            //if ( this->IsIdentity() )
            //    return m2;
            //if ( m2.IsIdentity() )
            //    return *this;

            Matrix4 m3; // ( false );

            const f32 *m1 = M;

            /*
            m3[0] = m1[0]*m2[0] + m1[4]*m2[1] + m1[8]*m2[2] + m1[12]*m2[3];
            m3[1] = m1[1]*m2[0] + m1[5]*m2[1] + m1[9]*m2[2] + m1[13]*m2[3];
            m3[2] = m1[2]*m2[0] + m1[6]*m2[1] + m1[10]*m2[2] + m1[14]*m2[3];
            m3[3] = m1[3]*m2[0] + m1[7]*m2[1] + m1[11]*m2[2] + m1[15]*m2[3];

            m3[4] = m1[0]*m2[4] + m1[4]*m2[5] + m1[8]*m2[6] + m1[12]*m2[7];
            m3[5] = m1[1]*m2[4] + m1[5]*m2[5] + m1[9]*m2[6] + m1[13]*m2[7];
            m3[6] = m1[2]*m2[4] + m1[6]*m2[5] + m1[10]*m2[6] + m1[14]*m2[7];
            m3[7] = m1[3]*m2[4] + m1[7]*m2[5] + m1[11]*m2[6] + m1[15]*m2[7];

            m3[8] = m1[0]*m2[8] + m1[4]*m2[9] + m1[8]*m2[10] + m1[12]*m2[11];
            m3[9] = m1[1]*m2[8] + m1[5]*m2[9] + m1[9]*m2[10] + m1[13]*m2[11];
            m3[10] = m1[2]*m2[8] + m1[6]*m2[9] + m1[10]*m2[10] + m1[14]*m2[11];
            m3[11] = m1[3]*m2[8] + m1[7]*m2[9] + m1[11]*m2[10] + m1[15]*m2[11];

            m3[12] = m1[0]*m2[12] + m1[4]*m2[13] + m1[8]*m2[14] + m1[12]*m2[15];
            m3[13] = m1[1]*m2[12] + m1[5]*m2[13] + m1[9]*m2[14] + m1[13]*m2[15];
            m3[14] = m1[2]*m2[12] + m1[6]*m2[13] + m1[10]*m2[14] + m1[14]*m2[15];
            m3[15] = m1[3]*m2[12] + m1[7]*m2[13] + m1[11]*m2[14] + m1[15]*m2[15];
            */

            m3[0] = m1[0] * m2[0] + m1[1] * m2[4] + m1[2] * m2[8] + m1[3] * m2[12];
            m3[1] = m1[0] * m2[1] + m1[1] * m2[5] + m1[2] * m2[9] + m1[3] * m2[13];
            m3[2] = m1[0] * m2[2] + m1[1] * m2[6] + m1[2] * m2[10] + m1[3] * m2[14];
            m3[3] = m1[0] * m2[3] + m1[1] * m2[7] + m1[2] * m2[11] + m1[3] * m2[15];

            m3[4] = m1[4] * m2[0] + m1[5] * m2[4] + m1[6] * m2[8] + m1[7] * m2[12];
            m3[5] = m1[4] * m2[1] + m1[5] * m2[5] + m1[6] * m2[9] + m1[7] * m2[13];
            m3[6] = m1[4] * m2[2] + m1[5] * m2[6] + m1[6] * m2[10] + m1[7] * m2[14];
            m3[7] = m1[4] * m2[3] + m1[5] * m2[7] + m1[6] * m2[11] + m1[7] * m2[15];

            m3[8] = m1[8] * m2[0] + m1[9] * m2[4] + m1[10] * m2[8] + m1[11] * m2[12];
            m3[9] = m1[8] * m2[1] + m1[9] * m2[5] + m1[10] * m2[9] + m1[11] * m2[13];
            m3[10] = m1[8] * m2[2] + m1[9] * m2[6] + m1[10] * m2[10] + m1[11] * m2[14];
            m3[11] = m1[8] * m2[3] + m1[9] * m2[7] + m1[10] * m2[11] + m1[11] * m2[15];

            m3[12] = m1[12] * m2[0] + m1[13]*m2[4] + m1[14]*m2[8] + m1[15]*m2[12];
            m3[13] = m1[12] * m2[1] + m1[13]*m2[5] + m1[14]*m2[9] + m1[15]*m2[13];
            m3[14] = m1[12] * m2[2] + m1[13]*m2[6] + m1[14]*m2[10] + m1[15]*m2[14];
            m3[15] = m1[12] * m2[3] + m1[13]*m2[7] + m1[14]*m2[11] + m1[15]*m2[15];
            return m3;
        }

        //! Multiply by another matrix.
        Matrix4& operator *= (const Matrix4& other)
        {
            //if ( !other.IsIdentity() )
            //{
            //    if ( this->IsIdentity() )
            //    {
            //        return (*this = other);
            //    }
            //    else
                //{
                    Matrix4 temp ( *this );
                    return SetByProduct_NoCheck( temp, other );
                //}
            //}
            //return *this;
        }

        //! Multiply by scalar.
        Matrix4 operator * (const f32& scalar) const
        {
            Matrix4 temp; //( EM4CONST_NOTHING );

            temp[0] = M[0] * scalar;
            temp[1] = M[1] * scalar;
            temp[2] = M[2] * scalar;
            temp[3] = M[3] * scalar;
            temp[4] = M[4] * scalar;
            temp[5] = M[5] * scalar;
            temp[6] = M[6] * scalar;
            temp[7] = M[7] * scalar;
            temp[8] = M[8] * scalar;
            temp[9] = M[9] * scalar;
            temp[10] = M[10] * scalar;
            temp[11] = M[11] * scalar;
            temp[12] = M[12] * scalar;
            temp[13] = M[13] * scalar;
            temp[14] = M[14] * scalar;
            temp[15] = M[15] * scalar;

            return temp;
        }

        //! Multiply by scalar.
        Matrix4& operator *= (const f32& scalar)
        {
            M[0] *= scalar;
            M[1] *= scalar;
            M[2] *= scalar;
            M[3] *= scalar;
            M[4] *= scalar;
            M[5] *= scalar;
            M[6] *= scalar;
            M[7] *= scalar;
            M[8] *= scalar;
            M[9] *= scalar;
            M[10] *= scalar;
            M[11] *= scalar;
            M[12] *= scalar;
            M[13] *= scalar;
            M[14] *= scalar;
            M[15] *= scalar;

            return *this;
        }

        //! multiply by another matrix
        // set this matrix to the product of two other matrices
        // goal is to reduce stack use and copy

        inline Matrix4& SetByProduct_NoCheck(const Matrix4& other_a,const Matrix4& other_b )
        {
            const f32 *m1 = other_a.M;
            const f32 *m2 = other_b.M;

            /*
            M[0] = m1[0] * m2[0] + m1[4] * m2[1] + m1[8] * m2[2] + m1[12] * m2[3];
            M[1] = m1[1] * m2[0] + m1[5] * m2[1] + m1[9] * m2[2] + m1[13] * m2[3];
            M[2] = m1[2]*m2[0] + m1[6]*m2[1] + m1[10]*m2[2] + m1[14]*m2[3];
            M[3] = m1[3]*m2[0] + m1[7]*m2[1] + m1[11]*m2[2] + m1[15]*m2[3];

            M[4] = m1[0]*m2[4] + m1[4]*m2[5] + m1[8]*m2[6] + m1[12]*m2[7];
            M[5] = m1[1]*m2[4] + m1[5]*m2[5] + m1[9]*m2[6] + m1[13]*m2[7];
            M[6] = m1[2]*m2[4] + m1[6]*m2[5] + m1[10]*m2[6] + m1[14]*m2[7];
            M[7] = m1[3]*m2[4] + m1[7]*m2[5] + m1[11]*m2[6] + m1[15]*m2[7];

            M[8] = m1[0]*m2[8] + m1[4]*m2[9] + m1[8]*m2[10] + m1[12]*m2[11];
            M[9] = m1[1]*m2[8] + m1[5]*m2[9] + m1[9]*m2[10] + m1[13]*m2[11];
            M[10] = m1[2]*m2[8] + m1[6]*m2[9] + m1[10]*m2[10] + m1[14]*m2[11];
            M[11] = m1[3]*m2[8] + m1[7]*m2[9] + m1[11]*m2[10] + m1[15]*m2[11];

            M[12] = m1[0]*m2[12] + m1[4]*m2[13] + m1[8]*m2[14] + m1[12]*m2[15];
            M[13] = m1[1]*m2[12] + m1[5]*m2[13] + m1[9]*m2[14] + m1[13]*m2[15];
            M[14] = m1[2]*m2[12] + m1[6]*m2[13] + m1[10]*m2[14] + m1[14]*m2[15];
            M[15] = m1[3]*m2[12] + m1[7]*m2[13] + m1[11]*m2[14] + m1[15]*m2[15];
            */

            M[0] = m1[0] * m2[0] + m1[1] * m2[4] + m1[2] * m2[8] + m1[3] * m2[12];
            M[1] = m1[0] * m2[1] + m1[1] * m2[5] + m1[2] * m2[9] + m1[3] * m2[13];
            M[2] = m1[0] * m2[2] + m1[1] * m2[6] + m1[2] * m2[10] + m1[3] * m2[14];
            M[3] = m1[0] * m2[3] + m1[1] * m2[7] + m1[2] * m2[11] + m1[3] * m2[15];

            M[4] = m1[4] * m2[0] + m1[5] * m2[4] + m1[6] * m2[8] + m1[7] * m2[12];
            M[5] = m1[4] * m2[1] + m1[5] * m2[5] + m1[6] * m2[9] + m1[7] * m2[13];
            M[6] = m1[4] * m2[2] + m1[5] * m2[6] + m1[6] * m2[10] + m1[7] * m2[14];
            M[7] = m1[4] * m2[3] + m1[5] * m2[7] + m1[6] * m2[11] + m1[7] * m2[15];

            M[8] = m1[8] * m2[0] + m1[9] * m2[4] + m1[10] * m2[8] + m1[11] * m2[12];
            M[9] = m1[8] * m2[1] + m1[9] * m2[5] + m1[10] * m2[9] + m1[11] * m2[13];
            M[10] = m1[8] * m2[2] + m1[9] * m2[6] + m1[10] * m2[10] + m1[11] * m2[14];
            M[11] = m1[8] * m2[3] + m1[9] * m2[7] + m1[10] * m2[11] + m1[11] * m2[15];

            M[12] = m1[12] * m2[0] + m1[13]*m2[4] + m1[14]*m2[8] + m1[15]*m2[12];
            M[13] = m1[12] * m2[1] + m1[13]*m2[5] + m1[14]*m2[9] + m1[15]*m2[13];
            M[14] = m1[12] * m2[2] + m1[13]*m2[6] + m1[14]*m2[10] + m1[15]*m2[14];
            M[15] = m1[12] * m2[3] + m1[13]*m2[7] + m1[14]*m2[11] + m1[15]*m2[15];
            //m_bIdentity = false;
            return *this;
        }


        //! multiply by another matrix
        // set this matrix to the product of two other matrices
        // goal is to reduce stack use and copy
        inline Matrix4& SetByProduct(const Matrix4& other_a, const Matrix4& other_b )
        {
            //if ( other_a.IsIdentity () )
            //    return (*this = other_b);
            //else
            //    if ( other_b.IsIdentity () )
            //        return (*this = other_a);
            //    else
                    return SetByProduct_NoCheck(other_a,other_b);
        }

        //! Set the translation of the current matrix. Will erase any previous values.
        Matrix4& SetTranslation( const Vector3D& translation )
        {
            M[12] = translation.x;
            M[13] = translation.y;
            M[14] = translation.z;

			M[15] = 1.0f;/////////
            //m_bIdentity=false;
            return *this;
        }

        Matrix4& SetTranslation( f32 x, f32 y, f32 z )
        {
            M[12] = x;
            M[13] = y;
            M[14] = z;

            M[15] = 1.0f;/////////
            //m_bIdentity=false;
            return *this;
        }

        //! Gets the current translation
        inline Vector3D GetTranslation() const
        {
            return Vector3D(M[12], M[13], M[14]);
        }

        //! Set the inverse translation of the current matrix. Will erase any previous values.
        Matrix4& SetInverseTranslation( const Vector3D& translation )
        {
            M[12] = -translation.x;
            M[13] = -translation.y;
            M[14] = -translation.z;

			M[15] = 1.0f;/////////
            //m_bIdentity=false;
            return *this;
        }

        //! Make a rotation matrix from Euler angles. The 4th row and column are unmodified.
        inline Matrix4& SetRotationRadians( const Vector3D& rotation )
        {
            const f64 cr = cos( rotation.x );
            const f64 sr = sin( rotation.x );
            const f64 cp = cos( rotation.y );
            const f64 sp = sin( rotation.y );
            const f64 cy = cos( rotation.z );
            const f64 sy = sin( rotation.z );

            M[0] = (f32)( cp*cy );
            M[1] = (f32)( cp*sy );
            M[2] = (f32)( -sp );

            const f64 srsp = sr*sp;
            const f64 crsp = cr*sp;

            M[4] = (f32)( srsp*cy-cr*sy );
            M[5] = (f32)( srsp*sy+cr*cy );
            M[6] = (f32)( sr*cp );

            M[8] = (f32)( crsp*cy+sr*sy );
            M[9] = (f32)( crsp*sy-sr*cy );
            M[10] = (f32)( cr*cp );

			M[15] = 1.0f;/////////
            //m_bIdentity=false;
            return *this;
        }

        //! Make a rotation matrix from Euler angles. The 4th row and column are unmodified.
        Matrix4& SetRotationDegrees( const Vector3D& rotation )
        {
            return SetRotationRadians( rotation * AIR::DEGTORAD );
        }

        //! Returns the rotation, as set by setRotation().
        /** This code was orginally written by by Chev. */
        Vector3D GetRotationDegrees() const
        {
            const Matrix4 &mat = *this;

            f64 y = -asin(mat(0,2));
            const f64 C = cos(y);
            y *= RADTODEG64;

            f64 rotx, roty, x, z;

            if (AIR::Abs(C)>ROUNDING_ERROR_64)
            {
                const f32 invC = (f32)(1.0/C);
                rotx = mat(2,2) * invC;
                roty = mat(1,2) * invC;
                x = atan2( roty, rotx ) * RADTODEG64;
                rotx = mat(0,0) * invC;
                roty = mat(0,1) * invC;
                z = atan2( roty, rotx ) * RADTODEG64;
            }
            else
            {
                x = 0.0;
                rotx = mat(1,1);
                roty = -mat(1,0);
                z = atan2( roty, rotx ) * RADTODEG64;
            }

            // fix values that get below zero
            // before it would set (!) values to 360
            // that where above 360:
            if (x < 0.0) x += 360.0;
            if (y < 0.0) y += 360.0;
            if (z < 0.0) z += 360.0;

            return Vector3D((f32)x,(f32)y,(f32)z);
        }

        //! Make an inverted rotation matrix from Euler angles.
        /** The 4th row and column are unmodified. */
        inline Matrix4& SetInverseRotationRadians( const Vector3D& rotation )
        {
            f64 cr = cos( rotation.x );
            f64 sr = sin( rotation.x );
            f64 cp = cos( rotation.y );
            f64 sp = sin( rotation.y );
            f64 cy = cos( rotation.z );
            f64 sy = sin( rotation.z );

            M[0] = (f32)( cp*cy );
            M[4] = (f32)( cp*sy );
            M[8] = (f32)( -sp );

            f64 srsp = sr*sp;
            f64 crsp = cr*sp;

            M[1] = (f32)( srsp*cy-cr*sy );
            M[5] = (f32)( srsp*sy+cr*cy );
            M[9] = (f32)( sr*cp );

            M[2] = (f32)( crsp*cy+sr*sy );
            M[6] = (f32)( crsp*sy-sr*cy );
            M[10] = (f32)( cr*cp );

			M[15] = 1.0f;/////////
            //m_bIdentity=false;
            return *this;
        }

        //! Make an inverted rotation matrix from Euler angles.
        /** The 4th row and column are unmodified. */
        inline Matrix4& SetInverseRotationDegrees( const Vector3D& rotation )
        {
            return SetInverseRotationRadians( rotation * AIR::DEGTORAD );
        }

		static  Matrix4 GetScaleMatirx4(f32 x,f32 y,f32 z)
		{
			return Matrix4(
				   x, 0.0f, 0.0f, 0.0f, 
				0.0f,    y, 0.0f, 0.0f,
				0.0f, 0.0f,    z, 0.0f, 
				0.0f, 0.0f, 0.0f, 1.0f
				);
		}

		static  Matrix4 GetScaleMatirx4(const Vector3D& s)
		{
			return Matrix4(
				s.x, 0.0f, 0.0f, 0.0f, 
				0.0f,    s.y, 0.0f, 0.0f,
				0.0f, 0.0f,    s.z, 0.0f, 
				0.0f, 0.0f, 0.0f, 1.0f
				);
		}
		inline Matrix4& SetScale( f32 x, f32 y, f32 z )
		{
			M[0] = x;
			M[5] = y;
			M[10] = z;
			M[15] = 1.0f;/////////
			//m_bIdentity=false;
			return *this;
		}

        //! Set Scale
        inline Matrix4& SetScale( const Vector3D& scale )
        {
			return SetScale( scale.x, scale.y, scale.z );
			//M[0] = scale.x;
			//M[5] = scale.y;
			//M[10] = scale.z;

			//M[15] = 1.0f;/////////
			////m_bIdentity=false;
			//return *this;
        }

        //! Set Scale
        inline Matrix4& SetScale( const f32 scale ) 
        { 
            return SetScale(scale,scale,scale); 
        }

		void GetMatrix3(Matrix3& mat) const
		{
			//assert( _14==0 && _24==0 && _34==0 && _44==1.0f );
			//既然调用GetMatrix3那肯定是抛弃位移分量了
			mat._11 = _11, mat._12 = _12, mat._13 = _13;
			mat._21 = _21, mat._22 = _22, mat._23 = _23;
			mat._31 = _31, mat._32 = _32, mat._33 = _33;
		}

        //! Get Scale
        inline AIR::Vector3D GetScale() const
        {
            //return Vector3D(M[0], M[5], M[10]);
			Matrix3 sr;
			Matrix3 srt;
			sr.Identity();
			srt.Identity();

			GetMatrix3(sr);
			srt._11 = sr._11;
			srt._12 = sr._21;
			srt._13 = sr._31;
			srt._21 = sr._12;
			srt._22 = sr._22;
			srt._23 = sr._32;
			srt._31 = sr._13;
			srt._32 = sr._23;
			srt._33 = sr._33;
			sr = sr * srt;

			Vector3D result;
			result.x = sqrt(sr._11);
			result.y = sqrt(sr._22);
			result.z = sqrt(sr._33);

			return result;
        }

        //! Translate a vector by the inverse of the translation part of this matrix.
        void InverseTranslateVect( Vector3Df& vect ) const
        {
            vect.x = vect.x-M[12];
            vect.y = vect.y-M[13];
            vect.z = vect.z-M[14];
        }

        //! Rotate a vector by the inverse of the rotation part of this matrix.
        void InverseRotateVect( Vector3Df& vect ) const
        {
            Vector3Df tmp = vect;
            vect.x = tmp.x*M[0] + tmp.y*M[1] + tmp.z*M[2];
            vect.y = tmp.x*M[4] + tmp.y*M[5] + tmp.z*M[6];
            vect.z = tmp.x*M[8] + tmp.y*M[9] + tmp.z*M[10];
        }

        //! Rotate a vector by the rotation part of this matrix.
        void RotateVect( Vector3Df& vect ) const
        {
            Vector3Df tmp = vect;
            vect.x = tmp.x*M[0] + tmp.y*M[4] + tmp.z*M[8];
            vect.y = tmp.x*M[1] + tmp.y*M[5] + tmp.z*M[9];
            vect.z = tmp.x*M[2] + tmp.y*M[6] + tmp.z*M[10];
        }

        //! An alternate transform vector method, writing into a second vector
        void RotateVect(AIR::Vector3Df& out, const AIR::Vector3Df& in) const
        {
            out.x = in.x*M[0] + in.y*M[4] + in.z*M[8];
            out.y = in.x*M[1] + in.y*M[5] + in.z*M[9];
            out.z = in.x*M[2] + in.y*M[6] + in.z*M[10];
        }

        //! An alternate transform vector method, writing into an array of 3 floats
        void RotateVect(f32 *out,const AIR::Vector3Df &in) const
        {
            out[0] = in.x*M[0] + in.y*M[4] + in.z*M[8];
            out[1] = in.x*M[1] + in.y*M[5] + in.z*M[9];
            out[2] = in.x*M[2] + in.y*M[6] + in.z*M[10];
        }

        //! Transforms the vector by this matrix
        void TransformVect( Vector3Df& vect) const
        {
            f32 vector[3];

            vector[0] = vect.x*M[0] + vect.y*M[4] + vect.z*M[8] + M[12];
            vector[1] = vect.x*M[1] + vect.y*M[5] + vect.z*M[9] + M[13];
            vector[2] = vect.x*M[2] + vect.y*M[6] + vect.z*M[10] + M[14];

            vect.x = vector[0];
            vect.y = vector[1];
            vect.z = vector[2];
        }

        //! Transforms input vector by this matrix and stores result in output vector
        void TransformVect( Vector3Df& out, const Vector3Df& in ) const
        {
            out.x = in.x*M[0] + in.y*M[4] + in.z*M[8] + M[12];
            out.y = in.x*M[1] + in.y*M[5] + in.z*M[9] + M[13];
            out.z = in.x*M[2] + in.y*M[6] + in.z*M[10] + M[14];
        }

        //! An alternate transform vector method, writing into an array of 4 floats
        void TransformVect(f32 *out,const AIR::Vector3Df &in) const
        {
            out[0] = in.x*M[0] + in.y*M[4] + in.z*M[8] + M[12];
            out[1] = in.x*M[1] + in.y*M[5] + in.z*M[9] + M[13];
            out[2] = in.x*M[2] + in.y*M[6] + in.z*M[10] + M[14];
            out[3] = in.x*M[3] + in.y*M[7] + in.z*M[11] + M[15];
        }

        //! Translate a vector by the translation part of this matrix.
        inline void TranslateVect( Vector3Df& vect ) const
        {
            vect.x = vect.x + M[12];
            vect.y = vect.y + M[13];
            vect.z = vect.z + M[14];
        }

        //! Transforms a plane by this matrix
        void TransformPlane( AIR::Plane3D &plane) const
        {
            Vector3Df member;
            // Fully transform the plane member point, i.e. rotate, translate and scale it.
            TransformVect(member, plane.GetMemberPoint());

            Vector3Df normal = plane.Normal;
            normal.Normalize();

            // The normal needs to be rotated and inverse scaled, but not translated.
            const Vector3Df scale = GetScale();

            if(!Equals(scale.x, 0.f) && !Equals(scale.y, 0.f) && !Equals(scale.z, 0.f)
                && (!Equals(scale.x, 1.f) || !Equals(scale.y, 1.f) || !Equals(scale.z, 1.f)))
            {
                // Rotating the vector will also apply the scale, so we have to invert it twice.
                normal /= (scale * scale);
            }

            RotateVect(normal);

            normal.Normalize();
            plane.SetPlane(member, normal);
        }

        //! Transforms a plane by this matrix
        void TransformPlane( const AIR::Plane3D &in, AIR::Plane3D &out) const
        {
            out = in;
            TransformPlane( out );
        }

        //! Transforms a axis aligned bounding box
        /** The result box of this operation may not be accurate at all. For
        correct results, use transformBoxEx() */
        void TransformBox(AIR::AABBBox3D& box) const
        {
            
#if defined ( USE_MATRIX_TEST )
            //if (IsIdentity())
            //    return;
#endif

            //TransformCoord(box.MinEdge, box.MinEdge);
            //TransformCoord(box.MaxEdge, box.MaxEdge);
            //box.Repair();
            //新方法

            Vector3Df points[8];

            box.GetEdges(points);

            Vector3Df minPos;
            TransformCoord(points[0], minPos);
            Vector3Df maxPos = minPos;
            Vector3Df point;
            for (size_t i = 1; i < 8; ++i)
            {
                TransformCoord(points[i], point);
                minPos = Vector3Df::Minimize(minPos, point);
                maxPos = Vector3Df::Maximize(maxPos, point);
            }

            box.Reset(minPos, maxPos);
        }

        //! Transforms a axis aligned bounding box
        /** The result box of this operation should by accurate, but this operation
        is slower than transformBox(). */
        void TransformBoxEx(AIR::AABBBox3D& box) const
        {
#if defined ( USE_MATRIX_TEST )
            if (isIdentity())
                return;
#endif

            const f32 Amin[3] = {box.MinEdge.x, box.MinEdge.y, box.MinEdge.z};
            const f32 Amax[3] = {box.MaxEdge.x, box.MaxEdge.y, box.MaxEdge.z};

            f32 Bmin[3];
            f32 Bmax[3];

            Bmin[0] = Bmax[0] = M[12];
            Bmin[1] = Bmax[1] = M[13];
            Bmin[2] = Bmax[2] = M[14];

            const Matrix4 &m = *this;

            for (u32 i = 0; i < 3; ++i)
            {
                for (u32 j = 0; j < 3; ++j)
                {
                    const f32 a = m(j,i) * Amin[j];
                    const f32 b = m(j,i) * Amax[j];

                    if (a < b)
                    {
                        Bmin[i] += a;
                        Bmax[i] += b;
                    }
                    else
                    {
                        Bmin[i] += b;
                        Bmax[i] += a;
                    }
                }
            }

            box.MinEdge.x = Bmin[0];
            box.MinEdge.y = Bmin[1];
            box.MinEdge.z = Bmin[2];

            box.MaxEdge.x = Bmax[0];
            box.MaxEdge.y = Bmax[1];
            box.MaxEdge.z = Bmax[2];
        }

        //! Multiplies this matrix by a 1x4 matrix
        void MultiplyWith1x4Matrix(f32* matrix) const
        {
            /*
            0  1  2  3
            4  5  6  7
            8  9  10 11
            12 13 14 15
            */

            f32 mat[4];
            mat[0] = matrix[0];
            mat[1] = matrix[1];
            mat[2] = matrix[2];
            mat[3] = matrix[3];

            matrix[0] = M[0]*mat[0] + M[4]*mat[1] + M[8]*mat[2] + M[12]*mat[3];
            matrix[1] = M[1]*mat[0] + M[5]*mat[1] + M[9]*mat[2] + M[13]*mat[3];
            matrix[2] = M[2]*mat[0] + M[6]*mat[1] + M[10]*mat[2] + M[14]*mat[3];
            matrix[3] = M[3]*mat[0] + M[7]*mat[1] + M[11]*mat[2] + M[15]*mat[3];
        }

        //! Calculates inverse of matrix. Slow.
        /** \return Returns false if there is no inverse matrix.*/
        bool Inverse()
        {
            //if (m_bIdentity)
            //    return true;

            Matrix4 temp; // ( EM4CONST_NOTHING );

            if (GetInverse(temp))
            {
                *this = temp;
                return true;
            }

            return false;
        }


        //! Inverts a primitive matrix which only contains a translation and a rotation
        /** \param out: where result matrix is written to. */
        bool GetInversePrimitive ( Matrix4& out ) const
        {
            out.M[0 ] = M[0];
            out.M[1 ] = M[4];
            out.M[2 ] = M[8];
            out.M[3 ] = 0;

            out.M[4 ] = M[1];
            out.M[5 ] = M[5];
            out.M[6 ] = M[9];
            out.M[7 ] = 0;

            out.M[8 ] = M[2];
            out.M[9 ] = M[6];
            out.M[10] = M[10];
            out.M[11] = 0;

            out.M[12] = (f32)-(M[12]*M[0] + M[13]*M[1] + M[14]*M[2]);
            out.M[13] = (f32)-(M[12]*M[4] + M[13]*M[5] + M[14]*M[6]);
            out.M[14] = (f32)-(M[12]*M[8] + M[13]*M[9] + M[14]*M[10]);
            out.M[15] = 1;

            //out.m_bIdentity = m_bIdentity;
            return true;
        }

        //! Gets the inversed matrix of this one
        /** \param out: where result matrix is written to.
        \return Returns false if there is no inverse matrix. */
        bool GetInverse(Matrix4& out) const
        {
            //if ( this->IsIdentity() )
            //{
            //    out = *this;
            //    return true;
            //}

            const Matrix4 &m = *this;

            f32 d = (m(0, 0) * m(1, 1) - m(0, 1) * m(1, 0)) * (m(2, 2) * m(3, 3) - m(2, 3) * m(3, 2)) -
                (m(0, 0) * m(1, 2) - m(0, 2) * m(1, 0)) * (m(2, 1) * m(3, 3) - m(2, 3) * m(3, 1)) +
                (m(0, 0) * m(1, 3) - m(0, 3) * m(1, 0)) * (m(2, 1) * m(3, 2) - m(2, 2) * m(3, 1)) +
                (m(0, 1) * m(1, 2) - m(0, 2) * m(1, 1)) * (m(2, 0) * m(3, 3) - m(2, 3) * m(3, 0)) -
                (m(0, 1) * m(1, 3) - m(0, 3) * m(1, 1)) * (m(2, 0) * m(3, 2) - m(2, 2) * m(3, 0)) +
                (m(0, 2) * m(1, 3) - m(0, 3) * m(1, 2)) * (m(2, 0) * m(3, 1) - m(2, 1) * m(3, 0));

            if( AIR::iszero ( d ) )
                return false;

            d = AIR::reciprocal ( d );

            out(0, 0) = d * (m(1, 1) * (m(2, 2) * m(3, 3) - m(2, 3) * m(3, 2)) +
                m(1, 2) * (m(2, 3) * m(3, 1) - m(2, 1) * m(3, 3)) +
                m(1, 3) * (m(2, 1) * m(3, 2) - m(2, 2) * m(3, 1)));
            out(0, 1) = d * (m(2, 1) * (m(0, 2) * m(3, 3) - m(0, 3) * m(3, 2)) +
                m(2, 2) * (m(0, 3) * m(3, 1) - m(0, 1) * m(3, 3)) +
                m(2, 3) * (m(0, 1) * m(3, 2) - m(0, 2) * m(3, 1)));
            out(0, 2) = d * (m(3, 1) * (m(0, 2) * m(1, 3) - m(0, 3) * m(1, 2)) +
                m(3, 2) * (m(0, 3) * m(1, 1) - m(0, 1) * m(1, 3)) +
                m(3, 3) * (m(0, 1) * m(1, 2) - m(0, 2) * m(1, 1)));
            out(0, 3) = d * (m(0, 1) * (m(1, 3) * m(2, 2) - m(1, 2) * m(2, 3)) +
                m(0, 2) * (m(1, 1) * m(2, 3) - m(1, 3) * m(2, 1)) +
                m(0, 3) * (m(1, 2) * m(2, 1) - m(1, 1) * m(2, 2)));
            out(1, 0) = d * (m(1, 2) * (m(2, 0) * m(3, 3) - m(2, 3) * m(3, 0)) +
                m(1, 3) * (m(2, 2) * m(3, 0) - m(2, 0) * m(3, 2)) +
                m(1, 0) * (m(2, 3) * m(3, 2) - m(2, 2) * m(3, 3)));
            out(1, 1) = d * (m(2, 2) * (m(0, 0) * m(3, 3) - m(0, 3) * m(3, 0)) +
                m(2, 3) * (m(0, 2) * m(3, 0) - m(0, 0) * m(3, 2)) +
                m(2, 0) * (m(0, 3) * m(3, 2) - m(0, 2) * m(3, 3)));
            out(1, 2) = d * (m(3, 2) * (m(0, 0) * m(1, 3) - m(0, 3) * m(1, 0)) +
                m(3, 3) * (m(0, 2) * m(1, 0) - m(0, 0) * m(1, 2)) +
                m(3, 0) * (m(0, 3) * m(1, 2) - m(0, 2) * m(1, 3)));
            out(1, 3) = d * (m(0, 2) * (m(1, 3) * m(2, 0) - m(1, 0) * m(2, 3)) +
                m(0, 3) * (m(1, 0) * m(2, 2) - m(1, 2) * m(2, 0)) +
                m(0, 0) * (m(1, 2) * m(2, 3) - m(1, 3) * m(2, 2)));
            out(2, 0) = d * (m(1, 3) * (m(2, 0) * m(3, 1) - m(2, 1) * m(3, 0)) +
                m(1, 0) * (m(2, 1) * m(3, 3) - m(2, 3) * m(3, 1)) +
                m(1, 1) * (m(2, 3) * m(3, 0) - m(2, 0) * m(3, 3)));
            out(2, 1) = d * (m(2, 3) * (m(0, 0) * m(3, 1) - m(0, 1) * m(3, 0)) +
                m(2, 0) * (m(0, 1) * m(3, 3) - m(0, 3) * m(3, 1)) +
                m(2, 1) * (m(0, 3) * m(3, 0) - m(0, 0) * m(3, 3)));
            out(2, 2) = d * (m(3, 3) * (m(0, 0) * m(1, 1) - m(0, 1) * m(1, 0)) +
                m(3, 0) * (m(0, 1) * m(1, 3) - m(0, 3) * m(1, 1)) +
                m(3, 1) * (m(0, 3) * m(1, 0) - m(0, 0) * m(1, 3)));
            out(2, 3) = d * (m(0, 3) * (m(1, 1) * m(2, 0) - m(1, 0) * m(2, 1)) +
                m(0, 0) * (m(1, 3) * m(2, 1) - m(1, 1) * m(2, 3)) +
                m(0, 1) * (m(1, 0) * m(2, 3) - m(1, 3) * m(2, 0)));
            out(3, 0) = d * (m(1, 0) * (m(2, 2) * m(3, 1) - m(2, 1) * m(3, 2)) +
                m(1, 1) * (m(2, 0) * m(3, 2) - m(2, 2) * m(3, 0)) +
                m(1, 2) * (m(2, 1) * m(3, 0) - m(2, 0) * m(3, 1)));
            out(3, 1) = d * (m(2, 0) * (m(0, 2) * m(3, 1) - m(0, 1) * m(3, 2)) +
                m(2, 1) * (m(0, 0) * m(3, 2) - m(0, 2) * m(3, 0)) +
                m(2, 2) * (m(0, 1) * m(3, 0) - m(0, 0) * m(3, 1)));
            out(3, 2) = d * (m(3, 0) * (m(0, 2) * m(1, 1) - m(0, 1) * m(1, 2)) +
                m(3, 1) * (m(0, 0) * m(1, 2) - m(0, 2) * m(1, 0)) +
                m(3, 2) * (m(0, 1) * m(1, 0) - m(0, 0) * m(1, 1)));
            out(3, 3) = d * (m(0, 0) * (m(1, 1) * m(2, 2) - m(1, 2) * m(2, 1)) +
                m(0, 1) * (m(1, 2) * m(2, 0) - m(1, 0) * m(2, 2)) +
                m(0, 2) * (m(1, 0) * m(2, 1) - m(1, 1) * m(2, 0)));
            //out.m_bIdentity = m_bIdentity;
            return true;
        }

        bool GetInverseAffine(Matrix4& out) const
        {
            const Matrix4 &m = *this;

            f32 d = (m(0, 0) * m(1, 1) - m(0, 1) * m(1, 0)) * (m(2, 2) * m(3, 3) - m(2, 3) * m(3, 2)) -
                (m(0, 0) * m(1, 2) - m(0, 2) * m(1, 0)) * (m(2, 1) * m(3, 3) - m(2, 3) * m(3, 1)) +
                (m(0, 0) * m(1, 3) - m(0, 3) * m(1, 0)) * (m(2, 1) * m(3, 2) - m(2, 2) * m(3, 1)) +
                (m(0, 1) * m(1, 2) - m(0, 2) * m(1, 1)) * (m(2, 0) * m(3, 3) - m(2, 3) * m(3, 0)) -
                (m(0, 1) * m(1, 3) - m(0, 3) * m(1, 1)) * (m(2, 0) * m(3, 2) - m(2, 2) * m(3, 0)) +
                (m(0, 2) * m(1, 3) - m(0, 3) * m(1, 2)) * (m(2, 0) * m(3, 1) - m(2, 1) * m(3, 0));

            if( AIR::iszero ( d ) )
                return false;

            d = AIR::reciprocal ( d );

            out(0, 0) = d * (m(1, 1) * (m(2, 2) * m(3, 3) - m(2, 3) * m(3, 2)) +
                m(1, 2) * (m(2, 3) * m(3, 1) - m(2, 1) * m(3, 3)) +
                m(1, 3) * (m(2, 1) * m(3, 2) - m(2, 2) * m(3, 1)));
            out(0, 1) = d * (m(2, 1) * (m(0, 2) * m(3, 3) - m(0, 3) * m(3, 2)) +
                m(2, 2) * (m(0, 3) * m(3, 1) - m(0, 1) * m(3, 3)) +
                m(2, 3) * (m(0, 1) * m(3, 2) - m(0, 2) * m(3, 1)));
            out(0, 2) = d * (m(3, 1) * (m(0, 2) * m(1, 3) - m(0, 3) * m(1, 2)) +
                m(3, 2) * (m(0, 3) * m(1, 1) - m(0, 1) * m(1, 3)) +
                m(3, 3) * (m(0, 1) * m(1, 2) - m(0, 2) * m(1, 1)));
            out(0, 3) = d * (m(0, 1) * (m(1, 3) * m(2, 2) - m(1, 2) * m(2, 3)) +
                m(0, 2) * (m(1, 1) * m(2, 3) - m(1, 3) * m(2, 1)) +
                m(0, 3) * (m(1, 2) * m(2, 1) - m(1, 1) * m(2, 2)));
            out(1, 0) = d * (m(1, 2) * (m(2, 0) * m(3, 3) - m(2, 3) * m(3, 0)) +
                m(1, 3) * (m(2, 2) * m(3, 0) - m(2, 0) * m(3, 2)) +
                m(1, 0) * (m(2, 3) * m(3, 2) - m(2, 2) * m(3, 3)));
            out(1, 1) = d * (m(2, 2) * (m(0, 0) * m(3, 3) - m(0, 3) * m(3, 0)) +
                m(2, 3) * (m(0, 2) * m(3, 0) - m(0, 0) * m(3, 2)) +
                m(2, 0) * (m(0, 3) * m(3, 2) - m(0, 2) * m(3, 3)));
            out(1, 2) = d * (m(3, 2) * (m(0, 0) * m(1, 3) - m(0, 3) * m(1, 0)) +
                m(3, 3) * (m(0, 2) * m(1, 0) - m(0, 0) * m(1, 2)) +
                m(3, 0) * (m(0, 3) * m(1, 2) - m(0, 2) * m(1, 3)));
            out(1, 3) = d * (m(0, 2) * (m(1, 3) * m(2, 0) - m(1, 0) * m(2, 3)) +
                m(0, 3) * (m(1, 0) * m(2, 2) - m(1, 2) * m(2, 0)) +
                m(0, 0) * (m(1, 2) * m(2, 3) - m(1, 3) * m(2, 2)));
            out(2, 0) = d * (m(1, 3) * (m(2, 0) * m(3, 1) - m(2, 1) * m(3, 0)) +
                m(1, 0) * (m(2, 1) * m(3, 3) - m(2, 3) * m(3, 1)) +
                m(1, 1) * (m(2, 3) * m(3, 0) - m(2, 0) * m(3, 3)));
            out(2, 1) = d * (m(2, 3) * (m(0, 0) * m(3, 1) - m(0, 1) * m(3, 0)) +
                m(2, 0) * (m(0, 1) * m(3, 3) - m(0, 3) * m(3, 1)) +
                m(2, 1) * (m(0, 3) * m(3, 0) - m(0, 0) * m(3, 3)));
            out(2, 2) = d * (m(3, 3) * (m(0, 0) * m(1, 1) - m(0, 1) * m(1, 0)) +
                m(3, 0) * (m(0, 1) * m(1, 3) - m(0, 3) * m(1, 1)) +
                m(3, 1) * (m(0, 3) * m(1, 0) - m(0, 0) * m(1, 3)));
            out(2, 3) = d * (m(0, 3) * (m(1, 1) * m(2, 0) - m(1, 0) * m(2, 1)) +
                m(0, 0) * (m(1, 3) * m(2, 1) - m(1, 1) * m(2, 3)) +
                m(0, 1) * (m(1, 0) * m(2, 3) - m(1, 3) * m(2, 0)));
            out(3, 0) = 0;
            out(3, 1) = 0;
            out(3, 2) = 0;
            out(3, 3) = 1;
            //out.m_bIdentity = m_bIdentity;
            return true;
        }

        //! Builds a right-handed perspective projection matrix based on a field of view
        Matrix4& BuildProjectionMatrixPerspectiveFovRH(f32 fieldOfViewRadians, f32 aspectRatio, f32 zNear, f32 zFar)
        {
            const f64 h = 1.0 / tan(fieldOfViewRadians / 2.0);
            const f32 w = h / aspectRatio;

            M[0] = w;
            M[1] = 0;
            M[2] = 0;
            M[3] = 0;

            M[4] = 0;
            M[5] = (f32)h;
            M[6] = 0;
            M[7] = 0;

            M[8] = 0;
            M[9] = 0;
            M[10] = (f32)(zFar/(zNear-zFar)); // DirectX version
            //		M[10] = (f32)(zFar+zNear/(zNear-zFar)); // OpenGL version
            M[11] = -1;

            M[12] = 0;
            M[13] = 0;
            M[14] = (f32)(zNear*zFar/(zNear-zFar)); // DirectX version
            //		M[14] = (f32)(2.0f*zNear*zFar/(zNear-zFar)); // OpenGL version
            M[15] = 0;
            //m_bIdentity=false;
            return *this;
        }

        //! Builds a left-handed perspective projection matrix based on a field of view
        Matrix4& BuildProjectionMatrixPerspectiveFovLH(f32 fieldOfViewRadians, f32 aspectRatio, f32 zNear, f32 zFar)
        {
            const f64 h = 1.0 / tan(fieldOfViewRadians / 2.0);
            const f32 w = (f32)(h / aspectRatio);

            M[0] = w;
            M[1] = 0;
            M[2] = 0;
            M[3] = 0;

            M[4] = 0;
            M[5] = (f32)h;
            M[6] = 0;
            M[7] = 0;

            M[8] = 0;
            M[9] = 0;
            M[10] = (f32)(zFar/(zFar-zNear));
            M[11] = 1;

            M[12] = 0;
            M[13] = 0;
            M[14] = (f32)(-zNear*zFar/(zFar-zNear));
            M[15] = 0;
            //m_bIdentity=false;
            return *this;
        };

        //! Builds a right-handed perspective projection matrix.
        Matrix4& BuildProjectionMatrixPerspectiveRH(f32 widthOfViewVolume, f32 heightOfViewVolume, f32 zNear, f32 zFar)
        {
            M[0] = (f32)( 2 * zNear / widthOfViewVolume);
            M[1] = 0;
            M[2] = 0;
            M[3] = 0;

            M[4] = 0;
            M[5] = (f32)(2 * zNear / heightOfViewVolume);
            M[6] = 0;
            M[7] = 0;

            M[8] = 0;
            M[9] = 0;
            M[10] = (f32)(zFar / (zNear-zFar) );
            M[11] = -1;

            M[12] = 0;
            M[13] = 0;
            M[14] = (f32)(zNear * zFar / (zNear - zFar));
            M[15] = 0;
            //m_bIdentity = false;
            return *this;
        }

        //! Builds a left-handed perspective projection matrix.
        Matrix4& BuildProjectionMatrixPerspectiveLH(f32 widthOfViewVolume, f32 heightOfViewVolume, f32 zNear, f32 zFar)
        {
            M[0] = (f32)(2 * zNear / widthOfViewVolume);
            M[1] = 0;
            M[2] = 0;
            M[3] = 0;

            M[4] = 0;
            M[5] = (f32)(2 * zNear / heightOfViewVolume);
            M[6] = 0;
            M[7] = 0;

            M[8] = 0;
            M[9] = 0;
            M[10] = (f32)(zFar / (zFar - zNear));
            M[11] = 1;

            M[12] = 0;
            M[13] = 0;
            M[14] = (f32)(zNear * zFar / (zNear - zFar));
            M[15] = 0;
            //m_bIdentity=false;
            return *this;
        }

        //! Builds a left-handed orthogonal projection matrix.
        Matrix4& BuildProjectionMatrixOrthoLH(f32 widthOfViewVolume, f32 heightOfViewVolume, f32 zNear, f32 zFar)
        {
            M[0] = (f32)(2 / widthOfViewVolume);
            M[1] = 0;
            M[2] = 0;
            M[3] = 0;

            M[4] = 0;
            M[5] = (f32)(2 / heightOfViewVolume);
            M[6] = 0;
            M[7] = 0;

            M[8] = 0;
            M[9] = 0;
            M[10] = (f32)(1 / (zFar - zNear));
            M[11] = 0;

            M[12] = 0;
            M[13] = 0;
            M[14] = (f32)(zNear / (zNear - zFar));
            M[15] = 1;
            //m_bIdentity = false;
            return *this;
        }

        Matrix4& BuildProjectionMatrixOrthoOffCenterLH(f32 l, f32 r, f32 b, f32 t, f32 zNear, f32 zFar)
        {
            M[0] = (f32)(2 / (r - l));
            M[1] = 0;
            M[2] = 0;
            M[3] = 0;

            M[4] = 0;
            M[5] = (f32)(2 / (t - b));
            M[6] = 0;
            M[7] = 0;

            M[8] = 0;
            M[9] = 0;
            M[10] = (f32)(1 / (zFar - zNear));
            M[11] = 0;

            M[12] = (l + r) / (l - r);
            M[13] = (t + b) / (b - t);
            M[14] = (f32)(zNear / (zNear - zFar));
            M[15] = 1;
            //m_bIdentity = false;
            return *this;
        }

        Matrix4& BuildProjectionMatrixOrthoOffCenterRH(f32 l, f32 r, f32 b, f32 t, f32 zNear, f32 zFar)
        {
            M[0] = (f32)(2 / (r - l));
            M[1] = 0;
            M[2] = 0;
            M[3] = 0;

            M[4] = 0;
            M[5] = (f32)(2 / (t - b));
            M[6] = 0;
            M[7] = 0;

            M[8] = 0;
            M[9] = 0;
            M[10] = (f32)(1 / (zNear - zFar));
            M[11] = 0;

            M[12] = (l + r) / (l - r);
            M[13] = (t + b) / (b - t);
            M[14] = (f32)(zNear / (zNear - zFar));
            M[15] = 1;
            //m_bIdentity = false;
            return *this;
        }

        //! Builds a right-handed orthogonal projection matrix.
        Matrix4& BuildProjectionMatrixOrthoRH(f32 widthOfViewVolume, f32 heightOfViewVolume, f32 zNear, f32 zFar)
        {
            M[0] = (f32)(2 / widthOfViewVolume);
            M[1] = 0;
            M[2] = 0;
            M[3] = 0;

            M[4] = 0;
            M[5] = (f32)(2 / heightOfViewVolume);
            M[6] = 0;
            M[7] = 0;

            M[8] = 0;
            M[9] = 0;
            M[10] = (f32)(1 / (zNear - zFar));
            M[11] = 0;

            M[12] = 0;
            M[13] = 0;
            M[14] = (f32)(zNear / (zNear - zFar));
            M[15] = -1;
            //m_bIdentity = false;
            return *this;
        }

        //! Builds a left-handed look-at matrix.
        Matrix4& BuildCameraLookAtMatrixLH(
            const Vector3Df& position,
            const Vector3Df& target,
            const Vector3Df& upVector)
        {
            Vector3Df zaxis = target - position;
            zaxis.Normalize();

            Vector3Df xaxis = upVector.CrossProduct(zaxis);
            xaxis.Normalize();

            Vector3Df yaxis = zaxis.CrossProduct(xaxis);

            M[0] = (f32)xaxis.x;
            M[1] = (f32)yaxis.x;
            M[2] = (f32)zaxis.x;
            M[3] = 0;

            M[4] = (f32)xaxis.y;
            M[5] = (f32)yaxis.y;
            M[6] = (f32)zaxis.y;
            M[7] = 0;

            M[8] = (f32)xaxis.z;
            M[9] = (f32)yaxis.z;
            M[10] = (f32)zaxis.z;
            M[11] = 0;

            M[12] = (f32) - xaxis.DotProduct(position);
            M[13] = (f32) - yaxis.DotProduct(position);
            M[14] = (f32) - zaxis.DotProduct(position);
            M[15] = 1;
            //m_bIdentity = false;
            return *this;
        }

        //! Builds a right-handed look-at matrix.
        Matrix4& BuildCameraLookAtMatrixRH(
            const Vector3Df& position,
            const Vector3Df& target,
            const Vector3Df& upVector)
        {
            Vector3Df zaxis = position - target;
            zaxis.Normalize();

            Vector3Df xaxis = upVector.CrossProduct(zaxis);
            xaxis.Normalize();

            Vector3Df yaxis = zaxis.CrossProduct(xaxis);

            M[0] = (f32)xaxis.x;
            M[1] = (f32)yaxis.x;
            M[2] = (f32)zaxis.x;
            M[3] = 0;

            M[4] = (f32)xaxis.y;
            M[5] = (f32)yaxis.y;
            M[6] = (f32)zaxis.y;
            M[7] = 0;

            M[8] = (f32)xaxis.z;
            M[9] = (f32)yaxis.z;
            M[10] = (f32)zaxis.z;
            M[11] = 0;

            M[12] = (f32) - xaxis.DotProduct(position);
            M[13] = (f32) - yaxis.DotProduct(position);
            M[14] = (f32) - zaxis.DotProduct(position);
            M[15] = 1;
            //m_bIdentity=false;
            return *this;
        }

        //! Builds a matrix that flattens geometry into a plane.
        /** \param light: light source
        \param plane: plane into which the geometry if flattened into
        \param point: value between 0 and 1, describing the light source.
        If this is 1, it is a point light, if it is 0, it is a directional light. */
        Matrix4& BuildShadowMatrix(const AIR::Vector3Df& light, AIR::Plane3Df plane, f32 point = 1.0f)
        {
            plane.Normal.Normalize();
            const f32 d = plane.Normal.DotProduct(light);

            M[ 0] = (f32)(-plane.Normal.x * light.x + d);
            M[ 1] = (f32)(-plane.Normal.x * light.y);
            M[ 2] = (f32)(-plane.Normal.x * light.z);
            M[ 3] = (f32)(-plane.Normal.x * point);

            M[ 4] = (f32)(-plane.Normal.y * light.x);
            M[ 5] = (f32)(-plane.Normal.y * light.y + d);
            M[ 6] = (f32)(-plane.Normal.y * light.z);
            M[ 7] = (f32)(-plane.Normal.y * point);

            M[ 8] = (f32)(-plane.Normal.z * light.x);
            M[ 9] = (f32)(-plane.Normal.z * light.y);
            M[10] = (f32)(-plane.Normal.z * light.z + d);
            M[11] = (f32)(-plane.Normal.z * point);

            M[12] = (f32)(-plane.D * light.x);
            M[13] = (f32)(-plane.D * light.y);
            M[14] = (f32)(-plane.D * light.z);
            M[15] = (f32)(-plane.D * point + d);
            //m_bIdentity=false;
            return *this;
        }

        //! Builds a matrix which transforms a normalized Device Coordinate to Device Coordinates.
        /** Used to scale <-1,-1><1,1> to viewport, for example from von <-1,-1> <1,1> to the viewport <0,0><0,640> */
        Matrix4& BuildNDCToDCMatrix( const AIR::Rect& viewport, f32 zScale)
        {
            const f32 scaleX = (viewport.GetWidth() - 0.75f ) / 2.0f;
            const f32 scaleY = -(viewport.GetHeight() - 0.75f ) / 2.0f;

            const f32 dx = -0.5f + ( (viewport.left + viewport.right ) / 2.0f );
            const f32 dy = -0.5f + ( (viewport.top + viewport.bottom ) / 2.0f );

            Identity();
            M[12] = (f32)dx;
            M[13] = (f32)dy;
            return SetScale(AIR::Vector3D((f32)scaleX, (f32)scaleY, (f32)zScale));
        }

        //! Creates a new matrix as interpolated matrix from two other ones.
        /** \param b: other matrix to interpolate with
        \param time: Must be a value between 0 and 1. */
        Matrix4 Interpolate(const AIR::Matrix4& b, f32 time) const
        {
            Matrix4 mat;

            for (u32 i = 0; i < 16; i += 4)
            {
                mat.M[i + 0] = (f32)(M[i+0] + ( b.M[i+0] - M[i + 0] ) * time);
                mat.M[i + 1] = (f32)(M[i+1] + ( b.M[i+1] - M[i + 1] ) * time);
                mat.M[i + 2] = (f32)(M[i+2] + ( b.M[i+2] - M[i + 2] ) * time);
                mat.M[i + 3] = (f32)(M[i+3] + ( b.M[i+3] - M[i + 3] ) * time);
            }
            return mat;
        }

        //! Gets transposed matrix
        Matrix4 GetTransposed() const
        {
            Matrix4 t;;
            GetTransposed ( t );
            return t;
        }

        //! Gets transposed matrix
        inline void GetTransposed( Matrix4& dest ) const
        {
            dest[ 0] = M[ 0];
            dest[ 1] = M[ 4];
            dest[ 2] = M[ 8];
            dest[ 3] = M[12];

            dest[ 4] = M[ 1];
            dest[ 5] = M[ 5];
            dest[ 6] = M[ 9];
            dest[ 7] = M[13];

            dest[ 8] = M[ 2];
            dest[ 9] = M[ 6];
            dest[10] = M[10];
            dest[11] = M[14];

            dest[12] = M[ 3];
            dest[13] = M[ 7];
            dest[14] = M[11];
            dest[15] = M[15];
            //dest.m_bIdentity = m_bIdentity;
        }

        /*
        construct 2D Texture transformations
        rotate about center, scale, and transform.
        */
        //! Set to a texture transformation matrix with the given parameters.
        Matrix4& BuildTextureTransform( f32 rotateRad,
            const AIR::Vector2Df &rotatecenter,
            const AIR::Vector2Df &translate,
            const AIR::Vector2Df &scale)
        {
            const f32 c = cosf(rotateRad);
            const f32 s = sinf(rotateRad);

            M[0] = (f32)(c * scale.x);
            M[1] = (f32)(s * scale.y);
            M[2] = 0;
            M[3] = 0;

            M[4] = (f32)(-s * scale.x);
            M[5] = (f32)(c * scale.y);
            M[6] = 0;
            M[7] = 0;

            M[8] = (f32)(c * scale.x * rotatecenter.x + -s * rotatecenter.y + translate.x);
            M[9] = (f32)(s * scale.y * rotatecenter.x +  c * rotatecenter.y + translate.y);
            M[10] = 1;
            M[11] = 0;

            M[12] = 0;
            M[13] = 0;
            M[14] = 0;
            M[15] = 1;
            //m_bIdentity=false;
            return *this;
        }

        //! Set texture transformation rotation
        /** Rotate about z axis, recenter at (0.5,0.5).
        Doesn't clear other elements than those affected
        \param radAngle Angle in radians
        \return Altered matrix */
        Matrix4& SetTextureRotationCenter( f32 radAngle )
        {
            const f32 c = cosf(radAngle);
            const f32 s = sinf(radAngle);
            M[0] = (f32)c;
            M[1] = (f32)s;

            M[4] = (f32)-s;
            M[5] = (f32)c;

            M[8] = (f32)(0.5f * ( s - c) + 0.5f);
            M[9] = (f32)(-0.5f * ( s + c) + 0.5f);
            //m_bIdentity = m_bIdentity && (radAngle == 0.0f);
            return *this;
        }

        //! Set texture transformation translation
        /** Doesn't clear other elements than those affected.
        \param x Offset on x axis
        \param y Offset on y axis
        \return Altered matrix */
        Matrix4& SetTextureTranslate( f32 x, f32 y )
        {
            M[8] = (f32)x;
            M[9] = (f32)y;
            //m_bIdentity = m_bIdentity && (x==0.0f) && (y==0.0f);
            return *this;
        }

        //! Set texture transformation translation, using a transposed representation
        /** Doesn't clear other elements than those affected.
        \param x Offset on x axis
        \param y Offset on y axis
        \return Altered matrix */
        Matrix4& SetTextureTranslateTransposed( f32 x, f32 y )
        {
            M[2] = (f32)x;
            M[6] = (f32)y;
            //m_bIdentity = m_bIdentity && (x==0.0f) && (y==0.0f) ;
            return *this;
        }

        //! Set texture transformation scale
        /** Doesn't clear other elements than those affected.
        \param sx Scale factor on x axis
        \param sy Scale factor on y axis
        \return Altered matrix. */
        Matrix4& SetTextureScale( f32 sx, f32 sy )
        {
            M[0] = (f32)sx;
            M[5] = (f32)sy;
            //m_bIdentity = m_bIdentity && (sx==1.0f) && (sy==1.0f);
            return *this;
        }

        //! Set texture transformation scale, and recenter at (0.5,0.5)
        /** Doesn't clear other elements than those affected.
        \param sx Scale factor on x axis
        \param sy Scale factor on y axis
        \return Altered matrix. */
        Matrix4& SetTextureScaleCenter( f32 sx, f32 sy )
        {
            M[0] = (f32)sx;
            M[5] = (f32)sy;
            M[8] = (f32)(0.5f - 0.5f * sx);
            M[9] = (f32)(0.5f - 0.5f * sy);
            //m_bIdentity = m_bIdentity && (sx==1.0f) && (sy==1.0f);
            return *this;
        };

        //! Sets all matrix data members at once
        Matrix4& SetM(const f32* data)
        {
            memcpy(M, data, 16 * sizeof(f32));

            //m_bIdentity = false;
            return *this;
        }

        //! Sets if the matrix is definitely identity matrix
        //void SetDefinitelyIdentityMatrix( bool isDefinitelyIdentityMatrix)
        //{
        //    m_bIdentity = isDefinitelyIdentityMatrix;
        //}

        ////! Gets if the matrix is definitely identity matrix
        //bool GetDefinitelyIdentityMatrix() const
        //{
        //    return m_bIdentity;
        //}

        //add by liangairan 坐标转换
        inline Vector3D& TransformCoord(const Vector3D& vCoord, Vector3D& vCoordOut) const
        {
            f32 vector[4];

            vector[0] = vCoord.x * M[0] + vCoord.y * M[4] + vCoord.z * M[8] + M[12];
            vector[1] = vCoord.x * M[1] + vCoord.y * M[5] + vCoord.z * M[9] + M[13];
            vector[2] = vCoord.x * M[2] + vCoord.y * M[6] + vCoord.z * M[10] + M[14];
            vector[3] = vCoord.x * M[3] + vCoord.y * M[7] + vCoord.z * M[11] + M[15];

            if (vector[3] == 0.0)
            {
                vector[3] = 1.0;
            }

            vector[0] /= vector[3];
            vector[1] /= vector[3];
            vector[2] /= vector[3];

            vCoordOut.x = vector[0];
            vCoordOut.y = vector[1];
            vCoordOut.z = vector[2];
            return vCoordOut;
        }

		inline Vector3D DotProduct(const Vector3D& vCoord) const
		{
			Vector3D vCoordOut(0, 0, 0);
			f32 vector[4];

			vector[0] = vCoord.x * M[0] + vCoord.y * M[4] + vCoord.z * M[8] + M[12];
			vector[1] = vCoord.x * M[1] + vCoord.y * M[5] + vCoord.z * M[9] + M[13];
			vector[2] = vCoord.x * M[2] + vCoord.y * M[6] + vCoord.z * M[10] + M[14];
			vector[3] = vCoord.x * M[3] + vCoord.y * M[7] + vCoord.z * M[11] + M[15];

			if (vector[3] == 0.0)
			{
				vector[3] = 1.0;
			}

			vector[0] /= vector[3];
			vector[1] /= vector[3];
			vector[2] /= vector[3];

			vCoordOut.x = vector[0];
			vCoordOut.y = vector[1];
			vCoordOut.z = vector[2];
			return vCoordOut;
		}
        inline void TransformCoordFast(const Vector3D& vCoord, Vector3D& vCoordOut) const
        {
            vCoordOut.x = vCoord.x * M[0] + vCoord.y * M[4] + vCoord.z * M[8] + M[12];
            vCoordOut.y = vCoord.x * M[1] + vCoord.y * M[5] + vCoord.z * M[9] + M[13];
            vCoordOut.z = vCoord.x * M[2] + vCoord.y * M[6] + vCoord.z * M[10] + M[14];
            //vector[3] = vCoord.x * M[3] + vCoord.y * M[7] + vCoord.z * M[11] + M[15];
			//v4sf vout = mulps(_mm_set_ps1( vCoord.x ), *(__m128*)( &M[0] )); 
			//vout = addps( vout, mulps(_mm_set_ps1( vCoord.y ), *(__m128*)( &M[4] )) );
			//vout = addps( vout, mulps(_mm_set_ps1( vCoord.z ), *(__m128*)( &M[8] )) );
			//vCoordOut.x = vout.m128_f32[0];
			//vCoordOut.y = vout.m128_f32[1];
			//vCoordOut.z = vout.m128_f32[2];
        }

        inline void TransformVector4(const Vector4& vCoord, Vector4& vCoordOut) const
        {
            vCoordOut.x = vCoord.x * M[0] + vCoord.y * M[4] + vCoord.z * M[8] + vCoord.w * M[12];
            vCoordOut.y = vCoord.x * M[1] + vCoord.y * M[5] + vCoord.z * M[9] + vCoord.w * M[13];
            vCoordOut.z = vCoord.x * M[2] + vCoord.y * M[6] + vCoord.z * M[10] + vCoord.w * M[14];
            vCoordOut.w = vCoord.x * M[3] + vCoord.y * M[7] + vCoord.z * M[11] + vCoord.w * M[15];
        }

        /*
        void TransformCoord(Vector3D& vOut, const Vector3D& vIn) const
        {
            float w = 1.0;

            vector[0] = vCoord.x * M[0] + vCoord.y * M[4] + vCoord.z * M[8] + 1 * M[12];
            vector[1] = vCoord.x * M[1] + vCoord.y * M[5] + vCoord.z * M[9] + 1 * M[13];
            vector[2] = vCoord.x * M[2] + vCoord.y * M[6] + vCoord.z * M[10] + 1 * M[14];
            w = vCoord.x * M[3] + vCoord.y * M[7] + vCoord.z * M[11] + M[15];

            vector[0] /= vector[3];
            vector[1] /= vector[3];
            vector[2] /= vector[3];

            vCoord.x = vector[0];
            vCoord.y = vector[1];
            vCoord.z = vector[2];
        }
        */

        //! Builds a left-handed look-at matrix.
        Matrix4& BuildCameraLookAtMatrixLHByDirection(
            const Vector3Df& vLook,
            const Vector3Df& vRight,
            const Vector3Df& upVector,
            const Vector3Df& position)
        {
            //Vector3Df vLook = vRight.CrossProduct(upVector);
            //vLook.Normalize();
            //zaxis.Normalize();

            //Vector3Df xaxis = upVector.CrossProduct(zaxis);
            //xaxis.Normalize();

            //Vector3Df yaxis = zaxis.CrossProduct(xaxis);

            M[0] = (f32)vRight.x;
            M[1] = (f32)upVector.x;
            M[2] = (f32)vLook.x;
            M[3] = 0.0f;

            M[4] = (f32)vRight.y;
            M[5] = (f32)upVector.y;
            M[6] = (f32)vLook.y;
            M[7] = 0.0f;

            M[8] = (f32)vRight.z;
            M[9] = (f32)upVector.z;
            M[10] = (f32)vLook.z;
            M[11] = 0.0f;

            M[12] =  -(f32)vRight.DotProduct(position);
            M[13] =  -(f32)upVector.DotProduct(position);
            M[14] =  -(f32)vLook.DotProduct(position);
            M[15] = 1.0f;
            //m_bIdentity = false;
            return *this;
        }

        Matrix4& BuildCameraLookAtMatrixRHByDirection(
            const Vector3Df& vLook,
            const Vector3Df& vRight,
            const Vector3Df& upVector,
            const Vector3Df& position)
        {
            //Vector3Df vLook = vRight.CrossProduct(upVector);
            //vLook.Normalize();
            //zaxis.Normalize();

            //Vector3Df xaxis = upVector.CrossProduct(zaxis);
            //xaxis.Normalize();

            //Vector3Df yaxis = zaxis.CrossProduct(xaxis);

            M[0] = (f32)vRight.x;
            M[1] = (f32)upVector.x;
            M[2] = -(f32)vLook.x;
            M[3] = 0.0f;

            M[4] = (f32)vRight.y;
            M[5] = (f32)upVector.y;
            M[6] = -(f32)vLook.y;
            M[7] = 0.0f;

            M[8] = (f32)vRight.z;
            M[9] = (f32)upVector.z;
            M[10] = -(f32)vLook.z;
            M[11] = 0.0f;

            M[12] =  -(f32)vRight.DotProduct(position);
            M[13] =  -(f32)upVector.DotProduct(position);
            M[14] =  -(f32)vLook.DotProduct(position);
            M[15] = 1.0f;
            //m_bIdentity = false;
            return *this;
        }

        //绕任意向量旋转矩阵
        Matrix4& BuildRotationMatrixByAxisDegrees(const Vector3D& axis, const f32 degree)
        {
            return BuildRotationMatrixByAxisRadians(axis, AIR::DEGTORAD * degree);
        };

        //绕任意向量旋转矩阵
        Matrix4& BuildRotationMatrixByAxisRadians(const Vector3D& axis, const f32 radians)
        {
            if (radians == 0.0)
            {
                return Identity();
            }
            f32 axisX = axis.x;
            f32 axisY = axis.y;
            f32 axisZ = axis.z;

            f32 fCos = (f32)cos(radians);
            f32 fSin = (f32)sin(radians);
            f32 t = 1.0f - fCos;

            M[0] = axisX * axisX * t + fCos;
            M[1] = axisX * axisY * t - axisZ * fSin;
            M[2] = axisX * axisZ * t + axisY * fSin;
			M[3] = 0.0f;

            M[4] = axisX * axisY * t + axisZ * fSin;
            M[5] = axisY * axisY * t + fCos;
            M[6] = axisY * axisZ * t - axisX * fSin;
			M[7] = 0.0f;

            M[8] = axisX * axisZ * t - axisY * fSin;
            M[9] = axisY * axisZ * t + axisX * fSin;
            M[10] = axisZ * axisZ * t + fCos;
			M[11] = 0.0f;

			M[12] = 0;
			M[13] = 0;
			M[14] = 0;
			M[15] = 1;

            //m_bIdentity = false;

            return *this;
        };

        Matrix4& BuildRotationMatrixYRadians(f32 radians)
        {
            f32 fCos = (f32)cos(radians);
            f32 fSin = (f32)sin(radians);

            M[0] = fCos;
            M[1] = 0;
            M[2] = -fSin;

            M[4] = 0.0;
            M[5] = 1.0;
            M[6] = 0.0;

            M[8] = fSin;
            M[9] = 0;
            M[10] = fCos;

            return *this;
        };

        Matrix4& BuildRotationMatrixYDegrees(f32 degrees)
        {
            return BuildRotationMatrixYRadians(AIR::DEGTORAD * degrees);
        }

        Matrix4& BuildRotationMatrixXRadians(f32 radians)
        {
            f32 fCos = (f32)cos(radians);
            f32 fSin = (f32)sin(radians);

            M[0] = 1.0;
            M[1] = 0;
            M[2] = 0;

            M[4] = 0.0;
            M[5] = fCos;
            M[6] = -fSin;

            M[8] = 0;
            M[9] = fSin;
            M[10] = fCos;

            return *this;
        };

        Matrix4& BuildRotationMatrixZRadians(f32 radians)
        {
            f32 fCos = (f32)cos(radians);
            f32 fSin = (f32)sin(radians);

            M[0] = fCos;
            M[1] = -fSin;
            M[2] = 0;

            M[4] = fSin;
            M[5] = fCos;
            M[6] = 0;

            M[8] = 0;
            M[9] = 0;
            M[10] = 1;

            return *this;
        };

        Matrix4& BuildReflectMatrix(const Plane3D& plane)
        {
            Plane3D P = plane;
            P.Normalize();

            M[0] = -2 * P.Normal.x * P.Normal.x + 1;
            M[1] = -2 * P.Normal.y * P.Normal.x;
            M[2] = -2 * P.Normal.z * P.Normal.x;
            M[3] = 0.0f;

            M[4] = -2 * P.Normal.x * P.Normal.y;
            M[5] = -2 * P.Normal.y * P.Normal.y + 1;
            M[6] = -2 * P.Normal.z * P.Normal.y;
            M[7] = 0.0f;

            M[8] = -2 * P.Normal.x * P.Normal.z;
            M[9] = -2 * P.Normal.y * P.Normal.z;
            M[10] = -2 * P.Normal.z * P.Normal.z + 1;
            M[11] = 0.0f;

            M[12] = -2 * P.Normal.x * P.D;
            M[13] = -2 * P.Normal.y * P.D;
            M[14] = -2 * P.Normal.z * P.D;
            M[15] = 1.0f;

            //m_bIdentity = false;

            return *this;
        }

        //! Transforms the vector by this matrix
        inline void TransformNormal( Vector3Df& vect) const
        {
            f32 vector[3];

            vector[0] = vect.x*M[0] + vect.y*M[4] + vect.z*M[8]; // + M[12];
            vector[1] = vect.x*M[1] + vect.y*M[5] + vect.z*M[9]; // + M[13];
            vector[2] = vect.x*M[2] + vect.y*M[6] + vect.z*M[10]; // + M[14];

            vect.x = vector[0];
            vect.y = vector[1];
            vect.z = vector[2];
        }

        //! Transforms input vector by this matrix and stores result in output vector
        inline void TransformNormal( Vector3Df& out, const Vector3Df& in ) const
        {
            out.x = in.x*M[0] + in.y*M[4] + in.z*M[8]; // + M[12];
            out.y = in.x*M[1] + in.y*M[5] + in.z*M[9]; // + M[13];
            out.z = in.x*M[2] + in.y*M[6] + in.z*M[10]; // + M[14];
			//v4sf vout = mulps(_mm_set_ps1( in.x ), *(__m128*)( &M[0] )); 
			//vout = addps( vout, mulps(_mm_set_ps1( in.y ), *(__m128*)( &M[4] )) );
			//vout = addps( vout, mulps(_mm_set_ps1( in.z ), *(__m128*)( &M[8] )) );
			//out.x = vout.m128_f32[0];
			//out.y = vout.m128_f32[1];
			//out.z = vout.m128_f32[2];
        }

        //! An alternate transform vector method, writing into an array of 4 floats
        inline void TransformNormal(f32 *out,const AIR::Vector3Df &in) const
        {
            out[0] = in.x*M[0] + in.y*M[4] + in.z*M[8]; // + M[12];
            out[1] = in.x*M[1] + in.y*M[5] + in.z*M[9]; // + M[13];
            out[2] = in.x*M[2] + in.y*M[6] + in.z*M[10]; // + M[14];
            out[3] = in.x*M[3] + in.y*M[7] + in.z*M[11]; // + M[15];
        }

		friend inline Vector4 operator * ( const Matrix4& mat, const Vector4& v )
		{
			return Vector4( 
				mat[0]*v.x  + mat[1]*v.y  + mat[2]*v.z  + mat[3]*v.w,
				mat[4]*v.x  + mat[5]*v.y  + mat[6]*v.z  + mat[7]*v.w,
				mat[8]*v.x  + mat[9]*v.y  + mat[10]*v.z + mat[11]*v.w,
				mat[12]*v.x + mat[13]*v.y + mat[14]*v.z + mat[15]*v.w );
		}

		inline void MakeShearMatrixByYAxis( f32 xOffSet, f32 zOffset )
		{
			M[ 0] = 1;			M[ 1] = 0;	M[ 2] = 0;			M[ 3] = 0; 
			M[ 4] = xOffSet;	M[ 5] = 1;	M[ 6] = zOffset;	M[ 7] = 0; 
			M[ 8] = 0;			M[ 9] = 0;	M[10] = 1;			M[11] = 0; 
			M[12] = 0;			M[13] = 0;	M[14] = 0;			M[15] = 1; 
		}

        inline void TransformTriangle(Triangle3D& triOut, const Triangle3D& triIn) const
        {
            TransformCoord(triIn.pointA, triOut.pointA);
            TransformCoord(triIn.pointB, triOut.pointB);
            TransformCoord(triIn.pointC, triOut.pointC);
        }

		inline AIR::Vector3Df GetRow3(size_t i) const
		{
			i *= 4;
			return AIR::Vector3Df( M[i], M[i+1], M[i+2] );
		}

		inline AIR::Vector3Df GetColumn3(size_t i) const
		{
			return AIR::Vector3Df( M[i], M[i+4], M[i+8] );
		}

		inline bool ContainsProjection(float epsilon) const
		{
			return !(GetRow4(3) == AIR::Vector4f(0.f, 0.f, 0.f, 1.f));
		}

		inline AIR::Vector4f GetRow4(size_t i) const
		{
			i *= 4;
			return AIR::Vector4f( M[i], M[i+1], M[i+2], M[i+3] );
		}

		inline AIR::Vector4f GetColumn4(size_t i) const
		{
			return AIR::Vector4f( M[i], M[i+4], M[i+8], M[i+12] );
		}

		inline f32 GetCell(size_t row, size_t col )
		{
			return _M[row][col];
		};
		inline void SetColumn(size_t i, float x, float y, float z, float w)
		{
			_M[0][i] = x;
			_M[1][i] = y;
			_M[2][i] = z;
			_M[3][i] = w;
		}

		inline void Apply4x4(AIR::Vector3Df &dest, const AIR::Vector3Df &src) const
		{
			f32 w, oneOverW;
			f32 x, y, z;

			w = src.x * M[3] + src.y * M[7] + src.z * M[11] + M[15];
			oneOverW = 1.0f / w;

			x = (src.x * M[0] + src.y * M[4] + src.z * M[8] + M[12]) * oneOverW;
			y = (src.x * M[1] + src.y * M[5] + src.z * M[9] + M[13]) * oneOverW;
			z = (src.x * M[2] + src.y * M[6] + src.z * M[10] + M[14]) * oneOverW;

			dest.x = x, dest.y = y, dest.z = z;
		}

		void MakeTransform(const Vector3D& position, const Vector3D& scale, const Quaternion& orientation);

    private:
        //! Matrix data, stored in row-major order
        //f32 M[16];
		union
		{
			struct
			{
				f32 _11, _12, _13, _14;
				f32 _21, _22, _23, _24;
				f32 _31, _32, _33, _34;
				f32 _41, _42, _43, _44;
			};
			f32			_M[4][4];
			f32			M[16];
		};

        //bool m_bIdentity;

    public:
        static const Matrix4 IDENTITY;
        static const Matrix4 ZERO;
        static const Matrix4 CLIPSPACE2DTOIMAGESPACE;
    };

    typedef Matrix4 Matrix4f;

    //const Matrix4f Matrix4f::IDENTITY(1, 0, 0, 0,
    //                                  0, 1, 0, 0,
    //                                  0, 0, 1, 0,
    //                                  0, 0, 0, 1);

    //const Matrix4f Matrix4f::ZERO(
    //    0, 0, 0, 0,
    //    0, 0, 0, 0,
    //    0, 0, 0, 0,
    //    0, 0, 0, 0);

    //const Matrix4f Matrix4f::CLIPSPACE2DTOIMAGESPACE(
    //    0.5,    0,    0,   0, 
    //    0,   -0.5,    0,   0, 
    //    0,      0,    1,   0,
    //    0.5,    0.5,  0,   1);


	inline Vector3Df operator *(const Vector3Df &vec, const Matrix4f &mat)
	{
		return Vector3Df(
			vec.x * mat[0] + vec.y * mat[4] + vec.z * mat[8] + mat[12],
			vec.x * mat[1] + vec.y * mat[5] + vec.z * mat[9] + mat[13],
			vec.x * mat[2] + vec.y * mat[6] + vec.z * mat[10] + mat[14]);
	}
};



