#pragma once 
#include "CustomRenderProcess.h"
#include "RenderContext.h"
#include "IteratorWrappers.h"
#include "PostRenderProcess.h"

class DepthStencil;
class RenderTarget;
class CCamera;
class ISceneManager;
class NormalSceneRenderProcess;
class TextureRenderTarget;

//typedef bool (*ProcessRenderCallback)(WindowRenderProcess*);

class EngineExport WindowRenderProcess : public CustomRenderProcess
{
	EngineAllocator(WindowRenderProcess);
public:
	WindowRenderProcess();
	virtual ~WindowRenderProcess();

	void AttachRenderWindow(RenderTarget* pRenderWindow)
	{
		m_pRenderWindow = pRenderWindow;
	}

	virtual void Render();

	//void AddClipPlane(const Plane3Df& plane);

	//void ClearClipPlanes()
	//{
	//	m_vecClipPlanes.clear();
	//}

	CCamera* GetCamera() 
	{
		return m_pCamera;
	}

	void SetCamera(CCamera* pCamera, bool bNeedDeleteCamera)
	{
		m_pCamera = pCamera;
		m_bNeedDeleteCamera = bNeedDeleteCamera;
	}

	/*
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
	*/

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

	void SetSceneMgr(ISceneManager* pSceneMgr)
	{
		m_pSceneMgr = pSceneMgr;
	}
	//void SetRenderFunc(ProcessRenderCallback pRenderFunc)
	//{
	//	m_pRenderFunc = pRenderFunc;
	//}
	static WindowRenderProcess* CreateWindowRenderProcess(const char* szName);

	void SetPostProcessEnable(bool bEnable)
	{
		m_bPostProcessEnable = bEnable;
	}

	bool IsPostProcessEnable() const
	{
		return m_bPostProcessEnable;
	}
	void DisableAllPostProcess();
	void EnablePostProcess(PostProcessType type, bool bEnable);
protected:
	void ComputeCasterAABB(AABBBox3Df& aabbCaster, const Matrix4f& matView);
protected:
	eRenderFlow m_RenderFlow;
	//private:
	bool m_bNeedDeleteCamera;
	CCamera* m_pCamera;
	//TextureRenderTarget* m_pRenderTarget;
	ISceneManager* m_pSceneMgr;
	RenderTarget*        m_pRenderWindow;
	AIRVector< Plane3Df > m_vecClipPlanes;
	DepthStencil*        m_pDepthStencil;

	RenderContextQueue   m_renderQueue;

	AIRList<CSceneNode*>  m_lstVisableNodes;

	//ProcessRenderCallback m_pRenderFunc;
	//下面是后处理有关的
	bool m_bPostProcessEnable;
	NormalSceneRenderProcess* m_pNormalSceneProcess;
	TextureRenderTarget* m_pRTTScene;

	PostRenderProcess* m_vtPostProcess[ePP_MAX];
	ScreenCompositor* m_pScreenCompositor;
};
