#include "stdafx.h"
#include "TerrainDecal.h"
#include "MaterialManager.h"
#include "Engine.h"
#include "HeightMap.h"
#include "NavEditor.h"
#include "Primitive.h"
//#include "TextureUnit.h"
#include "IMaterial.h"
//#include "Technique.h"
#include "FileData.h"
#include "Terrain.h"

TerrainDecal::TerrainDecal() : m_fWidth(0), m_fHeight(0)
, m_fBottomRightU(1.0f)
, m_fBottomRightV(1.0f)
, m_bNeedCaculate(false)
, m_bLoadFromFile(false)
, m_pFileData(NULL)
, m_pTerrain(NULL)
{
    m_szType = "TerrainDecal";
    m_bTransformByParent = false;
	m_pMaterial = MATERIAL_MANAGER->CreateMaterial("TemplateMaterial.xml/Particle");
	if ( !m_pMaterial->IsReady() )
	{
		m_pMaterial->Cache();
	}
}

TerrainDecal::~TerrainDecal()
{

}

bool TerrainDecal::Create(CHeightMap* pTerrain, const char* szTextureFile, float fWidth, float fHeight)
{
    m_pTerrain = pTerrain;
    if (m_pMaterial)
    {
        //m_pMaterial->SetTextureUnitTex(0, szTextureFile, 1);
    }
    else
    {
        m_pMaterial = MATERIAL_MANAGER->CreateMaterial("TemplateMaterial.xml/Particle");
        if (!m_pMaterial)
        {
            return false;
        }        
    }

    if (!m_pMaterial->IsReady())
    {
        m_pMaterial->Cache();
    }

    if (m_pMaterial->IsReady())
    {
        /*
        m_pMaterial->SetTextureUnitTex(0, szTextureFile, 1);
        CTextureUnit* pTextureUnit = m_pMaterial->GetTechnique(0)->GetPass(0)->GetTextureUnit(0);
        m_pMaterial->SetZWriteEnable(false);
        m_pMaterial->SetZBufferEnable(true);

        m_pMaterial->SetBlendSrc(RSRV_SrcAlpha);
        m_pMaterial->SetBlendDest(RSRV_InvSrcAlpha);
        */
    }
    
    //m_pMaterial->SetSlopeScaleDepthBias(0.0f);
    //m_pMaterial->SetDepthBias(-0.000005f);

    m_fWidth = fWidth;
    m_fHeight = fHeight;

	m_szDecalTexture = szTextureFile;

    m_bNeedCaculate = true;
    //InitializeRenderData();
    //ComputeRenderData();
    
    return true;
}

void TerrainDecal::SetDecalTexture( const char* szTextureFile )
{
	if ( m_szDecalTexture != szTextureFile )
	{
		if (m_pMaterial)
		{
			//m_pMaterial->SetTextureUnitTex(0, szTextureFile, 1);
		}
		else
		{
			m_pMaterial = MATERIAL_MANAGER->CreateMaterial("TemplateMaterial.xml/Particle");
			if (!m_pMaterial)
			{
				return;
			}        
		}

		if (!m_pMaterial->IsReady())
		{
			m_pMaterial->Cache();
		}

		if (m_pMaterial->IsReady())
		{
			//m_pMaterial->SetTextureUnitTex(0, szTextureFile, 1);
			//CTextureUnit* pTextureUnit = m_pMaterial->GetTechnique(0)->GetPass(0)->GetTextureUnit(0);
			//pTextureUnit->SetTextureAddressMode("TADDRESS_CLAMP");
			m_pMaterial->SetZWriteEnable(false);
			m_pMaterial->SetZBufferEnable(true);

            m_pMaterial->SetBlendSrc(RSRV_SrcAlpha);
            m_pMaterial->SetBlendDest(RSRV_InvSrcAlpha);
		}

		m_szDecalTexture = szTextureFile;
	}
}

