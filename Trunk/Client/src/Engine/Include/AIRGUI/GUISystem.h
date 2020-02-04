// ***************************************************************
//  �ļ�: GUISystem.h
//  ����: 2009-4-29
//  ����: liangairan
//  ˵��: GUIϵͳ�࣬��װ�˴󲿷ֵ�UI���������
// ***************************************************************
#pragma once

#include "AIRGUI.h"
#include "GUISchemeManager.h"
#include "MarkupSTL.h"
#include "CIME.h"
#include "FontManager.h"
#include "EventReader.h"
#include "IGUIFormater.h"
#include "GUIRenderer.h"
#include "GUIMouse.h"
#include "Singleton.h"

class CGUIDragImage;
class FileData;

class AIRGuiExport CGUISystem : public Singleton<CGUISystem>
{
    BE_SINGLETON(CGUISystem);
protected:    
    
    void UpdateWindowSize();
public:
    ~CGUISystem(void);

	CGUISchemeManager*  CreateScheme( const char* szName );

    bool LoadFromCFG(const char* szFilename);
    bool LoadFromXML(const char* szFilename);

    //bool LoadImageSet(const char* szFilename);

    //---------------------------------------------------------
    // ����:  LoadSelf
    // ����:  ����ڵ�����
    // ����:  ThisElement - XML�ڵ�
    // ����:  ���سɹ�
    //---------------------------------------------------------
    bool LoadSelf( CMarkupSTL& xml );

    //---------------------------------------------------------
    // ����:  LoadChild
    // ����:  �����ӽڵ�
    // ����:  ThisElement - XML�ڵ�
    // ����:  ���سɹ�
    //---------------------------------------------------------
    bool LoadChild( CMarkupSTL& xml );

    //---------------------------------------------------------
    // ����:  Update
    // ����:  ����GUI���ֹ��������
    // ����:  ��
    // ����:  ��
    //---------------------------------------------------------
    bool Update( void );

    void Initialize();

    void Clear();

    bool Render();

    void SetCurrentScheme(const AIRString& strSchemeName);

    void SetCurrentScheme(CGUISchemeManager* pScheme);

    CGUISchemeManager*  GetCurrentScheme( void );

    //void                SetEditViewSize(int nWidth, int nHeight);

    //void SetEditViewHeight(int nHeight);
    //float               GetRenderZoomW();
    //float               GetRenderZoomH();

    //CImageSet* GetImageSet(const AIRString& strImageSetName);
    //CImageUnit* GetImageUnit(const AIRString& strImageSetName, const AIRString& strImageUnitName);

    CGUISchemeManager*  LoadSceme(const char* szFilename);

#if AIR_TARGET_PLATFORM == AIR_PLATFORM_WIN32
    CIME*               GetIME() { return &m_ime; }

	LRESULT             MsgProc( HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam );

