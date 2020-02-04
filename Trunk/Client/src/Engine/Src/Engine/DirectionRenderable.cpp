#include "stdafx.h"
#include "DirectionRenderable.h"
#include "Engine.h"
#include "SceneNode.h"
#include "ISceneManager.h"
#include "Camera.h"
#include <math.h>


DirectionRenderable::DirectionRenderable() : m_fLength(50)//, m_bVisable(false)
, m_directionSelected(DS_NONE)
, m_transformSpace(CSceneNode::TS_PARENT)
, m_bGlobalDir( false )
{
    m_clrDirectionX = SColor(255, 255, 0, 0);
    m_clrDirectionY = SColor(255, 0, 255, 0);
    m_clrDirectionZ = SColor(255, 0, 0, 255);
    m_strName = "Direction";
    m_szType = "DirectionRenderable";

	m_nXAxisID = ENGINE_INST->GetFPSCounter()->AddTextInfo( "", 10, 10, SColor(255,255,0,0) );
	m_nYAxisID = ENGINE_INST->GetFPSCounter()->AddTextInfo( "", 10, 10, SColor(255,255,0,0) );
	m_nZAxisID = ENGINE_INST->GetFPSCounter()->AddTextInfo( "", 10, 10, SColor(255,255,0,0) );
}

DirectionRenderable::~DirectionRenderable()
{

}

void DirectionRenderable::SetAttachedNode(CSceneNode* pNode)
{
    Renderable::SetAttachedNode(pNode);
    CaculateDirectionEnds();
}

void DirectionRenderable::SetLineLength(float fLength)
{
    m_fLength = fLength;
}

