#include "stdafx.h"
#include "Billboard.h"
#include "ISceneManager.h"
#include "Camera.h"
#include "Engine.h"
#include "SceneNode.h"
#include "VertexDef.h"
#include "BillboardRenderer.h"
#include "MaterialManager.h"
#include "Primitive.h"
#include "VertexDeclarationManager.h"
#include "TextureMgr.h"

CBillboard::CBillboard() : m_vLocalDir(Vector3Df::UNIT_Y)
, m_bIsYLocalDir(true)
, m_fBBWidth(50.0f)
, m_fBBHeight(50.0f)
, m_ComColor(0xffffffff)
, m_LTColor(0xffffffff)
, m_RTColor(0xffffffff)
, m_LBColor(0xffffffff)
, m_RBColor(0xffffffff)
, m_bUseComColor(true)
, m_bNeedResetColor(false)
, m_bIsOrientAffectedByNode(true)
, m_BillBoardCreateType(eBBCT_Center)
, m_BillBoardFaceType(eBBFT_AlwaysFaceCamera)
, m_bUseRenderer(true)
, m_pTexture(NULL)
{
	m_szType = "billboard";
	m_renderGroupType = eRGT_Transparent;
	m_bForceRenderGroup = true;
	m_pVB = NULL;
	m_pNode = NULL;
	//m_vLocalPos = Vector3Df::ZERO;
	//m_vLocalDir = Vector3Df::UNIT_Y;
	//m_bIsYLocalDir = true;
	//m_fBBWidth = 50.0f;
	//m_fBBHeight = 50.0f;
	//m_ComColor = 0xffffffff;
	//m_LTColor = 0xffffffff;
	//m_RTColor = 0xffffffff;
	//m_LBColor = 0xffffffff;
	//m_RBColor = 0xffffffff;
	//m_bUseComColor = true;
	//m_bNeedResetColor = false;
	//m_bIsOrientAffectedByNode = true;
	//m_BillBoardCreateType = eBBCT_Center;
	//m_BillBoardFaceType = eBBFT_AlwaysFaceCamera;
	
    m_TopLeft.tu = 0.0f;
    m_TopLeft.tv = 0.0f;

    m_TopRight.tu = 1.0f;
    m_TopRight.tv = 0.0f;

    m_BottomLeft.tu = 0.0f;
    m_BottomLeft.tv = 1.0f;

    m_BottomRight.tu = 1.0f;
    m_BottomRight.tv = 1.0f;

	m_vertices[0].tu = 0.0f;
	m_vertices[0].tv = 0.0f;
	m_vertices[1].tu = 1.0f;
	m_vertices[1].tv = 0.0f;
	m_vertices[2].tu = 0.0f;
	m_vertices[2].tv = 1.0f;
	m_vertices[3].tu = 1.0f;
	m_vertices[3].tv = 1.0f;

	m_strVertexDecl = "E_VB_STANDARD";

}

CBillboard:: ~CBillboard()
{

}


void CBillboard::Render(CCamera* pCamera)
{
	//Update(ENGINE_INST->GetTimeSinceLastFrame());
	PreRender( pCamera );
    if (m_bUseRenderer)
    {
        BILLBOARD_RENDERER->Render(this);
    }
    else
    {
	    IRenderer* pRenderer = ENGINE_INST->GetRenderer();

		//if ( m_pNode )
		//{
		//	pRenderer->SetWorldTransform( &m_pNode->GetFullTransform() );
		//}
		//else
		{
			pRenderer->SetWorldTransform( &Matrix4f::IDENTITY );
		}

        _GetPrimitive();

        //m_pPrimitive->Draw(m_pMaterial != NULL ? m_pMaterial->GetCurrentTechnique() : NULL);
    }
	Renderable::AfterRender();
}

