#include "stdafx.h"//#include "stdafx.h"
#include "HeightMap.h"
//#include "SystemMgr.h"
#include "MyMath.h"
#include "Terrain.h"
#include <algorithm>
#include "FileSystem.h"
#include "StringUtil.h"
#include "MaterialManager.h"
//#include "ImageOperator.h"
#include "ColorValue.h"
#include "Color.h"
#include "ISceneManager.h"
#include "Shadowmap.h"
//#include "TechniqueSelector.h"
//#include "TextureUnit.h"
#include "TextureMgr.h"
//#include "Pass.h"
//#include "Technique.h"
#include "GPUProgramManager.h"
#include "FileData.h"

int CompareBlock(CTerrainBlock* pObj1, CTerrainBlock* pObj2)
{
    CCamera* pCamera = ENGINE_INST->GetSceneMgr()->GetCamera();

    f32 fDis1 = pCamera->GetDistanceFrom(pObj1->GetCenter());
    f32 fDis2 = pCamera->GetDistanceFrom(pObj2->GetCenter());
    return fDis1 < fDis2;
}

CHeightMap::CHeightMap() : m_dwSize(0), m_dwVertices(0), m_nWidth(0), m_nDepth(0)
//, m_pDetailLayer(NULL)
//, m_pDetailMaterial(NULL)
, m_fInitHeight(0)
, m_fWidth(0)
, m_pHeightmapTex(NULL)
, m_bHeightDataDirty(false)
, m_dwTrianglesCount(0)
, m_pResource(NULL)
{
    m_fYScale = 1.0;
    m_bDrawNormal = false;
    m_bLastKeyDown = false;

    m_szType = "terrain";
    m_bAcceptShadow = true;
    m_bAcceptLighting = true;
    m_renderGroupType = eRGT_Terrain;
}

CHeightMap::CHeightMap(int nSize, const LPTERRAINCONFIG pTerrainConfig, const char* szMaterial) : 
m_dwSize(nSize), m_dwVertices(0), m_nWidth(nSize), m_nDepth(nSize), 
m_fBlockWidth(0),
m_fBlockDepth(0),
m_fYScale(1.0f),
//m_pDetailLayer(NULL),
//m_pDetailMaterial(NULL),
m_fInitHeight(0)
, m_pHeightmapTex(NULL)
, m_bHeightDataDirty(false)
, m_dwTrianglesCount(0)
, m_pResource(NULL)
{
    //m_nWidth = m_nDepth = nSize;
    m_dwVertices = m_nWidth * m_nDepth;
    m_vtHeights.resize(m_dwVertices);

    for (u32 i = 0; i < m_dwVertices; i++)
    {
        //m_vtHeight[i] = bFileData[i] * 5;
        m_vtHeights[i] = 0;   //设置为0以调试之用
    }

    m_dwBlockSize = pTerrainConfig->dwBlockSize;
    m_fGridLength = pTerrainConfig->fGridLength;
    m_fWidth = m_fGridLength * (m_nWidth - 1);
    m_fDepth = m_fGridLength * (m_nDepth - 1);
    m_fYScale = 1.0f;

    ComputeVertices();
    ComputeNormals();

    CreateBlocks();

    //s32 nNodes = m_quadTree.Create(m_dwSize - 1, m_dwBlockSize - 1, m_fWidth, this);
    //AddBlocksToQuadTree(&m_quadTree);

    //IRenderer* pRenderer = ENGINE_INST->GetRenderer();
    //AIRString strFile;
    //AIRString strName;
    //CStringUtil::SplitFilename(szMaterial, strFile, strName);

    m_pMaterial = MATERIAL_MANAGER->CreateMaterial(szMaterial);//strFile.c_str(), strName.c_str());

    m_bDrawNormal = false;
    m_bAcceptLighting = true;
    m_bAcceptShadow = true;
}



CHeightMap::~CHeightMap(void)
{
    Clear();
}

CTerrainBlock* CHeightMap::GetBlock(int nIndex)
{
    AIRMap<int, CTerrainBlock*>::iterator it = m_mapBlock.find(nIndex);
    if (it != m_mapBlock.end())
    {
        return it->second;
    }
    
    return NULL;
}

void CHeightMap::Clear()
{
    for (AIRMap<int, CTerrainBlock*>::iterator it = m_mapBlock.begin(); it != m_mapBlock.end(); it++)
    {
        CTerrainBlock* pBlock = it->second;
        pBlock->Clear();
        SafeDelete(pBlock);
    }

    if (m_pHeightmapTex)
    {
        TEXTURE_MANAGER->ClearTexture(m_pHeightmapTex);
    }

    if (m_pMaterial)
    {
        MATERIAL_MANAGER->ClearMaterial(m_pMaterial);
        m_pMaterial = NULL;
    }

    //IRenderer* pRenderer = ENGINE_INST->GetRenderer();

    //for (u32 i = 0; i < m_vtTextures.Size(); i++)
    //{
    //    ITexture* pTexture = m_vtTextures[i];
    //    pRenderer->ClearTexture(pTexture);
    //}

    m_mapBlock.clear();
}

void CHeightMap::SetYScale(float fYscale)
{
    m_fYScale = fYscale;
}

bool CHeightMap::CreateHeightMap(const char* szFilename, const LPTERRAINCONFIG pTerrainConfig, float fInitialHeight,
                                 float fYScale, const AIRString& strMaterial)
{
	m_strName = szFilename;
    if (!TERRAIN->IsInialize())
    {
        TERRAIN->Init(pTerrainConfig);
    }
    m_fYScale = fYScale;
    m_fInitHeight = fInitialHeight;
    const char* szFileExt = strstr(szFilename, ".");
    
    m_dwBlockSize = pTerrainConfig->dwBlockSize;
    m_fGridDepth = m_fGridLength = pTerrainConfig->fGridLength;

    m_strHeightMap = szFilename;

    if (szFileExt != NULL)
    {
        if (strcmp(szFileExt, ".bmp") == 0 || strcmp(szFileExt, ".jpg") == 0 || strcmp(szFileExt, ".png") == 0 || strcmp(szFileExt, ".dds") == 0)
        {
            TERRAIN->Cache();
            if (!ReadBMPFile(szFilename))
            {
                return false;
            }
        }
        else if (strcmp(szFileExt, ".raw") == 0)
        {
            if (!ReadRawFile(szFilename))    //第一步，读入高度图，生成高度数据和所有顶点
            {
                return false;
            }
        }
    }
    else //if ((szFileExt = strcmp(szFilename, ".raw")) != NULL || 
        //(szFileExt = strstr(szFilename, ".heightmap")) != NULL)
    {
        return false;
    }
    
    m_strMaterial = strMaterial;
    if (!ENGINE_INST->GetConfig()->m_bNewTerrain)
    {
        m_pMaterial = MATERIAL_MANAGER->CreateMaterial(strMaterial.c_str());
    }
    

    if (m_pResource && m_pResource->IsLoaded())
    {
        CacheTerrain();
    }
    else
    {
        CacheTerrain();
        if (!ENGINE_INST->GetConfig()->m_bNewTerrain)
            m_pMaterial->Cache();
    }
    

    return true;
}

