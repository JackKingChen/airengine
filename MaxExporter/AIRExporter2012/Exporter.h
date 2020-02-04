#pragma once

#include "MaxInterface.h"
#include "AIRSkeleton.h"

class CExporter
{
public:
    virtual ~CExporter();
    CExporter();

    //static CExporter* GetInterface()
    //{
    //    if (m_pInterface == NULL)
    //    {
    //        m_pInterface = new CExporter;
    //    }
    //    return m_pInterface;
    //}
    CMaxInterface* GetMaxInterface() { return &m_MaxInterface; }

    virtual bool Create(ExpInterface *pExpInterface, Interface *pInterface, HWND hWnd);
    virtual bool Export(const char* szFilename) = 0;
    virtual bool ExportAnimation(CAIRSkeleton* pSkeleton, const std::string& strFilename) = 0;
    virtual bool ExportMaterial(const std::string& strFilename) = 0;
    virtual bool ExportMesh(const std::string& strFilename) = 0;
    virtual bool ExportSkeleton(CAIRSkeleton& skeletonOut, const std::string& strFilename) = 0;
    virtual bool ExportStaticModel(CAIRSkeleton* pSkeleton, const std::string& strFilename) = 0;

    virtual void ShowErrorMsg(const char* szMsg, const char* szFilename, int nLine);
protected:
    
    CMaxInterface  m_MaxInterface;
    HWND  m_hMainWnd;    //Ö÷´°¿Ú
private:
    

    
    //static CExporter* m_pInterface;
};
