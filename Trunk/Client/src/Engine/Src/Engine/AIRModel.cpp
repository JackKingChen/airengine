#include "stdafx.h"
#include "AIRModel.h"
#include "FileSystem.h"
#include "Util.h"
//#include "ModelManager.h"
#include "DataStream.h"
#include "Engine.h"
#include "InstancingObjectManager.h"
#include "IMaterial.h"
#include "AIRAnimationState.h"
#include "AIRSkeletonAnimation.h"
#include "StringUtil.h"

IMPLEMENT_RTTI(AIRModel, Resource)

AIRModel::AIRModel() : /*m_bStatic(false), */m_bLeftCoord(false)
, m_animationType(eMAT_NoAnimation)
//, m_pSkeletalRes(NULL)
, m_pVertexAnimationRes(NULL)
, m_pAnimationMixRes(NULL)
, m_fFileVersion(0.1f)
{

}

AIRModel::~AIRModel()
{
}

int AIRModel::Release()
{
    if (DropRef() == 0)
    {
        Clear();
    }
    return GetRef();
}

void AIRModel::Clear()
{
    for (MESH_ITER it = m_mapSubMeshes.begin(); it != m_mapSubMeshes.end(); ++it)
    {
        CAIRSubMesh* pMesh = it->second;
        pMesh->Clear();
        SafeDelete(pMesh);
    }

    //if (m_pSkeletalRes)
    //{
    //    RESOURCE_MANAGER->ClearResource(m_pSkeletalRes);
    //}

    if (m_pAnimationMixRes)
    {
        //ANIMATIONMIX_MANAGER->ClearAnimationMixRes(m_pAnimationMixRes);
        RESOURCE_MANAGER->ClearResource(m_pAnimationMixRes);
        m_pAnimationMixRes = NULL;
    }
    m_mapSubMeshes.clear();
}

