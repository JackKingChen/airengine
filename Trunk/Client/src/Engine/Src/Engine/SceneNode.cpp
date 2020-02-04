#include "stdafx.h"
#include "SceneNode.h"
#include "ISceneManager.h"
#include "Engine.h"

//int g_scenenodeCount = 0;

CSceneNode::CSceneNode() : 
m_pParent(NULL),
m_position(0, 0, 0), 
m_rotation(0, 0, 0, 1),
m_vScale(1.0f, 1.0f, 1.0f), 
m_DerivedScale(Vector3Df::UNIT_SCALE),
//m_pRenderable(NULL), 
m_bNeedUpdate(true),
m_bTransform(false)
, m_dwRayQueryMask(0x0),
m_bIsUpdatePerFrame(false),
m_vInitialPosition(Vector3Df::ZERO),
m_qInitialOrientation(Quaternion::IDENEITY),
m_vInitialScale(Vector3Df::UNIT_SCALE),
m_bMovable(false),
m_szType("scenenode")
, m_pCreator(NULL)
, m_nCullFlag(0xffffffff)
{
    //g_scenenodeCount++;
}

CSceneNode::CSceneNode(ISceneManager* pCreator, const AIRString& strName, const Vector3Df& vPosition, const Quaternion& qRotation /* = Quaternion */, const Vector3Df& vScale) :
m_pParent(NULL),
m_position(vPosition), 
m_rotation(qRotation),
m_vScale(vScale), 
m_DerivedScale(Vector3Df::UNIT_SCALE),
//m_pRenderable(NULL), 
m_bNeedUpdate(true),
m_strName(strName),
m_bTransform(false),
m_pCreator(pCreator),
m_dwRayQueryMask(0x0),
m_vInitialPosition(Vector3Df::ZERO),
m_qInitialOrientation(Quaternion::IDENEITY),
m_vInitialScale(Vector3Df::UNIT_SCALE),
m_bIsUpdatePerFrame(false),
m_bMovable(false),
m_szType("scenenode")
, m_nCullFlag(0xffffffff)
{
    //g_scenenodeCount++;
    //Update();
}

CSceneNode::CSceneNode(ISceneManager* pCreator, const Vector3Df& vPosition, const Quaternion& qRotation, const Vector3Df& vScale) : 
m_pParent(NULL),
m_position(vPosition), 
m_rotation(qRotation),
m_vScale(vScale), 
//m_pRenderable(NULL), 
m_bNeedUpdate(true),
m_bTransform(false)
, m_pCreator(pCreator),
m_dwRayQueryMask(0x0),
m_vInitialPosition(Vector3Df::ZERO),
m_qInitialOrientation(Quaternion::IDENEITY),
m_vInitialScale(Vector3Df::UNIT_SCALE),
m_szType("scenenode")
, m_fSphereRadius(0)
, m_nCullFlag(0xffffffff)
{
    m_worldBox.MaxEdge = Vector3Df(-99999, -99999, -99999);
    m_worldBox.MinEdge = Vector3Df(99999, 99999, 99999);
    //g_scenenodeCount++;
}


CSceneNode::~CSceneNode()
{
    RemoveAllChildren();
    if (m_pParent)
    {
        m_pParent->RemoveChild(this);
    }

    //g_scenenodeCount--;
    //OutputDebugString(GetString("now the scene node count is:%d\n", g_scenenodeCount));
}

CSceneNode::NodeRelation CSceneNode::IsNodesInRelation( CSceneNode* pNodeL, CSceneNode* pNodeR )
{
	if (!pNodeL || !pNodeR)
	{
		return NR_LNoneRelateR;
	}
	if (pNodeL == pNodeR)
	{
		return NR_LEqualR;
	}
	CSceneNode* pParent = pNodeL;
	while( pParent && pParent != pNodeR )
	{
		pParent = pParent->GetParent();
	}
	if (pParent == pNodeR)
	{
		return NR_LChildRParent;
	}
	pParent = pNodeR;
	while( pParent && pParent != pNodeL )
	{
		pParent = pParent->GetParent();
	}
	if (pParent == pNodeL)
	{
		return NR_LParentRChild;
	}
	return NR_LNoneRelateR;
}

