#include "GUITabCtrl.h"
#include "ImageSet.h"
#include "StringUtil.h"
#include "GUISystem.h"
#include "GUISchemeManager.h"

CGUITabButton::CGUITabButton()
{
	m_strType = "CGUITabButton";
	m_szPanelElemName.clear();
	m_pPanel = NULL;
	m_nTabID = 0;
	m_bMoveable = false;
}

CGUITabButton::~CGUITabButton()
{

}

void	CGUITabButton::SetTabID( int id )
{
	m_nTabID = id;
}

int		CGUITabButton::GetTabID( void )
{
	return m_nTabID;
}

void    CGUITabButton::SetCheck( bool bCheck )
{
	CGUICheckBox::SetCheck(bCheck);
	if ( m_pPanel )
	{
		if ( bCheck )
		{
			m_pPanel->Show();
		}
		else
		{
			m_pPanel->Hide();
		}
	}
}

bool	CGUITabButton::SetImage( const AIRString& szUnChecked, const AIRString& szChecked, const AIRString& szInactive )
{
	AIRString strImgSet;
	AIRString strImgUnit;
	CImageSet* pImageSet = NULL;
	CImageUnit* pIU = NULL;

	if ( szUnChecked.length() > 0 )
	{
		CStringUtil::SplitFilename( szUnChecked, strImgSet, strImgUnit );
		if( strImgSet.size() == 0 && strImgUnit.size() == 0 )
			strImgSet = m_szImgSrcUnChecked;
        pImageSet = GetScheme()->GetImageSetByName(strImgSet.c_str());
		pIU = pImageSet->GetImgUnit(strImgUnit); //GetScheme()->GetImageUnit(strImgSet, strImgUnit); 
		if( pIU ) 
		{
			m_szImgSrcUnChecked = szUnChecked;
			m_UVUnChecked = pIU->m_fUV;
			m_renderQuad.m_pTexture = pIU->m_pSet->GetTexture();
		}
		else
		{
			DT_TO_MSGBOX( LT_ERROR, GetString("Get ImgSrcUnChecked [%s] Failed!", m_szImgSrcUnChecked.c_str()) );
		}
	}

	if ( szChecked.length() > 0 )
	{
		CStringUtil::SplitFilename( szChecked, strImgSet, strImgUnit );
		if( strImgSet.size() == 0 && strImgUnit.size() == 0 )
			strImgSet = szChecked;
        pImageSet = GetScheme()->GetImageSetByName(strImgSet.c_str());
        pIU = pImageSet->GetImgUnit(strImgUnit);  
        //pIU = GetScheme()->GetImageUnit(strImgSet, strImgUnit); 
		if( pIU ) 
		{
			m_szImgSrcChecked = szChecked;
			m_UVChecked = pIU->m_fUV;
			m_renderQuad.m_pTexture = pIU->m_pSet->GetTexture();
		}
		else
		{
			DT_TO_MSGBOX( LT_ERROR, GetString("Get ImgSrcChecked [%s] Failed!", m_szImgSrcChecked.c_str()) );
		}
	}

	if ( szInactive.length() > 0 )
	{
		CStringUtil::SplitFilename( szInactive, strImgSet, strImgUnit );
		if( strImgSet.size() == 0 && strImgUnit.size() == 0 )
			strImgSet = szInactive;

        pImageSet = GetScheme()->GetImageSetByName(strImgSet.c_str());
        pIU = pImageSet->GetImgUnit(strImgUnit);  
		//pIU = GetScheme()->GetImageUnit(strImgSet, strImgUnit); //TextureMgr::GetInstancePtr()->GetImageUnit( strImgSet, strImgUnit );
		if( pIU ) 
		{
			m_szImgSrcInactive = szInactive;
			m_UVInactive = pIU->m_fUV;
			m_renderQuad.m_pTexture = pIU->m_pSet->GetTexture();
		}
		else
		{
			DT_TO_MSGBOX( LT_ERROR, GetString("Get ImgSrcInactive [%s] Failed!", m_szImgSrcInactive.c_str()) );
		}
	}

	if ( m_bChecked )
	{
		m_renderQuad.m_uv = m_UVChecked;
	}
	else
	{
		m_renderQuad.m_uv = m_UVUnChecked;
	}

	return true;
}

void	CGUITabButton::SetPanel( CGUIElement* pPanel )
{
	m_pPanel = pPanel;
}


// render the control itself
void    CGUITabButton::RenderSelf( void )
{
	CGUICheckBox::RenderSelf();
}

