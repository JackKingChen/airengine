#include "stdafx.h"
#include "RibbonTrail.h"
#include "SceneNode.h"
#include "VertexDef.h"
#include "Particle.h"
#include "BillboardSet.h"
#include "BillboardSetManager.h"
#include "DataStream.h"
#include "FileSystem.h"
#include "MaterialManager.h"
#include "MarkupSTL.h"
#include "Util.h"
//#include "TextureUnit.h"
//#include "Technique.h"

#include "Primitive.h"
//#include "IShader.h"



CColorValue SColorToCColorValue( const SColor& inc )
{
	CColorValue outc( 
		f32(inc.GetAlpha()) / 255.0f, 
		f32(inc.GetRed()) / 255.0f, 
		f32(inc.GetGreen()) / 255.0f, 
		f32(inc.GetBlue()) / 255.0f );
	return outc;
}

SColor CColorValueToSColor( const CColorValue& inc )
{
	SColor outc( u8(inc.a * 255.0f), u8(inc.r * 255.0f), u8(inc.g * 255.0f), u8(inc.b * 255.0f) );
	return outc;
}

RibbonTrailResource::RibbonTrailResource() : mMaxElementsPerChain(0)
, mChainCount(0)
, mUseTexCoords(true)
, mUseVertexColour(true)
, mDrawWithBillboard(false)
, mTexCoordDir(BillBoardChain::TCD_U)
//, m_fWidthInit(0)
//, m_fWidthChange(0)
{
	m_widthInit.clear();
	m_widthChange.clear();
	m_colInit.clear();
	m_colChange.clear();
}

RibbonTrailResource::~RibbonTrailResource()
{

}

bool RibbonTrailResource::Cache()
{
    AIRString strDoc = m_pStream->GetAsString();

    CMarkupSTL xml;
    xml.SetDoc(strDoc.c_str());

    if (!xml.FindElem())
    {
        SafeDelete(m_pStream);
        return false;
    }

    //Clear();

    m_strName = xml.GetAttrib("Name").c_str();

    mMaxElementsPerChain = atoi( xml.GetAttrib("MaxElemPerChain").c_str() );

    mChainCount = atoi( xml.GetAttrib("ChainCount").c_str() );

    AIRString szValue;
    szValue = xml.GetAttrib( "UseVertexColor" ).c_str();
    if ( szValue == "FALSE" )
    {
        mUseVertexColour = false;
    }
    else if ( szValue == "TRUE" )
    {
        mUseVertexColour = true;
    }

    szValue = xml.GetAttrib( "UseTexCoord" ).c_str();
    if ( szValue == "FALSE" )
    {
        mUseTexCoords = false;
    }
    else if ( szValue == "TRUE" )
    {
        mUseTexCoords = true;
    }

    //InitImpl( m_strName, mMaxElementsPerChain, mChainCount, mUseTexCoords, mUseVertexColour );

    szValue = xml.GetAttrib( "DrawWithBillBoard" ).c_str();
    if ( szValue == "FALSE" )
    {
        mDrawWithBillboard = false;
    }
    else if ( szValue == "TRUE" )
    {
        mDrawWithBillboard = true;
    }

    char buf[512];
    snprintf( buf, 512, "%s", xml.GetAttrib("TexCoordRange").c_str() );
    sscanf( buf, "(%f,%f)", &mOtherTexCoordRange[0], &mOtherTexCoordRange[1] );

    mTexCoordDir = (BillBoardChain::TexCoordDirection)atoi( xml.GetAttrib("TexCoordDirection").c_str() );

    m_szTrailImage = xml.GetAttrib( "TrailTexImage" ).c_str();
    //SetTrailImage( szTrailTexImage );

    xml.IntoElem();

    //int nCurID = 0;
    while( xml.FindElem() )
    {
        szValue = xml.GetAttrib("InitColor").c_str();
        CColorValue colInit;
        sscanf( szValue.c_str(), "(%f,%f,%f,%f)", &colInit.a, &colInit.r, &colInit.g, &colInit.a );
		m_colInit.push_back( colInit );

        m_widthInit.push_back( atof( xml.GetAttrib("InitWidth").c_str() ) );

        CColorValue colChg;
        szValue = xml.GetAttrib("ChangeColor").c_str();
        sscanf( szValue.c_str(), "(%f,%f,%f,%f)", &colChg.a, &colChg.r, &colChg.g, &colChg.a );
		m_colChange.push_back( colChg );

        m_widthChange.push_back( atof( xml.GetAttrib("ChangeWidth").c_str() ) );

        //SetInitialColour( nCurID, colInit );
        //SetInitialWidth( nCurID, fInitWidth );
        //SetColourChange( nCurID, colChg );
        //SetWidthChange( nCurID, fChgWidth );
        //++nCurID;
    }

    xml.OutOfElem();

    SafeDelete( m_pStream );
    m_bReady = true;
    return m_bReady;
}

