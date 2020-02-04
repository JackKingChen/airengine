#include "stdafx.h"
//#include "stdafx.h"
#include "Terrain.h"
//#include "DrawText.h"

#include "Camera.h"

#include "Engine.h"

//#include "TerrainSceneManager.h"
#include "MyMath.h"
//#include "EffectManager.h"
#include "CfgReader.h"
#include "FileSystem.h"


//#include <d3d9.h>

CTerrain* CTerrain::m_pInstance = NULL;

int CLODInfo::m_nMaxLOD = 4;

CTerrain::CTerrain(void) : m_bInitialize(false)
{
	m_pCurHeightMap = NULL;
    //m_pIB = NULL;
    //m_pEffect = NULL;
    m_nPass = 0;
    m_nLODInfoCount = 0;

    
}

CTerrain::~CTerrain(void)
{
    Clear();
}

void CTerrain::Clear()
{
	for (HeightMap_Iter it = m_mapHeightMap.begin(); it != m_mapHeightMap.end(); it++)
	{
        CHeightMap* pHeightMap = it->second;
        pHeightMap->Clear();
        SafeDelete(pHeightMap);
	}

    m_pCurHeightMap = NULL;
    m_mapHeightMap.clear();

    for (LODInfo_Iter it = m_mapLODInfo.begin(); it != m_mapLODInfo.end(); it++)
    {
        CLODInfo* pInfo = it->second;
        pInfo->CleanUp();
        SafeDelete<CLODInfo*>(pInfo);
    }
    m_mapLODInfo.clear();

    m_bInitialize = false;
    //IRenderer* pRenderer = ENGINE_INST->GetRenderer();

    //pRenderer->ClearEffect(m_pEffect);

    //EFFECTMANAGER->ClearEffect(m_pEffect);

    //m_pEffect = NULL;
}

void CTerrain::AddHeightMap(CHeightMap* pHeightMap)
{
    m_mapHeightMap.insert(make_pair(pHeightMap->GetID(), pHeightMap));
}

void CTerrain::RemoveHeightMap(int nID)
{
    HeightMap_Iter it = m_mapHeightMap.find(nID);

    if (it != m_mapHeightMap.end())
    {
        CHeightMap* pHeightMap = it->second;
        pHeightMap->Clear();
        SafeDelete<CHeightMap*>(pHeightMap);
        m_mapHeightMap.erase(it);
    }
}

bool CTerrain::LoadConfigFile(const char* szCFGFile)
{
    ConfigReader reader;
    AIRString strFilename = FILESYSTEM->GetFileFullPath(szCFGFile);
    if (!reader.ReadFile(strFilename.c_str()))
    {
        return false;
    }
    m_config.dwBlockSize = reader.GetInt("blocksize");
    //m_config.dwTerrainSize = reader.GetInt("terrainsize");//129;
    m_config.fGridLength = reader.GetDouble("gridlength");//30.0;
    m_config.wLODLevel = reader.GetInt("LODLevel");//3;
    //m_config.fYScale = reader.GetDouble("yscale");
    //m_config.wTextureCount = reader.GetInt("texturecount");//3;
    //m_config.m_vtTextures.reserve(config.wTextureCount);
    //m_config.m_vtTextures.push_back(string(reader.GetString("texture1")));
    //m_config.m_vtTextures.push_back(string(reader.GetString("texture2")));
    //m_config.m_vtTextures.push_back(string(reader.GetString("texture3")));
    //strcpy(m_config.szVertexShader, "./Shaders/VSTerrain.txt");
    //strcpy(m_config.szPixelShader, "./Shaders/PSTerrain.txt");
    CLODInfo::m_nMaxLOD = m_config.wLODLevel;
    //strcpy(m_config.szEffect, reader.GetString("shader"));

    //int nLODInfoCount = ComputeIndices(m_config.wLODLevel, m_config.dwBlockSize);
    //
    //m_bInitialize = true;
    return true;
}

void CTerrain::Cache()
{
    if (!m_bInitialize)
    {
        ComputeIndices(m_config.wLODLevel, m_config.dwBlockSize);
        m_bInitialize = true;
    }
    
}

CHeightMap* CTerrain::GetHeightMap(int nID)
{
    HeightMap_Iter it = m_mapHeightMap.find(nID);

    if (it != m_mapHeightMap.end())
    {
        return it->second;
    }

    return NULL;
}


float CTerrain::GetHeight(float fX, float fZ) const
{
    return m_pCurHeightMap->GetHeight(fX, fZ);
	
}

