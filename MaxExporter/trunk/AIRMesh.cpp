#include "AIRMesh.h"
#include "AIRExporter.h"

CAIRMesh::CAIRMesh(void) : /*m_pNode(NULL), m_pMesh(NULL), */m_bStatic(false), m_pMaxMesh(NULL)
, m_nFacesCount(0)
{
    
}

CAIRMesh::~CAIRMesh(void)
{
}

void CAIRMesh::Clear()
{
    if (m_pMaxMesh)
    {
        m_pMaxMesh->Clear();
        delete m_pMaxMesh;
        m_pMaxMesh = NULL;
    }
    
    for (SUBMESH_ITER it = m_vtSubMeshes.begin(); it != m_vtSubMeshes.end(); it++)
    {
        CAIRSubMesh* pSubMesh = *it;
        pSubMesh->Clear();
        delete pSubMesh;
    }
    m_vtSubMeshes.clear();
}

bool CAIRMesh::Create(CAIRSkeleton* pSkeleton, CMaxNode* pMaxNode, int maxBoneCount, float weightThreshold)
{
    //if (maxBoneCount == 0)
    //{
    //    m_bStatic = true;
    //}
    int nMeshCount = 0;  //网格数量
    CMaxInterface* pMaxInterface = AIREXPORTER->GetMaxInterface();
    //for (int nNodeID = 0; nNodeID < vtNodes.size(); nNodeID++)
    //{
    //    CMaxNode* pNode = vtNodes[nNodeID];//AIREXPORTER->GetNode(nNodeID);

    if (!pMaxNode->IsMesh())
    {
        AIREXPORTER->ShowErrorMsg("MAX的结点不含有网格！", __FILE__, __LINE__);
        return false;
    }

    if (m_pMaxMesh)
    {
        m_pMaxMesh->Clear();
        delete m_pMaxMesh;
        m_pMaxMesh = NULL;
    }
    m_pMaxMesh = new CMaxMesh;

    TimeValue time;
    
    time = pMaxInterface->GetTime();

    if (!m_pMaxMesh->Create(pMaxNode->GetINode(), time))
    {
        delete m_pMaxMesh;
        return false;
    }

    if (pSkeleton)
    {
        m_strSkeleton = pSkeleton->GetFileName();
        size_t nPos = m_strSkeleton.find_last_of("/");
        if (nPos == string::npos)
        {
            nPos = m_strSkeleton.find_last_of("\\");
        }

        if (nPos != string::npos)
        {
            m_strSkeleton = m_strSkeleton.c_str() + nPos + 1;
        }
    }
    else
    {
        if (AIREXPORTER->IsShareSkeleton())
        {
            m_strSkeleton = AIREXPORTER->GetSelectedSkeletonFile();
        }
        else
            m_strSkeleton = "none";   //没有的话，写成是none
    }

    //获得栽质数量
    int nMaterialCount = m_pMaxMesh->GetMaterialCount();

    //根据材质生成所有子网格
    char szSubMeshName[256] = { 0 };
    strcpy(szSubMeshName, m_pMaxMesh->GetName().c_str());
    
    for (int i = 0; i < nMaterialCount; i++)
    {
        CAIRSubMesh* pSubMesh = new CAIRSubMesh;

        if (nMaterialCount > 1)
        {
            sprintf(szSubMeshName, "%s_%d", m_pMaxMesh->GetName().c_str(), i);
        }
        if (!pSubMesh->Create(i, m_pMaxMesh->GetSubMeshMapCount(i), m_pMaxMesh, szSubMeshName))
        {
            AIREXPORTER->ShowErrorMsg("Submesh create failed!", __FILE__, __LINE__);
            delete pSubMesh;
            return false;
        }

        m_vtSubMeshes.push_back(pSubMesh);

    }

    if (m_pMaxMesh->GetFaceCount() == 0)
    {
        AIREXPORTER->ShowErrorMsg("Max mesh face num is 0!", __FILE__, __LINE__);
    }

    m_nFacesCount = m_pMaxMesh->GetFaceCount();

    //遍历所有面，根据材质生成新的面和vertex，添加到对应的submesh中
    for (int nFace = 0; nFace < m_pMaxMesh->GetFaceCount(); nFace++)
    {
        //更新progress的信息，暂时不做


        int nMaterialID = m_pMaxMesh->GetFaceMaterialId(nFace);

        if (nMaterialID == -1)
        {
            return false;
        }

        if (nMaterialID < 0 || nMaterialID >= nMaterialCount)
        {
            return false;
        }

        CAIRSubMesh* pSubMesh = m_vtSubMeshes[nMaterialID];

        //第一个顶点
        CAIRVertex* pVertex = m_pMaxMesh->GetAIRVertex(pSkeleton, nFace, 0);

        if (pVertex == NULL)
        {
            continue;
        }


        int nVertexID1 = pSubMesh->AddVertex(pVertex);

        //第二个顶点
        pVertex = m_pMaxMesh->GetAIRVertex(pSkeleton, nFace, 1);

        if (pVertex == NULL)
        {
            continue;
        }
        int nVertexID2 = pSubMesh->AddVertex(pVertex);


        //第三个顶点
        pVertex = m_pMaxMesh->GetAIRVertex(pSkeleton, nFace, 2);
        if (pVertex == NULL)
        {
            continue;
        }
        int nVertexID3 = pSubMesh->AddVertex(pVertex);

        if (nVertexID1 < 0 || nVertexID2 < 0 || nVertexID3 < 0)
        {
            //return false;
            //AIREXPORTER->ShowErrorMsg("有顶点没有纹理坐标！请检查", __FILE__, __LINE__);
            continue;
        }
        
        //这里左手坐标和右手坐标都无关
        //if (AIREXPORTER->IsLeftHand())
        {   //左手坐标要反过来
            if (m_pMaxMesh->IsMirrored())
            {
                pSubMesh->AddFace(nVertexID1, nVertexID2, nVertexID3);
            }
            else
                pSubMesh->AddFace(nVertexID3, nVertexID2, nVertexID1);
                
        }
        /*
        else   //这里是原来的做法 modify by liangairan at 2010-07-21
        {
            if (m_pMaxMesh->IsMirrored())
            {
                pSubMesh->AddFace(nVertexID3, nVertexID2, nVertexID1);
            }
            else
                pSubMesh->AddFace(nVertexID1, nVertexID2, nVertexID3);
        }
        */
        
    }

    SUBMESH_ITER it;
    for (it = m_vtSubMeshes.begin(); it != m_vtSubMeshes.end(); )
    {
        CAIRSubMesh* pSubMesh = *it;
        if (pSubMesh->GetVerticesCount() == 0)
        {
            delete pSubMesh;
            it = m_vtSubMeshes.erase(it);
        }
        else
        {
            ++it;
        }
    }

    for (size_t i = 0; i < m_vtSubMeshes.size(); i++)
    {
        m_vtSubMeshes[i]->CalculateBoundingBox();
    }

    return true;
}

