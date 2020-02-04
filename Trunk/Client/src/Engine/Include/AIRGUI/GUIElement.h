/********************************************************************
	created:	2009/11/17
	created:	17:11:2009   12:17
	filename: 	d:\My Codes\Engine\AIRGUI\GUIElement.h
	file path:	d:\My Codes\Engine\AIRGUI
	file base:	GUIElement
	file ext:	h
	author:		liangairan
	
	purpose:	GUIԪ�ػ���
*********************************************************************/
#pragma once


#include "Rect.h"
#include "GUIScriptObject.h"
#include "Util.h"
#include "MarkupSTL.h"
#include "EventReader.h"
#include "ITexture.h"
#include "Color.h"
#include "AIRGUI.h"
//#include "GUIRenderer.h"
#include "GUIElementIterator.h"
#include "IFontBase.h"

class CGUISchemeManager;

class CGUIHyperLink;

//GUI��Ⱦ����
class GUIRenderQuad
{
public:
    GUIRenderQuad() : m_pTexture(NULL), m_fDepth(0.0f)
    {

    }

    virtual ~GUIRenderQuad() {}

public:
    Rectf m_rectScreen;
    Rectf m_uv;
    ITexture* m_pTexture;
    SColor m_clrDiffuse;  //��ɫ
    float m_fDepth;   //���
};

struct FrameLayer 
{
    u32 m_nIndex;
    f32 m_fZ;   //���
};

enum GUIELEMENT_STATE
{
    UI_NORMAL,          //����״̬
    UI_HOVER,           //����������״̬
    UI_DRAGING,         //�϶�״̬
    UI_LBUTTONDOWN,     //����������״̬
    UI_GRAY,            //������״̬
};

//enum UILayerDefine
//{
//    UI_VIEW_NORMAL,     //����
//    UI_ALWAYS_TOP,      //����ǰ����ʾ
//    UI_ALWAYS_BOTTOM,   //��׶���ʾ
//};

enum UILayerDefine
{
    UI_Layer_Mouse = 0,
    UI_Layer_Model,
    UI_Layer_Top,
    UI_Layer_Middle,
    UI_Layer_Bottom,
    UI_Layer_Max,
};

enum eGUIOrientation                 
{
    eORIENT_HORZ            = 0x0000,    // ˮƽ�������
    eORIENT_VERT            = 0x0001,    // ��ֱ�������
    eORIENT_MAX             = 0xffff     
};

enum GUIELEMENT_KEYBOARD_STATE
{
    UI_KEY_UP,              //����UP
    UI_KEY_DOWN,            //����DOWN
};


//UI���뷽ʽ
enum GUIALIGN_STATE
{
    eGUIAlign_None,         //û�ж���
    eGUIAlign_LeftTop,      //���϶���
    eGUIAlign_RightTop,     //���϶���
    eGUIAlign_RightBottom,  //���¶���
    eGUIAlign_LeftBottom,   //���¶���
    eGUIAlign_Middle,       //������ʾ
    eGUIAlign_TopMiddle,    //�Ͼ���
    eGUIAlign_RightMiddle,  //�Ҿ���
    eGUIAlign_BottomMiddle, //�¾���
    eGUIAlign_LeftMiddle,   //�����
};

typedef bool (*LButtonDownCallback)(CGUIElement*);

typedef bool (*LButtonUpCallBack)(CGUIElement*);

typedef bool (*LButtonDBClickCallBack)(CGUIElement*);

typedef bool (*RButtonDownCallBack)(CGUIElement*);

typedef bool (*RButtonUpCallBack)(CGUIElement*);

typedef bool (*KeyPressCallBack)(CGUIElement*, int nKey);

typedef bool (*MouseMoveCallBack)(CGUIElement*);

typedef bool (*MouseLeaveCallBack)(CGUIElement*);

typedef bool (*OnShowCallBack)(CGUIElement*);

typedef bool (*OnHideCallBack)(CGUIElement*);

class CGUITabButton;

class  AIRGuiExport CGUIElement //: public EngineAllocator(CGUIElement)
{
	EngineAllocator(CGUIElement)
	friend class CGUITabButton;

public:
    CGUIElement(void);
    virtual ~CGUIElement(void);

