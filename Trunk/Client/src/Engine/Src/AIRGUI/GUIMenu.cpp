#include "GUIMenu.h"
#include "FontManager.h"
#include "Engine.h"
#include "StringUtil.h"
#include "ImageSet.h"
#include "GUISystem.h"

CMenuItem::CMenuItem() : m_pItemInfo(NULL), m_nIndex(0)
{
    m_align = eAlign_Left;
    m_nLeftOffset = 5;
}

CMenuItem::~CMenuItem()
{

}

bool CMenuItem::Create(CGUIMenu* pParent, const MENUITEM_INFO& menuItemInfo, int nOffsetX, int nOffsetY)
{
    m_pParent = pParent;

    m_pItemInfo = &menuItemInfo;
    //m_itemData = itemData;
    m_pScheme = pParent->GetScheme();


    int nTopLeftX = nOffsetX;
    int nTopLeftY = nOffsetY;
    if (m_pParent)
    {
        Rectf rectParent;
        pParent->GetRenderRect(rectParent);
        nTopLeftX += rectParent.left;
        nTopLeftY += rectParent.top;
    }
    m_renderQuad.m_rectScreen.SetRect(nTopLeftX, nTopLeftY, nTopLeftX + menuItemInfo.nWidth, nTopLeftY + menuItemInfo.nHeight);
    m_realRect = m_renderQuad.m_rectScreen;

    return true;
}

bool CMenuItem::CreateFromXML(CGUIElement* pParent, CMarkupSTL& xml)
{
    CreateSelf(pParent, xml);
    CreateChild(xml);
    return true;
}

bool CMenuItem::CreateSelf(CGUIElement* pParent, CMarkupSTL& xml)
{
    m_pParent = pParent;

    AIRString strValue = xml.GetAttrib("index").c_str();
    if (strValue.length() == 0)
    {
        return false;
    }
    m_nIndex = atoi(strValue.c_str());
    
    strValue = xml.GetAttrib("text").c_str();
    wchar_t wcText[256] = { 0 };
    MByteToWChar(strValue.c_str(), wcText, 256);
    //m_st = wcText;
    SetText(wcText);

    return true;
}

void CMenuItem::SetIndex(int nIndex)
{
    m_nIndex = nIndex;
}

bool CMenuItem::SaveSelfToXML(CMarkupSTL& xml )
{
    xml.AddElem("MenuItem");
    xml.AddAttrib("index", m_nIndex);

    char szText[256];
    WCharToMByte(m_strText.c_str(), szText, 256);
    xml.AddAttrib("text", szText);
    return true;
}

void CMenuItem::SetGUIMenuItemInfo(const LPMENUITEM_INFO pItemInfo)
{
    m_pItemInfo = pItemInfo;
}

bool CMenuItem::OnMouseMoveSelf(int x, int y)
{
    CGUIElement::OnMouseMoveSelf(x, y);
    //m_renderQuad.m_uv = m_pItemInfo->uvHover;
    return true;
}

bool CMenuItem::OnLButtonUpSelf(int x, int y)
{
    m_pParent->Hide();
    return CGUIElement::OnLButtonUpSelf(x, y);
}

void CMenuItem::RenderSelf()
{
    //CGUIElement::RenderSelf();
    if (m_pItemInfo->pItemTexture)
    {
        GUISYSTEM->GetGUIRenderer()->Render(m_renderQuad.m_rectScreen, m_renderQuad.m_uv, m_renderQuad.m_fDepth, m_pItemInfo->pItemTexture, m_renderQuad.m_clrDiffuse);
    }
    
    CGUIStaticText::RenderSelf();
}

void CMenuItem::OnStatusChange()
{
    switch ( m_nCurUIState )
    {
    case UI_NORMAL:
        {
            m_renderQuad.m_uv = m_pItemInfo->uvNormal;
            m_renderQuad.m_clrDiffuse = 0xffffffff;
        }
        break;
    case UI_HOVER:
    case UI_LBUTTONDOWN:
        {
            if (m_pItemInfo->strHoverImgSrc.length() == 0)
            {
                m_renderQuad.m_clrDiffuse = m_pItemInfo->m_clrHover;
            }
            else
            {
                m_renderQuad.m_uv = m_pItemInfo->uvHover;
            }
        }
        break;
    

    //case UI_GRAY:
    //    {
    //        m_renderQuad.m_uv = m_UVInactive;
    //    }
    //    break;
    }
}