bool CAIRMesh::Create(CMaxNode* pMaxNode, TimeValue tTime, bool bVertexAnimation)
{
    int nMeshCount = 0;  //网格数量
    CMaxInterface* pMaxInterface = AIREXPORTER->GetMaxInterface();
    //for (int nNodeID = 0; nNodeID < vtNodes.size(); nNodeID++)
    //{
    //    CMaxNode* pNode = vtNodes[nNodeID];//AIREXPORTER->GetNode(nNodeID);

    if (!pMaxNode->IsMesh())
    {
        AIREXPORTER->ShowErrorMsg("MAX的结点不含有网格！", __FILE__, __LINE__);
        return false;
    }

    if (m_pMaxMesh)
    {
        m_pMaxMesh->Clear();
        delete m_pMaxMesh;
        m_pMaxMesh = NULL;
    }
    m_pMaxMesh = new CMaxMesh;

    TimeValue time;

    time = pMaxInterface->GetTime();

    if (bVertexAnimation)
    {
        if (!m_pMaxMesh->CreateAnimatedMesh(pMaxNode->GetINode(), tTime))
        {
            delete m_pMaxMesh;
            m_pMaxMesh = NULL;
            return false;
        }
    }
    else
    {
        if (!m_pMaxMesh->Create(pMaxNode->GetINode(), tTime))
        {
            delete m_pMaxMesh;
            m_pMaxMesh = NULL;
            return false;
        }
    }


    m_strSkeleton = "none";   //没有的话，写成是none

    //获得栽质数量
    int nMaterialCount = m_pMaxMesh->GetMaterialCount();

    //根据材质生成所有子网格
    char szSubMeshName[256] = { 0 };
    strcpy(szSubMeshName, m_pMaxMesh->GetName().c_str());

    int nFaceNum = m_pMaxMesh->GetFaceCount();

    if (nFaceNum == 0)
    {
        AIREXPORTER->ShowErrorMsg("Max mesh face num is 0!", __FILE__, __LINE__);
        //CAIRMesh* pMesh = AIREXPORTER->GetExportMesh(m_pMaxMesh->GetName());
        //nFaceNum = pMesh->GetFaceNum();
    }

    for (int i = 0; i < nMaterialCount; i++)
    {
        CAIRSubMesh* pSubMesh = new CAIRSubMesh;

        if (nMaterialCount > 1)
        {
            sprintf(szSubMeshName, "%s_%d", m_pMaxMesh->GetName().c_str(), i);
        }
        if (!pSubMesh->Create(i, m_pMaxMesh->GetSubMeshMapCount(i), m_pMaxMesh, szSubMeshName))
        {
            AIREXPORTER->ShowErrorMsg("Submesh create failed!", __FILE__, __LINE__);
            delete pSubMesh;
            return false;
        }

        m_vtSubMeshes.push_back(pSubMesh);

    }

    //遍历所有面，根据材质生成新的面和vertex，添加到对应的submesh中
    for (int nFace = 0; nFace < nFaceNum; nFace++)
    {
        //更新progress的信息，暂时不做


        int nMaterialID = m_pMaxMesh->GetFaceMaterialId(nFace);

        if (nMaterialID == -1)
        {
            return false;
        }

        if (nMaterialID < 0 || nMaterialID >= nMaterialCount)
        {
            return false;
        }

        CAIRSubMesh* pSubMesh = m_vtSubMeshes[nMaterialID];

        //第一个顶点
        CAIRVertex* pVertex = m_pMaxMesh->GetAIRVertex(NULL, nFace, 0);

        if (pVertex == NULL)
        {
            continue;
        }


        int nVertexID1 = pSubMesh->AddVertex(pVertex);

        //第二个顶点
        pVertex = m_pMaxMesh->GetAIRVertex(NULL, nFace, 1);

        if (pVertex == NULL)
        {
            continue;
        }
        int nVertexID2 = pSubMesh->AddVertex(pVertex);


        //第三个顶点
        pVertex = m_pMaxMesh->GetAIRVertex(NULL, nFace, 2);
        if (pVertex == NULL)
        {
            continue;
        }
        int nVertexID3 = pSubMesh->AddVertex(pVertex);

        if (nVertexID1 < 0 || nVertexID2 < 0 || nVertexID3 < 0)
        {
            //return false;
            //AIREXPORTER->ShowErrorMsg("有顶点没有纹理坐标！请检查", __FILE__, __LINE__);
            continue;
        }

        if (m_pMaxMesh->IsMirrored())
        {
            pSubMesh->AddFace(nVertexID3, nVertexID2, nVertexID1);
        }
        else
            pSubMesh->AddFace(nVertexID1, nVertexID2, nVertexID3);

    }

    SUBMESH_ITER it;
    for (it = m_vtSubMeshes.begin(); it != m_vtSubMeshes.end(); )
    {
        CAIRSubMesh* pSubMesh = *it;
        if (pSubMesh->GetVerticesCount() == 0)
        {
            delete pSubMesh;
            it = m_vtSubMeshes.erase(it);
        }
        else
        {
            ++it;
        }
    }

    for (size_t i = 0; i < m_vtSubMeshes.size(); i++)
    {
        m_vtSubMeshes[i]->CalculateBoundingBox();
    }
    return true;
}

