/********************************************************************
	created:	2010/06/11
	created:	11:6:2010   16:29
	filename: 	PostScreenCompositor.h
	author:		liangairan
	
	purpose:	后处理合成器，专门处理屏幕上后处理的东西
*********************************************************************/

#pragma once 

#include "ScreenCompositor.h"
#include "RenderTargetScreenCompositor.h"
//#include "ManualGPUDataParamSource.h"
////---------------
//
//class PostCompositorManualGPUDataSource : public ManualGPUDataParamSource
//{
//public:
//    enum PostSreenManualType
//    {
//        ePSMT_BlurSamplerOffset = 0,
//        ePSMT_BlurSamplerWeight,
//    };
//public:
//    PostCompositorManualGPUDataSource( void );
//
//    virtual ~PostCompositorManualGPUDataSource( void );
//
//    //每帧要更新的函数
//    virtual void	Update(float fTimeElapse) {}
//
//    //创建手动更新的constant类型
//    virtual void	BuildManualConstantDefType(GPUConstantDefinition* pGPUDef);
//
//    AIRVector<Vector2Df>& GetBlurSamplerOffset()
//    {
//        return m_vtBlurSamplerOffset;
//    }
//
//    AIRVector<float>& GetBlurSamplerWeights()
//    {
//        return m_vtBlurSamplerWeight;
//    }
//protected:
//    virtual void	UpdateGPUParametesImpl(GPUParameters* pParameters, GPUConstantDefinition* pGPUDef);
//
//protected:
//    AIRVector<Vector2Df>  m_vtBlurSamplerOffset;
//    AIRVector<float>      m_vtBlurSamplerWeight;
//};
//
//class PostCompositorManualGPUDataParamSourceBuilder : public ManualGPUDataParamSourceBuilder
//{
//public:
//    PostCompositorManualGPUDataParamSourceBuilder( void ) {}
//    ~PostCompositorManualGPUDataParamSourceBuilder( void ) {}
//
//    ManualGPUDataParamSource* BuildManualGPUDataParamSource()
//    {
//        return new PostCompositorManualGPUDataSource();
//    }
//};
//
////---------------

class PostEffect;



class EngineExport PostScreenCompositor : public ScreenCompositor
{
public:
    PostScreenCompositor();
    virtual ~PostScreenCompositor();

    //virtual void Render(CCamera* pCamera);

    //void AddRenderTarget(RenderTarget* pRenderTarget);

    //清空数据
    virtual void Clear();

    TextureRenderTarget* GetRenderTarget()
    {
        return m_pScreenRenderTarget;
    }

    //RenderTarget* GetOutputRenderTarget();

    //void AddCompositor(PostScreenCompositor* pCompositor);

    //void RemoveCompositor(PostScreenCompositor* pCompositor);

    //void SetPostScreenType(PostScreenCompositorType type);

    //PostScreenCompositorType GetPostScreenType() const
    //{
    //    return m_pscType;
    //}

    //for test, save rendertarget to file
    void SaveToFile(const char* szFilename);

    virtual void SetActive(bool bActive);

    //先处理所有要画到rendertarget的内容
    //void ProcessAllCompositor();

    //预处理，拿输入的rtt作为纹理输出
    void Process(TextureRenderTarget* pRTTInput, float fScale, float fUVScale);

    //
    void SetHorziontalBlur(bool bHorizontal)
    {
        m_bHorizontalBlur = bHorizontal;
    }

    //void SetUseScreenRenderTarget(bool bUse);
    void SetPostEffect(PostEffect* pPostEffect);

    virtual void Create(int nPosX, int nPosY, int nWidth, int nHeight );

    //virtual bool Prepare();
	
	

protected:
    

    //virtual void _GetPrimitive();
protected:
    virtual void   PreRender(CCamera* pCamera);

	virtual void RebuildRenderData( void );

    TextureRenderTarget*  m_pScreenRenderTarget;     //场景的rendertarget

    ITexture*      m_pPreTexture;    //上一个rtt的纹理

    PostEffect*    m_pPostEffect;   //owner

    //PostScreenCompositorType  m_pscType;     //后处理类型

    bool           m_bHorizontalBlur;        //是否水平blur
    //bool           m_bUseSceneRenderTarget;   //通常是第一个才用到scenerendertarget，即把场景渲染到rendertarget里
    bool           m_bUseScreenRenderTarget;  //通常是中间的才用到这个，即把compositor渲染到rtt里

    //typedef std::list<RenderTarget*> RenderTargetList;
    //typedef RenderTargetList::iterator RenderTargetList_Iter;
    //RenderTargetList  m_lstRenderTargets;

    //typedef std::list<PostScreenCompositor*> CompositortList;
    //typedef CompositortList::iterator CompositortList_Iter;
    //CompositortList  m_lstCompositors;
	//
    
private:
	//根据缩放设置顶点数据
    void BuildScreenSizeData();
    
};
