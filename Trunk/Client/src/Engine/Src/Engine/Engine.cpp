// Engine.cpp : ���� DLL Ӧ�ó������ڵ㡣
//

#include "stdafx.h"

//#include <GdiPlus.h>
#include "Engine.h"
//#include "SEFps.h"
#include "FileSystem.h"
#include "FontManager.h"
#include "MaterialManager.h"
#include "Resource.h"
//#include "ParticleSystemManager.h"
//#include "ModelInstanceManager.h"
//#include "modelmanager.h"
#include "ParticleAffectorFactory.h"
#include "Terrain.h"
#include "RenderableFactory.h"
#include "ParticleEmitterFactory.h"
//#include "GEffectManager.h"
#include "CSoundSystem.h"
#include "BillboardSetManager.h"
#include "VertexDeclarationManager.h"
#include "BoundingBoxEntity.h"
#include "ISceneManager.h"
#include "PageStaticGeometory.h"
//#include "ManualGPUDataParamSource.h"
//#include "PostScreenCompositor.h"
#include "Water.h"
//#include "PostProcessManager.h"
#include "Shadowmap.h"
#include "TimeBaseProfile.h"
#include "InstancingObjectManager.h"
#include "LightingManager.h"
#include "BillboardRenderer.h"
#include "DecalManager.h"
//#include "SpeedTreeInstManager.h"
#include "EngineResource.h"
#include "GPUProgramManager.h"
#include "IInput.h"
#include "NodeAnimation.h"
//#include "SpeedTreeEngine.h"
#include "TextureMgr.h"
#include "ParticleSystem.h"
#include "WeatherManager.h"
#include "PluginDLL.h"
#include "RenderLoop.h"
#include "CustomRenderProcess.h"
#include "InternalFixedStrings.h"
//#include "AISInputSystem.h"
#include "AISInputDirector.h"
#include "WindowRenderProcess.h"

#if AIR_TARGET_PLATFORM == AIR_PLATFORM_ANDROID || AIR_TARGET_PLATFORM == AIR_PLATFORM_IOS
#include "OpenGLES2RenderSystem.h"
#endif

#ifdef _MANAGED
#pragma managed(push, off)
#endif

#if AIR_TARGET_PLATFORM == AIR_PLATFORM_WIN32
BOOL APIENTRY DllMain( HMODULE hModule,
                       u32  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
    return TRUE;
}
#define SLEEP(millisecond) Sleep(millisecond)
#else
#include <unistd.h> // sleep 
#define SLEEP(millisecond) usleep(millisecond * 1000)
#endif

#ifdef _MANAGED
#pragma managed(pop)
#endif


//CEngine* CEngine::m_pInstance = NULL;


//LRESULT CALLBACK WndProc( HWND hWnd, u32 uMsg, u32 wParam, u32 lParam )
//{
//    return ENGINE_INST->MsgProc( hWnd, uMsg, wParam, lParam );
//}


CEngine::CEngine() : 
//m_bResolutionChanged(false), 
//m_bNeedResetDevice(false),
#if AIR_TARGET_PLATFORM == AIR_PLATFORM_WIN32
m_hWnd(NULL), 
m_hInst(NULL), 
m_pIMEEventFun(NULL), 
#endif
m_bActive(false), 
m_pSceneMgr(NULL), 
m_pRenderer(NULL), 
m_pFrameFun(NULL), 
m_pRenderRun(NULL),
m_pGUIFrameFun(NULL), 
m_pGUIRenderFun(NULL), 
m_pGUIEventFun(NULL), 
m_pProcQuitFun(NULL), 
//m_gdiplusToken(NULL),
m_pGUIRenderer(NULL),
m_pResetDeviceFunc(NULL)
, m_bNeedRTBackbuffer(true)
, m_bPostProcessEnable(false)
, m_bDeferredLighting(false)
//, m_bShadowmapEnable(false)
, m_bUIReceiveEvent(true)
, m_fMeshFileVersion(0.1f)
, m_bPause(false)
, m_bShaderCreated(false)
, m_pInputSystem(NULL)
, m_pRTTCompositor(NULL)
, m_bRTTCompositorEnable(false)
, m_pActiveCamera(0)
, m_pCurRenderLoop(NULL)
, m_curRenderMode(eERM_Default)
//, m_pPostScreenCompositor(NULL)
{
    m_clrBackground = SColor(0, 0, 0, 0);
    m_pScriptSystem = NULL;
	m_bUseInEditor = false;
	m_ViewWidth = 800;
	m_ViewHeight = 480;
	memset( m_ResourcePath, 0, sizeof(m_ResourcePath) );

	AISInputDirector::GetInstance();
}


CEngine::~CEngine()
{

}


