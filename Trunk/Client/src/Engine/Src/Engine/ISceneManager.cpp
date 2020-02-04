#include "stdafx.h"
#include "ISceneManager.h"
//#include "RenderableFactory.h"
//#include "ParticleSystemManager.h"
//#include "ModelInstanceManager.h"
#include "ParticleSystem.h"
#include "Billboard.h"
#include "Water.h"
//#include "GEffectManager.h"
#include "PageStaticGeometory.h"
#include "Shadowmap.h"
#include "NavPathFinder.h"
#include "NavEditor.h"
#include "InstancingObjectManager.h"
//#include "ElectroLightening.h"
#include "RibbonTrail.h"
#include "DecalManager.h"
//#include "SpeedTreeInstManager.h"
#include "NodeAnimation.h"
//#include "Technique.h"
//#include "Pass.h"
#include "FileData.h"
#include "Terrain.h"
//#include "SpeedTreeEngine.h"
#include "ModelInstance.h"
#include "FileSystem.h"
#include "MemoryDataStream.h"
#include "WeatherManager.h"
#include "IOcclusionCull.h"
#include "SceneDebug.h"
#include "NormalSceneRenderProcess.h"

#define USE_RENDER_CONTEXT

AIRMap<AIRString, SceneCreateRenderableFunc> ISceneManager::s_mapSceneCreateRenderableFunc;

IMPLEMENT_ROOT_RTTI(ISceneManager);

int CompareSceneNodeDescending(CSceneNode* pObj1, CSceneNode* pObj2)
{
    CCamera* pCamera = ENGINE_INST->GetSceneMgr()->GetCamera();

    f32 fDis1 = pObj1->GetDistanceFromCamera(pCamera);
    f32 fDis2 = pObj2->GetDistanceFromCamera(pCamera);;
    return fDis1 < fDis2;
}

ISceneManager::ISceneManager() : m_pCamera(NULL), 
m_fogColor(CColorValue::White),
m_fogMode(eFOG_NONE),
m_fDensity(0.0f),
m_fFogStart(0.0f),
m_fFogEnd(0.0f),
m_nVisableObjsNum(0), 
//m_ScenePickMode(eSPM_BoundingBox),
m_SceneRayQueryMask(eRQM_SCENE)//, m_pBoundingBoxVB(NULL), m_pBoundingBoxMaterial(NULL)
, m_pResource(NULL)
, m_bReady(false)
, m_nRenderableIndex(0)
, m_bAttachPlantToTerrain(false)
, m_pOcclCuller(NULL)
//, m_bShadowmapEnable(false)
{
    //CreateBoundingBoxVBImpl();
    m_clrAmbient = CColorValue::White;
	m_bHasPickNodes = false;
    m_sunLight.SetType(CLight::LT_DIRECTIONAL);
    m_sunLight.SetAttentionConst(1.0f);
    m_sunLight.SetAmbient(CColorValue::Black);
    m_sunLight.SetSpecular(CColorValue::Black);

	CurFogToDefaultFog();

	SCENE_DEBUGER;

    //m_pDefaultSceneRenderer = new NormalSceneRenderProcess(FLOW_SCENE);
}

ISceneManager::~ISceneManager()
{
	//ClearBoundingBoxVBImpl();
}

void ISceneManager::SetCamera(CCamera* pCamera)
{
    //if (m_pCamera)
    //{
    //    SafeDelete(m_pCamera);
    //}
    m_pCamera = pCamera;
}

