// ***************************************************************
//  �ļ�: ScreenCompositor.h
//  ����: 2010-6-9
//  ����: huangjunjie
//  ˵��: 
// ***************************************************************

#pragma once


#include "Renderable.h"

class IMaterial;
class ITexture;
typedef void (*ScreenCompositorCallBackFunc)(void);

//���������
enum ScreenCompositorType
{
	ePPT_None = 0,
	ePPT_COLOR_Monochrome,
	ePPT_COLOR_DownFilter4,
	ePPT_COLOR_UpFilter4,
	ePPT_COLOR_ToneMap,
	ePPT_COLOR_BrightPass,
	ePPT_COLOR_BlurH,
	ePPT_COLOR_BlurV,
	ePPT_COLOR_BloomBlurH,
	ePPT_COLOR_BloomBlurV,
	ePPT_COLOR_NormalCombine,
	ePPT_COLOR_ScreenCombine,
	ePPT_COLOR_SoftLightCombine,
	ePPT_COLOR_ColorBurn,
	ePPT_Distortion,
};

class EngineExport ScreenCompositor// : public Renderable
{
	EngineAllocator(ScreenCompositor);
public:
	ScreenCompositor( void );

	virtual ~ScreenCompositor( void );

	//virtual void					Render(CCamera* pCamera);

	virtual void Clear();

    virtual void Create( ScreenCompositorType type, int nPosX, int nPosY, int nWidth, int nHeight );

    bool IsActive() const
    {
        return m_bActive;
    }

    virtual void SetActive(bool bActive)
    {
        m_bActive = bActive;
    }

    void SetWindowPosition(int nPosX, int nPosY, int nWidth, int nHeight);

	void SetCallBackFuncAfterRender( ScreenCompositorCallBackFunc func )
	{
		m_funcAfterRender = func;
	}

    //virtual void PostRender(CCamera* pCamera);

    void SetFullWindow(bool bFullWindow)
    {
        m_bFullWindow = bFullWindow;
    }

    bool IsFullWindow() const
    {
        return m_bFullWindow;
    }

	void SetInputTexture(ITexture* pTexture)
	{
		m_pTexture = pTexture;
	}

	void SetSceneTexture(ITexture* pSceneTexture)
	{
		m_pSceneTexture = pSceneTexture;
	}

	void Draw();

	void SetCompositorType(ScreenCompositorType ppType)
	{
		m_pscType = ppType;
	}

	ScreenCompositorType GetCompositorType()
	{
		return m_pscType;
	}

	IMaterial* GetMaterial()
	{
		return m_pMaterial;
	}
protected:
	//virtual void PreRender(CCamera* pCamera);

	virtual void RebuildRenderData( void );

    //virtual bool  CreateVertexDeclaration();

    //virtual void _GetPrimitive();

	IMaterial* m_pMaterial;
protected:
	Rectf		m_rcScreen;
    RHWVERTEX   m_renderQuad[4];
	SColor		m_color;
    bool        m_bActive;
    bool        m_bFullWindow;      //���洰�ڴ�С
    //CVertexDeclaration*             m_pVertexDecl;    //��������
	ScreenCompositorCallBackFunc	m_funcAfterRender;

protected:
	//�ύ��shader����
	float m_fScale;
	float m_fUVScale;                 //uv����
	//PostCompositorManualGPUDataSource*    m_pPostManualGPUDataSource;

	AIRVector< Vector4f >  m_vtBlurSamplerOffset;
	AIRVector< Vector4f >  m_vtDownFilter4SamplerOffset;
	AIRVector< Vector4f >      m_vtBlurWeight;
//private:
	ITexture* m_pTexture;   //Ҫ���Ƶ�texture
	ITexture* m_pSceneTexture; 

	ScreenCompositorType m_pscType;

	void   UpdateBlurGPUData(bool bHorizontal);
	void   UpdateDownFilter4GPUData();

	//IVertexBuffer*	m_pVB;					// GUI���㻺�����
	//IIndexBuffer*	m_pIB;					// GUI�����������
	//Primitive* m_pPrimitive;
};//! end class ScreenCompositor

#define POST_SCREEN_COMPOSITOR "PostProcessCompositor"  //

class EngineExport ScreenCompositorManager
{
	ScreenCompositorManager( void );

public:
	~ScreenCompositorManager( void );

	static ScreenCompositorManager* GetSingleton( void )
	{
		static ScreenCompositorManager inst;
		return &inst;
	}

	ScreenCompositor* CreateDefaultScreenCompositor( const AIRString& szName );

    //����compositor
    ScreenCompositor* CreateScreenCompositor(const AIRString& strName, const AIRString& strType);

	ScreenCompositor* GetScreenCompositor( const AIRString& szName );

	void RenderAllScreenCompositor( void );

	void DestroyAllScreenCompositor( void );

    //����compositor
	void DestroyScreenCompositor( const AIRString& szName );

    void DestoryScreenCompositor( ScreenCompositor* pCompositor );

protected:
	typedef std::map<AIRString, ScreenCompositor*> MAP_SCREEN_COMPOSITOR;
	typedef MAP_SCREEN_COMPOSITOR::iterator MAP_SCREEN_COMPOSITOR_ITOR;
	MAP_SCREEN_COMPOSITOR m_mapScreenCompositor;

};

#define SCREEN_COMPOSITOR_MANAGER (ScreenCompositorManager::GetSingleton())
