#include "stdafx.h"
#include "BoxShape.h"

BoxShape::BoxShape():BaseShape("box")
{
}

BoxShape::~BoxShape()
{
}



AIR::Vector3Df BoxShape::GetBoxSize()
{
	return m_vfSize;
}

void BoxShape::setBoxSize(const AIR::Vector3Df& size)
{
	m_vfSize = size;
}