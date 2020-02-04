/********************************************************************
	created:	2010/05/28
	created:	28:5:2010   10:20
	filename: 	VertexDeclarationManager.h
	author:		liangairan
	
	purpose:	顶点声明管理器
*********************************************************************/

#pragma once 

#include "VertexDeclaration.h"
#include "Singleton.h"
#include "FixedString.h"

class EngineExport VertexDeclarationManager : public Singleton<VertexDeclarationManager>
{
    BE_SINGLETON(VertexDeclarationManager);
public:
    typedef AIRVector<CVertexElement> ElementVector;
public:
    ~VertexDeclarationManager();

    bool LoadFromXML(const char* szFilename);

    //CVertexDeclaration* CreateVertexDeclaration(EVertexBufferType type);

	//void AddVertexDeclaration( CVertexDeclaration* pVertexDecl );

    //清空所有，由系统最后清空
    void Clear();

    ElementVector& GetElementVector(const FixedString& strName);

    CVertexDeclaration* CreateVertexDeclaration(const FixedString& strName, IGPUProgram* pVertexShader);
protected:
private:

    //typedef AIRMap<EVertexBufferType, CVertexDeclaration*> VertexDeclarationMap;
    //typedef VertexDeclarationMap::iterator  VertexDeclarationMap_Iter;

    //VertexDeclarationMap  m_mapDefaultVD;   //默认的顶点声明

    
    AIRMap<FixedString, ElementVector> m_mapVertexDeclElements;

    typedef AIRMap<FixedString, CVertexDeclaration*> VertexDeclarationMap;
    typedef VertexDeclarationMap::iterator  VertexDeclarationMap_Iter;
    AIRMap<FixedString, CVertexDeclaration*> m_mapVertexDeclarations;
};

#define VERTEXDECL_MANAGER VertexDeclarationManager::GetInstance()
