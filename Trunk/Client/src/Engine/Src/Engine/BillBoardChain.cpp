#include "stdafx.h"
#include "BillBoardChain.h"
#include "VertexDef.h"
#include "Renderable.h"
#include "IMaterial.h"
#include "Camera.h"
#include "IVertexBuffer.h"
#include "IIndexBuffer.h"
#include "BillboardSet.h"
#include "BillboardSetManager.h"
#include "SceneNode.h"
#include <assert.h>
#include "Primitive.h"
//#include "Technique.h"
#include "RenderQueueGroup.h"

const size_t BillBoardChain::SEGMENT_EMPTY = 0xffffffff;

BillboardChainData* BillBoardChain::m_pBillboardChainData = NULL;

BillBoardChain::Element::Element()
{
}


BillBoardChain::Element::Element(AIR::Vector3Df _position, float _width, float _texCoord, SColor _colour, AIR::Vector3Df _upDir ) :
position(_position),
width(_width),
texCoord(_texCoord),
colour(_colour),
upDir(_upDir)
{
}


BillBoardChain::BillBoardChain(
    //const char* szName,
    size_t maxElements,
    size_t numberOfChains, 
    bool useTextureCoords, 
    bool useColours, 
    bool dynamic):
mMaxElementsPerChain(maxElements),
mChainCount(numberOfChains),
mUseTexCoords(useTextureCoords),
mUseVertexColour(useColours),
mDynamic(dynamic),
mVertexDeclDirty(true),
mBuffersNeedRecreating(true),
mBoundsDirty(true),
mIndexContentDirty(true),
mRadius(0.0f),
mTexCoordDir(TCD_U),
mVertexCount(0),
mVertexDecl((EVertexBufferType)INVALID_VERTEX_DECL)//,
//m_pBillboardChainData(NULL)
{
    m_szType                = "billboardchain";
    //m_strName               = szName;
    //mVertexData             = NULL;
    //mIndexData              = NULL;
    mOtherTexCoordRange[0]  = 0.0f;
    mOtherTexCoordRange[1]  = 1.0f;
    SetupChainContainers();
    m_renderGroupType = eRGT_Transparent;
}



BillBoardChain::~BillBoardChain()
{
    Clear();
}


void BillBoardChain::SetupChainContainers(void)
{
    // 每个链段的最大元素数 * 链段数量
    mChainElementList.resize(mChainCount * mMaxElementsPerChain);
    // 计算最大的顶点数量
    mVertexCount = mChainElementList.size() * 2;
    mChainSegmentList.resize(mChainCount);
    for (size_t i = 0; i < mChainCount; ++i)
    {
        ChainSegment& seg = mChainSegmentList[i];
        seg.start = i * mMaxElementsPerChain;
        seg.tail = seg.head = SEGMENT_EMPTY;
    }
}


void BillBoardChain::SetupVertexDeclaration(void)
{
    if (mVertexDeclDirty)
    {
        // 有纹理坐标或顶点颜色
        if ( mUseTexCoords && mUseVertexColour )
        {
            // 设置顶点声明
            mVertexDecl = E_VB_STANDARD;
        }
        else if( mUseTexCoords && !mUseVertexColour )
        {
            mVertexDecl = E_VB_NOCOLOR;
        }
        else if ( !mUseTexCoords && mUseVertexColour )
        {
            mVertexDecl = E_VB_NOTEX;
        }
        // 没有纹理坐标和顶点颜色
        else if (!mUseTexCoords && !mUseVertexColour)
        {
            mVertexDecl = (EVertexBufferType)INVALID_VERTEX_DECL;
            DT_TO_MSGBOX_FMT( LT_ERROR, "必须为BillBoardChain指定使用顶点颜色或纹理坐标其中之1或2!" );
            return;
        }
        mVertexDeclDirty = false;
    }
}



