//#include "StdAfx.h"
#include "GUIElement.h"
//#include "RenderSystem.h"
//#include "ImageSetManager.h"
#include "GUIBuilderMgr.h"
#include "GUISystem.h"
#include "StringUtil.h"
#include "Engine.h"
#include <assert.h>
#include "GUISchemeManager.h"
#include "GUIRenderer.h"
#include "HyperLinkManager.h"
#include "ImageSet.h"
//#include "UILuaScriptMgr.h"
//#include "InputSystem.h"

//int g_uiElement = 0;

CGUIElement::CGUIElement(void) : m_pParent(NULL), m_bFocus(false), m_bMoveable(false), m_nCurUIState(UI_NORMAL), m_nLastUIState(UI_NORMAL),
m_pLButtonDownFun(NULL), m_pLButtonUpFun(NULL), m_pRButtonDownFun(NULL), m_pRButtonUpFun(NULL), 
m_pKeyPressFun(NULL),
m_pMouseMoveFun(NULL), 
m_pMouseLeaveFun(NULL), 
m_pLButtonDBClickFun(NULL),
m_pOnShowFun(NULL),
m_pOnHideFun(NULL),
m_uiLayerDef(UI_Layer_Middle)
, m_pScheme(NULL)
, m_pFrameLayer(0)
, m_bParentLayer(false)
//, m_pHyperLink(NULL)
{
    //m_pImage = NULL;
    m_byKeyUp = 0;
    m_byKeyDown = 0;
    //m_pVertexBuffer = NULL;
    //m_bIsInRange = false;
    m_renderQuad.m_pTexture = NULL;
    m_bShow = true;
    m_bFocus = false;
    m_bMoveable = false;
    m_pScriptObject = NULL;
    m_renderQuad.m_clrDiffuse = 0xffffffff;

    m_ptLastMouse.x = 0;
    m_ptLastMouse.y = 0;

    m_renderQuad.m_rectScreen.SetRect(0, 0, 60, 10);
    m_realRect = m_renderQuad.m_rectScreen;

    m_bDrawFrame = false;
    m_nFrameWidth = 1;
    m_colorRectFrame = SColor(255, 255, 0, 0);
    
    m_alignState = eGUIAlign_None;
    m_nEdgeOffsetX = 0;
    m_nEdgeOffsetY = 0;

	m_strType = "CGUIElement";

    //g_uiElement++;
}

CGUIElement::~CGUIElement(void)
{
    GUISYSTEM->NotifyElementDelete(this);
    for (GUIELEMENT_ITER it = m_lstChildren.begin(); it != m_lstChildren.end(); ++it)
    {
        SafeDelete(*it);
    }

    m_lstChildren.clear();

    //g_uiElement--;
    //OutputDebugString(GetString("Now the ui element count is %d\n", g_uiElement));
}

void CGUIElement::SetName(const char* szName)
{
    m_strName = szName;
    //strcpy(m_szName, szName);
}

void CGUIElement::SetName(const AIRString& strName)
{
    m_strName = strName;
}

void CGUIElement::SetType(const AIRString& strType)
{
    m_strType = strType;
}

CGUIElement* CGUIElement::Clone() 
{ 
    return NULL; 
};

void CGUIElement::AddChild(CGUIElement* pChild)
{
    CGUIElement* pExist = FindChildElement(pChild);
    if (!pExist)
    {
        pChild->SetParent(this);
		pChild->SetSchemeManager( m_pScheme );//////////////////////
        m_lstChildren.push_back(pChild);
    }
    
}

void CGUIElement::SetScreenPos(int x, int y)
{
    int nWidth = GetWidth();
    int nHeight = GetHeight();
    m_renderQuad.m_rectScreen.left = x;
    m_renderQuad.m_rectScreen.top = y;
    m_renderQuad.m_rectScreen.right = x + nWidth;
    m_renderQuad.m_rectScreen.bottom = y + nHeight;
}

void CGUIElement::SetRect(int x, int y, int nWidth, int nHeight)
{
    if (m_pParent == NULL)
    {
        m_renderQuad.m_rectScreen.top = y;
        m_renderQuad.m_rectScreen.left = x;
        m_renderQuad.m_rectScreen.right = x + nWidth;
        m_renderQuad.m_rectScreen.bottom = y + nHeight;
    }
    else
    {
        m_renderQuad.m_rectScreen.top = m_pParent->m_renderQuad.m_rectScreen.top + y;
        m_renderQuad.m_rectScreen.left = m_pParent->m_renderQuad.m_rectScreen.left + x;
        m_renderQuad.m_rectScreen.right = m_renderQuad.m_rectScreen.left + nWidth;
        m_renderQuad.m_rectScreen.bottom = m_renderQuad.m_rectScreen.top + nHeight;
    }
}

void CGUIElement::SetColor(u8 nR, u8 nG, u8 nB, u8 nA)
{
    m_renderQuad.m_clrDiffuse = SColor(nA, nR, nG, nB);
}

void CGUIElement::SetColor(const SColor& color)
{
    m_renderQuad.m_clrDiffuse = color;
}

void CGUIElement::Show()
{
    m_bShow = true;
    if (m_pScriptObject)
    {
        m_pScriptObject->RunOnShowScript();
    }
    if (m_pOnShowFun)
    {
        m_pOnShowFun(this);
    }
}

void CGUIElement::Hide()
{
    m_bShow = false;

    if (m_pScriptObject)
    {
        m_pScriptObject->RunOnHideScript();
    }

    if (m_pOnHideFun)
    {
        m_pOnHideFun(this);
    }
}

bool CGUIElement::Create(CGUIElement* pParent, CMarkupSTL& xml)
{
    if (!CreateSelf(pParent, xml))
    {
        return false;
    }
    if (!CreateChild(xml))
    {
        return false;
    }

    //CreateVertexBuffer();
    return true;
}

