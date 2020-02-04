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
    AIRString m_strLMouseUpScript;	// 鼠标左键弹起执行的脚本
    AIRString m_strLMouseDownScript;	// 鼠标左键按下执行的脚本
	AIRString m_strRMouseUpScript;	// 鼠标右键弹起执行的脚本
	AIRString m_strRMouseDownScript;	// 鼠标右键按下执行的脚本
    AIRString m_strMouseHoverScript;	// 鼠标悬浮在控件上时执行的脚本
    AIRString m_strMouseLeaveScript;	// 鼠标离开控件时执行的脚本
    AIRString m_strOnKeyUpScript;		// 键盘按键弹起时执行的脚本
	AIRString m_strOnKeyDownScript;	// 键盘按键按下时执行的脚本
    AIRString m_strOnShowScript;      // 控件显示时的脚本
    AIRString m_strOnHideScript;      // 控件隐藏时的脚本

    //拖放事件脚本
    AIRString   m_strOnDropScript;
private:
    u8		m_byKeyUp;				// 弹起的按键
    u8		m_byKeyDown;			// 按下的按键

	AIRString	m_strScriptFileName;	// 脚本文件
	AIRString m_strScriptFileContent; // 脚本文件的内容(编辑器用)
	bool		m_bHasLoadScriptFile;	// 是否已经加载脚本文件
};
