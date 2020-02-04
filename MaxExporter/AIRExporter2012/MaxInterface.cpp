#include "MaxInterface.h"
#include "MaxNode.h"
#include "decomp.h"

DWORD WINAPI ProgressFunction(LPVOID arg)
{
	return 0;
}

CMaxInterface::CMaxInterface() : m_pExpInterface(NULL), m_pInterface(NULL)
{

}

CMaxInterface::~CMaxInterface()
{
    
}

bool CMaxInterface::Create(ExpInterface *pExpInterface, Interface *pInterface)
{
    m_pExpInterface = pExpInterface;

    m_pInterface = pInterface;

    return true;
}

CMaxNode* CMaxInterface::GetNode(const std::string& strName)
{
    CMaxNode *pNode;
    pNode = new CMaxNode();
    if(pNode == 0)
    {
        //theExporter.SetLastError("Memory allocation failed.", __FILE__, __LINE__);
        return 0;
    }

    // create the max node
    if(!pNode->Create(NULL, m_pInterface->GetINodeByName(strName.c_str())))
    {
        delete pNode;
        return 0;
    }

    return pNode;
}

CMaxNode* CMaxInterface::GetSelectedNode(int nodeId)
{
    // get the number of selected nodes
    int nodeCount;
    nodeCount = m_pInterface->GetSelNodeCount();

    // if nothing is selected, we go with the scene root node
    if(nodeCount == 0)
    {
        // check if the given node id is valid
        if(nodeId == 0)
        {
            // allocate a new max node instance
            CMaxNode *pNode;
            pNode = new CMaxNode();
            if(pNode == 0)
            {
                //theExporter.SetLastError("Memory allocation failed.", __FILE__, __LINE__);
                return 0;
            }

            // create the max node
            if(!pNode->Create(NULL, m_pInterface->GetRootNode()))
            {
                delete pNode;
                return 0;
            }

            return pNode;
        }

        // invalid node id requested!
        return 0;
    }

    // check if the given node id is valid
    if((nodeId < 0) || (nodeId >= m_pInterface->GetSelNodeCount()))
    {
        //theExporter.SetLastError("Invalid handle.", __FILE__, __LINE__);
        return 0;
    }

    // allocate a new max node instance
    CMaxNode *pNode;
    pNode = new CMaxNode();
    if(pNode == 0)
    {
        //theExporter.SetLastError("Memory allocation failed.", __FILE__, __LINE__);
        return 0;
    }

    // create the max node
    if(!pNode->Create(NULL, m_pInterface->GetSelNode(nodeId)))
    {
        delete pNode;
        return 0;
    }

    return pNode;
}

int CMaxInterface::GetTime()
{
    return m_pInterface->GetTime();
}

CMaxMaterial* CMaxInterface::GetMaterial(int nMaterialID)
{
    return NULL;
}

int CMaxInterface::GetMaterialCount()
{
    int nMaterialCount = 0;

    return nMaterialCount;
}

void CMaxInterface::SetProgressInfo(int percentage)
{
    m_pInterface->ProgressUpdate(percentage);
}

void CMaxInterface::SetTime(int nTime)
{
    m_pInterface->SetTime(nTime);
}

void CMaxInterface::StartProgressInfo(char* szText)
{
	m_pInterface->ProgressStart(szText, TRUE, ProgressFunction, NULL);
}

void CMaxInterface::StopProgressInfo()
{
	m_pInterface->ProgressEnd();
}

