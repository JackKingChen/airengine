#include "stdafx.h"
#include "Octree.h"
#include "ISceneManager.h"
#include "Engine.h"
#include "TimeBaseProfile.h"
#include "Entity.h"

IMPLEMENT_ROOT_RTTI(COctree);

int COctree::m_nMaxDepth = 8;   //默认是8层

COctree::COctree(COctree* pParent) : m_pParent(pParent), m_nNodesNum(0)
{
    memset(m_pChildren, 0, sizeof(COctree*) * 8);
}

COctree::~COctree()
{
    //initialize all children to null.
    for ( int i = 0; i < 2; i++ )
    {
        for ( int j = 0; j < 2; j++ )
        {
            for ( int k = 0; k < 2; k++ )
            {
                if ( m_pChildren[ i ][ j ][ k ] != 0 )
                    /*delete*/SafeDelete( m_pChildren[ i ][ j ][ k ] );
            }
        }
    }

    m_pParent = 0;
}

void COctree::Clear()
{
    OCTREENODE_ITER it = m_lstNode.begin();
    ISceneManager* pScene = ENGINE_INST->GetSceneMgr();
    while (it != m_lstNode.end())
    {
        COctreeNode* pNode = *it;
        //Renderable* pRenderable = pNode->GetAttachRenderable();
        //if (pRenderable)
        //{
        //    pScene->DestroyRenderable(pRenderable);
        //}
        pNode->DetachAllRenderable();
        //SafeDelete(pNode);
        ++it;
    }

    m_lstNode.clear();

    //m_lstNavtriangles.clear();

    for ( int i = 0; i < 2; i++ )
    {
        for ( int j = 0; j < 2; j++ )
        {
            for ( int k = 0; k < 2; k++ )
            {
                if ( m_pChildren[ i ][ j ][ k ] != 0 )
                    m_pChildren[ i ][ j ][ k ]->Clear();
            }
        }
    }

}

void COctree::AddNode(COctreeNode* pNode, int nDepth)
{
    if (pNode->GetOctree() != 0)
    {
        pNode->GetOctree()->RemoveNode(pNode);
    }
    const AABBBox3Df& box = pNode->GetWorldBoundingBox();

    if (IsIntersectAlign(box))
    {
        _AddNode(pNode);
        return;
    }

    if (nDepth < m_nMaxDepth && IsFitSize(box))
    {
        int x, y, z;
        GetChildIndex( box, x, y, z );

        if ( m_pChildren[ x ][ y ][ z ] == 0 )
        {
            m_pChildren[ x ][ y ][ z ] = new COctree( this );
            //const Vector3Df& octantMin = m_wordBox.MinEdge;
            //const Vector3Df& octantMax = m_wordBox.MaxEdge;
            Vector3Df min, max;

            if ( x == 0 )
            {
                min.x = m_wordBox.MinEdge.x;
                max.x = ( m_wordBox.MinEdge.x + m_wordBox.MaxEdge.x ) / 2;
            }

            else
            {
                min.x = ( m_wordBox.MinEdge.x + m_wordBox.MaxEdge.x ) / 2;
                max.x = m_wordBox.MaxEdge.x;
            }

            if ( y == 0 )
            {
                min.y = m_wordBox.MinEdge.y;
                max.y = ( m_wordBox.MinEdge.y + m_wordBox.MaxEdge.y ) / 2;
            }

            else
            {
                min.y = ( m_wordBox.MinEdge.y + m_wordBox.MaxEdge.y ) / 2;
                max.y = m_wordBox.MaxEdge.y;
            }

            if ( z == 0 )
            {
                min.z = m_wordBox.MinEdge.z;
                max.z = ( m_wordBox.MinEdge.z + m_wordBox.MaxEdge.z ) / 2;
            }

            else
            {
                min.z = ( m_wordBox.MinEdge.z + m_wordBox.MaxEdge.z ) / 2;
                max.z = m_wordBox.MaxEdge.z;
            }

            m_pChildren[ x ][ y ][ z ] -> m_wordBox.MinEdge = min;
            m_pChildren[ x ][ y ][ z ] -> m_wordBox.MaxEdge = max;
        }

        m_pChildren[ x ][ y ][ z ]->AddNode(pNode, ++nDepth);
    }
    else
    {
		//if ( pNode->GetName() == "SceneNode_32049" )
		//{
		//	int a = 0;
		//}
        _AddNode(pNode);
    }
}

