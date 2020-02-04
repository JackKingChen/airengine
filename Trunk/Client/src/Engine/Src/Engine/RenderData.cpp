#include "stdafx.h"
#include "RenderData.h"
#include "Engine.h"
#include "IRenderer.h"


void CRenderData::Clear()
{
	if (m_pVB)
	{
		//m_pVB->Release();
		//SafeDelete(m_pVB);
		ENGINE_INST->GetRenderer()->ClearVertexBuffer( m_pVB );
		m_pVB = NULL;
	}

	if (m_pIB)
	{
		//m_pIB->Release();
		//SafeDelete(m_pIB);
		ENGINE_INST->GetRenderer()->ClearIndexBuffer( m_pIB );
		m_pIB = NULL;
	}
}