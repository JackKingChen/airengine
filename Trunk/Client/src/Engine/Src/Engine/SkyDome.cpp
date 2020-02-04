#include "stdafx.h"
#include "SkyDome.h"
#include "Camera.h"
#include "ISceneManager.h"
#include "IRenderer.h"
#include "Engine.h"
#include "WeatherManager.h"
#include "VertexDeclarationManager.h"
#include "Primitive.h"
#include "MathUtil.h"
#include <fstream>
//#include "WeatherTechniqueSelecter.h"

SkyDome::SkyDome(WeatherManager *s)
: mSkyX(s)
, mCreated(false)
, mSteps(/*70*/21)
, mCircles(/*80*/21)
, mSmoothSkydomeFading(true)
, mSkydomeFadingPercent(0.05f)
{
	m_szType = "SkyDome";
	mSmoothSkydomeFading = false;
	m_renderGroupType = eRGT_Skybox;
	m_bForceRenderGroup = true;
	m_strVertexDecl = "VS_SKYDOME";
}

SkyDome::~SkyDome()
{

}

void SkyDome::Clear()
{
	Renderable::Clear();
}


void SkyDome::Create()
{
	if ( !m_pMaterial )
	{
		m_pMaterial = MATERIAL_MANAGER->CreateMaterial( "WeatherSystemMaterial.xml/SkyDome" );
	}
	Prepare();

	_CreateGeometry();

	if ( !m_pNode )
	{
		m_pNode = mSkyX->GetSceneManager()->CreateSceneNode( "WeatherSkyDomeNode", false );
		m_pNode->AttachRenderable( this );
		m_pNode->SetPosition( mSkyX->GetCamera()->GetPosition() );
	}

	mCreated = true;

	_UpdateGeometry();
}

