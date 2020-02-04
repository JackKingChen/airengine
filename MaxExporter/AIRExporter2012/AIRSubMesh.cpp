#include "AIRSubMesh.h"
#include "AIRMaterial.h"
#include "AIRMesh.h"
#include "AIRExporter2012.h"

CMeshKeyFrameData::CMeshKeyFrameData(int nFrame) : m_nFrameIndex(nFrame)
{

}

CMeshKeyFrameData::~CMeshKeyFrameData()
{

}

void CMeshKeyFrameData::Clear()
{
    m_vtPositionIndices.clear();
    m_vtTexcoordIndices.clear();
    m_vtColors.clear();
}

bool CMeshKeyFrameData::SaveToFile(std::fstream& fout)
{
    //fout.write((const char*)&m_nFrameIndex, sizeof(int));
    //fout.write((const char*)&m_vtPositionIndices[0], sizeof(int) * m_vtPositionIndices.size());
    //fout.write((const char*)&m_vtTexcoordIndices[0], sizeof(int) * m_vtTexcoordIndices.size());
    //fout.write((const char*)&m_vtColors[0], sizeof(int) * m_vtColors.size());
    size_t nVerticesNum = m_vtPositionIndices.size();
    for (size_t i = 0; i < nVerticesNum; ++i)
    {
        fout.write((const char*)&m_vtPositionIndices[i], sizeof(WORD));
        fout.write((const char*)&m_vtTexcoordIndices[i], sizeof(WORD));
        fout.write((const char*)&m_vtColors[i], sizeof(DWORD));
    }
    return true;
}

bool CMeshKeyFrameData::SaveToFile(FILE* f)
{
    fprintf(f, "        frame %d {\n", m_nFrameIndex);
    for (size_t i = 0; i < m_vtPositionIndices.size(); ++i)
    {
        fprintf(f, "            vertex %d position %d texcoord %d color %u\n", i, m_vtPositionIndices[i], m_vtTexcoordIndices[i], m_vtColors[i]);
    }
    fprintf(f, "        }\n");
    return true;
}

void CMeshKeyFrameData::AddPositionIndex(WORD nPosIndex, WORD nVertexIndex)
{
    //m_vtPositionIndices.push_back(nPosIndex);

    int nOld = m_vtPositionIndices[nVertexIndex];
    m_vtPositionIndices[nVertexIndex] = nPosIndex;
    if (nOld != 65535)
    {
        assert(nOld == nPosIndex);
    }
}

void CMeshKeyFrameData::AddTexcoordIndex(WORD nTexcoordIndex, WORD nVertexIndex)
{
    //m_vtTexcoordIndices.push_back(nTexcoordIndex);
    WORD nOld = m_vtTexcoordIndices[nVertexIndex];
    m_vtTexcoordIndices[nVertexIndex] = nTexcoordIndex;
    if (nOld != 65535)
    {
        assert(nOld == nTexcoordIndex);
    }
}

void CMeshKeyFrameData::AddVertexColor(DWORD dwColor, WORD nVertexIndex)
{
    //m_vtColors.push_back(dwColor);
    m_vtColors[nVertexIndex] = dwColor;

}

void CMeshKeyFrameData::SetVerticesNum(size_t nNum)
{
    m_vtPositionIndices.resize(nNum);
    m_vtTexcoordIndices.resize(nNum);
    m_vtColors.resize(nNum);

    for (size_t i = 0; i < nNum; ++i)
    {
        m_vtPositionIndices[i] = 65535;
        m_vtTexcoordIndices[i] = 65535;
        m_vtColors[i] = 0;
    }
}

CAIRSubMesh::CAIRSubMesh(void) : m_nMaterialID(-1), m_nMapCount(0), m_pMaterial(NULL), m_nTotalWeightsNum(0)
, m_bVertexAnimate(false)
{
}

CAIRSubMesh::~CAIRSubMesh(void)
{
    Clear();
}

bool CAIRSubMesh::AddFace(int nVertexid1, int nVertexid2, int nVertexid3)
{
    AIRMESHTRI face;
    face.nIndex[0] = nVertexid1;
    face.nIndex[1] = nVertexid2;
    face.nIndex[2] = nVertexid3;

    m_vtFaces.push_back(face);

    return true;
}

