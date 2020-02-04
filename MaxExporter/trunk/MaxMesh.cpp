#include "MaxMesh.h"
#include <modstack.h>
#include <cs/bipexp.h>
#include <cs/Phyexp.h>
#if (MAX_RELEASE <= MAX_RELEASE_R11)
#include "maxscrpt/maxscrpt.h"
#else
#include "maxscript/maxscript.h"
#endif

#include "iskin.h"
#include "AIRExporter.h"

DWORD GetColorAsARGB(float a, float r, float g, float b)
{
    unsigned char val8;
    DWORD val32 = 0;

    // Convert to 32bit pattern
    // (ARGB = 8888)

    // Alpha
    val8 = static_cast<unsigned char>(a * 255);
    val32 = val8 << 24;

    // Red
    val8 = static_cast<unsigned char>(r * 255);
    val32 += val8 << 16;

    // Green
    val8 = static_cast<unsigned char>(g * 255);
    val32 += val8 << 8;

    // Blue
    val8 = static_cast<unsigned char>(b * 255);
    val32 += val8;

    return val32;
}

//#include "Math/Plane3D.h"
//#include "Math/Matrix4.h"

int CMaxMesh::s_nNewCount = 0;

CMaxMesh::CMaxMesh() : m_pNode(NULL), m_pMesh(NULL), m_pModifier(NULL), m_triObjNeedDelete(NULL), m_bDelete(false), m_bMirrored(false)
, m_modifierType(MODIFIER_NONE)
, m_bHasVertexColor(false)
{
    s_nNewCount++;
}

CMaxMesh::~CMaxMesh()
{
    s_nNewCount--;
}

void CMaxMesh::Clear()
{
    if (m_triObjNeedDelete)
    {
        m_triObjNeedDelete->DeleteMe();
    }

    for (int i = 0; i < m_vtUsedMaterial.size(); i++)
    {
        CMaxMaterial* pMaterial = m_vtUsedMaterial[i];
        delete pMaterial;
        pMaterial = NULL;
    }

    m_vtUsedMaterial.clear();

    m_vectorStdMat.clear();
}

bool CMaxMesh::Create(INode *pINode, unsigned int iMaxTime)
{
    // check for valid mesh
    if(pINode == 0)
    {
        //theExporter.SetLastError("Invalid handle.", __FILE__, __LINE__);
        return false;
    }

    m_strName = pINode->GetName();

    Object* pObj;

    assert(pINode);
    pObj = pINode->GetObjectRef();

    if (pObj == NULL)
    {
        return false;
    }
    ULONG superClassID = pObj->SuperClassID();

    bool bFoundGeomObject = false;

    switch(superClassID)
    {
    case GEN_DERIVOB_CLASS_ID:
        {
            do{
                pObj = ((IDerivedObject*)pObj)->GetObjRef();
                superClassID = pObj->SuperClassID();
            }while( superClassID == GEN_DERIVOB_CLASS_ID );

            switch(superClassID)
            {
            case GEOMOBJECT_CLASS_ID:
                bFoundGeomObject = true;
                break;
            }
        }
        break;

    case GEOMOBJECT_CLASS_ID:
        {
            bFoundGeomObject = true;
        }
        break;
    default:
        break;
    }
    if (!bFoundGeomObject)
    {
        return false;
    }

    m_pNode = pINode;

    BuildIMesh(iMaxTime);

    //创建mesh中的材质
    BuildMaterials(m_pNode->GetMtl());
    
    //测试用
    
    m_tm = m_pNode->GetObjectTM(iMaxTime);

    

    //如果没有材质，无效
    if (m_vectorStdMat.size() == 0)
    {
        //AIREXPORTER->ShowErrorMsg("Mesh不含有任何材质！", __FILE__, __LINE__);
        return false;
    }

    // try to get the physique modifier

    m_pModifier = FindPhysiqueModifier();
    if(m_pModifier == 0)
    {
        // try to get the skin modifier
        m_pModifier = FindSkinModifier();
        if(m_pModifier == 0)
        {
            //theExporter.SetLastError("No physique/skin modifier found.", __FILE__, __LINE__);
            //return false;
        }
        else
        {
            m_modifierType = MODIFIER_SKIN;
        }
    }
    else
    {
        m_modifierType = MODIFIER_PHYSIQUE;
    }

    //如果是静态模型，用GetObjTMAfterWSM，骨骼动画则用GetObjectTM
    if (m_pModifier)
    {
        m_tm = m_pNode->GetObjectTM(iMaxTime);
    }
    else
        m_tm = m_pNode->GetObjectTM(iMaxTime);//m_pNode->GetObjTMAfterWSM(iMaxTime);   

    Matrix3 tmNode = m_pNode->GetNodeTM(iMaxTime);
    Matrix3 tmObject = m_pNode->GetObjectTM(iMaxTime);
    Matrix3 tmObjWSM = m_pNode->GetObjTMAfterWSM(iMaxTime);

    Point3	r1, r2, r3, r12;
    float					Mirror;
    r1 = m_tm.GetRow(0);
    r2 = m_tm.GetRow(1);
    r3 = m_tm.GetRow(2);
    r12 = CrossProd(r1, r2);//法向量计算
    Mirror = DotProd(r12, r3);//角度计算
    m_bMirrored = Mirror < 0;

    if (AIREXPORTER->IsLeftHand() && AIREXPORTER->IsYUp())
    {
        m_tm = AIREXPORTER->GetMaxInterface()->ConvertToDXMatrix(m_tm);
    }
    else if (AIREXPORTER->IsYUp())
    {
        m_tm = AIREXPORTER->GetMaxInterface()->ConvertToYUpMatrix(m_tm);
        //m_tm = AIREXPORTER->GetMaxInterface()->ConvertToDXMatrix(m_tm);
    }

    return true;
}

