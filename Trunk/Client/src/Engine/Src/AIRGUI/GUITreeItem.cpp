#include "GUITreeItem.h"
#include "GUITreeCtrl.h"
#include "GUISystem.h"

CGUITreeItem::CGUITreeItem(void) : m_pBtnExpand(NULL), m_pStaticText(NULL), m_pSelectedItem(NULL), m_pOwner(NULL),
m_pParentItem(NULL),
m_bExpanded(false),
m_bSelected(false)
{
    m_strType = "CGUITreeItem";
}

CGUITreeItem::~CGUITreeItem(void)
{
}

void CGUITreeItem::CleanUp()
{
    if (m_pBtnExpand)
    {
        m_pBtnExpand->CleanUp();
        SafeDelete(m_pBtnExpand);
    }

    if (m_pStaticText)
    {
        m_pStaticText->CleanUp();
        SafeDelete(m_pStaticText);
    }

    TreeItemList_Iter itEnd = m_lstTreeItems.end();
    CGUITreeItem* pTreeItem = NULL;
    for (TreeItemList_Iter it = m_lstTreeItems.begin(); it != itEnd; ++it)
    {
        pTreeItem = *it;
        pTreeItem->CleanUp();
        SafeDelete(pTreeItem);
    }

    m_lstTreeItems.clear();
}

void CGUITreeItem::Move(int dx, int dy)
{
    m_renderQuad.m_rectScreen.Move(dx, dy);
    m_realRect.Move(dx, dy);
    m_pBtnExpand->Move(dx, dy);
    m_pStaticText->Move(dx, dy);

    TreeItemList_Iter itEnd = m_lstTreeItems.end();
    CGUITreeItem* pTreeItem = NULL;
    for (TreeItemList_Iter it = m_lstTreeItems.begin(); it != itEnd; ++it)
    {
        pTreeItem = *it;
        pTreeItem->Move(dx, dy);
    }
}

void CGUITreeItem::SetRect(const Rectf& rect)
{
    CGUIElement::SetRect(rect);
    if (m_pBtnExpand)
    {
        m_pBtnExpand->MoveTo(m_renderQuad.m_rectScreen.left, m_renderQuad.m_rectScreen.top);
    }
    if (m_pStaticText)
    {
        m_pStaticText->SetWidth(m_renderQuad.m_rectScreen.GetWidth() - m_pBtnExpand->GetWidth());
        m_pStaticText->SetHeight(m_renderQuad.m_rectScreen.GetHeight());
        m_pStaticText->MoveTo(m_pBtnExpand->GetRight(), m_renderQuad.m_rectScreen.top);
    }
}

void CGUITreeItem::SetRect(int x, int y, int nWidth, int nHeight)
{
    SetRect(Rectf(x, y, x + nWidth, y + nHeight));
}

bool CGUITreeItem::OnLButtonDownSelf(int x, int y)
{
    CGUIElement::OnLButtonDownSelf(x, y);
    if (m_pBtnExpand->IsVisable() && m_pBtnExpand->IsInRect(x, y))
    {
        m_pBtnExpand->OnLButtonDown(x, y);
        if (m_pBtnExpand->IsChecked())
        {
            Expand();
        }
        else
        {
            UnExpand();
        }
    }

    m_pOwner->SetSelectedItem(this);
    return true;
}

void CGUITreeItem::Expand()
{
    TreeItemList_Iter itEnd = m_lstTreeItems.end();
    CGUITreeItem* pTreeItem = NULL;
    for (TreeItemList_Iter it = m_lstTreeItems.begin(); it != itEnd; ++it)
    {
        pTreeItem = *it;
        pTreeItem->Show();
    }

    m_realRect.left = m_renderQuad.m_rectScreen.left;
    m_realRect.top = m_renderQuad.m_rectScreen.top;
    m_realRect.right = m_renderQuad.m_rectScreen.right;
    m_realRect.bottom = m_renderQuad.m_rectScreen.bottom + m_lstTreeItems.size() * m_renderQuad.m_rectScreen.GetHeight();

    //m_renderQuad.m_rectScreen = m_realRect;
    m_bExpanded = true;

    m_pOwner->CaculateItemsPosition();
}

