#pragma once

#include "GUIElement.h"



class AIRGuiExport CGUICheckBox : public CGUIElement
{
public:
    CGUICheckBox();
    virtual ~CGUICheckBox();

    // set whether the control is on check state
    virtual void    SetCheck( bool bCheck );

    // get the check state
    virtual bool    IsChecked( void );

    // get the unchecked state image source string
    virtual const AIRString& GetUnCheckedImageSrc() const
    {
        return m_szImgSrcUnChecked;
    }

    // get the checked state image source string
    virtual const AIRString& GetCheckedImageSrc() const
    {
        return m_szImgSrcChecked;
    }

    // get the inactived state image source string
    virtual const AIRString& GetInactiveImageSrc() const
    {
        return m_szImgSrcInactive;
    }

	virtual void SetUnCheckedImageSrc( const AIRString& strUnCheckedImgSrc );

    virtual void SetCheckedImageSrc(const AIRString& strImageSrc);

    virtual void SetInactiveImageSrc(const AIRString& strInactiveImageSrc);

    virtual void AutoSize();

protected:
    // render the control itself
    virtual void    RenderSelf( void );

    // update the control itself
    //virtual void    UpdateSelf( void );

    // create the control from xml
    virtual bool    CreateSelf( CGUIElement* pParent, CMarkupSTL& xml );

    // do left button down call back function
    virtual bool    OnLButtonDownSelf( int x, int y );

	virtual bool	SaveSelfToXML(CMarkupSTL& xml );

    virtual void            NotifyTextureDestroySelf(ITexture* pTex)
    {
        if ( m_renderQuad.m_pTexture == pTex )
        {
            m_renderQuad.m_pTexture = NULL;
            m_szImgSrc.clear();
            m_szImgSrcUnChecked.clear();
            m_szImgSrcChecked.clear();
            m_szImgSrcInactive.clear();
        }
    }

protected:
    bool            m_bChecked;     // is on check?
    Rectf           m_UVUnChecked;  // 按钮弹起时的纹理UV
    Rectf           m_UVChecked;    // 按钮按下时的纹理UV
    Rectf           m_UVInactive;   // 按钮不可用时的纹理UV

	AIRString		m_szImgSrcUnChecked;
	AIRString		m_szImgSrcChecked;
	AIRString		m_szImgSrcInactive;
};