bool CHeightMap::CreateHeightMap(int nTerrainSize, float fInitialHeight, float fYScale, const LPTERRAINCONFIG pTerrainConfig)
{
    if (!TERRAIN->IsInialize())
    {
        TERRAIN->Init(pTerrainConfig);
    }
    m_vtHeights.resize(nTerrainSize * nTerrainSize);
    for (int i = 0; i < nTerrainSize; ++i)
    {
        for (int j = 0; j < nTerrainSize; ++j)
        {
            m_vtHeights[i * nTerrainSize + j] = fInitialHeight;
        }
    }

    m_fInitHeight = fInitialHeight;

    m_dwSize = nTerrainSize;
    m_nWidth = m_nDepth = nTerrainSize;
    m_dwBlockSize = pTerrainConfig->dwBlockSize;
    m_fGridDepth = m_fGridLength = pTerrainConfig->fGridLength;
    m_fBlockWidth = (m_dwBlockSize - 1) * m_fGridLength;
    m_fBlockDepth = m_fBlockWidth;
    m_fWidth = m_fGridLength * (m_nWidth - 1);
    m_fDepth = m_fGridLength * (m_nDepth - 1);

    m_dwVertices = nTerrainSize * nTerrainSize;

    ComputeVertices();
    ComputeNormals();

    if (!ENGINE_INST->GetConfig()->m_bNewTerrain)
    {
        m_pMaterial = MATERIAL_MANAGER->CreateMaterial("TemplateMaterial.xml/Terrain");
    }

    if (m_mapBlock.empty())
    {
        CreateBlocks();
    }
    

    s32 nNodes = m_quadTree.Create(m_nWidth - 1, m_dwBlockSize - 1, m_fWidth, this);
    AddBlocksToQuadTree(&m_quadTree);

    TERRAIN->AddHeightMap(this);
    m_fYScale = fYScale;

    m_bPrepared = true;

    return true;
}

bool CHeightMap::ReadBMPFile(const char* szFilename)
{
    //ImageOperator* pImage = IMAGEOPERATORMGR->CreateImageOperatorFromFile("Heightmap", szFilename);
	AIRImage imgHeightMap;
	if (!imgHeightMap.LoadFromFile(szFilename))
	{
		return false;
	}

    //if (!pImage)
    //{
    //    return false;
    //}

    int nWidth = imgHeightMap.GetWidth();
    int nHeight = imgHeightMap.GetHeight();
    m_dwSize = nWidth;
    m_nWidth = nWidth;
    m_nDepth = nHeight;

    m_dwVertices = nWidth * nHeight;
    m_vtHeights.resize(m_dwVertices);

    u8* pData = (u8*)imgHeightMap.GetPixelBits();

    u8 bytePerPixel = imgHeightMap.GetBytePerPixel();

    //OriginType originType = pImage->GetOriginType();
    //if (originType == eOT_ORIGIN_UPPER_LEFT)
    {
        for (int i = 0; i < nHeight; ++i)
        {
            for (int j = 0; j < nWidth; ++j)
            {
                u32 uColor = imgHeightMap.GetPixelAsInt(j, i); //pImage->GetPixelAsInt(j, i);
                if (uColor > 128)
                {
                    int a = 0;
                }
                CColorValue value(uColor);
                float fHeight = 0.0f;
				/*
                switch (pImage->GetPixelFormat())
                {
                case ePF_LUMINANCE:
                    fHeight = uColor;
                    break;
                case ePF_RGB:
                    value.SetAsRGB(uColor);
                    fHeight = value.r;
                    break;
                case ePF_RGBA:
                    value.SetAsRGBA(uColor);
                    fHeight = value.r;
                    break;
                case ePF_BGRA:
                    value.SetAsBGRA(uColor);
                    fHeight = value.r;
                    break;
                case ePF_BGR:
                    value.SetAsBGR(uColor);
                    fHeight = value.r; // + 0.587f * value.g + 0.114f * value.b;
                    break;
                }
				*/
                m_vtHeights[i * nHeight + j] = m_fInitHeight + fHeight * m_fYScale * 255.0f;
            }
        }
    }
	/*
    else
    {
        for (int i = nHeight - 1, nRow = 0; i >= 0; --i, ++nRow)
        {
            for (int j = 0; j < nWidth; ++j)
            {
                u32 uColor = imgHeightMap.GetPixelAsInt(i, j); //pImage->GetPixelAsInt(j, i);
                if (uColor > 128)
                {
                    int a = 0;
                }
                CColorValue value;
                float fHeight = 0.0f;
                switch (pImage->GetPixelFormat())
                {
                case ePF_LUMINANCE:
                    fHeight = uColor;
                    break;
                case ePF_RGB:
                    value.SetAsRGB(uColor);
                    fHeight = value.r;
                    break;
                case ePF_RGBA:
                    value.SetAsRGBA(uColor);
                    fHeight = value.r;
                    break;
                case ePF_BGRA:
                    value.SetAsBGRA(uColor);
                    fHeight = value.r;
                    break;
                case ePF_BGR:
                    value.SetAsBGR(uColor);
                    fHeight = value.r; // + 0.587f * value.g + 0.114f * value.b;
                    break;
                }
                m_vtHeights[nRow * nHeight + j] = m_fInitHeight + fHeight * m_fYScale * 255.0f;
                //m_WorldAABB.MaxEdge.y = Max(m_vtHeights[i * nHeight + j], m_WorldAABB.MaxEdge.y); 
                //m_WorldAABB.MinEdge.y = Min(m_vtHeights[i * nHeight + j], m_WorldAABB.MinEdge.y);
            }
        }
    }
	*/

    m_fBlockWidth = m_dwBlockSize * m_fGridLength;
    m_fBlockDepth = m_dwBlockSize * m_fGridDepth;
    m_fWidth = m_fGridLength * (m_nWidth - 1);
    m_fDepth = m_fGridLength * (m_nDepth - 1);
    

    //
    return true;
}

