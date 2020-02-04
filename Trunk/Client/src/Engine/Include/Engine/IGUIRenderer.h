// ***************************************************************
//  文件: IGUIRenderer.h
//  日期: 2010-2-1
//  作者: huangjunjie
//  说明: GUI渲染器基类
// ***************************************************************


#pragma once

#ifndef __IGUIRendererer_H__
#define __IGUIRendererer_H__

#include "IRenderer.h"

class EngineExport IGUIRenderer
{
public:
	IGUIRenderer() {}
	virtual ~IGUIRenderer() {}

	virtual void SetRenderer( IRenderer* pRenderer ) = 0;

	virtual void InitGUIRenderer( int nMaxQuadCountPerDP ) = 0;

	virtual void Clear( void ) = 0;

	virtual void BeginGUIRender() = 0;

	virtual void EndGUIRender() = 0;

    virtual void Render( const AIR::Rectf& rectScreen, const AIR::Rectf& rectUV, f32 fDepth, ITexture* pTex, const CColorValue& color = CColorValue::White ) = 0;

    virtual void SetDepths(f32 fMin, f32 fMax) = 0;

    virtual f32 GetMinZ() const = 0;
    
    virtual f32 GetMaxZ() const = 0;
protected:
	virtual void RenderImmediately( void ) = 0;

};//! end class IGUIRenderer


#endif //! end define __IGUIRenderer_H__