    void SetName(const char* szName);
    void SetName(const AIRString& strName);
    void SetType(const AIRString& strType);
    const AIRString& GetName() const
    {
        return m_strName;
    }
    const AIRString& GetType() const
    {
        return m_strType;
    }

    virtual CGUIElement*    Clone();

    //����UIԪ��
    virtual bool            Create(CGUIElement* pParent, CMarkupSTL& xml);

    virtual void            AddChild(CGUIElement* pChild);

    virtual void            SetRect(int x, int y, int nWidth, int nHeight);

    //������Ⱦ����
    virtual void            SetRect(const Rectf& rect);

    //����ʵ��
    virtual void            SetRealRect(const Rectf& rect);

    virtual void            SetColor(u8 nR, u8 nG, u8 nB, u8 nA);
    virtual void            SetColor(const SColor& color);

    virtual void            Render();

    virtual void            CleanUp();

    virtual void            Update();

    virtual void            SetParent(CGUIElement* pParent);

    virtual float           GetWidth() const
    {
        return m_renderQuad.m_rectScreen.right - m_renderQuad.m_rectScreen.left; 
    }

    virtual float           GetHeight() const
    {
        return m_renderQuad.m_rectScreen.bottom - m_renderQuad.m_rectScreen.top; 
    }

    void                    Show();

    void                    Hide();

    void                    ChangeShow();

    virtual void            GetRenderRect(Rectf& rect) const
    {        
        rect = m_renderQuad.m_rectScreen;//m_renderQuad.m_rectScreen;
    }

	virtual const Rectf&	GetRenderRect( void ) const
    {
        return m_renderQuad.m_rectScreen;
    }

    virtual void            SetFocus(bool bFocus);

    virtual bool            OnLButtonDown(int x, int y);
    
    virtual bool            OnLButtonUp(int x, int y);

    virtual bool            OnRButtonDown(int x, int y);

    virtual bool            OnRButtonUp(int x, int y);

    virtual bool            OnMouseMove(int x, int y);

    virtual bool            OnMouseLeave(int x, int y);

    virtual bool            OnMouseWheel(int dz);

    virtual bool            OnKeyPress(int nKey);

    virtual bool            OnKeyUp(int nKey);

    //������˫��
    virtual bool            OnLButtonDBClick(int x, int y);

    virtual bool            IsInRect(int x, int y);

    virtual void            Move(int dx, int dy);

    virtual void            MoveTo(int nPosX, int nPosY);

    bool                    IsMoveable()
    {
        return m_bMoveable;
    }

    bool                    IsVisable()
    {
        return m_bShow;
    }

    virtual void            SetIsDrawFrame( bool b );

    //��Ϣ��������true�����·�
    virtual bool            ProcessInput(const LPINPUTEVENT pEvent);

    virtual CGUIElement*    FindChildElement(const AIRString& strName);

    virtual CGUIElement*    FindChildElement(CGUIElement* pChild);

    //�����ʾ��ʽ
    UILayerDefine   GetLayerDef()
    {
        return m_uiLayerDef;
    }

    //������ʾ��ʽ
    virtual void            SetLayerDef(UILayerDefine viewState);

    ////����UI��ǰ״̬
    //virtual void            SetUICurrentState(GUIELEMENT_STATE state);

    //virtual EGUI_Ctrl_Type  GetCtrlType() const;

    virtual void            SetLButtonDownFun(LButtonDownCallback pFun);

    virtual void            SetLButtonUpFun(LButtonUpCallBack pFun);

    virtual void            SetRButtonDownFun(RButtonDownCallBack pFun);

    virtual void            SetRButtonUpFun(RButtonUpCallBack pFun);

    virtual void            SetKeyPressFun(KeyPressCallBack pFun);

    virtual void            SetMouseMoveFun(MouseMoveCallBack pFun);

    virtual void            SetMouseLeaveFun(MouseLeaveCallBack pFun);

    virtual void            SetOnShowFun(OnShowCallBack pFun);

    virtual void            SetOnHideFun(OnHideCallBack pFun);

