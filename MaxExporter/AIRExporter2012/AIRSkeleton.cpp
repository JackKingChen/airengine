#include "AIRSkeleton.h"
#include "AIRExporter2012.h"
#include "MaxNode.h"
#include <fstream>

CAIRSkeleton::CAIRSkeleton(void)
{
}

CAIRSkeleton::~CAIRSkeleton(void)
{
    Clear();
}

bool CAIRSkeleton::BuildBones(CMaxNode* pNode, int nParentID)
{
    CMaxInterface* pInterface = AIREXPORTER->GetMaxInterface();

    if (pNode->IsBone() || pNode->IsDummy())
    {
        CAIRBone* pBone = new CAIRBone();
        if (pBone->Create(pNode, nParentID, (int)m_vtBones.size()))
        {
            m_vtBones.push_back(pBone);
        }

        nParentID = pBone->GetID();
    }
    else
    {
        
    }

    for (int i = 0; i < pNode->GetChildCount(); i++)
    {
        if (!BuildBones(pNode->GetChild(i), nParentID))
        {
            return false;
        }
    }

    return true;
}

void CAIRSkeleton::Clear()
{
    for (size_t i = 0; i < m_vtBones.size(); i++)
    {
        CAIRBone* pBone = m_vtBones[i];
        delete pBone;
        pBone = NULL;
    }

    m_vtBones.clear();
}

bool CAIRSkeleton::SaveToFile(const char* szFilename)
{
    if (m_vtBones.size() == 0)
    {
        return false;
    }
    FILE* f = fopen(szFilename, "wt");

    if (f == NULL)
    {
        AIREXPORTER->ShowErrorMsg("文件打开失败！", __FILE__, __LINE__);
        return false;
    }

    fprintf(f, "AIRVersion %s\n", AIREXPORTER->GetVersion().c_str());

    fprintf(f, "numBones %d\n", m_vtBones.size());

    for (size_t i = 0; i < m_vtBones.size(); i++)
    {
        CAIRBone* pBone = m_vtBones[i];
        Point3 position = pBone->GetPosition();
        Quat   quad = pBone->GetQuat();
        //add by liangairan at 2011-1-25
        if (AIREXPORTER->IsLeftHand())
        {
            quad = quad.Inverse();
        }
        else
        {
            quad = quad.Inverse();
        }
        //----end add--------
        Point3 pos2Parent = pBone->GetPos2Parent();
        Quat   rotate2Parent = pBone->GetRotation2Parent();
        std::string strBoneName = pBone->GetName();
        //把名字的空格全部替换成'_'
        for (size_t i = 0; i < strBoneName.length(); i++)
        {
            if (strBoneName[i] == ' ')
            {
                strBoneName[i] = '_';
            }
        }
        fprintf(f, "\"%s\" %d { %f %f %f } { %f %f %f %f }\n", 
            strBoneName.c_str(), 
            pBone->GetParentID(), 
            position.x, position.y, position.z, quad.x, quad.y, quad.z, quad.w);
            //pos2Parent.x, pos2Parent.y, pos2Parent.z,
            //rotate2Parent.x, rotate2Parent.y, rotate2Parent.z, rotate2Parent.w);
        //Point3 posAbsolute = pBone->GetAbsolutePosition();
        //Quat   rotationAbsolute = pBone->GetAbsoluteRotation();
        //fprintf(f, "absolute { %f %f %f } { %f %f %f %f }\n", posAbsolute.x, posAbsolute.y, posAbsolute.z,
        //    rotationAbsolute.x, rotationAbsolute.y, rotationAbsolute.z, rotationAbsolute.w);
    }

    //for (int i = 0; i < m_vtAnimation.size(); i++)
    //{
    //    CAIRAnimation* pAni = m_vtAnimation[i];
    //    pAni->AppendToFile(f);
    //}
    m_strFilename = szFilename;

    fclose(f);
    return true;
}