void CBillboard::_GetPrimitive()
{
	if ( m_bUseRenderer )
	{
		if (m_pPrimitive == NULL)
		{
		    m_pPrimitive = new Primitive(Primitive::Primitive_TriangleStrip/*, VERTEXDECL_MANAGER->CreateVertexDeclaration(E_VB_STANDARD)*/);
		    m_pPrimitive->m_pSrcRenderable = this;
		    m_pPrimitive->m_pVertexData = &m_vertices[0];
		    m_pPrimitive->SetPrimitiveCount(2);
		    m_pPrimitive->SetUseUserPrimitive(true);
		    m_pPrimitive->m_nVerticesStride = sizeof(VERTEX_STANDARD);
		}
		m_vertices[0] = m_TopLeft;
		m_vertices[1] = m_TopRight;
		m_vertices[2] = m_BottomLeft;
		m_vertices[3] = m_BottomRight;
	}
	else
	{
		if (m_pPrimitive == NULL)
		{
			m_pPrimitive = new Primitive(Primitive::Primitive_TriangleStrip/*, m_pVB->GetVertexDeclaration()*/);
			m_pPrimitive->m_pSrcRenderable = this;
			m_pPrimitive->SetUseUserPrimitive(true);
		}
		u32 nVertexCount = m_pVB->GetVertexCount();
		u32 nFaceCount = 2;
		m_pPrimitive->SetPrimitiveCount(nFaceCount);
		m_pPrimitive->SetVerticesNum(nVertexCount);
		m_pPrimitive->m_pIndexBuffer = NULL;
		m_pPrimitive->m_pVertexBuffer = m_pVB; 
	}
}

void CBillboard::PostRender(CCamera* pCamera)
{
    PreRender(pCamera);
    
    //RENDERER->DrawTriangleStrip( 0, 2, m_vertices, E_VB_STANDARD );
    _GetPrimitive();
    //m_pPrimitive->Draw(NULL);
}

void CBillboard::Clear()
{
    IRenderer* pRenderer = RENDERER;
    if (m_pVB)
    {
        pRenderer->ClearVertexBuffer( m_pVB );
        m_pVB = NULL;
    }
	
    if (m_pMaterial)
    {
        MATERIAL_MANAGER->ClearMaterial(m_pMaterial);
    }

    if (m_pTexture)
    {
        TEXTURE_MANAGER->ClearTexture(m_pTexture);
    }
}

