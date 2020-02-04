// ***************************************************************
//  �ļ�: GUITreeItem.h
//  ����: 2010-3-18
//  ����: liangairan
//  ˵��: ���ؼ�����Ҷ�ؼ�
// ***************************************************************
#pragma once
#include "guicheckbox.h"
#include "GUIStaticText.h"

class CGUITreeCtrl;

typedef struct tagTreeItemInfo 
{
    AIRString strExpandBtnUnCheckImgSrc;
    AIRString strExpandBtnCheckImgSrc;
    AIRString strItemBackgroundImgSrc;     //item����ͼƬ
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

    //������Ҷ
    void    AddChildItem(const char* szText);

    void    AddChildItem(CGUITreeItem* pItem);

    //ɾ����Ҷ
    void    DeleteChildItem(CGUITreeItem* pItem);

    //void    DeleteChildItem(size_t index);

    //����text
    void    SetText(const char* szText);

    //���ĳ����Ҷ
    CGUITreeItem*  GetSelectedItem()
    {
        return m_pSelectedItem;
    }

    CGUITreeItem*  GetChildItem(const char* szText);

    //ͨ������ҵ���item
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

    //���ø߶�
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

    //չ��
    void Expand();

    //����
    void UnExpand();

    virtual bool OnLButtonDownSelf(int x, int y);

    //������item��λ��
    virtual void CaculateChildrenItemPosition();
protected:
    CGUICheckBox*       m_pBtnExpand;
    CGUIStaticText*     m_pStaticText;
    CGUITreeItem*       m_pSelectedItem;
    CGUITreeCtrl*       m_pOwner;    //������treectrl
    CGUITreeItem*       m_pParentItem;   //�����

    bool                m_bExpanded;   //�Ƿ�չ��
    bool                m_bSelected;   //�Ƿ�ѡ��

    

    TreeItemList        m_lstTreeItems;   //

    
    //static TREEITEMINFO m_treeItemInfo;
};