s32 RibbonTrailResource::Release()
{
    if (DropRef() == 0)
    {
        Clear();
    }

    return GetRef();
}
/*
RibbonTrail::RibbonTrail( const char* szName ) : BillBoardChain(szName), m_pResource(NULL)
, mDrawWithBillboard(true)
//, m_bPrepared(false)
{
	m_szType = "ribbontrail";
	this->InitImpl( "" );
	m_bPauseAddElement = false;
	m_fRenderTimeLen = 0.0;
	m_fEnableTimeLen = 0.0;
	m_bAutoDetach = false;
}
*/
RibbonTrail::RibbonTrail(
            size_t maxElements, 
            size_t numberOfChains, 
            bool useTextureCoords, 
            bool useColours ) :
BillBoardChain( maxElements, numberOfChains, useTextureCoords, useColours )
, mDrawWithBillboard(true)
, m_pResource(NULL)
//, m_bPrepared(false)
{
    m_szType                = "ribbontrail";
    mTrackNodes.clear();
    mInitColors.clear();
    mInitWidths.clear();
    mColorChanges.clear();
    mWidthChanges.clear();
    mChangeMasks.clear();
    mTrackNodes.resize( numberOfChains, NULL );
    mInitColors.resize( numberOfChains, SColor(255,255,255,255) );
    mInitWidths.resize( numberOfChains, 10.0f );
    mColorChanges.resize( numberOfChains, CColorValue(-0.1f, 0.0f, 0.0f, 0.0f) );
    mWidthChanges.resize( numberOfChains, -1.0f );
    mChangeMasks.resize( numberOfChains, eCM_ALL_CHANGE );
    //mDrawWithBillboard = true;
	m_bPauseAddElement = false;
	m_fRenderTimeLen = 0.0;
	m_fEnableTimeLen = 0.0;
	m_bAutoDetach = false;
}

RibbonTrail::~RibbonTrail()
{
    Clear();
}


void RibbonTrail::SetNumberOfChains(size_t numChains)
{
	BillBoardChain::SetNumberOfChains( numChains );
	mTrackNodes.clear();
	mInitColors.clear();
	mInitWidths.clear();
	mColorChanges.clear();
	mWidthChanges.clear();
	mChangeMasks.clear();
	mTrackNodes.resize( numChains, NULL );
	mInitColors.resize( numChains, SColor(255,255,255,255) );
	mInitWidths.resize( numChains, 10.0f );
	mColorChanges.resize( numChains, CColorValue(-0.1f, 0.0f, 0.0f, 0.0f) );
	mWidthChanges.resize( numChains, -1.0f );
	mChangeMasks.resize( numChains, eCM_ALL_CHANGE );

}


// 设置链段的元素的初始颜色
void RibbonTrail::SetInitialColour(size_t chainIndex, const SColor& col)
{
    if ( chainIndex >= mInitColors.size() )
    {
        return;
    }
    mInitColors[chainIndex] = col;
}

// 设置链段的元素的初始颜色
void RibbonTrail::SetInitialColour(size_t chainIndex, u8 r, u8 g, u8 b, u8 a /*= 255*/)
{
    if ( chainIndex >= mInitColors.size() )
    {
        return;
    }
    mInitColors[chainIndex].Set( a, r, g, b );
}

// 设置链段的元素的初始颜色
void RibbonTrail::SetInitialColour(size_t chainIndex, const CColorValue& col )
{
    if ( chainIndex >= mInitColors.size() )
    {
        return;
    }
    mInitColors[chainIndex].Set( col.GetAsARGB() );
}

// 获得链段的元素的初始颜色
const SColor& RibbonTrail::GetInitialColour(size_t chainIndex) const
{
    return mInitColors[chainIndex];
}

