/********************************************************************
	created:	2009/11/17
	created:	17:11:2009   12:17
	filename: 	d:\My Codes\Engine\AIRGUI\GUIElement.h
	file path:	d:\My Codes\Engine\AIRGUI
	file base:	GUIElement
	file ext:	h
	author:		liangairan
	
	purpose:	GUI元素基类
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

//GUI渲染对象
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
    SColor m_clrDiffuse;  //颜色
    float m_fDepth;   //深度
};

struct FrameLayer 
{
    u32 m_nIndex;
    f32 m_fZ;   //深度
};

enum GUIELEMENT_STATE
{
    UI_NORMAL,          //正常状态
    UI_HOVER,           //鼠标在上面的状态
    UI_DRAGING,         //拖动状态
    UI_LBUTTONDOWN,     //鼠标左键按下状态
    UI_GRAY,            //不可用状态
};

//enum UILayerDefine
//{
//    UI_VIEW_NORMAL,     //正常
//    UI_ALWAYS_TOP,      //在最前端显示
//    UI_ALWAYS_BOTTOM,   //最底端显示
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
    eORIENT_HORZ            = 0x0000,    // 水平方向放置
    eORIENT_VERT            = 0x0001,    // 垂直方向放置
    eORIENT_MAX             = 0xffff     
};

enum GUIELEMENT_KEYBOARD_STATE
{
    UI_KEY_UP,              //按键UP
    UI_KEY_DOWN,            //按键DOWN
};


//UI对齐方式
enum GUIALIGN_STATE
{
    eGUIAlign_None,         //没有对齐
    eGUIAlign_LeftTop,      //左上对齐
    eGUIAlign_RightTop,     //右上对齐
    eGUIAlign_RightBottom,  //右下对齐
    eGUIAlign_LeftBottom,   //左下对齐
    eGUIAlign_Middle,       //居中显示
    eGUIAlign_TopMiddle,    //上居中
    eGUIAlign_RightMiddle,  //右居中
    eGUIAlign_BottomMiddle, //下居中
    eGUIAlign_LeftMiddle,   //左居中
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

    //创建UI元素
    virtual bool            Create(CGUIElement* pParent, CMarkupSTL& xml);

    virtual void            AddChild(CGUIElement* pChild);

    virtual void            SetRect(int x, int y, int nWidth, int nHeight);

    //设置渲染矩形
    virtual void            SetRect(const Rectf& rect);

    //设置实际
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

    //鼠标左键双击
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

    //消息处理，返回true不再下发
    virtual bool            ProcessInput(const LPINPUTEVENT pEvent);

    virtual CGUIElement*    FindChildElement(const AIRString& strName);

    virtual CGUIElement*    FindChildElement(CGUIElement* pChild);

    //落得显示方式
    UILayerDefine   GetLayerDef()
    {
        return m_uiLayerDef;
    }

    //设置显示方式
    virtual void            SetLayerDef(UILayerDefine viewState);

    ////设置UI当前状态
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

    //设置脚本
    void    SetLButtonUpScript(const AIRString& strScript);

    void    SetLButtonDownScript(const AIRString& strScript);

    void    SetRButtonUpScript(const AIRString& strScript);

    void    SetRButtonDownScript(const AIRString& strScript);

    //获得内容的高度
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

    //只移动实际区域，其他不移动
    virtual void            MoveRealRect(int dx, int dy);

    //只移动实际区域到
    virtual void            MoveRealRectTo(int nPosX, int nPosY);

    virtual bool            OnDrag(int x, int y);

    virtual void            SetSchemeManager(CGUISchemeManager* pScheme);

    inline CGUISchemeManager*      GetScheme() const;

    virtual void            SetScreenPos(int x, int y);

    int                     GetTop() const { return m_renderQuad.m_rectScreen.top; }

    int                     GetBottom() const { return m_renderQuad.m_rectScreen.bottom; }

    int                     GetLeft() const { return m_renderQuad.m_rectScreen.left; }

    int                     GetRight() const { return m_renderQuad.m_rectScreen.right; }

    //设置UI的中心坐标
    virtual void            SetCenterPos(int x, int y);

    //设置UI的对齐方式
    virtual void            SetGUIAlignState(GUIALIGN_STATE alignState);

    //加载完成后的操作
    virtual void            SetAfterLoad();

    //设置scissor
    virtual void            SetScissorRect(const RECT& rect) {}

    //获得ScissorRect
    virtual void            GetScissorRect(RECT& rect) {}

    //获得深度
    virtual float           GetDepth() 
    {
        return m_renderQuad.m_fDepth;
    }

    //设置深度
    virtual void            SetDepth(float fDepth);

    CGUIElementIterator     GetChildrenIterator();

    //获得parent控件
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

    //获得对齐方式
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

    //设置边框颜色
    virtual void            SetFrameColor(const SColor color);

    //设置UI图元
    virtual void            SetImageSrc(const AIRString& strImageSrc);

    //设置是否能移动
    virtual void            SetMovable(bool bMovable);

    //移除一个child
    virtual CGUIElement*    RemoveChild(CGUIElement* pChild);

    virtual CGUIElement*    RemoveChild(const AIRString& strName);

    //重新计算顶点位置
    virtual void            CaculateAlignPosition();

    //设置element的width
    virtual void            SetWidth(int nWidth);

	virtual void			SetRealRectWidth( int nRealRectWidth )
	{
		m_realRect.right = m_realRect.left + nRealRectWidth;
	}

    //设置element的height
    virtual void            SetHeight(int nHeight);

	virtual void			SetRealRectHeight( int nRealRectHeight )
	{
		m_realRect.bottom = m_realRect.top + nRealRectHeight;
	}

    //自动调整大小
    virtual void            AutoSize();

    //设置HyperLink
    //virtual void            SetHyperLink(CGUIHyperLink* pLink);


    //************************************
    // Method:    NotifyTextureDestroy
    // Returns:   void
    // Qualifier: 删除整个控件(包括子控件)所用到的pTex纹理
    // Parameter: ITexture * pTex
    //************************************
    virtual void            NotifyTextureDestroy(ITexture* pTex);

    //设置纹理
    void   SetTexture(ITexture* pTexture);

    //设置纹理UV
    void   SetUVRect(const Rectf& rcUV);

    virtual CGUIElement*    GetChild(const char* szName);

    //设置UI状态
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

    //移到所在层的最顶端
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
    //更新自己
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

    //子控件加载完后的设置
    virtual void            SetAfterLoadChild();

    //加载自己
    virtual void            SetAfterLoadSelf();

    //设置自己的深度
    virtual void            SetDepthSelf(float fDepth);

    //设置子控件的深度
    virtual void            SetDepthChild(float fDepth);

    //清空自己
    virtual void            ClearSelf();

    //清空子控件
    virtual void            ClearChild();

    virtual bool			SaveTextBlockToXML(CMarkupSTL& xml, IFontBase* pFont);

    virtual bool            SaveFontToXML(CMarkupSTL& xml, IFontBase* pFont);

    //创建字体
    virtual IFontBase*      CreateGUIFont(CMarkupSTL& xml);

    //************************************
    // Method:    NotifyTextureDestroySelf
    // Returns:   void
    // Qualifier: 删除自身控件里面用到的纹理pTex
    // Parameter: ITexture * pTex
    //************************************
    virtual void            NotifyTextureDestroySelf(ITexture* pTex);

    //************************************
    // Method:    NotifyTextureDestroyChild
    // Returns:   void
    // Qualifier: 删除子控件里面用到的纹理pTex
    // Parameter: ITexture * pTex
    //************************************
    virtual void            NotifyTextureDestroyChild(ITexture* pTex);

    //渲染renderquad
    inline void DrawRenderQuad();
public:
	virtual bool			SaveToXML( CMarkupSTL& xml );
	virtual bool			SaveSelfToXML( CMarkupSTL& xml );
	virtual bool			SaveChildToXML( CMarkupSTL& xml );
	virtual bool			SaveScriptObject( CMarkupSTL& xml );


protected:
    typedef std::list<CGUIElement*>   GUIELEMENT_LIST;
    typedef GUIELEMENT_LIST::iterator  GUIELEMENT_ITER;

    CGUIElement*            m_pParent;          //控件对应的父控件
    GUIELEMENT_LIST         m_lstChildren;      //子控件列表

protected:
    AIRString                  m_strName;          //控件名
    AIRString             m_strType;          //控件类型
    //Rectf                   m_renderQuad.m_rectScreen;       //渲染到屏幕的坐标
    Rectf                   m_realRect;         //UI的实际区域
    //Rectf                   m_UV;               //GUI元素显示的背景纹理UV    
    //SColor                  m_renderQuad.m_clrDiffuse;           //渲染时所用到的color
    SColor                  m_colorFont;        //默认字体颜色
    //ITexture*               m_renderQuad.m_pTexture;         //背景图片的纹理
    bool                    m_bShow;            //是否要渲染

    bool                    m_bFocus;           //是否设置了焦点
    bool                    m_bMoveable;        //是否可移动
    bool                    m_bDrawFrame;       //是否要画矩形
    int                     m_nFrameWidth;      //画矩形的边宽
    SColor                  m_colorRectFrame;   //边框颜色
                                                //这里的状态是通用状态，但不同控件有不同的状态，到时有些控件不能用该状态
    u8                    m_nLastUIState;     //上一次状态，
    u8                    m_nCurUIState;      //当前状态
    CGUIScriptObject*       m_pScriptObject;    //控件对应的脚本处理器
    LButtonDownCallback     m_pLButtonDownFun;  //鼠标左键按下的回调函数
    LButtonUpCallBack       m_pLButtonUpFun;    //鼠标左键弹起的回调函数
    RButtonDownCallBack     m_pRButtonDownFun;  //鼠标右键按下的回调函数
    RButtonUpCallBack       m_pRButtonUpFun;    //鼠标右键弹起的回调函数
    KeyPressCallBack        m_pKeyPressFun;     //按键按下的回调函数
    MouseMoveCallBack       m_pMouseMoveFun;    //鼠标在控件上移动的回调函数
    MouseLeaveCallBack      m_pMouseLeaveFun;   //鼠标离开的回调函数
    LButtonDBClickCallBack  m_pLButtonDBClickFun;  //鼠标左键双击回调函数
    OnShowCallBack          m_pOnShowFun;          //显示时的回调函数
    OnHideCallBack          m_pOnHideFun;          //隐藏时的回调函数
    UILayerDefine           m_uiLayerDef;        //显示方式（最前，最底，正常）
    u8                    m_byKeyUp;          //激活脚本的某个键
    u8                    m_byKeyDown;        //激活脚本的某个按键
    POINT                   m_ptLastMouse;      //鼠标上上一次的位置
    GUIALIGN_STATE          m_alignState;       //对齐方式（左下，左下，右上，居中……）
    int                     m_nEdgeOffsetX;     //偏移X位置，和对齐方式有关
    int                     m_nEdgeOffsetY;     //偏移Y位置，和对方方式有关
    CGUISchemeManager*      m_pScheme;          //GUI元素所属的Scheme
    //float                   m_fDepth;           //深度
	AIRString				m_szImgSrc;         //background image

    GUIRenderQuad           m_renderQuad;       //渲染对象
    //CGUIHyperLink*          m_pHyperLink;       //连接器

    FrameLayer*  m_pFrameLayer;

    bool m_bParentLayer;    //相对于父亲的layer层，false即父亲的下一层，true代表与父亲同一层framelayer
};
