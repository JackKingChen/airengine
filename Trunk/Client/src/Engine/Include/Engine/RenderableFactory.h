/********************************************************************
	created:	2009/10/19
	created:	19:10:2009   17:01
	filename: 	d:\My Codes\3DGraphic\Include\RenderableFactory.h
	file path:	d:\My Codes\3DGraphic\Include
	file base:	RenderableFactory
	file ext:	h
	author:		liangairan
	
	purpose:	渲染对象的类工厂
*********************************************************************/

#pragma once 

#include "Renderable.h"
#include "Singleton.h"
#include "ISceneManager.h"

typedef Renderable* (*CreateRenderableFunc)(const char* szName);

class EngineExport CRenderableFactory : public Singleton<CRenderableFactory>
{
    BE_SINGLETON(CRenderableFactory);
public:
    friend class ISceneManager;
    ~CRenderableFactory();

    void RegisterRenderableCreateFunc(const char* szType, CreateRenderableFunc func);

    template <class T>
    T* CreateRenderable(const char* szName)
    {
        if (T::IsRenderable())
           return new T(szName);
        return 0;
    }

    Renderable* CreateRenderable(const char* szName, const char* szType);
private:
    //Renderable* CreateRenderable(const char* szName, const char* szFilename);

    

    std::map<AIRString, CreateRenderableFunc> m_mapCreateFunc;

	//Renderable* FindRenderable( const char* szName, const char* szFilename );
/*
protected:
    Renderable* CreateModelInstance(const char* szName, const char* szFilename);

    Renderable* CreateParticleSystem(const char* szName, const char* szFilename);

	//Renderable* CreateGEffect(const char* szName, const char* szFilename);

	Renderable* CreateElectroLight(const char* szName, const char* szFilename);

	Renderable* CreateRibbonTrail(const char* szName, const char* szFilename);

    //创建贴花
    Renderable* CreateDecal(const char* szName, const char* szFilename);

    //创建speedtree
    Renderable* CreateSpeedTree(const char* szName, const char* szFilename);
*/
};

#define RENDERABLE_FACTORY CRenderableFactory::GetInstance()
