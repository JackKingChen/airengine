#include "stdafx.h"
#include "AIRSubMesh.h"
//#include "AIRStaticMeshFile.h"
#include "Engine.h"
#include "IVertexBuffer.h"
#include "VertexDef.h"
#include "StringUtil.h"
#include "Triangle3D.h"
#include "MaterialManager.h"
#include "ModelInstance.h"
#include "ProgressiveMesh.h"
#include "TimeBaseProfile.h"
#include "AIRModel.h"
#include "AIRKeyframeAnimationRes.h"

CAIRSubMesh::CAIRSubMesh() : 
//m_pSkeleton(NULL), 
m_pVertexAnimationRes(0),
m_nVerticesCount(0), 
m_nFacesCount(0)
, m_pNormalVB(NULL)
, m_bGenNormalVB(false)
, m_nCurLodLevel(0)
, m_animationType(0)
, m_pMaterial(NULL)
{

}


CAIRSubMesh::~CAIRSubMesh()
{
    Clear();
}

void CAIRSubMesh::ClearRenderableImp()
{
    IRenderer* pRenderer = ENGINE_INST->GetRenderer();

	if ( m_pNormalVB )
	{
		pRenderer->ClearVertexBuffer(m_pNormalVB);
	}

	ClearLodList();
}

void CAIRSubMesh::ComputeVertexBuffer(AIRVector<VERTEX_NORMAL>* vertices)
{
	if ( m_LodList.size() == 0 )
	{
		return;
	}
	LodData* pLod0 = m_LodList[0];

	//LPVERTEX_NORMAL pVertices = (LPVERTEX_NORMAL)pLod0->m_pMeshVB->Lock(true);
	LPVERTEX_NORMAL pVertices = (LPVERTEX_NORMAL)pLod0->m_VertexData.m_pMeshVB->Lock(true);
	LPVERTEX_NORMAL pVertex = NULL;
	for (size_t i = 0; i < /*m_nVerticesCount*/vertices->size(); i++)
	{
		pVertex = &(*vertices)[i];
		pVertices->normal.Normalize();
		memcpy(pVertices, pVertex, sizeof(VERTEX_NORMAL));
		pVertices++;
	}
	pLod0->m_VertexData.m_pMeshVB->Unlock();
	//pLod0->m_pMeshVB->Unlock();

	//LPVERTEX_NORMAL pBuf = AIR_NEW_ARRAY_T( VERTEX_NORMAL, m_nVerticesCount );
	//LPVERTEX_NORMAL pVertex = NULL;
	//LPVERTEX_NORMAL pVertices = pBuf;
	//for (s32 i = 0; i < m_nVerticesCount; i++)
	//{
	//	pVertex = &(*vertices)[i];
	//	pVertices->normal.Normalize();
	//	memcpy(pVertices, pVertex, sizeof(VERTEX_NORMAL));
	//	pVertices++;
	//}
	//pLod0->m_VertexData.m_pMeshVB->WriteData( 0, m_nVerticesCount * sizeof(VERTEX_NORMAL), pBuf, true );
	//AIR_DELETE_ARRAY_T( pBuf, VERTEX_NORMAL, m_nVerticesCount );
}

void CAIRSubMesh::ComputeIndexBuffer(AIRVector<AIRMESHTRI>* vtFaces)
{
	if ( m_LodList.size() == 0 )
	{
		return;
	}
	LodData* pLod0 = m_LodList[0];

	//WORD* pIndics = (WORD*)pLod0->m_pMeshIB->Lock(0, 0);
	u16* pIndics = (u16*)pLod0->m_VertexData.m_pMeshIB->Lock(0, 0);
	for (s32 i = 0; i < m_nFacesCount; i++)
	{
		const AIRMESHTRI& tri = (*vtFaces)[i];
		*pIndics++ = tri.nIndex[0];
		*pIndics++ = tri.nIndex[1];
		*pIndics++ = tri.nIndex[2];
	}
	//pLod0->m_pMeshIB->Unlock();
	pLod0->m_VertexData.m_pMeshIB->Unlock();

	//u32 indexCount = m_nFacesCount * 3;
	//u16* pBuf = AIR_NEW_ARRAY_T( u16, indexCount );
	//u16* pIndics = pBuf;
	//for (s32 i = 0; i < m_nFacesCount; i++)
	//{
	//	const AIRMESHTRI& tri = (*vtFaces)[i];
	//	*pIndics++ = tri.nIndex[0];
	//	*pIndics++ = tri.nIndex[1];
	//	*pIndics++ = tri.nIndex[2];
	//}
	//pLod0->m_VertexData.m_pMeshIB->WriteData(0, sizeof(u16) * indexCount, pBuf, true);
	//AIR_DELETE_ARRAY_T( pBuf, u16, indexCount );
}

void CAIRSubMesh::CreateVertices(int nVertexCount)
{
	if ( m_LodList.size() == 0 )
	{
		return;
	}
	LodData* pLod0 = m_LodList[0];

    IRenderer* pRenderer = ENGINE_INST->GetRenderer();
    if (m_animationType & eMAT_SkeletionAnimation && ENGINE_INST->IsHardwareSkeleton())
    {
		//pLod0->m_VertexData.m_pMeshVB = pRenderer->CreateVertexBuffer(E_VB_NORMAL_BONE_WEIGHT, nVertexCount);
        pLod0->m_VertexData.m_pMeshVB = pRenderer->CreateVertexBuffer(nVertexCount, sizeof(VERTEXNORMALBONEWEIGHTS));
    }
    else
	{
		//pLod0->m_VertexData.m_pMeshVB = pRenderer->CreateVertexBuffer(E_VB_NORMAL, nVertexCount/*, true*/);
        pLod0->m_VertexData.m_pMeshVB = pRenderer->CreateVertexBuffer(nVertexCount, sizeof(VERTEX_NORMAL));
	}
}

void CAIRSubMesh::CreateIndices(int nFacesCount)
{
	if ( m_LodList.size() == 0 )
	{
		return;
	}
	LodData* pLod0 = m_LodList[0];

    IRenderer* pRenderer = ENGINE_INST->GetRenderer();
	//pLod0->m_pMeshIB = pRenderer->CreateIndexBuffer(nFacesCount * 3/* * sizeof(WORD)*/);
	pLod0->m_VertexData.m_pMeshIB = pRenderer->CreateIndexBuffer(nFacesCount * 3/* * sizeof(WORD)*/);

    m_nFacesCount = nFacesCount;
}


void CAIRSubMesh::ComputeVertexNormal(AIRVector<VERTEX_NORMAL>* vertices, AIRVector<AIRMESHTRI>* vtFaces)
{
	LPAIRMESHTRI pFace = NULL;
	Vector3Df vFaceNormal;
    for (int i = 0; i < (int)vtFaces->size(); i++)
    {
        pFace = &(*vtFaces)[i];

		ComputeFaceNormal(
			(*vertices)[pFace->nIndex[0]].vPos, 
			(*vertices)[pFace->nIndex[1]].vPos, 
			(*vertices)[pFace->nIndex[2]].vPos,
			vFaceNormal);

        for (int j = 0; j < 3; j++)
        {
            (*vertices)[pFace->nIndex[j]].normal.x += vFaceNormal.x;
			(*vertices)[pFace->nIndex[j]].normal.y += vFaceNormal.y;
			(*vertices)[pFace->nIndex[j]].normal.z += vFaceNormal.z;
            (*vertices)[pFace->nIndex[j]].normal.Normalize();
        }
    }
}