void DirectionRenderable::Render(CCamera* pCamera)
{
	AIRString szX, szY, szZ;
    if (!m_bVisable)
    {
		Rectf rc;
		ENGINE_INST->GetFPSCounter()->ChangeTextInfo( m_nXAxisID, szX, rc);
		ENGINE_INST->GetFPSCounter()->ChangeTextInfo( m_nYAxisID, szY, rc);
		ENGINE_INST->GetFPSCounter()->ChangeTextInfo( m_nZAxisID, szZ, rc);
        return;
    }
	else
	{
		if ( m_pNode )
		{
			szX = "X";
			szY = "Y";
			szZ = "Z";
		}
	}
    IRenderer* pRenderer = ENGINE_INST->GetRenderer();

	//pRenderer->SetRenderState( D3DRS_ZWRITEENABLE, FALSE );
	//pRenderer->SetRenderState( D3DRS_ZENABLE, FALSE );
    pRenderer->SetZWriteEnable(false);
    pRenderer->SetDepthEnable(false);

	//如果有挂接在场景节点上，表明这个是场景中物体的坐标系
    if (m_pNode)
    {
		//计算场景节点在屏幕中的2D位置
        pCamera->Translate3Dto2D(m_pNode->GetDerivedPosition(), m_nCenterX, m_nCenterY);
		//计算坐标轴的端点
        CaculateDirectionEnds();

		Vector2Df vTmp;

		//计算X轴端点的2D位置
        pCamera->Translate3Dto2D(m_directionEndX, m_nDirectionXEndX, m_nDirectionXEndY);
		vTmp.Set( m_nDirectionXEndX - m_nCenterX, m_nDirectionXEndY - m_nCenterY );
		vTmp.Normalize();
		vTmp *= m_fLength;
		m_nDirectionXEndX = vTmp.x + m_nCenterX;
		m_nDirectionXEndY = vTmp.y + m_nCenterY;
        m_lineDirectionX.setLine(m_nCenterX, m_nCenterY, m_nDirectionXEndX, m_nDirectionXEndY);
		//画X轴
        pRenderer->Draw2DLine(m_nCenterX, m_nCenterY, m_nDirectionXEndX, m_nDirectionXEndY, 2.0f, m_clrDirectionX);

		//计算Y轴端点的2D位置
        pCamera->Translate3Dto2D(m_directionEndY, m_nDirectionYEndX, m_nDirectionYEndY);
		vTmp.Set( m_nDirectionYEndX - m_nCenterX, m_nDirectionYEndY - m_nCenterY );
		vTmp.Normalize();
		vTmp *= m_fLength;
		m_nDirectionYEndX = vTmp.x + m_nCenterX;
		m_nDirectionYEndY = vTmp.y + m_nCenterY;
        m_lineDirectionY.setLine(m_nCenterX, m_nCenterY, m_nDirectionYEndX, m_nDirectionYEndY);
		//画Y轴
        pRenderer->Draw2DLine(m_nCenterX, m_nCenterY, m_nDirectionYEndX, m_nDirectionYEndY, 2.0f, m_clrDirectionY);

		//计算Z轴端点的2D位置
        pCamera->Translate3Dto2D(m_directionEndZ, m_nDirectionZEndX, m_nDirectionZEndY);
		vTmp.Set( m_nDirectionZEndX - m_nCenterX, m_nDirectionZEndY - m_nCenterY );
		vTmp.Normalize();
		vTmp *= m_fLength;
		m_nDirectionZEndX = vTmp.x + m_nCenterX;
		m_nDirectionZEndY = vTmp.y + m_nCenterY;
        m_lineDirectionZ.setLine(m_nCenterX, m_nCenterY, m_nDirectionZEndX, m_nDirectionZEndY);
		//画Z轴
        pRenderer->Draw2DLine(m_nCenterX, m_nCenterY, m_nDirectionZEndX, m_nDirectionZEndY, 2.0f, m_clrDirectionZ);

		ENGINE_INST->GetFPSCounter()->AddTextInfo( szX, m_nDirectionXEndX + 5, m_nDirectionXEndY);
		ENGINE_INST->GetFPSCounter()->AddTextInfo( szY, m_nDirectionYEndX + 5, m_nDirectionYEndY);
		ENGINE_INST->GetFPSCounter()->AddTextInfo( szZ, m_nDirectionZEndX + 5, m_nDirectionZEndY);
    }
	else if ( m_bGlobalDir )
	{
		CaculateDirectionEnds();
		Vector3Df vStart = ENGINE_INST->GetSceneMgr()->GetCamera()->GetPosition() + ENGINE_INST->GetSceneMgr()->GetCamera()->GetLookDir();
		int nSX, nSY;
		pCamera->Translate3Dto2D(vStart, nSX, nSY);
		int deltax = m_nCenterX-nSX;
		int deltay = m_nCenterY-nSY;

		pCamera->Translate3Dto2D(m_directionEndX, m_nDirectionXEndX, m_nDirectionXEndY);
		m_lineDirectionX.setLine(m_nCenterX, m_nCenterY, m_nDirectionXEndX+deltax, m_nDirectionXEndY+deltay);
		pRenderer->Draw2DLine(m_nCenterX, m_nCenterY, m_nDirectionXEndX+deltax, m_nDirectionXEndY+deltay, 2.0f, m_clrDirectionX);

		pCamera->Translate3Dto2D(m_directionEndY, m_nDirectionYEndX, m_nDirectionYEndY);
		m_lineDirectionY.setLine(m_nCenterX, m_nCenterY, m_nDirectionYEndX+deltax, m_nDirectionYEndY+deltay);
		pRenderer->Draw2DLine(m_nCenterX, m_nCenterY, m_nDirectionYEndX+deltax, m_nDirectionYEndY+deltay, 2.0f, m_clrDirectionY);

		pCamera->Translate3Dto2D(m_directionEndZ, m_nDirectionZEndX, m_nDirectionZEndY);
		m_lineDirectionZ.setLine(m_nCenterX, m_nCenterY, m_nDirectionZEndX + deltax, m_nDirectionZEndY + deltay);
		pRenderer->Draw2DLine(m_nCenterX, m_nCenterY, m_nDirectionZEndX + deltax, m_nDirectionZEndY + deltay, 2.0f, m_clrDirectionZ);

		//ENGINE_INST->GetFPSCounter()->ChangeTextInfo( m_nXAxisID, szX, Rectf(m_nDirectionXEndX+deltax+5, m_nDirectionXEndY+deltay, m_nDirectionXEndX+deltax+25, m_nDirectionXEndY+deltay+20));
		//ENGINE_INST->GetFPSCounter()->ChangeTextInfo( m_nYAxisID, szY, Rectf(m_nDirectionYEndX+deltax+5, m_nDirectionYEndY+deltay, m_nDirectionYEndX+deltax+25, m_nDirectionYEndY+deltay+20));
		//ENGINE_INST->GetFPSCounter()->ChangeTextInfo( m_nZAxisID, szZ, Rectf(m_nDirectionZEndX+deltax+5, m_nDirectionZEndY+deltay, m_nDirectionZEndX+deltax+25, m_nDirectionZEndY+deltay+20));
        ENGINE_INST->GetFPSCounter()->AddTextInfo( "X", m_nDirectionXEndX + deltax + 5, m_nDirectionXEndY + deltay, SColor(255, 255, 0, 0));
        ENGINE_INST->GetFPSCounter()->AddTextInfo( "Y", m_nDirectionYEndX + deltax + 5, m_nDirectionYEndY + deltay, SColor(255, 0, 255, 0));
        ENGINE_INST->GetFPSCounter()->AddTextInfo( "Z", m_nDirectionZEndX + deltax + 5, m_nDirectionZEndY + deltay, SColor(255, 0, 0, 255));
	}
	else
	{
		//Rectf rc;
		//ENGINE_INST->GetFPSCounter()->ChangeTextInfo( m_nXAxisID, szX, rc);
		//ENGINE_INST->GetFPSCounter()->ChangeTextInfo( m_nYAxisID, szY, rc);
		//ENGINE_INST->GetFPSCounter()->ChangeTextInfo( m_nZAxisID, szZ, rc);
        ENGINE_INST->GetFPSCounter()->AddTextInfo( szX, m_nDirectionXEndX + 5, m_nDirectionXEndY);
        ENGINE_INST->GetFPSCounter()->AddTextInfo( szY, m_nDirectionYEndX + 5, m_nDirectionYEndY);
        ENGINE_INST->GetFPSCounter()->AddTextInfo( szZ, m_nDirectionZEndX + 5, m_nDirectionZEndY);
	}
    
	Renderable::AfterRender();
}

