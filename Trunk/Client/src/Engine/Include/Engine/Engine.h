// ***************************************************************
//  Engine   version:  1.0   ��  date: 01/01/2010
//  -------------------------------------------------------------
//  Author  liangairan
//  -------------------------------------------------------------
//  Copyright (C) 2010 - All Rights Reserved
// ***************************************************************
//  �������ӿ�
// ***************************************************************

#pragma once
#include "Common.h"
//#include "AABBBox3D.h"
//#include "Dimension2D.h"
//#include "Line2D.h"
//#include "Line3D.h"
//#include "Matrix4.h"
//#include "Plane3D.h"
//#include "Position2D.h"
//#include "Quaternion.h"
//#include "Rect.h"
//#include "Triangle3D.h"
//#include "Vector2D.h"
//#include "Vector3D.h"
//#include "Util.h"
//#include "IRenderer.h"
#include "AppConfig.h"
#include "CfgReader.h"
//#include "SystemMgr.h"
#include "EngineConfig.h"
#include "FPSCounter.h"
//#include "ISceneManager.h"
#include "Timer.h"
#include "DebugTools.h"
#include "Console.h"
#include "ConsoleWindow.h"
#include "IGUIRenderer.h"
//#include "HardwareInfoSystem.h"
#include "IScriptSystem.h"
//#include "RenderTarget.h"
#include "EventReader.h"
//#include "ScreenCompositor.h"
#include "Singleton.h"
#include "EventListener.h"

//#ifndef _IRR_STATIC_LIB_
 // IRRLICHT_EXPORT

//using namespace AIR;
//template class EngineExport Matrix4f;
//template class EngineExport Matrix4i;
//template class EngineExport Vector3Df;
//template class EngineExport Vector3Di;
//template class EngineExport Plane3Df;

//class IGraphicApp;
//
//
//
//GRAPHIC_API IGraphicApp* __cdecl CreateApplication(const char* szFilename);
//
//GRAPHIC_API IGraphicApp* __cdecl CreateApplication(const LPAPPCONFIG pConfig);
//
//GRAPHIC_API CSystemMgr* __cdecl GetSystemMgr();

typedef bool (*EngineCallback)();

typedef bool (*EventCallBack)(const LPINPUTEVENT pEvent);

#if AIR_TARGET_PLATFORM == AIR_PLATFORM_WIN32

typedef bool (*ImeEventCallBack)(HWND hwnd, u32 msg, u32 wParam, u32 lParam);

typedef bool (*ClientWindowMsgCall)(u32 wParam, u32 lParam);

#endif

class ISceneManager;

//class PostScreenCompositor;

class Shadowmap;

class IInput;
class CInputEventList;
class ScreenCompositor;
class RenderTarget;
class PluginDLL;
class CCamera;
class RenderLoop;
class WindowRenderProcess;

//�����ṩ��Ĭ����Ⱦģʽ
enum EngineRenderMode
{
	eERM_Default,   //Ĭ�ϵ�ǰ����Ⱦ
	eERM_DeferredLighting,   //deferred lighting
	eERM_DeferredShading,    //Deferred shading
	eERM_Custom,             //�Զ�����Ⱦ
	eERM_MAX = 99,
};

class EngineExport CEngine : public Singleton<CEngine>
{
    BE_SINGLETON(CEngine);
public:
    
    ~CEngine();

    //�����ʼ��
    bool Initialize(const char* szConfigFile);

	//for editor
	//bool                Initialize(HWND hWnd, const char* szConfigFile);

	//���������ļ�
	bool LoadCfg( const char* szConfigFile );

    //update per frame
	void DoFrameUpdate( void );

    //��������
    void Start();

    //ֹͣ����
    void Stop();

    //�����������
    void Clear();

    //Ϊ����װ����Ⱦ��
    void SetRenderer(IRenderer* pRenderer);

    //���ó���������
    void SetSceneManager(ISceneManager* pSceneMgr);

    inline IRenderer* GetRenderer()
    {
        return m_pRenderer;
    }

    //��õ�ǰ����
    inline ISceneManager*      GetSceneMgr()
    {
        return m_pSceneMgr;
    }