bool CAIRSkeleton::SaveToFileBinary(const char* szFilename)
{
    std::fstream fout;

    fout.open(szFilename, std::ios::out | std::ios::binary);

    //输出版本号
    //fout<<(float)0.1f;
    float fVersion = FILE_VERSION;
    fout.write((const char*)&fVersion, sizeof(float));

    //骨骼数量
    //fout<<(int)m_vtBones.size();
    int nBonesNum = m_vtBones.size();
    fout.write((const char*)&nBonesNum, sizeof(int));

    for (size_t i = 0; i < m_vtBones.size(); i++)
    {
        CAIRBone* pBone = m_vtBones[i];
        Point3 position = pBone->GetPosition();
        Quat   quad = pBone->GetQuat();
        //add by liangairan at 2011-1-25
        if (AIREXPORTER->IsLeftHand())
        {
            quad = quad.Inverse();
        }
        else
        {
            quad = quad.Inverse();
        }
        //----end add--------
        Point3 pos2Parent = pBone->GetPos2Parent();
        Quat   rotate2Parent = pBone->GetRotation2Parent();
        std::string strBoneName = pBone->GetName();
        //把名字的空格全部替换成'_'
        for (size_t i = 0; i < strBoneName.length(); i++)
        {
            if (strBoneName[i] == ' ')
            {
                strBoneName[i] = '_';
            }
        }

        //fout<<(int)strBoneName.length();
        fout.write(strBoneName.c_str(), 256);
        //fout<<pBone->GetParentID();
        //fout<<position.x;
        //fout<<position.y;
        //fout<<position.z;
        //fout<<quad.x;
        //fout<<quad.y;
        //fout<<quad.z;
        //fout<<quad.w;
        int nParentID = pBone->GetParentID();
        fout.write((const char*)&nParentID, sizeof(int));
        fout.write((const char*)&position.x, sizeof(float));
        fout.write((const char*)&position.y, sizeof(float));
        fout.write((const char*)&position.z, sizeof(float));
        fout.write((const char*)&quad.x, sizeof(float));
        fout.write((const char*)&quad.y, sizeof(float));
        fout.write((const char*)&quad.z, sizeof(float));
        fout.write((const char*)&quad.w, sizeof(float));
    }

    m_strFilename = szFilename;

    fout.close();
    return true;
}

bool CAIRSkeleton::SaveToFileBinary(std::ofstream& fout)
{
	int nBonesNum = m_vtBones.size();
	fout.write((const char*)&nBonesNum, sizeof(int));

	for (size_t i = 0; i < m_vtBones.size(); i++)
	{
		CAIRBone* pBone = m_vtBones[i];
		Point3 position = pBone->GetPosition();
		Quat   quad = pBone->GetQuat();
		//add by liangairan at 2011-1-25
		if (AIREXPORTER->IsLeftHand())
		{
			quad = quad.Inverse();
		}
		else
		{
			quad = quad.Inverse();
		}
		//----end add--------
		Point3 pos2Parent = pBone->GetPos2Parent();
		Quat   rotate2Parent = pBone->GetRotation2Parent();
		std::string strBoneName = pBone->GetName();
		//把名字的空格全部替换成'_'
		for (size_t i = 0; i < strBoneName.length(); i++)
		{
			if (strBoneName[i] == ' ')
			{
				strBoneName[i] = '_';
			}
		}

		//fout<<(int)strBoneName.length();
		fout.write(strBoneName.c_str(), 256);
		//fout<<pBone->GetParentID();
		//fout<<position.x;
		//fout<<position.y;
		//fout<<position.z;
		//fout<<quad.x;
		//fout<<quad.y;
		//fout<<quad.z;
		//fout<<quad.w;
		int nParentID = pBone->GetParentID();
		fout.write((const char*)&nParentID, sizeof(int));
		fout.write((const char*)&position.x, sizeof(float));
		fout.write((const char*)&position.y, sizeof(float));
		fout.write((const char*)&position.z, sizeof(float));
		fout.write((const char*)&quad.x, sizeof(float));
		fout.write((const char*)&quad.y, sizeof(float));
		fout.write((const char*)&quad.z, sizeof(float));
		fout.write((const char*)&quad.w, sizeof(float));
	}

	return true;
}

