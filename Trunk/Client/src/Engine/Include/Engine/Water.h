// ***************************************************************
//  文件: Water.h
//  日期: 2010-6-5
//  作者: huangjunjie
//  说明: water scene object
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
	eWCT_NoReflectNoRefractNoSoftEdge				= 0, // 无反射,无折射,无边缘柔化
	eWCT_NoReflectHasRefractNoSoftEdge				= 1, // 无反射,有折射,无边缘柔化
	eWCT_HasReflectNoRefractNoSoftEdge				= 2, // 有反射,无折射,无边缘柔化
	eWCT_HasReflectHasRefractNoSoftEdge				= 3, // 有反射,有折射,无边缘柔化
	eWCT_NoReflectHasRefractWithHightMapSoftEdge	= 4, // 无反射,有折射,带高度图边缘柔化
	eWCT_HasReflectNoRefractWithHightMapSoftEdge	= 5, // 有反射,无折射,带高度图边缘柔化
	eWCT_HasReflectHasRefractWithHightMapSoftEdge	= 6, // 有反射,有折射,带高度图边缘柔化
	eWCT_HasReflectHasRefractWithDepthMapSoftEdge	= 7, // 有反射,有折射,带深度图边缘柔化
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

	// 更新水体
    virtual void Update(float fTimeElapse );

	// 渲染水体
	//virtual void					Render(CCamera* pCamera);

	// 清空水体资源
	virtual void Clear();

	// 更新到渲染队列
	virtual void UpdateToRenderQueue(CCamera* pCamera, CRenderQueue* pQueue);

	//************************************
	// Method:    CreateWaterSuface
	// Qualifier: 创建水体对象
	// Parameter: int nGridCountX : 水体X坐标轴上的网格数
	// Parameter: int nGridCountZ : 水体Z坐标轴上的网格数
	// Parameter: float fGridScaleX : 水体X坐标轴上的缩放系数
	// Parameter: float fGridScaleZ : 水体Z坐标轴上的缩放系数
	// Parameter: Vector3Df & vWaterSufaceCenterPos : 水体网格中心的坐标点
	//************************************
	virtual bool CreateWaterSuface( int nGridCountX, int nGridCountZ, float fGridScaleX, float fGridScaleZ, Vector3Df& vWaterSufaceCenterPos );


	//************************************
	// Method:    CreateWaterRenderTarget
	// Qualifier: 创建水体RTT
	// Parameter: int nTargetWidth : 水体RTT的宽度
	// Parameter: int nTargetHeight : 水体RTT的高度
	//************************************
	bool CreateWaterRenderTarget( int nTargetWidth, int nTargetHeight );

	// 创建水底效果合成器
	void CreateUnderWaterCompositor( int nTargetWidth, int nTargetHeight );

	// 设置水体网格的大小
	void SetSize( f32 fWidth, f32 fHeight );

	// 设置水网格数量
	void SetGridCount( int nCountX, int nCountZ );

	// 设置水网格缩放
	void SetGridScale( float fScaleX, float fScaleZ );

	// 获得水网格X向的数量
	int GetGridCountX( void )   { return m_nGridCountX; }

	// 获得水网格Z向的数量
	int GetGridCountZ( void )	{ return m_nGridCountZ; }

	// 获得水网格X向大小
	float GetGridWidthX( void )	{ return m_fGridWidthX; }

	// 获得水网格Z向大小
	float GetGridWidthZ( void )	{ return m_fGridWidthZ; }

	// 获得水体网格的宽度
	f32 GetWidth( void )
    {
        return m_fWidth;
    }

	// 获得水体网格的高度
	f32 GetHeight( void )
    {
        return m_fHeight;
    }

	// 获得水体网格的深度
	void SetDepth( f32 fDepth );

	// 设置水体网格的中心坐标位置
	void SetWaterSufaceCenterPos( Vector3Df& vPos );

	// 设置是否打开反射
	void SetUseReflect( bool bUseReflect );

	// 设置是否打开折射
	void SetUseRefract( bool bUseRefract );

	// 获得是否打开反射
	//bool GetUseReflect( void );

	// 获得是否打开折射
	//bool GetUseRefract( void );

	//void SetUseDepthMap( bool bUseDepthMap );

	//bool GetUseDepthMap( void );

	// 获得反射RTT对象
	//RenderTargetReflect* GetRenderTargetReflect( void );

	// 获得折射RTT]对象
	//RenderTargetRefract* GetRenderTargetRefract( void );

	// 设置反射RTT对象
	//bool SetRenderTargetReflect( RenderTargetReflect* prtt );

	// 设置折射RTT对象
	//bool SetRenderTargetRefract( RenderTargetRefract* prtt );

	// 设置水体的颜色
	void SetWaterColor( const SColor& color );

	// 获得水体的颜色
	const SColor& GetWaterColor( void )
    {
        return m_clrWater;
    }

	// 默认初始化
	virtual void InitDefault( void );

	// 设置水波扰动
	void SetWaveDistorsion( float fDistorsion );

	// 获得水波扰动
	float GetWaveDistorsion( void )
    {
        return m_fWaveDistorsion;
    }

	// 设置水流方向
	void SetFlowDirection( const Vector2Df& vDir );

	// 获得水流方向
	const Vector2Df& GetFlowDirection( void ) const
    {
        return m_vWaveDirection;
    }

	// 设置水流速度
	void SetFlowSpeed( float fSpeed );

	// 获得水流速度
	float GetFlowSpeed( void )
    {
        return m_fFlowSpeed;
    }

	//virtual RenderTargetScreenCompositor* GetRenderTargetScreenCompositor( void ) { return m_pRTTUnderWaterCompositor; }

	// 从XML加载水
	virtual	bool CreateFromXML( CMarkupSTL& xml );

	// 将水保存到XML
	virtual bool SaveToXML( CMarkupSTL& xml );

	// 当前是否调试水
	//virtual void					SetDebugWater( bool b );

	// 设置水纹理的U缩放
	void SetUScale( float fUScale );

	// 设置水纹理的V缩放
	void SetVScale( float fVScale );

	// 获得水纹理的U缩放
	float GetUScale( void )
    {
        return m_fUScale;
    }

	// 获得水纹理的V缩放
	float GetVScale( void )
    {
        return m_fVScale;
    }

	// 设置是否加入地形高度图到水体的计算
	void SetIsCalByHeightMap( bool b );

	// 获得是否加入地形高度图到水体的计算
	bool GetIsCalByHeightMap( void )
    {
        return m_bIsCalByHeightMap;
    }

	// 设置水体的显示模式
	void SetWaterCmdType( eWaterCmdType eWCT );

	// 获得水体显示的模式
	eWaterCmdType GetWaterCmdType( void )
    {
        return m_eWaterCmdType;
    }

	// 刷新地形高度图到水体
	//virtual void RefreshHeightMapToWater( void );

	// 自动切换材质
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
	// 渲染预处理
	virtual void PreRender(CCamera* pCamera);

	// 重建水体网格数据
	void RebuildRenderData( bool bNeedRebuildRenderData = false );

	// 清除水体RTT
	void ClearWaterRTT( void );

	// 销毁水底效果合成器
	void ClearUnderWaterCompositor( void );

    virtual void _GetPrimitive();

