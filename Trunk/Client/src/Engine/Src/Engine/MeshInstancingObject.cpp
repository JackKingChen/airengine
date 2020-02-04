#include "stdafx.h"
#include "MeshInstancingObject.h"
#include "MeshInstance.h"
#include "Engine.h"
#include "ModelInstance.h"
#include "MaterialManager.h"
#include "TimeBaseProfile.h"
#include "Shadowmap.h"
//#include "TechniqueSelector.h"
#include "Primitive.h"
//#include "TextureUnit.h"
//#include "Pass.h"
//#include "Technique.h"
#include "GPUProgramManager.h"

MeshInstancingObject::MeshInstancingObject(InstancingObject* pOwner, int nLod, CAIRSubMesh* pMeshRes) : m_pMeshRes(pMeshRes)
//, m_nMaxInstanceNum(0)
, m_pInstanceDataVB(NULL)
, m_pInstanceDataIB(NULL)
, m_bHardwareInstance(true)
//, m_bIsInRenderQueue(false)
//, m_pHardwareVertexDecl(NULL)
//, m_pTechniqueRenderToShadow(NULL)
//, m_pTechniqueAcceptShadow(NULL)
//, m_pCurrentTechnique(NULL)
//, m_pTechniqueRenderToGBuffer(NULL)
, m_pMeshTexture(NULL)
, m_pOwner(pOwner)
, m_nLod(nLod)
, m_bIsInRenderQueue(false)
{

}

MeshInstancingObject::MeshInstancingObject(InstancingObject* pOwner, int nLod) : m_pMeshRes(NULL)
, m_pInstanceDataVB(NULL)
, m_pInstanceDataIB(NULL)
, m_bHardwareInstance(true)
//, m_bIsInRenderQueue(false)
//, m_pHardwareVertexDecl(NULL)
//, m_pTechniqueRenderToShadow(NULL)
//, m_pTechniqueAcceptShadow(NULL)
//, m_pCurrentTechnique(NULL)
//, m_pTechniqueRenderToGBuffer(NULL)
, m_pMeshTexture(NULL)
, m_pOwner(pOwner)
, m_nLod(nLod)
, m_bIsInRenderQueue(false)
{
	m_strVertexDecl = "";
}

MeshInstancingObject::~MeshInstancingObject()
{

}