//���������ļ�
bool CEngine::LoadCfg( const char* szConfigFile )
{
	if ( szConfigFile )
	{
		ConfigReader reader;
        if (!reader.ReadFile(std::string(szConfigFile)))
		{
			char* msg = GetString("�����ļ� [%s] ������!", szConfigFile);
			DT_TO_MSGBOX_FMT( LT_ERROR, msg );
			return false;
		}
		else
		{
			char* msg = GetString("�ɹ���ȡ�����ļ� [%s]!", szConfigFile);
			DT_TO_DBGSTR( LT_DEBUG, msg );
		}

		// ������־�ļ�
		if ( reader.Exist("logfile") )
		{
			AIRString logfile( reader.GetString("logfile") );
			DT_ADD_LOG( "SysLog", logfile.c_str() );
			DT_SET_LOG( "SysLog" );
		}
		else
		{
			AIRString logFile = "defaultLog.log";
#if AIR_TARGET_PLATFORM == AIR_PLATFORM_ANDROID
			logFile = GetResourcePath() + "/defaultLog.log";
#elif AIR_TARGET_PLATFORM == AIR_PLATFORM_IOS
            logFile = GetExternalResourcePath() + "/defaultLog.log";
#endif
			DT_ADD_LOG( "SysLog", logFile.c_str() );
			DT_SET_LOG( "SysLog" );
		}
		DT_CLR_CUR_LOG();

		int w = 800, h = 600;
		// ���ڿ��
		if ( reader.Exist("windowWidth") )
		{
			w = reader.GetInt("windowWidth");
		}

		// ���ڸ߶�
		if ( reader.Exist("windowHeight") )
		{
			h = reader.GetInt("windowHeight");
		}
		m_config.m_windowSize = WinSize( w, h );

		if ( reader.Exist("wndPosX") )
		{
			m_config.m_nWndPosX = reader.GetInt( "wndPosX" );
		}

		if ( reader.Exist("wndPosY") )
		{
			m_config.m_nWndPosY = reader.GetInt( "wndPosY" );
		}

		if( reader.Exist("centerWindow") )
		{
			m_config.m_bCenterWindow = reader.GetBool("centerWindow");
		}

		if ( reader.Exist( "autoAlignWnd" ) )
		{
			m_config.m_bAutoAlignWnd = reader.GetBool( "autoAlignWnd" );
		}

		// ���ڷֱ���ID
		if ( reader.Exist("winSize") )
		{
			int nWinSizeID = reader.GetInt("winSize");
			if ( nWinSizeID >= 0 && nWinSizeID < 15 )
			{
				m_config.m_windowSize = G_WinSizes[nWinSizeID];
			}
		}

		// �Ƿ�ȫ����ʾ��
		if ( reader.Exist("fullScreen") )
		{
			m_config.m_bFullScreen = reader.GetBool("fullScreen");
		}

		// ͼ���豸����
		if ( reader.Exist("device") )
		{
			m_config.m_nDeviceType = reader.GetInt("device");
		}

		// ���ڱ���
		if ( reader.Exist("windowTitle") )
		{
			strcpy(m_config.m_szWindowTitle, reader.GetString("windowTitle"));
		}

		// ����ͼ���ļ�
		if ( reader.Exist("iconFile") )
		{
			strcpy(m_config.m_szIconFile, reader.GetString("iconFile") );
		}

		// �����ļ�(��ʱû��)
		if ( reader.Exist("scene") )
		{
			strcpy(m_config.m_szSceneFile, reader.GetString("scene"));
		}

		// ���֡��
		if ( reader.Exist("maxFramePerSecond") )
		{
			m_config.m_nMaxFramePerSecond = reader.GetInt("maxFramePerSecond");
		}

		// �Ƿ�򿪵��Կ���̨����
		if ( reader.Exist("console") )
		{
			m_config.m_bShowDebugConsoleWin = reader.GetBool( "console" );
		}

		// �Ƿ��NVPERFHUD���Թ���
		if ( reader.Exist("openNVPerfHUD") )
		{
			m_config.m_bUseNVPerfHud = reader.GetBool("openNVPerfHUD");
		}

		// ͬ����������
		if ( reader.Exist("charLimit") )
		{
			m_config.m_nCharLimit = reader.GetInt("charLimit");
		}

		// ����ÿ������Ļ���m_nCharAddCount����ɫ
		if ( reader.Exist("charAddCount") )
		{
			m_config.m_nCharAddCount = reader.GetInt("charAddCount");
		}

		// ��������
		if ( reader.Exist("texQuality") )
		{
			m_config.m_eTexQuality = (ETexQuality)reader.GetInt("texQuality");
		}

		// ȫ������ݼ���
		if ( reader.Exist("fsaa") )
		{
			m_config.m_eFSAA = (EFSAA)reader.GetInt("fsaa");
		}

		// ��Ӱ����
		if ( reader.Exist("shadowQuality") )
		{
			m_config.m_eShadowQuality = (EShadowQuality)reader.GetInt("shadowQuality");
		}

		// ģ������汾
        if (reader.Exist("meshFileVersion"))
        {
            m_config.m_fMeshFileVersion = reader.GetDouble("meshFileVersion");
        }

		// �Ƿ�����Ƶ�ģ���ļ�
        if (reader.Exist("binaryMeshFile"))
        {
            m_config.m_bBinaryMeshFile = reader.GetBool("binaryMeshFile");
        }

		// �Ƿ��Ӳ����Ƥ
        if (reader.Exist("hardwareSkeleton"))
        {
            m_config.m_bHardwareSkeleton = reader.GetBool("hardwareSkeleton");
        }

		// �Ƿ��Զ�����ģ��lod��Ϣ
		if (reader.Exist("AutoGenLod"))
		{
			m_config.m_bAutoGenLod = reader.GetBool("AutoGenLod");
		}

		// �Զ�����lod�Ĳ���
		if (reader.Exist("AutoLodLevelCount"))
		{
			m_config.m_nAutoLodLevelCount = reader.GetInt("AutoLodLevelCount");
		}

		// �Զ�����lod������( 0-���̶��Ķ���������ɾ��, 1-����������ɾ�� )
		if (reader.Exist("AutoLodType"))
		{
			m_config.m_byAutoLodType = (BYTE)reader.GetInt("AutoLodType");
		}

		// �Զ�����lod�Ĵ�����������( m_byAutoLodType = 0 ʱ,��ʾɾ���Ķ�����, m_byAutoLodType = 1ʱ, ��ʾɾ���ı��� )
		if (reader.Exist("AutoLodParm"))
		{
			m_config.m_fAutoLodParm = reader.GetDouble("AutoLodParm");
		}

		// �Զ�����lodʱ�Ƿ���ԭʼ����Ķ��㻺��
		if (reader.Exist("AutoLodShareVB"))
		{
			m_config.m_bAutoLodShareVB = reader.GetBool("AutoLodShareVB");
		}

		// ϵͳ����С֡��
		if (reader.Exist("MinFpsTor"))
		{
			m_config.m_fMinFpsTor = reader.GetDouble("MinFpsTor");
			m_config.m_fMaxFrameTimeTor = 1.0f / m_config.m_fMinFpsTor;
		}

		if (reader.Exist("LoadPlant"))
		{
			m_config.m_bLoadPlant = reader.GetBool("LoadPlant");
		}

        if ( reader.Exist("speedWind") )
        {
            strcpy(m_config.m_szSpeedWindFile, reader.GetString("speedWind"));
        }

        if (reader.Exist("UseBackgroundThread"))
        {
            m_config.m_bBackgroundThread = ( reader.GetBool("UseBackgroundThread") );
        }

        if (reader.Exist("NewTerrain"))
        {
            m_config.m_bNewTerrain = reader.GetBool("NewTerrain");
        }

		memset( m_config.m_szDefaultScene, 0, sizeof(m_config.m_szDefaultScene) );
		if (reader.Exist("DefaultScene") )
		{
			strcpy(m_config.m_szDefaultScene, reader.GetString("DefaultScene"));
		}

		if ( reader.Exist("EnableOC") )
		{
			m_config.m_bEnableOC = reader.GetBool("NewTerrain");
		}

		if ( reader.Exist("CharacterCollision") )
		{
			m_config.m_bCharacterCollision =reader.GetBool("CharacterCollision");
		}

        if (reader.Exist("ShaderPatch"))
        {
            strcpy(m_config.m_szShaderPatch, reader.GetString("ShaderPatch"));
        }

		if ( reader.Exist("ResourceDir") )
		{
			strcpy(m_config.m_szResourceDir, reader.GetString("ResourceDir"));
			//SetCurrentDirectoryA( m_config.m_szResourceDir );
			//char buf[256];
			//GetCurrentDirectoryA( 256, buf );
		}

        if (reader.Exist("Font"))
        {
            strcpy(m_config.m_szFont, reader.GetString("Font"));
        }

		if (reader.Exist("useVAO"))
		{
			m_config.m_useVAO = reader.GetBool("useVAO");
		}
		return true;
	}

    return false;
}


bool CEngine::Initialize(const char* szConfigFile)
{
	if ( szConfigFile == NULL )
	{
		// ��Ĭ����������ʼ������
		return Initialize( &m_config );
	}

	// ���������ļ�����ʼ������
	LoadCfg( szConfigFile );

	if ( false == Initialize( &m_config ) )
	{
		DT_TO_MSGBOX_FMT( LT_FATAL, "��ʼ��ʧ��! CEngine::Initialize()" );
		return false;
	}

	//if ( false == InitializeBeforStart() )
	//{
	//	DT_TO_MSGBOX_FMT( LT_FATAL, "Initialzie failed before application start!" );
	//	return false;
	//}

	return true;
}