void ISceneManager::UpdateScene()
{
    if (!m_bReady)
    {
        if (m_pResource)
        {
            if (m_pResource->IsLoaded())
            {
                CacheScene();
            }
            else
            {
                return;
            }
        }
        else
        {
            m_bReady = true;
        }
    }
    
    if (m_bReady)
    {
        //--------------------------------------------------
	    //! add by hjj 2010-9-14
	    DefaultFogToCurFog();
	    //--------------------------------------------------

        float fTimeElapsed = ENGINE_INST->GetTimeSinceLastFrame();

        ENGINE_INST->TriggerEvent(Event_FrameStart, this, 0, 0);

        MapRenderableIter itRenderableEnd = m_mapSceneRenderables.end();
        Renderable* pRenderable = NULL;
        for (MapRenderableIter it = m_mapSceneRenderables.begin(); it != itRenderableEnd; ++it)
        {
            pRenderable = it->second;
            pRenderable->Update(fTimeElapsed);
        }

        INSTANCING_MANAGER->UpdateAllInstancingObjects(fTimeElapsed);
        //SPEEDTREE_ENGINE->Update(fTimeElapsed);
        

        LightMap_Iter itLightEnd = m_mapLights.end();
        for (LightMap_Iter it = m_mapLights.begin(); it != itLightEnd; ++it)
        {
            it->second->Update(fTimeElapsed);
        }
        
        
        SCENENODE_ITER itEnd = m_mapMovableNodes.end();
        for (SCENENODE_ITER it = m_mapMovableNodes.begin(); it != itEnd; ++it)
        {
            it->second->Update();
        }

        ApplySceneAnimations(fTimeElapsed);
        
        m_pCamera->Update();
        
        
        
	    //m_renderQueue.Clear();
        //m_lstVisableNodes.clear();
	    m_lstRangeNodes.clear();
        
#ifdef USE_RENDER_CONTEXT

#else
        GetVisableObjects();
        ProcessVisableObjects();
#endif
        
        //WATER_MANAGER->UpdateWater(fTimeElapsed);
        
	    m_bHasPickNodes = false;
    }
	
}
/*
void ISceneManager::GetVisableObjects()
{
    CSceneNode* pNode = NULL;
    for (SCENENODE_ITER it = m_mapSceneNodes.begin(); it != m_mapSceneNodes.end(); ++it)
    {
        pNode = it->second;
        if (pNode->IsVisable(m_pCamera))
        {
            m_lstVisableNodes.push_back(pNode);
        }
    }

    m_nVisableObjsNum = (int)m_lstVisableNodes.size();

}

void ISceneManager::GetVisableObjects(f32 fDistanceIn)
{
    CSceneNode* pNode = NULL;
    for (SCENENODE_ITER it = m_mapSceneNodes.begin(); it != m_mapSceneNodes.end(); ++it)
    {
        pNode = it->second;
        if (m_pCamera->IsInDistance(pNode->GetDerivedPosition(), pNode->GetSphereRadius(), fDistanceIn) < fDistanceIn && pNode->IsVisable(m_pCamera))
        {
            m_lstVisableNodes.push_back(pNode);
        }
    }

    m_nVisableObjsNum = (int)m_lstVisableNodes.size();
}


void ISceneManager::GetVisableRenderables(CCamera* pCamera, CRenderQueue& renderQueue)
{
    renderQueue.Clear();
    AIRList<CSceneNode*> lstVisableNodes;

    CSceneNode* pNode = NULL;
    for (SCENENODE_ITER it = m_mapSceneNodes.begin(); it != m_mapSceneNodes.end(); ++it)
    {
        pNode = it->second;
        if (pNode->IsVisable(pCamera))
        {
            m_lstVisableNodes.push_back(pNode);
        }
    }

    //lstVisableNodes = (int)m_lstVisableNodes.size();

    AIRList<CSceneNode*>::iterator itEnd = lstVisableNodes.end();
    //for (LIST_SCENENODE_ITER it = lstVisableNodes.begin(); it != itEnd; ++it)
    //{
    //    (*it)->AddAttachObjToRenderQueue(&renderQueue);
    //}

    for (LIST_SCENENODE_ITER it = lstVisableNodes.begin(); it != itEnd; ++it)
    {
        (*it)->UpdateToRenderQueue(pCamera, &renderQueue);
    }
}

void ISceneManager::GetVisableRenderables(CCamera* pCamera, CRenderQueue& renderQueue, f32 fDistanceIn)
{
    renderQueue.Clear();
    AIRList<CSceneNode*> lstVisableNodes;

    CSceneNode* pNode = NULL;
    for (SCENENODE_ITER it = m_mapSceneNodes.begin(); it != m_mapSceneNodes.end(); ++it)
    {
        pNode = it->second;
        if (pCamera->IsInDistance(pNode->GetDerivedPosition(), pNode->GetSphereRadius(), fDistanceIn) < fDistanceIn && pNode->IsVisable(pCamera))
        {
            m_lstVisableNodes.push_back(pNode);
        }
    }

    //lstVisableNodes = (int)m_lstVisableNodes.size();

    AIRList<CSceneNode*>::iterator itEnd = lstVisableNodes.end();
    //for (LIST_SCENENODE_ITER it = lstVisableNodes.begin(); it != itEnd; ++it)
    //{
    //    (*it)->AddAttachObjToRenderQueue(&renderQueue);
    //}

    for (LIST_SCENENODE_ITER it = lstVisableNodes.begin(); it != itEnd; ++it)
    {
        (*it)->UpdateToRenderQueue(pCamera, &renderQueue);
    }
}
*/
/*
void ISceneManager::GetRenderQueue(CRenderQueue* pQueue)
{
    pQueue->Clear();
    AIRList<CSceneNode*>::iterator itEnd = m_lstVisableNodes.end();

    for (LIST_SCENENODE_ITER it = m_lstVisableNodes.begin(); it != itEnd; ++it)
    {
        (*it)->UpdateToRenderQueue(m_pCamera, pQueue);
    }
}
*/
/*
void ISceneManager::RenderScene()
{
    if (!m_pCamera)
    {
        return;
    }
    ENGINE_INST->SetActiveCamera(m_pCamera);
    //UpdateScene();
    Render(m_pCamera);
	//SCENE_DEBUGER->Render();
}
*/
void ISceneManager::ExecOcclusionCull()
{
    /*
	m_renderQueue.Sort();
	if ( !m_pOcclCuller )
	{
		IRenderer* pRenderer = ENGINE_INST->GetRenderer();
		m_pOcclCuller = pRenderer->CreateOcclusionCull();
	}
	m_pOcclCuller->RenderCullScene( &m_renderQueue );
    */
}

void ISceneManager::DestroyRenderable(Renderable* pRenderable, bool bAutoDeleteNode)
{
	if ( !pRenderable )
	{
		return;
	}

    MapRenderableIter it = m_mapSceneRenderables.find(pRenderable->GetID());
    if (it != m_mapSceneRenderables.end())
    {
        m_mapSceneRenderables.erase(it);
        OnSceneRenderableDestroy(pRenderable);
        CSceneNode* pNode =  bAutoDeleteNode ? pRenderable->GetNodeParent() : 0;
        pRenderable->Clear();
        SafeDelete(pRenderable);

        if (pNode && pNode->GetAttachRenderablesNum() == 0 && pNode->GetChildrenNum() == 0)
        {
            DestroySceneNode(pNode);
        }
    }
    /*
    if (pRenderable->GetTypeString() == "particlesystem")
    {
        CParticleSystem* pParticleSys = static_cast<CParticleSystem*>(pRenderable);
        PARTICLESYSTEM_MANAGER->DestroyParticleSystem(pParticleSys->GetID());
        pRenderable = NULL;
    }
    else if (pRenderable->GetTypeString() == "model")
    {
        MODELINST_MANAGER->ClearModelInstance(pRenderable);
        pRenderable = NULL;
    }
	//--------------------------------------------------
	//! add by hjj 2010-9-29
	else if (pRenderable->GetTypeString() == "el")
	{
		ELECTROLIGHT_MANAGER->DestroyElectroLightening(((ElectroLightening*)pRenderable)->GetID());
		pRenderable = NULL;
	}
	else if (pRenderable->GetTypeString() == "ribbontrail")
	{
		RIBBONTRAIL_MANAGER->DestroyRibbonTrail(((RibbonTrail*)pRenderable)->GetID());
		pRenderable = NULL;
	}
	//else if (pRenderable->GetTypeString() == "GEffect")
	//{
	//	GEFFECT_MANAGER->DestroyGEffect( (GEffect*)pRenderable );
	//	pRenderable = NULL;
	//}
	else if (pRenderable->GetTypeString() == "water")
	{
		WATER_MANAGER->DestroyWater( (CWater*)pRenderable );
		pRenderable = NULL;
	}
	else if (pRenderable->GetTypeString() == "pagestaticgeometry")
	{
		PSG_MANAGER->ClearPageStaticGeometry( (PageStaticGeometory*)pRenderable );
		pRenderable = NULL;
	}
    else if (strcmp(pRenderable->GetTypeString().c_str(), "TerrainDecal") == 0)
    {
        DECAL_MANAGER->DestroyTerrainDecal(static_cast<TerrainDecal*>(pRenderable));
        pRenderable = NULL;
    }
	//--------------------------------------------------
    else
    {
        
        for (RENDERABLE_LIST_ITER it = m_lstRenderable.begin(); it != m_lstRenderable.end(); ++it)
        {
            if (pRenderable == *it)
            {
                ClearRenderable(pRenderable);
                m_lstRenderable.erase(it);
                break;
            }
        }
        
    }
    */
}