CGUIMenu::CGUIMenu(void) : m_pItemFont(NULL)
, m_nItemsNum(0)
{
    m_strType = "CGUIMenu";
}

CGUIMenu::~CGUIMenu(void)
{
}
/*
void CGUIMenu::SetImgSrcTopLeft(const AIRString& strImgSrc)
{
    if (m_pTopLeftImg)
    {
        m_pTopLeftImg->SetImageSrc(strImgSrc);
        m_strImgSrcTopLeft = m_pTopLeftImg->GetImageSrc();
        m_pTopLeftImg->AutoSize();
    }
}

void CGUIMenu::SetImgSrcTop(const AIRString& strImgSrc)
{
    if (m_pTopImg)
    {
        m_pTopImg->SetImageSrc(strImgSrc);
        m_strImgSrcTop = m_pTopImg->GetImageSrc();
        m_pTopImg->AutoSize();
    }
}

void CGUIMenu::SetImgSrcTopRight(const AIRString& strImgSrc)
{
    if (m_pTopRightImg)
    {
        m_pTopRightImg->SetImageSrc(strImgSrc);
        m_strImgSrcTopRight = m_pTopRightImg->GetImageSrc();
        m_pTopRightImg->AutoSize();
    }
}

void CGUIMenu::SetImgSrcRight(const AIRString& strImgSrc)
{
    if (m_pRightImg)
    {
        m_pRightImg->SetImageSrc(strImgSrc);
        m_strImgSrcRight = m_pRightImg->GetImageSrc();
        m_pRightImg->AutoSize();
    }
}

void CGUIMenu::SetImgSrcBottomRight(const AIRString& strImgSrc)
{
    if (m_pBottomRightImg)
    {
        m_pBottomRightImg->SetImageSrc(strImgSrc);
        m_strImgSrcBottomRight = m_pBottomRightImg->GetImageSrc();
        m_pBottomRightImg->AutoSize();
    }
}

void CGUIMenu::SetImgSrcBottom(const AIRString& strImgSrc)
{
    if (m_pBottomImg)
    {
        m_pBottomImg->SetImageSrc(strImgSrc);
        m_strImgSrcBottom = m_pBottomImg->GetImageSrc();
        m_pBottomImg->AutoSize();
    }
}

void CGUIMenu::SetImgSrcBottomLeft(const AIRString& strImgSrc)
{
    if (m_pBottomLeftImg)
    {
        m_pBottomLeftImg->SetImageSrc(strImgSrc);
        m_strImgSrcBottomLeft = m_pBottomLeftImg->GetImageSrc();
        m_pBottomLeftImg->AutoSize();
    }
}

void CGUIMenu::SetImgSrcLeft(const AIRString& strImgSrc)
{
    if (m_pLeftImg)
    {
        m_pLeftImg->SetImageSrc(strImgSrc);
        m_strImgSrcLeft = m_pLeftImg->GetImageSrc();
        m_pLeftImg->AutoSize();
    }
}

void CGUIMenu::SetImgSrcCenter(const AIRString& strImgSrc)
{
    if (m_pCenterImg)
    {
        m_pCenterImg->SetImageSrc(strImgSrc);
        m_strImgSrcCenter = m_pCenterImg->GetImageSrc();
        m_pCenterImg->AutoSize();
    }
}
*/
void CGUIMenu::SetShowPosition(int x, int y)
{
    //计算出menu的左上方的点
    int nScreenWidth = 0;
    int nScreenHeight = 0; 
    ENGINE_INST->GetBackBufferSize(nScreenWidth, nScreenHeight);
    if (x + (int)GetWidth() > nScreenWidth)
    {
        m_renderQuad.m_rectScreen.left = x - GetWidth();
    }
    else
        m_renderQuad.m_rectScreen.left = x;

    if (y + (int)GetHeight() > nScreenHeight)
    {
        m_renderQuad.m_rectScreen.top = nScreenHeight - GetHeight();
    }
    else
        m_renderQuad.m_rectScreen.top = y;

    CaculateMenuSize();
    Show();
}

bool CGUIMenu::CreateSelf(CGUIElement* pParent, CMarkupSTL& xml)
{
    CGUIAutoSizeFrame::CreateSelf(pParent, xml);

    AIRString strValue = xml.GetAttrib("ItemCount").c_str();
    m_nItemsNum = atoi(strValue.c_str());

    strValue = xml.GetAttrib("ItemWidth").c_str();
    m_menuItemInfo.nWidth = atoi(strValue.c_str());

    strValue = xml.GetAttrib("ItemHeight").c_str();
    m_menuItemInfo.nHeight = atoi(strValue.c_str());

    CreateMenuItems();

    strValue = xml.GetAttrib("ItemImgSrc").c_str();
    SetItemImageSrc(strValue);
    strValue = xml.GetAttrib("ItemImgSrcHover").c_str();
    SetItemHoverImgSrc(strValue);

    CaculateMenuSize();

    return true;
}

