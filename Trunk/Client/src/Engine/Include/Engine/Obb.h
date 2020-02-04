
#pragma once
#include "MyMath.h"
#include "Vector3D.h"
#include "Line3D.h"
#include "Plane3D.h"
#include "Matrix4.h"
#include "AABBBox3D.h"
#include "Quaternion.h"
#include <assert.h>
#include <vector>
#define UNREF_PARAM(x) (void)(x)

#define FLOAT_NAN std::numeric_limits<float>::quiet_NaN()
#define FLOAT_INF std::numeric_limits<float>::infinity()
#define FLOAT_MAX std::numeric_limits<float>::max()
namespace AIR
{





	inline Quaternion unit_axis_to_unit_axis(Vector3Df const & from, Vector3Df const & to)
	{
		f32 const cos_theta = (from.DotProduct(to) );
		if (cos_theta >= 0.999999999f && cos_theta <= 1)//=1
		{
			Quaternion q;
			q.Identity();
			return q;
		}
		else
		{
			if (cos_theta <= -0.999999999f && cos_theta >= -1)//=-1
			{
				return Quaternion(1, 0, 0, 0);
			}
			else
			{
				
				Vector3Df axis = from.CrossProduct(to).Normalize();

				f32 const sin_theta = sqrt(1 - cos_theta * cos_theta);
				f32 const sin_half_theta = sqrt((1 - cos_theta) / 2);
				f32 const cos_half_theta = sin_theta / (2 * sin_half_theta);

				axis = axis*sin_half_theta;
				return Quaternion(axis.x,axis.y,axis.z, cos_half_theta);
			}
		}
	}

	inline Vector3Df transform_quat(Vector3Df const & v, Quaternion const & quat)
	{
		// result = av + bq + c(q.v CROSS v)
		// where
		//  a = q.w()^2 - (q.v DOT q.v)
		//  b = 2 * (q.v DOT v)
		//  c = 2q.w()

		f32 const a(quat.w * quat.w - quat.v().DotProduct(quat.v()));
		f32 const b(2 * (quat.v().DotProduct(v) ));
		f32 const c(quat.w + quat.w);

		// Must store this, because result may alias v
		Vector3Df cross_v((quat.v().CrossProduct( v)));		// q.v CROSS v

		return Vector3Df(a * v.x + b * quat.x + c * cross_v.x,
			a * v.y + b * quat.y + c * cross_v.y,
			a * v.z + b * quat.z + c * cross_v.z);
	}


	class Vector3D;

	template <typename f32>
	class vector;

	class OBBox
	{
	public:
		OBBox()
			: extent_(0, 0, 0)
		{
		}
		OBBox(
			Vector3Df  const & center,
			Vector3Df  const & x_axis, 
			Vector3Df  const & y_axis, 
			Vector3Df  const & z_axis,
			Vector3Df  const & extent)
			: center_(center), extent_(extent)
		{
			UNREF_PARAM(x_axis);
			UNREF_PARAM(y_axis);
			rotation_ = unit_axis_to_unit_axis(Vector3Df (0, 0, 1), z_axis);
		}
		OBBox(
			Vector3Df  const & center,
			Quaternion const & rotation,
			Vector3Df  const & extent)
			: center_(center), rotation_(rotation), extent_(extent)
		{
		}

		
		OBBox(OBBox const & rhs)
			: 
			center_(rhs.center_), rotation_(rhs.rotation_), extent_(rhs.extent_)
		{
		}
		
