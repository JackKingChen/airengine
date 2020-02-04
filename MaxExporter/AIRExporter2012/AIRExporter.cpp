#include "AIRExporter.h"
#include "AIRMesh.h"
#include "AIRSkeleton.h"
#include "AIRAnimation.h"
#include "MarkupSTL.h"
#include "AIRKeyframeAnimation.h"
#include <fstream>

CAIRExporter* CAIRExporter::m_pInterface = NULL;

CAIRExporter::CAIRExporter(void) : m_strVersion("0.2"), m_strCoord(STR_COORDINATE_RIGHT), m_strUpVec(STR_AXIS_Z), m_hWndDlg(NULL), m_bLeftHand(false), m_nStartFrame(0),
    m_nEndFrame(0)
    , m_nTotalFrame(0)
    , m_pSelectedSkeleton(NULL), m_bYIsUp(false), m_bExportMaterial(false), m_bExportSelected(false)
    , m_bSaveAsBinary(false)
    , m_bShareSkeleton(false)
    , m_nVertexAnimationMask(0)
    , m_nAnimationsMark(0)
    , m_bExportStaticMesh(false)
{
    m_matFinal.IdentityMatrix();
}

CAIRExporter::~CAIRExporter(void)
{
    //for (int i = 0; i < m_vtMaxNodes.size(); i++)
    //{
    //    CMaxNode* pNode = m_vtMaxNodes[i];
    //    delete pNode;
    //    pNode = NULL;
    //}

    Clear();
}

bool CAIRExporter::Export(const char* szFilename)
{
    return true;
}

bool CAIRExporter::ExportAnimation(CAIRSkeleton* pSkeleton, const std::string& strFilename)
{
	m_MaxInterface.StartProgressInfo("Start exporting animation...");
    CAIRAnimation animation;
    if (animation.Create(*pSkeleton))
    {
        if (m_bSaveAsBinary)
        {
            animation.SaveToFileBinary(strFilename.c_str());
        }
        else
            animation.SaveToFile(strFilename.c_str());
    }

    for (size_t i = 0; i < m_vtExportMeshes.size(); ++i)
    {
        animation.BuildBoundingBoxes(m_vtExportMeshes[i]->GetMaxMesh(), i);
    }
	m_MaxInterface.StopProgressInfo();
    animation.Clear();
    return true;
}

