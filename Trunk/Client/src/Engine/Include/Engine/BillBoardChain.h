// ***************************************************************
//  �ļ�: BillBoardChain.h
//  ����: 2010-1-21
//  ����: huangjunjie
//  ˵��: billboard chain
// ***************************************************************


#pragma once

#ifndef __BillBoardChain_H_
#define __BillBoardChain_H_

#include "Renderable.h"
#include "IVertexBuffer.h"
#include "IIndexBuffer.h"
#include "EngineConfig.h"
#include "Engine.h"

#include <vector>
#include <string>

class Renderable;
class IMaterial;
class CCamera;
class IVertexBuffer;
class IIndexBuffer;
class BillboardChainData;


#define INVALID_VERTEX_DECL -1


class EngineExport BillBoardChain : public Renderable
{
public:
    // ���������ʹ�õ��������귽��
    enum TexCoordDirection
    {
        TCD_U = 0,
        TCD_V = 1,
    };

    // ���������Ԫ����Ϣ
    class EngineExport Element
    {
    public:
        Element();
        Element(
            AIR::Vector3Df  _position,
            float           _width,
            float           _texCoord,
            SColor          _colour,
            AIR::Vector3Df  _dir );

    public:
        AIR::Vector3Df      position;   // ����
        float               width;      // ���
        float               texCoord;   // �������귶Χֵ
        SColor              colour;     // ��ɫ
        AIR::Vector3Df      upDir;      

    };
    typedef AIRVector<Element> ElementList;


    struct ChainSegment
    {
        size_t start;                   // �����ڻ����е���ʼID 
        size_t head;                    // ���ε�ͷ
        size_t tail;                    // ���ε�β
		size_t curLen;
    };
    typedef AIRVector<ChainSegment> ChainSegmentList;
    typedef AIRList<ChainSegment*>  ChainSegmentPtrList;
    typedef ChainSegmentList::iterator  ChainSegment_Iter;
    typedef ChainSegmentPtrList::iterator ChainSegmentPtr_Iter;

	//BillBoardChain();

    // ctor
    BillBoardChain(
        //const char* szName,
        size_t              maxElements = 20, 
        size_t              numberOfChains = 1, 
        bool                useTextureCoords = true, 
        bool                useColours = true, 
        bool                dynamic = true);

    // dtor
	virtual ~BillBoardChain();


    // ����ÿ����������е�Ԫ�ص����ֵ
    virtual void                SetMaxChainElements(size_t maxElements);

    // ���ÿ����������е�Ԫ�ص����ֵ
    virtual size_t              GetMaxChainElements(void) const { return mMaxElementsPerChain; }

    // ���ù���������������� (ÿ����������ͬ�Ĳ���). 
    virtual void                SetNumberOfChains(size_t numChains);

    // ��ù����������������
    virtual size_t              GetNumberOfChains(void) const { return mChainCount; }

    //�������
    virtual ChainSegment*       CreateChainSegment();

    virtual ChainSegment*       GetChainSegment(int nIndex);

    //����һ����Ķ�
    virtual void                DestroyChainSegment(ChainSegment* pSegment);

    virtual void                DestroyChainSegment(int nIndex);

    // �����Ƿ������ɻ����ʱ��ʹ����������
    // ����Ϊ����ָ��������ɫ����������������֮1��2
    virtual void                SetUseTextureCoords(bool use);

    // ����Ƿ������ɻ����ʱ��ʹ����������
    virtual bool                GetUseTextureCoords(void) const { return mUseTexCoords; }

    // ����ÿ�������Ԫ����ʹ�õ��������귽��,Ĭ��Ϊ TCD_U
    virtual void                SetTextureCoordDirection(TexCoordDirection dir);

    // ���ÿ�������Ԫ����ʹ�õ��������귽��
    virtual TexCoordDirection   GetTextureCoordDirection(void) { return mTexCoordDir; }

    // ָ���������Ԫ�ؿ�ȷ����ϵ��������귶Χ
    // start: ��ʼ��������,Ĭ��Ϊ0.0
    // end: ������������,Ĭ��Ϊ1.0
    virtual void                SetOtherTextureCoordRange(float start, float end);

    // ��ù������Ԫ�ؿ�ȷ����ϵ��������귶Χ
    virtual const float*        GetOtherTextureCoordRange(void) const { return mOtherTexCoordRange; }

    // �����Ƿ�ʹ�ö�����ɫ
    virtual void                SetUseVertexColours(bool use);

    // ��õ�ǰ�Ƿ�ʹ�ö�����ɫ
    virtual bool                GetUseVertexColours(void) const { return mUseVertexColour; }

    // �����Ƿ��ڶ������仯ʱ��̬��������
    virtual void                SetDynamic(bool dyn);

    // ��ȡ�Ƿ��ڶ������仯ʱ��̬��������
    virtual bool                GetDynamic(void) const { return mDynamic; }

    // ����chainIndex��������Ԫ��
    virtual void                AddChainElement(size_t chainIndex, const Element& billboardChainElement);

