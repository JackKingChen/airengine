/********************************************************************
	created:	2010/01/25
	created:	25:1:2010   10:14
	filename: 	d:\My Codes\client\Current\Engine\Engine\Include\CircleRenderable.h
	file path:	d:\My Codes\client\Current\Engine\Engine\Include
	file base:	CircleRenderable
	file ext:	h
	author:		liangairan
	
	purpose:	3维椭圆，默认是在XZ平面上的圆
*********************************************************************/
#pragma once
#include "LineObject.h"
#include "IVertexBuffer.h"

class EngineExport CCircle3DRenderable : public LineObject
{
public:
    CCircle3DRenderable(float a, float b, const Vector3Df& position = Vector3Df::ZERO, const Vector3Df& direction = Vector3Df::UNIT_Y);
    virtual ~CCircle3DRenderable();

    //virtual void Render(CCamera* pCamera);

    virtual void SetA(float a);

    virtual void SetB(float b);

    virtual void SetSize(float a, float b);

    virtual void SetDirection(const Vector3Df& vDirection);

    virtual void SetPosition(const Vector3Df& vPosition);
protected:
    virtual void PreRender(CCamera* pCamera);
    virtual void ClearRenderableImp();

    virtual void CreateVertexBuffer();

    virtual void _GetPrimitive();
private:
    //IVertexBuffer*  m_pVertexBuffer;

    float  m_a;
    float  m_b;

    Vector3Df  m_position;   //椭圆中心的位置
    Vector3Df  m_direction;  //垂直于圆面的向量

    //是否已经创建了VB
    bool   m_bVBCreated;
};
