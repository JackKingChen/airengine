// ***************************************************************
//  文件: TerrainDecal.h
//  日期: 2010-4-8
//  作者: liangairan
//  说明: 地形贴花类
// ***************************************************************]

#pragma once 

#include "Renderable.h"

class CHeightMap;
class FileData;

class EngineExport TerrainDecal : public Renderable
{
public:
    TerrainDecal();

    virtual ~TerrainDecal();

    //创建decal
    bool Create(CHeightMap* pTerrain, const char* szTextureFile, float fWidth, float fHeight);

	void SetDecalTexture( const char* szTextureFile );

	inline const AIRString& GetDecalTexture() const
	{
		return m_szDecalTexture;
	}

    float GetWidth() const
    {
        return m_fWidth;
    }

    float GetHeight() const
    {
        return m_fHeight;
    }

    f32 GetBottomRightU() const
    {
        return m_fBottomRightU;
    }

    f32 GetBottomRightV() const
    {
        return m_fBottomRightV;
    }

    void SetWidth(float fWidth);

    void SetHeight(float fHeight);

    void SetBottomRightUV(float fU, float fV);

    //显示贴花
    void ShowDecal(const Vector3Df& terrainPosition);

	//从xml加载
	virtual bool LoadFromFile(const char* szFilename);

	virtual bool SaveToFile(const char* szFilename);

    //设置绑定的scenenode
    virtual void SetAttachedNode(CSceneNode* pNode);

    virtual void SetTransparent(bool bAlpha /* = true */, u8 nAlpha /* = 128 */);

    virtual bool Prepare();

	virtual const Matrix4f& GetWorldMatrix() const
	{
		return Matrix4f::IDENTITY;
	}

	//void Update( float fTimeElapse );
protected:
    void InitializeRenderData();

    // 渲染前要做的事
    virtual void    PreRender(CCamera* pCamera);

    virtual void _GetPrimitive();
    
    //计算，生成渲染数据
    void ComputeRenderData();

    bool Cache();
private:
    CHeightMap*		m_pTerrain;  //所属地形
    float			m_fWidth;   //宽
    float			m_fHeight;  //高
    float           m_fBottomRightU;   //右下角u
    float           m_fBottomRightV;   //右下角v
    bool            m_bNeedCaculate;   //是否需要重新计算
	AIRString		m_szDecalTexture;
    Vector3Df       m_worldPosition;   //中心点的世界坐标
    bool    m_bLoadFromFile;
    FileData*  m_pFileData;
};