		void  compute_obbox(AIRVector<Vector3Df>::iterator first, AIRVector<Vector3Df>::iterator last)
		{
			

			// Compute the mean of the points.
			Vector3Df center = *first;
			AIRVector<Vector3Df>::iterator iter = first;
			++ iter;
			f32 n = 1;
			for (; iter != last; ++ iter, ++ n)
			{
				center += Vector3Df(*iter);
			}
			f32 inv_num_points = f32(1) / n;
			center *= inv_num_points;

			// Compute the covariance matrix of the points.
			f32 cov[6];
			for (int i = 0; i < 6; ++ i)
			{
				cov[i] = 0;
			}

			for (AIRVector<Vector3Df>::iterator iter = first; iter != last; ++ iter)
			{
				Vector3Df diff = Vector3Df(*iter) - center;
				cov[0] += diff[0] * diff[0];
				cov[1] += diff[0] * diff[1];
				cov[2] += diff[0] * diff[2];
				cov[3] += diff[1] * diff[1];
				cov[4] += diff[1] * diff[2];
				cov[5] += diff[2] * diff[2];
			}

			for (int i = 0; i < 6; ++ i)
			{
				cov[i] *= inv_num_points;
			}

			// Tridiagonal

			f32 diagonal[3];
			f32 sub_diagonal[3];
			f32 matrix[3][3];
			bool is_rotation = false;

			f32 m00 = cov[0];
			f32 m01 = cov[1];
			f32 m02 = cov[2];
			f32 m11 = cov[3];
			f32 m12 = cov[4];
			f32 m22 = cov[5];

			diagonal[0] = m00;
			sub_diagonal[2] = 0;
			if (AIR::Abs(m02) > f32(1e-6))
			{
				f32 length = sqrt(m01 * m01 + m02 * m02);
				f32 inv_length = 1 / length;
				m01 *= inv_length;
				m02 *= inv_length;
				f32 q = 2 * m01 * m12 + m02 * (m22 - m11);
				diagonal[1] = m11 + m02 * q;
				diagonal[2] = m22 - m02 * q;
				sub_diagonal[0] = length;
				sub_diagonal[1] = m12 - m01 * q;
				matrix[0][0] = 1;
				matrix[0][1] = 0;
				matrix[0][2] = 0;
				matrix[1][0] = 0;
				matrix[1][1] = m01;
				matrix[1][2] = m02;
				matrix[2][0] = 0;
				matrix[2][1] = m02;
				matrix[2][2] = -m01;
				is_rotation = false;
			}
			else
			{
				diagonal[1] = m11;
				diagonal[2] = m22;
				sub_diagonal[0] = m01;
				sub_diagonal[1] = m12;
				matrix[0][0] = 1;
				matrix[0][1] = 0;
				matrix[0][2] = 0;
				matrix[1][0] = 0;
				matrix[1][1] = 1;
				matrix[1][2] = 0;
				matrix[2][0] = 0;
				matrix[2][1] = 0;
				matrix[2][2] = 1;
				is_rotation = true;
			}

			// QLAlgorithm

			int const nIterPower = 32;

			for (int i0 = 0; i0 < 3; ++ i0)
			{
				int i1;
				for (i1 = 0; i1 < nIterPower; ++ i1)
				{
					int i2;
					for (i2 = i0; i2 <= 3 - 2; ++ i2)
					{
						f32 tmp = AIR::Abs(diagonal[i2]) + AIR::Abs(diagonal[i2+1]);

						if (AIR::Abs(sub_diagonal[i2]) + tmp == tmp)
						{
							break;
						}
					}
					if (i2 == i0)
					{
						break;
					}

					f32 value0 = (diagonal[i0 + 1] - diagonal[i0]) / (2 * sub_diagonal[i0]);
					f32 value1 = sqrt(value0 * value0 + 1);
					if (value0 < 0)
					{
						value0 = diagonal[i2] - diagonal[i0] + sub_diagonal[i0] / (value0 - value1);
					}
					else
					{
						value0 = diagonal[i2] - diagonal[i0] + sub_diagonal[i0] / (value0 + value1);
					}

					f32 sn = 1, cs = 1, value2 = 0;
					for (int i3 = i2 - 1; i3 >= i0; -- i3)
					{
						f32 value3 = sn * sub_diagonal[i3];
						f32 value4 = cs * sub_diagonal[i3];
						if (AIR::Abs(value3) >= AIR::Abs(value0))
						{
							cs = value0 / value3;
							value1 = sqrt(cs * cs + 1);
							sub_diagonal[i3 + 1] = value3 * value1;
							sn = 1 / value1;
							cs *= sn;
						}
						else
						{
							sn = value3 / value0;
							value1 = sqrt(sn * sn + 1);
							sub_diagonal[i3 + 1] = value0 * value1;
							cs = 1 / value1;
							sn *= cs;
						}
						value0 = diagonal[i3 + 1] - value2;
						value1 = (diagonal[i3] - value0) * sn + 2 * value4 * cs;
						value2 = sn * value1;
						diagonal[i3 + 1] = value0 + value2;
						value0 = cs * value1 - value4;

						for (int i4 = 0; i4 < 3; ++ i4)
						{
							value3 = matrix[i4][i3 + 1];
							matrix[i4][i3 + 1] = sn * matrix[i4][i3] + cs * value3;
							matrix[i4][i3] = cs * matrix[i4][i3] - sn * value3;
						}
					}
					diagonal[i0] -= value2;
					sub_diagonal[i0] = value0;
					sub_diagonal[i2] = 0;
				}
			}

			// IncreasingSort

			// Sort the eigenvalues in increasing order, e[0] <= ... <= e[mSize-1]
			for (int i0 = 0, i1; i0 <= 3 - 2; ++ i0)
			{ 
				// Locate the minimum eigenvalue.
				i1 = i0;
				float min_value = diagonal[i1];
				int i2;
				for (i2 = i0 + 1; i2 < 3; ++ i2)
				{
					if (diagonal[i2] < min_value)
					{
						i1 = i2;
						min_value = diagonal[i1];
					}
				}

				if (i1 != i0)
				{
					// Swap the eigenvalues.
					diagonal[i1] = diagonal[i0];
					diagonal[i0] = min_value;

					// Swap the eigenvectors corresponding to the eigenvalues.
					for (i2 = 0; i2 < 3; ++ i2)
					{
						f32 tmp = matrix[i2][i0];
						matrix[i2][i0] = matrix[i2][i1];
						matrix[i2][i1] = tmp;
						is_rotation = !is_rotation;
					}
				}
			}

			// GuaranteeRotation

			if (!is_rotation)
			{
				// Change sign on the first column.
				for (int row = 0; row < 3; ++ row)
				{
					matrix[row][0] = -matrix[row][0];
				}
			}

			/*Vector_T<value_type, 3> axis[3];
			Vector_T<value_type, 3> extent;
			for (int i = 0; i < 3; ++ i)
			{
			extent[i] = diagonal[i];
			for (int row = 0; row < 3; ++row)
			{
			axis[i][row] = matrix[row][i];
			}
			}*/
			Vector3Df axis[3];
			Vector3Df extent;
			extent.x = diagonal[0];
			extent.y = diagonal[1];
			extent.z = diagonal[2];
			for (int i = 0; i < 3; ++ i)
			{
				axis[i].x = matrix[0][i];
				axis[i].y = matrix[1][i];
				axis[i].z = matrix[2][i];
			}

			

			// Let C be the box center and let U0, U1, and U2 be the box axes.  Each
			// input point is of the form X = C + y0*U0 + y1*U1 + y2*U2.  The
			// following code computes min(y0), max(y0), min(y1), max(y1), min(y2),
			// and max(y2).  The box center is then adjusted to be
			//   C' = C + 0.5*(min(y0)+max(y0))*U0 + 0.5*(min(y1)+max(y1))*U1 +
			//        0.5*(min(y2)+max(y2))*U2

			//extent_.DotProduct(extent_)
			Vector3Df diff = Vector3Df(*first) - center;
			Vector3Df pmin((diff.DotProduct(axis[0])), (diff.DotProduct(axis[1])), (diff.DotProduct(axis[2])));
			Vector3Df pmax = pmin;
			iter = first;
			++ iter;
			for (; iter != last; ++ iter)
			{
				diff = Vector3Df(*iter) - center;
				for (int j = 0; j < 3; ++j)
				{
					float d = (diff.DotProduct(axis[j]));
					if (d < pmin[j])
					{
						if (j == 0)
						{
							pmin.x = d;
						}
						else if (j== 1)
						{
							pmin.y = d;
						}
						else
						{
							pmin.z = d;
						}
					}
					else if (d > pmax[j])
					{
						if (j == 0 )
						{
							pmax.x = d;
						}
						else if (j == 1)
						{
							pmax.y = d;
						}
						else 
						{
							pmax.z = d;
						}
					}
				}
			}

			center += (f32(0.5) * (pmin[0] + pmax[0])) * axis[0]
			+ (f32(0.5) * (pmin[1] + pmax[1])) * axis[1]
			+ (f32(0.5) * (pmin[2] + pmax[2])) * axis[2];

			extent.x = f32(0.5) * (pmax[0] - pmin[0]);
			extent.y = f32(0.5) * (pmax[1] - pmin[1]);
			extent.z = f32(0.5) * (pmax[2] - pmin[2]);


			center_ = center;
			extent_ = extent;

			UNREF_PARAM(axis[0]);
			UNREF_PARAM(axis[1]);
			rotation_ = unit_axis_to_unit_axis(Vector3Df (0, 0, 1), axis[2].Normalize());

			//return OBBox<f32>(center, axis[0], axis[1], axis[2], extent);
		}

