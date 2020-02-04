//#include "StdAfx.h"
#include "GUISystem.h"
#include "GUIBuilderMgr.h"
//#include "ImageSetManager.h"
#include "cime.h"
#include "Engine.h"
#include "FileSystem.h"
#include "GUIEditBox.h"
//#include "CfgReader.h"
#include <string>
//#include "InputSystem.h"
//#include "RenderSystem.h"
//#include "Camera.h"
//#include "AIRFontManager.h"
#include "GUIRenderer.h"
#include "ImageOperator.h"
#include "ImageSet.h"
#include "GUITabCtrl.h"
#include "GUIListItem.h"
#include "GUIDragImage.h"
#include "TextureMgr.h"
#include "AIRImage.h"

#if AIR_TARGET_PLATFORM == AIR_PLATFORM_WIN32
#ifndef WINVER				// 允许使用特定于 Windows XP 或更高版本的功能。
#define WINVER 0x0501		// 将此值更改为相应的值，以适用于 Windows 的其他版本。
#endif
#endif

//CGUISystem* CGUISystem::m_pInstance = NULL;

CGUISystem::CGUISystem() : m_pFocusElement(NULL), m_pDragElement(NULL), m_pHoverElement(NULL),
m_pMouseDragImage(NULL),
m_pEditCtrl(NULL),
m_fMaxDepth(1.0f),
m_fMinDepth(0.0f),
m_nRenderWinWidth(0),
m_nRenderWinHeight(0),
m_pGUIFormater(NULL)
//, m_pSchemeResource(NULL)
{

    m_pCurSchemeMgr = NULL;
    m_bMouseDown = false;

    //int * p = new int[10];
}

CGUISystem::~CGUISystem(void)
{
    Clear();
    //SafeDelete(m_pInstance);
}

void CGUISystem::Clear()
{
    if (m_pEditCtrl)
    {
        m_pEditCtrl->CleanUp();
        SafeDelete(m_pEditCtrl);
    }

    m_pCurSchemeMgr = NULL;
    CGUISchemeManager* pScheme = NULL;
    for (GUISCHEME_ITER it = m_mapGUISchemeMgr.begin(); it != m_mapGUISchemeMgr.end(); it++)
    {
        pScheme = it->second;
        pScheme->Clear();
        SafeDelete<CGUISchemeManager*>(pScheme);
    }
    
    m_mapGUISchemeMgr.clear();
    CGUIBuilderMgr::GetInstance()->CleanUp();

    if (m_pGUIFormater)
    {
        SafeDelete(m_pGUIFormater);
    }
    //CGUIBuilderMgr::KillSelf();

    //m_fontMgr.Clear();
	m_uiRenderer.Clear();
    m_pCurSchemeMgr = NULL;
    m_pFocusElement = NULL;
    m_pEditCtrl = NULL;
    m_pHoverElement = NULL;
    m_pDragElement = NULL;
}

bool CGUISystem::LoadFromCFG(const char* szFilename)
{
    //ConfigReader reader;
    //reader.ReadFile(szFilename);
    
    return true;
}

void CGUISystem::NotifyElementDelete(CGUIElement* pElement)
{
	if (m_pEditCtrl)
	{
		if (m_pEditCtrl->GetAttachElement() == pElement)
		{
			m_pEditCtrl->AttachEditElement(NULL);
		}
	}
    if (pElement == m_pEditCtrl)
    {
        m_pEditCtrl = NULL;
    }
    if (pElement == m_pFocusElement)
    {
        m_pFocusElement = NULL;
    }
    if (pElement == m_pHoverElement)
    {
        m_pHoverElement = NULL;
    }
    if (pElement == m_pDragElement)
    {
        m_pDragElement = NULL;
    }
}

bool CGUISystem::LoadFromXML(const char* szFilename)
{
    CDataStream* pStream = FILESYSTEM->GetFileStream(szFilename);
    //AIRString strFilename = FILESYSTEM->GetFileAbsolutePath(szFilename);
    if (pStream == NULL)
    {
        return false;
    }
    AIRString strDoc = pStream->GetAsString();

    CMarkupSTL xml(strDoc.c_str());
    //if (!xml.Load(strFilename.c_str()))
    //{
    //    return false;
    //}

    //第一步，进入根结点
    if (!xml.FindElem())
    {
		SafeDelete(pStream);
        return false;
    }

    LoadSelf(xml);

    xml.IntoElem();

    if (!LoadChild(xml))
    {
        xml.OutOfElem();
		SafeDelete(pStream);
        return false;
    }

    xml.OutOfElem();

    SafeDelete(pStream);
    return true;
}

