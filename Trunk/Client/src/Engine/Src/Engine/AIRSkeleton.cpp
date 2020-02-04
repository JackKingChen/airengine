#include "stdafx.h"
#include "AIRSkeleton.h"
#include "Util.h"
#include "FileSystem.h"

CAIRSkeleton::CAIRSkeleton(CAIRSkeletonRes* pSkeletonRes) : m_nBoneNum(0), m_pSkeletonRes(pSkeletonRes)
{

}

CAIRSkeleton::CAIRSkeleton()
{

}

CAIRSkeleton::~CAIRSkeleton()
{
    Clear();
}

void CAIRSkeleton::Clear()
{
    for (int i = 0; i < (int)m_vtBones.size(); i++)
    {
        SafeDelete(m_vtBones[i]);
    }

    m_vtBones.clear();
}

void CAIRSkeleton::AddBone(CAIRBone* pBone)
{
    m_vtBones.push_back(pBone);
}

/*
s32 CAIRSkeleton::Release()
{
    if (DropRef() == 0)
    {
        Clear();
    }
    return GetRef();
}
*/

/*
bool CAIRSkeleton::CreateFromFile(const char* szFilename)
{
    AIRString strFilename = FILESYSTEM->GetFileAbsolutePath(szFilename);

    FILE* fp = fopen(strFilename.c_str(), "r");

    if (fp == NULL)
    {
        return false;
    }

    char buff[512];
    float fVersion = 0;
    int numMeshes = 0;
    char szTemp[512];

    AIRBONE bone;

    int nIndex = 0;

    while (!feof (fp))
    {
        // Read whole line 
        fgets (buff, sizeof (buff), fp);

        if (sscanf (buff, " AIRVersion %f", &fVersion) == 1)
        {
            if (fVersion != 0.1f)
            {
                // Bad version
                //fprintf (stderr, "Error: bad model version\n");
                fclose (fp);
                return false;
            }
        }
        else if (sscanf(buff, "numBones %s", szTemp) == 1)
        {
            m_nBoneNum = atoi(szTemp);
            m_vtBones = AURO_NEW_VERBOSE AIRBONE[m_nBoneNum];
        }
        else if (sscanf (buff, "\"%s\" %d { %f %f %f } { %f %f %f }\n", &szTemp, &bone.nParentID,
            &bone.vPosition.x, &bone.vPosition.y, &bone.vPosition.z, &bone.qRotation.x, &bone.qRotation.y, &bone.qRotation.z) == 8)
        {
            m_vtBones[nIndex] = bone;
            //bone.qRotation.
        }
        

    }

	fclose(fp);
    return true;
}
*/

/*
void CAIRSkeleton::Create(AIRVector<AIRBONE>& vtBones, int nBonesNum)
{
    if (nBonesNum < vtBones.size())
    {
        nBonesNum = vtBones.size();
    }

    m_nBoneNum = nBonesNum;
    if (m_vtBones != NULL)
    {
        SafeDeleteArray(m_vtBones);
        
    }

    if (m_nBoneNum > 0)
    {
        m_vtBones = AURO_NEW_VERBOSE AIRBONE[m_nBoneNum];
        for (int i = 0; i < vtBones.size(); i++)
        {
            m_vtBones[i] = vtBones[i];
        }
    }
    
}
*/

//void CAIRSkeleton::Create(int nBonesNum)
//{
//    m_nBoneNum = nBonesNum;
//    Clear();
//}

//整个骨骼系统的核心计算
/*
const Vector3Df& CAIRSkeleton::GetBoneTranslate(int nBoneID)
{
    //CAIRBone* pBone = m_vtBones[nBoneID];
    return m_vtBones[nBoneID]->GetTranslateBoneSpace();
}

const Quaternion& CAIRSkeleton::GetBoneRotation(int nBoneID)
{
	//CAIRBone* pBone = m_vtBones[nBoneID];
	return m_vtBones[nBoneID]->GetRotationBoneSpace();
}


void CAIRSkeleton::CaculateBones()
{
	u32 dwBoneCount = m_vtBones.size();
    for (int i = 0; i < dwBoneCount; i++)
    {
        m_vtBones[i]->CaculateTranslation();
    }
}

void CAIRSkeleton::CaculateRelatedLastFrame(CAIRSkeleton* pLastFrame)
{
	u32 dwBoneCount = m_vtBones.size();
    if (pLastFrame == NULL)
    {
        for (u32 i = 0; i < dwBoneCount; ++i)
        {
            m_vtBones[i]->CaculateRelatedToLastFrame(NULL);
        }
    }
    else
    {
        for (u32 i = 0; i < dwBoneCount; ++i)
        {
            m_vtBones[i]->CaculateRelatedToLastFrame(pLastFrame->GetBone(i));
        }
    }
}

CAIRBone* CAIRSkeleton::GetBoneByName(const char* szName)
{
    for (size_t i = 0; i < m_vtBones.size(); ++i)
    {
        if (strcmp(m_vtBones[i]->m_strName.c_str(), szName) == 0)
        {
            return m_vtBones[i];
        }
    }

    return NULL;
}
*/