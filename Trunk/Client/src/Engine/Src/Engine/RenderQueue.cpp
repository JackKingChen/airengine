#include "stdafx.h"
#include "RenderQueue.h"
#include "Engine.h"
#include "MaterialManager.h"
#include "ISceneManager.h"
#include "BillboardRenderer.h"
//#include "Technique.h"

int CompareRenderableDescending(Renderable* pObj1, Renderable* pObj2)
{
    CCamera* pCamera = ENGINE_INST->GetSceneMgr()->GetCamera();

    f32 fDis1 = pCamera->GetDistanceFrom(pObj1->GetCenter());
    f32 fDis2 = pCamera->GetDistanceFrom(pObj2->GetCenter());
    return fDis1 > fDis2;
}

CRenderQueue::CRenderQueue() : m_pTerrain(NULL), m_bIs2DQueue(false), m_pLineObjectMtl(NULL)
{

}

CRenderQueue::~CRenderQueue()
{
    for (size_t i = 0; i < m_vtRenderQueueGroup.size(); ++i)
    {
        if (m_vtRenderQueueGroup[i])
        {
            SafeDelete(m_vtRenderQueueGroup[i]);
        }
    }

    m_vtRenderQueueGroup.clear();
}

void CRenderQueue::AddRenderable(Renderable* pRenderable)
{
    /*
    if (!pRenderable->IsVisable())
    {
        return;
    }
    if (pRenderable->GetTypeString() == "terrain")
    {
        m_pTerrain = pRenderable;
    }
    else if (pRenderable->IsTransparent())
    {
        m_vtAlphaObjects.push_back(pRenderable);
    }
    else
    {
        m_lstSolidObjects.AddRenderable(pRenderable);
    }
    */

    //以下是新方法，要与旧的对比过性能不用，先注释掉
    if (pRenderable->IsVisable())
    {
        if (pRenderable->GetRenderGroupType() == eRGT_Line)
        {
            if (pRenderable->IsVisable())
            {
                m_lstLineObjects.push_back(pRenderable);
            }

            return;
        }

        RenderQueueGroup* pGroup = NULL;
        if (pRenderable->IsTransparent() && !pRenderable->GetForceRenderGroup())
        {
            pGroup = GetRenderQueueGroup(eRGT_Transparent);
        }
        else
        {
            pGroup = GetRenderQueueGroup((RenderGroup_Type)pRenderable->GetRenderGroupType());
        }

        pGroup->AddRenderable(pRenderable);
    }
    

    /*
    if (stricmp(pRenderable->GetTypeString().c_str(), "terrain") == 0)
    {
        pGroup = GetRenderQueueGroup(eRGT_Terrain);
    }
    else if (stricmp(pRenderable->GetTypeString().c_str(), "skybox") == 0)
    {
        pGroup = GetRenderQueueGroup(eRGT_Skybox);
    }
    else if (pRenderable->IsTransparent())
    {
        pGroup = GetRenderQueueGroup(eRGT_Transparent);
    }
    else 
    {
        pGroup = GetRenderQueueGroup(eRGT_Solid);
    }
    
    if (pRenderable->IsVisable())
    {
        pGroup->AddRenderable(pRenderable);
    }
    */
 }

void CRenderQueue::Render(CCamera* pCamera)
{
    //画出所有线框物体
    if (m_lstLineObjects.size() > 0)
    {
        if (m_pLineObjectMtl == NULL)
        {
            m_pLineObjectMtl = MATERIAL_MANAGER->CreateMaterial("TemplateMaterial.xml/LineObject");
            if (!m_pLineObjectMtl->IsReady())
            {
                m_pLineObjectMtl->Cache();
            }
        }
        if (m_pLineObjectMtl->IsReady())
        {
            //CPass* pPass = m_pLineObjectMtl->GetCurrentTechnique()->GetPass(0);
            //pPass->ApplyPass(ENGINE_INST->GetRenderer());
            LineObjects_Iter itEnd = m_lstLineObjects.end();
            for (LineObjects_Iter it = m_lstLineObjects.begin(); it != itEnd; ++it)
            {
                (*it)->Render(pCamera);
            }
        }
		else
		{
			m_pLineObjectMtl->Cache();
		}
        
    }
    
    //画渲染组的物体
    for (size_t i = 0; i < m_vtRenderQueueGroup.size(); ++i)
    {
        if (m_vtRenderQueueGroup[i])
        {
            m_vtRenderQueueGroup[i]->Render(pCamera);
        }        
    }
	//画billboard对象
    BILLBOARD_RENDERER->RenderImmediately();
}