bool CGUISystem::LoadSelf( CMarkupSTL& xml )
{
    // 读入 CGUISchemeMgrCtrl 的名称
    //if( ThisElement->Attribute( "Name" ) )
        m_strGUISystem = xml.GetAttrib("Name");//ThisElement->Attribute( "Name" );

    int nEditViewWidth = 1024;
    int nEditViewHeight = 768;
    AIRString strAttributeValue = xml.GetAttrib("EditViewWidth").c_str();//ThisElement->Attribute("EditViewWidth");
    if (strAttributeValue != "")
    {
        nEditViewWidth = atoi(strAttributeValue.c_str());
    }

    strAttributeValue = xml.GetAttrib("EditViewHeight").c_str();
    if (strAttributeValue != "")
    {
        //m_nEditViewHeight = atoi(strAttributeValue.c_str());
    }

    //SetEditViewSize(nEditViewWidth, nEditViewHeight);
    return true;
}

bool CGUISystem::LoadChild( CMarkupSTL& xml )
{
    AIRString value;
    // 获得当前节点的第一个子节点
    //TiXmlElement* pChildElement = ThisElement->FirstChildElement();

    while( xml.FindElem("CGUISchemeManager") )
    {
        // 创建一个 CGUISchemeManager 对象
        SCHEME_DATA scheme_data;
        scheme_data.bLoad = false;
        scheme_data.strName = xml.GetAttrib("Name").c_str();

        xml.IntoElem();
        while (xml.FindElem("CGUIResource"))
        {
            AIRString strFilename = xml.GetAttrib("File").c_str();
            scheme_data.vtResourceFiles.push_back(strFilename);
            
        }
        xml.OutOfElem();

        m_mapSchemeData.insert(make_pair(scheme_data.strName, scheme_data));
  
    }
    //SetCurrentScheme("LoginMode");
    return true;
}

bool CGUISystem::Update()
{
    UpdateWindowSize();
    if( this->m_pCurSchemeMgr )
    {
        if (m_pCurSchemeMgr->IsReady())
        {
            this->m_pCurSchemeMgr->Update();

            m_guiMouse.Update();
        }
        else
        {
            if (m_pCurSchemeMgr->IsLoaded())
            {
                m_pCurSchemeMgr->Cache();
            }
        }
    }

    return true;
}

void CGUISystem::UpdateWindowSize()
{
    int nCurWidth = 0;
    int nCurHeight = 0;
    ENGINE_INST->GetBackBufferSize(nCurWidth, nCurHeight);
    
    if (nCurWidth != m_nRenderWinWidth || nCurHeight != m_nRenderWinHeight)
    {
        if (m_pCurSchemeMgr)
        {
            m_pCurSchemeMgr->CaculateEntityAlignPosition();
        }
        m_nRenderWinWidth = nCurWidth;
        m_nRenderWinHeight = nCurHeight;
    }
}

