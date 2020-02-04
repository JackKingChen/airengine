// ***************************************************************
//  OTSceneManager   version:  1.0   ��  date: 03/24/2009
//  -------------------------------------------------------------
//  �����Ĭ�ϳ�����Ϊ�˲�������
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

    //��ӹ���
    virtual void AddLight(CLight* pLight);

    //��ó�����̫����
    virtual CLight* GetSunLight();

    //virtual Renderable* GetSkybox() { return (Renderable*)m_pSkybox; }

    virtual bool ProcessInputEvent(const LPINPUTEVENT pEvent);

    virtual void AddSceneNode(CSceneNode* pNode);

    //���°˲����ڵ�
    virtual void UpdateOctreeNode(COctreeNode* pNode);

    //ɾ�������ڵ�
    virtual void DestroySceneNode(CSceneNode* pNode);

    virtual void DestroySceneNode(const AIRString& strName);

    //���ð˲�����boundingbox
    void SetSceneBoundingBox(const AABBBox3Df& box);

    //��ó����еĵ���
    virtual CHeightMap* GetHeightMap()
    {
        return m_pHeightMap;
    }

    //virtual int GetRenderableObjectsNum() const;

    //virtual void  GetVisableRenderables(CCamera* pCamera, CRenderQueue& renderQueue);

    //virtual void GetVisableRenderables(CCamera* pCamera, CRenderQueue& renderQueue, f32 fDistanceIn);

	virtual void GetCollisionRenderables(const AABBBox3Df& aabbTest, AIRList<Renderable*>& renderList);

	//�Ƿ���shadowmap
    virtual void EnableShadowMap(bool bEnable);

    bool FindPickNAVTriangle(const Line3Df& ray, Vector3Df& vOut, NavTriangle& navTriOut);
    //���غó�����Գ�������cache
    virtual void CacheScene();

    //�ѵ�ǰ�����Ŀ�����ŵ�queue���档
    //virtual void GetRenderQueue(CRenderQueue* pQueue);

    virtual void Cull(CCamera* pCamera, AIRList<CSceneNode*>& lstVisableNodes, u32 nCullFlag);
protected:
    

	virtual CSceneNode* CreateSceneNodeImp(const AIRString& strName, const Vector3Df& vPosition /* = Vector3Df::ZERO */, const Quaternion& qRotation /* = Quaternion */,
		const Vector3Df& vScale = Vector3Df::UNIT_SCALE);

    //virtual void Render(CCamera* pCamera);
    //void   LoadTerrainConfig(TERRAINCONFIG& config, const char* szFilename);


    //virtual void GetVisableObjects();

    //virtual void GetVisableObjects(f32 fDistanceIn);

    //������ӷ�Χ�ڵĽ��
    //virtual void ProcessVisableObjects();

    virtual void LoadSceneNodeFromXML(CMarkupSTL& xml);

    virtual Renderable* LoadSceneRenderableFromXML(CMarkupSTL& xml, CSceneNode* pNode);

    //AIRString m_strName;

    COctree*  m_pOctree;  //�˲���
    //CCamera*  m_pCamera;  

    //CSkyBox*  m_pSkybox;

    //CWater    m_water;
    CHeightMap* m_pHeightMap;

private:
    //AIRList<Renderable*> m_lstRenderObject;
    //AIRList<Renderable*> m_lstAlphaObject;   //��alpha��Ⱦ״̬������
    //typedef AIRList<Renderable*>::iterator RENDERABLE_ITER;

    //AIRList<CCamera*> m_lstCamera;
    //typedef AIRList<CCamera*>::iterator CAMERA_ITER;

    //�����еĹ���
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

    
    //CLight m_ambientLight; //�����⣬Ψһ

    CSunLight  m_Sun;      //̫����

    AIRList<Renderable*>  m_lstPaparedRenderables;
    typedef AIRList<Renderable*>::iterator PrepareRenderableIter;
};
