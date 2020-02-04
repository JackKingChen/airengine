#include "AIRVertex.h"
#include "MaxMesh.h"
#include <math.h>
#include "AIRExporter.h"
#include <conio.h>

float CAIRVertex::EPSILON = 0.000001f;

CAIRVertex::CAIRVertex(void) : m_nWeightOffset(0)
, m_dwColor(0xffffffff)
{
}

CAIRVertex::~CAIRVertex(void)
{
}

void CAIRVertex::AddTextureCoordinate(TextureCoordinate& txtCoord)
{
    m_vtTexCoordinates.push_back(txtCoord);
}

void CAIRVertex::SetPosition(float x, float y, float z)
{
    m_position.x = x;
    m_position.y = y;
    m_position.z = z;
}

void CAIRVertex::GetPosition(AIRPOSITION& position)
{
    position = m_position;
}

bool CAIRVertex::operator == (const CAIRVertex& vertex)
{
    // compare the position
    if((fabs(m_position.x - vertex.m_position.x) >= EPSILON)
        || (fabs(m_position.y - vertex.m_position.y) >= EPSILON)
        || (fabs(m_position.z - vertex.m_position.z) >= EPSILON)) 
        return false;

    // compare the normal
    /*
    if((fabs(m_normal[0] - vertex.m_normal[0]) >= EPSILON)
        || (fabs(m_normal[1] - vertex.m_normal[1]) >= EPSILON)
        || (fabs(m_normal[2] - vertex.m_normal[2]) >= EPSILON)) 
        return false;
        */

    // compare the texture coordinates
    if(m_vtTexCoordinates.size() != vertex.m_vtTexCoordinates.size()) 
        return false;

    int textureCoordinateId;
    for(textureCoordinateId = 0; textureCoordinateId < m_vtTexCoordinates.size(); textureCoordinateId++)
    {
        if((fabs(m_vtTexCoordinates[textureCoordinateId].u - vertex.m_vtTexCoordinates[textureCoordinateId].u) >= EPSILON)
            || (fabs(m_vtTexCoordinates[textureCoordinateId].v - vertex.m_vtTexCoordinates[textureCoordinateId].v) >= EPSILON)) 
            return false;
    }

    return true;
}

bool CAIRVertex::Create(CMaxMesh* pMaxMesh, int nFaceID, int nFaceVertexIndex)
{
    if (pMaxMesh == NULL)
    {
        return false;
    }

    if (nFaceID < 0 || nFaceID >= pMaxMesh->GetFaceCount())
    {
        return false;
    }

    if (nFaceVertexIndex < 0 || nFaceVertexIndex > 2)
    {
        return false;
    }

    Mesh* pMesh = pMaxMesh->GetIMesh();

    //获得在mesh中的数组下标
    int nVertexID = pMesh->faces[nFaceID].v[nFaceVertexIndex];

    Point3 vertex;
    vertex = pMaxMesh->GetIMesh()->getVert(nVertexID) * pMaxMesh->GetWorldMatrix();

    SetPosition(vertex.x, vertex.y, vertex.z);

    //以下代码是用于获得权值，稍后再写


    //获得该顶点的所有纹理坐标
    int materialId = pMaxMesh->GetFaceMaterialId(nFaceID);
    if((materialId < 0) || (materialId >= pMaxMesh->GetMaterialCount()))
    {
        //delete pVertexCandidate;
        //theExporter.SetLastError("Invalid material id found.", __FILE__, __LINE__);
        return false;
    }

    // get the material of the face
    StdMat *pStdMat;
    pStdMat = pMaxMesh->GetStdMtl(materialId);

    // loop through all the mapping channels and extract texture coordinates
    int mapId;
    for(mapId = 0; mapId < pStdMat->NumSubTexmaps(); mapId++)
    {
        // get texture map
        Texmap *pTexMap;
        pTexMap = pStdMat->GetSubTexmap(mapId);

        // check if map is valid
        if((pTexMap != 0) && (pStdMat->MapEnabled(mapId)))
        {
            // get the mapping channel
            int channel;
            channel = pTexMap->GetMapChannel();

            bool bValidUV;
            bValidUV = false;

            // extract the texture coordinate
            UVVert uvVert;
            if(pMesh->mapSupport(channel))
            {
                TVFace *pTVFace;
                pTVFace = pMesh->mapFaces(channel);

                UVVert *pUVVert;
                pUVVert = pMesh->mapVerts(channel);

                uvVert = pUVVert[pTVFace[nFaceID].t[nFaceVertexIndex]];
                bValidUV = true;
            }
            else if(pMesh->numTVerts > 0)
            {
                uvVert = pMesh->tVerts[pMesh->tvFace[nFaceID].t[nFaceVertexIndex]];
                bValidUV = true;
            }

            // if we found valid texture coordinates, add them to the vertex candidate
            if(bValidUV)
            {
                // apply a possible uv generator
                StdUVGen *pStdUVGen;
                pStdUVGen = (StdUVGen *)pTexMap->GetTheUVGen();
                if(pStdUVGen != 0)
                {
                    Matrix3 tmUV;
                    pStdUVGen->GetUVTransform(tmUV);
                    uvVert = uvVert * tmUV;
                }

                // add texture coordinate to the vertex candidate
                TextureCoordinate texCoord;
                texCoord.u = uvVert.x;
                texCoord.v = uvVert.y;
                AddTextureCoordinate(texCoord);
            }
        }
    }

    return true;
}

void CAIRVertex::GetTexCoordinate(int nCoordinateIndex, TextureCoordinate& coordinate)
{
    if (nCoordinateIndex < 0 || nCoordinateIndex >= m_vtTexCoordinates.size())
    {
        return;
    }
    coordinate = m_vtTexCoordinates[nCoordinateIndex];
}

bool CAIRVertex::AddBoneInfluence(CAIRSkeleton* pSkeleton, INode* pINode, float fWeight)
{
    int nBoneID = pSkeleton->GetBoneID(pINode->GetName());
    if (nBoneID == -1)
    {
        _cprintf("Can not find the bone id!\n");
        //AIREXPORTER->ShowErrorMsg("Can not find the bone id!", __FILE__, __LINE__);
        return false;
    }

    Influence influence;
    influence.boneId = nBoneID;
    influence.weight = fWeight;

    m_vtInfluence.push_back(influence);

    return true;
}
