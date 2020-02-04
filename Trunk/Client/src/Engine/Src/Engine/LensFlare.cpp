#include "stdafx.h"
#include "LensFlare.h"
#include "WeatherManager.h"
#include "Atmosphere.h"
#include "Engine.h"
#include "MaterialManager.h"
#include "ISceneManager.h"
#include "Camera.h"
#include "Primitive.h"
#include "VertexDeclarationManager.h"
#include "IIndexBuffer.h"
#include "IVertexBuffer.h"
#include "VertexDef.h"
#include "LightingManager.h"
#include "RenderTarget.h"
#include "ITexture.h"
#include "HeightMap.h"
#include "Terrain.h"
#include "MathUtil.h"


LensFlare::LensFlare()
{
	m_szType = "LensFlare";
}

LensFlare::~LensFlare()
{

}


void LensFlare::Clear()
{
	Renderable::Clear();
}

void LensFlare::Create()
{
	if ( !m_pMaterial )
	{
		m_pMaterial = MATERIAL_MANAGER->CreateMaterial( "TemplateMaterial.xml/LensFlare" );
	}
	if ( m_pMaterial->IsReady() )
	{
		m_pMaterial->Cache();
	}

	IRenderer* pRenderer = ENGINE_INST->GetRenderer();
	m_renderData.m_nPrimCount = 7 * 2;
	m_renderData.m_renderType = RT_TRIANGLE_LIST;
	if ( !m_renderData.m_pVB )
	{
		//CVertexDeclaration* pVertexDecl = VERTEXDECL_MANAGER->CreateVertexDeclaration( E_VB_RHW );
		//m_renderData.m_pVB = pRenderer->CreateVertexBuffer( pVertexDecl, 28 );
        m_renderData.m_pVB = pRenderer->CreateVertexBuffer( 28, sizeof(RHWVERTEX) );
	}
	if ( !m_renderData.m_pIB )
	{
		m_renderData.m_pIB = pRenderer->CreateIndexBuffer( m_renderData.m_nPrimCount * 3 );
	}

	s16* pIndex = (s16*)m_renderData.m_pIB->Lock( 0, 0 );
	int vid = 0;
	int cur = 0;
	for( int i = 0; i < 7; ++i )
	{
		pIndex[ cur++ ] = vid;
		pIndex[ cur++ ] = vid + 1;
		pIndex[ cur++ ] = vid + 2;

		pIndex[ cur++ ] = vid + 2;
		pIndex[ cur++ ] = vid + 1;
		pIndex[ cur++ ] = vid + 3;

		vid += 4;
	}
	m_renderData.m_pIB->Unlock();

	LPRHWVERTEX pVer = (LPRHWVERTEX)m_renderData.m_pVB->Lock();

	int curv = 0;
	for ( int i = 0; i < 7; ++i )
	{
		pVer[curv].color = pVer[curv+1].color = pVer[curv+2].color = pVer[curv+3].color = SColor(0xffffffff);
		pVer[curv].rhw = pVer[curv+1].rhw = pVer[curv+2].rhw = pVer[curv+3].rhw = 1.0f;
		pVer[curv].tv = 0.0f;
		pVer[curv+1].tv = 0.0f;
		pVer[curv+2].tv = 1.0f;
		pVer[curv+3].tv = 1.0f;
		pVer[curv].z = pVer[curv+1].z = pVer[curv+2].z = pVer[curv+3].z = 2;

		if ( i == 0 )
		{
			pVer[curv].tu = 0.0f;		
			pVer[curv+1].tu = 0.25f;	
			pVer[curv+2].tu = 0.0f;		
			pVer[curv+3].tu = 0.25f;	
		}
		else if ( i == 1 )
		{
			pVer[curv].tu = 0.25f;		
			pVer[curv+1].tu = 0.5f;		
			pVer[curv+2].tu = 0.25f;	
			pVer[curv+3].tu = 0.5f;		
		}
		else if ( i == 3 || i == 5 )
		{
			pVer[curv].tu = 0.5f;		
			pVer[curv+1].tu = 0.75f;		
			pVer[curv+2].tu = 0.5f;	
			pVer[curv+3].tu = 0.75f;	
		}
		else
		{
			pVer[curv].tu = 0.75f;		
			pVer[curv+1].tu = 1.0f;		
			pVer[curv+2].tu = 0.75f;	
			pVer[curv+3].tu = 1.0f;	
		}

		curv += 4;
	}

	m_renderData.m_pVB->Unlock();
}