bool CGUIElement::CreateSelf(CGUIElement* pParent, CMarkupSTL& xml)
{ 
    SetParent(pParent);
    if (pParent)
    {
        m_pScheme = pParent->GetScheme();
    }

    m_bDrawFrame = false;
    

    AIRString strAttributeValue = xml.GetAttrib("Name").c_str();
    const char* pName = strstr(strAttributeValue.c_str(), ".");
    if (pName)
    {
        m_strName = pName + 1;
    }
    else
        m_strName = strAttributeValue;

    float topLeftX, topLeftY, bottomRightX, bottomRightY;
    strAttributeValue = xml.GetAttrib("Pos").c_str();
    if (strAttributeValue.length() > 0)
    {
        char szTmp[256] = { 0 };
        sscanf(strAttributeValue.c_str(), "(%f, %f), (%f, %f)", &topLeftX, &topLeftY, &bottomRightX, &bottomRightY);
        if (!pParent)
        {
            int nScreenWidth, nScreenHeight; // = GUISYSTEM->GetHGE()->System_GetState(HGE_SCREENWIDTH);
            ENGINE_INST->GetBackBufferSize(nScreenWidth, nScreenHeight);  //GUISYSTEM->GetHGE()->System_GetState(HGE_SCREENHEIGHT);
            m_renderQuad.m_rectScreen.SetRect(nScreenWidth * topLeftX, nScreenHeight * topLeftY, nScreenWidth * bottomRightX, nScreenHeight * bottomRightY);
        }
        else
        {
            int nParentWidth = pParent->GetWidth();
            int nParentHeight = pParent->GetHeight();
            m_renderQuad.m_rectScreen.SetRect(nParentWidth * topLeftX, nParentHeight * topLeftY, nParentWidth * bottomRightX, nParentHeight * bottomRightY);
            m_renderQuad.m_rectScreen.left += pParent->m_renderQuad.m_rectScreen.left;
            m_renderQuad.m_rectScreen.top += pParent->m_renderQuad.m_rectScreen.top;
            m_renderQuad.m_rectScreen.right += pParent->m_renderQuad.m_rectScreen.left;
            m_renderQuad.m_rectScreen.bottom += pParent->m_renderQuad.m_rectScreen.top;

            m_renderQuad.m_rectScreen.left = (int)m_renderQuad.m_rectScreen.left;
            m_renderQuad.m_rectScreen.top = (int)m_renderQuad.m_rectScreen.top;
            m_renderQuad.m_rectScreen.right = (int)m_renderQuad.m_rectScreen.right;
            m_renderQuad.m_rectScreen.bottom = (int)m_renderQuad.m_rectScreen.bottom;
        }
    }
    
    int x, y, w, h;
    strAttributeValue = xml.GetAttrib("PosX").c_str();
    if (!strAttributeValue.empty())
    {
        x = atoi(strAttributeValue.c_str());

        strAttributeValue = xml.GetAttrib("PosY").c_str();
        y = atoi(strAttributeValue.c_str());

        strAttributeValue = xml.GetAttrib("Width").c_str();
        w = atoi(strAttributeValue.c_str());

        strAttributeValue = xml.GetAttrib("Height").c_str();
        h = atoi(strAttributeValue.c_str());
        if (pParent)
        {
            Rectf rectParent;
            pParent->GetRenderRect(rectParent);
            m_renderQuad.m_rectScreen.left = rectParent.left + x;
            m_renderQuad.m_rectScreen.top = rectParent.top + y;
            m_renderQuad.m_rectScreen.right = m_renderQuad.m_rectScreen.left + w;
            m_renderQuad.m_rectScreen.bottom = m_renderQuad.m_rectScreen.top + h;
            
        }
        else
        {
            m_renderQuad.m_rectScreen.left = x;
            m_renderQuad.m_rectScreen.top = y;
            m_renderQuad.m_rectScreen.right = x + w;
            m_renderQuad.m_rectScreen.bottom = y + h;
        }
    }
    
    

    strAttributeValue = xml.GetAttrib("Moveable").c_str();
    if (strAttributeValue == "TRUE")
    {
        m_bMoveable = true;
    }

    strAttributeValue = xml.GetAttrib("Show").c_str();
    if (strAttributeValue == "TRUE")
    {
        m_bShow = true;
    }
    else
    {
        m_bShow = false;
    }

    strAttributeValue = xml.GetAttrib("EdgeOffsetX").c_str();
    if (!strAttributeValue.empty())
    {
        m_nEdgeOffsetX = atoi(strAttributeValue.c_str());
    }
    
    strAttributeValue = xml.GetAttrib("EdgeOffsetY").c_str();
    if (!strAttributeValue.empty())
    {
        m_nEdgeOffsetY = atoi(strAttributeValue.c_str());
    }

    strAttributeValue = xml.GetAttrib("GUIAlign").c_str();
    if (strAttributeValue == "eGUIAlign_LeftTop")
    {
        m_alignState = eGUIAlign_LeftTop;
    }
    else if (strAttributeValue == "eGUIAlign_RightTop")
    {
        m_alignState = eGUIAlign_RightTop;
    }
    else if (strAttributeValue == "eGUIAlign_RightBottom")
    {
        m_alignState = eGUIAlign_RightBottom;
    }
    else if (strAttributeValue == "eGUIAlign_LeftBottom")
    {
        m_alignState = eGUIAlign_LeftBottom;
    }
    else if (strAttributeValue == "eGUIAlign_Middle")
    {
        m_alignState = eGUIAlign_Middle;
    }
    else if (strAttributeValue == "eGUIAlign_TopMiddle")
    {
        m_alignState = eGUIAlign_TopMiddle;
    }
    else if (strAttributeValue == "eGUIAlign_RightMiddle")
    {
        m_alignState = eGUIAlign_RightMiddle;
    }
    else if (strAttributeValue == "eGUIAlign_BottomMiddle")
    {
        m_alignState = eGUIAlign_BottomMiddle;
    }
    else if (strAttributeValue == "eGUIAlign_LeftMiddle")
    {
        m_alignState = eGUIAlign_LeftMiddle;
    }
    else
    {
        m_alignState = eGUIAlign_None;
    }

    CaculateAlignPosition();

    /*
    strAttributeValue = xml.GetAttrib("ViewState").c_str();
    if (strAttributeValue == "UI_ALWAYS_TOP")
    {
        m_uiLayerDef = UI_ALWAYS_TOP;
    }
    else if (strAttributeValue == "UI_ALWAYS_BOTTOM")
    {
        m_uiLayerDef = UI_ALWAYS_BOTTOM;
    }
    */
    
    strAttributeValue = xml.GetAttrib("DrawFrame").c_str();
    if (strAttributeValue == "TRUE")
    {
        m_bDrawFrame = true;
    }
	else
	{
		m_bDrawFrame = false;
	}

	strAttributeValue = xml.GetAttrib("Color").c_str();
	if (!strAttributeValue.empty())
	{
		m_renderQuad.m_clrDiffuse = ATOL( strAttributeValue.c_str(), 16 );
	}

    strAttributeValue = xml.GetAttrib("ImgSrc").c_str();
	m_szImgSrc = strAttributeValue;

    if (!strAttributeValue.empty())
    {
        AIRString strImgSet;
        AIRString strImgUnit;
        //string value = ( pElement->Attribute( "ImgSrc" ) );
        CGUISchemeManager* pScheme = GetScheme();

        CStringUtil::SplitFilename( strAttributeValue, strImgSet, strImgUnit );
        if( strImgSet.size() == 0)
        //    
        //if( strImgUnit.size() == 0 )
        {
            strImgSet = strAttributeValue;
            CImageSet* is = pScheme->GetImageSet( strImgSet );
            if( is ) 
            {
                //m_renderQuad.tex = is->GetTexture();
                //m_pImage = is;
                m_renderQuad.m_pTexture = is->GetTexture();
                //m_renderQuad.m_uv.Set( 0.0, 1.0, 1.0, 0.0 );
            }
        }
        else
        {
            CImageSet* is = pScheme->GetImageSetByName( strImgSet.c_str() );
            if (is)
            {
                CImageUnit* iu = is->GetImgUnit(strImgUnit); //pScheme->GetImageUnit( strImgSet, strImgUnit );
                if( iu ) 
                {
                    m_renderQuad.m_pTexture = is->GetTexture();
                    //m_renderQuad.tex = is->GetTexture();
                    m_renderQuad.m_uv = iu->m_fUV;
                }
                else
                {
                    //OutputDebugStringA( strImgUnit.c_str() );
                    DT_TO_MSGBOX( LT_WARN, GetString( "找不到UI图像: [set:%s],[unit:%s]",strImgSet.c_str(), strImgUnit.c_str() ) );
                }
            }
            
        }

        assert(m_renderQuad.m_pTexture != NULL);
    }
    
    strAttributeValue = xml.GetAttrib("Layer").c_str();
    if (strAttributeValue == "UI_Layer_Top")
    {
        m_uiLayerDef = UI_Layer_Top;
    }
    else if (strAttributeValue == "UI_Layer_Bottom")
    {
        m_uiLayerDef = UI_Layer_Bottom;
    }
    else if (strAttributeValue == "UI_Layer_Middle")
    {
        m_uiLayerDef = UI_Layer_Middle;
    }
    else if (strAttributeValue == "UI_Layer_Model")
    {
        m_uiLayerDef = UI_Layer_Model;
    }
    else if (strAttributeValue == "UI_Layer_Mouse")
    {
        m_uiLayerDef = UI_Layer_Mouse;
    }

    strAttributeValue = xml.GetAttrib("ParentLayer").c_str();
    if (strAttributeValue == "TRUE")
    {
        m_bParentLayer = true;
    }
    else
    {
        m_bParentLayer = false;
    }

    RegisterToScript();

    m_realRect = m_renderQuad.m_rectScreen;

    return true;
}

void CGUIElement::Render()
{
    if (!m_bShow)
    {
        return;
    }
    if (m_renderQuad.m_pTexture)
    {
        if (m_pFrameLayer == 0)
        {
            if (!m_bParentLayer)
            {
                if (m_pParent)
                {
                    m_pFrameLayer = m_pScheme->GetUILayer(m_uiLayerDef)->GetFrameLayer(m_pParent->m_pFrameLayer->m_nIndex + 1);
                }
                else
                {
                    m_pFrameLayer = m_pScheme->GetUILayer(m_uiLayerDef)->CreateFrameLayer();
                }
            }
            else
            {
                m_pFrameLayer = m_pParent->m_pFrameLayer;
            }
        }
        m_renderQuad.m_fDepth = m_pFrameLayer->m_fZ;
        RenderSelf();
        RenderChild();
    }
    else
    {
        AIRString strImageSet;
        AIRString strImageUnit;
        CStringUtil::SplitFilename(m_szImgSrc, strImageSet, strImageUnit);

        CImageSet* pImageSet = m_pScheme->GetImageSetByName(strImageSet.c_str());
        CImageUnit* pUnit = NULL;
        if (pImageSet)
        {
            m_renderQuad.m_pTexture = pImageSet->GetTexture();
            pUnit = pImageSet->GetImgUnit(strImageUnit);
        }
        else
        {
            //m_szImgSrc.clear();
            m_renderQuad.m_pTexture = NULL;
        }

        if (!pUnit)
        {
            return;
        }
        m_renderQuad.m_uv = pUnit->m_fUV;
    }
    
}

