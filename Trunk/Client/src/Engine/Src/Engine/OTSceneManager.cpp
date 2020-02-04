#include "stdafx.h"
#include "OTSceneManager.h"
//#include "SystemMgr.h"
#include "MarkupSTL.h"
#include "Terrain.h"
#include "IMaterial.h"
//#include "D3D9Material.h"
#include "FileSystem.h"
#include "ThirdPersonCamera.h"
#include "ModelInstance.h"
#include "ParticleSystemManager.h"
//#include "RenderableFactory.h"
#include "PageStaticGeometory.h"
#include "Shadowmap.h"
#include "NavEditor.h"
#include "TimeBaseProfile.h"
#include "Octree.h"
#include "FileData.h"
//#include "SpeedTreeInstance.h"
#include "WeatherManager.h"
//#include "SkyBox.h"
#include "Water.h"
#include "HeightMap.h"
#include "StringUtil.h"

//--------------------------------------------------
//! add by hjj 2010-7-22
static float s_CreatePlantTime = 0.0f;
static float s_CreateWaterTime = 0.0f;
static float s_CreatePartiTime = 0.0f;
static float s_CreateModelTime = 0.0f;
static float s_CreateEffecTime = 0.0f;
//--------------------------------------------------



int CompareNavTriangleDescending(NavTriangle* pObj1, NavTriangle* pObj2)
{
    CCamera* pCamera = ENGINE_INST->GetSceneMgr()->GetCamera();

    f32 fDis1 = (pObj1->m_Triangle.GetCenterOfTriangle() - pCamera->GetPosition()).GetLength();
    f32 fDis2 = (pObj2->m_Triangle.GetCenterOfTriangle() - pCamera->GetPosition()).GetLength();
    return fDis1 < fDis2;
}

int ComparePickNavTriangle( NavIntersectInfo& tri1, NavIntersectInfo& tri2 )
{
	CCamera* pCamera = ENGINE_INST->GetSceneMgr()->GetCamera();

	f32 fDis1 = (tri1.m_vIntersectPoint - pCamera->GetPosition()).GetLength();
	f32 fDis2 = (tri2.m_vIntersectPoint - pCamera->GetPosition()).GetLength();
	return fDis1 < fDis2;
}

COTSceneManager::COTSceneManager() : m_nLightCount(0), m_pOctree(NULL), m_pHeightMap(NULL)
, m_bRenderWater(false)
{
    m_sunLight.SetDirection(Vector3Df(0, -1, 0));
}  

COTSceneManager::~COTSceneManager()
{

}

void COTSceneManager::InitScene()
{
    //��ʼ�������
    m_pCamera = new CCamera(Vector3Df(0.0, 200.0, -100.0), Vector3Df(0.0, 200.0, 0.0), 1.0, 1500.0);

    Vector3Df vCamera;
    m_pCamera->GetPosition(vCamera);


    //��ʼ��͸��ͶӰ����
    f32 fSkybox = 5000.0f; //m_pSkybox->GetWidth();
    f32 fFar = sqrtf(pow(0.5f * fSkybox, 2) + pow(0.5f * fSkybox, 2) + pow(0.5f * fSkybox, 2)) + 1.0f;
    IRenderer* pRenderer = ENGINE_INST->GetRenderer();
    

    m_pCamera->SetViewport(1.0f, fFar * 0.5f, AIR::PI * 0.25f, pRenderer->GetRenderTargetWidth(), pRenderer->GetRenderTargetHeight());

    Matrix4f matProj;
    m_pCamera->GetProjectionMatrix(&matProj);
    //matProj.BuildProjectionMatrixPerspectiveFovLH(AIR::PI / 4.0f, (f32)pRenderer->GetRenderTargetWidth() / pRenderer->GetRenderTargetHeight(), 1.0f, fFar);
    pRenderer->SetProjectionTransform(&matProj);

    //IMaterial* pTerrainMaterial = pRenderer->GetMaterial("Terrain");
    //pTerrainMaterial->RegisterCallBack(&CHeightMap::UpdateShader);
    //m_pHeightMap->SetMaterial(pTerrainMaterial);

    //FONTTYPE defaultType;
    //m_pText = pRenderer->CreateFontW(defaultType, L"Test");

    //m_Sun.Initialize(Vector3Df(-fTerrainBlockWidth, (float)(256 - 110) / 256.0f * fTerrainBlockWidth, (float)(400 - 256) / 256.0 * fTerrainBlockWidth), vCamera);
    //m_water.Create(Vector3Df(0.0, 80.0, 0.0), m_pHeightMap->GetWidth(), m_pHeightMap->GetWidth(), "./Media/water.jpg");
}

void COTSceneManager::DestroySceneNode(CSceneNode* pNode)
{
    if (m_pOctree)
    {
        m_pOctree->RemoveNode(static_cast<COctreeNode*>(pNode));
    }

    ISceneManager::DestroySceneNode(pNode);
}

void COTSceneManager::DestroySceneNode(const AIRString& strName)
{
    CSceneNode* pNode = FindSceneNode(strName);
    if (pNode)
    {
        if (m_pOctree)
        {
            m_pOctree->RemoveNode(static_cast<COctreeNode*>(pNode));
        }
    }
    ISceneManager::DestroySceneNode(strName);
}

