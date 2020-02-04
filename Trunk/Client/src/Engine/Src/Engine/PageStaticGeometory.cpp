#include "stdafx.h"
#include "PageStaticGeometory.h"
#include "VertexDef.h"
#include "SceneNode.h"
#include "ISceneManager.h"
#include "MaterialManager.h"
#include "FileSystem.h"
#include "DataStream.h"
#include "Shadowmap.h"
#include "MemoryDataStream.h"
#include <fstream>
#include "Primitive.h"
//#include "TextureUnit.h"
//#include "Technique.h"
//#include "Pass.h"
#include "Terrain.h"
#include "GPUProgramManager.h"
//#include "TechniqueSelector.h"
#include "GPUVariant.h"
#include "TextureMgr.h"

PageStaticGeometory::PageStaticGeometory() : m_nVertexCountPerObject(0)
, m_nIndexCountPerObject(0)
, m_nObjectCount(0)
, m_bHasGenVertexData(false)
, m_fXMaxOffset(1.0f)
, m_fZMaxOffset(1.0f)
, m_nMaxObjectCount(0)
, m_fObjectWidth(0)
, m_fObjectHeight(0)
, m_PsgAnimType(ePsgAnimSameOffset)
, m_PlantFaceType(ePFT_Never_Face_Camera)
, m_fOffsetFreq(1.0f)
, m_nQuadFaceCountPerObj(0)
, m_pPlantTexture(NULL)
{
	m_matShear.Identity();

	m_szType = "pagestaticgeometry";

	m_fXPos = AIR::RangeRandom(-AIR::PI, AIR::PI);
	m_fZPos = AIR::RangeRandom(-AIR::PI, AIR::PI);

	//m_bHasUpdated = false;

    m_pMaterial = MATERIAL_MANAGER->CreateMaterial("TemplateMaterial.xml/Plant");

	//m_gpuDataParamSource = static_cast<PSGManualGPUDataParamSource*>(CREATE_MANUAL_GPU_PARAM_SOURCE(PSGManualGPUDataParamSource));

    m_renderGroupType = eRGT_Solid;

    m_bAcceptLighting = true;

	m_strVertexDecl = "E_VB_NORMAL";
}

PageStaticGeometory:: ~PageStaticGeometory( void )
{
	Clear();
}

/*
void PageStaticGeometory::Render(CCamera* pCamera)
{
	if ( !m_pNode || !m_bVisable )
	{
		return;
	}

	PreRender( pCamera );

	IRenderer* pRenderer = ENGINE_INST->GetRenderer();
	if ( !m_bHasGenVertexData )
	{
		return;
	}

	if ( m_PsgAnimType == ePsgAnimShearMatrix )
	{
		pRenderer->SetWorldTransform( &(m_matShear ) );
	}
	else
	{
		pRenderer->SetWorldTransform( &m_pNode->GetFullTransform() );
	}

	if (m_pMaterial)
	{
		pRenderer->SetVertexBuffer( 0, m_renderData.m_pVB );
		pRenderer->SetIndexBuffer( m_renderData.m_pIB );
		CTechnique* pTechnique = m_pMaterial->GetCurrentTechnique();
		if (pTechnique)
		{
			size_t nPassNum = pTechnique->GetPassesNum();
			for (size_t i = 0; i < nPassNum; ++i)
			{
				CPass* pPass = pTechnique->GetPass(i);
				pPass->ApplyPass(pRenderer);
				pRenderer->DrawTriangleListByIndex( 0, 0, m_renderData.m_pVB->GetVertexCount(), 0, m_renderData.m_nPrimCount );
			}
		}
	}

	Renderable::AfterRender();
}
*/

void PageStaticGeometory::Clear()
{
	Renderable::Clear();
	m_vtObjectOffsetCur.clear();
	m_vtObjectOffsetInit.clear();
	m_vtObjectInitInfo.clear();
	m_nVertexCountPerObject = 0;	// 每个植物体的顶点数量
	m_nIndexCountPerObject = 0;		// 每个植物体的索引数量
	m_nObjectCount = 0;				// 植物体数量
	m_bHasGenVertexData = false;		// 是否已经创建顶点数据
	m_matShear.Identity();					// 物体切变矩阵
	m_fObjectHeight = 10.0f;			// 植物体的高度
	m_fObjectWidth = 10.0f;				// 植物体的宽度
	m_PlantFaceType = ePFT_Never_Face_Camera;			// 植物体的朝向方式
	m_nMaxObjectCount = 0;
	m_nQuadFaceCountPerObj = 0;

    TEXTURE_MANAGER->ClearTexture(m_pPlantTexture);
    m_pPlantTexture = NULL;

	//SafeDelete( m_gpuDataParamSource ); 
	//m_gpuDataParamSource = NULL;
}

void PageStaticGeometory::PreRender(CCamera* pCamera)
{
	//if ( m_bHasUpdated )
	//{
	//	return;
	//}
	//m_bHasUpdated = true;

	float  fTimeElapse = ENGINE_INST->GetTimeSinceLastFrame();
	m_fXPos += fTimeElapse;
	m_fZPos += fTimeElapse;

	if ( ePFT_Never_Face_Camera == m_PlantFaceType )
	{
		switch ( m_PsgAnimType )
		{
		case ePsgAnimNone:
			break;

		case ePsgAnimSameOffset:
			{
				Vector3Df offset;
				offset.x = AIR::asm_sin(m_fXPos * m_fOffsetFreq) * m_fXMaxOffset / m_fObjectHeight;
				offset.z = AIR::asm_sin(m_fZPos * m_fOffsetFreq) * m_fZMaxOffset / m_fObjectHeight;
				AnimAllObjects( offset );
			}
			break;

		case ePsgAnimDiffOffset:
			{
				float scale = sinf( 3 * ENGINE_INST->GetCurTime() );
				for ( size_t i = 0; i < m_nObjectCount; ++i )
				{
					m_vtObjectOffsetCur[ i ] = m_vtObjectOffsetInit[ i ] * scale;
				}
				AnimAllObjects( m_vtObjectOffsetCur );
			}
			break;

		case ePsgAnimShearMatrix:
			{
				Vector3Df offset;
				offset.x = AIR::asm_sin(m_fXPos * m_fOffsetFreq) * m_fXMaxOffset / m_fObjectHeight;
				offset.z = AIR::asm_sin(m_fZPos * m_fOffsetFreq) * m_fZMaxOffset / m_fObjectHeight;
				m_matShear.MakeShearMatrixByYAxis( offset.x, offset.z );
				AnimAllObjects( m_matShear );
			}
			break;

		case ePsgAnimUseVS:
			{
				Vector3Df offset;
				offset.x = AIR::asm_sin(m_fXPos * m_fOffsetFreq) * m_fXMaxOffset;
				offset.z = AIR::asm_sin(m_fZPos * m_fOffsetFreq) * m_fZMaxOffset;
				//m_gpuDataParamSource->SetOffset( Vector4f(offset.x, 0.0f, offset.z, 0.0f) );
                GPUVariant var;
                var.SetVector4(Vector4f(offset.x, 0.0f, offset.z, 0.0f));
                FixedString strName = "offset";
                m_pMaterial->SetMaterialParam(strName, var);
			}
			break;
		}
	}
	else
	{
		AnimAllObjects( pCamera );
	}
}
/*
void PageStaticGeometory::Update( float fTimeElapse )
{ 
	//m_bHasUpdated = false;

	//if ( m_pMaterial && m_pMaterial->IsReady() && m_pMaterial->GetCurrentTechnique() )
	//{
	//	if ( m_pMaterial->GetCurrentTechnique()->GetPass(0)->GetTextureUnit(0)->GetTextureFileName() != m_szPlantTexFile )
	//	{
	//		m_pMaterial->SetTextureUnitTex(0, m_szPlantTexFile.c_str() );
	//	}
	//}
}
*/
//void PageStaticGeometory::GenPageStaticGeometryData( const LST_OBJECT_INIT_INFO& vtObjectInitInfo, PlantObject* pObjRef )
//{
//	IRenderer* pRenderer		= ENGINE_INST->GetRenderer();
//	CRenderData* pRenderData	= pObjRef->GetRenderData();
//	m_nObjectCount				= vtObjectInitInfo.size();
//	m_nVertexCountPerObject		= pRenderData->m_pVB->GetVertexCount();
//	m_nIndexCountPerObject		= pRenderData->m_pIB->GetTotalIndex();
//	size_t nVertexCount			= m_nVertexCountPerObject * m_nObjectCount;
//	size_t nIndexCount			= m_nIndexCountPerObject * m_nObjectCount;
//	size_t nPrimitiveCount		= pRenderData->m_nPrimCount * m_nObjectCount;
//	m_renderData.m_nPrimCount	= nPrimitiveCount;
//	m_renderData.m_renderType	= pRenderData->m_renderType;
//	m_fObjectHeight				= pObjRef->GetPlantHeight();
//	m_fObjectWidth				= pObjRef->GetPlantWidth();
//	m_PlantFaceType				= pObjRef->GetPlantFaceType();
//	m_nMaxObjectCount			= m_nObjectCount;
//	m_nQuadFaceCountPerObj		= pObjRef->GetPlantFaces();
//
//	if ( NULL == m_renderData.m_pVB )
//	{
//		m_renderData.m_pVB = pRenderer->CreateVertexBuffer( E_VB_NORMAL, nVertexCount, true );
//	}
//	if ( NULL == m_renderData.m_pIB )
//	{
//		m_renderData.m_pIB = pRenderer->CreateIndexBuffer( nIndexCount, false );
//	}
//
//	LPVERTEX_NORMAL pObjVertex = (LPVERTEX_NORMAL)(pRenderData->m_pVB->Lock());
//	WORD* pObjIndex = (WORD*)(pRenderData->m_pIB->Lock(0,0));
//
//	LPVERTEX_NORMAL pVertex = (LPVERTEX_NORMAL)(m_renderData.m_pVB->Lock());
//	WORD* pIndex = (WORD*)(m_renderData.m_pIB->Lock(0,0));
//
//	size_t nCurVertex = 0;
//	size_t nCurIndex = 0;
//
//	m_vtObjectInitInfo.clear();
//	ObjectInitInfo pOII;
//	for ( LST_OBJECT_INIT_INFO::const_iterator it = vtObjectInitInfo.begin(); it != vtObjectInitInfo.end(); ++it )
//	{
//		pOII = *it;
//		m_vtObjectInitInfo.push_back( pOII );
//
//		for ( int j = 0; j < m_nVertexCountPerObject; ++j )
//		{
//			pVertex[nCurVertex].color		= pObjVertex[j].color;
//			pVertex[nCurVertex].tu			= pObjVertex[j].tu;
//			pVertex[nCurVertex].tv			= pObjVertex[j].tv;
//			pVertex[nCurVertex].vPos		= pObjVertex[j].vPos * pOII.m_vScale + pOII.m_vPosition;
//			pVertex[nCurVertex++].normal	= pObjVertex[j].normal;
//		}
//	}
//
//	for ( int i = 0; i < vtObjectInitInfo.size(); ++i )
//	{
//		for ( int j = 0; j < m_nIndexCountPerObject; ++j )
//		{
//			pIndex[nCurIndex++] = pObjIndex[j] + i * m_nVertexCountPerObject;
//		}
//	}
//
//	//////////////////////////////////////////////////////////////////////////
//	// calculate boundingbox
//	m_boundingBox.Reset(0,0,0);
//	for ( int i = 0; i < nCurVertex; ++i )
//	{
//		m_boundingBox.AddInternalPoint( pVertex[i].vPos );
//	}
//	//////////////////////////////////////////////////////////////////////////
//
//	m_renderData.m_pVB->Unlock();
//	m_renderData.m_pIB->Unlock();
//
//	pRenderData->m_pVB->Unlock();
//	pRenderData->m_pIB->Unlock();
//
//	m_bHasGenVertexData = true;
//
//	for ( int i = 0; i < m_nObjectCount; ++i )
//	{
//		float angle = AIR::RangeRandom(0,PI);
//		float x = 0.02 * AIR::asm_cos( angle );
//		float z = 0.02 * AIR::asm_sin( angle );
//		m_vtObjectOffsetInit.push_back( Vector3Df(x,0,z) );
//		m_vtObjectOffsetCur.push_back( Vector3Df::ZERO );
//	}
//}