// 设置链段元素每秒的颜色变化值
void RibbonTrail::SetColourChange(size_t chainIndex, f32 deltaR, f32 deltaG, f32 deltaB, f32 deltaA )
{
    if ( chainIndex >= mColorChanges.size() )
    {
        return;
    }
    mColorChanges[chainIndex] = CColorValue(deltaA, deltaR, deltaG, deltaB );
}

// 设置链段元素每秒的颜色变化值
void RibbonTrail::SetColourChange(size_t chainIndex, const CColorValue& deltaColor )
{
    if ( chainIndex >= mColorChanges.size() )
    {
        return;
    }
    mColorChanges[chainIndex] = deltaColor;
}

// 获得链段元素每秒的颜色变化值
void RibbonTrail::GetColourChange(size_t chainIndex, f32& deltaR, f32& deltaG, f32& deltaB, f32& deltaA )
{
    if ( chainIndex >= mColorChanges.size() )
    {
        return;
    }
    deltaR = mColorChanges[chainIndex].r;
    deltaG = mColorChanges[chainIndex].g;
    deltaB = mColorChanges[chainIndex].b;
    deltaA = mColorChanges[chainIndex].a;
}

// 获得链段元素每秒的颜色变化值
const CColorValue&  RibbonTrail::GetColourChange(size_t chainIndex)
{
    if ( chainIndex >= mColorChanges.size() )
    {
        return CColorValue::White;
    }
    return mColorChanges[chainIndex];
}

// 设置链段的元素的初始大小
void RibbonTrail::SetInitialWidth(size_t chainIndex, f32 width)
{
    if ( chainIndex >= mInitWidths.size() )
    {
        return;
    }
    mInitWidths[chainIndex] = width;
}

// 获得链段的元素的初始大小
f32 RibbonTrail::GetInitialWidth(size_t chainIndex) const
{
    if ( chainIndex >= mInitWidths.size() )
    {
        return 0.0f;
    }
    return mInitWidths[chainIndex];
}

// 设置链段的元素的每秒大小变化值
void RibbonTrail::SetWidthChange(size_t chainIndex, f32 widthDeltaPerSecond)
{
    if ( chainIndex >= mWidthChanges.size() )
    {
        return;
    }
    mWidthChanges[chainIndex] = widthDeltaPerSecond;
}

// 获得链段的元素的每秒大小变化值
f32 RibbonTrail::GetWidthChange(size_t chainIndex) const
{
    if ( chainIndex >= mWidthChanges.size() )
    {
        return 0.0f;
    }
    return mWidthChanges[chainIndex];
}

// 设置链段的追踪节点
void RibbonTrail::SetTrackNode( size_t chainIndex, CSceneNode* pNode )
{
    if ( chainIndex >= mTrackNodes.size() )
    {
        return;
    }
    mTrackNodes[chainIndex] = pNode;
}

// 设置链段的修改掩码
void RibbonTrail::SetChangeMask( size_t chainIndex, u32 changeMask )
{
    if ( chainIndex >= mChangeMasks.size() )
    {
        return;
    }
    mChangeMasks[chainIndex] = (CHANGE_MASK)changeMask;
}

// 渲染
/*
void                RibbonTrail::Render(CCamera* pCamera)
{
    BillBoardChain::Render( pCamera );
}
*/

// 清空
void RibbonTrail::Clear()
{
	//m_szTrailImage.clear();
    BillBoardChain::Clear();

    if (m_pResource)
    {
        RESOURCE_MANAGER->ClearResource(m_pResource);
        m_pResource = NULL;
    }
}


// 修改元素属性 
void RibbonTrail::ChanageElem(size_t chainIndex, Element& elem )
{
    if ( chainIndex >= mChangeMasks.size() )
    {
        return;
    }
    if ( mChangeMasks[chainIndex] == eCM_NO_CHANGE )
    {
        return;
    }
    if ( mChangeMasks[chainIndex] & eCM_COLOR_CHANGE )
    {
        elem.colour.ChangeDelta( 
            mColorChanges[chainIndex].a * ENGINE_INST->GetTimeSinceLastFrame(), 
            mColorChanges[chainIndex].r * ENGINE_INST->GetTimeSinceLastFrame(), 
            mColorChanges[chainIndex].g * ENGINE_INST->GetTimeSinceLastFrame(), 
            mColorChanges[chainIndex].b * ENGINE_INST->GetTimeSinceLastFrame() );
    }
    if ( mChangeMasks[chainIndex] & eCM_WIDTH_CHANGE )
    {
        elem.width += mWidthChanges[chainIndex] * ENGINE_INST->GetTimeSinceLastFrame();
        if ( elem.width < 0.0f )
        {
            elem.width = 0.0f;
        }
    }
}


