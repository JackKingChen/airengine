/********************************************************************
	created:	2010/06/12
	created:	12:6:2010   9:51
	filename: 	NormalSceneRenderProcess.h
	author:		liangairan
	
	purpose:	走一个正常的场景渲染流程，只是把场景渲染到rendertarget里
*********************************************************************/

#pragma once 

#include "RenderTarget.h"
#include "CustomRenderProcess.h"
#include "RenderContext.h"

class DepthStencil;

/*
class EngineExport NormalSceneRenderProcess : public RenderTarget
{
public:
    NormalSceneRenderProcess();
    virtual ~NormalSceneRenderProcess();

    // 初始化rtt
    virtual bool			Initialize(int nWidth, int nHeight);

protected:
    // 更新函数
    //virtual void			UpdateImp();
    virtual void RenderImpl();
};
*/
class EngineExport NormalSceneRenderProcess : public CustomRenderProcess
{
public:
	NormalSceneRenderProcess(eRenderFlow flow = FLOW_SCENE);

	virtual ~NormalSceneRenderProcess();

	void AttachRenderTarget(TextureRenderTarget* pRenderTarget)
	{
		m_pRenderTarget = pRenderTarget;
	}

    void AttachRenderWindow(RenderTarget* pRenderWindow)
    {
        m_pRenderWindow = pRenderWindow;
    }

	virtual void Render();

	void AddClipPlane(const Plane3Df& plane);

    void ClearClipPlanes()
    {
        m_vecClipPlanes.clear();
    }

    CCamera* GetCamera() 
    {
        return m_pCamera;
    }

    void SetCamera(CCamera* pCamera, bool bNeedDeleteCamera)
    {
        m_pCamera = pCamera;
        m_bNeedDeleteCamera = bNeedDeleteCamera;
    }

    TextureRenderTarget* GetRenderTarget()
    {
        return m_pRenderTarget;
    }

    Plane3Df GetClipPlane(size_t nIndex)
    {
        if (nIndex < m_vecClipPlanes.size())
        {
            return m_vecClipPlanes[nIndex];
        }

        return Plane3Df();
    }

    void SetDepthStencil(DepthStencil* pDepthStencil)
    {
        m_pDepthStencil = pDepthStencil;
    }

    RenderContextQueue* GetRenderQueue()
    {
        return &m_renderQueue;
    }

    size_t GetVisableObjectsNum() const
    {
        return m_lstVisableNodes.size();
    }

    ListIterator< AIRList<CSceneNode*> > GetListItorVisableNode( void )
    {
        return ListIterator< AIRList<CSceneNode*> >( m_lstVisableNodes );
    }
protected:
    void ComputeCasterAABB(AABBBox3Df& aabbCaster, const Matrix4f& matView);
protected:
    eRenderFlow m_RenderFlow;
//private:
    bool m_bNeedDeleteCamera;
	CCamera* m_pCamera;
	TextureRenderTarget* m_pRenderTarget;
    RenderTarget*        m_pRenderWindow;
	AIRVector< Plane3Df > m_vecClipPlanes;
    DepthStencil*        m_pDepthStencil;

    RenderContextQueue   m_renderQueue;

    AIRList<CSceneNode*>  m_lstVisableNodes;
};