bool CGUIMenu::CreateChild(CMarkupSTL& parent)
{
    parent.IntoElem();
    int nOffsetY = 0;
    int nOffsetX = 0;
    if (m_pTopLeftImg)
    {
        nOffsetY = m_pTopLeftImg->GetHeight();
        nOffsetX = m_pTopLeftImg->GetWidth();
    }
    AIRString strValue;
    bool bItemCreated = m_vtMenuItems.size() > 0 ? true : false;
    size_t index = 0;
    if (parent.FindElem("GUIFont"))
    {
        parent.IntoElem();
        m_pItemFont = CreateGUIFont(parent);
        parent.OutOfElem();
        //CreateGUIWords();
    }
    while (parent.FindElem())
    {
        AIRString strElementName = parent.GetTagName().c_str();
        if (strElementName == "MenuItem")    //创建脚本
        {
            CMenuItem* pMenuItem = bItemCreated ? m_vtMenuItems[index++] : new CMenuItem;

            pMenuItem->SetFont(m_pItemFont);

            if (bItemCreated)
            {
                pMenuItem->CreateFromXML(this, parent);
            }
            else
                pMenuItem->Create(this, m_menuItemInfo, 0, nOffsetY);
        }
        else if (strElementName == "Script")    //创建脚本
        {
            if( !m_pScriptObject )
            {
                m_pScriptObject = new CGUIScriptObject();	
            }
            if( !m_pScriptObject->Load(parent) )
            {
                SafeDelete(m_pScriptObject);
                //DELETE_ATOM( m_pScript );
                return false;
            }
            if (m_pScriptObject)
            {
                m_byKeyUp = m_pScriptObject->GetKeyUp();
                m_byKeyDown = m_pScriptObject->GetKeyDown();
            }
            //m_pScript->SetRunningOn(this);
        }
    }
    parent.OutOfElem();
    return true;
}

void CGUIMenu::SetItemFont(const FONTTYPE& ft)
{
    m_pItemFont = FONTMGR->CreateFontW(ft, L"0");
    //CGUIListItem* pItem = NULL;
    size_t itemSize = m_vtMenuItems.size();
    for (size_t i = 0; i < itemSize; ++i)
    {
        if (m_vtMenuItems[i])
        {
            m_vtMenuItems[i]->SetFont(m_pItemFont);
        }
    }
}

void CGUIMenu::ClearSelf()
{
    CGUIElement::ClearSelf();

    ClearItem();
}

void CGUIMenu::ClearItem()
{
    size_t itemSize = m_vtMenuItems.size();
    for (size_t i = 0; i < itemSize; ++i)
    {
        if (m_vtMenuItems[i])
        {
            m_vtMenuItems[i]->CleanUp();
            SafeDelete(m_vtMenuItems[i]);
        }
    }

    m_vtMenuItems.clear();
}

void CGUIMenu::CreateMenuItems()
{
    ClearItem();
    int nOffsetY = 0;
    int nOffsetX = 0;
    if (m_pTopLeftImg)
    {
        nOffsetY = m_pTopLeftImg->GetHeight();
        nOffsetX = m_pTopLeftImg->GetWidth();
    }
    m_vtMenuItems.resize(m_nItemsNum);
    for (int i = 0; i < m_nItemsNum; ++i)
    {
        CMenuItem* pMenuItem = new CMenuItem;
        pMenuItem->Create(this, m_menuItemInfo, nOffsetX, nOffsetY);
        nOffsetY += m_menuItemInfo.nHeight;
        m_vtMenuItems[i] = pMenuItem;
    }
}

void CGUIMenu::SetItemsCount(int nItemsCount)
{
    if (nItemsCount != m_nItemsNum)
    {
        m_nItemsNum = nItemsCount;
        CreateMenuItems();
    }
    
}

void CGUIMenu::SetItemText(int nIndex, const char* szText)
{
    if ((size_t)nIndex < m_vtMenuItems.size())
    {
        m_vtMenuItems[nIndex]->SetText(szText);
    }
}

