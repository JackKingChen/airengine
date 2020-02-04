// ***************************************************************
//  文件: GUIMenu.h
//  日期: 2010-3-15
//  作者: liangairan
//  说明: 弹出菜单控件，并且能自适应大小
// ***************************************************************
#pragma once
#include "GUIAutoSizeFrame.h"
#include "GUIListCtrl.h"

typedef struct tagMenuItemInfo
{
    AIRString strImgSrc;           //背景图
    AIRString strHoverImgSrc;      //hover纹理
    int nWidth;
    int nHeight;
    ITexture* pItemTexture;          //item的纹理
    Rectf  uvNormal;                 //item正常状态下的uv坐标
    Rectf  uvHover;                  //鼠标经过的图元
    SColor    m_clrHover;            //鼠标经过的blend颜色

    tagMenuItemInfo() : nWidth(0), nHeight(0), pItemTexture(NULL)//, m_clrHover(128, 255, 255, 255)
    {

    }
}MENUITEM_INFO, *LPMENUITEM_INFO;

class CGUIMenu;

class AIRGuiExport CMenuItem : public CGUIStaticText
{
public:
    CMenuItem();
    virtual ~CMenuItem();

    virtual bool Create(CGUIMenu* pParent, const MENUITEM_INFO& menuItemInfo, int nOffsetX, int nOffsetY);

    //从XML中创建
    virtual bool CreateFromXML(CGUIElement* pParent, CMarkupSTL& xml);

    void SetGUIMenuItemInfo(const LPMENUITEM_INFO pItemInfo);

    //设置索引值
    void SetIndex(int nIndex);

protected:
    virtual bool CreateSelf(CGUIElement* pParent, CMarkupSTL& xml);

    virtual bool SaveSelfToXML(CMarkupSTL& xml );

    virtual bool OnMouseMoveSelf(int x, int y);

    virtual bool OnLButtonUpSelf(int x, int y);

    virtual void RenderSelf();

    virtual void OnStatusChange();
protected:
    const MENUITEM_INFO* m_pItemInfo;
    int          m_nIndex;    //索引号
    WAIRString m_wcText;    //item的字体
private:
};