// for editor
/*
bool CEngine::Initialize(HWND hWnd, const char* szConfigFile)
{
	if (szConfigFile == NULL)
	{
		// ��Ĭ����������ʼ������
		return Initialize(hWnd, &m_config);
	}

	// ���������ļ�����ʼ������
	LoadCfg( szConfigFile );

	if ( false == Initialize(hWnd, &m_config) )
	{
		DT_TO_MSGBOX_FMT( LT_FATAL, "��ʼ��ʧ��! CEngine::Initialize()" );
		return false;
	}

	if ( false == InitializeBeforStart() )
	{
		DT_TO_MSGBOX_FMT( LT_FATAL, "Initialzie failed before application start!" );
		return false;
	}

	return true;
}
*/

bool CEngine::Initialize(const LPAPPCONFIG pConfig)
{
#if AIR_TARGET_PLATFORM == AIR_PLATFORM_ANDROID || AIR_TARGET_PLATFORM == AIR_PLATFORM_IOS
	m_config.m_windowSize.m_nWidth = m_ViewWidth;
	m_config.m_windowSize.m_nHeight = m_ViewHeight;
#endif

    //���ȴ�������
  //  if (!CreateRenderWindow(pConfig))
  //  {
		//DT_TO_LOG( LT_FATAL, "��������ʧ�ܣ�CEngine::Initialize()" );
  //      DT_TO_MSGBOX_FMT( LT_FATAL, "��������ʧ�ܣ�" );
  //      return false;
  //  } 

    m_fFrameSpeed = 1.0f / pConfig->m_nMaxFramePerSecond;

    FixedString::SysInit();
    InitAllInternalFixedStrings();

	//������Դ·��
#if AIR_TARGET_PLATFORM == AIR_PLATFORM_WIN32
    FILESYSTEM->LoadXMLResource("../../../bin/ConfigFile/resources.xml");
#elif AIR_TARGET_PLATFORM == AIR_PLATFORM_ANDROID || AIR_TARGET_PLATFORM == AIR_PLATFORM_IOS
	std::string szXmlPath = GetResourcePath() + "/ConfigFile/resources_android.xml";
	FILESYSTEM->LoadXMLResource(szXmlPath.c_str());
#endif
	//FILESYSTEM->GenSubFolderArchive();

    m_bActive = true;

    InitializeBeforStart();
    return true;
}


void CEngine::SetFrameFunc(EngineCallback pFrameFun)
{
    m_pFrameFun = pFrameFun;
}

void CEngine::SetRenderer(IRenderer* pRenderer)
{
    m_pRenderer = pRenderer;
    
    if (m_pRenderer)
    {
        m_fpsCounter.Init();
        if (!m_bShaderCreated)
        {
            CreateDefaultShaders();
            m_bShaderCreated = true;
        }
    }
}

void CEngine::SetRenderFunc(EngineCallback pRenderFun)
{
    m_pRenderRun = pRenderFun;
}

void CEngine::SetProcQuitFunc(EngineCallback pProcQuitFun )
{
	m_pProcQuitFun = pProcQuitFun;
}

void CEngine::SetSceneManager(ISceneManager* pSceneMgr)
{
    m_pSceneMgr = pSceneMgr;
}


void CEngine::DoFrameUpdate( void )
{
	Render();
}


bool CEngine::InitializeBeforStart( void )
{
//#if AIR_TARGET_PLATFORM == AIR_PLATFORM_WIN32
    LoadPlugins();   //������������в��
//#endif

    TERRAIN->LoadConfigFile("terrain.cfg");  //������ʼ��ʼ��

	//MANUAL_GPU_DATA_PARAM_SOUCE_BUILDER_MGR->RegistorAllBuilder();
	//WeatherManager::RegisterManualGPUParamBuilder();

    //�ŵ��������
    //SPEEDTREE_ENGINE->LoadSpeedWind(m_config.m_szSpeedWindFile);

	return true;
}


void CEngine::Start()
{
#if AIR_PLATFORM_WIN32 == AIR_TARGET_PLATFORM
    MSG		msg; 
    BOOL	fMessage;

    while(m_bActive)
    {
        fMessage = PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE);
        if (msg.message == WM_QUIT)
        {
            break;
        }
        if(fMessage)
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else
        {
            Render();

			PROFILE_FRAME_COMPLETED;
        }
    }

	if ( m_pProcQuitFun )
	{
		m_pProcQuitFun();
	}
#else
	if( m_bActive )
	{
		Render();

		PROFILE_FRAME_COMPLETED;
	}
	else
	{
		if( m_pProcQuitFun )
			m_pProcQuitFun();
	}
#endif	
}

void CEngine::Update()
{
	PROFILE_FUNC(CEngine_Update);
    m_timer.UpdateTimer();
    m_fpsCounter.Update();

	AISInputDirector::GetInstance()->update();

    if ( m_pInputSystem )
    {
        m_pInputSystem->Update();
    }

    if (m_pFrameFun)
    {
        m_pFrameFun();
    }

    RESOURCE_MANAGER->Update(m_timer.GetTimeSinceLastFrame());

	//GEFFECT_MANAGER->UpdateAllEffect( m_timer.GetTimeSinceLastFrame() );

	ANIMATION_MANAGER->Update( m_timer.GetTimeSinceLastFrame() );

    //�������rendertarget
    //m_lstRenderTargets.clear();
    //m_pRenderer->DetachAllRenderTargets();

    m_bNeedRTBackbuffer = true;
		 
    ProcessInputEvent();

    LIGHTING_MANAGER->Update();

	PROFILE_SCOPE_START( SceneManager_UpdateScene );
	if (m_pSceneMgr)
	{
		//���³���
		m_pSceneMgr->UpdateScene();
		//�����oc
		if ( m_config.m_bEnableOC )
		{
			//ʹ��oc�����ڵ��޳�
			m_pSceneMgr->ExecOcclusionCull();
		}		
	}
	PROFILE_SCOPE_END( SceneManager_UpdateScene );

	WeatherManager::GetInstance()->Update( m_timer.GetTimeSinceLastFrame() );

    if (m_bDeferredLighting)
    {
   //     if (LIGHTING_MANAGER->GetRenderTargetGBuffer() && LIGHTING_MANAGER->GetLightTarget())
   //     {
   //         //AddRenderTarget(LIGHTING_MANAGER->GetRenderTargetGBuffer());
   //         //AddRenderTarget(LIGHTING_MANAGER->GetLightTarget());
			//m_pRenderer->AttachRenderTarget(LIGHTING_MANAGER->GetRenderTargetGBuffer());
			//m_pRenderer->AttachRenderTarget(LIGHTING_MANAGER->GetLightTarget());
   //     }
		//AddCustomRenderProcess(LIGHTING_MANAGER->GetDeferredLightingGBuffer());
		//AddCustomRenderProcess(LIGHTING_MANAGER->GetDeferredLightingLightBuffer());
    }
    else
    {
		/*
        if (LIGHTING_MANAGER->GetRenderTargetGBuffer() && LIGHTING_MANAGER->GetLightTarget())
        {
            //RemoveRenderTarget(LIGHTING_MANAGER->GetRenderTargetGBuffer());
            //RemoveRenderTarget(LIGHTING_MANAGER->GetLightTarget());
			m_pRenderer->DetachRenderTarget(LIGHTING_MANAGER->GetRenderTargetGBuffer());
			m_pRenderer->DetachRenderTarget(LIGHTING_MANAGER->GetLightTarget());
        }
		*/
		//RemoveCustomRenderProcess(LIGHTING_MANAGER->GetDeferredLightingGBuffer());
		//RemoveCustomRenderProcess(LIGHTING_MANAGER->GetDeferredLightingLightBuffer());
    }

    //shadowmapҪ�ŵ�lightprepass����������ã��������Ҫ��һ��
    //ԭ����rendertarget��lightprepass��clearscene����
	if (SHADOWMAP->IsEnable())
	{
		if (!SHADOWMAP->IsInitialized())
		{
			SHADOWMAP->Initialize(SHADOWMAP->GetShadowmapSize(), SHADOWMAP->GetShadowmapSize());
		}
	}

    if (m_pGUIFrameFun)
    {
        m_pGUIFrameFun();
    }

    
}