    void SetFrameFunc(EngineCallback pFrameFun);

    void SetRenderFunc(EngineCallback pRenderFun);
 
    void SetGUIFrameFunc(EngineCallback pGUIFrameFun);

    void SetGUIRenderFunc(EngineCallback pGUIRenderFun);

    void SetGUIEventFunc(EventCallBack pGUIEventFun);

#if AIR_TARGET_PLATFORM == AIR_PLATFORM_WIN32
    void SetIMEEventFunc(ImeEventCallBack pImeEventFun);

	//ע��windows��Ϣ������
	void RegisterClientWindowMsgFunc(u32 uMsg, ClientWindowMsgCall pFun);

	void ProcessIMEEvent(HWND hwnd, u32 msg, u32 wParam, u32 lParam);

	inline HWND GetHWnd() const
	{
		return m_hWnd;
	}

	//windows��Ϣ������
	//LRESULT MsgProc(HWND hWnd, u32 uMsg, u32 wParam, u32 lParam);

	inline void SetHWnd(HWND hWnd)
	{
		m_hWnd = hWnd;
	}
#else
    inline unsigned long GetHWnd() const
    {
        return m_hWnd;
    }
    inline void SetHWnd( unsigned long hwnd )
    {
        m_hWnd = hwnd;
    }
#endif

	void SetProcQuitFunc(EngineCallback pProcQuitFun );

	inline void SetResetDeviceFunc(EngineCallback pLostDeviceFunc)
	{
		m_pResetDeviceFunc = pLostDeviceFunc;
	}


    //�������������Ϣ
    inline LPAPPCONFIG GetConfig()
    {
        return &m_config; 
    }

    //��ú󻺳��С
    void GetBackBufferSize(int& nWidth, int& nHeight) const;

	void GetViewSize( int &width, int &height )
	{
		width = m_ViewWidth;
		height = m_ViewHeight;
	}

	void SetViewSize( int width, int height )
	{
		m_ViewWidth = width;
		m_ViewHeight = height;
	}

    inline POINT GetLastFrameMousePoint() const
    {
        return m_lastMousePt;
    }


    //���֡��
    inline double GetFPS() const
    {
        return m_timer.GetFPS();
    }

    //��õ�ǰʱ��
    inline double GetCurTime() const
    {
        return m_timer.GetCurTime();
    }

    //��õ�ǰ����һ֡ʱ���
    inline double GetTimeSinceLastFrame() const
    {
		if ( m_timer.GetTimeSinceLastFrame() > m_config.m_fMaxFrameTimeTor )
		{
			return m_config.m_fMaxFrameTimeTor;
		}
        return m_timer.GetTimeSinceLastFrame();
    }

    inline Timer* GetTimer()
    {
        return &m_timer;
    }

    inline CFPSCounter* GetFPSCounter( void )
    {
        return &m_fpsCounter;
    }

    //���ñ�����ɫ
    void SetBackgroundColor(const SColor& color);

	inline SColor& GetBackgroundColor( void )
	{
		return m_clrBackground;
	}

    //����֡��
    //@nFrameRate ֡��
    void SetFrameRate(unsigned nFrameRate);

    //static void         KillSelf();

    //�����Ƿ���Ҫreset device
    virtual void NeedResetDevice();

	void SetGUIRenderer( IGUIRenderer* pGUIRenderer );

	inline IGUIRenderer* GetGUIRenderer( void )
    {
        return m_pGUIRenderer;
    }

	//void				QuitProgram( void );

    void SetScriptSystem( IScriptSystem* pScriptSys );

    inline IScriptSystem* GetScriptSystem( void )
    {
        return m_pScriptSystem;
    }

    //���RenderTarget
    //void AddRenderTarget(RenderTarget* pRenderTarget);

    //�Ƴ�RenderTarget
    //void DestroyRenderTarget(RenderTarget* pRenderTarget);

    //void  RemoveRenderTarget(RenderTarget* pRenderTarget);

    ////���RenderTarget
    //RenderTarget* GetRenderTarget(const AIRString& strName);

	////�������е�RTT
	//void DestroyAllRenderTarget( void );

    //�Ƿ���Ⱦ��backbuffer
    void SetNeedRTBackbuffer(bool bNeed);

