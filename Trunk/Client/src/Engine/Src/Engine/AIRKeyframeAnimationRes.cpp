#include "stdafx.h"
#include "AIRKeyframeAnimationRes.h"
#include "FileSystem.h"
#include "AIRAnimationState.h"
#include "Engine.h"

AIRKeyframeAnimationRes::AIRKeyframeAnimationRes()
{

}

AIRKeyframeAnimationRes::~AIRKeyframeAnimationRes()
{

}

void AIRKeyframeAnimationRes::Clear()
{
    VertexAnimationListIter itEnd = m_mapAnimations.end();
    for (VertexAnimationListIter it = m_mapAnimations.begin(); it != itEnd; ++it)
    {
        it->second->Clear();
        SafeDelete(it->second);
    }

    m_mapAnimations.clear();
}

bool AIRKeyframeAnimationRes::CreateFromFile(const char* szFilename)
{
    CDataStream* pStream = FILESYSTEM->GetFileStream(szFilename);
    if (pStream == NULL)
    {
        return false;
    }

    //char buff[512];
    AIRString lineBuff;
    float fVersion = 0;
    int numBones = 0;
    char szTemp[512];

    //AIRBONE bone;

    //int nBoneIndex = 0;

    while (!pStream->Eof())
    {
        /* Read whole line */
        //fgets (buff, sizeof (buff), fp);
        lineBuff = pStream->GetLine();

        if (sscanf (lineBuff.c_str(), " AIRVersion %f", &fVersion) == 1)
        {
            if (fVersion != 0.1f)
            {

                SafeDelete(pStream);
                return false;
            }
        }
        else if (sscanf(/*buff*/lineBuff.c_str(), "animation %s {\n", szTemp) == 1)
        {
            VertexAnimation* pAnimation = new VertexAnimation;
            pAnimation->SetName(szTemp);
            pAnimation->LoadFromFileData(pStream);
            m_mapAnimations.insert(std::make_pair(szTemp, pAnimation));
        }

    }

    m_strName = szFilename;

    SafeDelete(pStream);
    return true;
}

bool AIRKeyframeAnimationRes::CreateFromBinaryFile(const char* szFilename)
{
    //SetFilename(szFilename);
    m_strName = szFilename;
    return LoadFromFile(szFilename, ENGINE_INST->GetConfig()->m_bBackgroundThread);
}

s32 AIRKeyframeAnimationRes::Release()
{
    if (DropRef() == 0)
    {
        Clear();
    }

    return GetRef();
}

void AIRKeyframeAnimationRes::CreateAnimationStates(AIRMap<AIRString, CAIRAnimationState*>& mapAnimations)
{
	VertexAnimationListIter it_end = m_mapAnimations.end();
    for (VertexAnimationListIter it = m_mapAnimations.begin(); it != it_end; ++it)
    {
        VertexAnimation* pAnimation = it->second;
        CAIRAnimationState* pAnimationState = new CAIRAnimationState((CAIRAnimation*)pAnimation);
        pAnimationState->SetTimeLength(pAnimation->GetTimeRange());
        mapAnimations.insert(std::make_pair(it->first, pAnimationState));
    }
}

bool AIRKeyframeAnimationRes::Cache()
{
    float fVersion = 0.0f;
    m_pStream->Read(&fVersion, sizeof(float));

    if (fVersion < ENGINE_INST->GetMeshFileVersion())
    {
        SafeDelete(m_pStream);
        return false;
    }


    while (!m_pStream->Eof())
    {
        VertexAnimation* pAnimation = new VertexAnimation;
        if (pAnimation->LoadFromFileDataBinary(m_pStream))
        {
            m_mapAnimations.insert(std::make_pair(pAnimation->GetName(), pAnimation));
        }
        else
        {
            SafeDelete(pAnimation);
        }
    }

    SafeDelete(m_pStream);
    m_bReady = true;
    return m_bReady;
}