bool CMaxMesh::CreateAnimatedMesh(INode* pINode, unsigned int iMaxTime)
{
    if(pINode == 0)
    {
        //theExporter.SetLastError("Invalid handle.", __FILE__, __LINE__);
        return false;
    }

    m_strName = pINode->GetName();

    Object* pObj;

    assert(pINode);
    pObj = pINode->GetObjectRef();

    if (pObj == NULL)
    {
        return false;
    }
    ULONG superClassID = pObj->SuperClassID();

    bool bFoundGeomObject = false;

    switch(superClassID)
    {
    case GEN_DERIVOB_CLASS_ID:
        {
            do{
                pObj = ((IDerivedObject*)pObj)->GetObjRef();
                superClassID = pObj->SuperClassID();
            }while( superClassID == GEN_DERIVOB_CLASS_ID );

            switch(superClassID)
            {
            case GEOMOBJECT_CLASS_ID:
                bFoundGeomObject = true;
                break;
            }
        }
        break;

    case GEOMOBJECT_CLASS_ID:
        {
            bFoundGeomObject = true;
        }
        break;
    default:
        break;
    }
    if (!bFoundGeomObject)
    {
        return false;
    }

    m_pNode = pINode;

    BuildAnimatedIMesh(iMaxTime);

    //创建mesh中的材质
    BuildMaterials(m_pNode->GetMtl());

    //测试用

    m_tm = m_pNode->GetObjectTM(iMaxTime);



    //如果没有材质，无效
    if (m_vectorStdMat.size() == 0)
    {
        //AIREXPORTER->ShowErrorMsg("Mesh不含有任何材质！", __FILE__, __LINE__);
        return false;
    }

    // try to get the physique modifier

    m_pModifier = FindPhysiqueModifier();
    if(m_pModifier == 0)
    {
        // try to get the skin modifier
        m_pModifier = FindSkinModifier();
        if(m_pModifier == 0)
        {
            //theExporter.SetLastError("No physique/skin modifier found.", __FILE__, __LINE__);
            //return false;
        }
        else
        {
            m_modifierType = MODIFIER_SKIN;
        }
    }
    else
    {
        m_modifierType = MODIFIER_PHYSIQUE;
    }

    //如果是静态模型，用GetObjTMAfterWSM，骨骼动画则用GetObjectTM
    if (m_pModifier)
    {
        m_tm = m_pNode->GetObjectTM(iMaxTime);
    }
    else
        m_tm = m_pNode->GetObjectTM(iMaxTime);//m_pNode->GetObjTMAfterWSM(iMaxTime);   

    Matrix3 tmNode = m_pNode->GetNodeTM(iMaxTime);
    Matrix3 tmObject = m_pNode->GetObjectTM(iMaxTime);
    Matrix3 tmObjWSM = m_pNode->GetObjTMAfterWSM(iMaxTime);

    Point3	r1, r2, r3, r12;
    float					Mirror;
    r1 = m_tm.GetRow(0);
    r2 = m_tm.GetRow(1);
    r3 = m_tm.GetRow(2);
    r12 = CrossProd(r1, r2);//法向量计算
    Mirror = DotProd(r12, r3);//角度计算
    m_bMirrored = Mirror < 0;

    if (AIREXPORTER->IsLeftHand() && AIREXPORTER->IsYUp())
    {
        m_tm = AIREXPORTER->GetMaxInterface()->ConvertToDXMatrix(m_tm);
    }
    else if (AIREXPORTER->IsYUp())
    {
        m_tm = AIREXPORTER->GetMaxInterface()->ConvertToYUpMatrix(m_tm);
        //m_tm = AIREXPORTER->GetMaxInterface()->ConvertToDXMatrix(m_tm);
    }

    return true;
}

int CMaxMesh::GetFaceCount() const
{
    if (!m_pMesh)
    {
        return 0;
    }

    return m_pMesh->getNumFaces();
}

int CMaxMesh::GetMaterialCount() const
{
    return m_vectorStdMat.size();
}

int CMaxMesh::GetSubMeshMapCount(int nMaterialIndex)
{
    if (nMaterialIndex < 0 || nMaterialIndex >= m_vectorStdMat.size())
    {
        return -1;
    }
    StdMat* pStdMat = m_vectorStdMat[nMaterialIndex];

    int nMapCount = 0;

    for (int mapID = 0; mapID < pStdMat->NumSubTexmaps(); mapID++)
    {
        Texmap *pTexMap = pStdMat->GetSubTexmap(mapID);

        // check if map is valid
        if((pTexMap != 0) && (pStdMat->MapEnabled(mapID)))
        {
            // check if we have a valid texture coordinate
            if((m_pMesh->mapSupport(pTexMap->GetMapChannel())) || (m_pMesh->numTVerts > 0))
            {
                nMapCount++;
            }
        }
    }

    return nMapCount;
}

