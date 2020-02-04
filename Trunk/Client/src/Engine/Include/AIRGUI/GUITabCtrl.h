// ***************************************************************
//  �ļ�: GUITabCtrl.h
//  ����: 2010-3-2010/03/22
//  ����: huangjunjie
//  ˵��: tab control ��ǩ�ؼ�
// ***************************************************************


#pragma once

#ifndef __GUITabCtrl_H_
#define __GUITabCtrl_H_

#include <string>
#include "GUICheckBox.h"
#include "Rect.h"

// ��ǩ��ť�ؼ�
class AIRGuiExport CGUITabButton : public CGUICheckBox
{
public:
	CGUITabButton();

	virtual ~CGUITabButton();

	//************************************
	// Method:    SetTabID
	// Returns:   void
	// Qualifier: ���ñ�ǩ��ť��ID
	// Parameter: int id
	//************************************
	virtual void	SetTabID( int id );

	//************************************
	// Method:    GetTabID
	// Returns:   int
	// Qualifier: ��ñ�ǩ��ť��ID
	// Parameter: void
	//************************************
	virtual int		GetTabID( void );

	//************************************
	// Method:    SetCheck
	// Returns:   void
	// Qualifier: ���ñ�ǩ��ť��ǰ�Ƿ�ѡ��
	// Parameter: bool bCheck
	//************************************
	virtual void    SetCheck( bool bCheck );

	//************************************
	// Method:    SetImage
	// Returns:   bool
	// Qualifier: ���ñ�ǩ��ť��3��״̬��ͼ��
	// Parameter: const AIRString & szUnChecked
	// Parameter: const AIRString & szChecked
	// Parameter: const AIRString & szInactive
	//************************************
	virtual bool	SetImage( const AIRString& szUnChecked, const AIRString& szChecked, const AIRString& szInactive );

	//************************************
	// Method:    SetPanel
	// Returns:   void
	// Qualifier: ���ñ�ǩ��ť���Ƶ����ؼ�����
	// Parameter: CGUIElement * pPanel
	//************************************
	virtual void	SetPanel( CGUIElement* pPanel );

	//************************************
	// Method:    GetPanelElemName
	// Returns:   const AIRString&
	// Qualifier: ��ñ�ǩ��ť���Ƶ����ؼ�������
	// Parameter: void
	//************************************
	const AIRString& GetPanelElemName( void )
	{
		return m_szPanelElemName;
	}

	//************************************
	// Method:    SetPanelElemName
	// Returns:   void
	// Qualifier: ���ñ�ǩ��ť���Ƶ����ؼ�����
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
	// Qualifier: ��Ⱦ��ǩ��ť����
	// Parameter: void
	//************************************
	virtual void    RenderSelf( void );

	//************************************
	// Method:    CreateSelf
	// Returns:   bool 
	// Qualifier: ���ز�������ǩ��ť����
	// Parameter: CGUIElement * pParent
	// Parameter: CMarkupSTL & xml
	//************************************
	virtual bool    CreateSelf( CGUIElement* pParent, CMarkupSTL& xml );

	//************************************
	// Method:    OnLButtonDownSelf
	// Returns:   bool
	// Qualifier: ������±�ǩ��ť����Ĵ���
	// Parameter: int x
	// Parameter: int y
	//************************************
	virtual bool    OnLButtonDownSelf( int x, int y );


	//************************************
	// Method:    SaveSelfToXML
	// Returns:   bool
	// Qualifier: ����������Ϣ��xml�ڵ�
	// Parameter: CMarkupSTL & xml
	//************************************
	virtual bool	SaveSelfToXML(CMarkupSTL& xml );

	//************************************
	// Method:    SetAfterLoad
	// Returns:   void
	// Qualifier: ������ɺ�Ĳ���
	//************************************
	virtual void    SetAfterLoad();

protected:
	int				m_nTabID;			// ��ǰ��tab��ť��id
	CGUIElement*	m_pPanel;			// ��ǰtab��ť����Ӧ������ָ��
	AIRString		m_szPanelElemName;	// ��ǰtab��ť����Ӧ����������

private:
};






// ��ǩ�ؼ�
class AIRGuiExport CGUITabCtrl : public CGUIElement
{
public:
    CGUITabCtrl();