bool CAIRMesh::SaveToFile(FILE* f)
{
    //FILE* f = fopen(strFilename.c_str(), "wt");

    if (f == NULL)
    {
        return false;
    }

    //cut by liangairan at 2010-07-15
    //fprintf(f, "skeletalFile %s\n", m_strSkeleton.c_str());

    for (size_t i = 0; i < m_vtSubMeshes.size(); i++)
    {
        m_vtSubMeshes[i]->SaveToFile(this, f);
    }

    //fclose(f);
    
    return true;
}

bool CAIRMesh::SaveToFileBinary(std::ofstream& fout)
{
    for (size_t i = 0; i < m_vtSubMeshes.size(); i++)
    {
        m_vtSubMeshes[i]->SaveToFile(fout);
    }
    return true;
}

bool CAIRMesh::SavePositionToFile(FILE* f)
{
    if (f == NULL)
    {
        return false;
    }

    //fprintf(f, "skeletalFile %s\n", m_strSkeleton.c_str());

    for (size_t i = 0; i < m_vtSubMeshes.size(); i++)
    {
        m_vtSubMeshes[i]->SavePositionToFile(this, f);
    }
    return true;
}

bool CAIRMesh::SavePositionToFile(std::fstream& fout)
{
    for (size_t i = 0; i < m_vtSubMeshes.size(); i++)
    {
        m_vtSubMeshes[i]->SavePositionToFile(this, fout);
    }
    return true;
}