void CRenderQueue::Clear()
{
    //m_lstSolidObjects.Clear();
    //m_vtAlphaObjects.clear();
    m_lstLineObjects.clear();

    for (size_t i = 0; i < m_vtRenderQueueGroup.size(); ++i)
    {
        if (m_vtRenderQueueGroup[i])
        {
            m_vtRenderQueueGroup[i]->Clear();
        }
        
        //SafeDelete(m_vtRenderQueueGroup[i]);
    }
    //m_vtRenderQueueGroup.clear();
}

void CRenderQueue::Sort()
{
    //m_lstSolidObjects.Sort();
    //std::sort(m_vtAlphaObjects.begin(), m_vtAlphaObjects.end(), CompareRenderableDescending);

    //新方法
    
    for (size_t i = 0; i < m_vtRenderQueueGroup.size(); ++i)
    {
        if (m_vtRenderQueueGroup[i])
        {
            m_vtRenderQueueGroup[i]->Sort( m_bIs2DQueue );
        }
    }
    
}

void CRenderQueue::RenderSolidObjects(CCamera* pCamera)
{
    RenderQueueGroup* pQueueGroup = GetRenderQueueGroup(eRGT_Solid);
	pQueueGroup->Sort( m_bIs2DQueue );
    pQueueGroup->Render(pCamera);
    /*
    CShaderGroup* pGroup = m_lstSolidObjects.GetShaderFirst();
    while (pGroup)
    {
        //IShader* pShader = pGroup->GetShader();
        //if (pShader)
        //    pShader->Begin();
        //pRenderer->ApplyRenderState(*pShader);
        RenderableList* pRenderList = pGroup->GetRenderableList();

        for (RENDERABLE_ITER it = pRenderList->begin(); it != pRenderList->end(); it++)
        {
            Renderable* pRenderable = *it;
            pRenderable->Render(pCamera);
        }

        pGroup = m_lstSolidObjects.GetNext();
    }
    */
}

void CRenderQueue::RenderAlphaObjects(CCamera* pCamera)
{
    /*
    for (u32 i = 0; i < m_vtAlphaObjects.size(); i++)
    {
        Renderable* pRenderable = m_vtAlphaObjects[i];
        IMaterial* pMaterial = pRenderable->GetMaterial();
        if (pMaterial == NULL)
        {
            continue;
        }

        pRenderable->Render(pCamera);
    }
    */
    RenderQueueGroup* pQueueGroup = GetRenderQueueGroup(eRGT_Transparent);
	pQueueGroup->Sort( m_bIs2DQueue );
    pQueueGroup->Render(pCamera);
}

int CRenderQueue::GetRenderablesNum() const
{
    int nNums = 0;
    size_t nGroupsNum = m_vtRenderQueueGroup.size();
    for (size_t i = 0; i < nGroupsNum; ++i)
    {
        if (m_vtRenderQueueGroup[i] != NULL)
        {
            nNums += m_vtRenderQueueGroup[i]->GetRenderablesNum();
        }
    }
    
    return nNums;//m_lstSolidObjects.GetRenderablesNum() + m_vtAlphaObjects.size();
}

void CRenderQueue::SetRenderGroup(RenderGroup_Type type, RenderQueueGroup* pGroup)
{
    if (m_vtRenderQueueGroup.size() < (size_t)type)
    {
        m_vtRenderQueueGroup.resize((size_t)type + 1);
    }
    if (m_vtRenderQueueGroup[type] != NULL)
    {
        SafeDelete(m_vtRenderQueueGroup[type]);
        
    }
    m_vtRenderQueueGroup[type] = pGroup;
}

RenderQueueGroup* CRenderQueue::GetRenderQueueGroup(RenderGroup_Type type)
{
    if (m_vtRenderQueueGroup.size() < (size_t)type + 1)
    {
        m_vtRenderQueueGroup.resize((size_t)type + 1);
        m_vtRenderQueueGroup[(size_t)type] = CreateRenderQueueGroup(type);
    }
    else if (m_vtRenderQueueGroup[(size_t)type] == NULL)
    {
        m_vtRenderQueueGroup[(size_t)type] = CreateRenderQueueGroup(type);
    }

    return m_vtRenderQueueGroup[(size_t)type];
}

RenderQueueGroup* CRenderQueue::CreateRenderQueueGroup(RenderGroup_Type type)
{
	RenderQueueGroup* pRG = NULL;
    switch (type)
    {
    case eRGT_Skybox:
        pRG = new RenderQueueGroup(eRQST_SKYOBJ/*eRQST_NOTSORT*/);
        break;
    case eRGT_Solid:
        pRG = new RenderQueueGroup(eRQST_NEARTOFAR);
        break;
    case eRGT_Terrain:
        pRG = new RenderQueueGroup(eRQST_NEARTOFAR);
        break;
    case eRGT_Transparent:
        pRG = new RenderQueueGroup(eRQST_FARTONEAR);
        break;
    }
	if ( pRG )
	{
		pRG->SetGroupType( type );
	}
    return pRG;
}