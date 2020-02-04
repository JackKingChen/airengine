/********************************************************************
	created:	2010/01/03
	created:	3:1:2010   10:39
	filename: 	d:\My Codes\client\Current\Engine\Engine\Include\GroundObject.h
	file path:	d:\My Codes\client\Current\Engine\Engine\Include
	file base:	GroundObject
	file ext:	h
	author:		liangairan
	
	purpose:	������󣬿������߿��ʾ
*********************************************************************/

#pragma once 

#include "Renderable.h"
//#include "IVertexBuffer.h"

class EngineExport CGroundObject : public Renderable
{
	EngineAllocator(CGroundObject);
public:
    CGroundObject();
    virtual ~CGroundObject();

    //virtual void Render(CCamera* pCamera);

    //�����к��У�nRow��nCol���������
    virtual void SetSize(int nRow, int nCol, float fGridSize, bool bShowAxis = false );

	virtual void SetShowAxis( bool bShowAxis ) { m_bShowAxis = bShowAxis; }

	virtual void FillRenderContext(CCamera* pCamera, RenderContextQueue* pQueue, eRenderFlow flow);
protected:
    virtual void PreRender(CCamera* pCamera);

    //virtual void ClearRenderableImp();

    virtual void _GetPrimitive();
private:
    //IVertexBuffer*  m_pVertexBuffer;

    int             m_nRow;    //�к��еĶ�����
    int             m_nCol;
    //bool            m_bVBCreate;   //�Ƿ��Ѿ�������VB
	f32 m_fGridSize;

	bool			m_bShowAxis;
};