int CAIRSubMesh::AddVertex(CAIRVertex* pVertex)
{
    //假如顶点坐标的UV组数与所用的纹理数不相同，报错
    if (pVertex->GetTexCoordinateCount() == 0  /*!= m_nMapCount*/)
    {
        //MessageBox()
        return -1;
    }

    int nVerticeCount = (int)m_vertices.size();

    for (int i = 0; i < nVerticeCount; i++)
    {
        CAIRVertex* pV = m_vertices[i];
        if (*pV == *pVertex)   //如果该顶点已经存在，返回其下标
        {
            delete pVertex;    //删除该顶点
            return i;
        }
    }

    //不存在，添加到数组中
    m_vertices.push_back(pVertex);

    pVertex->SetWeightOffset(m_nTotalWeightsNum);
    m_nTotalWeightsNum += pVertex->GetInfluenceNum();
    return (int)m_vertices.size() - 1;
}

CAIRVertex* CAIRSubMesh::GetVertex(size_t nIndex)
{
    if (nIndex < 0 || nIndex >= m_vertices.size())
    {
        return NULL;
    }

    return m_vertices[nIndex];
}

void CAIRSubMesh::Clear()
{
    // destroy all vertex candidates stored in this submesh candidate
    int vertexId;
    for(vertexId = 0; vertexId < m_vertices.size(); vertexId++)
    {
        delete m_vertices[vertexId];
    }

    if (m_pMaterial)
    {
        delete m_pMaterial;
        m_pMaterial = NULL;
    }
    m_vertices.clear();

    for (size_t i = 0; i < m_vtAnimationDatas.size(); ++i)
    {
        m_vtAnimationDatas[i]->Clear();
        delete m_vtAnimationDatas[i];
    }
    m_vtAnimationDatas.clear();

    m_vtPositions.clear();
    m_vtTexcoords.clear();
}

void CAIRSubMesh::CalculateBoundingBox()
{
    m_boundingBox.maxX = m_boundingBox.maxY = m_boundingBox.maxZ = -9999.0f;
    m_boundingBox.minX = m_boundingBox.minY = m_boundingBox.minZ = 9999.0f;
    for (int i = 0; i < m_vertices.size(); i++)
    {
        CAIRVertex* pVertex = m_vertices[i];
        AIRPOSITION pos;
        pVertex->GetPosition(pos);
        m_boundingBox.maxX = max(pos.x, m_boundingBox.maxX);
        m_boundingBox.maxY = max(pos.y, m_boundingBox.maxY);
        m_boundingBox.maxZ = max(pos.z, m_boundingBox.maxZ);

        m_boundingBox.minX = min(pos.x, m_boundingBox.minX);
        m_boundingBox.minY = min(pos.y, m_boundingBox.minY);
        m_boundingBox.minZ = min(pos.z, m_boundingBox.minZ);
    }
}

bool CAIRSubMesh::Create(int nMaterialID, int nMapCount, CMaxMesh* pMesh, const char* szMeshName)
{
    Clear();
    m_nMaterialID = nMaterialID;
    m_nMapCount = nMapCount;
    m_pMaxMesh = pMesh;

    m_strName = szMeshName;

    m_pMaterial = new CAIRMaterial;
    if (!m_pMaterial->Create(m_pMaxMesh->GetMaterial(nMaterialID)))
    {
        delete m_pMaterial;
        return false;
    }

    return true;
}