bool CAIRExporter::ExportMaterial(const std::string& strFilename)
{
    
    CMarkupSTL xml;
    xml.SetDoc("<?xml version=\"1.0\" encoding=\"gb2312\"?>\r\n");
    xml.AddElem("Materials");

    xml.IntoElem();
    xml.AddNode(CMarkupSTL::MNT_TEXT, "\r\n    ");

    for (size_t i = 0; i < m_vtExportMeshes.size(); i++)
    {
        CAIRMesh* pMesh = m_vtExportMeshes[i];
        for (int j = 0; j < pMesh->GetSubMeshCount(); j++)
        {

            CAIRSubMesh* pSubMesh = pMesh->GetSubMesh(j);
            CAIRMaterial* pMaterial = pSubMesh->GetAIRMaterial();

            xml.AddElem("Material");
            xml.SetAttrib("Name", pMaterial->GetName().c_str());
            xml.SetAttrib("Template", m_bExportStaticMesh ? "mesh_template.xml" : "skinmesh_template.xml");

            /*
            //添加technique
            xml.IntoElem();
            xml.AddElem("Technique");
            xml.AddAttrib("Name", "Default");

            //添加pass
            xml.IntoElem();
            xml.AddElem("Pass");
            xml.AddAttrib("TextureCount", 1);

            xml.IntoElem();
            xml.AddNode(CMarkupSTL::MNT_TEXT, "\r\n        ");
            xml.AddElem("TextureUnit");
            xml.AddAttrib("File", pMaterial->GetTextureFileName().c_str());

            xml.AddAttrib("Mipmap", 0);
            xml.AddAttrib("Filter", "TEXF_LINEAR");
            xml.AddAttrib("AddressMode", "TADDRESS_WRAP");

            xml.IntoElem();
            xml.AddElem("TextureStage");
            xml.AddAttrib("Stage", 0);

            xml.IntoElem();
 
            xml.AddElem("TextureBlendColor");
            xml.AddAttrib("TextureOp", "TOP_MODULATE");
            xml.AddAttrib("Arg1", "TA_TEXTURE");
            xml.AddAttrib("Arg2", "TA_DIFFUSE");

            if (pMaterial->IsAlphaTest() || pMaterial->IsAlphaBlend())
            {
                xml.AddElem("TextureBlendAlpha");
                xml.AddAttrib("TextureOp", "TOP_MODULATE");
                xml.AddAttrib("Arg1", "TA_TEXTURE");
                xml.AddAttrib("Arg2", "TA_DIFFUSE");
            }

            //下面是采样
            xml.AddNode(CMarkupSTL::MNT_TEXT, "\r\n            ");

            xml.OutOfElem();   //退出TextureStage

            xml.AddElem("SamplerStateObject");
            xml.AddAttrib("Filter", "TEXF_LINEAR");
            xml.AddAttrib("AddressU", "TADDRESS_WRAP");
            xml.AddAttrib("AddressV", "TADDRESS_WRAP");
            xml.AddAttrib("AddressW", "TADDRESS_WRAP");

            xml.OutOfElem();   //退出TextureUnit

            xml.AddNode(CMarkupSTL::MNT_TEXT, "        ");

            bool bTwoSide = pMaterial->IsTwoSide();
            std::string strCullMode = bTwoSide ? "CULL_NONE" : "CULL_CCW";

            xml.AddElem("RenderStates");
            xml.IntoElem();
            xml.AddElem("BlendStateObject");
            xml.AddAttrib("BlendEnable", pMaterial->IsAlphaBlend() ? "TRUE" : "FALSE");
            xml.AddAttrib("SrcBlend", "BLEND_ONE");
            xml.AddAttrib("DstBlend", "BLEND_DESTCOLOR");
            xml.AddAttrib("BlendOpt", "BLENDOP_ADD");
            xml.AddAttrib("SrcBlendAlpha", "BLEND_ONE");
            xml.AddAttrib("DstBlendAlpha", "BLEND_DESTALPHA");
            xml.AddAttrib("BlendOptAlpha", "BLENDOP_ADD");

            xml.AddElem("RasterizerStateObject");
            xml.AddAttrib("FillMode", "FILL_SOLID");
            xml.AddAttrib("CullMode", strCullMode.c_str());
            
            xml.AddElem("DepthStencilStateObject");
            xml.AddAttrib("ZEnable", "TRUE");
            xml.AddAttrib("ZWrite", "TRUE");

            xml.AddElem("FixedPipeStateObject");
            xml.AddAttrib("AlphaTestEnable", pMaterial->IsAlphaTest() ? "TRUE" : "FALSE");
            xml.AddAttrib("AlphaRef", "00000080");
            xml.AddAttrib("FogEnable", "TRUE");
            xml.AddAttrib("LightEnable", "FALSE");

            xml.OutOfElem();

            xml.OutOfElem();  //退出pass

            xml.OutOfElem();  //退出technique

            xml.AddElem("LightParam");
            //顺序为rgba
            xml.AddAttrib("DIFFUSE", "(1.0,1.0,1.0,1.0)");
            xml.AddAttrib("SPECULAR", "(0.0,0.0,0.0,1.0)");
            xml.AddAttrib("EMISSIVE", "(0.0,0.0,0.0,1.0)");
            xml.AddAttrib("AMBIENT", "(1.0,1.0,1.0,1.0)");
            xml.AddAttrib("SHINESS", "255.0");
            */
            xml.IntoElem();
            xml.AddElem("MaterialParams");

            xml.IntoElem();
            xml.AddElem("var");
            xml.AddAttrib("name", "samplerMesh");
            xml.AddAttrib("constantType", "CS_Sampler");
            xml.AddAttrib("VariantType", "VAR_TEXTURE");
            xml.AddAttrib("value", pMaterial->GetTextureFileName().c_str());
            xml.OutOfElem();  //退出var

            xml.AddElem("var");
            xml.AddAttrib("name", "alphaTest");
            xml.AddAttrib("constantType", "CS_FLOAT");
            xml.AddAttrib("VariantType", "VAR_FLOAT");
            xml.AddAttrib("value", "0.2");
            xml.OutOfElem();  //退出var

            xml.OutOfElem();  //退出MaterialParams

            xml.OutOfElem();  //退出material
        }
    }

    xml.OutOfElem();

    xml.Save(strFilename.c_str());
    return true;
}