void CGUISystem::Initialize()
{
    //_FONTMANAGER->CreateDefaultFonts();
    //m_pHGE = pHGE;
#if AIR_TARGET_PLATFORM == AIR_PLATFORM_WIN32
    //初始化IME
    m_ime.Initialize();
#endif
    //测试用，看能否成功创建字体纹理
    //m_pFontTest = m_fontMgr.CreateDefaultFonts();
    
    CGUIBuilderMgr::GetInstance()->Register("CGUIWindow", new CGUIWindowBuilder);
    CGUIBuilderMgr::GetInstance()->Register("CGUIButton", new CGUIButtonBuilder);
    CGUIBuilderMgr::GetInstance()->Register("CGUIProgressBar", new CGUIProgressBarBuilder);
    CGUIBuilderMgr::GetInstance()->Register("CGUIStaticText", new CGUIStaticTextBuilder);
    CGUIBuilderMgr::GetInstance()->Register("CGUIEditBox", new CGUIEditBoxBuilder);
    CGUIBuilderMgr::GetInstance()->Register("CGUIAnimation", new CGUIAnimationBuilder);
    CGUIBuilderMgr::GetInstance()->Register("CGUICheckBox", new CGUICheckBoxBuilder);
    CGUIBuilderMgr::GetInstance()->Register("CGUISlider", new CGUISliderBuilder);
    CGUIBuilderMgr::GetInstance()->Register("CGUIScrollBar", new CGUIScrollBarBuilder);
    CGUIBuilderMgr::GetInstance()->Register("CGUIListCtrl", new CGUIListCtrlBuilder);
    CGUIBuilderMgr::GetInstance()->Register("CGUIMenu", new CGUIMenuBuilder);
    CGUIBuilderMgr::GetInstance()->Register("CGUITreeCtrl", new CGUITreeCtrlBuilder);
	CGUIBuilderMgr::GetInstance()->Register("CGUITabCtrl", new CGUITabCtrlBuilder);
	CGUIBuilderMgr::GetInstance()->Register("CGUITabButton", new CGUITabButtonBuilder);


    //CImageSetManager::GetInstancePtr()->LoadFromXML("./ConfigFile/TextureList.xml");
    //LoadImageSet("./Media/XML/TextureList.xml");

	GUIRENDERER->SetRenderer( ENGINE_INST->GetRenderer() );
	GUIRENDERER->InitGUIRenderer(20);
	ENGINE_INST->SetGUIRenderer( GUIRENDERER );
    
}

bool CGUISystem::Render()
{

    if (m_pCurSchemeMgr && m_pCurSchemeMgr->IsReady())
    {
        //OutputDebugString("Begin GUI rendering..\n");
        m_pCurSchemeMgr->Render();
        GUIRENDERER->RenderImmediately();
        if (m_pEditCtrl)
        {
            m_pEditCtrl->Render();
        }

        if (m_guiMouse.IsVisable())
        {
            m_guiMouse.Render();
        }
    }

    return true;
}

void CGUISystem::SetCurrentScheme(const AIRString& strSchemeName)
{
    GUISCHEME_ITER it = m_mapGUISchemeMgr.find(strSchemeName);
    if (it != m_mapGUISchemeMgr.end())
    {
        CGUISchemeManager* pScheme = it->second;
        SetCurrentScheme(pScheme);
        return;
    }

    GUISCHEMEDATA_ITER itData = m_mapSchemeData.find(strSchemeName);
    if (itData != m_mapSchemeData.end())
    {
        SCHEME_DATA data = itData->second;
        CGUISchemeManager* pScheme = new CGUISchemeManager();
        if (!pScheme->Load(data))
        {
            SafeDelete(pScheme);
        }
        else
        {
            m_mapGUISchemeMgr.insert(make_pair(data.strName, pScheme));
            SetCurrentScheme(pScheme);
            return;
        }
    }
    

    SetCurrentScheme(NULL);
}

void CGUISystem::SetCurrentScheme(CGUISchemeManager* pScheme)
{
    m_pCurSchemeMgr = pScheme;
    m_guiMouse.SetSchemeManager(pScheme);
}

CGUISchemeManager* CGUISystem::GetCurrentScheme( void )
{
    return m_pCurSchemeMgr;
}

ITexture* CGUISystem::LoadTexture(const char* szFilename)
{
    IRenderer* pRenderer = ENGINE_INST->GetRenderer();
    TextureParam param;
    param.nMipmaps = 1;
    ITexture* pTex = TEXTURE_MANAGER->CreateTextureFromFile(szFilename, param);
    if (pTex == NULL)
    {
        //hTex = m_pHGE->Texture_Load();
    }
    return pTex;
    //return m_pHGE->Texture_Load(szFilename);
}

void CGUISystem::ClearTexture(ITexture* pTex)
{
    //// for uieditor
    //if (m_pCurSchemeMgr)
    //{
    //    m_pCurSchemeMgr->NotifyTextureDestroy( pTex );
    //}

    TEXTURE_MANAGER->ClearTexture(pTex);
}

