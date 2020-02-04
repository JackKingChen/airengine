//#pragma once
//#include "stdafx.h"
#include "GUIScriptObject.h"
//#include "UILuaScriptMgr.h"
#include "IScriptSystem.h"
#include "Engine.h"
#include "FileSystem.h"
#include "MemoryDataStream.h"
#include "GUISystem.h"
#include "GUISchemeManager.h"

CGUIScriptObject::CGUIScriptObject()
{
    m_byKeyUp = 0;
    m_byKeyDown = 0;
	m_bHasLoadScriptFile = false;

	//m_strScriptFileName.clear();
	//m_strScriptFileContent.clear();
	//m_strRMouseUpScript.clear();
	//m_strRMouseDownScript.clear();
	//m_strLMouseUpScript.clear();
	//m_strLMouseDownScript.clear();
	//m_strOnKeyUpScript.clear();
	//m_strOnKeyDownScript.clear();
	//m_strMouseHoverScript.clear();
	//m_strMouseLeaveScript.clear();
	//m_strOnShowScript.clear();
}

CGUIScriptObject::~CGUIScriptObject()
{
    
}

bool CGUIScriptObject::Load(CMarkupSTL& xml)
{
    
    if (!LoadSelf(xml))
    {
        return false;
    }

    if (!LoadChild(xml))
    {
        return false;
    }
    

    return true;
}

bool CGUIScriptObject::LoadSelf(CMarkupSTL& xml)
{
	m_strScriptFileName = xml.GetAttrib( "File" ).c_str();

    return true;
}

bool CGUIScriptObject::LoadChild(CMarkupSTL& parentNode)
{ 
    parentNode.IntoElem();
    AIRString strTabName;
    while (parentNode.FindElem())
    {
        strTabName = parentNode.GetTagName().c_str();
        if (strTabName == "OnLoad")
        {
        }
        else if (strTabName == "OnEnter")
        {
        }
        else if (strTabName == "OnClick")
        {
        }
        else if (strTabName == "OnLMouseUp")
        {
            m_strLMouseUpScript = parentNode.GetData().c_str();
        }
        else if (strTabName == "OnLMouseDown")
        {
            m_strLMouseDownScript = parentNode.GetData().c_str();
        }
		else if (parentNode.GetTagName() == "OnRMouseUp")
		{
			m_strRMouseUpScript = parentNode.GetData().c_str();
		}
		else if (parentNode.GetTagName() == "OnRMouseDown")
		{
			m_strRMouseDownScript = parentNode.GetData().c_str();
		}
        else if (parentNode.GetTagName() == "OnMouseHover")
        {
            m_strMouseHoverScript = parentNode.GetData().c_str();
        }
        else if (parentNode.GetTagName() == "OnMouseLeave")
        {
            m_strMouseLeaveScript = parentNode.GetData().c_str();
        }
        else if (parentNode.GetTagName() == "OnTextChange")
        {
        }
        else if (parentNode.GetTagName() == "OnEvent")
        {
        }
        else if (parentNode.GetTagName() == "OnKeyDown")
        {
			m_strOnKeyDownScript = parentNode.GetData().c_str();
        }
        else if (parentNode.GetTagName() == "OnKeyUp")
        {
			m_strOnKeyUpScript = parentNode.GetData().c_str();
        }
        else if (strTabName == "OnShow")
        {
            m_strOnShowScript = parentNode.GetData().c_str();
        }
        else if (strTabName == "OnHide")
        {
            m_strOnHideScript = parentNode.GetData().c_str();
        }
        else if (strTabName == "OnDrop")
        {
            m_strOnDropScript = parentNode.GetData().c_str();
        }
    }
    parentNode.OutOfElem();
   
    return true;
}