/*
void ISceneManager::AddRenderableObject(Renderable* pRenderable)
{
    if (FindRenderable(pRenderable->GetID()))
    {
        DT_TO_MSGBOX(LT_WARN, "已经存在对象[%s]", pRenderable->GetName().c_str());
        return;
    }

    m_mapSceneRenderables.insert(std::make_pair(pRenderable->GetID(), pRenderable));
}
*/

void ISceneManager::ClearScene()
{
	WeatherManager::GetInstance()->Clear();

    // 先将scenenode和renderable断开
    for (SCENENODE_ITER it = m_mapSceneNodes.begin(); it != m_mapSceneNodes.end(); ++it)
    {
        CSceneNode* pNode = it->second;
        pNode->DetachAllRenderable();
        pNode->RemoveAllChildren();
        //SafeDelete(pNode);
    }

    // 销毁renderable
    DestroyAllRenderable();

    // 销毁场景节点
    for (SCENENODE_ITER it = m_mapSceneNodes.begin(); it != m_mapSceneNodes.end(); ++it)
    {
        CSceneNode* pNode = it->second;
        //pNode->DetachAllRenderable();
        //pNode->RemoveAllChildren();
        SafeDelete(pNode);
    }
    m_mapSceneNodes.clear();
    m_mapMovableNodes.clear();

    //删除场景动画
    NodeAnimationStateListIter itStateEnd = m_lstNodeAnimationStates.end();
    for (NodeAnimationStateListIter it = m_lstNodeAnimationStates.begin(); it != itStateEnd; ++it)
    {
        SafeDelete(*it);
    }
    m_lstNodeAnimationStates.clear();

    NodeAnimationMapIter itAniEnd = m_mapNodeAnimations.end();
    for (NodeAnimationMapIter it = m_mapNodeAnimations.begin(); it != itAniEnd; ++it)
    {
        SafeDelete(it->second);
    }
    m_mapNodeAnimations.clear();

    // 销毁摄像机
    if (m_pCamera)
    {
        SafeDelete(m_pCamera);
    }

	if (m_pOcclCuller)
	{
		m_pOcclCuller->Release();
		SafeDelete( m_pOcclCuller );
	}

	SCENE_DEBUGER->Clear();
}

void ISceneManager::DestroyAllRenderable()
{
	//--------------------------------------------------
	//! add by hjj 2010-6-12
	//GEFFECT_MANAGER->DestroyAllGEffect();

    MapRenderableIter itEnd = m_mapSceneRenderables.end();
    for (MapRenderableIter it = m_mapSceneRenderables.begin(); it != itEnd; ++it)
    {
        OnSceneRenderableDestroy(it->second);
        it->second->Clear();
        SafeDelete(it->second);
    }
    m_mapSceneRenderables.clear();

    //清空instancing
    INSTANCING_MANAGER->DestroyAllInstancingObjects();
    /*
	PSG_MANAGER->ClearAllPageStaticGeometry();
	WATER_MANAGER->DestroyAllWater();
	//--------------------------------------------------

    MODELINST_MANAGER->Clear();  //首先清空modelinst，因为modelinst里面可能会挂着粒子
    PARTICLESYSTEM_MANAGER->Clear();
	BILLBOARG_MANAGER->Clear();

	ELECTROLIGHT_MANAGER->Clear();
	RIBBONTRAIL_MANAGER->Clear();

    SPEEDTREEINST_MANAGER->DestroyAllTrees();

    
    */

    /*
    for (RENDERABLE_LIST_ITER it = m_lstRenderable.begin(); it != m_lstRenderable.end(); ++it)
    {
        Renderable* pRenderable = *it;
        {
            ClearRenderable(pRenderable);
        }
    }
    m_lstRenderable.clear();
    */

    //销毁所有光照
    LightMap_Iter itLightEnd = m_mapLights.end();
    for (LightMap_Iter it = m_mapLights.begin(); it != itLightEnd; ++it)
    {
        SafeDelete(it->second);
    }

    m_mapLights.clear();
    
}

void ISceneManager::ClearRenderable(Renderable* pRenderable)
{
    pRenderable->Clear();
    SafeDelete(pRenderable);
}

CSceneNode* ISceneManager::CreateSceneNode(const AIRString& strName, bool bMovable, const Vector3Df& vPosition /* = Vector3Df::ZERO */, const Quaternion& qRotation /* = Quaternion */, const Vector3Df& vScale)
{
    SCENENODE_ITER it = m_mapSceneNodes.find(strName);
    if (it != m_mapSceneNodes.end())
    {
        return it->second;
    }

    CSceneNode* pNode = CreateSceneNodeImp(strName, vPosition, qRotation, vScale);

    if (pNode)
    {
        m_mapSceneNodes.insert(std::make_pair(strName, pNode));
        if (bMovable)
        {
            //pNode->SetIsUpdatePerFrame(true);
            pNode->SetMovable(true);
            m_mapMovableNodes.insert(std::make_pair(strName, pNode));
        }
    }
	else
	{
		DT_TO_MSGBOX( LT_WARN, GetString("创建场景节点 %s 失败!", strName.c_str()) );
	}
    
    return pNode;
}


CSceneNode* ISceneManager::CreateSceneNodeImp(const AIRString& strName, const Vector3Df& vPosition /* = Vector3Df::ZERO */, const Quaternion& qRotation /* = Quaternion */, const Vector3Df& vScale)
{
    return new CSceneNode(this, strName, vPosition, qRotation, vScale);
}

void ISceneManager::DestroySceneNode(const AIRString& strName)
{
    SCENENODE_ITER it = m_mapMovableNodes.find(strName);
    if (it != m_mapMovableNodes.end())
    {
        m_mapMovableNodes.erase(it);
    }

    SCENENODE_ITER its = m_mapSceneNodes.find(strName);
    if (its != m_mapSceneNodes.end())
    {
        CSceneNode* pNode = its->second;
        pNode->DetachAllRenderable();
        //pNode->RemoveAllChildren();
        SafeDelete(pNode);
        m_mapSceneNodes.erase(its);
    }
}

