#include "GUICombobox.h"
CGUICombobox::CGUICombobox(void)
{
    m_strName = "CGUICombobox";
    m_bMoveable = false;
}

CGUICombobox::~CGUICombobox(void)
{
    
}
bool CGUICombobox::Create(CGUIElement* pParent, CMarkupSTL& xml)
{
    if (!CGUIElement::Create(pParent, xml))
    {
        return false;
    }

    m_strBtn = xml.GetAttrib("Btn").c_str();
    m_strEdt = xml.GetAttrib("Edt").c_str();
    m_strLCt = xml.GetAttrib("LCt").c_str();

    m_pBtn = dynamic_cast<CGUIButton*>(GetChild(m_strBtn.c_str()));
    m_pEdt = dynamic_cast<CGUIEditBox*>(GetChild(m_strEdt.c_str()));
    m_pLCt = dynamic_cast<CGUIListCtrl*>(GetChild(m_strLCt.c_str()));

    return true;
}
bool CGUICombobox::CreateSelf(CGUIElement* pParent, CMarkupSTL& xml)
{
    if (!CGUIElement::CreateSelf(pParent, xml))
    {
        return false;
    }
    m_strBtn = xml.GetAttrib("Btn").c_str();
    m_strEdt = xml.GetAttrib("Edt").c_str();
    m_strLCt = xml.GetAttrib("LCt").c_str();
    return true;
}

bool CGUICombobox::SaveSelfToXML(CMarkupSTL& xml )
{
    CGUIElement::SaveSelfToXML( xml );
    xml.AddAttrib( "Btn", m_strBtn.c_str());
    xml.AddAttrib( "Edt", m_strEdt.c_str());
    xml.AddAttrib( "LCt", m_strLCt.c_str());
    return true;
}

void CGUICombobox::CreateDefault()
{

    m_renderQuad.m_rectScreen.SetRect(10, 10, 300, 500);
    m_pBtn = new CGUIButton;
    m_pBtn->SetSchemeManager(m_pScheme);
    m_pBtn->SetName("Btn");
    m_pBtn->SetParent(this);
    AddChild(m_pBtn);
    m_pBtn->SetRect(Rectf(60, 10, 26, 20));
    m_strBtn = "Btn";

    m_pEdt = new CGUIEditBox;
    m_pEdt->SetSchemeManager(m_pScheme);
    m_pEdt->SetParent(this);
    m_pEdt->SetName("Edt");
    m_pEdt->SetRect(Rectf(10, 10, 50, 20));
    AddChild(m_pEdt);
    m_strEdt = "Edt";

    m_pLCt = new CGUIListCtrl;
    m_pLCt->SetSchemeManager(m_pScheme);
    m_pLCt->SetName("LCt");
    m_pLCt->SetParent(this);
    m_pLCt->SetRect(Rectf(10, 20, 50, 20));
    GUILIST_ITEM ItemInfo;
    ItemInfo.nHeight = 20;
    ItemInfo.nWidth  = 50;
    m_pLCt->SetItemInfo(ItemInfo);
    AddChild(m_pLCt);
    m_pLCt->SetCol(1);
    m_pLCt->SetRow(0);
    m_strLCt = "LCt";
    m_pLCt->Hide();
}


int  CGUICombobox::GetItemNum()
{
    return m_pLCt->GetRow();
}

//给combobox增加一个字符内容
bool CGUICombobox::AddItemStr(AIRString strText)
{
    m_pLCt->PlusRow();                   //增加一行
    int Len = GetItemNum() -1;           //计算出原有行数
    for (int i = 0  ; i < Len ; i++)     //把原先的内容set到listit中
    { 

        CGUIListItem* pLI = m_pLCt->GetItem(i,0);
        pLI->SetText(m_vtContent[i].c_str());
        
    }
    m_vtContent.push_back(strText);//插入新的Item
    CGUIListItem* pLI = m_pLCt->GetItem(Len,0);
    pLI->SetText(strText.c_str());
    return true;
}

//按内容位置删除
bool CGUICombobox::DeleteStrByPos(int nPos)
{
    if (nPos < 0 || nPos >= GetItemNum())//如果是不合法的位置
    {
        return false;
    }
    m_pLCt->DecRow();//减少一行
    AIRVector<AIRString>::iterator it = m_vtContent.begin()+ nPos;
    m_vtContent.erase(it);//把一行内容删除
    for (int i = 0 ; i < GetItemNum() ; i++)//显示其余行
    {
        CGUIListItem* pLI = m_pLCt->GetItem(i,0);
        pLI->SetText(m_vtContent[i].c_str());
    }
    
}

bool CGUICombobox::OnLButtonDown(int x, int y)
{
    
    if (m_pBtn && m_pBtn->IsInRect(x, y))
    {//按下按钮显示ListCtrl列表
        if (!m_pLCt->IsVisable())
        {
            m_pLCt->Show();
        }
        else
        {
            m_pLCt->Hide();
        }
        m_pBtn->OnLButtonDown(x,y);
        return true;
    }
    
    if (m_pEdt && m_pEdt->IsInRect(x,y))
    {//如果是点击到编辑框
        return m_pEdt->OnLButtonDown(x,y);
    }
    
    if(m_pLCt && m_pLCt->IsInRect(x,y))//如果点击了下列框
    {
        if (m_pLCt->OnLButtonDown(x,y))
        {
            CGUIListItem* pListItme = m_pLCt->GetSelectedItem();
            AIRString strTex = pListItme->GetText();//获得选择了的内容
            m_pEdt->SetText(strTex.c_str());//把内容放入到Editbox中
            m_pLCt->Hide();//并把下拉框隐藏
        }
    }
    return true;
}

void CGUICombobox::SetSchemeManager(CGUISchemeManager* pScheme)
{
    m_pScheme = pScheme;
}

AIRString CGUICombobox::GetSelectStr()
{
    return m_pEdt->GetText();
}