    virtual ~CGUITabCtrl();

	//************************************
	// Method:    DelTabBtn
	// Returns:   void
	// Qualifier: ���ݱ�ǩ��ť��ID��ɾ����ǩ��ť
	// Parameter: int id
	//************************************
	virtual void DelTabBtn( int id );

	//************************************
	// Method:    DelTabBtn
	// Returns:   void
	// Qualifier: ���ݱ�ǩ��ť��ָ����ɾ����ǩ��ť
	// Parameter: CGUITabButton * pBtn
	//************************************
	virtual void DelTabBtn( CGUITabButton* pBtn );

	//************************************
	// Method:    SetPanel
	// Returns:   void
	// Qualifier: ���õ�nID����ǩ��ť�����Ƶ����ؼ�pElem
	// Parameter: int nID
	// Parameter: CGUIElement * pElem
	//************************************
	virtual void SetPanel( int nID, CGUIElement* pElem );

	//************************************
	// Method:    GetTabCount
	// Returns:   int
	// Qualifier: ��ñ�ǩ��ť������
	// Parameter: void
	//************************************
	virtual int GetTabCount( void );

	//************************************
	// Method:    SetTabCount
	// Returns:   void
	// Qualifier: ���ñ�ǩ��ť������
	// Parameter: int nCount
	//************************************
	virtual void SetTabCount( int nCount )
	{
		m_nTabCount = nCount;
	}

	//************************************
	// Method:    CreateNewTabBtn
	// Returns:   void
	// Qualifier: ����һ���µı�ǩ��ť
	// Parameter: int id
	//************************************
	virtual void CreateNewTabBtn( int id );

	//************************************
	// Method:    SetChecked
	// Returns:   void
	// Qualifier: ���õ�nID����ǩ��ť��ѡ���
	// Parameter: int nID
	//************************************
	virtual void SetChecked( int nID );

	//************************************
	// Method:    GetChecked
	// Returns:   int
	// Qualifier: ��õ�ǰ�ĸ���ǩ��ť������
	// Parameter: void
	//************************************
	virtual int GetChecked( void )
	{
		return m_nCurCheckedID;
	}

	//************************************
	// Method:    Create
	// Returns:   bool
	// Qualifier: ���ز������ؼ�
	// Parameter: CGUIElement * pParent
	// Parameter: CMarkupSTL & xml
	//************************************
	virtual bool	Create(CGUIElement* pParent, CMarkupSTL& xml);

	//************************************
	// Method:    GetTabBtn
	// Returns:   CGUITabButton*
	// Qualifier: ����ID��ñ�ǩ��ť
	// Parameter: int nID
	//************************************
	virtual CGUITabButton* GetTabBtn( int nID );

	//************************************
	// Method:    SetImgSrcUnChecked
	// Returns:   void
	// Qualifier: ����Ĭ�ϵı�ǩ��ť��ѡ��״̬��ͼԪ
	// Parameter: const AIRString & szImgSrcUnChecked
	//************************************
	void SetImgSrcUnChecked( const AIRString& szImgSrcUnChecked )
	{
		m_szImgSrcUnChecked = szImgSrcUnChecked;
	}

	//************************************
	// Method:    SetImgSrcChecked
	// Returns:   void
	// Qualifier: ����Ĭ�ϵı�ǩ��ťѡ��״̬��ͼԪ
	// Parameter: const AIRString & szImgSrcChecked
	//************************************
	void SetImgSrcChecked( const AIRString& szImgSrcChecked )
	{
		m_szImgSrcChecked = szImgSrcChecked;
	}

	//************************************
	// Method:    SetImgSrcInactive
	// Returns:   void
	// Qualifier: ����Ĭ�ϵı�ǩ��ť������״̬��ͼԪ
	// Parameter: const AIRString & szImgSrcInactive
	//************************************
	void SetImgSrcInactive( const AIRString& szImgSrcInactive )
	{
		m_szImgSrcInactive = szImgSrcInactive;
	}

