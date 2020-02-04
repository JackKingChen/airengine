#include "stdafx.h"
#include "Renderable.h"
#include "Engine.h"
#include "StringUtil.h"
#include "SceneNode.h"
#include "MaterialManager.h"
#include "IVertexBuffer.h"
#include "RenderQueue.h"
#include "AIRAnimation.h"
#include "AIRAnimationState.h"
#include "NodeAnimation.h"
#include "BoundingBoxEntity.h"
#include "Shadowmap.h"
#include "TextureAnimationTrack.h"
#include "TimeBaseProfile.h"
#include "Primitive.h"
//#include "Technique.h"
//#include "IShader.h"
#include "GPUProgramManager.h"
#include "ISceneManager.h"
#include "BoxShape.h"
#include "SceneDebug.h"
#include "RenderContext.h"
#include "VertexDeclarationManager.h"
//#include "../Engine/src/RenderSystem/MaterialManager.h"
//int g_renderableCount = 0;

Renderable::Renderable() : 
m_szType( "irenderable" ),
m_pMaterial(NULL),
m_pNode(NULL), 
//m_bRenderWithShader(false),
m_bShowBoundingBox(false),
//m_renderableType(RT_Other),
m_bVisable(true),
m_bInitialize(false),
m_bTransformByParent(true),
m_boundingBoxEntity(NULL)
, m_bAcceptShadow(false)
, m_bRenderToShadow(false)
, m_bAcceptLighting(false)
//, m_pAnimationTexture(NULL)
//, m_pCurTextureAnimState(NULL)
, m_colorDiffuse(0xffffffff)
, m_pCurNodeAnimState(NULL)
, m_renderGroupType(eRGT_Solid)
, m_pPrimitive(NULL)
, m_pVertexDecl(NULL)
, m_bPrepared(false)
, m_bShareParentMaterial(false)
//, m_pTechniqueRenderToShadow(NULL)
//, m_pTechniqueRenderToGBuffer(NULL)
//, m_pTechniqueRenderToDepthMap(NULL)
, m_nID(0xffffffff)
, m_bForceRenderGroup(false)
, m_pShape(NULL)
, m_bCollision(false)
{
    //m_nID = g_renderableCount++;
}



Renderable::~Renderable() 
{
    //ClearShaderParams(); 
    //g_renderableCount--;
    //OutputDebugString(GetString("now the renderable[%d] type[%s] count is:%d\n", m_nID, m_szType.c_str(), g_renderableCount));
	
	//-----------------------------------
	//! add by hjj 2010-5-24
	DestroyAllAnimations();
	m_pCurNodeAnimState = NULL;
	//-----------------------------------
    if (m_pNode)
    {
        m_pNode->DetachRenderable(this);
    }

    if (m_pPrimitive)
    {
        SafeDelete(m_pPrimitive);
    }
}

const AIRString&  Renderable::GetTypeString( void ) const
{
    return m_szType;
}

void Renderable::Clear()
{
    if (m_pMaterial && !m_bShareParentMaterial)
    {
        MATERIAL_MANAGER->ClearMaterial(m_pMaterial);		
    }
    m_pMaterial = NULL;
    //ClearShaderParams(); 
    ClearRenderableImp();
    if (m_pNode)
    {
        m_pNode->DetachRenderable(this);
    }    
    m_renderData.Clear();
    m_pNode = NULL;

    if (m_boundingBoxEntity)
    {
        SafeDelete(m_boundingBoxEntity);
    }
	//-----------------------------------
	//! add by hjj 2010-5-24
	DestroyAllAnimations();
	m_pCurNodeAnimState = NULL;
	//-----------------------------------

	if ( m_pPrimitive )
	{
		SafeDelete( m_pPrimitive );
		m_pPrimitive = NULL;
	}
}

void Renderable::SetMaterial(IMaterial* pMaterial) 
{
    m_pMaterial = pMaterial;
}

void Renderable::SetName(const AIRString& strName) 
{ 
    m_strName = strName; 
}