void ISceneManager::DestroySceneNode(CSceneNode* pNode)
{
    /*
    LIST_SCENENODE::iterator it = std::find(m_lstVisableNodes.begin(), m_lstVisableNodes.end(), pNode);
    if (it != m_lstVisableNodes.end())
    {
        m_lstVisableNodes.erase(it);
    }
    */
    DestroySceneNode(pNode->GetName());
}

CSceneNode* ISceneManager::FindSceneNode(const AIRString& strName)
{
    SCENENODE_ITER it = m_mapSceneNodes.find(strName);
    if (it != m_mapSceneNodes.end())
    {
        return it->second;
    }

    return NULL;
}
/*
void ISceneManager::Render(CCamera* pCamera)
{
    //SPEEDTREE_ENGINE->UpdateCamera(pCamera);
    Matrix4f matProj, matView;
    pCamera->GetViewMatrix(&matView);
    pCamera->GetProjectionMatrix(&matProj);
    IRenderer* pRenderer = ENGINE_INST->GetRenderer();
    pRenderer->SetViewTransform(&matView);
    pRenderer->SetProjectionTransform(&matProj);
    //必须每帧设置环境光颜色才能生效
    pRenderer->SetAmbientLight(m_clrAmbient);

    CGPUDataParamSource* pGPUData = pRenderer->GetGPUDataParam();
    pGPUData->SetCameraPosition(pCamera->GetPosition());
    //pGPUData->SetViewMatrix(matView);
    //pGPUData->SetProjectionMatrix(matProj);
    pGPUData->SetAmbientColor(m_clrAmbient);
    pGPUData->SetFogColor(m_fogColor);
    pGPUData->SetFogStart(m_fFogStart);
    pGPUData->SetFogEnd(m_fFogEnd);
    pGPUData->SetFogDensity(m_fDensity);
    pGPUData->SetFogType(m_fogMode);
    pGPUData->SetSunDirection(m_sunLight.GetDerivedDirection());
    pGPUData->SetSunLightColor(m_sunLight.GetDiffuse());
    pGPUData->SetCameraFarPlane(pCamera->GetViewFar());

    //设置雾
    pRenderer->SetFog(m_fogMode, m_fogColor, m_fDensity, m_fFogStart, m_fFogEnd);

#ifdef USE_RENDER_CONTEXT
    m_pDefaultSceneRenderer->SetCamera(m_pCamera, false);
    m_pDefaultSceneRenderer->AttachRenderWindow(pRenderer->GetRenderWindow());
    m_pDefaultSceneRenderer->Render();
#else
    m_renderQueue.Render(pCamera);
#endif
}
*/


LIST_SCENENODE& ISceneManager::GetPickNodes( const Vector3Df& vOrg, const Vector3Df& vPickDir, bool bNeedArrange /* = false */ )
{
	if ( m_bHasPickNodes )
	{
		return m_lstPickNodes;
	}
	m_bHasPickNodes = true;

	// 清空列表
	m_lstPickNodes.clear();

	// 需要按顺序将节点排列
	if ( bNeedArrange )
	{
		// 遍历每个可见节点
        /*
		for (LIST_SCENENODE_ITER it = m_lstVisableNodes.begin(); it != m_lstVisableNodes.end(); ++it)
		{
			CSceneNode* pNode = *it;
			// 检查当前节点是否可选取
			if ( pNode->IsPick(vOrg, vPickDir))
			{
				// 计算节点到摄像机的距离
				float len = pNode->GetDistanceFromCamera( m_pCamera );

				// 按照距离,从小到大将节点插入列表
				bool bInsert = false;
				LIST_SCENENODE_ITER it_pick = m_lstPickNodes.begin();
				for ( ; it_pick != m_lstPickNodes.end(); ++it_pick )
				{
					if ( len > (*it_pick)->GetDistanceFromCamera(m_pCamera) )
					{
						m_lstPickNodes.insert( it_pick, pNode );
						bInsert = true;
						break;
					}
				}
				if ( !bInsert && it_pick == m_lstPickNodes.end() )
				{
					m_lstPickNodes.push_back( pNode );
				}
			}
		}
        */
	}
	// 不需要按顺序将节点排列
	else
	{
        /*
		for (LIST_SCENENODE_ITER it = m_lstVisableNodes.begin(); it != m_lstVisableNodes.end(); ++it)
		{
			CSceneNode* pNode = *it;
			if ( pNode->IsPick(vOrg, vPickDir))
			{
				m_lstPickNodes.push_back( pNode );
			}
		}
        */
	}

	return m_lstPickNodes;
}



// 获得测试包围盒内的节点
LIST_SCENENODE& ISceneManager::GetRangeNodes( const AABBBox3Df& rangeBoundingBox )
{
	// 清空列表
	m_lstRangeNodes.clear();
    /*
	for (LIST_SCENENODE_ITER it = m_lstVisableNodes.begin(); it != m_lstVisableNodes.end(); ++it)
	{
		CSceneNode* pNode = *it;
		if ( pNode->IsIntersect( rangeBoundingBox ) )
		{
			m_lstRangeNodes.push_back( pNode );
		}
	}
    */

	return m_lstRangeNodes;
}



