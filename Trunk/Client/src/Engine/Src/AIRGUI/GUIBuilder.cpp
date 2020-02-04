//#include "stdafx.h"
#include "GUIBuilder.h"
#include "GUIWindow.h"
#include "GUIButton.h"
#include "GUIProgressBar.h"
#include "GUIStaticText.h"
#include "GUIEditBox.h"
#include "GUIAnimation.h"
#include "GUICheckBox.h"
#include "GUISlider.h"
#include "GUIScrollBar.h"
#include "GUIListCtrl.h"
#include "GUIMenu.h"
#include "GUITabCtrl.h"
#include "GUITreeCtrl.h"

CGUIBuilder::CGUIBuilder()
{

}

CGUIBuilder::~CGUIBuilder()
{
  
}


CGUIWindowBuilder::CGUIWindowBuilder()
{
    //CGUIBuilder::Register("CGUIWindow", this);
}

CGUIWindowBuilder::~CGUIWindowBuilder()
{

}

CGUIElement* CGUIWindowBuilder::BuildElement(CGUIElement* pParent, CMarkupSTL& xml, CGUISchemeManager* pScheme)
{
    CGUIWindow* pGUIWindow = new CGUIWindow;
    pGUIWindow->SetSchemeManager(pScheme);
    if (pGUIWindow->Create(pParent, xml))
    {
        return pGUIWindow;
    }
    else
        /*delete*/SafeDelete( pGUIWindow );
    return NULL;
}

CGUIElement* CGUIWindowBuilder::BuildElement(CGUISchemeManager* pScheme)
{
    CGUIWindow* pGUIWindow = new CGUIWindow;
    pGUIWindow->SetSchemeManager(pScheme);
    return pGUIWindow;
}


CGUIButtonBuilder::CGUIButtonBuilder()
{
    //CGUIBuilder::Register("CGUIButton", this);
}

CGUIButtonBuilder::~CGUIButtonBuilder()
{

}



CGUIElement* CGUIButtonBuilder::BuildElement(CGUIElement* pParent, CMarkupSTL& xml, CGUISchemeManager* pScheme)
{
    CGUIButton* pButton = new CGUIButton;
    pButton->SetSchemeManager(pScheme);
    if (pButton->Create(pParent, xml))
        return pButton;
    else
        SafeDelete<CGUIButton*>(pButton);
    return NULL;
}

CGUIElement* CGUIButtonBuilder::BuildElement(CGUISchemeManager* pScheme)
{
    CGUIButton* pButton = new CGUIButton;
    pButton->SetSchemeManager(pScheme);
    return pButton;
}




CGUIElement* CGUIProgressBarBuilder::BuildElement(CGUIElement* pParent, CMarkupSTL& xml, CGUISchemeManager* pScheme)
{
    CGUIProgressBar* pBar = new CGUIProgressBar;
    pBar->SetSchemeManager(pScheme);
    if (pBar->Create(pParent, xml))
    {
        return pBar;
    }
    else
        SafeDelete<CGUIProgressBar*>(pBar);

    return NULL;
}

CGUIElement* CGUIProgressBarBuilder::BuildElement(CGUISchemeManager* pScheme)
{
    CGUIProgressBar* pBar = new CGUIProgressBar;
    pBar->SetSchemeManager(pScheme);
    return pBar;
}




CGUIElement* CGUIStaticTextBuilder::BuildElement(CGUIElement* pParent, CMarkupSTL& xml, CGUISchemeManager* pScheme)
{
    CGUIStaticText* pText = new CGUIStaticText;
    pText->SetSchemeManager(pScheme);
    if (pText->Create(pParent, xml))
    {
        return pText;
    }
    else
    {
        SafeDelete(pText);
    }
    return NULL;
}

CGUIElement* CGUIStaticTextBuilder::BuildElement(CGUISchemeManager* pScheme)
{
    CGUIStaticText* pText = new CGUIStaticText;
    pText->SetSchemeManager(pScheme);
    return pText;
}




CGUIElement* CGUIEditBoxBuilder::BuildElement(CGUIElement* pParent, CMarkupSTL& xml, CGUISchemeManager* pScheme)
{
    CGUIEditBox* pEditBox = new CGUIEditBox();
    pEditBox->SetSchemeManager(pScheme);
    if (pEditBox->Create(pParent, xml))
    {
        return pEditBox;
    }
    else
    {
        SafeDelete(pEditBox);
    }
    return NULL;
}

CGUIElement* CGUIEditBoxBuilder::BuildElement(CGUISchemeManager* pScheme)
{
    CGUIEditBox* pEditBox = new CGUIEditBox;
    pEditBox->SetSchemeManager(pScheme);
    return pEditBox;
}




CGUIElement* CGUIAnimationBuilder::BuildElement(CGUIElement* pParent, CMarkupSTL& xml, CGUISchemeManager* pScheme)
{
    CGUIAnimation* pAnimation = new CGUIAnimation();
    pAnimation->SetSchemeManager(pScheme);
    if (pAnimation->Create(pParent, xml))
    {
        return pAnimation;
    }
    else
    {
        SafeDelete(pAnimation);
    }

    return NULL;
}

CGUIElement* CGUIAnimationBuilder::BuildElement(CGUISchemeManager* pScheme)
{
    CGUIAnimation* pAnimation = new CGUIAnimation();
    pAnimation->SetSchemeManager(pScheme);
    return pAnimation;
}




CGUIElement* CGUICheckBoxBuilder::BuildElement(CGUIElement* pParent, CMarkupSTL& xml, CGUISchemeManager* pScheme)
{
    CGUICheckBox* pCheckBox = new CGUICheckBox;
    pCheckBox->SetSchemeManager(pScheme);

    if (pCheckBox->Create(pParent, xml))
    {
        return pCheckBox;
    }
    else
    {
        SafeDelete(pCheckBox);
    }

    return NULL;
}

