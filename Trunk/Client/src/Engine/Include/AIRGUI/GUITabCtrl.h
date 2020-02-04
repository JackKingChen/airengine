// ***************************************************************
//  文件: GUITabCtrl.h
//  日期: 2010-3-2010/03/22
//  作者: huangjunjie
//  说明: tab control 标签控件
// ***************************************************************


#pragma once

#ifndef __GUITabCtrl_H_
#define __GUITabCtrl_H_

#include <string>
#include "GUICheckBox.h"
#include "Rect.h"

// 标签按钮控件
class AIRGuiExport CGUITabButton : public CGUICheckBox
{
public:
	CGUITabButton();

	virtual ~CGUITabButton();

	//************************************
	// Method:    SetTabID
	// Returns:   void
	// Qualifier: 设置标签按钮的ID
	// Parameter: int id
	//************************************
	virtual void	SetTabID( int id );

	//************************************
	// Method:    GetTabID
	// Returns:   int
	// Qualifier: 获得标签按钮的ID
	// Parameter: void
	//************************************
	virtual int		GetTabID( void );

	//************************************
	// Method:    SetCheck
	// Returns:   void
	// Qualifier: 设置标签按钮当前是否被选中
	// Parameter: bool bCheck
	//************************************
	virtual void    SetCheck( bool bCheck );

	//************************************
	// Method:    SetImage
	// Returns:   bool
	// Qualifier: 设置标签按钮的3个状态的图像
	// Parameter: const AIRString & szUnChecked
	// Parameter: const AIRString & szChecked
	// Parameter: const AIRString & szInactive
	//************************************
	virtual bool	SetImage( const AIRString& szUnChecked, const AIRString& szChecked, const AIRString& szInactive );

	//************************************
	// Method:    SetPanel
	// Returns:   void
	// Qualifier: 设置标签按钮控制的面板控件对象
	// Parameter: CGUIElement * pPanel
	//************************************
	virtual void	SetPanel( CGUIElement* pPanel );

	//************************************
	// Method:    GetPanelElemName
	// Returns:   const AIRString&
	// Qualifier: 获得标签按钮控制的面板控件的名称
	// Parameter: void
	//************************************
	const AIRString& GetPanelElemName( void )
	{
		return m_szPanelElemName;
	}

	//************************************
	// Method:    SetPanelElemName
	// Returns:   void
	// Qualifier: 设置标签按钮控制的面板控件对象
	// Parameter: const AIRString & szName
	//************************************
	void SetPanelElemName( const AIRString& szName )
	{
		m_szPanelElemName = szName;
		if ( m_pParent && m_pParent->GetParent() )
		{
			m_pPanel = m_pParent->GetParent()->GetChild( szName.c_str() );
		}
	}

protected:
	//************************************
	// Method:    RenderSelf
	// Returns:   void
	// Qualifier: 渲染标签按钮自身
	// Parameter: void
	//************************************
	virtual void    RenderSelf( void );

	//************************************
	// Method:    CreateSelf
	// Returns:   bool 
	// Qualifier: 加载并创建标签按钮自身
	// Parameter: CGUIElement * pParent
	// Parameter: CMarkupSTL & xml
	//************************************
	virtual bool    CreateSelf( CGUIElement* pParent, CMarkupSTL& xml );

	//************************************
	// Method:    OnLButtonDownSelf
	// Returns:   bool
	// Qualifier: 左键按下标签按钮自身的处理
	// Parameter: int x
	// Parameter: int y
	//************************************
	virtual bool    OnLButtonDownSelf( int x, int y );


	//************************************
	// Method:    SaveSelfToXML
	// Returns:   bool
	// Qualifier: 保存自身信息到xml节点
	// Parameter: CMarkupSTL & xml
	//************************************
	virtual bool	SaveSelfToXML(CMarkupSTL& xml );