void CGUITreeItem::UnExpand()
{
    TreeItemList_Iter itEnd = m_lstTreeItems.end();
    CGUITreeItem* pTreeItem = NULL;
    for (TreeItemList_Iter it = m_lstTreeItems.begin(); it != itEnd; ++it)
    {
        pTreeItem = *it;
        pTreeItem->Hide();
    }
    const TREEITEMINFO& itemInfo = m_pOwner->GetTreeItemInfo();
    //m_realRect.bottom = m_realRect.top + itemInfo.m_nItemHeight;
    m_realRect = m_renderQuad.m_rectScreen;

    m_bExpanded = false;

    m_pOwner->CaculateItemsPosition();
}

bool CGUITreeItem::CreateSelf(CGUIElement* pParent, CMarkupSTL& xml)
{
    return CGUIElement::CreateSelf(pParent, xml);
}

void CGUITreeItem::AddChildItem(const char* szText)
{
    CGUITreeItem* pItem = new CGUITreeItem;
    pItem->SetSchemeManager(m_pScheme);
    pItem->SetOwner(m_pOwner);
    pItem->CreateDefault();
    pItem->m_pParentItem = this;
    CGUITreeCtrl* pTreeCtrl = static_cast<CGUITreeCtrl*>(m_pParent);
    const TREEITEMINFO& itemInfo = pTreeCtrl->GetTreeItemInfo();
    pItem->SetExpandBtnImgSrcCheck(itemInfo.strExpandBtnCheckImgSrc);
    pItem->SetExpandBtnImgSrcUnCheck(itemInfo.strExpandBtnUnCheckImgSrc);
    if (m_lstTreeItems.size() == 0)
    {
        pItem->SetRect(m_pBtnExpand->GetRight(), m_renderQuad.m_rectScreen.bottom, m_renderQuad.m_rectScreen.right - m_pBtnExpand->GetRight(), m_renderQuad.m_rectScreen.GetHeight());
    }
    else
    {
        TreeItemList::reverse_iterator rit = m_lstTreeItems.rbegin();
        CGUITreeItem* pLastItem = *rit;
        pItem->SetRect(pLastItem->GetLeft(), pLastItem->GetBottom(), pLastItem->GetWidth(), pLastItem->GetHeight());
    }
    
    
    pItem->SetFont(itemInfo.pFont);
    pItem->SetText(szText);
    pItem->SetItemHeight(itemInfo.m_nItemHeight);
    pItem->SetRealRect(pItem->GetRenderRect());
    
    m_lstTreeItems.push_back(pItem);
}

void CGUITreeItem::AddChildItem(CGUITreeItem* pItem)
{
    pItem->m_pParentItem = this;
    pItem->SetOwner(m_pOwner);
    pItem->SetSchemeManager(m_pScheme);
    if (m_lstTreeItems.size() == 0)
    {
        pItem->SetRect(m_pBtnExpand->GetRight(), m_renderQuad.m_rectScreen.bottom, m_renderQuad.m_rectScreen.GetWidth() - m_pBtnExpand->GetWidth(), m_renderQuad.m_rectScreen.GetHeight());
    }
    else
    {
        TreeItemList::reverse_iterator rit = m_lstTreeItems.rbegin();
        CGUITreeItem* pLastItem = *rit;
        pItem->SetRect(pLastItem->GetLeft(), pLastItem->GetBottom(), pLastItem->GetWidth(), pLastItem->GetHeight());
    }
    pItem->SetRealRect(pItem->GetRenderRect());
    m_lstTreeItems.push_back(pItem);
}

void CGUITreeItem::RenderSelf()
{
    /*
    if (m_bExpanded)
    {      
        TreeItemList_Iter itEnd = m_lstTreeItems.end();
        CGUITreeItem* pTreeItem = NULL;
        for (TreeItemList_Iter it = m_lstTreeItems.begin(); it != itEnd; ++it)
        {
            pTreeItem = *it;
            pTreeItem->Render();
        }
    }
    */
    //m_bDrawFrame = true;
    CGUIElement::RenderSelf();

    if (m_lstTreeItems.size() > 0)
    {
        m_pBtnExpand->Render();
    }
    
    //m_pStaticText->SetIsDrawFrame(true);
    m_pStaticText->Render();

    if (m_bSelected)
    {
        GUISYSTEM->GetGUIRenderer()->Render(m_renderQuad.m_rectScreen, Rectf::ZERO, m_renderQuad.m_fDepth, NULL, m_pOwner->GetTreeItemInfo().m_clrItemSelected);
    }
}