/*
bool CMaxMesh::CreateMaterial(Mtl *pMtl)
{
    // check for valid material
    if(pMtl == 0)
    {
        //theExporter.SetLastError("Invalid handle.", __FILE__, __LINE__);
        return false;
    }

    // check if we have a standard material
    if(pMtl->ClassID() == Class_ID(DMTL_CLASS_ID, 0))
    {
        // insert new material
        m_vectorStdMat.push_back((StdMat *)pMtl);
    }

    // handle all submaterials
    int subId;
    for(subId = 0; subId < pMtl->NumSubMtls(); subId++)
    {
        CreateMaterial(pMtl->GetSubMtl(subId));
    }

    return true;
}
*/

bool CMaxMesh::BuildIMesh(unsigned int iMaxTime)
{
    
    ObjectState os = m_pNode->EvalWorldState(0); 

    Object* pObj = os.obj;
    TriObject* tri = NULL;
    BOOL bNeedDelete = false;
    // The obj member of ObjectState is the actual object we will export.
    if (pObj) 
    {
        if (pObj->CanConvertToType(Class_ID(TRIOBJ_CLASS_ID, 0))) 
        { 
            tri = (TriObject *) pObj->ConvertToType(iMaxTime, //原来这里是0，由liangairan改为iMaxTime at 2010-7-27
                Class_ID(TRIOBJ_CLASS_ID, 0));
            // Note that the TriObject should only be deleted
            // if the pointer to it is not equal to the object
            // pointer that called ConvertToType()
            if(tri == NULL)
            {
                NullView view;
                GeomObject* pGeoObj = (GeomObject*)pObj;
                m_pMesh = pGeoObj->GetRenderMesh( iMaxTime , m_pNode , view , bNeedDelete );
            }
            else
            {
                //pMesh = &triObject->GetMesh();
                //this->m_bDelete = false;
                NullView view;
                GeomObject* pGeoObj = (GeomObject*)pObj;
                m_pMesh = pGeoObj->GetRenderMesh( iMaxTime , m_pNode , view , bNeedDelete );
                //this->m_bDelete = false;
            }
        }
        //需要删除的指针
        if(tri != pObj)
            m_triObjNeedDelete = tri;
    }

    //在这里判断是否有顶点颜色
    m_bHasVertexColor = m_pMesh->vcFace != NULL && m_pMesh->vertCol != NULL;

    return true;
}

bool CMaxMesh::BuildAnimatedIMesh(unsigned int iMaxTime)
{
    ObjectState os = m_pNode->EvalWorldState(iMaxTime); 
    Object* pObj = os.obj;
    TriObject* tri = NULL;
    BOOL bNeedDelete = false;
    // The obj member of ObjectState is the actual object we will export.
    if (pObj) 
    {
        if (pObj->CanConvertToType(Class_ID(TRIOBJ_CLASS_ID, 0))) 
        { 
            TriObject *tri = (TriObject *) pObj->ConvertToType(iMaxTime, 
                Class_ID(TRIOBJ_CLASS_ID, 0));
            // Note that the TriObject should only be deleted
            // if the pointer to it is not equal to the object
            // pointer that called ConvertToType()
            if (pObj != tri) 
            {
                m_triObjNeedDelete = tri;
            }
            if (tri)
            {
                m_pMesh = &tri->GetMesh();
            }
            else
            {
                return false;
            }
        }
    }
    return true;
}

bool CMaxMesh::BuildMaterials(Mtl* pMat)
{
    //Mtl* pMat = m_pNode->GetMtl();
    if (pMat == NULL)
    {
        return false;
    }

    //判断是否标准材质
    if (pMat->ClassID() == Class_ID(DMTL_CLASS_ID, 0))
    {
        m_vectorStdMat.push_back((StdMat*)pMat);
        CMaxMaterial* pMaterial = new CMaxMaterial;
        if (pMaterial->Create((StdMat*)pMat))
        {
            pMaterial->m_nMatID = m_vectorStdMat.size() - 1;  //数组下标
            
            m_vtUsedMaterial.push_back(pMaterial);
        }
        else
        {
            delete pMaterial;
        }
    }

    // 处理所有的子ID;
    int subId;
    for(subId = 0; subId < pMat->NumSubMtls(); subId++)
    {
        BuildMaterials(pMat->GetSubMtl(subId));
    }
    return true;
}

int CMaxMesh::FindMaxMatBySTDMatID(int nID)
{
    nID = nID % (int)m_vectorStdMat.size();
    size_t nMat = m_vtUsedMaterial.size();
    for(size_t i = 0 ; i < nMat ; ++i)
    {
        CMaxMaterial* pMaterial = m_vtUsedMaterial[i];
        //sMaxMatrial_t& mat = m_MeshData.m_Materials[i]; 
        if(pMaterial->m_nMatID == nID)
            return (int)i;
    }
    return -1;
}

