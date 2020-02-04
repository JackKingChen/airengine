#include "GUITreeCtrl.h"
#include "FontManager.h"
#include "GUISystem.h"

CGUITreeCtrl::CGUITreeCtrl(void) : m_pSelectedItem(NULL), m_pTreeItemRButtonUp(NULL), m_pTreeItemLButtonUp(NULL), m_pTreeItemLButtonDBClick(NULL)
{
    m_strType = "CGUITreeCtrl";
	m_bMoveable = false;
}

CGUITreeCtrl::~CGUITreeCtrl(void)
{
}

void CGUITreeCtrl::RenderChild()
{
    GetVisableItems();

    CGUITreeItem::TreeItemList_Iter itEnd = m_lstVisableItems.end();
    for (CGUITreeItem::TreeItemList_Iter it = m_lstVisableItems.begin(); it != itEnd; ++it)
    {
        (*it)->Render();
    }
}

void CGUITreeCtrl::GetVisableItems()
{
    m_lstVisableItems.clear();
    GUIELEMENT_ITER itEnd = m_lstChildren.end();
    CGUITreeItem* pTreeItem = NULL;
    for (GUIELEMENT_ITER it = m_lstChildren.begin(); it != itEnd; ++it)
    {
        pTreeItem = static_cast<CGUITreeItem*>(*it);
        if (m_renderQuad.m_rectScreen.GetRectRelation(pTreeItem->GetRenderRect()) == RR_DES_INSIDE_SELF)
        {
            m_lstVisableItems.push_back(pTreeItem);
            pTreeItem->GetVisableItems(m_renderQuad.m_rectScreen, m_lstVisableItems);
        }
    }
}

void CGUITreeCtrl::CaculateRealSize()
{
    m_realRect.left = m_renderQuad.m_rectScreen.left;
    m_realRect.right = m_renderQuad.m_rectScreen.right;
    int nIndex = 0;
    CGUITreeItem* pTreeItem = NULL;
    GUIELEMENT_ITER itEnd = m_lstChildren.end();
    for (GUIELEMENT_ITER it = m_lstChildren.begin(); it != itEnd; ++it, ++nIndex)
    {
        pTreeItem = static_cast<CGUITreeItem*>(*it);
        if (nIndex == 0)
        {
            m_realRect.bottom = m_realRect.top = pTreeItem->GetTop();
        }
        m_realRect.bottom += pTreeItem->GetRealHeight();
    }

    if (m_realRect.bottom < m_renderQuad.m_rectScreen.bottom)
    {
        m_realRect.bottom = m_renderQuad.m_rectScreen.bottom;
    }
}

void CGUITreeCtrl::SetItemExpandBtnCheckImgSrc(const AIRString& strImgSrc)
{
    if (m_treeItemInfo.strExpandBtnCheckImgSrc != strImgSrc)
    {
        m_treeItemInfo.strExpandBtnCheckImgSrc = strImgSrc;
        CGUITreeItem* pTreeItem = NULL;
        GUIELEMENT_ITER itEnd = m_lstChildren.end();
        for (GUIELEMENT_ITER it = m_lstChildren.begin(); it != itEnd; ++it)
        {
            pTreeItem = static_cast<CGUITreeItem*>(*it);
            pTreeItem->SetExpandBtnImgSrcCheck(strImgSrc);
        }
    }
    
}

void CGUITreeCtrl::SetItemExpandBtnUnCheckImgSrc(const AIRString& strImgSrc)
{
    if (m_treeItemInfo.strExpandBtnCheckImgSrc != strImgSrc)
    {
        m_treeItemInfo.strExpandBtnUnCheckImgSrc = strImgSrc;
        CGUITreeItem* pTreeItem = NULL;
        GUIELEMENT_ITER itEnd = m_lstChildren.end();
        for (GUIELEMENT_ITER it = m_lstChildren.begin(); it != itEnd; ++it)
        {
            pTreeItem = static_cast<CGUITreeItem*>(*it);
            pTreeItem->SetExpandBtnImgSrcUnCheck(strImgSrc);
        }
    }
}