// 更新
void RibbonTrail::Update( f32 fTimeElapsed )
{
	////////////////////////////////////////////////////////////////////////////
	//static f32 fTime = 0.0f;
	//fTime += fTimeElapsed;
	//if ( fTime < 0.05f )
	//{
	//	return;
	//}
	//else
	//{
	//	fTime = 0.0f;
	//}
	////////////////////////////////////////////////////////////////////////////

	if ( !m_bPauseAddElement )
	{
		m_fRenderTimeLen += fTimeElapsed;
		static f32 fCurUV = 0.0f;
		static f32 deltaUV = 1.0f / (f32)mMaxElementsPerChain;
		BillBoardChain::Element elem;
		for ( size_t i = 0; i < mTrackNodes.size(); ++i )
		{
			CSceneNode* pNode = mTrackNodes[i];
			if ( pNode )
			{
				elem.colour = mInitColors[i];
				elem.position = pNode->GetDerivedPosition();
				elem.upDir = /*pNode->GetDerivedRotation() * */Vector3Df::UNIT_Y;
				elem.texCoord = 0.5f;//fCurUV;
				elem.width = mInitWidths[i];
				AddChainElement( i, elem );
				//fCurUV += deltaUV;
				//if ( fCurUV > 1.0f )
				//{
				//	fCurUV = 0.0f;
				//}
			}
		}
	}

	if ( m_bAutoDetach && IsCompleted() )
	{
		m_pNode->DetachRenderable( this );
	}


    //CParticle* pParticle = NULL;
    /*
    int nIdx = 0;
    for (Particle_Iter it = m_lstTrackParticle.begin(); it != m_lstTrackParticle.end(); ++it, ++nIdx)
    {
        elem.colour = mInitColors[nIdx];
        elem.position = (*it)->m_position;
        elem.upDir = (*it)->m_direction * Vector3Df::UNIT_Y;
        elem.texCoord = 0.5f;//fCurUV;
        elem.width = mInitWidths[nIdx];
        addChainElement(nIdx, elem);
        fCurUV += deltaUV;
        if ( fCurUV > 1.0f )
        {
            fCurUV = 0.0f;
        }
    }
    */

	//-----------------------------------
	//! add by hjj 2010-5-24
	Renderable::UpdateNodeAnimation(fTimeElapsed);
	//-----------------------------------
}

void RibbonTrail::UpdateByParticle(f32 fTimeElapsed, CParticleIterator& particleIter)
{
	if ( m_bPauseAddElement )
	{
		return;
	}
	m_fRenderTimeLen += fTimeElapsed;
    //static f32 fCurUV = 0.0f;
    //static f32 deltaUV = 1.0f / (f32)mMaxElementsPerChain;
    BillBoardChain::Element elem;
    CParticle* pParticle = NULL;
    while (!particleIter.IsEnd())
    {
        //DT_TO_DBGSTR( LT_INFO, "add particle elem:------->\n" );
        pParticle = particleIter.GetNext();
        if ( pParticle->m_nSegmentIndex >= mInitWidths.size() )
        {
            continue;
        }
        elem.colour = pParticle->m_color.GetAsARGB();//mInitColors[pParticle->m_nSegmentIndex];
        elem.position = pParticle->m_position;
        elem.upDir = /*pParticle->m_direction * */Vector3Df::UNIT_Y;
        elem.texCoord = 0.5f;//fCurUV;
        elem.width = mInitWidths[pParticle->m_nSegmentIndex];
        //elem.width = pParticle->m_fWidth; 
        AddChainElement(pParticle->m_nSegmentIndex, elem);
        //fCurUV += deltaUV;
        //if ( fCurUV > 1.0f )
        //{
        //    fCurUV = 0.0f;
        //}
    }

	if ( m_bAutoDetach && IsCompleted() )
	{
		m_pNode->DetachRenderable( this );
	}
}


void RibbonTrail::SetDrawWithBillboard( bool b )
{
    mDrawWithBillboard = b;
    if (m_pResource)
    {
        m_pResource->SetDrawWithBillboard(b);
    }
}


