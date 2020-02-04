/********************************************************************
	created:	2010/06/24
	created:	24:6:2010   15:47
	filename: 	Shadowmap.h
	author:		liangairan
	
	purpose:	shadowmap的实现
*********************************************************************/

#pragma once


#include "RenderTarget.h"
//#include "ManualGPUDataParamSource.h"
#include "Singleton.h"
#include "MemoryAllocatorConfig.h"
#include "DepthStencil.h"
#include "NormalSceneRenderProcess.h"

class SamplerStateObject;
class RasterizerStateObject;
class TextureRenderTarget;

const int MAX_SPLITT_FRUSTUM = 4;
//--------------------------------------------------------
// PageStaticGeometry类的手动gpu参数
//--------------------------------------------------------

enum ShadowmapType
{
    SMT_ORTHO,   //最普通的shadowmap
    SMT_PSM,     //psm
    SMT_TSM,     //tsm
    SMT_CSM,
};

enum CSMMethod
{
    CSM_DX9,
    CSM_DX10,
};

struct BoundingPerspectiveData 
{
    Vector3Df direction;   //look direction
    f32 fFovX;
    f32 fFovY;
    f32 fNear;
    f32 fFar;
    Matrix4f matView;

    BoundingPerspectiveData() : fFovX(0), fFovY(0), fNear(0), fFar(0) {}
};

class EngineExport Shadowmap : public Singleton<Shadowmap>, public NormalSceneRenderProcess
{
    BE_SINGLETON(Shadowmap);
public:
    //Shadowmap();
    virtual ~Shadowmap();

    //初始化
    virtual bool Initialize(int nWidth, int nHeight);

    void SetLightPosition(const Vector3Df& position);

    void SetLightDirection(const Vector3Df& direction);


    //virtual void Render();

    //static Shadowmap* GetInstance()
    //{
    //    static Shadowmap instance;
    //    return &instance;
    //}
	virtual void Render();

    bool IsInitialized() const
    {
        return m_bInitialize;
    }

    //ShadowmapManualGPUDataParamSource* GetShadowmapGPUManualSource();

    void SetDebug(bool bDebug)
    {
        m_bDebug = bDebug;
    }

    const Matrix4f& GetLightViewProj() const
    {
        return m_matLightViewProj;
    }

    void SetTarget(CSceneNode* pTarget);

    //设置距离
    void SetDistanceLightToTarget(float fDistance);

    float GetDistanceLightToTarget() const
    {
        return m_fDisLightToTarget;
    }

    //设置是否透视
    void SetPerspective(bool bPerspective)
    {
        m_bPerspective = bPerspective;
    }

    bool IsPerspective() const
    {
        return m_bPerspective;
    }

    //void* GetDepthSurface();

    virtual void Clear();

    CSceneNode* GetTarget()
    {
        return m_pTarget;
    }

    f32 GetShadowDepthBias() const
    {
        return m_fShadowDepthBias;
    }

    f32 GetShadowScoleScaleBias() const
    {
        return m_fShadowScoleScaleBias;
    }

    bool IsSoftShadow() const
    {
        return true;
    }

    u32 GetShadowmapSize() const
    {
        return m_nShadowmapSize;
    }

    void SetShadowDepthBias(float fDepthBias)
    {
        m_fShadowDepthBias = fDepthBias;
    }

    void SetShadowScoleScaleBias(float fScoleScaleBias)
    {
        m_fShadowScoleScaleBias = fScoleScaleBias;
    }

    bool SaveShadowmap(const char* szFilename);
    //设置软阴影
    //void SetSoftShadow(bool bSoftShadow);

    f32 GetSoftNess() const
    {
        return m_fSoftNess;
    }

    void SetSoftNess(f32 fSoft)
    {
        m_fSoftNess = fSoft;
    }

    f32 GetJitterScale() const
    {
        return m_fJitterScale;
    }

    void SetJitterScale(f32 fJitterScale)
    {
        m_fJitterScale = fJitterScale;
    }

    ShadowmapType GetShadowmapType() const
    {
        return m_type;
    }

    void SetShadowmapType(ShadowmapType type);

    f32 GetShadowRangeRadius() const
    {
        return m_fShadowRangeRadius;
    }

    void SetShadowRangeRadius(f32 fRadius)
    {
        m_fShadowRangeRadius = fRadius;
    }

    void SetCSMSplitNum(s32 nSplitNum)
    {
        m_nSplitNum = nSplitNum;
    }

    bool IsUnitClip() const
    {
        return m_bUnitClip;
    }

    void SetUnitClip(bool bUnitClip)
    {
        m_bUnitClip = bUnitClip;
    }

    bool IsShadowTestInverted() const
    {
        return m_bShadowTestInverted;
    }

    const Matrix4f& GetMatrixR() const
    {
        return m_matR;
    }

	virtual int GetWidth() const
	{
		int nWidth = m_pShadowmap != NULL ? m_pShadowmap->GetWidth() : 0;

		if (nWidth == 0)
		{
			nWidth = m_pDepthStencil != NULL ? m_pDepthStencil->GetWidth() : 0;
		}
		return nWidth;
	}

	virtual int GetHeight() const
	{
		int nHeight = m_pShadowmap != NULL ? m_pShadowmap->GetHeight() : 0;
		if (nHeight == 0)
		{
			nHeight = m_pDepthStencil != NULL ? m_pDepthStencil->GetHeight() : 0;
		}
		return nHeight;
	}