bool CAIRSubMesh::LoadFromFileData(CDataStream* pFileStream, const char* szFilename)
{
	//清空原来的lod列表
	ClearLodList();

	//创建第0层的lod数据对象,pLod0是原始网格数据
	LodData* pLod0 = new LodData();
	//pLod0->m_pvtFaces = new FACE_LIST;
	//pLod0->m_pvtHardwareBlendVertex = new HARDWARE_BLEND_VERTEX_LIST;
	//pLod0->m_pvtSoftwareBlendVertex = new SOFTWARE_BLEND_VERTEX_LIST;
	//pLod0->m_pvtSoftwareBlendWeight = new SOFTWARE_BLEND_WEIGHT_LIST;
	m_LodList.push_back( pLod0 );
	m_pCurLodData = pLod0;

    int		vert_index = 0;
    int		tri_index = 0;
    float	fdata[4];
    float	uv[2];
    int		idata[3];
    int		nVertices = 0;
    int		nTriangles = 0;
    int		nVertexIndex = 0;
    int		weight_index = 0;
    int		nWeightCount = 0;
    float	fWeight = 0.0f;

    IRenderer* pRenderer = ENGINE_INST->GetRenderer();
    AIRString lineBuff;
    char buff[256] = { 0 };

	AIRVector<Vector3Df>  poins;

    while (buff[0] != '}' && !pFileStream->Eof())
    {
        lineBuff = pFileStream->GetLine();
        strncpy(buff, lineBuff.c_str(), 256);
        char szTmp[512];
	
		// 读取网格名称
        if (sscanf(lineBuff.c_str(), " name %s", szTmp) == 1)
        {
            const char* p = strstr(lineBuff.c_str(), "name");
            p = strstr(p, " ");
            strcpy(szTmp, p + 1);
            SetName(szTmp);
        }
		// 读取材质
        else if (sscanf (lineBuff.c_str(), " material %s", szTmp) == 1)
        {
            int quote = 0, j = 0;
            const char* p = strstr(lineBuff.c_str(), "material");
            p = strstr(p, " ");
            strcpy(szTmp, p + 1);
            m_strMaterialName = szTmp;
        }
		// 读取顶点数量
        else if (sscanf (lineBuff.c_str(), " num_vertices %d", &m_nVerticesCount) == 1)
        {
            if (m_nVerticesCount > 0)
            {
				// 如果是骨骼动画网格
                if (m_animationType & eMAT_SkeletionAnimation)
                {
					// 使用硬件蒙皮
                    if (ENGINE_INST->IsHardwareSkeleton())
                    {
						//pLod0->m_pvtHardwareBlendVertex->resize( m_nVerticesCount );
						pLod0->m_pvtHardwareBlendVertex.resize( m_nVerticesCount );
                    }
					// 使用软件蒙皮
                    else
					{
						pLod0->m_pvtSoftwareBlendVertex.resize( m_nVerticesCount );
					}                    
                }
				// 静态网格或者顶点动画网格
                else
                {
					pLod0->m_pvtSoftwareBlendVertex.resize( m_nVerticesCount );
                }

				// 创建顶点缓冲对象
                CreateVertices(m_nVerticesCount);
            }
			else
			{
				DT_TO_MSGBOX( LT_WARN, GetString("Mesh [%s] has 0 vertex!", m_strName.c_str()) );
				return false;
			}
        }
		// 读取网格三角面数量
        else if (sscanf (lineBuff.c_str(), " num_triangles %d", &m_nFacesCount) == 1)
        {
            if (m_nFacesCount > 0)
            {
				// 创建索引缓冲对象
				pLod0->m_pvtFaces.resize( m_nFacesCount );
                CreateIndices(m_nFacesCount);
            }
			else
			{
				DT_TO_MSGBOX( LT_WARN, GetString("Mesh [%s] has 0 face!", m_strName.c_str()) );
				return false;
			}
        }
		// 读取权重数量
   //     else if (sscanf (lineBuff.c_str(), " num_weights %d", &nVertices) == 1)
   //     {
			//pLod0->m_pvtSoftwareBlendWeight.resize( nVertices );
   //     }
		// 读取顶点信息
        else if (sscanf (lineBuff.c_str(), " vertex %d position (%f, %f, %f) tex_coordinate (%f, %f) %d %d", &vert_index,&fdata[0], &fdata[1], &fdata[2], &uv[0], &uv[1], &weight_index, &nWeightCount) == 8)
        {
			poins.push_back(Vector3Df(fdata[0],fdata[1],fdata[2]));
			//静态模型或顶点动画
            if (m_animationType == 0 || m_animationType & eMAT_VertexAnimation)   
            {
                LPMESHVERTNORMAL pVertex = AIR_NEW_T(MESHVERTNORMAL);
                pVertex->vertex.vPos.x = fdata[0];
                pVertex->vertex.vPos.y = fdata[1];
                pVertex->vertex.vPos.z = fdata[2];
                pVertex->vertex.color = 0xffffffff;
                pVertex->vertex.tu = uv[0];
                pVertex->vertex.tv = uv[1];
                //pVertex->nWeightIndex = weight_index;
                //pVertex->nWeightCount = nWeightCount;
				((pLod0->m_pvtSoftwareBlendVertex))[vert_index] = pVertex;
            }
			//带骨骼的模型
            else 
            {
				// 硬件蒙皮顶点
                if (ENGINE_INST->IsHardwareSkeleton())
                {
                    LPHARDWAREMESHVERTEX pVertex = AIR_NEW_T (HARDWAREMESHVERTEX);
                    memset(pVertex, 0, sizeof(HARDWAREMESHVERTEX));
                    pVertex->vertex.vertex.vPos.x = fdata[0];
                    pVertex->vertex.vertex.vPos.y = fdata[1];
                    pVertex->vertex.vertex.vPos.z = fdata[2];
                    pVertex->vertex.vertex.color = 0xffffffff;
                    pVertex->vertex.vertex.tu = uv[0];
                    pVertex->vertex.vertex.tv = uv[1];
                    //pVertex->nWeightIndex = weight_index;
                    //pVertex->nWeightCount = nWeightCount;
					((pLod0->m_pvtHardwareBlendVertex))[vert_index] = pVertex;
                }
				// 软件蒙皮顶点
                else
                {
                    LPMESHVERTNORMAL pVertex = AIR_NEW_T (MESHVERTNORMAL);
                    pVertex->vertex.vPos.x = fdata[0];
                    pVertex->vertex.vPos.y = fdata[1];
                    pVertex->vertex.vPos.z = fdata[2];
                    pVertex->vertex.color = 0xffffffff;
                    pVertex->vertex.tu = uv[0];
                    pVertex->vertex.tv = uv[1];
                    //pVertex->nWeightIndex = weight_index;
                    //pVertex->nWeightCount = nWeightCount;
					((pLod0->m_pvtSoftwareBlendVertex))[vert_index] = pVertex;
                }
                
            }
        }
		// 读取模型三角面信息
        else if (sscanf (lineBuff.c_str(), " mesh_triangle %d (%d %d %d)", &tri_index,&idata[0], &idata[1], &idata[2]) == 4)
        {
			AIRMESHTRI* pFace = &((pLod0->m_pvtFaces))[tri_index];
            pFace->nIndex[0] = idata[0];
            pFace->nIndex[1] = idata[1];
            pFace->nIndex[2] = idata[2];
        }
		// 读取权重信息
   //     else if (sscanf (lineBuff.c_str(), " weight %d %d %f",&weight_index, &idata[0], &fWeight) == 3)
   //     {
			//((pLod0->m_pvtSoftwareBlendWeight))[weight_index].nBoneID = idata[0];
			//((pLod0->m_pvtSoftwareBlendWeight))[weight_index].fWeight = fWeight;
   //     }
		// 读取网格包围盒信息
        else if (sscanf(lineBuff.c_str(), " mesh_boundingbox max(%f, %f, %f) min(%f, %f, %f)", &m_boundingBox.MaxEdge.x,&m_boundingBox.MaxEdge.y, &m_boundingBox.MaxEdge.z, &m_boundingBox.MinEdge.x,&m_boundingBox.MinEdge.y, &m_boundingBox.MinEdge.z) == 6)
        {
        }
    }

	//如果是静态模型或者顶点动画网格
    if (m_animationType == eMAT_NoAnimation || m_animationType == eMAT_VertexAnimation)   
    {
		//计算Normal
		ComputeVertexNormal(&(pLod0->m_pvtSoftwareBlendVertex), &(pLod0->m_pvtFaces));
		//填充VB
		ComputeVertexBuffer(&(pLod0->m_pvtSoftwareBlendVertex));
    }
	// 骨骼动画网格
    else
    {
		// 硬件蒙皮
        if (ENGINE_INST->IsHardwareSkeleton())
        {
			//计算每个顶点的权重
			/*
			for (size_t i = 0; i < pLod0->m_pvtHardwareBlendVertex.size(); ++i)
			{
				LPHARDWAREMESHVERTEX pVertex = ((pLod0->m_pvtHardwareBlendVertex))[i];
				int nWeightsNum = pVertex->nWeightCount <= 4 ? pVertex->nWeightCount : 4;
				for (int j = 0; j < nWeightsNum; ++j)
				{
					int nWeightIndex = pVertex->nWeightIndex + j;
					pVertex->vertex.boneIndex[j] = ((pLod0->m_pvtSoftwareBlendWeight))[nWeightIndex].nBoneID;
					pVertex->vertex.weights[j] = ((pLod0->m_pvtSoftwareBlendWeight))[nWeightIndex].fWeight;
				}
			}
			*/

			//计算Normal
			ComputeVertexNormal(&(pLod0->m_pvtHardwareBlendVertex), &(pLod0->m_pvtFaces));
			//填充VB
			ComputeVertexBuffer(&(pLod0->m_pvtHardwareBlendVertex));
        }
		// 软件蒙皮
        else
        {
			//计算Normal
			ComputeVertexNormal(&(pLod0->m_pvtSoftwareBlendVertex), &(pLod0->m_pvtFaces));
			//填充VB
			ComputeVertexBuffer(&(pLod0->m_pvtSoftwareBlendVertex));
        }

    }
	m_obb.compute_obbox(poins.begin(),poins.end());

    // 填充第0层lod的索引数据
	ComputeIndexBuffer( &(pLod0->m_pvtFaces) );

	// 创建网格的lod数据
	CreateLodDataList();

	//-----------------------------------------
	//! add by hjj 2010-5-25
	if ( m_bGenNormalVB )
	{
		GenNormalVB();
	}
	//-----------------------------------------
    return true;
}