void RibbonTrail::UpdateVertexBuffer(CCamera* cam)
{
    if ( mDrawWithBillboard )
    {
        BillBoardChain::UpdateVertexBuffer( cam );
        return;
    }

	bool bResetBB = false;

    // 设置缓冲
    BillBoardChain::SetupBuffers();

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
			f32 fDeltaUVOffset = 1.0f / f32(seg.curLen-1);
			f32 fBeginUV = 0.0f;
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

                ChanageElem( chainID, elem );

                Vector3Df vPerpendicular = elem.upDir;
                //vPerpendicular *= (elem.width * 0.5);
                //// 顶点0的坐标
                //Vector3Df pos0 = elem.position - vPerpendicular;
                //// 顶点1的坐标
                //Vector3Df pos1 = elem.position + vPerpendicular;

				vPerpendicular *= (elem.width * 0.5f);
				// 顶点0的坐标
				Vector3Df pos0 = elem.position;
				// 顶点1的坐标
				Vector3Df pos1 = elem.position + vPerpendicular;

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
                            pVSTAND->tv = fBeginUV;//elem.texCoord;
                        }
                        ++pVSTAND;
						fBeginUV += fDeltaUVOffset;

                        // 设置顶点1的坐标,颜色和纹理坐标
                        pVSTAND->position = pos1;
                        pVSTAND->color = elem.colour;
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

                        // 设置顶点1的坐标,颜色和纹理坐标
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

/*
void RibbonTrail::SetTrackParticle(CParticle* pParticle)
{
    //if ( chainIndex >= mTrackNodes.size() )
    //{
    //    return;
    //}
    BillBoardChain::ChainSegment* pChainSegment = createChainSegment();
    if (pChainSegment)
    {
        pParticle->m_bTrail = true;
        pParticle->m_pChainSeg = pChainSegment;
        m_lstTrackParticle.push_back(pParticle);
    }

    //pParticle->m_bTrail = true;
}
*/

/*
void RibbonTrail::NotifyParticleDestory(CParticle* pParticle)
{
    Particle_Iter it = std::find(m_lstTrackParticle.begin(), m_lstTrackParticle.end(), pParticle);
    if (it != m_lstTrackParticle.end())
    {
        m_lstTrackParticle.erase(it);
        //销毁所属的段
        destroyChainSegment(pParticle->m_pChainSeg);
    }
    pParticle->m_bTrail = false;
    pParticle->m_pChainSeg = NULL;
}
*/


