/********************************************************************
	created:	2010/08/09
	created:	9:8:2010   20:48
	filename: 	RenderControl.h

	author:		liangairan
	
	purpose:	渲染控制类，主要是控制渲染流程
*********************************************************************/

#pragma once 

#include "Renderable.h"
#include "Camera.h"

class CTextureUnit;

class EngineExport RenderControl
{
public:
    static  RenderControl* GetInstance()
    {
        static RenderControl instance;
        return &instance;
    }
    
    virtual ~RenderControl();

    virtual void RenderSingleObject(Renderable* pRenderable, CCamera* pCamera);

    virtual void RenderSingleObject(Renderable* pRenderable, CCamera* pCamera, CPass* pPass);

    virtual void RenderGeometry(Renderable* pRenderable, CCamera* pCamera);

    virtual void RenderGeometry(Renderable* pRenderable, CCamera* pCamera, CPass* pPass);

    //使用指定纹理来渲染顶点数据
    virtual void RenderGeometry(Renderable* pRenderable, CCamera* pCamera, CPass* pPass, CTextureUnit* pTextureUnit);
protected:
private:
    RenderControl();
};

#define RENDER_CONTROL RenderControl::GetInstance()