void CGUIMenu::CaculateMenuSize()
{
    //计算出所有Item的总高
    int nTotalItemHeight = 0;
    size_t itemSize = m_vtMenuItems.size();
    CMenuItem* pMenuItem = NULL;
    
    for (size_t i = 0; i < itemSize; ++i)
    {
        pMenuItem = m_vtMenuItems[i];
        if (pMenuItem->IsVisable())
        {
            nTotalItemHeight += m_menuItemInfo.nHeight;
        }
    }

    m_pTopLeftImg->MoveTo(m_renderQuad.m_rectScreen.left, m_renderQuad.m_rectScreen.top);
    m_pTopImg->SetWidth(m_menuItemInfo.nWidth);
    m_pBottomImg->SetWidth(m_menuItemInfo.nWidth);
    m_pLeftImg->SetHeight(nTotalItemHeight);
    m_pRightImg->SetHeight(nTotalItemHeight);
    m_pCenterImg->SetWidth(m_menuItemInfo.nWidth);
    m_pCenterImg->SetHeight(nTotalItemHeight);

    m_pTopImg->MoveTo(m_pTopLeftImg->GetRight(), m_pTopLeftImg->GetTop());
    m_pTopRightImg->MoveTo(m_pTopImg->GetRight(), m_pTopImg->GetTop());
    m_pRightImg->MoveTo(m_pTopRightImg->GetLeft(), m_pTopRightImg->GetBottom());
    m_pBottomRightImg->MoveTo(m_pRightImg->GetLeft(), m_pRightImg->GetBottom());
    m_pLeftImg->MoveTo(m_pTopLeftImg->GetLeft(), m_pTopLeftImg->GetBottom());
    m_pBottomLeftImg->MoveTo(m_pLeftImg->GetLeft(), m_pLeftImg->GetBottom());
    m_pBottomImg->MoveTo(m_pBottomLeftImg->GetRight(), m_pBottomLeftImg->GetTop());
    m_pCenterImg->MoveTo(m_pTopImg->GetLeft(), m_pTopImg->GetBottom());

    CMenuItem* pLastItem = NULL;
    for (size_t i = 0; i < itemSize; ++i)
    {
        pMenuItem = m_vtMenuItems[i];
        if (pMenuItem->IsVisable())
        {
            if (pLastItem == NULL)  //第一个
            {
                pMenuItem->MoveTo(m_pCenterImg->GetLeft(), m_pCenterImg->GetTop());
            }
            else
            {
                pMenuItem->MoveTo(pLastItem->GetLeft(), pLastItem->GetBottom());
            }
            pMenuItem->SetHeight(m_menuItemInfo.nHeight);
            pMenuItem->SetWidth(m_menuItemInfo.nWidth);
            pLastItem = pMenuItem;
            //nTotalItemHeight += m_menuItemInfo.nHeight;
        }
    }

    m_renderQuad.m_rectScreen.SetRect(m_pTopLeftImg->GetLeft(), m_pTopLeftImg->GetTop(), m_pBottomRightImg->GetRight(), m_pBottomRightImg->GetBottom());
}

void CGUIMenu::SetItemVisable(int nIndex, bool bVisable)
{
    if ((size_t)nIndex < m_vtMenuItems.size())
    {
        if (m_vtMenuItems[nIndex]->IsVisable() != bVisable)
        {
            if (bVisable)
            {
                m_vtMenuItems[nIndex]->Show();
            }
            else
            {
                m_vtMenuItems[nIndex]->Hide();
            }
            CaculateMenuSize();
        }
        
    }
}

void CGUIMenu::SetItemWidth(int nWidth)
{
    m_menuItemInfo.nWidth = nWidth;
    CaculateMenuSize();
}

void CGUIMenu::SetItemHeight(int nHeight)
{
    m_menuItemInfo.nHeight = nHeight;
    CaculateMenuSize();
}

bool CGUIMenu::SaveSelfToXML(CMarkupSTL& xml )
{
    CGUIAutoSizeFrame::SaveSelfToXML(xml);

    xml.AddAttrib("ItemCount", m_nItemsNum);

    xml.AddAttrib("ItemWidth", m_menuItemInfo.nWidth);
    xml.AddAttrib("ItemHeight", m_menuItemInfo.nHeight);

    xml.AddAttrib("ItemImgSrc", m_menuItemInfo.strImgSrc.c_str());
    xml.AddAttrib("ItemImgSrcHover", m_menuItemInfo.strHoverImgSrc.c_str());

    if (m_pItemFont)
    {
        SaveFontToXML(xml, m_pItemFont);
    }
    
    return true;
}

