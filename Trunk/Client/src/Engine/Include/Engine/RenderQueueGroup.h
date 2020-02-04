#pragma once 

#include "Renderable.h"
#include "IteratorWrappers.h"

enum RenderQueueSortType
{
    eRQST_FARTONEAR,     //��Զ����
    eRQST_NEARTOFAR,     //�ɽ���Զ
    eRQST_NOTSORT,       //������
	eRQST_SKYOBJ,
};

enum RenderGroup_Type
{
    eRGT_Skybox = 0,      //��ն�
    eRGT_Solid,           //��͸����
    eRGT_Terrain,         //����
    eRGT_Transparent,     //͸������
    eRGT_Line,            //�߿�����
};

class EngineExport RenderQueueGroup
{
public:
    typedef std::list<Renderable*> RenderableList;
    typedef RenderableList::iterator RenderableList_Iter;
public:
    RenderQueueGroup(RenderQueueSortType sortType = eRQST_NOTSORT);
    virtual ~RenderQueueGroup();

    //�����Ⱦ��
    virtual void AddRenderable(Renderable* pRenderable);

    //����
    virtual void Sort();

	virtual void Sort( bool bSort2DObj );

    //�����Ⱦ��
    virtual void Clear();

    //��Ⱦ���Ժ��ø÷�������Ⱦ��ֱ���ڳ����а���Ⱦ��һ��һ��ͬ����Ⱦ
    virtual void Render(CCamera* pCamera);

    RenderableList* GetRenderableList()
    {
        return &m_lstRenderable;
    }

    virtual size_t GetRenderablesNum() const
    {
        return m_lstRenderable.size();
    }

	ListIterator<RenderableList> GetRenderableListItorObject( void )
	{
		return ListIterator<RenderableList>(m_lstRenderable);
	}

	inline void SetGroupType( RenderGroup_Type t )
	{
		m_groupType = t;
	}

	inline RenderGroup_Type GetGroupType()
	{
		return m_groupType;
	}

protected:
    //��Զ����
    void SortFromFarToNear();
    //�ɽ���Զ
    void SortFromNearToFar();

    
protected:
    RenderableList  m_lstRenderable;

    RenderQueueSortType m_sortType;

    RenderGroup_Type    m_groupType;    //������

    /** Sort by distance functor */
    struct SortByDistanceFunctor
    {
        /// Position to sort in
        Vector3Df sortPos;

        SortByDistanceFunctor(const Vector3Df& pos);
        float operator()(Renderable* pRenderable) const;
    };


private:
};