bool CMaxMesh::BuildFaceInfo()
{
    Mtl* pMat =  m_pNode->GetMtl();
    if (pMat == NULL)
    {
        //没有用到材质，不合法
        return false;
    }

    int nMat = (int)m_vtUsedMaterial.size();
    int faceSum = m_pMesh->getNumFaces();

    typedef vector<MAXFACE> FaceList;
    FaceList* pFaceList = new FaceList[nMat];
    if(m_pMesh->faces == NULL)
    {
        //CMaxEnv::GetInstance().m_log<<"   {error}["<<m_pNode->GetName()<<"] faces指针为空 \r\n";
        return false;
    }
    if(m_pMesh->tvFace == NULL)
    {
        //CMaxEnv::GetInstance().m_log<<"   {error}["<<m_pNode->GetName()<<"] tvfaces指针为空 \r\n";
        return false;
    }

    //构造一个分组，就是把面按材质分组
    for(int iFace = 0 ; iFace < faceSum ; iFace ++)
    {

        Face&   vface =   m_pMesh->faces[iFace];
        TVFace& tface =   m_pMesh->tvFace[iFace];
        //NFace&  nrlface = m_NormalFaces[iFace];

        MAXFACE max_face;

        //max_face.normal[0] = nrlface.normal[0];
        //max_face.normal[1] = nrlface.normal[1];
        //max_face.normal[2] = nrlface.normal[2];

        if(m_pMesh->vcFace)
        {
            TVFace& cface = m_pMesh->vcFace[iFace];
            max_face.color[0] = cface.t[0];
            max_face.color[1] = cface.t[1];
            max_face.color[2] = cface.t[2];
        }
        else
        {
            max_face.color[0] = 0;
            max_face.color[1] = 0;
            max_face.color[2] = 0;
        }


        max_face.uv[0] = tface.t[0];
        max_face.uv[1] = tface.t[1];
        max_face.uv[2] = tface.t[2];

        max_face.vert[0] = vface.v[0];
        max_face.vert[1] = vface.v[1];
        max_face.vert[2] = vface.v[2];

        max_face.faceIndex = iFace;
        max_face.matID = vface.getMatID() % (int)m_vectorStdMat.size();

        //找到这个Face用的是第几个mat
        int matIndex = FindMaxMatBySTDMatID(max_face.matID);//__find_mat_by_matid(face->matID%m_vStdMtls.size());
        //把面加入到对应的材质分组里
        pFaceList[matIndex].push_back(max_face);
    }

    //拆分顶点到不同的材质

    //遍历所有的材质．
    for(int i = 0 ; i < nMat; ++i)
    {
        CMaxMaterial* pMaterial = m_vtUsedMaterial[i];
        //sMaxMatrial_t& mat = m_MeshData.m_Materials[i];
        FaceList& faceList = pFaceList[i];

        /*
        CAIRStaticMesh* pMesh = new CAIRStaticMesh;

        //取出这个材质的面的信息，
        //并构造IndexBuffer
        int nface = (int)faceList.size();
        //IndexBuffer_t pIndexBuffer = new sFace_t[nface];
        for(int j = 0 ; j <  nface; ++j)
        {
            MAXFACE& face = faceList[j];
            AIRMESHTRI airFace;
            memcpy(airFace.nIndex, face.vert, sizeof(airFace.nIndex));;
            pMesh->AddFace(airFace);
            //sMaxFace_t&  face = faceList[j];
            //pIndexBuffer[j] = __add_face(face);
        }

        m_vtExportMeshes.push_back(pMesh);
        */

        /*
        //第i个SubMesh和第i个Material是对应的
        //这里我们填写这个Material对应的SubMesh的信息
        m_MeshData.m_pMeshDesc->m_SubMesh[i].m_nFace = nface;
        m_MeshData.m_pMeshDesc->m_SubMesh[i].m_nMatIndex = i;//第几个Material
        //把构造好的IndexBuffer加入到IndexBuffer列表中
        //m_IndexBuffers中的第i个IndexBuffer和第i个SubMesh或者第i个Material都是相对应的．
        m_MeshData.m_IndexBuffers.push_back(pIndexBuffer);

        //构造第i个Material的一些附加信息：Face的个数，TextureLayer的个数
        m_MeshData.m_Materials[i].m_nTexLayer = (int)m_MeshData.m_Materials[i].m_TexLayers.size();
        */
    }
    //m_MeshData.m_pMeshDesc->m_nFace = faceSum;
    delete [] pFaceList;

    return true;
}

int CMaxMesh::GetFaceMaterialId(int faceId)
{
    // check for valid mesh
    if(m_pMesh == 0)
    {
        //theExporter.SetLastError("Invalid handle.", __FILE__, __LINE__);
        return -1;
    }

    // get the material id of the face
    int materialId;
    materialId = m_pMesh->getFaceMtlIndex(faceId);

    // get the material count of this mesh
    int materialCount;
    materialCount = m_vectorStdMat.size();

    // make sure we return a valid material id in all cases:
    // 3ds max assigns predefined material ids to primitives. these material
    // ids can be higher than the actual material count of the mesh itself.
    // the official workaround is to calculate the modulo of the value to the
    // number of existing materials.
    if(materialCount > 0)
    {
        materialId %= materialCount;
    }
    else
    {
        materialId = 0;
    }

    return materialId;
}

StdMat* CMaxMesh::GetStdMtl(int nMatID)
{
    if (nMatID < 0 || nMatID >= m_vectorStdMat.size())
    {
        //AfxMessageBox("获得标准材质的matID不合法！");
        return NULL;
    }

    return m_vectorStdMat[nMatID];
}