void CGUITreeItem::CreateDefault()
{
    if (m_pBtnExpand == NULL)
    {
        m_pBtnExpand = new CGUICheckBox;
        m_pBtnExpand->SetSchemeManager(m_pScheme);
        m_pBtnExpand->MoveTo(m_renderQuad.m_rectScreen.left, m_renderQuad.m_rectScreen.top);
        m_pBtnExpand->SetCheck(m_bExpanded);
    }

    if (m_pStaticText == NULL)
    {
        m_pStaticText = new CGUIStaticText;
        m_pStaticText->SetSchemeManager(m_pScheme);
        m_pStaticText->SetAlign(eAlign_Left);
        m_pStaticText->MoveTo(m_pBtnExpand->GetRight(), m_renderQuad.m_rectScreen.top);
    }
}

void CGUITreeItem::SetExpandBtnImgSrcUnCheck(const AIRString &strImgSrc)
{
    //m_treeItemInfo.strExpandBtnUnCheckImgSrc = strImgSrc;
    if (m_pBtnExpand)
    {
        m_pBtnExpand->SetUnCheckedImageSrc(strImgSrc);
        m_pBtnExpand->AutoSize();
        m_pBtnExpand->MoveTo(m_renderQuad.m_rectScreen.left, m_renderQuad.m_rectScreen.top + (GetHeight() - m_pBtnExpand->GetHeight()) / 2);
        m_pStaticText->MoveTo(m_pBtnExpand->GetRight(), m_renderQuad.m_rectScreen.top);
        //m_pStaticText->SetWidth(m_renderQuad.m_rectScreen.right - m_pBtnExpand->GetRight());
    }

    TreeItemList_Iter itEnd = m_lstTreeItems.end();
    CGUITreeItem* pTreeItem = NULL;
    for (TreeItemList_Iter it = m_lstTreeItems.begin(); it != itEnd; ++it)
    {
        pTreeItem = *it;
        pTreeItem->SetExpandBtnImgSrcUnCheck(strImgSrc);
    }

    CaculateChildrenItemPosition();
}

void CGUITreeItem::SetExpandBtnImgSrcCheck(const AIRString& strImgSrc)
{
    //m_treeItemInfo.strExpandBtnCheckImgSrc = strImgSrc;

    if (m_pBtnExpand)
    {
        m_pBtnExpand->SetCheckedImageSrc(strImgSrc);
        m_pBtnExpand->AutoSize();
        m_pBtnExpand->MoveTo(m_renderQuad.m_rectScreen.left, m_renderQuad.m_rectScreen.top + (GetHeight() - m_pBtnExpand->GetHeight()) / 2);
        m_pStaticText->MoveTo(m_pBtnExpand->GetRight(), m_renderQuad.m_rectScreen.top);
        //m_pStaticText->SetWidth(m_renderQuad.m_rectScreen.right - m_pBtnExpand->GetRight());
    }

    TreeItemList_Iter itEnd = m_lstTreeItems.end();
    CGUITreeItem* pTreeItem = NULL;
    for (TreeItemList_Iter it = m_lstTreeItems.begin(); it != itEnd; ++it)
    {
        pTreeItem = *it;
        pTreeItem->SetExpandBtnImgSrcCheck(strImgSrc);
    }

    CaculateChildrenItemPosition();
}

void CGUITreeItem::SetFont(IFontBase *pFont)
{
    if (m_pStaticText)
    {
        m_pStaticText->SetFont(pFont);
    }
}

void CGUITreeItem::SetText(const char* szText)
{
    if (m_pStaticText)
    {
        m_pStaticText->SetText(szText);
    }
}

void CGUITreeItem::GetVisableItems(const Rectf& rect, TreeItemList& lstVisable)
{
    if (m_bExpanded)
    {
        TreeItemList_Iter itEnd = m_lstTreeItems.end();
        CGUITreeItem* pTreeItem = NULL;
        for (TreeItemList_Iter it = m_lstTreeItems.begin(); it != itEnd; ++it)
        {
            pTreeItem = *it;
            if (rect.GetRectRelation(pTreeItem->GetRenderRect()) == RR_DES_INSIDE_SELF)
            {
                lstVisable.push_back(pTreeItem);
                pTreeItem->GetVisableItems(rect, lstVisable);
            }
        }
    }
}

