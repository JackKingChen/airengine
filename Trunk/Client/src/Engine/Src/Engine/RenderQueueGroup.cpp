#include "stdafx.h"
#include "RenderQueueGroup.h"
#include "Engine.h"
#include "ISceneManager.h"
#include "TimeBaseProfile.h"
#include "SkyDome.h"
//#include "Skybox.h"
#include "CloudsManager.h"

bool CompareDescRenderable(Renderable* pObj1, Renderable* pObj2)
{
    CCamera* pCamera = ENGINE_INST->GetSceneMgr()->GetCamera();

    f32 fDis1 = pCamera->GetDistanceFrom(pObj1->GetCenter());
    f32 fDis2 = pCamera->GetDistanceFrom(pObj2->GetCenter());
    return fDis1 < fDis2;
}

bool CompareAescRenderable(Renderable* pObj1, Renderable* pObj2)
{
    CCamera* pCamera = ENGINE_INST->GetSceneMgr()->GetCamera();

    f32 fDis1 = pCamera->GetDistanceFrom(pObj1->GetCenter());
    f32 fDis2 = pCamera->GetDistanceFrom(pObj2->GetCenter());
    return fDis1 > fDis2;
}

bool IsSky( Renderable* pObj )
{
	return (pObj->GetTypeString() == "skybox" || pObj->GetTypeString() == "SkyDome");
}

bool CompareSkyRenderable(Renderable* pObj1, Renderable* pObj2 )
{
	bool obj1Sky = IsSky(pObj1);
	bool obj2Sky = IsSky(pObj2);

	if ( obj1Sky )
	{
		return true;
	}
	else if ( !obj1Sky && obj2Sky )
	{
		return false;
	}
	else if ( !obj1Sky && !obj2Sky )
	{
		if ( pObj1->GetTypeString() == "CloudLayer" && pObj2->GetTypeString() == "CloudLayer" )
		{
			CloudLayer* pCL1 = (CloudLayer*)pObj1;
			CloudLayer* pCL2 = (CloudLayer*)pObj2;
			return pCL1->GetOptions().Height > pCL2->GetOptions().Height;
		}
		else
		{
			return CompareAescRenderable( pObj1, pObj2 );
		}
	}

    return false;
}

//////////////////////////////////////////////////////////////////////////
bool Compare2DDescRenderable( Renderable* pObj1, Renderable* pObj2 )
{
	f32 fDis1 = pObj1->GetCenter().z;
	f32 fDis2 = pObj2->GetCenter().z;
	return fDis1 < fDis2;
}

bool Compare2DAescRenderable( Renderable* pObj1, Renderable* pObj2 )
{
	f32 fDis1 = pObj1->GetCenter().z;
	f32 fDis2 = pObj2->GetCenter().z;
	return fDis1 > fDis2;
}
//////////////////////////////////////////////////////////////////////////

RenderQueueGroup::RenderQueueGroup(RenderQueueSortType sortType) : m_sortType(sortType)
{

}

RenderQueueGroup::~RenderQueueGroup()
{

}

void RenderQueueGroup::Clear()
{
    m_lstRenderable.clear();
}

void RenderQueueGroup::AddRenderable(Renderable* pRenderable)
{
    m_lstRenderable.push_back(pRenderable);
}

void RenderQueueGroup::Sort()
{
    //按由近到远排
    //CCamera* pCamera = ENGINE_INST->GetSceneMgr()->GetCamera();

    switch (m_sortType)
    {
    case eRQST_FARTONEAR:     //透明物
        m_lstRenderable.sort(CompareAescRenderable);
        break;
    case eRQST_NEARTOFAR:     //不透明物
        m_lstRenderable.sort(CompareDescRenderable);
        break;
	case eRQST_SKYOBJ:
		m_lstRenderable.sort(CompareSkyRenderable);
    }

}

void RenderQueueGroup::Sort( bool bSort2DObj )
{
	//按由近到远排
	if ( !bSort2DObj )
	{
		Sort();
	}
	else
	{
	    switch (m_sortType)
	    {
	    case eRQST_FARTONEAR:     //透明物
		    m_lstRenderable.sort(Compare2DAescRenderable);
		    break;
	    case eRQST_NEARTOFAR:     //不透明物
		    m_lstRenderable.sort(Compare2DDescRenderable);
		    break;
	    }
	}
}

RenderQueueGroup::SortByDistanceFunctor::SortByDistanceFunctor(const Vector3Df& pos) : sortPos(pos)
{

}

float RenderQueueGroup::SortByDistanceFunctor::operator()(Renderable* pRenderable) const
{
    return (sortPos - pRenderable->GetCenter()).GetLength();
}

void RenderQueueGroup::Render(CCamera* pCamera)
{
    //Sort();
    RenderableList_Iter itEnd = m_lstRenderable.end();
    for (RenderableList_Iter it = m_lstRenderable.begin(); it != itEnd; ++it)
    {
        (*it)->Render(pCamera);
    }
}