void SkyDome::_UpdateGeometry()
{
	if ( !mCreated )
	{
		return;
	}

	if ( !m_renderData.m_pVB )
	{
		return;
	}

	float minHeight = 0.0f;

	IRenderer* pRenderer = ENGINE_INST->GetRenderer();

	POS_UV_VERTEX* pVertices = (POS_UV_VERTEX*)m_renderData.m_pVB->Lock();

	u32 nLastVert = m_renderData.m_pVB->GetVertexCount() - 1;
	// 95% of camera far clip distance
	float TODO_Radius = mSkyX->GetCamera()->GetViewFar() * 0.95f;

	pVertices[0].x  = 0;  pVertices[0].z  = 0;	 pVertices[0].y = TODO_Radius;
	pVertices[0].nx = 0;  pVertices[0].nz = 0;   pVertices[0].ny = 1; 
	pVertices[0].u  = 4;  pVertices[0].v  = 4;
	pVertices[0].o  = 1;

	pVertices[nLastVert].x  = 0;  pVertices[nLastVert].z  = 0;	 pVertices[nLastVert].y = -TODO_Radius;
	pVertices[nLastVert].nx = 0;  pVertices[nLastVert].nz = 0;   pVertices[nLastVert].ny = 0; //////
	pVertices[nLastVert].u  = 4;  pVertices[nLastVert].v  = 4;
	pVertices[nLastVert].o  = 1;//mSmoothSkydomeFading ? 0 : 1;

	//f32 fFadeLen = 2 * TODO_Radius * mSkydomeFadingPercent;
	//f32 fStartFade = -TODO_Radius + fFadeLen;
	f32 fFadeLen = TODO_Radius * mSkydomeFadingPercent;

	float AngleStep = AIR::PI / (mCircles+1);
	float r, uvr, c, s, sc;
	u32   x, y;
	float dirAngle;
	for ( y = 0; y < mCircles; ++y )
	{
		dirAngle = AIR::HALF_PI - AngleStep * (y + 1);
		r = CMathUtil::Cos( dirAngle );
		uvr = static_cast<float>(y+1)/(mCircles + 1);

		for ( x = 0; x < mSteps; ++x )
		{
			c = CMathUtil::Cos(AIR::TWO_PI * x / mSteps) * r;
			s = CMathUtil::Sin(AIR::TWO_PI * x / mSteps) * r;
			sc = CMathUtil::Sin( dirAngle );

			u32 id = 1+y*mSteps + x;
			pVertices[id].x = c * TODO_Radius;
			pVertices[id].z = s * TODO_Radius;
			pVertices[id].y = sc * TODO_Radius;

			pVertices[id].nx = c;
			pVertices[id].nz = s;
			pVertices[id].ny = sc < minHeight ? 0 : sc;

			pVertices[id].u = (1 + c*uvr/r)*4;
			pVertices[id].v = (1 + s*uvr/r)*4;

			pVertices[id].o = 1;

			////if ( pVertices[id].y < fStartFade && mSmoothSkydomeFading )
			////{
			////	pVertices[id].o = 1.0f - (fStartFade - pVertices[id].y) / fFadeLen;
			////}
			//if ( CMathUtil::Abs(pVertices[id].y) < fFadeLen && mSmoothSkydomeFading )
			//{
			//	pVertices[id].o = 1.0f - (fFadeLen - pVertices[id].y) / fFadeLen;
			//}
		}
	}

	//float AngleStep = (AIR::PI/2) / (mCircles-1);

	//float r, uvr, c, s, sc;
	//int x, y;

	//for(y=0;y<mCircles-1;y++) 
	//{
	//	r = CMathUtil::Cos(AIR::PI/2 - AngleStep*(y+1));
	//	uvr = static_cast<float>(y+1)/(mCircles-1);

	//	for(x=0;x<mSteps;x++) 
	//	{
	//		c = CMathUtil::Cos(AIR::TWO_PI * x / mSteps) * r;
	//		s = CMathUtil::Sin(AIR::TWO_PI * x / mSteps) * r;
	//		sc = CMathUtil::Sin(CMathUtil::ACos(r));

	//		pVertices[1+y*mSteps + x].x = c * TODO_Radius;
	//		pVertices[1+y*mSteps + x].z = s * TODO_Radius;
	//		pVertices[1+y*mSteps + x].y = sc * TODO_Radius;

	//		pVertices[1+y*mSteps + x].nx = c;
	//		pVertices[1+y*mSteps + x].nz = s;
	//		pVertices[1+y*mSteps + x].ny = sc;

	//		pVertices[1+y*mSteps + x].u = (1 + c*uvr/r)*4;
	//		pVertices[1+y*mSteps + x].v = (1 + s*uvr/r)*4;

	//		pVertices[1+y*mSteps + x].o = 1;
	//	}
	//}

	//r = CMathUtil::Cos(AngleStep);
	//uvr = static_cast<float>(mCircles+1)/(mCircles-1);

	//for(x=0;x<mSteps;x++) 
	//{
	//	c = CMathUtil::Cos(AIR::TWO_PI * x / mSteps) * r;
	//	s = CMathUtil::Sin(AIR::TWO_PI * x / mSteps) * r;

	//	pVertices[1+(mCircles-1)*mSteps + x].x = pVertices[1+(mCircles-2)*mSteps + x].x;
	//	pVertices[1+(mCircles-1)*mSteps + x].z = pVertices[1+(mCircles-2)*mSteps + x].z;
	//	pVertices[1+(mCircles-1)*mSteps + x].y = -TODO_Radius;//pVertices[1+(mCircles-2)*mSteps + x].y - TODO_Radius*mSkydomeFadingPercent;

	//	pVertices[1+(mCircles-1)*mSteps + x].nx = pVertices[1+(mCircles-2)*mSteps + x].nx;
	//	pVertices[1+(mCircles-1)*mSteps + x].nz = pVertices[1+(mCircles-2)*mSteps + x].nz;
	//	pVertices[1+(mCircles-1)*mSteps + x].ny = pVertices[1+(mCircles-2)*mSteps + x].ny;

	//	pVertices[1+(mCircles-1)*mSteps + x].u = (1 + c*uvr/r)*4;
	//	pVertices[1+(mCircles-1)*mSteps + x].v = (1 + s*uvr/r)*4;

	//	pVertices[1+(mCircles-1)*mSteps + x].o = mSmoothSkydomeFading ? 0 : 1;
	//}

	m_renderData.m_pVB->Unlock();

	m_boundingBox.Reset( 
		Vector3Df(-TODO_Radius, 0,          -TODO_Radius), 
		Vector3Df( TODO_Radius, TODO_Radius, TODO_Radius) );

	//if ( m_pNode )
	//{
	//	m_pNode->NeedUpdate();
	//	m_pNode->Update();
	//}
}