CAIRVertex* CMaxMesh::GetAIRVertex(CAIRSkeleton* pSkeleton, int nFaceID, int nFaceVertexIndex)
{
    if (nFaceID < 0 || nFaceID >= this->GetFaceCount())
    {
        return NULL;
    }

    if (nFaceVertexIndex < 0 || nFaceVertexIndex > 2)
    {
        return NULL;
    }

    //Mesh* pMesh = pMaxMesh->GetIMesh();

    //获得在mesh中的数组下标
    int nVertexID = m_pMesh->faces[nFaceID].v[nFaceVertexIndex];

    Point3 vertex;
    vertex = m_pMesh->getVert(nVertexID);

    m_pMesh->getNormal(nVertexID);

    Matrix3 tmReflect;
    tmReflect.IdentityMatrix();
    if (m_bMirrored)
    {
        //约定是YZ作为反射面
        
        tmReflect.Scale(Point3(-1, 1, 1));
        
    }

    if (AIREXPORTER->IsLeftHand() && AIREXPORTER->IsYUp())
    {
        float t = vertex.y;
        vertex.y = vertex.z;
        vertex.z = t;

        AIREXPORTER->GetMaxInterface()->ConvertToDXMatrix(tmReflect);
    }
    else if (AIREXPORTER->IsYUp())
    {
        float t = vertex.y;
        vertex.y = vertex.z;
        vertex.z = t;
        vertex.x = -vertex.x;
        AIREXPORTER->GetMaxInterface()->ConvertToYUpMatrix(tmReflect);
        //AIREXPORTER->GetMaxInterface()->ConvertToDXMatrix(tmReflect);
    }
    
    vertex = vertex * m_tm;   //可能要旋转到以Y轴为坐标的空间，实际上是把Y坐标与Z坐标交换

    

    CAIRVertex* pVertex = new CAIRVertex;

    //VertColor vertexColor = m_pMesh->vertCol[nVertexID];
    //VertexDataDefault(VDATA_ALPHA);

    pVertex->SetPosition(vertex.x, vertex.y, vertex.z);

    //获得颜色
    if (m_bHasVertexColor)
    {
        int nColorIndex = m_pMesh->vcFace[nFaceID].t[nFaceVertexIndex];
        TVFace* alphaFaces = m_pMesh->mapFaces(MAP_ALPHA);
        float alpha = 1.0f;
        if (alphaFaces != NULL)
        {
            int nAlphaIndex = alphaFaces[nFaceID].t[nFaceVertexIndex];

            UVVert* pAlphaVerts = m_pMesh->mapVerts(MAP_ALPHA);
            alpha = pAlphaVerts[nAlphaIndex].x;
        }
        
        VertColor color = m_pMesh->vertCol[nColorIndex];

        pVertex->SetColor(alpha, color.x, color.y, color.z);
    }

    //以下代码是用于获得权值，稍后再写


    //获得该顶点的所有纹理坐标
    int materialId = GetFaceMaterialId(nFaceID);
    if((materialId < 0) || (materialId >= m_vectorStdMat.size()))
    {
        delete pVertex;
        //delete pVertexCandidate;
        //theExporter.SetLastError("Invalid material id found.", __FILE__, __LINE__);
        return NULL;
    }

    // get the material of the face
    StdMat *pStdMat;
    pStdMat = m_vectorStdMat[materialId];

    // loop through all the mapping channels and extract texture coordinates 这个版本只取Diffuse和Opacity的纹理，顶点纹理坐标只有一个
    int mapId;
    int nNumSubMap = pStdMat->NumSubTexmaps();
    //取Diffuse纹理
    Texmap *pTexMap = pStdMat->GetSubTexmap(ID_DI);

    std::string mapName;

    if (pTexMap != NULL && pStdMat->MapEnabled(ID_DI))
    {
        mapName = pTexMap->GetName();
        int channel;
        channel = pTexMap->GetMapChannel();

        bool bValidUV;
        bValidUV = false;

        // extract the texture coordinate
        UVVert uvVert;
        if(m_pMesh->mapSupport(channel))
        {
            TVFace *pTVFace;
            pTVFace = m_pMesh->mapFaces(channel);

            UVVert *pUVVert;
            pUVVert = m_pMesh->mapVerts(channel);

            //这里非常重要，UV的值等于了面的第nFaceVertexIndex个顶点的UV
            uvVert = pUVVert[pTVFace[nFaceID].t[nFaceVertexIndex]];
            bValidUV = true;
        }
        else if(m_pMesh->numTVerts > 0)
        {
            uvVert = m_pMesh->tVerts[m_pMesh->tvFace[nFaceID].t[nFaceVertexIndex]];
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
            CAIRVertex::TextureCoordinate texCoord;
            texCoord.u = uvVert.x;
            texCoord.v = uvVert.y;
            if (AIREXPORTER->IsLeftHand())
                texCoord.v = 1.0f - uvVert.y;
            else if (AIREXPORTER->IsYUp())
                texCoord.v = 1.0f - uvVert.y;
            pVertex->AddTextureCoordinate(texCoord);
        }
    }

    pTexMap = pStdMat->GetSubTexmap(ID_OP);
    if (pTexMap != NULL)
    {
        /*
        if (pTexMap->GetName() != mapName)
        {
            //不合格式
        }
        */
    }

    if (pVertex->GetTexCoordinateCount() == 0)
    {
        for(mapId = 0; mapId < nNumSubMap; mapId++)
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
                if(m_pMesh->mapSupport(channel))
                {
                    TVFace *pTVFace;
                    pTVFace = m_pMesh->mapFaces(channel);

                    UVVert *pUVVert;
                    pUVVert = m_pMesh->mapVerts(channel);

                    uvVert = pUVVert[pTVFace[nFaceID].t[nFaceVertexIndex]];
                    bValidUV = true;
                }
                else if(m_pMesh->numTVerts > 0)
                {
                    uvVert = m_pMesh->tVerts[m_pMesh->tvFace[nFaceID].t[nFaceVertexIndex]];
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
                    CAIRVertex::TextureCoordinate texCoord;
                    texCoord.u = uvVert.x;
                    texCoord.v = uvVert.y;
                    if (AIREXPORTER->IsLeftHand())
                        texCoord.v = 1.0f - uvVert.y;
                    else if (AIREXPORTER->IsYUp())
                        texCoord.v = 1.0f - uvVert.y;
                    pVertex->AddTextureCoordinate(texCoord);
                    break;
                }
            }
        }
    }
    
    

    if (m_modifierType == MODIFIER_PHYSIQUE)
    {
        // create a physique export interface
        IPhysiqueExport *pPhysiqueExport;
        pPhysiqueExport = (IPhysiqueExport *)m_pModifier->GetInterface(I_PHYINTERFACE);
        if(pPhysiqueExport == 0)
        {
            AIREXPORTER->ShowErrorMsg("Physique modifier interface not found.", __FILE__, __LINE__);
            return NULL;
        }

        // create a context export interface
        IPhyContextExport *pContextExport;
        pContextExport = (IPhyContextExport *)pPhysiqueExport->GetContextInterface(m_pNode);
        if(pContextExport == NULL)
        {
            m_pModifier->ReleaseInterface(I_PHYINTERFACE, pPhysiqueExport);
            
            AIREXPORTER->ShowErrorMsg("Context export interface not found.", __FILE__, __LINE__);
            return NULL;
        }

        // set the flags in the context export interface
        pContextExport->ConvertToRigid(TRUE);
        pContextExport->AllowBlending(TRUE);

        // get the vertex export interface
        IPhyVertexExport *pVertexExport;
        pVertexExport = (IPhyVertexExport *)pContextExport->GetVertexInterface(nVertexID);
        if(pVertexExport == NULL)
        {
            pPhysiqueExport->ReleaseContextInterface(pContextExport);
            m_pModifier->ReleaseInterface(I_PHYINTERFACE, pPhysiqueExport);
            AIREXPORTER->ShowErrorMsg("Vertex export interface not found.", __FILE__, __LINE__);
            return NULL;
        }

        // get the vertex type
        int vertexType;
        vertexType = pVertexExport->GetVertexType();

        // handle the specific vertex type
        if(vertexType == RIGID_TYPE)
        {
            // typecast to rigid vertex
            IPhyRigidVertex *pTypeVertex;
            pTypeVertex = (IPhyRigidVertex *)pVertexExport;

            // add the influence to the vertex candidate
            // get the influencing bone
            if(!pVertex->AddBoneInfluence(pSkeleton, pTypeVertex->GetNode(), 1.0f))
            {
                pPhysiqueExport->ReleaseContextInterface(pContextExport);
                m_pModifier->ReleaseInterface(I_PHYINTERFACE, pPhysiqueExport);
            
                //AIREXPORTER->ShowErrorMsg("Invalid bone assignment.", __FILE__, __LINE__);
                return 0;
            }
        }
        else if(vertexType == RIGID_BLENDED_TYPE)
        {
            // typecast to blended vertex
            IPhyBlendedRigidVertex *pTypeVertex;
            pTypeVertex = (IPhyBlendedRigidVertex *)pVertexExport;

            // loop through all influencing bones
            int nodeId;
            for(nodeId = 0; nodeId < pTypeVertex->GetNumberNodes(); nodeId++)
            {
                // add the influence to the vertex candidate
                if(!pVertex->AddBoneInfluence(pSkeleton, pTypeVertex->GetNode(nodeId), pTypeVertex->GetWeight(nodeId)))
                {
                    pPhysiqueExport->ReleaseContextInterface(pContextExport);
                    m_pModifier->ReleaseInterface(I_PHYINTERFACE, pPhysiqueExport);

                    //AIREXPORTER->ShowErrorMsg("Invalid bone assignment.", __FILE__, __LINE__);
                    return 0;
                }
            }
        }

        // release all interfaces
        pPhysiqueExport->ReleaseContextInterface(pContextExport);
        m_pModifier->ReleaseInterface(I_PHYINTERFACE, pPhysiqueExport);
    }
    else if (m_modifierType == MODIFIER_SKIN)
    {
        ISkin *pSkin;
        pSkin = (ISkin*)m_pModifier->GetInterface(I_SKIN);
        if(pSkin == NULL)
        {
            AIREXPORTER->ShowErrorMsg("Skin modifier interface not found.", __FILE__, __LINE__);
            return NULL;
        }

        // create a skin context data interface
        ISkinContextData *pSkinContextData;
        pSkinContextData = (ISkinContextData *)pSkin->GetContextInterface(m_pNode);
        if(pSkinContextData == NULL)
        {
            m_pModifier->ReleaseInterface(I_SKIN, pSkin);
         
            AIREXPORTER->ShowErrorMsg("Skin context data interface not found.", __FILE__, __LINE__);
            return NULL;
        }

        // loop through all influencing bones
        int nodeId;
        for(nodeId = 0; nodeId < pSkinContextData->GetNumAssignedBones(nVertexID); nodeId++)
        {
            // get the bone id
            int boneId;
            boneId = pSkinContextData->GetAssignedBone(nVertexID, nodeId);
            if(boneId < 0) 
                continue;

            // add the influence to the vertex candidate
            if(!pVertex->AddBoneInfluence(pSkeleton, pSkin->GetBone(boneId), pSkinContextData->GetBoneWeight(nVertexID, nodeId)))
            {
                m_pModifier->ReleaseInterface(I_SKIN, pSkin);
                //_cprintf("");
                //AIREXPORTER->ShowErrorMsg("Invalid bone assignment.", __FILE__, __LINE__);
                return NULL;
            }
        }

        // release all interfaces
        m_pModifier->ReleaseInterface(I_SKIN, pSkin);
    }

    return pVertex;
}