void CSceneNode::Translate(const Vector3Df& vPosition, TransformSpace space)
{
    switch (space)
    {
    case TS_LOCAL:
        m_position += m_rotation * vPosition;
        break;
    case TS_PARENT:
        m_position += vPosition;
        break;
    case TS_WORLD:
        if (m_pParent)
        {
            Quaternion derivedRotationInverse = m_pParent->GetDerivedRotation();
            derivedRotationInverse.Inverse();
            m_position += (derivedRotationInverse * vPosition)
                / m_pParent->GetDerivedScale();
        }
        else
        {
            m_position += vPosition;
        }
        break;
    }
    
    m_bNeedUpdate = true;
}

void CSceneNode::Rotate(const Vector3Df& axis, float fRadian, TransformSpace space)
{
    Quaternion q;
    q.FromAngleAxis(fRadian, axis);
    Rotate(q, space);
}

void CSceneNode::Rotate(const Quaternion& qRotate, TransformSpace space)
{
    Quaternion qFinal = qRotate;
    qFinal.Normalize();
    switch (space)
    {
    case TS_PARENT:
        m_rotation = qFinal * m_rotation;
        break;
    case TS_WORLD:
        {
			Quaternion derivedRotationInverse;
			if ( !m_pParent )
			{
				derivedRotationInverse = GetDerivedRotation();
			}
			else
			{
				derivedRotationInverse = m_pParent->GetDerivedRotation();
			}
            derivedRotationInverse.Inverse();
            m_rotation = m_rotation * derivedRotationInverse * qFinal * GetDerivedRotation();
        }
        
        break;
    case TS_LOCAL:
        m_rotation = m_rotation * qFinal;
        break;
    }
    //qFinal.Inverse();
    m_bNeedUpdate = true;
}

void CSceneNode::Scale(const Vector3Df& vScale)
{
    m_vScale *= vScale;
    m_bNeedUpdate = true;
}

void CSceneNode::SetPosition(const Vector3Df& vPosition)
{
    m_position = vPosition;
    m_bNeedUpdate = true;
}

void CSceneNode::SetPosition(float x, float y, float z)
{
    m_position.x = x;
    m_position.y = y;
    m_position.z = z;
    
    NeedUpdate();
}

void CSceneNode::SetRotation(const Quaternion& qRotate)
{
	m_rotation = qRotate;
	m_bNeedUpdate = true;
}

void CSceneNode::SetScale(const Vector3Df& vScale)
{
    m_vScale = vScale;
    m_bNeedUpdate = true;
}

void CSceneNode::MakeWorldTransform(/*Matrix4f* pMatWorld*/) const
{
    Matrix4f rot4x4, scale4x4;
    m_DerivedRotation.GetMatrix(rot4x4);
    scale4x4.Identity();;
	scale4x4[0] = m_DerivedScale.x;
	scale4x4[5] = m_DerivedScale.y;
	scale4x4[10] = m_DerivedScale.z;

    // Set up final matrix with scale, rotation and translation
	MultiplyMatrix4( rot4x4, scale4x4, m_matTransform );
    m_matTransform.SetTranslation(m_DerivedPosition);

    // No projection term
	m_matTransform[3] = 0; 
	m_matTransform[7] = 0; 
	m_matTransform[11] = 0; 
	m_matTransform[15] = 1;



	//////if (m_bNeedUpdate)
	//////{
	////Matrix4f rot4x4, scale4x4;
	////m_DerivedRotation.GetMatrix(rot4x4);
	////scale4x4.Identity();;
	////scale4x4(0, 0) = m_DerivedScale.x;
	////scale4x4(1, 1) = m_DerivedScale.y;
	////scale4x4(2, 2) = m_DerivedScale.z;

	////// Set up final matrix with scale, rotation and translation
	////m_matTransform = rot4x4 * scale4x4;
	////m_matTransform.SetTranslation(m_DerivedPosition);

	////// No projection term
	////m_matTransform(0, 3) = 0; 
	////m_matTransform(1, 3) = 0; 
	////m_matTransform(2, 3) = 0; 
	////m_matTransform(3, 3) = 1;
	//////m_bNeedUpdate = false;
	//////}

	//////*pMatWorld = m_matTransform;
}