void CGUITreeCtrl::SetItemBackgroundImgSrc(const AIRString& strImgSrc)
{
    if (m_treeItemInfo.strItemBackgroundImgSrc != strImgSrc)
    {
        m_treeItemInfo.strExpandBtnUnCheckImgSrc = strImgSrc;
        CGUITreeItem* pTreeItem = NULL;
        GUIELEMENT_ITER itEnd = m_lstChildren.end();
        for (GUIELEMENT_ITER it = m_lstChildren.begin(); it != itEnd; ++it)
        {
            pTreeItem = static_cast<CGUITreeItem*>(*it);
            pTreeItem->SetImageSrc(strImgSrc);
        }
    }
}

void CGUITreeCtrl::SetItemFont(const FONTTYPE& ft)
{
    m_treeItemInfo.pFont = FONTMGR->CreateFontW(ft, L"0");
    CGUITreeItem* pTreeItem = NULL;
    GUIELEMENT_ITER itEnd = m_lstChildren.end();
    for (GUIELEMENT_ITER it = m_lstChildren.begin(); it != itEnd; ++it)
    {
        pTreeItem = static_cast<CGUITreeItem*>(*it);
        pTreeItem->SetFont(m_treeItemInfo.pFont);
    }
}

CGUITreeItem* CGUITreeCtrl::CreateItem(const char* szText)
{
    CGUITreeItem* pItem = new CGUITreeItem;
    pItem->SetSchemeManager(m_pScheme);
    pItem->CreateDefault();
    pItem->SetItemHeight(m_treeItemInfo.m_nItemHeight);
    pItem->SetExpandBtnImgSrcCheck(m_treeItemInfo.strExpandBtnCheckImgSrc);
    pItem->SetExpandBtnImgSrcUnCheck(m_treeItemInfo.strExpandBtnUnCheckImgSrc);
    pItem->SetFont(m_treeItemInfo.pFont);
    pItem->SetText(szText);
    pItem->SetOwner(this);
    return pItem;
}

void CGUITreeCtrl::AddItem(const char* szText, CGUITreeItem* pParent)
{
    CGUITreeItem* pItem = CreateItem(szText);
    if (pParent == NULL)
    {
        if (m_lstChildren.size() == 0)
        {
            pItem->SetRect(m_renderQuad.m_rectScreen.left, m_renderQuad.m_rectScreen.top, m_renderQuad.m_rectScreen.GetWidth(), m_treeItemInfo.m_nItemHeight);
        }
        else
        {
            GUIELEMENT_LIST::reverse_iterator rit = m_lstChildren.rbegin();
            CGUITreeItem* pLastItem = static_cast<CGUITreeItem*>(*rit);
            pItem->SetRect(m_renderQuad.m_rectScreen.left, pLastItem->GetBottom(), m_renderQuad.m_rectScreen.GetWidth(), m_treeItemInfo.m_nItemHeight);
            
        }
        pItem->SetRealRect(pItem->GetRenderRect());
        AddChild(pItem);
    }
    else 
        pParent->AddChildItem(pItem);
}

void CGUITreeCtrl::CaculateItemsPosition()
{
    GUIELEMENT_ITER itEnd = m_lstChildren.end();
    CGUITreeItem* pItem = NULL;
    CGUITreeItem* pPreItem = NULL;
    size_t index = 0;
    for (GUIELEMENT_ITER it = m_lstChildren.begin(); it != itEnd; ++it, ++index)
    {
        pItem = static_cast<CGUITreeItem*>(*it);
        if (index == 0)
        {
            pItem->MoveTo(m_renderQuad.m_rectScreen.left, m_renderQuad.m_rectScreen.top);
            pItem->SetWidth(m_renderQuad.m_rectScreen.GetWidth());
        }
        else
        {
            pItem->MoveTo(m_renderQuad.m_rectScreen.left, pPreItem->GetRealRect().bottom);
            pItem->SetWidth(m_renderQuad.m_rectScreen.GetWidth());
        }
        pPreItem = pItem;
    }
}

void CGUITreeCtrl::SetRect(const Rectf& rect)
{
    CGUIElement::SetRect(rect);
    CaculateItemsPosition();
}

void CGUITreeCtrl::SetItemText(size_t index, const char* szText)
{
    if (m_lstChildren.size() == 0)
    {
        return;
    }
    GUIELEMENT_ITER it = m_lstChildren.begin();
    std::advance(it, index);

    if (it != m_lstChildren.end())
    {
        CGUITreeItem* pItem = static_cast<CGUITreeItem*>(*it);
        pItem->SetText(szText);
    }
}

