#include "stdafx.h"
#include "GroundObject.h"
#include "Engine.h"
#include "VertexDef.h"
#include "Primitive.h"
//#include "RenderQueueGroup.h"
#include "SceneDebug.h"

CGroundObject::CGroundObject() : //m_pVertexBuffer(NULL), 
m_nRow(0), m_nCol(0), m_fGridSize(0)//, m_bVBCreate(false)
{
    m_bShowAxis = false;
    m_szType = "LineObject";
    //m_renderGroupType = eRGT_Line;
}

CGroundObject::~CGroundObject()
{
    Clear();
}

void CGroundObject::SetSize(int nRow, int nCol, float fGridSize, bool bShowAxis /* = false  */)
{
	m_bShowAxis = bShowAxis;
	m_fGridSize = fGridSize;
    //if (m_pVertexBuffer != NULL)
    //{
    //    m_pVertexBuffer->Release();
    //    SafeDelete(m_pVertexBuffer);
    //}

    m_nRow = nRow;
    if (m_nRow % 2 == 0)
    {
        m_nRow++;
    }
    m_nCol = nCol;
    if (m_nCol % 2 == 0)
    {
        m_nCol++;
    }

    IRenderer* pRenderer = ENGINE_INST->GetRenderer();
    //if (!m_bVBCreate)
    //{
    //    //m_pVertexBuffer = pRenderer->CreateVertexBuffer(E_VB_NOTEX, m_nRow * 2 + m_nCol * 2 + 3 * 2);
    //    m_pVertexBuffer = pRenderer->CreateVertexBuffer(m_nRow * 2 + m_nCol * 2 + 3 * 2, sizeof(VERTEX_NOTEX));
    //    m_bVBCreate = (m_pVertexBuffer != NULL);
    //}

    //LPVERTEX_NOTEX pVertex = (LPVERTEX_NOTEX)m_pVertexBuffer->Lock();
    //首先处理行
    for (int i = 0; i < m_nRow; i++)
    {
        //左边顶点
  //      pVertex->position.x = -fGridSize * float(m_nCol - 1) / 2.0;
  //      pVertex->position.y = 0.0f;
  //      pVertex->position.z = fGridSize * (float(m_nRow - 1) / 2.0 - i);
  //      pVertex->color = SColor(0xffffffff);
		//++pVertex;

  //      //右边顶点
  //      pVertex->position.x = fGridSize * float(m_nCol - 1) / 2.0;
  //      pVertex->position.y = 0.0f;
  //      pVertex->position.z = fGridSize * (float(m_nRow - 1) / 2.0 - i);
  //      pVertex->color = SColor(0xffffffff);
  //      ++pVertex;
		SCENE_DEBUGER->RenderLine(Vector3Df(-fGridSize * float(m_nCol - 1) / 2.0, 0, fGridSize * (float(m_nRow - 1) / 2.0 - i)), 
			Vector3Df(fGridSize * float(m_nCol - 1) / 2.0, 0, fGridSize * (float(m_nRow - 1) / 2.0 - i)), 0xffffffff, true);
    }

    //处理列
    for (int i = 0; i < m_nCol; ++i)
    {
        //上边顶点
		/*
        pVertex->position.x = fGridSize * (i - float(m_nCol - 1) / 2.0);
        pVertex->position.y = 0.0f;
        pVertex->position.z = fGridSize * float(m_nRow - 1) / 2.0;
        pVertex->color = SColor(0xffffffff);
        ++pVertex;

		//下边顶点
        pVertex->position.x = fGridSize * (i - float(m_nCol - 1) / 2.0);
        pVertex->position.y = 0.0f;
        pVertex->position.z = -fGridSize * float(m_nRow - 1) / 2.0;
        pVertex->color = SColor(0xffffffff);
        ++pVertex;
		*/
		SCENE_DEBUGER->RenderLine(Vector3Df(fGridSize * (i - float(m_nCol - 1) / 2.0), 0, fGridSize * float(m_nRow - 1) / 2.0), 
			Vector3Df(fGridSize * (i - float(m_nCol - 1) / 2.0), 0, -fGridSize * float(m_nRow - 1) / 2.0), 0xffffffff, true);
    }

	// x axis
	/*
	pVertex->position = Vector3Df::ZERO;
	pVertex->color = SColor(0xffff0000);
	++pVertex;
	pVertex->position = Vector3Df( fGridSize * float(m_nCol - 1), 0, 0 );
	pVertex->color = SColor(0xffff0000);
	++pVertex;

	// y axis
	pVertex->position = Vector3Df::ZERO;
	pVertex->color = SColor(0xff00ff00);
	++pVertex;
	pVertex->position = Vector3Df( 0, fGridSize * float(m_nCol - 1 + m_nRow - 1) / 2.0f,  0 );
	pVertex->color = SColor(0xff00ff00);
	++pVertex;

	// z axis
	pVertex->position = Vector3Df::ZERO;
	pVertex->color = SColor(0xff0000ff);
	++pVertex;
	pVertex->position = Vector3Df( 0, 0, fGridSize * float(m_nRow - 1) );
	pVertex->color = SColor(0xff0000ff);
	++pVertex;

    m_pVertexBuffer->Unlock();
	*/
	if (m_bShowAxis)
	{
		SCENE_DEBUGER->RenderLine(Vector3Df::ZERO, 
			Vector3Df( fGridSize * float(m_nCol - 1), 0, 0 ), SColor(0xffff0000), true);

		SCENE_DEBUGER->RenderLine(Vector3Df::ZERO, 
			Vector3Df( 0, fGridSize * float(m_nCol - 1 + m_nRow - 1) / 2.0f,  0 ), SColor(0xff00ff00), true);

		SCENE_DEBUGER->RenderLine(Vector3Df::ZERO, 
			Vector3Df( 0, 0, fGridSize * float(m_nRow - 1) ), SColor(0xff0000ff), true);
	}

    m_boundingBox.MaxEdge.x = fGridSize * float(m_nCol - 1) / 2.0;
    m_boundingBox.MaxEdge.y = 0.0f;
    m_boundingBox.MaxEdge.z = fGridSize * float(m_nRow - 1) / 2.0;

    m_boundingBox.MinEdge.x = -fGridSize * float(m_nCol - 1) / 2.0;
    m_boundingBox.MinEdge.y = 0.0f;
    m_boundingBox.MinEdge.z = -fGridSize * float(m_nRow - 1) / 2.0;
}