	bool				ProcessIMEEvent(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

private:
	bool                IMEFollow(HWND hWnd);
#endif

public:

    ITexture*           LoadTexture(const char* szFilename);

    void                ClearTexture(ITexture* pTex);

    

    //IFontBase* CreateFont(const FONTTYPE& ft, const char* szText);

    //����������Ϣ���������true���������´���
    bool                ProcessInputEvent(const LPINPUTEVENT pEvent);

    CGUIElement*        GetFocusElement() { return m_pFocusElement; }

    CGUIElement*        GetDragElement() { return m_pDragElement; }

    CGUIElement*        GetHoverElement() { return m_pHoverElement; }

    CGUIDragImage*      GetMouseDragImage() { return m_pMouseDragImage; }

    CGUIEditCtrl*       GetEditCtrl()
    {
        return  m_pEditCtrl;
    }

    void SetEditCtrl(CGUIEditCtrl* pCtrl);

    void SetFocusElement(CGUIElement* e);

    void SetDragElement(CGUIElement* e);

    void SetHoverElement(CGUIElement* e);

    void SetMouseDragImage(CGUIDragImage* e);

    //ע��ؼ��ص�����
    void SetCtrlLButtonDownCallBack(const char* szCtrlName, LButtonDownCallback pFun);

    void SetCtrlLButtonUpCallBack(const char* szCtrlName, LButtonUpCallBack pFun);

    void SetCtrlRButtonDownCallBack(const char* szCtrlName, RButtonDownCallBack pFun);

    void SetCtrlRButtonUpCallBack(const char* szCtrlName, RButtonUpCallBack pFun);

    void SetCtrlKeyPressCallBack(const char* szCtrlName, KeyPressCallBack pFun);

    void SetCtrlMouseMoveCallBack(const char* szCtrlName, MouseMoveCallBack pFun);

    void SetCtrlMouseLeaveCallBack(const char* szCtrlName, MouseLeaveCallBack pFun);

    void SetCtrlOnShowCallBack(const char* szCtrlName, OnShowCallBack pFun);

    CGUIElement* FindCtrl(const char* szCtrlName);

    //CGUIElement*        FindCtrl(const AIRString& strCtrlName);

    void GetLastCursorPos(POINT& pt);

    CGUISchemeManager* GetScheme(const AIRString& strName);

    float GetMaxDepth() const
    {
        return m_fMaxDepth;
    }

    float GetMinDepth() const
    {
        return m_fMinDepth;
    }

	bool SaveToXML( const char* szFile );

    //static void			KillSelf()
    //{
    //    SafeDelete(m_pInstance);
    //}

	// ȡ��ĳ��ͼ�Ĳ�������
	// ps:��������ɹ�,ppData������ڴ�����Ҫ�û��Լ��ͷ�
	bool GetImagePixelData( void** ppData, u32& size, int& nWidth, int& nHeight, u8& bytesPerPixel, const char* szImgSetName, const char* szImgUnitName );


	bool RemoveScheme( CGUISchemeManager* pScheme );


    //֪ͨ�ؼ���ɾ��
    void NotifyElementDelete(CGUIElement* pElement);

    void SetGUIFormater(IGUIFormater* pFormater);

    //��ø�ʽ����
    IGUIFormater* GetGUIFormater()
    {
        return m_pGUIFormater;
    }

    GUIRenderer* GetGUIRenderer()
    {
        return &m_uiRenderer;
    }

    //�ѿؼ��Ƶ����ʾ
    //void MoveElementToTop(CGUIElement* pElement);
    CGUIMouse* GetGUIMouse()
    {
        return &m_guiMouse;
    }

    //����GUI��������
    //void CacheGUI();

private:

    typedef std::map<AIRString, SCHEME_DATA>           GUISCHEMEDATA_MAP;
    typedef GUISCHEMEDATA_MAP::iterator             GUISCHEMEDATA_ITER;
    typedef std::map<AIRString, CGUISchemeManager*>    GUISCHEME_MAP;
    typedef GUISCHEME_MAP::iterator                 GUISCHEME_ITER;

    GUISCHEME_MAP       m_mapGUISchemeMgr;
    GUISCHEMEDATA_MAP   m_mapSchemeData;
    CGUISchemeManager*  m_pCurSchemeMgr;
    
    string              m_strGUISystem;
    //float               m_fWndZoomW;        // ��ǰ�ֱ����µĴ������׼�ֱ����µĴ��ڵĿ������ֵ
    //float               m_fWndZoomH;        // ��ǰ�ֱ����µĴ������׼�ֱ����µĴ��ڵĸ߶�����ֵ
    //float               m_fRenderZoomW;     // ��ǰ�ֱ����µĿͻ������׼�ֱ����µĿͻ����Ŀ�ȵ�����ֵ
    //float               m_fRenderZoomH;     // ��ǰ�ֱ����µĿͻ������׼�ֱ����µĿͻ����ĸ߶ȵ�����ֵ
    //int                 m_nEditWndWidth;    // ��׼ģʽ�µĴ��ڿ��
    //int                 m_nEditWndHeight;   // ��׼ģʽ�µĴ��ڸ߶�
    //int                 m_nEditViewWidth;   // ��׼ģʽ�µĿͻ������
    //int                 m_nEditViewHeight;  // ��׼ģʽ�µĿͻ����߶�

#if AIR_TARGET_PLATFORM == AIR_PLATFORM_WIN32
    CIME                m_ime;
	HWND                m_hWnd;
#endif

    //IFontBase*          m_pFontTest;
    bool                m_bMouseDown;
    int                 m_nMousePosX;
    int                 m_nMousePosY;
    POINT               m_ptCursorPos;
    CGUIElement*        m_pFocusElement;
    CGUIElement*        m_pDragElement;
    CGUIElement*        m_pHoverElement;
    CGUIDragImage*      m_pMouseDragImage;    //�����󶨵�
    CGUIEditCtrl*       m_pEditCtrl;          //ר���Ǳ༭�Ŀؼ�

    float               m_fMaxDepth;          //�������
    float               m_fMinDepth;          //��С�����

    int                 m_nRenderWinWidth;    //��Ⱦ���ڿ�
    int                 m_nRenderWinHeight;   //��Ⱦ���ڸ�

    IGUIFormater*       m_pGUIFormater;       //��ʽ����
    GUIRenderer         m_uiRenderer;

    CGUIMouse           m_guiMouse;

    //FileData*  m_pSchemeResource;    //scheme�ļ�
};

//#define GUISYSTEM CGUISystem::GetInstancePtr()  
#define GUISYSTEM CGUISystem::GetInstance()  
#define GUIRENDERER	(GUISYSTEM->GetGUIRenderer())