/*
CImageSet* CGUISystem::GetImageSet(const AIRString& strImageSetName)
{
    return m_imgSetMgr.GetImageSet(strImageSetName);
}

CImageUnit* CGUISystem::GetImageUnit(const AIRString& strImageSetName, const AIRString& strImageUnitName)
{
    return m_imgSetMgr.GetImageUnit(strImageSetName, strImageUnitName);
}
*/

#if AIR_TARGET_PLATFORM == AIR_PLATFORM_WIN32
LRESULT CGUISystem::MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
    int x, y;
    //INPUTSYSTEM->GetMousePosInWindow(x, y);
    switch( msg )
    {
        // 按下键盘按键
    case WM_KEYDOWN:     // 0x0100
        //m_ime.OnKeyDown( hwnd, msg, wParam, lParam );
        //OnKeyDown(     hwnd, msg, wParam, lParam );
        break;

        // 松开键盘按键
    case WM_KEYUP:       // 0x0101
        //m_ime.OnKeyUp( hwnd, msg, wParam, lParam );
        //OnKeyUp(       hwnd, msg, wParam, lParam );
        break;

        // 接受字符串消息
    case WM_CHAR:        // 0x0102
        m_ime.OnChar( hwnd, msg, wParam, lParam );
        //OnChar(        hwnd, msg, wParam, lParam );
        break;

        // 鼠标移动
    case WM_MOUSEMOVE:   // 0x0200

        if (m_bMouseDown)
        {
            GetCursorPos(&m_ptCursorPos);
            int dx = m_ptCursorPos.x - m_nMousePosX;
            int dy = m_ptCursorPos.y - m_nMousePosY;
            m_nMousePosX = m_ptCursorPos.x;
            m_nMousePosY = m_ptCursorPos.y;
        }
        m_pCurSchemeMgr->OnMouseMove(x, y);
		if(GUISYSTEM->GetDragElement())
			return TRUE;
        break;

        // 按下鼠标左键
    case WM_LBUTTONDOWN: // 0x0201
        // 将鼠标点中的GUI元素置空
        m_bMouseDown = true;
        GetCursorPos(&m_ptCursorPos);
        m_nMousePosX = m_ptCursorPos.x;
        m_nMousePosY = m_ptCursorPos.y;
        return m_pCurSchemeMgr->OnLButtonDown(x, y);
        break;

        // 松开鼠标左键
    case WM_LBUTTONUP:   // 0x0202
        // 将鼠标点中的GUI元素置空
        m_bMouseDown = false;
        return m_pCurSchemeMgr->OnLButtonUp(x, y);
        break;

        // 按下鼠标右键
    case WM_RBUTTONDOWN: // 0x0204
        return m_pCurSchemeMgr->OnRButtonDown(x, y);
        break;

        // 松开鼠标右键
    case WM_RBUTTONUP:   // 0x0205
        return m_pCurSchemeMgr->OnRButtonUp(x, y);
        break;

        // 按下鼠标中键
    case WM_MBUTTONDOWN: // 0x0207
        
        break;

        // 松开鼠标中键
    case WM_MBUTTONUP:   // 0x0208
        
        break;

        // 鼠标滚轮滚动
    case WM_MOUSEWHEEL:  // 0x020a
        // 将鼠标点中的GUI元素置空
        //if( GUI_SCHEME_CTRL_MGR->GetCurrentScheme() )
        //    GUI_SCHEME_CTRL_MGR->GetCurrentScheme()->SetHittedElement( NULL );
        //OnMouseWheel(  hwnd, msg, wParam, lParam );
        break;

        // 鼠标左键拖动
    //case WM_LBUTTONDRAG: // 0x0400 + 3
        //// 将鼠标点中的GUI元素置空
        //if( GUI_SCHEME_CTRL_MGR->GetCurrentScheme() )
        //    GUI_SCHEME_CTRL_MGR->GetCurrentScheme()->SetHittedElement( NULL );
        //OnLButtonDrag( hwnd, msg, wParam, lParam );
        break;

        // 鼠标左键拖动
    //case WM_MBUTTONDRAG: // 0x0400 + 4
        // 将鼠标点中的GUI元素置空
        //if( GUI_SCHEME_CTRL_MGR->GetCurrentScheme() )
        //    GUI_SCHEME_CTRL_MGR->GetCurrentScheme()->SetHittedElement( NULL );
        //OnMButtonDrag( hwnd, msg, wParam, lParam );
        break;

        // 鼠标右键拖动
    //case WM_RBUTTONDRAG: // 0x0400 + 5
        // 将鼠标点中的GUI元素置空
        //if( GUI_SCHEME_CTRL_MGR->GetCurrentScheme() )
        //    GUI_SCHEME_CTRL_MGR->GetCurrentScheme()->SetHittedElement( NULL );
        //OnRButtonDrag( hwnd, msg, wParam, lParam );
        break;

    case WM_IME_SETCONTEXT:         // 0x0281
        return m_ime.OnWM_IME_SETCONTEXT() ;
        break;

    case WM_INPUTLANGCHANGEREQUEST: // 0x0050
        return m_ime.OnWM_INPUTLANGCHANGEREQUEST();
        break;

    case WM_INPUTLANGCHANGE:        // 0x0051
        return m_ime.OnWM_INPUTLANGCHANGE( hwnd );
        break;

    case WM_IME_STARTCOMPOSITION:   // 0x010d
        return m_ime.OnWM_IME_STARTCOMPOSITION();
        break;

    case WM_IME_ENDCOMPOSITION:     // 0x010e
        return m_ime.OnWM_IME_ENDCOMPOSITION() ;
        break;

    case WM_IME_NOTIFY:             // 0x0282
        return m_ime.OnWM_IME_NOTIFY( hwnd, wParam )  ;
        break;

    case WM_IME_COMPOSITION:        // 0x010f
        return m_ime.OnWM_IME_COMPOSITION( hwnd, lParam ) ;
        break;

    default:
        return FALSE;
    }

    return FALSE;
}
#endif

