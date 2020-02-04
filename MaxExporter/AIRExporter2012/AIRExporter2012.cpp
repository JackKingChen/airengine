//**************************************************************************/
// Copyright (c) 1998-2007 Autodesk, Inc.
// All rights reserved.
// 
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information written by Autodesk, Inc., and are
// protected by Federal copyright law. They may not be disclosed to third
// parties or copied or duplicated in any form, in whole or in part, without
// the prior written consent of Autodesk, Inc.
//**************************************************************************/
// DESCRIPTION: Appwizard generated plugin
// AUTHOR: 
//***************************************************************************/

#include "AIRExporter2012.h"
#include "stdmat.h"
#include "AirStaticMeshFile.h"
#include "MaxInterface.h"
//#include "AIRExporter.h"
#include "MaxMesh.h"
#include "AIRMaterial.h"
#include "AIRMesh.h"
#include "MaxInterface.h"
#include <sstream>
#include "AIRAnimation.h"
#include "MarkupSTL.h"
#include "AIRKeyframeAnimation.h"
#include <conio.h>



#define AIRExporter_CLASS_ID	Class_ID(0x94734f67, 0x49ccba62)

//static AIRExporter* s_pAIRExporter = NULL;
AIRExporter* AIREXPORTER;
bool g_bExport = true;

void AIRExporter::ShowErrorMsg(const char* szMsg, const char* szFilename, int nLine)
{
	std::stringstream strstrError;
	strstrError << std::string(szMsg) << "\n(" << std::string(szFilename) << " " << nLine << ")" << std::ends;
	MessageBox(m_hMainWnd, strstrError.str().c_str(), "ERROR", MB_OK);
}

bool AIRExporter::Export(const char* szFilename)
{
    return true;
}

bool AIRExporter::ExportAnimation(CAIRSkeleton* pSkeleton, const std::string& strFilename)
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

bool AIRExporter::ExportMaterial(const std::string& strFilename)
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
            xml.SetAttrib("template", m_bExportStaticMesh ? "mesh_template.xml" : "skinmesh_template.xml");

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
            //xml.OutOfElem();  //退出var

            xml.AddElem("var");
            xml.AddAttrib("name", "alphaTest");
            xml.AddAttrib("constantType", "CS_FLOAT");
            xml.AddAttrib("VariantType", "VAR_FLOAT");
            xml.AddAttrib("value", "0.2");
            xml.OutOfElem();  //退出MaterialParams

            //xml.OutOfElem();  //退出MaterialParams
			xml.AddElem("SamplerStateObject");
			xml.AddAttrib("name", "samplerMesh");
			xml.AddAttrib("stage", "0");
			xml.AddAttrib("Filter", "TEXF_LINEAR");
			xml.AddAttrib("AddressU", "TADDRESS_WRAP");
			xml.AddAttrib("AddressV", "TADDRESS_WRAP");
			xml.AddAttrib("AddressW", "TADDRESS_WRAP");

            xml.OutOfElem();  //退出material
        }
    }

    xml.OutOfElem();

    xml.Save(strFilename.c_str());
    return true;
}

bool AIRExporter::ExportMesh(const std::string& strFilename)
{
    return true;
}

bool AIRExporter::ExportSkeleton(CAIRSkeleton& skeletonOut)
{
    Clear();

    if (m_vtMaxNodes.empty())
    {
        ExtractAllNodes(NULL, m_MaxInterface.GetInterface()->GetRootNode());
    }
    //CAIRSkeleton skeleton;
    //m_MaxInterface->StartProgressInfo("Start exporting skeletons...");

    skeletonOut.BuildBones(m_vtMaxNodes[0], -1);
    {
        //if (m_bSaveAsBinary)
        //{
        //    skeletonOut.SaveToFileBinary(strFilename.c_str());
        //}
        //else
        //    skeletonOut.SaveToFile(strFilename.c_str());
    }

    //m_MaxInterface->StopProgressInfo();

    return skeletonOut.HasBones();
}

bool AIRExporter::ExportStaticModel(CAIRSkeleton* pSkeleton, const std::string& strFilename)
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
        bRes = SaveMeshToFileBinary(pSkeleton, strFilename.c_str());
    }
    else
        bRes = SaveMeshToFile(strFilename.c_str());

    m_MaxInterface.StopProgressInfo();
    
    return bRes;
}

bool AIRExporter::ExportVertexAnimation(const std::string& strFilename)
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

void AIRExporter::ExtractAllNodes(CMaxNode* pParent, INode* pNode)
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
void AIRExporter::BuildMaxMeshesFromNodes()
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