bool Renderable::LoadFromXML(CMarkupSTL& xml)
{
    AIRString strAttribute;

    strAttribute = xml.GetAttrib("CenterX").c_str();
    if (strAttribute.length())
    {
        m_vtCenter.x = atof(strAttribute.c_str());
    }

    strAttribute = xml.GetAttrib("CenterY").c_str();
    if (strAttribute.length())
    {
        m_vtCenter.y = atof(strAttribute.c_str());
    }

    strAttribute = xml.GetAttrib("CenterZ").c_str();
    if (strAttribute.length())
    {
        m_vtCenter.z = atof(strAttribute.c_str());
    }

    strAttribute = xml.GetAttrib("Material").c_str();
    IRenderer* pRenderer = ENGINE_INST->GetRenderer();
    if (strAttribute.length())
    {
        AIRString strPath, strName;
        CStringUtil::SplitFilename(strAttribute, strPath, strName);
        m_pMaterial = MATERIAL_MANAGER->CreateMaterial(strAttribute.c_str());
        if (m_pMaterial == NULL)
        {
            return false;
        }
        //if (m_pMaterial->GetShader()->GetVertexShader())   //存在shader
        //{
        //    m_bRenderWithShader = true;
        //}
    }
    return true;
}

/*
void Renderable::RegisterShaderParam(const AIRString& strName, CShaderParameter* pParam)
{
    m_mapShaderParam.insert(make_pair(strName, pParam));
};

CShaderParameter* Renderable::GetShaderParameter(AIRString& strName)
{
    SHADERPARAM_ITER it = m_mapShaderParam.find(strName);
    if (it != m_mapShaderParam.end())
    {
        return it->second;
    }
    return NULL;
}

void Renderable::ClearShaderParams()
{
    for (SHADERPARAM_ITER it = m_mapShaderParam.begin(); it != m_mapShaderParam.end(); it++)
    {
        CShaderParameter* pShaderParam = it->second;
        SafeDelete(pShaderParam);
    }
    m_mapShaderParam.clear();
}
*/

void Renderable::SetAttachedNode(CSceneNode* pNode)
{
    m_pNode = pNode;
}

void Renderable::SetTransparent(bool bAlpha /* = true */, u8 nAlpha /* = 128 */)
{
    if (m_pMaterial)
    {
        m_pMaterial->SetTransparent(bAlpha);
    }
    m_colorDiffuse.SetAlpha(nAlpha);
}


void Renderable::SetCurrentAnimation(const char* szAnimationName, float fBlendTime)
{

}

/*
void Renderable::SetRenderableType(RenderableType type)
{
    m_renderableType = type;
}

RenderableType Renderable::GetRenderableType() const
{
    return m_renderableType;
}
*/

void Renderable::SetVisable(bool bVisable)
{
    m_bVisable = bVisable;
}


void Renderable::Render(CCamera* pCamera, eRenderFlow flow)
{
    if (!m_bInitialize)
    {
        Initialize();
    }

    if (!m_bVisable)
    {
        return;
    }

    PreRender(pCamera);
    _GetPrimitive();

    IRenderer* pRenderer = ENGINE_INST->GetRenderer();
    pRenderer->SetWorldTransform(&GetWorldMatrix());

    if (m_pMaterial && m_pMaterial->GetMaterialTemplate() != NULL && m_pPrimitive)
    {
        if ( m_pPrimitive->m_pVertexBuffer )
        {
            pRenderer->SetVertexBuffer(0, m_pPrimitive->m_pVertexBuffer);
        }

        if ( m_pPrimitive->m_pIndexBuffer )
        {
            pRenderer->SetIndexBuffer(m_pPrimitive->m_pIndexBuffer);
        }    

        ShaderMarco macro = m_pMaterial->GetCurShaderMarco();

        //获得材质当前使用的technique
        size_t nTechIndex = m_pMaterial->GetTechniqueIndexByFlow(flow);
        if (nTechIndex != -1)
        {
            RenderTechnique* pTechnique = m_pMaterial->GetRenderTechnique(macro, nTechIndex);

            if (pTechnique)
            {
                for (size_t i = 0; i < pTechnique->m_vtRenderPass.size(); ++i)
                {
                    m_pMaterial->UpdateMaterialParams(&(pTechnique->m_vtRenderPass[i]));
                }

                pRenderer->DrawPrimitive(m_pPrimitive);
            }
        }
        
    }
    else
    {
        //if ( m_pPrimitive )
        //{
        //    m_pPrimitive->Draw(m_pMaterial != NULL ? m_pMaterial->GetCurrentTechnique() : NULL);
        //}    
    }
}