void COTSceneManager::ClearScene()
{
	ISceneManager::ClearScene();
    //if (m_pSkybox)
    //{
    //    m_pSkybox->Clear();
    //    SafeDelete(m_pSkybox);
    //}

    //m_water.Clear();

    if (m_pOctree)
    {
        //m_pOctree->Clear();
        SafeDelete(m_pOctree);
    }

    m_Sun.Clear();

    //SafeDelete(m_pCamera);

    TERRAIN->Clear();

    m_pHeightMap = NULL;

    
}
/*
void COTSceneManager::GetVisableObjects()
{
    // �޳���8�����ϵĿɼ�����
    if (m_pOctree)
    {
        m_pOctree->GetVisableObjects(m_pCamera, m_lstVisableNodes);
    }

    m_nVisableObjsNum = (int)m_lstVisableNodes.size();

}

void COTSceneManager::GetVisableObjects(f32 fDistanceIn)
{
    if (m_pOctree)
    {
        m_pOctree->GetVisableObjects(m_pCamera, m_lstVisableNodes, fDistanceIn);
    }

    m_nVisableObjsNum = (int)m_lstVisableNodes.size();

}

void COTSceneManager::GetVisableRenderables(CCamera* pCamera, CRenderQueue& renderQueue)
{
    renderQueue.Clear();
    AIRList<CSceneNode*> lstVisableNodes;

    if (m_pOctree)
    {
        m_pOctree->GetVisableObjects(pCamera, lstVisableNodes);
    }

    // �����εĿɼ��ؿ�ӵ���Ⱦ����
    if (m_pHeightMap && m_pHeightMap->IsVisable())
    {
        //m_pHeightMap->GetV
        //m_pHeightMap->GetVisableObjects(m_pCamera, m_lstVisableNodes);
        m_pHeightMap->GetRenderObject(pCamera, &renderQueue);
        //renderQueue.AddRenderable(m_pHeightMap);
    }

    ////////////////////////////////////////////////////////////////////////////
    //// �������պ�,�ͽ���պм�����Ⱦ����
    //if ( m_pSkybox && m_pSkybox->IsVisable())
    //{
    //    renderQueue.AddRenderable( m_pSkybox );
    //}
    ////////////////////////////////////////////////////////////////////////////
    m_nVisableObjsNum = (int)m_lstVisableNodes.size();

    // ���ɼ��ڵ��ϵ�����ӵ���Ⱦ����
    LIST_SCENENODE_ITER itEnd = lstVisableNodes.end();
    //for (LIST_SCENENODE_ITER it = lstVisableNodes.begin(); it != itEnd; ++it)
    //{
    //    (*it)->AddAttachObjToRenderQueue(&renderQueue);
    //}

    for (LIST_SCENENODE_ITER it = lstVisableNodes.begin(); it != itEnd; ++it)
    {
        (*it)->UpdateToRenderQueue(pCamera, &renderQueue);
    }
}

void COTSceneManager::GetVisableRenderables(CCamera* pCamera, CRenderQueue& renderQueue, f32 fDistanceIn)
{
    renderQueue.Clear();
    AIRList<CSceneNode*> lstVisableNodes;

    if (m_pOctree)
    {
        m_pOctree->GetVisableObjects(pCamera, lstVisableNodes, fDistanceIn);
    }

    // �����εĿɼ��ؿ�ӵ���Ⱦ����
    if (m_pHeightMap && m_pHeightMap->IsVisable())
    {
        //m_pHeightMap->GetV
        //m_pHeightMap->GetVisableObjects(m_pCamera, m_lstVisableNodes);
        m_pHeightMap->GetRenderObject(pCamera, &renderQueue);
        //renderQueue.AddRenderable(m_pHeightMap);
    }

    ////////////////////////////////////////////////////////////////////////////
    //// �������պ�,�ͽ���պм�����Ⱦ����
    //if ( m_pSkybox && m_pSkybox->IsVisable())
    //{
    //    renderQueue.AddRenderable( m_pSkybox );
    //}
    ////////////////////////////////////////////////////////////////////////////
    m_nVisableObjsNum = (int)m_lstVisableNodes.size();

    // ���ɼ��ڵ��ϵ�����ӵ���Ⱦ����
    LIST_SCENENODE_ITER itEnd = lstVisableNodes.end();

    for (LIST_SCENENODE_ITER it = lstVisableNodes.begin(); it != itEnd; ++it)
    {
        (*it)->UpdateToRenderQueue(pCamera, &renderQueue);
    }
}
*/
void COTSceneManager::GetCollisionRenderables(const AABBBox3Df& aabbTest, AIRList<Renderable*>& renderList)
{
	renderList.clear();
	if (m_pOctree)
	{
		m_pOctree->GetCollisionRenderables(aabbTest, renderList);
	}
}

CSceneNode* COTSceneManager::CreateSceneNodeImp(const AIRString& strName, const Vector3Df& vPosition , const Quaternion& qRotation , const Vector3Df& vScale /* = Vector3Df::UNIT_SCALE */)
{
    return new COctreeNode(this, strName, vPosition, qRotation, vScale);
}