CSceneNode* ISceneManager::GetNearestPickNode(const Vector3Df& vOrg, const Vector3Df& vPickDir)
{
    AIRVector<CSceneNode*> vtPickActors;
    CSceneNode* pNearestActor = NULL;
	//遍历所有可见节点
    /*
    for (LIST_SCENENODE_ITER it = m_lstVisableNodes.begin(); it != m_lstVisableNodes.end(); ++it)
    {
        CSceneNode* pNode = *it;
        //检查节点包围盒是否与射线有相交
        if ( pNode->IsPick(vOrg, vPickDir))
        {
            vtPickActors.push_back(pNode);
        }
    }
    */

    if (vtPickActors.size() == 1)
    {
        return vtPickActors[0];
    }
    else
    {
		CSceneNode* pActor = NULL;
		float fNearDistance = 0xfffffffe;
		
        //取离摄像机最近的那个
        for (u32 i = 0; i < vtPickActors.size(); i++)
        {
            if (pNearestActor == NULL)
            {
                pNearestActor = vtPickActors[i];
				fNearDistance = pNearestActor->GetDistanceFromCamera(m_pCamera);
            }
            else
            {
                pActor = vtPickActors[i];
                float fCurDistance = pActor->GetDistanceFromCamera(m_pCamera);
                if (fCurDistance < fNearDistance && (m_SceneRayQueryMask == eRQM_SCENE || pActor->GetRayQueryMask() & m_SceneRayQueryMask))
                {
                    pNearestActor = pActor;
					fNearDistance = fCurDistance;
                }
            }
        }
    }

    return pNearestActor;
}
/*
void ISceneManager::ProcessVisableObjects()
{
    for (LIST_SCENENODE_ITER it = m_lstVisableNodes.begin(); it != m_lstVisableNodes.end(); ++it)
    {
        (*it)->UpdateToRenderQueue(m_pCamera,&m_renderQueue);
    }
}
*/
//void ISceneManager::CreateBoundingBoxVBImpl( void )
//{
//	IRenderer* pRenderer = ENGINE_INST->GetRenderer();
//	m_pBoundingBoxVB = pRenderer->CreateVertexBuffer(E_VB_NOTEX, 12 * 2);
//}
//
//void ISceneManager::ClearBoundingBoxVBImpl( void )
//{
//	if ( m_pBoundingBoxVB )
//	{
//		m_pBoundingBoxVB->Release();
//		SafeDelete( m_pBoundingBoxVB );
//	}
//}

/*
void ISceneManager::RenderBoundingBoxImpl( Vector3Df* pVertexBuf )
{
	//  /3--------/7
	// /  |      / |
	///   |     /  |
	//1---------5  |
	//|   2- - -| -6
	//|  /      |  /
	//|/        | /
	//0---------4/
	if ( m_pBoundingBoxVB )
	{
		int arID[] = { 0,1, 4,5, 6,7, 2,3, 1,5, 0,4, 2,6, 3,7, 1,3, 5,7, 4,6, 0,2 };
		LPVERTEX_NOTEX pVertex = (LPVERTEX_NOTEX)m_pBoundingBoxVB->Lock();
		for ( int i = 0; i < 24; )
		{ 
			pVertex[i].position = pVertexBuf[ arID[i] ];
			pVertex[i++].color = 0xffffffff;
		}
		m_pBoundingBoxVB->Unlock();
		IRenderer* pRenderer = ENGINE_INST->GetRenderer();
		pRenderer->SetWorldTransform(&Matrix4f::IDENTITY);
		pRenderer->SetTexture(0, NULL);
		pRenderer->SetVertexShader(NULL);
		pRenderer->SetPixelShader(NULL);
		pRenderer->SetVertexBuffer(0, m_pBoundingBoxVB);
		pRenderer->DrawLineList(0, 12);
	}
}
*/

CLight* ISceneManager::CreateLight(const char* szName)
{
    LightMap::iterator it = m_mapLights.find(szName);
    if (it != m_mapLights.end())
    {
        return NULL;
    }

    CLight* pLight = new CLight();
    pLight->SetName(szName);
    m_mapLights.insert(std::make_pair(szName, pLight));
    return pLight;
}

void ISceneManager::PopulateLightList(const Vector3Df& position, LightList& destList, float fRadus)
{
    destList.clear();

    LightMap_Iter itEnd = m_mapLights.end();
    CLight* pLight = NULL;
    for (LightMap_Iter it = m_mapLights.begin(); it != itEnd; ++it)
    {
        pLight = it->second;
        if (pLight->GetType() == CLight::LT_DIRECTIONAL)
        {
            destList.push_back(pLight);
        }
        else
        {
            float fDistance = (position - pLight->GetPosition()).GetLength();
            if (fDistance < fRadus && pLight->GetRange() > fDistance)
            {
                destList.push_back(pLight);
            }
        }
    }

    //对destList进行排序，距离position最近的排在最前，最远的排在最后
    if (m_sunLight.GetDirection() != Vector3Df::ZERO)
    {
        destList.push_back(&m_sunLight);
    }
}

void ISceneManager::PopulateLightList(CSceneNode* pNode, LightList& destList, float fRadus)
{
    PopulateLightList(pNode->GetDerivedPosition(), destList, fRadus);
}

void ISceneManager::SetAmbientLight(const CColorValue& color)
{
    m_clrAmbient = color;
    //IRenderer* pRenderer = ENGINE_INST->GetRenderer();
    //pRenderer->SetAmbientLight(m_clrAmbient);
}

void ISceneManager::SetFogColor(const CColorValue& color)
{
    m_fogColor = color;
}

void ISceneManager::SetFogDensity(float fDensity)
{
    m_fDensity = fDensity;
}

void ISceneManager::SetFogMode(FogMode mode)
{
    m_fogMode = mode;
}

void ISceneManager::SetFogStart(float fStart)
{
    m_fFogStart = fStart;
}

void ISceneManager::SetFogEnd(float fEnd)
{
    m_fFogEnd = fEnd;
}
/*
void ISceneManager::RenderVisableObjects(CCamera* pCamera)
{
    size_t renderQueueGroupNum = m_renderQueue.GetRenderQueueGroupsNum();
    for (size_t i = 0; i < renderQueueGroupNum; ++i)
    {
        RenderQueueGroup* pGroup = m_renderQueue.GetRenderQueueGroup((RenderGroup_Type)i);
        if (pGroup)
        {
            //取得
            RenderQueueGroup::RenderableList* pRenderableList = pGroup->GetRenderableList();
            RenderQueueGroup::RenderableList_Iter itEnd = pRenderableList->end();
            for (RenderQueueGroup::RenderableList_Iter it = pRenderableList->begin(); it != itEnd; ++it)
            {
                RenderSingleObject(pCamera, *it);
            }
        }
    }
}
*/
void ISceneManager::RenderSingleObject(CCamera* pCamera, Renderable* pRenderable)
{
    
    //渲染单个物体，这里要走以下的渲染流程
    IRenderer* pRenderer = ENGINE_INST->GetRenderer();
    IMaterial* pMaterial = pRenderable->GetMaterial();
    CGPUDataParamSource* pGPUDataParam = pRenderer->GetGPUDataParam();

    /*
    CTechnique* pTechnique = pMaterial->GetCurrentTechnique();

    if (pTechnique)
    {
        size_t passNum = pTechnique->GetPassesNum();
        for (size_t i = 0; i < passNum; ++i)
        {
            CPass* pPass = pTechnique->GetPass(i);
            if (pPass)
            {
                //1、设置矩阵
                if (pPass->HasVertexShader())
                {
                    pGPUDataParam->SetWorldMatrix(pRenderable->GetWorldMatrix());
                }
                else
                {
                    pRenderer->SetWorldTransform(&pRenderable->GetWorldMatrix());
                }

                //2、设置pass（纹理、采样）
                pPass->ApplyPass(pRenderer);

                //3、处理光照
                pRenderable->ProcessLights(pPass);

                //4、设置渲染状态（包括shader的参数）

                //5、准备渲染数据（SetStreamSource）
                //pRenderable->PreRender()

                //6、渲染(DrawPrimitive)
            }
        }
    }
    */

    /*
    //1、设置矩阵
    if (pMaterial && pMaterial->HasVertexShader())
    {
        pGPUDataParam->SetWorldMatrix(pRenderable->GetWorldMatrix());
    }
    else
        pRenderer->SetWorldTransform(&pRenderable->GetWorldMatrix());

    //2、设置材质（纹理、采样）
    pMaterial->ApplyMaterial(pRenderer);

    //3、设置光照
    pRenderable->ProcessLights();

    //4、设置渲染状态（包括shader的参数）

    //5、准备渲染数据（SetStreamSource）

    //6、渲染(DrawPrimitive)
    */
}