void TerrainDecal::InitializeRenderData()
{
    //OutputDebugString(GetString("Terrain decal change width in thread[%d]\n", ::GetCurrentThreadId()));
    IRenderer* pRenderer = ENGINE_INST->GetRenderer();
    if (m_renderData.m_pVB)
    {
        pRenderer->ClearVertexBuffer(m_renderData.m_pVB);
        m_renderData.m_pVB = NULL;
    }
    if (m_renderData.m_pIB)
    {
        pRenderer->ClearIndexBuffer(m_renderData.m_pIB);
        m_renderData.m_pIB = NULL;
    }
    m_renderData.m_nPrimCount = 0;
    
    //计算出最多需要的顶点数，创建VB和IB
    u32 nGridsWidth  = 2;
    u32 nGridsHeight = 2;
	if ( m_pTerrain )
	{
		nGridsWidth = m_fWidth / m_pTerrain->GetGridWidth() + 2;
		nGridsHeight = m_fHeight / m_pTerrain->GetGridDepth() + 2;
	}
    int nMaxGridNum = nGridsWidth * nGridsHeight;
    u32 nMaxVertexNum = nMaxGridNum * 4;
    //m_renderData.m_pVB = pRenderer->CreateVertexBuffer(E_VB_STANDARD, nMaxVertexNum);
    m_renderData.m_pVB = pRenderer->CreateVertexBuffer(nMaxVertexNum, sizeof(VERTEX_STANDARD));
    m_renderData.m_pIB = pRenderer->CreateIndexBuffer(nMaxGridNum * 6);
    m_renderData.m_renderType = RT_TRIANGLE_LIST;

    u16* pIndices = (u16*)m_renderData.m_pIB->Lock(0, 0);

    for(size_t idx, idxOff, bboard = 0;
        bboard < nMaxGridNum;
		++bboard )
    {
        // Do indexes
        idx    = bboard * 6;
        idxOff = bboard * 4;

        pIndices[idx]		= static_cast<unsigned short>(idxOff); // + 0;, for clarity
        pIndices[idx + 1]	= static_cast<unsigned short>(idxOff + 1);
        pIndices[idx + 2]	= static_cast<unsigned short>(idxOff + 2);
        pIndices[idx + 3]	= static_cast<unsigned short>(idxOff + 1);
        pIndices[idx + 4]	= static_cast<unsigned short>(idxOff + 3);
        pIndices[idx + 5]	= static_cast<unsigned short>(idxOff + 2);

    }
    m_renderData.m_nPrimCount = nGridsWidth * nGridsHeight * 2;
    m_renderData.m_pIB->Unlock();
}

void TerrainDecal::SetWidth(float fWidth)
{
    if (!Equals(m_fWidth, fWidth))
    {
		//DT_TO_DBGSTR(LT_NONE, GetString("Set terraindecal width in thread[%d]\n", ::GetCurrentThreadId()));
        m_fWidth = fWidth;
        m_bNeedCaculate = true;
        //ComputeRenderData();
        //InitializeRenderData();
    }
    
}

void TerrainDecal::SetHeight(float fHeight)
{
    if (!Equals(m_fHeight, fHeight))
    {
        m_fHeight = fHeight;
        m_bNeedCaculate = true;
        //ComputeRenderData();
        //InitializeRenderData();
    }
    
}

