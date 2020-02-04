/********************************************************************
	created:	2010/05/26
	created:	26:5:2010   14:42
	filename: 	Obstruct.h
	author:		liangairan
	
	purpose:	�谭���࣬�������ó������赲
*********************************************************************/

#pragma once 

#include "EngineConfig.h"
#include "Triangle3D.h"
#include <vector>

using namespace AIR;

struct ObstructFace 
{
    Vector3Df topleft;
    Vector3Df topright;
    Vector3Df bottomright;
    Vector3Df bottomleft;
};

class CSceneNode;

class EngineExport Obstruct
{
public:
    Obstruct();
    virtual ~Obstruct();

    //�Ƿ��ཻ
    bool IsIntersert(const Vector3Df& start, const Vector3Df& lineDirection, Vector3Df& positionOut);

    bool CanWalkOn() const
    {
        return m_bCanWalk;
    }

    //�����Ƿ������
    void SetCanWalkOn(bool bCanWalk);

    void AddObstructFace(const ObstructFace& face);

    void RemoveObstructFace(size_t index);

    void SetNode(CSceneNode* pNode)
    {
        m_pNode = pNode;
    }

    size_t GetFacesNum() const
    {
        return m_vtObstructFace.size();
    }

    ObstructFace* GetObstructFace(size_t index)
    {
        if (index >= m_vtObstructFace.size())  
        {
            return NULL;
        }
        return &m_vtObstructFace[index];
    }

    CSceneNode* GetNode() const
    {
        return m_pNode;
    }

    //����λ��
    //void Update(const Matrix4f& matWorld);
private:
    AIRVector<ObstructFace> m_vtObstructFace;

    bool   m_bCanWalk;
    CSceneNode*  m_pNode;
};