bool AIRModel::CreateFromFile(const char* szFilename)
{
    CDataStream* pStream = FILESYSTEM->GetFileStream(szFilename);

    if (pStream == NULL)
    {
        return false;
    }

    //char buff[512];
    AIRString lineBuff;//
    float fVersion = 0;
    int numMeshes = 0; 
    char szTemp[512];
    char szSkeletalFile[256] = { 0 };
    char szKeyframeFile[256] = { 0 };

    bool bStaticModel = false;   //静态模型 
    //MeshAnimationType animationType = eMAT_NoAnimation;

    while (!pStream->Eof())
    {
        /* Read whole line */
        lineBuff = pStream->GetLine();
        //fgets (buff, sizeof (buff), fp);

        if (sscanf (/*buff*/lineBuff.c_str(), " AIRVersion %f", &fVersion) == 1)
        {
            if (fVersion != 0.1f)
            {
                /* Bad version */
                //fprintf (stderr, "Error: bad model version\n");
                //fclose (fp);
                SafeDelete(pStream);
                return false;
            }
        }
        else if (sscanf(/*buff*/lineBuff.c_str(), "Coordinate %s", szTemp) == 1)
        {
            if (strcmp(szTemp, STR_COORDINATE_LEFT) == 0)
            {
                //m_bLeftCoord = true;
            }
        }
        else if (sscanf (/*buff*/lineBuff.c_str(), "UpVec %s", &szTemp) == 1)
        {
            if (strcmp(szTemp, STR_AXIS_Z) == 0)
            {
                //m_upVec = Vector3Df(0.0f, 0.0f, 1.0f);
            }
            else
            {
                //m_upVec = Vector3Df(0.0f, 1.0f, 0.0f);
            }
        }
        else if (sscanf (/*buff*/lineBuff.c_str(), " numMeshes %d", &numMeshes) == 1)
        {
            if (numMeshes > 0)
            {
                //m_vtSubMeshes.reserve(numMeshes);
            }
            //if (pModel->nNumJoints > 0)
            //{
            //    /* Allocate memory for base skeleton joints */
            //    pModel->pBaseSkeleton = (LPMD5JOINT)
            //        calloc (pModel->nNumJoints, sizeof (MD5JOINT));
            //}
        }
        else if (sscanf(/*buff*/lineBuff.c_str(), "skeletalFile %s", szSkeletalFile) == 1)
        {
            const char* p = strstr(/*buff*/lineBuff.c_str(), " ");
            strcpy(szSkeletalFile, p + 1);
            if (strcmp(szSkeletalFile, "none") == 0)
            {
                bStaticModel = true;
                szSkeletalFile[0] = 0;
            }
            else
            {
                m_animationType = eMAT_SkeletionAnimation;
            }
        }
        else if (sscanf(lineBuff.c_str(), "keyframe_animation_file %s", szKeyframeFile) == 1)
        {
            const char* p = strstr(/*buff*/lineBuff.c_str(), " ");
            strcpy(szKeyframeFile, p + 1);
            m_animationType = eMAT_VertexAnimation;
        }

        else if (strncmp (/*buff*/lineBuff.c_str(), "mesh {", 6) == 0)
        {
            CAIRSubMesh* pSubMesh = new CAIRSubMesh;
            pSubMesh->SetAnimationType(m_animationType);
            //pSubMesh->SetIsStatic(bStaticModel);
            if (pSubMesh->LoadFromFileData(/*fp, buff*/pStream, szFilename))
            {
                //m_vtSubMeshes.push_back(pSubMesh);
                m_mapSubMeshes.insert(std::make_pair(pSubMesh->GetName(), pSubMesh));
            }
            else
            {
                pSubMesh->Clear();
                SafeDelete(pSubMesh);
            }

            //curr_mesh++;
        }
    }

    ComputeBoundingBox();

    //fclose(fp);

	//创建对应的骨骼与动画
	/*
    if (strlen(szSkeletalFile) > 0)
    {
        m_pSkeletalRes = RESOURCE_MANAGER->CreateResource<CAIRSkeletonRes>(szSkeletalFile); //MODELMGR->CreateSkeletonRes(szSkeletalFile);

        if (m_pSkeletalRes != NULL)
        {
            m_animationType = eMAT_SkeletionAnimation;
            //m_bStatic = true;

            //读取相关的混合文件，注意，混合文件和骨骼文件名字一定要相同，只是后缀名不同
            AIRString strAnimationMix = szSkeletalFile;
            int nPos = strAnimationMix.find('.');
            strAnimationMix = strAnimationMix.substr(0, nPos + 1) + "animix";
            m_pAnimationMixRes = RESOURCE_MANAGER->CreateResource<AIRAnimationMixRes>(strAnimationMix.c_str()); //ANIMATIONMIX_MANAGER->CreateAnimationMixRes(strAnimationMix.c_str());
        }
        //else
        //    m_bStatic = false;
    }
    

    if (strlen(szKeyframeFile) > 0)
    {
        m_pVertexAnimationRes = RESOURCE_MANAGER->CreateResource<AIRKeyframeAnimationRes>(szKeyframeFile); //MODELMGR->CreateVertexAnimationRes(szKeyframeFile);
        //m_bStatic = m_pVertexAnimationRes == NULL;
        if (m_pVertexAnimationRes)
        {
            m_animationType = eMAT_VertexAnimation;
        }
    }

    

    for (MESH_ITER it = m_mapSubMeshes.begin(); it != m_mapSubMeshes.end(); ++it)
    {
        it->second->SetSkeletonRes(m_pSkeletalRes);
    }
	*/

    m_strName = szFilename;

    //释放文件流
    SafeDelete(pStream);
    return true;
}

bool AIRModel::CreateFromBinaryFile(const char* szFilename)
{
    //SetFilename(szFilename);
    m_strName = szFilename;
	
	char szAnimCtrlFile[256] = { 0 };
	CStringUtil::GetFilenameNoExt(szFilename, szAnimCtrlFile);
	strcat(szAnimCtrlFile, ".airctrl");
	LoadAnimationConnects(szAnimCtrlFile);
    return LoadFromFile(szFilename, ENGINE_INST->GetConfig()->m_bBackgroundThread);  //Load();
    
}