bool CGUIScriptObject::SaveToXML(CMarkupSTL& parentNode)
{
	if ( m_strScriptFileName.length() > 0 )
	{
		parentNode.AddAttrib( "File", m_strScriptFileName.c_str() );
	}

	parentNode.IntoElem();

	if ( m_strLMouseUpScript.length() > 0 )
	{
		parentNode.AddElem( "OnLMouseUp" );
		parentNode.SetData( m_strLMouseUpScript.c_str() );
	}

	if ( m_strLMouseDownScript.length() > 0 )
	{
		parentNode.AddElem( "OnLMouseDown" );
		parentNode.SetData( m_strLMouseDownScript.c_str() );
	}

	if ( m_strRMouseUpScript.length() > 0 )
	{
		parentNode.AddElem( "OnRMouseUp" );
		parentNode.SetData( m_strRMouseUpScript.c_str() );
	}

	if ( m_strRMouseDownScript.length() > 0 )
	{
		parentNode.AddElem( "OnRMouseDown" );
		parentNode.SetData( m_strRMouseDownScript.c_str() );
	}

	if ( m_strMouseHoverScript.length() > 0 )
	{
		parentNode.AddElem( "OnMouseHover" );
		parentNode.SetData( m_strMouseHoverScript.c_str() );
	}

	if ( m_strMouseLeaveScript.length() > 0 )
	{
		parentNode.AddElem( "OnMouseLeave" );
		parentNode.SetData( m_strMouseLeaveScript.c_str() );
	}

	if ( m_strOnKeyUpScript.length() > 0 )
	{
		parentNode.AddElem( "OnKeyUp" );
		parentNode.SetData( m_strOnKeyUpScript.c_str() );
	}

	if ( m_strOnKeyDownScript.length() > 0 )
	{
		parentNode.AddElem( "OnKeyDown" );
		parentNode.SetData( m_strOnKeyDownScript.c_str() );
	}

    if (m_strOnShowScript.length() > 0)
    {
        parentNode.AddElem("OnShow");
        parentNode.SetData( m_strOnShowScript.c_str() );
    }

    if (m_strOnHideScript.length() > 0)
    {
        parentNode.AddElem("OnHide");
        parentNode.SetData(m_strOnHideScript.c_str());
    }

    if (m_strOnDropScript.length() > 0)
    {
        parentNode.AddElem("OnDrop");
        parentNode.SetData(m_strOnDropScript.c_str());
    }

	parentNode.OutOfElem();

	return true;
}


bool CGUIScriptObject::RunOnLMouseUpScript()
{
    if(m_strLMouseUpScript.empty() || GUISYSTEM->GetCurrentScheme()->GetEditMode())
	{
        return false;
    }
	PreLoadScriptFromFile();
	if ( ENGINE_INST->GetScriptSystem() )
    {
        return ENGINE_INST->GetScriptSystem()->RunString( m_strLMouseUpScript.c_str() );
    }
    return false;
}

bool CGUIScriptObject::RunOnLMouseDownScript()
{
    if(m_strLMouseDownScript.empty() || GUISYSTEM->GetCurrentScheme()->GetEditMode())
	{
        return false;
	}

	PreLoadScriptFromFile();
	if ( ENGINE_INST->GetScriptSystem() )
    {
        return ENGINE_INST->GetScriptSystem()->RunString( m_strLMouseDownScript.c_str() );
    }
    return false;
}

bool CGUIScriptObject::RunOnRMouseUpScript()
{
	if(m_strRMouseUpScript.empty() || GUISYSTEM->GetCurrentScheme()->GetEditMode())
	{
		return false;
	}

	PreLoadScriptFromFile();
	if ( ENGINE_INST->GetScriptSystem() )
	{
		return ENGINE_INST->GetScriptSystem()->RunString( m_strRMouseUpScript.c_str() );
	}
	return false;
}

bool CGUIScriptObject::RunOnRMouseDownScript()
{
	if(m_strRMouseDownScript.empty() || GUISYSTEM->GetCurrentScheme()->GetEditMode())
	{
		return false;
	}

	PreLoadScriptFromFile();
	if ( ENGINE_INST->GetScriptSystem() )
	{
		return ENGINE_INST->GetScriptSystem()->RunString( m_strRMouseDownScript.c_str() );
	}
	return false;
}

bool CGUIScriptObject::RunOnMouseHoverScript()
{
    if(m_strMouseHoverScript.empty() || GUISYSTEM->GetCurrentScheme()->GetEditMode())
	{
        return false;
	}

	PreLoadScriptFromFile();
    if ( ENGINE_INST->GetScriptSystem() )
    {
        return ENGINE_INST->GetScriptSystem()->RunString( m_strMouseHoverScript.c_str() );
    }

    return false;
}

bool CGUIScriptObject::RunOnMouseLeaveScript()
{
    if(m_strMouseLeaveScript.empty() || GUISYSTEM->GetCurrentScheme()->GetEditMode())
	{
        return false;
	}

	PreLoadScriptFromFile();
    if ( ENGINE_INST->GetScriptSystem() )
    {
        return ENGINE_INST->GetScriptSystem()->RunString( m_strMouseLeaveScript.c_str() );
    }

    return false;
}

