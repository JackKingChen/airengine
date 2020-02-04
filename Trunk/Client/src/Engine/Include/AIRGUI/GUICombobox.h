
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
    
    //获得下拉框中的数据量
    int GetItemNum();

    //获得选择的字符
    AIRString GetSelectStr();
    
    //根据位置删除一个选项
    bool DeleteStrByPos(int nPos);



    //增加一个字符的内容
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
    //combobox 的子控件
    CGUIEditBox*         m_pEdt;  //编辑框 
    CGUIButton *         m_pBtn;  //右侧的按钮
    CGUIListCtrl*        m_pLCt;  //下拉框
private:
    AIRString                    m_strBtn;        //按钮
    AIRString                    m_strEdt;        //编辑框
    AIRString                    m_strLCt;        //下拉控件
    AIRVector <AIRString>      m_vtContent;     //要显示的下列内容
};