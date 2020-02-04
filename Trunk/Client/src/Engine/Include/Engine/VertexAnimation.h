/********************************************************************
	created:	2010/07/16
	created:	16:7:2010   10:47
	filename: 	VertexAnimation.h
	author:		liangairan
	
	purpose:	顶点动画
*********************************************************************/

#pragma once 

#include "AIRAnimation.h"
#include "MemoryAllocatorConfig.h"

enum VertexAnimationMask
{
    eVAM_None = 0, //0的话不导出
    eVAM_Position = 1,
    eVAM_UV = 2,    
    eVAM_Color = 4,
};

class CAIRSubMesh;
/*
typedef struct tagVertexKeyframeData 
{
    Vector3Df position;
    f32     tu;
    f32     tv;
    u32       color;
}VertexKeyframeData;


class EngineExport VertexAnimationKeyframe
{
public:
    typedef AIRVector<VertexKeyframeData> VertexKeyframeDataArray;
    //VertexKeyframeDataArray m_vtKeyframeDatas;
    typedef std::map<AIRString, VertexKeyframeDataArray> VertexKeyframeDataArrayMap;
public:
    VertexAnimationKeyframe();
    virtual ~VertexAnimationKeyframe();

    VertexKeyframeData*  GetKeyframeData(const AIRString& strName, size_t index)
    {
        VertexKeyframeDataArrayMap_Iter it = m_mapMeshKeyframes.find(strName);
        if (it == m_mapMeshKeyframes.end())
        {
            return NULL;
        }
        //VertexKeyframeDataArray* pArray = &(it->second);
        return &((it->second)[index]);
    }

    void AddKeyframeData(const AIRString& strName, const VertexKeyframeData& data);

    VertexKeyframeDataArray* GetKeyframeDataArray(const AIRString& strName)
    {
        VertexKeyframeDataArrayMap_Iter it = m_mapMeshKeyframes.find(strName);
        if (it == m_mapMeshKeyframes.end())
        {
            return NULL;
        }

        return &(it->second);
    }

    //从地图读入
    bool LoadFromFileDataBinary(CDataStream* pStream);
protected:
private:
    
    typedef VertexKeyframeDataArrayMap::iterator  VertexKeyframeDataArrayMap_Iter;
    VertexKeyframeDataArrayMap  m_mapMeshKeyframes;
};
*/

class EngineExport VertexAnimationKeyframe //: public EngineAllocator(VertexAnimationKeyframe)
{
	EngineAllocator(VertexAnimationKeyframe)
public:
    //typedef AIRVector<VertexKeyframeData, CMemorySTLAllocator<Vector3Df, CMemoryNedAlloc>> VertexKeyframeDataArray;
    typedef AIRVector<Vector3Df> VertexKeyframePositionArray;
    typedef AIRVector<Vector2Df> VertexKeyframeUVArray;
    typedef AIRVector<u32>       VertexKeyframeColorArray;
public:
    VertexAnimationKeyframe();
    virtual ~VertexAnimationKeyframe();

    VertexKeyframePositionArray m_vtVertexKeyframePositions;
    VertexKeyframeUVArray m_vtVertexKeyframeUVs;
    VertexKeyframeColorArray m_vtVertexKeyframeColors;
};

class VertexAnimation;

class EngineExport MeshVertexAnimation //: public EngineAllocator(MeshVertexAnimation)
{
	EngineAllocator(MeshVertexAnimation)
public:
    struct VertexInfo 
    {
        u16 nPositionIndex;
        u16 nTexcoordIndex;
        u32 color;
    };
    typedef AIRVector<VertexAnimationKeyframe*> VertexAnimationKeyframes;
public:
    MeshVertexAnimation(VertexAnimation* pAnimation);
    virtual ~MeshVertexAnimation();

    bool LoadFromFileDataBinary(CDataStream* pFileStream);

    VertexAnimationKeyframe*  GetKeyframe(size_t index)
    {
        return m_vtKeyframes[index];
    }

    const AIRString& GetName() const
    {
        return m_strName;
    }

private:
    AIRVector<Vector3Df> m_vtAllVertices;
    AIRVector<Vector2Df> m_vtAllTexcoords;

    VertexAnimationKeyframes m_vtKeyframes;

    VertexAnimation*  m_pVertexAnimation;

    AIRString m_strName;  //名字
};

class EngineExport VertexAnimation : public CAIRAnimation
{
public:
    VertexAnimation();
    virtual ~VertexAnimation();

    //作用到模型上
    virtual void Apply(CModelInstance* pModelInst, float fTimePos, float fWeight);

    virtual void Blend(CModelInstance* pModelInst, float fTimePos, float fWeight);

    //清空
    virtual void Clear();

    bool    LoadFromFileData(CDataStream* pFileStream);

    bool    LoadFromFileDataBinary(CDataStream* pFileStream);

    //设置帧速（每秒放多少帧）
    virtual void SetFrameRate(float fFrameRate);

    void    ApplyToSubmesh(CAIRSubMesh* pMesh, float fTimePos);

    void ApplyToVertices(CAIRSubMesh* pMesh, LPVERTEX_NORMAL pVertices, u32 nVerticesNum, f32 fTimePos);

    s32 GetVertexAnimationMask() const
    {
        return m_nVertexAnimationMask;
    }
protected:
    float GetKeyFramesAtTime(CAIRSubMesh* pMesh, float fTimePos, VertexAnimationKeyframe** pKeyFrame1, VertexAnimationKeyframe** pKeyFrame2);
private:
    //AIRVector<VertexAnimationKeyframe*>  m_vtKeyframes;    //关键帧
    typedef std::map<AIRString, MeshVertexAnimation*> MeshVertexAnimationMap;
    typedef MeshVertexAnimationMap::iterator MeshVertexAnimationMap_Iter;
    std::map<AIRString, MeshVertexAnimation*> m_mapMeshVertexAnimations;
    AIRVector<f32>			m_vtKeyFrameTime;	//关键帧时间点

    u8   m_nVertexAnimationMask;
};