bool MeshInstancingObject::Create(int nMaxInstanceNum)
{
    IRenderer* pRenderer = ENGINE_INST->GetRenderer();
    if (m_pInstanceDataVB == NULL)
    {
        if (!m_bHardwareInstance)
        {
            int nRealInstance = m_pOwner->GetMaxInstancesNum() > MaxInstancesNum ? MaxInstancesNum : m_pOwner->GetMaxInstancesNum();
            //m_pInstanceDataVB = pRenderer->CreateVertexBuffer(E_VB_NORMAL_INSTANCE_DATA, nRealInstance * m_pMeshRes->GetVertexBuffer()->GetVertexCount());
            m_pInstanceDataVB = pRenderer->CreateVertexBuffer(nRealInstance * m_pMeshRes->GetVertexBuffer()->GetVertexCount(), sizeof(VERTEXNORMAL_INSTANCEDATA));

            m_pInstanceDataIB = pRenderer->CreateIndexBuffer(nRealInstance * m_pMeshRes->GetIndexBuffer()->GetTotalIndex());


            //填充顶点数据
            LPVERTEXNORMAL_INSTANCEDATA pVertices = (LPVERTEXNORMAL_INSTANCEDATA)m_pInstanceDataVB->Lock();

            //SOFTWARE_BLEND_VERTEX_LIST& vtMeshVertices = m_pMeshRes->GetVerticesData();
			SOFTWARE_BLEND_VERTEX_LIST& vtMeshVertices = (m_pMeshRes->GetCurLodData()->m_pvtSoftwareBlendVertex);

            int nMeshVertexNum = vtMeshVertices.size();
            LPMESHVERTNORMAL pMeshVertex = NULL;
            for (int i = 0; i < nRealInstance; ++i)
            {
                for (int j = 0; j < nMeshVertexNum; ++j)
                {
                    pMeshVertex = vtMeshVertices[j];
                    pVertices->vPos = pMeshVertex->vertex.vPos;
                    pVertices->normal = pMeshVertex->vertex.normal;
                    pVertices->color = pMeshVertex->vertex.color;
                    pVertices->tu = pMeshVertex->vertex.tu;
                    pVertices->tv = pMeshVertex->vertex.tv;
                    pVertices->fInstance = (float)i;

                    pVertices++;
                }
            }
            m_pInstanceDataVB->Unlock();

            //填充索引数据
            u16* pIndices = (u16*)m_pInstanceDataIB->Lock(0, 0);

            //const AIRVector<AIRMESHTRI>& meshFaces = m_pMeshRes->GetMeshFaces();
			const AIRVector<AIRMESHTRI>& meshFaces = (m_pMeshRes->GetLodData(m_nLod)->m_pvtFaces);
            for (int i = 0; i < nRealInstance; ++i)
            {
                for (size_t nFace = 0; nFace < meshFaces.size(); ++nFace)
                {
                    const AIRMESHTRI& tri = meshFaces[nFace];
                    *pIndices++ = tri.nIndex[0] + nMeshVertexNum * i;
                    *pIndices++ = tri.nIndex[1] + nMeshVertexNum * i;
                    *pIndices++ = tri.nIndex[2] + nMeshVertexNum * i;
                }
                
            }

            m_pInstanceDataIB->Unlock();

			m_strVertexDecl = "E_VB_NORMAL_INSTANCE_DATA";
        }
        else
        {
            /*
            m_pHardwareVertexDecl = pRenderer->CreateVertexDeclaration();
            m_pHardwareVertexDecl->SetAutoDelete(false);
            size_t offset = 0;
            m_pHardwareVertexDecl->AddVertexElement( 0, offset, eVET_FLOAT3, eVEU_POSITION, 0 );
            offset += sizeof(float) * 3;
            m_pHardwareVertexDecl->AddVertexElement( 0, offset, eVET_FLOAT3, eVEU_NORMAL, 0 );
            offset += sizeof(float) * 3;
            m_pHardwareVertexDecl->AddVertexElement( 0, offset, eVET_COLOUR, eVEU_DIFFUSE,  0 );
            offset += sizeof(u32);
            m_pHardwareVertexDecl->AddVertexElement( 0, offset, eVET_FLOAT2, eVEU_TEXTURE_COORDINATES, 0 );
            offset += sizeof(float) * 2;
            offset = 0;
            m_pHardwareVertexDecl->AddVertexElement( 1, offset, eVET_FLOAT4, eVEU_TEXTURE_COORDINATES, 1);
            offset += sizeof(float) * 4;
            m_pHardwareVertexDecl->AddVertexElement( 1, offset, eVET_FLOAT4, eVEU_TEXTURE_COORDINATES, 2);
            offset += sizeof(float) * 4;
            m_pHardwareVertexDecl->AddVertexElement( 1, offset, eVET_FLOAT4, eVEU_TEXTURE_COORDINATES, 3);
            offset += sizeof(float) * 4;
            m_pHardwareVertexDecl->AddVertexElement( 1, offset, eVET_FLOAT4, eVEU_TEXTURE_COORDINATES, 4);
            offset += sizeof(float) * 4;
            m_pHardwareVertexDecl->AddVertexElement( 1, offset, eVET_COLOUR, eVEU_DIFFUSE, 1);
            offset += sizeof(float) * 4;

            m_pHardwareVertexDecl->Build();
            */

            m_pInstanceDataVB = pRenderer->CreateVertexBuffer(m_pOwner->GetMaxInstancesNum(), sizeof(VERTEXHARDWARE_INSTANCEDATA));
			m_strVertexDecl = "VSInstancing";
        }
        
    }
    return m_pInstanceDataVB != NULL;
}

void MeshInstancingObject::Update(float fTimeElapse)
{
    m_lstMeshInstances.clear();
    m_bIsInRenderQueue = false;
    //m_lstRenderQueues.clear();
    //m_bIsInRenderQueue = false;
}