    //�Ƿ���Ⱦ��
    inline bool IsRenderToBackbuffer() const
    {
        return m_bNeedRTBackbuffer;
    }

    //add the active compositor
    void  AddActiveCompositor(ScreenCompositor* pCompositor);

    //remove the active compositor
    void  RemoveActiveCompositor(ScreenCompositor* pCompositor);

    //enable the post process, this will stop the normal render of scene
    void  EnablePostProcess(bool bEnable);

    //PostScreenCompositor* GetPostScreenCompositor()
    //{
    //    return m_pPostScreenCompositor;
    //}

    inline bool IsPostProcessEnable() const
    {
        return m_bPostProcessEnable;
    }

    //shadowmap��ʼ��
    void InitializeShadowmap();

    inline float GetMeshFileVersion() const
    {
        return m_config.m_fMeshFileVersion;
    }

    inline bool IsMeshFileBinary() const
    {
        return m_config.m_bBinaryMeshFile;
    }

    inline void SetMeshFileBinary(bool bBinary)
    {
        m_config.m_bBinaryMeshFile = bBinary;
    }

    inline bool IsHardwareSkeleton() const
    {
        return m_config.m_bHardwareSkeleton;
    }

    inline bool IsDeferredLighting() const
    {
        return m_bDeferredLighting;
    }

    //�����ӳ���Ⱦ
    void EnableDeferredLighting(bool bDeferredLightingEnable);

    void SetShadowmapEnable(bool bShadowmapEnable);

    bool IsShadowmapEnable() const;

	inline bool IsAutoGenLod( void )
	{
		return m_config.m_bAutoGenLod;
	}

	inline void SetAutoGenLod( bool b )
	{
		m_config.m_bAutoGenLod = b;
	}

	inline int GetAutoLodLevelCount( void )
	{
		return m_config.m_nAutoLodLevelCount;
	}

	inline void SetAutoLodLevelCount( int nLodLevelCount )
	{
		m_config.m_nAutoLodLevelCount = nLodLevelCount;
	}

	inline u8 GetAutoLodType( void )
	{
		return m_config.m_byAutoLodType;
	}

	inline void SetAutoLodType( u8 byLodType )
	{
		m_config.m_byAutoLodType = byLodType;
	}

	inline float GetAutoLodParm( void )
	{
		return m_config.m_fAutoLodParm;
	}

	inline void SetAutoLodParm( float fLodParm )
	{
		m_config.m_fAutoLodParm = fLodParm;
	}

	inline bool GetAutoLodShareVB( void )
	{
		return m_config.m_bAutoLodShareVB;
	}

	inline void SetAutoLodShareVB( bool bShareVB )
	{
		m_config.m_bAutoLodShareVB = bShareVB;
	}

	inline void SetMaxFrameTimeTor( float fMaxFrameTime )
	{
		m_config.m_fMaxFrameTimeTor = fMaxFrameTime;
		m_config.m_fMinFpsTor = 1.0f / fMaxFrameTime;
	}

	inline float GetMaxFrameTimeTor( void )
	{
		return m_config.m_fMaxFrameTimeTor;
	}

	inline void SetMinFpsTor( float fMinFps )
	{
		m_config.m_fMinFpsTor = fMinFps;
		m_config.m_fMaxFrameTimeTor = 1.0f / fMinFps;
	}

	inline float GetMinFpsTor( void )
	{
		return m_config.m_fMinFpsTor;
	}

	inline void SetUseInEditor( bool b )
	{
		m_bUseInEditor = b;
	}

	inline bool GetUseInEditor( void )
	{
		return m_bUseInEditor;
	}

	inline void SetPause( bool b )
	{
		m_bPause = b;
	}

	inline bool GetIsLoadPlant( void )
	{
		return m_config.m_bLoadPlant;
	}

	inline void SetEnableOC( bool bEnableOC )
	{
		m_config.m_bEnableOC = bEnableOC;
	}

	inline bool GetEnableOC()
	{
		return m_config.m_bEnableOC;
	}

	inline char* GetDefaultScene()
	{
		return m_config.m_szDefaultScene;
	}

    void ExcuteRenderFunc();

