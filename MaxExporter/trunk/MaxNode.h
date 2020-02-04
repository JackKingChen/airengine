#pragma once

#include "Max.h"
#include <string>
#include <vector>
#include "MaxMesh.h"

class CMaxNode
{
public:
    enum Type
    {
        TYPE_OTHER = 0,
        TYPE_BONE,
        TYPE_DUMMY,
        TYPE_MESH
    };

    CMaxNode();
    virtual ~CMaxNode();

    void Clear();

    bool Create(CMaxNode* pParent, INode *pNode);
    CMaxNode* GetChild(int childId);
    int GetChildCount() const;
    INode *GetINode() { return m_pNode; }
    std::string GetName();
    Type GetType();

    //�Ƿ�Ϊmesh
    bool IsMesh();

    //�Ƿ�Ϊ����
    bool IsBone();

    //�Ƿ�Ϊ������
    bool IsDummy();


    Matrix3 GetNodeTM(float fTime);

    Matrix3 GetObjectTM(float fTime);

    static int s_nNewCount;
protected:
    void BuildMeshes();
    INode* m_pNode;
    std::string m_strName;
    //std::vector<CMaxMesh*>  m_vtMaxMeshes;
    std::vector<CMaxNode*> m_vtChildNodes;

    CMaxNode* m_pParent;   //�����
private:
};

