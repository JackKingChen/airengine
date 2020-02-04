/********************************************************************
	created:	2010/05/21
	created:	21:5:2010   10:02
	filename: 	DirectionRenderable.h
	author:		liangairan
	
	purpose:	三个方向的2D渲染
*********************************************************************/


#pragma once

#include "Renderable.h"
#include "Color.h"
#include "Line2D.h"
#include "SceneNode.h"

enum DirectionSelection
{
    DS_NONE = 0,
    DS_X,
    DS_Y,
    DS_Z,
    DS_XY,
    DS_XZ,
    DS_YZ,
};

class EngineExport DirectionRenderable : public Renderable
{
public:
    DirectionRenderable();
    virtual ~DirectionRenderable();

    //void SetCenterPos(const Vector3Df& position);
    void SetAttachedNode(CSceneNode* pNode);

    void SetLineLength(float fLength);

    void Render(CCamera* pCamera);

    void OnMouseMove(int x, int y);

    void OnLButtonDown(int x, int y);

    DirectionSelection GetDirectionSelected() const
    {
        return m_directionSelected;
    }

    void SetDirectionSelected(DirectionSelection dirSelection);

    //设置变换空间
    void SetTransformSpace(CSceneNode::TransformSpace space);

	void Set2DCenterXY( int cx, int cy )
	{
		m_nCenterX = cx;
		m_nCenterY = cy;
	}

	void SetIsGlobalDir( bool b )
	{
		m_bGlobalDir = b;
	}

protected:
    //Vector3Df m_centerPos;

    float     m_fLength;    //长度

    //bool      m_bVisable;  //是否可见

    int  m_nCenterX;
    int  m_nCenterY;
    int  m_nDirectionXEndX;
    int  m_nDirectionXEndY;
    int  m_nDirectionYEndX;
    int  m_nDirectionYEndY;
    int  m_nDirectionZEndX;
    int  m_nDirectionZEndY;
    Line2Df    m_lineDirectionX;
    Line2Df    m_lineDirectionY;
    Line2Df    m_lineDirectionZ;
    Vector3Df  m_directionEndX;
    Vector3Df  m_directionEndY;
    Vector3Df  m_directionEndZ;

    SColor m_clrDirectionX;
    SColor m_clrDirectionY;
    SColor m_clrDirectionZ;

    DirectionSelection m_directionSelected;

	int						m_nXAxisID;
	int						m_nYAxisID;
	int						m_nZAxisID;
	bool					m_bGlobalDir;
private:

    void CaculateDirectionEnds();
    CSceneNode::TransformSpace    m_transformSpace;   //变换空间
};
