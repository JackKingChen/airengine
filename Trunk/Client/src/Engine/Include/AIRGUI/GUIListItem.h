/********************************************************************
	created:	2009/11/17
	created:	17:11:2009   14:15
	filename: 	d:\My Codes\Engine\AIRGUI\GUIListItem.h
	file path:	d:\My Codes\Engine\AIRGUI
	file base:	GUIListItem
	file ext:	h
	author:		liangairan
	
	purpose:	放在ListCtrl中的Item，可以把Icon放到Item中去，
                在游戏中用于物品栏，快
*********************************************************************/
#pragma once
#include "GUIStaticText.h"
//#include "IFontBase.h"

class CGUIDragImage;

enum eGUIItem_State
{
    eItem_Normal,   //正常状态
    eItem_Hover,    //鼠标盘旋状态
    eItem_Locked,   //锁定状态
    eItem_UnUsed,
    //eItem_Selected, //选中状态
    eItem_Cool,     //冷却状态
};

typedef struct tagGUIListItem 
{
    tagGUIListItem()  : sSelectedColor(0xffffffff),
        nSelectedFrameWidth(1),
        nWidth(10),
        nHeight(10)
        //m_pFont(NULL)
    { 
    }
    
	AIRString strImgSrc;           //背景图
    AIRString strHoverImgSrc;      //hover纹理
    AIRString strUnuseImgSrc;      //不可用时的纹理
    AIRString strSelectedImgSrc;   //选中时图元
    AIRString strLockImgSrc;       //锁定时图元
    SColor      sSelectedColor;      //选中的颜色
    int         nSelectedFrameWidth; //选中时的矩形边宽
    int         nWidth;
    int         nHeight;
    //IFontBase*  m_pFont;
    //SColor      m_clrFont;
}GUILIST_ITEM;

class CGUIListCtrl;

class AIRGuiExport CGUIListItem : public CGUIStaticText
{
public:
    CGUIListItem(void);
    virtual ~CGUIListItem(void);

    //创建Item
    virtual bool Create(CGUIListCtrl* pParent, const GUILIST_ITEM& itemData, int nParentOffsetX, int nParentOffsetY);

    //virtual bool Create(CGUIElement* pParent, CMarkupSTL& xml);

	virtual bool SaveSelfToXML(CMarkupSTL& xml );

    virtual void SetCurrentUIState(GUIELEMENT_STATE state);

    //设置冷却时间，单位秒
    virtual void SetCoolTime(u32 dwTime);

    virtual void SetSelected(bool bSelected);

    //渲染Item中的字体
    virtual void RenderText();

    int GetIndex() const
    {
        return m_nIndex;
    }

    virtual void SetIndex(int nIndex);

    //把图片装到item中
    void OnDrop(CGUIDragImage* pImage);

    //开始拖动
    void OnDrag();

    virtual void Move(int dx, int dy);

    //锁定
    void Lock();

    //解锁
    void UnLock();

    virtual void CleanUp();

    void SetContentElement(CGUIElement* pElement);

    CGUIElement* GetContentElement()
    {
        return m_pImage;
    }
    //void SetAutoDeleteElement(bool bAutoDelete);
    //attach the drag image
    void AttachDragImage(CGUIDragImage* pDragImage);

protected:
    virtual void RenderSelf();

    virtual void UpdateSelf();

    virtual bool OnMouseMoveSelf(int x, int y);

    virtual bool OnLButtonDownSelf(int x, int y);

    virtual bool OnLButtonUpSelf(int x, int y);

    virtual void OnStatusChange();

    virtual bool OnDragSelf(int x, int y);

    
    //virtual bool CreateSelf(CGUIElement* pParent, CMarkupSTL& xml);

private:
    //IFontBase*   m_pFont;
    CGUIDragImage*         m_pDragImage;
    CGUIElement*           m_pImage;
    eGUIItem_State         m_byItemStatus;
    eGUIItem_State         m_byLastItemStatus;
    u32        m_dwCoolEndTime;    //冷却时间
    WAIRString m_wcText;   //item的字体，一般是数字
    bool         m_bSelected;
    int          m_nIndex;    //索引号

    bool         m_bAutoDeleteElement;

    CGUIListCtrl*  m_pListCtrl;

    CGUIStaticText*  m_pDetailText;   //详细信息

    //Rectf        m_uvNormal;
    //Rectf        m_uvHover;
    //Rectf        m_uvLock;            //锁定状态
    //Rectf        m_uvUnUsed;          //不可用状态
    //Rectf        m_uvSelected;
    //Rectf        m_uvUnused;
    
    const GUILIST_ITEM*     m_pItemData;
};