bool CHeightMap::ReadRawFile(const char* szFilename)
{
    //m_pResource = static_cast<FileData*>(RESOURCE_MANAGER->CreateResource(Resource::Res_NormalFile, szFilename)); //FILEDATA_MANAGER->CreateFileData(szFilename);
    m_pResource = static_cast<FileData*>(RESOURCE_MANAGER->CreateResource<FileData>(szFilename));
    /*
    CDataStream* pStream = FILESYSTEM->GetFileStream(szFilename);
    if (pStream == NULL)
    {
        return false;
    }
    */
    //inFile.close();       //Close the file stream
    return true;
}

void CHeightMap::ComputeVertices()
{
    int startX = -m_fGridLength * (m_nWidth - 1) / 2 + (int)m_vtCenter.x;
    int startZ = m_fGridLength * (m_nDepth - 1) / 2 + (int)m_vtCenter.z;

    // coordinates to end generating vertices at
    int endX = m_fGridLength * (m_nWidth - 1) / 2 + (int)m_vtCenter.x;
    int endZ = -m_fGridLength * (m_nWidth - 1) / 2 + (int)m_vtCenter.z;
    float uCoordIncrementSize = 1.0f / (float)(m_nWidth - 1);
    float vCoordIncrementSize = 1.0f / (float)(m_nDepth - 1);
    m_vtVertices.resize(m_dwVertices);

	float fMaxH = 0;
	float fMinH = 0;

    int i = 0;
    for(int z = startZ; z >= endZ; z -= m_fGridDepth)
    {
        int j = 0;
        for(int x = startX; x <= endX; x += m_fGridLength)
        {
            // compute the correct index into the vertex buffer and heightmap
            // based on where we are in the nested loop.
            int index = i * m_nWidth + j;

            VERTEX_NORMAL cusVert;
            cusVert.vPos.x = (float)x;
            cusVert.vPos.y = m_vtHeights[index];//(float)m_vtHeight[index];
            cusVert.vPos.z = (float)z;
            cusVert.color = 0xffffffff;
            cusVert.tu = (float)j * uCoordIncrementSize;
            cusVert.tv = (float)i * vCoordIncrementSize;
            m_vtVertices[index] = cusVert;

			fMinH = AIR::Min( fMinH, cusVert.vPos.y );
			fMaxH = AIR::Max( fMaxH, cusVert.vPos.y );

            j++; // next column
        }
        i++; // next row
    }

    m_WorldAABB.MinEdge.x = startX;
    m_WorldAABB.MinEdge.z = endZ;
    m_WorldAABB.MaxEdge.x = endX;
    m_WorldAABB.MaxEdge.z = startZ;
	m_WorldAABB.MinEdge.y = fMinH;
	m_WorldAABB.MaxEdge.y = fMaxH;

    m_boundingBox = m_WorldAABB;
}


void CHeightMap::GetRenderableObject(AIRList<CTerrainBlock*>& lstRender, CCamera* pCamera)
{
    m_quadTree.GetRenderableObject(lstRender, pCamera);
}


void CHeightMap::AddBlocksToQuadTree(CTerrainQuadTree* pTree)
{
    for (AIRMap<int, CTerrainBlock*>::iterator it = m_mapBlock.begin(); it != m_mapBlock.end(); it++)
    {
        CTerrainBlock* pBlock = it->second;
        pTree->AddRenderObject(pBlock);
    }
}

void CHeightMap::CreateBlocks(/*const LPTERRAINCONFIG pTerrainConfig*/)
{
    int nBlockPerRow = (m_nWidth - 1) / (m_dwBlockSize - 1);

    for (int nRow = 0; nRow < nBlockPerRow; nRow++)
    {
        for (int nCol = 0; nCol < nBlockPerRow; nCol++)
        {
            int nCenterX = nCol * (m_dwBlockSize - 1) + (m_dwBlockSize - 1) / 2;
            int nCenterY = nRow * (m_dwBlockSize - 1) + (m_dwBlockSize - 1) / 2;

            CTerrainBlock* pBlock = new CTerrainBlock(nRow, nCol, nCenterX, nCenterY, m_dwBlockSize,
                m_nWidth, m_fGridLength, this);

            pBlock->Create(this, &m_vtVertices[0]);

            m_mapBlock.insert(make_pair(pBlock->GetBlockIndex(), pBlock));
        }
    }
}

float CHeightMap::GetHeight(float fX, float fZ)
{
    // Translate on xz-plane by the transformation that takes
    // the terrain START point to the origin.
    fX = (m_fWidth / 2.0f) + fX;
    fZ = (m_fWidth / 2.0f) - fZ;

    // Scale down by the transformation that makes the 
    // cellspacing equal to one.  This is given by 
    // 1 / cellspacing since; cellspacing * 1 / cellspacing = 1.
    fX /= m_fGridLength;
    fZ /= m_fGridDepth;

    // From now on, we will interpret our positive z-axis as
    // going in the 'down' direction, rather than the 'up' direction.
    // This allows to extract the row and column simply by 'flooring'
    // x and z:

    float col = ::floorf(fX - m_vtCenter.x);
    float row = ::floorf(fZ - m_vtCenter.z);

    // get the heights of the quad we're in:
    // 
    //  A   B
    //  *---*
    //  | / |
    //  *---*  
    //  C   D

    float A = GetHeightMapEntry((u32)row,   (u32)col);
    float B = GetHeightMapEntry((u32)row,   (u32)col + 1);
    float C = GetHeightMapEntry((u32)row + 1, (u32)col);
    float D = GetHeightMapEntry((u32)row + 1, (u32)col + 1);

    //
    // Find the triangle we are in:
    //

    // Translate by the transformation that takes the upper-left
    // corner of the cell we are in to the origin.  Recall that our 
    // cellspacing was nomalized to 1.  Thus we have a unit square
    // at the origin of our +x -> 'right' and +z -> 'down' system.
    float dx = fX - col;
    float dz = fZ - row;

    // Note the below compuations of u and v are unneccessary, we really
    // only need the height, but we compute the entire vector to emphasis
    // the books discussion.
    float height = 0.0f;
    if(dz < 1.0f - dx)  // upper triangle ABC
    {
        float uy = B - A; // A->B
        float vy = C - A; // A->C

        // Linearly interpolate on each vector.  The height is the vertex
        // height the vectors u and v originate from {A}, plus the heights
        // found by interpolating on each vector u and v.
        height = A + AIR::Lerp<float>(0.0f, uy, dx) + AIR::Lerp<float>(0.0f, vy, dz);
    }
    else // lower triangle DCB
    {
        float uy = C - D; // D->C
        float vy = B - D; // D->B

        // Linearly interpolate on each vector.  The height is the vertex
        // height the vectors u and v originate from {D}, plus the heights
        // found by interpolating on each vector u and v.
        height = D + AIR::Lerp<float>(0.0f, uy, 1.0f - dx) + AIR::Lerp<float>(0.0f, vy, 1.0f - dz);
    }

    return height;
}

