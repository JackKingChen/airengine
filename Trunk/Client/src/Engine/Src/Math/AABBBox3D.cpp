#include "AABBBox3D.h"
#include "Matrix4.h"

namespace AIR
{

	void AABBBox3D::TransformAffine(const AIR::Matrix4& m)
	{
		//assert(m.isAffine());

		//// Do nothing if current null or infinite
		//if ( mExtent != EXTENT_FINITE )
		//    return;

		AIR::Vector3Df centre = GetCenter();
		AIR::Vector3Df halfSize = GetHalfSize();

		AIR::Vector3Df newCentre = m.TransformCoord(centre, centre);

		AIR::Vector3Df newHalfSize(
			AIR::Abs(m[0]) * halfSize.x + AIR::Abs(m[4]) * halfSize.y + AIR::Abs(m[8]) * halfSize.z,
			AIR::Abs(m[1]) * halfSize.x + AIR::Abs(m[5]) * halfSize.y + AIR::Abs(m[9]) * halfSize.z,
			AIR::Abs(m[2]) * halfSize.x + AIR::Abs(m[6]) * halfSize.y + AIR::Abs(m[10]) * halfSize.z);

		Reset(newCentre - newHalfSize, newCentre + newHalfSize);
	}

};