void CBillboard::PreRender(CCamera* pCamera)
{
    //if ( !m_pNode )
    //{
    //    return;
    //}
    if (m_bUseRenderer)
    {
        CaculatePosition();
    }
    else
    {
        if ( !m_pVB )
        {
            CreateVertexBuffer();
        }

        Vector3Df vLook;
        Vector3Df vPosBase;
        if (m_pNode)
        {
            vPosBase = m_pNode->GetDerivedPosition() + m_vLocalPos;
        }
        else
        {
            vPosBase = m_vLocalPos;
        }


        //Vector3Df vDir     = m_pNode->GetDerivedRotation() * m_vLocalDir;
        //vDir.Normalize();
        //Vector3Df vLook    = vPosBase - ENGINE_INST->GetSceneMgr()->GetCamera()->GetPosition();
        //Vector3Df m_vRight   = vLook.CrossProduct( vDir );
        //m_vRight.Normalize();
        if ( m_bIsOrientAffectedByNode && m_pNode)
        {
            m_vDir = m_pNode->GetDerivedRotation() * m_vLocalDir;
        }
        else
        {
            m_vDir = m_vLocalDir;
        }

        // 总是面向摄像机
        if ( m_BillBoardFaceType == eBBFT_AlwaysFaceCamera )
        {
            ENGINE_INST->GetSceneMgr()->GetCamera()->GetRight( m_vRight );
            ENGINE_INST->GetSceneMgr()->GetCamera()->GetUp( m_vDir );
        }
        // 平行于贴图方向并面向摄像机
        else if ( m_BillBoardFaceType == eBBFT_HorzLocalDirFaceCamera )
        {
            if ( m_bIsYLocalDir )
            {
                vLook    = vPosBase - ENGINE_INST->GetSceneMgr()->GetCamera()->GetPosition();
                m_vRight   = vLook.CrossProduct( m_vDir );
            }
            else
            {
                vLook    = vPosBase - ENGINE_INST->GetSceneMgr()->GetCamera()->GetPosition();
                m_vRight   = m_vDir;
                m_vDir     = m_vRight.CrossProduct( vLook );
            }
            m_vRight.Normalize();
            m_vDir.Normalize();
        }
        // 垂直于贴图方向并面向摄像机
        else if ( m_BillBoardFaceType == eBBFT_VertLocalDirFaceCamera )
        {
            vLook		= vPosBase - ENGINE_INST->GetSceneMgr()->GetCamera()->GetPosition();
            m_vRight		= vLook.CrossProduct( m_vDir );
            m_vDir		= m_vRight.CrossProduct( m_vDir );
            m_vDir.Normalize();
            m_vRight.Normalize();
        }

        LPVERTEX_STANDARD pVertex = (LPVERTEX_STANDARD)(m_pVB->Lock());

        switch (m_BillBoardCreateType )
        {
        case eBBCT_LeftTop:		// 以左上角顶点对齐
            pVertex[0].position = vPosBase;
            break;
        case eBBCT_RightTop:	// 以右上角顶点对齐
            pVertex[0].position = vPosBase - m_vRight * m_fBBWidth;
            break;
        case eBBCT_LeftBottom:	// 以左下角顶点对齐
            pVertex[0].position = vPosBase + m_vDir   * m_fBBHeight;
            break;
        case eBBCT_RightBottom:	// 以右下角顶点对齐
            pVertex[0].position = vPosBase + m_vDir   * m_fBBHeight - m_vRight * m_fBBWidth;
            break;
        case eBBCT_LeftCenter:	// 以左边框中点对齐
            pVertex[0].position = vPosBase + m_vDir   * m_fBBHeight * 0.5f;
            break;
        case eBBCT_TopCenter:	// 以上边框中点对齐
            pVertex[0].position = vPosBase - m_vRight * m_fBBWidth  * 0.5;
            break;
        case eBBCT_RightCenter:	// 以右边框中点对齐
            pVertex[0].position = vPosBase + m_vDir   * m_fBBHeight * 0.5f   - m_vRight     * m_fBBWidth;
            break;
        case eBBCT_BottomCenter:// 以下边框中点对齐
            pVertex[0].position = vPosBase + m_vDir   * m_fBBHeight - m_vRight * m_fBBWidth * 0.5f;
            break;
        case eBBCT_Center:		// 以中心对齐
            pVertex[0].position = vPosBase + m_vDir   * m_fBBHeight * 0.5f   - m_vRight     * m_fBBWidth * 0.5f;
            break;
        }
        pVertex[1].position = pVertex[0].position + m_vRight * m_fBBWidth;
        pVertex[2].position = pVertex[0].position - m_vDir   * m_fBBHeight;
        pVertex[3].position = pVertex[2].position + m_vRight * m_fBBWidth;

		m_vertices[0].position = m_TopLeft.position = pVertex[0].position;
		m_vertices[1].position = m_TopRight.position = pVertex[1].position;
		m_vertices[2].position = m_BottomLeft.position = pVertex[2].position;
		m_vertices[3].position = m_BottomRight.position = pVertex[3].position;

        if ( m_bNeedResetColor )
        {
            m_bNeedResetColor = false;
            if ( m_bUseComColor )
            {
                pVertex[0].color = pVertex[1].color = pVertex[2].color = pVertex[3].color = m_ComColor;
            }
            else
            {
                pVertex[0].color = m_LTColor;
                pVertex[1].color = m_RTColor;
                pVertex[2].color = m_LBColor;
                pVertex[3].color = m_RBColor;
            }
        }

		m_vertices[0].color = m_TopLeft.color = pVertex[0].color;
		m_vertices[1].color = m_TopRight.color = pVertex[1].color;
		m_vertices[2].color = m_BottomLeft.color = pVertex[2].color;
		m_vertices[3].color = m_BottomRight.color = pVertex[3].color;


        m_tri1.Set(pVertex[0].position, pVertex[1].position, pVertex[2].position);
        m_tri2.Set(pVertex[1].position, pVertex[3].position, pVertex[2].position);

        m_pVB->Unlock();
    }
    
}


