// ***************************************************************
//  �ļ�: PageStaticGeometory.h
//  ����: 2010-5-27
//  ����: huangjunjie
//  ˵��: ��̬������ҳ
// ***************************************************************

#pragma once

#ifndef __PageStaticGeometory_H__
#define __PageStaticGeometory_H__

//#include "MemoryAllocatorConfig.h"
#include "Renderable.h"
#include "Engine.h"
//#include "PlantObject.h"
#include "GPUDataParam.h"
//#include "ManualGPUDataParamSource.h"
#include "IteratorWrappers.h"


// �����ʼ����Ϣ
class EngineExport ObjectInitInfo
{
public:
	ObjectInitInfo() : m_vPosition(Vector3Df::ZERO), m_vScale(1.0f, 1.0f, 1.0f), m_fAngle(0.0f), m_fBaseWidth(1.0f), m_fBaseHeight(1.0f)	{}

	inline void operator = ( const ObjectInitInfo& other )
	{
		m_vScale = other.m_vScale;
		m_vPosition = other.m_vPosition;
		m_fAngle = other.m_fAngle;
		m_fBaseWidth = other.m_fBaseWidth;
		m_fBaseHeight = other.m_fBaseHeight;
	}

	void normalize()
	{
		m_vScale.x *= m_fBaseWidth;
		m_vScale.y *= m_fBaseHeight;
		m_vScale.z *= m_fBaseWidth;
		m_fBaseWidth = m_fBaseHeight = 1.0f;
	}

	Vector3Df	m_vPosition;	// ��������
	Vector3Df	m_vScale;		// ����
	float		m_fAngle;		// ��Y����ת�ĽǶ�
	float		m_fBaseWidth;	// ��׼���
	float		m_fBaseHeight;	// ��׼�߶�
};

typedef AIRVector< Vector3Df >		VEC_OBJECT_OFFSET;
typedef AIRVector< ObjectInitInfo >		LST_OBJECT_INIT_INFO;

enum ePlantFaceType
{
    ePFT_Always_Face_Camera = 0,	// ��billboard����ʽ,�������������
    ePFT_Horz_Face_Camera = 1,		// ��billboard����ʽ,��ֱ�ڵ��沢��������� 
    ePFT_Never_Face_Camera = 2,		// ֲ����Ķ��㲻�����ӽǵı仯����
};


enum PSG_ANIM_TYPE
{
	ePsgAnimNone,			// û�ж���
	ePsgAnimSameOffset,		// ���е�����ʹ����ͬ��ƫ���� (CPU����,������,��)
	ePsgAnimDiffOffset,		// ÿ������ʹ�ø��Ե�ƫ���� (CPU����,������,��)
	ePsgAnimShearMatrix,	// ʹ���б������в���(ֻ�����ڷ���Y=0ƽ���ϵ�����) (CPU����,��������,��)
	ePsgAnimUseVS,			// ʹ��VS���еĶ������� (GPU����,��������,��)
};



// ��̬ҳ������
class EngineExport PageStaticGeometory : public Renderable
{
public:
	PageStaticGeometory();

	virtual ~PageStaticGeometory( void );

	// ��Ⱦ
	//virtual void Render(CCamera* pCamera);

	// ���
	virtual void Clear();

	virtual const Matrix4f& GetWorldMatrix() const;

	// ÿ֡����
	//virtual void Update( float fTimeElapse );

	// ����ҳ����������
	//virtual void GenPageStaticGeometryData( const LST_OBJECT_INIT_INFO& vtObjectInitInfo, PlantObject* pObjRef );

	// �����еĶ�����ж�������
	virtual void AnimAllObjects( const Vector3Df& vOffset );

	// �����еĶ�����ж�������
	virtual void AnimAllObjects( const VEC_OBJECT_OFFSET& vtOffset );

	// �����еĶ�����ж�������
	virtual void AnimAllObjects( const Matrix4f& matShear );

	// �����еĶ�����ж�������
	virtual void AnimAllObjects( CCamera* pCamera );

	// ���ò���
	virtual void SetMaterial(IMaterial* pMaterial);

	// ��������Ķ�������
	virtual void SetPsgAnimType( PSG_ANIM_TYPE pat );

	virtual void SetMaxXOffset( float fMaxXOffset )
	{
		m_fXMaxOffset = fMaxXOffset;
	}

	virtual void SetMaxZOffset( float fMaxZOffset )
	{
		m_fZMaxOffset = fMaxZOffset;
	}