bool CGUIMenu::SaveChildToXML(CMarkupSTL& xml )
{
    //这里只保存menuitem
    for (size_t i = 0; i < m_vtMenuItems.size(); ++i)
    {
        xml.IntoElem();
        m_vtMenuItems[i]->SaveToXML(xml);
        xml.OutOfElem();
    }
    return true;
}

void CGUIMenu::RenderChild()
{
    CGUIElement::RenderChild();

    size_t itemsNum = m_vtMenuItems.size();
    for (size_t i = 0; i < itemsNum; ++i)
    {
        if (m_vtMenuItems[i]->IsVisable())
        {
            m_vtMenuItems[i]->Render();
        }
    }
}

void CGUIMenu::CreateDefault()
{
    m_pTopLeftImg = new CGUIImage;
    m_pTopImg = new CGUIImage;
    m_pTopRightImg = new CGUIImage;
    m_pRightImg = new CGUIImage;
    m_pBottomRightImg = new CGUIImage;
    m_pBottomImg = new CGUIImage;
    m_pBottomLeftImg = new CGUIImage;
    m_pLeftImg = new CGUIImage;
    m_pCenterImg = new CGUIImage;

    AddChild(m_pTopLeftImg);
    AddChild(m_pTopImg);
    AddChild(m_pTopRightImg);
    AddChild(m_pRightImg);
    AddChild(m_pBottomRightImg);
    AddChild(m_pBottomImg);
    AddChild(m_pBottomLeftImg);
    AddChild(m_pLeftImg);
    AddChild(m_pCenterImg);

    m_pItemFont = FONTMGR->CreateDefaultFonts();
}

void CGUIMenu::SetRect(const Rectf& rect)
{
    CGUIElement::SetRect(rect);
    if (m_pTopLeftImg)
    {
        m_pTopLeftImg->MoveTo(m_renderQuad.m_rectScreen.left, m_renderQuad.m_rectScreen.top);
    }

    if (m_pTopRightImg)
    {
        m_pTopRightImg->MoveTo(m_renderQuad.m_rectScreen.right - m_pTopRightImg->GetWidth(), m_renderQuad.m_rectScreen.top);
    }

    if (m_pCenterImg)
    {
        m_pCenterImg->SetWidth(m_renderQuad.m_rectScreen.GetWidth() - m_pTopLeftImg->GetWidth() - m_pTopRightImg->GetWidth());
        m_pCenterImg->SetHeight(m_renderQuad.m_rectScreen.GetHeight() - m_pTopLeftImg->GetHeight() - m_pBottomLeftImg->GetHeight());
        m_pCenterImg->MoveTo(m_pTopLeftImg->GetRight(), m_pTopLeftImg->GetBottom());
    }

    if (m_pBottomRightImg)
    {
        m_pBottomRightImg->MoveTo(m_pCenterImg->GetRight(), m_pCenterImg->GetBottom());
    }
    
    if (m_pTopImg)
    {
        m_pTopImg->SetWidth(m_pCenterImg->GetWidth());
        m_pTopImg->MoveTo(m_pTopLeftImg->GetRight(), m_pTopLeftImg->GetTop());
    }

    if (m_pRightImg)
    {
        m_pRightImg->SetHeight(m_pCenterImg->GetHeight());
        m_pRightImg->MoveTo(m_pCenterImg->GetRight(), m_pTopRightImg->GetBottom());
    }

    if (m_pLeftImg)
    {
        m_pLeftImg->SetHeight(m_pCenterImg->GetHeight());
        m_pLeftImg->MoveTo(m_pTopLeftImg->GetLeft(), m_pTopLeftImg->GetBottom());
    }

    if (m_pBottomLeftImg)
    {
        m_pBottomLeftImg->MoveTo(m_pLeftImg->GetLeft(), m_pLeftImg->GetBottom());
    }
}

bool CGUIMenu::OnMouseMove(int x, int y)
{
    size_t itemsNum = m_vtMenuItems.size();
    for (size_t i = 0; i < itemsNum; ++i)
    {
        if (m_vtMenuItems[i]->IsVisable() && m_vtMenuItems[i]->IsInRect(x, y))
        {
            return m_vtMenuItems[i]->OnMouseMove(x, y);
        }
    }
    return OnMouseMoveSelf(x, y);
}


