#include "AIRAnimation.h"
#include "AIRExporter2012.h"
#include "MaxMesh.h"
#include <fstream>

CAIRAnimation::CAIRAnimation(void)
{
}

CAIRAnimation::~CAIRAnimation(void)
{
    Clear();
}

bool CAIRAnimation::Create(CAIRSkeleton& skeleton)
{
	Clear();
    m_strName = AIREXPORTER->GetAnimationName();
    CMaxInterface* pInterface = AIREXPORTER->GetMaxInterface();
    Interval ARange =  pInterface->GetInterface()->GetAnimRange(); //获得动画的长度

    TimeValue   tAniTime =  ARange.End() - ARange.Start();

    TimeValue   tTime = ARange.Start();

    m_nFrames = AIREXPORTER->GetEndFrame() - AIREXPORTER->GetStartFrame();

    int nBoneCount = skeleton.GetBoneNum();

	for (int i = 0; i < nBoneCount; i++)
	{
		CAIRAnimBone* pBone = new CAIRAnimBone(i);
		m_vtAnimBones.push_back(pBone);
	}

	float fTime = tTime;

	for (int nFrame = 0; nFrame < m_nFrames; nFrame++)
	{
		pInterface->SetProgressInfo(100.0f * (float)nFrame / (float)m_nFrames);
		for (size_t i = 0; i < skeleton.GetBoneNum(); i++)
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

	    fTime += 1.0f / GetFrameRate();
	}
    
    

    return true;
}

bool CAIRAnimation::SaveToFile(const char* szFilename)
{
    FILE* f = fopen(szFilename, "at");
    AppendToFile(f);
    /*

    fprintf(f, "AIRVersion %s\n\n", AIREXPORTER->GetVersion().c_str());

    fprintf(f, "numFrames %d\n", m_nFrames);

    fprintf(f, "numBones %d\n", m_vtAnimBones.size());

    fprintf(f, "frameRate %d\n", GetFrameRate());

    for (int i = 0; i < m_nFrames; i++)
    {
        
        fprintf(f, "frame %d {\n", i);

		
        
        for (int j = 0; j < m_vtAnimBones.size(); j++)
        {
            CAIRAnimBone* pAniBone = m_vtAnimBones[j];
			FRAMEDATA frame = pAniBone->GetFrameData(i);
            fprintf(f, "    bone %d (%f %f %f) (%f %f %f %f)\n", j, frame.posX, frame.posY, frame.posZ,
                frame.orientX, frame.orientY, frame.orientZ, frame.orientW);
        }

        fprintf(f, "}\n\n");
    }
    */
    fclose(f);
    
    
    return true;
}