void COctree::_AddNode(COctreeNode* pNode)
{
    m_lstNode.push_back(pNode);
    pNode->SetOctree(this);
    AddNodesNum();
}

void COctree::_AddEntity(Entity* pEntity)
{
	m_lstEntities.push_back(pEntity);
	pEntity->SetAttachedTree(this);
}

void COctree::UpdateOctreeNode(COctreeNode* pNode)
{
    const AABBBox3Df& worldBox = pNode->GetWorldBoundingBox();

    if ( worldBox.IsNull() )
        return ;

    if ( pNode -> GetOctree() == 0 )
    {
        //if outside the octree, force into the root node.
        if ( ! pNode -> IsIn( m_wordBox ) )
            _AddNode( pNode );
        else
            AddNode( pNode );
        return ;
    }

    if ( ! pNode -> IsIn( pNode->GetOctree() -> m_wordBox ) )
    {
        RemoveNode( pNode );

        //if outside the octree, force into the root node.
        if ( ! pNode -> IsIn( m_wordBox ) )
            _AddNode( pNode );
        else
            AddNode( pNode);
    }
}

void COctree::_RemoveNode(COctreeNode* pNode)
{
    if (m_lstNode.empty())
    {
        return;
    }
    m_lstNode.erase( std::find( m_lstNode.begin(), m_lstNode.end(), pNode ) );
    pNode -> SetOctree( 0 );

    //update total counts.
    DelNodesNum();
}

void COctree::RemoveNode(COctreeNode* pNode)
{
    COctree* pTree = pNode->GetOctree();
    if (pTree)
    {
        pTree->_RemoveNode(pNode);
    }
}

void COctree::RemoveEntity(Entity* pEntity)
{
	if (m_lstEntities.empty())
	{
		return;
	}
	m_lstEntities.erase(std::find(m_lstEntities.begin(), m_lstEntities.end(), pEntity));
	pEntity->SetAttachedTree(0);
	pEntity->Clear();
	delete pEntity;
}

bool COctree::IsFitSize(const AABBBox3Df& box)
{
    Vector3Df vTreeBoxSize = m_wordBox.GetExtent() / 2;

    Vector3Df boxSize = box.GetExtent();

    return ((boxSize.x <= vTreeBoxSize.x) && (boxSize.y <= vTreeBoxSize.y) && (boxSize.z <= vTreeBoxSize.z));
}

void COctree::GetChildIndex(const AABBBox3Df& box, int& x, int& y, int& z) const
{
    Vector3Df max = m_wordBox.MaxEdge;
    Vector3Df min = box.MinEdge;

    Vector3Df center = m_wordBox.MaxEdge.GetMidPoint( m_wordBox.MinEdge );

    Vector3Df ncenter = box.MaxEdge.GetMidPoint( box.MinEdge );

    if ( ncenter.x > center.x )
        x = 1;
    else
        x = 0;

    if ( ncenter.y > center.y )
        y = 1;
    else
        y = 0;

    if ( ncenter.z > center.z )
        z = 1;
    else
        z = 0;
}

