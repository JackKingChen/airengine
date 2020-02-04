#include "MaxNode.h"
#include <cs/bipexp.h>
#include <maxtypes.h>

int CMaxNode::s_nNewCount = 0;

CMaxNode::CMaxNode() : m_pNode(NULL), m_pParent(NULL)
{
    s_nNewCount++;
}

CMaxNode::~CMaxNode()
{
    Clear();
    s_nNewCount--;
}

bool CMaxNode::Create(CMaxNode* pParent, INode *pNode)
{
    if (pNode == NULL)
    {
        return false;
    }
    m_pNode = pNode;

    m_pParent = pParent;

    m_strName = m_pNode->GetName();

    return true;
}

void CMaxNode::Clear()
{
    for (size_t i = 0; i < m_vtChildNodes.size(); i++)
    {
        //CMaxNode* pChildNode = m_vtChildNodes[i];
        //pChildNode->Clear();
        //delete pChildNode;
        //pChildNode = NULL;
    }
}

CMaxNode* CMaxNode::GetChild(int childId)
{
    // check if the internal node is valid
    if(m_pNode == 0) 
        return 0;

    // check if the given node id is valid
    if((childId < 0) || (childId >= m_pNode->NumberOfChildren()))
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
        return false;
    }

    // create the max node
    if(!pNode->Create(this, m_pNode->GetChildNode(childId)))
    {
        delete pNode;
        return 0;
    }

    m_vtChildNodes.push_back(pNode);

    return pNode;
}

int CMaxNode::GetChildCount() const
{
    return m_pNode->NumberOfChildren();
}

std::string CMaxNode::GetName()
{
    // check if the internal node is valid
    if(m_pNode == 0) return "<void>";

    return m_pNode->GetName();
}

bool CMaxNode::IsMesh()
{
    if (m_pNode->IsRootNode())
    {
        return false;
    }

    // check for mesh node
    ObjectState os;
    os = m_pNode->EvalWorldState(0);
    if(os.obj->SuperClassID() == GEOMOBJECT_CLASS_ID) 
    {
        return true;
    }

    return false;
}

bool CMaxNode::IsBone()
{
    if (m_pNode->IsRootNode())
    {
        return false;
    }

    // check for bone node
    ObjectState os;
    os = m_pNode->EvalWorldState(0);
    if(os.obj->ClassID() == Class_ID(BONE_CLASS_ID, 0)) 
    {
        return true;
    }

#if MAX_RELEASE >= 4000
    if(os.obj->ClassID() == BONE_OBJ_CLASSID) 
    {
        return true;
    }
#endif

    if(os.obj->ClassID() == Class_ID(DUMMY_CLASS_ID, 0)) 
    {
        return false;
    }

    // check for biped node
    Control *pControl;
    pControl = m_pNode->GetTMController();
    if((pControl->ClassID() == BIPSLAVE_CONTROL_CLASS_ID) || (pControl->ClassID() == BIPBODY_CONTROL_CLASS_ID)) 
    {
        return true;
    }

    return false;
}

bool CMaxNode::IsDummy()
{
    // check for root node
    if(m_pNode->IsRootNode()) 
    {
        return false;
    }

    // check for dummy node
    ObjectState os;
    os = m_pNode->EvalWorldState(0);
    if(os.obj->ClassID() == Class_ID(DUMMY_CLASS_ID, 0)) 
    {
        return true;
    }

    return false;
}

Matrix3 CMaxNode::GetNodeTM(float fTime)
{
    Matrix3 tm;
    tm.IdentityMatrix();

    // only do this for valid nodes
    if(m_pNode != NULL)
    {
        // get the node transformation
        tm = m_pNode->GetNodeTM(SecToTicks(fTime));

        // make the transformation uniform
        tm.NoScale();
    }

    return tm;
}

Matrix3 CMaxNode::GetObjectTM(float fTime)
{
    Matrix3 tm;
    tm.IdentityMatrix();

    if (m_pNode != NULL)
    {
        // get the node transformation
        tm = m_pNode->GetObjectTM(SecToTicks(fTime));

        // make the transformation uniform
        tm.NoScale();
    }

    return tm;
}