void PageStaticGeometory::AnimAllObjects( const Vector3Df& vOffset )
{
	if ( !m_bHasGenVertexData )
	{
		return;
	}

	LPVERTEX_NORMAL pVertex = (LPVERTEX_NORMAL)(m_renderData.m_pVB->Lock());

	int jos = m_nQuadFaceCountPerObj < 2 ? 6 : 4;
	int vos = m_nQuadFaceCountPerObj < 2 ? 3 : 2;
	u32 nCurVertex = 0;
	for ( size_t i = 0; i < m_nObjectCount; ++i )
	{
		for ( size_t j = 0; j < m_nVertexCountPerObject; )
		{
			pVertex[nCurVertex++].vPos += vOffset;
			pVertex[nCurVertex++].vPos += vOffset;
			pVertex[nCurVertex++].vPos += vOffset;
			j += jos;
			nCurVertex += vos;
		}
	}

	m_renderData.m_pVB->Unlock();
}

void PageStaticGeometory::AnimAllObjects( const VEC_OBJECT_OFFSET& vtOffset )
{
	if ( !m_bHasGenVertexData )
	{
		return;
	}

	LPVERTEX_NORMAL pVertex = (LPVERTEX_NORMAL)(m_renderData.m_pVB->Lock());

	u32 nCurVertex = 0;
	int jos = m_nQuadFaceCountPerObj < 2 ? 6 : 4;
	int vos = m_nQuadFaceCountPerObj < 2 ? 3 : 2;
	for ( size_t i = 0; i < m_nObjectCount; ++i )
	{
		Vector3Df vOffset = vtOffset[i];
		for ( size_t j = 0; j < m_nVertexCountPerObject; )
		{
			pVertex[nCurVertex++].vPos += vOffset;
			pVertex[nCurVertex++].vPos += vOffset;
			pVertex[nCurVertex++].vPos += vOffset;
			j += jos;
			nCurVertex += vos;
		}
	}

	m_renderData.m_pVB->Unlock();
}



void PageStaticGeometory::AnimAllObjects( const Matrix4f& matShear )
{

}



void PageStaticGeometory::AnimAllObjects( CCamera* pCamera )
{
	if ( !m_bHasGenVertexData || m_nQuadFaceCountPerObj > 1 )
	{
		return;
	}

	LPVERTEX_NORMAL pVertex = (LPVERTEX_NORMAL)(m_renderData.m_pVB->Lock());

	u32 nCurVertex = 0;
	for ( size_t i = 0; i < m_nObjectCount; ++i )
	{
		Vector3Df vLook = pVertex[nCurVertex+4].vPos - ENGINE_INST->GetSceneMgr()->GetCamera()->GetPosition();
		Vector3Df vRight= vLook.CrossProduct( Vector3Df::UNIT_Y );
		vRight.Normalize();
		if ( vRight.Equals( Vector3Df::ZERO ) )
		{
			vRight = ENGINE_INST->GetSceneMgr()->GetCamera()->GetRight();
		}
		Vector3Df vUp;
		if ( ePFT_Always_Face_Camera == m_PlantFaceType )
		{
			vUp = vRight.CrossProduct( vLook );
			vUp.Normalize();
		}
		else if ( ePFT_Horz_Face_Camera == m_PlantFaceType )
		{
			vUp = Vector3Df::UNIT_Y;
		}

		pVertex[nCurVertex+0].vPos = pVertex[nCurVertex+4].vPos - vRight * m_fObjectWidth * 0.5f + vUp * m_fObjectHeight;
		pVertex[nCurVertex+1].vPos = pVertex[nCurVertex+4].vPos + vUp    * m_fObjectHeight;
		pVertex[nCurVertex+2].vPos = pVertex[nCurVertex+0].vPos + vRight * m_fObjectWidth;
		pVertex[nCurVertex+3].vPos = pVertex[nCurVertex+0].vPos - vUp    * m_fObjectHeight;
		pVertex[nCurVertex+5].vPos = pVertex[nCurVertex+3].vPos + vRight * m_fObjectWidth;

		nCurVertex += 6;
	}

	m_renderData.m_pVB->Unlock();
}


void PageStaticGeometory::SetMaterial(IMaterial* pMaterial)
{
	Renderable::SetMaterial(pMaterial);
	if ( !pMaterial )
	{
		DT_TO_MSGBOX_FMT( LT_ERROR, "Material is invalid!" );
		return;
	}

	if ( m_pMaterial->IsReady() )
	{
		//IGPUProgram* pVertexShader = m_pMaterial->GetCurrentTechnique()->GetPass(0)->GetVertexShader();
		//if ( pVertexShader )
		//{
		//	//pVertexShader->AddManualGPUDataParamSource(m_gpuDataParamSource);
		//}
	}
	else
	{
		m_pMaterial->Cache();
	}

}

