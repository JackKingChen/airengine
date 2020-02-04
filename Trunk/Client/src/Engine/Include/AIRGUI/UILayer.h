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

    //��ӿؼ�
    void AddEntity(CGUIElement* pEntity);

    //ɾ���ؼ�
    void RemoveEntity(CGUIElement* pEntity, bool bNeedDelElem = true);

    //��������ɾ���ؼ�
    void RemoveEntity(const char* szName, bool bNeedDelElem = true);
    void Render();
    void Update();
    
    void CleanUp();

    //���ĳ�ؼ�
    void ClearEntity(CGUIElement* pEntity);

    //�������ֲ��ҿؼ�
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

    //�������в�����
    void CaculateLayerDepth();

    ListIterator<GUIELEMENT_LIST> GetGUIElementIterator(void)
    {
        return ListIterator<GUIELEMENT_LIST>(m_lstElement);
    }

    FrameLayer* CreateFrameLayer();
protected:

    


    GUIELEMENT_LIST  m_lstElement;        //������UI�ؼ��б�

    typedef AIRList<FrameLayer*> FrameLayerList;
    typedef FrameLayerList::iterator FrameLayerList_Iter;

    FrameLayerList  m_lstFrameLayers;
private:
    CGUISchemeManager*  m_pScheme;

    f32  m_fMinZ;
    f32  m_fMaxZ;
};