void CGUIMenu::SetItemImageSrc(const AIRString& strImgSrc)
{
    if (m_menuItemInfo.strImgSrc != strImgSrc)
    {
        m_menuItemInfo.strImgSrc = strImgSrc;
        AIRString strImageSet;
        AIRString strImageUnit;
        CStringUtil::SplitFilename(strImgSrc, strImageSet, strImageUnit);

        CImageSet* pImageSet = m_pScheme->GetImageSet(strImageSet);
        CImageUnit* pUnit = NULL;
        if (pImageSet)
        {
            pUnit = pImageSet->GetImgUnit(strImageUnit);
        }

        if (!pUnit)
        {
            return;
        }
        m_menuItemInfo.uvNormal = pUnit->m_fUV;
        m_menuItemInfo.pItemTexture = pImageSet->GetTexture();
        //m_renderQuad.m_uv = pUnit->m_fUV;

        for (size_t i = 0; i < m_vtMenuItems.size(); ++i)
        {
            m_vtMenuItems[i]->SetUVRect(m_menuItemInfo.uvNormal);
        }
    }
}

void CGUIMenu::SetItemHoverImgSrc(const AIRString& strImgSrc)
{
    if (m_menuItemInfo.strHoverImgSrc != strImgSrc)
    {
        m_menuItemInfo.strHoverImgSrc = strImgSrc;
        AIRString strImageSet;
        AIRString strImageUnit;
        CStringUtil::SplitFilename(strImgSrc, strImageSet, strImageUnit);

        CImageSet* pImageSet = m_pScheme->GetImageSet(strImageSet);
        CImageUnit* pUnit = NULL;
        if (pImageSet)
        {
            pUnit = pImageSet->GetImgUnit(strImageUnit);
        }

        if (!pUnit)
        {
            return;
        }
        m_menuItemInfo.uvHover = pUnit->m_fUV;
        m_menuItemInfo.pItemTexture = pImageSet->GetTexture();
        //m_renderQuad.m_uv = pUnit->m_fUV;
    }
}

void CGUIMenu::SetItemHoverColor(const SColor& color)
{
    m_menuItemInfo.m_clrHover = color;
}

void CGUIMenu::Move(int dx, int dy)
{
    CGUIElement::Move(dx, dy);
    size_t numItem = m_vtMenuItems.size();

    for (size_t i = 0; i < numItem; ++i)
    {
        m_vtMenuItems[i]->Move(dx, dy);
    }
}

void CGUIMenu::SetItemLButtonUpCallBack(size_t nIndex, LButtonUpCallBack pFun)
{
    if (nIndex >= m_vtMenuItems.size())
    {
        return;
    }
    m_vtMenuItems[nIndex]->SetLButtonUpFun(pFun);
}

CMenuItem* CGUIMenu::GetMenuItem(size_t nIndex)
{
    if (nIndex >= m_vtMenuItems.size())
    {
        return NULL;
    }

    return m_vtMenuItems[nIndex];
}

bool CGUIMenu::OnLButtonDown(int x, int y)
{
    MenuItemArray_Iter itEnd = m_vtMenuItems.end();
    CMenuItem* pItem = NULL;
    for (MenuItemArray_Iter it = m_vtMenuItems.begin(); it != itEnd; ++it)
    {
        pItem = *it;
        if (pItem->IsVisable() && pItem->IsInRect(x, y))
        {
            if (pItem->IsEnable())
            {
                return pItem->OnLButtonDown(x, y); 
            }
            else
                break;
        }
    }

    return true;
}

bool CGUIMenu::OnLButtonUp(int x, int y)
{
    MenuItemArray_Iter itEnd = m_vtMenuItems.end();
    CMenuItem* pItem = NULL;
    for (MenuItemArray_Iter it = m_vtMenuItems.begin(); it != itEnd; ++it)
    {
        pItem = *it;
        if (pItem->IsVisable() && pItem->IsInRect(x, y))
        {
            if (pItem->IsEnable())
            {
                return pItem->OnLButtonUp(x, y); 
            }
            else
                break;
        }
    }

    return true;
}

void CGUIMenu::DeleteMenuItem(size_t nIndex)
{
    if (nIndex >= m_vtMenuItems.size())
    {
        return;
    }
    MenuItemArray_Iter it = m_vtMenuItems.begin();
    std::advance(it, nIndex);
    if (it != m_vtMenuItems.end())
    {
        (*it)->CleanUp();
        SafeDelete(*it);
        m_vtMenuItems.erase(it);
    }
}