void COctree::GetVisableObjects(CCamera* pCamera, AIRList<CSceneNode*>& lstVisables, u32 nCullFlag)
{
    if (m_nNodesNum == 0 || !(pCamera->GetFrustum()->IsAABBIn(m_wordBox)))
    {
        return;
    }

	
    //首先获取当前
    OCTREENODE_ITER it = m_lstNode.begin();
    while (it != m_lstNode.end())
    {
        COctreeNode* pNode = *it;

        if ((pNode->GetCullFlag() & nCullFlag) && pNode->IsVisable(pCamera))
		//if ( pNode->IsVisable( pCamera->GetUserClipFrustum() ) )
        {
            lstVisables.push_back(pNode);
        }
        //pNode->GetVisableRenderables(pCamera, pRenderQueue);
        //if (pCamera->IsVisable(pNode->GetWorldBoundingBox()))
        //{
        //    pRenderQueue->AddRenderable(pNode->GetAttachRenderable());
        //}
        it++;
    }
    
    if (m_pChildren[0][0][0] != 0)
    {
        m_pChildren[0][0][0]->GetVisableObjects(pCamera, lstVisables, nCullFlag);
    }

    if (m_pChildren[0][0][1] != 0)
    {
        m_pChildren[0][0][1]->GetVisableObjects(pCamera, lstVisables, nCullFlag);
    }
    
    if (m_pChildren[0][1][0] != 0)
    {
        m_pChildren[0][1][0]->GetVisableObjects(pCamera, lstVisables, nCullFlag);
    }
    
    if (m_pChildren[0][1][1] != 0)
    {
        m_pChildren[0][1][1]->GetVisableObjects(pCamera, lstVisables, nCullFlag);
    }

    if (m_pChildren[1][0][0] != 0)
    {
        m_pChildren[1][0][0]->GetVisableObjects(pCamera, lstVisables, nCullFlag);
    }

    if (m_pChildren[1][0][1] != 0)
    {
        m_pChildren[1][0][1]->GetVisableObjects(pCamera, lstVisables, nCullFlag);
    }

    if (m_pChildren[1][1][0] != 0)
    {
        m_pChildren[1][1][0]->GetVisableObjects(pCamera, lstVisables, nCullFlag);
    }

    if (m_pChildren[1][1][1] != 0)
    {
        m_pChildren[1][1][1]->GetVisableObjects(pCamera, lstVisables, nCullFlag);
    }
}

void COctree::GetVisableObjects(CCamera* pCamera, AIRList<CSceneNode*>& lstVisables, f32 fDistanceIn, u32 nCullFlag)
{
    if (m_nNodesNum == 0 || !(pCamera->GetFrustum()->IsAABBIn(m_wordBox)))
    {
        return;
    }


    //首先获取当前
    OCTREENODE_ITER it = m_lstNode.begin();
    while (it != m_lstNode.end())
    {
        COctreeNode* pNode = *it;

        if ((pNode->GetCullFlag() & nCullFlag) && pCamera->IsInDistance(pNode->GetDerivedPosition(), pNode->GetSphereRadius(), fDistanceIn) && pNode->IsVisable(pCamera))
            //if ( pNode->IsVisable( pCamera->GetUserClipFrustum() ) )
        {
            lstVisables.push_back(pNode);
        }
        //pNode->GetVisableRenderables(pCamera, pRenderQueue);
        //if (pCamera->IsVisable(pNode->GetWorldBoundingBox()))
        //{
        //    pRenderQueue->AddRenderable(pNode->GetAttachRenderable());
        //}
        it++;
    }

    if (m_pChildren[0][0][0] != 0)
    {
        m_pChildren[0][0][0]->GetVisableObjects(pCamera, lstVisables, fDistanceIn, nCullFlag);
    }

    if (m_pChildren[0][0][1] != 0)
    {
        m_pChildren[0][0][1]->GetVisableObjects(pCamera, lstVisables, fDistanceIn, nCullFlag);
    }

    if (m_pChildren[0][1][0] != 0)
    {
        m_pChildren[0][1][0]->GetVisableObjects(pCamera, lstVisables, fDistanceIn, nCullFlag);
    }

    if (m_pChildren[0][1][1] != 0)
    {
        m_pChildren[0][1][1]->GetVisableObjects(pCamera, lstVisables, fDistanceIn, nCullFlag);
    }

    if (m_pChildren[1][0][0] != 0)
    {
        m_pChildren[1][0][0]->GetVisableObjects(pCamera, lstVisables, fDistanceIn, nCullFlag);
    }

    if (m_pChildren[1][0][1] != 0)
    {
        m_pChildren[1][0][1]->GetVisableObjects(pCamera, lstVisables, fDistanceIn, nCullFlag);
    }

    if (m_pChildren[1][1][0] != 0)
    {
        m_pChildren[1][1][0]->GetVisableObjects(pCamera, lstVisables, fDistanceIn, nCullFlag);
    }

    if (m_pChildren[1][1][1] != 0)
    {
        m_pChildren[1][1][1]->GetVisableObjects(pCamera, lstVisables, fDistanceIn, nCullFlag);
    }
}