void CEngine::Render()
{
	if ( m_bPause )
	{
		SLEEP( 100 );
		return;
	}
	PROFILE_FUNC(CEngine_Render);
	////--------------------------------------------------
	////! add by hjj 2010-8-2
	//TIMEBASEPROFILER->ResetAll();
	////--------------------------------------------------

	bool needreset = m_pRenderer->GetNeedResetDevice();


	// �����ǰ����Ҫ�ֶ������豸
    m_pRenderer->CheckBeforeRender();



	//if (m_pCurRenderLoop)
	//{
	//	m_pCurRenderLoop->Render();
	//}

	{
		double fElapse = m_timer.GetTimeSinceLastFrame();
		if (fElapse < m_fFrameSpeed)
		{
			u32 dwTimeSleep = (u32)((m_fFrameSpeed - fElapse) * 1000);
			SLEEP(dwTimeSleep);
		}

		Update();

        //RenderAllRenderTargets();
		switch (m_curRenderMode)
		{
		case eERM_Default:
		case eERM_DeferredLighting:
			{
				//if (SHADOWMAP->IsEnable())   //�ŵ�������Ⱦ��ʵ��
				//{
				//	SHADOWMAP->Render();
				//}

				for (size_t i = 0; i < m_vtWindowRenderEvents.size(); ++i)
				{
					if (m_vtWindowRenderEvents[i]->IsActive())
					{
						m_vtWindowRenderEvents[i]->Render();
					}
				}
				//m_pRenderer->RenderAllRenderTargets();
				/*
				if (m_bPostProcessEnable)
				{
					POSTPROCESS_MANAGER->PostProcess();
					//RenderTarget* pRenderTargets[1] = { 0 };
					//pRenderTargets[0] = m_pRenderer->GetRenderWindow();
					//m_pRenderer->SetRenderTargets(1, pRenderTargets);

				}
#define USE_RENDER_CONTEXT
#ifndef USE_RENDER_CONTEXT
                RenderTarget* pTargets[1];
                pTargets[0] = m_pRenderer->GetRenderWindow();

                m_pRenderer->SetRenderTargets(1, pTargets);
#endif
				
				if (!m_bPostProcessEnable)
				{
					ExcuteRenderFunc();
				}
				else
				{
					POSTPROCESS_MANAGER->Render();
				}

				RenderActiveCompositor();
				//--------------------------------------------------	
				if (m_bPostProcessEnable)
				{
					//m_pRenderer->BeginScene();
					ExcuteGUIRender();
					//m_pRenderer->EndScene();
				}

				// ��present������ȡ��ÿ֡��Ⱦ�����
				m_pRenderer->GetRenderWindow()->Render();
				*/
			}
			break;
		}
	}


	////--------------------------------------------------
	////! add by hjj 2010-8-2
	//DT_TO_DBGSTR( LT_NONE, GetString("Vertex Cal: %d; Weight Cal: %d\n", TIMEBASEPROFILER->m_dwVertexCalCount, TIMEBASEPROFILER->m_dwWeightCount) );
	////--------------------------------------------------
}

void CEngine::ExcuteGUIRender()
{
    ////��ȾGUI
	m_pRenderer->BeginScene();
    if ( m_pGUIRenderer )
    {
        m_pGUIRenderer->BeginGUIRender();
        if (m_pGUIRenderFun)
        {
            m_pGUIRenderFun();
        }
        m_fpsCounter.Render();
        m_pGUIRenderer->EndGUIRender();
    }
    else
    {
        //m_pRenderer->BeginGUIRenderStates();
        m_fpsCounter.Render();
        //m_pRenderer->EndGUIRenderStates();
    }
	m_pRenderer->EndScene();
}
//
//void CEngine::DestroyAllRenderTarget( void )
//{
//	//���rendertarget��
//    /*
//	for (RenderTargetList_Iter it = m_lstRenderTargets.begin(); it != m_lstRenderTargets.end(); ++it)
//	{
//		RenderTarget* pRenderTarget = *it;
//		pRenderTarget->Clear();
//		SafeDelete(pRenderTarget);
//	}
//	m_lstRenderTargets.clear();
//    */
//    m_pRenderer->DestroyAllRenderTargets();
//}


void CEngine::Clear()
{
#if AIR_TARGET_PLATFORM == AIR_PLATFORM_WIN32
	DebugTools::ShutDebugConsoleWindow();
#endif
    TriggerEvent(Event_EngineClear, NULL, 0, 0);
	//��ճ���
	if (m_pSceneMgr)
	{
		m_pSceneMgr->ClearScene();
		SafeDelete(m_pSceneMgr);
	}

    if (m_pRTTCompositor)
    {
        m_pRTTCompositor->Clear();
        SafeDelete(m_pRTTCompositor);
    }

    INSTANCING_MANAGER->DestroyAllInstancingObjects();

    //m_pRenderer->DetachRenderTarget(SHADOWMAP);
    SHADOWMAP->Clear();

    //ClearPostScreenCompositor();

	for (size_t i = 0; i < m_vtWindowRenderEvents.size(); ++i)
	{
		m_vtWindowRenderEvents[i]->Clear();
		SafeDelete(m_vtWindowRenderEvents[i]);
	}
	m_vtWindowRenderEvents.clear();

    //POSTPROCESS_MANAGER->Clear();
	// ������Ļ�ϳ���
	SCREEN_COMPOSITOR_MANAGER->DestroyAllScreenCompositor();

	// ��ղ���
	MATERIAL_MANAGER->Clear();

	// �������е�RTT����Ϊ�ܶ�rendertarget�ɱ�ĵط��������Բ�������������
	//DestroyAllRenderTarget();
    BILLBOARD_RENDERER->Clear();

    //���δ������Ϣ
    //m_inputEventList.Clear();
    if ( m_pInputSystem )
    {
        m_pInputSystem->GetEventList()->Clear();
    }
    else
    {
        m_inputEventList.Clear();   
    }

    //���GUI

    //�������
    FONTMGR->Clear();

    //���ٹ��չ�����
    LIGHTING_MANAGER->Clear();

	// ���ٰ�Χ��ʵ��
    BoundingBoxInstance->Clear();

    //ANIMATIONMIX_MANAGER->Clear();

    RESOURCE_MANAGER->DestroyAllResources();

    ////��������ϵͳ��billboardset
    //if (CParticleSystem::m_pBillboardSet)
    //{
    //    CParticleSystem::m_pBillboardSet->Clear();
    //    SafeDelete(CParticleSystem::m_pBillboardSet);
    //}

    // ���ٹ���������
    BILLBOARDSET_MGR->Clear();

    // �����ļ�ϵͳ
    FILESYSTEM->Clear();

    //�����������
    TEXTURE_MANAGER->Clear();

    // ��ն����������Ժ��ǰ�����ŵ�renderer�����
    VERTEXDECL_MANAGER->Clear();

    GPUPROGRAM_MANAGER->Clear();

	// ���ٽű�ϵͳ
    if ( m_pScriptSystem )
    {
        m_pScriptSystem->Release();
        SafeDelete( m_pScriptSystem );
    }

#if AIR_TARGET_PLATFORM == AIR_PLATFORM_WIN32
    UnloadPlugins();
#endif

    // �����Ⱦ������Ⱦ������ͷ�
    //if (m_pRenderer)
    //{
    //    m_pRenderer->DestroyAllRenderTargets();
    //    m_pRenderer->Release();
    //    SafeDelete(m_pRenderer);
    //}

    //KillInstances();
    DestoryAllInternalFixedStrings();
    FixedString::SysRelease();
}