void MeshInstancingObject::Render(CCamera* pCamera)
{
    if (m_lstMeshInstances.size() < 3)
    {
        ListMeshInstanceIter itEnd = m_lstMeshInstances.end();
        for (ListMeshInstanceIter it = m_lstMeshInstances.begin(); it != itEnd; ++it)
        {
            (*it)->Render(pCamera);
        }
    }
    else
    {
        if (Prepare())
        {
            PreRender(pCamera);

            _GetPrimitive();

            IRenderer* pRenderer = RENDERER;
            pRenderer->SetWorldTransform(&Matrix4f::IDENTITY);


            CGPUDataParamSource* pGPUParamSource = pRenderer->GetGPUDataParam();
            if (m_pMaterial)
            {
                /*
                CTechnique* pTechnique = m_pMaterial->GetCurrentTechnique();
                if (pTechnique)
                {
                    if (!m_bHardwareInstance)
                    {

                        int nRemainingInstances = m_lstMeshInstances.size();
                        while( nRemainingInstances > 0 )
                        {
                            // determine how many instances are in this batch (up to g_nNumBatchInstance)   

                            int nRenderInstance = min( nRemainingInstances, MaxInstancesNum );

                            pGPUParamSource->SetWorldMatrixNum(nRenderInstance);
                            //pGPUParamSource->SetPositionsNum(nRenderInstance);
                            ListMeshInstanceIter it = m_lstMeshInstances.begin();
                            std::advance(it, m_lstMeshInstances.size() - nRemainingInstances);
                            for (int i = 0; i < nRenderInstance; ++i, ++it)
                            {
                                pGPUParamSource->SetWorldMatrixArray((*it)->GetNodeParent()->GetFullTransform(), i);
                                //pGPUParamSource->SetPositionArray((*it)->GetNodeParent()->GetDerivedPosition(), i);
                            }

                           
                            m_pPrimitive->Draw(pTechnique);

                            // subtract the rendered boxes from the remaining boxes
                            nRemainingInstances -= nRenderInstance;
                        }
                    }
                    else
                    {
                        m_pPrimitive->Draw(pTechnique);
                        
                    }
                }*/
            }
        }
        
    }
    

    //m_lstMeshInstances.clear();
    //m_lstRenderQueues.clear();
    //m_bIsInRenderQueue = false;
}

void MeshInstancingObject::PreRender(CCamera* pCamera)
{
    //BEGIN_FUNC_PROFILE;
    if (m_pInstanceDataVB == NULL)
    {
        Create(m_pOwner->GetMaxInstancesNum());
    }
    if (m_bHardwareInstance)
    {
        LPVERTEXHARDWARE_INSTANCEDATA pVertices = (LPVERTEXHARDWARE_INSTANCEDATA)m_pInstanceDataVB->Lock();
        ListMeshInstanceIter itEnd = m_lstMeshInstances.end();

        //设置shader参数
        CMeshInstance* pMeshInstance = NULL;
        for (ListMeshInstanceIter it = m_lstMeshInstances.begin(); it != itEnd; ++it)
        {
            pMeshInstance = *it;
            memcpy(&pVertices->matTransform, &pMeshInstance->GetOwnerModelInstance()->GetNodeParent()->GetFullTransform(), sizeof(Matrix4f));
            pVertices->color = pMeshInstance->GetOwnerModelInstance()->GetColorDiffuse().color;
            pVertices++;
        }
        
        m_pInstanceDataVB->Unlock();
    }
    

    //END_FUNC_PROFILE_WITH_REPORT("MeshInstance prerender time");
}

