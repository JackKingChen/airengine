// ***************************************************************
//  文件: GUISystem.h
//  日期: 2009-4-29
//  作者: liangairan
//  说明: GUI系统类，封装了大部分的UI操作和入口
// ***************************************************************
#pragma once

#include "AIRGUI.h"
#include "GUISchemeManager.h"
#include "MarkupSTL.h"
#include "CIME.h"
#include "FontManager.h"
#include "EventReader.h"
#include "IGUIFormater.h"
#include "GUIRenderer.h"
#include "GUIMouse.h"
#include "Singleton.h"

class CGUIDragImage;
class FileData;

class AIRGuiExport CGUISystem : public Singleton<CGUISystem>
{
    BE_SINGLETON(CGUISystem);
protected:    
    
    void UpdateWindowSize();
public:
    ~CGUISystem(void);

	CGUISchemeManager*  CreateScheme( const char* szName );

    bool LoadFromCFG(const char* szFilename);
    bool LoadFromXML(const char* szFilename);

    //bool LoadImageSet(const char* szFilename);

    //---------------------------------------------------------
    // 名称:  LoadSelf
    // 功能:  读入节点属性
    // 参数:  ThisElement - XML节点
    // 返回:  加载成功
    //---------------------------------------------------------
    bool LoadSelf( CMarkupSTL& xml );

    //---------------------------------------------------------
    // 名称:  LoadChild
    // 功能:  读入子节点
    // 参数:  ThisElement - XML节点
    // 返回:  加载成功
    //---------------------------------------------------------
    bool LoadChild( CMarkupSTL& xml );

    //---------------------------------------------------------
    // 名称:  Update
    // 功能:  更新GUI布局管理控制器
    // 参数:  无
    // 返回:  无
    //---------------------------------------------------------
    bool Update( void );

    void Initialize();

    void Clear();

    bool Render();

    void SetCurrentScheme(const AIRString& strSchemeName);

    void SetCurrentScheme(CGUISchemeManager* pScheme);

    CGUISchemeManager*  GetCurrentScheme( void );

    //void                SetEditViewSize(int nWidth, int nHeight);

    //void SetEditViewHeight(int nHeight);
    //float               GetRenderZoomW();
    //float               GetRenderZoomH();

    //CImageSet* GetImageSet(const AIRString& strImageSetName);
    //CImageUnit* GetImageUnit(const AIRString& strImageSetName, const AIRString& strImageUnitName);

    CGUISchemeManager*  LoadSceme(const char* szFilename);

#if AIR_TARGET_PLATFORM == AIR_PLATFORM_WIN32
    CIME*               GetIME() { return &m_ime; }

	LRESULT             MsgProc( HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam );

