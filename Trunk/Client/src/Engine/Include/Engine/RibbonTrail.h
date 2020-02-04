// ***************************************************************
//  �ļ�: RibbonTrail.h
//  ����: 2010-1-23
//  ����: huangjunjie
//  ˵��: β��
// ***************************************************************


#pragma once

#ifndef __RibbonTrail_H_
#define __RibbonTrail_H_

#include "BillBoardChain.h"
#include "ColorValue.h"
#include "Color.h"
#include "ParticleIterator.h"
#include "EngineResource.h"

class EngineExport RibbonTrailResource : public Resource
{
public:
    RibbonTrailResource();
    virtual ~RibbonTrailResource();

    virtual bool Cache();

    size_t GetMaxElementsPerChain() const
    {
        return mMaxElementsPerChain;
    }

    size_t GetChainCount() const
    {
        return mChainCount;
    }

    bool IsUseTexCoords() const
    {
        return mUseTexCoords;
    }

    bool IsUseVertexColor() const
    {
        return mUseVertexColour;
    }

    const AIRString& GetTrailImageFile() const
    {
        return m_szTrailImage;
    }

    bool IsDrawWithBillboard() const
    {
        return mDrawWithBillboard;
    }
    
    void GetOtherTexCoordRange(f32& range1, f32& range2) const
    {
        range1 = mOtherTexCoordRange[0];
        range2 = mOtherTexCoordRange[1];
    }

    BillBoardChain::TexCoordDirection GetTextureCoordDir() const
    {
        return mTexCoordDir;
    }

    virtual s32 Release();

    virtual void Clear() {}

    void SetDrawWithBillboard(bool b)
    {
        mDrawWithBillboard = b;
    }

    const CColorValue& GetColorInit( s32 id ) const
    {
        return m_colInit[ id ];
    }

    const CColorValue& GetColorChange( s32 id ) const
    {
        return m_colChange[ id ];
    }

    const f32 GetWidthInit( s32 id ) const
    {
        return m_widthInit[ id ];
    }

    const f32 GetWidthChange( s32 id ) const
    {
        return m_widthChange[ id ];
    }

	const AIRString&  GetName() const { return m_strName; }
protected:
private:
    size_t mMaxElementsPerChain;       // ÿ�����ε�Ԫ�ص����ֵ
    size_t mChainCount;                // ��������
    bool mUseTexCoords;              // �Ƿ�ʹ����������
    bool mUseVertexColour;           // �Ƿ�ʹ�ö�����ɫ
    AIRString m_szTrailImage;
    bool mDrawWithBillboard;
    f32 mOtherTexCoordRange[2];     // ��һ���������귽��ķ�Χ
    BillBoardChain::TexCoordDirection mTexCoordDir;   // �������귽��

    //f32 m_fWidthInit;     //��β��ʼ���
    //f32 m_fWidthChange;
    //CColorValue m_colInit;  //��β��ʼ������ɫ
    //CColorValue m_colChange;

	AIRVector<CColorValue>    m_colInit;
	AIRVector<f32>			m_widthInit;
	AIRVector<CColorValue>    m_colChange;
	AIRVector<f32>			m_widthChange;
	AIRString     m_strName;
};

class EngineExport RibbonTrail : public BillBoardChain
{
public:
    // �仯������
    enum CHANGE_MASK
    {
        eCM_NO_CHANGE       = 0x00000000,   // û���޸�
        eCM_COLOR_CHANGE    = 0x00000001,   // �޸���ɫ
        eCM_WIDTH_CHANGE    = 0x00000002,   // �޸Ĵ�С
        eCM_ALL_CHANGE      = 0xffffffff,   // ȫ���޸�
    };


public:
	//RibbonTrail( const char* szName );

    RibbonTrail(
        //const char* szName,
        size_t maxElements = 20, 
        size_t numberOfChains = 1, 
        bool useTextureCoords = true, 
        bool useColours = true);

	virtual ~RibbonTrail();

	// ���ù���������������� (ÿ����������ͬ�Ĳ���). 
	virtual void SetNumberOfChains(size_t numChains);

    // �������ε�Ԫ�صĳ�ʼ��ɫ
    void SetInitialColour(size_t chainIndex, const SColor& col);