	virtual float GetMaxXOffset( void )
	{
		return m_fXMaxOffset;
	}

	virtual float GetMaxZOffset( void )
	{
		return m_fZMaxOffset;
	}

	void SetOffsetFreq( float fFreq )
	{
		m_fOffsetFreq = fFreq;
	}

	float GetOffsetFreq( void )
	{
		return m_fOffsetFreq;
	}

	void SetObjectWidth( float width )
	{
		m_fObjectWidth = width;
	}

	float GetObjectWidth( void )
	{
		return m_fObjectWidth;
	}

	void SetObjectHeight( float height )
	{
		m_fObjectHeight = height;
	}

	float GetObjectHeight( void )
	{
		return m_fObjectHeight;
	}

	void SetQuadFaceCountPerObject( int nQuadFaceCountPerObj )
	{
		m_nQuadFaceCountPerObj	= nQuadFaceCountPerObj;
	}

    //��Ⱦ��GBuffer��technique�������ӳ���Ⱦ
    //virtual void GenRenderToGBufferTechnique();

    //virtual void RenderToShadowMap(CCamera* pCamera);

    virtual void SetAcceptLighting(bool bAccept);

    virtual void ResetCurrentTechnique();

    //PSGManualGPUDataParamSource* GetGPUDataParamSource()
    //{
    //    return m_gpuDataParamSource;
    //}
public:
	// for editor

	// �������
	virtual void AddPlantObject( const LST_OBJECT_INIT_INFO& vtObjectInitInfo, float fWidth, float fHeight, int nQuadFaceCountPerObj, ePlantFaceType ePFT );

	void CreateInternal();

	// ɾ������
	virtual void DelPlantObject( const Vector3Df& vCenterPos, float fRadius );

	// ����ҳ�������������ɵ����������
	virtual void SetMaxObjectCount( size_t nMaxObjectCount );

	// �ؽ�����������
	virtual void RebuildGeometryData( void );

	virtual void BuildGeometryDataFromBin( u8**pData );

	virtual bool GetIsCurPage( float fWidth, float fHeight, int nQuadFaceCountPerObj, ePlantFaceType ePFT, AIRString& szTexFileName );

	// ��õ�ǰֲ���������
	size_t		 GetObjectCount( void )
	{
		return m_nObjectCount;
	}

	// ���浽XML
	virtual bool SaveToXML( CMarkupSTL& xml );

	virtual bool SaveToBinFile( std::ofstream& fout );

	// ��XML����
	virtual bool CreateFromXML( CMarkupSTL& xml );

	virtual bool CreateFromBinData( u8** pData );

	// ���ö��������
	virtual void SetObjectTexture( const AIRString& szTex );

	// ��ö��������
	virtual const AIRString& GetObjectTexture( void ) const
	{
		return m_szPlantTexFile;
	}

	// ���ƶ����ʼ��������Ϣ
	virtual void CopyObjectInitInfo( LST_OBJECT_INIT_INFO& vtDstObjectInitInfo, const Vector3Df& vNewCenterPosY );

    virtual bool Prepare();

	VectorIterator<LST_OBJECT_INIT_INFO> GetObjectInitInfo()
	{
		return VectorIterator<LST_OBJECT_INIT_INFO>(m_vtObjectInitInfo);
	}

	const AIRString& GetPlantTexFile() const
	{
		return m_szPlantTexFile;
	}

	void ClearAllObjectInitInfo()
	{
		m_vtObjectInitInfo.clear();
	}

	void AddCluster( const ObjectInitInfo& objInitInfo )
	{
		m_vtObjectInitInfo.push_back( objInitInfo );
	}

	int GetClusterCount( void )
	{
		return m_vtObjectInitInfo.size();
	}

	const Vector3Df& GetObjectInitCenter() const
	{
		return m_vObjectInitCenter;
	}

	void SetObjectInitCenter( const Vector3Df& v )
	{
		m_vObjectInitCenter = v;
		//m_worldPosition = v;
	}

	AIRString& GetNodeName()
	{
		return m_szNodeName;
	}

    //const Vector3Df& GetInitWorldPosition() const
    //{
    //    return m_worldPosition;
    //}

protected:
    // ��ȾԤ����
    virtual void PreRender(CCamera* pCamera);