int CTerrain::ComputeIndices(int nLODCount, u16 nBlockSize)
{
    u16 nGridPerRow = nBlockSize - 1;
    u32 dwNumBlockIndices = (nBlockSize - 1) * (nBlockSize - 1) * 6;
    m_dwNumBlockVertices = nBlockSize * nBlockSize;

    IRenderer* pRenderer = ENGINE_INST->GetRenderer();

    //首先创建没有裂缝的索引层
    /*
    for (int nSelfLOD = 1; nSelfLOD <= nLODCount; nSelfLOD++)
    {
        u16 wIndexStride = (u16)pow(2.0, nSelfLOD - 1);
        u16 wRealCell = nGridPerRow / wIndexStride;

        CIndexBuffer* pIndexBuffer = AURO_NEW_VERBOSE CIndexBuffer;
        pIndexBuffer->CreateDefault(dwNumBlockIndices);
        u16* pIndices = NULL;
        u16 dwIndex = 0;

        u16 dwTopLeft = 0;//m_data.dwTopLeft;     //声明，topleft是要在block里的topleft，而不是在整个地形的topleft
        u16 dwTopRight = nBlockSize - 1;
        u16 dwBottomRight = m_dwNumBlockVertices - 1;
        u16 dwBottomLeft = dwBottomRight - (nBlockSize - 1);

        pIndexBuffer->Lock(0, 0, (void**)pIndices, 0);

        for (u16 nRow = 0; nRow < wRealCell; nRow++)    //行
        {
            for (u16 nCol = 0; nCol < wRealCell; nCol++)  
            {
                *(pIndices + dwIndex++) = dwTopLeft + nCol * wIndexStride + nRow * wIndexStride * nBlockSize;
                *(pIndices + dwIndex++) = dwTopLeft + (nCol + 1) * wIndexStride + nRow * wIndexStride * nBlockSize;
                *(pIndices + dwIndex++) = dwTopLeft + (nCol + 1) * wIndexStride + (nRow + 1) * wIndexStride * nBlockSize;

                *(pIndices + dwIndex++) = dwTopLeft + nCol * wIndexStride + nRow * wIndexStride * nBlockSize;
                *(pIndices + dwIndex++) = dwTopLeft + (nCol + 1) * wIndexStride + (nRow + 1) * wIndexStride * nBlockSize;
                *(pIndices + dwIndex++) = dwTopLeft + nCol * wIndexStride + (nRow + 1) * wIndexStride * nBlockSize;
            }
        }

        pIndexBuffer->Unlock();

        CLODInfo* pLODInfo = AURO_NEW_VERBOSE CLODInfo(nSelfLOD, -1, -1, -1, -1);
        pLODInfo->SetIndexBuffer(pIndexBuffer);

        m_mapLODInfo.insert(make_pair(m_nLODInfoCount++, pLODInfo));
        //m_mapIndexBuffer.insert(make_pair(CLODInfo(nSelfLOD, -1, -1, -1, -1), pIndexBuffer));
    }
    */

    for (int nSelfLOD = 1; nSelfLOD <= nLODCount; nSelfLOD++)
    {
        //左边LOD
        for (int nLeftLOD = 1; nLeftLOD <= nLODCount; nLeftLOD++)
        {
            for (int nTopLOD = 1; nTopLOD <= nLODCount; nTopLOD++)
            {
                for (int nRightLOD = 1; nRightLOD <= nLODCount; nRightLOD++)
                {
                    for (int nBottomLOD = 1; nBottomLOD <= nLODCount; nBottomLOD++)
                    {
                        int nDeltaTop = nTopLOD - nSelfLOD;
                        int nDeltaRight = nRightLOD - nSelfLOD;
                        int nDeltaBottom = nBottomLOD - nSelfLOD;
                        int nDeltaLeft = nLeftLOD - nSelfLOD;

                        nDeltaTop = nDeltaTop > 0 ? nDeltaTop : 0;
                        nDeltaRight = nDeltaRight > 0 ? nDeltaRight : 0;
                        nDeltaBottom = nDeltaBottom > 0 ? nDeltaBottom : 0;
                        nDeltaLeft = nDeltaLeft > 0 ? nDeltaLeft : 0;

                        CLODInfo info(nSelfLOD, nDeltaLeft, nDeltaTop, nDeltaRight, nDeltaBottom);

                        if (nSelfLOD == 1 && nDeltaTop == 3 && nDeltaLeft == 3)
                        {
                            int nDebug = 0;
                        }

                        if (FindLODInfo(info) != NULL)
                        {
                            continue;
                        }

                        //if (nDeltaTop < 0 || nDeltaRight < 0 || nDeltaBottom < 0 || nDeltaLeft < 0)
                        //{
                        //    continue;
                        //}

                        u16 wIndexStride = (u16)pow(2.0, nSelfLOD - 1);
                        u16 wRealCell = nGridPerRow / wIndexStride;
                        
                        IIndexBuffer* pIndexBuffer = pRenderer->CreateIndexBuffer(dwNumBlockIndices/* * sizeof(u16)*/);//new CIndexBuffer;
                        //pIndexBuffer->CreateDefault(dwNumBlockIndices * sizeof(u16));


                        u16* pIndices = NULL;
                        u32 dwIndex = 0;

                        u16 dwTopLeft = 0;//m_data.dwTopLeft;     //声明，topleft是要在block里的topleft，而不是在整个地形的topleft
                        u16 dwTopRight = nBlockSize - 1;
                        u16 dwBottomRight = m_dwNumBlockVertices - 1;
                        u16 dwBottomLeft = dwBottomRight - (nBlockSize - 1);

                        pIndices = (u16*)pIndexBuffer->Lock(0, 0);
                        //((LPDIRECT3DINDEXBUFFER9)(pIndexBuffer->GetInterface()))->Lock(0, 0, (void**)&pIndices, 0);

                        
                        u16 wIndex1 = 0;
                        u16 wIndex2 = 0;
                        u16 wIndex3 = 0;
                        
                        //处理上边
                        if (nDeltaTop == 0)
                        {
                            for (u16 wCol = 0; wCol < nGridPerRow; wCol += wIndexStride)
                            {
                                if (wCol == 0)
                                {
                                    wIndex1 = dwTopLeft + wIndexStride + wIndexStride * nBlockSize;
                                    wIndex2 = dwTopLeft;
                                    wIndex3 = dwTopLeft + wIndexStride;
                                    *(pIndices + dwIndex++) = wIndex1;  //dwTopLeft + wIndexStride + wIndexStride * nBlockSize;
                                    *(pIndices + dwIndex++) = wIndex2;  //dwTopLeft + wIndexStride;  //dwTopLeft;
                                    *(pIndices + dwIndex++) = wIndex3;  //dwTopLeft;               //dwTopLeft + wIndexStride;

                                }
                                else if (wCol == nGridPerRow - wIndexStride)
                                {
                                    wIndex1 = dwTopLeft + wCol;
                                    wIndex2 = dwTopLeft + wCol + wIndexStride;
                                    wIndex3 = dwTopLeft + wCol + wIndexStride * nBlockSize;
                                    *(pIndices + dwIndex++) = wIndex1;   //dwTopLeft + wCol;
                                    *(pIndices + dwIndex++) = wIndex2;   //dwTopLeft + wCol + wIndexStride;
                                    *(pIndices + dwIndex++) = wIndex3;   //dwTopLeft + wCol + wIndexStride * nBlockSize;
                                }
                                else
                                {
                                    wIndex1 = dwTopLeft + wCol;
                                    wIndex2 = dwTopLeft + wCol + wIndexStride;
                                    wIndex3 = dwTopLeft + wCol + wIndexStride + wIndexStride * nBlockSize;
                                    *(pIndices + dwIndex++) = wIndex1; //dwTopLeft + wCol;
                                    *(pIndices + dwIndex++) = wIndex2; //dwTopLeft + wCol + wIndexStride;
                                    *(pIndices + dwIndex++) = wIndex3; //dwTopLeft + wCol + wIndexStride + wIndexStride * nBlockSize;

                                    wIndex1 = dwTopLeft + wCol;
                                    wIndex2 = dwTopLeft + wCol + wIndexStride + wIndexStride * nBlockSize;
                                    wIndex3 = dwTopLeft + wCol + wIndexStride * nBlockSize;
                                    *(pIndices + dwIndex++) = wIndex1;  //dwTopLeft + wCol;
                                    *(pIndices + dwIndex++) = wIndex2;  //dwTopLeft + wCol + wIndexStride + wIndexStride * nBlockSize;
                                    *(pIndices + dwIndex++) = wIndex3;  //dwTopLeft + wCol + wIndexStride * nBlockSize;
                                }
                            }
                        }
                        else
                        {
                            u16 wEdgeStride = (u16)pow(2.0, nDeltaTop) * wIndexStride;   //实际上wEdgeStride为相邻block的indexStride
                            
                            for (u16 wCol = 0; wCol < nGridPerRow; wCol += wEdgeStride)
                            {
                                if (nDeltaTop == 2 && nSelfLOD == 2)
                                {
                                    int nDebug = 0;
                                }
                                u16 dwBottom = dwTopLeft + wCol + wEdgeStride / 2 + wIndexStride * nBlockSize;//m_wIndexStride + m_wIndexStride * m_wBlockSize + nCol * m_wIndexStride * 2;

                                //处理上边的三角形
                                wIndex1 = dwBottom;
                                wIndex2 = dwBottom - wEdgeStride * 0.5 - wIndexStride * nBlockSize;
                                wIndex3 = dwBottom + wEdgeStride * 0.5 - wIndexStride * nBlockSize;
                                *(pIndices + dwIndex++) = wIndex1;//dwBottom;                          
                                *(pIndices + dwIndex++) = wIndex2;//dwBottom - wEdgeStride * 0.5 - wIndexStride * nBlockSize;
                                *(pIndices + dwIndex++) = wIndex3;//dwBottom + wEdgeStride * 0.5 - wIndexStride * nBlockSize;

                                //处理左下边的三角形
                                if (wCol > 0)
                                {
                                    wIndex1 = wCol + wIndexStride + wIndexStride * nBlockSize;
                                    wIndex2 = wCol + wIndexStride * nBlockSize;
                                    wIndex3 = wCol;
                                    *(pIndices + dwIndex++) = wIndex1;//wCol + wIndexStride + wIndexStride * nBlockSize;                          
                                    *(pIndices + dwIndex++) = wIndex2;//wCol + wIndexStride * nBlockSize;
                                    *(pIndices + dwIndex++) = wIndex3;//wCol;
                                }

                                //处理右下边的三角形
                                if (wCol < nGridPerRow - wEdgeStride)
                                {
                                    wIndex1 = wCol + wEdgeStride + wIndexStride * nBlockSize;
                                    wIndex2 = wCol + wEdgeStride + wIndexStride * nBlockSize - wIndexStride;
                                    wIndex3 = wCol + wEdgeStride;
                                    *(pIndices + dwIndex++) = wIndex1;//wCol + wEdgeStride + wIndexStride * nBlockSize;                          
                                    *(pIndices + dwIndex++) = wIndex2;//wCol + wEdgeStride + wIndexStride * nBlockSize - wIndexStride;
                                    *(pIndices + dwIndex++) = wIndex3;//wCol + wEdgeStride;
                                }

                                //处理内边的三角形
                                int nTriangleCount = (int)pow(2.0, nDeltaTop) - 2;
                                for (int i = 0 ; i < nTriangleCount * 0.5; i++)
                                {
                                    wIndex1 = wCol + (i + 1) * wIndexStride + wIndexStride * nBlockSize;
                                    wIndex2 = dwTopLeft + wCol;
                                    wIndex3 = wCol + (i + 2) * wIndexStride + wIndexStride * nBlockSize;
                                    *(pIndices + dwIndex++) = wIndex1;//wCol + wEdgeStride + wIndexStride * nBlockSize;                          
                                    *(pIndices + dwIndex++) = wIndex2;//wCol + wEdgeStride + wIndexStride * nBlockSize - wIndexStride;
                                    *(pIndices + dwIndex++) = wIndex3;//wCol + wEdgeStride;
                                }

                                for (int i = nTriangleCount * 0.5; i < nTriangleCount; i++)
                                {
                                    wIndex1 = wCol + (i + 1) * wIndexStride + wIndexStride * nBlockSize;
                                    wIndex2 = dwTopLeft + wCol + wEdgeStride;
                                    wIndex3 = wCol + (i + 2) * wIndexStride + wIndexStride * nBlockSize;
                                    *(pIndices + dwIndex++) = wIndex1;//wCol + wEdgeStride + wIndexStride * nBlockSize;                          
                                    *(pIndices + dwIndex++) = wIndex2;//wCol + wEdgeStride + wIndexStride * nBlockSize - wIndexStride;
                                    *(pIndices + dwIndex++) = wIndex3;//wCol + wEdgeStride;
                                }

                            }

                        }
                        

                        //处理右边
                        
                        if (nDeltaRight == 0)
                        {
                            for (u16 nRow = 0; nRow < nGridPerRow; nRow += wIndexStride)
                            {
                                if (nRow == 0)         //第一格
                                {
                                    wIndex1 = dwTopRight;
                                    wIndex2 = dwTopRight + wIndexStride * nBlockSize;
                                    wIndex3 = dwTopRight + wIndexStride * nBlockSize - wIndexStride;
                                    *(pIndices + dwIndex++) = wIndex1;//dwTopRight;
                                    *(pIndices + dwIndex++) = wIndex2;//dwTopRight + wIndexStride * nBlockSize;
                                    *(pIndices + dwIndex++) = wIndex3;//dwTopRight + wIndexStride * nBlockSize - wIndexStride;
                                }
                                else if (nRow == nGridPerRow - wIndexStride)
                                {
                                    wIndex1 = dwTopRight + nRow * nBlockSize;
                                    wIndex2 = dwTopRight + (nRow + wIndexStride) * nBlockSize;
                                    wIndex3 = dwTopRight - wIndexStride + nRow * nBlockSize;
                                    *(pIndices + dwIndex++) = wIndex1;//dwTopRight + nRow * nBlockSize;
                                    *(pIndices + dwIndex++) = wIndex2;//dwTopRight + (nRow + wIndexStride) * nBlockSize;
                                    *(pIndices + dwIndex++) = wIndex3;//dwTopRight - wIndexStride + nRow * nBlockSize;
                                }
                                else
                                {
                                    wIndex1 = dwTopRight - wIndexStride + nRow * nBlockSize;
                                    wIndex2 = dwTopRight + nRow * nBlockSize;
                                    wIndex3 = dwTopRight + (nRow + wIndexStride) * nBlockSize;
                                    *(pIndices + dwIndex++) = wIndex1;//dwTopRight - wIndexStride + nRow * nBlockSize;
                                    *(pIndices + dwIndex++) = wIndex2;//dwTopRight + nRow * nBlockSize;
                                    *(pIndices + dwIndex++) = wIndex3;//dwTopRight + (nRow + wIndexStride) * nBlockSize;

                                    wIndex1 = dwTopRight - wIndexStride + nRow * nBlockSize;
                                    wIndex2 = dwTopRight + (nRow + wIndexStride) * nBlockSize;
                                    wIndex3 = dwTopRight - wIndexStride + (nRow + wIndexStride) * nBlockSize;
                                    *(pIndices + dwIndex++) = wIndex1;//dwTopRight - wIndexStride + nRow * nBlockSize;
                                    *(pIndices + dwIndex++) = wIndex2;//dwTopRight + (nRow + wIndexStride) * nBlockSize;
                                    *(pIndices + dwIndex++) = wIndex3;//dwTopRight - wIndexStride + (nRow + wIndexStride) * nBlockSize;
                                }
                            }
                        }
                        else
                        {
                            u16 wEdgeStride = pow(2.0, nDeltaRight) * wIndexStride;
                            for (u16 wRow = 0; wRow < nGridPerRow; wRow += wEdgeStride)
                            {
                                if (nDeltaRight == 2)
                                {
                                    int nDebug = 0;
                                }
                                u16 wCenter = (dwTopRight - wIndexStride) + (wRow + wEdgeStride * 0.5) * nBlockSize;

                                //处理右边的三角形
                                wIndex1 = wCenter;
                                wIndex2 = dwTopRight + wRow * nBlockSize;//wCenter + wIndexStride - (wRow + wEdgeStride * 0.5) * nBlockSize;
                                wIndex3 = dwTopRight + (wRow + wEdgeStride) * nBlockSize;//wCenter + wIndexStride + (wRow + wEdgeStride * 0.5) * nBlockSize;
                                *(pIndices + dwIndex++) = wIndex1;//wCenter;                          
                                *(pIndices + dwIndex++) = wIndex2;//wCenter + wIndexStride - (wRow + wEdgeStride * 0.5) * nBlockSize;
                                *(pIndices + dwIndex++) = wIndex3;//wCenter + wIndexStride + (wRow + wEdgeStride * 0.5) * nBlockSize;

                                //处理上边的三角形
                                if (wRow > 0)
                                {
                                    wIndex1 = dwTopRight - wIndexStride + (wRow + wIndexStride) * nBlockSize;
                                    wIndex2 = dwTopRight - wIndexStride + wRow * nBlockSize;
                                    wIndex3 = dwTopRight + wRow * nBlockSize;
                                    *(pIndices + dwIndex++) = wIndex1;//dwTopRight - wIndexStride + (wRow + wIndexStride) * nBlockSize;                          
                                    *(pIndices + dwIndex++) = wIndex2;//dwTopRight - wIndexStride + wRow * nBlockSize;
                                    *(pIndices + dwIndex++) = wIndex3;//dwTopRight + wRow * nBlockSize;
                                }

                                //处理下边的三角形
                                if (wRow < nGridPerRow - wEdgeStride)
                                {
                                    wIndex1 = dwTopRight - wIndexStride + (wRow + wEdgeStride - wIndexStride) * nBlockSize;
                                    wIndex2 = dwTopRight + (wRow + wEdgeStride) * nBlockSize;
                                    wIndex3 = dwTopRight - wIndexStride + (wRow + wEdgeStride) * nBlockSize;
                                    *(pIndices + dwIndex++) = wIndex1;//dwTopRight - wIndexStride + (wRow + wIndexStride) * nBlockSize;                          
                                    *(pIndices + dwIndex++) = wIndex2;//dwTopRight - wIndexStride + wRow * nBlockSize;
                                    *(pIndices + dwIndex++) = wIndex3;//dwTopRight + wRow * nBlockSize;
                                }

                                //处理内边的三角形
                                int nTriangleCount = pow(2.0, nDeltaRight) - 2;
                                for (int i = 0; i < nTriangleCount * 0.5; i++)
                                {
                                    wIndex1 = dwTopRight - wIndexStride + (wRow + (i + 1) * wIndexStride) * nBlockSize;
                                    wIndex2 = dwTopRight + wRow * nBlockSize;
                                    wIndex3 = dwTopRight - wIndexStride + (wRow + (i + 2) * wIndexStride) * nBlockSize;
                                    *(pIndices + dwIndex++) = wIndex1;//wCol + wEdgeStride + wIndexStride * nBlockSize;                          
                                    *(pIndices + dwIndex++) = wIndex2;//wCol + wEdgeStride + wIndexStride * nBlockSize - wIndexStride;
                                    *(pIndices + dwIndex++) = wIndex3;//wCol + wEdgeStride;
                                }

                                for (int i = nTriangleCount * 0.5; i < nTriangleCount; i++)
                                {
                                    wIndex1 = dwTopRight - wIndexStride + (wRow + (i + 1) * wIndexStride) * nBlockSize;
                                    wIndex2 = dwTopRight + (wRow + wEdgeStride) * nBlockSize;
                                    wIndex3 = dwTopRight - wIndexStride + (wRow + (i + 2) * wIndexStride) * nBlockSize;
                                    *(pIndices + dwIndex++) = wIndex1;//wCol + wEdgeStride + wIndexStride * nBlockSize;                          
                                    *(pIndices + dwIndex++) = wIndex2;//wCol + wEdgeStride + wIndexStride * nBlockSize - wIndexStride;
                                    *(pIndices + dwIndex++) = wIndex3;//wCol + wEdgeStride;
                                }

                            }

                        }

                        

                        //处理下边
                        if (nDeltaBottom == 0)
                        {
                            for (u16 wCol = 0; wCol < wRealCell; wCol++)
                            {
                                if (wCol == 0)
                                {
                                    wIndex1 = dwBottomLeft + wIndexStride - wIndexStride * nBlockSize;
                                    wIndex2 = dwBottomLeft + wIndexStride;
                                    wIndex3 = dwBottomLeft;
                                    *(pIndices + dwIndex++) = wIndex1;//dwBottomLeft + wIndexStride - wIndexStride * nBlockSize;
                                    *(pIndices + dwIndex++) = wIndex2;//dwBottomLeft + wIndexStride;
                                    *(pIndices + dwIndex++) = wIndex3;//dwBottomLeft;

                                }
                                else if (wCol == wRealCell - 1)
                                {
                                    wIndex1 = dwBottomLeft + wIndexStride * wCol - wIndexStride * nBlockSize;
                                    wIndex2 = dwBottomRight;
                                    wIndex3 = dwBottomLeft + wIndexStride * wCol;
                                    *(pIndices + dwIndex++) = wIndex1;//dwBottomLeft + wIndexStride * wCol - wIndexStride * nBlockSize;
                                    *(pIndices + dwIndex++) = wIndex2;//dwBottomRight;
                                    *(pIndices + dwIndex++) = wIndex3;//dwBottomLeft + wIndexStride * wCol;
                                }
                                else
                                {
                                    wIndex1 = dwBottomLeft + wIndexStride * wCol - wIndexStride * nBlockSize;
                                    wIndex2 = dwBottomLeft + wIndexStride * (wCol + 1) - wIndexStride * nBlockSize;
                                    wIndex3 = dwBottomLeft + wIndexStride * (wCol + 1);
                                    *(pIndices + dwIndex++) = wIndex1;//dwBottomLeft + wIndexStride * wCol - wIndexStride * nBlockSize;
                                    *(pIndices + dwIndex++) = wIndex2;//dwBottomLeft + wIndexStride * (wCol + 1) - wIndexStride * nBlockSize;
                                    *(pIndices + dwIndex++) = wIndex3;//dwBottomLeft + wIndexStride * (wCol + 1);

                                    wIndex1 = dwBottomLeft + wIndexStride * wCol - wIndexStride * nBlockSize;
                                    wIndex2 = dwBottomLeft + wIndexStride * (wCol + 1);
                                    wIndex3 = dwBottomLeft + wIndexStride * wCol;
                                    *(pIndices + dwIndex++) = wIndex1;//dwBottomLeft + wIndexStride * wCol - wIndexStride * nBlockSize;
                                    *(pIndices + dwIndex++) = wIndex2;//dwBottomLeft + wIndexStride * (wCol + 1);
                                    *(pIndices + dwIndex++) = wIndex3;//dwBottomLeft + wIndexStride * wCol;
                                }
                            }
                        }
                        else
                        {
                            u16 wEdgeStride = pow(2.0, nDeltaBottom) * wIndexStride;
                            //int i = 0;
                            for (u16 wCol = 0; wCol < nGridPerRow; wCol += wEdgeStride)
                            {
                                if (nDeltaBottom == 1 && nSelfLOD == 2)
                                {
                                    int nDebug = 0;
                                }
                                u16 dwCenter = dwBottomLeft + (wCol + wEdgeStride / 2) - wIndexStride * nBlockSize;//m_wIndexStride + m_wIndexStride * m_wBlockSize + nCol * m_wIndexStride * 2;

                                //处理下边的三角形
                                wIndex1 = dwCenter;
                                wIndex2 = dwCenter + wEdgeStride / 2 + wIndexStride * nBlockSize;
                                wIndex3 = dwCenter - wEdgeStride / 2 + wIndexStride * nBlockSize;
                                *(pIndices + dwIndex++) = wIndex1;//dwCenter;
                                *(pIndices + dwIndex++) = wIndex2;//dwCenter + wEdgeStride * 0.5 + wIndexStride * nBlockSize;
                                *(pIndices + dwIndex++) = wIndex3;//dwCenter - wEdgeStride * 0.5 + wIndexStride * nBlockSize;

                                //处理左上边的三角形
                                if (wCol > 0)
                                {
                                    wIndex1 = dwBottomLeft + wCol - wIndexStride * nBlockSize;                          
                                    wIndex2 = dwBottomLeft + wCol + wIndexStride - wIndexStride * nBlockSize;
                                    wIndex3 = dwBottomLeft + wCol;
                                    *(pIndices + dwIndex++) = wIndex1;//dwBottomLeft + wCol - wIndexStride * nBlockSize;                          
                                    *(pIndices + dwIndex++) = wIndex2;//dwBottomLeft + wCol + wIndexStride - wIndexStride * nBlockSize;
                                    *(pIndices + dwIndex++) = wIndex3;//dwBottomLeft + wCol;
                                }

                                //处理右上边的三角形
                                if (wCol < nGridPerRow - wEdgeStride)
                                {
                                    wIndex1 = dwBottomLeft + wCol + wEdgeStride - wIndexStride * nBlockSize;
                                    wIndex2 = dwBottomLeft + wCol + wEdgeStride;
                                    wIndex3 = dwBottomLeft + wCol + wEdgeStride - wIndexStride - wIndexStride * nBlockSize;
                                    *(pIndices + dwIndex++) = wIndex1;//dwBottomLeft + wCol - wIndexStride * nBlockSize;                 
                                    *(pIndices + dwIndex++) = wIndex2;//dwBottomLeft + wCol + wIndexStride;
                                    *(pIndices + dwIndex++) = wIndex3;//dwBottomLeft + wCol;
                                }

                                //处理内边的三角形
                                int nTriangleCount = (int)pow(2.0, nDeltaBottom) - 2;
                                for (int i = 0 ; i < nTriangleCount / 2; i++)
                                {
                                    wIndex1 = dwBottomLeft + wCol + (i + 1) * wIndexStride - wIndexStride * nBlockSize;
                                    wIndex2 = dwBottomLeft + wCol + (i + 2) * wIndexStride - wIndexStride * nBlockSize;
                                    wIndex3 = dwBottomLeft + wCol;
                                    *(pIndices + dwIndex++) = wIndex1;//wCol + wEdgeStride + wIndexStride * nBlockSize;                          
                                    *(pIndices + dwIndex++) = wIndex2;//wCol + wEdgeStride + wIndexStride * nBlockSize - wIndexStride;
                                    *(pIndices + dwIndex++) = wIndex3;//wCol + wEdgeStride;
                                }

                                for (int i = nTriangleCount / 2; i < nTriangleCount; i++)
                                {
                                    wIndex1 = dwBottomLeft + wCol + (i + 1) * wIndexStride - wIndexStride * nBlockSize;
                                    wIndex2 = dwBottomLeft + wCol + (i + 2) * wIndexStride - wIndexStride * nBlockSize;
                                    wIndex3 = dwBottomLeft + wCol + wEdgeStride;
                                    *(pIndices + dwIndex++) = wIndex1;//wCol + wEdgeStride + wIndexStride * nBlockSize;                          
                                    *(pIndices + dwIndex++) = wIndex2;//wCol + wEdgeStride + wIndexStride * nBlockSize - wIndexStride;
                                    *(pIndices + dwIndex++) = wIndex3;//wCol + wEdgeStride;
                                }

                                //i++;   //调试用
                            }
                        }
                        

                        //处理左边
                        if (nDeltaLeft == 0)
                        {
                            for (u16 wRow = 0; wRow < wRealCell; wRow++)
                            {
                                if (wRow == 0)
                                {
                                    wIndex1 = dwTopLeft;
                                    wIndex2 = dwTopLeft + wIndexStride + wIndexStride * nBlockSize;
                                    wIndex3 = dwTopLeft + wIndexStride * nBlockSize;
                                    *(pIndices + dwIndex++) = wIndex1;//dwTopLeft;
                                    *(pIndices + dwIndex++) = wIndex2;//dwTopLeft + wIndexStride + wIndexStride * nBlockSize;
                                    *(pIndices + dwIndex++) = wIndex3;//dwTopLeft + wIndexStride * nBlockSize;
                                }
                                else if (wRow == wRealCell - 1)
                                {
                                    wIndex1 = dwTopLeft + wRow * wIndexStride * nBlockSize;
                                    wIndex2 = dwTopLeft + wIndexStride + wRow * wIndexStride * nBlockSize;
                                    wIndex3 = dwBottomLeft;
                                    *(pIndices + dwIndex++) = wIndex1;//dwTopLeft + wRow * wIndexStride * nBlockSize;
                                    *(pIndices + dwIndex++) = wIndex2;//dwTopLeft + wIndexStride + wRow * wIndexStride * nBlockSize;
                                    *(pIndices + dwIndex++) = wIndex3;//dwBottomLeft;
                                }
                                else
                                {
                                    wIndex1 = dwTopLeft + wRow * wIndexStride * nBlockSize;
                                    wIndex2 = dwTopLeft + wIndexStride + wRow * wIndexStride * nBlockSize;
                                    wIndex3 = dwTopLeft + wIndexStride + (wRow + 1) * wIndexStride * nBlockSize;
                                    *(pIndices + dwIndex++) = wIndex1;//dwTopLeft + wRow * wIndexStride * nBlockSize;
                                    *(pIndices + dwIndex++) = wIndex2;//dwTopLeft + wIndexStride + wRow * wIndexStride * nBlockSize;
                                    *(pIndices + dwIndex++) = wIndex3;//dwTopLeft + wIndexStride + (wRow + 1) * wIndexStride * nBlockSize;

                                    wIndex1 = dwTopLeft + wRow * wIndexStride * nBlockSize;
                                    wIndex2 = dwTopLeft + wIndexStride + (wRow + 1) * wIndexStride * nBlockSize;
                                    wIndex3 = dwTopLeft + (wRow + 1) * wIndexStride * nBlockSize;
                                    *(pIndices + dwIndex++) = wIndex1;//dwTopLeft + wRow * wIndexStride * nBlockSize;
                                    *(pIndices + dwIndex++) = wIndex2;//dwTopLeft + wIndexStride + (wRow + 1) * wIndexStride * nBlockSize;
                                    *(pIndices + dwIndex++) = wIndex3;//dwTopLeft + (wRow + 1) * wIndexStride * nBlockSize;
                                }

                            }
                        }
                        else
                        {
                            u16 wEdgeStride = (u16)pow(2.0, nDeltaLeft) * wIndexStride;
                            for (u16 wRow = 0; wRow < nGridPerRow; wRow += wEdgeStride)
                            {
                                if (nDeltaLeft == 1)
                                {
                                    int nDebug = 0;
                                }
                                u16 wCenter = dwTopLeft + wIndexStride + (wRow + wEdgeStride / 2) * nBlockSize;

                                //处理左边的三角形
                                wIndex1 = wCenter;
                                wIndex2 = wCenter - wIndexStride + wEdgeStride / 2 * nBlockSize;
                                wIndex3 = wCenter - wIndexStride - wEdgeStride / 2 * nBlockSize;
                                *(pIndices + dwIndex++) = wIndex1;//wCenter;                          
                                *(pIndices + dwIndex++) = wIndex2;//wCenter - wIndexStride + wEdgeStride * 0.5 * nBlockSize;
                                *(pIndices + dwIndex++) = wIndex3;//wCenter - wIndexStride - wEdgeStride * 0.5 * nBlockSize;

                                //处理上边的三角形
                                if (wRow > 0)
                                {
                                    wIndex1 = dwTopLeft + wIndexStride + (wRow + wIndexStride) * nBlockSize;
                                    wIndex2 = dwTopLeft + wRow * nBlockSize;
                                    wIndex3 = dwTopLeft + wRow * nBlockSize + wIndexStride;
                                    *(pIndices + dwIndex++) = wIndex1;//dwTopLeft + wIndexStride + (wRow + wIndexStride) * nBlockSize;                          
                                    *(pIndices + dwIndex++) = wIndex2;//dwTopLeft + wRow * nBlockSize;
                                    *(pIndices + dwIndex++) = wIndex3;//dwTopLeft + wRow * nBlockSize + wIndexStride;
                                }

                                //处理下边的三角形
                                if (wRow < nGridPerRow - wEdgeStride)
                                {
                                    wIndex1 = dwTopLeft + wIndexStride + (wRow + wEdgeStride - wIndexStride) * nBlockSize;
                                    wIndex2 = dwTopLeft + wIndexStride + (wRow + wEdgeStride) * nBlockSize;
                                    wIndex3 = dwTopLeft + (wRow + wEdgeStride) * nBlockSize;
                                    *(pIndices + dwIndex++) = wIndex1;//dwTopLeft + wIndexStride + (wRow + wEdgeStride - wIndexStride) * nBlockSize;                          
                                    *(pIndices + dwIndex++) = wIndex2;//dwTopLeft + wIndexStride + (wRow + wEdgeStride) * nBlockSize;
                                    *(pIndices + dwIndex++) = wIndex3;//dwTopLeft + (wRow + wEdgeStride) * nBlockSize;
                                }

                                //处理内边的三角形
                                int nTriangleCount = (int)pow(2.0, nDeltaLeft) - 2;
                                for (int i = 0; i < nTriangleCount / 2; i++)
                                {
                                    wIndex1 = dwTopLeft + wIndexStride + (wRow + (i + 1) * wIndexStride) * nBlockSize;
                                    wIndex2 = dwTopLeft + wIndexStride + (wRow + (i + 2) * wIndexStride) * nBlockSize;
                                    wIndex3 = dwTopLeft + wRow * nBlockSize;
                                    *(pIndices + dwIndex++) = wIndex1;//wCol + wEdgeStride + wIndexStride * nBlockSize;                          
                                    *(pIndices + dwIndex++) = wIndex2;//wCol + wEdgeStride + wIndexStride * nBlockSize - wIndexStride;
                                    *(pIndices + dwIndex++) = wIndex3;//wCol + wEdgeStride;
                                }

                                for (int i = nTriangleCount / 2; i < nTriangleCount; i++)
                                {
                                    wIndex1 = dwTopLeft + wIndexStride + (wRow + (i + 1) * wIndexStride) * nBlockSize;
                                    wIndex2 = dwTopLeft + wIndexStride + (wRow + (i + 2) * wIndexStride) * nBlockSize;
                                    wIndex3 = dwTopLeft + (wRow + wEdgeStride) * nBlockSize;
                                    *(pIndices + dwIndex++) = wIndex1;//wCol + wEdgeStride + wIndexStride * nBlockSize;                          
                                    *(pIndices + dwIndex++) = wIndex2;//wCol + wEdgeStride + wIndexStride * nBlockSize - wIndexStride;
                                    *(pIndices + dwIndex++) = wIndex3;//wCol + wEdgeStride;
                                }

                            }

                        }
                        

                        //处理边界里面的三角形
                        
                        for (u16 wRow = 1; wRow < wRealCell - 1; wRow++)
                        {
                            for (u16 wCol = 1; wCol < wRealCell - 1; wCol++)
                            {
                                wIndex1 = dwTopLeft + wCol * wIndexStride + wRow * wIndexStride * nBlockSize;
                                wIndex2 = dwTopLeft + (wCol + 1) * wIndexStride + wRow * wIndexStride * nBlockSize;
                                wIndex3 = dwTopLeft + (wCol + 1) * wIndexStride + (wRow + 1) * wIndexStride * nBlockSize;
                                *(pIndices + dwIndex++) = wIndex1; //dwTopLeft + wCol * wIndexStride + wRow * wIndexStride * nBlockSize;
                                *(pIndices + dwIndex++) = wIndex2; //dwTopLeft + (wCol + 1) * wIndexStride + wRow * wIndexStride * nBlockSize;
                                *(pIndices + dwIndex++) = wIndex3; //dwTopLeft + (wCol + 1) * wIndexStride + (wRow + 1) * wIndexStride * nBlockSize;

                                wIndex1 = dwTopLeft + wCol * wIndexStride + wRow * wIndexStride * nBlockSize;
                                wIndex2 = dwTopLeft + (wCol + 1) * wIndexStride + (wRow + 1) * wIndexStride * nBlockSize;
                                wIndex3 = dwTopLeft + wCol * wIndexStride + (wRow + 1) * wIndexStride * nBlockSize;
                                *(pIndices + dwIndex++) = wIndex1;  //dwTopLeft + wCol * wIndexStride + wRow * wIndexStride * nBlockSize;
                                *(pIndices + dwIndex++) = wIndex2;  //dwTopLeft + (wCol + 1) * wIndexStride + (wRow + 1) * wIndexStride * nBlockSize;
                                *(pIndices + dwIndex++) = wIndex3;  //dwTopLeft + wCol * wIndexStride + (wRow + 1) * wIndexStride * nBlockSize;
                            }
                        }

                        //处理完毕

                        pIndexBuffer->Unlock();

                        pIndexBuffer->SetTotalIndex(dwIndex);

                        CLODInfo* pLODInfo = new CLODInfo(nSelfLOD, nDeltaLeft, nDeltaTop, nDeltaRight, nDeltaBottom);
                        pLODInfo->SetIndexBuffer(pIndexBuffer);

                        m_mapLODInfo.insert(make_pair(m_nLODInfoCount++, pLODInfo));
                        //m_nIndexCount++;
                    }
                }
            }
        }
    }

    return m_nLODInfoCount;
}