void BillBoardChain::SetupBuffers(void)
{
    //设置顶点格式声明
    SetupVertexDeclaration();
    //如果当前需要重新创建缓冲区
    if (mBuffersNeedRecreating)
    {
        // 如果顶点格式声明为NULL
        if ( mVertexDecl == INVALID_VERTEX_DECL )
        {
            DT_TO_MSGBOX_FMT( LT_ERROR, "必须为BillBoardChain指定使用顶点颜色或纹理坐标其中之1或2!" );
            return;
        }
        //// 销毁原来的顶点缓冲区
        //if ( mVertexData )
        //{
        //    ENGINE_INST->GetRenderer()->ClearVertexBuffer( mVertexData );
        //}
        //// 销毁原来的索引缓冲区
        //if ( mIndexData )
        //{
        //    ENGINE_INST->GetRenderer()->ClearIndexBuffer( mIndexData );
        //}
        //// 创建顶点缓冲
        //mVertexData = ENGINE_INST->GetRenderer()->CreateVertexBuffer( mVertexDecl, mVertexCount, false );
        //if ( !mVertexData )
        //{
        //    DT_TO_MSGBOX_FMT( LT_ERROR, "创建BillBoardChain顶点缓冲失败!" );
        //    return;
        //}   
        //// 计算索引缓冲区的大小
        //size_t nTotalIndexSize = (mMaxElementsPerChain*2 - 2) * 3 * mChainCount;
        //// 创建索引缓冲
        //mIndexData = ENGINE_INST->GetRenderer()->CreateIndexBuffer( nTotalIndexSize, false );
        //if ( !mIndexData )
        //{
        //    DT_TO_MSGBOX_FMT( LT_ERROR, "创建BillBoardChain索引缓冲失败!" );
        //    return;
        //}

		if ( !m_pBillboardChainData )
		{
			m_pBillboardChainData = BillboardSetManager::GetInstance()->CreateBillboardChainData( mChainElementList.size() );
		}
		else 
		{
			if ( m_pBillboardChainData->GetChainsNum() < mChainElementList.size() )
			{
				m_pBillboardChainData->Create( mChainElementList.size() );
			}
		}
		//if ( m_pBillboardChainData )
		//{
		//	BILLBOARDSET_MGR->ClearBillboardChainData(m_pBillboardChainData);
		//}
		//m_pBillboardChainData = BILLBOARDSET_MGR->CreateBillboardChainData( mChainElementList.size() );

        mBuffersNeedRecreating = false;
    }
}


void BillBoardChain::SetMaxChainElements(size_t maxElements)
{
    //设置元素的最大值
    mMaxElementsPerChain = maxElements;
    //设置链容器
    SetupChainContainers();
    //设置更新顶点缓冲和索引缓冲
    mBuffersNeedRecreating = mIndexContentDirty = true;
}


void BillBoardChain::SetNumberOfChains(size_t numChains)
{
    // 设置链段的数量
    mChainCount = numChains;
    // 设置链容器
    SetupChainContainers();
    // 设置更新顶点缓冲和索引缓冲
    mBuffersNeedRecreating = mIndexContentDirty = true;
}


void BillBoardChain::SetUseTextureCoords(bool use)
{
    // 设置使用纹理坐标
    mUseTexCoords = use;
    // 设置更新顶点缓冲和索引缓冲
    mVertexDeclDirty = mBuffersNeedRecreating = true;
    mIndexContentDirty = true;
}


void BillBoardChain::SetTextureCoordDirection(BillBoardChain::TexCoordDirection dir)
{
    // 设置纹理坐标的方向
    mTexCoordDir = dir;
}


void BillBoardChain::SetOtherTextureCoordRange(float start, float end)
{
    // 设置另外一个纹理坐标方向的范围
    mOtherTexCoordRange[0] = start;
    mOtherTexCoordRange[1] = end;
}


void BillBoardChain::SetUseVertexColours(bool use)
{
    // 设置使用顶点颜色
    mUseVertexColour = use;
    // 设置更新顶点缓冲和索引缓冲
    mVertexDeclDirty = mBuffersNeedRecreating = true;
    mIndexContentDirty = true;
}