void TerrainDecal::ShowDecal(const Vector3Df& terrainPosition)
{
    if (m_worldPosition != terrainPosition)
    {
        //ComputeRenderData();
		m_worldPosition = terrainPosition;
        m_bNeedCaculate = true;

		float fDecalTopLeftX	 = terrainPosition.x - m_fWidth / 2;
		float fDecalTopLeftY	 = terrainPosition.z + m_fHeight / 2;
		float fDecalBottomRightX = terrainPosition.x + m_fWidth / 2;
		float fDecalBottomRightY = terrainPosition.z - m_fHeight / 2;
		m_boundingBox.MinEdge.Set(fDecalTopLeftX, terrainPosition.y, fDecalBottomRightY);
		m_boundingBox.MaxEdge.Set(fDecalBottomRightX, 0, fDecalTopLeftY);

		if (m_pNode)
		{
			m_pNode->NeedUpdate();
			m_pNode->Update();
		}
    }
    /*
	Vector3Df maxEdge;
	Vector3Df minEdge;
    float fDecalTopLeftX	 = terrainPosition.x - m_fWidth / 2;
    float fDecalTopLeftY	 = terrainPosition.z + m_fHeight / 2;
    float fDecalBottomRightX = terrainPosition.x + m_fWidth / 2;
    float fDecalBottomRightY = terrainPosition.z - m_fHeight / 2;

	//没有地形时的贴花创建
	if ( !m_pTerrain )
	{
		LPVERTEX_STANDARD pVertices = (LPVERTEX_STANDARD)m_renderData.m_pVB->Lock();

		//左上
		pVertices->color = m_colorDiffuse;
		pVertices->position = Vector3Df( fDecalTopLeftX, 0, fDecalTopLeftY );
		pVertices->tu = 0;
		pVertices->tv = 0;
		pVertices++;

		//右上
		pVertices->color = m_colorDiffuse;
		pVertices->position = Vector3Df( fDecalBottomRightX, 0, fDecalTopLeftY );
		pVertices->tu = m_fBottomRightU;
		pVertices->tv = 0;
		pVertices++;

		//左下
		pVertices->color = m_colorDiffuse;
		pVertices->position = Vector3Df( fDecalTopLeftX, 0, fDecalBottomRightY );
		pVertices->tu = 0;
		pVertices->tv = m_fBottomRightV;
		pVertices++;

		//右下
		pVertices->color = m_colorDiffuse;
		pVertices->position = Vector3Df( fDecalBottomRightX, 0, fDecalBottomRightY );
		pVertices->tu = m_fBottomRightU;
		pVertices->tv = m_fBottomRightV;
		m_renderData.m_pVB->Unlock();

		maxEdge.Set( fDecalBottomRightX, 0, fDecalTopLeftY );
		minEdge.Set( fDecalTopLeftX, 0, fDecalBottomRightY );

		m_renderData.m_nPrimCount = 2;
	}
	//有地形时的贴花创建
	else if (m_pTerrain && m_pTerrain->IsPrepared())
	{
		//左上角所在的地形顶点格子
		TerrainGrid gridTopLeft = m_pTerrain->GetTerrainGrid( fDecalTopLeftX, fDecalTopLeftY );
		//右下角所在的地形顶点格子
		TerrainGrid gridBottomRight = m_pTerrain->GetTerrainGrid( fDecalBottomRightX, fDecalBottomRightY );

		//左上角的顶点的世界坐标
		const Vector3Df& VertexTopLeft = m_pTerrain->GetVertex(gridTopLeft.nRow * m_pTerrain->GetWidthSize() + gridTopLeft.nCol)->vPos;
		float fDecalOffsetX = fDecalTopLeftX - VertexTopLeft.x;
		float fDecalOffsetY = -(fDecalTopLeftY - VertexTopLeft.z);
    
		int nRealBottomRightRow = gridBottomRight.nRow + 1;
		int nRealBottomRightCol = gridBottomRight.nCol + 1;
		if (nRealBottomRightRow > m_pTerrain->GetWidthSize() - 1)
		{
			nRealBottomRightRow = m_pTerrain->GetWidthSize() - 1;
		}
		if (nRealBottomRightCol = m_pTerrain->GetWidthSize() - 1)
		{
			nRealBottomRightCol = m_pTerrain->GetWidthSize() - 1;
		}

		//右下角的项点
		const Vector3Df& VertexBottomRight = m_pTerrain->GetVertex((nRealBottomRightRow) * m_pTerrain->GetWidthSize() +
			nRealBottomRightCol)->vPos;

		float invDecalX  = 1.0f / m_fWidth;
		float invDecalY  = 1.0f / m_fHeight;

		LPVERTEX_STANDARD pVertices = (LPVERTEX_STANDARD)m_renderData.m_pVB->Lock();

		maxEdge.x = VertexBottomRight.x;
		maxEdge.z = VertexTopLeft.z;
		maxEdge.y = VertexTopLeft.y;
		minEdge.x = VertexTopLeft.x;
		minEdge.z = VertexBottomRight.z;
		minEdge.y = VertexBottomRight.y;
		float fUDelta = m_pTerrain->GetGridWidth() / m_fWidth * m_fBottomRightU;
		float fVDelta = m_pTerrain->GetGridDepth() / m_fHeight * m_fBottomRightV;

		int nGridWidth = gridBottomRight.nCol - gridTopLeft.nCol + 1;  //格的列数
		int nGridHeight = gridBottomRight.nRow - gridTopLeft.nRow + 1;  //格的行数
    
		for (int nRow = gridTopLeft.nRow, x = 0; nRow <= gridBottomRight.nRow; ++nRow, ++x)
		{
			for (int nCol = gridTopLeft.nCol, y = 0; nCol <= gridBottomRight.nCol; ++nCol, ++y)
			{
				//一格一格的数据填充
				//左上角
				LPVERTEX_NORMAL pTerrainVertex = m_pTerrain->GetVertex(nRow * m_pTerrain->GetWidthSize() + nCol);
				pVertices->color = m_colorDiffuse;
				pVertices->position = pTerrainVertex->vPos;
				pVertices->position.y += 5.0f;// + NAV_EDITOR->GetWorldPosHeight( pTerrainVertex->vPos.x, pTerrainVertex->vPos.z );
				pVertices->tu = y * m_pTerrain->GetGridWidth() * invDecalX - fDecalOffsetX * invDecalX;
				pVertices->tv = x * m_pTerrain->GetGridDepth() * invDecalY - fDecalOffsetY * invDecalY;
				maxEdge.y = Max(maxEdge.y, pVertices->position.y);
				minEdge.y = Min(minEdge.y, pVertices->position.y);
				pVertices++;

				//右上角
				int nRealCol = nCol + 1;
				if (nRealCol > m_pTerrain->GetGridsNumPerCol() )
				{
					nRealCol = m_pTerrain->GetGridsNumPerCol();
				}
				pTerrainVertex = m_pTerrain->GetVertex(nRow * m_pTerrain->GetWidthSize() + nRealCol);
				pVertices->color = m_colorDiffuse;
				pVertices->position = pTerrainVertex->vPos;
				pVertices->position.y += 5.0f;// + NAV_EDITOR->GetWorldPosHeight( pTerrainVertex->vPos.x, pTerrainVertex->vPos.z );
				pVertices->tu = y * m_pTerrain->GetGridWidth() * invDecalX - fDecalOffsetX * invDecalX + fUDelta;
				pVertices->tv = x * m_pTerrain->GetGridDepth() * invDecalY - fDecalOffsetY * invDecalY;
				maxEdge.y = Max(maxEdge.y, pVertices->position.y);
				minEdge.y = Min(minEdge.y, pVertices->position.y);
				pVertices++;

				//左下角
				int nRealRow = nRow + 1;
				if (nRealRow > m_pTerrain->GetGridsNumPerRow())
				{
					nRealRow = m_pTerrain->GetGridsNumPerRow();
				}
				pTerrainVertex = m_pTerrain->GetVertex(nRealRow * m_pTerrain->GetWidthSize() + nCol);
				pVertices->color = m_colorDiffuse;
				pVertices->position = pTerrainVertex->vPos;
				pVertices->position.y += 5.0f;// + NAV_EDITOR->GetWorldPosHeight( pTerrainVertex->vPos.x, pTerrainVertex->vPos.z );
				pVertices->tu = y * m_pTerrain->GetGridWidth() * invDecalX - fDecalOffsetX * invDecalX;
				pVertices->tv = x * m_pTerrain->GetGridDepth() * invDecalY - fDecalOffsetY * invDecalY + fVDelta;
				maxEdge.y = Max(maxEdge.y, pVertices->position.y);
				minEdge.y = Min(minEdge.y, pVertices->position.y);
				pVertices++;

				//右下角
				pTerrainVertex = m_pTerrain->GetVertex(nRealRow * m_pTerrain->GetWidthSize() + nRealCol);
				pVertices->color = m_colorDiffuse;
				pVertices->position = pTerrainVertex->vPos;
				pVertices->position.y += 5.0f;// + NAV_EDITOR->GetWorldPosHeight( pTerrainVertex->vPos.x, pTerrainVertex->vPos.z );
				pVertices->tu = y * m_pTerrain->GetGridWidth() * invDecalX - fDecalOffsetX * invDecalX + fUDelta;
				pVertices->tv = x * m_pTerrain->GetGridDepth() * invDecalY - fDecalOffsetY * invDecalY + fVDelta;
				maxEdge.y = Max(maxEdge.y, pVertices->position.y);
				minEdge.y = Min(minEdge.y, pVertices->position.y);
				pVertices++;
			}
		}

		m_renderData.m_pVB->Unlock();
		m_renderData.m_nPrimCount = nGridWidth * nGridHeight * 2;
	}

    m_boundingBox.MaxEdge = maxEdge;
    m_boundingBox.MinEdge = minEdge;
    m_WorldAABB = m_boundingBox;
    m_vtCenter = m_boundingBox.GetCenter();
    

    if (m_pNode)
    {
        //m_pNode->SetPosition(GetWorldPosition());
        m_pNode->NeedUpdate();
        m_pNode->Update();
		m_worldPosition = m_pNode->GetDerivedPosition();
    }
    
    m_bNeedCaculate = false;
    */
}