Matrix3 CMaxInterface::ConvertToDXMatrix(const Matrix3& mat)
{
    AffineParts localAff;
    Matrix3       newMat;
    decomp_affine(mat, &localAff);

    Point3 position = Point3(localAff.t.x, localAff.t.z, localAff.t.y);
    //ScaleValue(parts.k*parts.f, parts.u)
    Point3 scale = Point3(localAff.k.x, localAff.k.z, localAff.k.y);
    //localAff.u.
    //scale *= localAff.u; //Point3(localAff.k.x, localAff.k.z, localAff.k.y);
    ApplyScaling(newMat, ScaleValue(localAff.k * localAff.f, localAff.u));

    Quat   rotation(localAff.q.x, localAff.q.z, localAff.q.y, -localAff.q.w);    //原来是-w，现在去掉，看看效果

    Quat   sRotation(localAff.u.x, localAff.u.z, localAff.u.y, -localAff.u.w);

    bool bMirror = IsMatrixMirrored(mat);

    rotation.MakeMatrix(newMat);

    newMat.SetRow(3, position);
    newMat.Scale(scale);
    //newMat.SetTranslate(position);

    Matrix3 srtm, rtm, ptm, stm, ftm;

    ftm = ScaleMatrix(Point3(localAff.f, localAff.f, localAff.f));

    ptm.IdentityMatrix();

    ptm.SetTrans(position); 

    rotation.MakeMatrix(rtm);

    sRotation.MakeMatrix(srtm);

    stm = ScaleMatrix(scale);

    newMat = Inverse(srtm) * stm * srtm * rtm * ftm * ptm;

    return newMat;
}

Matrix3 CMaxInterface::ConvertToYUpMatrix(const Matrix3& mat)
{
    AffineParts localAff;
    Matrix3       newMat;
    decomp_affine(mat, &localAff);

    Point3 position = Point3(-localAff.t.x, localAff.t.z, localAff.t.y);
    //ScaleValue(parts.k*parts.f, parts.u)
    Point3 scale = Point3(localAff.k.x, localAff.k.z, localAff.k.y);
    //localAff.u.
    //scale *= localAff.u; //Point3(localAff.k.x, localAff.k.z, localAff.k.y);
    ApplyScaling(newMat, ScaleValue(localAff.k * localAff.f, localAff.u));

    Quat   rotation(-localAff.q.x, localAff.q.z, localAff.q.y, localAff.q.w);    //原来是-w，现在去掉，看看效果

    Quat   sRotation(-localAff.u.x, localAff.u.z, localAff.u.y, localAff.u.w);

    bool bMirror = IsMatrixMirrored(mat);

    rotation.MakeMatrix(newMat);

    newMat.SetRow(3, position);
    newMat.Scale(scale);
    //newMat.SetTranslate(position);

    Matrix3 srtm, rtm, ptm, stm, ftm;

    ftm = ScaleMatrix(Point3(localAff.f, localAff.f, localAff.f));

    ptm.IdentityMatrix();

    ptm.SetTrans(position); 

    rotation.MakeMatrix(rtm);

    sRotation.MakeMatrix(srtm);

    stm = ScaleMatrix(scale);

    newMat = Inverse(srtm) * stm * srtm * rtm * ftm * ptm;

    return newMat;
}

Matrix3 CMaxInterface::ConvertToOtherHandMatrix(const Matrix3& mat)
{
	Matrix3 newMat = mat;
	Point3 p(mat.GetRow(0).x, mat.GetRow(0).z, mat.GetRow(0).y);
	newMat.SetRow(0, p);
	p.Set(mat.GetRow(2).x, mat.GetRow(2).z, mat.GetRow(2).y);
	newMat.SetRow(1, p);

	p.Set(mat.GetRow(1).x, mat.GetRow(1).z, mat.GetRow(1).y);
	newMat.SetRow(2, p);

	p.Set(mat.GetRow(3).x, mat.GetRow(3).z, mat.GetRow(3).y);
	newMat.SetRow(3, p);
	//newMat(0, 1) = mat(0, 2);
	//newMat(0, 2) = mat(0, 1);

	//newMat(1, 0) = mat(2, 0);
	//newMat(1, 1) = mat(2, 2);
	//newMat(1, 2) = mat(2, 1);

	//newMat(2, 0) = mat(1, 0);
	//newMat(2, 1) = mat(1, 2);
	//newMat(2, 2) = mat(1, 1);

	//newMat(3, 1) = mat(3, 2);
	//newMat(3, 2) = mat(3, 1);

	return newMat;
}

void CMaxInterface::Clear()
{
    m_pInterface->ReleaseInterface();
}

bool CMaxInterface::IsMatrixMirrored(const Matrix3& tm)
{
    Point3	r1, r2, r3, r12;
    float					Mirror;
    r1 = tm.GetRow(0);
    r2 = tm.GetRow(1);
    r3 = tm.GetRow(2);
    r12 = CrossProd(r1, r2);//法向量计算
    Mirror = DotProd(r12, r3);//角度计算
    return Mirror < 0;
}
