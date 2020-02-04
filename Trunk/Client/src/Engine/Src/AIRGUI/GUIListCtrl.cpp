//#include "StdAfx.h"
#include "GUIListCtrl.h"
#include "GUISchemeManager.h"
#include "StringUtil.h"
#include "FontManager.h"
#include "FileSystem.h"
#include "GUIAnimation.h"
#include "GUIListItem.h"
#include "GUIDragImage.h"

CGUIListCtrl::CGUIListCtrl(void) : m_pSelectedItem(NULL), m_nCol(10), m_nRow(10), m_nItemOffsetX(0), m_nItemOffsetY(0),
m_bItemHasSelectedImage(false)
, m_bItemDragable(false)
, m_bItemDropable(false)
, m_nDragItemType(0)
, m_pOnDropFun(NULL)
{
	m_strType = "CGUIListCtrl";
	m_bMoveable = false;
}

CGUIListCtrl::~CGUIListCtrl(void)
{
}

bool CGUIListCtrl::Create(CGUIElement* pParent, CMarkupSTL& xml)
{
    if (!CGUIElement::Create(pParent, xml))
    {
        return false;
    }

    AIRString strValue = xml.GetAttrib("col").c_str();
    m_nCol = atoi(strValue.c_str());

    strValue = xml.GetAttrib("row").c_str();
    m_nRow = atoi(strValue.c_str());

    strValue = xml.GetAttrib("ItemOffsetX").c_str();
    m_nItemOffsetX = atoi(strValue.c_str());

    strValue = xml.GetAttrib("ItemOffsetY").c_str();
    m_nItemOffsetY = atoi(strValue.c_str());

    strValue = xml.GetAttrib("ItemWidth").c_str();
    m_ItemInfo.nWidth = atoi(strValue.c_str());

    strValue = xml.GetAttrib("ItemHeight").c_str();
    m_ItemInfo.nHeight = atoi(strValue.c_str());

    m_ItemInfo.strImgSrc = xml.GetAttrib("ItemImgSrc").c_str();
    m_ItemInfo.strHoverImgSrc = xml.GetAttrib("ItemHoverImgSrc").c_str();
    m_ItemInfo.strUnuseImgSrc = xml.GetAttrib("ItemUnuseImgSrc").c_str();
    m_ItemInfo.strSelectedImgSrc = xml.GetAttrib( "ItemSelectedImgSrc" ).c_str();
    m_ItemInfo.strLockImgSrc = xml.GetAttrib("ItemLockImgSrc").c_str();

    strValue = xml.GetAttrib("ItemDragable").c_str();
    if (strValue == "TRUE")
    {
        m_bItemDragable = true;
    }

    strValue = xml.GetAttrib("ItemDropable").c_str();
    if (strValue == "TRUE")
    {
        m_bItemDropable = true;
    }

    UpdateItemData();

    CreateItems();
    /*
    for (u32 nRow = 0; nRow < m_nRow; nRow++)
    {
        for (u32 nCol = 0; nCol < m_nCol; nCol++)
        {
            CGUIListItem* pItem = AURO_NEW_VERBOSE CGUIListItem;
            int nOffsetX = m_nItemOffsetX + nCol * (m_ItemInfo.nWidth + m_nItemOffsetX);
            int nOffsetY = m_nItemOffsetY + nRow * (m_ItemInfo.nHeight + m_nItemOffsetY);
            if (pItem->Create(this, m_ItemInfo, nOffsetX, nOffsetY))
            {
                AddChild(pItem);
            }
            else
            {
                SafeDelete(pItem);
            }
        }
    }
    */

    return true;
}


