// ***************************************************************
//  文件: RenderTarget.h
//  日期: 2010-3-22
//  作者: liangairan
//  说明: RenderTarget类，主要用于渲染到target
// ***************************************************************
#pragma once 

#include "ITexture.h"
#include "Camera.h"
#include "RenderQueue.h"
#include "RTTI.h"

typedef void (*RTTCallBackFunc)(void);


const u8 RTT_PRIORITY_TEX = 1;
const u8 RTT_PRIORITY_SWAPWINDOW = 3;
const u8 RTT_PRIORITY_DEFAULT = 4;

class EngineExport RenderTarget //: public EngineAllocator(RenderTarget)
{
	DECLARE_ROOT_RTTI;
	EngineAllocator(RenderTarget)
public:
    RenderTarget();
    virtual ~RenderTarget();

    //开始渲染
    virtual void Render() {}

    //初始化
    //@nWidth   宽
    //@nHeight  高
    virtual bool Initialize(int nWidth, int nHeight);


	// 获得rtt纹理
    virtual ITexture* GetTexture()
    {
        return NULL;
    }

    // 每帧更新
    //virtual void Update();

	// 释放资源
    virtual void Clear();
    
	// 获得rtt名称
    const AIRString&	GetName() const
    {
        return m_strName;
    }

	// 设置rtt名称
    void SetName(const AIRString& strName)
	{
		m_strName = strName;
	}

	// 获得rtt的索引
    u32 GetIndex()
    {
        return m_nIndex;
    }

	// 设置背景色
	void SetBackgroundColor( const SColor& col )
	{
		m_clrBackGround = col;
	}

	// 获取背景色
    SColor GetBackgroundColor() const
    {
        return m_clrBackGround;
    }

	virtual void* GetDepthSurface()
	{
		return NULL;
	}

	virtual void* GetPixelSurface()
	{
		return NULL;
	}
    virtual int GetWidth() const = 0;

    virtual int GetHeight() const = 0;

    //设置索引
    void SetIndex(u32 nIndex)
    {
        m_nIndex = nIndex;
    }

	/*
    //获得优先级
    u8 GetPriority() const
    {
        return m_Priority;
    }

    //设置优先级
    void SetPriority(u8 priority)
    {
        m_Priority = priority;
    }

    virtual void* GetPixelSurface();

    void SetFuncRender(RTTCallBackFunc func)
    {
        m_funcRender = func;
    }

	virtual ITexture* GetDepthTexture()
	{
		return NULL;
	}
	*/

	void Resize(u32 nWidth, u32 nHeight);

	bool IsUseDepthSurface() const
	{
		return m_bUseDepthSurface;
	}

	virtual bool IsWindow() const
	{
		return false;
	}

	bool IsActive() const
	{
		return m_bActive;
	}

	void SetActive(bool bActive)
	{
		m_bActive = bActive;
	}

	void SetUseDepthSurface(bool bUseDepth)
	{
		m_bUseDepthSurface = bUseDepth;
	}
protected:
    //CCamera*			m_pCamera;				//摄像机
    //ITexture*			m_pRenderTexture;		//被渲染的纹理
    AIRString			m_strName;				//名字
    u32					m_nIndex;				//渲染序号
    SColor				m_clrBackGround;		//背景颜色
	RTTCallBackFunc     m_funcRender;           //渲染回调函数
	bool                m_bUseDepthSurface;     //是否使用深度缓冲就个
	bool m_bActive;
	/*
    //CRenderQueue		m_renderQueue;			//渲染队列
	bool				m_bIsWorkOnCurFrame;	//当前帧是否需要渲染到rtt
	RTTCallBackFunc		m_funcBeforeRender;		//渲染到RTT的预处理回调函数
	RTTCallBackFunc		m_funcAfterRender;		//渲染到RTT后的回调函数
    RTTCallBackFunc     m_funcRender;           //渲染回调函数
    bool                m_bUseDepthSurface;     //是否使用深度缓冲
	bool				m_bRenderSceneToRTT;	//画场景内容到还是画RTT的渲染队列
    //bool  m_bDeleteCamera;

    u8   m_Priority;   //优先级
	*/
};


class EngineExport TextureRenderTarget : public RenderTarget
{
	DECLARE_RTTI;
public:
	TextureRenderTarget();
	virtual ~TextureRenderTarget();

	// 通过引用计数释放资源
	//virtual s32 Release();

	// 强行清空纹理
	virtual void Clear();

	virtual bool Cache()
	{
		return true;
	}

	virtual bool Create(const char* szName, u32 nWidth, u32 nHeight, RendererPixelFormat pixelFormat);

	// 获取背景色
	SColor GetBackgroundColor() const
	{
		return m_clrBackGround;
	}

	bool IsFullScreen() const
	{
		return m_bFullScreen;
	}

	void SetFullScreen(bool bFullScreen)
	{
		m_bFullScreen = bFullScreen;
	}

	// 获得rtt的索引
	u32 GetIndex()
	{
		return m_nIndex;
	}

	void SetIndex(u32 nIndex)
	{
		m_nIndex = nIndex;
	}

	ITexture* GetTexture()
	{
		return m_pTexture;
	}

	bool SaveToFile(const char* szFile);

	static TextureRenderTarget* CreateRenderTarget(const char* szName, u32 nWidth, u32 nHeight, RendererPixelFormat pixelFormat);

	virtual int GetWidth() const
	{
		return m_pTexture != NULL ? m_pTexture->GetWidth() : 0;
	}

	virtual int GetHeight() const
	{
		return m_pTexture != NULL ? m_pTexture->GetHeight() : 0;
	}
protected:
	//bool   m_bUseDepthSurface;
	SColor m_clrBackGround;		//背景颜色
	bool   m_bFullScreen;
	float  m_fScreenRatio;      //与屏幕的比例
	u32 m_nIndex;
	ITexture* m_pTexture;
};