// create the control from xml
bool    CGUITabButton::CreateSelf( CGUIElement* pParent, CMarkupSTL& xml )
{
	CGUICheckBox::CreateSelf( pParent, xml );

	AIRString szValue = xml.GetAttrib( "PanelName" ).c_str();
	if ( szValue.length() > 0 )
	{
		m_szPanelElemName = szValue;
	}

	szValue = xml.GetAttrib( "TabID" ).c_str();
	if ( szValue.length() > 0 )
	{
		m_nTabID = atoi( szValue.c_str() );
	}

	SetImage( m_szImgSrcUnChecked, m_szImgSrcChecked, m_szImgSrcInactive );
	return true;
}

// do left button down call back function
bool    CGUITabButton::OnLButtonDownSelf( int x, int y )
{
	//return CGUICheckBox::OnLButtonDownSelf( x, y );
	if ( m_bChecked )
	{
		GUISYSTEM->SetFocusElement(this);
	}
	
	if (m_pLButtonDownFun)
	{
		return m_pLButtonDownFun(this);
	}

	return true;
}

// save control itself to xml node
bool	CGUITabButton::SaveSelfToXML(CMarkupSTL& xml )
{
	CGUICheckBox::SaveSelfToXML(xml);

	xml.AddAttrib( "TabID", m_nTabID );

	if ( m_szPanelElemName.length() > 0 )
	{
		xml.AddAttrib( "PanelName", m_szPanelElemName.c_str() );
	}
	return true;
}

//加载完成后的操作
void    CGUITabButton::SetAfterLoad()
{
	if ( m_pParent && m_pParent->GetParent() )
	{
		m_pPanel = m_pParent->GetParent()->GetChild( m_szPanelElemName.c_str() );
	}
}






CGUITabCtrl::CGUITabCtrl()
{
	m_strType = "CGUITabCtrl";
	m_nTabCount = 0;			// tab按钮数量
	m_nCurCheckedID = 0;		// 当前按下的tab编号
	m_szImgSrcUnChecked.clear();	// 默认tab按钮非选中状态的图元字符串
	m_szImgSrcChecked.clear();		// 默认tab按钮选中状态的图元字符串
	m_szImgSrcInactive.clear();		// 默认tab按钮不可用状态的图元字符串
	m_nTabBtnWidth = 20;			// 默认tab按钮的宽度
	m_nTabBtnHeight = 10;		// 默认tab按钮的高度
	m_nGapX = 2;				// 默认2个tab之间的宽度
	m_bMoveable = false;
}

CGUITabCtrl::~CGUITabCtrl()
{

}

void CGUITabCtrl::DelTabBtn( int id )
{
	CGUIElement* pElem = NULL;
	pElem = RemoveChild( GetTabBtn( id ) );
	if ( pElem )
	{
		pElem->CleanUp();
		SafeDelete( pElem );
		//--m_nTabCount;
	}
}


void CGUITabCtrl::DelTabBtn( CGUITabButton* pBtn )
{
	if ( pBtn )
	{
		RemoveChild( pBtn );
		pBtn->CleanUp();
		SafeDelete( pBtn );
	}
}


void CGUITabCtrl::ResetAllTabBtn( void )
{
	CGUIElement* pElem = NULL;
	CGUITabButton* pTabBtn = NULL;
	for ( int i = 0; i < m_nTabCount; ++i )
	{
		pElem = GetTabBtn( i );
		if ( !pElem )
		{
			CreateNewTabBtn(i);
		}
		else
		{
			pTabBtn = (CGUITabButton*)pElem;
			//pTabBtn->SetImage( m_szImgSrcUnChecked, m_szImgSrcChecked, m_szImgSrcInactive );

			int x = 0, y = 0;
			if ( i != 0 )
			{
				CGUIElement* pLastElem = GetTabBtn( i - 1 );
				if ( pLastElem )
				{
					x = pLastElem->GetRenderRect().right - pLastElem->GetParent()->GetRenderRect().left + m_nGapX;
				}
			}
			pTabBtn->SetRect( x, y, m_nTabBtnWidth, m_nTabBtnHeight );
		}
	}
}

void CGUITabCtrl::SetPanel( int nID, CGUIElement* pElem )
{
	CGUITabButton* pTabBtn = GetTabBtn( nID );
	if ( pTabBtn )
	{
		pTabBtn->SetPanel( pElem );
	}
}

int CGUITabCtrl::GetTabCount( void )
{
	return m_nTabCount;
}

void CGUITabCtrl::CreateNewTabBtn( int id )
{
	CGUITabButton* pBtn = new CGUITabButton;
	pBtn->SetSchemeManager( GUISYSTEM->GetCurrentScheme() );
	pBtn->SetName( GetString("TabBtn%d", id) );
	pBtn->SetCheck( false );
	pBtn->SetImage( m_szImgSrcUnChecked, m_szImgSrcChecked, m_szImgSrcInactive );
	pBtn->SetTabID( id );
	this->AddChild( pBtn );
	int x = 0, y = 0;
	if ( id != 0 )
	{
		CGUIElement* pLastElem = GetTabBtn( id - 1 );
		if ( pLastElem )
		{
			x = pLastElem->GetRenderRect().right - pLastElem->GetParent()->GetRenderRect().left + m_nGapX;
		}
	}
	pBtn->SetRect( x, y, m_nTabBtnWidth, m_nTabBtnHeight );
}