CGUISchemeManager* CGUISystem::LoadSceme(const char* szFilename)
{
    CGUISchemeManager* pScheme = GetScheme(szFilename);

    if (pScheme == NULL)
    {
        pScheme = new CGUISchemeManager;
        if (!pScheme->LoadResource(szFilename))
        {
            SafeDelete(pScheme);
            return NULL;
        }

        m_mapGUISchemeMgr.insert(make_pair(szFilename, pScheme));
        SetCurrentScheme(pScheme);
    }
    return pScheme;
}

CGUISchemeManager*  CGUISystem::CreateScheme( const char* szName )
{
	CGUISchemeManager* pScheme = GetScheme(szName);
	if (pScheme == NULL)
	{
		pScheme = new CGUISchemeManager;
		m_mapGUISchemeMgr.insert(make_pair(szName, pScheme));
	}
	return pScheme;
}

//IFontBase* CGUISystem::CreateFont(const FONTTYPE& ft, const char* szText)
//{
//    return m_fontMgr.CreateFont(ft, szText);
//}

bool CGUISystem::ProcessInputEvent(const LPINPUTEVENT pEvent)
{
    
    if (m_pCurSchemeMgr)
    {
        return m_pCurSchemeMgr->ProcessInput(pEvent);
    }
    
    return false;
}

void                CGUISystem::SetHoverElement(CGUIElement* e) 
{ 
	if ( m_pHoverElement != e )
	{
		if (m_pHoverElement && m_pHoverElement->IsEnable() )
		{
			// 设置原来的hover elem为一般状态
			m_pHoverElement->SetCurrentUIState(UI_NORMAL);
		}
		m_pHoverElement = e; 
		if (m_pHoverElement && m_pHoverElement->IsEnable() )
		{
			// 设置新的hover elem为hover状态
			m_pHoverElement->SetCurrentUIState(UI_HOVER);
		}
	}
	else if( m_pHoverElement && m_pHoverElement->IsEnable() )
	{
		// 设置hover elem为hover状态
		m_pHoverElement->SetCurrentUIState(UI_HOVER);
	}
}

void                CGUISystem::SetFocusElement(CGUIElement* e) 
{ 
    if (m_pFocusElement && m_pFocusElement != e)
    {
        m_pFocusElement->SetFocus(false);
    }
    m_pFocusElement = e; 
    if (m_pFocusElement)
    {
        m_pFocusElement->SetFocus(true);
        m_pFocusElement->MoveToTop();
    }
}

