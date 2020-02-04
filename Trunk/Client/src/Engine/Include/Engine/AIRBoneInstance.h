/********************************************************************
	created:	2010/01/07
	created:	7:1:2010   18:02
	filename: 	d:\My Codes\client\Current\Engine\Engine\Include\AIRBoneInstance.h
	file path:	d:\My Codes\client\Current\Engine\Engine\Include
	file base:	AIRBoneInstance
	file ext:	h
	author:		liangairan
	
	purpose:	用于挂武器的骨骼实例
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

    //计算最终的变换
    //virtual void CaculateFinalTranslate(CAIRBone* pCurFrameBone, CAIRBone* pNextFrameBone, float fInterpolate);

    //取得最终的顶点变换
    virtual void GetOffsetTranform(Matrix4f& matOffset); 

    //设置初始状态
    virtual void SetInitState();

    //重置状态
    virtual void Reset();

    virtual void Update() 
    {
        //UpdateBounds();
    }

    int GetParentBoneID() const
    {
        return m_nParentBoneID;
    }

    //获得世界矩阵
    virtual const Matrix4f& GetFullTransform() const;

    //挂一个渲染物上去节点上
    virtual void AttachRenderable(Renderable* pRenderable);

    //取消挂渲染物
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
    Vector3Df    m_vFinalTranslate;   //最终影响顶点的平移
    Quaternion   m_qFinalRotation;    //最终影响顶点的旋转
    Vector3Df    m_vFinalScale;       //最终影响顶点的缩放

protected:
    //原始骨骼中相对父亲的位置
    Vector3Df m_InitPosition;

    //原始骨骼中相对父亲的旋转
    Quaternion m_InitRotation;

    //原始骨骼中相对父亲的缩放
    Vector3Df m_InitScale;

    //骨的编号
    int       m_nBoneID;  
    //父亲骨的编号
    int       m_nParentBoneID;

    Vector3Df m_vTranslateBone;   //模型空间中的位置
    Quaternion m_qRotationBone;   //模型空间中的旋转
    
private:
    CModelInstance*  m_pOwner;
};

