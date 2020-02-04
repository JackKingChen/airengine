// ***************************************************************
//  �ļ�: TerrainDecal.h
//  ����: 2010-4-8
//  ����: liangairan
//  ˵��: ����������
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

    //����decal
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

    //��ʾ����
    void ShowDecal(const Vector3Df& terrainPosition);

	//��xml����
	virtual bool LoadFromFile(const char* szFilename);

	virtual bool SaveToFile(const char* szFilename);

    //���ð󶨵�scenenode
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

    // ��ȾǰҪ������
    virtual void    PreRender(CCamera* pCamera);

    virtual void _GetPrimitive();
    
    //���㣬������Ⱦ����
    void ComputeRenderData();

    bool Cache();
private:
    CHeightMap*		m_pTerrain;  //��������
    float			m_fWidth;   //��
    float			m_fHeight;  //��
    float           m_fBottomRightU;   //���½�u
    float           m_fBottomRightV;   //���½�v
    bool            m_bNeedCaculate;   //�Ƿ���Ҫ���¼���
	AIRString		m_szDecalTexture;
    Vector3Df       m_worldPosition;   //���ĵ����������
    bool    m_bLoadFromFile;
    FileData*  m_pFileData;
};
