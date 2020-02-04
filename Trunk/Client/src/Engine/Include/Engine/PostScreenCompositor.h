/********************************************************************
	created:	2010/06/11
	created:	11:6:2010   16:29
	filename: 	PostScreenCompositor.h
	author:		liangairan
	
	purpose:	����ϳ�����ר�Ŵ�����Ļ�Ϻ���Ķ���
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
//    //ÿ֡Ҫ���µĺ���
//    virtual void	Update(float fTimeElapse) {}
//
//    //�����ֶ����µ�constant����
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

    //�������
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

    //�ȴ�������Ҫ����rendertarget������
    //void ProcessAllCompositor();

    //Ԥ�����������rtt��Ϊ�������
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

    TextureRenderTarget*  m_pScreenRenderTarget;     //������rendertarget

    ITexture*      m_pPreTexture;    //��һ��rtt������

    PostEffect*    m_pPostEffect;   //owner

    //PostScreenCompositorType  m_pscType;     //��������

    bool           m_bHorizontalBlur;        //�Ƿ�ˮƽblur
    //bool           m_bUseSceneRenderTarget;   //ͨ���ǵ�һ�����õ�scenerendertarget�����ѳ�����Ⱦ��rendertarget��
    bool           m_bUseScreenRenderTarget;  //ͨ�����м�Ĳ��õ����������compositor��Ⱦ��rtt��

    //typedef std::list<RenderTarget*> RenderTargetList;
    //typedef RenderTargetList::iterator RenderTargetList_Iter;
    //RenderTargetList  m_lstRenderTargets;

    //typedef std::list<PostScreenCompositor*> CompositortList;
    //typedef CompositortList::iterator CompositortList_Iter;
    //CompositortList  m_lstCompositors;
	//
    
private:
	//�����������ö�������
    void BuildScreenSizeData();
    
};