void CGUITreeCtrl::CreateDefault()
{
    m_treeItemInfo.m_nItemHeight = 20;
    m_treeItemInfo.pFont = FONTMGR->CreateDefaultFonts();
    AddItem("root", NULL);
    //AddItem("child1", static_cast<CGUITreeItem*>(*m_lstChildren.begin()));
    //AddItem("child2", static_cast<CGUITreeItem*>(*m_lstChildren.begin()));
    //AddItem("child3", static_cast<CGUITreeItem*>(*m_lstChildren.begin()));
    //AddItem("child4", static_cast<CGUITreeItem*>(*m_lstChildren.begin()));
    //AddItem("child5", static_cast<CGUITreeItem*>(*m_lstChildren.begin()));
    //AddItem("child6", static_cast<CGUITreeItem*>(*m_lstChildren.begin()));
}

void CGUITreeCtrl::DeleteItem(CGUITreeItem* pItem, CGUITreeItem* pParent)
{
    if (pParent)
    {
        pParent->DeleteChildItem(pItem);
    }
    else
    {
        GUIELEMENT_ITER it = std::find(m_lstChildren.begin(), m_lstChildren.end(), pItem);
        if (it != m_lstChildren.end())
        {
            if (pItem == static_cast<CGUITreeItem*>(*it))
            {
                pItem->CleanUp();
                SafeDelete(pItem);
                m_lstChildren.erase(it);
            }
        }
    }
}

bool CGUITreeCtrl::OnLButtonDown(int x, int y)
{
    CGUITreeItem* pDest = NULL;
    //CGUIElement* pElement = NULL;
    CGUITreeItem::TreeItemList_Iter itEnd = m_lstVisableItems.end();
    for (CGUITreeItem::TreeItemList_Iter it = m_lstVisableItems.begin(); it != itEnd; it++)
    {
        pDest = *it;
        if (pDest->IsInRect(x, y) && pDest->IsVisable())
        {
            return pDest->OnLButtonDown(x, y);
            //pDest = static_cast<CGUITreeItem*>(GUISYSTEM->GetFocusElement());
            //m_pSelectedItem = pDest;
            //return m_pSelectedItem->OnLButtonDown(x, y);
        }
    }
    return OnLButtonDownSelf(x, y);
}

bool CGUITreeCtrl::OnRButtonDown(int x, int y)
{
    CGUITreeItem* pDest = NULL;
    //CGUIElement* pElement = NULL;
    CGUITreeItem::TreeItemList_Iter itEnd = m_lstVisableItems.end();
    for (CGUITreeItem::TreeItemList_Iter it = m_lstVisableItems.begin(); it != itEnd; it++)
    {
        pDest = *it;
        if (pDest->IsInRect(x, y) && pDest->IsVisable())
        {
            SetSelectedItem(pDest);
            return pDest->OnRButtonDown(x, y);
            //pDest = static_cast<CGUITreeItem*>(GUISYSTEM->GetFocusElement());
            //m_pSelectedItem = pDest;
            //return m_pSelectedItem->OnLButtonDown(x, y);
        }
    }
    return OnRButtonDownSelf(x, y);
}

bool CGUITreeCtrl::OnRButtonUp(int x, int y)
{
    CGUITreeItem* pDest = NULL;
    //CGUIElement* pElement = NULL;
    CGUITreeItem::TreeItemList_Iter itEnd = m_lstVisableItems.end();
    for (CGUITreeItem::TreeItemList_Iter it = m_lstVisableItems.begin(); it != itEnd; it++)
    {
        pDest = *it;
        if (pDest->IsInRect(x, y) && pDest->IsVisable())
        {
            if (m_pTreeItemRButtonUp)
            {
                return m_pTreeItemRButtonUp(pDest);
            }
            return pDest->OnRButtonUp(x, y);
            //pDest = static_cast<CGUITreeItem*>(GUISYSTEM->GetFocusElement());
            //m_pSelectedItem = pDest;
            //return m_pSelectedItem->OnLButtonDown(x, y);
        }
    }
    return OnRButtonUpSelf(x, y);
}

void CGUITreeCtrl::SetTreeItemLButtonDBClickFun(TreeItemLButtonDBClickCallBack pFun)
{
    m_pTreeItemLButtonDBClick = pFun;
}