void CGUITreeItem::SetItemHeight(int nHeight)
{
    SetHeight(nHeight);
    m_pStaticText->SetHeight(nHeight);

    //调整button和statictext的位置
    m_pBtnExpand->MoveTo(m_renderQuad.m_rectScreen.left, m_renderQuad.m_rectScreen.top + (GetHeight() - m_pBtnExpand->GetHeight()) / 2);
    m_pStaticText->MoveTo(m_pBtnExpand->GetRight(), m_renderQuad.m_rectScreen.top);
    TreeItemList_Iter itEnd = m_lstTreeItems.end();
    CGUITreeItem* pTreeItem = NULL;
    for (TreeItemList_Iter it = m_lstTreeItems.begin(); it != itEnd; ++it)
    {
        pTreeItem = *it;
        pTreeItem->SetItemHeight(nHeight);
    }

    CaculateChildrenItemPosition();
}

void CGUITreeItem::SetOwner(CGUITreeCtrl* pTreeCtrl)
{
    m_pOwner = pTreeCtrl;
    for (TreeItemList_Iter it = m_lstTreeItems.begin(); it != m_lstTreeItems.end(); ++it)
    {
        (*it)->SetOwner(m_pOwner);
    }
}

void CGUITreeItem::DeleteChildItem(CGUITreeItem* pItem)
{
    if (m_lstTreeItems.size() == 0)
    {
        return;
    }
    TreeItemList_Iter it = std::find(m_lstTreeItems.begin(), m_lstTreeItems.end(), pItem);

    if (it != m_lstTreeItems.end())
    {
        if (pItem == m_pOwner->GetSelectedItem())
        {
            m_pOwner->SetSelectedItem(NULL);
        }
        pItem->CleanUp();
        SafeDelete(pItem);
        m_lstTreeItems.erase(it);
    }

    CaculateChildrenItemPosition();
    m_realRect.left = m_renderQuad.m_rectScreen.left;
    m_realRect.top = m_renderQuad.m_rectScreen.top;
    m_realRect.right = m_renderQuad.m_rectScreen.right;
    m_realRect.bottom = m_renderQuad.m_rectScreen.bottom + m_lstTreeItems.size() * m_renderQuad.m_rectScreen.GetHeight();
    m_pOwner->CaculateItemsPosition();
}

void CGUITreeItem::SetSelected(bool bSelected)
{
    m_bSelected = bSelected;
    m_pStaticText->SetLineSelected(0, bSelected);
}

void CGUITreeItem::CaculateChildrenItemPosition()
{
    TreeItemList_Iter itEnd = m_lstTreeItems.end();
    CGUITreeItem* pTreeItem = NULL;
    CGUITreeItem* pPreItem = NULL;
    int nIndex = 0;
    for (TreeItemList_Iter it = m_lstTreeItems.begin(); it != itEnd; ++it, ++nIndex)
    {
        pTreeItem = *it;
        if (nIndex == 0)
        {
            pTreeItem->MoveTo(m_pBtnExpand->GetRight(), m_renderQuad.m_rectScreen.bottom);
        }
        else
        {
            pTreeItem->MoveTo(pPreItem->GetLeft(), pPreItem->GetBottom());
        }

        pPreItem = pTreeItem;
    }
}

CGUITreeItem* CGUITreeItem::GetChildItem(const char* szText)
{
    if (m_lstTreeItems.empty())
    {
        return NULL;
    }

    TreeItemList_Iter itEnd = m_lstTreeItems.end();
    for (TreeItemList_Iter it = m_lstTreeItems.begin(); it != itEnd; ++it)
    {
        if (strcmp((*it)->GetText(), szText) == 0)
        {
            return *it;
        }
    }

    return NULL;
}

CGUITreeItem* CGUITreeItem::GetChildItem(int nIndex)
{
    TreeItemList_Iter it = m_lstTreeItems.begin();
    std::advance(it, nIndex);
    if (it != m_lstTreeItems.end())
    {
        return *it;
    }

    return NULL;
}