    //���ýű�
    void    SetLButtonUpScript(const AIRString& strScript);

    void    SetLButtonDownScript(const AIRString& strScript);

    void    SetRButtonUpScript(const AIRString& strScript);

    void    SetRButtonDownScript(const AIRString& strScript);

    //������ݵĸ߶�
    virtual int             GetRealHeight() const;

    virtual int             GetRealWidth() const;

    virtual void            GetRealRect(Rectf& rect) const
    {
        rect = m_realRect;
    }

    virtual Rectf&          GetRealRect( void )
    {
        return m_realRect;
    }

    //ֻ�ƶ�ʵ�������������ƶ�
    virtual void            MoveRealRect(int dx, int dy);

    //ֻ�ƶ�ʵ������
    virtual void            MoveRealRectTo(int nPosX, int nPosY);

    virtual bool            OnDrag(int x, int y);

    virtual void            SetSchemeManager(CGUISchemeManager* pScheme);

    inline CGUISchemeManager*      GetScheme() const;

    virtual void            SetScreenPos(int x, int y);

    int                     GetTop() const { return m_renderQuad.m_rectScreen.top; }

    int                     GetBottom() const { return m_renderQuad.m_rectScreen.bottom; }

    int                     GetLeft() const { return m_renderQuad.m_rectScreen.left; }

    int                     GetRight() const { return m_renderQuad.m_rectScreen.right; }

    //����UI����������
    virtual void            SetCenterPos(int x, int y);

    //����UI�Ķ��뷽ʽ
    virtual void            SetGUIAlignState(GUIALIGN_STATE alignState);

    //������ɺ�Ĳ���
    virtual void            SetAfterLoad();

    //����scissor
    virtual void            SetScissorRect(const RECT& rect) {}

    //���ScissorRect
    virtual void            GetScissorRect(RECT& rect) {}

    //������
    virtual float           GetDepth() 
    {
        return m_renderQuad.m_fDepth;
    }

    //�������
    virtual void            SetDepth(float fDepth);

    CGUIElementIterator     GetChildrenIterator();

    //���parent�ؼ�
    virtual CGUIElement*    GetParent()
    {
        return m_pParent;
    }

    virtual bool            IsDrawFrame() const
    {
        return m_bDrawFrame;
    }

    virtual SColor          GetRectFrameColor() const
    {
        return m_colorRectFrame;
    }

    virtual SColor          GetColor() const
    {
        return m_renderQuad.m_clrDiffuse;
    }

    //��ö��뷽ʽ
    virtual GUIALIGN_STATE  GetAlignState()
    {
        return m_alignState;
    }

    //Get the background image
    virtual AIRString&    GetImageSrc()
    {
        return m_szImgSrc;
    }

    //Get the edge offset of the element
    virtual int             GetEdgeOffsetX()
    {
        return m_nEdgeOffsetX;
    }

    //
    virtual int             GetEdgeOffsetY()
    {
        return m_nEdgeOffsetY;
    }

    virtual void            SetEdgeOffsetX(int nX);

    virtual void            SetEdgeOffsetY(int nY);

    //���ñ߿���ɫ
    virtual void            SetFrameColor(const SColor color);

    //����UIͼԪ
    virtual void            SetImageSrc(const AIRString& strImageSrc);

    //�����Ƿ����ƶ�
    virtual void            SetMovable(bool bMovable);

    //�Ƴ�һ��child
    virtual CGUIElement*    RemoveChild(CGUIElement* pChild);

    virtual CGUIElement*    RemoveChild(const AIRString& strName);

    //���¼��㶥��λ��
    virtual void            CaculateAlignPosition();

    //����element��width
    virtual void            SetWidth(int nWidth);

	virtual void			SetRealRectWidth( int nRealRectWidth )
	{
		m_realRect.right = m_realRect.left + nRealRectWidth;
	}

    //����element��height
    virtual void            SetHeight(int nHeight);

	virtual void			SetRealRectHeight( int nRealRectHeight )
	{
		m_realRect.bottom = m_realRect.top + nRealRectHeight;
	}

    //�Զ�������С
    virtual void            AutoSize();

