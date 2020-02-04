#include "stdafx.h"
#include "AIRBoneInstance.h"
#include "ModelInstance.h"



/*
CAIRBoneInstance::CAIRBoneInstance(const AIRString& strName) : m_nParentBoneID(-1)
{
    m_strName = strName;
    m_InitScale = m_vScale = m_DerivedScale = Vector3Df::UNIT_SCALE;
    m_worldBox.SetNull(true);
	m_szType = "boneinstance";
}
*/

CAIRBoneInstance::CAIRBoneInstance(int nBoneID, CModelInstance* pModelInstance) : m_nParentBoneID(-1)
, m_pOwner(pModelInstance)
{
    AIRModel* pModel = pModelInstance->GetModel();
	m_szType = "boneinstance";
    LPAIRBONE pBone = pModel->GetBone(nBoneID);
    m_nBoneID = nBoneID;
    m_nParentBoneID = pBone->nParentID;
    //m_strName = pBone->strName;
    m_strName = pBone->szName;
    m_vTranslateBone = pBone->vPosition;
    m_qRotationBone = pBone->qRotation;

    Matrix4f matObject;
    Quaternion rotation = pBone->qRotation;
    //rotation.Inverse();
    rotation.GetMatrix(matObject);
    matObject.SetTranslation(pBone->vPosition);
    //matObject.SetScale(Vector3Df::UNIT_SCALE);

    matObject.Inverse();
    
    m_DerivedPosition = matObject.GetTranslation();//-pBone->vPosition;
    m_DerivedRotation.FromMatrixEx(matObject);
    
    //m_InitRotation.Inverse();
    m_InitScale = m_vScale = m_DerivedScale = m_vFinalScale = Vector3Df::UNIT_SCALE;
    
    m_worldBox.SetNull(true);
}

CAIRBoneInstance::~CAIRBoneInstance()
{

}

void CAIRBoneInstance::SetInitPosition(const Vector3Df& position)
{
    m_InitPosition = position;
}

void CAIRBoneInstance::SetInitRotation(const Quaternion& rotation)
{
    m_InitRotation = rotation;
}

void CAIRBoneInstance::SetInitScale(const Vector3Df& scale)
{
    m_InitScale = scale;
}



void CAIRBoneInstance::GetOffsetTranform(Matrix4f& matOffset)
{
    UpdateFromParent();
    //Vector3Df offsetScale;
    m_vFinalScale = m_DerivedScale;
    //m_vFinalTranslate = m_DerivedRotation * (m_vFinalScale * m_vTranslateBone);
	QuaternionMultiplyVector( m_DerivedRotation, (m_vFinalScale * m_vTranslateBone), m_vFinalTranslate );
    m_vFinalTranslate.x += m_DerivedPosition.x;
	m_vFinalTranslate.y += m_DerivedPosition.y;
	m_vFinalTranslate.z += m_DerivedPosition.z;
    //m_qFinalRotation = m_DerivedRotation * m_qRotationBone;
	MultiplyQuaternion( m_DerivedRotation, m_qRotationBone, m_qFinalRotation );

    Matrix4f rot4x4, scale4x4;
    m_qFinalRotation.GetMatrix(rot4x4);
    scale4x4.Identity();
    //scale4x4(0, 0) = m_vFinalScale.x;
    //scale4x4(1, 1) = m_vFinalScale.y;
    //scale4x4(2, 2) = m_vFinalScale.z;
	scale4x4[0] = m_vFinalScale.x;
	scale4x4[5] = m_vFinalScale.y;
	scale4x4[10] = m_vFinalScale.z;

    // Set up final matrix with scale, rotation and translation
    //matOffset = rot4x4 * scale4x4;
	MultiplyMatrix4( rot4x4, scale4x4, matOffset );
    matOffset.SetTranslation(m_vFinalTranslate);

    // No projection term
    //matOffset(3, 3) = 1;
    //matOffset(0, 3) = 0; 
    //matOffset(1, 3) = 0; 
    //matOffset(2, 3) = 0; 
	matOffset[3] = 0; 
	matOffset[7] = 0; 
	matOffset[11] = 0; 
	matOffset[15] = 1;

    m_bTransform = false;  //这里要为false，不然挂在骨上的粒子会获取不了fulltransform，liangairan 2010-10-14

    //m_worldBox.MaxEdge = Vector3Df::ZERO;
    //m_worldBox.MinEdge = Vector3Df::ZERO;
    //m_worldBox.transformAffine( GetFullTransform() );

    //liangairan把下面这段代码开启，因为AirBoneInstance是每帧不需要用m_bNeedUpdate来判断的，
    //但如果m_mapChildren里面的是普通的CSceneNode，那么这个SceneNode就不会被update了
    for (ChildNode_Iter it = m_mapChildren.begin(); it != m_mapChildren.end(); ++it)
    {
        CSceneNode* pChild = it->second;
        pChild->NeedUpdate();
        pChild->Update();
    }

}