bool CGUIScriptObject::RunOnKeyUpScript()
{
    if (m_strOnKeyUpScript.empty() || GUISYSTEM->GetCurrentScheme()->GetEditMode())
    {
        return false;
    }

	PreLoadScriptFromFile();
    if ( ENGINE_INST->GetScriptSystem() )
    {
        return ENGINE_INST->GetScriptSystem()->RunString( m_strOnKeyUpScript.c_str() );
    }

    return false;
}

bool CGUIScriptObject::RunOnShowScript()
{
    if (m_strOnShowScript.empty() || GUISYSTEM->GetCurrentScheme()->GetEditMode())
    {
        return false;
    }

    PreLoadScriptFromFile();
    if ( ENGINE_INST->GetScriptSystem() )
    {
        return ENGINE_INST->GetScriptSystem()->RunString( m_strOnShowScript.c_str() );
    }
    return false;
}

bool CGUIScriptObject::RunOnHideScript()
{
    if (m_strOnHideScript.empty() || GUISYSTEM->GetCurrentScheme()->GetEditMode())
    {
        return false;
    }

    PreLoadScriptFromFile();
    if ( ENGINE_INST->GetScriptSystem() )
    {
        return ENGINE_INST->GetScriptSystem()->RunString( m_strOnHideScript.c_str() );
    }

    return false;
}

bool CGUIScriptObject::RunOnDropScript()
{
    if (m_strOnDropScript.empty() || GUISYSTEM->GetCurrentScheme()->GetEditMode())
    {
        return false;
    }

    PreLoadScriptFromFile();
    if ( ENGINE_INST->GetScriptSystem() )
    {
        return ENGINE_INST->GetScriptSystem()->RunString( m_strOnDropScript.c_str() );
    }

    return false;
}

void  CGUIScriptObject::PreLoadScriptFromFile( void )
{
	if ( m_bHasLoadScriptFile )
	{
		return;
	}
	if ( m_strScriptFileName.length() > 0 )
	{
		CDataStream* pStream = FILESYSTEM->GetFileStream( m_strScriptFileName.c_str() );
		if ( !pStream )
		{
			DT_TO_MSGBOX_FMT( LT_WARN, GetString("���ؽű��ļ� [%s] ʧ��!", m_strScriptFileName.c_str()) );
		}
		if ( ENGINE_INST->GetScriptSystem() )
		{
			ENGINE_INST->GetScriptSystem()->RunString( pStream->GetAsString().c_str() );
			m_bHasLoadScriptFile = true;
		}
		SafeDelete( pStream );
	}
	else
	{
		m_bHasLoadScriptFile = true;
	}
}

CGUIScriptObject& CGUIScriptObject::operator = (const CGUIScriptObject& other)
{
    m_bHasLoadScriptFile = other.m_bHasLoadScriptFile;
    m_byKeyDown = other.m_byKeyDown;
    m_byKeyUp = other.m_byKeyUp;
    
    m_strLMouseUpScript = other.m_strLMouseUpScript;	// ����������ִ�еĽű�
    m_strLMouseDownScript = other.m_strLMouseDownScript;	// ����������ִ�еĽű�
    m_strRMouseUpScript = other.m_strRMouseUpScript;	// ����Ҽ�����ִ�еĽű�
    m_strRMouseDownScript = other.m_strRMouseDownScript;	// ����Ҽ�����ִ�еĽű�
    m_strMouseHoverScript = other.m_strMouseHoverScript;	// ��������ڿؼ���ʱִ�еĽű�
    m_strMouseLeaveScript = other.m_strMouseLeaveScript;	// ����뿪�ؼ�ʱִ�еĽű�
    m_strOnKeyUpScript = other.m_strOnKeyUpScript;		// ���̰�������ʱִ�еĽű�
    m_strOnKeyDownScript = other.m_strOnKeyDownScript;	// ���̰�������ʱִ�еĽű�
    m_strOnShowScript = other.m_strOnShowScript;      // �ؼ���ʾʱ�Ľű�
    m_strOnHideScript = other.m_strOnHideScript;      // �ؼ�����ʱ�Ľű�
    m_strOnDropScript = other.m_strOnDropScript;
    return *this;
}
