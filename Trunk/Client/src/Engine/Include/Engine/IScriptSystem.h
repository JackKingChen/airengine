// ***************************************************************
//  文件: IScriptSystem.h
//  日期: 2010-3-12
//  作者: huangjunjie
//  说明: 
// ***************************************************************


#pragma once

#ifndef __IScriptSystem_H__
#define __IScriptSystem_H__

#include <string>
#include "EngineConfig.h"

class EngineExport IScriptSystem
{
public:
    IScriptSystem()                                     {}
    virtual ~IScriptSystem()                            {}

    virtual bool Initialize( void )                     { return true; }

    virtual bool Release( void )                        { return true; }

    virtual bool Register( void )                       { return true; }

    virtual bool RunFile( const char* szFile )   { return true; }

    virtual bool RunString( const char* szBuf )  { return true; }

    virtual bool RunString(const std::string& strBuf) { return true; }

protected:

};//! end class IScriptSystem


#endif //! end define __IScriptSystem_H__