bool CAIRExporter::ExportMesh(const std::string& strFilename)
{
    return true;
}

bool CAIRExporter::ExportSkeleton(CAIRSkeleton& skeletonOut, const std::string& strFilename)
{
    Clear();

    if (m_vtMaxNodes.empty())
    {
        ExtractAllNodes(NULL, m_MaxInterface.GetInterface()->GetRootNode());
    }
    //CAIRSkeleton skeleton;
    //m_MaxInterface->StartProgressInfo("Start exporting skeletons...");

    if (skeletonOut.BuildBones(m_vtMaxNodes[0], -1))
    {
        if (m_bSaveAsBinary)
        {
            skeletonOut.SaveToFileBinary(strFilename.c_str());
        }
        else
            skeletonOut.SaveToFile(strFilename.c_str());
    }

    //m_MaxInterface->StopProgressInfo();

    return true;
}

bool CAIRExporter::ExportStaticModel(CAIRSkeleton* pSkeleton, const std::string& strFilename)
{
    m_bExportStaticMesh = (pSkeleton == NULL);
    Clear();
    if (m_vtMaxNodes.empty())
    {
        ExtractAllNodes(NULL, m_MaxInterface.GetInterface()->GetRootNode());
    }
    

    m_MaxInterface.StartProgressInfo("Start exporting mesh file...");
    
    //BuildMaxMeshesFromNodes();

    //if (m_vtMaxMeshes.size() == 0)
    //{
    //    return false;
    //}

    //创建材质
    BuildMaterials();

    
    /*
    for (int i = 0; i < m_vtMaxMeshes.size(); i++)
    {
        CMaxMesh* pMesh = m_vtMaxMeshes[i];
        
    }
    */

    //CAIRMesh exportMesh;
    for (size_t i = 0; i < m_vtMaxNodes.size(); i++)
    {
        CMaxNode* pNode = m_vtMaxNodes[i];
        if (!pNode->IsMesh())
        {
            continue;
        }
        CAIRMesh* pMesh = new CAIRMesh;
        if (pMesh->Create(pSkeleton, m_vtMaxNodes[i], 0, 0))
        {
            m_vtExportMeshes.push_back(pMesh);
        }
        else
        {
            delete pMesh;
        }
    }

    if (m_bExportMaterial)
    {
        size_t nPos = strFilename.find_last_of("/");
        if (nPos == std::string::npos)
        {
            nPos = strFilename.find_last_of("\\");
        }
        std::string strPath = strFilename.substr(0, nPos + 1);
        ExportMaterial(strPath + m_strMaterialFile);
    }

    bool bRes = false;

    if (m_bSaveAsBinary)
    {
        bRes = SaveMeshToFileBinary(strFilename.c_str());
    }
    else
        bRes = SaveMeshToFile(strFilename.c_str());

    m_MaxInterface.StopProgressInfo();
    
    return bRes;
}

