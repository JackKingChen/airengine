#pragma once 

#include "GUIElement.h"
#include "IteratorWrappers.h"


class AIRGuiExport UILayer //: public EngineAllocator(UILayer)
{
	EngineAllocator(UILayer)
public:
    typedef AIRList<CGUIElement*> GUIELEMENT_LIST;
    typedef AIRList<CGUIElement*>::iterator  GUIELEMENT_ITER;
    typedef AIRList<CGUIElement*>::reverse_iterator GUIELEMENT_RESITER;
public:
    UILayer();
    virtual ~UILayer();

    //添加控件
    void AddEntity(CGUIElement* pEntity);

    //删除控件
    void RemoveEntity(CGUIElement* pEntity, bool bNeedDelElem = true);

    //根据名字删除控件
    void RemoveEntity(const char* szName, bool bNeedDelElem = true);
    void Render();
    void Update();
    
    void CleanUp();

    //清空某控件
    void ClearEntity(CGUIElement* pEntity);

    //根据名字查找控件
    CGUIElement* FindCtrl(const char* szCtrlName);

    bool OnLButtonDown(int x, int y);
    bool OnLButtonUp(int x, int y);
    bool OnRButtonDown(int x, int y);
    bool OnRButtonUp(int x, int y);
    bool OnMouseMove(int x, int y);
    bool OnMouseWheel(int dz);
    bool OnKeyPress(int nKey);
    bool OnKeyUp(int nKey);
    bool OnLButtonDBClick(int x, int y);

    void SetSchemeManager(CGUISchemeManager* pScheme)
    {
        m_pScheme = pScheme;
    }

    void SetMinZ(f32 fZ)
    {
        m_fMinZ = fZ;
    }

    void SetMaxZ(f32 fZ)
    {
        m_fMaxZ = fZ;
    }

    f32 GetMinZ() const
    {
        return m_fMinZ;
    }

    f32 GetMaxZ() const
    {
        return m_fMaxZ;
    }

    FrameLayer* GetFrameLayer(u32 nIndex);

    void SaveToXML(CMarkupSTL& xml);

    void CaculateEntityAlignPosition();

    void MoveFrameLayerTop(FrameLayer* pFrameLayer);

    void SetAfterLoad();

    //计算所有层的深度
    void CaculateLayerDepth();

    ListIterator<GUIELEMENT_LIST> GetGUIElementIterator(void)
    {
        return ListIterator<GUIELEMENT_LIST>(m_lstElement);
    }

    FrameLayer* CreateFrameLayer();
protected:

    


    GUIELEMENT_LIST  m_lstElement;        //正常的UI控件列表

    typedef AIRList<FrameLayer*> FrameLayerList;
    typedef FrameLayerList::iterator FrameLayerList_Iter;

    FrameLayerList  m_lstFrameLayers;
private:
    CGUISchemeManager*  m_pScheme;

    f32  m_fMinZ;
    f32  m_fMaxZ;
};