    // �������ε�Ԫ�صĳ�ʼ��ɫ
    void SetInitialColour(size_t chainIndex, u8 r, u8 g, u8 b, u8 a = 255);

    // �������ε�Ԫ�صĳ�ʼ��ɫ
    void SetInitialColour(size_t chainIndex, const CColorValue& col );

    // ������ε�Ԫ�صĳ�ʼ��ɫ
    const SColor& GetInitialColour(size_t chainIndex) const;

    // ��������Ԫ��ÿ�����ɫ�仯ֵ
    void SetColourChange(size_t chainIndex, f32 deltaR, f32 deltaG, f32 deltaB, f32 deltaA );

    // ��������Ԫ��ÿ�����ɫ�仯ֵ
    void SetColourChange(size_t chainIndex, const CColorValue& deltaColor );

    // �������Ԫ��ÿ�����ɫ�仯ֵ
    void GetColourChange(size_t chainIndex, f32& deltaR, f32& deltaG, f32& deltaB, f32& deltaA );

    // �������Ԫ��ÿ�����ɫ�仯ֵ
    const CColorValue&  GetColourChange(size_t chainIndex);

    // �������ε�Ԫ�صĳ�ʼ��С
    void SetInitialWidth(size_t chainIndex, f32 width);

    // ������ε�Ԫ�صĳ�ʼ��С
    f32 GetInitialWidth(size_t chainIndex) const;

    // �������ε�Ԫ�ص�ÿ���С�仯ֵ
    void SetWidthChange(size_t chainIndex, f32 widthDeltaPerSecond);

    // ������ε�Ԫ�ص�ÿ���С�仯ֵ
    f32 GetWidthChange(size_t chainIndex) const;

    // �������ε�׷�ٽڵ�
    void SetTrackNode( size_t chainIndex, CSceneNode* pNode );

    // �������ε��޸�����
    void SetChangeMask( size_t chainIndex, u32 changeMask );

    // ��Ⱦ
    //virtual void                Render(CCamera* pCamera);

    // ���
    virtual void Clear();

    // ����
    virtual void Update( f32 fTimeElapsed );

    void UpdateByParticle(f32 fTimeElapsed, CParticleIterator& particleIter);

    // ���¶��㻺��
    virtual void UpdateVertexBuffer(CCamera* cam);

    void SetDrawWithBillboard( bool b );

	bool IsDrawWithBillboard( void )
	{
		return mDrawWithBillboard;
	}

    void SetMaxChainWidth( f32 fWidth )
    {
        for ( size_t i = 0; i < mInitWidths.size(); ++i )
        {
            mInitWidths[i] = fWidth;
        }
    }

    void SetChainWidthChange( f32 fDlt )
    {
        for ( size_t i = 0; i < mWidthChanges.size(); ++i )
        {
            mWidthChanges[i] = fDlt;
        }
    }

    // ���ù���������������� (ÿ����������ͬ�Ĳ���). 
    void ResizeTrailChain(size_t numChains, f32 fMaxWidth, f32 fDltWidth )
    {
        mTrackNodes.clear();
        mInitColors.clear();
        mInitWidths.clear();
        mColorChanges.clear();
        mWidthChanges.clear();
        mChangeMasks.clear();
        m_lstTrackParticle.clear();
        mTrackNodes.resize( numChains, NULL );
        mInitColors.resize( numChains, SColor(255,255,255,255) );
        mInitWidths.resize( numChains, fMaxWidth );
        mColorChanges.resize( numChains, CColorValue(-0.1f, 0.0f, 0.0f, 0.0f) );
        mWidthChanges.resize( numChains, fDltWidth );
        mChangeMasks.resize( numChains, eCM_ALL_CHANGE );
        m_lstTrackParticle.resize( numChains, NULL );
    }

    // ��������
    //virtual void                SetTrackParticle(/*size_t chainIndex, */CParticle* pParticle);

    //virtual void                NotifyParticleDestory(CParticle* pParticle);


	virtual bool LoadFromFile( const char* szFile );

	virtual bool SaveToFile( const AIRString& szFile );

	void SetTrailImage( const AIRString& szFileName );

	inline AIRString GetTrailImage( void ) const
	{
		if( m_pResource )
			return m_pResource->GetTrailImageFile();
		return "";
	}

