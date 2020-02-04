// ***************************************************************
//  OTSceneManager   version:  1.0   ·  date: 03/24/2009
//  -------------------------------------------------------------
//  引擎的默认场景，为八叉树场景
//  -------------------------------------------------------------
//  Copyright (C) 2009 - All Rights Reserved
// ***************************************************************
// 
// ***************************************************************

#pragma once 

#include "ISceneManager.h"
//#include "Renderable.h"
//#include "SkyBox.h"
//#include "Water.h"
//#include "HeightMap.h"
//#include "IFontBase.h"
//#include "IEffect.h"
#include "SunLight.h"
//#include "ShaderGroup.h"
//#include "Octree.h"

class CHeightMap;
class CSkyBox;
class Renderable;
class COctreeNode;
class COctree;

class EngineExport COTSceneManager : public ISceneManager
{
public:
    COTSceneManager();
    virtual ~COTSceneManager();

    virtual void InitScene();

    //virtual void UpdateScene();

    //virtual void RenderScene();

    virtual void ClearScene();

    virtual bool LoadFromXML(const char* szFilename);

    

    //virtual void SetCamera(CCamera* pCamera);

    //virtual CCamera* GetCamera() { return m_pCamera; }

    //添加光照
    virtual void AddLight(CLight* pLight);

    //获得场景的太阳光
    virtual CLight* GetSunLight();

    //virtual Renderable* GetSkybox() { return (Renderable*)m_pSkybox; }

    virtual bool ProcessInputEvent(const LPINPUTEVENT pEvent);

    virtual void AddSceneNode(CSceneNode* pNode);

    //更新八叉树节点
    virtual void UpdateOctreeNode(COctreeNode* pNode);

    //删除场景节点
    virtual void DestroySceneNode(CSceneNode* pNode);

    virtual void DestroySceneNode(const AIRString& strName);

    //设置八叉树的boundingbox
    void SetSceneBoundingBox(const AABBBox3Df& box);

    //获得场景中的地形
    virtual CHeightMap* GetHeightMap()
    {
        return m_pHeightMap;
    }

    //virtual int GetRenderableObjectsNum() const;

    //virtual void  GetVisableRenderables(CCamera* pCamera, CRenderQueue& renderQueue);

    //virtual void GetVisableRenderables(CCamera* pCamera, CRenderQueue& renderQueue, f32 fDistanceIn);

	virtual void GetCollisionRenderables(const AABBBox3Df& aabbTest, AIRList<Renderable*>& renderList);

	//是否开启shadowmap
    virtual void EnableShadowMap(bool bEnable);

    bool FindPickNAVTriangle(const Line3Df& ray, Vector3Df& vOut, NavTriangle& navTriOut);
    //加载好场景后对场景进行cache
    virtual void CacheScene();

    //把当前场景的可视物放到queue里面。
    //virtual void GetRenderQueue(CRenderQueue* pQueue);

    virtual void Cull(CCamera* pCamera, AIRList<CSceneNode*>& lstVisableNodes, u32 nCullFlag);
protected:
    

	virtual CSceneNode* CreateSceneNodeImp(const AIRString& strName, const Vector3Df& vPosition /* = Vector3Df::ZERO */, const Quaternion& qRotation /* = Quaternion */,
		const Vector3Df& vScale = Vector3Df::UNIT_SCALE);

    //virtual void Render(CCamera* pCamera);
    //void   LoadTerrainConfig(TERRAINCONFIG& config, const char* szFilename);


    //virtual void GetVisableObjects();

    //virtual void GetVisableObjects(f32 fDistanceIn);

    //处理可视范围内的结点
    //virtual void ProcessVisableObjects();

    virtual void LoadSceneNodeFromXML(CMarkupSTL& xml);

    virtual Renderable* LoadSceneRenderableFromXML(CMarkupSTL& xml, CSceneNode* pNode);

    //AIRString m_strName;

    COctree*  m_pOctree;  //八叉树
    //CCamera*  m_pCamera;  

    //CSkyBox*  m_pSkybox;

    //CWater    m_water;
    CHeightMap* m_pHeightMap;

private:
    //AIRList<Renderable*> m_lstRenderObject;
    //AIRList<Renderable*> m_lstAlphaObject;   //带alpha渲染状态的物体
    //typedef AIRList<Renderable*>::iterator RENDERABLE_ITER;

    //AIRList<CCamera*> m_lstCamera;
    //typedef AIRList<CCamera*>::iterator CAMERA_ITER;

    //场景中的光照
#if AIR_TARGET_PLATFORM == AIR_PLATFORM_WIN32
    AIRHashMap<u32, CLight*> m_mapLight;
#else
	AIRMap<u32, CLight*> m_mapLight;
#endif
    typedef std::map<u32, CLight*>::iterator LIGHT_ITER;
    u32 m_nLightCount;
    

    char m_szSceneMsg[256];
    bool   m_bRenderWater;

    
    //CColorValue       m_fogColor;

    
    //CLight m_ambientLight; //环境光，唯一

    CSunLight  m_Sun;      //太阳光

    AIRList<Renderable*>  m_lstPaparedRenderables;
    typedef AIRList<Renderable*>::iterator PrepareRenderableIter;
};