		OBBox& operator+=(Vector3Df  const & rhs)
		{
			center_ += rhs;
			return *this;
		}
		OBBox& operator-=(Vector3Df  const & rhs)
		{
			center_ -= rhs;
			return *this;
		}
		OBBox& operator*=(f32 const & rhs)
		{
			extent_ *= rhs;
			return *this;
		}
		OBBox& operator/=(f32 const & rhs)
		{
			return this->operator*=(1.0f / rhs);
		}

		OBBox& operator=(OBBox const & rhs)
		{
			if (this != &rhs)
			{
				center_ = rhs.center_;
				rotation_ = rhs.rotation_;
				extent_ = rhs.extent_;
			}
			return *this;
		}

		OBBox const operator+() const
		{
			return *this;
		}
		OBBox const operator-() const
		{
			OBBox ret;
			ret.center_ = -center_;
			ret.rotation_ = -rotation_;
			ret.extent_ = extent_;
			return ret;
		}

		bool IsEmpty() const
		{
			return extent_.DotProduct(extent_) < f32(1e-6);
		}

		bool VecInBound(Vector3Df  const & v) const
		{
			Vector3Df d = v - Center();
			return (DotProduct(d, Axis(0)) <= HalfSize().x)
				&& (DotProduct(d, Axis(1)) <= HalfSize().y)
				&& (DotProduct(d, Axis(2)) <= HalfSize().z);
		}
		f32 MaxRadiusSq() const
		{
			return extent_.DotProduct(extent_);
		}