CLODInfo* CTerrain::FindLODInfo(const CLODInfo& info)
{
    for (LODInfo_Iter it = m_mapLODInfo.begin(); it != m_mapLODInfo.end(); it++)
    {
        CLODInfo* pInfo = it->second;
        if (*pInfo == info)
        {
            return pInfo;
        }
    }

    return CreateLODInfo(info, m_config.dwBlockSize);
}

/*
CHeightMap* CTerrain::CreateHeightMap(TiXmlElement* pElement)
{
    const char* szAttribute = pElement->Attribute("ID");

    if (szAttribute)
    {
        int nID = atoi(szAttribute);
        HeightMap_Iter it = m_mapHeightMap.find(nID);
        if (it == m_mapHeightMap.end())
        {
            CHeightMap* pHeightMap = AURO_NEW_VERBOSE CHeightMap;
            if (!pHeightMap->LoadFromXML(pElement))
            {
                //抛出异常
                SafeDelete<CHeightMap*>(pHeightMap);
                return NULL;
            }
            AddHeightMap(pHeightMap);
            return pHeightMap;
        }

        return it->second;
    }

    return NULL;
}
*/

bool CTerrain::Init(const LPTERRAINCONFIG pConfig)
{
    Clear();
    m_config = *pConfig;
    CLODInfo::m_nMaxLOD = m_config.wLODLevel;
 

    //int nLODInfoCount = ComputeIndices(m_config.wLODLevel, m_config.dwBlockSize);

    m_bInitialize = true;

    return true;
}