const Matrix4f& CSceneNode::GetFullTransform() const
{
    if (!m_bTransform)
    {
        //GetWorldTransform(&m_matTransform);
        MakeWorldTransform();
        m_bTransform = true;
    }
    return m_matTransform;
}

void CSceneNode::AttachRenderable(Renderable* pRenderable)
{
    //m_pRenderable = pRenderable;
    pRenderable->SetAttachedNode(this);

    RenderableListIter it = std::find(m_lstRenderable.begin(), m_lstRenderable.end(), pRenderable);

    if (it == m_lstRenderable.end())
    {
        m_lstRenderable.push_back(pRenderable);
    }

    NotifyParentUpdateBounds();
    //m_bNeedUpdate = true;
    //Update();
    //if (m_pParent)
    //{
    //    m_pParent->UpdateBounds();
    //}
}

void CSceneNode::Update()
{
    if (m_bNeedUpdate)
    {
        UpdateFromParent();
        /*
        Matrix4f rot4x4, scale4x4;
        m_DerivedRotation.GetMatrix(rot4x4);
        scale4x4.Identity();;
        scale4x4(0, 0) = m_DerivedScale.x;
        scale4x4(1, 1) = m_DerivedScale.y;
        scale4x4(2, 2) = m_DerivedScale.z;

        // Set up final matrix with scale, rotation and translation
        m_matTransform = rot4x4 * scale4x4;
        m_matTransform.SetTranslation(m_DerivedPosition);

        // No projection term
        m_matTransform(0, 3) = 0; 
        m_matTransform(1, 3) = 0; 
        m_matTransform(2, 3) = 0; 
        m_matTransform(3, 3) = 1;
        */
        m_bNeedUpdate = false;
        m_bTransform = false;

        //if (m_pRenderable)
        //{
        //    m_worldBox = m_pRenderable->GetBoundingBox();
        //    m_matTransform.TransformBox(m_worldBox);
        //}
        
        for (ChildNode_Iter it = m_mapChildren.begin(); it != m_mapChildren.end(); ++it)
        {
            CSceneNode* pChild = it->second;
            pChild->m_bNeedUpdate = true;
            pChild->Update();
        }

        UpdateBounds();

        //m_obstruct.Update(GetFullTransform());
    }
}

void CSceneNode::UpdateFromParent()
{
    if (m_pParent)
    {
        const Quaternion& parentRotate = m_pParent->GetDerivedRotation();
        //Quaternion invRotation = m_rotation;
        //m_DerivedRotation = parentRotate * m_rotation;
        MultiplyQuaternion( parentRotate, m_rotation, m_DerivedRotation );

        const Vector3Df& parentScale = m_pParent->GetDerivedScale();
        //m_DerivedScale = parentScale * m_vScale;
        MultiplyVector3( parentScale, m_vScale, m_DerivedScale );
        //m_vScale = parentScale;

        //Quaternion finalRotation = parentRotate;
        //m_DerivedPosition = parentRotate * (parentScale * m_position);
        QuaternionMultiplyVector( parentRotate, (parentScale * m_position), m_DerivedPosition );
        m_DerivedPosition += m_pParent->GetDerivedPosition();
    }
    else
    {
        m_DerivedPosition = m_position;
        m_DerivedRotation = m_rotation;
        m_DerivedScale = m_vScale;
    }
    
    for (RenderableListIter it = m_lstRenderable.begin(); it != m_lstRenderable.end(); ++it)
    {
        (*it)->NotifyMove();
    }

	//if (m_pParent)
	//{
	//	const Quaternion& parentRotate = m_pParent->GetDerivedRotation();
	//	Quaternion invRotation = m_rotation;
	//	m_DerivedRotation = parentRotate * m_rotation;
	//	//m_DerivedRotation.Inverse();

	//	const Vector3Df& parentScale = m_pParent->GetDerivedScale();
	//	m_DerivedScale = parentScale * m_vScale;
	//	//m_vScale = parentScale;

	//	//Quaternion finalRotation = parentRotate;

	//	m_DerivedPosition = parentRotate * (parentScale * m_position);
	//	m_DerivedPosition += m_pParent->GetDerivedPosition();
	//}
	//else
	//{
	//	m_DerivedPosition = m_position;
	//	m_DerivedRotation = m_rotation;
	//	m_DerivedScale = m_vScale;
	//}

	//for (RenderableListIter it = m_lstRenderable.begin(); it != m_lstRenderable.end(); ++it)
	//{
	//	it->second->NotifyMove();
	//}
}