bool CGUITreeCtrl::OnLButtonDBClick(int x, int y)
{
    CGUITreeItem* pDest = NULL;
    //CGUIElement* pElement = NULL;
    CGUITreeItem::TreeItemList_Iter itEnd = m_lstVisableItems.end();
    for (CGUITreeItem::TreeItemList_Iter it = m_lstVisableItems.begin(); it != itEnd; it++)
    {
        pDest = *it;
        if (pDest->IsInRect(x, y) && pDest->IsVisable())
        {
            if (m_pTreeItemLButtonDBClick)
            {
                return m_pTreeItemLButtonDBClick(pDest);
            }
            return pDest->OnLButtonDBClick(x, y);
            //pDest = static_cast<CGUITreeItem*>(GUISYSTEM->GetFocusElement());
            //m_pSelectedItem = pDest;
            //return m_pSelectedItem->OnLButtonDown(x, y);
        }
    }
    return OnLButtonDBClick(x, y);
}

void CGUITreeCtrl::SetItemHeight(int nHeight)
{
    m_treeItemInfo.m_nItemHeight = nHeight;
    GUIELEMENT_ITER itEnd = m_lstChildren.end();
    CGUITreeItem* pItem = NULL;
    for (GUIELEMENT_ITER it = m_lstChildren.begin(); it != itEnd; ++it)
    {
        pItem = static_cast<CGUITreeItem*>(*it);
        pItem->SetItemHeight(nHeight);
    }
}

void CGUITreeCtrl::SetSelectedItem(CGUITreeItem* pItem)
{
    if (m_pSelectedItem)
    {
        m_pSelectedItem->SetSelected(false);
    }
    m_pSelectedItem = pItem;
    if (m_pSelectedItem)
    {
        m_pSelectedItem->SetSelected(true);
    }
}

bool CGUITreeCtrl::SaveSelfToXML(CMarkupSTL& xml )
{
    CGUIElement::SaveSelfToXML(xml);

    xml.AddAttrib("ItemHeight", m_treeItemInfo.m_nItemHeight);
    //展开时的按钮
    xml.AddAttrib("ItemExpandBtnExpanded", m_treeItemInfo.strExpandBtnCheckImgSrc.c_str());
    //未展开时的按钮图片
    xml.AddAttrib("ItemExpandBtnUnExpanded", m_treeItemInfo.strExpandBtnUnCheckImgSrc.c_str());

    if (m_treeItemInfo.pFont)
    {
        SaveFontToXML(xml, m_treeItemInfo.pFont);
    }
    
    return true;
}

bool CGUITreeCtrl::CreateSelf(CGUIElement* pParent, CMarkupSTL& xml)
{
    CGUIElement::CreateSelf(pParent, xml);

    AIRString strValue = xml.GetAttrib("ItemHeight").c_str();
    m_treeItemInfo.m_nItemHeight = atoi(strValue.c_str());
    m_treeItemInfo.strExpandBtnCheckImgSrc = xml.GetAttrib("ItemExpandBtnExpanded").c_str();
    m_treeItemInfo.strExpandBtnUnCheckImgSrc = xml.GetAttrib("ItemExpandBtnUnExpanded").c_str();

    //font必须要先创建，否则后面出错
    if (xml.FindChildElem("GUIFont"))
    {
        xml.IntoElem();
        m_treeItemInfo.pFont = CreateGUIFont(xml);
        xml.OutOfElem();
    }

    AddItem("root", NULL);
    

    CaculateItemsPosition();
    return true;
}

CGUITreeItem* CGUITreeCtrl::GetTreeItem(size_t index)
{
    if (m_lstChildren.empty())
    {
        return NULL;
    }
    GUIELEMENT_ITER it = m_lstChildren.begin();
    std::advance(it, index);
    if (it != m_lstChildren.end())
    {
        return static_cast<CGUITreeItem*>(*it);
    }

    return NULL;
}

void CGUITreeCtrl::SetTreeItemLButtonUpFun(TreeItemLButtonUpCallBack pFun)
{
    m_pTreeItemLButtonUp = pFun;
}

void CGUITreeCtrl::SetTreeItemRButtonUpFun(TreeItemRButtonUpCallBack pFun)
{
    m_pTreeItemRButtonUp = pFun;
}
