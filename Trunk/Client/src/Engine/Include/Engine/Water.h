// ***************************************************************
//  �ļ�: Water.h
//  ����: 2010-6-5
//  ����: huangjunjie
//  ˵��: water scene object
// ***************************************************************

#pragma once

#ifndef __Water_H__
#define __Water_H__

#include "Renderable.h"
#include "IVertexBuffer.h"
#include "Matrix4.h"
#include "ITexture.h"
#include "IEffect.h"
#include "IMaterial.h"
#include "VertexDef.h"
//#include "RenderTargetReflect.h"
//#include "RenderTargetRefract.h"
//#include "RenderTargetDepthMap.h"
//#include "ManualGPUDataParamSource.h"
#include "ScreenCompositor.h"
class TextureRenderTarget;
//#include "RenderTargetScreenCompositor.h"

class SamplerStateObject;
class NormalSceneRenderProcess;
class DepthStencil;

enum eWaterCmdType
{
	eWCT_NoReflectNoRefractNoSoftEdge				= 0, // �޷���,������,�ޱ�Ե�ữ
	eWCT_NoReflectHasRefractNoSoftEdge				= 1, // �޷���,������,�ޱ�Ե�ữ
	eWCT_HasReflectNoRefractNoSoftEdge				= 2, // �з���,������,�ޱ�Ե�ữ
	eWCT_HasReflectHasRefractNoSoftEdge				= 3, // �з���,������,�ޱ�Ե�ữ
	eWCT_NoReflectHasRefractWithHightMapSoftEdge	= 4, // �޷���,������,���߶�ͼ��Ե�ữ
	eWCT_HasReflectNoRefractWithHightMapSoftEdge	= 5, // �з���,������,���߶�ͼ��Ե�ữ
	eWCT_HasReflectHasRefractWithHightMapSoftEdge	= 6, // �з���,������,���߶�ͼ��Ե�ữ
	eWCT_HasReflectHasRefractWithDepthMapSoftEdge	= 7, // �з���,������,�����ͼ��Ե�ữ
};

static const AIRString CSTR_WATER_TECHNIQUES[] = 
{
	AIRString( "WaterNoReflectNoRefractNoSoftEdge" ),
	AIRString( "WaterNoReflectHasRefractNoSoftEdge" ),
	AIRString( "WaterHasReflectNoRefractNoSoftEdge" ),
	AIRString( "WaterHasReflectHasRefractNoSoftEdge" ),
	AIRString( "WaterNoReflectHasRefractWithHightMapSoftEdge" ),
	AIRString( "WaterHasReflectNoRefractWithHightMapSoftEdge" ),
	AIRString( "WaterHasReflectHasRefractWithHightMapSoftEdge" ),
	AIRString( "WaterHasReflectHasRefractWithDepthMapSoftEdge" ),
};

class EngineExport CWater : public Renderable
{
public:
	CWater();

	virtual ~CWater( void );

	// ����ˮ��
    virtual void Update(float fTimeElapse );

	// ��Ⱦˮ��
	//virtual void					Render(CCamera* pCamera);

	// ���ˮ����Դ
	virtual void Clear();

	// ���µ���Ⱦ����
	virtual void UpdateToRenderQueue(CCamera* pCamera, CRenderQueue* pQueue);

	//************************************
	// Method:    CreateWaterSuface
	// Qualifier: ����ˮ�����
	// Parameter: int nGridCountX : ˮ��X�������ϵ�������
	// Parameter: int nGridCountZ : ˮ��Z�������ϵ�������
	// Parameter: float fGridScaleX : ˮ��X�������ϵ�����ϵ��
	// Parameter: float fGridScaleZ : ˮ��Z�������ϵ�����ϵ��
	// Parameter: Vector3Df & vWaterSufaceCenterPos : ˮ���������ĵ������
	//************************************
	virtual bool CreateWaterSuface( int nGridCountX, int nGridCountZ, float fGridScaleX, float fGridScaleZ, Vector3Df& vWaterSufaceCenterPos );


	//************************************
	// Method:    CreateWaterRenderTarget
	// Qualifier: ����ˮ��RTT
	// Parameter: int nTargetWidth : ˮ��RTT�Ŀ��
	// Parameter: int nTargetHeight : ˮ��RTT�ĸ߶�
	//************************************
	bool CreateWaterRenderTarget( int nTargetWidth, int nTargetHeight );

	// ����ˮ��Ч���ϳ���
	void CreateUnderWaterCompositor( int nTargetWidth, int nTargetHeight );

	// ����ˮ������Ĵ�С
	void SetSize( f32 fWidth, f32 fHeight );

	// ����ˮ��������
	void SetGridCount( int nCountX, int nCountZ );

	// ����ˮ��������
	void SetGridScale( float fScaleX, float fScaleZ );

	// ���ˮ����X�������
	int GetGridCountX( void )   { return m_nGridCountX; }

	// ���ˮ����Z�������
	int GetGridCountZ( void )	{ return m_nGridCountZ; }

	// ���ˮ����X���С
	float GetGridWidthX( void )	{ return m_fGridWidthX; }