void CBillboard::CreateVertexBuffer( void )
{
	//m_pVB = ENGINE_INST->GetRenderer()->CreateVertexBuffer( E_VB_STANDARD, 4, false );
    m_pVB = ENGINE_INST->GetRenderer()->CreateVertexBuffer( 4, sizeof(VERTEX_STANDARD), false );
	LPVERTEX_STANDARD pVertex = (LPVERTEX_STANDARD)(m_pVB->Lock());
	if ( m_bUseComColor )
	{
		pVertex[0].color = pVertex[1].color = pVertex[2].color = pVertex[3].color = m_ComColor;
	}
	else
	{
		pVertex[0].color = m_LTColor;
		pVertex[1].color = m_RTColor;
		pVertex[2].color = m_LBColor;
		pVertex[3].color = m_RBColor;
	}
	pVertex[0].position = pVertex[1].position = pVertex[2].position = pVertex[3].position = Vector3Df::ZERO;
	pVertex[0].tu = 0.0f; pVertex[0].tv = 0.0f;
	pVertex[1].tu = 1.0f; pVertex[1].tv = 0.0f;
	pVertex[2].tu = 0.0f; pVertex[2].tv = 1.0f;
	pVertex[3].tu = 1.0f; pVertex[3].tv = 1.0f;
	m_pVB->Unlock();
}

void CBillboard::SetBBSize(float fWidth, float fHeight )
{
    m_fBBWidth = fWidth;
    m_fBBHeight = fHeight;
    m_boundingBox.MaxEdge.x = fWidth / 2;
    m_boundingBox.MaxEdge.y = fHeight / 2;
    m_boundingBox.MaxEdge.z = fWidth / 2;
    m_boundingBox.MinEdge.x = -fWidth / 2;
    m_boundingBox.MinEdge.y = -fHeight / 2;
    m_boundingBox.MinEdge.z = -fWidth / 2;

	if ( m_pNode )
	{
		m_pNode->NeedUpdate();
		m_pNode->Update();
	}
}


bool CBillboard::IsPick(const Vector3Df& vOrg, const Vector3Df& vPickDir)
{
    Vector3Df point;
    if (m_tri1.GetIntersectionWithLine(vOrg, vPickDir, point))
    {
        return true;
    }
    if (m_tri2.GetIntersectionWithLine(vOrg, vPickDir, point))
    {
        return true;
    }

    return false;
}

bool CBillboard::IsPick(const Vector3Df& vOrg, const Vector3Df& vPickDir, float& distFromEyeToObject)
{
    Vector3Df point;
    if (m_tri1.GetIntersectionWithLine(vOrg, vPickDir, point))
    {
        distFromEyeToObject = vOrg.GetDistanceFrom(point);
        return true;
    }
    if (m_tri2.GetIntersectionWithLine(vOrg, vPickDir, point))
    {
        distFromEyeToObject = vOrg.GetDistanceFrom(point);
        return true;
    }

    return false;
}

