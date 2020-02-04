/********************************************************************
	created:	2010/05/29
	created:	29:5:2010   17:16
	filename: 	BoundingBoxEntity.h
	author:		liangairan
	
	purpose:	boundingbox的渲染类
*********************************************************************/

#pragma once 

#include "Renderable.h"

class DepthStencilStateObject;

class EngineExport BoundingBoxRenderData
{
public:

    ~BoundingBoxRenderData();

    static BoundingBoxRenderData* GetInstance()
    {
        static BoundingBoxRenderData instance;
        return &instance;
    }

    //清空数据
    void Clear();

    void Render(const AABBBox3Df& boundingBox);

	void RenderOcclusionBoundingBox( const AABBBox3Df& boundingBox, u32 color = 0xffffffff );
protected:
private:
    void CreateVertexBuffer();

    void RenderBoundingBox(const Vector3Df* pVertices);
	void RenderOcclusionBoundingBoxImpl( const Vector3Df* pVertices, u32 color );

    BoundingBoxRenderData();

    IVertexBuffer* m_pVB;
	IVertexBuffer* m_pOcclVB;

    Primitive* m_pPrimitive;
	Primitive* m_pOcclPtimitive;

	DepthStencilStateObject* m_DepthStencilSO;
};



class EngineExport BoundingBoxEntity : public Renderable
{
public:
    BoundingBoxEntity();
    virtual ~BoundingBoxEntity();

    virtual void Render(CCamera* pCamera);

    void SetWorldBoundingBox(const AABBBox3Df& boundingBox);
protected:
    virtual void PreRender(CCamera* pCamera) {}
private:
};

#define BoundingBoxInstance BoundingBoxRenderData::GetInstance()