TerrainGrid CHeightMap::GetTerrainGrid(float x, float z) const
{
    x = (m_fWidth / 2.0f) + x;
    z = (m_fWidth / 2.0f) - z;

    // Scale down by the transformation that makes the 
    // cellspacing equal to one.  This is given by 
    // 1 / cellspacing since; cellspacing * 1 / cellspacing = 1.
    x /= m_fGridLength;
    z /= m_fGridDepth;

    // From now on, we will interpret our positive z-axis as
    // going in the 'down' direction, rather than the 'up' direction.
    // This allows to extract the row and column simply by 'flooring'
    // x and z:

    float colTemp = x - m_vtCenter.x;
    
    float col = ::floorf(colTemp);
    if (colTemp <= 0.0f)
    {
        col = 0.0f;
    }
    if (col > m_nWidth - 1)
    {
        col = m_nWidth - 1;
    }

    float rowTemp = z - m_vtCenter.z;
    float row = ::floorf(rowTemp);
    if (rowTemp <= 0.0f)
    {
        row = 0;
    }
    if (row > m_nDepth - 1)
    {
        row = m_nDepth - 1;
    }
    TerrainGrid grid;
    grid.nCol = col;
    grid.nRow = row;
    return grid;
}

TerrainGrid CHeightMap::GetTerrainGrid(float x, float z, bool& bLeftPass, bool& bTopPass, bool& bRightPass, bool& bBottomPass) const
{
    x = (m_fWidth / 2.0f) + x;
    z = (m_fWidth / 2.0f) - z;

    // Scale down by the transformation that makes the 
    // cellspacing equal to one.  This is given by 
    // 1 / cellspacing since; cellspacing * 1 / cellspacing = 1.
    x /= m_fGridLength;
    z /= m_fGridDepth;

    // From now on, we will interpret our positive z-axis as
    // going in the 'down' direction, rather than the 'up' direction.
    // This allows to extract the row and column simply by 'flooring'
    // x and z:

    float colTemp = x - m_vtCenter.x;

    float col = ::floorf(colTemp);
    if (colTemp <= 0.0f)
    {
        col = 0.0f;
        bLeftPass = true;
    }
    if (col > m_nWidth - 1)
    {
        col = m_nWidth - 1;
        bRightPass = true;
    }

    float rowTemp = z - m_vtCenter.z;
    float row = ::floorf(rowTemp);
    if (rowTemp <= 0.0f)
    {
        row = 0;
        bTopPass = true;
    }
    if (row > m_nDepth - 1)
    {
        row = m_nDepth - 1;

        bBottomPass = true;
    }
    TerrainGrid grid;
    grid.nCol = col;
    grid.nRow = row;
    return grid;
}

float CHeightMap::GetHeightMapEntry(u32 nRow, u32 nCol)
{
    if (nRow >= m_nWidth)
    {
        nRow = m_nWidth - 1;
    }
    else if ((s32)nRow < 0)
    {
        nRow = 0;
    }

    if (nCol >= m_nDepth)
    {
        nCol = m_nDepth - 1;
    }
    else if ((s32)nCol < 0)
    {
        nCol = 0;
    }
    return m_vtVertices[nRow * m_nDepth + nCol].vPos.y; //m_vtHeight[nRow * m_dwSize + nCol];
}


void CHeightMap::ComputeNormals()
{
    Vector3Df vTopLeft(0.0, 0.0, 0.0);
    Vector3Df vTopRight(0.0, 0.0, 0.0);
    Vector3Df vBottomRight(0.0, 0.0, 0.0);
    Vector3Df vBottomLeft(0.0, 0.0, 0.0);

    u32 i = 0;
    for (u32 z = 0; z < m_nDepth; z++)
    {
        u32 j = 0;
        for (u32 x = 0; x < m_nWidth; x++)
        {

            u32 nIndex = i * m_nDepth + j;
            Vector3Df vSelf = GetVertexPos(nIndex); //->position;
            //计算左上面的法向量

            if (x > 0 && z > 0)
            {
                Vector3Df vLeft = GetVertexPos(z * m_nDepth + x - 1) - vSelf;
                Vector3Df vTop  = GetVertexPos((z - 1) * m_nDepth + x) - vSelf;

                vTopLeft = vLeft.CrossProduct(vTop);
                vTopLeft.Normalize();
            }

            //计算右上面的法向量
            if (x < m_nWidth - 1 && z > 0)
            {
                Vector3Df vRight = GetVertexPos(z * m_nDepth + x + 1) - vSelf;
                Vector3Df vTop  = GetVertexPos((z - 1) * m_nDepth  + x) - vSelf;

                vTopRight = vTop.CrossProduct(vRight).Normalize();
            }

            //计算右下面的法向量
            if (x < m_nWidth - 1 && z < m_nDepth - 1)
            {
                Vector3Df vRight = GetVertexPos(z * m_nDepth + x + 1) - vSelf;
                Vector3Df vBottom  = GetVertexPos((z + 1) * m_nDepth + x) - vSelf;

                vBottomRight = vRight.CrossProduct(vBottom).Normalize();
            }

            //计算左下面的法向量
            if (x > 0 && z < m_nDepth - 1)
            {
                Vector3Df vLeft = GetVertexPos(z * m_nDepth + x - 1) - vSelf;
                Vector3Df vBottom  = GetVertexPos((z + 1) * m_nDepth + x) - vSelf;

                vBottomLeft = vBottom.CrossProduct(vLeft).Normalize();
            }


            //计算该点的法向量
            Vector3Df vNormal = vTopLeft + vTopRight + vBottomRight + vBottomLeft;
            vNormal.Normalize();
            m_vtVertices[nIndex].normal = vNormal;

            j++;
        }
        i++;
    }
}

void CHeightMap::Render(CCamera* pCamera)
{
    if (Prepare())
    {
        PreRender(pCamera);  //问题出在这里，内存不断增在

        IRenderer* pRenderer = ENGINE_INST->GetRenderer();

        pRenderer->SetWorldTransform(&Matrix4f::IDENTITY);

        for (RenderObject_Iter it = m_lstRenderObject.begin(); it != m_lstRenderObject.end(); it++)
        {
            CTerrainBlock *pObject = *it;
            pObject->Render(pCamera);
            m_dwTrianglesCount += pObject->GetRenderTriangles();
        }


        if (m_bDrawNormal)
        {
            DrawNormals();
        }
    }
    
    
    //DT_TO_DBGSTR(LT_COMMAND, GetString("render terrain use time %d\n", timeGetTime() - dwTime));
}

