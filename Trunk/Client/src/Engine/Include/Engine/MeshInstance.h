/********************************************************************
	created:	2010/01/28
	created:	28:1:2010   12:26
	filename: 	d:\My Codes\client\Current\Engine\Engine\Include\MeshInstance.h
	file path:	d:\My Codes\client\Current\Engine\Engine\Include
	file base:	MeshInstance
	file ext:	h
	author:		liangairan
	
	purpose:	һ��mesh��ʵ��������AIRSubMesh��ָ��
*********************************************************************/

#pragma once 

#include "AIRSubMesh.h"
//#include "ManualGPUDataParamSource.h"

class CModelInstance;

//#define TEXTURE_RENDER_GROUP

struct TexLuminanceInfo 
{
	CColorValue		m_Color;		//��ɫ
	AIR::Vector4f	m_vStrength;	//ǿ��
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

	//���õ�ǰʹ�������⼼��
	virtual void				SetUseTexLiminance( bool b );

	//��õ�ǰ�Ƿ�ʹ�������⼼��
	virtual bool				GetUseTexLiminance( void );

	//�����������ĳʱ�̵���ɫ����
	void						InsertTexLuminanceColorPair( float time, CColorValue& col, AIR::Vector4f& strength );

	//ɾ���������ĳʱ�̵���ɫ����
	void						DeleteTexLuminanceColorPair( float time );

	//���������Ŀ��Ʋ���
	void						ClearTexLuminanceInfo( void );

	//�����Ƿ�ȫ������������Ǿֲ��������
	void						SetTexLuminanceWholeMesh( bool b );

	//ʹ�������������ʱ��֡����
	void						UpdateTexLuminance( void );

	//���gpu����
	//ManualGPUDataParamSource*	GetTexLuminanceGPUData( void );

	//�������������gpu����
	//void						CreateTexLuminanceGPUData( void );

    //virtual void				GenRenderToGBufferTechnique();

    //virtual void				GenRenderToDepthMapTechnique();

    virtual void FillRenderContext(CCamera* pCamera, RenderContextQueue* pQueue, eRenderFlow flow);

	//virtual CVertexDeclaration* CreateVertexDecl(IGPUProgram* pVertexShader);
protected:
    virtual void				PreRender(CCamera* pCamera);

    virtual void				OnUpdateToRenderQueue(CCamera* pCamera, CRenderQueue* pQueue);
    
    CAIRSubMesh*				m_pSubMesh;							//���������
    CModelInstance*				m_pOwnerModelInst;					//ģ��ʵ������
	bool						m_bUseTexLuminance;					//�Ƿ�ʹ��TexLuminance
    
    //CTechnique*					m_pTechniqueAcceptShadow;			//������Ӱ��technique
    //CTechnique*					m_pTechniqueDeferredLighting;		//�ӳ���Ⱦtechnique
    //bool						m_bVerticeCaculated;				//ÿһ֡�Ƿ��Ѿ�������
    

    //������Ⱦ��GBuffer��technique
    //virtual void CreateTechniqueGBuffer();
	//void						CreateTechniqueDepthMap();

    virtual void _GetPrimitive();
private:
	int							m_nTexLuminanceWholeMesh;			//0-�ֲ�����;1-�������
	TEX_LUMINANCE_INFO_MAP		m_mapTexLuminanceInfo;				//TexLuminance��Ϣ��
	//TexLuminanceManualGPUDataSource* m_pGPUDataParmSource;			//TexLuminance��gpu����
    
    bool           m_bNeedUpdateVertices;     //�Ƿ���Ҫ���¶���
};
