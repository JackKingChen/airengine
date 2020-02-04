#include "Matrix4.h"
#include "Quaternion.h"

namespace AIR
{
	const Matrix4f Matrix4f::IDENTITY = Matrix4f(1, 0, 0, 0,
	                                  0, 1, 0, 0,
	                                  0, 0, 1, 0,
	                                  0, 0, 0, 1);

	const Matrix4f Matrix4f::ZERO = Matrix4f(
	    0, 0, 0, 0,
	    0, 0, 0, 0,
	    0, 0, 0, 0,
	    0, 0, 0, 0);

	const Matrix4f Matrix4f::CLIPSPACE2DTOIMAGESPACE = Matrix4f(
	    0.5,    0,    0,   0, 
	    0,   -0.5,    0,   0, 
	    0,      0,    1,   0,
	    0.5,    0.5,  0,   1);

	void Matrix4f::MakeTransform(const Vector3D& position, const Vector3D& scale, const Quaternion& orientation)
	{
		// Ordering:
		//    1. Scale
		//    2. Rotate
		//    3. Translate

		Matrix3 rot3x3;
		orientation.ToRotationMatrix(rot3x3);

		// Set up final matrix with scale, rotation and translation
		_M[0][0] = scale.x * rot3x3[0][0]; _M[0][1] = scale.y * rot3x3[0][1]; _M[0][2] = scale.z * rot3x3[0][2]; _M[0][3] = position.x;
		_M[1][0] = scale.x * rot3x3[1][0]; _M[1][1] = scale.y * rot3x3[1][1]; _M[1][2] = scale.z * rot3x3[1][2]; _M[1][3] = position.y;
		_M[2][0] = scale.x * rot3x3[2][0]; _M[2][1] = scale.y * rot3x3[2][1]; _M[2][2] = scale.z * rot3x3[2][2]; _M[2][3] = position.z;

		// No projection term
		_M[3][0] = 0; _M[3][1] = 0; _M[3][2] = 0; _M[3][3] = 1;
	}

}