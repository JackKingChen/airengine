/********************************************************************
	created:	2010/01/07
	created:	7:1:2010   18:02
	filename: 	d:\My Codes\client\Current\Engine\Engine\Include\AIRBoneInstance.h
	file path:	d:\My Codes\client\Current\Engine\Engine\Include
	file base:	AIRBoneInstance
	file ext:	h
	author:		liangairan
	
	purpose:	���ڹ������Ĺ���ʵ��
*********************************************************************/

#pragma once 
#include "SceneNode.h"
#include "AIRBone.h"
//#include "AIRSkeletonRes.h"

class CModelInstance;

class EngineExport CAIRBoneInstance : public CSceneNode
{
public:
    //CAIRBoneInstance(const AIRString& strName);
    CAIRBoneInstance(int nBoneID, CModelInstance* pModelInstance);

    virtual ~CAIRBoneInstance();

    virtual void SetInitPosition(const Vector3Df& position);

    virtual void SetInitRotation(const Quaternion& rotation);

    virtual void SetInitScale(const Vector3Df& scale);

    virtual const Vector3Df& GetInitPosition() const
    {
        return m_InitPosition;
    }

    virtual const Quaternion& GetInitRotation() const
    {
        return m_InitRotation;
    }

    virtual const Vector3Df& GetInitScale() const
    {
        return m_InitScale;
    }

    //�������յı任
    //virtual void CaculateFinalTranslate(CAIRBone* pCurFrameBone, CAIRBone* pNextFrameBone, float fInterpolate);

    //ȡ�����յĶ���任
    virtual void GetOffsetTranform(Matrix4f& matOffset); 

    //���ó�ʼ״̬
    virtual void SetInitState();

    //����״̬
    virtual void Reset();

    virtual void Update() 
    {
        //UpdateBounds();
    }

    int GetParentBoneID() const
    {
        return m_nParentBoneID;
    }

    //����������
    virtual const Matrix4f& GetFullTransform() const;

    //��һ����Ⱦ����ȥ�ڵ���
    virtual void AttachRenderable(Renderable* pRenderable);

    //ȡ������Ⱦ��
    virtual Renderable* DetachRenderable(Renderable* pRenderable);

    //virtual Renderable* DetachRenderable(const AIRString& strName);

    virtual void DetachAllRenderable();

    virtual const Vector3Df& GetFinalPosition() const
    {
        return m_vFinalTranslate;
    }

    virtual const Quaternion& GetFinalRotation() const
    {
        return m_qFinalRotation;
    }

    virtual const Vector3Df& GetFinalScale() const
    {
        return m_vFinalScale;
    }

    virtual bool IsBone() const
    {
        return true;
    }
public:
    Vector3Df    m_vFinalTranslate;   //����Ӱ�춥���ƽ��
    Quaternion   m_qFinalRotation;    //����Ӱ�춥�����ת
    Vector3Df    m_vFinalScale;       //����Ӱ�춥�������

protected:
    //ԭʼ��������Ը��׵�λ��
    Vector3Df m_InitPosition;

    //ԭʼ��������Ը��׵���ת
    Quaternion m_InitRotation;

    //ԭʼ��������Ը��׵�����
    Vector3Df m_InitScale;

    //�ǵı��
    int       m_nBoneID;  
    //���׹ǵı��
    int       m_nParentBoneID;

    Vector3Df m_vTranslateBone;   //ģ�Ϳռ��е�λ��
    Quaternion m_qRotationBone;   //ģ�Ϳռ��е���ת
    
private:
    CModelInstance*  m_pOwner;
};