void CGUIElement::RenderSelf()
{
    IRenderer* pRenderer = ENGINE_INST->GetRenderer();
    DrawRenderQuad();

    if (m_bDrawFrame)
    {
        pRenderer->DrawRect(m_renderQuad.m_rectScreen, m_nFrameWidth, m_colorRectFrame);
    }
    
    //UpdateRenderQuad();
    //GUISYSTEM->GetHGE()->Gfx_RenderQuad(&m_renderQuad);
    /*
    LPDIRECT3DDEVICE9 pDevice = RENDERSYSTEM->GetDevice();
    m_renderQuad.m_pTexture->Use(0);
    //pDevice->SetTexture(0, m_renderQuad.m_pTexture->GetInterface());
    //pDevice->SetStreamSource(0, NULL, 0, 0);
    pDevice->SetFVF(RHWVERTEX::FVF);
    pDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, (void*)m_vtVertices, sizeof(RHWVERTEX));
    */
    //pDevice->SetFVF(RHWVERTEX::FVF);
    //pDevice->SetStreamSource(0, m_pVertexBuffer, 0, sizeof(RHWVERTEX));
    //pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);
}

void CGUIElement::RenderChild()
{
    for (GUIELEMENT_ITER it = m_lstChildren.begin(); it != m_lstChildren.end(); it++)
    {
        (*it)->Render();
    }
}

void CGUIElement::CleanUp()
{
    ClearSelf();
    ClearChild();
}

void CGUIElement::ClearSelf()
{
    //if (m_pHyperLink)
    //{
    //     HYPERLINK_MGR->DestroyHyperLink(m_pHyperLink);
    //}
    if (m_pScriptObject)
    {
        SafeDelete(m_pScriptObject);
    }
}

void CGUIElement::ClearChild()
{
    for (GUIELEMENT_ITER it = m_lstChildren.begin(); it != m_lstChildren.end(); ++it)
    {
        (*it)->CleanUp();
        SafeDelete(*it);
    }
    m_lstChildren.clear();
}

void CGUIElement::Update()
{
	// 如果控件不可见或者不可用,直接返回
	if ( !IsVisable() || !IsEnable() )
	{
		return;
	}

	// 首先更新自己
    UpdateSelf();   

	// 更新子控件
    for (GUIELEMENT_ITER it = m_lstChildren.begin(); it != m_lstChildren.end(); it++)
    {
        (*it)->Update();
    }
}

bool CGUIElement::CreateChild(CMarkupSTL& parent)
{
    parent.IntoElem();
    while (parent.FindElem())
    {
        AIRString strElementName = parent.GetTagName().c_str();
        if (strElementName == "Script")    //创建脚本
        {
            if( !m_pScriptObject )
			{
                m_pScriptObject = new CGUIScriptObject();	
			}
            if( !m_pScriptObject->Load(parent) )
            {
                SafeDelete(m_pScriptObject);
                //DELETE_ATOM( m_pScript );
                return false;
            }
            if (m_pScriptObject)
            {
                m_byKeyUp = m_pScriptObject->GetKeyUp();
                m_byKeyDown = m_pScriptObject->GetKeyDown();
            }
            //m_pScript->SetRunningOn(this);
        }
        else
        {
            CGUIBuilder* pBuilder = CGUIBuilderMgr::GetInstance()->GetBuilder(strElementName.c_str());

            if (!pBuilder)
            {
                continue;
            }

            CGUIElement* pUIElement = pBuilder->BuildElement(this, parent, m_pScheme);

            if (pUIElement)
            {
                AddChild(pUIElement);
            }
        }
        
        
    }
    parent.OutOfElem();
    

    return true;
}

void CGUIElement::SetRect(const Rectf& rect)
{
    //if (m_pParent == NULL)
    //{
        m_renderQuad.m_rectScreen = rect;
    //}
    //else
    //{
    //    m_renderQuad.m_rectScreen.left = rect.left + m_pParent->m_renderQuad.m_rectScreen.left;
    //    m_renderQuad.m_rectScreen.top = rect.top + m_pParent->m_renderQuad.m_rectScreen.top;
    //    m_renderQuad.m_rectScreen.right = m_renderQuad.m_rectScreen.left + rect.right - rect.left;
    //    m_renderQuad.m_rectScreen.bottom = m_renderQuad.m_rectScreen.top + rect.bottom - rect.top;
    //}
    //UpdateVertexBuffer();
}

void CGUIElement::OnStatusChange()
{
    switch(m_nCurUIState)
    {
    case UI_NORMAL:
        
        if (m_nLastUIState == UI_LBUTTONDOWN)
        {
            //if (m_pScriptObject)
            //{
            //    m_pScriptObject->RunOnLMouseUpScript();
            //}
            //m_renderQuad.m_uv = m_UVUp;
        }
        else if (m_nLastUIState == UI_HOVER)
        {
            //if (m_pScriptObject)
            //{
            //    m_pScriptObject->RunOnMouseLeaveScript();
            //}
            //m_renderQuad.m_uv = m_UVUp;
        }
        break;
        
    case UI_LBUTTONDOWN:
        if (m_nLastUIState != UI_LBUTTONDOWN)
        {
            //if (m_pScriptObject)
            //{
            //    m_pScriptObject->RunOnLMouseDownScript();
            //}
            //m_renderQuad.m_uv = m_UVDown;
        }
        break;
    case UI_HOVER:
        if (m_nLastUIState != UI_HOVER)
        {
            //if (m_pScriptObject)
            //{
            //    m_pScriptObject->RunOnMouseHoverScript();
            //}
            //m_renderQuad.m_uv = m_UVHover;
        }
        break;
    case UI_DRAGING:
        if (m_nLastUIState != UI_DRAGING)
        {
            //m_renderQuad.m_uv = m_UVInactive;
        }
        break;
        
    }
}


void CGUIElement::SetParent(CGUIElement* pParent)
{
    if (m_pParent != pParent)
    {
        if (m_pParent != NULL)
        {
            m_pParent->RemoveChild(this);
        }
        m_pParent = pParent;
        if (m_pParent)
        {
            m_pScheme = m_pParent->m_pScheme;
        }
        
        //m_pParent->AddChild(this);
    }
    
}

CGUIElement* CGUIElement::RemoveChild(const AIRString& strName)
{
    CGUIElement* pChild = NULL;
    GUIELEMENT_ITER it;
    for (it = m_lstChildren.begin(); it != m_lstChildren.end(); it++)
    {
        pChild = *it;
        if (pChild->GetName() == strName)
        {
            m_lstChildren.erase(it);
            break;
        }
    }

    return pChild;
}

CGUIElement* CGUIElement::RemoveChild(CGUIElement* pChild)
{
    GUIELEMENT_ITER it = std::find(m_lstChildren.begin(), m_lstChildren.end(), pChild);
    if (it != m_lstChildren.end())
    {
        m_lstChildren.erase(it);
        return pChild;
    }

    return NULL;
}

void CGUIElement::SetFocus(bool bFocus) 
{ 
    m_bFocus = bFocus; 
    //这里要把它的深度设到它所在队列的最浅
    //m_renderQuad.m_fDepth = 0.0f;
}

void CGUIElement::RegisterToScript()
{
    //UILUASCRIPTMGR->Declare(m_strName.c_str(), this);
}

void CGUIElement::ChangeShow()
{
    m_bShow = !m_bShow;
}

bool CGUIElement::IsInRect(int x, int y)
{
    return m_renderQuad.m_rectScreen.IsInRect(x, y);
}

bool CGUIElement::OnLButtonDown(int x, int y)
{
    CGUIElement* pElement = NULL;
    for (GUIELEMENT_ITER it = m_lstChildren.begin(); it != m_lstChildren.end(); it++)
    {
        pElement = *it;
        if (pElement->IsInRect(x, y))
        {
			if (pElement->IsVisable())
			{
                if (pElement->IsEnable())
                {
                    return pElement->OnLButtonDown(x, y);
                }
                else
                {
                    break;
                }
			}
        }
    }
    //m_bFocus = true;

    return OnLButtonDownSelf(x, y);
    
}

