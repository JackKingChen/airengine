#ifndef __Collision_H__
#define __Collision_H__
#include "EngineConfig.h"
#include <vector>
#include "Vector3D.h"
#include "BoxShape.h"
#include "ISceneManager.h"
struct SContact 
{
	BoxShape*       m_pBox1;
	BoxShape*       m_pBox2;
	float           m_fDepth;
	AIR::Vector3Df  m_fPoint;
	AIR::Vector3Df  m_vfNormal;

};
class EngineExport Collision
{
public:
	Collision(){}
	static void GetShapeContact(BaseShape* pShape, const AIR::AABBBox3Df& aabbTest, ISceneManager* sceneManager, AIRVector<SContact>& vtContacts);
	
	static void CollideSimulation(BaseShape* pShape, const AIR::AABBBox3Df& aabbTest, CSceneNode *pNode);
};
#endif