void CTerrain::GenLightmap(LPVERTEX_NORMAL pNormal, AIRImage* pHeightMap, float fYScale, const Vector3Df& sunLight, int nLightmapSize, float fAmbient, AIRImage& imgLightMapOut)
{
	/*
    bool bVerticleLight = (sunLight == Vector3Df::UNIT_Y || sunLight == Vector3Df::NEGATIVE_UNIT_Y);

    ImageOperator* pLightMap = IMAGEOPERATORMGR->CreateDynamicImageOperator("TerrainLightmap");

    pLightMap->SetBytePerPixel(1);
    pLightMap->SetCodecType(eCT_UNSIGNED_BYTE);
    pLightMap->SetImageDepth( 1 );
    pLightMap->SetImageHeight( pHeightMap->GetImageHeight() );
    pLightMap->SetImageWidth( pHeightMap->GetImageHeight() );
    pLightMap->SetOriginType( eOT_ORIGIN_LOWER_LEFT );
    pLightMap->SetPixelFormat( ePF_LUMINANCE );
    pLightMap->SetNumMips(1);

    nLightmapSize = pHeightMap->GetImageWidth();   //先暂时赋这个值

    u32 dwSize = nLightmapSize * nLightmapSize;
    u8* pData = new u8[dwSize];

    Vector3Df position;     //纹理上的坐标

    Vector3Df orign;        //太阳坐标

    Vector3Df sunDirection = -sunLight;
    sunDirection.Normalize();

    OriginType originType = pHeightMap->GetOriginType();
    if (originType == eOT_ORIGIN_UPPER_LEFT)
    {
        for (int nRow = 0; nRow < nLightmapSize; ++nRow)
        {
            for (int nCol = 0; nCol < nLightmapSize; ++nCol)
            {
                position.z = nRow;
                position.x = nCol;
                //int nHeightMapCol = (int)((float)nCol * ((float)pHeightMap->GetImageWidth() / nLightmapSize) + 0.5f);
                //int nHeightMapRow = (int)((float)nRow * ((float)pHeightMap->GetImageWidth() / nLightmapSize) + 0.5f);
                position.y = pHeightMap->GetPixelAsInt(nCol, nRow) * fYScale / 255.0f;

                float fColor = fAmbient;
                orign = position + sunDirection * 3000.0f;
  

                if (!bVerticleLight)
                {
                    if (!IntersectMap(position, orign, sunDirection, pHeightMap, fYScale))
                    {
                        fColor += 0.5f * (1.0f + pNormal[nRow * pHeightMap->GetImageHeight() + nCol].normal.DotProduct(sunDirection));
                        if (fColor > 1.0f)
                        {
                            fColor = 1.0f;
                        }
                    }
                    else
                    {
                        int a = 0;
                    }
                }
                else
                {
                    fColor += 0.5f * (1.0f + pNormal[nRow * pHeightMap->GetImageHeight() + nCol].normal.DotProduct(sunDirection));
                    if (fColor > 1.0f)
                    {
                        fColor = 1.0f;
                    }
                }

                pData[nRow * nLightmapSize + nCol] = fColor * 255;
            }
        }
    }
    
    else
    {
        for (int nRow = nLightmapSize - 1, nPixelRow = 0; nRow >= 0; --nRow, ++nPixelRow)
        {
            for (int nCol = 0; nCol < nLightmapSize; ++nCol)
            {
                position.z = nRow;
                position.x = nCol;
                //int nHeightMapCol = (int)((float)nCol * ((float)pHeightMap->GetImageWidth() / nLightmapSize) + 0.5f);
                //int nHeightMapRow = (int)((float)nRow * ((float)pHeightMap->GetImageWidth() / nLightmapSize) + 0.5f);
                //int nNormalRow = (int)((float)nPixelRow * ((float)pHeightMap->GetImageWidth() / nLightmapSize) + 0.5f);
                position.y = pHeightMap->GetPixelAsInt(nCol, nRow) * fYScale / m_config.fGridLength;

                float fColor = fAmbient;
                orign = position + sunDirection * 3000.0f;

                if (!IntersectMap(position, orign, sunDirection, pHeightMap, fYScale))
                {
                    fColor += 0.5f * (1.0f + pNormal[nRow * pHeightMap->GetImageHeight() + nCol].normal.DotProduct(sunDirection));
                    if (fColor > 1.0f)
                    {
                        fColor = 1.0f;
                    }
                }
                else
                {
                    int a = 0;
                }

                pData[nPixelRow * nLightmapSize + nCol] = fColor * 255;
            }
        }
    }
    

    pLightMap->SetData(pData, dwSize);
    SafeDeleteArray(pData);

    //pLightMap->ScaleSize(1024, 1024);
    return pLightMap;
	*/
}