	//************************************
	// Method:    SetAfterLoad
	// Returns:   void
	// Qualifier: 加载完成后的操作
	//************************************
	virtual void    SetAfterLoad();

protected:
	int				m_nTabID;			// 当前的tab按钮的id
	CGUIElement*	m_pPanel;			// 当前tab按钮所对应的面板的指针
	AIRString		m_szPanelElemName;	// 当前tab按钮所对应的面板的名称

private:
};






// 标签控件
class AIRGuiExport CGUITabCtrl : public CGUIElement
{
public:
    CGUITabCtrl();

    virtual ~CGUITabCtrl();

	//************************************
	// Method:    DelTabBtn
	// Returns:   void
	// Qualifier: 根据标签按钮的ID来删除标签按钮
	// Parameter: int id
	//************************************
	virtual void DelTabBtn( int id );

	//************************************
	// Method:    DelTabBtn
	// Returns:   void
	// Qualifier: 根据标签按钮的指针来删除标签按钮
	// Parameter: CGUITabButton * pBtn
	//************************************
	virtual void DelTabBtn( CGUITabButton* pBtn );

	//************************************
	// Method:    SetPanel
	// Returns:   void
	// Qualifier: 设置第nID个标签按钮所控制的面板控件pElem
	// Parameter: int nID
	// Parameter: CGUIElement * pElem
	//************************************
	virtual void SetPanel( int nID, CGUIElement* pElem );

	//************************************
	// Method:    GetTabCount
	// Returns:   int
	// Qualifier: 获得标签按钮的数量
	// Parameter: void
	//************************************
	virtual int GetTabCount( void );

	//************************************
	// Method:    SetTabCount
	// Returns:   void
	// Qualifier: 设置标签按钮的数量
	// Parameter: int nCount
	//************************************
	virtual void SetTabCount( int nCount )
	{
		m_nTabCount = nCount;
	}

	//************************************
	// Method:    CreateNewTabBtn
	// Returns:   void
	// Qualifier: 创建一个新的标签按钮
	// Parameter: int id
	//************************************
	virtual void CreateNewTabBtn( int id );

	//************************************
	// Method:    SetChecked
	// Returns:   void
	// Qualifier: 设置第nID个标签按钮是选择的
	// Parameter: int nID
	//************************************
	virtual void SetChecked( int nID );

	//************************************
	// Method:    GetChecked
	// Returns:   int
	// Qualifier: 获得当前哪个标签按钮被按下
	// Parameter: void
	//************************************
	virtual int GetChecked( void )
	{
		return m_nCurCheckedID;
	}

	//************************************
	// Method:    Create
	// Returns:   bool
	// Qualifier: 加载并创建控件
	// Parameter: CGUIElement * pParent
	// Parameter: CMarkupSTL & xml
	//************************************
	virtual bool	Create(CGUIElement* pParent, CMarkupSTL& xml);

	//************************************
	// Method:    GetTabBtn
	// Returns:   CGUITabButton*
	// Qualifier: 根据ID获得标签按钮
	// Parameter: int nID
	//************************************
	virtual CGUITabButton* GetTabBtn( int nID );

	//************************************
	// Method:    SetImgSrcUnChecked
	// Returns:   void
	// Qualifier: 设置默认的标签按钮非选中状态的图元
	// Parameter: const AIRString & szImgSrcUnChecked
	//************************************
	void SetImgSrcUnChecked( const AIRString& szImgSrcUnChecked )
	{
		m_szImgSrcUnChecked = szImgSrcUnChecked;
	}

	//************************************
	// Method:    SetImgSrcChecked
	// Returns:   void
	// Qualifier: 设置默认的标签按钮选中状态的图元
	// Parameter: const AIRString & szImgSrcChecked
	//************************************
	void SetImgSrcChecked( const AIRString& szImgSrcChecked )
	{
		m_szImgSrcChecked = szImgSrcChecked;
	}

	//************************************
	// Method:    SetImgSrcInactive
	// Returns:   void
	// Qualifier: 设置默认的标签按钮不可用状态的图元
	// Parameter: const AIRString & szImgSrcInactive
	//************************************
	void SetImgSrcInactive( const AIRString& szImgSrcInactive )
	{
		m_szImgSrcInactive = szImgSrcInactive;
	}

