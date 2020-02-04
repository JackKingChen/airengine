// ***************************************************************
//  文件: GUITreeCtrl.h
//  日期: 2010-3-18
//  作者: liangairan
//  说明: 树控件
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

    //.设置Item背景图
    void SetItemBackgroundImgSrc(const AIRString& strImgSrc);

    void SetItemFont(const FONTTYPE& ft);

    const TREEITEMINFO& GetTreeItemInfo() const
    {
        return m_treeItemInfo;
    }

    //添加叶子
    void AddItem(const char* szText, CGUITreeItem* pParent);

    //添加叶子
    //nParentIndex 父亲的序号
    //void AddItem(const char* szText, int nParentIndex);

    //删除叶子
    void DeleteItem(CGUITreeItem* pItem, CGUITreeItem* pParent);

    //设置item的文本
    void SetItemText(size_t index, const char* szText);

    virtual void CreateDefault();

    //鼠标左键按下
    virtual bool OnLButtonDown(int x, int y);

    //鼠标右键
    virtual bool OnRButtonDown(int x, int y);

    //鼠标右键松开
    virtual bool OnRButtonUp(int x, int y);

    //设置item的高度
    void   SetItemHeight(int nHeight);

    //设置选中的物体
    void   SetSelectedItem(CGUITreeItem* pItem);

    virtual void SetRect(const Rectf& rect);

    //获得Item
    CGUITreeItem* GetTreeItem(size_t index);
    CGUITreeItem* GetSelectedItem()
    {
        return m_pSelectedItem;
    }

    //设置Item鼠标左键向上回调函数
    void SetTreeItemLButtonUpFun(TreeItemLButtonUpCallBack pFun);

    //设置Item鼠标右键向上回调函数
    void SetTreeItemRButtonUpFun(TreeItemRButtonUpCallBack pFun);

    //设置Item鼠标左键双击事件
    void SetTreeItemLButtonDBClickFun(TreeItemLButtonDBClickCallBack pFun);

    virtual bool OnLButtonDBClick(int x, int y);

protected:
    //获得可视范围内的item
    void GetVisableItems();

    virtual void RenderChild();

    //计算实际大小
    virtual void CaculateRealSize();

    //计算Item的位置
    virtual void CaculateItemsPosition();

    //创建一个item
    CGUITreeItem* CreateItem(const char* szText);

    virtual bool SaveSelfToXML(CMarkupSTL& xml );

    virtual bool CreateSelf(CGUIElement* pParent, CMarkupSTL& xml);

protected:
    CGUITreeItem*  m_pSelectedItem;
    TREEITEMINFO m_treeItemInfo;

    CGUITreeItem::TreeItemList m_lstVisableItems;   //需要栽剪

    TreeItemRButtonUpCallBack m_pTreeItemRButtonUp;
    TreeItemLButtonUpCallBack m_pTreeItemLButtonUp;
    TreeItemLButtonDBClickCallBack   m_pTreeItemLButtonDBClick;
};