void MeshInstancingObject::AddModelInstance(CMeshInstance* pInstance)
{
    //ListMeshInstanceIter it = std::find(m_lstMeshInstances.begin(), m_lstMeshInstances.end(), pInstance);
    //if (it == m_lstMeshInstances.end())
    {
        if (m_pOwner->GetMaxInstancesNum() > m_lstMeshInstances.size())
        {
            m_lstMeshInstances.insert(pInstance);
        }
        else
        {
            //assert(false);
        }
        /*
        if (m_pMaterial == NULL)
        {
            IRenderer* pRenderer = ENGINE_INST->GetRenderer();
            m_pMaterial = MATERIAL_MANAGER->CreateMaterial();
            m_pMaterial->DeepCopy(pInstance->GetMaterial());

            
            for (size_t i = 0; i < m_pMaterial->GetTechniquesNum(); ++i)
            {
                CTechnique* pTechnique = m_pMaterial->GetTechnique(i);
                if (pTechnique)
                {
                    //pTechnique->SetVSVersion("vs_2_0");
                    //pTechnique->SetPSVersion("ps_2_0");
                    for (size_t nPass = 0; nPass < pTechnique->GetPassesNum(); ++nPass)
                    {
                        CPass* pPass = pTechnique->GetPass(nPass);
                        //IShader* pShader = pPass->GetShader();

                        //if (pShader)
                        {
                        //    pShader->Clear();
                            //pPass->SetVertexShader(NULL);
                            //pPass->SetPixelShader(NULL);
							pPass->SetShaderObject(NULL);
                            if (m_bHardwareInstance)
                            {
                                IGPUProgram* pVSShadowmap = GPUPROGRAM_MANAGER->GetGPUProgram("VSInstancing"); //pRenderer->CreateVertexShader("VSHardwareInstancing.txt", pTechnique->GetVSVersion().c_str());
                                //pPass->SetVertexShader(pVSShadowmap);
								IGPUProgram* pGpuPrograms[4] = { 0 };
								pGpuPrograms[0] = pVSShadowmap;
								ShaderObject* pShaderObject = pRenderer->CreateShaderObject(pGpuPrograms, 1);
								pPass->SetShaderObject(pShaderObject);
                            }
                            else
                            {
                                IGPUProgram* pVSShadowmap = GPUPROGRAM_MANAGER->GetGPUProgram("VSShaderInstancing.txt"); //pRenderer->CreateVertexShader("VSShaderInstancing.txt", pTechnique->GetVSVersion().c_str());
                                //pPass->SetVertexShader(pVSShadowmap);
								IGPUProgram* pGpuPrograms[4] = { 0 };
								pGpuPrograms[0] = pVSShadowmap;
								ShaderObject* pShaderObject = pRenderer->CreateShaderObject(pGpuPrograms, 1);
								pPass->SetShaderObject(pShaderObject);
                            }

                        }
                    }
                }

            }

            m_pMaterial->SetIsReady(false);

            if (pInstance->IsAcceptLighting() != IsAcceptLighting())
            {
                SetAcceptLighting(pInstance->IsAcceptLighting());
            }

            if (pInstance->IsAcceptShadowMap() != IsAcceptShadowMap())
            {
                AcceptShadowmap(pInstance->IsAcceptShadowMap());
            }

            if (pInstance->IsRenderToShadow() != IsRenderToShadow())
            {
                SetRenderToShadow(pInstance->IsRenderToShadow());
            }
        }

        if (m_pMeshTexture == NULL)
        {
            IMaterial* pMaterial = pInstance->GetMaterial();
            CTextureUnit* pUnit = pMaterial->GetTechnique(0)->GetPass(0)->GetTextureUnit(0);
            m_pMeshTexture = pUnit->GetTexture();
        }
        */
    }
    
    
}
/*
void MeshInstancingObject::DrawOneByOne()
{
    //ListMeshInstanceIter itEnd = m_lstMeshInstances.end();
    //for (ListMeshInstanceIter it = m_lstMeshInstances.begin(); it != itEnd; ++it)
    //{
    //    (*it)->Render(pCamera);
    //}
}
*/
void MeshInstancingObject::Clear()
{
    if (m_pMaterial)
    {
        MATERIAL_MANAGER->ClearMaterial(m_pMaterial);
    }

    /*
    if (m_pHardwareVertexDecl)
    {
        m_pHardwareVertexDecl->Clear();
        SafeDelete(m_pHardwareVertexDecl);
    }
    */

    if (m_pInstanceDataVB)
    {
        m_pInstanceDataVB->Release();
        SafeDelete(m_pInstanceDataVB);
    }

    if (m_pInstanceDataIB)
    {
        m_pInstanceDataIB->Release();
        SafeDelete(m_pInstanceDataIB);
    }

    m_pMaterial = NULL;
}

