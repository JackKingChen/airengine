/********************************************************************
	created:	2010/01/03
	created:	3:1:2010   10:39
	filename: 	d:\My Codes\client\Current\Engine\Engine\Include\GroundObject.h
	file path:	d:\My Codes\client\Current\Engine\Engine\Include
	file base:	GroundObject
	file ext:	h
	author:		liangairan
	
	purpose:	地面对象，可以用线框表示
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

    //设置行和列，nRow和nCol最好是奇数
    virtual void SetSize(int nRow, int nCol, float fGridSize, bool bShowAxis = false );

	virtual void SetShowAxis( bool bShowAxis ) { m_bShowAxis = bShowAxis; }

	virtual void FillRenderContext(CCamera* pCamera, RenderContextQueue* pQueue, eRenderFlow flow);
protected:
    virtual void PreRender(CCamera* pCamera);

    //virtual void ClearRenderableImp();

    virtual void _GetPrimitive();
private:
    //IVertexBuffer*  m_pVertexBuffer;

    int             m_nRow;    //行和列的顶点数
    int             m_nCol;
    //bool            m_bVBCreate;   //是否已经创建了VB
	f32 m_fGridSize;

	bool			m_bShowAxis;
};