bool CAIRSubMesh::LoadFromFileDataBinary(CDataStream* pFileStream, AIRModel* pModel)
{
	//清空原来的lod列表
	//ClearLodList();

	//创建第0层的lod数据对象,pLod0是原始网格数据
	LodData* pLod0 = new LodData();
	//pLod0->m_pvtFaces = new FACE_LIST;
	//pLod0->m_pvtHardwareBlendVertex = new HARDWARE_BLEND_VERTEX_LIST;
	//pLod0->m_pvtSoftwareBlendVertex = new SOFTWARE_BLEND_VERTEX_LIST;
	//pLod0->m_pvtSoftwareBlendWeight = new SOFTWARE_BLEND_WEIGHT_LIST;
	m_LodList.push_back( pLod0 );
	m_pCurLodData = pLod0;

	// 读取网格的名称
    int nNameLen = 0;
    pFileStream->Read(&nNameLen, sizeof(nNameLen));
    char szName[256] = { 0 };
    pFileStream->Read(szName, nNameLen);
    m_strName = szName;

	// 读取材质
    pFileStream->Read(&nNameLen, sizeof(nNameLen));
    pFileStream->Read(szName, nNameLen);

    m_strMaterialName = szName;

    m_pMaterial = MATERIAL_MANAGER->CreateMaterial(m_strMaterialName.c_str());

    // 读入顶点数
    pFileStream->Read(&m_nVerticesCount, sizeof(m_nVerticesCount));

    if (m_nVerticesCount > 0)
    {
        if (!(m_animationType & eMAT_SkeletionAnimation))
        {
			pLod0->m_pvtSoftwareBlendVertex.resize( m_nVerticesCount );
        }
        else
        {
            if (ENGINE_INST->IsHardwareSkeleton())
            {
				pLod0->m_pvtHardwareBlendVertex.resize(m_nVerticesCount);
            }
            else
			{
				pLod0->m_pvtSoftwareBlendVertex.resize( m_nVerticesCount );
			}
        }
        CreateVertices(m_nVerticesCount);
    }
    else
    {
		DT_TO_MSGBOX( LT_WARN, GetString("Mesh [%s] has 0 vertex!", m_strName.c_str()) );
        return false;
    }

    //读取顶点数据
    float fdata[5];
    u32 color = 0xffffffff;
    //int   nWeight[2];
	u8 boneID[4] = { 0 };
	float fWeights[4] = { 0 };

	AIRVector<Vector3Df>  poins;
    for (int i = 0; i < m_nVerticesCount; ++i)
    {
        pFileStream->Read(fdata, sizeof(fdata));
        if (pModel->GetFileVersion() > 0.1f)
        {
            pFileStream->Read(&color, sizeof(u32));
        }
        //pFileStream->Read(nWeight, sizeof(nWeight));
		pFileStream->Read(boneID, sizeof(boneID));
		pFileStream->Read(fWeights, sizeof(fWeights));
		poins.push_back(Vector3Df(fdata[0],fdata[1],fdata[2]));
        if (!(m_animationType & eMAT_SkeletionAnimation))
        {
            LPMESHVERTNORMAL pVertex = AIR_NEW_T (MESHVERTNORMAL);
            pVertex->vertex.vPos.x = fdata[0];
            pVertex->vertex.vPos.y = fdata[1];
            pVertex->vertex.vPos.z = fdata[2];
            pVertex->vertex.color = color;
            pVertex->vertex.tu = fdata[3];
            pVertex->vertex.tv = fdata[4];
			for (int j = 0; j < 4; ++j)
			{
				pVertex->boneID[j] = boneID[j];
				pVertex->fWeights[j] = fWeights[j];
			}
			((pLod0->m_pvtSoftwareBlendVertex))[i] = pVertex;
        }
        else  //带骨骼的模型
        {
            if (ENGINE_INST->IsHardwareSkeleton())
            {
                LPHARDWAREMESHVERTEX pVertex = AIR_NEW_T (HARDWAREMESHVERTEX);
                memset(pVertex, 0, sizeof(HARDWAREMESHVERTEX));
                pVertex->vertex.vertex.vPos.x = fdata[0];
                pVertex->vertex.vertex.vPos.y = fdata[1];
                pVertex->vertex.vertex.vPos.z = fdata[2];
                pVertex->vertex.vertex.color = color;
                pVertex->vertex.vertex.tu = fdata[3];
                pVertex->vertex.vertex.tv = fdata[4];
				for (int j = 0; j < 4; ++j)
				{
					pVertex->vertex.boneIndex[j] = boneID[j];
					pVertex->vertex.weights[j] = fWeights[j];
				}
                //pVertex->nWeightIndex = nWeight[0];
                //pVertex->nWeightCount = nWeight[1];
				((pLod0->m_pvtHardwareBlendVertex))[i] = pVertex;
            }
            else
            {
                LPMESHVERTNORMAL pVertex = AIR_NEW_T (MESHVERTNORMAL);
                pVertex->vertex.vPos.x = fdata[0];
                pVertex->vertex.vPos.y = fdata[1];
                pVertex->vertex.vPos.z = fdata[2];
                pVertex->vertex.color = color;
                pVertex->vertex.tu = fdata[3];
                pVertex->vertex.tv = fdata[4];
				for (int j = 0; j < 4; ++j)
				{
					pVertex->boneID[j] = boneID[j];
					pVertex->fWeights[j] = fWeights[j];
				}
                //pVertex->nWeightIndex = nWeight[0];
                //pVertex->nWeightCount = nWeight[1];
				((pLod0->m_pvtSoftwareBlendVertex))[i] = pVertex;
            }
        }
    }
	m_obb.compute_obbox(poins.begin(),poins.end());

    //读取三角形数
    pFileStream->Read(&m_nFacesCount, sizeof(m_nFacesCount));

	if ( m_nFacesCount <= 0 )
	{
		DT_TO_MSGBOX( LT_WARN, GetString("Mesh [%s] has 0 face!", m_strName.c_str()) );
		return false;
	}

	pLod0->m_pvtFaces.resize( m_nFacesCount );
    CreateIndices(m_nFacesCount);
	pFileStream->Read(&((pLod0->m_pvtFaces))[0], pLod0->m_pvtFaces.size() * sizeof(AIRMESHTRI));

    //读取权重信息
	/*
    int nWeigthsNum = 0;
    pFileStream->Read(&nWeigthsNum, sizeof(nWeigthsNum));
    if (nWeigthsNum > 0)
    {
		pLod0->m_pvtSoftwareBlendWeight.resize(nWeigthsNum);
		pFileStream->Read(&((pLod0->m_pvtSoftwareBlendWeight))[0], nWeigthsNum * sizeof(MESHWEIGHT));
    }
	*/
    
    //读取boundingbox信息
    pFileStream->Read(&m_boundingBox, sizeof(float) * 6);

    if (!(m_animationType & eMAT_SkeletionAnimation))   //如果是静态模型
    {
		//计算Normal
		ComputeVertexNormal(&(pLod0->m_pvtSoftwareBlendVertex), &(pLod0->m_pvtFaces));
		//填充VB
		ComputeVertexBuffer(&(pLod0->m_pvtSoftwareBlendVertex));
    }
    else
    {
        if (ENGINE_INST->IsHardwareSkeleton())
        {
			//计算每个顶点的权重
			/*
			for (size_t i = 0; i < pLod0->m_pvtHardwareBlendVertex.size(); ++i)
			{
				LPHARDWAREMESHVERTEX pVertex = ((pLod0->m_pvtHardwareBlendVertex))[i];
				for (int j = 0; j < pVertex->nWeightCount; ++j)
				{
					int nWeightIndex = pVertex->nWeightIndex + j;
					pVertex->vertex.boneIndex[j] = ((pLod0->m_pvtSoftwareBlendWeight))[nWeightIndex].nBoneID;
					pVertex->vertex.weights[j] = ((pLod0->m_pvtSoftwareBlendWeight))[nWeightIndex].fWeight;
				}
			}
			*/
			ComputeVertexNormal(&(pLod0->m_pvtHardwareBlendVertex), &(pLod0->m_pvtFaces));
			ComputeVertexBuffer(&(pLod0->m_pvtHardwareBlendVertex));
        }
        else
        {
			//计算Normal
			ComputeVertexNormal(&(pLod0->m_pvtSoftwareBlendVertex), &(pLod0->m_pvtFaces));
			//填充VB
			ComputeVertexBuffer(&(pLod0->m_pvtSoftwareBlendVertex));
        }
        
    }

	// 创建索引缓冲
	ComputeIndexBuffer( &(pLod0->m_pvtFaces) );

	// 创建网格lod信息
	CreateLodDataList();

	// 创建顶点的法线顶点缓冲
	if ( m_bGenNormalVB )
	{
		GenNormalVB();
	}

    return true;
}