    void ExcuteGUIRender();



    inline IInput* GetInputSystem()
    {
        return m_pInputSystem;
    }

    inline void SetInputSystem( IInput* pIS )
    {
        m_pInputSystem = pIS;
    }

    //������Ҫ�ǵ�����
    //void SetRTTCompositorTexture(ITexture* pTexture);

    void SetRTTCompositorEnable(bool bEnable);

    inline bool IsRTTCompositorEnable() const
    {
        return m_bRTTCompositorEnable;
    }

    //ע���¼�
    void RegistEventListener(EventListenerType type, EventListener* pListener);

    //ɾ���¼�
    void RemoveEventListener(EventListener* pListener);

    void TriggerEvent(EventListenerType type, void* pParam1, void* pParam2, void* pParam3);

    CCamera* GetActiveCamera()
    {
        return m_pActiveCamera;
    }

    void SetActiveCamera(CCamera* pCamera);

	void AddWindowRenderProcess(WindowRenderProcess* pRenderProcess);

	void RemoveWindowRenderProcess(WindowRenderProcess* pRenderProcess);

	WindowRenderProcess* GetWindowRenderProcess(const char* szName);

	void SetEngineRenderMode(EngineRenderMode mode);

	void SetResourcePath( const char* szPath )
	{
        memset( m_ResourcePath, 0, sizeof(m_ResourcePath) );
		size_t size = strlen( szPath );
		if( size > 1023 )
			size = 1023;
		memcpy( m_ResourcePath, szPath, size );
	}
    
    void SetExternalResourcePath( const char* szPath )
    {
        memset( m_ExternalResourcePath, 0, sizeof(m_ExternalResourcePath) );
        size_t size = strlen( szPath );
        if( size > 1023 )
            size = 1023;
        memcpy( m_ExternalResourcePath, szPath, size );
    }

	std::string GetResourcePath()
	{
		return std::string(m_ResourcePath);
	}
    
    std::string GetExternalResourcePath()
    {
        return std::string(m_ExternalResourcePath);
    }

	inline bool GetUseVAO() { return m_config.m_useVAO; }
	inline void SetUseVAO( bool b ) { m_config.m_useVAO = b; }

	//void AddRenderWindow(RenderTarget* pWindow);

	//void RemoveRenderWindow(RenderTarget* pWindow);
protected:
	bool InitializeBeforStart( void );

    bool Initialize(const LPAPPCONFIG pConfig);

	// for editor
	//bool                Initialize(HWND hWnd, const LPAPPCONFIG pConfig);

	//bool				ChangeScreenResolution(int width,int height, int bitsPerPixel = 32 );

    //bool                CreateRenderWindow(const LPAPPCONFIG pConfig, const char* szIconFile = NULL);

    //��Ⱦ����
    void Render();

    void Update();

    void ProcessInputEvent();

	/*
    void OnLButtonDown(int x, int y);

    void OnRButtonDown(int x, int y);

    void OnLButtonUp(int x, int y);

    void OnRButtonUp(int x, int y);

    void OnLButtonDBClick(int x, int y);

    void OnMouseMove(int x, int y);

    void OnMouseWheel(s16 dz);

    void OnKeyDown(int nKey, u32 lParam);

    void OnKeyUp(int nKey);
	*/


	void OnClose( u32 wParam, u32 lParam );

	

    ////��������rendertarget
    //void RenderAllRenderTargets();

    //ɾ������
    //void KillInstances();

    //void RenderActiveCompositor();

    //����Ĭ�ϵ�shader��������SetRenderer��ʵ��
    void CreateDefaultShaders();


private:

#if AIR_TARGET_PLATFORM == AIR_PLATFORM_WIN32
    LRESULT ProcessClientWindowMsg(u32 uMsg, u32 wParam, u32 lParam);

	void OnSize( u32 wParam, u32 lParam );

	void OnCopyData( u32 wParam, u32 lParam );
#endif

    void LoadPlugins();

    void UnloadPlugins();
private:
    //bool                                m_bResolutionChanged;   //�ֱ��ʸ�����
	//bool								m_bNeedResetDevice;