void BillBoardChain::SetDynamic(bool dyn)
{
    // 动态更新
    mDynamic = dyn;
    mBuffersNeedRecreating = mIndexContentDirty = true;
}


void BillBoardChain::AddChainElement(size_t chainIndex,
                                     const BillBoardChain::Element& dtls)
{
    if (chainIndex >= mChainCount)
    {
        DT_TO_MSGBOX_FMT( LT_ERROR, "公告板链ID越界!" );
        return;
    }
    // 获得链段
    ChainSegment& seg = mChainSegmentList[chainIndex];
    // 如果头标记为空
    if (seg.head == SEGMENT_EMPTY)
    {
        // 链段尾标记从链段的最后一个元素开始
        // 链段头标记向后增长
        seg.tail = mMaxElementsPerChain - 1;
        seg.head = seg.tail;
        mIndexContentDirty = true;
		seg.curLen = 0;
    }
    // 链段头标记不为空
    else
    {
        // 如果链段头标记为0,表示当前链段已满
        if (seg.head == 0)
        {
            // 头标记移到最后一个元素
            seg.head = mMaxElementsPerChain - 1;
        }
        // 如果链段头标记不为0,向前跳一个元素
        else
        {
            --seg.head;
        }

        // 如果头标记与尾标记一样
        if (seg.head == seg.tail)
        {
            // 如果尾标记为0,表示已经到了链段的起始元素
            if (seg.tail == 0)
            {
                // 将尾标记移到链段的最后元素位置处
                seg.tail = mMaxElementsPerChain - 1;
            }
            else
            {
                // 尾标记向前移一个位置
                --seg.tail;
            }
        }
    }

    // 往元素表插入一个元素
    mChainElementList[seg.start + seg.head] = dtls;

    // 更新索引缓冲
    mIndexContentDirty = true;
    mBoundsDirty = true;

	if ( seg.curLen != mMaxElementsPerChain )
	{
		++seg.curLen;
	}

    //// tell parent node to update bounds
    //if (mParentNode)
    //    mParentNode->needUpdate();
}


void BillBoardChain::RemoveChainElement(size_t chainIndex)
{
    if (chainIndex >= mChainCount)
    {
        DT_TO_MSGBOX_FMT( LT_ERROR, "公告板链ID越界!" );
        return;
    }
    // 获得链段
    ChainSegment& seg = mChainSegmentList[chainIndex];
    // 如果链段为空,直接返回
    if (seg.head == SEGMENT_EMPTY)
    {
        return; 
    }

    // 如果头标记与尾标记一样
    if (seg.tail == seg.head)
    {
        // 设置链段为空
        seg.head = seg.tail = SEGMENT_EMPTY;
    }
    // 如果链段尾标记为0
    else if (seg.tail == 0)
    {
        // 将尾标记移到链段的最后一个元素处
        seg.tail = mMaxElementsPerChain - 1;
    }
    else
    {
        // 尾标记向前移一个位置
        --seg.tail;
    }

    // 更新索引缓冲
    mIndexContentDirty = true;
    mBoundsDirty = true;

    //// tell parent node to update bounds
    //if (mParentNode)
    //    mParentNode->needUpdate();
}


void BillBoardChain::ClearChain(size_t chainIndex)
{
    if (chainIndex >= mChainCount)
    {
        DT_TO_MSGBOX_FMT( LT_ERROR, "公告板链ID越界!" );
        return;
    }
    // 获得链段对象
    ChainSegment& seg = mChainSegmentList[chainIndex];


    //DT_TO_DBGSTR( LT_INFO, GetString("clear chain:\n%d,%d,%d,%d\n",chainIndex, seg.start, seg.head, seg.tail) );


    // 重设尾标记和头标记
    seg.tail = seg.head = SEGMENT_EMPTY;

    // 更新索引缓冲
    mIndexContentDirty = true;
    mBoundsDirty = true;
    //// tell parent node to update bounds
    //if (mParentNode)
    //    mParentNode->needUpdate();
}