bool CAIRAnimation::SaveToFileBinary(const char* szFilename)
{
    std::fstream fout;
    fout.open(szFilename, std::ios::out | std::ios::binary/* | std::ios::app*/);

    //fout<<m_strName.length();
    fout.write(m_strName.c_str(), 256);
    //fout<<m_nFrames;
    //fout<<GetFrameRate();
    float fFrameRate = GetFrameRate();
    fout.write((const char*)&m_nFrames, sizeof(int));
    fout.write((const char*)&fFrameRate, sizeof(float));

	unsigned int nBonesNum = m_vtAnimBones.size();
	fout.write((const char*)&nBonesNum, sizeof(nBonesNum));
	/*
    for (int i = 0; i < m_nFrames; i++)
    {
        for (int j = 0; j < m_vtAnimBones.size(); j++)
        {
            CAIRAnimBone* pAniBone = m_vtAnimBones[j];
            FRAMEDATA frame = pAniBone->GetFrameData(i);
            //fout<<frame.posX;
            //fout<<frame.posY;
            //fout<<frame.posZ;
            //fout<<frame.orientX;
            //fout<<frame.orientY;
            //fout<<frame.orientZ;
            //fout<<frame.orientW;
            fout.write((const char*)&frame.posX, sizeof(float));
            fout.write((const char*)&frame.posY, sizeof(float));
            fout.write((const char*)&frame.posZ, sizeof(float));
            fout.write((const char*)&frame.orientX, sizeof(float));
            fout.write((const char*)&frame.orientY, sizeof(float));
            fout.write((const char*)&frame.orientZ, sizeof(float));
            fout.write((const char*)&frame.orientW, sizeof(float));            
            //fprintf(fp, "        bone %d (%f %f %f) (%f %f %f %f)\n", j, frame.posX, frame.posY, frame.posZ,
            //    frame.orientX, frame.orientY, frame.orientZ, frame.orientW);
            //frame.vAbsolutePos.x, frame.vAbsolutePos.y, frame.vAbsolutePos.z,
            //frame.qAbsoluteRotation.x, frame.qAbsoluteRotation.y, frame.qAbsoluteRotation.z, frame.qAbsoluteRotation.w);
            //fprintf(fp, "        absolute (%f %f %f) (%f %f %f %f)\n", frame.vAbsolutePos.x, frame.vAbsolutePos.y, frame.vAbsolutePos.z,
            //    frame.qAbsoluteRotation.x, frame.qAbsoluteRotation.y, frame.qAbsoluteRotation.z, frame.qAbsoluteRotation.w);
        }

        //fprintf(fp, "    }\n\n");
    }
	*/
	
	for (size_t i = 0; i < m_vtAnimBones.size(); ++i)
	{
		CAIRAnimBone* pAniBone = m_vtAnimBones[i];
		for (int j = 0; j < m_nFrames; j++)
		{
			
			FRAMEDATA frame = pAniBone->GetFrameData(j);
			//fout<<frame.posX;
			//fout<<frame.posY;
			//fout<<frame.posZ;
			//fout<<frame.orientX;
			//fout<<frame.orientY;
			//fout<<frame.orientZ;
			//fout<<frame.orientW;
			fout.write((const char*)&frame.posX, sizeof(float));
			fout.write((const char*)&frame.posY, sizeof(float));
			fout.write((const char*)&frame.posZ, sizeof(float));
			fout.write((const char*)&frame.orientX, sizeof(float));
			fout.write((const char*)&frame.orientY, sizeof(float));
			fout.write((const char*)&frame.orientZ, sizeof(float));
			fout.write((const char*)&frame.orientW, sizeof(float)); 
			//fout.write((const char*)&frame.vScale.x, sizeof(float));
			//fout.write((const char*)&frame.vScale.y, sizeof(float));
			//fout.write((const char*)&frame.vScale.z, sizeof(float));
		}
	}
	

    fout.close();
    return true;
}

void CAIRAnimation::Clear()
{
    for (size_t i = 0; i < m_vtAnimBones.size(); i++)
    {
        CAIRAnimBone* pAniBone = m_vtAnimBones[i];
        delete pAniBone;
    }

    m_vtAnimBones.clear();
}

bool CAIRAnimation::AppendToFile(FILE* fp)
{
    fprintf(fp, "\n");
    fprintf(fp, "animation %s {\n", m_strName.c_str());
    
    fprintf(fp, "    numFrames %d\n", m_nFrames);

    //fprintf(f, "numBones %d\n", m_vtAnimBones.size());

    fprintf(fp, "    frameRate %d\n", GetFrameRate());

    for (int i = 0; i < m_nFrames; i++)
    {

        fprintf(fp, "    frame %d {\n", i);

        for (size_t j = 0; j < m_vtAnimBones.size(); j++)
        {
            CAIRAnimBone* pAniBone = m_vtAnimBones[j];
            FRAMEDATA frame = pAniBone->GetFrameData(i);
            fprintf(fp, "        bone %d (%f %f %f) (%f %f %f %f)\n", j, frame.posX, frame.posY, frame.posZ,
                frame.orientX, frame.orientY, frame.orientZ, frame.orientW);
                //frame.vAbsolutePos.x, frame.vAbsolutePos.y, frame.vAbsolutePos.z,
                //frame.qAbsoluteRotation.x, frame.qAbsoluteRotation.y, frame.qAbsoluteRotation.z, frame.qAbsoluteRotation.w);
            //fprintf(fp, "        absolute (%f %f %f) (%f %f %f %f)\n", frame.vAbsolutePos.x, frame.vAbsolutePos.y, frame.vAbsolutePos.z,
            //    frame.qAbsoluteRotation.x, frame.qAbsoluteRotation.y, frame.qAbsoluteRotation.z, frame.qAbsoluteRotation.w);
        }

        fprintf(fp, "    }\n\n");
    }

    fprintf(fp, "}\n");

    return true;
}