void PageStaticGeometory::SetPsgAnimType( PSG_ANIM_TYPE pat )
{
	m_PsgAnimType = pat;
	switch (pat)
	{
	case ePsgAnimUseVS:
		//m_pMaterial->SetCurrentTechnique("WaveUsingVS");
		break;
	default:
		//m_pMaterial->SetCurrentTechnique("WaveNotUseVS");
		break;
	}
}



void PageStaticGeometory::AddPlantObject( const LST_OBJECT_INIT_INFO& vtObjectInitInfo, float fWidth, float fHeight, int nQuadFaceCountPerObj, ePlantFaceType ePFT )
{
	IRenderer* pRenderer		= ENGINE_INST->GetRenderer();
	size_t nBeginObjectID		= 0;

	// 如果物体的面数改变了
	if ( nQuadFaceCountPerObj > m_nQuadFaceCountPerObj )
	{
		// 释放原来的顶点数据
		m_renderData.Clear();
	}

	size_t i = 0;
	LST_OBJECT_INIT_INFO::const_iterator it = vtObjectInitInfo.begin();
	// 加入新的物体摆放信息
	while( m_vtObjectInitInfo.size() < m_nMaxObjectCount )
	{
		m_vtObjectInitInfo.push_back( *it++ );
		m_nObjectCount++;
		if ( ++i >= vtObjectInitInfo.size() )
		{
			break;
		}
	}

	// 计算每个物体的顶点数量
	m_nVertexCountPerObject = nQuadFaceCountPerObj * (nQuadFaceCountPerObj < 2 ? 6 : 4);
	// 计算每个物体的索引数量
	m_nIndexCountPerObject	= nQuadFaceCountPerObj * (nQuadFaceCountPerObj < 2 ? 12 : 6);
	m_bHasGenVertexData		= false;
	// 物体的宽高
	m_fObjectHeight			= fHeight;		
	m_fObjectWidth			= fWidth;	
	// 物体朝向方式
	m_PlantFaceType			= ePFT;	
	// 每个物体的四边面数
	m_nQuadFaceCountPerObj	= nQuadFaceCountPerObj;

	RebuildGeometryData();
}


void PageStaticGeometory::CreateInternal()
{
	IRenderer* pRenderer		= ENGINE_INST->GetRenderer();
	size_t nBeginObjectID		= 0;

	// 释放原来的顶点数据
	m_renderData.Clear();

	m_nMaxObjectCount = m_nObjectCount = m_vtObjectInitInfo.size();

	// 计算每个物体的顶点数量
	m_nVertexCountPerObject = m_nQuadFaceCountPerObj * (m_nQuadFaceCountPerObj < 2 ? 6 : 4);
	// 计算每个物体的索引数量
	m_nIndexCountPerObject	= m_nQuadFaceCountPerObj * (m_nQuadFaceCountPerObj < 2 ? 12 : 6);
	m_bHasGenVertexData		= false;
	// 物体朝向方式
	m_PlantFaceType			= ePFT_Never_Face_Camera;	

	RebuildGeometryData();
}


void PageStaticGeometory::DelPlantObject( const Vector3Df& vCenterPos, float fRadius )
{
	bool bNeedRebuildData = false;
	int i = 0;
	for ( LST_OBJECT_INIT_INFO::iterator it = m_vtObjectInitInfo.begin(); it != m_vtObjectInitInfo.end(); )
	{
		Vector3Df v = vCenterPos - (*it).m_vPosition;
		if ( v.GetLength() <= fRadius )
		{
			m_vtObjectInitInfo.erase( it );
			it = m_vtObjectInitInfo.begin();
			std::advance( it, i );
			m_nObjectCount--;
			bNeedRebuildData = true;
		}
		else
		{
			++it;
			++i;
		}
	}
	if ( !bNeedRebuildData )
	{
		return;
	}
	RebuildGeometryData();
}


void PageStaticGeometory::SetMaxObjectCount(size_t nMaxObjectCount )
{
	if ( nMaxObjectCount > m_nMaxObjectCount )
	{
		m_renderData.Clear();
	}
	m_nMaxObjectCount = nMaxObjectCount;
}


void PageStaticGeometory::BuildGeometryDataFromBin( u8**pData )
{
	IRenderer* pRenderer = ENGINE_INST->GetRenderer();
	if ( m_nObjectCount <= 0 )
	{
		m_renderData.m_nPrimCount = 0;
		return;
	}

	// 重建顶点缓冲
	if ( NULL == m_renderData.m_pVB )
	{
		//m_renderData.m_pVB = pRenderer->CreateVertexBuffer( E_VB_NORMAL, m_nMaxObjectCount * m_nVertexCountPerObject, false );
        m_renderData.m_pVB = pRenderer->CreateVertexBuffer( m_nMaxObjectCount * m_nVertexCountPerObject, sizeof(VERTEX_NORMAL), false );
	}

	// 重建索引缓冲
	if ( NULL == m_renderData.m_pIB )
	{
		m_renderData.m_pIB = pRenderer->CreateIndexBuffer( m_nMaxObjectCount * m_nIndexCountPerObject, false );
	}

	u32 vertexDataSize = m_renderData.m_pVB->GetWholeSize();
	u32 indexDataSize = m_renderData.m_pIB->GetWholeSize();

	// 取得顶点数据
	LPVERTEX_NORMAL pVertex = (LPVERTEX_NORMAL)(m_renderData.m_pVB->Lock());
	// 取得索引数据
	u16* pIndex = (u16*)(m_renderData.m_pIB->Lock(0,0));

	memcpy( pVertex, (*pData), vertexDataSize );
	(*pData) += vertexDataSize;
	memcpy( pIndex, (*pData), indexDataSize );
	(*pData) += indexDataSize;

	m_renderData.m_pVB->Unlock();
	m_renderData.m_pIB->Unlock();

	m_bHasGenVertexData = true;

	// 计算三角形数量
	m_renderData.m_nPrimCount	= m_nObjectCount * m_nQuadFaceCountPerObj * (m_nQuadFaceCountPerObj < 2 ? 4 : 2);
	m_renderData.m_renderType	= RT_TRIANGLE_LIST;

	memcpy( &m_boundingBox.MinEdge, *pData, sizeof(Vector3Df) );
	(*pData) += sizeof(Vector3Df);
	memcpy( &m_boundingBox.MaxEdge, *pData, sizeof(Vector3Df) );
	(*pData) += sizeof(Vector3Df);

	m_vtObjectOffsetInit.clear();
	m_vtObjectOffsetCur.clear();
	for ( size_t i = 0; i < m_nObjectCount; ++i )
	{
		float angle = AIR::RangeRandom(0,PI);
		float x = 0.02 * AIR::asm_cos( angle );
		float z = 0.02 * AIR::asm_sin( angle );
		m_vtObjectOffsetInit.push_back( Vector3Df(x,0,z) );
		m_vtObjectOffsetCur.push_back( Vector3Df::ZERO );
	}

	if ( m_pNode )
	{
		m_pNode->NeedUpdate();
		m_pNode->Update();
	}
}