void CGUITabCtrl::SetChecked( int nID )
{
	if ( nID >= m_nTabCount )
	{
		return;
	}
	for ( int i = 0; i < m_nTabCount; ++i )
	{
		CGUITabButton* pTabBtn = GetTabBtn( i );
		if ( pTabBtn )
		{
			if ( i == nID )
			{
				pTabBtn->SetCheck( true );
			}
			else
			{
				pTabBtn->SetCheck( false );
			}
		}
	}
	m_nCurCheckedID = nID;
}

bool	CGUITabCtrl::Create(CGUIElement* pParent, CMarkupSTL& xml)
{
	CGUIElement::Create(pParent, xml);
	return true;
}

CGUITabButton* CGUITabCtrl::GetTabBtn( int nID )
{
	CGUITabButton* pBtn = NULL;
	CGUIElement::GUIELEMENT_ITER it = m_lstChildren.begin();
	for ( ; it != m_lstChildren.end(); ++it )
	{
		if ( (*it)->GetType() == "CGUITabButton" )
		{
			pBtn = static_cast<CGUITabButton*>(*it);
			if ( pBtn->GetTabID() == nID )
			{
				return pBtn;
			}
		}
	}
	return NULL;
}


bool    CGUITabCtrl::CreateSelf( CGUIElement* pParent, CMarkupSTL& xml )
{
	CGUIElement::CreateSelf( pParent, xml );

	// 读入按钮弹起时的纹理
	AIRString szValue = xml.GetAttrib( "ImgSrcUnChecked" ).c_str();
	m_szImgSrcUnChecked = szValue;

	//读入按钮按下时的纹理
	szValue = xml.GetAttrib("ImgSrcChecked").c_str();
	m_szImgSrcChecked = szValue;

	// 读入按钮不可用时的纹理
	szValue = xml.GetAttrib("ImgSrcInactive").c_str();
	m_szImgSrcInactive = szValue;

	// 默认的tab按钮宽度
	szValue = xml.GetAttrib( "TabBtnWidth" ).c_str();
	m_nTabBtnWidth = atoi( szValue.c_str() );

	// 默认的tab按钮高度
	szValue = xml.GetAttrib( "TabBtnHeight" ).c_str();
	m_nTabBtnHeight = atoi( szValue.c_str() );

	// 默认的2个tab按钮的X轴之间的距离
	szValue = xml.GetAttrib( "GapX" ).c_str();
	m_nGapX = atoi( szValue.c_str() );

	// 当前打开的tab索引
	szValue = xml.GetAttrib("CurCheckedID").c_str();
	m_nCurCheckedID = atoi( szValue.c_str() );

	// tab按钮数量
	szValue = xml.GetAttrib( "TabCount" ).c_str();
	m_nTabCount = atoi( szValue.c_str() );

	return true;
}

bool    CGUITabCtrl::OnLButtonDown( int x, int y )
{
	CGUIElement* pElement = NULL;
	for (GUIELEMENT_ITER it = m_lstChildren.begin(); it != m_lstChildren.end(); it++)
	{
		pElement = *it;
		if (pElement->IsInRect(x, y) && pElement->IsVisable())
		{
			CGUITabButton* pTabBtn = static_cast<CGUITabButton*>(pElement);
			SetChecked( pTabBtn->GetTabID() );
			return pElement->OnLButtonDown(x, y);
		}
	}
	m_bFocus = true;
	return OnLButtonDownSelf(x, y);
}

bool	CGUITabCtrl::SaveSelfToXML(CMarkupSTL& xml )
{
	CGUIElement::SaveSelfToXML( xml );
	if ( m_szImgSrcUnChecked.length() > 0 )
	{
		xml.AddAttrib( "ImgSrcUnChecked", m_szImgSrcUnChecked.c_str() );
	}
	if ( m_szImgSrcChecked.length() > 0 )
	{
		xml.AddAttrib( "ImgSrcChecked", m_szImgSrcChecked.c_str() );
	}
	if ( m_szImgSrcInactive.length() > 0 )
	{
		xml.AddAttrib( "ImgSrcInactive", m_szImgSrcInactive.c_str() );
	}
	xml.AddAttrib( "TabBtnWidth", m_nTabBtnWidth );
	xml.AddAttrib( "TabBtnHeight", m_nTabBtnHeight );
	xml.AddAttrib( "GapX", m_nGapX );
	xml.AddAttrib( "CurCheckedID", m_nCurCheckedID );
	xml.AddAttrib( "TabCount", m_nTabCount );
	return true;		
}