void CHeightMap::PreRender(CCamera* pCamera)
{

    //m_dwTrianglesCount = 0;
    //GetRenderObject(pCamera);
    for (RenderObject_Iter it = m_lstRenderObject.begin(); it != m_lstRenderObject.end(); it++)
    {
        CTerrainBlock *pObject = *it;
        pObject->ComputeLOD(pCamera);
    }  

}

void CHeightMap::GetRenderObject(CCamera* pCamera, CRenderQueue* pQueue)
{
    m_lstRenderObject.clear();
    m_quadTree.GetRenderableObject(m_lstRenderObject, pCamera);
    //排序
    //Sort();

    RenderObject_Iter itEnd = m_lstRenderObject.end();
    for (RenderObject_Iter it = m_lstRenderObject.begin(); it != itEnd; ++it)
    {
        CTerrainBlock *pObject = *it;
        if (pObject->Prepare())
        {
            pObject->ComputeLOD(pCamera);   //必须在这里计算好真正的lod
            pQueue->AddRenderable(pObject);
            //pObject->GetVisibleRenderables(pCamera, pQueue);  还是要在场景八叉树中裁剪
        }
        
    }
    
}

bool CHeightMap::LoadFromXML(CMarkupSTL& xml)
{
    //xml.IntoElem();
    if (!Renderable::LoadFromXML(xml))
    {
        return false;
    }
    AIRString strAttribute;

    strAttribute = xml.GetAttrib("ID").c_str();
    if (strAttribute.length() > 0)
    {
        m_nID = atoi(strAttribute.c_str());
    }

    strAttribute = xml.GetAttrib("Size").c_str();
    if (strAttribute.length() > 0)
    {
        m_dwSize = (u32)atoi(strAttribute.c_str());
    }

    strAttribute = xml.GetAttrib("YScale").c_str();
    if (strAttribute.length() > 0)
    {
        m_fYScale = atof(strAttribute.c_str());
    }

    strAttribute = xml.GetAttrib("InitHeight").c_str();
    if (strAttribute.length() > 0)
    {
        m_fInitHeight = atof(strAttribute.c_str());
    }

    AIRString strMaterial = xml.GetAttrib("Material").c_str();
    //IRenderer* pRenderer = ENGINE_INST->GetRenderer();
    //AIRString strFile;
    //AIRString strName;
    //CStringUtil::SplitFilename(strAttribute.c_str(), strFile, strName);

    strAttribute = xml.GetAttrib("HeightMap").c_str();
    if (strAttribute.length())
    {
        if (!CreateHeightMap(strAttribute.c_str(), TERRAIN->GetConfig(), m_fInitHeight, m_fYScale, strMaterial))
        {
            //xml.OutOfElem();
            return false;
        }
    }

    TERRAIN->AddHeightMap(this);

    return true;
}

void CHeightMap::DrawNormals()
{
    for (RenderObject_Iter it = m_lstRenderObject.begin(); it != m_lstRenderObject.end(); it++)
    {
        CTerrainBlock *pObject = *it;
        pObject->DrawNormals();
    }
}

void CHeightMap::Sort()
{
    //qsort(m_lstRenderObject.begin(), m_lstRenderObject.end(), sizeof(Renderable*), CompareBlock);
    m_lstRenderObject.sort(CompareBlock);
    //sort(m_lstRenderObject.begin(), m_lstRenderObject.end(), CompareBlock);
}

void CHeightMap::GetVisableObjects(CCamera* pCamera, AIRList<CSceneNode*>& lstVisables, u32 nCullFlag)
{
    m_quadTree.GetVisableObjects(pCamera, lstVisables, nCullFlag);
}

bool CHeightMap::GetRayPoint(const Vector3Df& vOrg, const Vector3Df& vEnd, Vector3Df& vPointOut)
{
    Line3Df shortLine = LinearSearch(vOrg, vEnd);
    return BinarySearchRayPoint(shortLine, vPointOut);
}

Line3Df CHeightMap::LinearSearch(const Vector3Df& vOrg, const Vector3Df& vEnd)
{
    Line3Df line(vOrg, vEnd);
    float fLength = line.GetLength() / 50.0f;   // 分成50段
    Vector3Df direction = vEnd - vOrg;
    direction.Normalize();
    Vector3Df nextPoint = vOrg + direction * fLength;    //射线点

    float fHeightAtNextPoint = GetHeight(nextPoint.x, nextPoint.z);
    while (fHeightAtNextPoint < nextPoint.y)   //地形上的点低于射线的点
    {
        
        if (direction.z > 0 && nextPoint.z > vEnd.z)
        {
            return Line3Df(Vector3Df::ZERO, Vector3Df::ZERO);   //没有摄取
        }
        else if (direction.z < 0 && nextPoint.z < vEnd.z)
        {
            return Line3Df(Vector3Df::ZERO, Vector3Df::ZERO);
        }
        line.start = nextPoint;
        nextPoint = line.start + direction * fLength;
        line.end = nextPoint;
        fHeightAtNextPoint = GetHeight(nextPoint.x, nextPoint.z);
    }
    return line;
}

bool CHeightMap::BinarySearchRayPoint(Line3Df& line, Vector3Df& vPointOut)
{
    float fLength = line.GetLength();
    if (fLength == 0.0f)
    {
        return false;
    }
    
    float accuracy = 0.01f; 

    float heightAtStartingPoint = GetHeight(line.start.x, line.start.z); 
    float currentError = line.start.y - heightAtStartingPoint; 

    int counter = 0; 
    Vector3Df nextPoint;
    Vector3Df direction = line.GetVector();
    direction.Normalize();
    while (currentError > accuracy)    //如果误差大于指定数
    {
        fLength /= 2.0f;
        nextPoint = line.start + direction * fLength;
        float fHeightAtNextPoint = GetHeight(nextPoint.x, nextPoint.z);
        if (nextPoint.y > fHeightAtNextPoint)   //当下一个射线上的点的高度高于对应地形的点，射线取后半段
        {
            line.start = nextPoint;
            currentError = line.start.y - fHeightAtNextPoint;   //误差=射线当前点-地形高度点
        }

        if (counter++ == 1000)
        {
            break;
        }
    }
    vPointOut = line.start;
    return true;
}

