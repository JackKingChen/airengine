/********************************************************************
	created:	2009/11/23
	created:	23:11:2009   15:47
	filename: 	d:\My Codes\Engine\Engine\Include\Billboard.h
	file path:	d:\My Codes\Engine\Engine\Include
	file base:	Billboard
	file ext:	h
	author:		liangairan
	
	purpose:	BILLBOARD�࣬��������ͷ������ֻ�Ǹ�����Ⱦ
*********************************************************************/
#ifndef __CBILLBOARD_H__
#define __CBILLBOARD_H__

#include "Renderable.h"

class ITexture;

enum eBillBoardCtreateType
{
	eBBCT_LeftTop,		// �����ϽǶ������
	eBBCT_RightTop,		// �����ϽǶ������
	eBBCT_LeftBottom,	// �����½Ƕ������
	eBBCT_RightBottom,	// �����½Ƕ������
	eBBCT_LeftCenter,	// ����߿��е����
	eBBCT_TopCenter,	// ���ϱ߿��е����
	eBBCT_RightCenter,	// ���ұ߿��е����
	eBBCT_BottomCenter,	// ���±߿��е����
	eBBCT_Center,		// �����Ķ���
};

enum eBillBoardFaceType
{
	//ʼ�����������
	eBBFT_AlwaysFaceCamera,

	//ƽ���ڹ�������(m_vCommonDir)�����������
	eBBFT_HorzLocalDirFaceCamera,

	//��ֱ�ڹ�������(m_vCommonDir)�����������
	eBBFT_VertLocalDirFaceCamera,
};

class EngineExport CBillboard : public Renderable
{
public:
    CBillboard();

    virtual ~CBillboard();

	void SetBBSize( float fWidth, float fHeight );
	

	float GetBBWidth( void )
	{
		return m_fBBWidth;
	}

	float GetBBHeight( void )
	{
		return m_fBBHeight;
	}

	void SetUseComColor( bool b )
	{
		m_bUseComColor = b;
		m_bNeedResetColor = true;
	}

	bool GetUseComColor( void )
	{
		return m_bUseComColor;
	}

	void SetComColor( const SColor& col )
	{
		m_ComColor = col;
		m_bNeedResetColor = true;
	}

	SColor GetComColor( void )
	{
		return m_ComColor;
	}

	void SetLTColor( const SColor& col )
	{
		m_LTColor = col;
		m_bNeedResetColor = true;
	}

	SColor GetLTColor( void )
	{
		return m_LTColor;
	}

	void SetRTColor( const SColor& col )
	{
		m_RTColor = col;
		m_bNeedResetColor = true;
	}

	SColor GetRTColor( void )
	{
		return m_RTColor;
	}

	void SetLBColor( const SColor& col )
	{
		m_LBColor = col;
		m_bNeedResetColor = true;
	}

	SColor GetLBColor( void )
	{
		return m_LBColor;
	}

	void SetRBColor( const SColor& col )
	{
		m_RBColor = col;
		m_bNeedResetColor = true;
	}

	SColor GetRBColor( void )
	{
		return m_RBColor;
	}

	void SetBillBoardCreateType( eBillBoardCtreateType eBBCT )
	{
		m_BillBoardCreateType = eBBCT;
	}

	eBillBoardCtreateType GetBillBoardCreateType( void )
	{
		return m_BillBoardCreateType;
	}

	void SetBillBoardFaceType( eBillBoardFaceType eBBFT )
	{
		m_BillBoardFaceType = eBBFT;
	}

	eBillBoardFaceType GetBillBoardFaceType( void )
	{
		return m_BillBoardFaceType;
	}

	void SetLocalDir( const Vector3Df& vLocalDir, bool bIsYLocalDir = true )
	{
		m_vLocalDir = vLocalDir;
		m_bIsYLocalDir = bIsYLocalDir;
	}

	void SetIsOrientAffectedByNode( bool b )
	{
		m_bIsOrientAffectedByNode = b;
	}

	bool GetIsOrientAffectedByNode( void )
	{
		return m_bIsOrientAffectedByNode;
	}

	//virtual void Update( float fTimeElapsed );

    virtual void Render(CCamera* pCamera);

	virtual void Clear();

	virtual void PreRender(CCamera* pCamera);

    virtual void PostRender(CCamera* pCamera);

