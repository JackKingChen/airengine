/********************************************************************
	created:	2009/10/06
	created:	6:10:2009   11:37
	filename: 	d:\My Codes\3DGraphic\Engine\src\Scene\AIRSkeletonAnimation.h
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
#include "AIRAnimation.h"
#include "EngineResource.h"
class CAIRBone;

//�ؼ�֡����
/*
typedef struct tagFrameData 
{
    Vector3Df  vPosition;    //��Ը��׵�λ��
    Quaternion qOrient;      //��Ը��׵���ת
}FRAMEDATA, *LPFRAMEDATA;


class CAIRSkeletonRes;
class CModelInstance;

class CKeyFrame
{
public:
    CKeyFrame() : m_scale(Vector3Df::UNIT_SCALE) {}
    virtual ~CKeyFrame() {}

public:
    Vector3Df	m_translate;	//�����һ�ؼ�֡�ı任
    Quaternion	m_rotation;		//�����һ�ؼ�֡����ת
    Vector3Df	m_scale;		//�����һ�ؼ�֡������
};
*/

class AIRModel;

class EngineExport CAIRSkeletonAnimation : public CAIRAnimation, public Resource
{
	DECLARE_RTTI;
	EngineAllocator(CAIRSkeletonAnimation)
public:
    CAIRSkeletonAnimation(AIRModel* pModel);
    virtual ~CAIRSkeletonAnimation();

    // ���ļ��м���
    bool LoadFromFileData(/*FILE* fp, char* buff*/CDataStream* pStream, AIRModel* pBaseModel);

    //�Ӷ������ļ���ȡ
    bool LoadFromFileDataBinary(CDataStream* pStream, AIRModel* pBaseModel);

    void SaveToFileStream(std::fstream& fout, AIRModel* pBaseModel) const;

    // �����������
    virtual void Clear();

    //const AIRString& GetName() const
    //{
    //    return m_strName;
    //}

    //void SetName(const char* szName)
    //{
    //    m_strName = szName;
    //}

	//************************************
	// Method:    BuildFrameSkeletons
	// FullName:  CAIRSkeletonAnimation::BuildFrameSkeleton
	// Access:    public 
	// Returns:   void
	// Qualifier: ����ÿһ֡�Ĺ���
	// Parameter: CAIRSkeleton * pBaseSkeleton   ��������
	// Parameter: int nFrame                     ֡���������ڼ�֡
	// Parameter: AIRVector<FRAMEDATA> & vtFrameData  һ֡�������йǵ�λ�ú���ת��Ϣ
	//************************************
	void BuildFrameSkeleton(int nBoneIndex, AIRVector<FRAMEDATA>& vtFrameData);

	virtual bool Cache();

    //��������ÿ֡������
    void CaculateFrameData();

    //��ӹ�����֡
    //virtual void AddFrameSkeleton(CAIRSkeleton* pFrameSkel);

    //���ö���ʱ��
    virtual void SetTimeRange(float fTime);

    //����֡�٣�ÿ��Ŷ���֡��
    virtual void SetFrameRate(float fFrameRate);

    //ɾ��֡���ݣ��ڹ��ɶ������õ�
    virtual void RemoveAllFrames(bool bDelete = false);

    //����ʱ���������ؼ�֡
    //return ���ز�ֵ��ֵ
    //virtual float GetKeyFramesAtTime(float fTimePos, CAIRSkeleton** pKeyFrame1, CAIRSkeleton** pKeyFrame2);

	virtual float GetKeyFramesAtTime(f32 fTimePos, u32& nFrame1, u32& nFrame2);

    virtual void AddKeyFrameTime(float fTime);

    //���õ�ģ����
    virtual void Apply(CModelInstance* pModelInst, float fTimePos, float fWeight);

    //���õ������
    virtual void ApplyToNode(CSceneNode* pNode, int nBoneIndex, float fTimePos, float fWeight);

    //��ϵ�ģ��
    virtual void Blend(CModelInstance* pModelInst, float fTimePos, float fWeight);

    //��ϵ������
    virtual void BlendToNode(CSceneNode* pNode, int nBoneIndex, float fTimePos, float fWeight);

	//CAIRSkeletonRes* GetSkeletonRes()
	//{
	//	return m_pSkeletonRes;
	//}

protected:
    //���ĳһ���ǵĲ�ֵ��Ĺؼ�֡����
    virtual void GetInterpolatedKeyFrame(float fTimePos, int nBoneIndex, CKeyFrame& keyFrame);

protected:
    
    //int							m_nFrames;			//�ö����Ĺؼ�֡��
    //float						m_fFrameRate;		//֡�٣�ÿ�����֡
    //float						m_fTimeRange;		//ʱ�䵥λ��
    //AIRString					m_strName;			//��������
    //AIRVector<CAIRSkeleton*>  m_vtFrameSkeleton;	//ÿһ֡�Ĺ���
    AIRVector<float>			m_vtKeyFrameTime;	//�ؼ�֡ʱ���
    CKeyFrame					m_keyFrameTransform;//�任�Ĺؼ�֡
	//CAIRSkeletonRes* m_pSkeletonRes;
	AIRModel* m_pModelRes;
	AIRVector<CAIRBone*>      m_vtFrameBones;
private:

    
    //MyVector<CAIRAnibone*> m_vtAniBones;
};