void                CGUISystem::SetDragElement(CGUIElement* e) 
{ 
    m_pDragElement = e; 
}

void  CGUISystem::SetMouseDragImage(CGUIDragImage* e)
{
    if (m_pMouseDragImage && m_pMouseDragImage != e)
    {
        CGUIListItem* pItem = m_pMouseDragImage->GetListItem();
        if (pItem)
        {
            pItem->UnLock();
        }
    }
    m_pMouseDragImage = e;
}

#if AIR_TARGET_PLATFORM == AIR_PLATFORM_WIN32
bool CGUISystem::ProcessIMEEvent(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    bool bRes = false;
    switch( msg )
    {
    case WM_CHAR:
        if (m_pFocusElement && m_pFocusElement->GetType() == "CGUIEditBox")
        {
            //if (((char)wParam >= 48 && (char)wParam <= 57) || ((char)wParam >= 65 && (char)wParam <= 90) ||
            //    (char)wParam >= 97 /*&& (char)wParam <= 122)*/ || (char)wParam == ' ')
            if ((char)wParam >= 32)
            {
                CGUIEditBox* pCtrl = (CGUIEditBox*)m_pFocusElement;
                return pCtrl->OnChar(wParam);
            }
            
        }
        break;
    case WM_IME_SETCONTEXT:         // 0x0281

        return m_ime.OnWM_IME_SETCONTEXT();
        break;

    case WM_INPUTLANGCHANGEREQUEST: // 0x0050
        return m_ime.OnWM_INPUTLANGCHANGEREQUEST();
        break;

    case WM_INPUTLANGCHANGE:        // 0x0051
        return m_ime.OnWM_INPUTLANGCHANGE( hwnd );
        break;

    case WM_IME_STARTCOMPOSITION:   // 0x010d
        return m_ime.OnWM_IME_STARTCOMPOSITION();
        break;

    case WM_IME_ENDCOMPOSITION:     // 0x010e
        return m_ime.OnWM_IME_ENDCOMPOSITION() ;
        break;

    case WM_IME_NOTIFY:             // 0x0282
        return m_ime.OnWM_IME_NOTIFY( hwnd, wParam )  ;
        break;

    case WM_IME_COMPOSITION:        // 0x010f
        bRes = m_ime.OnWM_IME_COMPOSITION( hwnd, lParam ) ;
        if (bRes && m_pFocusElement && m_pFocusElement->GetType() == "CGUIEditBox")
        {
            CGUIEditBox* pCtrl = (CGUIEditBox*)m_pFocusElement;
            if (!pCtrl->IsPasswordMode())   //非密码模型才添加中文
            {
                pCtrl->AddText(m_ime.GetTextMessageW());
            }
            m_ime.ClearBuffer();
        }
        break;
    }

    return true;
}

bool CGUISystem::IMEFollow(HWND hWnd)
{
    //判断输入法是否打开
    if (!ImmIsIME(GetKeyboardLayout(0)))
        return false;

    //获得输入框左上坐标
    bool result;
    POINT point;
    float height = 0.0f;
    result = true;//getFocusedInputBoxCoord(point, height);
    if(!result)
        return false;

    //获得客户区的坐标
    RECT rect;
    GetClientRect(hWnd, &rect);
    point.x+=rect.left;
    point.y+=rect.top;

    //设置输入法位置
    HIMC hImc = ImmGetContext(hWnd);
    if(hImc==NULL) return false;
    COMPOSITIONFORM form;
    ImmGetCompositionWindow(hImc, &form);
    form.ptCurrentPos.x = point.x;
    form.ptCurrentPos.y = point.y + height;
    ImmSetCompositionWindow(hImc, &form);

    return true;	
}
#endif