void COctree::GetVisableEntities(CCamera* pCamera, AIRList<Entity*>& lstVisables, u32 nCullFlag)
{
	if (m_lstEntities.empty() || !(pCamera->GetFrustum()->IsAABBIn(m_wordBox)))
	{
		return;
	}


	//首先获取当前
	AIRList<Entity*>::iterator it = m_lstEntities.begin();
	while (it != m_lstEntities.end())
	{
		Entity* pEntity = *it;

		if ((pEntity->GetCullFlag() & nCullFlag) && pEntity->IsActive() && pCamera->GetFrustum()->IsAABBIn(pEntity->GetWorldBox()))
			//if ( pNode->IsVisable( pCamera->GetUserClipFrustum() ) )
		{
			lstVisables.push_back(pEntity);
		}
		//pNode->GetVisableRenderables(pCamera, pRenderQueue);
		//if (pCamera->IsVisable(pNode->GetWorldBoundingBox()))
		//{
		//    pRenderQueue->AddRenderable(pNode->GetAttachRenderable());
		//}
		it++;
	}
}

void COctree::GetVisableEntities(CCamera* pCamera, AIRList<Entity*>& lstVisables, f32 fDistanceIn, u32 nCullFlag)
{

}

void COctree::AddEntity(Entity* pEntity, int nDepth)
{
	if (pEntity->GetOctree() != 0)
	{
		pEntity->GetOctree()->RemoveEntity(pEntity);
	}
	const AABBBox3D& box = pEntity->GetWorldBoundingBox();

	if (IsIntersectAlign(box))
	{
		_AddEntity(pEntity);
		return;
	}

	if (nDepth < m_nMaxDepth && IsFitSize(box))
	{
		int x, y, z;
		GetChildIndex(box, x, y, z);

		if (m_pChildren[x][y][z] == 0)
		{
			m_pChildren[x][y][z] = new COctree(this);
			//const Vector3Df& octantMin = m_wordBox.MinEdge;
			//const Vector3Df& octantMax = m_wordBox.MaxEdge;
			Vector3Df min, max;

			if (x == 0)
			{
				min.x = m_wordBox.MinEdge.x;
				max.x = (m_wordBox.MinEdge.x + m_wordBox.MaxEdge.x) / 2;
			}

			else
			{
				min.x = (m_wordBox.MinEdge.x + m_wordBox.MaxEdge.x) / 2;
				max.x = m_wordBox.MaxEdge.x;
			}

			if (y == 0)
			{
				min.y = m_wordBox.MinEdge.y;
				max.y = (m_wordBox.MinEdge.y + m_wordBox.MaxEdge.y) / 2;
			}

			else
			{
				min.y = (m_wordBox.MinEdge.y + m_wordBox.MaxEdge.y) / 2;
				max.y = m_wordBox.MaxEdge.y;
			}

			if (z == 0)
			{
				min.z = m_wordBox.MinEdge.z;
				max.z = (m_wordBox.MinEdge.z + m_wordBox.MaxEdge.z) / 2;
			}

			else
			{
				min.z = (m_wordBox.MinEdge.z + m_wordBox.MaxEdge.z) / 2;
				max.z = m_wordBox.MaxEdge.z;
			}

			m_pChildren[x][y][z]->m_wordBox.MinEdge = min;
			m_pChildren[x][y][z]->m_wordBox.MaxEdge = max;
		}

		m_pChildren[x][y][z]->AddEntity(pEntity, ++nDepth);
	}
	else
	{
		//if ( pNode->GetName() == "SceneNode_32049" )
		//{
		//	int a = 0;
		//}
		_AddEntity(pEntity);
	}
}

