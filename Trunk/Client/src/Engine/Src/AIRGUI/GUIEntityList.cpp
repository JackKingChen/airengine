//#include "StdAfx.h"
#include "GUIEntityList.h"
#include "GUISystem.h"
#include "GUISchemeManager.h"

CGUIEntityList::CGUIEntityList(void)
{
}

CGUIEntityList::~CGUIEntityList(void)
{
}

/*
void CGUIEntityList::AddEntity(CGUIElement* pEntity)
{
	pEntity->SetSchemeManager( dynamic_cast<CGUISchemeManager*>(this) );////////////////
    UILayerDefine viewState = pEntity->GetViewState();
    switch (viewState)
    {
    case UI_VIEW_NORMAL:
        m_lstElement.push_front(pEntity);
        break;
    case UI_ALWAYS_TOP:
        m_lstElementTop.push_front(pEntity);
        break;
    case UI_ALWAYS_BOTTOM:
        m_lstElementBottom.push_front(pEntity);
        break;
    }
    
}
*/

void CGUIEntityList::ClearEntity(CGUIElement* pEntity)
{
    for (GUIELEMENT_ITER it = m_lstElement.begin(); it != m_lstElement.end(); it++)
    {
        if (pEntity == *it)
        {
            pEntity->CleanUp();
            SafeDelete(pEntity);
            m_lstElement.erase(it);
            return;
        }
    }

    for (GUIELEMENT_ITER it = m_lstElementBottom.begin(); it != m_lstElementBottom.end(); it++)
    {
        if (pEntity == *it)
        {
            pEntity->CleanUp();
            SafeDelete(pEntity);
            m_lstElementBottom.erase(it);
            return;
        }
    }

    for (GUIELEMENT_ITER it = m_lstElementTop.begin(); it != m_lstElementTop.end(); it++)
    {
        if (pEntity == *it)
        {
            pEntity->CleanUp();
            SafeDelete(pEntity);
            m_lstElementTop.erase(it);
            return;
        }
    }

    
}

void CGUIEntityList::RemoveEntity(CGUIElement* pEntity, bool bNeedDelElem /* = true */)
{
    //通知父亲
    CGUIElement* pParent = pEntity->GetParent();
    if (pParent != NULL)
    {
        pParent->RemoveChild(pEntity);
        pEntity->CleanUp();
        SafeDelete(pEntity);
        return;
    }
    for (GUIELEMENT_ITER it = m_lstElement.begin(); it != m_lstElement.end(); it++)
    {
        if (pEntity == *it)
        {
            m_lstElement.erase(it);
			if ( bNeedDelElem )
			{
				pEntity->CleanUp();
				SafeDelete(pEntity);
			}
            return;
        }
    }

    for (GUIELEMENT_ITER it = m_lstElementBottom.begin(); it != m_lstElementBottom.end(); it++)
    {
        if (pEntity == *it)
        {
            m_lstElementBottom.erase(it);
			if ( bNeedDelElem )
			{
				pEntity->CleanUp();
				SafeDelete(pEntity);
			}
            return;
        }
    }

    for (GUIELEMENT_ITER it = m_lstElementTop.begin(); it != m_lstElementTop.end(); it++)
    {
        if (pEntity == *it)
        {
            m_lstElementTop.erase(it);
			if ( bNeedDelElem )
			{
				pEntity->CleanUp();
				SafeDelete(pEntity);
			}
            return;
        }
    }
}

