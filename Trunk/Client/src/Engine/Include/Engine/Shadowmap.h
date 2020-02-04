/********************************************************************
	created:	2010/06/24
	created:	24:6:2010   15:47
	filename: 	Shadowmap.h
	author:		liangairan
	
	purpose:	shadowmap��ʵ��
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
// PageStaticGeometry����ֶ�gpu����
//--------------------------------------------------------

enum ShadowmapType
{
    SMT_ORTHO,   //����ͨ��shadowmap
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

    //��ʼ��
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

    //���þ���
    void SetDistanceLightToTarget(float fDistance);

    float GetDistanceLightToTarget() const
    {
        return m_fDisLightToTarget;
    }

    //�����Ƿ�͸��
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
    //��������Ӱ
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

    //��������ʺϵ����������
    void ComputeFitCameraParams(CCamera* pCamera);

    void ComputeFOVByBounds(AIRList<AABBBox3Df>& boxList, const Matrix4f& matProj, const Vector3Df& posEye, const Vector3Df& _direction, f32& fFovX, f32& fFovY);

    void CaculateBoundingSphere(const AIRVector<Vector3Df>& points, Vector3Df& center, f32& fRadius);

    void ComputeBoundPerpespectiveData(AIRList<AABBBox3Df>& boxList, const Matrix4f& matProj, const Vector3Df& posEye, const Vector3Df& _direction, BoundingPerspectiveData& out);

    void ComputeBoundPerpespectiveData(AIRList<AABBBox3Df>& boxList, const Matrix4f& matProj, const Vector3Df& posEye, BoundingPerspectiveData& out);

    void RenderCascadeShadowmap();

    void GetAABBFromProjectMatrix(AABBBox3Df& aabb, const Matrix4f& matProj);
private:
    Vector3Df  m_positionLight;   //�ƹ�λ��
    Vector3Df  m_directionLight;  //�ƹⷽ��
    bool       m_bInitialize;     //�Ƿ��Ѿ���ʼ��
    float      m_fDisLightToTarget;    //�ƹ⵽
    
    Matrix4f   m_matR;            //��ͶӰ�任�ľ���
    Matrix4f   m_matLightViewProj;      //lightspace����
    
    //ShadowmapManualGPUDataParamSource*  m_pGPUShadowmapSource;
    CSceneNode*  m_pTarget;   //shadowmap�ĵƹ۲��ʥ��
    bool            m_bPerspective;
    bool            m_bDebug;
    bool            m_bHardwareShadowmap;   //�Ƿ�Ϊhardwareshadowmap
    //ITexture*       m_pDepthTexture;        //���ͼ��ʵ���Ͼ���shadowmap��
    //IMaterial*      m_pMaterial;

    //bool            m_bSoftShadow;   //�Ƿ�Ϊ����Ӱ
    ITexture*   m_pJitterTexture;   //����������������Ӱ

    float       m_fShadowDepthBias;
    float       m_fShadowScoleScaleBias;

    u32   m_nShadowmapSize;  //shadowmap�Ĵ�С

    static const int JITTER_SIZE = 32;
    static const int JITTER_SAMPLES = 8;

    //CCamera*  m_pVirtualCamera;   //���������
    Matrix4f  m_matVirtualCameraViewProj;   //���������ͶӰ����
    ShadowmapType m_type;   //shadowmap����
    f32 m_fMinInfinityZ;    //�����֪����ʲô�ã�����psm�������Ͽ��������Ⱦ�кܴ�Ӱ�졣
    f32 m_fSlideBack;
    f32 m_fPPNear;   //postperspective near
    f32 m_fPPFar;    //postperspective far
    f32 m_fAspect;
    f32 m_zNear;
    f32 m_zFar;
    bool m_bShadowTestInverted;

    AIRVector<AABBBox3Df> m_lstShadowCasters;
    AIRList<AABBBox3Df> m_lstShadowReceivers;

    f32 m_fSoftNess;   //����Ӱ����
    f32 m_fJitterScale; 
    f32 m_fShadowRangeRadius;   //ֻ������psmϵ�У�psm���ʺϴ�Χ��Ӱ��ֻ������һ����Χ�ڵģ����򼫲����
    f32 m_fTSM_Delta;
    
    CFrustum m_frustumSplit[MAX_SPLITT_FRUSTUM];
    float m_fFars[MAX_SPLITT_FRUSTUM];
    int m_nSplitNum;   //�����Ļ���
    CSMMethod  m_csmMethod;   //CSM������Ĭ����DX9

    bool m_bUnitClip;
	SamplerStateObject* m_pShadowSamplerState;
	SamplerStateObject* m_pJitterSamplerState;
	RasterizerStateObject* m_pRasterizerStateObject;

	TextureRenderTarget* m_pShadowmap;
	DepthStencil*        m_pDepthStencil;
	bool m_bEnable;   //�Ƿ񼤻�

	//CCamera* m_pCamera;
	//CRenderQueue		m_renderQueue;			//��Ⱦ����
	AIRString m_strName;

    AABBBox3Df m_casterAABB;   //������ʾ�ڳ����ڵĲ���Ҫ��Ⱦ����Ӱ��shadowmap���ݶ���view�ռ���
};

#define SHADOWMAP Shadowmap::GetInstance()
