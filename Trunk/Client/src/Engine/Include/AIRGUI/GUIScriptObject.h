#pragma once 
#include "MarkupSTL.h"
//#include "XMLParser.h"
#include "Common.h"
#include "AIRGUI.h"
#include "MemoryAlloter.h"

class AIRGuiExport CGUIScriptObject //: public EngineAllocator(CGUIScriptObject)
{
	EngineAllocator(CGUIScriptObject)
public:
    CGUIScriptObject();
    ~CGUIScriptObject();
    bool  Load(CMarkupSTL& xml);
    bool  LoadSelf(CMarkupSTL& xml);
    bool  LoadChild(CMarkupSTL& parentNode);
	bool  SaveToXML(CMarkupSTL& parentNode);
	bool  IsScriptObjectNotEmpty( void )
	{
		return 
			(
			m_strScriptFileName.length() > 0 || 
			m_strScriptFileContent.length() > 0 ||
			m_strRMouseUpScript.length() > 0 ||
			m_strRMouseDownScript.length() > 0 ||
			m_strLMouseUpScript.length() > 0 ||
			m_strLMouseDownScript.length() > 0 ||
			m_strOnKeyUpScript.length() > 0 ||
			m_strOnKeyDownScript.length() > 0 ||
			m_strMouseHoverScript.length() > 0 ||
			m_strMouseLeaveScript.length() > 0 ||
			m_strOnShowScript.length() > 0 ||
            m_strOnDropScript.length() > 0
			);
	}

	void  PreLoadScriptFromFile( void );

    bool  RunOnLMouseUpScript();
    bool  RunOnLMouseDownScript();
	bool  RunOnRMouseUpScript();
	bool  RunOnRMouseDownScript();
    bool  RunOnKeyUpScript();
	bool  RunOnKeyDownScript();
    bool  RunOnMouseHoverScript();
    bool  RunOnMouseLeaveScript();
    bool  RunOnShowScript();

    bool  RunOnHideScript();

    bool  RunOnDropScript();

	void  SetKeyUp( u8 key )		{ m_byKeyUp = key; }
	void  SetKeyDown( u8 key )	{ m_byKeyDown = key; }

    u8  GetKeyUp()				{ return m_byKeyUp; }
    u8  GetKeyDown()				{ return m_byKeyDown; }

	void SetLMouseUpScript( const AIRString& str )
	{
		m_strLMouseUpScript = str;
	}

	const AIRString& GetLMouseUpScript( void ) const
	{
		return m_strLMouseUpScript;
	}

	void SetRMouseUpScript( const AIRString& str )
	{
		m_strRMouseUpScript = str;
	}

	const AIRString& GetRMouseUpScript( void ) const
	{
		return m_strRMouseUpScript;
	}

	void SetLMouseDownScript( const AIRString& str )
	{
		m_strLMouseDownScript = str;
	}

	const AIRString& GetLMouseDownScript( void ) const
	{
		return m_strLMouseDownScript;
	}

	void SetRMouseDownScript( const AIRString& str )
	{
		m_strRMouseDownScript = str;
	}

	const AIRString& GetRMouseDownScript( void ) const
	{
		return m_strRMouseDownScript;
	}

	void SetKeyUpScript( const AIRString& str )
	{
		m_strOnKeyUpScript = str;
	}

	const AIRString& GetKeyUpScript( void ) const
	{
		return m_strOnKeyUpScript;
	}

	void SetKeyDownScript( const AIRString& str )
	{
		m_strOnKeyDownScript = str;
	}

	const AIRString& GetKeyDownScript( void ) const
	{
		return m_strOnKeyDownScript;
	}

	void SetMouseHoverScript( const AIRString& str )
	{
		m_strMouseHoverScript = str;
	}

	const AIRString& GetMouseHoverScript( void ) const
	{
		return m_strMouseHoverScript;
	}

	void SetMouseLeaveScript( const AIRString& str )
	{
		m_strMouseLeaveScript = str;
	}

	const AIRString& GetMouseLeaveScript( void ) const
	{
		return m_strMouseLeaveScript;
	}

    void SetOnShowScript(const AIRString& str)
    {
        m_strOnShowScript = str;
    }

    const AIRString& GetOnShowScript() const
    {
        return m_strOnShowScript;
    }

    void SetOnHideScript(const AIRString& str)
    {
        m_strOnHideScript = str;
    }

    const AIRString& GetOnHideScript() const
    {
        return m_strOnHideScript;
    }

	void SetScriptFileName( const AIRString& strScriptFileName )
	{
		m_strScriptFileName = strScriptFileName;
	}

	const AIRString& GetScriptFileName( void ) const
	{
		return m_strScriptFileName;
	}

	void SetScriptFileContent( const AIRString& strScriptFileContent )
	{
		m_strScriptFileContent = strScriptFileContent;
	}

	const AIRString& GetScriptFileContent( void ) const
	{
		return m_strScriptFileContent;
	}

    void SetOnDropScript(const AIRString& strScript)
    {
        m_strOnDropScript = strScript;
    }

    const AIRString& GetOnDropScript() const
    {
        return m_strOnDropScript;
    }

    CGUIScriptObject& operator = (const CGUIScriptObject& other);

protected:
    AIRString m_strLMouseUpScript;	// ����������ִ�еĽű�
    AIRString m_strLMouseDownScript;	// ����������ִ�еĽű�
	AIRString m_strRMouseUpScript;	// ����Ҽ�����ִ�еĽű�
	AIRString m_strRMouseDownScript;	// ����Ҽ�����ִ�еĽű�
    AIRString m_strMouseHoverScript;	// ��������ڿؼ���ʱִ�еĽű�
    AIRString m_strMouseLeaveScript;	// ����뿪�ؼ�ʱִ�еĽű�
    AIRString m_strOnKeyUpScript;		// ���̰�������ʱִ�еĽű�
	AIRString m_strOnKeyDownScript;	// ���̰�������ʱִ�еĽű�
    AIRString m_strOnShowScript;      // �ؼ���ʾʱ�Ľű�
    AIRString m_strOnHideScript;      // �ؼ�����ʱ�Ľű�

    //�Ϸ��¼��ű�
    AIRString   m_strOnDropScript;
private:
    u8		m_byKeyUp;				// ����İ���
    u8		m_byKeyDown;			// ���µİ���

	AIRString	m_strScriptFileName;	// �ű��ļ�
	AIRString m_strScriptFileContent; // �ű��ļ�������(�༭����)
	bool		m_bHasLoadScriptFile;	// �Ƿ��Ѿ����ؽű��ļ�
};