class AIRGuiExport CGUIMenu :
    public CGUIAutoSizeFrame
{
public:
    CGUIMenu(void);
    virtual ~CGUIMenu(void);

    /*
    //设置左上角的图
    virtual void SetImgSrcTopLeft(const AIRString& strImgSrc);

    //设置上方的图
    virtual void SetImgSrcTop(const AIRString& strImgSrc);

    //设置右上角的图
    virtual void SetImgSrcTopRight(const AIRString& strImgSrc);

    //设置右边的图
    virtual void SetImgSrcRight(const AIRString& strImgSrc);

    virtual void SetImgSrcBottomRight(const AIRString& strImgSrc);

    virtual void SetImgSrcBottom(const AIRString& strImgSrc);

    virtual void SetImgSrcBottomLeft(const AIRString& strImgSrc);

    virtual void SetImgSrcLeft(const AIRString& strImgSrc);

    //设置菜单中间的图
    virtual void SetImgSrcCenter(const AIRString& strImgSrc);
    */

    //触发显示的屏幕坐标
    virtual void SetShowPosition(int x, int y);

    //设置字体
    virtual void SetItemFont(const FONTTYPE& ft);

    //设置子项数量
    void SetItemsCount(int nItemsCount);

    //设置item的文本
    void SetItemText(int nIndex, const char* szText);

    //设置item是否可见
    void SetItemVisable(int nIndex, bool bVisable);

    //设置item的宽度
    void SetItemWidth(int nWidth);

    //设计item的高度
    void SetItemHeight(int nHeight);

    /*
    const AIRString& GetImgSrcTopLeft() const
    {
        return m_strImgSrcTopLeft;
    }

    const AIRString& GetImgSrcTop() const
    {
        return m_strImgSrcTop;
    }

    const AIRString& GetImgSrcTopRight() const
    {
        return m_strImgSrcTopRight;
    }

    const AIRString& GetImgSrcRight() const
    {
        return m_strImgSrcRight;
    }

    const AIRString& GetImgSrcBottomRight() const
    {
        return m_strImgSrcBottomRight;
    }

    const AIRString& GetImgSrcBottom() const
    {
        return m_strImgSrcBottom;
    }

    const AIRString& GetImgSrcBottomLeft() const
    {
        return m_strImgSrcBottomLeft;
    }

    const AIRString& GetImgSrcLeft() const
    {
        return m_strImgSrcLeft;
    }

    const AIRString& GetImgSrcCenter() const
    {
        return m_strImgSrcCenter;
    }
    */

    int GetItemWidth() const
    {
        return m_menuItemInfo.nWidth;
    }

    int GetItemHeight() const
    {
        return m_menuItemInfo.nHeight;
    }

    int GetItemsNum() const
    {
        return m_nItemsNum;
    }

    IFontBase* GetFont() const
    {
        return m_pItemFont;
    }

    virtual void CreateDefault();

    virtual void SetRect(const Rectf& rect);

    void SetItemImageSrc(const AIRString& strImgSrc);

    void SetItemHoverImgSrc(const AIRString& strImgSrc);

    void SetItemHoverColor(const SColor& color);

    const MENUITEM_INFO& GetItemInfo() const
    {
        return m_menuItemInfo;
    }
    //virtual bool Create(CGUIElement* pParent, CMarkupSTL& xml);

    virtual bool OnMouseMove(int x, int y);

    virtual void Move(int dx, int dy);

    //设置item鼠标左键弹出事件
    virtual void SetItemLButtonUpCallBack(size_t nIndex, LButtonUpCallBack pFun);

    CMenuItem* GetMenuItem(size_t nIndex);

    virtual bool OnLButtonDown(int x, int y);

    virtual bool OnLButtonUp(int x, int y);

    //删除菜单项
    void DeleteMenuItem(size_t nIndex);
protected:
    virtual bool CreateSelf(CGUIElement* pParent, CMarkupSTL& xml);

    virtual bool CreateChild(CMarkupSTL& parent);

    virtual void ClearSelf();

    virtual void ClearItem();
    //计算menu的大小，自适应
    void CaculateMenuSize();

    void CreateMenuItems();

    virtual bool SaveSelfToXML(CMarkupSTL& xml );

    virtual bool SaveChildToXML(CMarkupSTL& xml );

    //渲染子控件
    virtual void RenderChild();

    

    //鼠标经过
    //virtual bool OnMouseMoveSelf(int x, int y);

protected:
    /*
    AIRString m_strImgSrcTopLeft;
    AIRString m_strImgSrcTop;
    AIRString m_strImgSrcTopRight;
    AIRString m_strImgSrcRight;
    AIRString m_strImgSrcBottomRight;
    AIRString m_strImgSrcBottom;
    AIRString m_strImgSrcBottomLeft;
    AIRString m_strImgSrcLeft;
    AIRString m_strImgSrcCenter;

    CGUIImage*  m_pTopLeftImg;
    CGUIImage*  m_pTopImg;
    CGUIImage*  m_pTopRightImg;
    CGUIImage*  m_pRightImg;
    CGUIImage*  m_pBottomRightImg;
    CGUIImage*  m_pBottomImg;
    CGUIImage*  m_pBottomLeftImg;
    CGUIImage*  m_pLeftImg;
    CGUIImage*  m_pCenterImg;     //中间图
    */

    IFontBase*   m_pItemFont;        //Item的字体

    int  m_nItemsNum;      //menuitem的数量

    typedef AIRVector<CMenuItem*> MenuItemArray;
    typedef MenuItemArray::iterator MenuItemArray_Iter;
    MenuItemArray  m_vtMenuItems;

    MENUITEM_INFO m_menuItemInfo;
    //CGUIListCtrl*  m_pMenuItem;
};