CGUIElement* CGUISystem::FindCtrl(const char* szCtrlName)
{
	CGUIElement* pElement = NULL;
	// 先从当前的scheme查找
	if ( m_pCurSchemeMgr )
	{
		// 如果找得到,就返回该控件对象
		pElement = m_pCurSchemeMgr->FindCtrl( szCtrlName );
		if ( pElement )
		{
			return pElement;
		}
	}
	// 否则,遍历所有的scheme寻找
	else
	{
		for (GUISCHEME_ITER it = m_mapGUISchemeMgr.begin(); it != m_mapGUISchemeMgr.end(); it++)
		{
			CGUISchemeManager* pScheme = it->second;
			pElement = pScheme->FindCtrl(szCtrlName);
			if (pElement != NULL)
			{
				return pElement;
			}
		}
	}
    return NULL;
}

//CGUIElement* CGUISystem::FindCtrl(const AIRString& strCtrlName)
//{
//    return FindCtrl(strCtrlName.c_str());
//}

void CGUISystem::SetCtrlLButtonDownCallBack(const char* szCtrlName, LButtonDownCallback pFun)
{
    CGUIElement* pCtrl = FindCtrl(szCtrlName);
    if (pCtrl)
    {
        pCtrl->SetLButtonDownFun(pFun);
    }
}

void CGUISystem::SetCtrlLButtonUpCallBack(const char* szCtrlName, LButtonUpCallBack pFun)
{
    CGUIElement* pCtrl = FindCtrl(szCtrlName);
    if (pCtrl)
    {
        pCtrl->SetLButtonUpFun(pFun);
    }
}

void CGUISystem::SetCtrlRButtonDownCallBack(const char* szCtrlName, RButtonDownCallBack pFun)
{
    CGUIElement* pCtrl = FindCtrl(szCtrlName);
    if (pCtrl)
    {
        pCtrl->SetRButtonDownFun(pFun);
    }
}

void CGUISystem::SetCtrlRButtonUpCallBack(const char* szCtrlName, RButtonUpCallBack pFun)
{
    CGUIElement* pCtrl = FindCtrl(szCtrlName);
    if (pCtrl)
    {
        pCtrl->SetRButtonUpFun(pFun);
    }
}

void CGUISystem::SetCtrlKeyPressCallBack(const char* szCtrlName, KeyPressCallBack pFun)
{
    CGUIElement* pCtrl = FindCtrl(szCtrlName);
    if (pCtrl)
    {
        pCtrl->SetKeyPressFun(pFun);
    }
}

void CGUISystem::SetCtrlMouseMoveCallBack(const char* szCtrlName, MouseMoveCallBack pFun)
{
    CGUIElement* pCtrl = FindCtrl(szCtrlName);
    if (pCtrl)
    {
        pCtrl->SetMouseMoveFun(pFun);
    }
}

void CGUISystem::SetCtrlMouseLeaveCallBack(const char* szCtrlName, MouseLeaveCallBack pFun)
{
    CGUIElement* pCtrl = FindCtrl(szCtrlName);
    if (pCtrl)
    {
        pCtrl->SetMouseLeaveFun(pFun);
    }
}

void CGUISystem::SetCtrlOnShowCallBack(const char* szCtrlName, OnShowCallBack pFun)
{
    CGUIElement* pCtrl = FindCtrl(szCtrlName);
    if (pCtrl)
    {
        pCtrl->SetOnShowFun(pFun);
    }
}

void CGUISystem::GetLastCursorPos(POINT& pt)
{
    m_ptCursorPos = pt;
}

CGUISchemeManager* CGUISystem::GetScheme(const AIRString& strName)
{
    GUISCHEME_ITER it = m_mapGUISchemeMgr.find(strName);

    if (it != m_mapGUISchemeMgr.end())
    {
        return it->second;
    }

    return NULL;
}

void CGUISystem::SetEditCtrl(CGUIEditCtrl* pCtrl)
{
    m_pEditCtrl = pCtrl;
}



