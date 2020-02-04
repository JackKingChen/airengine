// ***************************************************************
//  File: GUIButton.h
//  Date: 2010-2-12
//  Author: liangairan
//  Description: 按钮控件
// ***************************************************************

#pragma once

#include "GUIElement.h"

class AIRGuiExport CGUIButton : public CGUIElement
{
public:
    CGUIButton();
    ~CGUIButton();

    // 克隆操作
    virtual CGUIElement*    Clone();

    // 清空
    virtual void            CleanUp();

    virtual SColor          GetColorUp()
    {
        return m_ColorUp;
    }

    virtual SColor          GetColorDown()
    {
        return m_ColorDown;
    }

    virtual SColor          GetColorHover()
    {
        return m_ColorHover;
    }

    virtual SColor          GetColorGray()
    {
        return m_ColorInactive;
    }

    const AIRString& GetImgSrcUpStr() const
    {
        return m_szImgSrcUp;
    }

    const AIRString& GetImgSrcDownStr() const
    {
        return m_szImgSrcDown;
    }

    const AIRString& GetImgSrcHoverStr() const
    {
        return m_szImgSrcHover;
    }

    const AIRString& GetImgSrcGrayStr() const
    {
        return m_szImgSrcInactive;
    }

	virtual void SetImgSrcUp( const AIRString& strImgSrcUp );

    virtual void SetImgSrcDown(const AIRString& strImgSrcDown);

    virtual void SetImgSrcHover(const AIRString& strImgSrcHover);

    virtual void SetImgSrcGray(const AIRString& strImgSrcGray);

    virtual void SetColorDown(const SColor color);

    virtual void SetColorHover(const SColor color);

    virtual void SetColorGray(const SColor color);

    virtual void AutoSize();

protected:

    // 注册到脚本
    virtual void            RegisterToScript();

    // 创建自身
    virtual bool            CreateSelf(CGUIElement* pParent, CMarkupSTL& xml);

    // 渲染自身
    virtual void            RenderSelf();
    
    // 状态改变
    virtual void            OnStatusChange();

	virtual bool			SaveSelfToXML(CMarkupSTL& xml );

    virtual void            NotifyTextureDestroySelf(ITexture* pTex)
    {
        if ( m_renderQuad.m_pTexture == pTex )
        {
            m_renderQuad.m_pTexture = NULL;
            m_szImgSrc.clear();
            m_szImgSrcDown.clear();
            m_szImgSrcHover.clear();
            m_szImgSrcInactive.clear();
            m_szImgSrcUp.clear();
        }
    }


protected:
    string                  m_strScript;            // 脚本
    Rectf                   m_UVUp;                 // 按钮弹起时的纹理UV
    Rectf                   m_UVDown;               // 按钮按下时的纹理UV
    Rectf                   m_UVHover;              // 鼠标盘旋在按钮上时的纹理UV
    Rectf                   m_UVInactive;           // 按钮不可用时的纹理UV
    SColor                  m_ColorUp;              // 按钮弹起时的颜色
    SColor                  m_ColorDown;            // 按钮按下时的颜色
    SColor                  m_ColorHover;           // 鼠标盘旋在按钮上时的颜色
    SColor                  m_ColorInactive;        // 按钮不可用时的颜色

private:
	AIRString				m_szImgSrcUp;
	AIRString				m_szImgSrcDown;
	AIRString				m_szImgSrcHover;
	AIRString				m_szImgSrcInactive;
};