void MeshInstancingObject::GenRenderToShadowTechnique()
{
    /*
    m_pTechniqueRenderToShadow = m_pMaterial->GetTechnique("Instancing_Shadowmap");
    if (m_pTechniqueRenderToShadow == NULL)
    {
        CTechnique* pCurrentTechnique = m_pMaterial->GetCurrentTechnique();
        m_pTechniqueRenderToShadow = new CTechnique(m_pMaterial);
        m_pTechniqueRenderToShadow->SetName("Instancing_Shadowmap");
        //m_pTechniqueRenderToShadow->SetVSVersion("vs_2_0");
        //m_pTechniqueRenderToShadow->SetPSVersion("ps_2_0");
        CPass* pPass = new CPass(m_pTechniqueRenderToShadow);
        *pPass = *(pCurrentTechnique->GetPass(0));
        //把第1阶段以后的纹理都清除
        for (size_t i = 1; i < 8; ++i)
        {
            pPass->RemoveTextureUnit(i);

        }
        //IShader* pShader = pPass->GetShader();

        IRenderer* pRenderer = ENGINE_INST->GetRenderer();
        //if (pShader)
        {
        //    pShader->Clear();
            pPass->SetFogEnable(false);
            pPass->SetAlphaBlend(false);
            pPass->SetZBufferEnable(true);
            pPass->SetZWriteEnable(true);
			IGPUProgram* pVSShadowmap = NULL;
            if (m_bHardwareInstance)
            {
                pVSShadowmap = GPUPROGRAM_MANAGER->GetGPUProgram("VSInstancingRenderToShadow"); //pRenderer->CreateVertexShader("VSHardwareInstancingShadowmap.txt", m_pTechniqueRenderToShadow->GetVSVersion().c_str());
                //pPass->SetVertexShader(pVSShadowmap);
            }
            else
            {
                pVSShadowmap = GPUPROGRAM_MANAGER->GetGPUProgram("VSInstancingRenderToShadow"); //pRenderer->CreateVertexShader("VSShaderInstancingShadowmap.txt", m_pTechniqueRenderToShadow->GetVSVersion().c_str());
                //pPass->SetVertexShader(pVSShadowmap);
                
            }
            IGPUProgram* pPSShadowmap = GPUPROGRAM_MANAGER->GetGPUProgram("PSRenderToShadowmap"); //pRenderer->CreatePixelShader("PSShadowmap.txt", m_pTechniqueRenderToShadow->GetPSVersion().c_str());
            //pPass->SetPixelShader(pPSShadowmap);
            pPass->SetRasterizerStateObject(0);

			IGPUProgram* pGpuPrograms[4] = { 0 };
			pGpuPrograms[0] = pVSShadowmap;
			pGpuPrograms[1] = pPSShadowmap;
			ShaderObject* pShaderObject = pRenderer->CreateShaderObject(pGpuPrograms, 1);
			pPass->SetShaderObject(pShaderObject);
        }

        //pPass->SetDepthBias(SHADOWMAP->GetShadowDepthBias());
        //pPass->SetSlopeScaleDepthBias(SHADOWMAP->GetShadowScoleScaleBias());
        m_pTechniqueRenderToShadow->AddPass(pPass);

        m_pMaterial->AddTechnique(m_pTechniqueRenderToShadow);
    }
    */
}

void MeshInstancingObject::AcceptShadowmap(bool bAccept)
{
    m_bAcceptShadow = bAccept;
    //m_pMaterial->SetCurrentTechnique(TECHNIQUE_SELECTOR->SelectTechnique(this));
}

void MeshInstancingObject::SetAcceptLighting(bool bAccept)
{
    m_bAcceptLighting = bAccept;
    //m_pMaterial->SetCurrentTechnique(TECHNIQUE_SELECTOR->SelectTechnique(this));
}

/*
void MeshInstancingObject::RenderToShadowMap(CCamera* pCamera)
{
    if (m_lstMeshInstances.size() < 3)
    {
        ListMeshInstanceIter itEnd = m_lstMeshInstances.end();
        for (ListMeshInstanceIter it = m_lstMeshInstances.begin(); it != itEnd; ++it)
        {
            (*it)->RenderToShadowMap(pCamera);
        }
    }
    else
    {
        //if (m_pTechniqueRenderToShadow == NULL)
        //{
        //    GenRenderToShadowTechnique();
        //}

        IRenderer* pRenderer = ENGINE_INST->GetRenderer();
        PreRender(pCamera);

        _GetPrimitive();
    }
}
*/
/*
void MeshInstancingObject::ResetCurrentTechnique()
{
    if (m_pMaterial)
    {
        m_pMaterial->SetCurrentTechnique(TECHNIQUE_SELECTOR->SelectTechnique(this));
    }
    
}
*/