protected:
	int								m_nGridCountX;					// X轴网格数
	int								m_nGridCountZ;					// Z轴网格数
	f32								m_fGridWidthX;					// 网络X轴长度     缩放因子
	f32								m_fGridWidthZ;					// 网络Z轴长度     缩放因子
	f32								m_fWidth;						// 水体X方向总宽度
	f32								m_fHeight;						// 水体Z方向总长度
	f32								m_fDepth;						// 海拔
	f32								m_fUScale;						// 纹理U向缩放值
	f32								m_fVScale;						// 纹理V向缩放值
	Vector3Df						m_vCenter;						// 水体网格中心坐标点
	SColor							m_clrWater;						// 水体颜色
	//RenderTargetReflect*			m_pRTTReflect;					// 反射RTT
	//RenderTargetRefract*			m_pRTTRefract;					// 折射RTT
	bool							m_bUnderWater;					// 是否在水底
	//WaterManualGPUDataParamSource*  m_pGPUDataParmSource;			// GPU参数
	float							m_fWaveDistorsion;				// 水波扰动参数
	Vector2Df						m_vWaveDirection;				// 水波流动方向
	float							m_fFlowSpeed;					// 水波流动速度
	//ScreenCompositor*				m_pUnderWaterCompositor;		// 水底特效合成器
	//RenderTargetScreenCompositor*	m_pRTTUnderWaterCompositor;		// 水底特效RTT
	bool							m_bIsCalByHeightMap;			// 设置海水是否需要地形高度图进行计算
	eWaterCmdType					m_eWaterCmdType;

	ITexture*						m_pHeightMapTex;

	//--------------------------------------------------
	//! add by hjj 2010-9-14
	CColorValue						m_fogColorUnderWater;			// 水下雾的颜色
	FogMode							m_fogModeUnderWater;			// 水下雾的模式
	float							m_fDensityUnderWater;			// 水下雾的密度
	float							m_fFogStartUnderWater;			// 水下雾的起始
	float							m_fFogEndUnderWater;			// 水下雾的结束
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
	NormalSceneRenderProcess* m_pWaterReflectProcess;   //反射
	NormalSceneRenderProcess* m_pWaterRefactProcess;    //折射

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

    //更新水体
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