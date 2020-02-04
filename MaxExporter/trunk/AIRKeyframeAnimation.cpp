#include "AIRKeyframeAnimation.h"
#include "AIRExporter.h"

CAIRKeyFrameData::CAIRKeyFrameData(int nFrame) : m_nFrameIndex(nFrame)
{

}

CAIRKeyFrameData::~CAIRKeyFrameData()
{

}

void CAIRKeyFrameData::Clear()
{
    for (size_t i = 0; i < m_vtMeshes.size(); ++i)
    {
        m_vtMeshes[i]->Clear();
        delete m_vtMeshes[i];
    }
    m_vtMeshes.clear();
}

void CAIRKeyFrameData::Create(TimeValue time, std::vector<CMaxNode*>& vtMaxNodes)
{
    for (size_t i = 0; i < vtMaxNodes.size(); ++i)
    {
        
        if (vtMaxNodes[i]->IsMesh())
        {
            CAIRMesh* pMesh = new CAIRMesh;

            if (pMesh->Create(vtMaxNodes[i], time, true))
            {
                m_vtMeshes.push_back(pMesh);
            }
            else
            {
                delete pMesh;
            }
        }
        
    }
}

bool CAIRKeyFrameData::SaveToFile(FILE* f)
{
    fprintf(f, "    frame %d {\n", m_nFrameIndex);
    for (size_t i = 0; i < m_vtMeshes.size(); ++i)
    {
        m_vtMeshes[i]->SavePositionToFile(f);
    }
    fprintf(f, "    }\n");
    return true;
}

bool CAIRKeyFrameData::SaveToFile(std::fstream& fout)
{
    //fout<<(int)m_vtMeshes.size();
    size_t nMeshesNum = m_vtMeshes.size();
    fout.write((const char*)&nMeshesNum, sizeof(size_t));
    for (size_t i = 0; i < m_vtMeshes.size(); ++i)
    {
        m_vtMeshes[i]->SavePositionToFile(fout);
    }
    return true;
}


CAIRKeyframeAnimation::CAIRKeyframeAnimation(void) : m_nFrames(0)
{
}

CAIRKeyframeAnimation::~CAIRKeyframeAnimation(void)
{
}

bool CAIRKeyframeAnimation::Create(std::vector<CMaxNode*>& vtMaxNodes)
{
    CMaxInterface* pInterface = AIREXPORTER->GetMaxInterface();
    Interval ARange =  pInterface->GetInterface()->GetAnimRange(); //获得动画的长度

    TimeValue   tAniTime =  ARange.End() - ARange.Start();

    TimeValue   tTime = ARange.Start();

    m_nFrames = AIREXPORTER->GetEndFrame() - AIREXPORTER->GetStartFrame();

    TimeValue interval = tAniTime / m_nFrames;

    //int nBoneCount = skeleton.GetBoneNum();

    //for (int i = 0; i < nBoneCount; i++)
    //{
    //    CAIRAnimBone* pBone = new CAIRAnimBone(i);
    //    m_vtAnimBones.push_back(pBone);
    //}

    float fTime = tTime;

    for (int nFrame = 0; nFrame < m_nFrames; nFrame++)
    {
        pInterface->SetProgressInfo(/*100.0f * (float)nFrame / (float)m_nFrames*/tTime / tAniTime * 100);
        pInterface->SetTime(tTime);

        CAIRKeyFrameData* pKeyframeData = new CAIRKeyFrameData(nFrame);
        pKeyframeData->Create(tTime, vtMaxNodes);
        m_vtKeyFrameData.push_back(pKeyframeData);
        /*
        for (int i = 0; i < skeleton.GetBoneNum(); i++)
        {
            CAIRAnimBone* pAniBone = m_vtAnimBones[i];


            FRAMEDATA keyFrame;
            Point3 position;
            Quat   rotation;
            skeleton.GetTranslationAndRotation(i, fTime, position, rotation);
            skeleton.GetAbsoluteTranslationAndRotation(i, fTime, keyFrame.vAbsolutePos, keyFrame.qAbsoluteRotation);
            keyFrame.posX = position.x;
            keyFrame.posY = position.y;
            keyFrame.posZ = position.z;
            keyFrame.orientX = rotation.x;
            keyFrame.orientY = rotation.y;
            keyFrame.orientZ = rotation.z;
            keyFrame.orientW = rotation.w;
            pAniBone->AddFrameData(keyFrame);	

        }
        */

        tTime += interval;
        //fTime += 1.0f / GetFrameRate();
    }
    return true;
}

void CAIRKeyframeAnimation::Clear()
{
    for (size_t i = 0; i < m_vtKeyFrameData.size(); ++i)
    {
        m_vtKeyFrameData[i]->Clear();
        delete m_vtKeyFrameData[i];
    }
    m_vtKeyFrameData.clear();
}

bool CAIRKeyframeAnimation::SaveToFile(const char* szFilename)
{
    FILE* f = fopen(szFilename, "wt");

    if (f == NULL)
    {
        //ShowErrorMsg("文件打开失败！", __FILE__, __LINE__);
        return false;
    }

    fprintf(f, "AIRVersion %s\n", AIREXPORTER->GetVersion().c_str());

    fprintf(f, "animation %s {\n", m_strName.c_str());
    fprintf(f, "    frames %d\n", m_nFrames);
    fprintf(f, "    frameRate %d\n", GetFrameRate());
    //m_strCoord = STR_COORDINATE_LEFT;
    //
    //fprintf(f, "Coordinate %s\n", m_strCoord.c_str());

    //fprintf(f, "UpVec %s\n", m_strUpVec.c_str());
    int nMeshesCount = 0;
    for (size_t i = 0; i < m_vtKeyFrameData.size(); i++)
    {
        m_vtKeyFrameData[i]->SaveToFile(f);
    }

    fprintf(f, "}\n\n");

    fclose(f);
    return true;
}

bool CAIRKeyframeAnimation::SaveToFileBinary(const char* szFilename)
{
    std::fstream fout;
    fout.open(szFilename, std::ios::out | std::ios::binary );
    float fVersion = FILE_VERSION;
    fout.write((const char*)&fVersion, sizeof(float));
    fout.write(m_strName.c_str(), 256);
    fout.write((const char*)&m_nFrames, sizeof(int));

    float fFrameRate = GetFrameRate();
    fout.write((const char*)&fFrameRate, sizeof(float));
    //fout<<(float)0.1f;
    //fout<<(int)m_strName.length();
    //fout<<m_strName.c_str();
    //fout<<m_nFrames;
    //fout<<GetFrameRate();
    for (size_t i = 0; i < m_vtKeyFrameData.size(); i++)
    {
        m_vtKeyFrameData[i]->SaveToFile(fout);
    }
    return true;
}

void CAIRKeyframeAnimation::SetName(const char* szName)
{
    m_strName = szName;
}