void AIRExporter::BuildSkeletonsFromNodes()
{

}

void AIRExporter::BuildSkeleton(CAIRSkeleton& skeletonOut)
{

}

void AIRExporter::Clear()
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

int AIRExporter::GetMaxNodesCount() const
{ 
    return (int)m_vtMaxNodes.size();
}

CMaxNode* AIRExporter::GetNode(int nNodeID)
{
    if (nNodeID < 0 || nNodeID >= m_vtMaxNodes.size())
    {
        //报错
        return NULL;
    }

    return m_vtMaxNodes[nNodeID];
}

void AIRExporter::BuildMaterials()
{
    ClearMaterials();

    INode* pRoot = m_MaxInterface.GetInterface()->GetRootNode();

    BuildMaterial(pRoot->GetMtl());

}

void AIRExporter::BuildMaterial(Mtl* pMat)
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

void AIRExporter::ClearMaterials()
{
    for (size_t i = 0; i < m_vectorMaterials.size(); i++)
    {
        CAIRMaterial* pMaterial = m_vectorMaterials[i];
        delete pMaterial;
        pMaterial = NULL;
    }

    m_vectorMaterials.clear();
}

CAIRMaterial* AIRExporter::GetMaterial(int nMaterialID)
{
    if (nMaterialID < 0 || (size_t)nMaterialID > m_vectorMaterials.size())
    {
        return NULL;
    }

    return m_vectorMaterials[nMaterialID];
}

bool AIRExporter::SaveMeshToFile(const char* szFilename)
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

    fprintf(f, "AIRVersion %s\n", GetVersion().c_str());

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

bool AIRExporter::SaveMeshToFileBinary(CAIRSkeleton* pSkeleton, const char* szFilename)
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
    if (IsLeftHand())
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
    if (IsYUp())
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
        //fout.write(m_vtExportMeshes[0]->GetSkeletonFile().c_str(), 256/*m_vtExportMeshes[0]->GetSkeletonFile().length()*/);
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

	//新版本把骨骼数据写入模型里
	pSkeleton->SaveToFileBinary(fout);

    fout.close();
    return true;
}

bool AIRExporter::CreateSelectedSkeleton(const char* szFilename)
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
	

    m_strSkeletonFile = szFilename;
	return true;
}

CMaxNode* AIRExporter::GetNode(const char* szName)
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

void AIRExporter::InitExportConfig()
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

void AIRExporter::SetUpY(bool bUpY)
{
    m_bYIsUp = bUpY;
}

CAIRMesh* AIRExporter::GetExportMesh(const std::string& strName)
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