	// ���ˮ����Z���С
	float GetGridWidthZ( void )	{ return m_fGridWidthZ; }

	// ���ˮ������Ŀ��
	f32 GetWidth( void )
    {
        return m_fWidth;
    }

	// ���ˮ������ĸ߶�
	f32 GetHeight( void )
    {
        return m_fHeight;
    }

	// ���ˮ����������
	void SetDepth( f32 fDepth );

	// ����ˮ���������������λ��
	void SetWaterSufaceCenterPos( Vector3Df& vPos );

	// �����Ƿ�򿪷���
	void SetUseReflect( bool bUseReflect );

	// �����Ƿ������
	void SetUseRefract( bool bUseRefract );

	// ����Ƿ�򿪷���
	//bool GetUseReflect( void );

	// ����Ƿ������
	//bool GetUseRefract( void );

	//void SetUseDepthMap( bool bUseDepthMap );

	//bool GetUseDepthMap( void );

	// ��÷���RTT����
	//RenderTargetReflect* GetRenderTargetReflect( void );

	// �������RTT]����
	//RenderTargetRefract* GetRenderTargetRefract( void );

	// ���÷���RTT����
	//bool SetRenderTargetReflect( RenderTargetReflect* prtt );

	// ��������RTT����
	//bool SetRenderTargetRefract( RenderTargetRefract* prtt );

	// ����ˮ�����ɫ
	void SetWaterColor( const SColor& color );

	// ���ˮ�����ɫ
	const SColor& GetWaterColor( void )
    {
        return m_clrWater;
    }

	// Ĭ�ϳ�ʼ��
	virtual void InitDefault( void );

	// ����ˮ���Ŷ�
	void SetWaveDistorsion( float fDistorsion );

	// ���ˮ���Ŷ�
	float GetWaveDistorsion( void )
    {
        return m_fWaveDistorsion;
    }

	// ����ˮ������
	void SetFlowDirection( const Vector2Df& vDir );

	// ���ˮ������
	const Vector2Df& GetFlowDirection( void ) const
    {
        return m_vWaveDirection;
    }

	// ����ˮ���ٶ�
	void SetFlowSpeed( float fSpeed );

	// ���ˮ���ٶ�
	float GetFlowSpeed( void )
    {
        return m_fFlowSpeed;
    }

	//virtual RenderTargetScreenCompositor* GetRenderTargetScreenCompositor( void ) { return m_pRTTUnderWaterCompositor; }

	// ��XML����ˮ
	virtual	bool CreateFromXML( CMarkupSTL& xml );

	// ��ˮ���浽XML
	virtual bool SaveToXML( CMarkupSTL& xml );

	// ��ǰ�Ƿ����ˮ
	//virtual void					SetDebugWater( bool b );

	// ����ˮ�����U����
	void SetUScale( float fUScale );

	// ����ˮ�����V����
	void SetVScale( float fVScale );

	// ���ˮ�����U����
	float GetUScale( void )
    {
        return m_fUScale;
    }

	// ���ˮ�����V����
	float GetVScale( void )
    {
        return m_fVScale;
    }

	// �����Ƿ������θ߶�ͼ��ˮ��ļ���
	void SetIsCalByHeightMap( bool b );

	// ����Ƿ������θ߶�ͼ��ˮ��ļ���
	bool GetIsCalByHeightMap( void )
    {
        return m_bIsCalByHeightMap;
    }

	// ����ˮ�����ʾģʽ
	void SetWaterCmdType( eWaterCmdType eWCT );

	// ���ˮ����ʾ��ģʽ
	eWaterCmdType GetWaterCmdType( void )
    {
        return m_eWaterCmdType;
    }

	// ˢ�µ��θ߶�ͼ��ˮ��
	//virtual void RefreshHeightMapToWater( void );

	// �Զ��л�����
	void AutoSelectMaterial( void );

	virtual bool IsPick(const Vector3Df& vOrg, const Vector3Df& vPickDir, float& distFromEyeToObject);

	//--------------------------------------------------
	//! add by hjj 2010-9-14
	void SetFogColorUnderWater(const CColorValue& color)
	{
		m_fogColorUnderWater = color;
	}

	void SetFogModeUnderWater(FogMode mode)
	{
		m_fogModeUnderWater = mode;
	}

	void SetFogStartUnderWater( float fStart)
	{
		m_fFogStartUnderWater = fStart;
	}

	void SetFogEndUnderWater(float fEnd)
	{
		m_fFogEndUnderWater = fEnd;
	}

	void SetFogDensityUnderWater(float fDensity)
	{
		m_fDensityUnderWater = fDensity;
	}

	float GetFogStartUnderWater() const
	{
		return m_fFogStartUnderWater;
	}

	float GetFogEndUnderWater() const
	{
		return m_fFogEndUnderWater;
	}

	float GetFogDensityUnderWater() const
	{
		return m_fDensityUnderWater;
	}

	FogMode GetFogModeUnderWater() const
	{
		return m_fogModeUnderWater;
	}

	const CColorValue& GetFogColorUnderWater() const 
	{
		return m_fogColorUnderWater;
	}
	//--------------------------------------------------