void PageStaticGeometory::RebuildGeometryData( void )
{
	IRenderer* pRenderer = ENGINE_INST->GetRenderer();

	if ( m_nObjectCount <= 0 )
	{
		m_renderData.m_nPrimCount = 0;
		return;
	}

	// 重建顶点缓冲
	if ( NULL == m_renderData.m_pVB )
	{
		//m_renderData.m_pVB = pRenderer->CreateVertexBuffer( E_VB_NORMAL, m_nMaxObjectCount * m_nVertexCountPerObject, false );
        m_renderData.m_pVB = pRenderer->CreateVertexBuffer( m_nMaxObjectCount * m_nVertexCountPerObject, sizeof(VERTEX_NORMAL), false );
	}

	// 重建索引缓冲
	if ( NULL == m_renderData.m_pIB )
	{
		m_renderData.m_pIB = pRenderer->CreateIndexBuffer( m_nMaxObjectCount * m_nIndexCountPerObject, false );
	}

	// 取得顶点数据
	LPVERTEX_NORMAL pVertex = (LPVERTEX_NORMAL)(m_renderData.m_pVB->Lock());
	// 取得索引数据
	u16* pIndex = (u16*)(m_renderData.m_pIB->Lock(0,0));

	size_t nCurVertex = 0;
	size_t nCurIndex = 0;
	u16 nIndexID = 0;

	ObjectInitInfo pOII;
	LST_OBJECT_INIT_INFO::iterator it = m_vtObjectInitInfo.begin();
	// 遍历所有的摆放信息
	for ( ; it != m_vtObjectInitInfo.end(); ++it )
	{
		pOII = *it;

		float avg_angle = AIR::PI / float(m_nQuadFaceCountPerObj);
		float radius = pOII.m_fBaseWidth / 2.0f;
		float start_angle = pOII.m_fAngle;

		// 遍历物体的各个面, 填充顶点缓冲
		for ( size_t face_id = 0; face_id < m_nQuadFaceCountPerObj; ++face_id )
		{
			float xr = radius * AIR::asm_cos( start_angle );
			float zr = radius * AIR::asm_sin( start_angle );
			start_angle += avg_angle;

			//////////////////////////////////////////////////////////////////////////
			// 每个草丛只有一个四边面的填充方式
			// 0        1        2
			// @--------@--------@
			// |      / |      / |
			// |    /   |    /   |
			// |  /     |  /     |
			// |/       |/       |
			// @--------@--------@
			// 3        4        5	// 4:草丛的下边中心点
			//////////////////////////////////////////////////////////////////////////
			if ( m_nQuadFaceCountPerObj < 2 )
			{
				// 0 左上
				pVertex[nCurVertex].color	= 0xffffffff;
				pVertex[nCurVertex].vPos	= Vector3Df( -xr,  pOII.m_fBaseHeight, -zr  ) * pOII.m_vScale + pOII.m_vPosition;
				pVertex[nCurVertex].normal	= Vector3Df::UNIT_Y;
				pVertex[nCurVertex].tu		= 0.0f;
				pVertex[nCurVertex++].tv	= 0.0f;

				// 1 中上
				pVertex[nCurVertex].color	= 0xffffffff;
				pVertex[nCurVertex].vPos	= Vector3Df( 0.0f, pOII.m_fBaseHeight, 0.0f ) * pOII.m_vScale + pOII.m_vPosition;
				pVertex[nCurVertex].normal	= Vector3Df::UNIT_Y;
				pVertex[nCurVertex].tu		= 0.5f;
				pVertex[nCurVertex++].tv	= 0.0f;

				// 2 右上
				pVertex[nCurVertex].color	= 0xffffffff;
				pVertex[nCurVertex].vPos	= Vector3Df(  xr,  pOII.m_fBaseHeight, zr   ) * pOII.m_vScale + pOII.m_vPosition;
				pVertex[nCurVertex].normal	= Vector3Df::UNIT_Y;
				pVertex[nCurVertex].tu		= 1.0f;
				pVertex[nCurVertex++].tv	= 0.0f;

				// 3 左下
				pVertex[nCurVertex].color	= 0xffffffff;
				pVertex[nCurVertex].vPos	= Vector3Df( -xr,  0.0f,      -zr  ) * pOII.m_vScale + pOII.m_vPosition;
				pVertex[nCurVertex].normal	= Vector3Df::UNIT_Y;
				pVertex[nCurVertex].tu		= 0.0f;
				pVertex[nCurVertex++].tv	= 1.0f;

				// 4 中下
				pVertex[nCurVertex].color	= 0xffffffff;
				pVertex[nCurVertex].vPos	= Vector3Df( 0.0f, 0.0f,      0.0f  ) * pOII.m_vScale + pOII.m_vPosition;
				pVertex[nCurVertex].normal	= Vector3Df::UNIT_Y;
				pVertex[nCurVertex].tu		= 0.5f;
				pVertex[nCurVertex++].tv	= 1.0f;

				// 5 右下
				pVertex[nCurVertex].color	= 0xffffffff;
				pVertex[nCurVertex].vPos	= Vector3Df( xr,   0.0f,      zr    ) * pOII.m_vScale + pOII.m_vPosition;
				pVertex[nCurVertex].normal	= Vector3Df::UNIT_Y;
				pVertex[nCurVertex].tu		= 1.0f;
				pVertex[nCurVertex++].tv	= 1.0f;

				// 填充索引数据
				// 013
				pIndex[nCurIndex++] = nIndexID;
				pIndex[nCurIndex++] = nIndexID + 1;
				pIndex[nCurIndex++] = nIndexID + 3;

				// 314
				pIndex[nCurIndex++] = nIndexID + 3;
				pIndex[nCurIndex++] = nIndexID + 1;
				pIndex[nCurIndex++] = nIndexID + 4;

				// 124
				pIndex[nCurIndex++] = nIndexID + 1;
				pIndex[nCurIndex++] = nIndexID + 2;
				pIndex[nCurIndex++] = nIndexID + 4;

				// 425
				pIndex[nCurIndex++] = nIndexID + 4;
				pIndex[nCurIndex++] = nIndexID + 2;
				pIndex[nCurIndex++] = nIndexID + 5;

				nIndexID += 6;
			}

			//////////////////////////////////////////////////////////////////////////
			// 多个面的立体填充方式
			// 0                 1
			// @--------@--------@
			// |                 |
			// |                 |
			// |                 |
			// |                 |
			// @--------@--------@
			// 2                 3
			//////////////////////////////////////////////////////////////////////////
			else
			{
				// 填充顶点数据
				// 0 左上
				pVertex[nCurVertex].color	= 0xffffffff;
				pVertex[nCurVertex].vPos	= Vector3Df( -xr,  pOII.m_fBaseHeight, -zr  ) * pOII.m_vScale + pOII.m_vPosition;
				pVertex[nCurVertex].normal	= Vector3Df::UNIT_Y;
				pVertex[nCurVertex].tu		= 0.0f;
				pVertex[nCurVertex++].tv	= 0.0f;

				// 1 右上
				pVertex[nCurVertex].color	= 0xffffffff;
				pVertex[nCurVertex].vPos	= Vector3Df(  xr,  pOII.m_fBaseHeight, zr   ) * pOII.m_vScale + pOII.m_vPosition;
				pVertex[nCurVertex].normal	= Vector3Df::UNIT_Y;
				pVertex[nCurVertex].tu		= 1.0f;
				pVertex[nCurVertex++].tv	= 0.0f;

				// 2 左下
				pVertex[nCurVertex].color	= 0xffffffff;
				pVertex[nCurVertex].vPos	= Vector3Df( -xr,  0.0f,      -zr  ) * pOII.m_vScale + pOII.m_vPosition;
				pVertex[nCurVertex].normal	= Vector3Df::UNIT_Y;
				pVertex[nCurVertex].tu		= 0.0f;
				pVertex[nCurVertex++].tv	= 1.0f;

				// 3 右下
				pVertex[nCurVertex].color	= 0xffffffff;
				pVertex[nCurVertex].vPos	= Vector3Df( xr,   0.0f,      zr    ) * pOII.m_vScale + pOII.m_vPosition;
				pVertex[nCurVertex].normal	= Vector3Df::UNIT_Y;
				pVertex[nCurVertex].tu		= 1.0f;
				pVertex[nCurVertex++].tv	= 1.0f;

				// 填充索引数据
				// 012
				pIndex[nCurIndex++] = nIndexID;
				pIndex[nCurIndex++] = nIndexID + 1;
				pIndex[nCurIndex++] = nIndexID + 2;

				// 213
				pIndex[nCurIndex++] = nIndexID + 2;
				pIndex[nCurIndex++] = nIndexID + 1;
				pIndex[nCurIndex++] = nIndexID + 3;

				nIndexID += 4;
			}
		}
	}

	//////////////////////////////////////////////////////////////////////////
	// calculate boundingbox
	m_boundingBox.Reset(0,0,0);
	for ( size_t i = 0; i < nCurVertex; ++i )
	{
		m_boundingBox.AddInternalPoint( pVertex[i].vPos );
	}
	//////////////////////////////////////////////////////////////////////////

	m_renderData.m_pVB->Unlock();
	m_renderData.m_pIB->Unlock();

	m_bHasGenVertexData = true;

	// 计算三角形数量
	m_renderData.m_nPrimCount	= m_nObjectCount * m_nQuadFaceCountPerObj * (m_nQuadFaceCountPerObj < 2 ? 4 : 2);//4;
	m_renderData.m_renderType	= RT_TRIANGLE_LIST;

	m_vtObjectOffsetInit.clear();
	m_vtObjectOffsetCur.clear();
	for ( size_t i = 0; i < m_nObjectCount; ++i )
	{
		float angle = AIR::RangeRandom(0,PI);
		float x = 0.02 * AIR::asm_cos( angle );
		float z = 0.02 * AIR::asm_sin( angle );
		m_vtObjectOffsetInit.push_back( Vector3Df(x,0,z) );
		m_vtObjectOffsetCur.push_back( Vector3Df::ZERO );
	}

	if ( m_pNode )
	{
		m_pNode->NeedUpdate();
		m_pNode->Update();
	}
}