bool CGUIElement::OnLButtonDBClick(int x, int y)
{
    CGUIElement* pElement = NULL;
    for (GUIELEMENT_ITER it = m_lstChildren.begin(); it != m_lstChildren.end(); it++)
    {
        pElement = *it;
        if (pElement->IsInRect(x, y))
        {
            if (pElement->IsVisable())
            {
                if (pElement->IsEnable())
                {
                    return pElement->OnLButtonDBClick(x, y);
                }
                else
                {
                    break;
                }
            }
        }
    }
    //m_bFocus = true;

    return OnLButtonDBClickSelf(x, y);
}

bool CGUIElement::OnLButtonDBClickSelf(int x, int y)
{
    if (!IsEnable())
    {
        return true;
    }

    if (m_pLButtonDBClickFun)
    {
        return m_pLButtonDBClickFun(this);
    }
    return true;
}

bool CGUIElement::OnLButtonDownSelf(int x, int y)
{
	// 如果控件不可用,直接返回
    if (!IsEnable())
    {
        return true;
    }
	// 设置控件当前处于按下状态
	SetCurrentUIState( UI_LBUTTONDOWN );

    GUISYSTEM->SetFocusElement(this);
    m_bFocus = true;

	// 执行脚本事件
	if (m_pScriptObject)
	{
		m_pScriptObject->RunOnLMouseDownScript();
	}

	// 执行回调函数
    if (m_pLButtonDownFun)
    {
        return m_pLButtonDownFun(this);
    }

	// 执行超链接
    //if (m_pHyperLink)
    //{
    //    return m_pHyperLink->OnLButtonDown(x, y);
    //}

    return true;
}

bool CGUIElement::OnLButtonUp(int x, int y)
{
    CGUIElement* pElement = NULL;
    for (GUIELEMENT_ITER it = m_lstChildren.begin(); it != m_lstChildren.end(); it++)
    {
        pElement = *it;
        if (pElement->IsInRect(x, y))
        {
			if ( pElement->IsVisable() )
			{
                if (pElement->IsEnable())
                {
                    return pElement->OnLButtonUp(x, y);
                }
                else
                    break;
			}
            //break;            
        }
    }
    return OnLButtonUpSelf(x, y);

}

bool CGUIElement::OnLButtonUpSelf(int x, int y)
{
    if (!IsEnable())
    {
        return true;
    }
	// 设置控件当前处于鼠标悬浮状态
	SetCurrentUIState( UI_HOVER );

	GUISYSTEM->SetHoverElement(this);

	// 执行脚本事件
	if ( m_pScriptObject )
	{
		m_pScriptObject->RunOnLMouseUpScript();
	}

	// 执行回调函数
    if (m_pLButtonUpFun)
    {
        return m_pLButtonUpFun(this);
    }
    
    return true;
}

bool CGUIElement::OnRButtonDown(int x, int y)
{
    CGUIElement* pElement = NULL;
    for (GUIELEMENT_ITER it = m_lstChildren.begin(); it != m_lstChildren.end(); it++)
    {
        pElement = *it;
        if (pElement->IsInRect(x, y) )
        {
			if ( pElement->IsVisable() )
			{
                if (pElement->IsEnable())
                {
                    return pElement->OnRButtonDown(x, y);
                }
                else
                    break;
			}
        }
    }
    OnRButtonDownSelf(x, y);
    return true;
}

bool CGUIElement::OnRButtonUp(int x, int y)
{
    CGUIElement* pElement = NULL;
    for (GUIELEMENT_ITER it = m_lstChildren.begin(); it != m_lstChildren.end(); it++)
    {
        pElement = *it;
        if (pElement->IsInRect(x, y) )
        {
			if ( pElement->IsVisable())
			{
                if (pElement->IsEnable())
                {
                    return pElement->OnRButtonUp(x, y);
                }
                else
                    break;
			}
            //break;            
        }
    }
    return OnRButtonUpSelf(x, y);
    
}

bool CGUIElement::OnMouseMove(int x, int y)
{
    CGUIElement* pElement = NULL;
    for (GUIELEMENT_ITER it = m_lstChildren.begin(); it != m_lstChildren.end(); it++)
    {
        pElement = *it;
        if (pElement->IsInRect(x, y) )
        {
			if ( pElement->IsVisable() )
			{
                if (pElement->IsEnable())
                {
                    return pElement->OnMouseMove(x, y);
                }
                else
                    break;
			}
        }
    }
    //char szDebug[256] = { 0 };
    //snprintf(szDebug, 256, "hover ctrl:%s\n", m_strName.c_str());
    //OutputDebugStr(szDebug);
    return OnMouseMoveSelf(x, y);

}

bool CGUIElement::OnMouseWheel(int dz)
{
    return false;
}

bool CGUIElement::OnMouseLeave(int x, int y)
{
    CGUIElement* pElement = NULL;
    for (GUIELEMENT_ITER it = m_lstChildren.begin(); it != m_lstChildren.end(); it++)
    {
        pElement = *it;
        if (pElement->IsInRect(x, y) )
        {
			if (pElement->IsVisable() && pElement->IsEnable())
			{
				return pElement->OnMouseLeave(x, y);
			}
            break;
        }
    }
    return OnMouseLeaveSelf(x, y);
}

bool CGUIElement::OnMouseLeaveSelf(int x, int y)
{
	if (!IsEnable())
	{
		return true;		
	}
	// 设置控件处于一般状态
	SetCurrentUIState( UI_NORMAL );


	if (m_pScriptObject)
	{
		return m_pScriptObject->RunOnMouseLeaveScript();
	}

	if (m_pMouseLeaveFun)
	{
		return m_pMouseLeaveFun(this);
	}

    return true;
}

void    CGUIElement::SetIsDrawFrame( bool b )
{ 
    m_bDrawFrame = b; 
}

bool CGUIElement::OnMouseMoveSelf(int x, int y)
{
    if (!IsEnable())
    {
        return true;
    }

#if AIR_TARGET_PLATFORM == AIR_PLATFORM_WIN32
    if ((GetAsyncKeyState(VK_LBUTTON) & 0x8000) && m_bMoveable)
    {
		SetCurrentUIState( UI_DRAGING );
        Move(x - m_ptLastMouse.x, y - m_ptLastMouse.y);
        //OutputDebugString(GetString("move element dx=%d, dy=%d\n", x - m_ptLastMouse.x, y - m_ptLastMouse.y));
        GUISYSTEM->SetDragElement(this);
    }
    else
    {
        GUISYSTEM->SetHoverElement(this);
    }
#endif

    //OutputDebugString(GetString("mousemoveself element dx=%d, dy=%d\n", x - m_ptLastMouse.x, y - m_ptLastMouse.y));

    m_ptLastMouse.x = x;
    m_ptLastMouse.y = y;

    if (m_pMouseMoveFun)
    {
        return m_pMouseMoveFun(this);
    }

    return false;
}

void CGUIElement::Move(int dx, int dy)
{
    m_renderQuad.m_rectScreen.Move(dx, dy);
    m_realRect.Move(dx, dy);
    MoveChild(dx, dy);
}

void CGUIElement::MoveChild(int dx, int dy)
{
    CGUIElement* pElement = NULL;
    for (GUIELEMENT_ITER it = m_lstChildren.begin(); it != m_lstChildren.end(); it++)
    {
        pElement = *it;
        pElement->Move(dx, dy);
    }
}

bool CGUIElement::ProcessInput(const LPINPUTEVENT pEvent)
{
    CGUIElement* pElement = NULL;
    bool bRes = true;
    for (GUIELEMENT_ITER it = m_lstChildren.begin(); it != m_lstChildren.end(); it++)
    {
        pElement = *it;
        bRes = pElement->ProcessInput(pEvent);
        if (bRes = true)
        {
            return bRes;
        }
    }

    return ProcessInputSelf(pEvent);
}

bool CGUIElement::ProcessInputSelf(const LPINPUTEVENT pEvent)
{
    switch(pEvent->nType)
    {
    case eINPUT_MOUSE_LBUTTONDOWN:
        return OnLButtonDownSelf(pEvent->x, pEvent->y);
        break;
    case eINPUT_MOUSE_LBUTTONUP:
        return OnLButtonUpSelf(pEvent->x, pEvent->y);
        break;
    case eINPUT_MOUSE_RBUTTONDOWN:
        return OnRButtonDownSelf(pEvent->x, pEvent->y);
        break;
    case eINPUT_MOUSE_RBUTTONUP:
        return OnRButtonUpSelf(pEvent->x, pEvent->y);
        break;
    case eINPUT_MOUSE_MOVE:
        return OnMouseMoveSelf(pEvent->x, pEvent->y);
        break;
    case eINPUT_MOUSE_WHEEL:
		return OnMouseWheel( pEvent->nWheel );///////////// add by hjj 
        break;
    case eINPUT_KEYDOWN:
		return OnKeyPress( pEvent->nKey ); ///////////// add by hjj 
        break;
    case eINPUT_KEYUP:
		return OnKeyUp( pEvent->nKey ); ///////////// add by hjj 
        break;
    }
    return false;
}