void SkyDome::_CreateGeometry()
{
	IRenderer* pRenderer = ENGINE_INST->GetRenderer();

	//int numVertices = mSteps * mCircles + 1;
	//int numEle = 6 * mSteps * (mCircles-1) + 3 * mSteps;

	u32 numVertices = mSteps * mCircles + 2;
	u32 numEle = 6 * mSteps * (mCircles-1) + 3 * mSteps * 2;

	u32 primityCount = numEle / 3;

	if ( m_renderData.m_pVB && m_renderData.m_pVB->GetVertexCount() < numVertices )
	{
		pRenderer->ClearVertexBuffer( m_renderData.m_pVB );
		m_renderData.m_pVB = NULL;
	}

	if ( m_renderData.m_pIB && m_renderData.m_pIB->GetTotalIndex() < numEle )
	{
		pRenderer->ClearIndexBuffer( m_renderData.m_pIB );
		m_renderData.m_pIB = NULL;
	}

	if ( NULL == m_renderData.m_pVB )
	{
        /*
		CVertexDeclaration* pVertexDecl = pRenderer->CreateVertexDeclaration();
		size_t offset = 0;
		pVertexDecl->AddVertexElement( 0, offset, eVET_FLOAT3, eVEU_POSITION, 0 );
		offset += CVertexDeclaration::GetTypeSize( eVET_FLOAT3 );
		pVertexDecl->AddVertexElement( 0, offset, eVET_FLOAT3, eVEU_TEXTURE_COORDINATES, 0 );
		offset += CVertexDeclaration::GetTypeSize( eVET_FLOAT3 );
		pVertexDecl->AddVertexElement( 0, offset, eVET_FLOAT2, eVEU_TEXTURE_COORDINATES, 1 );
		offset += CVertexDeclaration::GetTypeSize( eVET_FLOAT2 );
		pVertexDecl->AddVertexElement( 0, offset, eVET_FLOAT1, eVEU_TEXTURE_COORDINATES, 2 );
		offset += CVertexDeclaration::GetTypeSize( eVET_FLOAT1 );
		pVertexDecl->Build();
		VERTEXDECL_MANAGER->AddVertexDeclaration( pVertexDecl );

		m_renderData.m_pVB = pRenderer->CreateVertexBuffer(pVertexDecl, numVertices);
        */
        m_renderData.m_pVB = pRenderer->CreateVertexBuffer(numVertices, 36);

	}

	if ( !m_renderData.m_pIB )
	{
		m_renderData.m_pIB = pRenderer->CreateIndexBuffer( numEle );
	}

	
	u16 *indexbuffer = (u16*)m_renderData.m_pIB->Lock( 0, 0 );

	u32 nCurID = 0;
	for (u32 k = 0; k < mSteps; k++)
	{
		indexbuffer[k*3] = 0;
		indexbuffer[k*3+1] = k+1;

		if (k != mSteps-1)
		{
			indexbuffer[k*3+2] = k+2;
		}
		else
		{
			indexbuffer[k*3+2] = 1;
		}

		nCurID += 3;
	}

	for(u32 y=0; y<mCircles-1; y++) 
	{
		for(u32 x=0; x<mSteps; x++) 
		{
			u16 *twoface = indexbuffer + (y*mSteps+x)*6 + 3 * mSteps;

			u16 p0 = 1+y * mSteps + x;
			u16 p1 = 1+y * mSteps + x + 1;
			u16 p2 = 1+(y+1)* mSteps + x;
			u16 p3 = 1+(y+1)* mSteps + x + 1;

			if (x == mSteps-1)
			{
				p1 -= x+1;
				p3 -= x+1;
			}

			// First triangle
			twoface[2]=p0;
			twoface[1]=p1;
			twoface[0]=p2;

			// Second triangle
			twoface[5]=p1;
			twoface[4]=p3;
			twoface[3]=p2;

			nCurID += 6;
		}
	}

	//begine add by hjj 2011-11-3
	u32 curVert = numVertices - 1 - mSteps;
	u32 lastVert = numVertices - 1;
	u32 beginVert = curVert;
	for (u32 k = 0; k < mSteps; k++)
	{
		indexbuffer[nCurID++] = curVert;	
		indexbuffer[nCurID++] = lastVert;
		if (k != mSteps-1)
		{
			indexbuffer[nCurID++]= ++curVert;
		}
		else
		{
			indexbuffer[nCurID++] = beginVert;
		}
	}
	//end add


	m_renderData.m_pIB->Unlock();

	m_renderData.m_nPrimCount = primityCount;
	m_renderData.m_renderType = RT_TRIANGLE_LIST;

}