bool RibbonTrail::LoadFromFile( const char* szFile )
{
    /*
	CDataStream* pStream = FILESYSTEM->GetFileStream(szFile.c_str());
	if (pStream == NULL)
	{
		return false;
	}

	AIRString strDoc = pStream->GetAsString();

	CMarkupSTL xml;
	xml.SetDoc(strDoc.c_str());

	if (!xml.FindElem())
	{
		SafeDelete(pStream);
		return false;
	}

	Clear();

	m_strName = xml.GetAttrib("Name");

	mMaxElementsPerChain = atoi( xml.GetAttrib("MaxElemPerChain").c_str() );

	mChainCount = atoi( xml.GetAttrib("ChainCount").c_str() );

	AIRString szValue;
	szValue = xml.GetAttrib( "UseVertexColor" );
	if ( szValue == "FALSE" )
	{
		mUseVertexColour = false;
	}
	else if ( szValue == "TRUE" )
	{
		mUseVertexColour = true;
	}

	szValue = xml.GetAttrib( "UseTexCoord" );
	if ( szValue == "FALSE" )
	{
		mUseTexCoords = false;
	}
	else if ( szValue == "TRUE" )
	{
		mUseTexCoords = true;
	}
	
	InitImpl( m_strName, mMaxElementsPerChain, mChainCount, mUseTexCoords, mUseVertexColour );

	szValue = xml.GetAttrib( "DrawWithBillBoard" );
	if ( szValue == "FALSE" )
	{
		mDrawWithBillboard = false;
	}
	else if ( szValue == "TRUE" )
	{
		mDrawWithBillboard = true;
	}

	char buf[512];
	snprintf( buf, 512, "%s", xml.GetAttrib("TexCoordRange").c_str() );
	sscanf( buf, "(%f,%f)", &mOtherTexCoordRange[0], &mOtherTexCoordRange[1] );

	mTexCoordDir = (BillBoardChain::TexCoordDirection)atoi( xml.GetAttrib("TexCoordDirection").c_str() );

	AIRString szTrailTexImage = xml.GetAttrib( "TrailTexImage" );
	SetTrailImage( szTrailTexImage );
	//if ( !m_pMaterial )
	//{
	//	m_pMaterial = MATERIAL_MANAGER->CreateMaterial("TemplateMaterial.xml/RibbonTrail");
	//}
	//m_pMaterial->GetCurrentTechnique()->SetTextureUnitTex( 0, szTrailTexImage.c_str(), 1 );

	xml.IntoElem();

	int nCurID = 0;
	while( xml.FindElem() )
	{
		szValue = xml.GetAttrib("InitColor");
		CColorValue colInit;
		sscanf( szValue.c_str(), "(%f,%f,%f,%f)", &colInit.a, &colInit.r, &colInit.g, &colInit.a );
		f32 fInitWidth = atof( xml.GetAttrib("InitWidth").c_str() );
		CColorValue colChg;
		szValue = xml.GetAttrib("ChangeColor");
		sscanf( szValue.c_str(), "(%f,%f,%f,%f)", &colChg.a, &colChg.r, &colChg.g, &colChg.a );
		f32 fChgWidth = atof( xml.GetAttrib("ChangeWidth").c_str() );
	
		SetInitialColour( nCurID, colInit );
		SetInitialWidth( nCurID, fInitWidth );
		SetColourChange( nCurID, colChg );
		SetWidthChange( nCurID, fChgWidth );

		++nCurID;
	}

	xml.OutOfElem();

	SafeDelete( pStream );

	return true;
    */
    
    //m_pResource = static_cast<RibbonTrailResource*>(RESOURCE_MANAGER->CreateResource(Resource::Res_RibbonTrail, szFile));
    m_pResource = RESOURCE_MANAGER->CreateResource<RibbonTrailResource>(szFile);

    if (m_pResource)
    {
        //m_pResource->LoadFromFile(szFile);

        if (m_pResource->IsLoaded())
        {
            if (!m_pResource->IsReady())
            {
                m_pResource->Cache();
                SetTrailImage(m_pResource->GetTrailImageFile());

				mMaxElementsPerChain = m_pResource->GetMaxElementsPerChain();
				mChainCount = m_pResource->GetChainCount();
				m_pResource->GetOtherTexCoordRange( mOtherTexCoordRange[0], mOtherTexCoordRange[1] );
				mTexCoordDir = m_pResource->GetTextureCoordDir();
				mUseTexCoords = m_pResource->IsUseTexCoords();
				mUseVertexColour = m_pResource->IsUseVertexColor();
				mDrawWithBillboard = m_pResource->IsDrawWithBillboard();
            }
        }
    }
    return m_pResource;
}


bool RibbonTrail::SaveToFile( const AIRString& szFile )
{
	CMarkupSTL xml;

	xml.SetDoc("<?xml version=\"1.0\" encoding=\"gb2312\"?>\r\n");
	xml.AddElem("RibbonTrail");

	xml.AddAttrib("Name", m_strName.c_str());

	char szTmp[256] = { 0 };

	xml.AddAttrib("MaxElemPerChain",		mMaxElementsPerChain );

	xml.AddAttrib("ChainCount",	mChainCount );

	if ( mUseVertexColour )
	{
		xml.AddAttrib( "UseVertexColour", "TRUE" );
	}
	else
	{
		xml.AddAttrib( "UseVertexColour", "FALSE" );
	}

	if ( mUseTexCoords )
	{
		xml.AddAttrib( "UseTexCoord", "TRUE" );
	}
	else
	{
		xml.AddAttrib( "UseTexCoord", "FALSE" );
	}

	if ( m_pResource->IsDrawWithBillboard() )
	{
		xml.AddAttrib( "DrawWithBillBoard", "TRUE" );
	}
	else
	{
		xml.AddAttrib( "DrawWithBillBoard", "FALSE" );
	}

	xml.AddAttrib("TexCoordRange",	GetString("(%f,%f)", mOtherTexCoordRange[0], mOtherTexCoordRange[1]) );
	xml.AddAttrib("TexCoordDirection",	int(mTexCoordDir) );
	xml.AddAttrib("TrailTexImage",		m_pResource->GetTrailImageFile().c_str() );

	xml.IntoElem();

	for ( size_t i = 0; i < mChainCount; ++i )
	{
		CColorValue col = SColorToCColorValue( mInitColors[i] );
		xml.AddElem( "ChainInfo" );
		xml.AddAttrib( "InitColor", GetString("(%f,%f,%f,%f)", col.a, col.r, col.g, col.b ) );
		xml.AddAttrib( "InitWidth", GetString("%f",mInitWidths[i]) );
		col = mColorChanges[i];
		xml.AddAttrib( "ChangeColor", GetString("(%f,%f,%f,%f)", col.a, col.r, col.g, col.b ) );
		xml.AddAttrib( "ChangeWidth", GetString("%f",mWidthChanges[i]) );
	}

	xml.OutOfElem();

	return xml.Save(szFile.c_str());
	return true;
}




