#ifndef __BaseShape_H__
#define __BaseShape_H__
#include <string>
#include "EngineConfig.h"
#include "Vector3D.h"
#include "Matrix4.h"
class EngineExport BaseShape
{
public:
	BaseShape(const char* szName);
	virtual ~BaseShape();

	void SetWorldTransform(const AIR::Matrix4f& matWorldTransform );

	const AIR::Matrix4f& getWorldTransform () const;
	
	std::string getType(){ return m_stType ; }

	void setPosition(const AIR::Vector3Df& size);
	AIR::Vector3Df GetWorldPostion();

protected:
	AIR::Matrix4f     m_matWorldTransform;
	std::string  m_stType;
};
#endif