bool TerrainDecal::LoadFromFile(const char* szFilename)
{
	//XMLFileData* pFileData = static_cast<XMLFileData*>(FILEDATA_MANAGER->CreateFileData(szFilename));
    m_pFileData = RESOURCE_MANAGER->CreateResource<FileData>(szFilename);

    if (m_pFileData && m_pFileData->IsLoaded())
    {
        Cache();
    }
   

	SetResourceName(szFilename);

	return true;
}

bool TerrainDecal::Cache()
{
    CDataStream* pStream = m_pFileData->GetFileStream(); //FILESYSTEM->GetFileStream(szFilename);
    if (pStream == NULL)
    {
        return false;
    }

    //AIRString strDoc = pStream->GetAsString();

    CMarkupSTL xml;
    xml.SetDoc(pStream->GetAsString().c_str());

    if (!xml.FindElem())
    {
        return false;
    }

    AIRString szValue = xml.GetAttrib( "Width" ).c_str();
    m_fWidth = atof( szValue.c_str() );

    szValue = xml.GetAttrib( "Height" ).c_str();
    m_fHeight = atof( szValue.c_str() );

    szValue = xml.GetAttrib("BottomRightU").c_str();
    if (szValue.length())
    {
        m_fBottomRightU = atof(szValue.c_str());
    }
    szValue = xml.GetAttrib("BottomRightV").c_str();
    if (szValue.length())
    {
        m_fBottomRightV = atof(szValue.c_str());
    }

    szValue = xml.GetAttrib( "DecalTexture" ).c_str();
    Create(TERRAIN->GetCurHeightmap(), szValue.c_str(), m_fWidth, m_fHeight );
    RESOURCE_MANAGER->ClearResource(m_pFileData);
    m_pFileData = NULL;
    return true;
}