bool ISceneManager::ChangeSceneNodeName(CSceneNode* pNode, const AIRString& strName)
{
    AIRString strOrigName = pNode->GetName();
    

    SCENENODE_ITER itExist = m_mapSceneNodes.find(strName);
    if (itExist != m_mapSceneNodes.end())
    {
        return false;
    }

    SCENENODE_ITER it = m_mapSceneNodes.find(strOrigName);
    if (it != m_mapSceneNodes.end())
    {
        m_mapSceneNodes.erase(it);
    }
    
    
    m_mapSceneNodes.insert(std::make_pair(strName, pNode));

    if (pNode->IsMovable())
    {
        SCENENODE_ITER itMove = m_mapMovableNodes.find(strOrigName);
        if (itMove != m_mapMovableNodes.end())
        {
            m_mapMovableNodes.erase(itMove);
        }
        m_mapMovableNodes.insert(std::make_pair(strName, pNode));
    }
    return true;
}

void ISceneManager::SetFogEnable(bool bFogEnable)
{
    IRenderer* pRenderer = ENGINE_INST->GetRenderer();
    if (!bFogEnable)
    {
        pRenderer->SetFog(eFOG_NONE, m_fogColor, m_fDensity, m_fFogStart, m_fFogEnd);
    }
    else
    {
        pRenderer->SetFog(m_fogMode, m_fogColor, m_fDensity, m_fFogStart, m_fFogEnd);
    }
}

Renderable* ISceneManager::FindRenderable(u32 nID)
{
    Renderable* pRenderable = NULL;
    /*
    pRenderable = MODELINST_MANAGER->FindModelInstance(strName);
    if (pRenderable != NULL)
    {
        return pRenderable;
    }
    */
    MapRenderableIter it = m_mapSceneRenderables.find(nID);
    if (it != m_mapSceneRenderables.end())
    {
        return it->second;
    }

    return pRenderable;
}

void ISceneManager::EnableShadowMap(bool bEnable)
{
    //m_bShadowmapEnable = bEnable;
    ENGINE_INST->SetShadowmapEnable(bEnable);
    if (bEnable)
    {
        
        ENGINE_INST->InitializeShadowmap();
        SHADOWMAP->SetLightDirection(m_sunLight.GetDirection());
        SHADOWMAP->SetLightPosition(m_sunLight.GetPosition());
    }
}

Renderable* ISceneManager::GetNearestPickObject( const Vector3Df& vOrg, const Vector3Df& vEnd, ScenePickMode spm /* = eSPM_Face */ )
{
	if ( spm == eSPM_Face )
	{
		return GetNearestPickObjectByFace( vOrg, vEnd );
	}
	else if ( spm == eSPM_BoundingBox )
	{
		return GetNearestPickObjectByBoundingBox( vOrg, vEnd );
	}

    return NULL;
}

CModelInstance* ISceneManager::GetNearestPickModel( const Vector3Df& vOrg, const Vector3Df& vEnd, ScenePickMode spm /* = eSPM_Face */ )
{
	if ( spm == eSPM_Face )
	{
		return GetNearestPickModelByFace( vOrg, vEnd );
	}
	else if ( spm == eSPM_BoundingBox )
	{
		return GetNearestPickModelByBoundingBox( vOrg, vEnd );
	}

    return NULL;
}