void MeshInstancingObject::_GetPrimitive()
{
    if (m_bHardwareInstance)
    {
        if (m_pPrimitive == NULL)
        {
            m_pPrimitive = new InstancingPrimitive(Primitive::Primitive_TriangleList/*, m_pHardwareVertexDecl*/);
            m_pPrimitive->m_pSrcRenderable = this;
        }
        m_pPrimitive->m_pVertexBuffer = m_pMeshRes->GetVertexBuffer();
        m_pPrimitive->m_pIndexBuffer = m_pMeshRes->GetIndexBuffer();
        (static_cast<InstancingPrimitive*>(m_pPrimitive))->m_pInstanceVB = m_pInstanceDataVB;
    }
    else
    {
        if (m_pPrimitive == NULL)
        {
            m_pPrimitive = new Primitive(Primitive::Primitive_TriangleList/*, m_pHardwareVertexDecl*/);
            m_pPrimitive->m_pSrcRenderable = this;
        }
    }
}
/*
void MeshInstancingObject::GenRenderToGBufferTechnique()
{
    m_pTechniqueRenderToGBuffer = m_pMaterial->GetTechnique("MeshInstancing_RenderToGBuffer");

    if (m_pTechniqueRenderToGBuffer == NULL)
    {
        CTechnique* pCurrentTechnique = m_pMaterial->GetCurrentTechnique();
        m_pTechniqueRenderToGBuffer = new CTechnique(m_pMaterial);
        m_pTechniqueRenderToGBuffer->SetName("MeshInstancing_RenderToGBuffer");
        //AIRString strVSVersion = "vs_2_0";
        //AIRString strPSVersion = "ps_2_0";

        //m_pTechniqueRenderToGBuffer->SetVSVersion(strVSVersion.c_str());
        //m_pTechniqueRenderToGBuffer->SetPSVersion(strPSVersion.c_str());
        CPass* pPass = new CPass(m_pTechniqueRenderToGBuffer);
        *pPass = *(pCurrentTechnique->GetPass(0));
        //把第1阶段以后的纹理都清除
        for (size_t i = 1; i < 8; ++i)
        {
            pPass->RemoveTextureUnit(i);

        }
        //IShader* pShader = pPass->GetShader();

        IRenderer* pRenderer = ENGINE_INST->GetRenderer();
        //if (pShader)
        {
        //    pShader->Clear();
            pPass->SetFogEnable(false);
            pPass->SetAlphaBlend(false);
            pPass->SetZWriteEnable(true);

            AIRString vsShader = m_bHardwareInstance ? "VSInstancingRenderToGBuffer" : "VSShaderInstancingLightPrePassGBuffer.txt";
            //AIRString psShader = "";

            IGPUProgram* pVertexShader = GPUPROGRAM_MANAGER->GetGPUProgram(vsShader); //pRenderer->CreateVertexShader(vsShader.c_str(), m_pTechniqueRenderToGBuffer->GetVSVersion().c_str());
            //pPass->SetVertexShader(pVertexShader);

            IGPUProgram* pPixelShader = GPUPROGRAM_MANAGER->GetGPUProgram("PSLPPRenderToGBuffer"); //pRenderer->CreatePixelShader("PSLightPrePassGBuffer.txt", m_pTechniqueRenderToGBuffer->GetPSVersion().c_str());
            //pPass->SetPixelShader(pPixelShader);
			
			IGPUProgram* pGpuPrograms[4] = { 0 };
			pGpuPrograms[0] = pVertexShader;
			pGpuPrograms[1] = pPixelShader;
			ShaderObject* pShaderObject = pRenderer->CreateShaderObject(pGpuPrograms, 2);
			pPass->SetShaderObject(pShaderObject);
        }

        //pPass->SetDepthBias(SHADOWMAP->GetShadowDepthBias());
        //pPass->SetSlopeScaleDepthBias(SHADOWMAP->GetShadowScoleScaleBias());
        m_pTechniqueRenderToGBuffer->AddPass(pPass);

        m_pMaterial->AddTechnique(m_pTechniqueRenderToGBuffer);
    }
}
*/