bool CGUIListCtrl::SaveSelfToXML(CMarkupSTL& xml )
{
	CGUIElement::SaveSelfToXML(xml);
	xml.AddAttrib( "col", GetString("%d",m_nCol) );
	xml.AddAttrib( "row", GetString("%d",m_nRow) );
	xml.AddAttrib( "ItemOffsetX", GetString("%d",m_nItemOffsetX));
	xml.AddAttrib( "ItemOffsetY", GetString("%d",m_nItemOffsetY));
	xml.AddAttrib( "ItemWidth", GetString("%d",m_ItemInfo.nWidth) );
	xml.AddAttrib( "ItemHeight", GetString("%d",m_ItemInfo.nHeight) );
	xml.AddAttrib( "ItemImgSrc", GetString("%s",m_ItemInfo.strImgSrc.c_str()) );
	xml.AddAttrib( "ItemHoverImgSrc", GetString("%s",m_ItemInfo.strHoverImgSrc.c_str()) );
	xml.AddAttrib( "ItemUnuseImgSrc", GetString("%s",m_ItemInfo.strUnuseImgSrc.c_str()) );
    xml.AddAttrib( "ItemSelectedImgSrc", GetString("%s",m_ItemInfo.strSelectedImgSrc.c_str()) );
    xml.AddAttrib( "ItemLockImgSrc", GetString("%s", m_ItemInfo.strLockImgSrc.c_str()));
    if (m_bItemDragable)
    {
        xml.AddAttrib( "ItemDragable", "TRUE" );
    }
    if (m_bItemDropable)
    {
        xml.AddAttrib( "ItemDropable", "TRUE" );
    }
    
	return true;
}


bool CGUIListCtrl::OnLButtonDown(int x, int y)
{
    for (GUIELEMENT_ITER it = m_lstChildren.begin(); it != m_lstChildren.end(); ++it)
    {
        CGUIElement* pChild = *it;
        if (pChild->IsInRect(x, y) && pChild->IsVisable())
        {
            CGUIListItem* pItem = dynamic_cast<CGUIListItem*>(pChild);
            if (pItem)
            {
                if (m_pSelectedItem)
                {
                    m_pSelectedItem->SetSelected(false);
                }
                m_pSelectedItem = pItem;
                m_pSelectedItem->SetSelected(true);
            }
            //return pItem->OnLButtonDown(x, y);
        }
    }

    return OnLButtonDownSelf(x, y);
}


void CGUIListCtrl::SetRow(unsigned int nRow)
{
    if (nRow != m_nRow)
    {
        m_nRow = nRow;
        CreateItems();
    }
    
}

void CGUIListCtrl::SetCol(unsigned int nCol)
{
    if (nCol != m_nCol)
    {
        m_nCol = nCol;
        CreateItems();
    }
    
}


void CGUIListCtrl::SetItemsSize(unsigned nRow, unsigned nCol)
{
    m_nRow = nRow;
    m_nCol = nCol;
    CreateItems();
}

void CGUIListCtrl::SetItemOffsetX(int nOffsetX)
{
    m_nItemOffsetX = nOffsetX;
}

void CGUIListCtrl::SetItemOffsetY(int nOffsetY)
{
    m_nItemOffsetY = nOffsetY;
}

void CGUIListCtrl::SetItemInfo(const GUILIST_ITEM& item)
{
    m_ItemInfo = item;
    UpdateItemData();
}

void CGUIListCtrl::UpdateItemData()
{
    AIRString strImageSet;
    AIRString strImageUnit;
    CStringUtil::SplitFilename(m_ItemInfo.strImgSrc, strImageSet, strImageUnit);
    CImageSet* pImageSet = m_pScheme->GetImageSet(strImageSet);

    if (pImageSet)
    {
        if (m_renderQuad.m_pTexture == NULL)
        {
            m_renderQuad.m_pTexture = pImageSet->GetTexture();
        }
    }
    
    CImageUnit* pUnitItem = m_pScheme->GetImageUnit(strImageSet, strImageUnit);
    if (pUnitItem)
    {
        m_uvItem = pUnitItem->m_fUV;
    }

    CStringUtil::SplitFilename(m_ItemInfo.strHoverImgSrc, strImageSet, strImageUnit);
    CImageUnit* pItemHover = m_pScheme->GetImageUnit(strImageSet, strImageUnit);
    if (pItemHover)
    {
        m_uvItemHover = pItemHover->m_fUV;
    }
    //else
    //    m_uvItemHover = m_uvItem;

    CStringUtil::SplitFilename(m_ItemInfo.strSelectedImgSrc, strImageSet, strImageUnit);
    CImageUnit* pItemSelected = m_pScheme->GetImageUnit(strImageSet, strImageUnit);
    if (pItemSelected)
    {
        m_uvItemSelected = pItemSelected->m_fUV;
        m_bItemHasSelectedImage = true;
    }
    else
    {
        m_uvItemSelected = m_uvItem;
        m_bItemHasSelectedImage = false;
    }

    CStringUtil::SplitFilename(m_ItemInfo.strUnuseImgSrc, strImageSet, strImageUnit);
    CImageUnit* pItemUnused = m_pScheme->GetImageUnit(strImageSet, strImageUnit);
    if (pItemUnused)
    {
        m_uvItemUnused = pItemUnused->m_fUV;
    }
    else
    {
        m_uvItemUnused = Rectf::ZERO;
    }

    CStringUtil::SplitFilename(m_ItemInfo.strLockImgSrc, strImageSet, strImageUnit);
    CImageUnit* pItemLock = m_pScheme->GetImageUnit(strImageSet, strImageUnit);
    if (pItemUnused)
    {
        m_uvItemLock = pItemLock->m_fUV;
    }
    else
    {
        m_uvItemLock = Rectf::ZERO;
    }

    CaculateItemsPosition();
}