bool CAIRExporter::ExportVertexAnimation(const std::string& strFilename)
{
    Clear();
    if (m_vtMaxNodes.empty())
    {
        ExtractAllNodes(NULL, m_MaxInterface.GetInterface()->GetRootNode());
    }
    

    m_MaxInterface.StartProgressInfo("Start exporting mesh file...");

    
    //BuildMaxMeshesFromNodes();

    //if (m_vtMaxMeshes.size() == 0)
    //{
    //    return false;
    //}

    //创建材质
    BuildMaterials();

    for (size_t i = 0; i < m_vtMaxNodes.size(); i++)
    {
        CMaxNode* pNode = m_vtMaxNodes[i];
        if (!pNode->IsMesh())
        {
            continue;
        }
        CAIRMesh* pMesh = new CAIRMesh;
        if (pMesh->Create(NULL, m_vtMaxNodes[i], 0, 0))
        {
            m_vtExportMeshes.push_back(pMesh);
        }
        else
        {
            delete pMesh;
        }
    }

    if (m_bExportMaterial)
    {
        size_t nPos = strFilename.find_last_of("/");
        if (nPos == std::string::npos)
        {
            nPos = strFilename.find_last_of("\\");
        }
        std::string strPath = strFilename.substr(0, nPos + 1);
        ExportMaterial(strPath + m_strMaterialFile);
    }

    BuildMeshVertexAnimation();
    if (m_bSaveAsBinary)
    {
        //SaveMeshToFileBinary(strFilename.c_str());
        SaveVertexAnimationBin(m_strKeyFrameFile.c_str());
        //animation.SaveToFileBinary(m_strKeyFrameFile.c_str());
    }
    else
    {
        //SaveMeshToFile(strFilename.c_str());
        SaveVertexAnimation(m_strKeyFrameFile.c_str());
        //animation.SaveToFile(m_strKeyFrameFile.c_str());
    }

    
    /*
    CAIRKeyframeAnimation animation;
    animation.SetName(m_strAnimationName.c_str());
    animation.Create(m_vtMaxNodes);

    if (m_bSaveAsBinary)
    {
        SaveMeshToFileBinary(strFilename.c_str());
        animation.SaveToFileBinary(m_strKeyFrameFile.c_str());
    }
    else
    {
        SaveMeshToFile(strFilename.c_str());

        animation.SaveToFile(m_strKeyFrameFile.c_str());
    }
    
    animation.Clear();
    */

    m_MaxInterface.StopProgressInfo();
    return true;
}

void CAIRExporter::ExtractAllNodes(CMaxNode* pParent, INode* pNode)
{
    if (pNode == NULL)
    {
        return;
    }
    CMaxNode* pMaxNode = new CMaxNode();
    if (m_bExportSelected)
    {
        if (pNode->Selected())
        {
            
            if (pMaxNode->Create(pParent, pNode))
            {
                m_vtMaxNodes.push_back(pMaxNode);
            }
            else
                delete pMaxNode;
        }
    }
    else
    {
        if (pMaxNode->Create(pParent, pNode))
        {
            m_vtMaxNodes.push_back(pMaxNode);
        }
        else
            delete pMaxNode;
    }
    

    for (int i = 0; i < pNode->NumberOfChildren(); i++)
    {
        ExtractAllNodes(pMaxNode, pNode->GetChildNode(i));
    }
}

/*
void CAIRExporter::BuildMaxMeshesFromNodes()
{
    //得到当前的时间．我们只需要导出当前那一帧
    unsigned int cur_time = m_MaxInterface.GetInterface()->GetTime();
    for (int i = 0; i < m_vtMaxNodes.size(); i++)
    {
        CMaxMesh* pMesh = new CMaxMesh();
        if (pMesh->Create(m_vtMaxNodes[i]->GetINode(), cur_time))
            m_vtMaxMeshes.push_back(pMesh);
        else
            delete pMesh;
    }
}
*/

void CAIRExporter::BuildSkeletonsFromNodes()
{

}

void CAIRExporter::BuildSkeleton(CAIRSkeleton& skeletonOut)
{

}

