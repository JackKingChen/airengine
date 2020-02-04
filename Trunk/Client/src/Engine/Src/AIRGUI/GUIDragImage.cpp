#include "GUIDragImage.h"

CGUIDragImage::CGUIDragImage(void) : m_nType(0), m_pListItem(NULL)
{
    m_strType = "CGUIDragImage";
}

CGUIDragImage::~CGUIDragImage(void)
{
}

void CGUIDragImage::SetGUIListItem(CGUIListItem* pItem)
{
    m_pListItem = pItem;
}