bool COTSceneManager::LoadFromXML(const char* szFilename)
{
    //m_pResource = static_cast<FileData*>(RESOURCE_MANAGER->CreateResource(Resource::Res_NormalFile, szFilename));
    m_pResource = RESOURCE_MANAGER->CreateResource<FileData>(szFilename);

    if (m_pResource && m_pResource->IsLoaded())
    {
        CacheScene();
    }

    return true;
}

void COTSceneManager::AddLight(CLight* pLight)
{
    m_mapLight.insert(make_pair(m_nLightCount++, pLight));
}


CLight* COTSceneManager::GetSunLight()
{
    return &m_sunLight;
}

/*
CLight* COTSceneManager::GetLight(u32 nID)
{
    LIGHT_ITER it = m_mapLight.find(nID);
    if (it != m_mapLight.end())
    {
        CLight* pLight = it->second;
        return pLight;
    }
    return NULL;
}
*/

bool COTSceneManager::ProcessInputEvent(const LPINPUTEVENT pEvent)
{
    return false;
}
/*
void COTSceneManager::GetRenderQueue(CRenderQueue* pQueue)
{
    ISceneManager::GetRenderQueue(pQueue);
    // �����εĿɼ��ؿ�ӵ���Ⱦ����
    if (m_pHeightMap && m_pHeightMap->IsVisable())
    {
        m_pHeightMap->GetRenderObject(m_pCamera, &m_renderQueue);
        //m_renderQueue.AddRenderable(m_pHeightMap);
    }

    //////////////////////////////////////////////////////////////////////////
    // �������պ�,�ͽ���պм�����Ⱦ����
    if ( m_pSkybox && m_pHeightMap->IsVisable() )
    {
        m_renderQueue.AddRenderable( m_pSkybox );
    }
    //////////////////////////////////////////////////////////////////////////
}
*/

void COTSceneManager::AddSceneNode(CSceneNode* pNode)
{
    if (m_pOctree)
    {
        m_pOctree->RemoveNode(static_cast<COctreeNode*>(pNode));
        m_pOctree->AddNode(static_cast<COctreeNode*>(pNode));
    }
}

void COTSceneManager::UpdateOctreeNode(COctreeNode* pNode)
{
    if (m_pOctree)
    {
        m_pOctree->UpdateOctreeNode(pNode);
    }
    else
    {
        m_pOctree = new COctree();
        m_pOctree->AddNode(pNode);
    }
}

void COTSceneManager::SetSceneBoundingBox(const AABBBox3Df& box)
{
    if (m_pOctree == NULL)
    {
        m_pOctree = new COctree;
    }
    m_pOctree->SetAABBBox(box);
    //ȫ���ڵ����һ��
    SCENENODE_ITER itEnd = m_mapSceneNodes.end();
    for (SCENENODE_ITER it = m_mapSceneNodes.begin(); it != itEnd; ++it)
    {
        UpdateOctreeNode(static_cast<COctreeNode*>(it->second));
    }
}

void COTSceneManager::LoadSceneNodeFromXML(CMarkupSTL& xml)
{
    AIRString strName = xml.GetAttrib("Name").c_str();

    Vector3Df vPosition;
    AIRString strValue = xml.GetAttrib("Position").c_str();
    sscanf(strValue.c_str(), "(%f %f %f)", &vPosition.x, &vPosition.y, &vPosition.z);
    Quaternion rotation;
    strValue = xml.GetAttrib("Rotation").c_str();
    sscanf(strValue.c_str(), "(%f %f %f %f)", &rotation.x, &rotation.y, &rotation.z, &rotation.w);

    strValue = xml.GetAttrib("Scale").c_str();
    Vector3Df vScale = Vector3Df::UNIT_SCALE;
    if (strValue.length() > 0)
    {
        sscanf(strValue.c_str(), "(%f %f %f)", &vScale.x, &vScale.y, &vScale.z);
    }

    strValue = xml.GetAttrib("Movable").c_str();
    bool bMovable = false;
    if (strValue == "TRUE")
    {
        bMovable = true;
    }
    

    COctreeNode* pNode = static_cast<COctreeNode*>(CreateSceneNode(strName, bMovable, vPosition, rotation, vScale)); //new COctreeNode(this, strValue, vPosition, rotation, vScale);

    AIRVector<AIRString> vtCullFlags;
    CStringUtil::SplitString(xml.GetAttrib("CullFlag").c_str(), AIRString("|"), vtCullFlags);

    if (!vtCullFlags.empty())
    {
        u32 nCullFlag = 0;
        for (size_t i = 0; i < vtCullFlags.size(); ++i)
        {
            nCullFlag |= EngineParamSetting::GetRenderFlow(vtCullFlags[i].c_str());
        }

        pNode->SetCullFlag(nCullFlag);
    }
    //pNode->NeedUpdate();
    //pNode->Update();
    xml.IntoElem();

    if (xml.FindElem("Obstruct"))
    {
        Obstruct* pObstruct = pNode->GetObstruct();
        xml.IntoElem();
        while (xml.FindElem("Face"))
        {
            ObstructFace face;
            strValue = xml.GetAttrib("topleft").c_str();
            sscanf(strValue.c_str(), "(%f %f %f)", face.topleft.x, face.topleft.y, face.topleft.z);
            strValue = xml.GetAttrib("topright").c_str();
            sscanf(strValue.c_str(), "(%f %f %f)", face.topright.x, face.topright.y, face.topright.z);
            strValue = xml.GetAttrib("bottomleft").c_str();
            sscanf(strValue.c_str(), "(%f %f %f)", face.bottomleft.x, face.bottomleft.y, face.bottomleft.z);
            strValue = xml.GetAttrib("bottomright").c_str();
            sscanf(strValue.c_str(), "(%f %f %f)", face.bottomright.x, face.bottomright.y, face.bottomright.z);
            pObstruct->AddObstructFace(face);
        }
        xml.OutOfElem();
    }

    while (xml.FindElem("Renderable"))
    {
		BEGIN_FUNC_PROFILE;

        
        Renderable* pRenderable = LoadSceneRenderableFromXML(xml, pNode);
		if ( pRenderable )
		{
			pNode->AttachRenderable(pRenderable);
		}
    }

    pNode->NeedUpdate();
    pNode->Update();
    
    while (xml.FindElem("SceneNode"))
    {
        LoadSceneNodeFromXML(xml);
    }

    xml.OutOfElem();
}