void CEngine::ProcessInputEvent()
{
    LPINPUTEVENT pEvent = NULL;
    if ( !m_pInputSystem )
    {
        pEvent = m_inputEventList.GetHead();
    }
    else
    {
        pEvent = m_pInputSystem->GetEventList()->GetHead();   
    }
    //LPINPUTEVENT pEvent = m_inputEventList.GetHead();
    //LPINPUTEVENT pEvent = m_pInputSystem->GetEventList()->GetHead();
    while(pEvent)
    {
        //GUI����
        bool bRes = false;
        if (m_pGUIEventFun)
        {
            bRes = m_pGUIEventFun(pEvent);
        }

        //����
        if (!bRes && m_pSceneMgr)
        {
            m_pSceneMgr->ProcessInputEvent(pEvent);
        }

        pEvent = pEvent->pNext;
    }
    //m_inputEventList.Clear();
    if( m_pInputSystem )
    {
        m_pInputSystem->GetEventList()->Clear();
    }
    else
    {
        m_inputEventList.Clear();
    }
    
}

void CEngine::SetFrameRate(unsigned nFrameRate)
{
    if (nFrameRate == 0)
    {
        nFrameRate = 10;
    }

    m_fFrameSpeed = 1.0f / nFrameRate;
}

#if AIR_TARGET_PLATFORM == AIR_PLATFORM_WIN32
void CEngine::ProcessIMEEvent(HWND hwnd, u32 msg, u32 wParam, u32 lParam)
{
    if (m_pIMEEventFun)
    {
        m_pIMEEventFun(hwnd, msg, wParam, lParam);
    }
}
/*
LRESULT CEngine::MsgProc(HWND hWnd, u32 uMsg, u32 wParam, u32 lParam)
{
    switch( uMsg )
    {
    case WM_CLOSE:
		{
			OnClose( wParam, lParam );
			PostQuitMessage(0);
		}
        return 0;

    case WM_MOUSEMOVE:
        OnMouseMove(LOWORD(lParam), HIWORD(lParam));
        break;

    case WM_LBUTTONDBLCLK:
        OnLButtonDBClick(LOWORD(lParam), HIWORD(lParam));
        break;

    case WM_LBUTTONDOWN:
        OnLButtonDown(LOWORD(lParam), HIWORD(lParam));
        break;

    case WM_LBUTTONUP:
        OnLButtonUp(LOWORD(lParam), HIWORD(lParam));
        break;

    case WM_RBUTTONDOWN:
        OnRButtonDown(LOWORD(lParam), HIWORD(lParam));
        break;

    case WM_RBUTTONUP:
        OnRButtonUp(LOWORD(lParam), HIWORD(lParam));
        break;

    case WM_MOUSEWHEEL:
        OnMouseWheel(HIWORD(wParam));
        break;

    case WM_KEYUP:
        OnKeyUp(wParam);
        break;

    case WM_KEYDOWN:
        OnKeyDown(wParam, lParam);
        break;

    case WM_CHAR:
        ProcessIMEEvent(hWnd, WM_CHAR, wParam, lParam);
        break;

    case WM_IME_SETCONTEXT:         // 0x0281
        //DT_TO_DBGSTR( LT_NONE, "WM_IME_SETCONTEXT:         // 0x0281\n" );
        ProcessIMEEvent(hWnd, WM_IME_SETCONTEXT, wParam, lParam);
        break;

    case WM_INPUTLANGCHANGEREQUEST: // 0x0050
        //DT_TO_DBGSTR( LT_NONE, "WM_INPUTLANGCHANGEREQUEST: // 0x0050\n" );
        ProcessIMEEvent(hWnd, WM_INPUTLANGCHANGEREQUEST, wParam, lParam);
        break;

    case WM_INPUTLANGCHANGE:        // 0x0051
        //DT_TO_DBGSTR( LT_NONE, "WM_INPUTLANGCHANGE:        // 0x0051\n" );
        ProcessIMEEvent(hWnd, WM_INPUTLANGCHANGE, wParam, lParam);
        break;

    case WM_IME_STARTCOMPOSITION:   // 0x010d
        //DT_TO_DBGSTR( LT_NONE, "WM_IME_STARTCOMPOSITION:   // 0x010d\n" );
        ProcessIMEEvent(hWnd, WM_IME_STARTCOMPOSITION, wParam, lParam);
        break;

    case WM_IME_ENDCOMPOSITION:     // 0x010e
        //DT_TO_DBGSTR( LT_NONE, "WM_IME_ENDCOMPOSITION:     // 0x010e\n" );
        ProcessIMEEvent(hWnd, WM_IME_ENDCOMPOSITION, wParam, lParam);
        break;

    case WM_IME_NOTIFY:             // 0x0282
        //DT_TO_DBGSTR( LT_NONE, "WM_IME_NOTIFY:             // 0x0282\n" );
        ProcessIMEEvent(hWnd, WM_IME_NOTIFY, wParam, lParam);
        break;

    case WM_IME_COMPOSITION:        // 0x010f
        //DT_TO_DBGSTR( LT_NONE, "WM_IME_COMPOSITION:        // 0x010f\n" );
        ProcessIMEEvent(hWnd, WM_IME_COMPOSITION, wParam, lParam);
        break;

	case WM_COPYDATA:
        //DT_TO_DBGSTR( LT_NONE, "WM_COPYDATA:         \n" );
		OnCopyData( wParam, lParam );
		break;

	case WM_SIZE:
		OnSize( wParam, lParam );
		break;
    default:
        if (uMsg >= WM_USER)
        {
            ProcessClientWindowMsg(uMsg, wParam, lParam);
        }
    }
    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}
*/

void CEngine::OnSize( u32 wParam, u32 lParam )
{
	switch (wParam)																
	{
		// ������С��?
	case SIZE_MINIMIZED:      
		break;	

		// �������
	case SIZE_MAXIMIZED:
        break;

	case SIZE_RESTORED:	    
		if ( LOWORD(lParam) != m_config.m_windowSize.m_nWidth || HIWORD(lParam) != m_config.m_windowSize.m_nHeight )
		{
			NeedResetDevice();

            u32 nWidth = LOWORD(lParam);
            u32 nHeight = HIWORD(lParam);
            TriggerEvent(Event_WindowResize, &nWidth, &nHeight, 0);
		}
		break;	
	}
}