int CAIRSkeleton::GetBoneID(const char* szNodeName)
{
    for (size_t i = 0; i < m_vtBones.size(); i++)
    {
        CAIRBone* pBone = m_vtBones[i];
        if (strcmp(pBone->GetName().c_str(), szNodeName) == 0)
        {
            return pBone->GetID();
        }
    }

    return -1;
}

void CAIRSkeleton::GetTranslationAndRotation(int nBoneID, float fTime, Point3& Pos, Quat& Rotation)
{
	CAIRBone* pBone = m_vtBones[nBoneID];
    CMaxNode* pNode = pBone->GetNode();

	if (!pNode->IsBone() && !pNode->IsDummy())
	{
		AIREXPORTER->ShowErrorMsg("导出动画过程中，结点并不是骨骼！", __FILE__, __LINE__);
	}

	CMaxNode* pParentNode = NULL;
	if (pBone->GetParentID() != -1)
	{
		//获得父结点
		pParentNode = m_vtBones[pBone->GetParentID()]->GetNode();
		
	}

    GetTranslationAndRotation(pNode, pParentNode, fTime, Pos, Rotation);
}

void CAIRSkeleton::GetAbsoluteTranslationAndRotation(int nBoneID, float fTime, Point3& Pos, Quat& Rotation)
{
    CAIRBone* pBone = m_vtBones[nBoneID];
    CMaxNode* pNode = pBone->GetNode();
    INode* pINode = pNode->GetINode();
    Matrix3 tm = pINode->GetNodeTM(SecToTicks(fTime));

    //变换到Y轴向上的空间
    if (AIREXPORTER->IsLeftHand() && AIREXPORTER->IsYUp())
    {
        tm = AIREXPORTER->GetMaxInterface()->ConvertToDXMatrix(tm);
    }
    else if (AIREXPORTER->IsYUp())
    {
        tm = AIREXPORTER->GetMaxInterface()->ConvertToYUpMatrix(tm);
        //tm = AIREXPORTER->GetMaxInterface()->ConvertToDXMatrix(tm);
    }

    Pos = tm.GetTrans();
    Rotation = Quat(tm);
}

void CAIRSkeleton::GetTranslationAndRotation(CMaxNode* pNode, CMaxNode* pParentNode, float fTime, Point3& Pos, Quat& Rotation)
{
    //这里是求出相对父亲的矩阵，最终是由父亲矩阵X求出的矩阵=模型空间的坐标
	Matrix3 tm, tmParent;
	INode* pINode = pNode->GetINode();
	tm = pINode->GetNodeTM(SecToTicks(fTime)); // * AIREXPORTER->GetFinalMatrix(); 
	tm.NoScale();

    //变换到Y轴向上的空间
    if (AIREXPORTER->IsLeftHand() && AIREXPORTER->IsYUp())
    {
        tm = AIREXPORTER->GetMaxInterface()->ConvertToDXMatrix(tm);
    }
    else if (AIREXPORTER->IsYUp())
    {
        tm = AIREXPORTER->GetMaxInterface()->ConvertToYUpMatrix(tm);
        //tm = AIREXPORTER->GetMaxInterface()->ConvertToDXMatrix(tm);
    }
    
	if (pParentNode != NULL)
	{
        tmParent = pParentNode->GetINode()->GetNodeTM(SecToTicks(fTime));
        if (AIREXPORTER->IsLeftHand() && AIREXPORTER->IsYUp())
        {
            tmParent = AIREXPORTER->GetMaxInterface()->ConvertToDXMatrix(tmParent);
        }
        else if (AIREXPORTER->IsYUp())
        {
            tmParent = AIREXPORTER->GetMaxInterface()->ConvertToYUpMatrix(tmParent);
            //tmParent = AIREXPORTER->GetMaxInterface()->ConvertToDXMatrix(tmParent);
        }
		tmParent = Inverse(tmParent);   // * AIREXPORTER->GetFinalMatrix());
        tmParent.NoScale();
		tm = tm * tmParent;  //正确的时候是tm * tmParent，liangairan change at 2009-10-30
        //变换空间
        //tm = AIREXPORTER->GetFinalMatrix() * tm;
        //------
        
	}
    

	// calculate the translation component
	Pos = tm.GetTrans();

	// calculate the rotation component
    //Quat qWorld(tmWorldTransform);
	Rotation = Quat(tm);
    //if (pParentNode == NULL)
    //{
    //    Rotation = Rotation.Inverse();  
    //}
}