void CAIRSubMesh::ComputeFaceNormal(const Vector3Df& v1, const Vector3Df& v2, const Vector3Df& v3, Vector3Df& vNormal)
{
	vNormal = (v2 - v1).CrossProduct(v3 - v1);
}


void CAIRSubMesh::ComputeVertexBuffer(AIRVector<LPMESHVERTNORMAL>* vertices)
{
	if ( m_LodList.size() == 0 )
	{
		return;
	}
	LodData* pLod0 = m_LodList[0];

	LPVERTEX_NORMAL pVertices = (LPVERTEX_NORMAL)pLod0->m_VertexData.m_pMeshVB->Lock(true);
	LPVERTEX_NORMAL pVertex = NULL;
	for (s32 i = 0; i < m_nVerticesCount; i++)
	{
		pVertex = &(*vertices)[i]->vertex;
		pVertices->normal.Normalize();
		memcpy(pVertices, pVertex, sizeof(VERTEX_NORMAL));
		pVertices++;
	}
	pLod0->m_VertexData.m_pMeshVB->Unlock();

	//LPVERTEX_NORMAL pBuf = AIR_NEW_ARRAY_T( VERTEX_NORMAL, m_nVerticesCount );
	//LPVERTEX_NORMAL pVertex = NULL;
	//LPVERTEX_NORMAL pVertices = pBuf;
	//for (s32 i = 0; i < m_nVerticesCount; i++)
	//{
	//	pVertex = &(*vertices)[i]->vertex;
	//	pVertices->normal.Normalize();
	//	memcpy(pVertices, pVertex, sizeof(VERTEX_NORMAL));
	//	pVertices++;
	//}
	//pLod0->m_VertexData.m_pMeshVB->WriteData(0, m_nVerticesCount * sizeof(VERTEX_NORMAL), pBuf, true);
	//AIR_DELETE_ARRAY_T( pBuf, VERTEX_NORMAL, m_nVerticesCount );
}

void CAIRSubMesh::ComputeVertexNormal(AIRVector<LPMESHVERTNORMAL>* vertices, AIRVector<AIRMESHTRI>* vtFaces)
{
	LPAIRMESHTRI pFace = NULL;
	Vector3Df vFaceNormal;
    for (int i = 0; i < (int)vtFaces->size(); i++)
    {
        pFace = &(*vtFaces)[i];
        ComputeFaceNormal(
			(*vertices)[pFace->nIndex[0]]->vertex.vPos, 
			(*vertices)[pFace->nIndex[1]]->vertex.vPos,
			(*vertices)[pFace->nIndex[2]]->vertex.vPos,
			vFaceNormal);

        for (int j = 0; j < 3; j++)
        {
            (*vertices)[pFace->nIndex[j]]->vertex.normal.x += vFaceNormal.x;
			(*vertices)[pFace->nIndex[j]]->vertex.normal.y += vFaceNormal.y;
			(*vertices)[pFace->nIndex[j]]->vertex.normal.z += vFaceNormal.z;
            (*vertices)[pFace->nIndex[j]]->vertex.normal.Normalize();
        }
    }
}

void CAIRSubMesh::CaculateVertices(const SColor& colorDiffuse, Matrix4f* pBoneMatrix, f32 fTimePos)
{
    if (/*m_pSkeleton && */m_pCurLodData)
    {
		LPVERTEX_NORMAL pVertices = (LPVERTEX_NORMAL)m_pCurLodData->m_VertexData.m_pMeshVB->Lock(true);
		CaculateVertices(colorDiffuse, pBoneMatrix, &pVertices, fTimePos);
		m_pCurLodData->m_VertexData.m_pMeshVB->Unlock();
    }

	//-----------------------------------------
	//! add by hjj 2010-5-25
	if ( m_bGenNormalVB )
	{
		GenNormalVB();
	}
	//-----------------------------------------
}

