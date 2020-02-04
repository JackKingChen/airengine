#include "Exporter.h"
#include <sstream>
//CExporter* CExporter::m_pInterface = NULL;

CExporter::CExporter()
{

}

CExporter::~CExporter()
{

}

bool CExporter::Create(ExpInterface *pExpInterface, Interface *pInterface, HWND hWnd)
{
    //m_MaxInterface = pInterface;
    m_MaxInterface.Create(pExpInterface, pInterface);
    m_hMainWnd = hWnd;
    return true;
}

void CExporter::ShowErrorMsg(const char* szMsg, const char* szFilename, int nLine)
{
    std::stringstream strstrError;
    strstrError << std::string(szMsg) << "\n(" << std::string(szFilename) << " " << nLine << ")" << std::ends;
    MessageBox(m_hMainWnd, strstrError.str().c_str(), "ERROR", MB_OK);
}