    // �Ƴ���chainIndex���������һ��Ԫ��
    virtual void                RemoveChainElement(size_t chainIndex);

    // ����ĳ�����ϵ�ĳ��Ԫ�ص���Ϣ
    // chainIndex: ��ID
    // elementIndex: Ԫ��ID
    // billboardChainElement: �������Ԫ��
    virtual void                UpdateChainElement(size_t chainIndex, size_t elementIndex, const Element& billboardChainElement);

    // ���ĳ�����ϵ�ĳ��Ԫ��
    // chainIndex: ��ID
    // elementIndex: Ԫ��ID
    virtual const Element&      GetChainElement(size_t chainIndex, size_t elementIndex) const;

    // ���ĳ������Ԫ�ص�����
    virtual size_t              GetNumChainElements(size_t chainIndex) const;

    // ���ĳ����
    virtual void                ClearChain(size_t chainIndex);

    // ������е���
    virtual void                ClearAllChains(void);

    // ��ò�������
    virtual const AIRString&  GetMaterialName(void) const { return mMaterialName; }

    // ���ò���
    virtual void                SetMaterialName( const AIRString& name );

    // ������������仯ʱ���и���
    virtual void                _NotifyCurrentCamera(CCamera* cam);

    // �������������������Χ�����ĵĳ��ȵ�ƽ��
    virtual float               GetSquaredViewDepth(const CCamera* cam) const;

    // ��ð�Χ�뾶
    virtual float               GetBoundingRadius(void) const;

    // ��ð�Χ��
    //virtual const AABBBox3Df&   GetBoundingBox(void) const;

    // ��ò��ʶ���
    //virtual const IMaterial*    getMaterial(void) const;

    // ��Ⱦ
    //virtual void                Render(CCamera* pCamera);

    // ���
    virtual void                Clear();

    // ����
    virtual void                Update( float fTimeElapsed );

	virtual void				Reset();
	
    virtual const AABBBox3Df& GetWorldBoundingBox(bool bTransform)
    {
        return m_WorldAABB;
    }

protected:
    // �޸�Ԫ������ 
    virtual void                ChanageElem( size_t chainIndex, Element& elem );

    // ��Ⱦǰ�Ĵ���
    virtual void                PreRender(CCamera* pCamera);

    // ���ù����������
    virtual void                SetupChainContainers(void);

    // ���ö�������
    virtual void                SetupVertexDeclaration(void);

    // ���û���
    virtual void                SetupBuffers(void);

    // ���¶��㻺��
    virtual void                UpdateVertexBuffer(CCamera* cam);

    // ������������
    virtual void                UpdateIndexBuffer(void);

    // ���°�Χ��
    virtual void                UpdateBoundingBox(void) const;

	virtual void				InitImpl( 
		const AIRString&  name, 
		size_t              maxElements = 20, 
		size_t              numberOfChains = 1, 
		bool                useTextureCoords = true, 
		bool                useColours = true, 
		bool                dynamic = true);

    virtual void _GetPrimitive();

protected:
    size_t                      mMaxElementsPerChain;       // ÿ�����ε�Ԫ�ص����ֵ
    size_t                      mChainCount;                // ��������
    bool                        mUseTexCoords;              // �Ƿ�ʹ����������
    bool                        mUseVertexColour;           // �Ƿ�ʹ�ö�����ɫ
    bool                        mDynamic;                   // ��̬ʹ��
    //IVertexBuffer*              mVertexData;                // ���㻺��
    //IIndexBuffer*               mIndexData;                 // ��������
	//BillboardSet*				m_pBillboardSet;
    static BillboardChainData*         m_pBillboardChainData;
    bool                        mVertexDeclDirty;           // ��Ҫ���¶����ʽ����?
    bool                        mBuffersNeedRecreating;     // ��Ҫ���´�������?
    mutable bool                mBoundsDirty;               // ��Ҫ���°�Χ��
    bool                        mIndexContentDirty;         // ��Ҫ������������?
    mutable AABBBox3Df          mAABB;                      // ��Χ��
    mutable float               mRadius;                    // ��Χ�뾶
    AIRString                 mMaterialName;              // ��������
    IMaterial*                  mMaterial;                  // ���ʶ���
    TexCoordDirection           mTexCoordDir;               // �������귽��
    float                       mOtherTexCoordRange[2];     // ��һ���������귽��ķ�Χ
    EVertexBufferType           mVertexDecl;                // ��������
    ElementList                 mChainElementList;          // �������������Ԫ�ص�����
    ChainSegmentList            mChainSegmentList;          // ��������
    ChainSegmentPtrList            mActiveChainList;
    ChainSegmentPtrList            mFreeChainList;
    static const size_t         SEGMENT_EMPTY;              // ����Ϊ��ʱ��ֵ
    size_t                      mTriangleCount;             // ����������
    size_t                      mVertexCount;               // ��������
}; //! end class BillBoardChain



#endif //! end __BillBoardChain_H_