	//************************************
	// Method:    GetImgSrcUnChecked
	// Returns:   const AIRString&
	// Qualifier: ���Ĭ�ϵı�ǩ��ť��ѡ��״̬��ͼԪ
	// Parameter: void
	//************************************
	const AIRString& GetImgSrcUnChecked( void )
	{
		return m_szImgSrcUnChecked;
	}

	//************************************
	// Method:    GetImgSrcChecked
	// Returns:   const AIRString&
	// Qualifier: ���Ĭ�ϵı�ǩ��ťѡ��״̬��ͼԪ
	// Parameter: void
	//************************************
	const AIRString& GetImgSrcChecked( void )
	{
		return m_szImgSrcChecked;
	} 

	//************************************
	// Method:    GetImgSrcInactive
	// Returns:   const AIRString&
	// Qualifier: ���Ĭ�ϵı�ǩ��ť������״̬��ͼԪ
	// Parameter: void
	//************************************
	const AIRString& GetImgSrcInactive( void )
	{
		return m_szImgSrcInactive;
	}

	//************************************
	// Method:    SetTabBtnWidth
	// Returns:   void
	// Qualifier: ����Ĭ�ϵı�ǩ��ť�Ŀ��
	// Parameter: int width
	//************************************
	void SetTabBtnWidth( int width )
	{
		m_nTabBtnWidth = width;
	}

	//************************************
	// Method:    GetTabBtnWidth
	// Returns:   int
	// Qualifier: ���Ĭ�ϵı�ǩ��ť�Ŀ��
	// Parameter: void
	//************************************
	int GetTabBtnWidth( void )
	{
		return m_nTabBtnWidth;
	}

	//************************************
	// Method:    SetTabBtnHeight
	// Returns:   void
	// Qualifier: ����Ĭ�ϵı�ǩ��ť�ĸ߶�
	// Parameter: int height
	//************************************
	void SetTabBtnHeight( int height )
	{
		m_nTabBtnHeight = height;
	}

	//************************************
	// Method:    GetTabBtnHeight
	// Returns:   int
	// Qualifier: ���Ĭ�ϵı�ǩ��ť�ĸ߶�
	// Parameter: void
	//************************************
	int GetTabBtnHeight( void )
	{
		return m_nTabBtnHeight;
	}

	//************************************
	// Method:    SetTabBtnGapX
	// Returns:   void
	// Qualifier: ����2����ǩ��ť֮��ļ��
	// Parameter: int gapx
	//************************************
	void SetTabBtnGapX( int gapx )
	{
		m_nGapX = gapx;
	}

	//************************************
	// Method:    GetTabBtnGapX
	// Returns:   int
	// Qualifier: ���2����ǩ��ť֮��ļ��
	// Parameter: void
	//************************************
	int GetTabBtnGapX( void )
	{
		return m_nGapX;
	}

	//************************************
	// Method:    ResetAllTabBtn
	// Returns:   void
	// Qualifier: �������еı�ǩ��ť
	// Parameter: void
	//************************************
	void ResetAllTabBtn( void );


protected:
	// �����ؼ�����
	virtual bool    CreateSelf( CGUIElement* pParent, CMarkupSTL& xml );

	// �������
	virtual bool    OnLButtonDown( int x, int y );

	// ���浽XML�ڵ�
	virtual bool	SaveSelfToXML(CMarkupSTL& xml );

protected:
	int				m_nTabCount;			// tab��ť����
	int				m_nCurCheckedID;		// ��ǰ���µ�tab���
	AIRString		m_szImgSrcUnChecked;	// Ĭ��tab��ť��ѡ��״̬��ͼԪ�ַ���
	AIRString		m_szImgSrcChecked;		// Ĭ��tab��ťѡ��״̬��ͼԪ�ַ���
	AIRString		m_szImgSrcInactive;		// Ĭ��tab��ť������״̬��ͼԪ�ַ���
	int				m_nTabBtnWidth;			// Ĭ��tab��ť�Ŀ��
	int				m_nTabBtnHeight;		// Ĭ��tab��ť�ĸ߶�
	int				m_nGapX;				// Ĭ��2��tab֮��Ŀ��

	//CGUIElement::GUIELEMENT_LIST m_lstTabBtn;



}; //! end class CGUITabCtrl


#endif //! end __GUITabCtrl_H_