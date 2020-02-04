/********************************************************************
	created:	2009/11/16
	created:	16:11:2009   12:05
	filename: 	d:\My Codes\Engine\AIRGUI\GUIScrollBar.h
	file path:	d:\My Codes\Engine\AIRGUI
	file base:	GUIScrollBar
	file ext:	h
	author:		liangairan
	
	purpose:	ScrollBar�Ŀؼ��࣬����Ҫע��һ�㣬����һ����������
                �������൱��CGUIElement����������Ҫ��д��Self�ĺ�����
*********************************************************************/

#pragma once
#include "guielement.h"
#include "GUISlider.h"
#include "GUIButton.h"

class CGUIScrollBar :
    public CGUIElement
{
public:
    CGUIScrollBar(void);
    virtual ~CGUIScrollBar(void);

    virtual bool Create(CGUIElement* pParent, CMarkupSTL& xml);

	virtual bool SaveSelfToXML(CMarkupSTL& xml );

    //virtual bool IsInRect(int x, int y);

    //���ù����Ŀؼ�
    virtual void SetRelatedCtrl(CGUIElement* pElement);

    virtual void SetRelatedCtrl(const char* szCtrlName);

	const AIRString& GetRelatedCtrlName( void )
	{
		return m_strRelatedCtrl;
	}

    //��Щ��������Ҫ��д����Ϊ���Ǹ������
    virtual bool OnMouseWheel(int dz);

    virtual bool OnMouseMove(int x, int y);

    virtual bool OnLButtonUp(int x, int y);

    virtual bool OnLButtonDown(int x, int y);

    virtual CGUIButton* GetButtonStart() 
    {
        return m_pBtnStart;
    }

    virtual CGUIButton* GetButtonEnd()
    {
        return m_pBtnEnd;
    }

    virtual CGUISlider* GetSlider()
    {
        return m_pSlider;
    }

    //
    virtual void CreateDefault();
    
    //��������button�Ŀ�
    virtual void SetButtonWidth(int nWidth);

    //��������button�ĸ�
    virtual void SetButtonHeight(int nHeight);

    //����slider��Button��С
    virtual void SetSliderBtnWidth(int nWidth);

    virtual void SetSliderBtnHeight(int nHeight);

    virtual void SetRect(const Rectf& rect);

    

protected:
    //virtual void ClearChild();
    CGUIButton* m_pBtnStart;
    CGUIButton* m_pBtnEnd;
    CGUISlider* m_pSlider;

    CGUIElement* m_pRelatedElement;   //������GUIԪ��

    //�����ӿؼ���λ��
    virtual void ModifyChildrenPosition();
protected:
    virtual bool CreateSelf(CGUIElement* pParent, CMarkupSTL& xml);
    
    virtual void SetAfterLoadSelf();

    //����λ��
    virtual void CalculateRelatedCtrlPos();

    virtual void NotifyTextureDestroyChild(ITexture* pTex)
    {
        m_pBtnStart->NotifyTextureDestroy( pTex );
        m_pBtnEnd->NotifyTextureDestroy( pTex );
        m_pSlider->NotifyTextureDestroy( pTex );
        CGUIElement::NotifyTextureDestroyChild( pTex );
    }

    //�����Լ�����Ҫ�Ǹ������ݵ���slider��λ��
    virtual void UpdateSelf();

    //virtual bool OnMouseMoveSelf(int x, int y);

    //virtual bool OnMouseWheelSelf();
private:
    //�ӿؼ�������
    AIRString m_strBtnUp;    //���ϵ�button
    AIRString m_strBtnDown;  //���µ�button
    AIRString m_strSlider;   //����������
    AIRString m_strRelatedCtrl;   //��صĿؼ���
    int         m_nRelatedCtrlOffset;    //���ƶ����ȣ�ʵ�����ǹ����ؼ������߶���scrollbar�߶ȵĲ�ֵ
};