bool PageStaticGeometory::GetIsCurPage( float fWidth, float fHeight, int nQuadFaceCountPerObj, ePlantFaceType ePFT, AIRString& szTexFileName )
{
	if ( !m_pMaterial )
	{
		return false;
	}
    /*
	if ( !m_pMaterial->GetCurrentTechnique()->GetPass(0)->GetTextureUnit(0) )
	{
		return false;
	}
	if ( m_pMaterial->GetCurrentTechnique()->GetPass(0)->GetTextureUnit(0)->GetTextureFileName() != szTexFileName )
	{
		return false;
	}
    */
	if ( fWidth != m_fObjectWidth ||
		 fHeight != m_fObjectHeight ||
		 m_nQuadFaceCountPerObj != nQuadFaceCountPerObj ||
		 m_PlantFaceType != ePFT )
	{
		return false;
	}
	return true;
}

bool PageStaticGeometory::SaveToXML( CMarkupSTL& xml )
{
	xml.AddElem( "PageStaticGeometory" );
	xml.AddAttrib( "Name", m_strName.c_str() );
	xml.AddAttrib( "MaxObjectCount", m_nMaxObjectCount );
	xml.AddAttrib( "CurObjectCount", m_nObjectCount );
	xml.AddAttrib( "MaxXOffset", GetString("%f",m_fXMaxOffset) );
	xml.AddAttrib( "MaxZOffset", GetString("%f",m_fZMaxOffset) );
	xml.AddAttrib( "AnimType", int(m_PsgAnimType) );
	xml.AddAttrib( "ObjectWidth", GetString("%f",m_fObjectWidth) );
	xml.AddAttrib( "ObjectHeight", GetString("%f",m_fObjectHeight) );
	xml.AddAttrib( "FaceType", int(m_PlantFaceType) );
	xml.AddAttrib( "QuadFaceCount", m_nQuadFaceCountPerObj );
	xml.AddAttrib( "PlantTexFile", m_szPlantTexFile.c_str() );
	xml.AddAttrib( "OffsetFreq", GetString("%f", m_fOffsetFreq) );

	xml.IntoElem();
	for ( LST_OBJECT_INIT_INFO::iterator it = m_vtObjectInitInfo.begin(); it != m_vtObjectInitInfo.end(); ++it )
	{
		xml.AddElem( "ObjectInitInfo" );
		xml.AddAttrib( "position", GetString("(%f,%f,%f)",(*it).m_vPosition.x, (*it).m_vPosition.y, (*it).m_vPosition.z) );
		xml.AddAttrib( "scale", GetString("(%f,%f,%f)",(*it).m_vScale.x, (*it).m_vScale.y, (*it).m_vScale.z) );
		xml.AddAttrib( "horzRotateAngle", GetString("%f", (*it).m_fAngle) );
		xml.AddAttrib( "baseWidth", GetString("%f", (*it).m_fBaseWidth) );
		xml.AddAttrib( "baseHeight", GetString("%f", (*it).m_fBaseHeight) );
	}
	xml.OutOfElem();
	return true;
}


bool PageStaticGeometory::SaveToBinFile( std::ofstream& fout )
{
	u32	dwInitInfoCount = m_vtObjectInitInfo.size();
	int		nPsgAnimType = (int)m_PsgAnimType;
	int		nPlantFaceType = (int)m_PlantFaceType;
	char	buf[256];

	m_szNodeName = m_pNode->GetName();
	snprintf( buf, 256, "%s", m_szNodeName.c_str() );
	fout.write( (char*)(buf),						sizeof(buf) );
	snprintf( buf, 256, "%s", m_strName.c_str() );
	fout.write( (char*)(buf),						sizeof(buf) );
	snprintf( buf, 256, "%s", m_szPlantTexFile.c_str() );
	fout.write( (char*)(buf),						sizeof(buf) );
	fout.write( (char*)(&m_vObjectInitCenter),		sizeof(Vector3Df) );
	fout.write( (char*)(&m_nMaxObjectCount),		sizeof(size_t) );
	fout.write( (char*)(&m_nObjectCount),			sizeof(size_t) );
	fout.write( (char*)(&m_fXMaxOffset),			sizeof(float) );
	fout.write( (char*)(&m_fZMaxOffset),			sizeof(float) );
	fout.write( (char*)(&nPsgAnimType),				sizeof(int) );
	fout.write( (char*)(&m_fObjectWidth),			sizeof(float) );
	fout.write( (char*)(&m_fObjectHeight),			sizeof(float) );
	fout.write( (char*)(&nPlantFaceType),			sizeof(int) );
	fout.write( (char*)(&m_nQuadFaceCountPerObj),	sizeof(size_t) );	
	fout.write( (char*)(&m_fOffsetFreq),			sizeof(float) );
	fout.write( (char*)(&dwInitInfoCount),			sizeof(u32) );
	fout.write( (char*)(&m_vtObjectInitInfo[0]),	sizeof(ObjectInitInfo) * m_vtObjectInitInfo.size() );

	//if ( m_renderData.m_pVB )
	//{
	//	u32 size = m_renderData.m_pVB->GetWholeSize();
	//	char* data = new char[size];
	//	m_renderData.m_pVB->ReadData( 0, size, data );
	//	fout.write( data,	size );
	//}

	//if ( m_renderData.m_pIB )
	//{
	//	u32 size = m_renderData.m_pIB->GetWholeSize();
	//	char* data = new char[size];
	//	m_renderData.m_pIB->ReadData( 0, size, data );
	//	fout.write( data,	size );
	//}

	fout.write( (char*)(&m_boundingBox.MinEdge), sizeof(Vector3Df) );
	fout.write( (char*)(&m_boundingBox.MaxEdge), sizeof(Vector3Df) );

	return true;
}


bool PageStaticGeometory::CreateFromBinData( u8** pData )
{
	u32	dwInitInfoCount;
	int		nPsgAnimType = 0;
	int		nPlantFaceType = 0;
	char	buf[256];

	memcpy( buf,						(*pData), sizeof(u8) * 256 );		(*pData) += sizeof(u8) * 256;
	m_szNodeName = buf;
	memcpy( buf,						(*pData), sizeof(u8) * 256 );		(*pData) += sizeof(u8) * 256;
	m_strName = buf;
	memcpy( buf,						(*pData), sizeof(u8) * 256 );		(*pData) += sizeof(u8) * 256;
	m_szPlantTexFile = buf;

	memcpy( &m_vObjectInitCenter,		(*pData), sizeof(Vector3Df) );		(*pData) += sizeof(Vector3Df);
	memcpy( &m_nMaxObjectCount,			(*pData), sizeof(size_t) );			(*pData) += sizeof(size_t);
	memcpy( &m_nObjectCount,			(*pData), sizeof(size_t) );			(*pData) += sizeof(size_t);
	memcpy( &m_fXMaxOffset,				(*pData), sizeof(float) );			(*pData) += sizeof(float);
	memcpy( &m_fZMaxOffset,				(*pData), sizeof(float) );			(*pData) += sizeof(float);
	memcpy( &nPsgAnimType,				(*pData), sizeof(int) );			(*pData) += sizeof(int);
	memcpy( &m_fObjectWidth,			(*pData), sizeof(float) );			(*pData) += sizeof(float);
	memcpy( &m_fObjectHeight,			(*pData), sizeof(float) );			(*pData) += sizeof(float);
	memcpy( &nPlantFaceType,			(*pData), sizeof(int) );			(*pData) += sizeof(int);
	memcpy( &m_nQuadFaceCountPerObj,	(*pData), sizeof(size_t) );			(*pData) += sizeof(size_t);
	memcpy( &m_fOffsetFreq,				(*pData), sizeof(float) );			(*pData) += sizeof(float);
	memcpy( &dwInitInfoCount,			(*pData), sizeof(u32) );			(*pData) += sizeof(u32);
	m_vtObjectInitInfo.resize( dwInitInfoCount );
	u32 dwDataSize = sizeof(ObjectInitInfo) * dwInitInfoCount;
	memcpy( &m_vtObjectInitInfo[0],		(*pData), dwDataSize );				(*pData) += dwDataSize;

	//--------------------------------------
	// add by hjj (2010-11-3)
	// load bounding box data
	memcpy( &m_boundingBox.MinEdge,		*pData,		sizeof(Vector3Df) );	(*pData) += sizeof(Vector3Df);
	memcpy( &m_boundingBox.MaxEdge,		*pData,		sizeof(Vector3Df) );	(*pData) += sizeof(Vector3Df);
	//--------------------------------------

	m_PsgAnimType = PSG_ANIM_TYPE(nPsgAnimType);
	m_PlantFaceType = ePlantFaceType(nPlantFaceType);
	// 计算每个物体的顶点数量
	m_nVertexCountPerObject = m_nQuadFaceCountPerObj * (m_nQuadFaceCountPerObj < 2 ? 6 : 4);//6;
	// 计算每个物体的索引数量
	m_nIndexCountPerObject	= m_nQuadFaceCountPerObj * (m_nQuadFaceCountPerObj < 2 ? 12 : 6);//12;
	m_bHasGenVertexData		= false;

	RebuildGeometryData();
	//BuildGeometryDataFromBin( pData );
	SetPsgAnimType( m_PsgAnimType );

    m_pPlantTexture = TEXTURE_MANAGER->CreateTextureFromFile(m_szPlantTexFile.c_str());

	if ( m_pMaterial->IsReady() == false )
	{
		m_pMaterial->Cache();
	}
	else
	{
		//m_pMaterial->SetTextureUnitTex( 0, m_szPlantTexFile.c_str() );
	}
	return true;
}