void DirectionRenderable::CaculateDirectionEnds()
{
	//如果有场景节点
    if (m_pNode)
    {
        switch (m_transformSpace)
        {
			//局部空间
        case CSceneNode::TS_LOCAL:
            {             
                //const Vector3Df& centerPos = m_pNode->GetPosition();
				//X轴的端点
                m_directionEndX = Vector3Df::UNIT_X * m_fLength;
				//Y轴的端点
                m_directionEndY = Vector3Df::UNIT_Y * m_fLength;
				//Z轴的端点
                m_directionEndZ = Vector3Df::UNIT_Z * m_fLength;
                m_directionEndX = m_pNode->ConvertLocalToWorldPosition(m_directionEndX);
                m_directionEndY = m_pNode->ConvertLocalToWorldPosition(m_directionEndY);
                m_directionEndZ = m_pNode->ConvertLocalToWorldPosition(m_directionEndZ);
            }
            break;
        case CSceneNode::TS_PARENT:
            {
                CSceneNode* pParent = m_pNode->GetParent();
                const Vector3Df& centerPos = m_pNode->GetPosition();
                if (pParent)
                {
                    //Vector3Df xDirection = Vector3Df::UNIT_X;
                    //Vector3Df yDirection = Vector3Df::UNIT_Y;
                    //Vector3Df zDirection = Vector3Df::UNIT_Z;
                    m_directionEndX = centerPos + Vector3Df::UNIT_X * m_fLength;
                    m_directionEndY = centerPos + Vector3Df::UNIT_Y * m_fLength;
                    m_directionEndZ = centerPos + Vector3Df::UNIT_Z * m_fLength;
                    m_directionEndX = pParent->ConvertLocalToWorldPosition(m_directionEndX);
                    m_directionEndY = pParent->ConvertLocalToWorldPosition(m_directionEndY);
                    m_directionEndZ = pParent->ConvertLocalToWorldPosition(m_directionEndZ);
                    
                }
                else
                {
                    m_directionEndX = centerPos + Vector3Df::UNIT_X * m_fLength;
                    m_directionEndY = centerPos + Vector3Df::UNIT_Y * m_fLength;
                    m_directionEndZ = centerPos + Vector3Df::UNIT_Z * m_fLength;
                }
            }
            break;
        case CSceneNode::TS_WORLD:
            const Vector3Df& centerPos = m_pNode->GetDerivedPosition();
            m_directionEndX = centerPos + Vector3Df::UNIT_X * m_fLength;
            m_directionEndY = centerPos + Vector3Df::UNIT_Y * m_fLength;
            m_directionEndZ = centerPos + Vector3Df::UNIT_Z * m_fLength;
            break;
        }
    }
	else if ( m_bGlobalDir )
	{
		Vector3Df vCamPos = ENGINE_INST->GetSceneMgr()->GetCamera()->GetPosition() + ENGINE_INST->GetSceneMgr()->GetCamera()->GetLookDir();
		m_directionEndX = vCamPos + Vector3Df::UNIT_X * m_fLength;
		m_directionEndY = vCamPos + Vector3Df::UNIT_Y * m_fLength;
		m_directionEndZ = vCamPos + Vector3Df::UNIT_Z * m_fLength;
	}
    
}

