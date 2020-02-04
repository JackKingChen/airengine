/********************************************************************
	created:	2009/10/06
	created:	6:10:2009   11:37
	filename: 	d:\My Codes\3DGraphic\Engine\src\Scene\AIRAnimation.h
	file path:	d:\My Codes\3DGraphic\Engine\src\Scene
	file base:	AIRAnimation
	file ext:	h
	author:		liangairan
	
	purpose:	���������࣬���ڹ�������
*********************************************************************/

#pragma once

#include "IReference.h"
//#include "AIRSkeleton.h"
//#include "MyVector.h"
#include "DataStream.h"
#include "SceneNode.h"

//�ؼ�֡����
typedef struct tagFrameData 
{
    Vector3Df  vPosition;    //��Ը��׵�λ��
    Quaternion qOrient;      //��Ը��׵���ת
}FRAMEDATA, *LPFRAMEDATA;


class EngineExport CKeyFrame// : public EngineAllocator(CKeyFrame)
{
	EngineAllocator(CKeyFrame)

public:
	CKeyFrame() : m_scale(Vector3Df::UNIT_SCALE), m_translate(Vector3Df::ZERO), m_rotation(Quaternion::IDENEITY) {}
    virtual ~CKeyFrame() {}

	// ����λ��
	virtual void SetTranslate(const Vector3Df& trans)
	{
		m_translate = trans;
	}

	// ���λ��
	const Vector3Df& GetTranslate(void) const
	{
		return m_translate;
	}

	// ��������
	virtual void SetScale(const Vector3Df& scale)
	{
		m_scale = scale;
	}

	// �������
	virtual const Vector3Df& GetScale(void) const
	{
		return m_scale;
	}

	// ������ת
	virtual void SetRotation(const Quaternion& rot)
	{
		m_rotation = rot;
	}

	// �����ת
	virtual const Quaternion& GetRotation(void) const
	{
		return m_rotation;
	}

public:
    Vector3Df	m_translate;	//�����һ�ؼ�֡�ı任
    Quaternion	m_rotation;		//�����һ�ؼ�֡����ת
    Vector3Df	m_scale;		//�����һ�ؼ�֡������
};

class CModelInstance;

class EngineExport CAIRAnimation// : public EngineAllocator(CAIRAnimation)
{
	DECLARE_ROOT_RTTI;
	EngineAllocator(CAIRAnimation)
public:
    CAIRAnimation();
    virtual ~CAIRAnimation();

    // ���ļ��м���
    //bool LoadFromFileData(/*FILE* fp, char* buff*/CDataStream* pStream, CAIRSkeletonRes* pBaseSkeleton);

    //void Update(float fTime);

    //virtual s32  Release();

    // �����������
    virtual void Clear() = 0;

    const AIRString& GetName() const
    {
        return m_strName;
    }

    void SetName(const char* szName)
    {
        m_strName = szName;
    }

	// ���֡������
    int GetFramesNum() const { return m_nFrames; }

    //���֡��
    float GetFrameRate() const { return m_fFrameRate; }


    //��ö����Ĳ���ʱ��
    float GetTimeRange() const
    {
        return m_fTimeRange;
    }

    //���ö���ʱ��
    virtual void SetTimeRange(float fTime)
    {
        m_fTimeRange = fTime;
    }

    //����֡�٣�ÿ��Ŷ���֡��
    virtual void SetFrameRate(float fFrameRate);

    //ɾ��֡���ݣ��ڹ��ɶ������õ�
	virtual void RemoveAllFrames(bool bDelete = false) {}

	virtual void AddKeyFrameTime(float fTime) {}

	virtual void Apply(float timePos, float weight, float scale) {}

    //���õ�ģ����
	virtual void Apply(CModelInstance* pModelInst, float fTimePos, float fWeight) {}

    //���õ������
	virtual void ApplyToNode(CSceneNode* pNode, int nBoneIndex, float fTimePos, float fWeight) {}

    //��ϵ�ģ��
	virtual void Blend(CModelInstance* pModelInst, float fTimePos, float fWeight) {}

    //��ϵ������
	virtual void BlendToNode(CSceneNode* pNode, int nBoneIndex, float fTimePos, float fWeight) {}

    //��ùؼ��ǣ��������ǵ�ʱ������ֻ���ڹ���������
    //virtual float GetKeyFramesAtTime(float fTimePos, CAIRSkeleton** pKeyFrame1, CAIRSkeleton** pKeyFrame2)
    //{
    //    return 0;
    //}

protected:
    //���ĳһ���ǵĲ�ֵ��Ĺؼ�֡����
    //virtual void GetInterpolatedKeyFrame(float fTimePos, int nBoneIndex, CKeyFrame& keyFrame) = 0;

protected:
    
    u16							m_nFrames;			//�ö����Ĺؼ�֡��
    f32						m_fFrameRate;		//֡�٣�ÿ�����֡
    float						m_fTimeRange;		//ʱ�䵥λ��
    AIRString					m_strName;			//��������
private:

    
    //MyVector<CAIRAnibone*> m_vtAniBones;
};