Renderable* ISceneManager::GetNearestPickObjectByFace(const Vector3Df& vOrg, const Vector3Df& vEnd)
{
	//记录击中的场景节点
    AIRVector<CSceneNode*> vtPickActors;
	CSceneNode* pNode = NULL;
    /*
    for (LIST_SCENENODE_ITER it = m_lstVisableNodes.begin(); it != m_lstVisableNodes.end(); ++it)
    {
        pNode = *it;
        if ( pNode->IsPick(vOrg, vEnd))
        {
            vtPickActors.push_back(pNode);
        }
    }
    */

	//点击射线
    Vector3Df vPickDir = vEnd - vOrg;
    vPickDir.Normalize();

	float fCurDistFromEyeToObject = float(0xfffffffe);
	float fMinDistFromEyeToObject = float(0xfffffffe);
	Renderable* pNearestObject = NULL;
	Renderable* pRenderable = NULL;
	AABBBox3Df box;

	for (u32 i = 0; i < vtPickActors.size(); i++)
	{
		//取得场景节点里面的所有渲染物
		ListIterator<CSceneNode::RenderableList> iter = vtPickActors[i]->GetMapItorRenderable();
		while (!iter.IsEnd())
		{
			pRenderable = iter.GetNext();
			if (pRenderable->GetTypeString() == "model")
			{
				if ((static_cast<CModelInstance*>(pRenderable))->IsPick(vOrg, vPickDir, fCurDistFromEyeToObject))
				{
					if ( fCurDistFromEyeToObject < fMinDistFromEyeToObject )
					{
						fMinDistFromEyeToObject = fCurDistFromEyeToObject;
						pNearestObject = pRenderable;
					}
				}
			}
			else if (pRenderable->GetTypeString() == "water")
			{
				if ((static_cast<CWater*>(pRenderable))->IsPick(vOrg, vPickDir, fCurDistFromEyeToObject))
				{
					if ( fCurDistFromEyeToObject < fMinDistFromEyeToObject )
					{
						fMinDistFromEyeToObject = fCurDistFromEyeToObject;
						pNearestObject = pRenderable;
					}
				}
			}
			else 
			{
				box = pRenderable->GetWorldBoundingBox(false);
				if ( box.IntersectsWithLine(vOrg, vPickDir, float(0xfffffffe)) )
				{
					fCurDistFromEyeToObject = box.GetCenter().GetDistanceFrom( vOrg );
					if ( fCurDistFromEyeToObject < fMinDistFromEyeToObject )
					{
						fMinDistFromEyeToObject = fCurDistFromEyeToObject;
						pNearestObject = pRenderable;
					}
				}				
			}
		}
	}
	return pNearestObject;

    //if (vtPickActors.size() == 1)
    //{
    //    MapIterator<CSceneNode::MAP_RENDERABLE> iter = vtPickActors[0]->GetMapItorRenderable();
    //    while (!iter.IsEnd())
    //    {
    //        Renderable* pRenderable = iter.GetNext();
    //        if (pRenderable->GetTypeString() == "model")
    //        {
    //            if ((static_cast<CModelInstance*>(pRenderable))->IsPick(vOrg, vPickDir))
    //            {
    //                return pRenderable;
    //            }
    //        }
    //        else
    //        {
    //            return pRenderable;
    //        }
    //    }
    //    return NULL;
    //}
    //else
    //{
    //    //取离摄像机最近的那个
    //    //由近到远排序
    //    std::sort(vtPickActors.begin(), vtPickActors.end(), CompareSceneNodeDescending);
    //    
    //    for (u32 i = 0; i < vtPickActors.size(); i++)
    //    {
    //        MapIterator<CSceneNode::MAP_RENDERABLE> iter = vtPickActors[i]->GetMapItorRenderable();
    //        while (!iter.IsEnd())
    //        {
    //            Renderable* pRenderable = iter.GetNext();
    //            if (pRenderable->GetTypeString() == "model")
    //            {
    //                if ((static_cast<CModelInstance*>(pRenderable))->IsPick(vOrg, vPickDir))
    //                {
    //                    return pRenderable;
    //                }
    //            }
    //            else 
    //            {
    //                return pRenderable;
    //            }
    //        }
    //    }
    //    
    //}

    return NULL;
}


Renderable* ISceneManager::GetNearestPickObjectByBoundingBox(const Vector3Df& vOrg, const Vector3Df& vEnd)
{
	//记录击中的场景节点
	AIRVector<CSceneNode*> vtPickActors;
	CSceneNode* pNode = NULL;
    /*
	for (LIST_SCENENODE_ITER it = m_lstVisableNodes.begin(); it != m_lstVisableNodes.end(); ++it)
	{
		pNode = *it;
		if ( pNode->IsPick(vOrg, vEnd))
		{
			vtPickActors.push_back(pNode);
		}
	}
    */

	//点击射线
	Vector3Df vPickDir = vEnd - vOrg;
	vPickDir.Normalize();

	float fCurDistFromEyeToObject = float(0xfffffffe);
	float fMinDistFromEyeToObject = float(0xfffffffe);
	Renderable* pNearestObject = NULL;
	Renderable* pRenderable = NULL;
	AABBBox3Df box;

	for (u32 i = 0; i < vtPickActors.size(); i++)
	{
		//取得场景节点里面的所有渲染物
		ListIterator<CSceneNode::RenderableList> iter = vtPickActors[i]->GetMapItorRenderable();
		while (!iter.IsEnd())
		{
			fCurDistFromEyeToObject = float(0xfffffffe);

			pRenderable = iter.GetNext();

			if ( pRenderable->IsPickByWorldBoundingBox(vOrg, vPickDir, fCurDistFromEyeToObject) )
			{
				if ( fCurDistFromEyeToObject < fMinDistFromEyeToObject )
				{
					fMinDistFromEyeToObject = fCurDistFromEyeToObject;
					pNearestObject = pRenderable;
				}
			}				
		}
	}
	return pNearestObject;
}


CModelInstance* ISceneManager::GetNearestPickModelByFace(const Vector3Df& vOrg, const Vector3Df& vEnd)
{
    //记录击中的场景节点
    AIRVector<CSceneNode*> vtPickActors;
    CSceneNode* pNode = NULL;
    /*
    for (LIST_SCENENODE_ITER it = m_lstVisableNodes.begin(); it != m_lstVisableNodes.end(); ++it)
    {
        pNode = *it;
        if ( pNode->IsPick(vOrg, vEnd))
        {
            vtPickActors.push_back(pNode);
        }
    }
    */

    //点击射线
    Vector3Df vPickDir = vEnd - vOrg;
    vPickDir.Normalize();

    float fCurDistFromEyeToObject = float(0xfffffffe);
    float fMinDistFromEyeToObject = float(0xfffffffe);
    Renderable* pNearestObject = NULL;
    Renderable* pRenderable = NULL;
    AABBBox3Df box;

    for (u32 i = 0; i < vtPickActors.size(); i++)
    {
        //取得场景节点里面的所有渲染物
        ListIterator<CSceneNode::RenderableList> iter = vtPickActors[i]->GetMapItorRenderable();
        while (!iter.IsEnd())
        {
            pRenderable = iter.GetNext();
            if (pRenderable->GetTypeString() == "model")
            {
                if ((static_cast<CModelInstance*>(pRenderable))->IsPick(vOrg, vPickDir, fCurDistFromEyeToObject))
                {
                    if ( fCurDistFromEyeToObject < fMinDistFromEyeToObject )
                    {
                        fMinDistFromEyeToObject = fCurDistFromEyeToObject;
                        pNearestObject = pRenderable;
                    }
                }
            }
        }
    }
    return static_cast<CModelInstance*>(pNearestObject);
}

