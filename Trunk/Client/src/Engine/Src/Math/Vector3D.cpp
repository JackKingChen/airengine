#include "Vector3D.h"

namespace AIR
{
	const Vector3D Vector3D::ZERO = Vector3D(0, 0, 0);
	const Vector3D Vector3D::UNIT_X = Vector3D(1, 0, 0);
	const Vector3D Vector3D::UNIT_Y = Vector3D(0, 1, 0);
	const Vector3D Vector3D::UNIT_Z = Vector3D(0, 0, 1);
	const Vector3D Vector3D::NEGATIVE_UNIT_X = Vector3D(-1, 0, 0);
	const Vector3D Vector3D::NEGATIVE_UNIT_Y = Vector3D(0, -1, 0);
	const Vector3D Vector3D::NEGATIVE_UNIT_Z = Vector3D(0, 0, -1);
	const Vector3D Vector3D::UNIT_SCALE = Vector3D(1, 1, 1);
};