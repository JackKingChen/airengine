#include "AIRBone.h"
#include "MaxNode.h"
#include "MaxInterface.h"
#include "AIRExporter2012.h"

int CAIRBone::s_nNewCount = 0;

CAIRBone::CAIRBone(void) : m_nID(-1), m_nParentID(-1), m_pNode(NULL), m_bMirrored(false)
{
    s_nNewCount++;
}

CAIRBone::CAIRBone(int nID, int nParentID, const char* szName, const Point3& pos, const Quat& rotation) : m_nID(nID),
m_nParentID(nParentID), m_strName(szName), m_Position(pos), m_Quat(rotation)
, m_bMirrored(false)
{
	CMaxInterface* pInterface = AIREXPORTER->GetMaxInterface();

	m_pNode = AIREXPORTER->GetNode(m_strName.c_str());
	if (m_pNode == NULL)
	{
		m_pNode = pInterface->GetNode(m_strName);
	}

    Matrix3 tm = m_pNode->GetNodeTM(0);

    Point3	r1, r2, r3, r12;
    float					Mirror;
    r1 = tm.GetRow(0);
    r2 = tm.GetRow(1);
    r3 = tm.GetRow(2);
    r12 = CrossProd(r1, r2);//法向量计算
    Mirror = DotProd(r12, r3);//角度计算
    m_bMirrored = Mirror < 0;

    if (m_bMirrored)
    {
        char szTemp[256];
        sprintf(szTemp, "bone[%s] is a mirror bone!");
        MessageBox(NULL, szTemp, "提示", MB_OK);
    }

    s_nNewCount++;
}

CAIRBone::~CAIRBone(void)
{
    int a = 0;
    s_nNewCount--;
}

/*
 *  注意，这里是求得骨骼在相对世界空间中的位置，但不明白为什么要是inverse
 *
 *
 */
bool CAIRBone::Create(CMaxNode* pNode, int nParentId, int nID)
{
    if (pNode->IsBone() || pNode->IsDummy())
    {
        m_strName = pNode->GetName();
        m_nParentID = nParentId;

        m_nID = nID;

        //获得在max 空间中的位置
        Matrix3 tmNoInverse = pNode->GetNodeTM(0.0f); // * AIREXPORTER->GetFinalMatrix();

        //m_PosAbsolute = tmNoInverse.GetTrans();
        //m_QuatAbsolute = Quat(tmNoInverse);
        tmNoInverse.NoScale();

        Point3	r1, r2, r3, r12;
        float					Mirror;
        r1 = tmNoInverse.GetRow(0);
        r2 = tmNoInverse.GetRow(1);
        r3 = tmNoInverse.GetRow(2);
        r12 = CrossProd(r1, r2);//法向量计算
        Mirror = DotProd(r12, r3);//角度计算
        m_bMirrored = Mirror < 0;

        if (m_bMirrored)
        {
            char szTemp[256];
            sprintf(szTemp, "bone[%s] is a mirror bone!", m_strName.c_str());
            //MessageBox(NULL, szTemp, "提示", MB_OK);
        }

        Matrix3 tm = Inverse(tmNoInverse);

        r1 = tm.GetRow(0);
        r2 = tm.GetRow(1);
        r3 = tm.GetRow(2);
        r12 = CrossProd(r1, r2);//法向量计算
        Mirror = DotProd(r12, r3);//角度计算

        if (AIREXPORTER->IsLeftHand() && AIREXPORTER->IsYUp())
        {
            tm = AIREXPORTER->GetMaxInterface()->ConvertToDXMatrix(tm);
			//tmOtherHand = AIREXPORTER->GetMaxInterface()->ConvertToOtherHandMatrix(tmNoInverse);
        }
        else if (AIREXPORTER->IsYUp())
        {
            tm = AIREXPORTER->GetMaxInterface()->ConvertToYUpMatrix(tm);
            //tm = AIREXPORTER->GetMaxInterface()->ConvertToDXMatrix(tm);
        }

        m_Position = tm.GetTrans();
		m_Quat = Quat(tm);
        
		/*
		if (AIREXPORTER->IsLeftHand())
		{
			m_Quat = Quat(tm).Inverse();
		}
		else
		{
			m_Quat = Quat(tm);
		}
		*/
        
        if (m_bMirrored)
        {
            //m_Quat.w = -m_Quat.w;
            //m_Position.x = -m_Position.x;
        }

        m_pNode = pNode;

        GetTranformToParent(pNode);   

        return true;
    }

    return false;
}

void CAIRBone::GetTranformToParent(CMaxNode* pNode)
{
    INode* pINode = pNode->GetINode();
    if (m_nParentID == -1)
    {
        m_Pos2Parent = Point3(0, 0, 0); //m_PosAbsolute;
        m_Quat2Parent.Identity(); //= m_QuatAbsolute;
        return;
    }
    INode* pParentNode = pINode->GetParentNode();
    if (!pParentNode)
    {
        return;
    }
    Matrix3 matParent = pParentNode->GetNodeTM(0);
    Matrix3 tm = pINode->GetNodeTM(0);
    if (AIREXPORTER->IsLeftHand() && AIREXPORTER->IsYUp())
    {
        matParent = AIREXPORTER->GetMaxInterface()->ConvertToDXMatrix(matParent);
        tm = AIREXPORTER->GetMaxInterface()->ConvertToDXMatrix(tm);
    }
    else if (AIREXPORTER->IsYUp())
    {
        //matParent = AIREXPORTER->GetMaxInterface()->ConvertToDXMatrix(matParent);
        //tm = AIREXPORTER->GetMaxInterface()->ConvertToDXMatrix(tm);
        matParent = AIREXPORTER->GetMaxInterface()->ConvertToYUpMatrix(matParent);
        tm = AIREXPORTER->GetMaxInterface()->ConvertToYUpMatrix(tm);
    }
    matParent = Inverse(matParent);
    Matrix3 tm2Parent = tm * matParent;

    m_Pos2Parent = tm2Parent.GetTrans();
    m_Quat2Parent = Quat(tm2Parent);

    if (m_bMirrored)
    {
        //m_Quat2Parent.w = -m_Quat2Parent.w;
    }
}