void    Renderable::SetShowBoundingBox( bool b )
{
    m_bShowBoundingBox = b;
}

void Renderable::UpdateToRenderQueue(CCamera* pCamera, CRenderQueue* pQueue)
{
    if (Prepare())
    {
        if (m_bShowBoundingBox)
        {
            ShowBoundingBox();
        }

        CommitNodeAnimation();

        OnUpdateToRenderQueue(pCamera, pQueue);
    }
    
}

void Renderable::OnUpdateToRenderQueue(CCamera* pCamera, CRenderQueue* pQueue)
{
    pQueue->AddRenderable(this);
}

void Renderable::ShowBoundingBox()
{
    //if (m_boundingBoxEntity == NULL)
    //{
    //    m_boundingBoxEntity = new BoundingBoxEntity;
    //}
    //m_boundingBoxEntity->SetWorldBoundingBox(m_WorldAABB);
    //pQueue->AddRenderable(m_boundingBoxEntity);

	//SCENE_DEBUGER->RenderBox(m_WorldAABB, SColor::White, true);
	//SceneDebug::GetInstance()->RenderSphere(m_WorldAABB.GetCenter(), 300, SColor::Red, true);
	SCENE_DEBUGER->RenderAABB(m_WorldAABB, SColor::White, true);
	//Vector3Df v1 = m_WorldAABB.GetCenter();
	//Vector3Df v2 = v1;
	//Vector3Df ext = m_WorldAABB.GetHalfSize();
	//v1.y -= ext.y;
	//v2.y += ext.y;
	//ext.y = 0;
	//SceneDebug::GetInstance()->RenderCylinder(v1, v2, ext.GetLength(), SColor::White, true);
	//SceneDebug::GetInstance()->RenderCone(v2,30,m_WorldAABB.GetExtent().y,Vector3Df::NEGATIVE_UNIT_Y,SColor::Red, true);

}

void Renderable::RenderOcclusionBoundingBox()
{
	u32 k = *((u32*)(this));
	SColor c(k);
	c.SetAlpha(255);
	BoundingBoxInstance->RenderOcclusionBoundingBox( m_WorldAABB, c.color );
}

/*
void    Renderable::RenderBoundingBoxImpl( void )
{
    //  /3--------/7
    // /  |      / |
    ///   |     /  |
    //1---------5  |
    //|   2- - -| -6
    //|  /      |  /
    //|/        | /
    //0---------4/
    if ( m_bShowBoundingBox )
    {
        //ISceneManager* p
        
		Vector3Df vlist[8];
		m_WorldAABB.GetEdges( vlist );
        //if (m_pNode)
        //{
        //    m_pNode->GetWorldBoundingBox().GetEdges(vlist);
        //}
		ENGINE_INST->GetSceneMgr()->RenderBoundingBoxImpl( vlist );
        
    }
}
*/

const AABBBox3Df& Renderable::GetWorldBoundingBox(bool bTransform)
{
    if (bTransform)
    {
        m_WorldAABB = GetBoundingBox();
        if (m_pNode && m_bTransformByParent)
        {
            m_WorldAABB.TransformAffine( m_pNode->GetFullTransform() );
        }
        m_vtCenter = m_WorldAABB.GetCenter();
    }
    return m_WorldAABB;
}

bool Renderable::Initialize()
{
    //由程序员自己创建VB和IB等
    m_bInitialize = true;
    return true;
}

void Renderable::EnableLights(bool bEnable)
{
    if (m_pMaterial)
    {
        m_pMaterial->SetLightEnable(bEnable);
        //m_pMaterial->GetShader()->m_bLight = true;
    }
}

const Vector3Df& Renderable::GetWorldPosition() const
{
    if (m_pNode && m_bTransformByParent)
    {
        return m_pNode->GetDerivedPosition();
    }
    if (m_pNode && !m_bTransformByParent)
    {
        return m_vtCenter;
    }
    return Vector3Df::ZERO;
}

const Matrix4f& Renderable::GetWorldMatrix() const
{
    if (m_pNode && m_bTransformByParent)
    {
        return m_pNode->GetFullTransform();
    }
    return Matrix4f::IDENTITY;
}