void CSceneNode::UpdateBounds()
{
    //if (m_pRenderable)
    //{
    //    m_worldBox = m_pRenderable->GetWorldBoundingBox(true);
    //}
    m_worldBox.SetNull();
    //if (m_pRenderable)
    //{
    //    m_worldBox = m_pRenderable->GetWorldBoundingBox(true);
    //}
    
    RenderableListIter itEnd = m_lstRenderable.end();
    for (RenderableListIter it = m_lstRenderable.begin(); it != itEnd; ++it)
    {
        m_worldBox.AddInternalBox((*it)->GetWorldBoundingBox(true));
    }
    
    ChildNode_Iter itNodeEnd = m_mapChildren.end();
    for (ChildNode_Iter itNode = m_mapChildren.begin(); itNode != itNodeEnd; ++itNode)
    {
        m_worldBox.AddInternalBox(itNode->second->m_worldBox);
    }

    Vector3Df halfSize = m_worldBox.GetHalfSize();
    m_fSphereRadius = Max(Max(halfSize.x, halfSize.y), halfSize.z); 
}

bool CSceneNode::IsVisable(const CCamera* pCamera) const
{
	// 用包围盒的方法进行相交测试
	return pCamera->GetFrustum()->IsAABBIn(m_worldBox);
	
	//// 用包围球的方法进行相交测试
	//return pFrustum->IsSphereIn( m_worldBox.GetCenter(), m_worldBox.GetHalfSize().GetLength() );
}

const AABBBox3Df& CSceneNode::GetWorldBoundingBox() const
{
    return m_worldBox;
}

void CSceneNode::Roll(const float fRadian, TransformSpace relativeTo)
{
    Rotate(Vector3Df::UNIT_Z, fRadian, relativeTo);
}

void CSceneNode::Pitch(const float fRadian, TransformSpace relativeTo)
{
    Rotate(Vector3Df::UNIT_X, fRadian, relativeTo);
}

void CSceneNode::Yaw(const float fRadian, TransformSpace relativeTo)
{
    Rotate(Vector3Df::UNIT_Y, fRadian, relativeTo);
}

void CSceneNode::SetParent(CSceneNode* pParent)
{
    bool different = (pParent != m_pParent);

    if (m_pParent && different)
    {
        //m_pParent->RemoveChild(m_strName);
    }

    m_pParent = pParent;
    if (m_pParent)
    {
        //m_pParent->AddChild(this);
    }
}

CSceneNode* CSceneNode::CreateChild(const AIRString& strName, bool bMovable, const Vector3Df& position /* = Vector3Df::ZERO */, const Quaternion& rotate /* = Quaternion */,
                                    const Vector3Df& scale)
{
    CSceneNode* pChild = GetChild(strName);
    if (pChild != NULL)
    {
        pChild->SetPosition(position);
        pChild->SetRotation(rotate);
        pChild->SetScale(scale);
        return pChild;
    }

    if (m_pCreator)
    {
        pChild = m_pCreator->CreateSceneNode(strName, false, position, rotate, scale);//new CSceneNode(strName, position, rotate, Vector3Df::UNIT_SCALE);
    }
    else
    {
        ISceneManager* pScene = ENGINE_INST->GetSceneMgr();
        pChild = pScene->CreateSceneNode(strName, false, position, rotate, scale);
    }
    
    AddChild(pChild);
    return pChild;
}