Renderable* COTSceneManager::LoadSceneRenderableFromXML(CMarkupSTL& xml, CSceneNode* pNode)
{
    AIRString strName = xml.GetAttrib("Name").c_str();
    AIRString strValue = xml.GetAttrib("Resource").c_str();
    AIRString strType = xml.GetAttrib( "Type" ).c_str();
    AIRString strCurAnim = xml.GetAttrib( "CurAnim" ).c_str();
    Renderable* pRenderable = NULL;

    if ( strType == "pagestaticgeometry" )
    {
		if ( ENGINE_INST->GetIsLoadPlant() )    //ΪʲôҪIsLoadPlant()��
		{
			//pRenderable = PSG_MANAGER->GetPageStaticGeometry( strName.c_str() );
            //��Ϊplant�Ѿ�������psg_manager���洴�������������Ǵ�������plant��

			//s_CreatePlantTime += END_FUNC_PROFILE;
		}

    }
    else if ( strType == "water" )
    {        
        CWater* pWater = CreateRenderable<CWater>(); //(CWater*)CreateRenderable(strName.c_str(), strType.c_str());   //WATER_MANAGER->CreateDefaultWaterImpl("DefaultWater");
        pRenderable = pWater;
        pWater->CreateFromXML( xml );
        //WATER_MANAGER->AddWater( pWater );

        //s_CreateWaterTime += END_FUNC_PROFILE;
        
    }
    else
    {
        if ( strType == "particlesystem" )
        {
            pRenderable = CreateRenderable<CParticleSystem>(strValue.c_str()); //CreateRenderable(strName.c_str(), strType.c_str(), strValue.c_str());

            //END_FUNC_PROFILE_WITH_REPORT( GetString("Create particle system [%s].", strName.c_str() ) );
            //s_CreatePartiTime += END_FUNC_PROFILE;
            if (pRenderable)
            {
                m_lstPaparedRenderables.push_back(pRenderable);
            }
            
        }
        else if ( strType == "GEffect" )
        {
            //pRenderable = CreateRenderable(strName.c_str(), strType.c_str(), strValue.c_str());

            //END_FUNC_PROFILE_WITH_REPORT( GetString("Create geffect [%s].", strName.c_str() ) );
            //s_CreateEffecTime += END_FUNC_PROFILE;
        }
        //--------------------------------------------------
        //! add by hjj 2010-6-26
        else if ( strType == "model"/* || strType == "speedtree"*/)
        {
            //pRenderable = CreateRenderable(strName.c_str(), strType.c_str(), strValue.c_str());
            CModelInstance* pModelInst = NULL;
            if (strType == "model")
            {
                pModelInst = CreateRenderable<CModelInstance>(strValue.c_str()); //static_cast<CModelInstance*>(pRenderable);
            }
            else if (strType == "speedtree")
            {
                //����Ҫ˼��һ�����Ū��speedtree
                //pModelInst = CreateRenderable<SpeedTreeInstance>(strValue.c_str());
            }
            if ( !pModelInst->IsStatic() && strCurAnim.length() )
            {
                pModelInst->SetCurrentAnimation( strCurAnim.c_str() );
                pModelInst->GetCurrentAnimation()->SetLoop( true );
            }

            strValue = xml.GetAttrib("InstancingEnable").c_str();
            if (strValue == "TRUE")
            {
                pModelInst->SetInstancingEnable(true);
            }
            else
            {
                pModelInst->SetInstancingEnable(false);
            }

            strValue = xml.GetAttrib("AcceptShadowmap").c_str();
            if (strValue == "TRUE")
            {
                pModelInst->AcceptShadowmap(true);
            }
            else
            {
                pModelInst->AcceptShadowmap(false);
            }

            strValue = xml.GetAttrib("RenderToShadow").c_str();
            if (strValue == "TRUE")
            {
                pModelInst->SetRenderToShadow(true);
            }
            else
            {
                pModelInst->SetRenderToShadow(false);
            }

            strValue = xml.GetAttrib("AcceptDeferredLighting").c_str();
            if (strValue == "TRUE")
            {
                pModelInst->SetAcceptLighting(true);
            }
            else
            {
                pModelInst->SetAcceptLighting(false);
            }

            strValue = xml.GetAttrib("Alphablend").c_str();
            if (strValue == "TRUE")
            {
                strValue = xml.GetAttrib("alpha").c_str();
                pModelInst->SetTransparent(true, atoi(strValue.c_str()));
            }

            if (pModelInst)
            {
                m_lstPaparedRenderables.push_back(pModelInst);
            }
            

            pRenderable = pModelInst;
            //END_FUNC_PROFILE_WITH_REPORT( GetString("Create model [%s].", strName.c_str() ) );
            //s_CreateModelTime += END_FUNC_PROFILE;
        }
        else if (strcmp(strType.c_str(), "light") == 0)
        {
            //����ǹ�Դ
            pRenderable = CreateLight(strName.c_str());
            CLight* pLight = static_cast<CLight*>(pRenderable);
            strValue = xml.GetAttrib("Range").c_str();
            pLight->SetRange(atof(strValue.c_str()));
            CColorValue diffuse;
            strValue = xml.GetAttrib("Color").c_str();
            sscanf(strValue.c_str(), "%f, %f, %f, %f", &diffuse.r, &diffuse.g, &diffuse.b, &diffuse.a);
            pLight->SetDiffuse(diffuse);
            strValue = xml.GetAttrib("Position").c_str();
            Vector3Df position;
            sscanf(strValue.c_str(), "%f, %f, %f", &position.x, &position.y, &position.z);
            pLight->SetPosition(position);
        }
        else
        {
            AIRMap<AIRString, SceneCreateRenderableFunc>::iterator it = ISceneManager::s_mapSceneCreateRenderableFunc.find(strType);

            if (it != ISceneManager::s_mapSceneCreateRenderableFunc.end())
            {
                SceneCreateRenderableFunc pFunc = it->second;
                pRenderable = pFunc(xml, this, strValue);

                if (pRenderable)
                {
                    m_lstPaparedRenderables.push_back(pRenderable);
                }
            }
        }
        //--------------------------------------------------
    }
    //if ( pRenderable )
    //{
    //    pNode->AttachRenderable(pRenderable);
    //}

    return pRenderable;
}

