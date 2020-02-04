/********************************************************************
	created:	2009/11/21
	created:	21:11:2009   15:15
	filename: 	RenderQueue.h
	author:		liangairan
	
	purpose:	渲染队列
*********************************************************************/
#pragma once 

#include "EngineConfig.h"
//#include "ShaderGroupList.h"
#include "RenderQueueGroup.h"
#include "MemoryAllocatorConfig.h"
#include "IteratorWrappers.h"

class EngineExport CRenderQueue
{
public:
    CRenderQueue();
    virtual ~CRenderQueue();

    //排序
    virtual void Sort();

    //渲染
    virtual void Render(CCamera* pCamera);

    //添加渲染物
    virtual void AddRenderable(Renderable* pRenderable);

    //清空渲染队列
    virtual void Clear();

    virtual void RenderSolidObjects(CCamera* pCamera);

    virtual void RenderAlphaObjects(CCamera* pCamera);

    //获得渲染队列中所有物体的个数
    int GetRenderablesNum() const;

    //该方法可以让用户自定义自己的渲染组
    void SetRenderGroup(RenderGroup_Type type, RenderQueueGroup* pGroup);

    //获得对应的group
    RenderQueueGroup* GetRenderQueueGroup(RenderGroup_Type type);

    size_t GetRenderQueueGroupsNum() const
    {
        return m_vtRenderQueueGroup.size();
    }

	void SetIs2DQueue( bool b )
	{
		m_bIs2DQueue = b;
	}

	VectorIterator< AIRVector<RenderQueueGroup*> > GetRenderQueueGroupItorObject( void )
	{
		return VectorIterator< AIRVector<RenderQueueGroup*> >(m_vtRenderQueueGroup);
	}



protected:

    RenderQueueGroup* CreateRenderQueueGroup(RenderGroup_Type type);

    //CShaderGroupList m_lstSolidObjects;            //不透明物体
    Renderable*     m_pTerrain;                   //地形
    //AIRVector<Renderable*>  m_vtAlphaObjects;   //透明物体

    AIRVector<RenderQueueGroup*>    m_vtRenderQueueGroup;   //渲染组

    AIRList<Renderable*>    m_lstLineObjects;    //画线的所有物体
    typedef AIRList<Renderable*>::iterator LineObjects_Iter;

	bool m_bIs2DQueue;

    IMaterial* m_pLineObjectMtl;    //lineobject的材质
private:
};