void BillBoardChain::ClearAllChains(void)
{
    for (size_t i = 0; i < mChainCount; ++i)
    {
        ClearChain(i);
    }

}


void BillBoardChain::UpdateChainElement(size_t chainIndex, size_t elementIndex,
                                        const BillBoardChain::Element& dtls)
{
    if (chainIndex >= mChainCount)
    {
        DT_TO_MSGBOX_FMT( LT_ERROR, "公告板链ID越界!" );
        return;
    }
    ChainSegment& seg = mChainSegmentList[chainIndex];
    if (seg.head == SEGMENT_EMPTY)
    {
        DT_TO_MSGBOX_FMT( LT_ERROR, "公告板链段是空的!" );
        return;
    }

    // 计算元素的索引值
    size_t idx = seg.head + elementIndex;
    idx = (idx % mMaxElementsPerChain) + seg.start;

    mChainElementList[idx] = dtls;

    mBoundsDirty = true;
    //// tell parent node to update bounds
    //if (mParentNode)
    //    mParentNode->needUpdate();

}


const BillBoardChain::Element& BillBoardChain::GetChainElement(size_t chainIndex, size_t elementIndex) const
{
    const BillBoardChain::Element elem;
    if (chainIndex >= mChainCount)
    {
        DT_TO_MSGBOX_FMT( LT_ERROR, "公告板链ID越界!" );
        return elem;
    }
    const ChainSegment& seg = mChainSegmentList[chainIndex];

    size_t idx = seg.head + elementIndex;
    idx = (idx % mMaxElementsPerChain) + seg.start;

    return mChainElementList[idx];
}


size_t BillBoardChain::GetNumChainElements(size_t chainIndex) const
{
    if (chainIndex >= mChainCount)
    {
        DT_TO_MSGBOX_FMT( LT_ERROR, "公告板链ID越界!" );
        return 0;
    }
    const ChainSegment& seg = mChainSegmentList[chainIndex];

    return seg.tail - seg.head + 1;
}


void BillBoardChain::UpdateBoundingBox(void) const
{
    if (mBoundsDirty)
    {
        mAABB.SetNull(true);
        Vector3Df widthVector;
        for (ChainSegmentList::const_iterator segi = mChainSegmentList.begin();
            segi != mChainSegmentList.end(); ++segi)
        {
            const ChainSegment& seg = *segi;
            if (seg.head != SEGMENT_EMPTY)
            {
                for(size_t e = seg.head; ; ++e)
                {
                    if (e == mMaxElementsPerChain)
                        e = 0;
                    const Element& elem = mChainElementList[seg.start + e];
                    widthVector.x = widthVector.y = widthVector.z = elem.width;
                    mAABB.AddInternalPoint(elem.position - widthVector);
                    mAABB.AddInternalPoint(elem.position + widthVector);
                    if (e == seg.tail)
                        break;
                }
            }
        }
        // Set the current radius
        if (mAABB.IsNull())
        {
            mRadius = 0.0f;
        }
        else
        {
            mRadius = sqrtf( std::max(mAABB.MinEdge.GetLengthSQ(), mAABB.MaxEdge.GetLengthSQ()) );
        }
        mBoundsDirty = false;
    }
}