void DirectionRenderable::OnMouseMove(int x, int y)
{
    Vector2Df mousePoint(x, y);
	float d2X = fabsf(m_lineDirectionX.GetDistanceFromPoint(mousePoint));
    float d2Y = fabsf(m_lineDirectionY.GetDistanceFromPoint(mousePoint));
    float d2Z = fabsf(m_lineDirectionZ.GetDistanceFromPoint(mousePoint));
    if (d2X <= 4.0f && m_lineDirectionX.IsPointBetweenStartAndEnd(mousePoint))
    {
        SetDirectionSelected(DS_X);
    }
    else if (d2Y <= 4.0f && m_lineDirectionY.IsPointBetweenStartAndEnd(mousePoint))
    {
        SetDirectionSelected(DS_Y);
    }
    else if (d2Z <= 4.0f && m_lineDirectionZ.IsPointBetweenStartAndEnd(mousePoint))
    {
        SetDirectionSelected(DS_Z);
    }
    else
    {
        SetDirectionSelected(DS_NONE);
    }
}

void DirectionRenderable::OnLButtonDown(int x, int y)
{

}


void DirectionRenderable::SetDirectionSelected(DirectionSelection dirSelection)
{
    m_directionSelected = dirSelection;
    switch (m_directionSelected)
    {
    case DS_NONE:
        m_clrDirectionX = SColor(255, 255, 0, 0);
        m_clrDirectionY = SColor(255, 0, 255, 0);
        m_clrDirectionZ = SColor(255, 0, 0, 255);
        break;
    case DS_X:
        m_clrDirectionX = SColor(255, 255, 255, 0);
        m_clrDirectionY = SColor(255, 0, 255, 0);
        m_clrDirectionZ = SColor(255, 0, 0, 255);
        break;
    case DS_Y:
        m_clrDirectionX = SColor(255, 255, 0, 0);
        m_clrDirectionY = SColor(255, 255, 255, 0);
        m_clrDirectionZ = SColor(255, 0, 0, 255);
        break;
    case DS_Z:
        m_clrDirectionX = SColor(255, 255, 0, 0);
        m_clrDirectionY = SColor(255, 0, 255, 0);
        m_clrDirectionZ = SColor(255, 255, 255, 0);
        break;
    }
}

void DirectionRenderable::SetTransformSpace(CSceneNode::TransformSpace space)
{
    m_transformSpace = space;
}
