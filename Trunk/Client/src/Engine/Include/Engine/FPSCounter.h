#pragma once

#include "IFontBase.h"
//#include "MemorySTLAllocator.h"

class TEXT_INFO// : public EngineAllocator(TEXT_INFO)
{
	EngineAllocator(TEXT_INFO)
public:
    TEXT_INFO( const std::wstring& buf, const Rectf& rc, const SColor& col ) : m_szInfo(buf)
        , m_RC(rc), m_Color(col)
    {
    }
    std::wstring		m_szInfo;
    Rectf				m_RC;
    SColor				m_Color;
};


class EngineExport CFPSCounter
{
public:
    CFPSCounter();
    ~CFPSCounter();

    void Init();

    void Render();

    void Update();

    void CleanUp();

	int AddTextInfo( const AIRString& buf, int x = 10, int y = 10, const SColor& c = SColor::Yellow );

    void ChangeTextInfo( int id, const AIRString& buf );

	void ChangeTextInfo( int id, const AIRString& buf, const Rectf& rc );

	float GetCurFps( void ) 
	{ 
		return m_fCurFps; 
	}

	void SetShow( bool b )
	{ 
		m_bShow = b; 
	}

	float GetFrameTime( void )
	{
		return m_fFrameTime;
	}

protected:
private:

    IFontBase* m_pFont;
    wchar_t m_szText[256];
    wchar_t m_szSceneInfo[256];
    double m_dCurRenderTime;
    AIRVector<TEXT_INFO*> m_vtInfo;
    
	float	m_fMinFps;
	float	m_fMaxFps;
	float	m_fAvgFps;
	float	m_fCurFps;

	float   m_fFrameTime;

	bool	m_bShow;
    
    //CVertexDeclaration*  m_pVertexDecl;
};