void COctree::GetCollisionRenderables(const AABBBox3Df& aabbTest, AIRList<Renderable*>& renderList)
{
	if (m_nNodesNum == 0 || !aabbTest.IntersectsWithBox(m_wordBox))
	{
		return ;
	}
	OCTREENODE_ITER it = m_lstNode.begin();
	while (it != m_lstNode.end())
	{
		COctreeNode* pNode = *it;

		if (aabbTest.IntersectsWithBox( pNode->GetWorldBoundingBox() ) )
		{
			pNode->GetCollisionRenderables(aabbTest, renderList);
		}
		it++;
	}

	if (m_pChildren[0][0][0] != 0)
	{
		m_pChildren[0][0][0]->GetCollisionRenderables(aabbTest, renderList);
	}

	if (m_pChildren[0][0][1] != 0)
	{
		m_pChildren[0][0][1]->GetCollisionRenderables(aabbTest, renderList);
	}

	if (m_pChildren[0][1][0] != 0)
	{
		m_pChildren[0][1][0]->GetCollisionRenderables(aabbTest, renderList);
	}

	if (m_pChildren[0][1][1] != 0)
	{
		m_pChildren[0][1][1]->GetCollisionRenderables(aabbTest, renderList);
	}

	if (m_pChildren[1][0][0] != 0)
	{
		m_pChildren[1][0][0]->GetCollisionRenderables(aabbTest, renderList);
	}

	if (m_pChildren[1][0][1] != 0)
	{
		m_pChildren[1][0][1]->GetCollisionRenderables(aabbTest, renderList);
	}

	if (m_pChildren[1][1][0] != 0)
	{
		m_pChildren[1][1][0]->GetCollisionRenderables(aabbTest, renderList);
	}

	if (m_pChildren[1][1][1] != 0)
	{
		m_pChildren[1][1][1]->GetCollisionRenderables(aabbTest, renderList);
	}
}

void COctree::SetAABBBox(const AABBBox3Df& box)
{
    m_wordBox = box;
}


void COctree::AddNavTriangle(int nDepth, NavTriangle* pNavTriangle)
{
    AABBBox3Df boundingBox(Vector3Df(999999, 999999, 999999), Vector3Df(-999999, -999999, -999999));
    boundingBox.AddInternalPoint(pNavTriangle->m_Triangle.pointA);
    boundingBox.AddInternalPoint(pNavTriangle->m_Triangle.pointB);
    boundingBox.AddInternalPoint(pNavTriangle->m_Triangle.pointC);

    _AddNavTriangle(nDepth, pNavTriangle, boundingBox);
}

void COctree::_AddNavTriangle(int nDepth, NavTriangle* pNAVTriangle, const AABBBox3Df& boundingBox)
{
    if (nDepth < m_nMaxDepth && IsFitSize(boundingBox))
    {
        int x, y, z;
        GetChildIndex( boundingBox, x, y, z );

        if ( m_pChildren[ x ][ y ][ z ] == 0 )
        {
            
            m_lstNavtriangles.push_back(pNAVTriangle);
        }
        else
            m_pChildren[ x ][ y ][ z ]->_AddNavTriangle(++nDepth, pNAVTriangle, boundingBox);
    }
    else
    {
        m_lstNavtriangles.push_back(pNAVTriangle);
        //_AddNode(pNode);
    }
}


void COctree::GetRayPickOctrees(const Line3Df& ray, AIRList<COctree*>& lstOut)
{
    if (m_wordBox.IntersectsWithLine(ray))
    {
        lstOut.push_back(this);
    }

    for ( int i = 0; i < 2; i++ )
    {
        for ( int j = 0; j < 2; j++ )
        {
            for ( int k = 0; k < 2; k++ )
            {
                if ( m_pChildren[ i ][ j ][ k ] != 0 )
                    m_pChildren[ i ][ j ][ k ]->GetRayPickOctrees(ray, lstOut);
            }
        }
    }
}


void COctree::GetRayPickNavTriangles(const Line3Df& ray, AIRList<NavIntersectInfo>& lstTriangles)
//void COctree::GetRayPickNavTriangles(const Line3Df& ray, AIRList<NavTriangle*>& lstTriangles)
{
    NavTriangle_List_Iter itEnd = m_lstNavtriangles.end();
    Vector3Df out;
    for (NavTriangle_List_Iter it = m_lstNavtriangles.begin(); it != itEnd; ++it)
    {
        if ((*it)->m_Triangle.GetIntersectionWithLimitedLine(ray, out))
        {
			NavIntersectInfo info;
			info.m_pIntersectTriangle = *it;
			info.m_vIntersectPoint = out;
            lstTriangles.push_back(info);
        }
    }
}

bool COctree::IsIntersectAlign(const AABBBox3Df& box)
{
    const Vector3Df& center = m_wordBox.GetCenter();

    if (box.MaxEdge.x > center.x && box.MinEdge.x < center.x)
    {
        return true;
    }

    if (box.MaxEdge.y > center.y && box.MinEdge.y < center.y)
    {
        return true;
    }

    if (box.MaxEdge.z > center.z && box.MinEdge.z < center.z)
    {
        return true;
    }

    return false;
}