	void SetShiness( float fShiness )
	{
		m_fShiness = fShiness;
	}

	float GetShiness( void )
	{
		return m_fShiness;
	}

	void SetGradient( float fGradient )
	{
		m_fGradient = fGradient;
	}

	float GetGradient( void )
	{
		return m_fGradient;
	}

	virtual bool Prepare();

	void GetWorldRect( Rectf& rc );

    //virtual bool IsTransparent() const
    //{
    //    return true;
    //}

    virtual void FillRenderContext(CCamera* pCamera, RenderContextQueue* pQueue, eRenderFlow flow);
protected:
	// ��ȾԤ����
	virtual void PreRender(CCamera* pCamera);

	// �ؽ�ˮ����������
	void RebuildRenderData( bool bNeedRebuildRenderData = false );

	// ���ˮ��RTT
	void ClearWaterRTT( void );

	// ����ˮ��Ч���ϳ���
	void ClearUnderWaterCompositor( void );

    virtual void _GetPrimitive();

protected:
	int								m_nGridCountX;					// X��������
	int								m_nGridCountZ;					// Z��������
	f32								m_fGridWidthX;					// ����X�᳤��     ��������
	f32								m_fGridWidthZ;					// ����Z�᳤��     ��������
	f32								m_fWidth;						// ˮ��X�����ܿ��
	f32								m_fHeight;						// ˮ��Z�����ܳ���
	f32								m_fDepth;						// ����
	f32								m_fUScale;						// ����U������ֵ
	f32								m_fVScale;						// ����V������ֵ
	Vector3Df						m_vCenter;						// ˮ���������������
	SColor							m_clrWater;						// ˮ����ɫ
	//RenderTargetReflect*			m_pRTTReflect;					// ����RTT
	//RenderTargetRefract*			m_pRTTRefract;					// ����RTT
	bool							m_bUnderWater;					// �Ƿ���ˮ��
	//WaterManualGPUDataParamSource*  m_pGPUDataParmSource;			// GPU����
	float							m_fWaveDistorsion;				// ˮ���Ŷ�����
	Vector2Df						m_vWaveDirection;				// ˮ����������
	float							m_fFlowSpeed;					// ˮ�������ٶ�
	//ScreenCompositor*				m_pUnderWaterCompositor;		// ˮ����Ч�ϳ���
	//RenderTargetScreenCompositor*	m_pRTTUnderWaterCompositor;		// ˮ����ЧRTT
	bool							m_bIsCalByHeightMap;			// ���ú�ˮ�Ƿ���Ҫ���θ߶�ͼ���м���
	eWaterCmdType					m_eWaterCmdType;

	ITexture*						m_pHeightMapTex;

	//--------------------------------------------------
	//! add by hjj 2010-9-14
	CColorValue						m_fogColorUnderWater;			// ˮ�������ɫ
	FogMode							m_fogModeUnderWater;			// ˮ�����ģʽ
	float							m_fDensityUnderWater;			// ˮ������ܶ�
	float							m_fFogStartUnderWater;			// ˮ�������ʼ
	float							m_fFogEndUnderWater;			// ˮ����Ľ���
	//--------------------------------------------------

	bool							m_bIsCameraInWater;

	float							m_fShiness;

	float							m_fGradient;

	TextureRenderTarget*			m_pRTTDepthMap;
	//RenderTarget*					m_pRTTDepthMap;

    ITexture*  m_pBumpTexture;    //bumpmap
	//SamplerStateObject* m_pSamplerReflect;
	//SamplerStateObject* m_pSamplerRefract;
    //SamplerStateObject* m_pSamplerBump;
	NormalSceneRenderProcess* m_pDepthMapProcess;
	NormalSceneRenderProcess* m_pWaterReflectProcess;   //����
	NormalSceneRenderProcess* m_pWaterRefactProcess;    //����

    TextureRenderTarget* m_pRTTReflect;
    TextureRenderTarget* m_pRTTRefract;

    DepthStencil* m_pRTTDepthStencil;
};//! end class Water



typedef std::map< AIRString, CWater* > MAP_WATER;

class EngineExport WaterManager
{

	WaterManager( void );

public:

	~WaterManager( void );

	static WaterManager* GetSingleton( void )
	{
		static WaterManager inst;
		return &inst;
	}

	bool AddWater( CWater* pWater );

	CWater* GetWater( const AIRString& szName );

	CWater* CreateDefaultWater( const AIRString& szName );

	void DestroyWater( const AIRString& szName );

	void DestroyWater( CWater* pWater );

	void DestroyAllWater( void );

	bool ChangeWaterName( const AIRString& szOldName, const AIRString& szNewName );

    //����ˮ��
    void UpdateWater(float fTimeElapsed);

	//void RefreshHeightMapToWater( void );

	CWater* CreateDefaultWaterImpl();

	MapIterator<MAP_WATER> GetWaterMapItorObject( void )
	{
		return MapIterator<MAP_WATER>( m_mapWater );
	}

	void SetShowAllWater( bool bShow );

protected:
	MAP_WATER m_mapWater;
};

//#define WATER_MANAGER (WaterManager::GetSingleton())

#endif //! __Water_H__