		Vector3Df  const & Center() const
		{
			return center_;
		}
		Quaternion const & Rotation() const
		{
			return rotation_;
		}
		Vector3Df  Axis(unsigned long index) const
		{
			Vector3Df  v(0, 0, 0);

			if (index == 0)
			{
				v.x = 1;
				return transform_quat(v, rotation_);
			}
			else if (index == 1)
			{
				v.y = 1;
				return transform_quat(v, rotation_);
			}
			else 
			{
				v.z = 1;
				return transform_quat(v, rotation_);
			}

		}
		//用于测试，返回局部空间中轴的末端
		Vector3Df  AxisLine(unsigned long index) const
		{
			Vector3Df  v(0, 0, 0);
			if (index == 0)
			{
				v.x = 1;
				Vector3Df  l_center(center_.x,center_.y,center_.z);
				Vector3Df  l_extent(extent_.x,extent_.y,extent_.z);
				return l_center + transform_quat(v, rotation_)*l_extent.x ;
			}
			else if (index == 1)
			{
				v.y = 1;
				Vector3Df  l_center(center_.x,center_.y,center_.z);
				Vector3Df  l_extent(extent_.x,extent_.y,extent_.z);
				return l_center + transform_quat(v, rotation_)*l_extent.y ;
			}
			else 
			{
				v.z = 1;
				Vector3Df  l_center(center_.x,center_.y,center_.z);
				Vector3Df  l_extent(extent_.x,extent_.y,extent_.z);
				return l_center + transform_quat(v, rotation_)*l_extent.z ;
			}

		
		}

