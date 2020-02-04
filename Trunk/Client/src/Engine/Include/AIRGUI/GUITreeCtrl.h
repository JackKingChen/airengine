// ***************************************************************
//  �ļ�: GUITreeCtrl.h
//  ����: 2010-3-18
//  ����: liangairan
//  ˵��: ���ؼ�
// ***************************************************************
#pragma once
#include "guitreeitem.h"

typedef bool (*TreeItemRButtonUpCallBack)(CGUITreeItem*);

typedef bool (*TreeItemLButtonUpCallBack)(CGUITreeItem*);

typedef bool (*TreeItemLButtonDBClickCallBack)(CGUITreeItem*);

class AIRGuiExport CGUITreeCtrl :
    public CGUIElement
{
public:
    friend class CGUITreeItem;
public:
    CGUITreeCtrl(void);
    virtual ~CGUITreeCtrl(void);

    void SetItemExpandBtnCheckImgSrc(const AIRString& strImgSrc);

    void SetItemExpandBtnUnCheckImgSrc(const AIRString& strImgSrc);

    //.����Item����ͼ
    void SetItemBackgroundImgSrc(const AIRString& strImgSrc);

    void SetItemFont(const FONTTYPE& ft);

    const TREEITEMINFO& GetTreeItemInfo() const
    {
        return m_treeItemInfo;
    }

    //���Ҷ��
    void AddItem(const char* szText, CGUITreeItem* pParent);

    //���Ҷ��
    //nParentIndex ���׵����
    //void AddItem(const char* szText, int nParentIndex);

    //ɾ��Ҷ��
    void DeleteItem(CGUITreeItem* pItem, CGUITreeItem* pParent);

    //����item���ı�
    void SetItemText(size_t index, const char* szText);

    virtual void CreateDefault();

    //����������
    virtual bool OnLButtonDown(int x, int y);

    //����Ҽ�
    virtual bool OnRButtonDown(int x, int y);

    //����Ҽ��ɿ�
    virtual bool OnRButtonUp(int x, int y);

    //����item�ĸ߶�
    void   SetItemHeight(int nHeight);

    //����ѡ�е�����
    void   SetSelectedItem(CGUITreeItem* pItem);

    virtual void SetRect(const Rectf& rect);

    //���Item
    CGUITreeItem* GetTreeItem(size_t index);
    CGUITreeItem* GetSelectedItem()
    {
        return m_pSelectedItem;
    }

    //����Item���������ϻص�����
    void SetTreeItemLButtonUpFun(TreeItemLButtonUpCallBack pFun);

    //����Item����Ҽ����ϻص�����
    void SetTreeItemRButtonUpFun(TreeItemRButtonUpCallBack pFun);

    //����Item������˫���¼�
    void SetTreeItemLButtonDBClickFun(TreeItemLButtonDBClickCallBack pFun);

    virtual bool OnLButtonDBClick(int x, int y);

protected:
    //��ÿ��ӷ�Χ�ڵ�item
    void GetVisableItems();

    virtual void RenderChild();

    //����ʵ�ʴ�С
    virtual void CaculateRealSize();

    //����Item��λ��
    virtual void CaculateItemsPosition();

    //����һ��item
    CGUITreeItem* CreateItem(const char* szText);

    virtual bool SaveSelfToXML(CMarkupSTL& xml );

    virtual bool CreateSelf(CGUIElement* pParent, CMarkupSTL& xml);

protected:
    CGUITreeItem*  m_pSelectedItem;
    TREEITEMINFO m_treeItemInfo;

    CGUITreeItem::TreeItemList m_lstVisableItems;   //��Ҫ�Լ�

    TreeItemRButtonUpCallBack m_pTreeItemRButtonUp;
    TreeItemLButtonUpCallBack m_pTreeItemLButtonUp;
    TreeItemLButtonDBClickCallBack   m_pTreeItemLButtonDBClick;
};