bool CMaxMesh::GetVertexData(AIRTEXCOORD& texCoord, AIRPOSITION& position, DWORD& dwColor, int nFaceID, int nFaceVertexIndex)
{
    if (nFaceID < 0 || nFaceID >= this->GetFaceCount())
    {
        return false;
    }

    if (nFaceVertexIndex < 0 || nFaceVertexIndex > 2)
    {
        return false;
    }

    //Mesh* pMesh = pMaxMesh->GetIMesh();

    //获得在mesh中的数组下标
    int nVertexID = m_pMesh->faces[nFaceID].v[nFaceVertexIndex];

    Point3 vertex;
    vertex = m_pMesh->getVert(nVertexID);

	//int nNormalID = m_pMesh->faces[nFaceID].normal[nFaceVertexIndex];

    Matrix3 tmReflect;
    tmReflect.IdentityMatrix();
    if (m_bMirrored)
    {
        //约定是YZ作为反射面

        tmReflect.Scale(Point3(-1, 1, 1));

    }

    if (AIREXPORTER->IsLeftHand() && AIREXPORTER->IsYUp())
    {
        float t = vertex.y;
        vertex.y = vertex.z;
        vertex.z = t;

        AIREXPORTER->GetMaxInterface()->ConvertToDXMatrix(tmReflect);
    }
    else if (AIREXPORTER->IsYUp())
    {
        float t = vertex.y;
        vertex.y = vertex.z;
        vertex.z = t;
        vertex.x = -vertex.x;
        AIREXPORTER->GetMaxInterface()->ConvertToYUpMatrix(tmReflect);
        //AIREXPORTER->GetMaxInterface()->ConvertToDXMatrix(tmReflect);
    }

    vertex = vertex * m_tm;   //可能要旋转到以Y轴为坐标的空间，实际上是把Y坐标与Z坐标交换

    position.x = vertex.x;
    position.y = vertex.y;
    position.z = vertex.z;

    //CAIRVertex* pVertex = new CAIRVertex;

    //pVertex->SetPosition(vertex.x, vertex.y, vertex.z);

    //获得颜色
    if (m_bHasVertexColor)
    {
        int nColorIndex = m_pMesh->vcFace[nFaceID].t[nFaceVertexIndex];
        TVFace* alphaFaces = m_pMesh->mapFaces(MAP_ALPHA);
        float alpha = 1.0f;
        if (alphaFaces != NULL)
        {
            int nAlphaIndex = alphaFaces[nFaceID].t[nFaceVertexIndex];

            UVVert* pAlphaVerts = m_pMesh->mapVerts(MAP_ALPHA);
            alpha = pAlphaVerts[nAlphaIndex].x;
        }
        VertColor color = m_pMesh->vertCol[nColorIndex];

        //pVertex->SetColor(alpha, color.x, color.y, color.z);
        dwColor = GetColorAsARGB(alpha, color.x, color.y, color.z);
    }

    //以下代码是用于获得权值，稍后再写


    //获得该顶点的所有纹理坐标
    int materialId = GetFaceMaterialId(nFaceID);
    if((materialId < 0) || (materialId >= m_vectorStdMat.size()))
    {
        //delete pVertex;
        //delete pVertexCandidate;
        //theExporter.SetLastError("Invalid material id found.", __FILE__, __LINE__);
        return false;
    }

    // get the material of the face
    StdMat *pStdMat;
    pStdMat = m_vectorStdMat[materialId];

    // loop through all the mapping channels and extract texture coordinates 这个版本只取Diffuse和Opacity的纹理，顶点纹理坐标只有一个
    int mapId;
    int nNumSubMap = pStdMat->NumSubTexmaps();
    //取Diffuse纹理
    Texmap *pTexMap = pStdMat->GetSubTexmap(ID_DI);

    std::string mapName;

    bool bHasTextCoord = false;
    if (pTexMap != NULL && pStdMat->MapEnabled(ID_DI))
    {
        mapName = pTexMap->GetName();
        int channel;
        channel = pTexMap->GetMapChannel();

        bool bValidUV;
        bValidUV = false;

        // extract the texture coordinate
        UVVert uvVert;
        if(m_pMesh->mapSupport(channel))
        {
            TVFace *pTVFace;
            pTVFace = m_pMesh->mapFaces(channel);

            UVVert *pUVVert;
            pUVVert = m_pMesh->mapVerts(channel);

            //这里非常重要，UV的值等于了面的第nFaceVertexIndex个顶点的UV
            uvVert = pUVVert[pTVFace[nFaceID].t[nFaceVertexIndex]];
            bValidUV = true;
        }
        else if(m_pMesh->numTVerts > 0)
        {
            uvVert = m_pMesh->tVerts[m_pMesh->tvFace[nFaceID].t[nFaceVertexIndex]];
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
            //CAIRVertex::TextureCoordinate texCoord;
            texCoord.u = uvVert.x;
            texCoord.v = uvVert.y;
            if (AIREXPORTER->IsLeftHand())
                texCoord.v = 1.0f - uvVert.y;
            else if (AIREXPORTER->IsYUp())
                texCoord.v = 1.0f - uvVert.y;
            //pVertex->AddTextureCoordinate(texCoord);

            bHasTextCoord = true;
        }
    }

    pTexMap = pStdMat->GetSubTexmap(ID_OP);
    if (pTexMap != NULL)
    {
        /*
        if (pTexMap->GetName() != mapName)
        {
        //不合格式
        }
        */
    }

    if (!bHasTextCoord)
    {
        for(mapId = 0; mapId < nNumSubMap; mapId++)
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
                if(m_pMesh->mapSupport(channel))
                {
                    TVFace *pTVFace;
                    pTVFace = m_pMesh->mapFaces(channel);

                    UVVert *pUVVert;
                    pUVVert = m_pMesh->mapVerts(channel);

                    uvVert = pUVVert[pTVFace[nFaceID].t[nFaceVertexIndex]];
                    bValidUV = true;
                }
                else if(m_pMesh->numTVerts > 0)
                {
                    uvVert = m_pMesh->tVerts[m_pMesh->tvFace[nFaceID].t[nFaceVertexIndex]];
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
                    //CAIRVertex::TextureCoordinate texCoord;
                    texCoord.u = uvVert.x;
                    texCoord.v = uvVert.y;
                    if (AIREXPORTER->IsLeftHand())
                        texCoord.v = 1.0f - uvVert.y;
                    else if (AIREXPORTER->IsYUp())
                        texCoord.v = 1.0f - uvVert.y;
                    //pVertex->AddTextureCoordinate(texCoord);
                    break;
                }
            }
        }
    }

    return true;
}