void CEngine::OnCopyData(u32 wParam, u32 lParam)
{
	COPYDATASTRUCT* pCopyDataStruct = (COPYDATASTRUCT*)lParam; 
	DebugTools::OnCopyData( pCopyDataStruct );
}

LRESULT CEngine::ProcessClientWindowMsg(u32 uMsg, u32 wParam, u32 lParam)
{
	if (m_mapWindowMsgFun.find(uMsg) != m_mapWindowMsgFun.end()) 
		return m_mapWindowMsgFun[uMsg](wParam, lParam); 
	else 
		return -1;
}
#endif



void CEngine::OnClose( u32 wParam, u32 lParam )
{
#if AIR_TARGET_PLATFORM == AIR_PLATFORM_WIN32
	DebugTools::ShutDebugConsoleWindow();
#endif
}


/*
void CEngine::OnLButtonDown(int x, int y)
{
    if ( m_pInputSystem )
    {
        m_pInputSystem->GetEventList()->AddInputEvent(eINPUT_MOUSE_LBUTTONDOWN, 0, 0, 0, 0, x, y);
    }
    else
    {
        m_inputEventList.AddInputEvent(eINPUT_MOUSE_LBUTTONDOWN, 0, 0, 0, 0, x, y);
    }    
}

void CEngine::OnLButtonUp(int x, int y)
{
    if ( m_pInputSystem )
    {
        m_pInputSystem->GetEventList()->AddInputEvent(eINPUT_MOUSE_LBUTTONUP, 0, 0, 0, 0, x, y);
    }
    else
    {
        m_inputEventList.AddInputEvent(eINPUT_MOUSE_LBUTTONUP, 0, 0, 0, 0, x, y);
    }
}

void CEngine::OnRButtonDown(int x, int y)
{
    if ( m_pInputSystem )
    {
        m_pInputSystem->GetEventList()->AddInputEvent(eINPUT_MOUSE_RBUTTONDOWN, 0, 0, 0, 0, x, y);
    }
    else
    {
        m_inputEventList.AddInputEvent(eINPUT_MOUSE_RBUTTONDOWN, 0, 0, 0, 0, x, y);
    }
}

void CEngine::OnRButtonUp(int x, int y)
{
    if ( m_pInputSystem )
    {
        m_pInputSystem->GetEventList()->AddInputEvent(eINPUT_MOUSE_RBUTTONUP, 0, 0, 0, 0, x, y);
    }
    else
    {
        m_inputEventList.AddInputEvent(eINPUT_MOUSE_RBUTTONUP, 0, 0, 0, 0, x, y);
    }
}

void CEngine::OnLButtonDBClick(int x, int y)
{
    if ( m_pInputSystem )
    {
        m_pInputSystem->GetEventList()->AddInputEvent(eINPUT_MOUSE_LBUTTONDBCLICK, 0, 0, 0, 0, x, y);
    }
    else
    {
        m_inputEventList.AddInputEvent(eINPUT_MOUSE_LBUTTONDBCLICK, 0, 0, 0, 0, x, y);
    }
}

void CEngine::OnMouseMove(int x, int y)
{
    //int dx = x - m_lastMousePt.x;
    //int dy = y - m_lastMousePt.y;
	m_lastMousePt.x = x;
	m_lastMousePt.y = y;
    if ( m_pInputSystem )
    {
        m_pInputSystem->GetEventList()->AddInputEvent(eINPUT_MOUSE_MOVE, 0, 0, 0, 0, x, y);
    }
    else
    {
        m_inputEventList.AddInputEvent(eINPUT_MOUSE_MOVE, 0, 0, 0, 0, x, y);
    }
}

void CEngine::OnMouseWheel(s16 dz)
{
#if AIR_TARGET_PLATFORM == AIR_PLATFORM_WIN32
    if ( m_pInputSystem )
    {
        m_pInputSystem->GetEventList()->AddInputEvent(eINPUT_MOUSE_WHEEL, 0, 0, 0, dz / WHEEL_DELTA, 0, 0);
    }
    else
    {
        m_inputEventList.AddInputEvent(eINPUT_MOUSE_WHEEL, 0, 0, 0, dz / WHEEL_DELTA, 0, 0);
    }
#endif
}

void CEngine::OnKeyDown(int nKey, u32 lParam)
{
    if ( m_pInputSystem )
    {
        m_pInputSystem->GetEventList()->AddInputEvent(eINPUT_KEYDOWN, nKey, 0, 0, 0, 0, 0);
    }
    else
    {
        m_inputEventList.AddInputEvent(eINPUT_KEYDOWN, nKey, 0, 0, 0, 0, 0);
    }
}

void CEngine::OnKeyUp(int nKey)
{
    if ( m_pInputSystem )
    {
        m_pInputSystem->GetEventList()->AddInputEvent(eINPUT_KEYUP, nKey, 0, 0, 0, 0, 0);
    }
    else
    {
        m_inputEventList.AddInputEvent(eINPUT_KEYUP, nKey, 0, 0, 0, 0, 0);
    }
}
*/
void CEngine::SetGUIFrameFunc(EngineCallback pGUIFrameFun)
{
    m_pGUIFrameFun = pGUIFrameFun;
}

void CEngine::SetGUIRenderFunc(EngineCallback pGUIRenderFun)
{
    m_pGUIRenderFun = pGUIRenderFun;
}

void CEngine::SetGUIEventFunc(EventCallBack pGUIEventFun)
{
    m_pGUIEventFun = pGUIEventFun;
}

#if AIR_TARGET_PLATFORM == AIR_PLATFORM_WIN32
void CEngine::SetIMEEventFunc(ImeEventCallBack pImeEventFun)
{
    m_pIMEEventFun = pImeEventFun;
}

//ע��windows��Ϣ������
void CEngine::RegisterClientWindowMsgFunc(u32 uMsg, ClientWindowMsgCall pFun)
{
	m_mapWindowMsgFun[uMsg] = pFun;
}
#endif

void CEngine::Stop()
{
    m_bActive = false;
#if AIR_TARGET_PLATFORM == AIR_PLATFORM_WIN32
	::PostMessage( m_hWnd, WM_QUIT, 0, 0 );
#endif
}

void CEngine::SetBackgroundColor(const SColor& color)
{
    m_clrBackground = color;
    if (m_pRenderer)
    {
        m_pRenderer->SetBackGroundColor(color);
    }
}
/*
void CEngine::KillInstances()
{
    //CFileSystem::KillSelf();
    //CModelManager::KillSelf();
    //CRenderableFactory::KillSelf();
    //CModelInstanceManager::KillSelf();
    CParticleSystemManager::KillSelf();
    CFontManager::KillSelf();
    //CMaterialManager::KillSelf();
    //CTerrain::KillSelf();
    //CParticleAffectorFactory::KillSelf();
    //CModelManager::KillSelf();
    //CParticleEmitterFactory::KillSelf();
}
*/



//��ú󻺳��С
void CEngine::GetBackBufferSize(int& nWidth, int& nHeight) const
{
    m_pRenderer->GetBackBufferSize(nWidth, nHeight);
}




void CEngine::NeedResetDevice()
{
    if (m_pRenderer)
    {
        m_pRenderer->NeedResetDevice();
    }
    
    //m_bNeedResetDevice = true;
}


void CEngine::SetGUIRenderer( IGUIRenderer* pGUIRenderer )
{
	m_pGUIRenderer = pGUIRenderer;
}

