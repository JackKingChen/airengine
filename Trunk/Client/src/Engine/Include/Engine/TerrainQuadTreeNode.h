/********************************************************************
	created:	2009/12/08
	created:	8:12:2009   11:39
	filename: 	d:\My Codes\client\Current\Engine\Engine\Include\TerrainQuadTreeNode.h
	file path:	d:\My Codes\client\Current\Engine\Engine\Include
	file base:	TerrainQuadTreeNode
	file ext:	h
	author:		liangairan
	
	purpose:	地形的四叉树结点
*********************************************************************/
#pragma once

#include "SceneNode.h"
#include "Camera.h"
#include "TerrainBlock.h"
class CHeightMap;
class CTerrainQuadTree;

class EngineExport CTerrainQuadTreeNode : public CSceneNode
{
public:
    CTerrainQuadTreeNode(float fWidth, int nCenterX, int nCenterY, int nGridsPerRow, CHeightMap* pHeightMap);
    virtual ~CTerrainQuadTreeNode();

    //************************************
    // Method:    Attach
    // FullName:  CQuadTreeNode::Attach
    // Access:    public 
    // Returns:   void
    // Qualifier: 把渲染体绑定到结点中
    // Parameter: CTerrainBlock * pObject 渲染物，这里暂定为地形块
    //************************************
    void Attach(CTerrainBlock* pObject);

    //************************************
    // Method:    IsInFrustum
    // FullName:  CQuadTreeNode::IsInFrustum
    // Access:    public 
    // Returns:   bool
    // Qualifier: 是否在可视区间内
    // Parameter: CFrustum * pFrustum  可视空间
    //************************************
    virtual bool IsVisable(const CFrustum* pFrustum);

    CTerrainBlock* GetObject() { return m_pObject; }

    CTerrainQuadTreeNode* FindAttachNode(CTerrainBlock* pObject);

    //************************************
    // Method:    IsInside
    // FullName:  CQuadTreeNode::IsInside
    // Access:    public 
    // Returns:   bool
    // Qualifier: 判断地形块是否在该结点范围内
    // Parameter: CTerrainBlock * pBlock
    //************************************
    bool IsInside(CTerrainBlock* pBlock);

    //************************************
    // Method:    CanAttach
    // FullName:  CQuadTreeNode::CanAttach
    // Access:    public 
    // Returns:   bool
    // Qualifier: 判断地形块能否被绑定
    // Parameter: CTerrainBlock * pBlock
    //************************************
    bool CanAttach(CTerrainBlock* pBlock);

    void GetRenderObjects(AIRList<CTerrainBlock*>& lstRenderQueue, CCamera* pCamera);

    CTerrainQuadTree* GetQuadTree()
    {
        return m_pQuadTree;
    }

    void SetQuadTree(CTerrainQuadTree* pTree);
    //virtual void GetVisableObjects(CCamera* pCamera)

    CTerrainQuadTreeNode* m_pChildren[4];

    void GetVisableNodes(CCamera* pCamera, AIRList<CSceneNode*>& lstVisableNodes);

    virtual void FillRenderContext(CCamera* pCamera, RenderContextQueue* pQueue, eRenderFlow flow);

protected:
    CTerrainQuadTree*   m_pQuadTree;
private:
    float  m_fSphereHalf;   //半径
    float  m_fWidth;        //方块宽度
    Vector3Df    m_CenterPos;     //中心点   

    int m_nCenterX; 
    int m_nCenterY;

    int m_nGridsPerRow;

    CTerrainBlock*  m_pObject;
};