void CAIRBoneInstance::SetInitState()
{
    ////首先设置相对父亲的平移和旋转
    ////for test----
    //Quaternion rotateParentAbs(0, -0.707108, 0, 0.707106);
    //Quaternion rotate2Parent(0.455021, 0.403566, 0.513285, 0.605499);
    //Quaternion rotateChildAbs = rotateParentAbs * rotate2Parent;
    //rotate2Parent = rotateParentAbs.Inverse() * rotateChildAbs;
    ////------------
    if (m_pParent)
    {
        m_pParent->UpdateFromParent();
        const Vector3Df& posParent = m_pParent->GetDerivedPosition();
        const Quaternion& rotateParent = m_pParent->GetDerivedRotation();
        const Vector3Df& scaleParent = m_pParent->GetDerivedScale();

		//m_position = m_DerivedPosition - posParent;
		//Quaternion rotationInverse = rotateParent;
		//rotationInverse.Inverse();
		//m_rotation = rotationInverse * m_DerivedRotation;
		//m_vScale *= m_pParent->GetDerivedScale();

		m_position.x = m_DerivedPosition.x - posParent.x;
		m_position.y = m_DerivedPosition.y - posParent.y;
		m_position.z = m_DerivedPosition.z - posParent.z;
		Quaternion rotationInverse;
		QuaternionInverse( rotateParent, rotationInverse );
		MultiplyQuaternion( m_DerivedRotation, rotationInverse, m_rotation );
		//m_vScale *= m_pParent->GetDerivedScale();
		m_vScale.x *= m_pParent->GetDerivedScale().x;
		m_vScale.y *= m_pParent->GetDerivedScale().y;
		m_vScale.z *= m_pParent->GetDerivedScale().z;
    }
    else
    {
        //m_DerivedPosition = m_InitPosition;
        //m_DerivedRotation = m_InitRotation;
        m_position = m_DerivedPosition;
        m_rotation = m_DerivedRotation;
        m_vScale = m_DerivedScale;
    }

    m_InitPosition = m_position;
    m_InitRotation = m_rotation;
    m_InitScale = m_vScale;

    //UpdateFromParent();
}

void CAIRBoneInstance::Reset()
{
    m_position = m_InitPosition;
    m_rotation = m_InitRotation;
    m_vScale = m_InitScale;

    NeedUpdate();
}

const Matrix4f& CAIRBoneInstance::GetFullTransform() const
{
    return CSceneNode::GetFullTransform();
}

void CAIRBoneInstance::AttachRenderable(Renderable* pRenderable)
{
    CSceneNode::AttachRenderable(pRenderable);
    m_pOwner->AddBoneAttachedRenderable(pRenderable);
}

Renderable* CAIRBoneInstance::DetachRenderable(Renderable* pRenderable)
{
    Renderable* pRend = CSceneNode::DetachRenderable(pRenderable);
    m_pOwner->RemoveBoneAttachedRenderable(pRenderable);
    return pRenderable;
}
/*
Renderable* CAIRBoneInstance::DetachRenderable(const AIRString& strName)
{
    Renderable* pRenderable = CSceneNode::DetachRenderable(strName);
    m_pOwner->RemoveBoneAttachedRenderable(pRenderable);
    return pRenderable;
}
*/

void CAIRBoneInstance::DetachAllRenderable()
{
    for (RenderableListIter it = m_lstRenderable.begin(); it != m_lstRenderable.end(); ++it)
    {
        Renderable* pRenderable = *it;
        pRenderable->SetAttachedNode(0);
        m_pOwner->RemoveBoneAttachedRenderable(pRenderable);
    }

    m_lstRenderable.clear();

    for (ChildNode_Iter it = m_mapChildren.begin(); it != m_mapChildren.end(); ++it)
    {
        CSceneNode* pChild = it->second;
        pChild->DetachAllRenderable();
    }
}