CMaxMaterial* CMaxMesh::GetMaterial(int nMaterialID)
{
    if (nMaterialID < 0 || nMaterialID > m_vtUsedMaterial.size())
    {
        return NULL;
    }

    return m_vtUsedMaterial[nMaterialID];
}

Modifier* CMaxMesh::FindPhysiqueModifier()
{
    Object *pObject;
    INode *pINode = m_pNode;
    pObject = pINode->GetObjectRef();
    if(pObject == 0) return 0;

    // loop through all derived objects
    while(pObject->SuperClassID() == GEN_DERIVOB_CLASS_ID)
    {
        IDerivedObject *pDerivedObject;
        pDerivedObject = static_cast<IDerivedObject *>(pObject);

        // loop through all modifiers
        int stackId;
        for(stackId = 0; stackId < pDerivedObject->NumModifiers(); stackId++)
        {
            // get the modifier
            Modifier *pModifier;
            pModifier = pDerivedObject->GetModifier(stackId);

            // check if we found the physique modifier
            if(pModifier->ClassID() == Class_ID(PHYSIQUE_CLASS_ID_A, PHYSIQUE_CLASS_ID_B)) 
            {
                m_modifierType = MODIFIER_PHYSIQUE;
                return pModifier;
            }
        }

        // continue with next derived object
        pObject = pDerivedObject->GetObjRef();
    }

    return NULL;
}

