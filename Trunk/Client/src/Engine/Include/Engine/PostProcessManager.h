/********************************************************************
	created:	2010/06/17
	created:	17:6:2010   10:41
	filename: 	PostProcessManager.h
	author:		liangairan
	
	purpose:	后处理管理器
*********************************************************************/

#pragma once 

#include "PostScreenCompositor.h"
#include "Singleton.h"

//enum PostProcessFilter
//{
//    ePPF_Distortion = 0,
//    ePPF_HBloom,
//    ePPF_VBloom,
//    ePPF_VBlur,
//    ePPF_HBlur,
//    ePPF_RayBlur,
//    ePPF_HDR,
//    ePPF_MAX,
//};

class SamplerStateObject;
class NormalSceneRenderProcess;

enum PostEffectType
{
 //   ePET_PostScene = 0,	//后处理
//ePET_Monochrome,    //黑白
 //   
 //   ePET_Bloom,			//Bloom
 //   ePET_Blur,			//blur
 //   ePET_RayBlur,       //射线模糊
 //   ePET_HDR,			//high dynamin range
 //   ePET_MAX,
    ePET_Monochrome = 0,    //黑白
    ePET_Bloom,			//Bloom
	ePET_Distortion,	//扭曲
    ePET_Blur,          //模糊
    ePET_SoftLight,     //柔光
    ePET_Glow,          //全屏泛光
	ePET_MAX,
};


class EngineExport PostEffect
{
public:
    typedef AIRList<PostScreenCompositor*>	CompositortList;
    typedef CompositortList::iterator			CompositortList_Iter;

public:
    PostEffect();
    virtual ~PostEffect();

    bool				IsActive() const
    {
        return m_bActive;
    }

    void				SetActive(bool bActive);

    PostEffectType		GetPostEffectType() const
    {
        return m_postEffectType;
    }

    CompositortList&	GetCompositorList()
    {
        return m_lstCompositors;
    }

    //创建后处理
    virtual void		Create(int nPosX, int nPosY, int nWidth, int nHeight, PostEffectType type);

    //清空
    virtual void		Clear();

    void				OnResetDevice();

protected:
    bool				m_bActive;
    PostEffectType		m_postEffectType;
    CompositortList		m_lstCompositors;

private:
    PostScreenCompositor* CreatePostScreenCompositor(int nPosX, int nPosY, int nWidth, int nHeight, const char* szName, ScreenCompositorType pscType);
};

class EngineExport PostProcess
{
	EngineAllocator(PostProcess);
public:
	PostProcess();
	virtual ~PostProcess();

	void Process(ITexture* pInputTex);

	ITexture* GetOutputTexture();

protected:
	TextureRenderTarget* m_pOutputTarget;
	RHWVERTEX   m_renderQuad[4];
	//根据缩放设置顶点数据
	void BuildScreenSizeData();
	float m_fScale;
	float m_fUVScale;
};


class EngineExport PostProcessManager : public Singleton<PostProcessManager>
{
    BE_SINGLETON(PostProcessManager);
	typedef AIRList<PostScreenCompositor*>	CompositortList;
	typedef CompositortList::iterator			CompositortList_Iter;

public:
    ~PostProcessManager();

    //预处理
    void PostProcess();

    //最终渲染到屏幕上
    void		Render();

	//创建后处理特效
    PostEffect* CreatePostEffect(PostEffectType type, int nWidth, int nHeight);

	//获得后处理特效
    PostEffect* GetPostEffect(PostEffectType type)
    {
        return m_PostEffects[type];
    }

    //清空
    void		Clear();

	//打开/关闭后处理特效
    void		EnablePostEffect(bool bActive, PostEffectType type);
 
	//
	TextureRenderTarget*  GetSceneRenderTarget()
	{
		return m_pSceneRenderTarget;
	}
	//获得手动的gpu传参对象
    //PostCompositorManualGPUDataSource* GetPostManualGPUDataSource()
    //{
    //    return m_pPostManualGPUDataSource;
    //}

	//查询某个类型的合成器当前是否激活
    bool		IsPostEffectActive(PostEffectType type);

    void		SaveToFile(const char* szFilename);

	//获得激活的合成器数量
    size_t		GetActiveCompositorsNum() const;

    //设备重置
    void		OnResetDevice();


protected:
    
private:
	NormalSceneRenderProcess*           m_pSceneRenderProcess;
    TextureRenderTarget*						m_pSceneRenderTarget;		//场景的rendertarget
    //PostCompositorManualGPUDataSource*  m_pPostManualGPUDataSource;	//
    ScreenCompositor*					m_pCompositor;				//最终输出到屏幕的compositor
    PostEffect*							m_PostEffects[ePET_MAX];	//
    CompositortList						m_lstActiveCompositors;		//
    float                               m_fScreenScale;             //顶点缩放比例
	float								m_fUVScale;                 //uv缩放
    SamplerStateObject* m_pSamplerState;
};

#define POSTPROCESS_MANAGER PostProcessManager::GetInstance()