bool TerrainDecal::SaveToFile(const char* szFilename)
{
	CMarkupSTL xml;

	xml.SetDoc("<?xml version=\"1.0\" encoding=\"gb2312\"?>\r\n");
	xml.AddElem("Decal");
	
	xml.AddAttrib( "Width", GetString("%f",m_fWidth) );
	xml.AddAttrib( "Height", GetString("%f",m_fHeight) );
    xml.AddAttrib("BottomRightU", GetString("%f", m_fBottomRightU));
    xml.AddAttrib("BottomRightV", GetString("%f", m_fBottomRightV));
	xml.AddAttrib( "DecalTexture", m_szDecalTexture.c_str() );

	return xml.Save(szFilename);
}

void TerrainDecal::SetBottomRightUV(float fU, float fV)
{
    m_fBottomRightU = fU;
    m_fBottomRightV = fV;
}

void TerrainDecal::SetAttachedNode(CSceneNode* pNode)
{
    Renderable::SetAttachedNode(pNode);
    if (pNode)
    {
        m_worldPosition = m_pNode->GetDerivedPosition();
        m_bNeedCaculate = true;
        //ShowDecal(pNode->GetDerivedPosition());
    }
    
}

void TerrainDecal::PreRender(CCamera* pCamera)
{
    /*
    if (m_pNode && m_pNode->GetDerivedPosition() != m_worldPosition || m_bNeedCaculate)
    {
        ShowDecal(m_worldPosition);
    }
    */
    if (m_bNeedCaculate)
    {
        ComputeRenderData();
    }

    //OutputDebugString(GetString("Terrain decal render in thread[%d]\n", ::GetCurrentThreadId()));
}

void TerrainDecal::SetTransparent(bool bAlpha , u8 nAlpha)
{
    if (m_colorDiffuse.GetAlpha() != nAlpha)
    {
        m_bNeedCaculate = true;
    }
    Renderable::SetTransparent(bAlpha, nAlpha);
}

void TerrainDecal::_GetPrimitive()
{
    if (m_pPrimitive == NULL)
    {
        m_pPrimitive = new Primitive(Primitive::Primitive_TriangleList/*, m_renderData.m_pVB->GetVertexDeclaration()*/);
	}
    m_pPrimitive->m_pSrcRenderable = this;
    m_pPrimitive->m_pVertexBuffer = m_renderData.m_pVB;
    m_pPrimitive->m_pIndexBuffer = m_renderData.m_pIB; 
    
}

bool TerrainDecal::Prepare()
{
    if (!m_bPrepared)
    {
		bool bReady = true;
        if (m_pFileData)
        {
            if (m_pFileData->IsLoaded())
            {
                bReady = Cache();
            }
			else
			{
				bReady = false;
			}
        }

        if (bReady && !m_pMaterial->IsReady())
        {
            bReady = m_pMaterial->Cache();
        }

        if (bReady && m_pMaterial->IsReady())
        {
            /*
            CTextureUnit* pUnit = m_pMaterial->GetTechnique(0)->GetPass(0)->GetTextureUnit(0);

            if (pUnit->GetTextureFileName() != m_szDecalTexture)
            {
                pUnit->SetTexture(m_szDecalTexture.c_str(), 0);
            }
            else
            {
                m_bPrepared = true;
            }
            */
        }

		m_bPrepared = bReady; 
    }

    return m_bPrepared;
}

