#include "stdafx.h"
#include "Collision.h"
#include "Renderable.h"
#include "ISceneManager.h"
#include "BoxCollider.h"
#include "Engine.h"
#include "SceneNode.h"
#include "HeightMap.h"
#include "NavEditor.h"

void Collision::GetShapeContact(BaseShape* pShape, const AABBBox3Df& aabbTest, ISceneManager* sceneManager, AIRVector<SContact>& vtContacts)
{
	AIRList<Renderable*> ltRenderable;
	sceneManager->GetCollisionRenderables(aabbTest, ltRenderable);
	if ( ltRenderable.size() == 0 )
	{
		vtContacts.clear();
		return ;
	}
	if ( pShape->getType() == std::string("box"))
	{
		AIRList<Renderable*>::iterator it = ltRenderable.begin();

		for ( ; it != ltRenderable.end(); it++ )
		{
			Renderable* pRenderable = *it;
			//���ԣ�ֻ����monster��ײ
			if (!pRenderable->GetCollision())
			{
				continue ;
			}
			BaseShape* pShape2 = pRenderable->GetShape();
			if (!pShape2)
			{
				continue ;
			}
			BoxBoxCollision(pShape, pShape2, vtContacts);
		}
	}
}

void Collision::CollideSimulation(BaseShape *pShape, const AIR::AABBBox3Df &aabbTest, CSceneNode *pNode)
{
	if(!ENGINE_INST->GetConfig()->m_bCharacterCollision)
	{
		return ;
	}
	AIRVector <SContact> contacts;
	GetShapeContact(pShape, aabbTest, SCENEMGR, contacts);
	if (contacts.size() == 0)
	{
		return ;
	}

	float inum = 1.0f/contacts.size();
	Vector3Df offset ;
	for (size_t i = 0 ; i < contacts.size() ; i++)
	{
		offset += contacts[i].m_vfNormal*(contacts[i].m_fDepth*inum);
	}
	pNode->SetPosition(pNode->GetPosition() - offset);
	Vector3Df positon = pNode->GetPosition();

	if ( NAV_EDITOR->GetHasGenNavMesh() )
	{
		NavTriangle nav_tri;
		// ��ý�ɫ���ڵĵ���������
		//if ( NAV_EDITOR->CalPickTriangle( positon.x, positon.z, nav_tri) )
		if ( NAV_EDITOR->OptimiseFindIntersectNavMesh( positon.x, positon.z, nav_tri ) )
		{
			// ������������������Ϊ0����ʾ��ɫ��վ�ڵ�������
			if ( nav_tri.m_byAttrib == 0 )
			{
				// ��ý�ɫ���ڵ���ĸ߶�
				float y = SCENEMGR->GetHeightMap()->GetHeight(positon.x, positon.z);
				pNode->SetPosition(positon.x, y, positon.z);
			}
			// ����,��ʾ��ɫվ�ڵ�����������
			else
			{
				// �����ɫ���ڵ�Ĵ�ֱ���뵼������Ľ���
				positon.y = -float( 0xffffff );
				Vector3Df out;
				if ( nav_tri.m_Triangle.GetIntersectionWithLine( positon, Vector3Df::UNIT_Y, out ) )
				{
					pNode->SetPosition(out);
				}						
			}
		}
		else
		{
			//std::cout<<"char position invalid\n";
		}
	}
	else
	{
		// ��ý�ɫ���ڵ���ĸ߶�
		float y = SCENEMGR->GetHeightMap()->GetHeight(positon.x, positon.z);
		pNode->SetPosition(positon.x, y, positon.z);
	}
	pNode->NeedUpdate();
	pNode->Update();
}