void CAIRAnimation::BuildBoundingBoxes(CMaxMesh* pNode, int nNodeIndex)
{
    Box3 boundingBox;
    INode* pMaxNode = pNode->GetINode();

    CMaxInterface* pInterface = AIREXPORTER->GetMaxInterface();
    Interval ARange =  pInterface->GetInterface()->GetAnimRange(); //获得动画的长度

    TimeValue   tAniTime =  ARange.End() - ARange.Start();

    TimeValue   tTime = ARange.Start();

    TimeValue   tTimeOffset = tAniTime / m_nFrames;

    m_nFrames = AIREXPORTER->GetEndFrame() - AIREXPORTER->GetStartFrame();

    float fTime = tTime;

    for (int nFrame = 0; nFrame < m_nFrames; nFrame++)
    {
        ObjectState os = pMaxNode->EvalWorldState(tTime);
        Object *obj = os.obj;
        if (!obj)
        {
            return ;
        }
        if (obj && obj->CanConvertToType(Class_ID(TRIOBJ_CLASS_ID, 0)))
        {
            TriObject *tri = NULL;
            std::string str = pMaxNode->GetName();
            tri = (TriObject *)obj->ConvertToType(0, Class_ID(TRIOBJ_CLASS_ID, 0));
            if(tri)
            {
                Mesh *pkMesh = &(tri->GetMesh());
                assert(pkMesh);
                Matrix3 tmObj = pMaxNode->GetObjectTM(tTime);
                Box3 boundingBox = pkMesh->getBoundingBox(&tmObj);

                Point3 pMax = boundingBox.pmax;
                Point3 pMin = boundingBox.pmin;
                

                if (AIREXPORTER->IsLeftHand() && AIREXPORTER->IsYUp())
                {
                    float t = pMin.y;
                    pMin.y = pMin.z;
                    pMin.z = t;

                    t = pMax.y;
                    pMax.y = pMax.z;
                    pMax.z = t;

                    tmObj = AIREXPORTER->GetMaxInterface()->ConvertToDXMatrix(tmObj);
                    
                }
                else if (AIREXPORTER->IsYUp())
                {
                    float t = pMin.y;
                    pMin.y = pMin.z;
                    pMin.z = t;
                    pMin.x = -pMin.x;

                    t = pMax.y;
                    pMax.y = pMax.z;
                    pMax.z = t;
                    pMax.x = -pMax.x;

                    tmObj = AIREXPORTER->GetMaxInterface()->ConvertToYUpMatrix(tmObj);

                    
                }

                //pMin = pMin * tmObj;
                //pMax = pMax * tmObj;

                AIRBOUNDINGBOX airBoundingBox;
                airBoundingBox.maxX = max(pMax.x, pMin.x);
                airBoundingBox.maxY = max(pMax.y, pMin.y);
                airBoundingBox.maxZ = max(pMax.z, pMin.z);
                airBoundingBox.minX = min(pMin.x, pMax.x);
                airBoundingBox.minY = min(pMin.y, pMax.y);
                airBoundingBox.minZ = min(pMin.z, pMax.z);

                if (m_vtBoundingBoxes.size() <= nNodeIndex)
                {
                    BOUNDINGBOX_VECTOR vecBoxes;
                    m_vtBoundingBoxes.push_back(vecBoxes);
                }
                
                m_vtBoundingBoxes[nNodeIndex].push_back(airBoundingBox);
            }
        }

        tTime += tTimeOffset;
    }
    

}