bool AIRExporter::BuildMeshVertexAnimation()
{
    CMaxInterface* pInterface = GetMaxInterface();
    Interval ARange =  pInterface->GetInterface()->GetAnimRange(); //获得动画的长度

    //TimeValue   tAniTime =  ARange.End() - ARange.Start();

    TimeValue   tTime = ARange.Start();

    int nTotalFrames = GetEndFrame() - GetStartFrame();

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

bool AIRExporter::SaveVertexAnimation(const char* szFilename)
{
    FILE* f = fopen(szFilename, "wt");

    if (f == NULL)
    {
        //ShowErrorMsg("文件打开失败！", __FILE__, __LINE__);
        return false;
    }

    int nFrames = GetEndFrame() - GetStartFrame();

    fprintf(f, "AIRVersion %s\n", GetVersion().c_str());

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

bool AIRExporter::SaveVertexAnimationBin(const char* szFilename)
{
    std::fstream fout;
    fout.open(szFilename, std::ios::out | std::ios::binary);
    //fout<<(float)0.1f;
    float fVersion = FILE_VERSION;
    //fout<<fVersion;
    fout.write((const char*)&fVersion, sizeof(float));

    fout.write(m_strAnimationName.c_str(), 256);
    WORD nFrames = GetEndFrame() - GetStartFrame();
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

class AIRExporterClassDesc : public ClassDesc2 
{
public:
	virtual int IsPublic() 							{ return TRUE; }
	virtual void* Create(BOOL /*loading = FALSE*/) 		{ return new AIRExporter(); }
	virtual const TCHAR *	ClassName() 			{ return GetString(IDS_CLASS_NAME); }
	virtual SClass_ID SuperClassID() 				{ return SCENE_EXPORT_CLASS_ID; }
	virtual Class_ID ClassID() 						{ return AIRExporter_CLASS_ID; }
	virtual const TCHAR* Category() 				{ return GetString(IDS_CATEGORY); }

	virtual const TCHAR* InternalName() 			{ return _T("AIRExporter"); }	// returns fixed parsable name (scripter-visible name)
	virtual HINSTANCE HInstance() 					{ return hInstance; }					// returns owning module handle
	

};


ClassDesc2* GetAIRExporterDesc() { 
	static AIRExporterClassDesc AIRExporterDesc;
	return &AIRExporterDesc; 
}





INT_PTR CALLBACK AIRExporterOptionsDlgProc(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam) {
	static AIRExporter *imp = NULL;
	char szAnimationName[256] = { 0 };
	char szMaterialFileName[256] = { 0 };

	switch(message) {
		case WM_INITDIALOG:
			imp = (AIRExporter *)lParam;
			AIREXPORTER = imp;
			CenterWindow(hWnd,GetParent(hWnd));
			//CenterWindow(hWnd,GetParent(hWnd));
			imp->OnInitDialog(hWnd);
			return TRUE;

		case WM_CLOSE:
			EndDialog(hWnd, 0);
			return 1;

		case WM_COMMAND:
			switch(LOWORD(wParam))
			{
			case IDC_BTN_EXPORT:
				AIREXPORTER->SetLeftHand(Button_GetCheck(GetDlgItem(hWnd, IDC_CHK_LEFTHAND)));
				AIREXPORTER->SetUpY(Button_GetCheck(GetDlgItem(hWnd, IDC_CHECK_YUP)));
				AIREXPORTER->SetSaveAsBinary(Button_GetCheck(GetDlgItem(hWnd, IDC_CHK_SAVEASBINARY)));
				AIREXPORTER->SetExportMaterial(Button_GetCheck(GetDlgItem(hWnd, IDC_CHECK_EXPORT_MATERIAL)));
				imp->SetExportSkeletalAnimation(Button_GetCheck(GetDlgItem(hWnd, IDC_CHK_EXPORT_SKELETAL_ANIMATION)));
				AIREXPORTER->SetMeshAnimationType(eMAT_SkeletionAnimation, Button_GetCheck(GetDlgItem(hWnd, IDC_CHK_EXPORT_SKELETAL_ANIMATION)));
				if (!imp->OnBtnExport())
				{
					return FALSE;
				}
				EndDialog(hWnd, 0);
				return TRUE;

			case IDC_BTN_EXPORT_ANIMATION:
				AIREXPORTER->SetLeftHand(Button_GetCheck(GetDlgItem(hWnd, IDC_CHK_LEFTHAND)));
				AIREXPORTER->SetUpY(Button_GetCheck(GetDlgItem(hWnd, IDC_CHECK_YUP)));
				AIREXPORTER->SetSaveAsBinary(Button_GetCheck(GetDlgItem(hWnd, IDC_CHK_SAVEASBINARY)));
				AIREXPORTER->SetExportMaterial(Button_GetCheck(GetDlgItem(hWnd, IDC_CHECK_EXPORT_MATERIAL)));
				imp->SetExportSkeletalAnimation(Button_GetCheck(GetDlgItem(hWnd, IDC_CHK_EXPORT_SKELETAL_ANIMATION)));
				AIREXPORTER->SetMeshAnimationType(eMAT_SkeletionAnimation, Button_GetCheck(GetDlgItem(hWnd, IDC_CHK_EXPORT_SKELETAL_ANIMATION)));

				AIREXPORTER->SetStartFrame(GetDlgItemInt(hWnd, IDC_EDIT_StartFrame, NULL, FALSE));
				AIREXPORTER->SetEndFrame(GetDlgItemInt(hWnd, IDC_EDIT_EndFrame, NULL, FALSE));
				if (!imp->OnBtnExportAnimation())
				{
					return TRUE;
				}
				imp->SetExportStaticMesh(false);
				imp->SetExportVertexAnimation(false);
				imp->SetExportAnimateOnly(true);
				EndDialog(hWnd, 0);
				break;

			case IDC_BTN_EXPORT_STATICMESH:
				imp->SetExportStaticMesh(true);
				imp->SetExportVertexAnimation(false);
				AIREXPORTER->SetLeftHand(Button_GetCheck(GetDlgItem(hWnd, IDC_CHK_LEFTHAND)));
				AIREXPORTER->SetUpY(Button_GetCheck(GetDlgItem(hWnd, IDC_CHECK_YUP)));
				AIREXPORTER->SetExportMaterial(Button_GetCheck(GetDlgItem(hWnd, IDC_CHECK_EXPORT_MATERIAL)));
				if (AIREXPORTER->IsExportMaterial())
				{
					GetDlgItemText(hWnd, IDC_EDT_MATERIAL, szMaterialFileName, 256);
					AIREXPORTER->SetMaterialFileName(szMaterialFileName);
					if (strlen(szMaterialFileName) == 0)
					{
						MessageBox(hWnd, "请输入要导出的材质文件名！", "提示", MB_OK);
						return TRUE;
					}
				}
				if (AIREXPORTER->IsShareSkeleton())
				{
					GetDlgItemText(hWnd, IDC_EDIT_SKELETAL_FILE, szMaterialFileName, 256);
					if (strlen(szMaterialFileName) == 0)
					{
						MessageBox(hWnd, "请输选择骨骼动画文件！", "提示", MB_OK);
						return TRUE;
					}
				}
				AIREXPORTER->SetStartFrame(GetDlgItemInt(hWnd, IDC_EDIT_StartFrame, NULL, FALSE));
				AIREXPORTER->SetEndFrame(GetDlgItemInt(hWnd, IDC_EDIT_EndFrame, NULL, FALSE));
				EndDialog(hWnd, 0);
				break;

			case IDC_BTN_EXPORT_VERTEXANIMATION:
				//导出顶点动画
				imp->SetExportVertexAnimation(true);
				imp->SetExportStaticMesh(false);
				imp->SetExportAnimateOnly(false);
				//imp->SetE
				AIREXPORTER->SetLeftHand(Button_GetCheck(GetDlgItem(hWnd, IDC_CHK_LEFTHAND)));
				AIREXPORTER->SetUpY(Button_GetCheck(GetDlgItem(hWnd, IDC_CHECK_YUP)));
				AIREXPORTER->SetExportMaterial(Button_GetCheck(GetDlgItem(hWnd, IDC_CHECK_EXPORT_MATERIAL)));
				if (AIREXPORTER->IsExportMaterial())
				{
					GetDlgItemText(hWnd, IDC_EDT_MATERIAL, szMaterialFileName, 256);
					AIREXPORTER->SetMaterialFileName(szMaterialFileName);
					if (strlen(szMaterialFileName) == 0)
					{
						MessageBox(hWnd, "请输入要导出的材质文件名！", "提示", MB_OK);
						return TRUE;
					}
				}
				AIREXPORTER->SetStartFrame(GetDlgItemInt(hWnd, IDC_EDIT_StartFrame, NULL, FALSE));
				AIREXPORTER->SetEndFrame(GetDlgItemInt(hWnd, IDC_EDIT_EndFrame, NULL, FALSE));

				GetDlgItemText(hWnd, IDC_EDIT_ANIMATION_NAME, szAnimationName, 256);
				if (strlen(szAnimationName) == 0)
				{
					MessageBox(hWnd, "请输入动画的名字！", "Warning", MB_OK);
					return TRUE;
				}
				AIREXPORTER->SetAnimationName(szAnimationName);
				EndDialog(hWnd, 0);
				break;

			case IDC_CHK_LEFTHAND:
				AIREXPORTER->SetLeftHand(Button_GetCheck(GetDlgItem(hWnd, IDC_CHK_LEFTHAND)));
				//imp->SetLHCoord(Button_GetCheck(GetDlgItem(hWnd, IDC_CHK_LEFTHAND)));
				//int a = 0;
				break;

			case IDC_CHECK_YUP:
				AIREXPORTER->SetUpY(Button_GetCheck(GetDlgItem(hWnd, IDC_CHECK_YUP)));
				break;

			case IDC_CHECK_EXPORT_MATERIAL:
				{
					HWND hEdit = GetDlgItem(hWnd, IDC_EDT_MATERIAL);

					//DWORD dwStyle = GetWindowLong(hEdit, GWL_STYLE);

					if (Button_GetCheck(GetDlgItem(hWnd, IDC_CHECK_EXPORT_MATERIAL)))
					{
						::SendMessage(hEdit, EM_SETREADONLY, FALSE, 0);
					}
					else
					{
						::SendMessage(hEdit, EM_SETREADONLY, TRUE, 0);
					}

					//SetWindowLong(hEdit, GWL_EXSTYLE, dwStyle);

				}
				break;

			case IDC_CHK_SAVEASBINARY:
				AIREXPORTER->SetSaveAsBinary(Button_GetCheck(GetDlgItem(hWnd, IDC_CHK_SAVEASBINARY)));
				break;

			case IDC_CHK_SHARE_SKELETON:
				AIREXPORTER->SetShareSkeleton(Button_GetCheck(GetDlgItem(hWnd, IDC_CHK_SHARE_SKELETON)));
				break;

			case IDC_EDIT_StartFrame:
				if (HIWORD(wParam) == WM_CHAR)
				{
					AIREXPORTER->SetStartFrame(GetDlgItemInt(hWnd, IDC_EDIT_StartFrame, NULL, FALSE));
				}
				break;

			case IDC_BUTTON_CHOOSE_FILE:
				imp->OnChooseSkeletonFile();
				break;

			case IDC_CHK_EXPORT_VERTEXANIMATION:
				imp->SetExportVertexAnimation(Button_GetCheck(GetDlgItem(hWnd, IDC_CHK_EXPORT_VERTEXANIMATION)));
				AIREXPORTER->SetMeshAnimationType(eMAT_VertexAnimation, Button_GetCheck(GetDlgItem(hWnd, IDC_CHK_EXPORT_VERTEXANIMATION)));
				break;
			case IDC_CHK_POSITION:
				AIREXPORTER->SetVertexAniExportMask(eVAM_Position, Button_GetCheck(GetDlgItem(hWnd, IDC_CHK_POSITION)));
				break;
			case IDC_CHK_UV:
				AIREXPORTER->SetVertexAniExportMask(eVAM_UV, Button_GetCheck(GetDlgItem(hWnd, IDC_CHK_UV)));
				break;
			case IDC_CHK_COLOR:
				AIREXPORTER->SetVertexAniExportMask(eVAM_Color, Button_GetCheck(GetDlgItem(hWnd, IDC_CHK_COLOR)));
				break;
			case IDC_CHK_EXPORT_SKELETAL_ANIMATION:
				imp->SetExportSkeletalAnimation(Button_GetCheck(GetDlgItem(hWnd, IDC_CHK_EXPORT_SKELETAL_ANIMATION)));
				AIREXPORTER->SetMeshAnimationType(eMAT_SkeletionAnimation, Button_GetCheck(GetDlgItem(hWnd, IDC_CHK_EXPORT_SKELETAL_ANIMATION)));
				break;
			}
			break;
		case BM_GETCHECK:
			if (LOWORD(wParam) == IDC_CHK_LEFTHAND)
			{
				AIREXPORTER->SetLeftHand(true);
			}
			else if (LOWORD(wParam) == IDC_CHECK_EXPORT_MATERIAL)
			{
				HWND hEdit = GetDlgItem(hWnd, IDC_EDT_MATERIAL);
				DWORD dwStyle = GetWindowLong(hEdit, GWL_STYLE);
				dwStyle &= ~ES_READONLY;
				SetWindowLong(hEdit, GWL_STYLE, dwStyle);
				AIREXPORTER->SetExportMaterial(true);
			}
			break;

		case WM_CHAR:
			break;
	}
	return 0;
}


//--- AIRExporter -------------------------------------------------------
AIRExporter::AIRExporter() : m_strVersion("0.2"), m_strCoord(STR_COORDINATE_RIGHT), m_strUpVec(STR_AXIS_Z), m_hWndDlg(NULL), m_bLeftHand(false), m_nStartFrame(0),
	m_nEndFrame(0)
	, m_nTotalFrame(0)
	, m_pSelectedSkeleton(NULL), m_bYIsUp(false), m_bExportMaterial(false), m_bExportSelected(false)
	, m_bSaveAsBinary(false)
	, m_bShareSkeleton(false)
	, m_nVertexAnimationMask(0)
	, m_nAnimationsMark(0)
	, m_bExportStaticMesh(false)
	, m_bExportSkeletalAnimation(false)
	, m_nMeshCount(0)
	, m_nTotalNodes(0)
	, m_nCurNode(0)
	, m_nMaterialCount(0)
	, m_bSelected(false)
	, m_bLHCoord(false)
	, m_bExportAnimateOnly(false)
	, m_bExportVertexAnimation(false)
{

}

AIRExporter::~AIRExporter() 
{

}

int AIRExporter::ExtCount()
{
	#pragma message(TODO("Returns the number of file name extensions supported by the plug-in."))
	return 1;
}

const TCHAR *AIRExporter::Ext(int n)
{		
	#pragma message(TODO("Return the 'i-th' file name extension (i.e. \"3DS\")."))
	//return _T("");
	switch (n)
	{
	case 0:
		return _T("airmesh");
	case 1:
		return _T("airanim");
	}
	return _T("airmesh");
}

const TCHAR *AIRExporter::LongDesc()
{
	#pragma message(TODO("Return long ASCII description (i.e. \"Targa 2.0 Image File\")"))
	return _T("AIR Engine file");
}
	
const TCHAR *AIRExporter::ShortDesc() 
{			
	#pragma message(TODO("Return short ASCII description (i.e. \"Targa\")"))
	return _T("AIR Mesh File");
}

const TCHAR *AIRExporter::AuthorName()
{			
	#pragma message(TODO("Return ASCII Author name"))
	return _T("Liangairan");
}

const TCHAR *AIRExporter::CopyrightMessage() 
{	
	#pragma message(TODO("Return ASCII Copyright message"))
	return _T("");
}

const TCHAR *AIRExporter::OtherMessage1() 
{		
	//TODO: Return Other message #1 if any
	return _T("");
}

const TCHAR *AIRExporter::OtherMessage2() 
{		
	//TODO: Return other message #2 in any
	return _T("");
}

unsigned int AIRExporter::Version()
{				
	#pragma message(TODO("Return Version number * 100 (i.e. v3.01 = 301)"))
	return 100;
}

void AIRExporter::ShowAbout(HWND hWnd)
{			
	// Optional
}

BOOL AIRExporter::SupportsOptions(int ext, DWORD options)
{
	#pragma message(TODO("Decide which options to support.  Simply return true for each option supported by each Extension the exporter supports."))
	return TRUE;
}

void AIRExporter::OnInitDialog(HWND hWnd)
{
	CenterWindow(hWnd,GetParent(hWnd));

	CMaxInterface* pInterface = AIREXPORTER->GetMaxInterface();
	Interval ARange =  pInterface->GetInterface()->GetAnimRange(); //获得动画的长度
	SetDlgItemInt(hWnd, IDC_EDIT_StartFrame, ARange.Start(), TRUE);
	SetDlgItemInt(hWnd, IDC_EDIT_EndFrame, ARange.End() / GetTicksPerFrame(), TRUE);
	AIREXPORTER->SetTotalFrame(ARange.End() / GetTicksPerFrame());

	Button_SetCheck(GetDlgItem(hWnd, IDC_CHK_LEFTHAND), true);
	Button_SetCheck(GetDlgItem(hWnd, IDC_CHECK_YUP), true);
	Button_SetCheck(GetDlgItem(hWnd, IDC_CHECK_EXPORT_MATERIAL), true);
	Button_SetCheck(GetDlgItem(hWnd, IDC_CHK_SAVEASBINARY), true);
	Button_SetCheck(GetDlgItem(hWnd, IDC_CHK_EXPORT_SKELETAL_ANIMATION), true);

	char szMaterialName[256] = { 0 };
	if (!m_strFileInput.empty())
	{
		strcpy(szMaterialName, m_strFileInput.c_str());
	}
	
	char* p = strrchr(szMaterialName, '.');
	if (p)
	{
		szMaterialName[p - szMaterialName] = '\0';
	}
	
	if (szMaterialName)
	{
		char* pName = strrchr(szMaterialName, '\\');
		if (pName == NULL)
		{
			pName = strrchr(szMaterialName, '/');
		}
		if (pName)
		{
			strcpy(szMaterialName, pName + 1);
			strcat(szMaterialName, ".material");
		}
		else
		{
			strcat(szMaterialName, ".material");
		}
	}
	else
		strcat(szMaterialName, ".material");
	SetDlgItemText(hWnd, IDC_EDT_MATERIAL, szMaterialName);

	SetDlgHWnd(hWnd);
}

bool AIRExporter::Create(ExpInterface *pExpInterface, Interface *pInterface, HWND hWnd)
{
	//m_MaxInterface = pInterface;
	m_MaxInterface.Create(pExpInterface, pInterface);
	m_hMainWnd = hWnd;
	return true;
}


int	AIRExporter::DoExport(const TCHAR *name,ExpInterface *ei,Interface *i, BOOL suppressPrompts, DWORD options)
{
	#pragma message(TODO("Implement the actual file Export here and"))
	Create(ei, i, GetActiveWindow());
	SetFileInputName(name);
	if(!suppressPrompts)
		DialogBoxParam(hInstance, 
				MAKEINTRESOURCE(IDD_PANEL), 
				GetActiveWindow(), 
				AIRExporterOptionsDlgProc, (LPARAM)this);

	#pragma message(TODO("return TRUE If the file is exported properly"))

	if (!g_bExport)
	{
		AIREXPORTER->Clear();
		//CAIRExporter::KillSelf();
		return 1;
	}

	AIREXPORTER->SetExportSelected(options & SCENE_EXPORT_SELECTED);

	m_ip = i;

	AllocConsole();   //创建console

	//m_ip->ProgressStart(GetString(IDS_PROGRESS), TRUE, fn, NULL);
	bool bResult = false;

	AIREXPORTER->InitExportConfig();

	const char* szSkeleton = name;
	const char* szExt = strstr(szSkeleton, ".");

	char szAnimFile[256] = { 0 };
	char szMeshFile[256] = { 0 };

	AIREXPORTER->SetKeyframeFile("");


	if (m_bExportAnimateOnly)
	{
		CAIRSkeleton skeletonExport;
		bResult = AIREXPORTER->ExportSkeleton(skeletonExport);
		if (bResult)
		{
			strncpy(szAnimFile, szSkeleton, szExt - szSkeleton);
			char szTmp[256] = { 0 };
			sprintf_s(szTmp, 255, "_%s.airanim", GetAnimationName().c_str());
			strcat(szAnimFile, szTmp);
			bResult = AIREXPORTER->ExportAnimation(&skeletonExport, szAnimFile);
		}
		else
		{
			MessageBox(NULL, "没有骨骼动画！", "Error", MB_OK);
		}
	}
	else
	{
		bool bExportMesh = false;
		CAIRSkeleton skeletonExport;
		bResult = AIREXPORTER->ExportSkeleton(skeletonExport);
		strncpy(szMeshFile, szSkeleton, szExt - szSkeleton);
		strcat(szMeshFile, ".airmesh");
		if (bResult)
		{
			bResult = AIREXPORTER->ExportStaticModel(&skeletonExport, szMeshFile);
		}
		else
		{
			bResult = AIREXPORTER->ExportStaticModel(NULL, szMeshFile);
		}
		bExportMesh = true;
		if (m_bExportSkeletalAnimation && skeletonExport.HasBones())
		{
			strncpy(szAnimFile, szSkeleton, szExt - szSkeleton);
			char szTmp[256] = { 0 };
			sprintf_s(szTmp, 255, "_%s.airanim", GetAnimationName().c_str());
			strcat(szAnimFile, szTmp);

			//下面开始导出动画 
			//strncpy(szAnimationFile, szSkeleton, szExt - szSkeleton);
			//strcat(szAnimationFile, ".airanim");
			bResult = AIREXPORTER->ExportAnimation(&skeletonExport, szAnimFile);
		}

		if (m_bExportVertexAnimation)
		{
			char szKeyframefile[256] = { 0 };
			strncpy(szKeyframefile, szSkeleton, szExt - szSkeleton);
			strcat(szKeyframefile, ".airkey");
			AIREXPORTER->SetKeyframeFile(szKeyframefile);

			if (!bExportMesh)
			{
				strncpy(szMeshFile, szSkeleton, szExt - szSkeleton);
				strcat(szMeshFile, ".airmesh");
				bResult = AIREXPORTER->ExportStaticModel(NULL, szMeshFile);
				bExportMesh = true;
			}
			bResult = AIREXPORTER->ExportVertexAnimation(szKeyframefile);
		}

		if (!bExportMesh)
		{
			strncpy(szMeshFile, szSkeleton, szExt - szSkeleton);
			strcat(szMeshFile, ".airmesh");
			bResult = ExportStaticModel(NULL, szMeshFile);
		}

	}

	AIREXPORTER->Clear();


	//m_ip->ProgressEnd();

	cprintf("now the max_node count is:%d\n", CMaxMaterial::s_nNewCount);
	cprintf("now the max_mesh count is:%d\n", CMaxMesh::s_nNewCount);
	cprintf("now the max_material count is:%d\n", CMaxMaterial::s_nNewCount);
	cprintf("now the airbones count is:%d\n", CAIRBone::s_nNewCount);

	if (bResult)
	{
		MessageBox(NULL, "导出模型成功！", "提示", MB_OK);
	}
	else
	{
		MessageBox(NULL, "导出失败！请检查！", "提示", MB_OK);
	}

	FreeConsole();
	return FALSE;
}

bool AIRExporter::OnBtnExportAnimation()
{
	char szFileName[256] = { 0 };
	GetDlgItemText(AIREXPORTER->GetDlgHWnd(), IDC_EDIT_SKELETAL_FILE, szFileName, 256);

	//if (strlen(szFileName) == 0)
	//{
	//	MessageBox(AIREXPORTER->GetDlgHWnd(), "请先输入骨骼文件！", "Warning", MB_OK);
	//	return false;
	//}

	GetDlgItemText(AIREXPORTER->GetDlgHWnd(), IDC_EDIT_ANIMATION_NAME, szFileName, 256);
	if (strlen(szFileName) == 0)
	{
		MessageBox(AIREXPORTER->GetDlgHWnd(), "请输入动画的名字！", "Warning", MB_OK);
		return false;
	}

	AIREXPORTER->SetAnimationName(szFileName);

	return true;
	//AIREXPORTER->ExportAnimation()
}

void AIRExporter::SetExportAnimateOnly(bool bExportAni)
{
	m_bExportAnimateOnly = bExportAni;
}

void AIRExporter::SetExportStaticMesh(bool bExportStatic)
{
	m_bExportStaticMesh = bExportStatic;
}

void AIRExporter::OnChooseSkeletonFile()
{
	OPENFILENAME   ofn;               //   common   dialog   box   structure   
	char   szFile[260];               //   buffer   for   file   name   
	//HWND   hwnd;                             //   owner   window   
	HANDLE   hf;                             //   file   handle   

	//   Initialize   OPENFILENAME   
	ZeroMemory(&ofn,   sizeof(ofn));   
	ofn.lStructSize   =   sizeof(ofn);   
	ofn.hwndOwner   =   GetActiveWindow();   
	ofn.lpstrFile   =   szFile;   
	//   
	//   Set   lpstrFile[0]   to   '\0'   so   that   GetOpenFileName   does   not     
	//   use   the   contents   of   szFile   to   initialize   itself.   
	//   
	ofn.lpstrFile[0]   =   '\0';   
	ofn.nMaxFile   =   sizeof(szFile);   
	ofn.lpstrFilter   =   "AIR Animation Files (*.airanim)|*.airanim|All Files (*.*)|*.*||";   
	ofn.nFilterIndex   =   1;   
	ofn.lpstrFileTitle   =   NULL;   
	ofn.nMaxFileTitle   =   0;   
	ofn.lpstrInitialDir   =   NULL;   
	ofn.Flags   =   OFN_PATHMUSTEXIST   |   OFN_FILEMUSTEXIST;   

	//   Display   the   Open   dialog   box.     

	if   (GetOpenFileName(&ofn)==TRUE)
	{
		if (AIREXPORTER->CreateSelectedSkeleton(szFile))
		{
			SetDlgItemText(AIREXPORTER->GetDlgHWnd(), IDC_EDIT_SKELETAL_FILE, szFile);
		}
	}

}

bool AIRExporter::OnBtnExport()
{
	HWND hWnd = AIREXPORTER->GetDlgHWnd();
	AIREXPORTER->SetLeftHand(Button_GetCheck(GetDlgItem(hWnd, IDC_CHK_LEFTHAND)));
	AIREXPORTER->SetUpY(Button_GetCheck(GetDlgItem(hWnd, IDC_CHECK_YUP)));
	AIREXPORTER->SetExportMaterial(Button_GetCheck(GetDlgItem(hWnd, IDC_CHECK_EXPORT_MATERIAL)));
	SetExportVertexAnimation(false);
	if (AIREXPORTER->IsExportMaterial())
	{
		char szMaterialFileName[256] = { 0 };
		GetDlgItemText(hWnd, IDC_EDT_MATERIAL, szMaterialFileName, 256);
		AIREXPORTER->SetMaterialFileName(szMaterialFileName);
		if (strlen(szMaterialFileName) == 0)
		{
			MessageBox(hWnd, "请输入要导出的材质文件名！", "提示", MB_OK);
			return false;
		}
	}
	AIREXPORTER->SetStartFrame(GetDlgItemInt(hWnd, IDC_EDIT_StartFrame, NULL, FALSE));
	AIREXPORTER->SetEndFrame(GetDlgItemInt(hWnd, IDC_EDIT_EndFrame, NULL, FALSE));
	SetExportStaticMesh(false);
	SetExportAnimateOnly(false);
	char szAnimationName[256] = { 0 };
	GetDlgItemText(hWnd, IDC_EDIT_ANIMATION_NAME, szAnimationName, 256);
	if (m_bExportSkeletalAnimation && strlen(szAnimationName) == 0)
	{
		MessageBox(hWnd, "请输入动画的名字！", "Warning", MB_OK);
		return false;
	}
	AIREXPORTER->SetAnimationName(szAnimationName);

	return true;
}