/*
void				CEngine::QuitProgram( void )
{
	m_bActive = false;
	::PostMessage( m_hWnd, WM_QUIT, 0, 0 );
}
*/

void CEngine::SetScriptSystem( IScriptSystem* pScriptSys )
{
    m_pScriptSystem = pScriptSys;
}

//void CEngine::RenderAllRenderTargets()
//{
//    /*
//    RenderTargetList_Iter itEnd = m_lstRenderTargets.end();
//    RenderTarget* pTarget = NULL;
//    for (RenderTargetList_Iter it = m_lstRenderTargets.begin(); it != itEnd; ++it)
//    {
//        pTarget = *it;
//        //m_pRenderer->SetRenderTarget(0, pTarget->GetRenderTexture());
//		if ( pTarget->GetIsWorkOnCurFrame() )
//		{
//			pTarget->Render();
//		}
//    }
//    */
//    m_pRenderer->RenderAllRenderTargets();
//}
/*
void CEngine::AddRenderTarget(RenderTarget* pRenderTarget)
{
    m_pRenderer->AttachRenderTarget(pRenderTarget);
}

void CEngine::DestroyRenderTarget(RenderTarget* pRenderTarget)
{
    m_pRenderer->DestroyRenderTarget(pRenderTarget);
}

void CEngine::RemoveRenderTarget(RenderTarget* pRenderTarget)
{
    m_pRenderer->DetachRenderTarget(pRenderTarget);
}
*/
//RenderTarget* CEngine::GetRenderTarget(const AIRString& strName)
//{
//    /*
//    RenderTargetList_Iter itEnd = m_lstRenderTargets.end();
//    for (RenderTargetList_Iter it = m_lstRenderTargets.begin(); it != itEnd; ++it)
//    {
//        if ((*it)->GetName() == strName)
//        {
//            return *it;
//        }
//    }
//    
//    return NULL;
//    */
//    return m_pRenderer->GetRenderTarget(strName);
//}

void CEngine::SetNeedRTBackbuffer(bool bNeed)
{
    m_bNeedRTBackbuffer = bNeed;
}

void CEngine::AddActiveCompositor(ScreenCompositor* pCompositor)
{
    m_lstActiveCompositor.push_back(pCompositor);
}

void CEngine::RemoveActiveCompositor(ScreenCompositor* pCompositor)
{
    CompositorList_Iter it = std::find(m_lstActiveCompositor.begin(), m_lstActiveCompositor.end(), pCompositor);
    if (it != m_lstActiveCompositor.end())
    {
        m_lstActiveCompositor.remove(pCompositor);
    }
}
/*
void CEngine::RenderActiveCompositor()
{
    if (!m_lstActiveCompositor.empty())
    {
        m_pRenderer->BeginScene();
        CompositorList_Iter itEnd = m_lstActiveCompositor.end();
        for (CompositorList_Iter it = m_lstActiveCompositor.begin(); it != itEnd; ++it)
        {
            if ((*it)->Prepare())
            {
                (*it)->Render(NULL);
            }

        }
        m_pRenderer->EndScene();
    }
    
}
*/
void CEngine::EnablePostProcess(bool bEnable)
{
    m_bNeedRTBackbuffer = !bEnable;
    m_bPostProcessEnable = bEnable;
    /*
    if (bEnable && m_pPostScreenCompositor == NULL)
    {
        CreatePostScreenCompositor();
    }
    if (m_pPostScreenCompositor)
    {
        m_pPostScreenCompositor->SetActive(bEnable);
    }
    */
}

//void CEngine::CreatePostScreenCompositor()
//{
//    /*
//    m_pPostScreenCompositor = static_cast<PostScreenCompositor*>(SCREEN_COMPOSITOR_MANAGER->CreateScreenCompositor(POST_SCREEN_COMPOSITOR, "PostScreenCompositor"));
//    int nWidth, nHeight;
//    GetBackBufferSize(nWidth, nHeight);
//    m_pPostScreenCompositor->Create(0, 0, nWidth, nHeight);
//    m_pPostScreenCompositor->SetPostScreenType(ePSCT_PostScene);
//    */
//}
//
//void CEngine::ClearPostScreenCompositor()
//{
//    POSTPROCESS_MANAGER->Clear();
//    /*
//    if (m_pPostScreenCompositor)
//    {
//        SCREEN_COMPOSITOR_MANAGER->DestoryScreenCompositor(m_pPostScreenCompositor);
//    }
//    */
//}

void CEngine::InitializeShadowmap()
{
    /*
    if (m_pShadowmap == NULL)
    {
        m_pShadowmap = new Shadowmap;
    }

    if (!m_pShadowmap->IsInitialized())
    {
        m_pShadowmap->Initialize(1024, 1024);
    }
    */
    if (!SHADOWMAP->IsInitialized())
    {
        SHADOWMAP->Initialize(1024, 1024);
    }
    
}

void CEngine::EnableDeferredLighting(bool bDeferredLightingEnable)
{
    if (bDeferredLightingEnable)
    {
        IRenderer* pRenderer = ENGINE_INST->GetRenderer();
        if (!pRenderer->IsSupportMultiRenderTarget())
        {
            bDeferredLightingEnable = false;
        }
    }
    LIGHTING_MANAGER->EnableDeferredLighting(bDeferredLightingEnable);
    if (m_bDeferredLighting != bDeferredLightingEnable)
    {
        m_bDeferredLighting = bDeferredLightingEnable;
        
        //m_pSceneMgr->ResetAllRenderablesTechnique();
    }

    ENGINE_INST->TriggerEvent(Event_ResetTechnique, NULL, 0, 0);
}

void CEngine::SetShadowmapEnable(bool bShadowmapEnable)
{
	/*
    if (m_bShadowmapEnable != bShadowmapEnable)
    {
        m_bShadowmapEnable = bShadowmapEnable;
        ISceneManager* pScene = ENGINE_INST->GetSceneMgr();
        //����Ҫ������shadowmap���ſ���ѡ��technique����ΪtechniqueҪ����shadowmap
        if (m_bShadowmapEnable && pScene)
        {
            //SHADOWMAP->InitializeShadowmap();
            if (SHADOWMAP->IsInitialized())
            {
                SHADOWMAP->Initialize(1024, 1024);
            }
            CLight* pLight = pScene->GetSunLight();
            SHADOWMAP->SetLightDirection(pLight->GetDirection());
            SHADOWMAP->SetLightPosition(pLight->GetPosition());
        }

       
        m_pSceneMgr->ResetAllRenderablesTechnique();

        ENGINE_INST->TriggerEvent(Event_ResetTechnique, NULL);
    }
    */
	if (bShadowmapEnable != SHADOWMAP->IsEnable())
	{
		SHADOWMAP->SetEnable(bShadowmapEnable);
		if (!SHADOWMAP->IsInitialized())
		{
			SHADOWMAP->Initialize(1024, 1024);
			ISceneManager* pScene = ENGINE_INST->GetSceneMgr();
			CLight* pLight = pScene->GetSunLight();
			SHADOWMAP->SetLightDirection(pLight->GetDirection());
			SHADOWMAP->SetLightPosition(pLight->GetPosition());
		}

		//m_pSceneMgr->ResetAllRenderablesTechnique();

		ENGINE_INST->TriggerEvent(Event_ResetTechnique, NULL, 0, 0);
	}
	
}