//EGUI_Ctrl_Type  CGUIElement::GetCtrlType() const
//{
//    return m_ctrlType;
//}

int  CGUIElement::GetRealHeight() const
{
    return m_realRect.GetHeight();
}

int CGUIElement::GetRealWidth() const
{
    return m_realRect.GetWidth();
}

/*
void CGUIElement::GetRealRect(Rectf& rect) const
{
    rect = m_realRect;
}

Rectf& CGUIElement::GetRealRect( void )
{
    return m_realRect;
}
*/

void CGUIElement::UpdateSelf()
{
    //if (!INPUTSYSTEM->IsMouseInRect(m_renderQuad.m_rectScreen.top, m_renderQuad.m_rectScreen.left, m_renderQuad.m_rectScreen.bottom, m_renderQuad.m_rectScreen.right))
    //{
    //    m_nCurUIState = UI_NORMAL;
    //}



	//if (m_nCurUIState != m_nLastUIState)
	//{
	//	OnStatusChange();
	//	m_nLastUIState = m_nCurUIState;
	//}
        
}

CGUIElement* CGUIElement::FindChildElement(const AIRString& strName)
{
    CGUIElement* pChild = NULL;
    for (GUIELEMENT_ITER it = m_lstChildren.begin(); it != m_lstChildren.end(); ++it)
    {
        pChild = *it;
        if (pChild->GetName() == strName)
        {
            return pChild;
        }
        else 
        {
            pChild = pChild->FindChildElement(strName);
            if (pChild != NULL)
            {
                return pChild;
            }
        }
    }

    return pChild;
}

CGUIElement* CGUIElement::FindChildElement(CGUIElement* pChild)
{
    GUIELEMENT_ITER it = std::find(m_lstChildren.begin(), m_lstChildren.end(), pChild);
    if (it != m_lstChildren.end())
    {
        return *it;
    }

    return NULL;
}

bool CGUIElement::OnRButtonDownSelf(int x, int y)
{
	if (!IsEnable())
	{
		return true;
	}

	if ( m_pScriptObject )
	{
		m_pScriptObject->RunOnRMouseDownScript();
	}

    if (m_pRButtonDownFun)
    {
        return m_pRButtonDownFun(this);
    }

	//if ( m_pHyperLink )
	//{
	//	return m_pHyperLink->OnRButtonDown( x, y );
	//}

    return false;
}

bool CGUIElement::OnRButtonUpSelf(int x, int y)
{
	if (!IsEnable())
	{
		return true;
	}

	if ( m_pScriptObject )
	{
		m_pScriptObject->RunOnRMouseUpScript();
	}

    if (m_pRButtonUpFun)
    {
        return m_pRButtonUpFun(this);
    }

    return true;
}


//void CGUIElement::SetUICurrentState(GUIELEMENT_STATE state)
//{
//	m_nLastUIState = m_nCurUIState;
//    m_nCurUIState = state;
//	if ( m_nCurUIState != m_nLastUIState )
//	{
//		OnStatusChange();
//	}
//}

bool CGUIElement::OnKeyPress(int nKey)
{
    bool bRes = false;
    CGUIElement* pElement = NULL;
    for (GUIELEMENT_ITER it = m_lstChildren.begin(); it != m_lstChildren.end(); it++)
    {
        pElement = *it;
        bRes = pElement->OnKeyPress(nKey);
        if (bRes == true)
        {
            return true;
        }
    }
    return OnKeyPressSelf(nKey);
}

bool CGUIElement::OnKeyPressSelf(int nKey)
{
    if (m_pKeyPressFun)
    {
        return m_pKeyPressFun(this, nKey);
    }
    return false;
}

bool CGUIElement::OnKeyUp(int nKey)
{
    return OnKeyUpSelf(nKey);
}

bool CGUIElement::OnKeyUpSelf(int nKey)
{
    return false;
}

void CGUIElement::SetLButtonDownFun(LButtonDownCallback pFun)
{
    m_pLButtonDownFun = pFun;
}

void CGUIElement::SetLButtonUpFun(LButtonUpCallBack pFun)
{
    m_pLButtonUpFun = pFun;
}

void CGUIElement::SetRButtonDownFun(RButtonDownCallBack pFun)
{
    m_pRButtonDownFun = pFun;
}

void CGUIElement::SetRButtonUpFun(RButtonUpCallBack pFun)
{
    m_pRButtonUpFun = pFun;
}

void CGUIElement::SetKeyPressFun(KeyPressCallBack pFun)
{
    m_pKeyPressFun = pFun;
}

void CGUIElement::SetMouseMoveFun(MouseMoveCallBack pFun)
{
    m_pMouseMoveFun = pFun;
}

void CGUIElement::SetMouseLeaveFun(MouseLeaveCallBack pFun)
{
    m_pMouseLeaveFun = pFun;
}

void CGUIElement::SetOnShowFun(OnShowCallBack pFun)
{
    m_pOnShowFun = pFun;
}

CGUIElement* CGUIElement::GetChild(const char* szName)
{
    for (GUIELEMENT_ITER it = m_lstChildren.begin(); it != m_lstChildren.end(); it++)
    {
        CGUIElement* pElement = *it;
        if (strcmp(pElement->GetName().c_str(), szName) == 0)
        {
            return pElement;
        }
    }
    return NULL;
}

void CGUIElement::MoveRealRect(int dx, int dy)
{
    m_realRect.Move(dx, dy);
}

bool CGUIElement::OnDrag(int x, int y)
{
    CGUIElement* pElement = NULL;
    for (GUIELEMENT_ITER it = m_lstChildren.begin(); it != m_lstChildren.end(); it++)
    {
        pElement = *it;
        if (pElement->IsInRect(x, y) && pElement->IsVisable())
        {
            return pElement->OnDrag(x, y);

        }
    }
    return OnDragSelf(x, y);
}

bool CGUIElement::OnDragSelf(int x, int y)
{
    return true;
}

void CGUIElement::SetSchemeManager(CGUISchemeManager* pScheme)
{
    m_pScheme = pScheme;
}

inline CGUISchemeManager* CGUIElement::GetScheme() const
{
    //if (m_pScheme != NULL)
    //{
        return m_pScheme;
    //}

    //if (m_pParent)
    //{
    //    return m_pParent->GetScheme();
    //}

    //return NULL;
}

void CGUIElement::SetLayerDef(UILayerDefine viewState)
{
    if (m_uiLayerDef != viewState)
    {
        m_uiLayerDef = viewState;
        if (m_pParent == NULL )
        {
            //m_pScheme->RemoveEntity(this, false);   //从原来的列表删除
            //m_pScheme->AddEntity(this);     
        }
    }
}

void CGUIElement::SetCenterPos(int x, int y)
{
    //int nWidth = GetRealWidth();
    //int nHeight = GetRealHeight();
    //int orgX = m_realRect.left;
    //int orgY = m_realRect.top;
    //m_realRect.left = x - nWidth / 2;
    //m_realRect.top = y - nHeight / 2;
    //m_realRect.right = m_realRect.left + nWidth;
    //m_realRect.bottom = m_realRect.top + nHeight;

    //int dx = m_realRect.left - orgX;
    //int dy = m_realRect.top - orgY;
    //m_renderQuad.m_rectScreen.Move(dx, dy);
    int dx = x - /*GetRealWidth()/2*/(m_renderQuad.m_rectScreen.left + (m_renderQuad.m_rectScreen.right - m_renderQuad.m_rectScreen.left) / 2);
    int dy = y - /*GetRealHeight()/2*/(m_renderQuad.m_rectScreen.top + (m_renderQuad.m_rectScreen.bottom - m_renderQuad.m_rectScreen.top) / 2);
    Move( dx, dy );
}

