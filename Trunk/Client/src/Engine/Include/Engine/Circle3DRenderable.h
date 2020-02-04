/********************************************************************
	created:	2010/01/25
	created:	25:1:2010   10:14
	filename: 	d:\My Codes\client\Current\Engine\Engine\Include\CircleRenderable.h
	file path:	d:\My Codes\client\Current\Engine\Engine\Include
	file base:	CircleRenderable
	file ext:	h
	author:		liangairan
	
	purpose:	3ά��Բ��Ĭ������XZƽ���ϵ�Բ
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

    Vector3Df  m_position;   //��Բ���ĵ�λ��
    Vector3Df  m_direction;  //��ֱ��Բ�������

    //�Ƿ��Ѿ�������VB
    bool   m_bVBCreated;
};
