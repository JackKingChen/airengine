#include "GUIElementIterator.h"

CGUIElementIterator::CGUIElementIterator(std::list<CGUIElement*>::iterator start, std::list<CGUIElement*>::iterator end)
{
    m_Start = m_Pos = start;
    m_End = end;
}

CGUIElementIterator::~CGUIElementIterator(void)
{
}

bool CGUIElementIterator::IsEnd()
{
    return m_Pos == m_End;
}

CGUIElement* CGUIElementIterator::GetNext()
{
    return static_cast<CGUIElement*>(*m_Pos++);
}