void AIRModel::ComputeBoundingBox()
{
	CAIRSubMesh* pAIRMesh = NULL;
    m_boundingBox.MaxEdge = Vector3Df(-99999.0f, -99999.0f, -99999.0f);
    m_boundingBox.MinEdge = Vector3Df(99999.0f, 99999.0f, 99999.0f);
	m_obbinfs.clear();
    for (MESH_ITER it = m_mapSubMeshes.begin(); it != m_mapSubMeshes.end(); ++it)
    {
        pAIRMesh = it->second;//m_vtSubMeshes[i];
        m_boundingBox.MaxEdge.x = Max(m_boundingBox.MaxEdge.x, pAIRMesh->GetBoundingBox().MaxEdge.x);
        m_boundingBox.MaxEdge.y = Max(m_boundingBox.MaxEdge.y, pAIRMesh->GetBoundingBox().MaxEdge.y);
        m_boundingBox.MaxEdge.z = Max(m_boundingBox.MaxEdge.z, pAIRMesh->GetBoundingBox().MaxEdge.z);

        m_boundingBox.MinEdge.x = Min(m_boundingBox.MinEdge.x, pAIRMesh->GetBoundingBox().MinEdge.x);
        m_boundingBox.MinEdge.y = Min(m_boundingBox.MinEdge.y, pAIRMesh->GetBoundingBox().MinEdge.y);
        m_boundingBox.MinEdge.z = Min(m_boundingBox.MinEdge.z, pAIRMesh->GetBoundingBox().MinEdge.z);

		const OBBoxf & obb = pAIRMesh->GetObb();
		m_obbinfs.push_back(OBBInf(obb.Center(),obb.AxisLine(0),obb.AxisLine(1),obb.AxisLine(2)));
    }

    //m_vCenter = (m_boundingBox.MaxEdge + m_boundingBox.MinEdge) / 2;
}

void AIRModel::GetMeshVector(AIRVector<CAIRSubMesh*>& meshVector)
{
    meshVector.clear();
	MESH_ITER it_end = m_mapSubMeshes.end();
    for (MESH_ITER it = m_mapSubMeshes.begin(); it != it_end; ++it)
    {
        meshVector.push_back(it->second);
        INSTANCING_MANAGER->CreateInstancingObject(it->second);
    }
}