bool CAIRSubMesh::SaveToFile(CAIRMesh* pMesh, FILE* f)
{
    if (GetVerticesCount() == 0)
    {
        return true;
    }
    fprintf(f, "%s {\n", STR_MESH_HEADER);

    //string& strMeshName = mesh.GetName();
    fprintf(f, "    name %s\n", m_strName.c_str());
    //CAIRMaterial* pMaterial = m_pMaxMesh->GetMaterial(m_nMaterialID);
    const std::string& mtlName = m_pMaterial->GetName();

    std::string strMtlFile = AIREXPORTER->GetMaterialFileName();
    if (strMtlFile.length() > 0 && AIREXPORTER->IsExportMaterial())
    {
        fprintf(f, "    material %s/%s\n", strMtlFile.c_str(), mtlName.c_str());
    }
    else
    {
        fprintf(f, "    material %s\n", mtlName.c_str());
    }

    

    //if (m_bVertexAnimate)
    //{
    //    int nVerticesNum = m_vtPositions.size();
    //    fprintf(f, "    %s %d\n", STR_NUM_VERTS, nVerticesNum);
    //    for (int i = 0; i < nVerticesNum; ++i)
    //    {
    //        const AIRPOSITION& position = m_vtPositions[i];
    //        fprintf(f, "    vertex %d position(%f, %f, %f)\n", i, position.x, position.y, position.z);
    //    }

    //    int nTexcoordsNum = m_vtTexcoords.size();
    //    fprintf(f, "    num_texcoords %d\n", nTexcoordsNum);
    //    for (int i = 0; i < nTexcoordsNum; ++i)
    //    {
    //        const AIRTEXCOORD& texcoord = m_vtTexcoords[i];
    //        fprintf(f, "    vertex %d texcoord(%f, %f)\n", i, texcoord.u, texcoord.v);
    //    }
    //}
    //else
    {
        fprintf(f, "    %s %d\n", STR_NUM_VERTS, GetVerticesCount());
        for (int i = 0; i < GetVerticesCount(); i++)
        {
            CAIRVertex* pVert = GetVertex(i);
            //fprintf(f, "    vertex %d {\n", i);
            AIRPOSITION pos;
            pVert->GetPosition(pos);
            CAIRVertex::TextureCoordinate coord;
            pVert->GetTexCoordinate(0, coord);
            fprintf(f, "    vertex %d position (%f, %f, %f) tex_coordinate (%f, %f) color(%u) %d %d \n", i, pos.x, pos.y, pos.z, coord.u, coord.v,
                pVert->GetColor(), pVert->GetWeightOffset(), pVert->GetInfluenceNum());
            //fprintf(f, "        map_num %d\n", pVert->GetTexCoordinateCount());
            /*
            for (int j = 0; j < pVert->GetTexCoordinateCount(); j++)
            {
            fprintf(f, "        map %d {\n", j);
            CAIRVertex::TextureCoordinate coord;
            pVert->GetTexCoordinate(j, coord);
            fprintf(f, "            coordinate(%f, %f)\n", coord.u, coord.v);
            fprintf(f, "        }\n");
            }
            */
            //fprintf(f, "    }\n");
        }
    }
    

    fprintf(f, "    %s %d\n", STR_NUM_TRIANGLES, GetFaceCount());
    for (int i = 0; i < GetFaceCount(); i++)
    {
        const AIRMESHTRI& Tri = m_vtFaces[i];
        fprintf(f, "    %s %d (%d %d %d)\n", STR_MESH_TRIANGLE, i, Tri.nIndex[0], Tri.nIndex[1], Tri.nIndex[2]);
    }

    //const AIRMATERIAL& material = mesh.GetMaterial();
    /*
    fprintf(f, "    %s color %d ambient(%f, %f, %f) difusse(%f, %f, %f) specular(%f, %f, %f) shiness %f\n", STR_MATERIAL, 
        mesh.m_material.Ambient[0], mesh.m_material.Ambient[1], mesh.m_material.Ambient[2], 
        mesh.m_material.Diffuse[0], mesh.m_material.Diffuse[1], mesh.m_material.Diffuse[2],
        mesh.m_material.Specular[0], mesh.m_material.Specular[1], mesh.m_material.Specular[2],
        mesh.m_material.Shiness);
        */

    //const AIRBOUNDINGBOX& boundingBox = mesh.GetBoundingBox();

    if (!m_bVertexAnimate)
    {
        fprintf(f, "    num_weights %d\n", m_nTotalWeightsNum);
        int nWeights = 0;
        for (size_t i = 0; i < m_vertices.size(); i++)
        {
            CAIRVertex* pVertex = m_vertices[i];
            for (size_t j = 0; j < pVertex->GetInfluenceNum(); j++)
            {
                CAIRVertex::Influence influence = pVertex->GetInfluence(j);
                fprintf(f, "    weight %d %d %f\n", nWeights++, influence.boneId, influence.weight);
            }
        }
    }
    

    fprintf(f, "    %s max(%f, %f, %f) min(%f, %f, %f)\n", STR_MESH_BOUNDINGBOX, m_boundingBox.maxX, m_boundingBox.maxY, m_boundingBox.maxZ,
        m_boundingBox.minX, m_boundingBox.minY, m_boundingBox.minZ);
    //fprintf()

    fprintf(f, "}\n\n");
    return true;
}