//---------------------------

// 创建动画对象
NodeAnimation* Renderable::CreateAnimation( const AIRString& szName, float fLength )
{
	NodeAnimationMap::iterator it = m_mapNodeAnimations.find( szName );
	if ( it != m_mapNodeAnimations.end() )
	{
		DT_TO_MSGBOX_FMT( LT_WARN, GetString("An animation with the name [%s]  already exists.", szName.c_str()) );
		return it->second;
	}
	NodeAnimation* pAnim = new NodeAnimation(m_pNode, szName, fLength);
	m_mapNodeAnimations[szName] = pAnim;
	return pAnim;

}

// 获得动画对象
NodeAnimation* Renderable::GetAnimation( const AIRString& szName )
{
	NodeAnimationMap::iterator it = m_mapNodeAnimations.find(szName);
	if ( it != m_mapNodeAnimations.end())
	{
		return it->second;
	}
	//DT_TO_MSGBOX_FMT( LT_WARN, GetString("Cannot find animation with name [%s].", szName.c_str()) );
	return NULL;
}

// 是否存在动画
bool Renderable::HasAnimation(const AIRString& szName) const
{
	return ( m_mapNodeAnimations.find( szName ) == m_mapNodeAnimations.end() );
}

// 销毁动画
bool Renderable::DestroyAnimation(const AIRString& name)
{
	DestroyAnimationState( name );
	NodeAnimationMap::iterator i = m_mapNodeAnimations.find(name);
	if (i == m_mapNodeAnimations.end())
	{
		DT_TO_MSGBOX_FMT( LT_WARN, GetString("Cannot find animation with name [%s].", name.c_str()) );
		return false;
	}
	SafeDelete( i->second );
	m_mapNodeAnimations.erase(i);
	return true;
}

// 销毁所有动画
void Renderable::DestroyAllAnimations(void)
{
	DestroyAllAnimationStates();
	NodeAnimationMap::iterator i;
	for (i = m_mapNodeAnimations.begin(); i != m_mapNodeAnimations.end(); ++i)
	{
		SafeDelete( i->second );
	}
	m_mapNodeAnimations.clear();

    
}

// 创建动画状态
CAIRAnimationState* Renderable::CreateNodeAnimationState(const AIRString& animName)
{	
	NodeAnimation* pAnim = GetAnimation(animName);

	CAIRAnimationState* pAnimState = GetNodeAnimationState(animName);
	if ( !pAnimState )
	{
		pAnimState = new CAIRAnimationState(pAnim);
//#if NODE_ANIM_TYPE == 0
//		pAnimState->SetTimeLength( pAnim->GetLength() );
//#else
		pAnimState->SetTimeLength( pAnim->GetTimeRange() );
//#endif
		pAnimState->SetTimePos( 0.0f );
		m_mapNodeAnimationStates[ animName ] = pAnimState;
	}
	return pAnimState;
}

// 获得动画状态
CAIRAnimationState* Renderable::GetNodeAnimationState(const AIRString& animName)
{
	NodeAnimationStateMap::iterator it = m_mapNodeAnimationStates.find( animName );
	if ( it == m_mapNodeAnimationStates.end() )
	{
		return NULL;
	}
	return it->second;
}

// 是否存在动画状态
bool Renderable::HasAnimationState(const AIRString& name) const
{
	return ( m_mapNodeAnimationStates.find( name ) == m_mapNodeAnimationStates.end() );
}

// 销毁动画状态
void Renderable::DestroyAnimationState(const AIRString& name)
{
	NodeAnimationStateMap::iterator it = m_mapNodeAnimationStates.find( name );
	if ( it == m_mapNodeAnimationStates.end() )
	{
		return;
	}
	SafeDelete( it->second );
	m_mapNodeAnimationStates.erase( it );
}

// 销毁所有动画状态
void Renderable::DestroyAllAnimationStates(void)
{
	NodeAnimationStateMap::iterator i;
	for (i = m_mapNodeAnimationStates.begin(); i != m_mapNodeAnimationStates.end(); ++i)
	{
		// destroy
		SafeDelete( i->second );
	}
	m_mapNodeAnimationStates.clear();

    
}