bool AIRModel::Cache()
{
    if (!m_bReady)
    {
        if (m_pStream)
        {
			char szAnimCtrlFile[256] = { 0 };
			CStringUtil::GetFilenameNoExt(m_strFilename.c_str(), szAnimCtrlFile);
			strcat(szAnimCtrlFile, ".airctrl");
			LoadAnimationConnects(szAnimCtrlFile);

            m_pStream->Read(&m_fFileVersion, sizeof(float));
            if (m_fFileVersion < ENGINE_INST->GetMeshFileVersion())
            {
                SafeDelete(m_pStream);
                return false;
            }

            bool bStaticModel = false;

            u8 bCoord;
            m_pStream->Read(&bCoord, sizeof(u8));
            u8 bUpVec;
            m_pStream->Read(&bUpVec, sizeof(u8));

            int nMeshNum = 0;
            m_pStream->Read(&nMeshNum, sizeof(int));

            u8 nAnimationType = 0;
            m_pStream->Read(&nAnimationType, sizeof(u8));

            m_animationType = nAnimationType;

			
			/*
            char szSkelAnimationFile[256] = { 0 };
            char szKeyframeAnimationFile[256] = { 0 };

            if (m_animationType & eMAT_SkeletionAnimation)
            {
                m_pStream->Read(szSkelAnimationFile, 256);
            }

            if (m_animationType & eMAT_VertexAnimation)
            {
                m_pStream->Read(szKeyframeAnimationFile, 256);
            }
			*/

            for (int i = 0; i < nMeshNum; ++i)
            {
                CAIRSubMesh* pSubMesh = new CAIRSubMesh;
                pSubMesh->SetAnimationType(m_animationType);
                //pSubMesh->SetIsStatic(bStaticModel);
                if (pSubMesh->LoadFromFileDataBinary(m_pStream, this))
                {
                    m_mapSubMeshes.insert(std::make_pair(pSubMesh->GetName(), pSubMesh));
                }
                else
                {
                    pSubMesh->Clear();
                    SafeDelete(pSubMesh);
                }
            }

			/*
            if (nAnimationType & eMAT_SkeletionAnimation)    //0代表骨骼动画
            {
                m_pSkeletalRes = RESOURCE_MANAGER->CreateResource<CAIRSkeletonRes>(szSkelAnimationFile); //MODELMGR->CreateSkeletonRes(szAnimationFile);

                if (m_pSkeletalRes == NULL)
                {
                    m_animationType = eMAT_NoAnimation;
                }

                for (MESH_ITER it = m_mapSubMeshes.begin(); it != m_mapSubMeshes.end(); ++it)
                {
                    it->second->SetSkeletonRes(m_pSkeletalRes);
                }

                //读取相关的混合文件，注意，混合文件和骨骼文件名字一定要相同，只是后缀名不同
                AIRString strAnimationMix = szSkelAnimationFile;
                int nPos = strAnimationMix.find('.');
                strAnimationMix = strAnimationMix.substr(0, nPos + 1) + "animix";
                m_pAnimationMixRes = RESOURCE_MANAGER->CreateResource<AIRAnimationMixRes>(strAnimationMix.c_str()); //ANIMATIONMIX_MANAGER->CreateAnimationMixRes(strAnimationMix.c_str());
            }
            if (nAnimationType & eMAT_VertexAnimation)  //顶点动画
            {
                m_pVertexAnimationRes = RESOURCE_MANAGER->CreateResource<AIRKeyframeAnimationRes>(szKeyframeAnimationFile); //MODELMGR->CreateVertexAnimationRes(szAnimationFile);
                //m_bStatic = m_pVertexAnimationRes == NULL;
                if (m_pVertexAnimationRes != NULL)
                {
                    for (MESH_ITER it = m_mapSubMeshes.begin(); it != m_mapSubMeshes.end(); ++it)
                    {
                        it->second->SetKeyframeAnimationRes(m_pVertexAnimationRes);
                    }
                }
            }
			*/

			int numBones = 0;
			m_pStream->Read(&numBones, sizeof(numBones));
			m_vtBoneHierarchy.resize(numBones);

			for (int i = 0; i < numBones; ++i)
			{
				LPAIRBONE pBone = &m_vtBoneHierarchy[i];
				m_pStream->Read(pBone, sizeof(pBone->szName) + sizeof(pBone->nParentID) + sizeof(pBone->vPosition) + sizeof(pBone->qRotation));
				//因为是左手坐标，所以要取反
				//pBone->qRotation.Inverse();    //右手坐标系时又要打开这个
			}

            SafeDelete(m_pStream);
            m_bReady = true;
        }
        
		/*
        if (m_pSkeletalRes)
        {
            if (!m_pSkeletalRes->IsReady())
            {
                if (m_pSkeletalRes->IsLoaded())
                {
                    m_pSkeletalRes->Cache();
                }
            }

            m_bReady = m_pSkeletalRes->IsReady();
			if ( !m_bReady )
			{
				return false;
			}
        }
		*/

        if (m_pAnimationMixRes)
        {
            if (!m_pAnimationMixRes->IsReady())
            {
                if (m_pAnimationMixRes->GetStatus() == Resource::ResStatus_Loaded)
                {
                    m_pAnimationMixRes->Cache();
                }
                else if (m_pAnimationMixRes->GetStatus() == Resource::ResStatus_LoadFailed)
                {
                    RESOURCE_MANAGER->ClearResource(m_pAnimationMixRes);
                    m_pAnimationMixRes = NULL;
                }
            }

            if (m_pAnimationMixRes)
            {
                m_bReady = m_pAnimationMixRes->IsReady();

                if ( !m_bReady )
                {
                    return false;
                }
            }
            
        }

        if (m_pVertexAnimationRes)
        {
            if (!m_pVertexAnimationRes->IsReady())
            {
                if (m_pVertexAnimationRes->IsLoaded())
                {
                    m_pVertexAnimationRes->Cache();
                }
            }

            m_bReady = m_pVertexAnimationRes->IsReady();

			if ( !m_bReady )
			{
				return false;
			}
        }

        //cache submesh中的材质
        bool bMaterialReady = true;
        for (MESH_ITER it = m_mapSubMeshes.begin(); it != m_mapSubMeshes.end(); ++it)
        {
            IMaterial* pMaterial = it->second->GetMaterial();
            if (!pMaterial->IsReady())
            {
                if (!pMaterial->Cache())
                {
                    bMaterialReady = false;
                }
            }
        }
        m_bReady = bMaterialReady;

        if (m_bReady)
        {
            NotifyAllListenerReady();
        }
    }
    
    return m_bReady;
}

/*
bool AIRModel::Load()
{
    CDataStream* pStream = FILESYSTEM->GetFileStream(m_strFilename.c_str());

    if (pStream == NULL)
    {
        return false;
    }
    //pStream->Seek(0);
    
    m_bLoaded = true;
    return m_bLoaded;
}
*/