    virtual void _GetPrimitive();




protected:
	LST_OBJECT_INIT_INFO			m_vtObjectInitInfo;			// ��¼ֲ�����λ��,��С,�������Ϣ������
	size_t							m_nVertexCountPerObject;	// ÿ��ֲ����Ķ�������
	size_t							m_nIndexCountPerObject;		// ÿ��ֲ�������������
	size_t							m_nObjectCount;				// ֲ��������
	bool							m_bHasGenVertexData;		// �Ƿ��Ѿ�������������
	Matrix4f						m_matShear;					// �����б����
	//PSGManualGPUDataParamSource*    m_gpuDataParamSource;		// gpu����
	float							m_fXMaxOffset;				// x��������ƫ����
	float							m_fZMaxOffset;				// z��������ƫ����
	VEC_OBJECT_OFFSET				m_vtObjectOffsetInit;		// 
	VEC_OBJECT_OFFSET				m_vtObjectOffsetCur;		// 
	PSG_ANIM_TYPE					m_PsgAnimType;				// ������������
	float							m_fObjectHeight;			// ֲ����ĸ߶�
	float							m_fObjectWidth;				// ֲ����Ŀ��
	ePlantFaceType					m_PlantFaceType;			// ֲ����ĳ���ʽ
	size_t							m_nMaxObjectCount;			// ������������
	size_t							m_nQuadFaceCountPerObj;		// ÿ��������ı�����
	AIRString						m_szPlantTexFile;
    ITexture*                       m_pPlantTexture;
	float							m_fOffsetFreq;
private:
	float							m_fXPos;
	float							m_fZPos;
	//bool							m_bHasUpdated;
	Vector3Df						m_vObjectInitCenter;
	AIRString						m_szNodeName;
    //Vector3Df                       m_worldPosition;     //��������

};//! end class PageStaticGeometory


typedef AIRList<PageStaticGeometory*>				LST_PSG;
typedef std::map<AIRString, LST_PSG*>				MAP_MATERIAL_PSG_LIST;
typedef std::map<int, MAP_MATERIAL_PSG_LIST*>		MAP_TRUNK_MATERIAL_PSG_LIST;
//
//class EngineExport PsgManager
//{
//public:
//	PsgManager( void ) : m_nID(0), m_bShowAllPSG(true), m_bIsTrunkPSG(false), m_nTrunkRow(0), m_nTrunkCol(0)
//	{
//
//	}
//
//	virtual ~PsgManager( void )
//	{
//
//	}
//
//	static PsgManager* GetSingleton( void )
//	{
//		static PsgManager inst;
//		return &inst;
//	}
//
//	PageStaticGeometory* CreatePageStaticGeometry( void );
//
//	void ClearPageStaticGeometry( PageStaticGeometory* pPsg );
//
//	void ClearAllPageStaticGeometry( void );
//
//	PageStaticGeometory* GetPageStaticGeometry( const char* szName );
//
//	bool SaveToXML( const char* szFile );
//
//	bool SaveToBinFile( const char* szFile );
//
//	bool CreateFromXML( CMarkupSTL& xml );
//
//	bool LoadFromFile( const char* szFile );
//
//	bool LoadFromBinFile( const char* szFile );
//
//	const AIRString& GetEditFileName( void ) const
//	{
//		return m_szFile;
//	}
//
//	void SetEditFileName( const AIRString& szFile )
//	{
//		m_szFile = szFile;
//	}
//
//	bool IsEmpty( void )
//	{
//		return (m_lstPSG.size() == 0);
//	}
//
//	void SetShowBoundingBox( bool b );
//
//	void SetShowAllPSG( bool bShow );
//
//	bool GetShowAllPSG( void )
//	{
//		return m_bShowAllPSG;
//	}
//
//	void SetParmToAllPlant( float fXMaxOffset, float fZMaxOffset, float fOffsetFreq );
//
//	void UpdateAllPageStaticGeometory( float fFrameTime );
//
//	//void GenerateOptimiseTrunkPSG( int nRowCount, int nColCount );
//
//	void DestroyAllPSG();
//
//	int GetTrunkRow()
//	{
//		return m_nTrunkRow;
//	}
//
//	int GetTrunkCol()
//	{
//		return m_nTrunkCol;
//	}
//
//    void ResetAllPlantsTechnique();
//
//protected:
//	LST_PSG			m_lstPSG;
//	AIRString		m_szFile;
//	u32			m_nID;
//	bool			m_bShowAllPSG;
//	bool			m_bIsTrunkPSG;
//	int				m_nTrunkRow;
//	int				m_nTrunkCol;
//};
//
//
////#define PSG_MANAGER (PsgManager::GetSingleton())
//










#endif //! __PageStaticGeometory_H__