void LensFlare::Update( float fTimeElapse )
{
	Prepare();
	Vector3Df vpos = WeatherManager::GetInstance()->GetAtmosphereManager()->GetSunPosition();
	int sw, sh;
	CCamera* pCamera = ENGINE_INST->GetSceneMgr()->GetCamera();
	pCamera->Translate3Dto2D( vpos, mSunX, mSunY );
	ENGINE_INST->GetBackBufferSize( sw, sh );
	mCenterX = sw / 2;
	mCenterY = sh / 2;

	//是否在视口内
	if ( mSunX < 0 || mSunY < 0 || mSunX > sw || mSunY > sh )
	{
		SetVisable( false );
	}
	else
	{
		//计算太阳到摄像机的向量
		Vector3Df vSunToEye = vpos - pCamera->GetPosition();
		//计算太阳到摄像机的向量与视线方向的夹角的余弦值
		if( CMathUtil::Cos( vSunToEye.GetRadianAngleWith( pCamera->GetLookDir() ) ) < 0 )
		{
			SetVisable( false );
		}
		else
		{
			////取深度图
			//RenderTarget* pRTT = LIGHTING_MANAGER->GetRenderTargetGBuffer();
			//if ( pRTT )
			//{
			//	ITexture* pDepthTex = pRTT->GetRenderTexture();
			//	if ( pDepthTex )
			//	{
			//		f32* pPixelData = (f32*)( pDepthTex->RetriveRenderTargetPixelData(false) );
			//		f32 dx = f32(mSunX) / f32(sw) * pDepthTex->GetWidth();
			//		f32 dy = f32(mSunY) / f32(sh) * pDepthTex->GetHeight();
			//		u32 id = (u32(dy) * pDepthTex->GetWidth() + u32(dx));
			//		f32 fDepth = -pPixelData[ id ] / pCamera->GetViewFar();
			//		Vector3Df vout;
			//		pCamera->GetViewMatrix().TransformCoord( vpos, vout );
			//		if ( vout.z > fDepth )
			//		{
			//			SetVisable( false );
			//		}
			//		SafeDeleteArray( pPixelData );
			//	}
			//}
			//else
			{
				Vector3Df vpick;
				CHeightMap* pHM = TERRAIN->GetCurHeightmap();
				SetVisable( !pHM->GetRayPoint( vpos, pCamera->GetPosition(), vpick ) );
			}
		}		
	}
}


void LensFlare::PreRender(CCamera* pCamera)
{
	static const float s_len[7] = { 1.0f, 1.0/2.0f, 1.0f/3.0f, 1.0f/8.0f, -1.0f/2.0f, -1.0f/4.0f, -1.0f/5.5f };
	static const float s_sca[7] = { 1.0f, 0.5f,     0.25f,     1.0f,      0.5f,       0.25f,      0.25f      };
	static const float s_radius = 100.0f;
	Vector2Df vcen( mCenterX, mCenterY );
	Vector2Df vsun( mSunX, mSunY );
	Vector2Df vdir = vsun - vcen;
	Vector2Df vpos;
	float flen = vdir.GetLength();
	float fradius;
	vdir.Normalize();

	LPRHWVERTEX pV = (LPRHWVERTEX)m_renderData.m_pVB->Lock();

	int curv = 0;
	for ( int i = 0; i < 7; ++i )
	{
		vpos = vcen + vdir * flen * s_len[i];
		fradius = s_radius * s_sca[i];
		pV[curv].x = vpos.x - fradius;
		pV[curv].y = vpos.y - fradius;

		pV[curv+1].x = vpos.x + fradius;
		pV[curv+1].y = vpos.y - fradius;

		pV[curv+2].x = vpos.x - fradius;
		pV[curv+2].y = vpos.y + fradius;

		pV[curv+3].x = vpos.x + fradius;
		pV[curv+3].y = vpos.y + fradius;

		curv += 4;
	}

	m_renderData.m_pVB->Unlock();
}

void LensFlare::_GetPrimitive()
{
	if (m_pPrimitive == NULL)
	{
		m_pPrimitive = new Primitive(Primitive::Primitive_TriangleList/*, m_renderData.m_pVB->GetVertexDeclaration()*/);
		m_pPrimitive->m_pSrcRenderable = this;
		m_pPrimitive->SetUseUserPrimitive(true);
	}
	u32 nVertexCount = m_renderData.m_pVB->GetVertexCount();
	u32 nFaceCount = m_renderData.m_nPrimCount;
	m_pPrimitive->SetPrimitiveCount(nFaceCount);
	m_pPrimitive->SetVerticesNum(nVertexCount);
	m_pPrimitive->m_pIndexBuffer = m_renderData.m_pIB;
	m_pPrimitive->m_pVertexBuffer = m_renderData.m_pVB; 
}