/*
void CAIRSkeleton::GetBoneSpaceTranslation(int nBoneID, float fTime, Point3& Pos, Quat& Rotation)
{
    Matrix3 tm;
    INode* pINode = pNode->GetINode();
    tm = pINode->GetNodeTM(SecToTicks(fTime)); 
    tm.NoScale();

    // calculate the translation component
    Pos = tm.GetTrans();

    // calculate the rotation component
    Rotation = Quat(tm);
}
*/

void CAIRSkeleton::AddAnimation(CAIRAnimation* pAnimation)
{
    for (size_t i = 0; i < m_vtAnimation.size(); i++)
    {
		CAIRAnimation* pAniExist = m_vtAnimation[i];
		if (pAnimation->GetName() == pAniExist->GetName())
		{
			m_vtAnimation[i] = pAnimation;
			return;
		}
    }

	m_vtAnimation.push_back(pAnimation);
}

bool CAIRSkeleton::LoadFromFile(const char* szFilename)
{
	FILE* fp = fopen(szFilename, "rt");

	if (fp == NULL)
	{
		return false;
	}

	char buff[512];
	float fVersion = 0;
	int numMeshes = 0;
	char szTemp[512];
	int nBoneNum = 0;

	//CAIRBone bone;

	int nIndex = 0;
	int nParentID = 0;
	Point3 vPosition;
	Quat   qRotation;

	while (!feof (fp))
	{
		/* Read whole line */
		fgets (buff, sizeof (buff), fp);

		if (sscanf (buff, " AIRVersion %f", &fVersion) == 1)
		{
			if (fVersion != FILE_VERSION)
			{
				/* Bad version */
				//fprintf (stderr, "Error: bad model version\n");
				fclose (fp);
				return false;
			}
		}
		else if (sscanf(buff, "numBones %d", &nBoneNum) == 1)
		{
			m_vtBones.resize(nBoneNum);
		}
		else if (sscanf (buff, "\"%s %d { %f %f %f } { %f %f %f }\n", &szTemp, &nParentID,
			&vPosition.x, &vPosition.y, &vPosition.z, &qRotation.x, &qRotation.y, &qRotation.z) == 8)
		{
			char* pEnd = strstr(szTemp, "\"");
			if (pEnd)
			{
				int nLen = strlen(szTemp);
				szTemp[nLen - 1] = 0;
			}
			//CAIRBone* pBone = new CAIRBone(nIndex, nParentID, szTemp, vPosition, qRotation);
   //         m_vtBones[nIndex++] = pBone;
			//bone.qRotation.
		}
	}

	fclose(fp);
    return true;
}

bool CAIRSkeleton::LoadFromBinaryFile(const char* szFilename)
{
    std::ifstream fin;
    fin.open(szFilename, std::ios::in | std::ios::binary);

    float fVersion;
    fin.read((char*)&fVersion, sizeof(float));

    int nBonesNum;
    fin.read((char*)&nBonesNum, sizeof(int));

    m_vtBones.resize(nBonesNum);

    fin.close();
    return true;
}

CAIRAnimation* CAIRSkeleton::GetAnimation(const char* szName)
{
	for (size_t i = 0; i < m_vtAnimation.size(); i++)
	{
		CAIRAnimation* pAniExist = m_vtAnimation[i];
		if (strcmp(szName, pAniExist->GetName().c_str()) == 0)
		{
			return pAniExist;
		}
	}

	return NULL;
}

void CAIRSkeleton::SaveAnimation(CAIRAnimation* pAnimation)
{
    if (GetAnimation(pAnimation->GetName().c_str()) == NULL)
    {
		m_vtAnimation.push_back(pAnimation);

		FILE* fp = fopen(m_strFilename.c_str(), "at");
        pAnimation->AppendToFile(fp);
		fclose(fp);
    }
}