void TerrainDecal::ComputeRenderData()
{
    float fDecalTopLeftX	 = m_worldPosition.x - m_fWidth / 2;
    float fDecalTopLeftY	 = m_worldPosition.z + m_fHeight / 2;
    float fDecalBottomRightX = m_worldPosition.x + m_fWidth / 2;
    float fDecalBottomRightY = m_worldPosition.z - m_fHeight / 2;

    bool bLeftPass = false;
    bool bTopPass = false;
    bool bRightPass = false;
    bool bBottomPass = false;
    
	//计算出最多需要的顶点数，创建VB和IB
	u32 nGridRowsNum = 2;
	u32 nGridColsNum = 2;

	TerrainGrid gridTopLeft;
	TerrainGrid gridBottomRight;
	if ( m_pTerrain )
	{
		//左上角所在的地形顶点格子
		gridTopLeft = m_pTerrain->GetTerrainGrid( fDecalTopLeftX, fDecalTopLeftY, bLeftPass, bTopPass, bRightPass, bBottomPass );
		//右下角所在的地形顶点格子
		gridBottomRight = m_pTerrain->GetTerrainGrid( fDecalBottomRightX, fDecalBottomRightY, bLeftPass, bTopPass, bRightPass, bBottomPass );
		
		int nRealBottomRightRow = gridBottomRight.nRow + 1;
		int nRealBottomRightCol = gridBottomRight.nCol + 1;
		if (nRealBottomRightRow > m_pTerrain->GetWidthSize() - 1)
		{
			nRealBottomRightRow = m_pTerrain->GetWidthSize() - 1;
		}
		if (nRealBottomRightCol > m_pTerrain->GetWidthSize() - 1)
		{
			nRealBottomRightCol = m_pTerrain->GetWidthSize() - 1;
		}
		nGridRowsNum = nRealBottomRightRow - gridTopLeft.nRow;
		nGridColsNum = nRealBottomRightCol - gridTopLeft.nCol;
		if (nGridRowsNum == 0)
		{
			nGridRowsNum = 1;
		}
		if (nGridColsNum == 0)
		{
			nGridColsNum = 1;
		}
	}

    u32 nMaxVertexNum = nGridRowsNum * nGridColsNum * 4;
    u32 nMaxGridsNum = nGridRowsNum * nGridColsNum;
    IRenderer* pRenderer = ENGINE_INST->GetRenderer();
    if (m_renderData.m_pVB)
    {
        if (m_renderData.m_pVB->GetVertexCount() != nMaxVertexNum)
        {
            pRenderer->ClearVertexBuffer(m_renderData.m_pVB);
            m_renderData.m_pVB = NULL;
        }
    }
    if (m_renderData.m_pIB)
    {
        if (nMaxGridsNum * 6 != m_renderData.m_pIB->GetTotalIndex())
        {
            pRenderer->ClearIndexBuffer(m_renderData.m_pIB);
            m_renderData.m_pIB = NULL;
        }        
    }

    if (m_renderData.m_pVB == NULL)
    {
        //m_renderData.m_pVB = pRenderer->CreateVertexBuffer(E_VB_STANDARD, nMaxVertexNum);
        m_renderData.m_pVB = pRenderer->CreateVertexBuffer(nMaxVertexNum, sizeof(VERTEX_STANDARD));
    }

	if (m_renderData.m_pIB == NULL)
	{
		m_renderData.m_pIB = pRenderer->CreateIndexBuffer(nMaxGridsNum * 6);

		u16* pIndices = (u16*)m_renderData.m_pIB->Lock(0, 0);

		for(size_t idx, idxOff, bboard = 0;
			bboard < nMaxGridsNum;
			++bboard )
		{
			// Do indexes
			idx    = bboard * 6;
			idxOff = bboard * 4;

			pIndices[idx]		= static_cast<unsigned short>(idxOff); // + 0;, for clarity
			pIndices[idx + 1]	= static_cast<unsigned short>(idxOff + 1);
			pIndices[idx + 2]	= static_cast<unsigned short>(idxOff + 2);
			pIndices[idx + 3]	= static_cast<unsigned short>(idxOff + 1);
			pIndices[idx + 4]	= static_cast<unsigned short>(idxOff + 3);
			pIndices[idx + 5]	= static_cast<unsigned short>(idxOff + 2);

		}
		m_renderData.m_pIB->Unlock();
	}
    

    Vector3Df minEdge(9999999, 9999999, 9999999);
    Vector3Df maxEdge(-9999999, -9999999, -9999999);

    //没有地形时的贴花创建
    if ( !m_pTerrain )
    {
        LPVERTEX_STANDARD pVertices = (LPVERTEX_STANDARD)m_renderData.m_pVB->Lock();

        //左上
        pVertices->color = m_colorDiffuse;
        pVertices->position = Vector3Df( fDecalTopLeftX, 0, fDecalTopLeftY );
        pVertices->tu = 0;
        pVertices->tv = 0;
        pVertices++;

        //右上
        pVertices->color = m_colorDiffuse;
        pVertices->position = Vector3Df( fDecalBottomRightX, 0, fDecalTopLeftY );
        pVertices->tu = m_fBottomRightU;
        pVertices->tv = 0;
        pVertices++;

        //左下
        pVertices->color = m_colorDiffuse;
        pVertices->position = Vector3Df( fDecalTopLeftX, 0, fDecalBottomRightY );
        pVertices->tu = 0;
        pVertices->tv = m_fBottomRightV;
        pVertices++;

        //右下
        pVertices->color = m_colorDiffuse;
        pVertices->position = Vector3Df( fDecalBottomRightX, 0, fDecalBottomRightY );
        pVertices->tu = m_fBottomRightU;
        pVertices->tv = m_fBottomRightV;
        m_renderData.m_pVB->Unlock();

        maxEdge.Set( fDecalBottomRightX, 0, fDecalTopLeftY );
        minEdge.Set( fDecalTopLeftX, 0, fDecalBottomRightY );
    }
    //有地形时的贴花创建
    else if (m_pTerrain && m_pTerrain->IsPrepared())
    {
		int nVertex = 0;
        LPVERTEX_STANDARD pVertices = (LPVERTEX_STANDARD)m_renderData.m_pVB->Lock();
        for (u32 row = gridTopLeft.nRow; row < gridTopLeft.nRow + nGridRowsNum; ++row)
        {
            for (u32 col = gridTopLeft.nCol; col < gridTopLeft.nCol + nGridColsNum; ++col)
            {
                //左上角
                LPVERTEX_NORMAL pTerrainVertex = m_pTerrain->GetVertex(row * m_pTerrain->GetWidthSize() + col);
                
                if (row == gridTopLeft.nRow)
                {
                    if (bTopPass)
                    {
                        pVertices->position.z = pTerrainVertex->vPos.z;
                    }
                    else
                    {
                        pVertices->position.z = m_worldPosition.z + m_fHeight / 2;
                    }
                }
                else
                {
                    pVertices->position.z = pTerrainVertex->vPos.z;
                }

                if (col == gridTopLeft.nCol)
                {
                    if (bLeftPass)
                    {
                        pVertices->position.x = pTerrainVertex->vPos.x;
                    }
                    else
                    {
                        pVertices->position.x = m_worldPosition.x - m_fWidth / 2;
                    }
                }
                else
                {
                    pVertices->position.x = pTerrainVertex->vPos.x;
                }

                if (bLeftPass && bTopPass)
                {
                    pVertices->position.y = pTerrainVertex->vPos.y + 5.0f;
                }
                else
                {
                    pVertices->position.y = m_pTerrain->GetHeight(pVertices->position.x, pVertices->position.z) + 5.0f;
                }
                pVertices->color = m_colorDiffuse;
                pVertices->tu = (pVertices->position.x - m_worldPosition.x + m_fWidth / 2) / m_fWidth;
                pVertices->tv = (pVertices->position.z - m_worldPosition.z + m_fHeight / 2) / m_fHeight;
                maxEdge.y = Max(maxEdge.y, pVertices->position.y);
                minEdge.y = Min(minEdge.y, pVertices->position.y);
                maxEdge.x = Max(maxEdge.x, pVertices->position.x);
                minEdge.x = Min(minEdge.x, pVertices->position.x);
                maxEdge.z = Max(maxEdge.z, pVertices->position.z);
                minEdge.z = Min(minEdge.z, pVertices->position.z);
                pVertices++;
				nVertex++;

                //右上角
                int nRealCol = col + 1;
                if (nRealCol > m_pTerrain->GetGridsNumPerCol() )
                {
                    nRealCol = m_pTerrain->GetGridsNumPerCol();
                }
                pTerrainVertex = m_pTerrain->GetVertex(row * m_pTerrain->GetWidthSize() + nRealCol);
                if (row == gridTopLeft.nRow)
                {
                    if (bTopPass)
                    {
                        pVertices->position.z = pTerrainVertex->vPos.z;
                    }
                    else
                    {
                        pVertices->position.z = m_worldPosition.z + m_fHeight / 2;
                    }
                }
                else
                {
                    pVertices->position.z = pTerrainVertex->vPos.z;
                }

                if (nRealCol == gridTopLeft.nCol + nGridColsNum)
                {
                    if (bRightPass)
                    {
                        pVertices->position.x = pTerrainVertex->vPos.x;
                    }
                    else
                    {
                        pVertices->position.x = m_worldPosition.x + m_fWidth / 2;
                    }
                }
                else
                {
                    pVertices->position.x = pTerrainVertex->vPos.x;
                }

				if (bRightPass && bTopPass)
				{
					pVertices->position.y = pTerrainVertex->vPos.y + 5.0f;
				}
				else
				{
					pVertices->position.y = m_pTerrain->GetHeight(pVertices->position.x, pVertices->position.z) + 5.0f;
				}

                pVertices->color = m_colorDiffuse;
                pVertices->tu = (pVertices->position.x - m_worldPosition.x + m_fWidth / 2) / m_fWidth;
                pVertices->tv = (pVertices->position.z - m_worldPosition.z + m_fHeight / 2) / m_fHeight;
                maxEdge.y = Max(maxEdge.y, pVertices->position.y);
                minEdge.y = Min(minEdge.y, pVertices->position.y);
                maxEdge.x = Max(maxEdge.x, pVertices->position.x);
                minEdge.x = Min(minEdge.x, pVertices->position.x);
                maxEdge.z = Max(maxEdge.z, pVertices->position.z);
                minEdge.z = Min(minEdge.z, pVertices->position.z);
                pVertices++;
				nVertex++;

                //左下角
                int nRealRow = row + 1;
                if (nRealRow > m_pTerrain->GetGridsNumPerRow())
                {
                    nRealRow = m_pTerrain->GetGridsNumPerRow();
                }
                pTerrainVertex = m_pTerrain->GetVertex(nRealRow * m_pTerrain->GetWidthSize() + col);
                if (nRealRow == gridTopLeft.nRow + nGridRowsNum)  //已经是最下面的格
                {
                    if (bBottomPass)
                    {
                        pVertices->position.z = pTerrainVertex->vPos.z;
                    }
                    else
                    {
#ifdef LEFT_HAND_COORDINATE
                        pVertices->position.z = m_worldPosition.z - m_fHeight / 2;
#endif // LEFT_HAND_COORDINATE
                        
                    }
                }
                else
                {
                    pVertices->position.z = pTerrainVertex->vPos.z;
                }

                if (col == gridTopLeft.nCol)
                {
                    if (bLeftPass)
                    {
                        pVertices->position.x = pTerrainVertex->vPos.x;
                    }
                    else
                    {
                        pVertices->position.x = m_worldPosition.x - m_fWidth / 2;
                    }
                }
                else
                {
                    pVertices->position.x = pTerrainVertex->vPos.x;
                }

				if (bLeftPass && bBottomPass)
				{
					pVertices->position.y = pTerrainVertex->vPos.y + 5.0f;
				}
				else
				{
					pVertices->position.y = m_pTerrain->GetHeight(pVertices->position.x, pVertices->position.z) + 5.0f;
				}

                pVertices->color = m_colorDiffuse;
                pVertices->tu = (pVertices->position.x - m_worldPosition.x + m_fWidth / 2) / m_fWidth;
                pVertices->tv = (pVertices->position.z - m_worldPosition.z + m_fHeight / 2) / m_fHeight;
                maxEdge.y = Max(maxEdge.y, pVertices->position.y);
                minEdge.y = Min(minEdge.y, pVertices->position.y);
                maxEdge.x = Max(maxEdge.x, pVertices->position.x);
                minEdge.x = Min(minEdge.x, pVertices->position.x);
                maxEdge.z = Max(maxEdge.z, pVertices->position.z);
                minEdge.z = Min(minEdge.z, pVertices->position.z);
                pVertices++;
				nVertex++;

                //右下角
                pTerrainVertex = m_pTerrain->GetVertex(nRealRow * m_pTerrain->GetWidthSize() + nRealCol);
                //pTerrainVertex = m_pTerrain->GetVertex(nRealRow * m_pTerrain->GetWidthSize() + col);
                if (nRealRow == gridTopLeft.nRow + nGridRowsNum)  //已经是最下面的格
                {
                    if (bBottomPass)
                    {
                        pVertices->position.z = pTerrainVertex->vPos.z;
                    }
                    else
                    {
#ifdef LEFT_HAND_COORDINATE
                        pVertices->position.z = m_worldPosition.z - m_fHeight / 2;
#endif // LEFT_HAND_COORDINATE

                    }
                }
                else
                {
                    pVertices->position.z = pTerrainVertex->vPos.z;
                }

                if (nRealCol == gridTopLeft.nCol + nGridColsNum)
                {
                    if (bRightPass)
                    {
                        pVertices->position.x = pTerrainVertex->vPos.x;
                    }
                    else
                    {
                        pVertices->position.x = m_worldPosition.x + m_fWidth / 2;
                    }
                }
                else
                {
                    pVertices->position.x = pTerrainVertex->vPos.x;
                }

				if (bRightPass && bBottomPass)
				{
					pVertices->position.y = pTerrainVertex->vPos.y + 5.0f;
				}
				else
				{
					pVertices->position.y = m_pTerrain->GetHeight(pVertices->position.x, pVertices->position.z) + 5.0f;
				}
                pVertices->color = m_colorDiffuse;
                pVertices->tu = (pVertices->position.x - m_worldPosition.x + m_fWidth / 2) / m_fWidth;
                pVertices->tv = (pVertices->position.z - m_worldPosition.z + m_fHeight / 2) / m_fHeight;
                maxEdge.y = Max(maxEdge.y, pVertices->position.y);
                minEdge.y = Min(minEdge.y, pVertices->position.y);
                maxEdge.x = Max(maxEdge.x, pVertices->position.x);
                minEdge.x = Min(minEdge.x, pVertices->position.x);
                maxEdge.z = Max(maxEdge.z, pVertices->position.z);
                minEdge.z = Min(minEdge.z, pVertices->position.z);
                pVertices++;
				nVertex++;
            }
        }

        m_renderData.m_pVB->Unlock();
    }

    m_boundingBox.MaxEdge = maxEdge;
    m_boundingBox.MinEdge = minEdge;
    m_WorldAABB = m_boundingBox;
    m_vtCenter = m_boundingBox.GetCenter();

    if (m_pNode)
    {
        //m_pNode->SetPosition(m_worldPosition);
        m_pNode->NeedUpdate();
        m_pNode->Update();
        //m_worldPosition = m_pNode->GetDerivedPosition();
    }

    m_bNeedCaculate = false;
}

//
//void TerrainDecal::Update( float fTimeElapse )
//{
//	if ( !m_pNode || !m_pTerrain )
//	{
//		return;
//	}
//	Vector3Df vNodeWorldPos = m_pNode->GetDerivedPosition();
//	vNodeWorldPos.y = m_pTerrain->GetHeight( vNodeWorldPos.x, vNodeWorldPos.z );
//	if ( m_pNode && m_worldPosition != vNodeWorldPos )
//	{
//		//m_worldPosition = vNodeWorldPos;
//		ShowDecal( vNodeWorldPos );
//	}
//}