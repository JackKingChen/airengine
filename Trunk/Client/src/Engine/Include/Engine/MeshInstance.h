/********************************************************************
	created:	2010/01/28
	created:	28:1:2010   12:26
	filename: 	d:\My Codes\client\Current\Engine\Engine\Include\MeshInstance.h
	file path:	d:\My Codes\client\Current\Engine\Engine\Include
	file base:	MeshInstance
	file ext:	h
	author:		liangairan
	
	purpose:	一个mesh的实例，共享AIRSubMesh的指针
*********************************************************************/

#pragma once 

#include "AIRSubMesh.h"
//#include "ManualGPUDataParamSource.h"

class CModelInstance;

//#define TEXTURE_RENDER_GROUP

struct TexLuminanceInfo 
{
	CColorValue		m_Color;		//颜色
	AIR::Vector4f	m_vStrength;	//强度
};


typedef std::map< float, TexLuminanceInfo >			TEX_LUMINANCE_INFO_MAP;
typedef TEX_LUMINANCE_INFO_MAP::iterator			TEX_LUMINANCE_INFO_MAP_ITOR;



class EngineExport CMeshInstance : public Renderable
{
public:
    CMeshInstance();
    CMeshInstance(CAIRSubMesh* pSubMesh, CModelInstance* pOwner);
    virtual ~CMeshInstance();

    //virtual void				Render(CCamera* pCamera);

    CAIRSubMesh*				GetMeshRes()
    {
        return m_pSubMesh;
    }

    virtual void				SetTransparent(bool bAlpha = true, u8 nAlpha = 128);

    virtual void				GenRenderToShadowTechnique();

    //virtual void				RenderToShadowMap(CCamera* pCamera);

    virtual bool				IsPick(const Vector3Df& vOrg, const Vector3Df& vPickDir, const Matrix4f& matTransform);

	virtual bool				IsPick(const Vector3Df& vOrg, const Vector3Df& vPickDir, const Matrix4f& matTransform, float& distFromEyeToObject );

    virtual void				AcceptShadowmap(bool bAccept);

	//virtual bool				CreateHardwareSkinAnimMaterial( void );

    CModelInstance*				GetOwnerModelInstance()
    {
        return m_pOwnerModelInst;
    }

    virtual IVertexBuffer*		GetVertexBuffer()
    {
        return m_pSubMesh->GetVertexBuffer();
    }

    virtual IIndexBuffer*		GetIndexBuffer()
    {
        return m_pSubMesh->GetIndexBuffer();
    }

    //virtual void				CommitRenderBuffers();

    virtual void				CommitTransform();

    virtual void				PostRender(CCamera* pCamera);

    virtual const AABBBox3Df&   GetWorldBoundingBox(bool bTransform);

    virtual void				SetAcceptLighting(bool bAccept);
    
    /*
    CTechnique*					GetTechniqeuAcceptShadow()
    {
        return m_pTechniqueAcceptShadow;
    }

    CTechnique*					GetTechniqeuDeferredLighting()
    {
        return m_pTechniqueDeferredLighting;
    }
    */

    //virtual void				RenderToGBuffer(CCamera* pCamera, CPass* pPass);

	//virtual void				RenderToDepthMap(CCamera* pCamera, CPass* pPass);

    //virtual void				ResetCurrentTechnique();

    virtual const Matrix4f&     GetWorldMatrix() const;

	//设置当前使用纹理发光技术
	virtual void				SetUseTexLiminance( bool b );

	//获得当前是否使用纹理发光技术
	virtual bool				GetUseTexLiminance( void );

	//插入纹理发光的某时刻的颜色参数
	void						InsertTexLuminanceColorPair( float time, CColorValue& col, AIR::Vector4f& strength );

	//删除纹理发光的某时刻的颜色参数
	void						DeleteTexLuminanceColorPair( float time );

	//清空纹理发光的控制参数
	void						ClearTexLuminanceInfo( void );

	//设置是否全局网格高亮还是局部网格高亮
	void						SetTexLuminanceWholeMesh( bool b );

	//使用纹理高亮技术时的帧更新
	void						UpdateTexLuminance( void );

	//获得gpu参数
	//ManualGPUDataParamSource*	GetTexLuminanceGPUData( void );

	//创建纹理高亮的gpu参数
	//void						CreateTexLuminanceGPUData( void );

    //virtual void				GenRenderToGBufferTechnique();

    //virtual void				GenRenderToDepthMapTechnique();

    virtual void FillRenderContext(CCamera* pCamera, RenderContextQueue* pQueue, eRenderFlow flow);

	//virtual CVertexDeclaration* CreateVertexDecl(IGPUProgram* pVertexShader);
protected:
    virtual void				PreRender(CCamera* pCamera);

    virtual void				OnUpdateToRenderQueue(CCamera* pCamera, CRenderQueue* pQueue);
    
    CAIRSubMesh*				m_pSubMesh;							//子网格对象
    CModelInstance*				m_pOwnerModelInst;					//模型实例对象
	bool						m_bUseTexLuminance;					//是否使用TexLuminance
    
    //CTechnique*					m_pTechniqueAcceptShadow;			//接收阴影的technique
    //CTechnique*					m_pTechniqueDeferredLighting;		//延迟渲染technique
    //bool						m_bVerticeCaculated;				//每一帧是否已经更新了
    

    //创建渲染到GBuffer的technique
    //virtual void CreateTechniqueGBuffer();
	//void						CreateTechniqueDepthMap();

    virtual void _GetPrimitive();
private:
	int							m_nTexLuminanceWholeMesh;			//0-局部发光;1-整体高亮
	TEX_LUMINANCE_INFO_MAP		m_mapTexLuminanceInfo;				//TexLuminance信息表
	//TexLuminanceManualGPUDataSource* m_pGPUDataParmSource;			//TexLuminance的gpu参数
    
    bool           m_bNeedUpdateVertices;     //是否需要更新顶点
};