CGUIListItem* CGUIListCtrl::GetItem(int nRow, int nCol)
{
    if ( m_lstChildren.size() == 0 )
    {
        return NULL;
    }
    size_t nDis = nRow * m_nCol + nCol;
    GUIELEMENT_ITER it = m_lstChildren.begin();
    std::advance(it, nDis);
    if (it == m_lstChildren.end())
    {
        return NULL;
    }
    CGUIElement* pChild = *it;
    return static_cast<CGUIListItem*>(pChild);
}

CGUIListItem* CGUIListCtrl::GetItem(int nIndex)
{
    if ( m_lstChildren.size() == 0 )
    {
        return NULL;
    }
    size_t nDis = nIndex;
    GUIELEMENT_ITER it = m_lstChildren.begin();
    std::advance(it, nDis);
    if (it == m_lstChildren.end())
    {
        return NULL;
    }
    CGUIElement* pChild = *it;
    return static_cast<CGUIListItem*>(pChild);
}

void CGUIListCtrl::CreateItems()
{
    ClearChild();
    m_pSelectedItem = NULL;
    for (u32 nRow = 0; nRow < m_nRow; nRow++)
    {
        for (u32 nCol = 0; nCol < m_nCol; nCol++)
        {
            CGUIListItem* pItem = new CGUIListItem;
            int nOffsetX = m_nItemOffsetX + nCol * (m_ItemInfo.nWidth + m_nItemOffsetX);
            int nOffsetY = m_nItemOffsetY + nRow * (m_ItemInfo.nHeight + m_nItemOffsetY);
            if (pItem->Create(this, m_ItemInfo, nOffsetX, nOffsetY))
            {
                pItem->SetIndex(nRow * m_nCol + nCol);
                pItem->SetFont(m_pItemFont);
                AddChild(pItem);
            }
            else
            {
                SafeDelete(pItem);
            }
        }
    }
}

void CGUIListCtrl::CaculateItemsPosition()
{
    int nTopLeftX = 0;
    int nTopLeftY = 0;
    for (u32 nRow = 0; nRow < m_nRow; ++nRow)
    {
        for (u32 nCol = 0; nCol < m_nCol; ++nCol)
        {
            CGUIListItem* pItem = GetItem(nRow, nCol);
            if ( pItem )
            {
                nTopLeftX = m_nItemOffsetX + nCol * (m_ItemInfo.nWidth + m_nItemOffsetX) + m_renderQuad.m_rectScreen.left;
                nTopLeftY = m_nItemOffsetY + nRow * (m_ItemInfo.nHeight + m_nItemOffsetY) + m_renderQuad.m_rectScreen.top;
                pItem->SetRect(Rectf(nTopLeftX, nTopLeftY, nTopLeftX + m_ItemInfo.nWidth, nTopLeftY + m_ItemInfo.nHeight));
            }
        }
    }
}

void CGUIListCtrl::RenderChild()
{
    CGUIElement::RenderChild();

    CGUIListItem* pItem = NULL;
    for (GUIELEMENT_ITER it = m_lstChildren.begin(); it != m_lstChildren.end(); ++it)
    {
        pItem = static_cast<CGUIListItem*>(*it);
        pItem->RenderText();
    }
}