		Vector3Df  const & HalfSize() const
		{
			return extent_;
		}

		bool Intersect(AABBBox3Df const & lhs) const
		{
			return Intersect(OBBox(lhs.GetCenter(), Quaternion::IDENEITY, lhs.GetHalfSize()));
		}
		bool Intersect(OBBox const & obb) const
		{
			// From Real-Time Collision Detection, p. 101-106. See http://realtimecollisiondetection.net/

			f32 epsilon = f32(1e-3);

			//Matrix4 r_mat = Matrix4::Identity();
            Matrix4 r_mat = Matrix4::IDENTITY;
			for (int i = 0; i < 3; ++ i)
			{
				for (int j = 0; j < 3; ++ j)
				{
					r_mat(i, j) = Axis(i).DotProduct(obb.Axis(j));
				}
			}

			Vector3Df t = obb.Center() - Center();
			t = Vector3Df(t.DotProduct( Axis(0)), t.DotProduct(Axis(1)),t.DotProduct( Axis(2)));

			Matrix4 abs_r_mat = Matrix4::IDENTITY;
			for (int i = 0; i < 3; ++ i)
			{
				for (int j = 0; j < 3; ++ j)
				{
					abs_r_mat(i, j) = AIR::Abs(r_mat.GetCell(i, j)) + epsilon;
				}
			}

			Vector3Df const & lr = HalfSize();
			Vector3Df const & rr = obb.HalfSize();

			// Test the three major axes of this OBB.
			for (int i = 0; i < 3; ++ i)
			{
				f32 ra = lr.GetElement(i);
				f32 rb = rr.x * abs_r_mat(i, 0) +  rr.y * abs_r_mat(i, 1) + rr.z * abs_r_mat(i, 2);
				if (AIR::Abs(t.GetElement(i)) > ra + rb) 
				{
					return false;
				}
			}

			// Test the three major axes of the OBB b.
			for (int i = 0; i < 3; ++ i)
			{
				f32 ra = lr.x * abs_r_mat(0, i) + lr.y * abs_r_mat(1, i) + lr.z * abs_r_mat(2, i);
				f32 rb = rr.GetElement(i);
				if (AIR::Abs(t.x + r_mat.GetCell(0, i) + t.y * r_mat.GetCell(1, i) + t.z * r_mat.GetCell(2, i)) > ra + rb)
				{
					return false;
				}
			}

			// Test the 9 different cross-axes.

			// A.x <cross> B.x
			f32 ra = lr.y * abs_r_mat(2, 0) + lr.z * abs_r_mat(1, 0);
			f32 rb = rr.y * abs_r_mat(0, 2) + rr.z * abs_r_mat(0, 1);
			if (AIR::Abs(t.z * r_mat.GetCell(1, 0) - t.y * r_mat.GetCell(2, 0)) > ra + rb)
			{
				return false;
			}

			// A.x < cross> B.y
			ra = lr.y * abs_r_mat(2, 1) + lr.z * abs_r_mat(1, 1);
			rb = rr.x * abs_r_mat(0, 2) + rr.z * abs_r_mat(0, 0);
			if (AIR::Abs(t.z * r_mat.GetCell(1, 1) - t.y * r_mat.GetCell(2, 1)) > ra + rb)
			{
				return false;
			}

			// A.x <cross> B.z
			ra = lr.y * abs_r_mat(2, 2) + lr.z * abs_r_mat(1, 2);
			rb = rr.x * abs_r_mat(0, 1) + rr.y * abs_r_mat(0, 0);
			if (AIR::Abs(t.z * r_mat.GetCell(1, 2) - t.y * r_mat.GetCell(2, 2)) > ra + rb)
			{
				return false;
			}

			// A.y <cross> B.x
			ra = lr.x * abs_r_mat(2, 0) + lr.z * abs_r_mat(0, 0);
			rb = rr.y * abs_r_mat(1, 2) + rr.z * abs_r_mat(1, 1);
			if (AIR::Abs(t.x * r_mat.GetCell(2, 0) - t.z * r_mat.GetCell(0, 0)) > ra + rb)
			{
				return false;
			}

			// A.y <cross> B.y
			ra = lr.x * abs_r_mat(2, 1) + lr.z * abs_r_mat(0, 1);
			rb = rr.x * abs_r_mat(1, 2) + rr.z * abs_r_mat(1, 0);
			if (AIR::Abs(t.x * r_mat.GetCell(2, 1) - t.z * r_mat.GetCell(0, 1)) > ra + rb)
			{
				return false;
			}

			// A.y <cross> B.z
			ra = lr.x * abs_r_mat(2, 2) + lr.z * abs_r_mat(0, 2);
			rb = rr.x * abs_r_mat(1, 1) + rr.y * abs_r_mat(1, 0);
			if (AIR::Abs(t.x * r_mat.GetCell(2, 2) - t.z * r_mat.GetCell(0, 2)) > ra + rb)
			{
				return false;
			}

			// A.z <cross> B.x
			ra = lr.x * abs_r_mat(1, 0) + lr.y * abs_r_mat(0, 0);
			rb = rr.y * abs_r_mat(2, 2) + rr.z * abs_r_mat(2, 1);
			if (AIR::Abs(t.y * r_mat.GetCell(0, 0) - t.x * r_mat.GetCell(1, 0)) > ra + rb)
			{
				return false;
			}

			// A.z <cross> B.y
			ra = lr.x * abs_r_mat(1, 1) + lr.y * abs_r_mat(0, 1);
			rb = rr.x * abs_r_mat(2, 2) + rr.z * abs_r_mat(2, 0);
			if (AIR::Abs(t.y * r_mat.GetCell(0, 1) - t.x * r_mat.GetCell(1, 1)) > ra + rb)
			{
				return false;
			}

			// A.z <cross> B.z
			ra = lr.x * abs_r_mat(1, 2) + lr.y * abs_r_mat(0, 2);
			rb = rr.x * abs_r_mat(2, 1) + rr.y * abs_r_mat(2, 0);
			if (AIR::Abs(t.y * r_mat.GetCell(0, 2) - t.x * r_mat.GetCell(1, 2)) > ra + rb)
			{
				return false;
			}

			return true;
		}
		/*bool Intersect(Sphere_T const & sphere) const
		{
			return MathLib::intersect_obb_sphere(*this, sphere);
		}*/
		/*bool Intersect(Frustum_T const & frustum) const
		{
			return MathLib::intersect_obb_frustum(*this, frustum) != BO_No;
		}*/

		friend bool
			operator==(OBBox const & lhs, OBBox const & rhs)
		{
			return (lhs.center_ == rhs.center_)
				&& (lhs.rotation_ == rhs.rotation_)
				&& (rhs.extent_ == rhs.extent_);
		}

	public:
		Vector3Df  center_;
		Quaternion rotation_;
		Vector3Df  extent_;
	};


	typedef OBBox OBBoxf;

}