void CAIRSubMesh::CaculateVertices(const SColor& colorDiffuse, Matrix4f* pBoneMatrix, LPVERTEX_NORMAL* pVertex, f32 fTimePos)
{
    Vector3Df			vFinalPos;
    Vector3Df			vWorldPos;  //变换后的世界坐标
    Vector3Df			vFinalNormal;
    Vector3Df			vNormal;
    size_t				nStride = sizeof(VERTEX_NORMAL) / sizeof(f32);

    //-----------------------------------------------------------------------------------------
    LPMESHVERTNORMAL	pMeshVertex = NULL;
	//LPMESHWEIGHT		pWeight = NULL;
	Matrix4f*			pMatrix = NULL;
    float*				pDestNormal = &((*pVertex)->normal.x);
    float*				pDestPos = &((*pVertex)->vPos.x);
    //SColor*				pDiffuseColor = &((*pVertex)->color);
	f32*                pDiffuseColor = &((*pVertex)->color.r);
    f32*                pUV = &((*pVertex)->tu);
	Vector3Df*			vTmpPos = NULL;
	Vector3Df*			vTmpNormal = NULL;
	int					nWeightCount = 4;
	float				fWeight = 0.0f; 
	s32					nCurID = 0;
	size_t				nVecSize = sizeof(float) * 3;
	s32					nCurVertexCount = m_pCurLodData->m_VertexData.m_pMeshVB->GetVertexCount();

	// 遍历所有的顶点
	for ( s32 i = 0; i < nCurVertexCount; ++i )
	{
		pDestPos[0] = pDestPos[1] = pDestPos[2] = 0.0f;
		pDestNormal[0] = pDestNormal[1] = pDestNormal[2] = 0.0f;

		pMeshVertex = ((m_pCurLodData->m_pvtSoftwareBlendVertex))[i];
		//pWeight = &((m_pCurLodData->m_pvtSoftwareBlendWeight))[pMeshVertex->nWeightIndex];
		vTmpPos = &(pMeshVertex->vertex.vPos);
		vTmpNormal = &(pMeshVertex->vertex.normal);
		//nWeightCount = pMeshVertex->nWeightCount;

		// 遍历所有影响到该顶点的骨骼
		for ( nCurID = 0; nCurID < nWeightCount; ++nCurID ) 
		{
			// 取得权值与对应的骨骼
			fWeight = pMeshVertex->fWeights[nCurID]; //pWeight[nCurID].fWeight;

			//if ( fWeight > 0.0f )
			{
				// 获得变换矩阵
				int nBoneID = pMeshVertex->boneID[ nCurID ];
				pMatrix = &pBoneMatrix[ nBoneID ];
				// 变换顶点坐标
				pMatrix->TransformCoordFast( *vTmpPos, vWorldPos );
				// 变换法线
				pMatrix->TransformNormal( vNormal, *vTmpNormal );

				pDestPos[0]		+= vWorldPos.x * fWeight;
				pDestPos[1]		+= vWorldPos.y * fWeight;
				pDestPos[2]		+= vWorldPos.z * fWeight;

				pDestNormal[0]	+= vNormal.x * fWeight;
				pDestNormal[1]	+= vNormal.y * fWeight;
				pDestNormal[2]	+= vNormal.z * fWeight;
			}
		}

		FastNormalize( pDestNormal );		

        CColorValue* pColorValue = (CColorValue*)pDiffuseColor;
		*pColorValue = colorDiffuse;
        pUV[0] = pMeshVertex->vertex.tu;
        pUV[1] = pMeshVertex->vertex.tv;

		pDestPos += nStride;
		pDestNormal += nStride;
		pDiffuseColor += nStride;
        pUV += nStride;
	}

    //如果都带有顶点动画
    if (m_animationType & eMAT_VertexAnimation && m_pVertexAnimationRes)
    {
        VertexAnimation* pVertexAnimation = m_pVertexAnimationRes->GetVertexAnimation();
        pVertexAnimation->ApplyToVertices(this, *pVertex, nCurVertexCount, fTimePos);
    }
}

void CAIRSubMesh::SetTransparent(bool bAlpha /* = true */, u8 nAlpha /* = 128 */)
{
    //Renderable::SetTransparent(bAlpha, nAlpha);
    SColor color(0xffffffff);
    if (bAlpha)
    {
        color.SetAlpha(nAlpha);
    }
    else
    {
        color.SetAlpha(255);
    }

    if (!(m_animationType & eMAT_SkeletionAnimation))
    {
		LPVERTEX_NORMAL pVertices = (LPVERTEX_NORMAL)m_pCurLodData->m_VertexData.m_pMeshVB->Lock(true);
		for (s32 i = 0; i < m_nVerticesCount; i++)
		{
			pVertices->color = color.color;
			pVertices++;
		}
		m_pCurLodData->m_VertexData.m_pMeshVB->Unlock();
    }
}



void CAIRSubMesh::GenNormalVB( void )
{
	if ( m_LodList.size() == 0 )
	{
		return;
	}
	LodData* pLod0 = m_LodList[0];

	// 暂时创建初始网格的法线信息
	IRenderer* pRenderer = ENGINE_INST->GetRenderer();
	if ( !m_pNormalVB )
	{
		//m_pNormalVB = pRenderer->CreateVertexBuffer(E_VB_NOTEX, m_nVerticesCount * 2);
        m_pNormalVB = pRenderer->CreateVertexBuffer(m_nVerticesCount * 2, sizeof(VERTEX_NOTEX));
	}

	LPVERTEX_NORMAL pVBMesh = (LPVERTEX_NORMAL)(pLod0->m_VertexData.m_pMeshVB->Lock());
	LPVERTEX_NOTEX pVB = (LPVERTEX_NOTEX)(m_pNormalVB->Lock());
	
	int id = 0;
	for ( int i = 0; i < m_nVerticesCount; ++i )
	{
		pVB[id].color = 0xffffffff;
		pVB[id++].position = pVBMesh->vPos;

		pVB[id].color = 0xffffffff;
		pVB[id++].position = pVBMesh->vPos + pVBMesh->normal * 10.0f;

        pVBMesh++;
	}

	m_pNormalVB->Unlock();
	pLod0->m_VertexData.m_pMeshVB->Unlock();
}


void CAIRSubMesh::SetDrawMeshNormal( bool b )
{
	m_bGenNormalVB = b;
	if ( m_bGenNormalVB )
	{
		GenNormalVB();
	}
}

bool CAIRSubMesh::IsPick(const Vector3Df& vOrg, const Vector3Df& vPickDir, const Matrix4f& matTransform)
{
	bool bPick = false;
	LPVERTEX_NORMAL pVertices = (LPVERTEX_NORMAL)m_pCurLodData->m_VertexData.m_pMeshVB->Lock(true);
	size_t size = m_pCurLodData->m_pvtFaces.size();
	Triangle3Df tri;
	Vector3Df pointIntersect;
	LPAIRMESHTRI pFace = NULL;
	//循环size次,找到击中的最近点
	for (size_t i = 0; i < size; i++)
	{
		pFace = &((m_pCurLodData->m_pvtFaces))[i];
		tri.Set(pVertices[pFace->nIndex[0]].vPos, pVertices[pFace->nIndex[1]].vPos, pVertices[pFace->nIndex[2]].vPos);
		matTransform.TransformTriangle(tri, tri);
		if (tri.GetIntersectionWithLine(vOrg, vPickDir, pointIntersect))
		{
			bPick = true;
			break;
		}
	}
	m_pCurLodData->m_VertexData.m_pMeshVB->Unlock();
	return bPick;
}


bool CAIRSubMesh::IsPick(const Vector3Df& vOrg, const Vector3Df& vPickDir, const Matrix4f& matTransform, float& distFromEyeToObject)
{
	//初始化摄像机到击中面的距离
	distFromEyeToObject = float(0xfffffffe);
	//检查的三角面数
	size_t size = m_pCurLodData->m_pvtFaces.size();
	//三角面临时对象
	Triangle3Df tri;
	//击中点
	Vector3Df pointIntersect;
	LPAIRMESHTRI pFace = NULL;
	//点击结果
	bool bPick = false;

	LPVERTEX_NORMAL pVertices = (LPVERTEX_NORMAL)m_pCurLodData->m_VertexData.m_pMeshVB->Lock(true);
	//循环size次,找到击中的最近点
	for (size_t i = 0; i < size; i++)
	{
		pFace = &((m_pCurLodData->m_pvtFaces))[i];
		tri.Set(pVertices[pFace->nIndex[0]].vPos, pVertices[pFace->nIndex[1]].vPos, pVertices[pFace->nIndex[2]].vPos);
		matTransform.TransformTriangle(tri, tri);
		if (tri.GetIntersectionWithLine(vOrg, vPickDir, pointIntersect))
		{
			//计算摄像机到击中点的距离
			distFromEyeToObject = vOrg.GetDistanceFrom( pointIntersect );
			bPick = true;
		}
	}
	m_pCurLodData->m_VertexData.m_pMeshVB->Unlock();
	return bPick;
}