bool PageStaticGeometory::CreateFromXML( CMarkupSTL& xml )
{
	AIRString szValue("");
	m_strName = xml.GetAttrib( "Name" ).c_str();

	szValue = xml.GetAttrib( "MaxObjectCount" ).c_str();
	if ( szValue.length() )
	{
		m_nMaxObjectCount = atoi(szValue.c_str());
	}
	
	szValue = xml.GetAttrib( "CurObjectCount" ).c_str();
	if ( szValue.length() )
	{
		m_nObjectCount = atoi(szValue.c_str());
	}
	
	szValue = xml.GetAttrib( "MaxXOffset" ).c_str();
	if ( szValue.length() )
	{
		m_fXMaxOffset = atof(szValue.c_str());
	}
	
	szValue = xml.GetAttrib( "MaxZOffset" ).c_str();
	if ( szValue.length() )
	{
		m_fZMaxOffset = atof(szValue.c_str());
	}
	
	szValue = xml.GetAttrib( "AnimType" ).c_str();
	if ( szValue.length() )
	{
		m_PsgAnimType = (PSG_ANIM_TYPE)atoi(szValue.c_str());
	}
	
	szValue = xml.GetAttrib( "ObjectWidth" ).c_str();
	if ( szValue.length() )
	{
		m_fObjectWidth = atof(szValue.c_str());
	}
	
	szValue = xml.GetAttrib( "ObjectHeight" ).c_str();
	if ( szValue.length() )
	{
		m_fObjectHeight = atof(szValue.c_str());
	}
	
	szValue = xml.GetAttrib( "FaceType" ).c_str();
	if ( szValue.length() )
	{
		m_PlantFaceType = (ePlantFaceType)atoi(szValue.c_str());
	}
	
	szValue = xml.GetAttrib( "QuadFaceCount" ).c_str();
	if ( szValue.length() )
	{
		m_nQuadFaceCountPerObj = atoi(szValue.c_str());
	}
	
	m_szPlantTexFile = xml.GetAttrib( "PlantTexFile" ).c_str();

	szValue = xml.GetAttrib("OffsetFreq").c_str();
	if ( szValue.length() )
	{
		m_fOffsetFreq = atof( szValue.c_str() );
	}

	xml.IntoElem();
	
	m_vtObjectInitInfo.clear();
	while( xml.FindElem() )
	{
		ObjectInitInfo oii;
		szValue = xml.GetAttrib( "position" ).c_str();
		if ( szValue.length() )
		{
			sscanf( szValue.c_str(), "(%f,%f,%f)", &oii.m_vPosition.x, &oii.m_vPosition.y, &oii.m_vPosition.z );
		}		

		szValue = xml.GetAttrib( "scale" ).c_str();
		if ( szValue.length() )
		{
			sscanf( szValue.c_str(), "(%f,%f,%f)", &oii.m_vScale.x, &oii.m_vScale.y, &oii.m_vScale.z );
		}		

		szValue = xml.GetAttrib( "horzRotateAngle" ).c_str();
		if ( szValue.length() )
		{
			oii.m_fAngle = atof(szValue.c_str());
		}		

		szValue = xml.GetAttrib("baseWidth").c_str();
		if ( szValue.length() )
		{
			oii.m_fBaseWidth = atof( szValue.c_str() );
		}
		else
		{
			oii.m_fBaseWidth = m_fObjectWidth;
		}

		szValue = xml.GetAttrib("baseHeight").c_str();
		if ( szValue.length() )
		{
			oii.m_fBaseHeight = atof( szValue.c_str() );
		}
		else
		{
			oii.m_fBaseHeight = m_fObjectHeight;
		}

		m_vtObjectInitInfo.push_back( oii );
	}

	xml.OutOfElem();

	// 计算每个物体的顶点数量
	m_nVertexCountPerObject = m_nQuadFaceCountPerObj * (m_nQuadFaceCountPerObj < 2 ? 6 : 4);//6;
	// 计算每个物体的索引数量
	m_nIndexCountPerObject	= m_nQuadFaceCountPerObj * (m_nQuadFaceCountPerObj < 2 ? 12 : 6);//12;
	m_bHasGenVertexData		= false;

	RebuildGeometryData();
	SetPsgAnimType( m_PsgAnimType );
	//m_pMaterial->SetTextureUnitTex( 0, m_szPlantTexFile.c_str() );

	return true;
}


void PageStaticGeometory::SetObjectTexture( const AIRString& szTex )
{
	m_szPlantTexFile = szTex;
	if ( m_pMaterial )
	{
		//m_pMaterial->SetTextureUnitTex( 0, szTex.c_str() );
	}
}


void PageStaticGeometory::CopyObjectInitInfo( LST_OBJECT_INIT_INFO& vtDstObjectInitInfo, const Vector3Df& vNewCenterPosY )
{
	Vector3Df vOldCenterPos = m_pNode->GetDerivedPosition();
	LST_OBJECT_INIT_INFO::iterator it = m_vtObjectInitInfo.begin();
	for ( ; it != m_vtObjectInitInfo.end(); ++it )
	{
		(*it).m_vPosition.x += vOldCenterPos.x - vNewCenterPosY.x;
		(*it).m_vPosition.y += vOldCenterPos.y - vNewCenterPosY.y;
		(*it).m_vPosition.z += vOldCenterPos.z - vNewCenterPosY.z;
		vtDstObjectInitInfo.push_back( *it );
	}
}

/*
void PageStaticGeometory::RenderToShadowMap(CCamera* pCamera)
{
    CTechnique* pOldTechnique = m_pMaterial->GetCurrentTechnique();

    CTechnique* pShadowmapTechnique = m_pMaterial->GetTechnique(SHADOWMAP->GetName());
    m_pMaterial->SetCurrentTechnique(pShadowmapTechnique);
    Render(pCamera);

    m_pMaterial->SetCurrentTechnique(pOldTechnique);
}
*/

void PageStaticGeometory::_GetPrimitive()
{
    if (m_pPrimitive == NULL)
    {
        m_pPrimitive = new Primitive(Primitive::Primitive_TriangleList/*, m_renderData.m_pVB->GetVertexDeclaration()*/);
        m_pPrimitive->m_pSrcRenderable = this;
    }
    m_pPrimitive->m_pVertexBuffer = m_renderData.m_pVB;
    m_pPrimitive->m_pIndexBuffer = m_renderData.m_pIB;
}

const Matrix4f& PageStaticGeometory::GetWorldMatrix() const
{
    if ( m_PsgAnimType == ePsgAnimShearMatrix )
    {
        return m_matShear;
    }
    else
    {
        return Renderable::GetWorldMatrix();
    }
}
//
//PageStaticGeometory* PsgManager::CreatePageStaticGeometry( void )
//{
//	// 创建一个植被对象
//	
//	// 设置当前植被对象的名称
//	AIRString szPsgName = GetString("PSG_%d",m_nID++);
//	// 查找是否有相同名称的植被对象存在
//	while( GetPageStaticGeometry(szPsgName.c_str()) )
//	{
//		// 重新设置名称
//		szPsgName = GetString("PSG_%d",m_nID++);
//	}
//    PageStaticGeometory* pPSG = new PageStaticGeometory();
//	pPSG->SetName( szPsgName );
//	IMaterial* pMatPsg = MATERIAL_MANAGER->CreateMaterial("TemplateMaterial.xml/Plant");
//	pPSG->SetMaterial( pMatPsg );
//	pPSG->SetMaxObjectCount( 10000 );
//	m_lstPSG.push_back( pPSG );
//	return pPSG;
//}