void CGUIElement::CaculateAlignPosition()
{
    if (m_alignState == eGUIAlign_None)
    {
        return;
    }
 
    int nLeftTopX = 0;     //父控件的左上角
    int nLeftTopY = 0;
    int nRightBottomX = 0;
    int nRightBottomY = 0;
    
    ENGINE_INST->GetBackBufferSize(nRightBottomX, nRightBottomY);
    int nParentWidth = nRightBottomX;
    int nParentHeight = nRightBottomY;
    if (m_pParent)
    {
        nLeftTopX = m_pParent->GetLeft();
        nLeftTopY = m_pParent->GetTop();
        nRightBottomX = m_pParent->GetRight();
        nRightBottomY = m_pParent->GetBottom();
        nParentWidth = nRightBottomX - nLeftTopX;
        nParentHeight = nRightBottomY - nLeftTopY;
    }

    //int dx = 0;
    //int dy = 0;

    switch (m_alignState)
    {
    case eGUIAlign_LeftTop:
        MoveTo(nLeftTopX + m_nEdgeOffsetX, nLeftTopY + m_nEdgeOffsetY);
        //dx = nLeftTopX + m_nEdgeOffsetX - m_renderQuad.m_rectScreen.left;
        //dy = nLeftTopY + m_nEdgeOffsetY - m_renderQuad.m_rectScreen.top;
        break;
    case eGUIAlign_RightTop:
        MoveTo(nRightBottomX - m_renderQuad.m_rectScreen.GetWidth() - m_nEdgeOffsetX, nLeftTopY + m_nEdgeOffsetY);
        return;
        //dx = nRightBottomX - m_nEdgeOffsetX - m_renderQuad.m_rectScreen.right;
        //dy = nLeftTopY + m_nEdgeOffsetY - m_renderQuad.m_rectScreen.top;
        //break;
    case eGUIAlign_RightBottom:
        //dx = nRightBottomX - m_nEdgeOffsetX - m_renderQuad.m_rectScreen.right;
        //dy = nRightBottomY - m_nEdgeOffsetY - m_renderQuad.m_rectScreen.bottom;
        //DT_TO_DBGSTR(LT_WARN, GetString("now the nEdgeOffsetY is %d\n", m_nEdgeOffsetY));
        MoveTo(nRightBottomX - m_renderQuad.m_rectScreen.GetWidth() - m_nEdgeOffsetX, nRightBottomY - m_renderQuad.m_rectScreen.GetHeight() - m_nEdgeOffsetY);
        return;
        break;
    case eGUIAlign_LeftBottom:
        MoveTo(nLeftTopX + m_nEdgeOffsetX, nRightBottomY - m_renderQuad.m_rectScreen.GetHeight() - m_nEdgeOffsetY);
        return;
        //dx = nLeftTopX + m_nEdgeOffsetX - m_renderQuad.m_rectScreen.left;
        //dy = nRightBottomY - m_nEdgeOffsetY - m_renderQuad.m_rectScreen.bottom;
        break;
    case eGUIAlign_Middle:
        SetCenterPos((nLeftTopX + nRightBottomX) / 2, (nLeftTopY + nRightBottomY) / 2);
        return;
    case eGUIAlign_TopMiddle:
        MoveTo(nLeftTopX + nParentWidth / 2 - m_renderQuad.m_rectScreen.GetWidth() / 2, nLeftTopY + m_nEdgeOffsetY);
        //SetCenterPos((nLeftTopX + nRightBottomX) / 2, (nLeftTopY + m_renderQuad.m_rectScreen.GetHeight()) / 2 + m_nEdgeOffsetY);
        return;
    case eGUIAlign_RightMiddle:
        MoveTo(nRightBottomX - m_renderQuad.m_rectScreen.GetWidth() - m_nEdgeOffsetX, nLeftTopY + nParentHeight / 2 - m_renderQuad.m_rectScreen.GetHeight() / 2);
        //SetCenterPos((nRightBottomX - m_renderQuad.m_rectScreen.left) / 2 - m_nEdgeOffsetX, (nLeftTopY + nRightBottomY) / 2);
        return;
    case eGUIAlign_BottomMiddle:
        MoveTo(nLeftTopX + nParentWidth / 2 - m_renderQuad.m_rectScreen.GetWidth() / 2, nRightBottomY - m_renderQuad.m_rectScreen.GetHeight() - m_nEdgeOffsetY);
        //SetCenterPos((nLeftTopX + nRightBottomX) / 2, (nRightBottomY - m_renderQuad.m_rectScreen.top) / 2 - m_nEdgeOffsetY);
        return;
    case eGUIAlign_LeftMiddle:
        MoveTo(nLeftTopX + m_nEdgeOffsetX, nLeftTopY + nParentHeight / 2 - m_renderQuad.m_rectScreen.GetHeight() / 2);
        //SetCenterPos((nLeftTopX + m_renderQuad.m_rectScreen.right) / 2 + m_nEdgeOffsetX, (nLeftTopY + nRightBottomY) / 2);
        return;
    }

    //Move(dx, dy);
}

void CGUIElement::SetGUIAlignState(GUIALIGN_STATE alignState)
{
    m_alignState = alignState;
    CaculateAlignPosition();
}

void CGUIElement::SetRealRect(const Rectf& rect)
{
    m_realRect = rect;
}

void CGUIElement::SetAfterLoad()
{
    SetAfterLoadSelf();
    SetAfterLoadChild();
}

void CGUIElement::SetAfterLoadSelf()
{

}

void CGUIElement::SetAfterLoadChild()
{
    for (GUIELEMENT_ITER it = m_lstChildren.begin(); it != m_lstChildren.end(); ++it)
    {
        (*it)->SetAfterLoad();
    }
}

void CGUIElement::MoveRealRectTo(int nPosX, int nPosY)
{
    m_realRect.MoveTo(nPosX, nPosY);
}

void CGUIElement::MoveTo(int nPosX, int nPosY)
{
    //m_realRect.MoveTo(nPosX, nPosY);
    int dx = nPosX - m_renderQuad.m_rectScreen.left;
    int dy = nPosY - m_renderQuad.m_rectScreen.top;
    this->Move( dx, dy );
}

void CGUIElement::SetDepth(float fDepth)
{
    SetDepthSelf(fDepth);
    SetDepthChild(fDepth);
}

void CGUIElement::SetDepthSelf(float fDepth)
{
    //m_fDepth = fDepth;
    m_renderQuad.m_fDepth = fDepth;
}

void CGUIElement::SetDepthChild(float fDepth)
{
    for (GUIELEMENT_ITER it = m_lstChildren.begin(); it != m_lstChildren.end(); ++it)
    {
        (*it)->SetDepth(fDepth);
    }
}



bool			CGUIElement::SaveToXML( CMarkupSTL& xml )
{
	if ( m_strType == "CGUIEditCtrl" )
	{
		return true;
	}
	SaveSelfToXML( xml );
	SaveChildToXML( xml );
	SaveScriptObject( xml );
	return true;
}


