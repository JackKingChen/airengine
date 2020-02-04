#ifndef __BoxShape_H__
#define __BoxShape_H__
#include "EngineConfig.h"
#include "BaseShape.h"
#include "Vector3D.h"
class EngineExport BoxShape : public BaseShape
{
public:
	BoxShape();

	~BoxShape();

	void setBoxSize(const AIR::Vector3Df& size);
	AIR::Vector3Df GetBoxSize();
	
	

protected:

private:
	AIR::Vector3Df		      m_vfSize;
};
#endif