void BillBoardChain::UpdateVertexBuffer(CCamera* cam)
{
	bool bResetBB = false;

    // 设置缓冲
    SetupBuffers();

    // 锁定顶点缓冲
    //void* pBufferStart = mVertexData->Lock();
	void* pBufferStart = m_pBillboardChainData->GetVertexBuffer()->Lock();

    // 当前顶点的切向量
    Vector3Df chainTangent;

    // 遍历所有的链段
    for ( size_t chainID = 0; chainID < mChainSegmentList.size(); ++chainID )
    {
        // 取得当前的链段
        ChainSegment& seg = mChainSegmentList[chainID];//*segi;

        // 如果该链段的元素个数小于2,就跳过
        if (seg.head != SEGMENT_EMPTY && seg.head != seg.tail)
        {
			//////////////////////////////////////////////////////////////////////////
			int   nCurElem = 0;
			float fDeltaUVOffset = 1.0f / float(seg.curLen-1);
			float fBeginUV = 0.0f;
			//////////////////////////////////////////////////////////////////////////

            size_t laste = seg.head;
            // 从头元素开始遍历
            for (size_t e = seg.head; ; ++e) 
            {
                // Wrap forwards
                if (e == mMaxElementsPerChain)
                    e = 0;

                // 获得元素信息
                Element& elem = mChainElementList[e + seg.start];
                assert (((e + seg.start) * 2) < 65536 && "Too many elements!");
                u16 baseIdx = static_cast<u16>((e + seg.start) * 2);

                // 找到该元素所在的顶点缓冲区的位置
                //void* pBase = static_cast<void*>( static_cast<char*>(pBufferStart) + mVertexData->GetUnitSize() * baseIdx );
				void* pBase = static_cast<void*>( static_cast<char*>(pBufferStart) + m_pBillboardChainData->GetVertexBuffer()->GetUnitSize() * baseIdx );

                // 获得下一个元素的ID
                size_t nexte = e + 1;
                if (nexte == mMaxElementsPerChain)
                    nexte = 0;

                // 如果当前元素是头元素
                if (e == seg.head)
                {
                    // 使用下一个元素来计算当前元素顶点的切向量
                    chainTangent = mChainElementList[nexte + seg.start].position - elem.position;
                }
                // 如果当前元素是尾元素
                else if (e == seg.tail)
                {
                    // 使用头元素来计算当前元素顶点的切向量
                    chainTangent = elem.position - mChainElementList[laste + seg.start].position;
                }
                // 如果当前的元素在中间
                else
                {
                    chainTangent = mChainElementList[nexte + seg.start].position - mChainElementList[laste + seg.start].position;
                }

				ChanageElem( chainID, elem );

                // 计算眼睛到元素的向量
                Vector3Df vEyeToElem = elem.position - cam->GetPosition();

                // 元素顶点的上方向向量
                Vector3Df vPerpendicular = chainTangent.CrossProduct(vEyeToElem);

                vPerpendicular.Normalize();
                vPerpendicular *= (elem.width * 0.5);

                // 顶点0的坐标
                Vector3Df pos0 = elem.position + vPerpendicular;
                // 顶点1的坐标
                Vector3Df pos1 = elem.position - vPerpendicular;

				//--------------------------------------------------
				//! add by hjj 2010-9-29
				if ( !bResetBB )
				{
					m_boundingBox.Reset( pos0 );
					bResetBB = true;
				}
				else
				{
					m_boundingBox.AddInternalPoint( pos0 );
				}				
				m_boundingBox.AddInternalPoint( pos1 );
				//--------------------------------------------------

                // 设置顶点0的坐标,颜色和纹理坐标
                switch( mVertexDecl )
                {
                case E_VB_STANDARD:
                    {
                        LPVERTEX_STANDARD pVSTAND = static_cast<LPVERTEX_STANDARD>(pBase);
                        pVSTAND->position = pos0;
                        pVSTAND->color = elem.colour;
                        if (mTexCoordDir == TCD_U)
                        {
                            pVSTAND->tu = fBeginUV;//elem.texCoord;
                            pVSTAND->tv = mOtherTexCoordRange[0];
                        }
                        else
                        {
                            pVSTAND->tu = mOtherTexCoordRange[0];
                            pVSTAND->tv = elem.texCoord;
                        }
                        ++pVSTAND;

						fBeginUV += fDeltaUVOffset;

                        // 设置顶点1的坐标,颜色和纹理坐标
                        pVSTAND->position = pos1;
                        pVSTAND->color = 0xffffffff;//elem.colour;
                        if (mTexCoordDir == TCD_U)
                        {
                            pVSTAND->tu = fBeginUV;//elem.texCoord;
                            pVSTAND->tv = mOtherTexCoordRange[1];
                        }
                        else
                        {
                            pVSTAND->tu = mOtherTexCoordRange[1];
                            pVSTAND->tv = fBeginUV;//elem.texCoord;
                        }
                    }
                    break;
                case E_VB_NOCOLOR:
                    {
                        LPVERTEX_NOCOLOR pVSTAND = static_cast<LPVERTEX_NOCOLOR>(pBase);
                        pVSTAND->x = pos0.x;
                        pVSTAND->y = pos0.y;
                        pVSTAND->z = pos0.z;
                        if (mTexCoordDir == TCD_U)
                        {
                            pVSTAND->fu = fBeginUV;//elem.texCoord;
                            pVSTAND->fv = mOtherTexCoordRange[0];
                        }
                        else
                        {
                            pVSTAND->fu = mOtherTexCoordRange[0];
                            pVSTAND->fv = fBeginUV;//elem.texCoord;
                        }
                        ++pVSTAND;

						fBeginUV += fDeltaUVOffset;

                        // 设置顶点1的坐标,颜色和纹理坐标
                        pVSTAND->x = pos1.x;
                        pVSTAND->y = pos1.y;
                        pVSTAND->z = pos1.z;
                        if (mTexCoordDir == TCD_U)
                        {
                            pVSTAND->fu = fBeginUV;//elem.texCoord;
                            pVSTAND->fv = mOtherTexCoordRange[1];
                        }
                        else
                        {
                            pVSTAND->fu = mOtherTexCoordRange[1];
                            pVSTAND->fv = fBeginUV;//elem.texCoord;
                        }
                    }
                    break;
                case E_VB_NOTEX:
                    {
                        LPVERTEX_NOTEX pVSTAND = static_cast<LPVERTEX_NOTEX>(pBase);
                        pVSTAND->position = pos0;
                        pVSTAND->color = elem.colour;
                        ++pVSTAND;

                        // 设置顶点1的坐标,颜色
                        pVSTAND->position = pos1;
                        pVSTAND->color = elem.colour;
                    }
                    break;
                }


                // 如果到最后一个,就结束
                if (e == seg.tail)
                    break; 

                laste = e;

            } 
        } 

    } 

    //mVertexData->Unlock();
	m_pBillboardChainData->GetVertexBuffer()->Unlock();

	m_WorldAABB.Reset( m_boundingBox );
    m_vtCenter = m_WorldAABB.GetCenter();
	if ( m_pNode )
	{
		m_pNode->NeedUpdate();
		m_pNode->Update();      //这里很奇怪，会引起视觉上的错误
	}
}