	void SetPauseAddElement( bool bPauseAddElem )
	{
		m_bPauseAddElement = bPauseAddElem;
	}

	bool IsCompleted( void );

	virtual void Start();

	void SetEnableTimeLength(f32 fTime)
	{
		m_fEnableTimeLen = fTime;
	}

	f32 GetEnableTimeLength() const
	{
		return m_fEnableTimeLen;
	}

	void SetAutoDetach( bool b )
	{
		m_bAutoDetach = b;
	}

	bool GetAutoDetach( void )
	{
		return m_bAutoDetach;
	}

    virtual bool Prepare();

    virtual const Matrix4f& GetWorldMatrix() const
    {
        return Matrix4f::IDENTITY;
    }
	//void Render(CCamera* pCamera);

    virtual const AABBBox3Df& GetWorldBoundingBox(bool bTransform)
    {
		return m_WorldAABB;
    }

    

protected:
    // ��Ⱦǰ�Ĵ���
    //virtual void                PreRender(CCamera* pCamera);

    // �޸�Ԫ������ 
    virtual void                ChanageElem( size_t chainIndex, Element& elem );

	virtual void				InitImpl( 
		const AIRString&  name, 
		size_t              maxElements = 20, 
		size_t              numberOfChains = 1, 
		bool                useTextureCoords = true, 
		bool                useColours = true, 
		bool                dynamic = true);


private:
    typedef AIRVector<CSceneNode*>    VT_TRACK_NODES;
    typedef AIRVector<ChainSegment*>  VT_NODE_CHAINS;
    typedef AIRVector<SColor>         VT_INIT_COLORS;
    typedef AIRVector<f32>          VT_INIT_WIDTHS;
    typedef AIRVector<CColorValue>    VT_COLOR_CHANGES;
    typedef AIRVector<f32>          VT_WIDTH_CHANGES;
    typedef AIRVector<CHANGE_MASK>    VT_CHANGE_MASKS;

    typedef AIRList<CParticle*>       PARTICLE_LIST;
    typedef PARTICLE_LIST::iterator     Particle_Iter;

    VT_TRACK_NODES              mTrackNodes;
    VT_INIT_COLORS              mInitColors;
    VT_INIT_WIDTHS              mInitWidths;
    VT_COLOR_CHANGES            mColorChanges;
    VT_WIDTH_CHANGES            mWidthChanges;
    VT_CHANGE_MASKS             mChangeMasks;
    PARTICLE_LIST               m_lstTrackParticle;   //Ӱ��β����particle
    bool                        mDrawWithBillboard;

	//AIRString					m_szTrailImage;

	bool						m_bPauseAddElement;
	f32						m_fEnableTimeLen;     
	f32						m_fRenderTimeLen;
	bool						m_bAutoDetach;
    RibbonTrailResource*        m_pResource;
    //bool  m_bPrepared;   //�Ƿ��Ѿ�׼����
    //bool  m_bIni

    
}; //! end class RibbonTrail







class EngineExport RibbonTrailManager : public Singleton<RibbonTrailManager>
{
    BE_SINGLETON(RibbonTrailManager);
public:
	~RibbonTrailManager();

	static RibbonTrailManager* GetInstance()
	{
		static RibbonTrailManager inst;
		return &inst;
	}

	RibbonTrail*	CreateRibbonTrail(const char* szName, const char* szFilename);

	void			DestroyRibbonTrail(unsigned int nID);

	void			DestroyRibbonTrail(const AIRString& strName);

	void			UpdateAllRibbonTrail(f32 fTimeElapsed);

	void			Clear();

	void			AddRibbonTrail(RibbonTrail* pRenderable);

	RibbonTrail*	GetRibbonTrail(const AIRString& strName);

	RibbonTrail*	GetRibbonTrail(unsigned int nID);

private:

	typedef AIRVector<RibbonTrail*>		OBJECT_ARRAY;
	typedef OBJECT_ARRAY::iterator			OBJECT_ARRAY_ITER;
	OBJECT_ARRAY							m_vtObject;
	unsigned int							m_nCounter;    //������
};

//#define RIBBONTRAIL_MANAGER (RibbonTrailManager::GetInstance())



#endif //! end __RibbonTrail_H_