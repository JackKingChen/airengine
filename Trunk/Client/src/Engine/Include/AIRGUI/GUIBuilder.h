// ***************************************************************
//  File: GUIBuilder.h
//  Date: 2010-2-12
//  Author: liangairan
//  Description: Builder模式的类工厂
// ***************************************************************


#pragma once
//#include "Common.h"
//#include "Singleton.h"
#include "GUIElement.h"
#include "GUISchemeManager.h"
//#include "XMLParser.h"

class AIRGuiExport CGUIBuilder //: public EngineAllocator(CGUIBuilder)
{
	EngineAllocator(CGUIBuilder)
public:
    CGUIBuilder();
    virtual ~CGUIBuilder();
    
    virtual CGUIElement* BuildElement(CGUIElement* pParent, CMarkupSTL& xml, CGUISchemeManager* pScheme){ return NULL; };

    //创建默认的控件
    virtual CGUIElement* BuildElement(CGUISchemeManager* pScheme) = 0;
};




class CGUIWindowBuilder : public CGUIBuilder
{
public:
    CGUIWindowBuilder();
    virtual ~CGUIWindowBuilder();

    virtual CGUIElement* BuildElement(CGUIElement* pParent, CMarkupSTL& xml, CGUISchemeManager* pScheme);

    virtual CGUIElement* BuildElement(CGUISchemeManager* pScheme);
};




class CGUIButtonBuilder : public CGUIBuilder
{
public:
    CGUIButtonBuilder();
    virtual ~CGUIButtonBuilder();

    virtual CGUIElement* BuildElement(CGUIElement* pParent, CMarkupSTL& xml, CGUISchemeManager* pScheme);

    virtual CGUIElement* BuildElement(CGUISchemeManager* pScheme);
private:
};



class CGUIProgressBarBuilder : public CGUIBuilder
{
public:
    CGUIProgressBarBuilder() {}
    virtual ~CGUIProgressBarBuilder() {}

    virtual CGUIElement* BuildElement(CGUIElement* pParent, CMarkupSTL& xml, CGUISchemeManager* pScheme);

    virtual CGUIElement* BuildElement(CGUISchemeManager* pScheme);
};



class CGUIStaticTextBuilder : public CGUIBuilder
{
public:
    CGUIStaticTextBuilder() {};
    virtual ~CGUIStaticTextBuilder() {}

    virtual CGUIElement* BuildElement(CGUIElement* pParent, CMarkupSTL& xml, CGUISchemeManager* pScheme);

    virtual CGUIElement* BuildElement(CGUISchemeManager* pScheme);
};




class CGUIEditBoxBuilder : public CGUIBuilder
{
public:
    CGUIEditBoxBuilder() {}
    virtual ~CGUIEditBoxBuilder() {}

    virtual CGUIElement* BuildElement(CGUIElement* pParent, CMarkupSTL& xml, CGUISchemeManager* pScheme);

    virtual CGUIElement* BuildElement(CGUISchemeManager* pScheme);
};




class CGUIAnimationBuilder : public CGUIBuilder
{
public:
    CGUIAnimationBuilder() {}
    virtual ~CGUIAnimationBuilder() {}

    virtual CGUIElement* BuildElement(CGUIElement* pParent, CMarkupSTL& xml, CGUISchemeManager* pScheme);

    virtual CGUIElement* BuildElement(CGUISchemeManager* pScheme);
};




class CGUICheckBoxBuilder: public CGUIBuilder
{
public:
    CGUICheckBoxBuilder() {}
    virtual ~CGUICheckBoxBuilder() {}

    virtual CGUIElement* BuildElement(CGUIElement* pParent, CMarkupSTL& xml, CGUISchemeManager* pScheme);

    virtual CGUIElement* BuildElement(CGUISchemeManager* pScheme);
};




class CGUISliderBuilder : public CGUIBuilder
{
public:
    CGUISliderBuilder() {}
    virtual ~CGUISliderBuilder() {}

    virtual CGUIElement* BuildElement(CGUIElement* pParent, CMarkupSTL& xml, CGUISchemeManager* pScheme);

    virtual CGUIElement* BuildElement(CGUISchemeManager* pScheme);
};




class CGUIScrollBarBuilder : public CGUIBuilder
{
public:
    CGUIScrollBarBuilder() {}
    virtual ~CGUIScrollBarBuilder() {}

    virtual CGUIElement* BuildElement(CGUIElement* pParent, CMarkupSTL& xml, CGUISchemeManager* pScheme);

    virtual CGUIElement* BuildElement(CGUISchemeManager* pScheme);
};





class CGUIListCtrlBuilder : public CGUIBuilder
{
public:
    CGUIListCtrlBuilder() {}
    virtual ~CGUIListCtrlBuilder() {}

    virtual CGUIElement* BuildElement(CGUIElement* pParent, CMarkupSTL& xml, CGUISchemeManager* pScheme);

    virtual CGUIElement* BuildElement(CGUISchemeManager* pScheme);
};

class CGUIMenuBuilder : public CGUIBuilder
{
public:
    CGUIMenuBuilder() {}
    virtual ~CGUIMenuBuilder() {}

    virtual CGUIElement* BuildElement(CGUIElement* pParent, CMarkupSTL& xml, CGUISchemeManager* pScheme);

    virtual CGUIElement* BuildElement(CGUISchemeManager* pScheme);
};


class CGUITabButtonBuilder : public CGUIBuilder
{
public:
	CGUITabButtonBuilder() {}
	virtual ~CGUITabButtonBuilder() {}

	virtual CGUIElement* BuildElement(CGUIElement* pParent, CMarkupSTL& xml, CGUISchemeManager* pScheme);

	virtual CGUIElement* BuildElement(CGUISchemeManager* pScheme);
};


class CGUITabCtrlBuilder : public CGUIBuilder
{
public:
	CGUITabCtrlBuilder() {}
	virtual ~CGUITabCtrlBuilder() {}

	virtual CGUIElement* BuildElement(CGUIElement* pParent, CMarkupSTL& xml, CGUISchemeManager* pScheme);

	virtual CGUIElement* BuildElement(CGUISchemeManager* pScheme);
};


class CGUITreeCtrlBuilder : public CGUIBuilder
{
public:
    CGUITreeCtrlBuilder() {}
    virtual ~CGUITreeCtrlBuilder() {}

    virtual CGUIElement* BuildElement(CGUIElement* pParent, CMarkupSTL& xml, CGUISchemeManager* pScheme);

    virtual CGUIElement* BuildElement(CGUISchemeManager* pScheme);
};


/*
class CGUIListItemBuilder : public CGUIBuilder
{
public:
    CGUIListItemBuilder() {}
    virtual ~CGUIListItemBuilder() {}

    virtual CGUIElement* BuildElement(CGUIElement* pParent, CMarkupSTL& xml, CGUISchemeManager* pScheme);

    virtual CGUIElement* BuildElement(CGUISchemeManager* pScheme);
};
*/