void CGroundObject::PreRender(CCamera* pCamera)
{

}

void CGroundObject::FillRenderContext(CCamera* pCamera, RenderContextQueue* pQueue, eRenderFlow flow)
{
	for (int i = 0; i < m_nRow; i++)
    {
        //左边顶点
  //      pVertex->position.x = -fGridSize * float(m_nCol - 1) / 2.0;
  //      pVertex->position.y = 0.0f;
  //      pVertex->position.z = fGridSize * (float(m_nRow - 1) / 2.0 - i);
  //      pVertex->color = SColor(0xffffffff);
		//++pVertex;

  //      //右边顶点
  //      pVertex->position.x = fGridSize * float(m_nCol - 1) / 2.0;
  //      pVertex->position.y = 0.0f;
  //      pVertex->position.z = fGridSize * (float(m_nRow - 1) / 2.0 - i);
  //      pVertex->color = SColor(0xffffffff);
  //      ++pVertex;
		SCENE_DEBUGER->RenderLine(Vector3Df(-m_fGridSize * float(m_nCol - 1) / 2.0, 0, m_fGridSize * (float(m_nRow - 1) / 2.0 - i)), 
			Vector3Df(m_fGridSize * float(m_nCol - 1) / 2.0, 0, m_fGridSize * (float(m_nRow - 1) / 2.0 - i)), 0xffffffff, true);
    }

    //处理列
    for (int i = 0; i < m_nCol; ++i)
    {
        //上边顶点

		SCENE_DEBUGER->RenderLine(Vector3Df(m_fGridSize * (i - float(m_nCol - 1) / 2.0), 0, m_fGridSize * float(m_nRow - 1) / 2.0), 
			Vector3Df(m_fGridSize * (i - float(m_nCol - 1) / 2.0), 0, -m_fGridSize * float(m_nRow - 1) / 2.0), 0xffffffff, true);
    }

	// x axis

	if (m_bShowAxis)
	{
		SCENE_DEBUGER->RenderLine(Vector3Df::ZERO, 
			Vector3Df( m_fGridSize * float(m_nCol - 1), 0, 0 ), SColor(0xffff0000), true);

		SCENE_DEBUGER->RenderLine(Vector3Df::ZERO, 
			Vector3Df( 0, m_fGridSize * float(m_nCol - 1 + m_nRow - 1) / 2.0f,  0 ), SColor(0xff00ff00), true);

		SCENE_DEBUGER->RenderLine(Vector3Df::ZERO, 
			Vector3Df( 0, 0, m_fGridSize * float(m_nRow - 1) ), SColor(0xff0000ff), true);
	}
}
/*
void CGroundObject::Render(CCamera* pCamera)
{
	if ( !m_bVisable )
	{
		return;
	}

    IRenderer* pRenderer = ENGINE_INST->GetRenderer();
    pRenderer->SetWorldTransform(&Matrix4f::IDENTITY);

    if (m_pMaterial)
    {
        //pRenderer->SetMaterial(m_pMaterial);
        CTechnique* pTechnique = m_pMaterial->GetCurrentTechnique();
        if (pTechnique)
        {
            size_t nPassNum = pTechnique->GetPassesNum();
            for (size_t i = 0; i < nPassNum; ++i)
            {
                CPass* pPass = pTechnique->GetPass(i);
                pPass->ApplyPass(pRenderer);

                if (m_pVertexBuffer)
                {
                    if (i == 0)
                    {
                        pRenderer->SetVertexBuffer(0, m_pVertexBuffer);
                    }
                    
					if ( m_bShowAxis )
					{
						pRenderer->DrawLineList(0, m_nRow + m_nCol + 3);
					}
					else
					{
						pRenderer->DrawLineList(0, m_nRow + m_nCol);
					}
                }
            }
        }
    }

    
    Renderable::AfterRender();
}
*/

//void CGroundObject::ClearRenderableImp()
//{
//    if (m_pVertexBuffer)
//    {
//        m_pVertexBuffer->Release();
//        SafeDelete(m_pVertexBuffer);
//    }
//}

void CGroundObject::_GetPrimitive()
{
    if (m_pPrimitive == NULL)
    {
        m_pPrimitive = new Primitive(Primitive::Primitive_LineList/*, m_pVertexBuffer->GetVertexDeclaration()*/);
        m_pPrimitive->m_pSrcRenderable = this;
    }

    //m_pPrimitive->m_pVertexBuffer = m_pVertexBuffer;
}