int CTerrain::IntersectMap(const Vector3Df& pos, const Vector3Df& vOrig, const Vector3Df& Direction, AIRImage& imgHeightMap, float fHeightScale)
{
    int nHits = 0;
    float d, h, D; 

    Vector3Df texPos = pos + Direction;
    Vector3Df texDir;

    int w = imgHeightMap.GetHeight(); //pHeightMap->GetImageWidth();

    while (!(( texPos.x >= w - 1 ) || ( texPos.x <= 0 ) || ( texPos.z >= w - 1 ) || ( texPos.z <= 0 )))
    {
        // length of lightdir's projection
        D = (Vector3Df(texPos.x, 0, texPos.z) - Vector3Df(vOrig.x, 0, vOrig.z)).GetLength();
        d = (pos - texPos).GetLength();

        h = pos.y + (d * vOrig.y) / D;  // X(P) point

		if (imgHeightMap.GetPixelAsInt((int)texPos.x, (int)texPos.z) * fHeightScale / m_config.fGridLength > h)
		{
			nHits++;   // if so, mark as hit, and skip this work point.
			break;
		}
		/*
        // check if height in point P is bigger than point X's height
        if (pHeightMap->GetOriginType() == eOT_ORIGIN_UPPER_LEFT)
        {
            if (pHeightMap->GetPixelAsInt((int)texPos.x, (int)texPos.z) * fHeightScale / m_config.fGridLength > h) //data[int)texPos.z * w + ifloor(v.x)] * fHeightScale > h)
            {
                nHits++;   // if so, mark as hit, and skip this work point.
                break;
            };
        }
        else
        {
            //if (texPos.x > 181 && texPos.x < 186 && texPos.z > 255)
            //{
            //    int a = 0;
            //}
            float currentHeight = pHeightMap->GetPixelAsInt((int)(texPos.x), (int)(texPos.z)) * fHeightScale / m_config.fGridLength;
            if (currentHeight > h)
            {
                nHits++;   // if so, mark as hit, and skip this work point.
                break;
            }
        }
        */

        texDir = Direction;
        texDir.y = 0;
        texPos += texDir.Normalize();   // fetch new working point
    }

    return nHits;
}

