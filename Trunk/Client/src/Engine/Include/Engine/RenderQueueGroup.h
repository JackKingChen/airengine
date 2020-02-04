#pragma once 

#include "Renderable.h"
#include "IteratorWrappers.h"

enum RenderQueueSortType
{
    eRQST_FARTONEAR,     //由远到近
    eRQST_NEARTOFAR,     //由近到远
    eRQST_NOTSORT,       //不排序
	eRQST_SKYOBJ,
};

enum RenderGroup_Type
{
    eRGT_Skybox = 0,      //天空顶
    eRGT_Solid,           //非透明物
    eRGT_Terrain,         //地形
    eRGT_Transparent,     //透明物体
    eRGT_Line,            //线框物体
};

class EngineExport RenderQueueGroup
{
public:
    typedef std::list<Renderable*> RenderableList;
    typedef RenderableList::iterator RenderableList_Iter;
public:
    RenderQueueGroup(RenderQueueSortType sortType = eRQST_NOTSORT);
    virtual ~RenderQueueGroup();

    //添加渲染物
    virtual void AddRenderable(Renderable* pRenderable);

    //排序
    virtual void Sort();

	virtual void Sort( bool bSort2DObj );

    //清空渲染物
    virtual void Clear();

    //渲染，以后不用该方法来渲染，直接在场景中把渲染物一个一个同来渲染
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
    //由远到近
    void SortFromFarToNear();
    //由近到远
    void SortFromNearToFar();

    
protected:
    RenderableList  m_lstRenderable;

    RenderQueueSortType m_sortType;

    RenderGroup_Type    m_groupType;    //组类型

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