void BillBoardChain::UpdateIndexBuffer(void)
{
    SetupBuffers();
    if (mIndexContentDirty)
    {
        //u16* pShort = static_cast<u16*>( mIndexData->Lock(0, 0) );
		u16* pShort = static_cast<u16*>( m_pBillboardChainData->GetIndexBuffer()->Lock(0, 0) );
        
        u32 nTotalIndexCount = 0;
        mTriangleCount = 0;

        // indexes
        for (ChainSegmentList::iterator segi = mChainSegmentList.begin();
            segi != mChainSegmentList.end(); ++segi)
        {
            ChainSegment& seg = *segi;

            // Skip 0 or 1 element segment counts
            if (seg.head != SEGMENT_EMPTY && seg.head != seg.tail)
            {
                // Start from head + 1 since it's only useful in pairs
                size_t laste = seg.head;
                while(1) // until break
                {
                    size_t e = laste + 1;
                    // Wrap forwards
                    if (e == mMaxElementsPerChain)
                        e = 0;
                    // indexes of this element are (e * 2) and (e * 2) + 1
                    // indexes of the last element are the same, -2
                    assert (((e + seg.start) * 2) < 65536 && "Too many elements!");
                    u16 baseIdx = static_cast<u16>((e + seg.start) * 2);
                    u16 lastBaseIdx = static_cast<u16>((laste + seg.start) * 2);
                    *pShort++ = lastBaseIdx;
                    *pShort++ = lastBaseIdx + 1;
                    *pShort++ = baseIdx;
                    *pShort++ = lastBaseIdx + 1;
                    *pShort++ = baseIdx + 1;
                    *pShort++ = baseIdx;
                    nTotalIndexCount += 6;
                    mTriangleCount += 2;
                    if (e == seg.tail)
                    {
                        break; // last one
                    }
                    laste = e;
                }
            }

        }

        //DT_TO_DBGSTR( LT_INFO, GetString("total index: %d\n", nTotalIndexCount));
        //mIndexData->SetTotalIndex( nTotalIndexCount );
        //mIndexData->Unlock();
		m_pBillboardChainData->GetIndexBuffer()->SetTotalIndex( nTotalIndexCount );
		m_pBillboardChainData->GetIndexBuffer()->Unlock();
        mIndexContentDirty = false;
    }

}