bool CGUISystem::SaveToXML( const char* szFile )
{
	// save .schemectrl file
	CMarkupSTL xml;
	xml.SetDoc("<?xml version=\"1.0\" encoding=\"gb2312\"?>\r\n");
	xml.AddElem("GUISystem");
	xml.AddAttrib( "Name", m_strGUISystem.c_str() );
	//xml.AddAttrib( "EditWndWidth", GetString("%d",m_nEditWndWidth) );
	//xml.AddAttrib( "EditWndHeight", GetString("%d",m_nEditWndHeight) );
	//xml.AddAttrib( "EditViewWidth", GetString("%d",m_nEditViewWidth) );
	//xml.AddAttrib( "EditViewHeight", GetString("%d",m_nEditViewHeight) );
	xml.IntoElem();
	//xml.AddNode(CMarkupSTL::MNT_TEXT, "\r\n    ");
	GUISCHEMEDATA_ITER dataItor = m_mapSchemeData.begin();
	for( ; dataItor != m_mapSchemeData.end(); ++dataItor )
	{
		xml.AddElem( "CGUISchemeManager" );
		xml.AddAttrib( "Name", dataItor->second.strName.c_str() );
		xml.IntoElem();
		//xml.AddNode(CMarkupSTL::MNT_TEXT, "\r\n    ");
		for( size_t i = 0; i < dataItor->second.vtResourceFiles.size(); ++i )
		{
			xml.AddElem( "CGUIResource" );
			xml.AddAttrib( "File", dataItor->second.vtResourceFiles[i].c_str() );
		}
		xml.OutOfElem();
	}
	xml.OutOfElem();
	xml.Save(szFile);

	// save .scheme file
	GUISCHEME_ITER schemeItor = m_mapGUISchemeMgr.begin();
	for( ; schemeItor != m_mapGUISchemeMgr.end(); ++schemeItor )
	{
		schemeItor->second->SaveToXML();
	}

	return true;
}

bool CGUISystem::GetImagePixelData(void** ppData, u32& size, int& nWidth, int& nHeight, u8& bytesPerPixel, const char* szImgSetName, const char* szImgUnitName )
{
	if ( !szImgSetName || !szImgUnitName || !m_pCurSchemeMgr )
	{
		*ppData = NULL;
		return false;
	}
	CImageUnit* pImgUnit = NULL;
	CImageSet* pImgSet = m_pCurSchemeMgr->GetImageSet(szImgSetName);
	if( !pImgSet )
	{
		*ppData = NULL;
		return false;
	}
	if( strlen( szImgUnitName ) > 0 )
	{
		pImgUnit = pImgSet->GetImgUnit(szImgUnitName);
		if( !pImgUnit )
		{
			*ppData = NULL;
			return false;
		}
	}

    /*
	ImageOperator* pIO = IMAGEOPERATORMGR->CreateImageOperatorFromFile(szImgSetName, pImgSet->GetFilename().c_str());
	if(!pIO)
	{
		*ppData = NULL;
		return false;
	}
    */
    AIRImage image;
    if (!image.LoadFromFile(pImgSet->GetFilename().c_str()))
    {
        return false;
    }

	int nOffsetX = 0, nOffsetY = 0;
	if( pImgUnit )
	{
		nOffsetX = pImgUnit->m_nOffsetX;
		nOffsetY = pImgUnit->m_nOffsetY;
		nWidth = pImgUnit->m_nWidth;
		nHeight = pImgUnit->m_nHeight;
	}
	else
	{
		nWidth = pImgSet->GetWidth();
		nHeight = pImgSet->GetHeight();
	}

	/*if( !pIO->CopyPixelDataFromImage( nOffsetX, nOffsetY, nWidth, nHeight, ppData ) )
	{
		SafeDeleteArray( *ppData );
		return false;
	}*/
    image.CopyPixelFromImage(nOffsetX, nOffsetY, nWidth, nHeight, ppData);

	bytesPerPixel = image.GetBytePerPixel(); //pIO->GetBytePerPixel();
	size = nWidth * nHeight * bytesPerPixel;

	return true;
}


bool	CGUISystem::RemoveScheme( CGUISchemeManager* pScheme )
{
	for (GUISCHEME_ITER it = m_mapGUISchemeMgr.begin(); it != m_mapGUISchemeMgr.end(); it++)
	{
		if( pScheme == it->second )
		{
			pScheme->CleanUp();
			SafeDelete<CGUISchemeManager*>(pScheme);
			m_mapGUISchemeMgr.erase(it);
			return true;
		}
		
	}
    return false;
}

void CGUISystem::SetGUIFormater(IGUIFormater* pFormater)
{
    m_pGUIFormater = pFormater;
}

