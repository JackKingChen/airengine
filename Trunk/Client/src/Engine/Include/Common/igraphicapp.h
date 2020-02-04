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
    // Qualifier: 创建应用程序窗口和渲染设备。
    // Parameter: const LPAPPCONFIG pConfig
    //************************************
    //virtual bool Create(const LPAPPCONFIG pCreationConfig, WNDPROC wndProcFunc = NULL);

    //virtual bool CreateWithoutWindow(const LPAPPCONFIG pCreationConfig);

    //************************************
    // Method:    Close
    // FullName:  IGraphicApp::Close
    // Access:    virtual public 
    // Returns:   void
    // Qualifier: 关闭应用程序，释放所有对象与销毁窗口。
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

    //必须要实现
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

    //创建全局的材质
    //virtual bool CreateGolbalMaterial();

    

    //从文件读取场景，暂时创建一个默认的场景
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
    //IRenderer* m_pRenderDriver;    //渲染器
    ISceneManager* m_pSceneMgr;    //场景
    //HWND       m_hWnd;             //窗口句柄
    //HINSTANCE  m_hInst;            //实例句柄
    //Dimension2Di m_wndSize;        //窗口大小

    //bool       m_bFullScreen;      //是否为全屏

    //CFPSCounter m_fpsCounter;

    

    //float       m_fFrameSpeed;   //帧速

    //CInputEventList m_inputEventList;

    //POINT m_lastMousePt;

    //Matrix4f   m_matOrthoLH;       //正交矩阵
    //Matrix4f   m_matPerspetiveLH;  //透视投影

    CreateRendererFunc m_pCreateRenderFunc;
private:
};