    virtual IVertexBuffer* GetVertexBuffer()
    {
        return m_pVB;
    }

    virtual bool IsPick(const Vector3Df& vOrg, const Vector3Df& vPickDir);

    virtual bool IsPick(const Vector3Df& vOrg, const Vector3Df& vPickDir, float& distFromEyeToObject);

    virtual void CaculatePosition();

    const VERTEX_STANDARD& GetTopLeft() const
    {
        return m_TopLeft;
    }

    const VERTEX_STANDARD& GetTopRight() const
    {
        return m_TopRight;
    }

    const VERTEX_STANDARD& GetBottomLeft() const
    {
        return m_BottomLeft;
    }

    const VERTEX_STANDARD& GetBottomRight() const
    {
        return m_BottomRight;
    }

    void SetTopLeftUV(f32 fu, f32 fv);

    void SetTopRightUV(f32 fu, f32 fv);

    void SetBottomLeftUV(f32 fu, f32 fv);

    void SetBottomRightUV(f32 fu, f32 fv);

    void SetUseRenderer(bool bUseRenderer);

    void SetTexture(ITexture* pTexture);

    ITexture* GetTexture()
    {
        return m_pTexture;
    }

    void SetQuadUV(const Rectf& uvRect);

    const Vector3Df& GetRightDir() const
    {
        return m_vRight;
    }

    const Vector3Df& GetDownDir() const
    {
        return m_vDir;
    }

    void SetTopLeftPosition(const Vector3Df& position);

    void SetTopRightPosition(const Vector3Df& position);

    void SetBottomLeftPosition(const Vector3Df& position);

    void SetBottomRightPosition(const Vector3Df& position);

    virtual void SetTransparent(bool bAlpha = true, u8 nAlpha = 128);

    virtual const Matrix4f& GetWorldMatrix() const
    {
        return Matrix4f::IDENTITY;
    }

    virtual bool Prepare();
protected:
	virtual void CreateVertexBuffer( void );

    virtual void _GetPrimitive();

    IVertexBuffer*			m_pVB;
	Vector3Df				m_vLocalPos;
	Vector3Df				m_vLocalDir;
	float					m_fBBWidth;
	float					m_fBBHeight;
	SColor					m_ComColor;
	SColor					m_LTColor;
	SColor					m_RTColor;
	SColor					m_LBColor;
	SColor					m_RBColor;
	bool					m_bUseComColor;
	bool					m_bNeedResetColor;
	bool					m_bIsYLocalDir;
	bool					m_bIsOrientAffectedByNode;
	eBillBoardCtreateType	m_BillBoardCreateType;
	eBillBoardFaceType		m_BillBoardFaceType;
    bool                    m_bUseRenderer;
    ITexture*               m_pTexture;   //������Ⱦ��ʱ��Ҫ�õ������������Ⱦ���Ļ���ֱ����material
    Triangle3Df  m_tri1;   //����ʰȡ
    Triangle3Df  m_tri2;

    VERTEX_STANDARD  m_TopLeft;
    VERTEX_STANDARD  m_TopRight;
    VERTEX_STANDARD  m_BottomLeft;
    VERTEX_STANDARD  m_BottomRight;

    VERTEX_STANDARD  m_vertices[4];

    Vector3Df m_vDir;     //���µ�����
    Vector3Df m_vRight;   //���ҵ�����
};

/*
class EngineExport BillboardManager
{
	BillboardManager( void )
	{
		m_vtBillboard.clear();
	}

public:

	static BillboardManager* GetSingleton( void )
	{
		static BillboardManager inst;
		return &inst;
	}

	~BillboardManager( void );

	CBillboard* CreateDefaultBillboard( const AIRString& szName );

	void AddBillboard( CBillboard* pBB );

	CBillboard* GetBillboard( const AIRString& szName );

	void Clear();

	void UpdateAllBillboard( float fTimeElapse );

    

protected:
	typedef AIRVector<CBillboard*> BILLBOARD_ARRAY;
	typedef BILLBOARD_ARRAY::iterator BILLBOARD_ARRAY_ITER;

	BILLBOARD_ARRAY    m_vtBillboard;
};

//#define BILLBOARG_MANAGER (BillboardManager::GetSingleton())
*/

#endif //! __CBILLBOARD_H__