bool CAIRSubMesh::SaveToFile(std::ofstream& fout)
{
    //fout<<(int)m_strName.length();
    //fout<<m_strName.c_str();
    int nLength = m_strName.length();
    fout.write((const char*)&nLength, sizeof(int));
    fout.write(m_strName.c_str(), m_strName.length());
    std::string strMtlFile = AIREXPORTER->GetMaterialFileName();
    const std::string& mtlName = m_pMaterial->GetName();
    
    if (strMtlFile.length() > 0 && AIREXPORTER->IsExportMaterial())
    {
        //fout<<(int)strMtlFile.length();
        //fout<<strMtlFile.c_str();
        char szTemp[512] = { 0 };
        sprintf(szTemp, "%s/%s", strMtlFile.c_str(), mtlName.c_str());
        nLength = strlen(szTemp);
        fout.write((const char*)&nLength, sizeof(int));
        fout.write(szTemp, nLength);
    }
    else
    {
        char szTemp[512] = { 0 };
        strcpy(szTemp, "TemplateMaterial.xml/Model");
        nLength = strlen(szTemp);
        fout.write((const char*)&nLength, sizeof(int));
        fout.write(szTemp, nLength);
    }

    //输出顶点数
    //fout<<(int)GetVerticesCount();

    //if (m_bVertexAnimate)
    //{
    //    int nVerticesCount = m_vtPositions.size();
    //    fout.write((const char*)&nVerticesCount, sizeof(int));

    //    fout.write((const char*)&m_vtPositions[0], sizeof(AIRPOSITION) * nVerticesCount);

    //    int nTexcoordsNum = m_vtTexcoords.size();
    //    fout.write((const char*)&m_vtTexcoords[0], sizeof(AIRTEXCOORD) * nTexcoordsNum);
    //}
    //else
    {
        int nVerticesCount = GetVerticesCount();
        fout.write((const char*)&nVerticesCount, sizeof(int));

        for (int i = 0; i < GetVerticesCount(); i++)
        {
            CAIRVertex* pVert = GetVertex(i);
            //fprintf(f, "    vertex %d {\n", i);
            AIRPOSITION pos;
            pVert->GetPosition(pos);
            //输出坐标
            //fout<<pos.x;
            //fout<<pos.y;
            //fout<<pos.z;
            fout.write((const char*)&pos, sizeof(AIRPOSITION));
 
            CAIRVertex::TextureCoordinate coord;
            pVert->GetTexCoordinate(0, coord);
            //fout<<coord.u;
            //fout<<coord.v;
            fout.write((const char*)&coord, sizeof(CAIRVertex::TextureCoordinate));

            //写入颜色
            DWORD dwColor = pVert->GetColor();
            fout.write((const char*)&dwColor, sizeof(DWORD));

            //输出受影响的权值
            //fout<<pVert->GetWeightOffset();
            //fout<<pVert->GetInfluenceNum();
            //int nWeightOffset = pVert->GetWeightOffset();
            //int nWeightInfluence = pVert->GetInfluenceNum();
			unsigned char boneID[4] = { 0 };
			float fWeights[4] = { 0 };

			for (size_t j = 0; j < pVert->GetInfluenceNum(); j++)
			{
				CAIRVertex::Influence influence = pVert->GetInfluence(j);
				boneID[j] = influence.boneId;
				fWeights[j] = influence.weight;
				//fout.write((const char*)&influence.boneId, sizeof(int));
				//fout.write((const char*)&influence.weight, sizeof(float));
				//fprintf(f, "    weight %d %d %f\n", nWeights++, influence.boneId, influence.weight);
			}

			fout.write((const char*)boneID, sizeof(boneID));
			fout.write((const char*)fWeights, sizeof(fWeights));
            //fout.write((const char*)&nWeightOffset, sizeof(int));
            //fout.write((const char*)&nWeightInfluence, sizeof(int));
        }

        //输出三角形面
        //fout<<GetFaceCount();
        int nFaceCount = GetFaceCount();
        fout.write((const char*)&nFaceCount, sizeof(int));
        for (int i = 0; i < GetFaceCount(); ++i)
        {
            const AIRMESHTRI& Tri = m_vtFaces[i];
            fout.write((const char*)&Tri, sizeof(AIRMESHTRI));
            //fout<<Tri.nIndex[0];
            //fout<<Tri.nIndex[1];
            //fout<<Tri.nIndex[2];
        }

        //输出权重
        //fout<<m_nTotalWeightsNum;
		/*
        fout.write((const char*)&m_nTotalWeightsNum, sizeof(int));
        for (size_t i = 0; i < m_vertices.size(); i++)
        {
            CAIRVertex* pVertex = m_vertices[i];
            for (size_t j = 0; j < pVertex->GetInfluenceNum(); j++)
            {
                CAIRVertex::Influence influence = pVertex->GetInfluence(j);
                //fout<<influence.boneId;
                //fout<<influence.weight;
                fout.write((const char*)&influence.boneId, sizeof(int));
                fout.write((const char*)&influence.weight, sizeof(float));
                //fprintf(f, "    weight %d %d %f\n", nWeights++, influence.boneId, influence.weight);
            }
        }
		*/
    }
    

    //输出boundingbox
    //fout<<m_boundingBox.minX;
    //fout<<m_boundingBox.minY;
    //fout<<m_boundingBox.minZ;

    //fout<<m_boundingBox.maxX;
    //fout<<m_boundingBox.maxY;
    //fout<<m_boundingBox.maxZ;
    fout.write((const char*)&m_boundingBox, sizeof(AIRBOUNDINGBOX));

    return true;
}

