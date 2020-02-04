/********************************************************************
	created:	2009/11/17
	created:	17:11:2009   11:00
	filename: 	d:\My Codes\Engine\AIRGUI\GUIListCtrl.h
	file path:	d:\My Codes\Engine\AIRGUI
	file base:	GUIListCtrl
	file ext:	h
	author:		liangairan
	
	purpose:	ListCtrl�Ŀؼ���
*********************************************************************/
#pragma once
#include "GUIListItem.h"

//�ϷŵĻص�����
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

    //���ĳ��Item
    //@nRow   �к�
    //@nCol   �к�
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

    //�����б��ļ�
    bool ImportListFile(const char* szFilename);

    //���ûص�����
    void SetOnDropFun(OnDropCallBack pFun);

    //���ÿ��϶�ͼƬ
    void SetDragImage(int nCol, int nRow, CGUIDragImage* pImage);

    //���ļ�����
    void LoadDragImages(const char* szFilename);

    //�����
    void PlusRow();

    //�����
    void PlusCol();

    //�ݼ���
    void DecRow();

    //�ݼ���
    void DecCol();

    //���item
    void AddItem(CGUIListItem* pItem);

protected:
    //����item
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
    int  m_nItemOffsetX;     //��һ��Item����Լ���ƫ����X
    int  m_nItemOffsetY;     //��һ��Item����Լ���ƫ����Y

    IFontBase*   m_pItemFont;        //Item������
    Rectf        m_uvItem;
    Rectf        m_uvItemHover;    
    Rectf        m_uvItemSelected;
    Rectf        m_uvItemUnused;
    Rectf        m_uvItemLock;            //����״̬

    bool         m_bItemHasSelectedImage;
    bool         m_bItemDragable;     //�Ƿ����
    bool         m_bItemDropable;     //�Ƿ�ɷ�

    int          m_nDragItemType;     //�϶������ͣ�ע�⣬�����һ��ctrl�а�һ������Ž���һ��ctrl��
                                      //������ctrl��������;ͱ���һ����
    OnDropCallBack   m_pOnDropFun;

	GUILIST_ITEM m_ItemInfo;
};
