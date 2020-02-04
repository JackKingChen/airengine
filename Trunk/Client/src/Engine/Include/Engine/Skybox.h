#pragma once 

#include "Common.h"
#include "ITexture.h"
#include "Vector3D.h"
#include "Matrix4.h"
#include "Camera.h"
#include "IVertexBuffer.h"
#include "MarkupSTL.h"
#include "Renderable.h"
#include "ImageSet.h"
#include <assert.h>
//#include "../RenderSystem/D3D9Material.h"

class CTechnique;

struct SKYBOX_VERTEX
{
	Vector3Df	pos;
	Vector3Df	tex;
	SColor		color;
};

//struct SKYBOX_VERTEX
//{
//	Vector4f	pos;
//	Vector3Df	tex;
//};


enum ESkyBoxSides 
{
    ESIDE_TOP		= 0,	// posY
    ESIDE_BOTTOM	= 1,	// negY
    ESIDE_LEFT		= 2,	// negX
    ESIDE_RIGHT		= 3,	// posX
    ESIDE_FRONT		= 4,	// posZ
    ESIDE_BACK		= 5,	// negZ
    ESIDE_MAX
};

enum SkyType
{
	eST_BOX			= 0,
	eST_CUBEMAP		= 1,
	eST_SKYDOME		= 2,
};

class EngineExport CSkyBox : public Renderable
{
public:
    CSkyBox();
    virtual ~CSkyBox();

    //virtual void	Render(CCamera* pCamera);

    virtual void	Clear();
  
    bool			Create(const Vector3Df& vSize);

    virtual bool	LoadFromXML(CMarkupSTL& xml);

	virtual bool	SaveToXML(CMarkupSTL& xml);

    //载入每个面的纹理
    //bool  LoadSideTexture(ESkyBoxSides side, LPCTSTR szFileName);
    void			SetBoxSize(const Vector3Df& vSize);

    void			SetBoxPos(const Vector3Df& vPos)
    {
        m_vPos = vPos;
        m_vPosOrgin = m_vPos;
    }

    virtual const Matrix4f& GetWorldMatrix() const
    {
        return m_matWorld;
    }

    //获得物体当前的世界坐标
    void			GetWorldMatrix(Matrix4f* matWorld) const { *matWorld = m_matWorld; }

    void			SetWorldMatrix(const Matrix4f* matWorld) { m_matWorld = *matWorld; }

    virtual u32	GetRenderTriangles() { return 10; }

    f32				GetWidth() const { return m_vSize.x; }

    virtual f32		GetMaxZ() { return 0.0; }

    virtual void	SetMaterial(IMaterial* pMaterial) { m_pMaterial = pMaterial; }

    virtual Vector3Df* GetCenter() { return &m_vPos; }

	bool			LoadSideTexture(ESkyBoxSides side, const char* szFileName);

	virtual const AIRString& GetSideTexture( ESkyBoxSides side ) const
	{
		assert( side < 6 && "side 超过 6!" );
		return m_szSkyBox[ side ];
	}

	virtual void	InitDefault( void );

	//--------------------------------------------------
	//! add by hjj 2010-6-30
	bool			CreateCubeMapSkyBox( void );

	//void			DrawCubeMapSkyBox( void );

	//void			DrawNormalSkyBox( void );

	void			SetRenderByCubeMap( bool b );

	void			ResetCubeMapSkyBoxData( void );

	void			SetCubeMapTexture( const AIRString& szFile );

	const AIRString& GetCubeMapTexture( void ) const
	{
		return m_szCubeMapTextureFile;
	}

	bool			GetRenderByCubeMap( void )
	{
		return m_bRenderByCubeMap;
	}

	//virtual void	RenderToDepthMap(CCamera* pCamera, CPass* pPass);
	//--------------------------------------------------

	virtual void	CreateSkyDome( float fRadius, int nMeridian, int nLatitude );

	//virtual void	DrawSkyDome( void );

	void			SetSkyDomeTexture( const AIRString& szFile );

	virtual void	SetSkyType( SkyType st );

	AIRString		GetSkyTypeString( SkyType st );

    virtual bool Prepare();

protected:
    virtual void	PreRender(CCamera* pCamera);

    virtual void _GetPrimitive();

private:
    bool			CreateVertexBuffer();   

	//void			GenTechniqueRenderToDepthMap( void );


private:
    ITexture*		m_pSideTexture[6];	// 
    IVertexBuffer*	m_pVertexBuffer;	// 
    Vector3Df		m_vSize;			// z, x, y分别为长，宽，高
    Vector3Df		m_vPos;				// skybox的中心点
    Vector3Df		m_vPosOrgin;		// 原始点

	bool			m_bUseOneTex;
	Rectf			m_rcUV[6];
	CImageSet*		m_pImgCube;

	AIRString     m_szSkyBox[6];
	float			m_fUVOffset;

	bool			m_bRenderByCubeMap;

	AIRString		m_szCubeMapTextureFile;

	CTechnique*		m_pTechniqueSkyBoxRenderToDepthMap;
	CTechnique*		m_pTechniqueCubeMapRenderToDepthMap;

	SkyType			m_SkyType;
	int				m_nMeridian;
	int				m_nLatitude;
	AIRString		m_szSkyDomeTextureFile;
};