	bool				ProcessIMEEvent(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

private:
	bool                IMEFollow(HWND hWnd);
#endif

public:

    ITexture*           LoadTexture(const char* szFilename);

    void                ClearTexture(ITexture* pTex);

    

    //IFontBase* CreateFont(const FONTTYPE& ft, const char* szText);

    //处理输入消息，如果返回true，不再向下传递
    bool                ProcessInputEvent(const LPINPUTEVENT pEvent);

    CGUIElement*        GetFocusElement() { return m_pFocusElement; }

    CGUIElement*        GetDragElement() { return m_pDragElement; }

    CGUIElement*        GetHoverElement() { return m_pHoverElement; }

    CGUIDragImage*      GetMouseDragImage() { return m_pMouseDragImage; }

    CGUIEditCtrl*       GetEditCtrl()
    {
        return  m_pEditCtrl;
    }

    void SetEditCtrl(CGUIEditCtrl* pCtrl);

    void SetFocusElement(CGUIElement* e);

    void SetDragElement(CGUIElement* e);

    void SetHoverElement(CGUIElement* e);

    void SetMouseDragImage(CGUIDragImage* e);

    //注册控件回调函数
    void SetCtrlLButtonDownCallBack(const char* szCtrlName, LButtonDownCallback pFun);

    void SetCtrlLButtonUpCallBack(const char* szCtrlName, LButtonUpCallBack pFun);

    void SetCtrlRButtonDownCallBack(const char* szCtrlName, RButtonDownCallBack pFun);

    void SetCtrlRButtonUpCallBack(const char* szCtrlName, RButtonUpCallBack pFun);

    void SetCtrlKeyPressCallBack(const char* szCtrlName, KeyPressCallBack pFun);

    void SetCtrlMouseMoveCallBack(const char* szCtrlName, MouseMoveCallBack pFun);

    void SetCtrlMouseLeaveCallBack(const char* szCtrlName, MouseLeaveCallBack pFun);

    void SetCtrlOnShowCallBack(const char* szCtrlName, OnShowCallBack pFun);

    CGUIElement* FindCtrl(const char* szCtrlName);

    //CGUIElement*        FindCtrl(const AIRString& strCtrlName);

    void GetLastCursorPos(POINT& pt);

    CGUISchemeManager* GetScheme(const AIRString& strName);

    float GetMaxDepth() const
    {
        return m_fMaxDepth;
    }

    float GetMinDepth() const
    {
        return m_fMinDepth;
    }

	bool SaveToXML( const char* szFile );

    //static void			KillSelf()
    //{
    //    SafeDelete(m_pInstance);
    //}

	// 取得某张图的部分像素
	// ps:如果操作成功,ppData申请的内存数据要用户自己释放
	bool GetImagePixelData( void** ppData, u32& size, int& nWidth, int& nHeight, u8& bytesPerPixel, const char* szImgSetName, const char* szImgUnitName );


	bool RemoveScheme( CGUISchemeManager* pScheme );


    //通知控件被删除
    void NotifyElementDelete(CGUIElement* pElement);

    void SetGUIFormater(IGUIFormater* pFormater);

    //获得格式化器
    IGUIFormater* GetGUIFormater()
    {
        return m_pGUIFormater;
    }

    GUIRenderer* GetGUIRenderer()
    {
        return &m_uiRenderer;
    }

    //把控件移到最顶显示
    //void MoveElementToTop(CGUIElement* pElement);
    CGUIMouse* GetGUIMouse()
    {
        return &m_guiMouse;
    }

    //生成GUI所有数据
    //void CacheGUI();

private:

    typedef std::map<AIRString, SCHEME_DATA>           GUISCHEMEDATA_MAP;
    typedef GUISCHEMEDATA_MAP::iterator             GUISCHEMEDATA_ITER;
    typedef std::map<AIRString, CGUISchemeManager*>    GUISCHEME_MAP;
    typedef GUISCHEME_MAP::iterator                 GUISCHEME_ITER;

    GUISCHEME_MAP       m_mapGUISchemeMgr;
    GUISCHEMEDATA_MAP   m_mapSchemeData;
    CGUISchemeManager*  m_pCurSchemeMgr;
    
    string              m_strGUISystem;
    //float               m_fWndZoomW;        // 当前分辨率下的窗口与标准分辨率下的窗口的宽度缩放值
    //float               m_fWndZoomH;        // 当前分辨率下的窗口与标准分辨率下的窗口的高度缩放值
    //float               m_fRenderZoomW;     // 当前分辨率下的客户区与标准分辨率下的客户区的宽度的缩放值
    //float               m_fRenderZoomH;     // 当前分辨率下的客户区与标准分辨率下的客户区的高度的缩放值
    //int                 m_nEditWndWidth;    // 标准模式下的窗口宽度
    //int                 m_nEditWndHeight;   // 标准模式下的窗口高度
    //int                 m_nEditViewWidth;   // 标准模式下的客户区宽度
    //int                 m_nEditViewHeight;  // 标准模式下的客户区高度

#if AIR_TARGET_PLATFORM == AIR_PLATFORM_WIN32
    CIME                m_ime;
	HWND                m_hWnd;
#endif

    //IFontBase*          m_pFontTest;
    bool                m_bMouseDown;
    int                 m_nMousePosX;
    int                 m_nMousePosY;
    POINT               m_ptCursorPos;
    CGUIElement*        m_pFocusElement;
    CGUIElement*        m_pDragElement;
    CGUIElement*        m_pHoverElement;
    CGUIDragImage*      m_pMouseDragImage;    //与鼠标绑定的
    CGUIEditCtrl*       m_pEditCtrl;          //专门是编辑的控件

    float               m_fMaxDepth;          //最大的深度
    float               m_fMinDepth;          //最小的深度

    int                 m_nRenderWinWidth;    //渲染窗口宽
    int                 m_nRenderWinHeight;   //渲染窗口高

    IGUIFormater*       m_pGUIFormater;       //格式化器
    GUIRenderer         m_uiRenderer;

    CGUIMouse           m_guiMouse;

    //FileData*  m_pSchemeResource;    //scheme文件
};

//#define GUISYSTEM CGUISystem::GetInstancePtr()  
#define GUISYSTEM CGUISystem::GetInstance()  
#define GUIRENDERER	(GUISYSTEM->GetGUIRenderer())