bool CAIRSubMesh::SavePositionToFile(CAIRMesh* pMesh, FILE* f)
{
    if (GetVerticesCount() == 0)
    {
        return true;
    }
    fprintf(f, "%s {\n", STR_MESH_HEADER);

    //string& strMeshName = mesh.GetName();
    fprintf(f, "    name %s\n", m_strName.c_str());
    //CAIRMaterial* pMaterial = m_pMaxMesh->GetMaterial(m_nMaterialID);

    //fprintf(f, "    %s %d\n", STR_NUM_VERTS, GetVerticesCount());
    if (GetVerticesCount() == 0)
    {
        AIREXPORTER->ShowErrorMsg("导出顶点动画顶点数为0！", __FILE__, __LINE__);
        return false;
    }

    {
        for (size_t i = 0; i < GetVerticesCount(); i++)
        {
            CAIRVertex* pVert = GetVertex(i);
            //fprintf(f, "    vertex %d {\n", i);
            AIRPOSITION pos;
            pVert->GetPosition(pos);
            CAIRVertex::TextureCoordinate coord;
            pVert->GetTexCoordinate(0, coord);
            fprintf(f, "    vertex %d position (%f, %f, %f) tex_coordinate (%f, %f) color(%u) %d %d \n", i, pos.x, pos.y, pos.z, coord.u, coord.v,
                pVert->GetColor(), pVert->GetWeightOffset(), pVert->GetInfluenceNum());

        }
    }
    

    fprintf(f, "}\n\n");
    return true;
}

bool CAIRSubMesh::SavePositionToFile(CAIRMesh* pMesh, std::fstream& fout)
{
    //fout<<(int)m_strName.length();
    //fout<<m_strName.c_str();
    fout.write(m_strName.c_str(), 256);
    int nVerticesNum = GetVerticesCount();
    if (nVerticesNum == 0)
    {
        char szTemp[256] = { 0 };
        sprintf(szTemp, "submesh[%s]导出顶点动画顶点数为0！", m_strName.c_str());
        AIREXPORTER->ShowErrorMsg(szTemp, __FILE__, __LINE__);
        return false;
    }
    fout.write((const char*)&nVerticesNum, sizeof(int));
    //fout<<GetVerticesCount();

    for (size_t i = 0; i < GetVerticesCount(); ++i)
    {
        CAIRVertex* pVert = GetVertex(i);
        //fprintf(f, "    vertex %d {\n", i);
        AIRPOSITION pos;
        pVert->GetPosition(pos);
        CAIRVertex::TextureCoordinate coord;
        pVert->GetTexCoordinate(0, coord);
        //fout<<pos.x;
        //fout<<pos.y;
        //fout<<pos.z;
        //fout<<coord.u;
        //fout<<coord.v;
        fout.write((const char*)&pos, sizeof(AIRPOSITION));
        DWORD dwColor = pVert->GetColor();
        fout.write((const char*)&dwColor, sizeof(DWORD));
        fout.write((const char*)&coord, sizeof(CAIRVertex::TextureCoordinate));
    }
    return true;
}

