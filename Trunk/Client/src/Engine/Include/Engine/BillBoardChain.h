// ***************************************************************
//  文件: BillBoardChain.h
//  日期: 2010-1-21
//  作者: huangjunjie
//  说明: billboard chain
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
    // 公告板链所使用的纹理坐标方向
    enum TexCoordDirection
    {
        TCD_U = 0,
        TCD_V = 1,
    };

    // 公告板链的元素信息
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
        AIR::Vector3Df      position;   // 坐标
        float               width;      // 宽度
        float               texCoord;   // 纹理坐标范围值
        SColor              colour;     // 颜色
        AIR::Vector3Df      upDir;      

    };
    typedef AIRVector<Element> ElementList;


    struct ChainSegment
    {
        size_t start;                   // 链段在缓冲中的起始ID 
        size_t head;                    // 链段的头
        size_t tail;                    // 链段的尾
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


    // 设置每条公告板链中的元素的最大值
    virtual void                SetMaxChainElements(size_t maxElements);

    // 获得每条公告板链中的元素的最大值
    virtual size_t              GetMaxChainElements(void) const { return mMaxElementsPerChain; }

    // 设置公告板链中链的数量 (每条链共享相同的材质). 
    virtual void                SetNumberOfChains(size_t numChains);

    // 获得公告板链中链的数量
    virtual size_t              GetNumberOfChains(void) const { return mChainCount; }

    //创建活动链
    virtual ChainSegment*       CreateChainSegment();

    virtual ChainSegment*       GetChainSegment(int nIndex);

    //销毁一个活动的段
    virtual void                DestroyChainSegment(ChainSegment* pSegment);

    virtual void                DestroyChainSegment(int nIndex);

    // 设置是否在生成缓冲的时候使用纹理坐标
    // 必须为顶点指定顶点颜色或者纹理坐标其中之1或2
    virtual void                SetUseTextureCoords(bool use);

    // 获得是否在生成缓冲的时候使用纹理坐标
    virtual bool                GetUseTextureCoords(void) const { return mUseTexCoords; }

    // 设置每个公告板元素所使用的纹理坐标方向,默认为 TCD_U
    virtual void                SetTextureCoordDirection(TexCoordDirection dir);

    // 获得每个公告板元素所使用的纹理坐标方向
    virtual TexCoordDirection   GetTextureCoordDirection(void) { return mTexCoordDir; }

    // 指定公告板链元素宽度方向上的纹理坐标范围
    // start: 起始纹理坐标,默认为0.0
    // end: 结束纹理坐标,默认为1.0
    virtual void                SetOtherTextureCoordRange(float start, float end);

    // 获得公告板链元素宽度方向上的纹理坐标范围
    virtual const float*        GetOtherTextureCoordRange(void) const { return mOtherTexCoordRange; }

    // 设置是否使用顶点颜色
    virtual void                SetUseVertexColours(bool use);

    // 获得当前是否使用顶点颜色
    virtual bool                GetUseVertexColours(void) const { return mUseVertexColour; }

    // 设置是否在对象发生变化时动态创建缓冲
    virtual void                SetDynamic(bool dyn);

    // 获取是否在对象发生变化时动态创建缓冲
    virtual bool                GetDynamic(void) const { return mDynamic; }

    // 往第chainIndex条链插入元素
    virtual void                AddChainElement(size_t chainIndex, const Element& billboardChainElement);

    // 移除第chainIndex条链的最后一个元素
    virtual void                RemoveChainElement(size_t chainIndex);

    // 更新某条链上的某个元素的信息
    // chainIndex: 链ID
    // elementIndex: 元素ID
    // billboardChainElement: 公告板链元素
    virtual void                UpdateChainElement(size_t chainIndex, size_t elementIndex, const Element& billboardChainElement);

    // 获得某条链上的某个元素
    // chainIndex: 链ID
    // elementIndex: 元素ID
    virtual const Element&      GetChainElement(size_t chainIndex, size_t elementIndex) const;

    // 获得某条链的元素的数量
    virtual size_t              GetNumChainElements(size_t chainIndex) const;

    // 清空某条链
    virtual void                ClearChain(size_t chainIndex);

    // 清空所有的链
    virtual void                ClearAllChains(void);

    // 获得材质名称
    virtual const AIRString&  GetMaterialName(void) const { return mMaterialName; }

    // 设置材质
    virtual void                SetMaterialName( const AIRString& name );

    // 当摄像机发生变化时进行更新
    virtual void                _NotifyCurrentCamera(CCamera* cam);

    // 获得摄像机到公告板链包围框中心的长度的平方
    virtual float               GetSquaredViewDepth(const CCamera* cam) const;

    // 获得包围半径
    virtual float               GetBoundingRadius(void) const;

    // 获得包围框
    //virtual const AABBBox3Df&   GetBoundingBox(void) const;

    // 获得材质对象
    //virtual const IMaterial*    getMaterial(void) const;

    // 渲染
    //virtual void                Render(CCamera* pCamera);

    // 清空
    virtual void                Clear();

    // 更新
    virtual void                Update( float fTimeElapsed );

	virtual void				Reset();
	
    virtual const AABBBox3Df& GetWorldBoundingBox(bool bTransform)
    {
        return m_WorldAABB;
    }

protected:
    // 修改元素属性 
    virtual void                ChanageElem( size_t chainIndex, Element& elem );

    // 渲染前的处理
    virtual void                PreRender(CCamera* pCamera);

    // 设置公告板链容器
    virtual void                SetupChainContainers(void);

    // 设置顶点声明
    virtual void                SetupVertexDeclaration(void);

    // 设置缓冲
    virtual void                SetupBuffers(void);

    // 更新顶点缓冲
    virtual void                UpdateVertexBuffer(CCamera* cam);

    // 更新索引缓冲
    virtual void                UpdateIndexBuffer(void);

    // 更新包围盒
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
    size_t                      mMaxElementsPerChain;       // 每条链段的元素的最大值
    size_t                      mChainCount;                // 链段数量
    bool                        mUseTexCoords;              // 是否使用纹理坐标
    bool                        mUseVertexColour;           // 是否使用顶点颜色
    bool                        mDynamic;                   // 动态使用
    //IVertexBuffer*              mVertexData;                // 顶点缓冲
    //IIndexBuffer*               mIndexData;                 // 索引缓冲
	//BillboardSet*				m_pBillboardSet;
    static BillboardChainData*         m_pBillboardChainData;
    bool                        mVertexDeclDirty;           // 需要更新顶点格式声明?
    bool                        mBuffersNeedRecreating;     // 需要重新创建缓冲?
    mutable bool                mBoundsDirty;               // 需要更新包围盒
    bool                        mIndexContentDirty;         // 需要更新索引缓冲?
    mutable AABBBox3Df          mAABB;                      // 包围盒
    mutable float               mRadius;                    // 包围半径
    AIRString                 mMaterialName;              // 材质名称
    IMaterial*                  mMaterial;                  // 材质对象
    TexCoordDirection           mTexCoordDir;               // 纹理坐标方向
    float                       mOtherTexCoordRange[2];     // 另一个纹理坐标方向的范围
    EVertexBufferType           mVertexDecl;                // 顶点声明
    ElementList                 mChainElementList;          // 公告板链的所有元素的容器
    ChainSegmentList            mChainSegmentList;          // 链段容器
    ChainSegmentPtrList            mActiveChainList;
    ChainSegmentPtrList            mFreeChainList;
    static const size_t         SEGMENT_EMPTY;              // 链段为空时的值
    size_t                      mTriangleCount;             // 三角形数量
    size_t                      mVertexCount;               // 顶点数量
}; //! end class BillBoardChain



#endif //! end __BillBoardChain_H_