bool PageStaticGeometory::Prepare()
{
    //return Renderable::Prepare();

	if (!m_bPrepared)
	{
		if (m_pMaterial)
		{
			if (!m_pMaterial->IsReady())
			{
				m_pMaterial->Cache();
			}
			else
			{
                

				//m_pMaterial->SetTextureUnitTex( 0, m_szPlantTexFile.c_str() );

				//m_bPrepared = m_pMaterial->IsReady();
			}

            m_bPrepared = m_pMaterial->IsReady();

            if (m_bPrepared)
            {
                if (m_pPlantTexture && m_pPlantTexture->IsLoaded())
                {
                    if (!m_pPlantTexture->IsReady())
                    {
                        m_pPlantTexture->Cache();
                    }

                    m_bPrepared = m_pPlantTexture->IsReady();
                }

                if (m_bPrepared)
                {
                    GPUVariant var;
                    var.SetTexture(m_pPlantTexture);
                    m_pMaterial->SetMaterialParam("samplerMesh", var);
                }
            }
            
		}
	}
	return m_bPrepared;
}
/*
void PageStaticGeometory::GenRenderToGBufferTechnique()
{
    m_pTechniqueRenderToGBuffer = m_pMaterial->GetTechnique("Plant_RenderToGBuffer");

    if (m_pTechniqueRenderToGBuffer == NULL)
    {
        CTechnique* pCurrentTechnique = m_pMaterial->GetCurrentTechnique();
        m_pTechniqueRenderToGBuffer = new CTechnique(m_pMaterial);
        m_pTechniqueRenderToGBuffer->SetName("Plant_RenderToGBuffer");
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

            AIRString vsShader = "VSPlantRenderToGBuffer";
            //AIRString psShader = "";

            IGPUProgram* pVertexShader = GPUPROGRAM_MANAGER->GetGPUProgram(vsShader); //pRenderer->CreateVertexShader(vsShader.c_str(), m_pTechniqueRenderToGBuffer->GetVSVersion().c_str());
            //pVertexShader->AddManualGPUDataParamSource(m_gpuDataParamSource);
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
void PageStaticGeometory::SetAcceptLighting(bool bAccept)
{
    m_bAcceptLighting = bAccept;
    //m_pMaterial->SetCurrentTechnique(TECHNIQUE_SELECTOR->SelectTechnique(this));
}

void PageStaticGeometory::ResetCurrentTechnique()
{
    //m_pMaterial->SetCurrentTechnique(TECHNIQUE_SELECTOR->SelectTechnique(this));
}



//
//void PsgManager::ClearPageStaticGeometry( PageStaticGeometory* pPsg )
//{
//	for ( LST_PSG::iterator it = m_lstPSG.begin(); it != m_lstPSG.end(); ++it )
//	{
//		if ( (*it) == pPsg )
//		{
//			pPsg->Clear();
//			SafeDelete( pPsg );
//			m_lstPSG.erase( it );
//			return;
//		}
//	}
//}
//
//PageStaticGeometory* PsgManager::GetPageStaticGeometry( const char* szName )
//{
//	for ( LST_PSG::iterator it = m_lstPSG.begin(); it != m_lstPSG.end(); ++it )
//	{
//		if ( (*it)->GetName() == AIRString( szName ) )
//		{
//			return (*it);
//		}
//	}
//	return NULL;
//}
//
//void PsgManager::ClearAllPageStaticGeometry( void )
//{
//	for ( LST_PSG::iterator it = m_lstPSG.begin(); it != m_lstPSG.end(); ++it )
//	{
//		PageStaticGeometory* pPsg = *it;
//		pPsg->Clear();
//		SafeDelete( pPsg );
//	}
//	m_lstPSG.clear();
//	m_nID = 0;
//}
//
//bool PsgManager::SaveToXML( const char* szFile )
//{
//	m_szFile = szFile;
//
//	CMarkupSTL xml;
//	xml.SetDoc("<?xml version=\"1.0\" encoding=\"gb2312\"?>\r\n");
//	xml.AddElem("PlantInfo");
//
//	xml.IntoElem();
//	xml.AddNode(CMarkupSTL::MNT_TEXT, "\r\n    ");
//
//	for ( LST_PSG::iterator it = m_lstPSG.begin(); it != m_lstPSG.end(); ++it )
//	{
//		PageStaticGeometory* pPsg = *it;
//		pPsg->SaveToXML( xml );
//	}
//
//	xml.OutOfElem();
//	
//	return xml.Save(szFile);
//}
//
//
//bool PsgManager::SaveToBinFile( const char* szFile )
//{
//	m_szFile = szFile;
//
//	std::locale loc1 = std::locale::global(std::locale(".936"));
//	std::ofstream fout;
//	fout.open( szFile, std::ios::out | std::ios::binary);
//	fout.flush();
//
//	// plant object count : u32
//	u32 dwPSGCount = m_lstPSG.size();
//	fout.write( (char*)(&dwPSGCount), sizeof(dwPSGCount) );
//
//	for ( LST_PSG::iterator it = m_lstPSG.begin(); it != m_lstPSG.end(); ++it )
//	{
//		PageStaticGeometory* pPsg = *it;
//		pPsg->SaveToBinFile( fout );	
//	}
//
//	fout.flush();
//	fout.close();
//	std::locale::global(std::locale(loc1));
//
//	return true;
//}
//
//
//bool PsgManager::CreateFromXML( CMarkupSTL& xml )
//{
//	PageStaticGeometory* pPSG = NULL;
//	while( xml.FindElem() )
//	{
//		pPSG = CreatePageStaticGeometry();
//		pPSG->CreateFromXML( xml );
//	}
//	return true;
//}
//
//bool PsgManager::LoadFromFile( const char* szFile )
//{
//	CDataStream* pStream = FILESYSTEM->GetFileStream( szFile );
//	if (pStream == NULL)
//	{
//		return false;
//	}
//	AIRString strDoc = pStream->GetAsString();
//	CMarkupSTL xml(strDoc.c_str());
//
//	//第一步，进入根结点
//	if (!xml.FindElem())
//	{
//		SafeDelete( pStream );
//		return false;
//	}
//
//	xml.IntoElem();
//
//	CreateFromXML( xml );
//
//	xml.OutOfElem();
//
//	SafeDelete( pStream );
//
//	m_szFile = szFile;
//
//	return true;
//}
//
//
//bool PsgManager::LoadFromBinFile( const char* szFile )
//{
//	CDataStream* pStream = FILESYSTEM->GetFileStream( szFile );
//	if (pStream == NULL)
//	{
//		return false;
//	}
//
//	CMemoryDataStream memDataStream( *pStream, true );
//	u8* pDataPtr = memDataStream.GetPtr();
//
//	u32 dwPsgCount = *(u32*)(pDataPtr);
//	pDataPtr += sizeof(u32);
//
//	PageStaticGeometory* pPSG = NULL;
//	for ( int i = 0; i < dwPsgCount; ++i )
//	{
//		pPSG = CreatePageStaticGeometry();
//		pPSG->CreateFromBinData( &pDataPtr );
//	}
//
//	SafeDelete( pStream );
//
//	m_szFile = szFile;
//
//	return true;
//}
//
//
//void PsgManager::SetShowBoundingBox( bool b )
//{
//	for ( LST_PSG::iterator it = m_lstPSG.begin(); it != m_lstPSG.end(); ++it )
//	{
//		PageStaticGeometory* pPsg = *it;
//		pPsg->SetShowBoundingBox( b );
//	}
//}
//
//
//void PsgManager::SetShowAllPSG( bool bShow )
//{
//	m_bShowAllPSG = bShow;
//	for ( LST_PSG::iterator it = m_lstPSG.begin(); it != m_lstPSG.end(); ++it )
//	{
//		PageStaticGeometory* pPsg = *it;
//		pPsg->SetVisable( bShow );
//	}
//}
//
//
//void PsgManager::SetParmToAllPlant( float fXMaxOffset, float fZMaxOffset, float fOffsetFreq )
//{
//	for ( LST_PSG::iterator it = m_lstPSG.begin(); it != m_lstPSG.end(); ++it )
//	{
//		PageStaticGeometory* pPsg = *it;
//		pPsg->SetPsgAnimType(ePsgAnimUseVS);
//		pPsg->SetMaxXOffset( fXMaxOffset );
//		pPsg->SetMaxZOffset( fZMaxOffset );
//		pPsg->SetOffsetFreq( fOffsetFreq );
//	}
//}
//
//void PsgManager::UpdateAllPageStaticGeometory( float fFrameTime )
//{
//	for ( LST_PSG::iterator it = m_lstPSG.begin(); it != m_lstPSG.end(); ++it )
//	{
//		PageStaticGeometory* pPsg = *it;
//		pPsg->Update( fFrameTime );
//	}
//}
////
////void PsgManager::GenerateOptimiseTrunkPSG( int nRowCount, int nColCount )
////{
////	m_bIsTrunkPSG = true;
////	//块的行数
////	m_nTrunkRow = nRowCount;
////	//块的列数
////	m_nTrunkCol = nColCount;
////	MAP_TRUNK_MATERIAL_PSG_LIST tmpTrunkPsgMap;
////	MAP_TRUNK_MATERIAL_PSG_LIST::iterator itTrunkPsgMap, itTrunkPsgMapEnd;
////	MAP_MATERIAL_PSG_LIST::iterator itMatPsgMap, itMatPsgMapEnd;
////	MAP_MATERIAL_PSG_LIST* pTmpMatPsgMap = NULL;
////	int nTrunkID;
////	//块的宽度
////	float trunkW = TERRAIN->GetCurHeightmap()->GetWidth() / nColCount;
////	//块的高度
////	float trunkH = TERRAIN->GetCurHeightmap()->GetWidth() / nRowCount;
////	float worldX = TERRAIN->GetCurHeightmap()->GetBoundingBox().MinEdge.x;
////	float worldZ = TERRAIN->GetCurHeightmap()->GetBoundingBox().MinEdge.z;
////	int nCurR, nCurC;
////	PageStaticGeometory *pPsg = NULL, *pTmpPsg = NULL;
////	ObjectInitInfo init_info;
////	LST_PSG* pTmpPsgList = NULL;
////
////	//遍历当前所有的PSG
////	for ( LST_PSG::iterator it = m_lstPSG.begin(); it != m_lstPSG.end(); ++it )
////	{
////		pPsg = *it;
////		Vector3Df vPsgNodePos = pPsg->GetNodeParent()->GetDerivedPosition();
////		Vector3Df vTrunkCenterPos;
////		VectorIterator<LST_OBJECT_INIT_INFO> itw_init_objs = pPsg->GetObjectInitInfo();
////		//遍历当前PSG的簇信息
////		while( itw_init_objs.IsEnd() == false )
////		{
////			init_info = itw_init_objs.GetNext();
////			init_info.normalize();
////			//计算当前簇所在的块
////			nCurC = int( init_info.m_vPosition.x + vPsgNodePos.x - worldX ) / trunkW;
////			nCurR = int( init_info.m_vPosition.z + vPsgNodePos.z - worldZ ) / trunkH;
////			AIR::clamp( nCurR, 0, m_nTrunkRow-1 );
////			AIR::clamp( nCurC, 0, m_nTrunkCol-1 );
////			nTrunkID = nCurR * nColCount + nCurC;
////			//计算块的中心位置(作为块的节点位置)
////			vTrunkCenterPos.x = (float(nCurC) + 0.5f) * trunkW;
////			vTrunkCenterPos.z = (float(nCurR) + 0.5f) * trunkH;
////			vTrunkCenterPos.x += worldX;
////			vTrunkCenterPos.z += worldZ;
////			vTrunkCenterPos.y = TERRAIN->GetCurHeightmap()->GetHeight( vTrunkCenterPos.x, vTrunkCenterPos.z );
////
////			//找出/创建这个块所容纳的所有植被层的容器
////			itTrunkPsgMap = tmpTrunkPsgMap.find( nTrunkID );
////			if( itTrunkPsgMap != tmpTrunkPsgMap.end() )
////			{
////				pTmpMatPsgMap = itTrunkPsgMap->second;
////			}
////			else
////			{
////				pTmpMatPsgMap = new MAP_MATERIAL_PSG_LIST;
////				tmpTrunkPsgMap.insert( std::make_pair( nTrunkID, pTmpMatPsgMap ) );
////			}
////
////			//根据材质找出/创建块中相同材质的PSG
////			itMatPsgMap = pTmpMatPsgMap->find( pPsg->GetPlantTexFile() );
////			if ( itMatPsgMap != pTmpMatPsgMap->end() )
////			{
////				pTmpPsg = itMatPsgMap->second;
////			}
////			else
////			{		
////				
////				// 设置当前植被对象的名称
////				AIRString szPsgName = GetString( "PSG_%d_%s", nTrunkID, pPsg->GetPlantTexFile().c_str() );
////                pTmpPsg = new PageStaticGeometory();
////				pTmpPsg->SetName( szPsgName );
////				IMaterial* pMatPsg = MATERIAL_MANAGER->CreateMaterial("TemplateMaterial.xml/Plant");
////				pTmpPsg->SetMaterial( pMatPsg );
////				pTmpPsg->SetObjectTexture( pPsg->GetPlantTexFile() );
////				pTmpPsg->SetPsgAnimType(ePsgAnimUseVS);
////				pTmpPsg->SetShowBoundingBox( pPsg->GetShowBoundingBox() );
////				pTmpPsg->SetMaxXOffset( pPsg->GetMaxXOffset() );
////				pTmpPsg->SetMaxZOffset( pPsg->GetMaxZOffset() );
////				pTmpPsg->SetOffsetFreq( pPsg->GetOffsetFreq() );
////				pTmpPsg->SetQuadFaceCountPerObject( 3 );
////				pTmpPsg->SetObjectInitCenter( vTrunkCenterPos );
////				pTmpMatPsgMap->insert( std::make_pair(pPsg->GetPlantTexFile(), pTmpPsg) );
////			}
////
////			init_info.m_vPosition += vPsgNodePos;
////			init_info.m_vPosition.y = TERRAIN->GetCurHeightmap()->GetHeight( init_info.m_vPosition.x, init_info.m_vPosition.z ) - vTrunkCenterPos.y;
////			init_info.m_vPosition.x -= vTrunkCenterPos.x;
////			init_info.m_vPosition.z -= vTrunkCenterPos.z;
////			pTmpPsg->AddCluster( init_info );
////		}
////	}
////	DestroyAllPSG();
////
////	itTrunkPsgMap = tmpTrunkPsgMap.begin();
////	itTrunkPsgMapEnd = tmpTrunkPsgMap.end();
////	CSceneNode* pNode = NULL;
////	for ( ; itTrunkPsgMap != itTrunkPsgMapEnd; ++itTrunkPsgMap )
////	{
////		pTmpMatPsgMap = itTrunkPsgMap->second;		
////		itMatPsgMap = pTmpMatPsgMap->begin();
////		itMatPsgMapEnd = pTmpMatPsgMap->end();
////		for ( ; itMatPsgMap != itMatPsgMapEnd; ++itMatPsgMap )
////		{
////			pTmpPsg = itMatPsgMap->second;
////			m_lstPSG.push_back( pTmpPsg );
////			pTmpPsg->CreateInternal();
////			if ( pNode == NULL )
////			{
////				AIRString szNodeName = GetString( "PSG_Node_Trunk_%d", itTrunkPsgMap->first );
////				pNode = ENGINE_INST->GetSceneMgr()->CreateSceneNode( szNodeName, true, pTmpPsg->GetObjectInitCenter() );
////			}
////			pNode->AttachRenderable( pTmpPsg );
////			pNode->NeedUpdate();
////			pNode->Update();
////		}
////		SafeDelete( pTmpMatPsgMap );
////		pNode = NULL;
////	}
////}
//
//
//void PsgManager::DestroyAllPSG()
//{
//	PageStaticGeometory* pPsg;
//	CSceneNode* pNode;
//	for ( LST_PSG::iterator it = m_lstPSG.begin(); it != m_lstPSG.end(); ++it )
//	{
//		pPsg = *it;
//		pNode = pPsg->GetNodeParent();
//		if ( pNode )
//		{
//			ListIterator<CSceneNode::RenderableList> map_itor_renderable = pNode->GetMapItorRenderable();
//			Renderable* pRenderable = NULL;
//			while( !map_itor_renderable.IsEnd() )
//			{
//				pRenderable = map_itor_renderable.GetNext();
//				if ( pRenderable->GetTypeString() == "pagestaticgeometry" )
//				{
//					pNode->DetachRenderable( pRenderable );
//				}
//			}
//
//			if ( pNode->GetAttachRenderablesNum() == 0 )
//			{
//				ENGINE_INST->GetSceneMgr()->DestroySceneNode( pNode );
//			}
//		}
//	}
//	
//	ClearAllPageStaticGeometry();
//}
//
//void PsgManager::ResetAllPlantsTechnique()
//{
//    for ( LST_PSG::iterator it = m_lstPSG.begin(); it != m_lstPSG.end(); ++it )
//    {
//        PageStaticGeometory* pPsg = *it;
//        pPsg->ResetCurrentTechnique();
//    }
//}
