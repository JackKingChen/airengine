/********************************************************************
	created:	23:2:2011   15:52
	filename: 	FullScreenQuad.h
	author:		liangairan
	
	purpose:	全屏矩形渲染
*********************************************************************/

#pragma once

#include "Renderable.h"


class EngineExport FullScreenQuad : public Renderable
{
public:
    FullScreenQuad();
    virtual ~FullScreenQuad();
protected:
    virtual void PreRender(CCamera* pCamera);

    virtual void _GetPrimitive();
protected:
    VERTEX_NOCOLOR      m_fullScreenQuad[4];    //用于shader的，不需要X矩阵，因此是-1到1之间
    //CVertexDeclaration*  m_pQuadVertexDecl;   //full screen quad vertex decalartion
private:
};