CTerrainBlock* CHeightMap::GetTerrainBlock(float x, float z)
{
    x = (m_fWidth / 2.0f) + x;
    z = (m_fWidth / 2.0f) - z;

    // Scale down by the transformation that makes the 
    // cellspacing equal to one.  This is given by 
    // 1 / cellspacing since; cellspacing * 1 / cellspacing = 1.
    x /= m_fBlockWidth;
    z /= m_fBlockDepth;

    // From now on, we will interpret our positive z-axis as
    // going in the 'down' direction, rather than the 'up' direction.
    // This allows to extract the row and column simply by 'flooring'
    // x and z:

    float colTemp = x - m_vtCenter.x;
    float col = ::floorf(colTemp);
    if (colTemp < 0)
    {
        col = 0;
    }
    else if (colTemp > GetBlocksNumPerRow() - 1)
    {
        col = GetBlocksNumPerRow() - 1;
    }

    float rowTemp = z - m_vtCenter.z;
    float row = ::floorf(rowTemp);
    if (rowTemp < 0)
    {
        row = 0;
    }
    else if (rowTemp > GetBlocksNumPerRow() - 1)
    {
        row = GetBlocksNumPerRow() - 1;
    }

    int nIndex = row * GetBlocksNumPerRow() + col;

    return GetBlock(nIndex);
}

void CHeightMap::GetBlocksInRange(AIRList<CTerrainBlock*>& lstBlocks, float x, float z, float fRadius)
{
    //获得左上角的block
    CTerrainBlock* pTopLeft = GetTerrainBlock(x - fRadius, z + fRadius);

    //获得左下角的block
    CTerrainBlock* pBottomRight = GetTerrainBlock(x + fRadius, z - fRadius);

    int nBlocksPerRow = (m_dwSize - 1) / (m_dwBlockSize - 1);
    int nBlocksPerCol = nBlocksPerRow;
    
    int nStartRow = pTopLeft->GetRow();
    int nStartCol = pTopLeft->GetCol();
    int nEndRow = pBottomRight->GetRow();
    int nEndCol = pBottomRight->GetCol();

    for (int nRow = nStartRow; nRow < nEndRow + 1; ++nRow)
    {
        for (int nCol = nStartCol; nCol < nEndCol + 1; ++nCol)
        {
            lstBlocks.push_back(GetBlock(nRow * nBlocksPerCol + nCol));
        }
    }
}

void CHeightMap::GenLightMap(const Vector3Df& vecSunLight, float fAmbient, int nLightMapSize, AIRImage& imgLightMap)
{
    //Vector3Df vOrig = 
    //ImageOperator* pLightMap = IMAGEOPERATORMGR->CreateDynamicImageOperator("TerrainLightMap");
	
    //pLightMap->SetBytePerPixel(1);
    //pLightMap->SetCodecType(eCT_UNSIGNED_BYTE);
    //pLightMap->SetImageDepth( 1 );
    //pLightMap->SetImageHeight( m_nDepth );
    //pLightMap->SetImageWidth( m_nWidth );
    //pLightMap->SetOriginType( eOT_ORIGIN_UPPER_LEFT );
    //pLightMap->SetPixelFormat( ePF_LUMINANCE );
    //pLightMap->SetNumMips(1);

	imgLightMap.Create(FIT_FLOAT, m_nWidth, m_nDepth);

    Vector3Df position;   //地平面上的坐标
    Vector3Df orign;      //太阳坐标

    Vector3Df sunDirection = -vecSunLight;
    sunDirection.Normalize();

    u32 dwSize = m_nWidth * m_nDepth;
    u8* pData = AIR_NEW_ARRAY_T(u8, dwSize);

    for (u32 row = 0; row < m_nWidth; ++row)
    {
        for (u32 col = 0; col < m_nDepth; ++col)
        {
            position.z = m_vtVertices[row * m_nDepth + col].vPos.z;
            position.x = col;

            float fColor = fAmbient;
            orign = m_vtVertices[row * m_nDepth + col].vPos + vecSunLight * 10000.0f;
            
            if (!IsIntersectMap(m_vtVertices[row * m_nDepth + col].vPos, orign, sunDirection))
            {
                fColor += 0.5f * (1.0f + m_vtVertices[row * m_nWidth + col].normal.DotProduct(sunDirection));
                if (fColor > 1.0f)
                {
                    fColor = 1.0f;
                }
            }
            else
            {
                int a = 0;
            }

            pData[row * m_nWidth + col] = fColor * 255;
        }
    }

    //pLightMap->SetData(pData, dwSize);
	imgLightMap.SetPixel(pData, 0, 0);
	AIR_DELETE_ARRAY_T(pData, u8, dwSize);
    //SafeDeleteArray(pData);
    //return pLightMap;
}


bool CHeightMap::IsIntersectMap(const Vector3Df vertex, const Vector3Df& vOrig, const Vector3Df& vDirection)
{
    int nHits = 0;

    float d, h, D; 

    Vector3Df texPos = vertex + vDirection;
    Vector3Df texDir;

    int w = m_nWidth;

    while (!(( texPos.x >= w - 1 ) || ( texPos.x <= 0 ) || ( texPos.z >= w - 1 ) || ( texPos.z <= 0 )))
    {
        // length of lightdir's projection
        D = (Vector3Df(texPos.x, 0, texPos.z) - Vector3Df(vOrig.x, 0, vOrig.z)).GetLength();
        d = (vertex - texPos).GetLength();

        h = vertex.y + (d * vOrig.y) / D;  // X(P) point

        // check if height in point P is bigger than point X's height
        if (m_vtVertices[texPos.z * m_nWidth + texPos.x].vPos.y > h)
        {
            nHits++;
            break;
        }

        texDir = vDirection;
        texDir.y = 0;
        texPos += texDir.Normalize();   // fetch new working point
    }

    return nHits > 0;
}

LPVERTEX_NORMAL CHeightMap::GetVertex(int nRow, int nCol)
{
    if (nRow < 0 || nRow >= m_nDepth || nCol < 0 || nCol >= m_nWidth)
    {
        return NULL;
    }

    return GetVertex(nRow * GetWidthSize() + nCol);
}