bool CEngine::IsShadowmapEnable() const
{
	return SHADOWMAP->IsEnable();
}

void CEngine::CreateDefaultShaders()
{
    

    GPUPROGRAM_MANAGER->CreateDefaultShaders(m_config.m_szShaderPatch);

    //GPUPROGRAM_MANAGER->CreateAllShaders();
}

void CEngine::SetRTTCompositorEnable(bool bEnable)
{
    m_bRTTCompositorEnable = bEnable;
    if (m_bRTTCompositorEnable)
    {
        if (m_pRTTCompositor == NULL)
        {
            m_pRTTCompositor = new ScreenCompositor();
            int nWidth, nHeight;
            GetBackBufferSize(nWidth, nHeight);
            m_pRTTCompositor->Create( ePPT_None, nWidth - 256, 0, 256, 256);
        }

        AddActiveCompositor(m_pRTTCompositor);
    }
    else
    {
        if (m_pRTTCompositor)
        {
            RemoveActiveCompositor(m_pRTTCompositor);
        }
    }
}
/*
void CEngine::SetRTTCompositorTexture(ITexture* pTexture)
{
    if (m_pRTTCompositor == NULL)
    {
        m_pRTTCompositor = new ScreenCompositor();
        int nWidth, nHeight;
        GetBackBufferSize(nWidth, nHeight);
        m_pRTTCompositor->Create(nWidth - 256, 0, 256, 256);
    }

    IMaterial* pMaterial = m_pRTTCompositor->GetMaterial();
    if (pMaterial)
    {
        if (!pMaterial->IsReady())
        {
            pMaterial->Cache();
        }
        //pTexture->AddRef();
        //pMaterial->SetTextureUnitTex(0, pTexture);
		m_pRTTCompositor->SetTexture(pTexture);
    }
}
*/
void CEngine::LoadPlugins()
{
	unsigned long hwnd = 0;

#if AIR_TARGET_PLATFORM == AIR_PLATFORM_WIN32

#ifdef _DEBUG
    std::ifstream stream("../../../bin/ConfigFile/plugin_d.txt");
#else
    std::ifstream stream("../../../bin/ConfigFile/plugin.txt");
#endif
	char szLine[256] = { 0 };
	while (!stream.eof())
	{
		stream.getline(szLine, sizeof(szLine));
		if (szLine[0] == '#')
		{
			continue;
		}

		PluginDLL* pPlugin = new PluginDLL(szLine);
		m_vecPlugins.push_back(pPlugin);
	}

	hwnd = (unsigned long)m_hWnd;

#elif AIR_TARGET_PLATFORM == AIR_PLATFORM_ANDROID || AIR_TARGET_PLATFORM == AIR_PLATFORM_IOS

    hwnd = (unsigned long)m_hWnd;
	CreateGLES2RenderSystem();

#endif
    
	AISInputDirector::GetInstance()->initialise( hwnd );

}

void CEngine::UnloadPlugins()
{
	AISInputDirector::GetInstance()->shutdown();

#if AIR_TARGET_PLATFORM == AIR_PLATFORM_WIN32

    for (PluginsVector::reverse_iterator it = m_vecPlugins.rbegin(); it != m_vecPlugins.rend(); ++it)
    {
        PluginDLL* pPlugin = *it;
        SafeDelete(pPlugin);
    }

    m_vecPlugins.clear();

#elif AIR_TARGET_PLATFORM == AIR_PLATFORM_ANDROID || AIR_TARGET_PLATFORM == AIR_PLATFORM_IOS

	DestroyGLESRenderSystem();

#endif
}

void CEngine::RegistEventListener(EventListenerType type, EventListener* pListener)
{
    EventListenerArray::iterator it = std::find(m_vecEventListeners[type].begin(), m_vecEventListeners[type].end(), pListener);

    if (it == m_vecEventListeners[type].end())
    {
        m_vecEventListeners[type].push_back(pListener);
    }
}

void CEngine::RemoveEventListener(EventListener* pListener)
{
    for (int i = 0; i < Event_Max; ++i)
    {
        EventListenerArray::iterator it = std::find(m_vecEventListeners[i].begin(), m_vecEventListeners[i].end(), pListener);

        if (it != m_vecEventListeners[i].end())
        {
            m_vecEventListeners[i].erase(it);
        }
    }
}

void CEngine::TriggerEvent(EventListenerType type, void* pParam1, void* pParam2, void* pParam3)
{
    for (size_t i = 0; i < m_vecEventListeners[type].size(); ++i)
    {
        m_vecEventListeners[type][i]->OnEvent(type, pParam1, pParam2, pParam3);
    }
}

void CEngine::SetActiveCamera(CCamera* pCamera)
{
    m_pActiveCamera = pCamera;

    if (m_pActiveCamera)
    {
        m_pRenderer->SetProjectionTransform(&(m_pActiveCamera->GetProjectionMatrix()));
        m_pRenderer->SetViewTransform(&(m_pActiveCamera->GetViewMatrix()));
        CGPUDataParamSource* pGPUData = m_pRenderer->GetGPUDataParam();
        pGPUData->SetCameraPosition(m_pActiveCamera->GetPosition());
    }
}

void CEngine::ExcuteRenderFunc()
{
	
	if (m_pRenderRun && !m_bPostProcessEnable)
	{
		//u32 dwTimeStart = timeGetTime();
		m_pRenderRun();
	}
	//m_pRenderer->BeginScene();
	ExcuteGUIRender();
	//m_pRenderer->EndScene();
}

void CEngine::AddWindowRenderProcess(WindowRenderProcess* pRenderProcess)
{
	if (!pRenderProcess)
	{
		return;
	}
	bool bCanAdd = true;
	for (size_t i = 0; i < m_vtWindowRenderEvents.size(); ++i)
	{
		if (m_vtWindowRenderEvents[i] == pRenderProcess)
		{
			bCanAdd = false;
			break;
		}
	}

	if (bCanAdd)
	{
		m_vtWindowRenderEvents.push_back(pRenderProcess);
	}
}

void CEngine::RemoveWindowRenderProcess(WindowRenderProcess* pRenderProcess)
{
	for (WindowRenderProcessVector::iterator it = m_vtWindowRenderEvents.begin(); it != m_vtWindowRenderEvents.end();)
	{
		if (*it == pRenderProcess)
		{
			it = m_vtWindowRenderEvents.erase(it);
		}
		else
		{
			++it;
		}
	}
}

WindowRenderProcess* CEngine::GetWindowRenderProcess(const char* szName)
{
	for (size_t i = 0; i < m_vtWindowRenderEvents.size(); ++i)
	{
		if (strcmp(m_vtWindowRenderEvents[i]->GetName().c_str(), szName) == 0)
		{
			return m_vtWindowRenderEvents[i];
		}
	}

	return NULL;
}

void CEngine::SetEngineRenderMode(EngineRenderMode mode)
{
	m_curRenderMode = mode;
	switch (mode)
	{
	case eERM_Default:
		break;
	}
}

/*
void CEngine::AddRenderWindow(RenderTarget* pWindow)
{
	m_vtRenderWindows.push_back(pWindow);
}

void CEngine::RemoveRenderWindow(RenderTarget* pWindow)
{

}
*/