void CSceneNode::AddChild(CSceneNode* pNode)
{
    //if (pNode->m_pParent)
    //{
    //    //出错
    //}
    if (pNode == this)
    {
        //如果是自己，出错
        return;
    }

	if ( pNode && pNode->GetParent() == this )
	{
		return;
	}

    if (pNode && pNode->GetParent() != this)
    {
        if (pNode->GetParent())
        {
            pNode->GetParent()->RemoveChild(pNode);
        }
    }

    m_mapChildren.insert(std::make_pair(pNode->GetName(), pNode));
    pNode->SetParent(this);

    //Update();
}

CSceneNode* CSceneNode::GetChild(const AIRString& strName) const
{
    ChildNode_Map::const_iterator it = m_mapChildren.find(strName);

    if (it != m_mapChildren.end())
    {
        return it->second;
    }

    return NULL;
}

CSceneNode* CSceneNode::RemoveChild(const AIRString& strName)
{
    ChildNode_Iter it = m_mapChildren.find(strName);
    if (it != m_mapChildren.end())
    {
        CSceneNode* pChild = it->second;
        m_mapChildren.erase(it);
        pChild->SetParent(NULL);
        NotifyParentUpdateBounds();
        return pChild;
    }

    return NULL;
}

unsigned short CSceneNode::GetChildrenNum()
{
    return (unsigned short)m_mapChildren.size();
}

void CSceneNode::RemoveAllChildren()
{
    if ( m_mapChildren.size() == 0 )
    {
        return;
    }
    ChildNode_Iter itend = m_mapChildren.end();
    for (ChildNode_Iter it = m_mapChildren.begin(); it != itend; ++it)
    {
        CSceneNode* pChild = it->second;
        pChild->SetParent(0);
        //// SetParent()里面执行RemoveChild(),RemoveChild()里修改了容器m_mapChildren,所以必须从新获取m_mapChildren的尾迭代器
        //itend = m_mapChildren.end();
    }
    m_mapChildren.clear();

	//UpdateBounds();

    NotifyParentUpdateBounds();
}

CSceneNode* CSceneNode::RemoveChild(CSceneNode* pNode)
{
    if (pNode)
    {
        ChildNode_Iter it = m_mapChildren.find(pNode->GetName());
        if (it != m_mapChildren.end() && it->second == pNode)
        {
            m_mapChildren.erase(it);
            pNode->SetParent(0);
            
        }
    }

    /*
    if (!m_bIsUpdatePerFrame)
    {
        UpdateBounds();
    }
    else
    {
        NeedUpdate();
    }
    */
    NotifyParentUpdateBounds();

    return pNode;
}
/*
Renderable* CSceneNode::GetAttachRenderable(const AIRString& strName) const
{
    RenderableList::const_iterator it = m_lstRenderable.find(strName);
    if (it != m_lstRenderable.end())
    {
        return *it;
    }
    return NULL;
}
*/
Renderable* CSceneNode::DetachRenderable(Renderable* pRenderable)
{
    
    RenderableListIter it = std::find(m_lstRenderable.begin(), m_lstRenderable.end(), pRenderable);

    if (it != m_lstRenderable.end())
    {
        m_lstRenderable.erase(it);
        pRenderable->SetAttachedNode(0);
        NotifyParentUpdateBounds();
        return pRenderable;
    }

    return NULL; //DetachRenderable(pRenderable->GetName());
}
/*
Renderable* CSceneNode::DetachRenderable(const AIRString& strName)
{
	RenderableListIter it = m_lstRenderable.find(strName);

	if (it != m_lstRenderable.end())
	{
		Renderable* pRenderable = it->second;
		m_lstRenderable.erase(it);
		pRenderable->SetAttachedNode(0);
        NotifyParentUpdateBounds();
		return pRenderable;
	}

	return NULL;
}
*/
void CSceneNode::DetachAllRenderable()
{
    //if (m_pRenderable)
    //{
    //    m_pRenderable->SetAttachedNode(0);
    //}
    for (RenderableListIter it = m_lstRenderable.begin(); it != m_lstRenderable.end(); ++it)
    {
        Renderable* pRenderable = *it;
        pRenderable->SetAttachedNode(0);
    }

    m_lstRenderable.clear();

    for (ChildNode_Iter it = m_mapChildren.begin(); it != m_mapChildren.end(); ++it)
    {
        CSceneNode* pChild = it->second;
        pChild->DetachAllRenderable();
    }

    //NotifyParentUpdateBounds();
}


