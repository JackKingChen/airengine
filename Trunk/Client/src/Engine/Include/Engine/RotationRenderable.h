/********************************************************************
‰÷»æ–˝◊™«Ú
*********************************************************************/
#ifndef __RotationRenderable_H__
#define __RotationRenderable_H__

#define NUM_SEGMENT 64
#include "Camera.h"
#include "Engine.h"
#include "SceneNode.h"


enum RotationSelection
{
	RS_NONE = 0,
	RS_X,
	RS_Y,
	RS_Z
};

class EngineExport RotationRenderable : public Renderable
{
public:
	RotationRenderable();

	virtual ~RotationRenderable();

	void Render(CCamera* pCamera);

	void RenderRotation(CCamera* pCamera);

	void OnMouseMove(int x, int y);

	void OnLButtonDown(int x, int y);

	void OnLButtonUp(int x, int y);


	void SetArcSelected(RotationSelection arcSelection);
	

	RotationSelection GetArcSelected();

	//»∆x÷·
	void AddUpAngleX(float fAngle);
	//»∆y÷·
	void AddUpAngleY(float fAngle);
	//»∆z÷·
	void AddUpAngleZ(float fAngle);
	//÷ÿ÷√
	void ResetAngles();

protected:
	
	virtual void PreRender(CCamera* pCamera);
	virtual void RebuildRenderData( void );
	virtual void _GetPrimitive();

private:

	void Caculate2DArc(CCamera* pCamera);
	void Get3DPoint(CCamera *pCamera, int x, int y, AIR::Vector3Df &intersection, const AIR::Plane3Df &plane);
	
	//»∆x ÷·
	AIRVector<Vector2Df>    m_vtXLines;
	//»∆y ÷·
	AIRVector<Vector2Df>    m_vtYLines;
	//»∆z ÷·
	AIRVector<Vector2Df>    m_vtZLines;
	//±≥æ∞‘≤
	AIRVector<Vector2Df>    m_vtBackLines;
	

	//x÷·µ„
	Vector2Df               m_v2XEnd;
	//y÷·µ„
	Vector2Df               m_v2YEnd;
	//z÷·µ„
	Vector2Df               m_v2ZEnd;
	
	//÷––ƒµ„
	Vector2Df               m_v2Center;
	
	//–≈œ¢Œª÷√
	Vector2Df               m_v2InfoPos;
	Vector2Df               m_v2SelectPoint;
	
	//Vector2Df               m_v2InitPoint;
	Vector2Df               m_v2BeginClickPoint;

	RotationSelection		m_nRSelection;

	float                   m_fRadius;
	SColor                  m_scXColor;
	SColor                  m_scYColor;
	SColor                  m_scZColor;
	SColor                  m_scSelectColor;
	SColor                  m_scBackColor;
	
	//Vector2Df               m_v2Test;
	//Vector2Df               m_v2Test2;
	Vector2Df               m_v2BeginLine;
	Vector2Df               m_v2EndLine;
	
	Vector3Df               m_v3RotationAngle;
	Vector2Df               mSinCos[NUM_SEGMENT + 1];
	CSceneNode::TransformSpace    m_transformSpace;   //±‰ªªø’º‰

	RHWVERTEXNOTEX          m_fanshapedVertex[NUM_SEGMENT + 3];
	int                     m_nNumT;
};
#endif