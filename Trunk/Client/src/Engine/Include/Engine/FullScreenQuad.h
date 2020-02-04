/********************************************************************
	created:	23:2:2011   15:52
	filename: 	FullScreenQuad.h
	author:		liangairan
	
	purpose:	ȫ��������Ⱦ
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
    VERTEX_NOCOLOR      m_fullScreenQuad[4];    //����shader�ģ�����ҪX���������-1��1֮��
    //CVertexDeclaration*  m_pQuadVertexDecl;   //full screen quad vertex decalartion
private:
};