void BillBoardChain::_NotifyCurrentCamera(CCamera* cam)
{
    UpdateVertexBuffer(cam);
    UpdateIndexBuffer();
}


float BillBoardChain::GetSquaredViewDepth(const CCamera* cam) const
{
    Vector3Df min, max, mid, dist;
    min = mAABB.MinEdge;
    max = mAABB.MaxEdge;
    mid = ((max - min) * 0.5) + min;
    dist = cam->GetPosition() - mid;

    return dist.GetLengthSQ();
}


float BillBoardChain::GetBoundingRadius(void) const
{
    return mRadius;
}

/*
const AABBBox3Df& BillBoardChain::getBoundingBox(void) const
{
    updateBoundingBox();
    return mAABB;
}
*/

//const IMaterial* BillBoardChain::getMaterial(void) const
//{
//    return mMaterial;
//}

// 设置材质
void BillBoardChain::SetMaterialName( const AIRString& name )
{
    mMaterialName = name;
}

/*
void BillBoardChain::Render(CCamera* pCamera)
{
    PreRender( pCamera );
    IRenderer* pRenderer = ENGINE_INST->GetRenderer();
    pRenderer->SetWorldTransform( &Matrix4f::IDENTITY );
    
    //if ( !mVertexData || !mIndexData || !mTriangleCount )
	if ( !m_pBillboardChainData->GetVertexBuffer() || !m_pBillboardChainData->GetIndexBuffer() || !mTriangleCount )
    {
        return;
    }

    if (m_pMaterial)
    {
        CTechnique* pTechnique = m_pMaterial->GetCurrentTechnique();
        if (pTechnique)
        {
			//pRenderer->SetVertexBuffer(0, mVertexData);
            //pRenderer->SetIndexBuffer(mIndexData);
			pRenderer->SetVertexBuffer(0, m_pBillboardChainData->GetVertexBuffer());
			pRenderer->SetIndexBuffer(m_pBillboardChainData->GetIndexBuffer());
            size_t nPassNum = pTechnique->GetPassesNum();
            for (size_t i = 0; i < nPassNum; ++i)
            {
                CPass* pPass = pTechnique->GetPass(i);
                pPass->ApplyPass(pRenderer);

                //pRenderer->DrawTriangleListByIndex( 0, 0, mVertexData->GetVertexCount(), 0, mVertexData, mIndexData, mTriangleCount );
				pRenderer->DrawTriangleListByIndex( 0, 0, m_pBillboardChainData->GetVertexBuffer()->GetVertexCount(), 0, mTriangleCount );
            }
        }
    }

	Renderable::AfterRender();
}
*/