bool			CGUIElement::SaveSelfToXML( CMarkupSTL& xml )
{
	xml.AddElem( m_strType.c_str() );
	xml.AddAttrib( "Name", m_strName.c_str() );

	xml.AddAttrib( "Color", m_renderQuad.m_clrDiffuse.GetAsHexString().c_str() );

	int x, y, w, h;
	if ( m_pParent )
	{
		x = m_renderQuad.m_rectScreen.left - m_pParent->GetRenderRect().left;
		y = m_renderQuad.m_rectScreen.top - m_pParent->GetRenderRect().top;
	}
	else
	{
		x = m_renderQuad.m_rectScreen.left;
		y = m_renderQuad.m_rectScreen.top;
	}
	w = m_renderQuad.m_rectScreen.GetWidth();
	h = m_renderQuad.m_rectScreen.GetHeight();
	xml.AddAttrib( "PosX", GetString("%d",x) );
	xml.AddAttrib( "PosY", GetString("%d",y) );
	xml.AddAttrib( "Width", GetString("%d",w) );
	xml.AddAttrib( "Height", GetString("%d",h) );

	if ( m_bMoveable )
	{
		xml.AddAttrib( "Moveable", "TRUE" );
	}
	else
	{
		xml.AddAttrib( "Moveable", "FALSE" );
	}

	if ( m_bShow )
	{
		xml.AddAttrib( "Show", "TRUE" );
	}
	else
	{
		xml.AddAttrib( "Show", "FALSE" );
	}

	xml.AddAttrib( "EdgeOffsetX", GetString("%d",m_nEdgeOffsetX) );
	xml.AddAttrib( "EdgeOffsetY", GetString("%d",m_nEdgeOffsetY) );

	if (m_alignState == eGUIAlign_LeftTop)
	{
		xml.AddAttrib( "GUIAlign", "eGUIAlign_LeftTop" );
	}
	else if (m_alignState == eGUIAlign_RightTop)
	{
		xml.AddAttrib( "GUIAlign", "eGUIAlign_RightTop" );
	}
	else if (m_alignState == eGUIAlign_RightBottom)
	{
		xml.AddAttrib( "GUIAlign", "eGUIAlign_RightBottom" );
	}
	else if (m_alignState == eGUIAlign_LeftBottom)
	{
		xml.AddAttrib( "GUIAlign", "eGUIAlign_LeftBottom" );
	}
	else if (m_alignState == eGUIAlign_Middle)
	{
		xml.AddAttrib( "GUIAlign", "eGUIAlign_Middle" );
	}
    else if (m_alignState == eGUIAlign_TopMiddle)
    {
        xml.AddAttrib( "GUIAlign", "eGUIAlign_TopMiddle" );
    }
    else if (m_alignState == eGUIAlign_RightMiddle)
    {
        xml.AddAttrib( "GUIAlign", "eGUIAlign_RightMiddle" );
    }
    else if (m_alignState == eGUIAlign_BottomMiddle)
    {
        xml.AddAttrib( "GUIAlign", "eGUIAlign_BottomMiddle" );
    }
    else if (m_alignState == eGUIAlign_LeftMiddle)
    {
        xml.AddAttrib( "GUIAlign", "eGUIAlign_LeftMiddle" );
    }
	else
	{
		xml.AddAttrib( "GUIAlign", "eGUIAlign_None" );
	}

	if ( m_bDrawFrame )
	{
		xml.AddAttrib( "DrawFrame", "TRUE" );
	}
	else
	{
		xml.AddAttrib( "DrawFrame", "FALSE" );
	}

	if ( m_szImgSrc.size() > 0 )
	{
		xml.AddAttrib( "ImgSrc", m_szImgSrc.c_str() );
	}

    if (m_uiLayerDef == UI_Layer_Middle)
    {
        xml.AddAttrib("Layer", "UI_Layer_Middle");
    }
    else if (m_uiLayerDef == UI_Layer_Top)
    {
        xml.AddAttrib("Layer", "UI_Layer_Top");
    }
    else if (m_uiLayerDef == UI_Layer_Bottom)
    {
        xml.AddAttrib("Layer", "UI_Layer_Bottom");
    }
    else if (m_uiLayerDef == UI_Layer_Model)
    {
        xml.AddAttrib("Layer", "UI_Layer_Model");
    }
    else if (m_uiLayerDef == UI_Layer_Mouse)
    {
        xml.AddAttrib("Layer", "UI_Layer_Mouse");
    }

    xml.AddAttrib("ParentLayer", m_bParentLayer ? "TRUE" : "FALSE");

	return true;
}


bool			CGUIElement::SaveChildToXML( CMarkupSTL& xml )
{
	for ( GUIELEMENT_ITER itor = m_lstChildren.begin(); itor != m_lstChildren.end(); ++itor )
	{
		xml.IntoElem();
		(*itor)->SaveToXML( xml );
		xml.OutOfElem();
	}
	return true;
}

CGUIElementIterator CGUIElement::GetChildrenIterator()
{
    return CGUIElementIterator(m_lstChildren.begin(), m_lstChildren.end());
}

void CGUIElement::SetFrameColor(const SColor color)
{
    m_colorRectFrame = color;
}

void CGUIElement::SetImageSrc(const AIRString& strImageSrc)
{
    if (strImageSrc == m_szImgSrc || !m_pScheme)
    {
        return;
    }
    m_szImgSrc = strImageSrc;
    AIRString strImageSet;
    AIRString strImageUnit;
    CStringUtil::SplitFilename(strImageSrc, strImageSet, strImageUnit);

    CImageSet* pImageSet = m_pScheme->GetImageSet(strImageSet);
    CImageUnit* pUnit = NULL;
    if (pImageSet)
    {
        m_renderQuad.m_pTexture = pImageSet->GetTexture();
        pUnit = pImageSet->GetImgUnit(strImageUnit);
    }
    else
    {
        //m_szImgSrc.clear();
        m_renderQuad.m_pTexture = NULL;
    }
    
    if (!pUnit)
    {
        return;
    }
    m_renderQuad.m_uv = pUnit->m_fUV;
    
    //if (m_renderQuad.m_pTexture)
    //{
    //    GUISYSTEM->ClearTexture(m_renderQuad.m_pTexture);
    //}

    
}

void CGUIElement::SetEdgeOffsetX(int nX)
{
    m_nEdgeOffsetX = nX;
    CaculateAlignPosition();
}

void CGUIElement::SetEdgeOffsetY(int nY)
{
    m_nEdgeOffsetY = nY;
    CaculateAlignPosition();
    //OutputDebugStr(GetString("set the edgeoffsety to %d\n", nY));
}

void CGUIElement::SetMovable(bool bMovable)
{
    m_bMoveable = bMovable;
}

void CGUIElement::SetWidth(int nWidth)
{
    m_renderQuad.m_rectScreen.right = m_renderQuad.m_rectScreen.left + nWidth;
}

void CGUIElement::SetHeight(int nHeight)
{
    m_renderQuad.m_rectScreen.bottom = m_renderQuad.m_rectScreen.top + nHeight;
}

void CGUIElement::AutoSize()
{
    if (!m_pScheme)
    {
        return;
    }
    AIRString strImgSet, strImgUnit;
    CStringUtil::SplitFilename(m_szImgSrc, strImgSet, strImgUnit);
    CImageUnit* pUnit = m_pScheme->GetImageUnit(strImgSet, strImgUnit);
    if (pUnit)
    {
        SetWidth(pUnit->m_nWidth);
        SetHeight(pUnit->m_nHeight);
    }
}

//void CGUIElement::SetHyperLink(CGUIHyperLink* pLink)
//{
//    m_pHyperLink = pLink;
//}


void CGUIElement::NotifyTextureDestroy(ITexture* pTex)
{
    NotifyTextureDestroySelf( pTex );
    NotifyTextureDestroyChild( pTex );
}

//************************************
// Method:    NotifyTextureDestroySelf
// Returns:   void
// Qualifier: 删除自身控件里面用到的纹理pTex
// Parameter: ITexture * pTex
//************************************
void CGUIElement::NotifyTextureDestroySelf(ITexture* pTex)
{
    if ( m_renderQuad.m_pTexture == pTex )
    {
        m_renderQuad.m_pTexture = NULL;
        m_szImgSrc.clear();
    }
}

//************************************
// Method:    NotifyTextureDestroyChild
// Returns:   void
// Qualifier: 删除子控件里面用到的纹理pTex
// Parameter: ITexture * pTex
//************************************
void CGUIElement::NotifyTextureDestroyChild(ITexture* pTex)
{
    GUIELEMENT_ITER it = m_lstChildren.begin();
    for ( ; it != m_lstChildren.end(); ++it )
    {
        (*it)->NotifyTextureDestroy( pTex );
    }
}

bool CGUIElement::SaveTextBlockToXML(CMarkupSTL& xml, IFontBase* pFont)
{
    //xml.IntoElem();
    xml.AddElem( "CXMLTextBlock" );

    const FONTTYPE* pFT = pFont->GetFontTypePtr();
    xml.AddAttrib( "Face", pFT->m_strFaceName.c_str() );
    xml.AddAttrib( "SizeID", GetString("%d",pFT->m_nFontSize) );
    if ( pFT->m_bBold )
    {
        xml.AddAttrib( "Bold", "TRUE" );
    }
    else
    {
        xml.AddAttrib( "Bold", "FALSE" );
    }

    if ( pFT->m_bItalic )
    {
        xml.AddAttrib( "Italic", "TRUE" );
    }
    else
    {
        xml.AddAttrib( "Italic", "FALSE" );
    }

    if ( pFT->m_bUnderLine )
    {
        xml.AddAttrib( "Underline", "TRUE" );
    }
    else
    {
        xml.AddAttrib( "Underline", "FALSE" );
    }

    /*
    switch ( pFT->m_nFontHint )
    {
    case FONT_BMP:			xml.AddAttrib( "Style", "FONT_BMP" );		break;
    case FONT_BMP_GRAY2:	xml.AddAttrib( "Style", "FONT_BMP_GRAY2" ); break;
    case FONT_BMP_GRAY4:	xml.AddAttrib( "Style", "FONT_BMP_GRAY4" ); break;
    case FONT_BMP_GRAY8:	xml.AddAttrib( "Style", "FONT_BMP_GRAY8" ); break;
    }
    */

    xml.AddAttrib( "FontColor", m_renderQuad.m_clrDiffuse.GetAsHexString().c_str() );

    // 保存文本内容
    /*
    if ( m_strText.length() > 0 )
    {
        AIRString s = CStringUtil::ws2s(m_strText);
        xml.SetData( s.c_str() );
    }
    */

    //xml.OutOfElem();
    return true;
}

