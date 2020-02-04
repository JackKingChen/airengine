#pragma once 
#include "Common.h"
#include "EngineConfig.h"
#include "IMaterial.h"
#include "Singleton.h"
//#include "MaterialSet.h"

class EngineExport CMaterialManager : public Singleton<CMaterialManager>
{
    BE_SINGLETON(CMaterialManager);
public:
    
    ~CMaterialManager();

    //IMaterial* GetMaterial(u32 nID);

    IMaterial* GetMaterial(const AIRString& strName);

    IMaterial* GetMaterial(const char* szName);

    void AddMaterial(IMaterial* pMaterial);

    void ClearMaterial(IMaterial*& pMaterial);

    void Clear();

    //加载材质资源文件
    //szFilename   材质文件
    //bLoaded      true为加载资源，false为只加载文件，但资源没加载
    //返回材质的数量
    int LoadResource(const char* szFilename, bool bLoaded = true);

    //int LoadMaterialFile(const char* szFilename);

    //读取材质脚本
    //IMaterial* CreateMaterial(const char* szFilename, const char* szMaterialName);

    //创建材质
    IMaterial* CreateMaterial(const char* szName);

    //创建一个空的材质
    IMaterial* CreateMaterial();

    //通过模板创建材质
    IMaterial* CreateFromTemplate(const char* szTemplate);
    
protected:
private:
    //AIRMap<s32, IMaterial*> m_mapMaterial;
    //typedef AIRMap<s32, IMaterial*>::iterator MATERIAL_ITER;

    //AIRVector<CMaterialSet*>  m_vtMaterialSets;
    //typedef AIRVector<CMaterialSet*>::iterator MATERIALSET_ITER;

    typedef AIRList<IMaterial*> MATERIAL_LIST; 
    typedef MATERIAL_LIST::iterator MATERIAL_LIST_ITER;
    MATERIAL_LIST    m_lstMaterials;

    typedef AIRMap<AIRString, IMaterial*> MaterialMap;
    typedef MaterialMap::iterator MaterialMapIter;

    u32 m_nMaterialCount;
};

#define MATERIAL_MANAGER CMaterialManager::GetInstance()
