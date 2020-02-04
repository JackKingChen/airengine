// ***************************************************************
//  �ļ�: RenderTarget.h
//  ����: 2010-3-22
//  ����: liangairan
//  ˵��: RenderTarget�࣬��Ҫ������Ⱦ��target
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

    //��ʼ��Ⱦ
    virtual void Render() {}

    //��ʼ��
    //@nWidth   ��
    //@nHeight  ��
    virtual bool Initialize(int nWidth, int nHeight);


	// ���rtt����
    virtual ITexture* GetTexture()
    {
        return NULL;
    }

    // ÿ֡����
    //virtual void Update();

	// �ͷ���Դ
    virtual void Clear();
    
	// ���rtt����
    const AIRString&	GetName() const
    {
        return m_strName;
    }

	// ����rtt����
    void SetName(const AIRString& strName)
	{
		m_strName = strName;
	}

	// ���rtt������
    u32 GetIndex()
    {
        return m_nIndex;
    }

	// ���ñ���ɫ
	void SetBackgroundColor( const SColor& col )
	{
		m_clrBackGround = col;
	}

	// ��ȡ����ɫ
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

    //��������
    void SetIndex(u32 nIndex)
    {
        m_nIndex = nIndex;
    }

	/*
    //������ȼ�
    u8 GetPriority() const
    {
        return m_Priority;
    }

    //�������ȼ�
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
    //CCamera*			m_pCamera;				//�����
    //ITexture*			m_pRenderTexture;		//����Ⱦ������
    AIRString			m_strName;				//����
    u32					m_nIndex;				//��Ⱦ���
    SColor				m_clrBackGround;		//������ɫ
	RTTCallBackFunc     m_funcRender;           //��Ⱦ�ص�����
	bool                m_bUseDepthSurface;     //�Ƿ�ʹ����Ȼ���͸�
	bool m_bActive;
	/*
    //CRenderQueue		m_renderQueue;			//��Ⱦ����
	bool				m_bIsWorkOnCurFrame;	//��ǰ֡�Ƿ���Ҫ��Ⱦ��rtt
	RTTCallBackFunc		m_funcBeforeRender;		//��Ⱦ��RTT��Ԥ����ص�����
	RTTCallBackFunc		m_funcAfterRender;		//��Ⱦ��RTT��Ļص�����
    RTTCallBackFunc     m_funcRender;           //��Ⱦ�ص�����
    bool                m_bUseDepthSurface;     //�Ƿ�ʹ����Ȼ���
	bool				m_bRenderSceneToRTT;	//���������ݵ����ǻ�RTT����Ⱦ����
    //bool  m_bDeleteCamera;

    u8   m_Priority;   //���ȼ�
	*/
};


class EngineExport TextureRenderTarget : public RenderTarget
{
	DECLARE_RTTI;
public:
	TextureRenderTarget();
	virtual ~TextureRenderTarget();

	// ͨ�����ü����ͷ���Դ
	//virtual s32 Release();

	// ǿ���������
	virtual void Clear();

	virtual bool Cache()
	{
		return true;
	}

	virtual bool Create(const char* szName, u32 nWidth, u32 nHeight, RendererPixelFormat pixelFormat);

	// ��ȡ����ɫ
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

	// ���rtt������
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
	SColor m_clrBackGround;		//������ɫ
	bool   m_bFullScreen;
	float  m_fScreenRatio;      //����Ļ�ı���
	u32 m_nIndex;
	ITexture* m_pTexture;
};