void CAIRExporter::Clear()
{
    for (size_t i = 0; i < m_vtMaxNodes.size(); i++)
    {
        CMaxNode* pNode = m_vtMaxNodes[i];
        pNode->Clear();
        delete pNode;
        pNode = NULL;
    }
    m_vtMaxNodes.clear();


    for (size_t i = 0; i < m_vtExportMeshes.size(); i++)
    {
        CAIRMesh* pMesh = m_vtExportMeshes[i];
        pMesh->Clear();
        delete pMesh;
    }
    m_vtExportMeshes.clear();

    //for (int i = 0; i < m_vectorMaterials.size(); i++)
    //{
    //    CAIRMaterial* pMaterial = m_vectorMaterials[i];
    //    pMaterial->Clear();
    //    delete pMaterial;
    //}
    m_vectorMaterials.clear();

    if (m_pSelectedSkeleton)
    {
        m_pSelectedSkeleton->Clear();
        delete m_pSelectedSkeleton;
        m_pSelectedSkeleton = NULL;
    }

    //if (m_MaxInterface)
    //{
    //    m_MaxInterface->Clear();
    //    delete m_MaxInterface;
    //}
    

    
}

int CAIRExporter::GetMaxNodesCount() const
{ 
    return (int)m_vtMaxNodes.size();
}

CMaxNode* CAIRExporter::GetNode(int nNodeID)
{
    if (nNodeID < 0 || nNodeID >= m_vtMaxNodes.size())
    {
        //报错
        return NULL;
    }

    return m_vtMaxNodes[nNodeID];
}

void CAIRExporter::BuildMaterials()
{
    ClearMaterials();

    INode* pRoot = m_MaxInterface.GetInterface()->GetRootNode();

    BuildMaterial(pRoot->GetMtl());

}