void CGUIListCtrl::SetItemFont(IFontBase* pFont)
{
    m_pItemFont = pFont;
    CGUIListItem* pItem = NULL;
    for (GUIELEMENT_ITER it = m_lstChildren.begin(); it != m_lstChildren.end(); ++it)
    {
        pItem = static_cast<CGUIListItem*>(*it);
        pItem->SetFont(m_pItemFont);
    }
}

void CGUIListCtrl::SetItemFont(const FONTTYPE& ft)
{
    m_pItemFont = FONTMGR->CreateFontW(ft, L"0");
    CGUIListItem* pItem = NULL;
    for (GUIELEMENT_ITER it = m_lstChildren.begin(); it != m_lstChildren.end(); ++it)
    {
        pItem = static_cast<CGUIListItem*>(*it);
        pItem->SetFont(m_pItemFont);
    }
}

bool CGUIListCtrl::ImportListFile(const char* szFilename)
{
    CDataStream* pStream = FILESYSTEM->GetFileStream(szFilename);
    // 获取数据失败,返回
    if (pStream == NULL)
    {
        return false;
    }

    // 获取XML节点树
    AIRString strDoc = pStream->GetAsString();
    CMarkupSTL xml(strDoc.c_str());;

    // 如果XML没有信息,返回
    if (!xml.FindElem())
    {
        SafeDelete(pStream);
        return false;
    }

    AIRString strValue;

    //strValue = xml.GetAttrib("");


    xml.IntoElem();
    int i = 0;
    while (xml.FindElem())
    {
        CGUIListItem* pItem = GetItem(i++);
        if (xml.GetTagName() == "CGUIAnimation")
        {
            CGUIAnimation* pAnimation = new CGUIAnimation;
            if (pAnimation->Create(NULL, xml))
            {
                pAnimation->SetWidth(pItem->GetWidth());
                pAnimation->SetHeight(pItem->GetHeight());
                pItem->SetContentElement(pAnimation);
            }
        }
        //pItem->SetContentElement();
    }

    xml.OutOfElem();

    SafeDelete(pStream);

    return true;
}

void CGUIListCtrl::SetOnDropFun(OnDropCallBack pFun)
{
    m_pOnDropFun = pFun;
}

void CGUIListCtrl::SetDragImage(int nCol, int nRow, CGUIDragImage* pImage)
{
    CGUIListItem* pItem = GetItem(nRow, nCol);
    pItem->AttachDragImage(pImage);
}

void CGUIListCtrl::LoadDragImages(const char* szFilename)
{
    CDataStream* pStream = FILESYSTEM->GetFileStream(szFilename);
    //AIRString strFilename = FILESYSTEM->GetFileAbsolutePath(szFilename);
    if (pStream == NULL)
    {
        return;
    }
    AIRString strDoc = pStream->GetAsString();

    CMarkupSTL xml(strDoc.c_str());

    //第一步，进入根结点
    if (!xml.FindElem())
    {
        SafeDelete(pStream);
        return;
    }
    
    xml.IntoElem();

    AIRString strValue;

    while (xml.FindElem("Image"))
    {
        strValue = xml.GetAttrib("Imagesrc").c_str();
        CGUIDragImage* pDragImage = new CGUIDragImage;
        pDragImage->SetImageSrc(strValue);

        strValue = xml.GetAttrib("row").c_str();
        int nRow = atoi(strValue.c_str());
        strValue = xml.GetAttrib("col").c_str();
        int nCol = atoi(strValue.c_str());
        SetDragImage(nCol, nRow, pDragImage);
    }

    xml.OutOfElem();

    SafeDelete(pStream);
}

void CGUIListCtrl::PlusRow()
{
    m_nRow++;
    CreateItems();
}

void CGUIListCtrl::PlusCol()
{
    m_nCol++;
    CreateItems();
}

void CGUIListCtrl::DecRow()
{
    if (m_nRow > 0 )
    {
        m_nRow--;
        CreateItems();
    }
}

void CGUIListCtrl::DecCol()
{
    if (m_nCol > 0)
    { 
        m_nCol--;
        CreateItems();
    }
}

void CGUIListCtrl::AddItem(CGUIListItem* pItem)
{
    pItem->SetSchemeManager(m_pScheme);
    AddChild(pItem);
    CaculateItemsPosition();
}