void CHeightMap::AcceptShadowmap(bool bAccept)
{
    m_bAcceptShadow = bAccept;
    if (ENGINE_INST->GetConfig()->m_bNewTerrain)
    {
        MapTerrainBlock::iterator itEnd = m_mapBlock.end();
        for (MapTerrainBlock::iterator it = m_mapBlock.begin(); it != itEnd; ++it)
        {
            CTerrainBlock* pBlock = it->second;
            if (pBlock->IsPrepared())
            {
                pBlock->AcceptShadowmap(bAccept);
            }
            
        }
    }
    else
    {
        //m_pMaterial->SetCurrentTechnique(TECHNIQUE_SELECTOR->SelectTechnique(this));
    }

    
}
/*
void CHeightMap::RenderToGBuffer(CCamera* pCamera)
{
    if (m_pTechniqueRenderToGBuffer == NULL)
    {
        GenRenderToGBufferTechnique();
    }
    for (RenderObject_Iter it = m_lstRenderObject.begin(); it != m_lstRenderObject.end(); it++)
    {
        CTerrainBlock *pObject = *it;
        pObject->RenderToGBuffer(pCamera);
        //m_dwTrianglesCount += pObject->GetRenderTriangles();
    }
}

void CHeightMap::GenRenderToGBufferTechnique()
{
    m_pTechniqueRenderToGBuffer = m_pMaterial->GetTechnique("TerrainRenderToGBuffer");
    if (m_pTechniqueRenderToGBuffer == NULL)
    {
        m_pTechniqueRenderToGBuffer = new CTechnique(m_pMaterial);
        *m_pTechniqueRenderToGBuffer = *(m_pMaterial->GetTechnique(0));
        m_pTechniqueRenderToGBuffer->SetName("TerrainRenderToGBuffer");
        //m_pTechniqueRenderToGBuffer->SetVSVersion("vs_2_0");
        //m_pTechniqueRenderToGBuffer->SetPSVersion("ps_2_0");

        CPass* pPass = m_pTechniqueRenderToGBuffer->GetPass(0);
        CTextureUnit* pUnit = pPass->GetTextureUnit(0);
        CTextureUnit* pUnit2 = pPass->GetTextureUnit(1);
        pUnit->SetTexture(pUnit2->GetTexture());
        pUnit2->GetTexture()->AddRef();
        //把第1阶段以后的纹理都清除
        for (size_t i = 1; i < 8; ++i)
        {
            pPass->RemoveTextureUnit(i);
        }
        //IShader* pShader = pPass->GetShader();
        //pShader->Clear();
        pPass->SetFogEnable(false);
        pPass->SetAlphaBlend(false);

        AIRString strPixelShader = "PSLPPRenderToGBuffer";
        AIRString strVertexShader = "VSTerrainRenderToGBuffer";

        IRenderer* pRenderer = ENGINE_INST->GetRenderer();
        IGPUProgram* pVertexShader = GPUPROGRAM_MANAGER->GetGPUProgram(strVertexShader); //pRenderer->CreateVertexShader(strVertexShader.c_str(), m_pTechniqueRenderToGBuffer->GetVSVersion().c_str());
        IGPUProgram* pPixelShader = GPUPROGRAM_MANAGER->GetGPUProgram(strPixelShader); //pRenderer->CreatePixelShader(strPixelShader.c_str(), m_pTechniqueRenderToGBuffer->GetPSVersion().c_str());
        //pPass->SetVertexShader(pVertexShader);
        //pPass->SetPixelShader(pPixelShader);
		IGPUProgram* pGpuPrograms[4] = { 0 };
		pGpuPrograms[0] = pVertexShader;
		pGpuPrograms[1] = pPixelShader;
		ShaderObject* pShaderObject = pRenderer->CreateShaderObject(pGpuPrograms, 2);
		pPass->SetShaderObject(pShaderObject);

        m_pMaterial->AddTechnique(m_pTechniqueRenderToGBuffer);
    }
}

void CHeightMap::RenderToDepthMap(CCamera* pCamera, CPass* pPass)
{
    if (!ENGINE_INST->GetConfig()->m_bNewTerrain)
    {
        if (m_pTechniqueRenderToDepthMap == NULL)
        {
            GenRenderToDepthMapTechnique();
        }
    }
    
	for (RenderObject_Iter it = m_lstRenderObject.begin(); it != m_lstRenderObject.end(); it++)
	{
		CTerrainBlock *pObject = *it;
		pObject->RenderToDepthMap(pCamera, pPass);
	}
}
*/

void CHeightMap::SetAcceptLighting(bool bAccept)
{
    if (ENGINE_INST->GetConfig()->m_bNewTerrain)
    {
        MapTerrainBlock::iterator itEnd = m_mapBlock.end();
        for (MapTerrainBlock::iterator it = m_mapBlock.begin(); it != itEnd; ++it)
        {
            CTerrainBlock* pBlock = it->second;
            pBlock->SetAcceptLighting(bAccept);
        }
    }
    else
    {
        m_bAcceptLighting = bAccept;
        //m_pMaterial->SetCurrentTechnique(TECHNIQUE_SELECTOR->SelectTechnique(this));
    }
    
}
/*
void CHeightMap::ResetCurrentTechnique()
{
    if (ENGINE_INST->GetConfig()->m_bNewTerrain)
    {
        MapTerrainBlock::iterator itEnd = m_mapBlock.end();
        for (MapTerrainBlock::iterator it = m_mapBlock.begin(); it != itEnd; ++it)
        {
            CTerrainBlock* pBlock = it->second;
            if (pBlock->IsPrepared())
            {
                pBlock->GetMaterial()->SetCurrentTechnique(TECHNIQUE_SELECTOR->SelectTechnique(pBlock));
            }
            
        }
    }
    else
    {
        m_pMaterial->SetCurrentTechnique(TECHNIQUE_SELECTOR->SelectTechnique(this));
    }  
}
*/

ITexture* CHeightMap::GetHeightmapTexture()
{
    if (m_pHeightmapTex == NULL)
    {
        //
        IRenderer* pRenderer = ENGINE_INST->GetRenderer();
        m_pHeightmapTex = TEXTURE_MANAGER->CreateTexture(m_strName.c_str(), TextureParam(GetWidthSize(), GetDepthSize(), 0, 1,
            eRSU_Default, RPF_R32F, TT_TEXTURE, false));
        //pRenderer->CreateFloatTexture(m_strName, GetWidthSize(), GetDepthSize(), true, 1, false);
        //int nPos = m_strHeightMap.find(".raw");
        //if (nPos > 0)
        //{
            //如果是raw文件，则创建浮点纹理
		m_pHeightmapTex->CopyPixelBoxToTexture((const u8*)&m_vtHeights[0], NULL, 0, GetWidthSize() * GetDepthSize() * sizeof(f32), GetWidthSize() * sizeof(f32), 0);
            //void* pData = m_pHeightmapTex->Lock();
            //memcpy(pData, &m_vtHeights[0], GetWidthSize() * GetDepthSize() * 4);
            //m_pHeightmapTex->Unlock();
        //}
        //else
        //{
            //如果是bmp图像
        //}
    }
	else if (m_bHeightDataDirty)
	{
		void* pData = m_pHeightmapTex->Lock();
		memcpy(pData, &m_vtHeights[0], GetWidthSize() * GetDepthSize() * 4);
		m_pHeightmapTex->Unlock();
		m_bHeightDataDirty = false;
	}

    return m_pHeightmapTex;
}

