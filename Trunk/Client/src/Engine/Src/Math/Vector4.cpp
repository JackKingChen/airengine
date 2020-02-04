#include "Vector4.h"

namespace AIR
{
	const Vector4 Vector4::ZERO = Vector4(0, 0, 0, 0);
	const Vector4 Vector4::UNIT_X = Vector4(1, 0, 0, 0);
	const Vector4 Vector4::UNIT_Y = Vector4(0, 1, 0, 0);
	const Vector4 Vector4::UNIT_Z = Vector4(0, 0, 1, 0);
	const Vector4 Vector4::NEGATIVE_UNIT_X = Vector4(-1, 0, 0, 0);
	const Vector4 Vector4::NEGATIVE_UNIT_Y = Vector4(0, -1, 0, 0);
	const Vector4 Vector4::NEGATIVE_UNIT_Z = Vector4(0, 0, -1, 0);
	const Vector4 Vector4::UNIT_SCALE = Vector4(1, 1, 1, 1);
}
