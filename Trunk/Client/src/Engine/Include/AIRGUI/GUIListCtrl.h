/********************************************************************
	created:	2009/11/17
	created:	17:11:2009   11:00
	filename: 	d:\My Codes\Engine\AIRGUI\GUIListCtrl.h
	file path:	d:\My Codes\Engine\AIRGUI
	file base:	GUIListCtrl
	file ext:	h
	author:		liangairan
	
	purpose:	ListCtrl的控件类
*********************************************************************/
#pragma once
#include "GUIListItem.h"

//拖放的回调函数
typedef bool (*OnDropCallBack)(CGUIDragImage*);

class AIRGuiExport CGUIListCtrl :  public CGUIElement
{
public:
    friend class CGUIListItem;
public:
    CGUIListCtrl(void);
    virtual ~CGUIListCtrl(void);

    virtual bool Create(CGUIElement* pParent, CMarkupSTL& xml);

	virtual bool SaveSelfToXML(CMarkupSTL& xml );

    virtual bool OnLButtonDown(int x, int y);

    unsigned int     GetCol() 
    {
        return m_nCol;
    }

    unsigned int     GetRow()
    {
        return m_nRow;
    }

    int     GetItemOffsetX()
    {
        return m_nItemOffsetX;
    }

    int     GetItemOffsetY()
    {
        return m_nItemOffsetY;
    }

    GUILIST_ITEM*  GetItemInfoPtr()
    {
        return &m_ItemInfo;
    }

    virtual void SetCol(unsigned int nCol);

    virtual void SetRow(unsigned int nRow); 
    virtual void SetItemsSize(unsigned nRow, unsigned nCol);

    virtual void SetItemOffsetX(int nOffsetX);

    virtual void SetItemOffsetY(int nOffsetY);

    virtual void SetItemInfo(const GUILIST_ITEM& item);

    virtual void SetItemFont(IFontBase* pFont);

    virtual void SetItemFont(const FONTTYPE& ft);

    //获得某个Item
    //@nRow   行号
    //@nCol   列号
    virtual CGUIListItem* GetItem(int nRow, int nCol);

    CGUIListItem* GetItem(int nIndex);

    CGUIListItem* GetSelectedItem()
    {
        return m_pSelectedItem;
    }
    //virtual void AddItem(CGUI)

    bool IsItemDragable() const
    {
        return m_bItemDragable;
    }

    bool IsItemDropable() const
    {
        return m_bItemDropable;
    }

    void SetItemDragable(bool bDragable)
    {
        m_bItemDragable = bDragable;
    }

    void SetItemDropable(bool bDropable)
    {
        m_bItemDropable = bDropable;
    }

    //导入列表文件
    bool ImportListFile(const char* szFilename);

    //设置回调函数
    void SetOnDropFun(OnDropCallBack pFun);

    //设置可拖动图片
    void SetDragImage(int nCol, int nRow, CGUIDragImage* pImage);

    //从文件加载
    void LoadDragImages(const char* szFilename);

    //添加行
    void PlusRow();

    //添加列
    void PlusCol();

    //递减行
    void DecRow();

    //递减列
    void DecCol();

    //添加item
    void AddItem(CGUIListItem* pItem);

protected:
    //创建item
    virtual void CreateItems();

    virtual void UpdateItemData();

    virtual void CaculateItemsPosition();

    virtual void RenderChild();
    //virtual void RenderSelf();

    //virtual void UpdateSelf();

    //virtual bool OnLButtonDownSelf(int x, int y);

    //virtual bool OnMouseMoveSelf(int x, int y);
    CGUIListItem*  m_pSelectedItem;
protected:

    unsigned int  m_nCol;
    unsigned int  m_nRow;
    int  m_nItemOffsetX;     //第一个Item相对自己的偏移量X
    int  m_nItemOffsetY;     //第一个Item相对自己的偏移量Y

    IFontBase*   m_pItemFont;        //Item的字体
    Rectf        m_uvItem;
    Rectf        m_uvItemHover;    
    Rectf        m_uvItemSelected;
    Rectf        m_uvItemUnused;
    Rectf        m_uvItemLock;            //锁定状态

    bool         m_bItemHasSelectedImage;
    bool         m_bItemDragable;     //是否可拖
    bool         m_bItemDropable;     //是否可放

    int          m_nDragItemType;     //拖动的类型，注意，如果由一个ctrl中把一个物体放进另一个ctrl，
                                      //这两个ctrl中这个类型就必须一样。
    OnDropCallBack   m_pOnDropFun;

	GUILIST_ITEM m_ItemInfo;
};