bool CGUIElement::SaveFontToXML(CMarkupSTL& xml, IFontBase* pFont)
{
    xml.IntoElem();
    xml.AddElem( "GUIFont" );

    const FONTTYPE* pFT = pFont->GetFontTypePtr();
    xml.AddAttrib( "Face", pFT->m_strFaceName.c_str() );
    xml.AddAttrib( "SizeID", GetString("%d",pFT->m_nFontSize) );
    if ( pFT->m_bBold )
    {
        xml.AddAttrib( "Bold", "TRUE" );
    }
    else
    {
        xml.AddAttrib( "Bold", "FALSE" );
    }

    if ( pFT->m_bItalic )
    {
        xml.AddAttrib( "Italic", "TRUE" );
    }
    else
    {
        xml.AddAttrib( "Italic", "FALSE" );
    }

    if ( pFT->m_bUnderLine )
    {
        xml.AddAttrib( "Underline", "TRUE" );
    }
    else
    {
        xml.AddAttrib( "Underline", "FALSE" );
    }

    xml.AddAttrib("Border", pFT->m_nBorderSize);

    /*
    switch ( pFT->m_nFontHint )
    {
    case FONT_BMP:			xml.AddAttrib( "Style", "FONT_BMP" );		break;
    case FONT_BMP_GRAY2:	xml.AddAttrib( "Style", "FONT_BMP_GRAY2" ); break;
    case FONT_BMP_GRAY4:	xml.AddAttrib( "Style", "FONT_BMP_GRAY4" ); break;
    case FONT_BMP_GRAY8:	xml.AddAttrib( "Style", "FONT_BMP_GRAY8" ); break;
    }
    */

    xml.OutOfElem();

    return true;
}

IFontBase* CGUIElement::CreateGUIFont(CMarkupSTL& xml)
{
    IFontBase* pFont = NULL;

    AIRString strAttribute = xml.GetAttrib("Face").c_str();
    AIRString strText = xml.GetData().c_str();
    FONTTYPE ft;
    ft.m_strFaceName = strAttribute;

    strAttribute = xml.GetAttrib("SizeID").c_str();
    if (strAttribute.length() > 0)
    {
        ft.m_nFontSize = atoi(strAttribute.c_str());
    }

    strAttribute = xml.GetAttrib("Bold").c_str();
    if (strAttribute == "TRUE")
    {
        ft.m_bBold = true;
    }
    else
        ft.m_bBold = false;

    strAttribute = xml.GetAttrib("Underline").c_str();
    if (strAttribute == "TRUE")
    {
        ft.m_bUnderLine = true;
    }
    else
    {
        ft.m_bUnderLine = false;
    }

    strAttribute = xml.GetAttrib("Style").c_str();
    /*
    if (strAttribute == "FONT_BMP")
    {
        ft.m_nFontHint = FONT_BMP;
    }
    else if (strAttribute == "FONT_BMP_GRAY2")
    {
        ft.m_nFontHint = FONT_BMP_GRAY2;
    }
    else if (strAttribute == "FONT_BMP_GRAY4")
    {
        ft.m_nFontHint = FONT_BMP_GRAY4;
    }
    else if (strAttribute == "FONT_BMP_GRAY8")
    {
        ft.m_nFontHint = FONT_BMP_GRAY8;
    }
    */

    strAttribute = xml.GetAttrib("Border").c_str();
    if (strAttribute.length() > 0)
    {
        ft.m_nBorderSize = atoi(strAttribute.c_str());
    }

    pFont = FONTMGR->CreateFontA(ft, "0123456789");

    //SafeDeleteArray(wText);

    //strAttribute = xml.GetAttrib("FontColor");

    return pFont;
}

void CGUIElement::SetTexture(ITexture* pTexture)
{
    m_renderQuad.m_pTexture = pTexture;
}

void CGUIElement::SetUVRect(const Rectf& rcUV)
{
    m_renderQuad.m_uv = rcUV;
}

void CGUIElement::SetCurrentUIState(GUIELEMENT_STATE state)
{
    m_nLastUIState = m_nCurUIState;
    m_nCurUIState = state;
	//if ( m_nLastUIState != m_nCurUIState )
	{
		OnStatusChange();
	}
}


bool CGUIElement::SaveScriptObject( CMarkupSTL& xml )
{
	if ( m_pScriptObject && m_pScriptObject->IsScriptObjectNotEmpty() )
	{
		xml.IntoElem();
		xml.AddElem( "Script" );
		m_pScriptObject->SaveToXML( xml );
		xml.OutOfElem();
	}
	return true;
}

void CGUIElement::SetOnHideFun(OnHideCallBack pFun)
{
    m_pOnHideFun = pFun;
}

void CGUIElement::MoveToTop()
{
    //if (m_pScheme)
    //{
    //    m_pScheme->MoveElementToTop(this);
    //}
    UILayer* pLayer = m_pScheme->GetUILayer(m_uiLayerDef);
    pLayer->MoveFrameLayerTop(m_pFrameLayer);
}

void CGUIElement::SetLButtonDownScript(const AIRString& strScript)
{
    if (m_pScriptObject == NULL)
    {
        m_pScriptObject = new CGUIScriptObject;
    }
    m_pScriptObject->SetLMouseDownScript(strScript);
}

void CGUIElement::SetLButtonUpScript(const AIRString& strScript)
{
    if (m_pScriptObject == NULL)
    {
        m_pScriptObject = new CGUIScriptObject;
    }
    m_pScriptObject->SetLMouseUpScript(strScript);
}

void CGUIElement::SetRButtonDownScript(const AIRString& strScript)
{
    if (m_pScriptObject == NULL)
    {
        m_pScriptObject = new CGUIScriptObject;
    }
    m_pScriptObject->SetRMouseDownScript(strScript);
}

void CGUIElement::SetRButtonUpScript(const AIRString& strScript)
{
    if (m_pScriptObject == NULL)
    {
        m_pScriptObject = new CGUIScriptObject;
    }
    m_pScriptObject->SetRMouseUpScript(strScript);
}

void CGUIElement::CopyFrom(const CGUIElement* pOther)
{
    m_pScheme = pOther->m_pScheme;
    m_nCurUIState = pOther->m_nCurUIState;
    m_realRect = pOther->m_realRect;
    m_renderQuad.m_pTexture = pOther->m_renderQuad.m_pTexture;
    if (pOther->m_pScriptObject)
    {
        m_pScriptObject = new CGUIScriptObject;
        *m_pScriptObject = *(pOther->m_pScriptObject);
    }
    m_renderQuad.m_clrDiffuse = pOther->m_renderQuad.m_clrDiffuse;
    m_szImgSrc = pOther->m_szImgSrc;
    m_strType = pOther->m_strType;
    m_strName = pOther->m_strName + GetString("_copy%d", rand());
    m_renderQuad.m_uv = pOther->m_renderQuad.m_uv;
    m_alignState = pOther->m_alignState;
    m_uiLayerDef = pOther->m_uiLayerDef;
    m_nEdgeOffsetX = pOther->m_nEdgeOffsetX;
    m_nEdgeOffsetY = pOther->m_nEdgeOffsetY;
    m_renderQuad = pOther->m_renderQuad;
    //m_pHyperLink = pOther->m_pHyperLink;

    m_pOnHideFun = pOther->m_pOnHideFun;
    m_pOnShowFun = pOther->m_pOnShowFun;
    m_pLButtonDBClickFun = pOther->m_pLButtonDBClickFun;
    m_pLButtonDownFun = pOther->m_pLButtonDownFun;
    m_pLButtonUpFun = pOther->m_pLButtonUpFun;
    m_pRButtonDownFun = pOther->m_pRButtonDownFun;
    m_pRButtonUpFun = pOther->m_pRButtonUpFun;

}

void CGUIElement::DrawRenderQuad()
{
    if (m_renderQuad.m_pTexture)
    {
        //GUIRENDERER->Render(m_renderQuad.m_rectScreen, m_renderQuad.m_uv, m_renderQuad.m_fDepth, m_renderQuad.m_pTexture, m_renderQuad.m_clrDiffuse);
        GUIRENDERER->Render(&m_renderQuad);
    }
}