void CGUIEntityList::RemoveEntity(const char* szName, bool bNeedDelElem /* = true */)
{
    CGUIElement* pEntity = NULL;
    for (GUIELEMENT_ITER it = m_lstElement.begin(); it != m_lstElement.end(); it++)
    {
        pEntity = *it;
        if (strcmp(pEntity->GetName().c_str(), szName) == 0)
        {
            m_lstElement.erase(it);
			if ( bNeedDelElem )
			{
				pEntity->CleanUp();
				SafeDelete(pEntity);
			}
            return;
        }
    }

    for (GUIELEMENT_ITER it = m_lstElementBottom.begin(); it != m_lstElementBottom.end(); it++)
    {
        pEntity = *it;
        if (strcmp(pEntity->GetName().c_str(), szName) == 0)
        {
            m_lstElementBottom.erase(it);
			if ( bNeedDelElem )
			{
				pEntity->CleanUp();
				SafeDelete(pEntity);
			}
            return;
        }
    }

    for (GUIELEMENT_ITER it = m_lstElementTop.begin(); it != m_lstElementTop.end(); it++)
    {
        pEntity = *it;
        if (strcmp(pEntity->GetName().c_str(), szName) == 0)
        {
            m_lstElementTop.erase(it);
			if ( bNeedDelElem )
			{
				pEntity->CleanUp();
				SafeDelete(pEntity);
			}
            return;
        }
    }

    CGUIElement* pElement = GUISYSTEM->FindCtrl(szName);
    CGUIElement* pParent = pElement->GetParent();
    pParent->RemoveChild(pElement);
    pElement->CleanUp();
    SafeDelete(pElement);
}

void CGUIEntityList::CleanUp()
{
    CGUIElement* pEntity = NULL;
    for (GUIELEMENT_ITER it = m_lstElement.begin(); it != m_lstElement.end(); it++)
    {
        pEntity = *it;
        pEntity->CleanUp();
        SafeDelete<CGUIElement*>(pEntity);
    }
    m_lstElement.clear();

    for (GUIELEMENT_ITER it = m_lstElementBottom.begin(); it != m_lstElementBottom.end(); it++)
    {
        pEntity = *it;
        pEntity->CleanUp();
        SafeDelete<CGUIElement*>(pEntity);
    }
    m_lstElementBottom.clear();

    for (GUIELEMENT_ITER it = m_lstElementTop.begin(); it != m_lstElementTop.end(); it++)
    {
        pEntity = *it;
        pEntity->CleanUp();
        SafeDelete<CGUIElement*>(pEntity);
    }
    m_lstElementTop.clear();
}

void CGUIEntityList::Render()
{
    CGUIElement* pEntity = NULL;

    
    //先渲染最底端的
    for (GUIELEMENT_ITER it = m_lstElementBottom.begin(); it != m_lstElementBottom.end(); ++it)
    {
        pEntity = *it;        
        pEntity->Render();
    }

    //渲染正常的
    for (GUIELEMENT_ITER it = m_lstElement.begin(); it != m_lstElement.end(); ++it)
    {
        pEntity = *it;
        pEntity->Render();
    }

    //渲染最上层的
    for (GUIELEMENT_ITER it = m_lstElementTop.begin(); it != m_lstElementTop.end(); ++it)
    {
        pEntity = *it;
        pEntity->Render();
    }
}

void CGUIEntityList::Update()
{
    //float fMaxDepth = GUISYSTEM->GetMaxDepth();
    //float fMinDepth = GUISYSTEM->GetMinDepth();
    //float fDepthDelta = (fMaxDepth - fMinDepth) / (m_lstElement.size() + 1);
    //float fDepth = fMaxDepth - fDepthDelta;
    CGUIElement* pEntity = NULL;
    for (GUIELEMENT_ITER it = m_lstElement.begin(); it != m_lstElement.end(); ++it)
    {
        pEntity = *it;
        if (pEntity->IsVisable())
        {
            //pEntity->SetDepth(fDepth);
            //fDepth -= fDepthDelta;
            pEntity->Update();
        }
    }
  
    for (GUIELEMENT_ITER it = m_lstElementBottom.begin(); it != m_lstElementBottom.end(); ++it)
    {
        pEntity = *it;
        if (pEntity->IsVisable())
        {
            //pEntity->SetDepth(fMaxDepth);
            pEntity->Update();
        }
    }

    for (GUIELEMENT_ITER it = m_lstElementTop.begin(); it != m_lstElementTop.end(); ++it)
    {
        pEntity = *it;
        if (pEntity->IsVisable())
        {
            //pEntity->SetDepth(fMinDepth);
            pEntity->Update();
        }
    }

    CGUIEditCtrl* pEditCtrl = GUISYSTEM->GetEditCtrl();
    if (pEditCtrl)
    {
        pEditCtrl->Update();
    }
}