// 设置当前的动画
void Renderable::SetCurrentNodeAnimation( const AIRString& szName )
{
	m_pCurNodeAnimState = GetNodeAnimationState( szName );
}

// 每帧更新所有打开的动画
void Renderable::UpdateNodeAnimation( float fTimeElapse )
{
	if ( !m_pCurNodeAnimState )
	{
		return;
	}
	m_pCurNodeAnimState->AddTime( fTimeElapse );
    
}

void Renderable::CommitNodeAnimation()
{
    if (m_pCurNodeAnimState && m_pCurNodeAnimState->IsEnable())
    {
        NodeAnimation* anim = GetAnimation(m_pCurNodeAnimState->GetName());
        //if ( m_pNode )
        //{
        //    m_pNode->ResetToInitialState();
        //}
        //anim->Apply( m_pCurNodeAnimState->GetTimePos(), m_pCurNodeAnimState->GetWeight() );
        anim->GetSceneNode()->ResetToInitialState();
        anim->ApplyToNode(anim->GetSceneNode(), m_pCurNodeAnimState->GetTimePos(), m_pCurNodeAnimState->GetWeight());

        if (m_pCurNodeAnimState->GetTimePos() >= m_pCurNodeAnimState->GetTimeLength() && !m_pCurNodeAnimState->IsLoop())
        {
			//修改为set enable = false
            //m_pCurNodeAnimState = NULL;
			m_pCurNodeAnimState->SetEnable( false );
        }
    }
    
}

void Renderable::AddNodeAnimation(NodeAnimation* pAnimation)
{
    m_mapNodeAnimations.insert(std::make_pair(pAnimation->GetName(), pAnimation));
    CAIRAnimationState* pState = new CAIRAnimationState(pAnimation);
    pState->SetTimeLength(pAnimation->GetTimeRange());
    m_mapNodeAnimationStates.insert(std::make_pair(pAnimation->GetName(), pState));
}


void Renderable::ResetNodeAnimation( void )
{
	if ( m_pCurNodeAnimState )
	{
		m_pCurNodeAnimState->SetTimePos( 0.001f );
		m_pCurNodeAnimState->SetEnable( true );
	}
}

/*
void Renderable::GenRenderToShadowTechnique()
{
    if (m_pMaterial)
    {
        //SHADOWMAP->GenShadowTechnique(m_pMaterial);
    }
}

void Renderable::RenderToShadowMap(CCamera* pCamera)
{
    
}
*/

void Renderable::SetColorDiffuse(const SColor color)
{
    m_colorDiffuse = color;
}

void Renderable::AfterRender( void )
{
	COUNT_OBJECT( GetTypeString() );
}

bool Renderable::Prepare()
{
    if (m_pMaterial)
    {
        if (!m_pMaterial->IsReady())
        {
            m_pMaterial->Cache();
        }
        return m_pMaterial->IsReady();
    }

    return true;
}

bool Renderable::IsTransparent() const
{
    if (m_pMaterial)
    {
        return m_pMaterial->IsTransparent();
    }
    return false;
}

BaseShape* Renderable::GetShape()
{
	//测试用用aabb代替obb
	if(!m_pShape)
	{
		m_pShape = new BoxShape();
	}
	BoxShape* pBox = static_cast<BoxShape*>(m_pShape);
	Vector3Df size = m_boundingBox.MaxEdge - m_boundingBox.MinEdge;
	pBox->setPosition(m_WorldAABB.GetCenter());
	pBox->setBoxSize(size);
	return m_pShape;
}

void Renderable::FillRenderContext(CCamera* pCamera, RenderContextQueue* pQueue, eRenderFlow flow)
{
    if (Prepare())
    {
        CommitNodeAnimation();

        PreRender(pCamera);

        _GetPrimitive();

        pQueue->DrawRenderableToContext(this, pCamera, flow);
    }
    
}

//CVertexDeclaration* Renderable::CreateVertexDecl(IGPUProgram* pVertexShader)
//{
//	assert(m_strVertexDecl != "");
//	if (m_pVertexDecl == NULL)
//	{
//		m_pVertexDecl = VERTEXDECL_MANAGER->CreateVertexDeclaration(m_strVertexDecl, pVertexShader);
//	}
//
//	return m_pVertexDecl;
//}