void RibbonTrail::InitImpl(const AIRString& name, size_t maxElements /* = 20 */, size_t numberOfChains /* = 1 */, bool useTextureCoords /* = true */, bool useColours /* = true */, bool dynamic /* = true */)
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
	mVertexDecl = (EVertexBufferType)INVALID_VERTEX_DECL;
	m_pBillboardChainData = NULL;
	m_strName               = name;
	mOtherTexCoordRange[0]  = 0.0f;
	mOtherTexCoordRange[1]  = 1.0f;
	SetupChainContainers();

	mTrackNodes.clear();
	mInitColors.clear();
	mInitWidths.clear();
	mColorChanges.clear();
	mWidthChanges.clear();
	mChangeMasks.clear();
	mTrackNodes.resize( numberOfChains, NULL );
	mInitColors.resize( numberOfChains, SColor(255,255,255,255) );
	mInitWidths.resize( numberOfChains, 10.0f );
	mColorChanges.resize( numberOfChains, CColorValue(-0.1f, 0.0f, 0.0f, 0.0f) );
	mWidthChanges.resize( numberOfChains, -1.0f );
	mChangeMasks.resize( numberOfChains, eCM_ALL_CHANGE );

	//mDrawWithBillboard = true;
}

void RibbonTrail::SetTrailImage( const AIRString& szFileName )
{
	if ( !m_pMaterial )
	{
		m_pMaterial = MATERIAL_MANAGER->CreateMaterial("TemplateMaterial.xml/RibbonTrail");
	}
    if (m_pMaterial->IsReady())
    {
        //m_pMaterial->GetCurrentTechnique()->SetTextureUnitTex( 0, szFileName.c_str(), 1 );
    }
	
	//m_szTrailImage = szFileName;
}



bool RibbonTrail::IsCompleted( void )
{
	if ( m_bPauseAddElement )
	{
		if ( m_fRenderTimeLen >= m_fEnableTimeLen && m_fEnableTimeLen > 0.0 )
		{
			return true;
		}
	}
	return false;
}

void RibbonTrail::Start()
{
	m_fRenderTimeLen = 0;
	Reset();
}

bool RibbonTrail::Prepare()
{
    if (!m_bPrepared)
    {
        if (m_pResource)
        {
            if (m_pResource->IsLoaded())
            {
                if (m_pResource->IsReady())
                {
                    //if (!m_pMaterial)
                    //{
                    //    //m_bInitialize = true;
                    //    //return false;
                    //}

                    if (m_pMaterial)
                    {
                        if (!m_pMaterial->IsReady())
                        {
                            m_pMaterial->Cache();
                        }
                        if (m_pMaterial->IsReady())
                        {
                            //m_pMaterial->GetCurrentTechnique()->GetPass(0)->GetTextureUnit(0)->SetTexture(m_pResource->GetTrailImageFile().c_str(), 0);
                            m_bPrepared = true;
                        }
                        //m_bPrepared = m_pMaterial->IsReady();
                    }

					InitImpl( m_strName, m_pResource->GetMaxElementsPerChain(), m_pResource->GetChainCount(), 
						m_pResource->IsUseTexCoords(), m_pResource->IsUseVertexColor() );

					SetTrailImage(m_pResource->GetTrailImageFile());

					SetInitialColour(0, m_pResource->GetColorInit(0));
					SetColourChange(0, m_pResource->GetColorChange(0));
					SetInitialWidth(0, m_pResource->GetWidthInit(0));
					SetWidthChange(0, m_pResource->GetWidthChange(0));

					mDrawWithBillboard = m_pResource->IsDrawWithBillboard();
					SetTextureCoordDirection(m_pResource->GetTextureCoordDir());
                }
                else
                {
                    m_pResource->Cache();
                }
            }
        }
        else
        {
            m_bPrepared = true;
        }
    }

    if (m_bPrepared)
    {
        m_bPrepared = Renderable::Prepare();
    }
    
    
    return m_bPrepared;
}