InstancingObject::InstancingObject() : m_pMeshRes(NULL), m_nMaxInstancesNum(0)
{

}

InstancingObject::InstancingObject(CAIRSubMesh* pSubMesh) : m_pMeshRes(pSubMesh), m_nMaxInstancesNum(0)
{
    //CreateMeshInstancings();
}

InstancingObject::~InstancingObject()
{

}

void InstancingObject::Clear()
{
    MeshInstancingObjectMapIter itEnd = m_mapMeshInstancingArray.end();
    for (MeshInstancingObjectMapIter it = m_mapMeshInstancingArray.begin(); it != itEnd; ++it)
    {
        MeshInstancingObjectArray* meshArray = &it->second;
        for (size_t i = 0; i < (*meshArray).size(); ++i)
        {
            (*meshArray)[i]->Clear();
            SafeDelete((*meshArray)[i]);
        }

        meshArray->clear();
    }
    m_mapMeshInstancingArray.clear();
    //for (size_t i = 0; i < m_vtMeshInstancings.size(); ++i)
    //{
    //    m_vtMeshInstancings[i]->Clear();
    //    SafeDelete(m_vtMeshInstancings[i]);
    //}
    //m_vtMeshInstancings.clear();
}

MeshInstancingObject* InstancingObject::GetMeshInstancingObject(CRenderQueue* pQueue, int nLod)
{
    MeshInstancingObjectMapIter it = m_mapMeshInstancingArray.find(pQueue);
    if (it != m_mapMeshInstancingArray.end())
    {
        MeshInstancingObjectArray* pArray = &it->second;
        return (*pArray)[nLod];
    }
    else
    {
        CreateMeshInstancings(pQueue);
        it = m_mapMeshInstancingArray.find(pQueue);
        if (it != m_mapMeshInstancingArray.end())
        {
            MeshInstancingObjectArray* pArray = &it->second;
            return (*pArray)[nLod];
        }
    }

    return NULL;
}


void InstancingObject::CreateMeshInstancings(CRenderQueue* pQueue)
{
    int nLOD = ENGINE_INST->GetConfig()->m_bAutoGenLod ? ENGINE_INST->GetConfig()->m_nAutoLodLevelCount : 1;
    MeshInstancingObjectArray vtMeshInstancings;
    for (int i = 0; i < nLOD; ++i)
    {
        MeshInstancingObject* pInstancing = new MeshInstancingObject(this, i, m_pMeshRes);
        
        //m_vtMeshInstancings.push_back(pInstancing);
        vtMeshInstancings.push_back(pInstancing);
    }
    m_mapMeshInstancingArray.insert(std::make_pair(pQueue, vtMeshInstancings));
}

void InstancingObject::ResetCurrentTechnique()
{
    //for (size_t i = 0; i < m_vtMeshInstancings.size(); ++i)
    //{
    //    m_vtMeshInstancings[i]->ResetCurrentTechnique();
    //}

    MeshInstancingObjectMapIter itEnd = m_mapMeshInstancingArray.end();
    for (MeshInstancingObjectMapIter it = m_mapMeshInstancingArray.begin(); it != itEnd; ++it)
    {
        MeshInstancingObjectArray* meshArray = &it->second;
        for (size_t i = 0; i < (*meshArray).size(); ++i)
        {
            //(*meshArray)[i]->ResetCurrentTechnique();
            //SafeDelete((*meshArray)[i]);
        }

        //meshArray->clear();
    }
}

void InstancingObject::Update(f32 fTimeElapse)
{
    //for (size_t i = 0; i < m_vtMeshInstancings.size(); ++i)
    //{
    //    m_vtMeshInstancings[i]->Update(fTimeElapse);
    //}

    MeshInstancingObjectMapIter itEnd = m_mapMeshInstancingArray.end();
    for (MeshInstancingObjectMapIter it = m_mapMeshInstancingArray.begin(); it != itEnd; ++it)
    {
        MeshInstancingObjectArray* meshArray = &it->second;
        for (size_t i = 0; i < (*meshArray).size(); ++i)
        {
            (*meshArray)[i]->Update(fTimeElapse);
            //SafeDelete((*meshArray)[i]);
        }

        //meshArray->clear();
    }
}