CGUIElement* CGUIEntityList::FindCtrl(const char* szCtrlName)
{
    CGUIElement* pEntity = NULL;
    CGUIElement* pFindCtrl = NULL;
    for (GUIELEMENT_ITER it = m_lstElement.begin(); it != m_lstElement.end(); ++it)
    {
        pEntity = *it;
        if (strcmp(pEntity->GetName().c_str(), szCtrlName) == 0)
        {
            return pEntity;
        }
        else
        {
            pFindCtrl = pEntity->FindChildElement(szCtrlName);
            if (pFindCtrl)
            {
                return pFindCtrl;
            }
        }
    }

    for (GUIELEMENT_ITER it = m_lstElementBottom.begin(); it != m_lstElementBottom.end(); ++it)
    {
        pEntity = *it;
        if (strcmp(pEntity->GetName().c_str(), szCtrlName) == 0)
        {
            return pEntity;
        }
        else
        {
            pFindCtrl = pEntity->FindChildElement(szCtrlName);
            if (pFindCtrl)
            {
                return pFindCtrl;
            }
        }
    }

    for (GUIELEMENT_ITER it = m_lstElementTop.begin(); it != m_lstElementTop.end(); ++it)
    {
        pEntity = *it;
        if (strcmp(pEntity->GetName().c_str(), szCtrlName) == 0)
        {
            return pEntity;
        }
        else
        {
            pFindCtrl = pEntity->FindChildElement(szCtrlName);
            if (pFindCtrl)
            {
                return pFindCtrl;
            }
        }
    }

    return NULL;
}


bool CGUIEntityList::SaveToXML(CMarkupSTL& xml)
{
	GUIELEMENT_ITER itor = m_lstElement.begin();
	for ( ; itor != m_lstElement.end(); ++itor )
	{
		(*itor)->SaveToXML( xml );
	}

	itor = m_lstElementBottom.begin();
	for ( ; itor != m_lstElementBottom.end(); ++itor )
	{
		(*itor)->SaveToXML( xml );
	}

	itor = m_lstElementTop.begin();
	for ( ; itor != m_lstElementTop.end(); ++itor )
	{
		(*itor)->SaveToXML( xml );
	}

	return true;
}

CGUIElementIterator CGUIEntityList::GetTopGUIElementIterator(void)
{
    return CGUIElementIterator(m_lstElementTop.begin(), m_lstElementTop.end());
}

CGUIElementIterator CGUIEntityList::GetNormalGUIElementIterator(void)
{
    return CGUIElementIterator(m_lstElement.begin(), m_lstElement.end());
}

CGUIElementIterator CGUIEntityList::GetBottomGUIElementIterator(void)
{
    return CGUIElementIterator(m_lstElementBottom.begin(), m_lstElementBottom.end());
}

void CGUIEntityList::CaculateEntityAlignPosition()
{
    CGUIElement* pEntity = NULL;


    //先渲染最底端的
    for (GUIELEMENT_ITER it = m_lstElementBottom.begin(); it != m_lstElementBottom.end(); ++it)
    {
        pEntity = *it;        
        pEntity->CaculateAlignPosition();
    }

    //渲染正常的
    for (GUIELEMENT_ITER it = m_lstElement.begin(); it != m_lstElement.end(); ++it)
    {
        pEntity = *it;
        pEntity->CaculateAlignPosition();
    }

    //渲染最上层的
    for (GUIELEMENT_ITER it = m_lstElementTop.begin(); it != m_lstElementTop.end(); ++it)
    {
        pEntity = *it;
        pEntity->CaculateAlignPosition();
    }
}
