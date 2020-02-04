/********************************************************************
	created:	2009/11/21
	created:	21:11:2009   15:15
	filename: 	RenderQueue.h
	author:		liangairan
	
	purpose:	��Ⱦ����
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

    //����
    virtual void Sort();

    //��Ⱦ
    virtual void Render(CCamera* pCamera);

    //�����Ⱦ��
    virtual void AddRenderable(Renderable* pRenderable);

    //�����Ⱦ����
    virtual void Clear();

    virtual void RenderSolidObjects(CCamera* pCamera);

    virtual void RenderAlphaObjects(CCamera* pCamera);

    //�����Ⱦ��������������ĸ���
    int GetRenderablesNum() const;

    //�÷����������û��Զ����Լ�����Ⱦ��
    void SetRenderGroup(RenderGroup_Type type, RenderQueueGroup* pGroup);

    //��ö�Ӧ��group
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

    //CShaderGroupList m_lstSolidObjects;            //��͸������
    Renderable*     m_pTerrain;                   //����
    //AIRVector<Renderable*>  m_vtAlphaObjects;   //͸������

    AIRVector<RenderQueueGroup*>    m_vtRenderQueueGroup;   //��Ⱦ��

    AIRList<Renderable*>    m_lstLineObjects;    //���ߵ���������
    typedef AIRList<Renderable*>::iterator LineObjects_Iter;

	bool m_bIs2DQueue;

    IMaterial* m_pLineObjectMtl;    //lineobject�Ĳ���
private:
};