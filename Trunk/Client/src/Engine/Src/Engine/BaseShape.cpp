#include "stdafx.h"
#include "BaseShape.h"
BaseShape::BaseShape(const char* type):m_stType(type)
{
	m_matWorldTransform.Identity();
}

BaseShape::~BaseShape(){}

void BaseShape::SetWorldTransform(const AIR::Matrix4f &matWorldTransform)
{
	m_matWorldTransform = matWorldTransform;
}

const AIR::Matrix4f& BaseShape::getWorldTransform() const
{
	return m_matWorldTransform;
}

AIR::Vector3Df BaseShape::GetWorldPostion()
{
	AIR::Vector3Df pos;
	pos.x = m_matWorldTransform(0, 3);
	pos.y = m_matWorldTransform(1, 3);
	pos.z = m_matWorldTransform(2, 3);
	return pos;
}

void BaseShape::setPosition(const AIR::Vector3Df& size)
{
	m_matWorldTransform(0, 3) = size.x;
	m_matWorldTransform(1, 3) = size.y;
	m_matWorldTransform(2, 3) = size.z;
}