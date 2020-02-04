/********************************************************************
	created:	2010/01/22
	created:	22:1:2010   10:55
	filename: 	d:\My Codes\client\Current\Engine\Engine\Include\BoxRenderable.h
	file path:	d:\My Codes\client\Current\Engine\Engine\Include
	file base:	BoxRenderable
	file ext:	h
	author:		liangairan
	
	purpose:	渲染box的类，默认是与XYZ轴平衡的
*********************************************************************/


#include "LineObject.h"
#include "VertexDef.h"

class EngineExport CBoxRenderable : public LineObject
{
public:
    CBoxRenderable();
    virtual ~CBoxRenderable();

    //virtual void Render(CCamera* pCamera);

    virtual void SetSize(const Vector3Df& size);

    virtual void SetPosition(const Vector3Df& position);
protected:
    // 渲染前要做的事
    virtual void PreRender(CCamera* pCamera);

    //生成boundingbox
    virtual void GenBoundingBox();

    //生成VertexBuffer
    virtual void GenVertexBuffer();

    virtual void ClearRenderableImp();

    virtual void GenLine(LPVERTEX_NOTEX* pVertex, Vector3Df& pos1, Vector3Df& pos2);

    virtual void _GetPrimitive();

    Vector3Df m_size;
    Vector3Df m_position;

    IVertexBuffer* m_pVertexBuffer;
    bool      m_bVBCreated;
};