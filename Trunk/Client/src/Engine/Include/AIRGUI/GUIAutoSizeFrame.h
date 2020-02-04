// ***************************************************************
//  文件: GUIAutoSizeFrame.h
//  日期: 2010-4-2010/04/02
//  作者: huangjunjie
//  说明: 自动调整大小的框
// ***************************************************************


#pragma once

#ifndef __GUIAutoSizeFrame_H_
#define __GUIAutoSizeFrame_H_


#include "GUIElement.h"
#include "GUIImage.h"
#include "GUIStaticText.h"
#include "IFontBase.h"
#include "FontManager.h"

class AIRGuiExport CGUIAutoSizeFrame : public CGUIElement
{
public:
	CGUIAutoSizeFrame() : 
	  m_nMaxWidth(800),
	  m_nMaxHeight(600),
	  m_bCtrlCreated(false),
	  m_pTopLeftImg(NULL),
	  m_pTopImg(NULL),
	  m_pTopRightImg(NULL),
	  m_pRightImg(NULL),
	  m_pBottomRightImg(NULL),
	  m_pBottomImg(NULL),
	  m_pBottomLeftImg(NULL),
	  m_pLeftImg(NULL),
	  m_pCenterImg(NULL)
	  //m_pTextContent(NULL),
	  //m_pFont(NULL)
	{
		m_strType = "CGUIAutoSizeFrame";
	}

    virtual ~CGUIAutoSizeFrame()
	{

	}

	void SetImgSrcTopLeft( const AIRString& szImgSrc );

	void SetImgSrcTop( const AIRString& szImgSrc );

	void SetImgSrcTopRight( const AIRString& szImgSrc );

	void SetImgSrcRight( const AIRString& szImgSrc );

	void SetImgSrcBottomRight( const AIRString& szImgSrc );

	void SetImgSrcBottom( const AIRString& szImgSrc );

	void SetImgSrcBottomLeft( const AIRString& szImgSrc );

	void SetImgSrcLeft( const AIRString& szImgSrc );

	void SetImgSrcCenter( const AIRString& szImgSrc );

	//void SetTextContent( );

	CGUIImage* GetTopLeftImgElem( void )
	{
		return m_pTopLeftImg;
	}

	CGUIImage* GetTopImgElem( void )
	{
		return m_pTopImg;
	}

	CGUIImage* GetTopRightImgElem( void )
	{
		return m_pTopRightImg;
	}

	CGUIImage* GetRightImgElem( void )
	{
		return m_pRightImg;
	}

	CGUIImage* GetBottomRightImgElem( void )
	{
		return m_pBottomRightImg;
	}

	CGUIImage* GetBottomImgElem( void )
	{
		return m_pBottomImg;
	}

	CGUIImage* GetBottomLeftImgElem( void )
	{
		return m_pBottomLeftImg;
	}

	CGUIImage* GetLeftImgElem( void )
	{
		return m_pLeftImg;
	}

	CGUIImage* GetCenterImgElem( void )
	{
		return m_pCenterImg;
	}

	int GetMaxWidth( void )
	{
		return m_nMaxWidth;
	}

	int GetMaxHeight( void )
	{
		return m_nMaxHeight;
	}

	void SetMaxWidth( int nMaxWidth )
	{
		m_nMaxWidth = nMaxWidth;
	}

	void SetMaxHeight( int nMaxHeight )
	{
		m_nMaxHeight = nMaxHeight;
	}

	virtual void Resize( void );

    const AIRString& GetImgSrcTopLeft() const
    {
        return m_strImgSrcTopLeft;
    }

    const AIRString& GetImgSrcTop() const
    {
        return m_strImgSrcTop;
    }

    const AIRString& GetImgSrcTopRight() const
    {
        return m_strImgSrcTopRight;
    }

    const AIRString& GetImgSrcRight() const
    {
        return m_strImgSrcRight;
    }

    const AIRString& GetImgSrcBottomRight() const
    {
        return m_strImgSrcBottomRight;
    }

    const AIRString& GetImgSrcBottom() const
    {
        return m_strImgSrcBottom;
    }

    const AIRString& GetImgSrcBottomLeft() const
    {
        return m_strImgSrcBottomLeft;
    }

    const AIRString& GetImgSrcLeft() const
    {
        return m_strImgSrcLeft;
    }

    const AIRString& GetImgSrcCenter() const
    {
        return m_strImgSrcCenter;
    }
protected:
    virtual bool CreateSelf(CGUIElement* pParent, CMarkupSTL& xml);

    virtual bool SaveSelfToXML(CMarkupSTL& xml);
protected:
	int					m_nMaxWidth;
	int					m_nMaxHeight;
	bool				m_bCtrlCreated;   // is the children has been created

	CGUIImage*			m_pTopLeftImg;
	CGUIImage*			m_pTopImg;
	CGUIImage*			m_pTopRightImg;
	CGUIImage*			m_pRightImg;
	CGUIImage*			m_pBottomRightImg;
	CGUIImage*			m_pBottomImg;
	CGUIImage*			m_pBottomLeftImg;
	CGUIImage*			m_pLeftImg;
	CGUIImage*			m_pCenterImg;

    AIRString m_strImgSrcTopLeft;
    AIRString m_strImgSrcTop;
    AIRString m_strImgSrcTopRight;
    AIRString m_strImgSrcRight;
    AIRString m_strImgSrcBottomRight;
    AIRString m_strImgSrcBottom;
    AIRString m_strImgSrcBottomLeft;
    AIRString m_strImgSrcLeft;
    AIRString m_strImgSrcCenter;

	//CGUIStaticText*		m_pTextContent;
	//IFontBase*			m_pFont;

}; //! end class CGUIAutoSizeFrame


#endif //! end __GUIAutoSizeFrame_H_