void BillBoardChain::_GetPrimitive()
{
    if (m_pPrimitive == NULL)
    {
        m_pPrimitive = new Primitive(Primitive::Primitive_TriangleList);
        m_pPrimitive->m_pSrcRenderable = this;
        m_pPrimitive->SetUseUserPrimitive(true);
    }

    m_pPrimitive->m_pVertexBuffer = m_pBillboardChainData->GetVertexBuffer();
    //m_pPrimitive->m_pVertexDecl = m_pPrimitive->m_pVertexBuffer->GetVertexDeclaration();
    m_pPrimitive->m_pIndexBuffer = m_pBillboardChainData->GetIndexBuffer();
    m_pPrimitive->SetPrimitiveCount(mTriangleCount);

}

void BillBoardChain::Clear()
{
    //ENGINE_INST->GetRenderer()->ClearVertexBuffer( mVertexData );
    //ENGINE_INST->GetRenderer()->ClearIndexBuffer( mIndexData );

	//if (m_pBillboardChainData)
	//{
	//	BILLBOARDSET_MGR->ClearBillboardChainData(m_pBillboardChainData);
	//	m_pBillboardChainData = NULL;
	//}
}

void BillBoardChain::PreRender(CCamera* pCamera)
{
    _NotifyCurrentCamera( pCamera );
}

// 修改元素属性 
void BillBoardChain::ChanageElem(size_t chainIndex, Element& elem )
{

}

// 更新
void BillBoardChain::Update( float fTimeElapsed )
{
    //判断是否超时
}

void BillBoardChain::Reset()
{
	ClearAllChains();
}

BillBoardChain::ChainSegment* BillBoardChain::CreateChainSegment()
{
    ChainSegment* p = 0;
    if (!mFreeChainList.empty())
    {
        p = mFreeChainList.front();
        mActiveChainList.splice(mFreeChainList.end(), mFreeChainList, mFreeChainList.begin());
    }
    return p;
}

void BillBoardChain::DestroyChainSegment(ChainSegment* pSegment)
{
    ChainSegmentPtr_Iter it = std::find(mActiveChainList.begin(), mActiveChainList.end(), pSegment);
    if (it != mActiveChainList.end())
    {
        mFreeChainList.splice(mFreeChainList.end(), mActiveChainList, it);
        pSegment->head = SEGMENT_EMPTY;
    }
}

void BillBoardChain::DestroyChainSegment(int nIndex)
{
    //if (nIndex >= mChainSegmentList.size())
    //{
    //    return;
    //}
    //mChainSegmentList[nIndex].head = SEGMENT_EMPTY;
    ClearChain(nIndex);
    //removeChainElement(nIndex);
}

BillBoardChain::ChainSegment* BillBoardChain::GetChainSegment(int nIndex)
{
    if (nIndex >= mChainSegmentList.size())
    {
        return NULL;
    }
    return &mChainSegmentList[nIndex];
}


void BillBoardChain::InitImpl(const AIRString& name, size_t maxElements /* = 20 */, size_t numberOfChains /* = 1 */, bool useTextureCoords /* = true */, bool useColours /* = true */, bool dynamic /* = true */)
{
	mMaxElementsPerChain = maxElements;
	mChainCount = numberOfChains;
	mUseTexCoords = useTextureCoords;
	mUseVertexColour = useColours;
	mDynamic = dynamic;
	mVertexDeclDirty = true;
	mBuffersNeedRecreating = true;
	mBoundsDirty = true;
	mIndexContentDirty = true;
	mRadius = 0.0f;
	mTexCoordDir = TCD_U;
	mVertexCount = 0;
	mVertexDecl = (EVertexBufferType)INVALID_VERTEX_DECL,
	//m_pBillboardChainData = NULL;
	m_szType                = "billboardchain";
	m_strName               = name;
	mOtherTexCoordRange[0]  = 0.0f;
	mOtherTexCoordRange[1]  = 1.0f;
	SetupChainContainers();
}