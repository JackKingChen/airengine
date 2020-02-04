#pragma once
//#include "Common.h"
#include "TerrainBlock.h"
#include "Frustum.h"
#include "SceneNode.h"
//#include "HeightMap.h"
class CHeightMap;

class EngineExport CQuadTreeNode : public CSceneNode
{
public:
    //CQuadTreeNode(float fWidth, int nCenterX, int nCenterY, int nGridsPerRow, int nVerticesPerRow, const Vector3Df& vCenter/*CHeightMap* pHeightMap*/);
    //CQuadTreeNode(float fWidth, float fCenterX, float fCenterY, int nGridsPerRow);
    CQuadTreeNode(void);
    CQuadTreeNode(ISceneManager* pCreator, const AIRString& strName, const Vector3Df& vPosition = Vector3Df::ZERO, const Quaternion& qRotation = Quaternion(0, 0, 0, 1), const Vector3Df& vScale = Vector3Df::UNIT_SCALE);
    ~CQuadTreeNode(void);

    //************************************
    // Method:    Attach
    // FullName:  CQuadTreeNode::Attach
    // Access:    public 
    // Returns:   void
    // Qualifier: ����Ⱦ��󶨵������
    // Parameter: CTerrainBlock * pObject ��Ⱦ������ݶ�Ϊ���ο�
    //************************************
    //void Attach(CTerrainBlock* pObject);

    //************************************
    // Method:    IsInFrustum
    // FullName:  CQuadTreeNode::IsInFrustum
    // Access:    public 
    // Returns:   bool
    // Qualifier: �Ƿ��ڿ���������
    // Parameter: CFrustum * pFrustum  ���ӿռ�
    //************************************
    //bool IsInFrustum(const CFrustum* pFrustum);

    //CTerrainBlock* GetObject() { return m_pObject; }

    //CQuadTreeNode* FindAttachNode(CTerrainBlock* pObject);

    //************************************
    // Method:    IsInside
    // FullName:  CQuadTreeNode::IsInside
    // Access:    public 
    // Returns:   bool
    // Qualifier: �жϵ��ο��Ƿ��ڸý�㷶Χ��
    // Parameter: CTerrainBlock * pBlock
    //************************************
    //bool IsInside(CTerrainBlock* pBlock);

    //************************************
    // Method:    CanAttach
    // FullName:  CQuadTreeNode::CanAttach
    // Access:    public 
    // Returns:   bool
    // Qualifier: �жϵ��ο��ܷ񱻰�
    // Parameter: CTerrainBlock * pBlock
    //************************************
    //bool CanAttach(CTerrainBlock* pBlock);

    //void GetRenderObjects(std::list<CTerrainBlock*>& lstRenderQueue, CCamera* pCamera);

    //CQuadTreeNode* GetChild(int nIndex) { return m_pChilds[nIndex]; }
    //CQuadTreeNode* m_pChilds[4];
private:
    float  m_fSphereHalf;   //�뾶
    float  m_fWidth;        //������
    Vector3Df    m_CenterPos;     //���ĵ�   

    int m_nCenterX; 
    int m_nCenterY;

    int m_nGridsPerRow;

    

    CTerrainBlock*  m_pObject;
};