void SkyDome::_SetGeometryParameters( int Steps, int Circles)
{
	if( mSteps == Steps && mCircles == Circles )
		return;
	mSteps = Steps;
	mCircles = Circles;
	//begine add by hjj 2011-11-3
	if ( mCircles < 5 )
	{
		mCircles = 5;
	}
	if ( mCircles % 2 == 0 )
	{
		++mCircles;
	}
	if ( mSteps < 3 )
	{
		mSteps = 3;
	}
	//end add
	Create();
}


float SkyDome::GetSkydomeRadius() 
{
	return mSkyX->GetCamera()->GetViewFar() * 0.95f;
}

bool SkyDome::Prepare()
{
	if ( !m_bPrepared )
	{
		if ( !m_pMaterial->IsReady() )
		{
			m_bPrepared = m_pMaterial->Cache();
		}
		else
		{
			m_bPrepared = true;
		}

        if (m_bPrepared)
        {
            if (m_pMaterial->GetMaterialTemplate())
            {
                ShaderMarco marco;
                marco.Reset();
                WeatherManager* pWM = WeatherManager::GetInstance();
                //是否夜晚
                if ( pWM->IsStarfieldEnabled() )
                {
                    marco.Starfield = 1;
                    //LDR模式
                    if ( pWM->GetLightingMode() == LM_LDR )
                    {
                        //pMat->SetCurrentTechnique( "SkydomeStarfieldLDR" );
                        marco.Ldr = 1;
                    }
                    //HDR模式
                    else
                    {
                        //pMat->SetCurrentTechnique( "SkydomeStarfieldHDR" );
                    }
                }
                //白天
                else
                {
                    //LDR模式
                    if ( pWM->GetLightingMode() == LM_LDR )
                    {
                        marco.Ldr = 1;		
                    }
                    //HDR模式
                    else
                    {
                        //pMat->SetCurrentTechnique( "SkydomeHDR" );
                    }
                }

                m_pMaterial->SetCurShaderMarco(marco);
            }
            else
            {
                //WeatherTechniqueSelecter::SelectTechnique( this );
                m_pMaterial->SetFogEnable(false);
            }

        }
	}
	return m_bPrepared;
}

void SkyDome::Update( float fTimeElapse )
{
	Prepare();

	m_pNode->SetPosition( mSkyX->GetCamera()->GetPosition() );
	m_pNode->NeedUpdate();
	m_pNode->Update();
}

void SkyDome::PreRender(CCamera* pCamera)
{
    m_pNode->SetPosition( ENGINE_INST->GetActiveCamera()->GetPosition() );
    m_pNode->NeedUpdate();
    m_pNode->Update();

    if (m_pMaterial->GetMaterialTemplate())
    {
        ShaderMarco marco;
        marco.Reset();
        WeatherManager* pWM = WeatherManager::GetInstance();
        //是否夜晚
        if ( pWM->IsStarfieldEnabled() )
        {
            marco.Starfield = 1;
            //LDR模式
            if ( pWM->GetLightingMode() == LM_LDR )
            {
                //pMat->SetCurrentTechnique( "SkydomeStarfieldLDR" );
                marco.Ldr = 1;
            }
            //HDR模式
            else
            {
                //pMat->SetCurrentTechnique( "SkydomeStarfieldHDR" );
            }
        }
        //白天
        else
        {
            //LDR模式
            if ( pWM->GetLightingMode() == LM_LDR )
            {
                marco.Ldr = 1;		
            }
            //HDR模式
            else
            {
                //pMat->SetCurrentTechnique( "SkydomeHDR" );
            }
        }

        m_pMaterial->SetCurShaderMarco(marco);
    }

    Renderable::PreRender(pCamera);
}

void SkyDome::_GetPrimitive()
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
