#include "stdafx.h"
#include "BillboardComposer.h"
#include "SceneNode.h"
#include "Engine.h"
#include "ISceneManager.h"
#include "BillboardRenderer.h"

BillboardComposer::BillboardComposer() : m_nRow(0), m_nCol(0)
{

}

BillboardComposer::~BillboardComposer()
{

}

void BillboardComposer::SetSize(int nRow, int nCol)
{
    m_nCol = nCol;
    m_nRow = nRow;

    m_vtBillboards.clear();
    m_vtBillboards.resize(nRow * nCol);
}

void BillboardComposer::PreRender(CCamera* pCamera)
{
    CaculatePosition();
    float fRightDis = m_fBBWidth / m_nCol;
    float fDownDis = m_fBBHeight / m_nRow;
    Vector3Df leftEdge;  //一行的最左边的顶点
    Vector3Df topEdge;   //一列的最上边顶点
    for (int nRow = 0; nRow < m_nRow; ++nRow)
    {
        leftEdge = m_TopLeft.position + m_vDir * nRow * fDownDis;
        for (int nCol = 0; nCol < m_nCol; ++nCol)
        {
            CBillboard* pBillboard = m_vtBillboards[nRow * m_nCol + nCol];
            //Vector3Df pos = 
            pBillboard->SetTopLeftPosition(leftEdge + m_vRight * fRightDis * nCol);
            pBillboard->SetTopRightPosition(leftEdge + m_vRight * fRightDis * (nCol + 1));
            pBillboard->SetBottomLeftPosition(pBillboard->GetTopLeft().position - m_vDir * fDownDis);
            pBillboard->SetBottomRightPosition(pBillboard->GetBottomLeft().position + m_vRight * fRightDis);
            pBillboard->SetTransparent(true, m_TopLeft.color.a);
        }
    }
}

void BillboardComposer::Render(CCamera* pCamera)
{
    PreRender(pCamera);

    for (size_t i = 0; i < m_vtBillboards.size(); ++i)
    {
        BILLBOARD_RENDERER->Render(m_vtBillboards[i]);
    }

	Renderable::AfterRender();
}

void BillboardComposer::AddBillboard(int nRow, int nCol, CBillboard* pBillboard)
{
    m_vtBillboards[nRow * m_nCol + nCol] = pBillboard;
}

bool BillboardComposer::Prepare()
{
    if (!m_bPrepared)
    {
        bool bPrepared = true;
        for (size_t i = 0; i < m_vtBillboards.size(); ++i)
        {
            if (!m_vtBillboards[i]->Prepare())
            {
                bPrepared = false;
                break;
            }
        }

        m_bPrepared = bPrepared;
    }

    return m_bPrepared;
}

/*
void BillboardComposer::CaculatePosition()
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
    }

    m_tri1.Set(m_TopLeft.position, m_TopRight.position, m_BottomLeft.position);
    m_tri2.Set(m_TopRight.position, m_BottomRight.position, m_BottomLeft.position);
}
*/