void CSceneNode::DestroyAllRenderable( void )
{
	for (RenderableListIter it = m_lstRenderable.begin(); it != m_lstRenderable.end(); ++it)
	{
		Renderable* pRenderable = *it;
		pRenderable->SetAttachedNode(0);
		m_pCreator->DestroyRenderable( pRenderable );
	}
	m_lstRenderable.clear();

	for (ChildNode_Iter it = m_mapChildren.begin(); it != m_mapChildren.end(); ++it)
	{
		CSceneNode* pChild = it->second;
		pChild->DestroyAllRenderable();
	}
}


void CSceneNode::GetVisableRenderables(CCamera* pCamera, CRenderQueue* pRenderQueue)
{
    if (IsVisable(pCamera))
	//if ( IsVisable( pCamera->GetUserClipFrustum() ) )
    {
        for (RenderableListIter it = m_lstRenderable.begin(); it != m_lstRenderable.end(); ++it)
        {
            Renderable* pRenderable = *it;
            pRenderQueue->AddRenderable(pRenderable);
        }

        //遍历子结点
        for (ChildNode_Iter it = m_mapChildren.begin(); it != m_mapChildren.end(); ++it)
        {
            it->second->GetVisableRenderables(pCamera, pRenderQueue);
        }
    }
}

void CSceneNode::GetCollisionRenderables(const AABBBox3Df& aabbTest, AIRList<Renderable*>& renderList)
{
	for (RenderableListIter it = m_lstRenderable.begin(); it != m_lstRenderable.end(); ++it)
	{
		Renderable* pRenderable = *it;
		if ( aabbTest.IntersectsWithBox( pRenderable->GetWorldBoundingBox(true) ) )
		{
			renderList.push_back(pRenderable);
		}
	}

	//遍历子结点
	for (ChildNode_Iter it = m_mapChildren.begin(); it != m_mapChildren.end(); ++it)
	{
		it->second->GetCollisionRenderables(aabbTest, renderList);
	}
}

void CSceneNode::AddAttachObjToRenderQueue(CRenderQueue* pRenderQueue)
{
    
    for (RenderableListIter it = m_lstRenderable.begin(); it != m_lstRenderable.end(); ++it)
    {
        Renderable* pRenderable = *it;
        pRenderQueue->AddRenderable(pRenderable);
    }
    
}

bool CSceneNode::IsPick(const Vector3Df& vOrg, const Vector3Df& vEnd) const
{
    return m_worldBox.IntersectsWithLine(Line3Df(vOrg, vEnd));
}


void CSceneNode::SetCreator(ISceneManager* pSceneMgr)
{
    m_pCreator = pSceneMgr;
}

bool CSceneNode::IsIn(const AABBBox3Df& box)
{
    return m_worldBox.IsFullInside(box);
}

bool CSceneNode::IsIntersect(const AABBBox3Df& box)
{
	return (m_worldBox.IntersectsWithBox(box) || box.IsFullInside(m_worldBox));
}