int CAIRSubMesh::AddPosition(const AIRPOSITION& position)
{
    size_t num = m_vtPositions.size();
    for (size_t i = 0; i < num; ++i)
    {
        if (m_vtPositions[i] == position)
        {
            return i;
        }
    }

    m_vtPositions.push_back(position);
    return num;
}

int CAIRSubMesh::AddTexcoord(const AIRTEXCOORD& texCoord)
{

    size_t num = m_vtTexcoords.size();
    for (size_t i = 0; i < num; ++i)
    {
        if (m_vtTexcoords[i] == texCoord)
        {
            return i;
        }
    }

    m_vtTexcoords.push_back(texCoord);
    return num;
}

CMeshKeyFrameData* CAIRSubMesh::CreateAnimationKeyframe(int nFrame)
{
    if (m_vtAnimationDatas.size() > (size_t)nFrame)
    {
        return m_vtAnimationDatas[(size_t)nFrame];
    }
    CMeshKeyFrameData* pData = new CMeshKeyFrameData(nFrame);
    pData->SetVerticesNum(GetVerticesCount());
    m_vtAnimationDatas.push_back(pData);
    return pData;
}

bool CAIRSubMesh::SaveVertexAnimation(FILE* f)
{
    if (m_vtPositions.size() == 0)
    {
        return false;
    }
    fprintf(f, "%s {\n", STR_MESH_HEADER);

    //string& strMeshName = mesh.GetName();
    fprintf(f, "    name %s\n", m_strName.c_str());
    int nVerticesNum = m_vtPositions.size();
    fprintf(f, "    vertex_num %d\n", nVerticesNum);
    for (int i = 0; i < nVerticesNum; ++i)
    {
        const AIRPOSITION& position = m_vtPositions[i];
        fprintf(f, "    vertex %d position(%f, %f, %f)\n", i, position.x, position.y, position.z);
    }

    int nTexcoordsNum = m_vtTexcoords.size();
    fprintf(f, "    num_texcoords %d\n", nTexcoordsNum);
    for (int i = 0; i < nTexcoordsNum; ++i)
    {
        const AIRTEXCOORD& texcoord = m_vtTexcoords[i];
        fprintf(f, "    vertex %d texcoord(%f, %f)\n", i, texcoord.u, texcoord.v);
    }

    for (size_t i = 0; i < m_vtAnimationDatas.size(); ++i)
    {
        m_vtAnimationDatas[i]->SaveToFile(f);
    }

    fprintf(f, "    }\n");
    return true;
}

bool CAIRSubMesh::SaveVertexAnimationBin(std::fstream& out)
{
    if (m_vtPositions.size() == 0)
    {
        return false;
    }
    out.write(m_strName.c_str(), 256);
    int nRealVertices = GetVerticesCount();
    out.write((const char*)&nRealVertices, sizeof(nRealVertices));
    int nVerticesNum = m_vtPositions.size(); //GetVerticesCount();
    out.write((const char*)&nVerticesNum, sizeof(INT32));

    out.write((const char*)&m_vtPositions[0], sizeof(AIRPOSITION) * nVerticesNum);

    int nTexcoordsNum = m_vtTexcoords.size();
    out.write((const char*)&nTexcoordsNum, sizeof(INT32));
    out.write((const char*)&m_vtTexcoords[0], sizeof(AIRTEXCOORD) * nTexcoordsNum);

    for (size_t i = 0; i < m_vtAnimationDatas.size(); ++i)
    {
        m_vtAnimationDatas[i]->SaveToFile(out);
    }

    return true;
}

const AIRMESHTRI& CAIRSubMesh::GetFace(int i)
{
    return m_vtFaces[i];
}
