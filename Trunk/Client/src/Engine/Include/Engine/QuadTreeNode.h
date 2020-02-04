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
    // Qualifier: 把渲染体绑定到结点中
    // Parameter: CTerrainBlock * pObject 渲染物，这里暂定为地形块
    //************************************
    //void Attach(CTerrainBlock* pObject);

    //************************************
    // Method:    IsInFrustum
    // FullName:  CQuadTreeNode::IsInFrustum
    // Access:    public 
    // Returns:   bool
    // Qualifier: 是否在可视区间内
    // Parameter: CFrustum * pFrustum  可视空间
    //************************************
    //bool IsInFrustum(const CFrustum* pFrustum);

    //CTerrainBlock* GetObject() { return m_pObject; }

    //CQuadTreeNode* FindAttachNode(CTerrainBlock* pObject);

    //************************************
    // Method:    IsInside
    // FullName:  CQuadTreeNode::IsInside
    // Access:    public 
    // Returns:   bool
    // Qualifier: 判断地形块是否在该结点范围内
    // Parameter: CTerrainBlock * pBlock
    //************************************
    //bool IsInside(CTerrainBlock* pBlock);

    //************************************
    // Method:    CanAttach
    // FullName:  CQuadTreeNode::CanAttach
    // Access:    public 
    // Returns:   bool
    // Qualifier: 判断地形块能否被绑定
    // Parameter: CTerrainBlock * pBlock
    //************************************
    //bool CanAttach(CTerrainBlock* pBlock);

    //void GetRenderObjects(std::list<CTerrainBlock*>& lstRenderQueue, CCamera* pCamera);

    //CQuadTreeNode* GetChild(int nIndex) { return m_pChilds[nIndex]; }
    //CQuadTreeNode* m_pChilds[4];
private:
    float  m_fSphereHalf;   //半径
    float  m_fWidth;        //方块宽度
    Vector3Df    m_CenterPos;     //中心点   

    int m_nCenterX; 
    int m_nCenterY;

    int m_nGridsPerRow;

    

    CTerrainBlock*  m_pObject;
};