CLODInfo* CTerrain::CreateLODInfo(const CLODInfo& info, u16 nBlockSize)
{
    u16 nSelfLOD = info.m_nLODLevel;
    s32 nDeltaTop = info.m_nTopLODDelta;
    s32 nDeltaLeft = info.m_nLeftLODDelta;
    s32 nDeltaRight = info.m_nRightLODDelta;
    s32 nDeltaBottom = info.m_nBottomLODDelta;
    u16 nGridPerRow = nBlockSize - 1;
    u32 dwNumBlockIndices = (nBlockSize - 1) * (nBlockSize - 1) * 6;

    m_dwNumBlockVertices = nBlockSize * nBlockSize;
    u16 wIndexStride = (u16)pow(2.0, nSelfLOD - 1);
    u16 wRealCell = nGridPerRow / wIndexStride;

    IRenderer* pRenderer = ENGINE_INST->GetRenderer();
    IIndexBuffer* pIndexBuffer = pRenderer->CreateIndexBuffer(dwNumBlockIndices);


    u16* pIndices = NULL;
    u32 dwIndex = 0;

    u16 dwTopLeft = 0;//m_data.dwTopLeft;     //声明，topleft是要在block里的topleft，而不是在整个地形的topleft
    u16 dwTopRight = nBlockSize - 1;
    u16 dwBottomRight = m_dwNumBlockVertices - 1;
    u16 dwBottomLeft = dwBottomRight - (nBlockSize - 1);

    pIndices = (u16*)pIndexBuffer->Lock(0, 0);
    //((LPDIRECT3DINDEXBUFFER9)(pIndexBuffer->GetInterface()))->Lock(0, 0, (void**)&pIndices, 0);


    u16 wIndex1 = 0;
    u16 wIndex2 = 0;
    u16 wIndex3 = 0;

    //处理上边
    if (nDeltaTop == 0)
    {
        for (u16 wCol = 0; wCol < nGridPerRow; wCol += wIndexStride)
        {
            if (wCol == 0)
            {
                wIndex1 = dwTopLeft + wIndexStride + wIndexStride * nBlockSize;
                wIndex2 = dwTopLeft;
                wIndex3 = dwTopLeft + wIndexStride;
                *(pIndices + dwIndex++) = wIndex1;  //dwTopLeft + wIndexStride + wIndexStride * nBlockSize;
                *(pIndices + dwIndex++) = wIndex2;  //dwTopLeft + wIndexStride;  //dwTopLeft;
                *(pIndices + dwIndex++) = wIndex3;  //dwTopLeft;               //dwTopLeft + wIndexStride;

            }
            else if (wCol == nGridPerRow - wIndexStride)
            {
                wIndex1 = dwTopLeft + wCol;
                wIndex2 = dwTopLeft + wCol + wIndexStride;
                wIndex3 = dwTopLeft + wCol + wIndexStride * nBlockSize;
                *(pIndices + dwIndex++) = wIndex1;   //dwTopLeft + wCol;
                *(pIndices + dwIndex++) = wIndex2;   //dwTopLeft + wCol + wIndexStride;
                *(pIndices + dwIndex++) = wIndex3;   //dwTopLeft + wCol + wIndexStride * nBlockSize;
            }
            else
            {
                wIndex1 = dwTopLeft + wCol;
                wIndex2 = dwTopLeft + wCol + wIndexStride;
                wIndex3 = dwTopLeft + wCol + wIndexStride + wIndexStride * nBlockSize;
                *(pIndices + dwIndex++) = wIndex1; //dwTopLeft + wCol;
                *(pIndices + dwIndex++) = wIndex2; //dwTopLeft + wCol + wIndexStride;
                *(pIndices + dwIndex++) = wIndex3; //dwTopLeft + wCol + wIndexStride + wIndexStride * nBlockSize;

                wIndex1 = dwTopLeft + wCol;
                wIndex2 = dwTopLeft + wCol + wIndexStride + wIndexStride * nBlockSize;
                wIndex3 = dwTopLeft + wCol + wIndexStride * nBlockSize;
                *(pIndices + dwIndex++) = wIndex1;  //dwTopLeft + wCol;
                *(pIndices + dwIndex++) = wIndex2;  //dwTopLeft + wCol + wIndexStride + wIndexStride * nBlockSize;
                *(pIndices + dwIndex++) = wIndex3;  //dwTopLeft + wCol + wIndexStride * nBlockSize;
            }
        }
    }
    else
    {
        u16 wEdgeStride = (u16)pow(2.0, nDeltaTop) * wIndexStride;   //实际上wEdgeStride为相邻block的indexStride

        for (u16 wCol = 0; wCol < nGridPerRow; wCol += wEdgeStride)
        {
            if (nDeltaTop == 2 && nSelfLOD == 2)
            {
                int nDebug = 0;
            }
            u16 dwBottom = dwTopLeft + wCol + wEdgeStride / 2 + wIndexStride * nBlockSize;//m_wIndexStride + m_wIndexStride * m_wBlockSize + nCol * m_wIndexStride * 2;

            //处理上边的三角形
            wIndex1 = dwBottom;
            wIndex2 = dwBottom - wEdgeStride * 0.5 - wIndexStride * nBlockSize;
            wIndex3 = dwBottom + wEdgeStride * 0.5 - wIndexStride * nBlockSize;
            *(pIndices + dwIndex++) = wIndex1;//dwBottom;                          
            *(pIndices + dwIndex++) = wIndex2;//dwBottom - wEdgeStride * 0.5 - wIndexStride * nBlockSize;
            *(pIndices + dwIndex++) = wIndex3;//dwBottom + wEdgeStride * 0.5 - wIndexStride * nBlockSize;

            //处理左下边的三角形
            if (wCol > 0)
            {
                wIndex1 = wCol + wIndexStride + wIndexStride * nBlockSize;
                wIndex2 = wCol + wIndexStride * nBlockSize;
                wIndex3 = wCol;
                *(pIndices + dwIndex++) = wIndex1;//wCol + wIndexStride + wIndexStride * nBlockSize;                          
                *(pIndices + dwIndex++) = wIndex2;//wCol + wIndexStride * nBlockSize;
                *(pIndices + dwIndex++) = wIndex3;//wCol;
            }

            //处理右下边的三角形
            if (wCol < nGridPerRow - wEdgeStride)
            {
                wIndex1 = wCol + wEdgeStride + wIndexStride * nBlockSize;
                wIndex2 = wCol + wEdgeStride + wIndexStride * nBlockSize - wIndexStride;
                wIndex3 = wCol + wEdgeStride;
                *(pIndices + dwIndex++) = wIndex1;//wCol + wEdgeStride + wIndexStride * nBlockSize;                          
                *(pIndices + dwIndex++) = wIndex2;//wCol + wEdgeStride + wIndexStride * nBlockSize - wIndexStride;
                *(pIndices + dwIndex++) = wIndex3;//wCol + wEdgeStride;
            }

            //处理内边的三角形
            int nTriangleCount = (int)pow(2.0, nDeltaTop) - 2;
            for (int i = 0 ; i < nTriangleCount * 0.5; i++)
            {
                wIndex1 = wCol + (i + 1) * wIndexStride + wIndexStride * nBlockSize;
                wIndex2 = dwTopLeft + wCol;
                wIndex3 = wCol + (i + 2) * wIndexStride + wIndexStride * nBlockSize;
                *(pIndices + dwIndex++) = wIndex1;//wCol + wEdgeStride + wIndexStride * nBlockSize;                          
                *(pIndices + dwIndex++) = wIndex2;//wCol + wEdgeStride + wIndexStride * nBlockSize - wIndexStride;
                *(pIndices + dwIndex++) = wIndex3;//wCol + wEdgeStride;
            }

            for (int i = nTriangleCount * 0.5; i < nTriangleCount; i++)
            {
                wIndex1 = wCol + (i + 1) * wIndexStride + wIndexStride * nBlockSize;
                wIndex2 = dwTopLeft + wCol + wEdgeStride;
                wIndex3 = wCol + (i + 2) * wIndexStride + wIndexStride * nBlockSize;
                *(pIndices + dwIndex++) = wIndex1;//wCol + wEdgeStride + wIndexStride * nBlockSize;                          
                *(pIndices + dwIndex++) = wIndex2;//wCol + wEdgeStride + wIndexStride * nBlockSize - wIndexStride;
                *(pIndices + dwIndex++) = wIndex3;//wCol + wEdgeStride;
            }

        }

    }


    //处理右边

    if (nDeltaRight == 0)
    {
        for (u16 nRow = 0; nRow < nGridPerRow; nRow += wIndexStride)
        {
            if (nRow == 0)         //第一格
            {
                wIndex1 = dwTopRight;
                wIndex2 = dwTopRight + wIndexStride * nBlockSize;
                wIndex3 = dwTopRight + wIndexStride * nBlockSize - wIndexStride;
                *(pIndices + dwIndex++) = wIndex1;//dwTopRight;
                *(pIndices + dwIndex++) = wIndex2;//dwTopRight + wIndexStride * nBlockSize;
                *(pIndices + dwIndex++) = wIndex3;//dwTopRight + wIndexStride * nBlockSize - wIndexStride;
            }
            else if (nRow == nGridPerRow - wIndexStride)
            {
                wIndex1 = dwTopRight + nRow * nBlockSize;
                wIndex2 = dwTopRight + (nRow + wIndexStride) * nBlockSize;
                wIndex3 = dwTopRight - wIndexStride + nRow * nBlockSize;
                *(pIndices + dwIndex++) = wIndex1;//dwTopRight + nRow * nBlockSize;
                *(pIndices + dwIndex++) = wIndex2;//dwTopRight + (nRow + wIndexStride) * nBlockSize;
                *(pIndices + dwIndex++) = wIndex3;//dwTopRight - wIndexStride + nRow * nBlockSize;
            }
            else
            {
                wIndex1 = dwTopRight - wIndexStride + nRow * nBlockSize;
                wIndex2 = dwTopRight + nRow * nBlockSize;
                wIndex3 = dwTopRight + (nRow + wIndexStride) * nBlockSize;
                *(pIndices + dwIndex++) = wIndex1;//dwTopRight - wIndexStride + nRow * nBlockSize;
                *(pIndices + dwIndex++) = wIndex2;//dwTopRight + nRow * nBlockSize;
                *(pIndices + dwIndex++) = wIndex3;//dwTopRight + (nRow + wIndexStride) * nBlockSize;

                wIndex1 = dwTopRight - wIndexStride + nRow * nBlockSize;
                wIndex2 = dwTopRight + (nRow + wIndexStride) * nBlockSize;
                wIndex3 = dwTopRight - wIndexStride + (nRow + wIndexStride) * nBlockSize;
                *(pIndices + dwIndex++) = wIndex1;//dwTopRight - wIndexStride + nRow * nBlockSize;
                *(pIndices + dwIndex++) = wIndex2;//dwTopRight + (nRow + wIndexStride) * nBlockSize;
                *(pIndices + dwIndex++) = wIndex3;//dwTopRight - wIndexStride + (nRow + wIndexStride) * nBlockSize;
            }
        }
    }
    else
    {
        u16 wEdgeStride = pow(2.0, nDeltaRight) * wIndexStride;
        for (u16 wRow = 0; wRow < nGridPerRow; wRow += wEdgeStride)
        {
            if (nDeltaRight == 2)
            {
                int nDebug = 0;
            }
            u16 wCenter = (dwTopRight - wIndexStride) + (wRow + wEdgeStride * 0.5) * nBlockSize;

            //处理右边的三角形
            wIndex1 = wCenter;
            wIndex2 = dwTopRight + wRow * nBlockSize;//wCenter + wIndexStride - (wRow + wEdgeStride * 0.5) * nBlockSize;
            wIndex3 = dwTopRight + (wRow + wEdgeStride) * nBlockSize;//wCenter + wIndexStride + (wRow + wEdgeStride * 0.5) * nBlockSize;
            *(pIndices + dwIndex++) = wIndex1;//wCenter;                          
            *(pIndices + dwIndex++) = wIndex2;//wCenter + wIndexStride - (wRow + wEdgeStride * 0.5) * nBlockSize;
            *(pIndices + dwIndex++) = wIndex3;//wCenter + wIndexStride + (wRow + wEdgeStride * 0.5) * nBlockSize;

            //处理上边的三角形
            if (wRow > 0)
            {
                wIndex1 = dwTopRight - wIndexStride + (wRow + wIndexStride) * nBlockSize;
                wIndex2 = dwTopRight - wIndexStride + wRow * nBlockSize;
                wIndex3 = dwTopRight + wRow * nBlockSize;
                *(pIndices + dwIndex++) = wIndex1;//dwTopRight - wIndexStride + (wRow + wIndexStride) * nBlockSize;                          
                *(pIndices + dwIndex++) = wIndex2;//dwTopRight - wIndexStride + wRow * nBlockSize;
                *(pIndices + dwIndex++) = wIndex3;//dwTopRight + wRow * nBlockSize;
            }

            //处理下边的三角形
            if (wRow < nGridPerRow - wEdgeStride)
            {
                wIndex1 = dwTopRight - wIndexStride + (wRow + wEdgeStride - wIndexStride) * nBlockSize;
                wIndex2 = dwTopRight + (wRow + wEdgeStride) * nBlockSize;
                wIndex3 = dwTopRight - wIndexStride + (wRow + wEdgeStride) * nBlockSize;
                *(pIndices + dwIndex++) = wIndex1;//dwTopRight - wIndexStride + (wRow + wIndexStride) * nBlockSize;                          
                *(pIndices + dwIndex++) = wIndex2;//dwTopRight - wIndexStride + wRow * nBlockSize;
                *(pIndices + dwIndex++) = wIndex3;//dwTopRight + wRow * nBlockSize;
            }

            //处理内边的三角形
            int nTriangleCount = pow(2.0, nDeltaRight) - 2;
            for (int i = 0; i < nTriangleCount * 0.5; i++)
            {
                wIndex1 = dwTopRight - wIndexStride + (wRow + (i + 1) * wIndexStride) * nBlockSize;
                wIndex2 = dwTopRight + wRow * nBlockSize;
                wIndex3 = dwTopRight - wIndexStride + (wRow + (i + 2) * wIndexStride) * nBlockSize;
                *(pIndices + dwIndex++) = wIndex1;//wCol + wEdgeStride + wIndexStride * nBlockSize;                          
                *(pIndices + dwIndex++) = wIndex2;//wCol + wEdgeStride + wIndexStride * nBlockSize - wIndexStride;
                *(pIndices + dwIndex++) = wIndex3;//wCol + wEdgeStride;
            }

            for (int i = nTriangleCount * 0.5; i < nTriangleCount; i++)
            {
                wIndex1 = dwTopRight - wIndexStride + (wRow + (i + 1) * wIndexStride) * nBlockSize;
                wIndex2 = dwTopRight + (wRow + wEdgeStride) * nBlockSize;
                wIndex3 = dwTopRight - wIndexStride + (wRow + (i + 2) * wIndexStride) * nBlockSize;
                *(pIndices + dwIndex++) = wIndex1;//wCol + wEdgeStride + wIndexStride * nBlockSize;                          
                *(pIndices + dwIndex++) = wIndex2;//wCol + wEdgeStride + wIndexStride * nBlockSize - wIndexStride;
                *(pIndices + dwIndex++) = wIndex3;//wCol + wEdgeStride;
            }

        }

    }



    //处理下边
    if (nDeltaBottom == 0)
    {
        for (u16 wCol = 0; wCol < wRealCell; wCol++)
        {
            if (wCol == 0)
            {
                wIndex1 = dwBottomLeft + wIndexStride - wIndexStride * nBlockSize;
                wIndex2 = dwBottomLeft + wIndexStride;
                wIndex3 = dwBottomLeft;
                *(pIndices + dwIndex++) = wIndex1;//dwBottomLeft + wIndexStride - wIndexStride * nBlockSize;
                *(pIndices + dwIndex++) = wIndex2;//dwBottomLeft + wIndexStride;
                *(pIndices + dwIndex++) = wIndex3;//dwBottomLeft;

            }
            else if (wCol == wRealCell - 1)
            {
                wIndex1 = dwBottomLeft + wIndexStride * wCol - wIndexStride * nBlockSize;
                wIndex2 = dwBottomRight;
                wIndex3 = dwBottomLeft + wIndexStride * wCol;
                *(pIndices + dwIndex++) = wIndex1;//dwBottomLeft + wIndexStride * wCol - wIndexStride * nBlockSize;
                *(pIndices + dwIndex++) = wIndex2;//dwBottomRight;
                *(pIndices + dwIndex++) = wIndex3;//dwBottomLeft + wIndexStride * wCol;
            }
            else
            {
                wIndex1 = dwBottomLeft + wIndexStride * wCol - wIndexStride * nBlockSize;
                wIndex2 = dwBottomLeft + wIndexStride * (wCol + 1) - wIndexStride * nBlockSize;
                wIndex3 = dwBottomLeft + wIndexStride * (wCol + 1);
                *(pIndices + dwIndex++) = wIndex1;//dwBottomLeft + wIndexStride * wCol - wIndexStride * nBlockSize;
                *(pIndices + dwIndex++) = wIndex2;//dwBottomLeft + wIndexStride * (wCol + 1) - wIndexStride * nBlockSize;
                *(pIndices + dwIndex++) = wIndex3;//dwBottomLeft + wIndexStride * (wCol + 1);

                wIndex1 = dwBottomLeft + wIndexStride * wCol - wIndexStride * nBlockSize;
                wIndex2 = dwBottomLeft + wIndexStride * (wCol + 1);
                wIndex3 = dwBottomLeft + wIndexStride * wCol;
                *(pIndices + dwIndex++) = wIndex1;//dwBottomLeft + wIndexStride * wCol - wIndexStride * nBlockSize;
                *(pIndices + dwIndex++) = wIndex2;//dwBottomLeft + wIndexStride * (wCol + 1);
                *(pIndices + dwIndex++) = wIndex3;//dwBottomLeft + wIndexStride * wCol;
            }
        }
    }
    else
    {
        u16 wEdgeStride = pow(2.0, nDeltaBottom) * wIndexStride;
        //int i = 0;
        for (u16 wCol = 0; wCol < nGridPerRow; wCol += wEdgeStride)
        {
            if (nDeltaBottom == 1 && nSelfLOD == 2)
            {
                int nDebug = 0;
            }
            u16 dwCenter = dwBottomLeft + (wCol + wEdgeStride / 2) - wIndexStride * nBlockSize;//m_wIndexStride + m_wIndexStride * m_wBlockSize + nCol * m_wIndexStride * 2;

            //处理下边的三角形
            wIndex1 = dwCenter;
            wIndex2 = dwCenter + wEdgeStride / 2 + wIndexStride * nBlockSize;
            wIndex3 = dwCenter - wEdgeStride / 2 + wIndexStride * nBlockSize;
            *(pIndices + dwIndex++) = wIndex1;//dwCenter;
            *(pIndices + dwIndex++) = wIndex2;//dwCenter + wEdgeStride * 0.5 + wIndexStride * nBlockSize;
            *(pIndices + dwIndex++) = wIndex3;//dwCenter - wEdgeStride * 0.5 + wIndexStride * nBlockSize;

            //处理左上边的三角形
            if (wCol > 0)
            {
                wIndex1 = dwBottomLeft + wCol - wIndexStride * nBlockSize;                          
                wIndex2 = dwBottomLeft + wCol + wIndexStride - wIndexStride * nBlockSize;
                wIndex3 = dwBottomLeft + wCol;
                *(pIndices + dwIndex++) = wIndex1;//dwBottomLeft + wCol - wIndexStride * nBlockSize;                          
                *(pIndices + dwIndex++) = wIndex2;//dwBottomLeft + wCol + wIndexStride - wIndexStride * nBlockSize;
                *(pIndices + dwIndex++) = wIndex3;//dwBottomLeft + wCol;
            }

            //处理右上边的三角形
            if (wCol < nGridPerRow - wEdgeStride)
            {
                wIndex1 = dwBottomLeft + wCol + wEdgeStride - wIndexStride * nBlockSize;
                wIndex2 = dwBottomLeft + wCol + wEdgeStride;
                wIndex3 = dwBottomLeft + wCol + wEdgeStride - wIndexStride - wIndexStride * nBlockSize;
                *(pIndices + dwIndex++) = wIndex1;//dwBottomLeft + wCol - wIndexStride * nBlockSize;                 
                *(pIndices + dwIndex++) = wIndex2;//dwBottomLeft + wCol + wIndexStride;
                *(pIndices + dwIndex++) = wIndex3;//dwBottomLeft + wCol;
            }

            //处理内边的三角形
            int nTriangleCount = (int)pow(2.0, nDeltaBottom) - 2;
            for (int i = 0 ; i < nTriangleCount / 2; i++)
            {
                wIndex1 = dwBottomLeft + wCol + (i + 1) * wIndexStride - wIndexStride * nBlockSize;
                wIndex2 = dwBottomLeft + wCol + (i + 2) * wIndexStride - wIndexStride * nBlockSize;
                wIndex3 = dwBottomLeft + wCol;
                *(pIndices + dwIndex++) = wIndex1;//wCol + wEdgeStride + wIndexStride * nBlockSize;                          
                *(pIndices + dwIndex++) = wIndex2;//wCol + wEdgeStride + wIndexStride * nBlockSize - wIndexStride;
                *(pIndices + dwIndex++) = wIndex3;//wCol + wEdgeStride;
            }

            for (int i = nTriangleCount / 2; i < nTriangleCount; i++)
            {
                wIndex1 = dwBottomLeft + wCol + (i + 1) * wIndexStride - wIndexStride * nBlockSize;
                wIndex2 = dwBottomLeft + wCol + (i + 2) * wIndexStride - wIndexStride * nBlockSize;
                wIndex3 = dwBottomLeft + wCol + wEdgeStride;
                *(pIndices + dwIndex++) = wIndex1;//wCol + wEdgeStride + wIndexStride * nBlockSize;                          
                *(pIndices + dwIndex++) = wIndex2;//wCol + wEdgeStride + wIndexStride * nBlockSize - wIndexStride;
                *(pIndices + dwIndex++) = wIndex3;//wCol + wEdgeStride;
            }

            //i++;   //调试用
        }
    }


    //处理左边
    if (nDeltaLeft == 0)
    {
        for (u16 wRow = 0; wRow < wRealCell; wRow++)
        {
            if (wRow == 0)
            {
                wIndex1 = dwTopLeft;
                wIndex2 = dwTopLeft + wIndexStride + wIndexStride * nBlockSize;
                wIndex3 = dwTopLeft + wIndexStride * nBlockSize;
                *(pIndices + dwIndex++) = wIndex1;//dwTopLeft;
                *(pIndices + dwIndex++) = wIndex2;//dwTopLeft + wIndexStride + wIndexStride * nBlockSize;
                *(pIndices + dwIndex++) = wIndex3;//dwTopLeft + wIndexStride * nBlockSize;
            }
            else if (wRow == wRealCell - 1)
            {
                wIndex1 = dwTopLeft + wRow * wIndexStride * nBlockSize;
                wIndex2 = dwTopLeft + wIndexStride + wRow * wIndexStride * nBlockSize;
                wIndex3 = dwBottomLeft;
                *(pIndices + dwIndex++) = wIndex1;//dwTopLeft + wRow * wIndexStride * nBlockSize;
                *(pIndices + dwIndex++) = wIndex2;//dwTopLeft + wIndexStride + wRow * wIndexStride * nBlockSize;
                *(pIndices + dwIndex++) = wIndex3;//dwBottomLeft;
            }
            else
            {
                wIndex1 = dwTopLeft + wRow * wIndexStride * nBlockSize;
                wIndex2 = dwTopLeft + wIndexStride + wRow * wIndexStride * nBlockSize;
                wIndex3 = dwTopLeft + wIndexStride + (wRow + 1) * wIndexStride * nBlockSize;
                *(pIndices + dwIndex++) = wIndex1;//dwTopLeft + wRow * wIndexStride * nBlockSize;
                *(pIndices + dwIndex++) = wIndex2;//dwTopLeft + wIndexStride + wRow * wIndexStride * nBlockSize;
                *(pIndices + dwIndex++) = wIndex3;//dwTopLeft + wIndexStride + (wRow + 1) * wIndexStride * nBlockSize;

                wIndex1 = dwTopLeft + wRow * wIndexStride * nBlockSize;
                wIndex2 = dwTopLeft + wIndexStride + (wRow + 1) * wIndexStride * nBlockSize;
                wIndex3 = dwTopLeft + (wRow + 1) * wIndexStride * nBlockSize;
                *(pIndices + dwIndex++) = wIndex1;//dwTopLeft + wRow * wIndexStride * nBlockSize;
                *(pIndices + dwIndex++) = wIndex2;//dwTopLeft + wIndexStride + (wRow + 1) * wIndexStride * nBlockSize;
                *(pIndices + dwIndex++) = wIndex3;//dwTopLeft + (wRow + 1) * wIndexStride * nBlockSize;
            }

        }
    }
    else
    {
        u16 wEdgeStride = (u16)pow(2.0, nDeltaLeft) * wIndexStride;
        for (u16 wRow = 0; wRow < nGridPerRow; wRow += wEdgeStride)
        {
            if (nDeltaLeft == 1)
            {
                int nDebug = 0;
            }
            u16 wCenter = dwTopLeft + wIndexStride + (wRow + wEdgeStride / 2) * nBlockSize;

            //处理左边的三角形
            wIndex1 = wCenter;
            wIndex2 = wCenter - wIndexStride + wEdgeStride / 2 * nBlockSize;
            wIndex3 = wCenter - wIndexStride - wEdgeStride / 2 * nBlockSize;
            *(pIndices + dwIndex++) = wIndex1;//wCenter;                          
            *(pIndices + dwIndex++) = wIndex2;//wCenter - wIndexStride + wEdgeStride * 0.5 * nBlockSize;
            *(pIndices + dwIndex++) = wIndex3;//wCenter - wIndexStride - wEdgeStride * 0.5 * nBlockSize;

            //处理上边的三角形
            if (wRow > 0)
            {
                wIndex1 = dwTopLeft + wIndexStride + (wRow + wIndexStride) * nBlockSize;
                wIndex2 = dwTopLeft + wRow * nBlockSize;
                wIndex3 = dwTopLeft + wRow * nBlockSize + wIndexStride;
                *(pIndices + dwIndex++) = wIndex1;//dwTopLeft + wIndexStride + (wRow + wIndexStride) * nBlockSize;                          
                *(pIndices + dwIndex++) = wIndex2;//dwTopLeft + wRow * nBlockSize;
                *(pIndices + dwIndex++) = wIndex3;//dwTopLeft + wRow * nBlockSize + wIndexStride;
            }

            //处理下边的三角形
            if (wRow < nGridPerRow - wEdgeStride)
            {
                wIndex1 = dwTopLeft + wIndexStride + (wRow + wEdgeStride - wIndexStride) * nBlockSize;
                wIndex2 = dwTopLeft + wIndexStride + (wRow + wEdgeStride) * nBlockSize;
                wIndex3 = dwTopLeft + (wRow + wEdgeStride) * nBlockSize;
                *(pIndices + dwIndex++) = wIndex1;//dwTopLeft + wIndexStride + (wRow + wEdgeStride - wIndexStride) * nBlockSize;                          
                *(pIndices + dwIndex++) = wIndex2;//dwTopLeft + wIndexStride + (wRow + wEdgeStride) * nBlockSize;
                *(pIndices + dwIndex++) = wIndex3;//dwTopLeft + (wRow + wEdgeStride) * nBlockSize;
            }

            //处理内边的三角形
            int nTriangleCount = (int)pow(2.0, nDeltaLeft) - 2;
            for (int i = 0; i < nTriangleCount / 2; i++)
            {
                wIndex1 = dwTopLeft + wIndexStride + (wRow + (i + 1) * wIndexStride) * nBlockSize;
                wIndex2 = dwTopLeft + wIndexStride + (wRow + (i + 2) * wIndexStride) * nBlockSize;
                wIndex3 = dwTopLeft + wRow * nBlockSize;
                *(pIndices + dwIndex++) = wIndex1;//wCol + wEdgeStride + wIndexStride * nBlockSize;                          
                *(pIndices + dwIndex++) = wIndex2;//wCol + wEdgeStride + wIndexStride * nBlockSize - wIndexStride;
                *(pIndices + dwIndex++) = wIndex3;//wCol + wEdgeStride;
            }

            for (int i = nTriangleCount / 2; i < nTriangleCount; i++)
            {
                wIndex1 = dwTopLeft + wIndexStride + (wRow + (i + 1) * wIndexStride) * nBlockSize;
                wIndex2 = dwTopLeft + wIndexStride + (wRow + (i + 2) * wIndexStride) * nBlockSize;
                wIndex3 = dwTopLeft + (wRow + wEdgeStride) * nBlockSize;
                *(pIndices + dwIndex++) = wIndex1;//wCol + wEdgeStride + wIndexStride * nBlockSize;                          
                *(pIndices + dwIndex++) = wIndex2;//wCol + wEdgeStride + wIndexStride * nBlockSize - wIndexStride;
                *(pIndices + dwIndex++) = wIndex3;//wCol + wEdgeStride;
            }

        }

    }


    //处理边界里面的三角形

    for (u16 wRow = 1; wRow < wRealCell - 1; wRow++)
    {
        for (u16 wCol = 1; wCol < wRealCell - 1; wCol++)
        {
            wIndex1 = dwTopLeft + wCol * wIndexStride + wRow * wIndexStride * nBlockSize;
            wIndex2 = dwTopLeft + (wCol + 1) * wIndexStride + wRow * wIndexStride * nBlockSize;
            wIndex3 = dwTopLeft + (wCol + 1) * wIndexStride + (wRow + 1) * wIndexStride * nBlockSize;
            *(pIndices + dwIndex++) = wIndex1; //dwTopLeft + wCol * wIndexStride + wRow * wIndexStride * nBlockSize;
            *(pIndices + dwIndex++) = wIndex2; //dwTopLeft + (wCol + 1) * wIndexStride + wRow * wIndexStride * nBlockSize;
            *(pIndices + dwIndex++) = wIndex3; //dwTopLeft + (wCol + 1) * wIndexStride + (wRow + 1) * wIndexStride * nBlockSize;

            wIndex1 = dwTopLeft + wCol * wIndexStride + wRow * wIndexStride * nBlockSize;
            wIndex2 = dwTopLeft + (wCol + 1) * wIndexStride + (wRow + 1) * wIndexStride * nBlockSize;
            wIndex3 = dwTopLeft + wCol * wIndexStride + (wRow + 1) * wIndexStride * nBlockSize;
            *(pIndices + dwIndex++) = wIndex1;  //dwTopLeft + wCol * wIndexStride + wRow * wIndexStride * nBlockSize;
            *(pIndices + dwIndex++) = wIndex2;  //dwTopLeft + (wCol + 1) * wIndexStride + (wRow + 1) * wIndexStride * nBlockSize;
            *(pIndices + dwIndex++) = wIndex3;  //dwTopLeft + wCol * wIndexStride + (wRow + 1) * wIndexStride * nBlockSize;
        }
    }

    //处理完毕

    pIndexBuffer->Unlock();

    pIndexBuffer->SetTotalIndex(dwIndex);

    CLODInfo* pLODInfo = new CLODInfo(nSelfLOD, nDeltaLeft, nDeltaTop, nDeltaRight, nDeltaBottom);
    pLODInfo->SetIndexBuffer(pIndexBuffer);

    m_mapLODInfo.insert(make_pair(m_nLODInfoCount++, pLODInfo));

    return pLODInfo;
}
