/********************************************************************
	created:	2009/11/17
	created:	17:11:2009   14:15
	filename: 	d:\My Codes\Engine\AIRGUI\GUIListItem.h
	file path:	d:\My Codes\Engine\AIRGUI
	file base:	GUIListItem
	file ext:	h
	author:		liangairan
	
	purpose:	����ListCtrl�е�Item�����԰�Icon�ŵ�Item��ȥ��
                ����Ϸ��������Ʒ������
*********************************************************************/
#pragma once
#include "GUIStaticText.h"
//#include "IFontBase.h"

class CGUIDragImage;

enum eGUIItem_State
{
    eItem_Normal,   //����״̬
    eItem_Hover,    //�������״̬
    eItem_Locked,   //����״̬
    eItem_UnUsed,
    //eItem_Selected, //ѡ��״̬
    eItem_Cool,     //��ȴ״̬
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
    
	AIRString strImgSrc;           //����ͼ
    AIRString strHoverImgSrc;      //hover����
    AIRString strUnuseImgSrc;      //������ʱ������
    AIRString strSelectedImgSrc;   //ѡ��ʱͼԪ
    AIRString strLockImgSrc;       //����ʱͼԪ
    SColor      sSelectedColor;      //ѡ�е���ɫ
    int         nSelectedFrameWidth; //ѡ��ʱ�ľ��α߿�
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

    //����Item
    virtual bool Create(CGUIListCtrl* pParent, const GUILIST_ITEM& itemData, int nParentOffsetX, int nParentOffsetY);

    //virtual bool Create(CGUIElement* pParent, CMarkupSTL& xml);

	virtual bool SaveSelfToXML(CMarkupSTL& xml );

    virtual void SetCurrentUIState(GUIELEMENT_STATE state);

    //������ȴʱ�䣬��λ��
    virtual void SetCoolTime(u32 dwTime);

    virtual void SetSelected(bool bSelected);

    //��ȾItem�е�����
    virtual void RenderText();

    int GetIndex() const
    {
        return m_nIndex;
    }

    virtual void SetIndex(int nIndex);

    //��ͼƬװ��item��
    void OnDrop(CGUIDragImage* pImage);

    //��ʼ�϶�
    void OnDrag();

    virtual void Move(int dx, int dy);

    //����
    void Lock();

    //����
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
    u32        m_dwCoolEndTime;    //��ȴʱ��
    WAIRString m_wcText;   //item�����壬һ��������
    bool         m_bSelected;
    int          m_nIndex;    //������

    bool         m_bAutoDeleteElement;

    CGUIListCtrl*  m_pListCtrl;

    CGUIStaticText*  m_pDetailText;   //��ϸ��Ϣ

    //Rectf        m_uvNormal;
    //Rectf        m_uvHover;
    //Rectf        m_uvLock;            //����״̬
    //Rectf        m_uvUnUsed;          //������״̬
    //Rectf        m_uvSelected;
    //Rectf        m_uvUnused;
    
    const GUILIST_ITEM*     m_pItemData;
};