void CAIRSubMesh::CaculateVerticesByVertexAnimation(float fInterpolate, VertexAnimationKeyframe* pKeyframe1, VertexAnimationKeyframe* pKeyframe2, s32 nAnimationMask)
{
    if ((m_animationType & eMAT_SkeletionAnimation) && ENGINE_INST->IsHardwareSkeleton())
    {
        LPHARDWAREMESHVERTEX pVertices = (LPHARDWAREMESHVERTEX)m_pCurLodData->m_VertexData.m_pMeshVB->Lock(true);
        if (nAnimationMask & eVAM_UV)
        {
            for (s32 i = 0; i < m_nVerticesCount; i++)
            {
                pVertices[i].vertex.vertex.tu = fInterpolate < 0.5f ? pKeyframe1->m_vtVertexKeyframeUVs[i].x : pKeyframe2->m_vtVertexKeyframeUVs[i].y; //pKeyframe1->m_vtVertexKeyframeDatas[i].tu * (1.0f - fInterpolate) + pKeyframe2->m_vtVertexKeyframeDatas[i].tu * fInterpolate;
                pVertices[i].vertex.vertex.tv = fInterpolate < 0.5f ? pKeyframe1->m_vtVertexKeyframeUVs[i].x : pKeyframe2->m_vtVertexKeyframeUVs[i].y; //pKeyframe1->m_vtVertexKeyframeDatas[i].tv * (1.0f - fInterpolate) + pKeyframe2->m_vtVertexKeyframeDatas[i].tv * fInterpolate;
            }
        }

        if (nAnimationMask & eVAM_Color)
        {
            for (s32 i = 0; i < m_nVerticesCount; i++)
            {
                pVertices->vertex.vertex.color = fInterpolate < 0.5f ? pKeyframe1->m_vtVertexKeyframeColors[i] : pKeyframe2->m_vtVertexKeyframeColors[i];
            }
        }
        
        m_pCurLodData->m_VertexData.m_pMeshVB->Unlock();
    }
    else
    {
	    LPVERTEX_NORMAL pVertices = (LPVERTEX_NORMAL)m_pCurLodData->m_VertexData.m_pMeshVB->Lock(true);
        if (nAnimationMask & eVAM_Position)
        {
            for (s32 i = 0; i < m_nVerticesCount; i++)
            {
                pVertices[i].vPos.x = pKeyframe1->m_vtVertexKeyframePositions[i].x + fInterpolate * (pKeyframe2->m_vtVertexKeyframePositions[i].x - pKeyframe1->m_vtVertexKeyframePositions[i].x);
                pVertices[i].vPos.y = pKeyframe1->m_vtVertexKeyframePositions[i].y + fInterpolate * (pKeyframe2->m_vtVertexKeyframePositions[i].y - pKeyframe1->m_vtVertexKeyframePositions[i].y);
                pVertices[i].vPos.z = pKeyframe1->m_vtVertexKeyframePositions[i].z + fInterpolate * (pKeyframe2->m_vtVertexKeyframePositions[i].z - pKeyframe1->m_vtVertexKeyframePositions[i].z);
            }
        }
        if (nAnimationMask & eVAM_UV)
        {
            for (s32 i = 0; i < m_nVerticesCount; i++)
            {
                pVertices[i].tu = fInterpolate < 0.5f ? pKeyframe1->m_vtVertexKeyframeUVs[i].x : pKeyframe2->m_vtVertexKeyframeUVs[i].x; //pKeyframe1->m_vtVertexKeyframeDatas[i].tu * (1.0f - fInterpolate) + pKeyframe2->m_vtVertexKeyframeDatas[i].tu * fInterpolate;
                pVertices[i].tv = fInterpolate < 0.5f ? pKeyframe1->m_vtVertexKeyframeUVs[i].y : pKeyframe2->m_vtVertexKeyframeUVs[i].y; //pKeyframe1->m_vtVertexKeyframeDatas[i].tv * (1.0f - fInterpolate) + pKeyframe2->m_vtVertexKeyframeDatas[i].tv * fInterpolate;
            }
        }

        if (nAnimationMask & eVAM_Color)
        {
            for (s32 i = 0; i < m_nVerticesCount; i++)
            {
                pVertices[i].color = fInterpolate < 0.5f ? pKeyframe1->m_vtVertexKeyframeColors[i] : pKeyframe2->m_vtVertexKeyframeColors[i];
            }
        }
        /*
	    for (s32 i = 0; i < m_nVerticesCount; i++)
	    {
		    pVertices->vPos.x = pKeyframe1->m_vtVertexKeyframeDatas[i].position.x + fInterpolate * (pKeyframe2->m_vtVertexKeyframeDatas[i].position.x - pKeyframe1->m_vtVertexKeyframeDatas[i].position.x);
		    pVertices->vPos.y = pKeyframe1->m_vtVertexKeyframeDatas[i].position.y + fInterpolate * (pKeyframe2->m_vtVertexKeyframeDatas[i].position.y - pKeyframe1->m_vtVertexKeyframeDatas[i].position.y);
		    pVertices->vPos.z = pKeyframe1->m_vtVertexKeyframeDatas[i].position.z + fInterpolate * (pKeyframe2->m_vtVertexKeyframeDatas[i].position.z - pKeyframe1->m_vtVertexKeyframeDatas[i].position.z);
            pVertices->tu = fInterpolate < 0.5f ? pKeyframe1->m_vtVertexKeyframeDatas[i].tu : pKeyframe2->m_vtVertexKeyframeDatas[i].tu; //pKeyframe1->m_vtVertexKeyframeDatas[i].tu * (1.0f - fInterpolate) + pKeyframe2->m_vtVertexKeyframeDatas[i].tu * fInterpolate;
		    pVertices->tv = fInterpolate < 0.5f ? pKeyframe1->m_vtVertexKeyframeDatas[i].tv : pKeyframe2->m_vtVertexKeyframeDatas[i].tv; //pKeyframe1->m_vtVertexKeyframeDatas[i].tv * (1.0f - fInterpolate) + pKeyframe2->m_vtVertexKeyframeDatas[i].tv * fInterpolate;
            pVertices->color = fInterpolate < 0.5f ? pKeyframe1->m_vtVertexKeyframeDatas[i].color : pKeyframe2->m_vtVertexKeyframeDatas[i].color;
		    pVertices++;
	    }
        */
	    m_pCurLodData->m_VertexData.m_pMeshVB->Unlock();
    }
}

void CAIRSubMesh::ComputeVertexNormal(AIRVector<LPHARDWAREMESHVERTEX>* vertices, AIRVector<AIRMESHTRI>* vtFaces)
{
	LPAIRMESHTRI pFace = NULL;
    for (int i = 0; i < (int)vtFaces->size(); i++)
    {
		pFace = &((m_pCurLodData->m_pvtFaces))[i];
        Vector3Df vFaceNormal;
        ComputeFaceNormal(
			(*vertices)[pFace->nIndex[0]]->vertex.vertex.vPos,
			(*vertices)[pFace->nIndex[1]]->vertex.vertex.vPos, 
			(*vertices)[pFace->nIndex[2]]->vertex.vertex.vPos, 
			vFaceNormal);

        for (int j = 0; j < 3; j++)
        {
            (*vertices)[pFace->nIndex[j]]->vertex.vertex.normal.x += vFaceNormal.x;
			(*vertices)[pFace->nIndex[j]]->vertex.vertex.normal.y += vFaceNormal.y;
			(*vertices)[pFace->nIndex[j]]->vertex.vertex.normal.z += vFaceNormal.z;
            (*vertices)[pFace->nIndex[j]]->vertex.vertex.normal.Normalize();
        }
    }
}

void CAIRSubMesh::ComputeVertexBuffer(AIRVector<LPHARDWAREMESHVERTEX>* vertices)
{
	if ( m_LodList.size() == 0 )
	{
		return;
	}
	LodData* pLod0 = m_LodList[0];

	LPVERTEXNORMALBONEWEIGHTS pVertices = (LPVERTEXNORMALBONEWEIGHTS)pLod0->m_VertexData.m_pMeshVB->Lock(true);
	LPVERTEXNORMALBONEWEIGHTS pVertex = NULL;
	for (s32 i = 0; i < m_nVerticesCount; i++)
	{
		pVertex = &(*vertices)[i]->vertex;
		pVertex->vertex.normal.Normalize();
		memcpy(pVertices, pVertex, sizeof(VERTEXNORMALBONEWEIGHTS));
		pVertices++;
	}
	pLod0->m_VertexData.m_pMeshVB->Unlock();

	//LPVERTEXNORMALBONEWEIGHTS pBuf = AIR_NEW_ARRAY_T( VERTEXNORMALBONEWEIGHTS, m_nVerticesCount );
	//LPVERTEXNORMALBONEWEIGHTS pVertex = NULL;
	//LPVERTEXNORMALBONEWEIGHTS pVertices = pBuf;
	//for (s32 i = 0; i < m_nVerticesCount; i++)
	//{
	//	pVertex = &(*vertices)[i]->vertex;
	//	pVertex->vertex.normal.Normalize();
	//	memcpy(pVertices, pVertex, sizeof(VERTEXNORMALBONEWEIGHTS));
	//	pVertices++;
	//}
	//pLod0->m_VertexData.m_pMeshVB->WriteData( 0, m_nVerticesCount * sizeof(VERTEXNORMALBONEWEIGHTS), pBuf, true);
	//AIR_DELETE_ARRAY_T( pBuf, VERTEXNORMALBONEWEIGHTS, m_nVerticesCount );
}