void CAIRExporter::BuildMaterial(Mtl* pMat)
{
    //Mtl* pMat = m_pNode->GetMtl();
    if (pMat == NULL)
    {
        return;
    }

    //判断是否标准材质
    if (pMat->ClassID() == Class_ID(DMTL_CLASS_ID, 0))
    {
        //m_vectorStdMat.push_back((StdMat*)pMat);
        CMaxMaterial* pMaterial = new CMaxMaterial;
        if (pMaterial->Create((StdMat*)pMat))
        {
            pMaterial->m_nMatID = m_vectorMaterials.size() - 1;  //数组下标
            CAIRMaterial* pAIRMaterial = new CAIRMaterial;
            pAIRMaterial->Create(pMaterial);
            m_vectorMaterials.push_back(pAIRMaterial);
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
        BuildMaterial(pMat->GetSubMtl(subId));
    }
}

void CAIRExporter::ClearMaterials()
{
    for (size_t i = 0; i < m_vectorMaterials.size(); i++)
    {
        CAIRMaterial* pMaterial = m_vectorMaterials[i];
        delete pMaterial;
        pMaterial = NULL;
    }

    m_vectorMaterials.clear();
}

CAIRMaterial* CAIRExporter::GetMaterial(int nMaterialID)
{
    if (nMaterialID < 0 || (size_t)nMaterialID > m_vectorMaterials.size())
    {
        return NULL;
    }

    return m_vectorMaterials[nMaterialID];
}

bool CAIRExporter::SaveMeshToFile(const char* szFilename)
{
    /*
    if (m_vtMaxMeshes.size() == 0)
    {
        ShowErrorMsg("没有网格可以导出！", __FILE__, __LINE__);
        return false;
    }
    */
    FILE* f = fopen(szFilename, "wt");

    if (f == NULL)
    {
        ShowErrorMsg("文件打开失败！", __FILE__, __LINE__);
        return false;
    }

    fprintf(f, "AIRVersion %s\n", AIREXPORTER->GetVersion().c_str());

    //CMaxMesh* pMaxMesh = m_vtMaxMeshes[0];

    //if (pMaxMesh->IsMirrored())  //如果是镜面，则是左手坐标
    //{
        m_strCoord = STR_COORDINATE_LEFT;
    //}
    fprintf(f, "Coordinate %s\n", m_strCoord.c_str());

    fprintf(f, "UpVec %s\n", m_strUpVec.c_str());
    int nMeshesCount = 0;
    for (size_t i = 0; i < m_vtExportMeshes.size(); i++)
    {
        CAIRMesh* pMesh = m_vtExportMeshes[i];
        nMeshesCount += pMesh->GetSubMeshCount();
    }

    fprintf(f, "%s %d\n", STR_NUMMESH, nMeshesCount);

    if (nMeshesCount == 0)
    {
        fclose(f);
        return false;
    }

    if (m_vtExportMeshes[0]->GetSkeletonFile().length() > 0)
    {
        fprintf(f, "skeletalFile %s\n", m_vtExportMeshes[0]->GetSkeletonFile().c_str());
    }

    if (m_strKeyFrameFile.length() > 0)
    {
        size_t nPos = m_strKeyFrameFile.find_last_of("/");
        if (nPos == string::npos)
        {
            nPos = m_strKeyFrameFile.find_last_of("\\");
        }

        std::string strFilename = m_strKeyFrameFile;
        if (nPos != string::npos)
        {
            strFilename = m_strKeyFrameFile.c_str() + nPos + 1;
        }
        fprintf(f, "keyframe_animation_file %s\n", strFilename.c_str());
    }

    for (size_t i = 0; i < m_vtExportMeshes.size(); i++)
    {
        CAIRMesh* pMesh = m_vtExportMeshes[i];
        pMesh->SaveToFile(f);
    }

    fclose(f);

    return true;
}

bool CAIRExporter::SaveMeshToFileBinary(const char* szFilename)
{
    if (szFilename == NULL)
    {
        MessageBox(NULL, "请输入导出的文件名！", "ERROR", MB_OK);
        return false;
    }
    std::ofstream fout;
    fout.open(szFilename, std::ios::out | std::ios::binary);
    //fout<<(float)0.1f;
    float fVersion = FILE_VERSION;
    //fout<<fVersion;
    fout.write((const char*)&fVersion, sizeof(float));
    BYTE bLeftHand = 0;
    if (AIREXPORTER->IsLeftHand())
    {
        //fout<<(BYTE)1;
        bLeftHand = 1;
        
    }
    else
    {
        bLeftHand = 0;
        //fout<<(BYTE)0;
    }
    fout.write((const char*)&bLeftHand, sizeof(BYTE));

    BYTE bYUp = 0;
    if (AIREXPORTER->IsYUp())
    {
        bYUp = 0;
        //fout<<(BYTE)0;
    }
    else
    {
        bYUp = 1;
        //fout<<(BYTE)1;
    }
    fout.write((const char*)&bYUp, sizeof(BYTE));

    int nMeshesCount = 0;
    for (size_t i = 0; i < m_vtExportMeshes.size(); i++)
    {
        CAIRMesh* pMesh = m_vtExportMeshes[i];
        nMeshesCount += pMesh->GetSubMeshCount();
    }

    if (nMeshesCount == 0)
    {
        fout.close();
        return false;
    }

    fout.write((const char*)&nMeshesCount, sizeof(nMeshesCount));
    //fout<<nMeshesCount;

    BYTE bAnimationType = m_nAnimationsMark;
    /*
    if (m_strKeyFrameFile.length() > 0)
    {
        bAnimationType = 1;   //顶点动画 
        //fout<<(BYTE)1;
    }
    else if (m_vtExportMeshes[0]->GetSkeletonFile().length() > 0)
    {
        bAnimationType = 0;
        if (m_vtExportMeshes[0]->GetSkeletonFile() == "none")
        {
            bAnimationType = 2;
        }
        //fout<<(BYTE)2;
    }
    else
    {
        bAnimationType = 2;
    }
    */
    fout.write((const char*)&bAnimationType, sizeof(BYTE));
    //fout<<bAnimationType;

    if (bAnimationType & eMAT_SkeletionAnimation)
    {
        //int nLength = m_vtExportMeshes[0]->GetSkeletonFile().length();
        //fout.write((const char*)&nLength, sizeof(int));
        fout.write(m_vtExportMeshes[0]->GetSkeletonFile().c_str(), 256/*m_vtExportMeshes[0]->GetSkeletonFile().length()*/);
    }
    if (bAnimationType & eMAT_VertexAnimation)
    {
        //fout<<(int)m_strKeyFrameFile.length();
        

        size_t nPos = m_strKeyFrameFile.find_last_of("/");
        if (nPos == string::npos)
        {
            nPos = m_strKeyFrameFile.find_last_of("\\");
        }

        std::string strFilename = m_strKeyFrameFile;
        if (nPos != string::npos)
        {
            strFilename = m_strKeyFrameFile.c_str() + nPos + 1;
        }
        //fout<<strFilename.c_str();
        //int nLength = strFilename.length();
        //fout.write((const char*)&nLength, sizeof(int));
        fout.write(strFilename.c_str(), /*strFilename.length()*/256);
    }
    
    for (size_t i = 0; i < m_vtExportMeshes.size(); i++)
    {
        CAIRMesh* pMesh = m_vtExportMeshes[i];
        pMesh->SaveToFileBinary(fout);
    }

    fout.close();
    return true;
}

bool CAIRExporter::CreateSelectedSkeleton(const char* szFilename)
{
    Clear();
    ExtractAllNodes(NULL, m_MaxInterface.GetInterface()->GetRootNode());
	if (m_pSelectedSkeleton != NULL)
	{
		m_pSelectedSkeleton->Clear();
	}

	if (m_pSelectedSkeleton == NULL)
	{
		m_pSelectedSkeleton = new CAIRSkeleton;
        
        m_pSelectedSkeleton->BuildBones(m_vtMaxNodes[0], -1);
        m_pSelectedSkeleton->SetFileName(szFilename);
	}

    CAIRSkeleton skeletonCheck;
    if (m_bSaveAsBinary)
    {
        if (!skeletonCheck.LoadFromBinaryFile(szFilename))
        {
            //弹出错误窗口
            ShowErrorMsg("读取骨骼文件失败！", __FILE__, __LINE__);
            return false;


        }
        else
        {
            if (skeletonCheck.GetBoneNum() != m_pSelectedSkeleton->GetBoneNum())
            {
                MessageBox(m_hWndDlg, "读取的骨骼文件与当前模型骨骼不一致，请检查！", "Error", MB_OK);
                return false;
            }
        }
    }
    else
    {
        if (!skeletonCheck.LoadFromFile(szFilename))
        {
            //弹出错误窗口
            ShowErrorMsg("读取骨骼文件失败！", __FILE__, __LINE__);
            return false;


        }
        else
        {
            if (skeletonCheck.GetBoneNum() != m_pSelectedSkeleton->GetBoneNum())
            {
                MessageBox(m_hWndDlg, "读取的骨骼文件与当前模型骨骼不一致，请检查！", "Error", MB_OK);
                return false;
            }
        }
    }
	

    m_strSkeletonFile = szFilename;
	return true;
}

CMaxNode* CAIRExporter::GetNode(const char* szName)
{
	for (size_t i = 0; i < m_vtMaxNodes.size(); i++)
	{
	    CMaxNode* pNode = m_vtMaxNodes[i];
		if (strcmp(pNode->GetName().c_str(), szName) == 0)
		{
			return pNode;
		}
	}

	return NULL;
}

void CAIRExporter::InitExportConfig()
{
    if (m_bYIsUp)
    {
        m_matFinal.RotateX(-PI / 2);
    }

    if (m_bLeftHand)
    {
        Matrix3 mat;
        mat.IdentityMatrix();
        mat.Scale(Point3(1, 1, -1));
        m_matFinal = m_matFinal * mat;
    }
}

void CAIRExporter::SetUpY(bool bUpY)
{
    m_bYIsUp = bUpY;
}

CAIRMesh* CAIRExporter::GetExportMesh(const std::string& strName)
{
    for (size_t i = 0; i < m_vtExportMeshes.size(); ++i)
    {
        if (m_vtExportMeshes[i]->GetName() == strName)
        {
            return m_vtExportMeshes[i];
        }
    }

    return NULL;
}

bool CAIRExporter::BuildMeshVertexAnimation()
{
    CMaxInterface* pInterface = AIREXPORTER->GetMaxInterface();
    Interval ARange =  pInterface->GetInterface()->GetAnimRange(); //获得动画的长度

    //TimeValue   tAniTime =  ARange.End() - ARange.Start();

    TimeValue   tTime = ARange.Start();

    int nTotalFrames = AIREXPORTER->GetEndFrame() - AIREXPORTER->GetStartFrame();

    TimeValue   tAniTime = nTotalFrames * GetTicksPerFrame();

    TimeValue interval = tAniTime / nTotalFrames;

    float fTime = tTime;

    for (int nFrame = 0; nFrame < nTotalFrames; nFrame++)
    {
        pInterface->SetProgressInfo(/*100.0f * (float)nFrame / (float)m_nFrames*/tTime / tAniTime * 100);
        pInterface->SetTime(tTime);

        for (size_t i = 0; i < m_vtExportMeshes.size(); ++i)
        {
            m_vtExportMeshes[i]->CreateVertexAnimation(nFrame);
        }

        /*
        CAIRKeyFrameData* pKeyframeData = new CAIRKeyFrameData(nFrame);
        pKeyframeData->Create(tTime, vtMaxNodes);
        m_vtKeyFrameData.push_back(pKeyframeData);
        */

        tTime += interval;
        //fTime += 1.0f / GetFrameRate();
    }
    return true;
}

bool CAIRExporter::SaveVertexAnimation(const char* szFilename)
{
    FILE* f = fopen(szFilename, "wt");

    if (f == NULL)
    {
        //ShowErrorMsg("文件打开失败！", __FILE__, __LINE__);
        return false;
    }

    int nFrames = AIREXPORTER->GetEndFrame() - AIREXPORTER->GetStartFrame();

    fprintf(f, "AIRVersion %s\n", AIREXPORTER->GetVersion().c_str());

    fprintf(f, "animation %s\n", m_strAnimationName.c_str());

    fprintf(f, "    frames %d\n", nFrames);
    fprintf(f, "    frameRate %d\n", GetFrameRate());

    for (size_t i = 0; i < m_vtExportMeshes.size(); ++i)
    {
        m_vtExportMeshes[i]->SaveVertexAnimation(f);
    }

    fclose(f);
    return true;
}

bool CAIRExporter::SaveVertexAnimationBin(const char* szFilename)
{
    std::fstream fout;
    fout.open(szFilename, std::ios::out | std::ios::binary);
    //fout<<(float)0.1f;
    float fVersion = FILE_VERSION;
    //fout<<fVersion;
    fout.write((const char*)&fVersion, sizeof(float));

    fout.write(m_strAnimationName.c_str(), 256);
    WORD nFrames = AIREXPORTER->GetEndFrame() - AIREXPORTER->GetStartFrame();
    fout.write((const char*)&nFrames, sizeof(WORD));
    float fFrameRate = GetFrameRate();
    fout.write((const char*)&fFrameRate, sizeof(float));

    //写入mask
    fout.write((const char*)m_nVertexAnimationMask, sizeof(INT32));
    WORD nMeshesNum = 0;

    for (size_t i = 0; i < m_vtExportMeshes.size(); ++i)
    {
        nMeshesNum += m_vtExportMeshes[i]->GetSubMeshCount();
    }
    fout.write((const char*)&nMeshesNum, sizeof(WORD));

    for (size_t i = 0; i < m_vtExportMeshes.size(); ++i)
    {
        m_vtExportMeshes[i]->SaveVertexAnimationBin(fout);
    }

    fout.close();
    return true;
}
