// ***************************************************************
//  �ļ�: GUIMenu.h
//  ����: 2010-3-15
//  ����: liangairan
//  ˵��: �����˵��ؼ�������������Ӧ��С
// ***************************************************************
#pragma once
#include "GUIAutoSizeFrame.h"
#include "GUIListCtrl.h"

typedef struct tagMenuItemInfo
{
    AIRString strImgSrc;           //����ͼ
    AIRString strHoverImgSrc;      //hover����
    int nWidth;
    int nHeight;
    ITexture* pItemTexture;          //item������
    Rectf  uvNormal;                 //item����״̬�µ�uv����
    Rectf  uvHover;                  //��꾭����ͼԪ
    SColor    m_clrHover;            //��꾭����blend��ɫ

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

    //��XML�д���
    virtual bool CreateFromXML(CGUIElement* pParent, CMarkupSTL& xml);

    void SetGUIMenuItemInfo(const LPMENUITEM_INFO pItemInfo);

    //��������ֵ
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
    int          m_nIndex;    //������
    WAIRString m_wcText;    //item������
private:
};

class AIRGuiExport CGUIMenu :
    public CGUIAutoSizeFrame
{
public:
    CGUIMenu(void);
    virtual ~CGUIMenu(void);

    /*
    //�������Ͻǵ�ͼ
    virtual void SetImgSrcTopLeft(const AIRString& strImgSrc);

    //�����Ϸ���ͼ
    virtual void SetImgSrcTop(const AIRString& strImgSrc);

    //�������Ͻǵ�ͼ
    virtual void SetImgSrcTopRight(const AIRString& strImgSrc);

    //�����ұߵ�ͼ
    virtual void SetImgSrcRight(const AIRString& strImgSrc);

    virtual void SetImgSrcBottomRight(const AIRString& strImgSrc);

    virtual void SetImgSrcBottom(const AIRString& strImgSrc);

    virtual void SetImgSrcBottomLeft(const AIRString& strImgSrc);

    virtual void SetImgSrcLeft(const AIRString& strImgSrc);

    //���ò˵��м��ͼ
    virtual void SetImgSrcCenter(const AIRString& strImgSrc);
    */

    //������ʾ����Ļ����
    virtual void SetShowPosition(int x, int y);

    //��������
    virtual void SetItemFont(const FONTTYPE& ft);

    //������������
    void SetItemsCount(int nItemsCount);

    //����item���ı�
    void SetItemText(int nIndex, const char* szText);

    //����item�Ƿ�ɼ�
    void SetItemVisable(int nIndex, bool bVisable);

    //����item�Ŀ��
    void SetItemWidth(int nWidth);

    //���item�ĸ߶�
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

    //����item�����������¼�
    virtual void SetItemLButtonUpCallBack(size_t nIndex, LButtonUpCallBack pFun);

    CMenuItem* GetMenuItem(size_t nIndex);

    virtual bool OnLButtonDown(int x, int y);

    virtual bool OnLButtonUp(int x, int y);

    //ɾ���˵���
    void DeleteMenuItem(size_t nIndex);
protected:
    virtual bool CreateSelf(CGUIElement* pParent, CMarkupSTL& xml);

    virtual bool CreateChild(CMarkupSTL& parent);

    virtual void ClearSelf();

    virtual void ClearItem();
    //����menu�Ĵ�С������Ӧ
    void CaculateMenuSize();

    void CreateMenuItems();

    virtual bool SaveSelfToXML(CMarkupSTL& xml );

    virtual bool SaveChildToXML(CMarkupSTL& xml );

    //��Ⱦ�ӿؼ�
    virtual void RenderChild();

    

    //��꾭��
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
    CGUIImage*  m_pCenterImg;     //�м�ͼ
    */

    IFontBase*   m_pItemFont;        //Item������

    int  m_nItemsNum;      //menuitem������

    typedef AIRVector<CMenuItem*> MenuItemArray;
    typedef MenuItemArray::iterator MenuItemArray_Iter;
    MenuItemArray  m_vtMenuItems;

    MENUITEM_INFO m_menuItemInfo;
    //CGUIListCtrl*  m_pMenuItem;
};