CModelInstance*  ISceneManager::GetNearestPickModelByBoundingBox(const Vector3Df& vOrg, const Vector3Df& vEnd)
{
	//记录击中的场景节点
	AIRVector<CSceneNode*> vtPickActors;
	CSceneNode* pNode = NULL;
    /*
	for (LIST_SCENENODE_ITER it = m_lstVisableNodes.begin(); it != m_lstVisableNodes.end(); ++it)
	{
		pNode = *it;
		if ( pNode->IsPick(vOrg, vEnd))
		{
			vtPickActors.push_back(pNode);
		}
	}
    */

	//点击射线
	Vector3Df vPickDir = vEnd - vOrg;
	vPickDir.Normalize();

	float fCurDistFromEyeToObject = float(0xfffffffe);
	float fMinDistFromEyeToObject = float(0xfffffffe);
	Renderable* pNearestObject = NULL;
	Renderable* pRenderable = NULL;
	AABBBox3Df box;

	for (u32 i = 0; i < vtPickActors.size(); i++)
	{
		//取得场景节点里面的所有渲染物
		ListIterator<CSceneNode::RenderableList> iter = vtPickActors[i]->GetMapItorRenderable();
		while (!iter.IsEnd())
		{
			fCurDistFromEyeToObject = float(0xfffffffe);
			pRenderable = iter.GetNext();
			if (pRenderable->GetTypeString() == "model")
			{
				if ( pRenderable->IsPickByWorldBoundingBox(vOrg, vPickDir, fCurDistFromEyeToObject) )
				{
					if ( fCurDistFromEyeToObject < fMinDistFromEyeToObject )
					{
						fMinDistFromEyeToObject = fCurDistFromEyeToObject;
						pNearestObject = pRenderable;
					}
				}
			}
		}
	}
	return static_cast<CModelInstance*>(pNearestObject);
}

bool ISceneManager::FindPickNAVTriangle(const Line3Df& ray, NavTriangle& navTriOut)
{
    bool res = NAV_EDITOR->CalPickTriangle(ray, navTriOut);
    return res;
}

void ISceneManager::SetFixPipeLightingEnable(bool bEnable)
{
    CModelInstance* pModelInst = NULL;
    /*
    MapIterator<MODELINST_LIST> iter = MODELINST_MANAGER->GetAnimateModelList();
    while (!iter.IsEnd())
    {
        pModelInst = iter.GetNext();
        pModelInst->EnableLights(bEnable);
    }

    iter = MODELINST_MANAGER->GetStaticModelList();
    while (!iter.IsEnd())
    {
        pModelInst = iter.GetNext();
        pModelInst->EnableLights(bEnable);
    }
    */
}

void ISceneManager::ApplySceneAnimations(f32 fTimeElapse)
{
    NodeAnimationStateListIter itStateEnd = m_lstNodeAnimationStates.end();
    for (NodeAnimationStateListIter it = m_lstNodeAnimationStates.begin(); it != itStateEnd; ++it)
    {
        (*it)->AddTime(fTimeElapse);

        NodeAnimation* pAnimation = GetSceneNodeAnimation((*it)->GetName());
        //pAnimation->Apply((*it)->GetTimePos());
    }

    
}

void ISceneManager::AddNodeAnimation(NodeAnimation* pAnimation)
{
    m_mapNodeAnimations.insert(std::make_pair(pAnimation->GetName(), pAnimation));
}

NodeAnimation* ISceneManager::GetSceneNodeAnimation(const AIRString& strName)
{
    NodeAnimationMapIter it = m_mapNodeAnimations.find(strName);
    if (it != m_mapNodeAnimations.end())
    {
        return it->second;
    }

    return NULL;
}

/*
void ISceneManager::ResetAllRenderablesTechnique()
{
    MapRenderableIter itRenderableEnd = m_mapSceneRenderables.end();
    for (MapRenderableIter it = m_mapSceneRenderables.begin(); it != itRenderableEnd; ++it)
    {
        //it->second->ResetCurrentTechnique();
    }

    CHeightMap* pTerrain = TERRAIN->GetCurHeightmap();
    //pTerrain->ResetCurrentTechnique();

    INSTANCING_MANAGER->ResetAllInstancingTechnique();
}
*/
bool ISceneManager::CreatePlants(const char* szFilename)
{
	CDataStream* pStream = FILESYSTEM->GetFileStream( szFilename );
	if (pStream == NULL)
	{
		return false;
	}

	m_strPlantFile = szFilename;

	CMemoryDataStream memDataStream( *pStream, true );
	u8* pDataPtr = memDataStream.GetPtr();

	//是否将植被加到地形四叉树中
	m_bAttachPlantToTerrain = *(bool*)(pDataPtr);
	pDataPtr += sizeof(bool);

	//植被块数量
	u32 dwPsgCount = *(u32*)(pDataPtr);
	pDataPtr += sizeof(u32);

	PageStaticGeometory* pPSG = NULL;
	char szNodeName[256] = { 0 };
	CHeightMap* pTerrain = TERRAIN->GetCurHeightmap();
	for ( u32 i = 0; i < dwPsgCount; ++i )
	{
		if ( m_bAttachPlantToTerrain )
		{
		    pPSG = new PageStaticGeometory;
		    pPSG->SetID( m_nRenderableIndex++ );
		}
		else
		{
			pPSG = CreateRenderable<PageStaticGeometory>();
		}

		pPSG->SetMaxObjectCount( 10000 );
		pPSG->CreateFromBinData( &pDataPtr );
		//sprintf(szNodeName, "%s_plant_%d", this->m_strName.c_str(), i);
		CSceneNode* pNode = CreateSceneNode( pPSG->GetNodeName(), false, pPSG->GetObjectInitCenter());
        pNode->SetCullFlag(FLOW_SCENE | FLOW_REFLECTION | FLOW_RETRACTION);
		pNode->AttachRenderable(pPSG);   
		pNode->NeedUpdate();
		pNode->Update();

		if ( m_bAttachPlantToTerrain )
		{
		    CTerrainBlock* pBlock = pTerrain->GetTerrainBlock(pNode->GetDerivedPosition().x, pNode->GetDerivedPosition().z);
		    pBlock->AddRenderable(pPSG);
		}
	}

	SafeDelete( pStream );

	return true;
}

void ISceneManager::Cull(CCamera* pCamera, AIRList<CSceneNode*>& lstVisableNodes, u32 nCullFlag)
{
    CSceneNode* pNode = NULL;
    for (SCENENODE_ITER it = m_mapSceneNodes.begin(); it != m_mapSceneNodes.end(); ++it)
    {
        pNode = it->second;
        if ((pNode->GetCullFlag() & nCullFlag) && pNode->IsVisable(pCamera))
        {
            lstVisableNodes.push_back(pNode);
        }
    }
}
/*
u32 ISceneManager::GetRenderableObjectsNum() const
{
    return m_pDefaultSceneRenderer->GetVisableObjectsNum();
}
*/