// 设置当前的lod级别
void CAIRSubMesh::SetCurLodLevel( u16 nCurLodLevel )
{
	m_nCurLodLevel = nCurLodLevel;
	if ( m_LodList.size() )
	{
		u16 curLod = AIR::Clamp<u16>( m_nCurLodLevel, 0, (m_LodList.size()-1) );
		m_pCurLodData = m_LodList[curLod];
	}
}

void CAIRSubMesh::Clear()
{
    ClearLodList();
}

void CAIRSubMesh::ClearLodList( void )
{	
	IRenderer* pRenderer = ENGINE_INST->GetRenderer();

	m_pCurLodData = NULL;

	// 没有创建lod信息
	if ( m_LodList.size() == 0 )
	{
		return;
	}

	// 释放第0层的原始网格数据
	LodData* pCurLodData = m_LodList[0];

	// 释放索引缓冲
	if ( pCurLodData->m_VertexData.m_pMeshIB )
	{
		pRenderer->ClearIndexBuffer( pCurLodData->m_VertexData.m_pMeshIB );
	}
	// 释放顶点缓冲
	if ( pCurLodData->m_VertexData.m_pMeshVB )
	{
		pRenderer->ClearVertexBuffer( pCurLodData->m_VertexData.m_pMeshVB );
	}
	// 释放三角面列表
	//if ( pCurLodData->m_pvtFaces )
	{
		pCurLodData->m_pvtFaces.clear();
		//SafeDelete( pCurLodData->m_pvtFaces ); 
	}
	// 释放硬件蒙皮顶点列表
	//if ( pCurLodData->m_pvtHardwareBlendVertex )
	{
		for ( size_t i = 0; i < pCurLodData->m_pvtHardwareBlendVertex.size(); ++i )
		{
			AIR_DELETE_T( ((pCurLodData->m_pvtHardwareBlendVertex))[i], HARDWAREMESHVERTEX );
		}
		pCurLodData->m_pvtHardwareBlendVertex.clear();
		//SafeDelete( pCurLodData->m_pvtHardwareBlendVertex );
	}
	// 释放软件蒙皮顶点列表
	//if ( pCurLodData->m_pvtSoftwareBlendVertex )
	{
		for ( size_t i = 0; i < pCurLodData->m_pvtSoftwareBlendVertex.size(); ++i )
		{
			AIR_DELETE_T( ((pCurLodData->m_pvtSoftwareBlendVertex))[i], MESHVERTNORMAL );
		}
		pCurLodData->m_pvtSoftwareBlendVertex.clear();
		//SafeDelete( pCurLodData->m_pvtSoftwareBlendVertex );
	}
	// 释放权重数组
	//if ( pCurLodData->m_pvtSoftwareBlendWeight )
	//{
	//	pCurLodData->m_pvtSoftwareBlendWeight.clear();
		//SafeDelete( pCurLodData->m_pvtSoftwareBlendWeight );
	//}
	SafeDelete( pCurLodData );

	// 遍历第1层起的所有的lod层
	for ( size_t i = 1; i < m_LodList.size(); ++i )
	{
		pCurLodData = m_LodList[i];

		// 释放索引缓冲
		if ( pCurLodData->m_VertexData.m_pMeshIB )
		{
			pRenderer->ClearIndexBuffer( pCurLodData->m_VertexData.m_pMeshIB );
		}

		// 如果是创建独立数据的lod层
		if ( !ENGINE_INST->GetAutoLodShareVB() )
		{
			//释放顶点缓冲
			if ( pCurLodData->m_VertexData.m_pMeshVB )
			{
				pRenderer->ClearVertexBuffer( pCurLodData->m_VertexData.m_pMeshVB );
			}

			//释放三角面列表
			//if ( pCurLodData->m_pvtFaces )
			{
				pCurLodData->m_pvtFaces.clear();
				//SafeDelete( pCurLodData->m_pvtFaces ); 
			}

			//释放硬件蒙皮顶点列表
			//if ( pCurLodData->m_pvtHardwareBlendVertex )
			{
				for ( size_t i = 0; i < pCurLodData->m_pvtHardwareBlendVertex.size(); ++i )
				{
					AIR_DELETE_T( ((pCurLodData->m_pvtHardwareBlendVertex))[i], HARDWAREMESHVERTEX );
				}
				pCurLodData->m_pvtHardwareBlendVertex.clear();
				//SafeDelete( pCurLodData->m_pvtHardwareBlendVertex );
			}

			//释放软件蒙皮顶点列表
			//if ( pCurLodData->m_pvtSoftwareBlendVertex )
			{
				for ( size_t i = 0; i < pCurLodData->m_pvtSoftwareBlendVertex.size(); ++i )
				{
					AIR_DELETE_T( ((pCurLodData->m_pvtSoftwareBlendVertex))[i], MESHVERTNORMAL );
				}
				pCurLodData->m_pvtSoftwareBlendVertex.clear();
				//SafeDelete( pCurLodData->m_pvtSoftwareBlendVertex );
			}
		}

		SafeDelete( pCurLodData );
	}
	m_LodList.clear();
}


void CAIRSubMesh::CreateLodDataList( void )
{
	// 自动创建lod信息
	if ( ENGINE_INST->IsAutoGenLod() )
	{
		ProgressiveMesh pm( this );
		pm.Build( 
			ENGINE_INST->GetAutoLodLevelCount(), 
			m_LodList, 
			VertexReductionType( ENGINE_INST->GetAutoLodType() ), 
			ENGINE_INST->GetAutoLodParm() );
	}
	// 不创建lod信息
	else
	{
		// 设置默认的当前lod
		m_pCurLodData = m_LodList[0];
	}
}

LodData* CAIRSubMesh::GetLodData(size_t nLod)
{
    if (nLod < m_LodList.size())
    {
        return m_LodList[nLod];
    }

    return NULL;
}

void CAIRSubMesh::AddLodData(size_t nLod, LodData* pLodData)
{
    if (m_LodList.size() < nLod)
    {
        m_LodList.push_back(pLodData);
    }
    else
    {
        if (m_LodList[nLod] != pLodData)
        {
			// 先释放原来的lod数据
			ClearLodData( nLod );

            // 再赋值新的lod数据
            m_LodList[nLod] = pLodData;
        }
    }
}


//清空第nLod层的lod数据
void CAIRSubMesh::ClearLodData( size_t nLod )
{
	if ( nLod >= m_LodList.size() )
	{
		DT_TO_MSGBOX( LT_WARN, "Invalid lod id!" );
		return;
	}

	if ( nLod == 0 )
	{
		DT_TO_MSGBOX( LT_WARN, "Clear lod0 is denied!" );
		return;
	}

	IRenderer* pRenderer = ENGINE_INST->GetRenderer();
	LodData* pLod = m_LodList[ nLod ];
	if ( pLod )
	{
		// 释放索引缓冲
		if ( pLod->m_VertexData.m_pMeshIB )
		{
			pRenderer->ClearIndexBuffer( pLod->m_VertexData.m_pMeshIB );
		}

		// 释放以独立数据形式创建的lod数据
		if ( !ENGINE_INST->GetAutoLodShareVB() )
		{
			// 释放顶点缓冲
			if ( pLod->m_VertexData.m_pMeshVB )
			{
				pRenderer->ClearVertexBuffer( pLod->m_VertexData.m_pMeshVB );
			}
			// 释放三角面列表
			//if ( pLod->m_pvtFaces )
			{
				pLod->m_pvtFaces.clear();
				//SafeDelete( pLod->m_pvtFaces ); 
			}
			// 释放硬件蒙皮顶点列表
			//if ( pLod->m_pvtHardwareBlendVertex )
			{
				for ( size_t i = 0; i < pLod->m_pvtHardwareBlendVertex.size(); ++i )
				{
					//SafeDelete( ((pLod->m_pvtHardwareBlendVertex))[i] );
					AIR_DELETE_T( ((pLod->m_pvtHardwareBlendVertex))[i], HARDWAREMESHVERTEX );
				}
				pLod->m_pvtHardwareBlendVertex.clear();
				//SafeDelete( pLod->m_pvtHardwareBlendVertex );
			}
			// 释放软件蒙皮顶点列表
			//if ( pLod->m_pvtSoftwareBlendVertex )
			{
				for ( size_t i = 0; i < pLod->m_pvtSoftwareBlendVertex.size(); ++i )
				{
					//SafeDelete( ((pLod->m_pvtSoftwareBlendVertex))[i] );
					AIR_DELETE_T( ((pLod->m_pvtSoftwareBlendVertex))[i], MESHVERTNORMAL );
				}
				pLod->m_pvtSoftwareBlendVertex.clear();
				//SafeDelete( pLod->m_pvtSoftwareBlendVertex );
			}
		}
		SafeDelete( pLod );

		m_LodList[ nLod ] = NULL;
	}
}

