// ***************************************************************
//  文件: GUITreeItem.h
//  日期: 2010-3-18
//  作者: liangairan
//  说明: 树控件的树叶控件
// ***************************************************************
#pragma once
#include "guicheckbox.h"
#include "GUIStaticText.h"

class CGUITreeCtrl;

typedef struct tagTreeItemInfo 
{
    AIRString strExpandBtnUnCheckImgSrc;
    AIRString strExpandBtnCheckImgSrc;
    AIRString strItemBackgroundImgSrc;     //item背景图片
    IFontBase*  pFont; 
    int         m_nItemHeight;
    SColor      m_clrItemSelected;
    tagTreeItemInfo() : pFont(NULL), m_nItemHeight(0), m_clrItemSelected(128, 255, 255, 255)
    {

    }
}TREEITEMINFO;

class AIRGuiExport CGUITreeItem :  public CGUIElement
{
public:
    typedef std::list<CGUITreeItem*> TreeItemList;
    typedef TreeItemList::iterator TreeItemList_Iter;
public:
    CGUITreeItem(void);
    virtual ~CGUITreeItem(void);

    //增加树叶
    void    AddChildItem(const char* szText);

    void    AddChildItem(CGUITreeItem* pItem);

    //删除树叶
    void    DeleteChildItem(CGUITreeItem* pItem);

    //void    DeleteChildItem(size_t index);

    //设置text
    void    SetText(const char* szText);

    //获得某个树叶
    CGUITreeItem*  GetSelectedItem()
    {
        return m_pSelectedItem;
    }

    CGUITreeItem*  GetChildItem(const char* szText);

    //通过序号找到子item
    CGUITreeItem*  GetChildItem(int nIndex);

    const char* GetText() const
    {
        return m_pStaticText->GetText();
    }

    virtual void Move(int dx, int dy);

    virtual void CreateDefault();

    void SetFont(IFontBase* pFont);

    void SetExpandBtnImgSrcCheck(const AIRString& strImgSrc);

    void SetExpandBtnImgSrcUnCheck(const AIRString& strImgSrc);


    virtual void CleanUp();

    virtual void GetVisableItems(const Rectf& rect, TreeItemList& lstVisable);

    //设置高度
    void  SetItemHeight(int nHeight);

    void  SetOwner(CGUITreeCtrl* pTreeCtrl);

    void  SetSelected(bool bSelected);

    virtual void SetRect(const Rectf& rect);

    virtual void SetRect(int x, int y, int nWidth, int nHeight);

    CGUITreeItem* GetParentItem()
    {
        return m_pParentItem;
    }
protected:
    virtual bool CreateSelf(CGUIElement* pParent, CMarkupSTL& xml);

    virtual void RenderSelf();

    //展开
    void Expand();

    //收埋
    void UnExpand();

    virtual bool OnLButtonDownSelf(int x, int y);

    //计算子item的位置
    virtual void CaculateChildrenItemPosition();
protected:
    CGUICheckBox*       m_pBtnExpand;
    CGUIStaticText*     m_pStaticText;
    CGUITreeItem*       m_pSelectedItem;
    CGUITreeCtrl*       m_pOwner;    //所属的treectrl
    CGUITreeItem*       m_pParentItem;   //父结点

    bool                m_bExpanded;   //是否展开
    bool                m_bSelected;   //是否被选择

    

    TreeItemList        m_lstTreeItems;   //

    
    //static TREEITEMINFO m_treeItemInfo;
};