void CBillboard::CaculatePosition()
{
    Vector3Df vLook;
    Vector3Df vPosBase;
    if (m_pNode)
    {
        vPosBase = m_pNode->GetDerivedPosition() + m_vLocalPos;
    }
    else
    {
        vPosBase = m_vLocalPos;
    }


    //Vector3Df vDir     = m_pNode->GetDerivedRotation() * m_vLocalDir;
    //vDir.Normalize();
    //Vector3Df vLook    = vPosBase - ENGINE_INST->GetSceneMgr()->GetCamera()->GetPosition();
    //Vector3Df m_vRight   = vLook.CrossProduct( vDir );
    //m_vRight.Normalize();
    if ( m_bIsOrientAffectedByNode && m_pNode)
    {
        m_vDir = m_pNode->GetDerivedRotation() * m_vLocalDir;
    }
    else
    {
        m_vDir = m_vLocalDir;
    }

    // 总是面向摄像机
    if ( m_BillBoardFaceType == eBBFT_AlwaysFaceCamera )
    {
        ENGINE_INST->GetSceneMgr()->GetCamera()->GetRight( m_vRight );
        ENGINE_INST->GetSceneMgr()->GetCamera()->GetUp( m_vDir );
    }
    // 平行于贴图方向并面向摄像机
    else if ( m_BillBoardFaceType == eBBFT_HorzLocalDirFaceCamera )
    {
        if ( m_bIsYLocalDir )
        {
            vLook    = vPosBase - ENGINE_INST->GetSceneMgr()->GetCamera()->GetPosition();
            m_vRight   = vLook.CrossProduct( m_vDir );
        }
        else
        {
            vLook    = vPosBase - ENGINE_INST->GetSceneMgr()->GetCamera()->GetPosition();
            m_vRight   = m_vDir;
            m_vDir     = m_vRight.CrossProduct( vLook );
        }
        m_vRight.Normalize();
        m_vDir.Normalize();
    }
    // 垂直于贴图方向并面向摄像机
    else if ( m_BillBoardFaceType == eBBFT_VertLocalDirFaceCamera )
    {
        vLook		= vPosBase - ENGINE_INST->GetSceneMgr()->GetCamera()->GetPosition();
        m_vRight		= vLook.CrossProduct( m_vDir );
        m_vDir		= m_vRight.CrossProduct( m_vDir );
        m_vDir.Normalize();
        m_vRight.Normalize();
    }

    //LPVERTEX_STANDARD pVertex = (LPVERTEX_STANDARD)(m_pVB->Lock());

    switch (m_BillBoardCreateType )
    {
    case eBBCT_LeftTop:		// 以左上角顶点对齐
        m_TopLeft.position = vPosBase;
        break;
    case eBBCT_RightTop:	// 以右上角顶点对齐
        m_TopLeft.position = vPosBase - m_vRight * m_fBBWidth;
        break;
    case eBBCT_LeftBottom:	// 以左下角顶点对齐
        m_TopLeft.position = vPosBase + m_vDir   * m_fBBHeight;
        break;
    case eBBCT_RightBottom:	// 以右下角顶点对齐
        m_TopLeft.position = vPosBase + m_vDir   * m_fBBHeight - m_vRight * m_fBBWidth;
        break;
    case eBBCT_LeftCenter:	// 以左边框中点对齐
        m_TopLeft.position = vPosBase + m_vDir   * m_fBBHeight * 0.5f;
        break;
    case eBBCT_TopCenter:	// 以上边框中点对齐
        m_TopLeft.position = vPosBase - m_vRight * m_fBBWidth  * 0.5;
        break;
    case eBBCT_RightCenter:	// 以右边框中点对齐
        m_TopLeft.position = vPosBase + m_vDir   * m_fBBHeight * 0.5f   - m_vRight     * m_fBBWidth;
        break;
    case eBBCT_BottomCenter:// 以下边框中点对齐
        m_TopLeft.position = vPosBase + m_vDir   * m_fBBHeight - m_vRight * m_fBBWidth * 0.5f;
        break;
    case eBBCT_Center:		// 以中心对齐
        m_TopLeft.position = vPosBase + m_vDir   * m_fBBHeight * 0.5f   - m_vRight     * m_fBBWidth * 0.5f;
        break;
    }
    m_TopRight.position = m_TopLeft.position + m_vRight * m_fBBWidth;
    m_BottomLeft.position = m_TopLeft.position - m_vDir   * m_fBBHeight;
    m_BottomRight.position = m_BottomLeft.position + m_vRight * m_fBBWidth;

	m_vertices[0].position = m_TopLeft.position;
	m_vertices[1].position = m_TopRight.position;
	m_vertices[2].position = m_BottomLeft.position;
	m_vertices[3].position = m_BottomRight.position;

    if ( m_bNeedResetColor )
    {
        m_bNeedResetColor = false;
        if ( m_bUseComColor )
        {
            m_TopLeft.color = m_TopRight.color = m_BottomLeft.color = m_BottomRight.color = m_ComColor;
        }
        else
        {
            m_TopLeft.color = m_LTColor;
            m_TopRight.color = m_RTColor;
            m_BottomLeft.color = m_LBColor;
            m_BottomRight.color = m_RBColor;
        }

		m_vertices[0].color = m_TopLeft.color;
		m_vertices[1].color = m_TopRight.color;
		m_vertices[2].color = m_BottomLeft.color;
		m_vertices[3].color = m_BottomRight.color;
    }

    m_tri1.Set(m_TopLeft.position, m_TopRight.position, m_BottomLeft.position);
    m_tri2.Set(m_TopRight.position, m_BottomRight.position, m_BottomLeft.position);
}

