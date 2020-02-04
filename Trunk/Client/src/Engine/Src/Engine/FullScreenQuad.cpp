#include "stdafx.h"
#include "FullScreenQuad.h"
#include "Primitive.h"
#include "Engine.h"
#include "VertexDeclarationManager.h"

FullScreenQuad::FullScreenQuad() //: m_pQuadVertexDecl(NULL)
{
    //左上角
    m_fullScreenQuad[0].x = -1.0f;
    m_fullScreenQuad[0].y = 1.0f;
    m_fullScreenQuad[0].z = 0;
    m_fullScreenQuad[0].fu = 0;
    m_fullScreenQuad[0].fv = 0;

    //右上角
    m_fullScreenQuad[1].x = 1.0f;
    m_fullScreenQuad[1].y = 1.0f;
    m_fullScreenQuad[1].z = 0;
    m_fullScreenQuad[1].fu = 1.0f;
    m_fullScreenQuad[1].fv = 0;

    //左下角
    m_fullScreenQuad[2].x = -1.0f;
    m_fullScreenQuad[2].y = -1.0f;
    m_fullScreenQuad[2].z = 0;
    m_fullScreenQuad[2].fu = 0.0f;
    m_fullScreenQuad[2].fv = 1.0f;

    //右下角
    m_fullScreenQuad[3].x = 1.0f;
    m_fullScreenQuad[3].y = -1.0f;
    m_fullScreenQuad[3].z = 0;
    m_fullScreenQuad[3].fu = 1.0f;
    m_fullScreenQuad[3].fv = 1.0f;
}

FullScreenQuad::~FullScreenQuad()
{

}

void FullScreenQuad::PreRender(CCamera* pCamera)
{
    int nWidth, nHeight;
    ENGINE_INST->GetBackBufferSize(nWidth, nHeight);
	f32 fPixelOffset = RENDERER->GetPixelOffset();
	f32 fHalfPixelWidth = fPixelOffset > 0.0f ? 1.0f / nWidth : 0;//0.5f; //不明白为什么这里的半个像素要1.0才行，在shader中要+回半个像素，不是减
	f32 fHalfPixelHeight = fPixelOffset > 0.0f ? -1.0f / nHeight : 0;//0.5f; // 
    //左上角
    m_fullScreenQuad[0].x = -1.0f - fHalfPixelWidth;
    m_fullScreenQuad[0].y = 1.0f - fHalfPixelHeight;

    //右上角
    m_fullScreenQuad[1].x = 1.0f - fHalfPixelWidth; //(float)nWidth - fHalfPixelWidth;
    m_fullScreenQuad[1].y = 1.0f - fHalfPixelHeight; //0.0f - fHalfPixelHeight;

    //左下角
    m_fullScreenQuad[2].x = -1.0f - fHalfPixelWidth; //0.0f - fHalfPixelWidth;
    m_fullScreenQuad[2].y = -1.0f - fHalfPixelHeight; //(float)nHeight - fHalfPixelHeight;


    //右下角
    m_fullScreenQuad[3].x = 1.0f - fHalfPixelWidth; //(float)nWidth - fHalfPixelWidth;
    m_fullScreenQuad[3].y = -1.0f - fHalfPixelHeight; //(float)nHeight - fHalfPixelHeight;

    //if (m_pQuadVertexDecl == NULL)
    //{
    //    m_pQuadVertexDecl = VERTEXDECL_MANAGER->CreateVertexDeclaration(E_VB_NOCOLOR);
    //}
}

void FullScreenQuad::_GetPrimitive()
{
    if (m_pPrimitive == NULL)
    {
        m_pPrimitive = new Primitive(Primitive::Primitive_TriangleStrip/*, m_pQuadVertexDecl*/);
        m_pPrimitive->m_pSrcRenderable = this;
        m_pPrimitive->m_pVertexData = &m_fullScreenQuad[0];
        m_pPrimitive->m_nVerticesStride = sizeof(VERTEX_NOCOLOR);
        m_pPrimitive->SetPrimitiveCount(2);
        m_pPrimitive->SetUseUserPrimitive(true);
		m_pPrimitive->SetVerticesNum(4);
    }
}