IVertexBuffer* CAIRSubMesh::GetVertexBuffer()
{
    return m_pCurLodData->m_VertexData.m_pMeshVB;
}

IIndexBuffer* CAIRSubMesh::GetIndexBuffer()
{
    return m_pCurLodData->m_VertexData.m_pMeshIB;
}

void CAIRSubMesh::CaculateKeyframeVertices(f32 fTimePos)
{
    if (m_pVertexAnimationRes)
    {
        VertexAnimation* pAnimation = m_pVertexAnimationRes->GetVertexAnimation();
        pAnimation->ApplyToSubmesh(this, fTimePos);
    }
}

void CAIRSubMesh::CacheVertexBuffer()
{
    for (size_t i = 0; i < m_LodList.size(); ++i)
    {
        if (!m_LodList[i]->m_pvtSoftwareBlendVertex.empty())
        {
            ComputeVertexBuffer(&m_LodList[i]->m_pvtSoftwareBlendVertex);
        }
        
        if (!m_LodList[i]->m_pvtHardwareBlendVertex.empty())
        {
            ComputeVertexBuffer(&m_LodList[i]->m_pvtHardwareBlendVertex);
        }
    }
    
}

bool CAIRSubMesh::Create(LPVERTEX_NORMAL pVertices, u32 nVerticesNum, u16* pIndices, u32 nIndicesNum)
{
	return true;
}

bool CAIRSubMesh::Create(LPMESHVERTNORMAL pVertices, u32 nVerticesNum, u16* pIndices, u32 nIndicesNum)
{
	//创建第0层的lod数据对象,pLod0是原始网格数据
	LodData* pLod0 = new LodData();

	m_LodList.push_back( pLod0 );
	m_pCurLodData = pLod0;

	m_nVerticesCount = nVerticesNum;

	m_boundingBox.MaxEdge = Vector3Df(-99999, -99999, -99999);
	m_boundingBox.MinEdge = Vector3Df(99999, 99999, 99999);

    if (m_nVerticesCount > 0)
    {
		if (ENGINE_INST->IsHardwareSkeleton())
		{
			pLod0->m_pvtHardwareBlendVertex.resize(m_nVerticesCount);
		}
		else
		{
			pLod0->m_pvtSoftwareBlendVertex.resize( m_nVerticesCount );
		}

        CreateVertices(m_nVerticesCount);
    }
    else
    {
		DT_TO_MSGBOX( LT_WARN, GetString("Mesh [%s] has 0 vertex!", m_strName.c_str()) );
        return false;
    }

    //读取顶点数据
    float fdata[5];
    u32 color = 0xffffffff;
    //int   nWeight[2];
	u8 boneID[4] = { 0 };
	float fWeights[4] = { 0 };

	AIRVector<Vector3Df>  poins;
    for (int i = 0; i < m_nVerticesCount; ++i)
    {
        if (ENGINE_INST->IsHardwareSkeleton())
        {
            LPHARDWAREMESHVERTEX pVertex = AIR_NEW_T (HARDWAREMESHVERTEX);
            memset(pVertex, 0, sizeof(HARDWAREMESHVERTEX));
            pVertex->vertex.vertex.vPos = pVertices[i].vertex.vPos;
            pVertex->vertex.vertex.color = pVertices[i].vertex.color;
            pVertex->vertex.vertex.tu = pVertices[i].vertex.tu;
            pVertex->vertex.vertex.tv = pVertices[i].vertex.tv;
			for (int j = 0; j < 4; ++j)
			{
				pVertex->vertex.boneIndex[j] = pVertices[i].boneID[j]; //boneID[j];
				pVertex->vertex.weights[j] = pVertices[i].fWeights[j]; //fWeights[j];
			}
            //pVertex->nWeightIndex = nWeight[0];
            //pVertex->nWeightCount = nWeight[1];
			((pLod0->m_pvtHardwareBlendVertex))[i] = pVertex;
        }
        else
        {
            LPMESHVERTNORMAL pVertex = AIR_NEW_T (MESHVERTNORMAL);
            *pVertex = pVertices[i];
			((pLod0->m_pvtSoftwareBlendVertex))[i] = pVertex;
        }
        poins.push_back(pVertices[i].vertex.vPos);

		m_boundingBox.MaxEdge.x = Max(m_boundingBox.MaxEdge.x, pVertices[i].vertex.vPos.x);
		m_boundingBox.MaxEdge.y = Max(m_boundingBox.MaxEdge.y, pVertices[i].vertex.vPos.y);
		m_boundingBox.MaxEdge.z = Max(m_boundingBox.MaxEdge.z, pVertices[i].vertex.vPos.z);

		m_boundingBox.MinEdge.x = Min(m_boundingBox.MinEdge.x, pVertices[i].vertex.vPos.x);
		m_boundingBox.MinEdge.y = Min(m_boundingBox.MinEdge.y, pVertices[i].vertex.vPos.y);
		m_boundingBox.MinEdge.z = Min(m_boundingBox.MinEdge.z, pVertices[i].vertex.vPos.z);
    }
	m_obb.compute_obbox(poins.begin(),poins.end());

    m_nFacesCount = nIndicesNum / 3;

	if ( m_nFacesCount <= 0 )
	{
		DT_TO_MSGBOX( LT_WARN, GetString("Mesh [%s] has 0 face!", m_strName.c_str()) );
		return false;
	}

	pLod0->m_pvtFaces.resize( m_nFacesCount );
    CreateIndices(m_nFacesCount);
	//pFileStream->Read(&((pLod0->m_pvtFaces))[0], pLod0->m_pvtFaces.size() * sizeof(AIRMESHTRI));
	for (s32 i = 0; i < m_nFacesCount; ++i)
	{
		u32 nIndices = i * 3;
		pLod0->m_pvtFaces[i].nIndex[0] = pIndices[nIndices];
		pLod0->m_pvtFaces[i].nIndex[1] = pIndices[nIndices + 1];
		pLod0->m_pvtFaces[i].nIndex[2] = pIndices[nIndices + 2];
	}
    
    //读取boundingbox信息
    //pFileStream->Read(&m_boundingBox, sizeof(float) * 6);

    
    if (ENGINE_INST->IsHardwareSkeleton())
    {
		//计算每个顶点的权重
		ComputeVertexNormal(&(pLod0->m_pvtHardwareBlendVertex), &(pLod0->m_pvtFaces));
		ComputeVertexBuffer(&(pLod0->m_pvtHardwareBlendVertex));
    }
    else
    {
		//计算Normal
		ComputeVertexNormal(&(pLod0->m_pvtSoftwareBlendVertex), &(pLod0->m_pvtFaces));
		//填充VB
		ComputeVertexBuffer(&(pLod0->m_pvtSoftwareBlendVertex));
    }

	// 创建索引缓冲
	ComputeIndexBuffer( &(pLod0->m_pvtFaces) );

	// 创建网格lod信息
	CreateLodDataList();

	// 创建顶点的法线顶点缓冲
	if ( m_bGenNormalVB )
	{
		GenNormalVB();
	}

	return true;
}
