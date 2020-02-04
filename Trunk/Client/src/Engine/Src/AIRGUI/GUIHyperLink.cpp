#include "GUIHyperLink.h"

CGUIHyperLink::CGUIHyperLink(void)
{
}

CGUIHyperLink::~CGUIHyperLink(void)
{
    //ListElement_Iter itEnd = m_lstElements.end();
    //for (ListElement_Iter it = m_lstElements.begin(); it != itEnd; ++it)
    //{
    //    (*it)->SetHyperLink(NULL);

    //}
}

void CGUIHyperLink::AddElement(CGUIElement* pElement)
{
    m_lstElements.push_back(pElement);
}

bool CGUIHyperLink::IsInRange(int x, int y)
{
    for (ListElement_Iter it = m_lstElements.begin(); it != m_lstElements.end(); ++it)
    {
        if ((*it)->IsInRect(x, y))
        {
            return true;
        }
    }

    return false;
}

bool CGUIHyperLink::OnLButtonDown(int x, int y)
{
    if (IsInRange(x, y))
    {
        return true;
    }

    return false;
}

bool CGUIHyperLink::OnRButtonDown(int x, int y)
{
    if (IsInRange(x, y))
    {
        return true;
    }

    return false;
}

const char* CGUIHyperLink::ToString()
{
    return m_szContext;
}