void CHeightMap::RebuildHeightmapTexture()
{
    if (m_pHeightmapTex == NULL)
    {
        //
        IRenderer* pRenderer = ENGINE_INST->GetRenderer();
        m_pHeightmapTex = TEXTURE_MANAGER->CreateTexture(m_strName.c_str(), TextureParam(GetWidthSize(), GetDepthSize(), 0, 1,
            eRSU_Default, RPF_R32F, TT_TEXTURE, false));
    }

    size_t uSize = m_vtHeights.size();
    for (size_t i = 0; i < uSize; ++i)
    {
        m_vtHeights[i] = m_vtVertices[i].vPos.y;
        
    }
    m_bHeightDataDirty = true;
}

void  CHeightMap::RetriveHeightMapIndexMapData( void )
{
    if (m_pMaterial && m_pMaterial->IsReady())
    {
        /*
        CTechnique* pTechnique = m_pMaterial->GetCurrentTechnique();
        if (pTechnique)
        {
            ITexture* pIndexTex = NULL;
            size_t nPassNum = pTechnique->GetPassesNum();
            for (size_t i = 0; i < nPassNum; ++i)
            {
                CPass* pPass = pTechnique->GetPass(i);
                if ( pPass && pPass->GetTextureUnit(0) )
                {
                    pIndexTex = pPass->GetTextureUnit(0)->GetTexture();
                    pIndexTex->Lock();
                    pIndexTex->Unlock();
                }
            }
        }
        */
    }
	

}

void CHeightMap::ComputeVerticesByHeights(const AIRVector<float>& vtHeights)
{
    if (m_vtVertices.empty())
    {
        return;
    }
    for (size_t i = 0; i < m_vtVertices.size(); ++i)
    {
        m_vtVertices[i].vPos.y = vtHeights[i];
        m_vtHeights[i] = vtHeights[i];
    }

    MapBlockIter itEnd = m_mapBlock.end();
    for (MapBlockIter it = m_mapBlock.begin(); it != itEnd; ++it)
    {
        CTerrainBlock* pBlock = it->second;
        pBlock->ComputeVertices(&m_vtVertices[0]);
    }
}

bool CHeightMap::Prepare()
{
    if (!m_bPrepared)
    {
        m_bPrepared = CacheTerrain();
    }
    return Renderable::Prepare();
}

bool CHeightMap::CacheTerrain()
{
    if (m_pResource)
    {
        if (!m_pResource->IsLoaded())
        {
            return false;
        }
        TERRAIN->Cache();
        CDataStream* pStream = m_pResource->GetFileStream();
        int nFileLength = pStream->Size();

        int nFloatDataLength = nFileLength / sizeof(float);
        float* bFileData = AIR_NEW_ARRAY_T(float, nFloatDataLength);

        //inFile.read(
        //    (char*)bFileData, // buffer
        //    nFileLength);// number of bytes to read into buffer
        pStream->Read(bFileData, nFileLength);

        //m_dwNumGroundVertices = nFileLength;          //计算出顶点总数

        //m_vtHeight.resize(m_dwNumGroundVertices);
        //int nSize = m_vtHeight.size();
        m_vtHeights.resize(nFloatDataLength);

        for (int i = 0; i < nFloatDataLength; i++)
        {
            //m_vtHeight[i] = bFileData[i] * 5;
            m_vtHeights[i] = bFileData[i];   //设置为0以调试之用
        }


        m_dwSize = (u32)sqrtf(nFloatDataLength);
        //m_dwSize = nWidth;
        m_nWidth = m_dwSize;
        m_nDepth = m_dwSize;
        m_dwVertices = nFloatDataLength;

        //SafeDeleteArray( bFileData );
		AIR_DELETE_ARRAY_T(bFileData, float, nFloatDataLength);

        //SafeDelete(m_pResource);
        //FILEDATA_MANAGER->ClearFileData(m_pResource);
        RESOURCE_MANAGER->ClearResource(m_pResource);
        m_pResource = NULL;

        //m_dwBlockSize = pTerrainConfig->dwBlockSize;
        //m_fGridDepth = m_fGridLength = pTerrainConfig->fGridLength;
        m_fBlockWidth = m_dwBlockSize * m_fGridLength;
        m_fBlockDepth = m_dwBlockSize * m_fGridDepth;
        m_fWidth = m_fGridLength * (m_nWidth - 1);
        m_fDepth = m_fGridLength * (m_nDepth - 1);
    }
    

    
    //strcpy(m_szHeightMap, pTerrainConfig->strHeightmap.c_str());
    if (m_vtVertices.empty())
    {
        ComputeVertices();
        ComputeNormals();
    }
    
    if (m_mapBlock.empty())
    {
        CreateBlocks();

        s32 nNodes = m_quadTree.Create(m_nWidth - 1, m_dwBlockSize - 1, m_fWidth, this);
        AddBlocksToQuadTree(&m_quadTree);

        TERRAIN->AddHeightMap(this);
    }
    
    return true;
}

void CHeightMap::GetDecalVertices(AIRVector<Vector3Df>& vtVertices, const Vector3Df& centerPoint, f32 fWidth, f32 fHeight, u32 nRowStart, u32 nColStart, u32 nRowEnd, u32 nColEnd)
{
    int nColStride = nColEnd - nColStart - 1;
    int nRowStride = nRowEnd - nRowStart - 1;
    for (u32 nRow = nRowStart; nRow <= nRowEnd; ++nRow)
    {
        for (u32 nCol = nColStart; nCol <= nColEnd; ++nCol)
        {
            Vector3Df vertex;
            if (nRow == 0)
            {
                vertex.z = centerPoint.z - fHeight / 2;
                if (nCol == 0)
                {
                    vertex.x = centerPoint.x - fWidth / 2;
                    
                    vertex.y = GetHeight(vertex.x, vertex.z);
                }
            }
        }
    }
}

void CHeightMap::Cull(CCamera* pCamera, AIRList<CSceneNode*>& lstVisableNodes, u32 nCullFlag)
{
    m_quadTree.GetVisableObjects(pCamera, lstVisableNodes, nCullFlag);
}