    //����HyperLink
    //virtual void            SetHyperLink(CGUIHyperLink* pLink);


    //************************************
    // Method:    NotifyTextureDestroy
    // Returns:   void
    // Qualifier: ɾ�������ؼ�(�����ӿؼ�)���õ���pTex����
    // Parameter: ITexture * pTex
    //************************************
    virtual void            NotifyTextureDestroy(ITexture* pTex);

    //��������
    void   SetTexture(ITexture* pTexture);

    //��������UV
    void   SetUVRect(const Rectf& rcUV);

    virtual CGUIElement*    GetChild(const char* szName);

    //����UI״̬
    virtual void SetCurrentUIState(GUIELEMENT_STATE state);

    bool IsEnable() const
    {
        return m_nCurUIState != UI_GRAY;
    }

	CGUIScriptObject* GetScriptObject( void )
	{
		return m_pScriptObject;
	}

	void SetScriptObject( CGUIScriptObject* pScriptObject )
	{
		m_pScriptObject = pScriptObject;
	}

	GUIELEMENT_STATE	GetCurrentUIState( void )
	{
		return (GUIELEMENT_STATE)m_nCurUIState;
	}

    //�Ƶ����ڲ�����
    void MoveToTop();

    virtual void CopyFrom(const CGUIElement* pOther);

    bool IsParentLayer() const
    {
        return m_bParentLayer;
    }

    void SetParentLayer(bool bParentLayer)
    {
        m_bParentLayer = bParentLayer;
    }

protected:
    virtual void            RenderSelf();
    virtual void            RenderChild();
    virtual bool            CreateSelf(CGUIElement* pParent, CMarkupSTL& xml);
    virtual bool            CreateChild(CMarkupSTL& parent);
    virtual void            OnStatusChange();
    //�����Լ�
    virtual void            UpdateSelf();

    virtual void            RegisterToScript();

    virtual bool            OnLButtonDownSelf(int x, int y);

    virtual bool            OnLButtonUpSelf(int x, int y);

    virtual bool            OnRButtonDownSelf(int x, int y);

    virtual bool            OnRButtonUpSelf(int x, int y);

    virtual bool            OnMouseMoveSelf(int x, int y);

    virtual bool            OnMouseLeaveSelf(int x, int y);

    virtual bool            OnKeyPressSelf(int nKey);

    virtual bool            OnKeyUpSelf(int nKey);

    virtual bool            OnLButtonDBClickSelf(int x, int y);

    virtual bool            OnDragSelf(int x, int y);

    virtual bool            ProcessInputSelf(const LPINPUTEVENT pEvent);

    virtual void            MoveChild(int dx, int dy);

    //�ӿؼ�������������
    virtual void            SetAfterLoadChild();

    //�����Լ�
    virtual void            SetAfterLoadSelf();

    //�����Լ������
    virtual void            SetDepthSelf(float fDepth);

    //�����ӿؼ������
    virtual void            SetDepthChild(float fDepth);

    //����Լ�
    virtual void            ClearSelf();

    //����ӿؼ�
    virtual void            ClearChild();

    virtual bool			SaveTextBlockToXML(CMarkupSTL& xml, IFontBase* pFont);

    virtual bool            SaveFontToXML(CMarkupSTL& xml, IFontBase* pFont);

    //��������
    virtual IFontBase*      CreateGUIFont(CMarkupSTL& xml);

    //************************************
    // Method:    NotifyTextureDestroySelf
    // Returns:   void
    // Qualifier: ɾ������ؼ������õ�������pTex
    // Parameter: ITexture * pTex
    //************************************
    virtual void            NotifyTextureDestroySelf(ITexture* pTex);

    //************************************
    // Method:    NotifyTextureDestroyChild
    // Returns:   void
    // Qualifier: ɾ���ӿؼ������õ�������pTex
    // Parameter: ITexture * pTex
    //************************************
    virtual void            NotifyTextureDestroyChild(ITexture* pTex);