bool CAIRMesh::CreateVertexAnimation(int nFrame)
{

    CMaxInterface* pMaxInterface = AIREXPORTER->GetMaxInterface();

    TimeValue time;

    time = pMaxInterface->GetTime();

    if (m_pMaxMesh)
    {
        m_pMaxMesh->SetCurrentTime(time);
    }

    //获得栽质数量
    int nMaterialCount = m_pMaxMesh->GetMaterialCount();

    int nFaceNum = m_pMaxMesh->GetFaceCount();

    if (nFaceNum == 0)
    {
        AIREXPORTER->ShowErrorMsg("Max mesh face num is 0!", __FILE__, __LINE__);
        //CAIRMesh* pMesh = AIREXPORTER->GetExportMesh(m_pMaxMesh->GetName());
        //nFaceNum = pMesh->GetFaceNum();
    }

    //遍历所有面，根据材质生成新的面和vertex，添加到对应的submesh中

    AIRTEXCOORD texCoord;
    memset(&texCoord, 0, sizeof(AIRTEXCOORD));
    AIRPOSITION position;
    memset(&position, 0, sizeof(AIRPOSITION));
    DWORD dwColor = 0xffffffff;
    for (int nFace = 0; nFace < nFaceNum; nFace++)
    {
        //更新progress的信息，暂时不做


        int nMaterialID = m_pMaxMesh->GetFaceMaterialId(nFace);

        if (nMaterialID == -1)
        {
            return false;
        }

        if (nMaterialID < 0 || nMaterialID >= nMaterialCount)
        {
            return false;
        }

        CAIRSubMesh* pSubMesh = m_vtSubMeshes[nMaterialID];

        const AIRMESHTRI& face = pSubMesh->GetFace(nFace);
        pSubMesh->SetVertexAnimate(true);
        CMeshKeyFrameData* pKeyFrameData = pSubMesh->CreateAnimationKeyframe(nFrame);

        //第一个顶点
        //CAIRVertex* pVertex = m_pMaxMesh->GetAIRVertex(NULL, nFace, 0);
        if (m_pMaxMesh->GetVertexData(texCoord, position, dwColor, nFace, 0))
        {
            pKeyFrameData->AddPositionIndex(pSubMesh->AddPosition(position), m_pMaxMesh->IsMirrored() ? face.nIndex[0] : face.nIndex[2]);
            pKeyFrameData->AddTexcoordIndex(pSubMesh->AddTexcoord(texCoord), m_pMaxMesh->IsMirrored() ? face.nIndex[0] : face.nIndex[2]);
            pKeyFrameData->AddVertexColor(dwColor, m_pMaxMesh->IsMirrored() ? face.nIndex[0] : face.nIndex[2]);
        }
        else
        {
            continue;
        }

        //第二个顶点
        if (m_pMaxMesh->GetVertexData(texCoord, position, dwColor, nFace, 1))
        {
            pKeyFrameData->AddPositionIndex(pSubMesh->AddPosition(position), face.nIndex[1]);
            pKeyFrameData->AddTexcoordIndex(pSubMesh->AddTexcoord(texCoord), face.nIndex[1]);
            pKeyFrameData->AddVertexColor(dwColor, face.nIndex[1]);
        }
        else
        {
            continue;
        }


        //第三个顶点
        if (m_pMaxMesh->GetVertexData(texCoord, position, dwColor, nFace, 2))
        {
            pKeyFrameData->AddPositionIndex(pSubMesh->AddPosition(position), m_pMaxMesh->IsMirrored() ? face.nIndex[2] : face.nIndex[0]);
            pKeyFrameData->AddTexcoordIndex(pSubMesh->AddTexcoord(texCoord), m_pMaxMesh->IsMirrored() ? face.nIndex[2] : face.nIndex[0]);
            pKeyFrameData->AddVertexColor(dwColor, m_pMaxMesh->IsMirrored() ? face.nIndex[2] : face.nIndex[0]);
        }
        else
        {
            continue;
        }
    }

    return true;
}

bool CAIRMesh::SaveVertexAnimation(FILE* f)
{
    for (size_t i = 0; i < m_vtSubMeshes.size(); i++)
    {
        m_vtSubMeshes[i]->SaveVertexAnimation(f);
    }
    return true;
}

bool CAIRMesh::SaveVertexAnimationBin(std::fstream& fout)
{
    for (size_t i = 0; i < m_vtSubMeshes.size(); i++)
    {
        m_vtSubMeshes[i]->SaveVertexAnimationBin(fout);
    }
    return true;
}
