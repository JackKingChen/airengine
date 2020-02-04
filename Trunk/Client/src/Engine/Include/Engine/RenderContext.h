#pragma once 
#include "MemoryAlloter.h"
#include "ObjectPool.h"
#include "Matrix4.h"
#include "EngineDefination.h"
#include "FixedString.h"

class IVertexBuffer;
class IIndexBuffer;
class Primitive;
class IMaterial;
struct RenderPass;
class CSceneNode;
class Renderable;
class CMeshInstance;
class CCamera;
class Entity;
//class CVertexDeclaration;

using namespace AIR;

class EngineExport RenderContext //: public EngineAllocator(RenderContext)
{
	EngineAllocator(RenderContext)
public:
    RenderContext();
    virtual ~RenderContext();

    //IVertexBuffer* m_pVB;
    //IIndexBuffer*  m_pIB;

    RenderPass*    m_pRenderPass;
    Primitive*     m_pPrimitive;
    IMaterial*     m_pMaterial;
    CSceneNode*    m_pNode;

    Matrix4f m_matWorld;
    Matrix4f m_matWVP;
	Matrix4f m_matView;
	Matrix4f m_matProj;

	bool m_bSetViewMat;
	bool m_bSetProjMat;

    Matrix4f* m_pBoneMatrices;
    Renderable* m_pRenderable;
	//CVertexDeclaration* m_pDcl;
    u32       m_nBonesNum;

    f32      m_fDisToCamera;

    bool    IsTransparent() const;
protected:
private:
};

class EngineExport RenderContextQueue //: public EngineAllocator(RenderContextQueue)
{
	EngineAllocator(RenderContextQueue)
public:
    RenderContextQueue();
    virtual ~RenderContextQueue();

	void DrawPrimitive(Primitive* pPrim, IMaterial* pMaterial, const Matrix4f& matWorld, const Matrix4f& matView, const Matrix4f& matProj, eRenderFlow flow,
		const FixedString& strVertexDecl);

    void DrawRenderableToContext(Renderable* pRenderable, CCamera* pCamera, eRenderFlow flow);

	void DrawEntityToContext(Entity* pEntiy, CCamera* pCamera, eRenderFlow flow);

    void DrawSkinmeshToContext(CMeshInstance* pMesh, CCamera* pCamera, Matrix4f* pBoneMatrices, u32 nBonesNum, eRenderFlow flow);

    void DrawContexts();

    size_t GetRenderContextsNum() const
    {
        return m_vtContexts.size();
    }
protected:
private:
    ObjectPool<RenderContext> m_RenderContexPool;

    AIRVector<RenderContext*> m_vtContexts;
};