	//virtual ITexture* GetDepthTexture()
	//{
	//	return m_pDepthTexture;
	//}

	void SetEnable(bool bEnable)
	{
		m_bEnable = bEnable;
	}

	bool IsEnable() const
	{
		return m_bEnable;
	}

	const AIRString& GetName() const
	{
		return m_strName;
	}

	CCamera* GetCamera()
	{
		return m_pCamera;
	}

	TextureRenderTarget* GetShadowmap()
	{
		return m_pShadowmap;
	}

    void SetCasterAABB(const AABBBox3Df& casterAABB)
    {
        m_casterAABB = casterAABB;
    }
protected:
    
    //virtual void UpdateImp();

    virtual void UpdateCamera();

    //virtual void RenderImpl();

    void CreateSoftShadowJitterTexture();

private:
    void BuildPSMMatrices();

    void BuildTSMMatrices();

    void BuildOrthoShadowProjectionMatrix(const CFrustum& frustum, Matrix4f& matOut/*, CRenderQueue& renderQueue*/);

    void UpdateCSMData();

    //计算出最适合的摄像机参数
    void ComputeFitCameraParams(CCamera* pCamera);

    void ComputeFOVByBounds(AIRList<AABBBox3Df>& boxList, const Matrix4f& matProj, const Vector3Df& posEye, const Vector3Df& _direction, f32& fFovX, f32& fFovY);

    void CaculateBoundingSphere(const AIRVector<Vector3Df>& points, Vector3Df& center, f32& fRadius);

    void ComputeBoundPerpespectiveData(AIRList<AABBBox3Df>& boxList, const Matrix4f& matProj, const Vector3Df& posEye, const Vector3Df& _direction, BoundingPerspectiveData& out);

    void ComputeBoundPerpespectiveData(AIRList<AABBBox3Df>& boxList, const Matrix4f& matProj, const Vector3Df& posEye, BoundingPerspectiveData& out);

    void RenderCascadeShadowmap();

    void GetAABBFromProjectMatrix(AABBBox3Df& aabb, const Matrix4f& matProj);
private:
    Vector3Df  m_positionLight;   //灯光位置
    Vector3Df  m_directionLight;  //灯光方向
    bool       m_bInitialize;     //是否已经初始化
    float      m_fDisLightToTarget;    //灯光到
    
    Matrix4f   m_matR;            //做投影变换的矩阵
    Matrix4f   m_matLightViewProj;      //lightspace矩阵
    
    //ShadowmapManualGPUDataParamSource*  m_pGPUShadowmapSource;
    CSceneNode*  m_pTarget;   //shadowmap的灯观察的圣像
    bool            m_bPerspective;
    bool            m_bDebug;
    bool            m_bHardwareShadowmap;   //是否为hardwareshadowmap
    //ITexture*       m_pDepthTexture;        //深度图（实际上就是shadowmap）
    //IMaterial*      m_pMaterial;

    //bool            m_bSoftShadow;   //是否为软阴影
    ITexture*   m_pJitterTexture;   //抖动纹理，用于软阴影

    float       m_fShadowDepthBias;
    float       m_fShadowScoleScaleBias;

    u32   m_nShadowmapSize;  //shadowmap的大小

    static const int JITTER_SIZE = 32;
    static const int JITTER_SAMPLES = 8;

    //CCamera*  m_pVirtualCamera;   //虚拟摄像机
    Matrix4f  m_matVirtualCameraViewProj;   //虚拟摄像机投影矩阵
    ShadowmapType m_type;   //shadowmap类型
    f32 m_fMinInfinityZ;    //这个不知道有什么用，但从psm的例子上看，会对渲染有很大影响。
    f32 m_fSlideBack;
    f32 m_fPPNear;   //postperspective near
    f32 m_fPPFar;    //postperspective far
    f32 m_fAspect;
    f32 m_zNear;
    f32 m_zFar;
    bool m_bShadowTestInverted;

    AIRVector<AABBBox3Df> m_lstShadowCasters;
    AIRList<AABBBox3Df> m_lstShadowReceivers;

    f32 m_fSoftNess;   //软阴影参数
    f32 m_fJitterScale; 
    f32 m_fShadowRangeRadius;   //只适用于psm系列，psm不适合大范围阴影，只适用于一定范围内的，否则极不清楚
    f32 m_fTSM_Delta;
    
    CFrustum m_frustumSplit[MAX_SPLITT_FRUSTUM];
    float m_fFars[MAX_SPLITT_FRUSTUM];
    int m_nSplitNum;   //真正的划分
    CSMMethod  m_csmMethod;   //CSM方法，默认是DX9

    bool m_bUnitClip;
	SamplerStateObject* m_pShadowSamplerState;
	SamplerStateObject* m_pJitterSamplerState;
	RasterizerStateObject* m_pRasterizerStateObject;

	TextureRenderTarget* m_pShadowmap;
	DepthStencil*        m_pDepthStencil;
	bool m_bEnable;   //是否激活

	//CCamera* m_pCamera;
	//CRenderQueue		m_renderQueue;			//渲染队列
	AIRString m_strName;

    AABBBox3Df m_casterAABB;   //所有显示在场景内的并且要渲染到阴影的shadowmap，暂定在view空间里
};

#define SHADOWMAP Shadowmap::GetInstance()