	//************************************
	// Method:    GetImgSrcUnChecked
	// Returns:   const AIRString&
	// Qualifier: 获得默认的标签按钮非选中状态的图元
	// Parameter: void
	//************************************
	const AIRString& GetImgSrcUnChecked( void )
	{
		return m_szImgSrcUnChecked;
	}

	//************************************
	// Method:    GetImgSrcChecked
	// Returns:   const AIRString&
	// Qualifier: 获得默认的标签按钮选中状态的图元
	// Parameter: void
	//************************************
	const AIRString& GetImgSrcChecked( void )
	{
		return m_szImgSrcChecked;
	} 

	//************************************
	// Method:    GetImgSrcInactive
	// Returns:   const AIRString&
	// Qualifier: 获得默认的标签按钮不可用状态的图元
	// Parameter: void
	//************************************
	const AIRString& GetImgSrcInactive( void )
	{
		return m_szImgSrcInactive;
	}

	//************************************
	// Method:    SetTabBtnWidth
	// Returns:   void
	// Qualifier: 设置默认的标签按钮的宽度
	// Parameter: int width
	//************************************
	void SetTabBtnWidth( int width )
	{
		m_nTabBtnWidth = width;
	}

	//************************************
	// Method:    GetTabBtnWidth
	// Returns:   int
	// Qualifier: 获得默认的标签按钮的宽度
	// Parameter: void
	//************************************
	int GetTabBtnWidth( void )
	{
		return m_nTabBtnWidth;
	}

	//************************************
	// Method:    SetTabBtnHeight
	// Returns:   void
	// Qualifier: 设置默认的标签按钮的高度
	// Parameter: int height
	//************************************
	void SetTabBtnHeight( int height )
	{
		m_nTabBtnHeight = height;
	}

	//************************************
	// Method:    GetTabBtnHeight
	// Returns:   int
	// Qualifier: 获得默认的标签按钮的高度
	// Parameter: void
	//************************************
	int GetTabBtnHeight( void )
	{
		return m_nTabBtnHeight;
	}

	//************************************
	// Method:    SetTabBtnGapX
	// Returns:   void
	// Qualifier: 设置2个标签按钮之间的间距
	// Parameter: int gapx
	//************************************
	void SetTabBtnGapX( int gapx )
	{
		m_nGapX = gapx;
	}

	//************************************
	// Method:    GetTabBtnGapX
	// Returns:   int
	// Qualifier: 获得2个标签按钮之间的间距
	// Parameter: void
	//************************************
	int GetTabBtnGapX( void )
	{
		return m_nGapX;
	}

	//************************************
	// Method:    ResetAllTabBtn
	// Returns:   void
	// Qualifier: 重置所有的标签按钮
	// Parameter: void
	//************************************
	void ResetAllTabBtn( void );


protected:
	// 创建控件自身
	virtual bool    CreateSelf( CGUIElement* pParent, CMarkupSTL& xml );

	// 左键按下
	virtual bool    OnLButtonDown( int x, int y );

	// 保存到XML节点
	virtual bool	SaveSelfToXML(CMarkupSTL& xml );

protected:
	int				m_nTabCount;			// tab按钮数量
	int				m_nCurCheckedID;		// 当前按下的tab编号
	AIRString		m_szImgSrcUnChecked;	// 默认tab按钮非选中状态的图元字符串
	AIRString		m_szImgSrcChecked;		// 默认tab按钮选中状态的图元字符串
	AIRString		m_szImgSrcInactive;		// 默认tab按钮不可用状态的图元字符串
	int				m_nTabBtnWidth;			// 默认tab按钮的宽度
	int				m_nTabBtnHeight;		// 默认tab按钮的高度
	int				m_nGapX;				// 默认2个tab之间的宽度

	//CGUIElement::GUIELEMENT_LIST m_lstTabBtn;



}; //! end class CGUITabCtrl


#endif //! end __GUITabCtrl_H_