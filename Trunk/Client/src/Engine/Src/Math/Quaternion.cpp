#include "stdafx.h"
#include "Quaternion.h"

namespace AIR
{
	const float Quaternion::ms_fEpsilon(1e-03);
	const Quaternion Quaternion::ZERO(0, 0, 0, 0);
	const Quaternion Quaternion::IDENEITY(0, 0, 0, 1);

	void Quaternion::ToRotationMatrix(Matrix3& mat) const
	{
		f32 fTx = x + x;
		f32 fTy = y + y;
		f32 fTz = z + z;
		f32 fTwx = fTx*w;
		f32 fTwy = fTy*w;
		f32 fTwz = fTz*w;
		f32 fTxx = fTx*x;
		f32 fTxy = fTy*x;
		f32 fTxz = fTz*x;
		f32 fTyy = fTy*y;
		f32 fTyz = fTz*y;
		f32 fTzz = fTz*z;

		mat[0][0] = 1.0f - (fTyy + fTzz);
		mat[0][1] = fTxy - fTwz;
		mat[0][2] = fTxz + fTwy;
		mat[1][0] = fTxy + fTwz;
		mat[1][1] = 1.0f - (fTxx + fTzz);
		mat[1][2] = fTyz - fTwx;
		mat[2][0] = fTxz - fTwy;
		mat[2][1] = fTyz + fTwx;
		mat[2][2] = 1.0f - (fTxx + fTyy);
	}
}