CGUIElement* CGUICheckBoxBuilder::BuildElement(CGUISchemeManager* pScheme)
{
    CGUICheckBox* pCheckBox = new CGUICheckBox;
    pCheckBox->SetSchemeManager(pScheme);

    return pCheckBox;
}




CGUIElement* CGUISliderBuilder::BuildElement(CGUIElement* pParent, CMarkupSTL& xml, CGUISchemeManager* pScheme)
{
    CGUISlider* pSlider = new CGUISlider;
    pSlider->SetSchemeManager(pScheme);

    if (pSlider->Create(pParent, xml))
    {
        return pSlider;
    }
    else 
    {
        SafeDelete(pSlider);
    }

    return NULL;
}

CGUIElement* CGUISliderBuilder::BuildElement(CGUISchemeManager* pScheme)
{
    CGUISlider* pSlider = new CGUISlider;
    pSlider->SetSchemeManager(pScheme);
    pSlider->CreateDefault();
    
    return pSlider;
}




CGUIElement* CGUIScrollBarBuilder::BuildElement(CGUIElement* pParent, CMarkupSTL& xml, CGUISchemeManager* pScheme)
{
    CGUIScrollBar* pScrollBar = new CGUIScrollBar;
    pScrollBar->SetSchemeManager(pScheme);

    if (pScrollBar->Create(pParent, xml))
    {
        return pScrollBar;
    }
    else
    {
        SafeDelete(pScrollBar);
    }

    return NULL;
}

CGUIElement* CGUIScrollBarBuilder::BuildElement(CGUISchemeManager* pScheme)
{
    CGUIScrollBar* pScrollBar = new CGUIScrollBar;
    pScrollBar->SetSchemeManager(pScheme);
    pScrollBar->CreateDefault();
    
    return pScrollBar;
}




CGUIElement* CGUIListCtrlBuilder::BuildElement(CGUIElement* pParent, CMarkupSTL& xml, CGUISchemeManager* pScheme)
{
    CGUIListCtrl* pCtrl = new CGUIListCtrl;
    pCtrl->SetSchemeManager(pScheme);
    if (pCtrl->Create(pParent, xml))
    {
        return pCtrl;
    }
    else
    {
        SafeDelete(pCtrl);
    }

    return NULL;
}

CGUIElement* CGUIListCtrlBuilder::BuildElement(CGUISchemeManager* pScheme)
{
    CGUIListCtrl* pCtrl = new CGUIListCtrl;
    pCtrl->SetSchemeManager(pScheme);
    return pCtrl;
}




CGUIElement* CGUIMenuBuilder::BuildElement(CGUISchemeManager* pScheme)
{
    CGUIMenu* pMenu = new CGUIMenu;
    pMenu->SetSchemeManager(pScheme);
    pMenu->CreateDefault();
    return pMenu;
}

CGUIElement* CGUIMenuBuilder::BuildElement(CGUIElement* pParent, CMarkupSTL& xml, CGUISchemeManager* pScheme)
{
    CGUIMenu* pCtrl = new CGUIMenu;
    pCtrl->SetSchemeManager(pScheme);
    if (pCtrl->Create(pParent, xml))
    {
        return pCtrl;
    }
    else
    {
        SafeDelete(pCtrl);
    }

    return NULL;
}




CGUIElement* CGUITabButtonBuilder::BuildElement(CGUIElement* pParent, CMarkupSTL& xml, CGUISchemeManager* pScheme)
{
	CGUITabButton* pCtrl = new CGUITabButton;
	pCtrl->SetSchemeManager(pScheme);
	if (pCtrl->Create(pParent, xml))
	{
		return pCtrl;
	}
	else
	{
		SafeDelete(pCtrl);
	}

	return NULL;
}

CGUIElement* CGUITabButtonBuilder::BuildElement(CGUISchemeManager* pScheme)
{
	CGUITabButton* pCtrl = new CGUITabButton;
	pCtrl->SetSchemeManager(pScheme);
	return pCtrl;
}




CGUIElement* CGUITabCtrlBuilder::BuildElement(CGUIElement* pParent, CMarkupSTL& xml, CGUISchemeManager* pScheme)
{
	CGUITabCtrl* pCtrl = new CGUITabCtrl;
	pCtrl->SetSchemeManager(pScheme);
	if (pCtrl->Create(pParent, xml))
	{
		return pCtrl;
	}
	else
	{
		SafeDelete(pCtrl);
	}

	return NULL;
}

CGUIElement* CGUITabCtrlBuilder::BuildElement(CGUISchemeManager* pScheme)
{
	CGUITabCtrl* pCtrl = new CGUITabCtrl;
	pCtrl->SetSchemeManager(pScheme);
	return pCtrl;
}





CGUIElement* CGUITreeCtrlBuilder::BuildElement(CGUISchemeManager* pScheme)
{
    CGUITreeCtrl* pCtrl = new CGUITreeCtrl;
    pCtrl->SetSchemeManager(pScheme);
    pCtrl->CreateDefault();
    return pCtrl;
}

CGUIElement* CGUITreeCtrlBuilder::BuildElement(CGUIElement* pParent, CMarkupSTL& xml, CGUISchemeManager* pScheme)
{
    CGUITreeCtrl* pCtrl = new CGUITreeCtrl;
    pCtrl->SetSchemeManager(pScheme);
    if (pCtrl->Create(pParent, xml))
    {
        return pCtrl;
    }
    else
    {
        SafeDelete(pCtrl);
    }

    return NULL;
}

