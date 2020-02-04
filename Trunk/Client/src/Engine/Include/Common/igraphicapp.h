#pragma once 


#include "Engine.h"
#include "ISceneManager.h"
//#include "D3D9RenderSystem.h"

//#pragma comment(lib, "D3D9RenderSystemD.lib")
#if AIR_TARGET_PLATFORM == AIR_PLATFORM_WIN32
typedef IRenderer* (*CreateRendererFunc)(HINSTANCE);
#else
typedef IRenderer* (*CreateRendererFunc)();
#endif

class IGraphicApp
{
    
public:
    IGraphicApp() : m_pCreateRenderFunc(NULL) {};
    virtual ~IGraphicApp() {};

    virtual bool Setup(/*HINSTANCE hInst*/) 
    {
#if AIR_TARGET_PLATFORM == AIR_PLATFORM_WIN32
        if (!ENGINE_INST->Initialize("../../ConfigFile/engine.cfg"))
#else
		AIRString cfgPath = ENGINE_INST->GetResourcePath() + "/ConfigFile/engine.cfg";
		if (!ENGINE_INST->Initialize(cfgPath.c_str()))
#endif
        {
            return false;
        }

        /*
        IRenderer* pRenderer = Create3DEnviroment(hInst);
        if (pRenderer == NULL)
        {
            ENGINE_INST->Clear();
            return false;
        }
        ENGINE_INST->SetRenderer(pRenderer);
        */

        ISceneManager* pScene = CreateScene();
        ENGINE_INST->SetSceneManager(pScene);
        return true;
    }

    //************************************
    // Method:    Create
    // FullName:  IGraphicApp::Create
    // Access:    public 
    // Returns:   bool
    // Qualifier: ����Ӧ�ó��򴰿ں���Ⱦ�豸��
    // Parameter: const LPAPPCONFIG pConfig
    //************************************
    //virtual bool Create(const LPAPPCONFIG pCreationConfig, WNDPROC wndProcFunc = NULL);

    //virtual bool CreateWithoutWindow(const LPAPPCONFIG pCreationConfig);

    //************************************
    // Method:    Close
    // FullName:  IGraphicApp::Close
    // Access:    virtual public 
    // Returns:   void
    // Qualifier: �ر�Ӧ�ó����ͷ����ж��������ٴ��ڡ�
    //************************************
    virtual void Close() 
    {
        ENGINE_INST->Stop();
    };

    virtual s32 Run()
    {
        ENGINE_INST->Start();

        return 0;
    }

    virtual bool Render() { return false; };

    virtual bool Update() { return false; };

    //����Ҫʵ��
    virtual ISceneManager* CreateScene() = 0;

    //virtual void SetSceneManager(ISceneManager* pSceneMgr);

    //virtual IRenderer* GetRenderer() { return m_pRenderDriver; }

    //virtual Dimension2Di GetWindowDimension() { return m_wndSize; }

    //virtual LRESULT    MsgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
    void SetCreateRendererFunc(CreateRendererFunc pFunc)
    {
        m_pCreateRenderFunc = pFunc;
    }

protected:
    //virtual bool CreateAppWindow(const LPAPPCONFIG pCreationConfig, WNDPROC wndProcFunc = NULL);
    //virtual IRenderer* Create3DEnviroment(HINSTANCE hInst)
    //{
    //    /*
    //    IRenderer* pRenderer = NULL;
    //    if (nDeviceType == DRIVER_D3D9)
    //    {
    //        pRenderer = CreateD3D9RenderSystem(hInst);
    //    }

    //    return pRenderer;
    //    */
    //    if (m_pCreateRenderFunc)
    //    {
    //        return m_pCreateRenderFunc(hInst);
    //    }
    //    return NULL;
    //}
    //virtual bool CreateInputSystem();
    

    //virtual bool UpdateSetting();

    //����ȫ�ֵĲ���
    //virtual bool CreateGolbalMaterial();

    

    //���ļ���ȡ��������ʱ����һ��Ĭ�ϵĳ���
    //virtual ISceneManager* CreateScene(const char* szFilename) = 0;

    /*
    virtual void ProcessInputEvent();

    virtual void OnLButtonDown(int x, int y);

    virtual void OnRButtonDown(int x, int y);

    virtual void OnLButtonUp(int x, int y);

    virtual void OnRButtonUp(int x, int y);

    virtual void OnMouseMove(int x, int y);

    virtual void OnMouseWheel(short dz);

    virtual void OnKeyDown(int nKey, UINT lParam);

    virtual void OnKeyUp(int nKey);
    */
    
    //IInput* m_pInput;
    //IRenderer* m_pRenderDriver;    //��Ⱦ��
    ISceneManager* m_pSceneMgr;    //����
    //HWND       m_hWnd;             //���ھ��
    //HINSTANCE  m_hInst;            //ʵ�����
    //Dimension2Di m_wndSize;        //���ڴ�С

    //bool       m_bFullScreen;      //�Ƿ�Ϊȫ��

    //CFPSCounter m_fpsCounter;

    

    //float       m_fFrameSpeed;   //֡��

    //CInputEventList m_inputEventList;

    //POINT m_lastMousePt;

    //Matrix4f   m_matOrthoLH;       //��������
    //Matrix4f   m_matPerspetiveLH;  //͸��ͶӰ

    CreateRendererFunc m_pCreateRenderFunc;
private:
};

