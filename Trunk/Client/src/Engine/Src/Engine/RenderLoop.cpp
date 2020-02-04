#include "stdafx.h"
#include "RenderLoop.h"
#include "Shadowmap.h"
#include "CustomRenderProcess.h"

RLForward::RLForward()
{

}

RLForward::~RLForward()
{

}

void RLForward::Render()
{
	SHADOWMAP->Render();

	for (size_t i = 0; i < m_vtCustomRenderObjs.size(); ++i)
	{
		if (m_vtCustomRenderObjs[i]->IsActive())
		{
			m_vtCustomRenderObjs[i]->Render();
		}
		
	}
}