Modifier *CMaxMesh::FindSkinModifier()
{

    // get the object reference of the node
    Object *pObject;
    pObject = m_pNode->GetObjectRef();
    if(pObject == 0) 
    {
        return 0;
    }

    // loop through all derived objects
    while(pObject->SuperClassID() == GEN_DERIVOB_CLASS_ID)
    {
        IDerivedObject *pDerivedObject;
        pDerivedObject = static_cast<IDerivedObject *>(pObject);

        // loop through all modifiers
        int stackId;
        for(stackId = 0; stackId < pDerivedObject->NumModifiers(); stackId++)
        {
            // get the modifier
            Modifier *pModifier;
            pModifier = pDerivedObject->GetModifier(stackId);

            // check if we found the skin modifier
            if(pModifier->ClassID() == SKIN_CLASSID) 
            {
                m_modifierType = MODIFIER_SKIN;
                return pModifier;
            }
        }

        // continue with next derived object
        pObject = pDerivedObject->GetObjRef();
    }

    return 0;
}

bool CMaxMesh::SetCurrentTime(unsigned int iMaxTime)
{
    if (BuildAnimatedIMesh(iMaxTime))
    {
        m_tm = m_pNode->GetObjectTM(iMaxTime);

        m_tm = m_pNode->GetObjectTM(iMaxTime);//m_pNode->GetObjTMAfterWSM(iMaxTime);   

        Matrix3 tmNode = m_pNode->GetNodeTM(iMaxTime);
        Matrix3 tmObject = m_pNode->GetObjectTM(iMaxTime);
        Matrix3 tmObjWSM = m_pNode->GetObjTMAfterWSM(iMaxTime);

        Point3	r1, r2, r3, r12;
        float					Mirror;
        r1 = m_tm.GetRow(0);
        r2 = m_tm.GetRow(1);
        r3 = m_tm.GetRow(2);
        r12 = CrossProd(r1, r2);//法向量计算
        Mirror = DotProd(r12, r3);//角度计算
        m_bMirrored = Mirror < 0;

        if (AIREXPORTER->IsLeftHand() && AIREXPORTER->IsYUp())
        {
            m_tm = AIREXPORTER->GetMaxInterface()->ConvertToDXMatrix(m_tm);
        }
        else if (AIREXPORTER->IsYUp())
        {
            m_tm = AIREXPORTER->GetMaxInterface()->ConvertToYUpMatrix(m_tm);
            //m_tm = AIREXPORTER->GetMaxInterface()->ConvertToDXMatrix(m_tm);
        }
    }
    return true;
}