void AIRModel::AddSubMesh(const AIRString& strName, CAIRSubMesh* pSubmesh)
{
	m_mapSubMeshes.insert(std::make_pair(strName, pSubmesh));
}

//void AIRModel::SetSkeletonRes(CAIRSkeletonRes* pSkeletonRes)
//{
//	if (m_pSkeletalRes)
//	{
//		RESOURCE_MANAGER->ClearResource(m_pSkeletalRes);
//	}
//
//	m_pSkeletalRes = pSkeletonRes;
//}

int AIRModel::GetBoneIndexByName(const char* szName)
{
	for (size_t i = 0; i < m_vtBoneHierarchy.size(); ++i)
	{
		if (strcmp(m_vtBoneHierarchy[i].szName, szName) == 0)
		{
			return i;
		}
	}

	return -1;
}

void AIRModel::CreateAnimationStates(AIRMap<AIRString, CAIRAnimationState*>& mapAnimations)
{
	typedef AIRMap<AIRString, CAIRSkeletonAnimation*>::iterator ANIMATION_ITER;
	for (ANIMATION_ITER it = m_mapAnimation.begin(); it != m_mapAnimation.end(); ++it)
	{
		CAIRSkeletonAnimation* pAnimation = it->second;
		CAIRAnimationState* pAnimationState = new CAIRAnimationState(pAnimation);
		pAnimationState->SetTimeLength(pAnimation->GetTimeRange());
		mapAnimations.insert(std::make_pair(it->first, pAnimationState));
	}
}

CAIRSkeletonAnimation* AIRModel::GetAnimation(const char* szAnimationName)
{
	typedef AIRMap<AIRString, CAIRSkeletonAnimation*>::iterator ANIMATION_ITER;
	ANIMATION_ITER it = m_mapAnimation.find(AIRString(szAnimationName));
	if (it != m_mapAnimation.end())
	{
		return it->second;
	}

	return NULL;
}

CAIRSkeletonAnimation* AIRModel::GetAnimation( int nIndex )
{
	if ( nIndex < 0 || nIndex >= m_mapAnimation.size() )
	{
		++nIndex;
	}

	typedef AIRMap<AIRString, CAIRSkeletonAnimation*>::iterator ANIMATION_ITER;
	ANIMATION_ITER it = m_mapAnimation.begin();
	std::advance(it, nIndex);
	if (it != m_mapAnimation.end())
	{
		return it->second;
	}
	return NULL;
}

void AIRModel::AddSkeletonAnimation(const AIRString& strName, CAIRSkeletonAnimation* pAnimation)
{
	m_mapAnimation.insert(std::make_pair(strName, pAnimation));
}

void AIRModel::LoadAnimationConnects(const char* szFilename)
{
	CMarkupSTL xml;
	AIRString strFile = FILESYSTEM->GetFileFullPath(szFilename);
	if (!xml.Load(strFile.c_str()))
	{
		return;
	}

	//xml.IntoElem();

	AIRString strNode;

	if (xml.FindElem("Animations"))
	{
		xml.IntoElem();
		while (xml.FindElem())
		{
			strNode = xml.GetTagName().c_str();
			if (strNode == "Animation")
			{
				AIRString strName = xml.GetAttrib("name").c_str();
				AIRString strFile = xml.GetAttrib("file").c_str();
				m_mapAnimationNames.insert(std::make_pair(strName, strFile));
			}
		}

		xml.OutOfElem();
	}
}

CAIRSkeletonAnimation* AIRModel::CreateSkeletonAnimation(const char* szName)
{
	AIRMap<AIRString, CAIRSkeletonAnimation*>::iterator it = m_mapAnimation.find(szName);
	if (it != m_mapAnimation.end())
	{
		return it->second;
	}

	AIRMap<AIRString, AIRString>::iterator itName = m_mapAnimationNames.find(szName);
	if (itName != m_mapAnimationNames.end())
	{
		CAIRSkeletonAnimation* pAnimation = new CAIRSkeletonAnimation(this);
		pAnimation->LoadFromFile(itName->second.c_str(), ENGINE_INST->GetConfig()->m_bBackgroundThread);
		m_mapAnimation.insert(std::make_pair(szName, pAnimation));
		return pAnimation;
	}

	return NULL;
}