void CBillboard::SetUseRenderer(bool bUseRenderer)
{
    m_bUseRenderer = bUseRenderer;
}

void CBillboard::SetTexture(ITexture* pTexture)
{
    m_pTexture = pTexture;
    m_bPrepared = false;
}

void CBillboard::SetQuadUV(const Rectf& uvRect)
{
    m_TopLeft.tu = uvRect.left;
    m_TopLeft.tv = uvRect.top;

    m_TopRight.tu = uvRect.right;
    m_TopRight.tv = uvRect.top;

    m_BottomLeft.tu = uvRect.left;
    m_BottomLeft.tv = uvRect.bottom;

    m_BottomRight.tu = uvRect.right;
    m_BottomRight.tv = uvRect.bottom;
}

void CBillboard::SetTopLeftPosition(const Vector3Df& position)
{
    m_TopLeft.position = position;
}

void CBillboard::SetTopRightPosition(const Vector3Df& position)
{
    m_TopRight.position = position;
}

void CBillboard::SetBottomLeftPosition(const Vector3Df& position)
{
    m_BottomLeft.position = position;
}

void CBillboard::SetBottomRightPosition(const Vector3Df& position)
{
    m_BottomRight.position = position;
}

void CBillboard::SetTransparent(bool bAlpha /* = true */, u8 nAlpha /* = 128 */)
{
    //m_TopLeft.color.SetAlpha(nAlpha);
    //m_TopRight.color.SetAlpha(nAlpha);
    //m_BottomLeft.color.SetAlpha(nAlpha);
    //m_BottomRight.color.SetAlpha(nAlpha);
	m_TopLeft.color.a = nAlpha;
	m_TopRight.color.a = nAlpha;
	m_BottomLeft.color.a = nAlpha;
	m_BottomRight.color.a = nAlpha;
}

bool CBillboard::Prepare()
{
    if (!m_bPrepared)
    {
        if (m_pTexture)
        {
            if (!m_pTexture->IsReady())
            {
                if (m_pTexture->IsLoaded())
                {
                    m_pTexture->Cache();
                }
            }

            m_bPrepared = m_pTexture->IsReady();
        }
        else
        {
            m_bPrepared = true;
        }
    }

    return m_bPrepared;
}

//---------------------------------------

/*
BillboardManager::~BillboardManager( void )
{
	Clear();
}

CBillboard* BillboardManager::CreateDefaultBillboard( const AIRString& szName )
{
	CBillboard* pBB = GetBillboard( szName );
	if ( pBB )
	{
		return pBB;
	}
	pBB = new CBillboard(szName.c_str());
	pBB->SetName( szName.c_str() );
	m_vtBillboard.push_back( pBB );

    return pBB;
}

void BillboardManager::AddBillboard( CBillboard* pBB )
{
	for ( BILLBOARD_ARRAY_ITER it = m_vtBillboard.begin(); it != m_vtBillboard.end(); ++it )
	{
		if ( (*it) == pBB )
		{
			return;
		}
	}
	m_vtBillboard.push_back( pBB );
}

CBillboard* BillboardManager::GetBillboard( const AIRString& szName )
{
	for ( BILLBOARD_ARRAY_ITER it = m_vtBillboard.begin(); it != m_vtBillboard.end(); ++it )
	{
		if ( (*it)->GetName() == szName )
		{
			return *it;
		}
	}
	return NULL;
}

void BillboardManager::Clear()
{
	CBillboard* pBB = NULL;
	for (BILLBOARD_ARRAY_ITER it = m_vtBillboard.begin(); it != m_vtBillboard.end(); ++it)
	{
		pBB = *it;
		pBB->Clear();
		SafeDelete(pBB);
	}
	m_vtBillboard.clear();
}

void BillboardManager::UpdateAllBillboard( float fTimeElapse )
{
	CBillboard* pBB = NULL;
	for (BILLBOARD_ARRAY_ITER it = m_vtBillboard.begin(); it != m_vtBillboard.end(); ++it)
	{
		pBB = *it;
		pBB->Update( fTimeElapse );
	}
}
*/