    //��Ⱦrenderquad
    inline void DrawRenderQuad();
public:
	virtual bool			SaveToXML( CMarkupSTL& xml );
	virtual bool			SaveSelfToXML( CMarkupSTL& xml );
	virtual bool			SaveChildToXML( CMarkupSTL& xml );
	virtual bool			SaveScriptObject( CMarkupSTL& xml );


protected:
    typedef std::list<CGUIElement*>   GUIELEMENT_LIST;
    typedef GUIELEMENT_LIST::iterator  GUIELEMENT_ITER;

    CGUIElement*            m_pParent;          //�ؼ���Ӧ�ĸ��ؼ�
    GUIELEMENT_LIST         m_lstChildren;      //�ӿؼ��б�

protected:
    AIRString                  m_strName;          //�ؼ���
    AIRString             m_strType;          //�ؼ�����
    //Rectf                   m_renderQuad.m_rectScreen;       //��Ⱦ����Ļ������
    Rectf                   m_realRect;         //UI��ʵ������
    //Rectf                   m_UV;               //GUIԪ����ʾ�ı�������UV    
    //SColor                  m_renderQuad.m_clrDiffuse;           //��Ⱦʱ���õ���color
    SColor                  m_colorFont;        //Ĭ��������ɫ
    //ITexture*               m_renderQuad.m_pTexture;         //����ͼƬ������
    bool                    m_bShow;            //�Ƿ�Ҫ��Ⱦ

    bool                    m_bFocus;           //�Ƿ������˽���
    bool                    m_bMoveable;        //�Ƿ���ƶ�
    bool                    m_bDrawFrame;       //�Ƿ�Ҫ������
    int                     m_nFrameWidth;      //�����εı߿�
    SColor                  m_colorRectFrame;   //�߿���ɫ
                                                //�����״̬��ͨ��״̬������ͬ�ؼ��в�ͬ��״̬����ʱ��Щ�ؼ������ø�״̬
    u8                    m_nLastUIState;     //��һ��״̬��
    u8                    m_nCurUIState;      //��ǰ״̬
    CGUIScriptObject*       m_pScriptObject;    //�ؼ���Ӧ�Ľű�������
    LButtonDownCallback     m_pLButtonDownFun;  //���������µĻص�����
    LButtonUpCallBack       m_pLButtonUpFun;    //����������Ļص�����
    RButtonDownCallBack     m_pRButtonDownFun;  //����Ҽ����µĻص�����
    RButtonUpCallBack       m_pRButtonUpFun;    //����Ҽ�����Ļص�����
    KeyPressCallBack        m_pKeyPressFun;     //�������µĻص�����
    MouseMoveCallBack       m_pMouseMoveFun;    //����ڿؼ����ƶ��Ļص�����
    MouseLeaveCallBack      m_pMouseLeaveFun;   //����뿪�Ļص�����
    LButtonDBClickCallBack  m_pLButtonDBClickFun;  //������˫���ص�����
    OnShowCallBack          m_pOnShowFun;          //��ʾʱ�Ļص�����
    OnHideCallBack          m_pOnHideFun;          //����ʱ�Ļص�����
    UILayerDefine           m_uiLayerDef;        //��ʾ��ʽ����ǰ����ף�������
    u8                    m_byKeyUp;          //����ű���ĳ����
    u8                    m_byKeyDown;        //����ű���ĳ������
    POINT                   m_ptLastMouse;      //�������һ�ε�λ��
    GUIALIGN_STATE          m_alignState;       //���뷽ʽ�����£����£����ϣ����С�����
    int                     m_nEdgeOffsetX;     //ƫ��Xλ�ã��Ͷ��뷽ʽ�й�
    int                     m_nEdgeOffsetY;     //ƫ��Yλ�ã��ͶԷ���ʽ�й�
    CGUISchemeManager*      m_pScheme;          //GUIԪ��������Scheme
    //float                   m_fDepth;           //���
	AIRString				m_szImgSrc;         //background image

    GUIRenderQuad           m_renderQuad;       //��Ⱦ����
    //CGUIHyperLink*          m_pHyperLink;       //������

    FrameLayer*  m_pFrameLayer;

    bool m_bParentLayer;    //����ڸ��׵�layer�㣬false�����׵���һ�㣬true�����븸��ͬһ��framelayer
};