    bool                                m_bActive;    //����Ļ״̬
    ISceneManager*                      m_pSceneMgr;
    IRenderer*                          m_pRenderer;
    EngineCallback                      m_pFrameFun;        //ÿ֡���»ص�����
    EngineCallback                      m_pRenderRun;       //ÿ֡��Ⱦ�ص�����
    EngineCallback                      m_pGUIFrameFun;     //GUIÿ֡���º���
    EngineCallback                      m_pGUIRenderFun;    //GUI��Ⱦ�ص�����
	EngineCallback						m_pProcQuitFun;		//�����˳�ʱ�Ļص�����
    EventCallBack                       m_pGUIEventFun;     //GUI��Ϣ����ص�����

#if AIR_TARGET_PLATFORM == AIR_PLATFORM_WIN32
	HWND                                m_hWnd;
	HINSTANCE                           m_hInst;

    ImeEventCallBack                    m_pIMEEventFun;     //IME��Ϣ����ص�����

	HASH_MAP<u32, ClientWindowMsgCall>  m_mapWindowMsgFun;
#else
    unsigned long                       m_hWnd;
#endif

    float                               m_fFrameSpeed;      //֡��
    SColor                              m_clrBackground;    //������ɫ
    CInputEventList                     m_inputEventList;
    //ULONG_PTR                           m_gdiplusToken;
    POINT                               m_lastMousePt;
    APPCONFIG                           m_config;
    Timer                               m_timer;            //��ʱ��
    CFPSCounter                         m_fpsCounter;
    
	IGUIRenderer*						m_pGUIRenderer;

    //HardwareInfoSystem                  m_HardwareInfoSystem;

    IScriptSystem*                      m_pScriptSystem;

	EngineCallback						m_pResetDeviceFunc;	

    bool                                m_bNeedRTBackbuffer;  //�Ƿ���Ҫ��Ⱦ��backbuffer
    bool                                m_bPostProcessEnable;     //�Ƿ�ȫ������
    bool                                m_bDeferredLighting;      //�ӳ���Ⱦ�Ƿ��
    //bool                                m_bShadowmapEnable;       //�Ƿ�Ҫ������Ӱ
    bool   m_bUIReceiveEvent;    //UI�Ƿ�Ҫ������Ϣ
    //PostScreenCompositor*               m_pPostScreenCompositor;

    //rendettarget�Ĵ���
    //typedef std::list<RenderTarget*>    RenderTargetList;
    //typedef RenderTargetList::iterator  RenderTargetList_Iter;
    //RenderTargetList					m_lstRenderTargets;

    typedef AIRList<ScreenCompositor*>  CompositorList;
    typedef CompositorList::iterator    CompositorList_Iter;
    CompositorList                      m_lstActiveCompositor;

    float								m_fMeshFileVersion;

	bool								m_bUseInEditor;
	bool								m_bPause;
    bool m_bShaderCreated;
	IInput*								m_pInputSystem;
    ScreenCompositor*      m_pRTTCompositor;     //��Ҫ�ǿ������õ�
    bool m_bRTTCompositorEnable;

    typedef AIRVector<PluginDLL*> PluginsVector;
    typedef PluginsVector::iterator PluginsVector_Iter;
    PluginsVector m_vecPlugins;

    typedef AIRVector<EventListener*> EventListenerArray;
    EventListenerArray m_vecEventListeners[Event_Max];

    CCamera* m_pActiveCamera;    //��ǰ������Ⱦ��ʹ�õ�Camera

	RenderLoop* m_pCurRenderLoop;

	typedef AIRVector<WindowRenderProcess*> WindowRenderProcessVector;
	WindowRenderProcessVector m_vtWindowRenderEvents;

	EngineRenderMode  m_curRenderMode;

	char m_ResourcePath[1024];
    char m_ExternalResourcePath[1024];
    

	int m_ViewWidth;
	int m_ViewHeight;

	//AIRVector<RenderTarget*> m_vtRenderWindows;
};

#define ENGINE_INST (CEngine::GetInstance())

#define RENDERER    (ENGINE_INST->GetRenderer())

#define SCENEMGR	(ENGINE_INST->GetSceneMgr())

#define INPUTSYSTEM	(ENGINE_INST->GetInputSystem())