RibbonTrailManager::RibbonTrailManager()
{
	m_nCounter = 0;
}

RibbonTrailManager::~RibbonTrailManager()
{
	Clear();
}


RibbonTrail* RibbonTrailManager::CreateRibbonTrail(const char* szName, const char* szFilename)
{
	RibbonTrail* pObject = new RibbonTrail();
	if (pObject->LoadFromFile(szFilename))
	{
		pObject->SetName(szName);
		pObject->SetID(m_nCounter++);
		m_vtObject.push_back(pObject);

		pObject->SetResourceName( szFilename );
	}
	else 
	{
		DT_TO_MSGBOX_FMT( LT_WARN, GetString("Create object [%s] from file [%s] failed!", szName, szFilename) );
		pObject->Clear();
		SafeDelete(pObject);
	}
	return pObject;
}

void RibbonTrailManager::DestroyRibbonTrail(unsigned int nID)
{
	RibbonTrail* pObject = NULL;
	OBJECT_ARRAY_ITER itEnd = m_vtObject.end();
	for (OBJECT_ARRAY_ITER it = m_vtObject.begin(); it != itEnd; ++it)
	{
		pObject = *it;
		if (pObject->GetID() == nID)
		{
			pObject->Clear();
			SafeDelete(pObject);
			m_vtObject.erase(it);
			break;
		}
	}
}

void RibbonTrailManager::DestroyRibbonTrail(const AIRString& strName)
{
	RibbonTrail* pObject = NULL;
	OBJECT_ARRAY_ITER itEnd = m_vtObject.end();
	for (OBJECT_ARRAY_ITER it = m_vtObject.begin(); it != itEnd; ++it)
	{
		pObject = *it;
		if (pObject->GetName() == strName)
		{
			pObject->Clear();
			SafeDelete(pObject);
			m_vtObject.erase(it);
			break;
		}
	}
}

void RibbonTrailManager::UpdateAllRibbonTrail(f32 fTimeElapsed)
{
	RibbonTrail* pObject = NULL;
	OBJECT_ARRAY_ITER itEnd = m_vtObject.end();
	for (OBJECT_ARRAY_ITER it = m_vtObject.begin(); it != itEnd; ++it)
	{
		pObject = *it;
		pObject->Update(fTimeElapsed);
	}
}

void RibbonTrailManager::Clear()
{
	RibbonTrail* pObject = NULL;
	OBJECT_ARRAY_ITER itEnd = m_vtObject.end();
	for (OBJECT_ARRAY_ITER it = m_vtObject.begin(); it != itEnd; ++it)
	{
		pObject = *it;
		pObject->Clear();
		SafeDelete(pObject);
	}
	m_vtObject.clear();
}

void RibbonTrailManager::AddRibbonTrail(RibbonTrail* pRenderable)
{
	for (OBJECT_ARRAY_ITER it = m_vtObject.begin(); it != m_vtObject.end(); ++it)
	{
		if (pRenderable == *it)
		{
			DT_TO_MSGBOX_FMT( LT_WARN, GetString("object [addres:%p, name:%s, id:%d] has exist!", pRenderable, pRenderable->GetName().c_str(), pRenderable->GetID() ) );
			return;
		}
	}
	m_vtObject.push_back(pRenderable);
}

RibbonTrail* RibbonTrailManager::GetRibbonTrail(const AIRString& strName)
{
	for (OBJECT_ARRAY_ITER it = m_vtObject.begin(); it != m_vtObject.end(); ++it)
	{
		if ((*it)->GetName() == strName)
		{
			DT_TO_MSGBOX_FMT( LT_WARN, GetString("object [name:%s, id:%d]] has exist!", strName.c_str(), (*it)->GetID() ) );
			return *it;
		}
	}

    return NULL;
}

RibbonTrail* RibbonTrailManager::GetRibbonTrail(unsigned int nID)
{
	for (OBJECT_ARRAY_ITER it = m_vtObject.begin(); it != m_vtObject.end(); ++it)
	{
		if ((*it)->GetID() == nID)
		{
			DT_TO_MSGBOX_FMT( LT_WARN, GetString("object [name:%s, id:%d]] has exist!", (*it)->GetName().c_str(), (*it)->GetID() ) );
			return *it;
		}
	}

    return NULL;
}