void COTSceneManager::EnableShadowMap(bool bEnable)
{
    ISceneManager::EnableShadowMap(bEnable);
    if (m_pHeightMap)
    {
        m_pHeightMap->AcceptShadowmap(bEnable);
    }
}



bool COTSceneManager::FindPickNAVTriangle(const Line3Df& ray, Vector3Df& vOut, NavTriangle& navTriOut)
{
    AIRList<COctree*> lstOctrees;
    m_pOctree->GetRayPickOctrees(ray, lstOctrees);

    AIRList<NavIntersectInfo> lstPick;
    for (AIRList<COctree*>::iterator it = lstOctrees.begin(); it != lstOctrees.end(); ++it)
    {
        (*it)->GetRayPickNavTriangles(ray, lstPick);
    }

    if (lstPick.size() == 0)
    {
        return false;
    }

    lstPick.sort( ComparePickNavTriangle );

    AIRList<NavIntersectInfo>::iterator it = lstPick.begin();
    navTriOut = *((*it).m_pIntersectTriangle);
	vOut = (*it).m_vIntersectPoint;

    return true;
}

void COTSceneManager::CacheScene()
{
    if (m_pResource)
    {
        if (!m_pResource->IsLoaded())
        {
            return;
        }
        BEGIN_FUNC_PROFILE;

        CDataStream* pStream = m_pResource->GetFileStream(); //FILESYSTEM->GetFileStream(szFilename);

        if (pStream == NULL)
        {
            return;
        }
        AIRString strDoc = pStream->GetAsString();

        CMarkupSTL xml(strDoc.c_str());

        //��һ������������
        if (!xml.FindElem())
        {
            return;
        }

        m_strName = xml.GetAttrib("name").c_str();
        AIRString strNode = xml.GetTagName().c_str();
        AIRString strAttribute;

        xml.IntoElem();

        IRenderer* pRenderer = ENGINE_INST->GetRenderer();

        static float s_CreateSceneNodeTime = 0.0f;

        //���￪ʼ���½���
        AIRString strValue;
        AIRString tagName;
        while (xml.FindElem())
        {
			tagName = xml.GetTagName().c_str();
            // ���ز��������������
            if ( tagName == "Camera" )
            {
                BEGIN_FUNC_PROFILE;

                if (m_pCamera)
                {
                    SafeDelete(m_pCamera);
                }
                AIRString strType = xml.GetAttrib("Type").c_str();

                Vector3Df vCameraPosition;
                Vector3Df vLookat;
                strValue = xml.GetAttrib("Position").c_str();
                sscanf(strValue.c_str(), "(%f, %f, %f)", &vCameraPosition.x, &vCameraPosition.y, &vCameraPosition.z);
                strValue = xml.GetAttrib("Lookat").c_str();
                sscanf(strValue.c_str(), "(%f, %f, %f)", &vLookat.x, &vLookat.y, &vLookat.z);
                strValue = xml.GetAttrib("Pespective").c_str();
                bool bPerspective = true;
                if (strValue == "FALSE")
                {
                    bPerspective = false;
                }

                strValue = xml.GetAttrib("Width").c_str();
                int nViewPortWidth;
                int nViewPortHeight;
                pRenderer->GetBackBufferSize(nViewPortWidth, nViewPortHeight);

                if (strValue != "Default" && strValue.length() > 0)
                {
                    nViewPortWidth = atoi(strValue.c_str());
                }

                strValue = xml.GetAttrib("Height").c_str();
                if (strValue != "Default" && strValue.length() > 0)
                {
                    nViewPortHeight = atoi(strValue.c_str());
                }

                strValue = xml.GetAttrib("Near").c_str();
                float fNear = atof(strValue.c_str());

                strValue = xml.GetAttrib("Far").c_str();
                float fFar = atof(strValue.c_str());

                strValue = xml.GetAttrib("ClipFar").c_str();
                float fClipFar = fFar;
                if ( strValue.length() )
                {
                    fClipFar = atof( strValue.c_str() );
                }

                if (strValue == "ThirdPersonCamera")
                {
                    m_pCamera = new CThirdPersonCamera(vCameraPosition, vLookat);
                }
                else 
                {
                    m_pCamera = new CCamera(vCameraPosition, vLookat);
                }

                m_pCamera->SetViewport(fNear, fFar, AIR::PI / 4, nViewPortWidth, nViewPortHeight, bPerspective);

                m_pCamera->SetUserClipViewFar( fClipFar );

                END_FUNC_PROFILE_WITH_REPORT( "�������������." );
            }

            // ���ز�����������Ϣ
            else if ( tagName == "Terrain" )   
            {
                BEGIN_FUNC_PROFILE;

                strValue = xml.GetAttrib("File").c_str();
                if (m_pHeightMap != NULL)
                {
                    m_pHeightMap->Clear();
                    SafeDelete(m_pHeightMap);
                }

                //TERRAINCONFIG terrainConfig;
                //LoadTerrainConfig(terrainConfig, strValue.c_str());
                //TERRAIN->LoadConfigFile(strValue.c_str());
                AIRString strHeightMap = xml.GetAttrib("HeightMap").c_str();
                m_pHeightMap = new CHeightMap();   //��ʱ��Ĭ�ϵĵ������֣��Ժ�CHeightmap���ټ̳�Renderable��

                strValue = xml.GetAttrib("YScale").c_str();
                float fYScale = atof(strValue.c_str());

                strValue = xml.GetAttrib("InitHeight").c_str();
                float fInitHeight = 0;
                if (strValue.length() > 0)
                {
                    fInitHeight = atof(strValue.c_str());
                }

                AIRString strMaterial = xml.GetAttrib("Material").c_str();
                if (!m_pHeightMap->CreateHeightMap(strHeightMap.c_str(), TERRAIN->GetConfig(), fInitHeight, fYScale, strMaterial))
                {
                    m_pHeightMap->Clear();
                    SafeDelete(m_pHeightMap);
                }

                TERRAIN->SetCurHeightmap(m_pHeightMap);

                /*
                float fTerrainWidth = m_pHeightMap->GetWidth() / 2;
                if (m_pOctree == NULL)
                {
                    m_pOctree = new COctree();
                    AABBBox3Df box;
                    box.MaxEdge = Vector3Df(fTerrainWidth, fTerrainWidth, fTerrainWidth);
                    box.MinEdge = -box.MaxEdge;
                    m_pOctree->SetAABBBox(box);
                }
                */

                //-------------------------------------------------------------------
                //���win7���豸��ʧʱ������Ⱦ���������
                ENGINE_INST->NeedResetDevice();
                //-------------------------------------------------------------------

                END_FUNC_PROFILE_WITH_REPORT( "����������Ϣ." );
            }

            // ���ز���������ֲ����Ϣ
            else if ( tagName == "Plant" )
            {
                //   ��ʱ��Ļֲ���Ϊ���������б仯��
                if ( ENGINE_INST->GetIsLoadPlant() )
                {
					BEGIN_FUNC_PROFILE;

					bool bCreateFromBinFile = false;
					strValue = xml.GetAttrib( "Bin" ).c_str();
					if ( strValue == "TRUE" )
					{
						bCreateFromBinFile = true;
					}
					strValue = xml.GetAttrib( "File" ).c_str();

                    CreatePlants(strValue.c_str());
                    /*
					PSG_MANAGER->SetEditFileName( strValue );
					PSG_MANAGER->ClearAllPageStaticGeometry();
					if ( bCreateFromBinFile )
					{
						PSG_MANAGER->LoadFromBinFile( strValue.c_str() );
					}
					else
					{
						PSG_MANAGER->LoadFromFile( strValue.c_str() );
					}
                    */

					END_FUNC_PROFILE_WITH_REPORT( "��������ֲ����Ϣ." );
                }
                
            }

            // ���ص�����Ϣ
            else if ( tagName == "Navigator" )
            {
                BEGIN_FUNC_PROFILE;

                bool bCreateFromBinFile = false;
                strValue = xml.GetAttrib( "Bin" ).c_str();
                if ( strValue == "TRUE" )
                {
                    bCreateFromBinFile = true;
                }
                NAV_EDITOR->ResetNavEditor();
                strValue = xml.GetAttrib( "File" ).c_str();

                if ( bCreateFromBinFile )
                {
                    if ( !NAV_EDITOR->LoadFromBinFile( strValue.c_str() ) )
                    {
                        DT_TO_MSGBOX_FMT( LT_WARN, GetString("Load map navigator file [%s] failed!", strValue.c_str()) );
                        NAV_EDITOR->SetNavFile( "" );
                    }
                }
                else
                {
                    if ( !NAV_EDITOR->LoadFromXML( strValue.c_str() ) )
                    {
                        DT_TO_MSGBOX_FMT( LT_WARN, GetString("Load map navigator file [%s] failed!", strValue.c_str()) );
                        NAV_EDITOR->SetNavFile( "" );
                    }	
                }
                NAV_EDITOR->SetNavFile( strValue );

                END_FUNC_PROFILE_WITH_REPORT( "���ص�����Ϣ." );
            }

            // ���ز�����������Դ��Ϣ
            else if ( tagName == "Light" )  
            {
                BEGIN_FUNC_PROFILE;

                END_FUNC_PROFILE_WITH_REPORT( "����������Դ��Ϣ." );
            }

            // ���ز�����������
            else if ( tagName == "Ambient" )
            {
                BEGIN_FUNC_PROFILE;

                strValue = xml.GetAttrib("Color").c_str();
                sscanf(strValue.c_str(), "%f %f %f %f", &m_clrAmbient.r, &m_clrAmbient.g, &m_clrAmbient.b, &m_clrAmbient.a);

                END_FUNC_PROFILE_WITH_REPORT( "����������t." );
            }

            // ���ش�����
            else if ( tagName == "Fog" )
            {
                BEGIN_FUNC_PROFILE;

                strValue = xml.GetAttrib("fogMode").c_str();
                m_fogMode = (FogMode)atoi(strValue.c_str());

                strValue = xml.GetAttrib("start").c_str();
                m_fFogStart = atof(strValue.c_str());

                strValue = xml.GetAttrib("end").c_str();
                m_fFogEnd = atof(strValue.c_str());

                strValue = xml.GetAttrib("density").c_str();
                m_fDensity = atof(strValue.c_str());

                strValue = xml.GetAttrib("color").c_str();
                sscanf(strValue.c_str(), "%f %f %f %f", &m_fogColor.r, &m_fogColor.g, &m_fogColor.b, &m_fogColor.a);

                CurFogToDefaultFog();

                END_FUNC_PROFILE_WITH_REPORT( "������." );
            }

            // ���ز������չ�
            else if ( tagName == "Sunlight" )
            {
                BEGIN_FUNC_PROFILE;

                strValue = xml.GetAttrib("Direction").c_str();
                Vector3Df direction;
                sscanf(strValue.c_str(), "%f %f %f", &direction.x, &direction.y, &direction.z);
                m_sunLight.SetDirection(direction);

                strValue = xml.GetAttrib("Color").c_str();
                CColorValue color;
                sscanf(strValue.c_str(), "%.4f %.4f %.4f %.4f", &color.r, &color.g, &color.b, &color.a);
                m_sunLight.SetDiffuse(color);

                END_FUNC_PROFILE_WITH_REPORT( "�����չ�." );
            }

            // ���ز����������ڵ������Ϣ
            else if ( tagName == "SceneNode" )   
            {
                LoadSceneNodeFromXML(xml);
            }

            // ���ز�������պ�
            //else if ( tagName == "Skybox" )  
            //{
            //    BEGIN_FUNC_PROFILE;

            //    m_pSkybox = new CSkyBox();
            //    if (!m_pSkybox->LoadFromXML(xml))
            //    {
            //        return;
            //    }

            //    END_FUNC_PROFILE_WITH_REPORT( "������պ�." );
            //}

            // ������Χ��
            else if ( tagName == "boundingbox" )
            {
                BEGIN_FUNC_PROFILE;

                strValue = xml.GetAttrib("MaxEdge").c_str();
                Vector3Df maxEdge;
                sscanf(strValue.c_str(), "(%f %f %f)", &maxEdge.x, &maxEdge.y, &maxEdge.z);

                strValue = xml.GetAttrib("MinEdge").c_str();
                Vector3Df minEdge;
                sscanf(strValue.c_str(), "(%f %f %f)", &minEdge.x, &minEdge.y, &minEdge.z);

                AABBBox3Df boxWorld(minEdge, maxEdge);
                if (m_pOctree == NULL)
                {
                    m_pOctree = new COctree;
                }
                m_pOctree->SetAABBBox(boxWorld);

                END_FUNC_PROFILE_WITH_REPORT( "����������Χ��." );
            }
            else if ( tagName == "Shadowmap" )
            {
                strValue = xml.GetAttrib("SoftNess").c_str();
                if (!strValue.empty())
                {
                    SHADOWMAP->SetSoftNess(atof(strValue.c_str()));
                }

                strValue = xml.GetAttrib("DepthBias").c_str();
                if (!strValue.empty())
                {
                    SHADOWMAP->SetShadowDepthBias(atof(strValue.c_str()));
                }

                strValue = xml.GetAttrib("ScoleScaleBias").c_str();
                if (!strValue.empty())
                {
                    SHADOWMAP->SetShadowScoleScaleBias(atof(strValue.c_str()));
                }

                strValue = xml.GetAttrib("Type").c_str();
                if (!strValue.empty())
                {
                    SHADOWMAP->SetShadowmapType((ShadowmapType)atoi(strValue.c_str()));
                }
            }
			else if( tagName == "WeatherManager" )
			{
				WeatherManager* pWM = WeatherManager::GetInstance();
				pWM->Create( this, m_pCamera );
				pWM->LoadFromXML( xml );		
			}

        }

        xml.OutOfElem();

        

        REPORT_FUNC_PROFILE( "����ֲ������.", s_CreatePlantTime );
        REPORT_FUNC_PROFILE( "����ˮ�����.", s_CreateWaterTime );
        REPORT_FUNC_PROFILE( "�������Ӷ���.", s_CreatePartiTime );
        REPORT_FUNC_PROFILE( "����ģ�Ͷ���.", s_CreateModelTime );
        REPORT_FUNC_PROFILE( "������Ч����.", s_CreateEffecTime );

        END_FUNC_PROFILE_WITH_REPORT( GetString("������������.") );

        //ɾ��filedata
        RESOURCE_MANAGER->ClearResource(m_pResource);
        m_pResource = NULL;

        //�����������ļ�����
        //FILEDATA_MANAGER->Clear();
    }
    
    if (m_pHeightMap)
    {
        if (!m_pHeightMap->IsPrepared())
        {
            m_bReady = m_pHeightMap->Prepare();
        }

        if (m_pHeightMap->IsPrepared())
        {
            //���ó����˲���
            float fTerrainWidth = m_pHeightMap->GetWidth() / 2;
            if (m_pOctree == NULL)
            {
                m_pOctree = new COctree();
                
            }
            AABBBox3Df box;
            box.MaxEdge = Vector3Df(fTerrainWidth, fTerrainWidth, fTerrainWidth);
            box.MinEdge = -box.MaxEdge;
            m_pOctree->SetAABBBox(box);

            //�������sunlight��λ��
            Vector3Df lightDir = m_sunLight.GetDirection();
            lightDir = -lightDir;
            m_sunLight.SetPosition(lightDir * m_pOctree->GetBoundingbox().GetHalfSize().GetLength() * 5.0f);

            AIRVector< NavTriangle >& vtNavTriangles = NAV_EDITOR->GetNavMesh();
            size_t navTriangles = vtNavTriangles.size();
            for (size_t i = 0; i < navTriangles; ++i)
            {
                m_pOctree->AddNavTriangle(0, &vtNavTriangles[i]);
            }
        }
    }
    
    bool bRenderableReady = true;
    for (PrepareRenderableIter it = m_lstPaparedRenderables.begin(); it != m_lstPaparedRenderables.end();)
    {
        Renderable* pRenderable = *it;
        if (!pRenderable->Prepare())
        {
            pRenderable->Prepare();
            ++it;
        }
        else
        {
            it = m_lstPaparedRenderables.erase(it);
        }
    }
    bRenderableReady = (m_lstPaparedRenderables.size() == 0);

    if (bRenderableReady)
    {
        MAP_SCENENODE::iterator itEnd = m_mapSceneNodes.end();
        for (MAP_SCENENODE::iterator it = m_mapSceneNodes.begin(); it != itEnd; ++it)
        {
            CSceneNode* pNode = it->second;
            pNode->NeedUpdate();
            pNode->Update();
        }
    }
    //DT_TO_DBGSTR(LT_NONE, GetString("loading resources num:%d\n", RESOURCE_MANAGER->GetLoadingResourcesNum()));

    m_bReady = bRenderableReady;
}
/*
void COTSceneManager::ProcessVisableObjects()
{
    ISceneManager::ProcessVisableObjects();

    // �����εĿɼ��ؿ�ӵ���Ⱦ����
    if (m_pHeightMap && m_pHeightMap->IsVisable())
    {
        m_pHeightMap->GetRenderObject(m_pCamera, &m_renderQueue);
    }

    //////////////////////////////////////////////////////////////////////////
    // �������պ�,�ͽ���պм�����Ⱦ����
    //if ( m_pSkybox )
    //{
    //    m_renderQueue.AddRenderable( m_pSkybox );
    //}
    //////////////////////////////////////////////////////////////////////////

	WeatherManager* pWM = WeatherManager::GetInstance();
	if ( pWM->IsCreated() && pWM->GetLensFlare() )
	{
		m_renderQueue.AddRenderable( (Renderable*)pWM->GetLensFlare() );
	}
}

int COTSceneManager::GetRenderableObjectsNum() const
{
    int nNum = m_lstVisableNodes.size();//m_renderQueue.GetRenderablesNum();
    if (m_pHeightMap)
    {
        nNum += m_pHeightMap->GetRenderableObjectsCount() - 1;
    }

    return nNum;
}
*/
void COTSceneManager::Cull(CCamera* pCamera, AIRList<CSceneNode*>& lstVisableNodes, u32 nCullFlag)
{
    if (m_pOctree)
    {
        m_pOctree->GetVisableObjects(pCamera, lstVisableNodes, nCullFlag);
    }

    if (m_pHeightMap)
    {
        m_pHeightMap->Cull(pCamera, lstVisableNodes, nCullFlag);
    }
}