void CSceneNode::UpdateToRenderQueue(CCamera* pCamera, CRenderQueue* pQueue )
{
    Renderable* pRenderable = NULL;
    for (RenderableListIter it = m_lstRenderable.begin(); it != m_lstRenderable.end(); ++it)
    {
        pRenderable = *it;
        if (pRenderable->IsVisable())
        {
            pRenderable->UpdateToRenderQueue(pCamera, pQueue);
        }
        //it->second->UpdateToRenderQueue(pCamera, pQueue);
    }
}

void CSceneNode::FillRenderContext(CCamera* pCamera, RenderContextQueue* pQueue, eRenderFlow flow)
{
    Renderable* pRenderable = NULL;
    for (RenderableListIter it = m_lstRenderable.begin(); it != m_lstRenderable.end(); ++it)
    {
        pRenderable = *it;
        if (pRenderable->IsVisable())
        {
            pRenderable->FillRenderContext(pCamera, pQueue, flow);
        }
        //it->second->UpdateToRenderQueue(pCamera, pQueue);
    }
}

void CSceneNode::FindLights(LightList& destLists, float fRaduis)
{
    if (m_pCreator)
    {
        //查找场景中的光照
        m_pCreator->PopulateLightList(this, destLists, fRaduis);
    }
    else
    {
        destLists.clear();
    }
}

//void CSceneNode::AddWorldBoundingBox(const AABBBox3Df& worldBox)
//{
//    m_worldBox.AddInternalBox(worldBox);
//}

void CSceneNode::NotifyParentUpdateBounds()
{
    //NeedUpdate();
	UpdateBounds();
    if (m_pParent)
    {
        m_pParent->NotifyParentUpdateBounds();
    }
    //else
    //    UpdateBounds();
}

void CSceneNode::SetName(const AIRString& strName)
{
    if (strName != m_strName)
    {
        if (m_pCreator)
        {
            if (m_pCreator->ChangeSceneNodeName(this, strName))
            {
                m_strName = strName;
            }
        }
    }
}

void CSceneNode::SetName(const char* szName)
{
    SetName(AIRString(szName));
}

void CSceneNode::SetDerivedPosition(const Vector3Df& position)
{
    if (m_pParent)
    {
        SetPosition(m_pParent->ConvertWorldToLocalPosition(position));
    }
    else
    {
        SetPosition(position);
    }
}

void CSceneNode::SetDerivedRotation(const Quaternion& rotation)
{
    if (m_pParent)
    {
        SetRotation(m_pParent->ConvertWorldToLocalOrientation(rotation));
    }
    else
    {
        SetRotation(rotation);
    }
}

Quaternion CSceneNode::ConvertLocalToWorldOrientation(const Quaternion& localOrient)
{
    if (m_bNeedUpdate)
    {
        UpdateFromParent();
    }
    return m_DerivedRotation * localOrient;
}

Quaternion CSceneNode::ConvertWorldToLocalOrientation(const Quaternion& worldOrient)
{
    if (m_bNeedUpdate)
    {
        UpdateFromParent();
    }
    return m_DerivedRotation.Inverse() * worldOrient;
}

Vector3Df CSceneNode::ConvertLocalToWorldPosition(const Vector3Df& localPos)
{
    if (m_bNeedUpdate)
    {
        UpdateFromParent();
    }
    return (m_DerivedRotation * localPos * m_DerivedScale) + m_DerivedPosition;;
}

Vector3Df CSceneNode::ConvertWorldToLocalPosition(const Vector3Df& worldPos)
{
    if (m_bNeedUpdate)
    {
        UpdateFromParent();
    }
    return m_DerivedRotation.Inverse() * (worldPos - m_DerivedPosition) / m_DerivedScale;
}


//bool CSceneNode::IsAllRenderablePSG( void )
//{
//	bool bFindNonePSG = false;
//	Renderable* pRenderable = NULL;
//	for (RenderableListIter it = m_lstRenderable.begin(); it != m_lstRenderable.end(); ++it)
//	{
//		pRenderable = *it;
//		if ( pRenderable->GetTypeString() != "pagestaticgeometry" )
//		{
//			bFindNonePSG = true;
//			break;
//		}
//	}
//	return !bFindNonePSG;
//}