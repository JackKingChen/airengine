
#pragma once
#include "GUIElement.h"
#include "GUIListCtrl.h"
#include "GUIEditBox.h"
#include "GUIButton.h"

class AIRGuiExport CGUICombobox:
    public CGUIElement
{
public:
    CGUICombobox(void);
    virtual ~CGUICombobox(void);

    virtual bool Create(CGUIElement* pParent, CMarkupSTL& xml);
    
    virtual bool SaveSelfToXML(CMarkupSTL& xml );
    
    virtual bool OnLButtonDown(int x, int y);

    virtual void CreateDefault();
    
    //����������е�������
    int GetItemNum();

    //���ѡ����ַ�
    AIRString GetSelectStr();
    
    //����λ��ɾ��һ��ѡ��
    bool DeleteStrByPos(int nPos);



    //����һ���ַ�������
    bool AddItemStr(AIRString strText);
    

    virtual CGUIButton* GetButton()
    {
        return m_pBtn;
    }

    virtual CGUIListCtrl* GetListCtrl()
    {
        return m_pLCt;
    }
    
    virtual CGUIEditBox* GetEditBox()
    {
        return m_pEdt;
    }

    virtual void SetSchemeManager(CGUISchemeManager* pScheme);
protected:
    virtual bool CreateSelf(CGUIElement* pParent, CMarkupSTL& xml);



    virtual void NotifyTextureDestroyChild(ITexture* pTex)
    {
        m_pBtn->NotifyTextureDestroy( pTex );
        m_pEdt->NotifyTextureDestroy( pTex );
        m_pLCt->NotifyTextureDestroy( pTex );
        CGUIElement::NotifyTextureDestroyChild( pTex );
    }


protected:
    //combobox ���ӿؼ�
    CGUIEditBox*         m_pEdt;  //�༭�� 
    CGUIButton *         m_pBtn;  //�Ҳ�İ�ť
    CGUIListCtrl*        m_pLCt;  //������
private:
    AIRString                    m_strBtn;        //��ť
    AIRString                    m_strEdt;        //�༭��
    AIRString                    m_strLCt;        //�����ؼ�
    AIRVector <AIRString>      m_vtContent;     //Ҫ��ʾ����������
};