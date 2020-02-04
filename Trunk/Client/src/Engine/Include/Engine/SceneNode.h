/********************************************************************
	created:	2009/10/21
	created:	21:10:2009   11:17
	filename: 	d:\My Codes\3DGraphic\Include\SceneNode.h
	file path:	d:\My Codes\3DGraphic\Include
	file base:	SceneNode
	file ext:	h
	author:		liangairan
	
	purpose:	�����е�һ���ڵ㣬ֻ�ž�̬���壬������Ⱦ���������еķ�λ
*********************************************************************/

#pragma once
#include "EngineConfig.h"
#include "Common.h"
#include "Quaternion.h"
//#include "Renderable.h"
#include "AABBBox3D.h"
#include "Camera.h"
//#include "RenderQueue.h"
#include "Light.h"
#include "Obstruct.h"
#include "IteratorWrappers.h"
//#include "Engine.h"

class ISceneManager;
class RenderContextQueue;


class EngineExport CSceneNode //: public EngineAllocator(CSceneNode)
{
	EngineAllocator(CSceneNode)
public:
    enum TransformSpace
    {
        /// Transform is relative to the local space
        TS_LOCAL,
        /// Transform is relative to the space of the parent node
        TS_PARENT,
        /// Transform is relative to world space
        TS_WORLD
    };
	//�ڵ��ϵ
	enum NodeRelation
	{
		NR_LParentRChild, //��ڵ����ҽڵ�ĸ��ڵ�
		NR_LChildRParent, //��ڵ����нڵ���ӽڵ�
		NR_LEqualR,  //��ڵ���ҽڵ�һ��
		NR_LNoneRelateR	 //��ڵ���нڵ㲻���ڹ�ϵ
	};
    typedef std::map<AIRString, CSceneNode*> ChildNode_Map;
    typedef ChildNode_Map::iterator  ChildNode_Iter;
    typedef AIRList<CLight*> LightList;

	//typedef std::map<AIRString, Renderable*> MAP_RENDERABLE;
    typedef AIRList<Renderable*> RenderableList;
	typedef RenderableList::iterator    RenderableListIter;

public:
    CSceneNode();
    CSceneNode(ISceneManager* pCreator, const AIRString& strName, const Vector3Df& vPosition, const Quaternion& qRotation = Quaternion(0, 0, 0, 1), const Vector3Df& vScale = Vector3Df::UNIT_SCALE);
    CSceneNode(ISceneManager* pCreator, const Vector3Df& vPosition, const Quaternion& qRotation = Quaternion(0, 0, 0, 1), const Vector3Df& vScale = Vector3Df::UNIT_SCALE);
    virtual ~CSceneNode();

	static NodeRelation IsNodesInRelation( CSceneNode* pNodeL, CSceneNode* pNodeR );

    //��ת���
    //@axis    ��ת��
    //@fRadian ����
    virtual void Rotate(const Vector3Df& axis, float fRadian, TransformSpace space = TS_LOCAL);

    //��ת���
    //@qRotate ��ת����Ԫ��
    virtual void Rotate(const Quaternion& qRotate, TransformSpace space = TS_LOCAL);

    //ƽ��
    //@vPosition ƽ������
    virtual void Translate(const Vector3Df& vPosition, TransformSpace space = TS_PARENT);

    //����
    //@vScale    ��������
    virtual void Scale(const Vector3Df& vScale);

    //���ý��ķ�λ
    //@qRotate ��λ
    virtual void SetRotation(const Quaternion& qRotate);

    //����λ��
    //@vPosition 
    virtual void SetPosition(const Vector3Df& vPosition);

    virtual void SetPosition(float x, float y, float z);

    //��������
    //@vScale ���ű���
    virtual void SetScale(const Vector3Df& vScale);

    //��������λ��
    virtual void SetDerivedPosition(const Vector3Df& position);

    //����������ת
    virtual void SetDerivedRotation(const Quaternion& rotation);

    //����������ת���ý���
    virtual Vector3Df ConvertWorldToLocalPosition(const Vector3Df& worldPos);

    //�ѱ�������ת����������
    virtual Vector3Df ConvertLocalToWorldPosition(const Vector3Df& localPos);

    //��������ת�任���ý��ı�����ת
    virtual Quaternion ConvertWorldToLocalOrientation(const Quaternion& worldOrient);

    //�ѱ�����ת�任��������ת
    virtual Quaternion ConvertLocalToWorldOrientation(const Quaternion& localOrient);

    const Vector3Df& GetPosition() const
    {
        return m_position;
    }

    virtual const Vector3Df& GetScale() const
    {
        return m_vScale;
    }

    virtual const Quaternion& GetRotation() const
    {
        return m_rotation;
    }

    virtual const Vector3Df& GetDerivedPosition() const
    {
        return m_DerivedPosition;
    }

    virtual const Quaternion& GetDerivedRotation() const
    {
        return m_DerivedRotation;
    }

    virtual const Vector3Df& GetDerivedScale() const
    {
        return m_DerivedScale;
    }

    //��һ����Ⱦ����ȥ�ڵ���
    virtual void AttachRenderable(Renderable* pRenderable);

    //ȡ������Ⱦ��
    virtual Renderable* DetachRenderable(Renderable* pRenderable);

	//virtual Renderable* DetachRenderable(const AIRString& strName);

    //ȡ��������Ⱦ��
    virtual void DetachAllRenderable();

    //virtual Renderable* GetAttachRenderable(const AIRString& strName) const;

    //virtual Renderable* GetAttachRenderable() const
    //{
    //    return m_pRenderable;
    //}

    //��Z����ת
    //@fRadian ��ת����
    virtual void Roll(const float fRadian, TransformSpace relativeTo = TS_LOCAL);

    //��X����ת
    virtual void Pitch(const float fRadian, TransformSpace relativeTo = TS_LOCAL);

    //��Y����ת
    virtual void Yaw(const float fRadian, TransformSpace relativeTo = TS_LOCAL);

    //����������任
    virtual void MakeWorldTransform(/*Matrix4f* pMatWorld*/) const;

    //����������
    virtual const Matrix4f& GetFullTransform() const;

    //���AABB
    virtual const AABBBox3Df& GetWorldBoundingBox() const;
    

    //����ĸ���ֻ����λ�ú��߼��ȣ���Renderable��Update��ͬ��Renderable������ȾǰUpdate��Ⱦ����
    //�����ǳ���Update��ʱ��Update��ֻ�������������е�λ�ã�boundingbox��
    virtual void Update();

    //�Ƿ��ڿ��ӷ�Χ��
    virtual bool IsVisable(const CCamera* pCamera) const;

    //��ý����������ľ���
    float GetDistanceFromCamera(CCamera* pCamera)
    {
        Vector3Df vCameraPos;
        pCamera->GetPosition(vCameraPos);
        return (m_worldBox.GetCenter() - vCameraPos).GetLength();
    }

    //��ý������
    virtual const AIRString& GetName() const
    {
        return m_strName;
    }

    //��������
    void SetName(const char* szName);

    void SetName(const AIRString& strName);

    CSceneNode* GetParent()
    {
        return m_pParent;
    }

    //�����ӽ��
    virtual CSceneNode* CreateChild(const AIRString& strName, bool bMovable = false, const Vector3Df& position = Vector3Df::ZERO, const Quaternion& rotate = Quaternion(0, 0, 0, 1), 
        const Vector3Df& scale = Vector3Df::UNIT_SCALE);

    //����ӽ��
    virtual void AddChild(CSceneNode* pNode);

    //ɾ���ӽ��
    virtual CSceneNode* RemoveChild(const AIRString& strName);

    virtual CSceneNode* RemoveChild(CSceneNode* pNode);

    //ͨ�����ֻ���ӽ��
    virtual CSceneNode* GetChild(const AIRString& strName) const;

    

    virtual unsigned short GetChildrenNum();

    //�Ӹ���update
    virtual void UpdateFromParent();

    //��������ӽ��
    virtual void RemoveAllChildren();

    //��ÿ��ӷ�Χ��renderable
    virtual void GetVisableRenderables(CCamera* pCamera, CRenderQueue* pRenderQueue);

	//��ȡ��aabbTest����ײ����Ⱦ��
	virtual void GetCollisionRenderables(const AABBBox3Df& aabbTest, AIRList<Renderable*>& renderList);

    //��attach��renderable��ӵ���Ⱦ������
    virtual void AddAttachObjToRenderQueue(CRenderQueue* pRenderQueue);

    //vOrg���������
    //vEnd�������ϵ�һ��
    virtual bool IsPick(const Vector3Df& vOrg, const Vector3Df& vEnd) const;

    virtual void    SetNeedUpdate( bool b )
    {
        m_bNeedUpdate = b;
    }

    virtual void    SetCreator(ISceneManager* pSceneMgr);

    virtual bool    IsIn(const AABBBox3Df& box);

	virtual bool    IsIntersect(const AABBBox3Df& box);

    ISceneManager* GetCreator()
    {
        return m_pCreator;
    }

    void    SetRayQueryMask( u32 mask )
    {
        m_dwRayQueryMask = mask;
    }

    u32 GetRayQueryMask( void )
    {
        return m_dwRayQueryMask;
    }

    void NeedUpdate()
    {
        m_bNeedUpdate = true;
    }

    //���µ���Ⱦ����
    void UpdateToRenderQueue(CCamera* pCamera, CRenderQueue* pQueue );

    virtual void FillRenderContext(CCamera* pCamera, RenderContextQueue* pQueue, eRenderFlow flow);

    //����Ӱ�����Ĺ���
    //@destLists  �������
    //@fRaduis    Ӱ��뾶
    virtual void FindLights(LightList& destLists, float fRaduis);

    //�������aabb
    //void AddWorldBoundingBox(const AABBBox3Df& worldBox);

    void SetIsUpdatePerFrame(bool bUpdate)
    {
        m_bIsUpdatePerFrame = bUpdate;
    }

    virtual size_t GetAttachRenderablesNum() const
    {
        return m_lstRenderable.size();
    }

    //���ø����
    virtual void SetParent(CSceneNode* pParent);


	void SetInitialState(void)
	{
		m_vInitialPosition = m_position;
		m_vInitialScale = m_vScale;
		m_qInitialOrientation = m_rotation;
	}

	void ResetToInitialState(void)
	{
		m_position = m_vInitialPosition;
		m_rotation = m_qInitialOrientation;
		m_vScale = m_vInitialScale;
		NeedUpdate();
	}


    bool IsMovable() const
    {
        return m_bMovable;
    }

    void SetMovable(bool bMovable)
    {
        m_bMovable = bMovable;
    }

    //����谭��
    Obstruct* GetObstruct() 
    {
        m_obstruct.SetNode(this);
        return &m_obstruct;
    }


	ListIterator<CSceneNode::RenderableList> GetMapItorRenderable( void )
	{
		return ListIterator<CSceneNode::RenderableList>( m_lstRenderable );
	}

    MapIterator<CSceneNode::ChildNode_Map> GetChildNodeIterator()
    {
        return MapIterator<CSceneNode::ChildNode_Map>(m_mapChildren);
    }

	const AIRString& GetType( void ) const 
	{
		return m_szType;
	}

	virtual void DestroyAllRenderable( void );

    f32 GetSphereRadius() const
    {
        return m_fSphereRadius;
    }

    void SetInitialRotation(const Quaternion& rotation)
    {
        m_qInitialOrientation = rotation;
    }

    void SetInitialPosition(const Vector3Df& position)
    {
        m_vInitialPosition = position;
    }

    u32 GetCullFlag() const
    {
        return m_nCullFlag;
    }

    void SetCullFlag(u32 nFlag)
    {
        m_nCullFlag = nFlag;
    }

	//bool IsAllRenderablePSG( void );
protected:
    //����boundingbox
    virtual void UpdateBounds();

    //֪ͨ���׸���boundingbox
    virtual void NotifyParentUpdateBounds();
protected:
    //�����
    CSceneNode* m_pParent;

    //ע�⣺������Щ����������������꣬��ʱ��������������Ժ�������ӽڵ�
    //��Ը��׵�λ�ã�û�и��׾�������λ��
    Vector3Df  m_position;  
    //��Ը��׵����ţ�û�и��׾���������������
    Vector3Df  m_vScale;
    //��Ը��׵���ת��û�и��׾�����������
    Quaternion m_rotation;

    //�̳����и��׵�λ�ã�������λ��
    Vector3Df m_DerivedPosition;

    //�̳����и��׵���ת
    Quaternion m_DerivedRotation;

    //�̳����и��׵�����
    Vector3Df m_DerivedScale;

    //�����һ�������ڹ���������
    
    
    //����Ⱦ����
    RenderableList     m_lstRenderable;
    //Renderable* m_pRenderable;

    mutable Matrix4f m_matTransform;   //���յ��������任

    bool    m_bNeedUpdate;     //�Ƿ���Ҫ���¼������

    AABBBox3Df m_worldBox;     //�������

    AIRString  m_strName;    //�������

    ChildNode_Map  m_mapChildren;   //�ӽ���

    mutable bool         m_bTransform;    //�Ƿ��Ѿ��任��

    ISceneManager*       m_pCreator;      //��㴴����

    u32               m_dwRayQueryMask; //���߲�ѯ����(ֻ�����߲�ѯ���Ĳ�ѯ�����볡���ڵ�Ĳ�ѯ������"��"���㲻Ϊ0,�ýڵ�Żᱻѡ��,Ĭ���ǲ���ѡ�е�)

    bool                m_bIsUpdatePerFrame;   //�Ƿ�ÿ֡����

    Obstruct            m_obstruct;   //�谭��
protected:
	/// The position to use as a base for keyframe animation
	Vector3Df	m_vInitialPosition;
	/// The orientation to use as a base for keyframe animation
	Quaternion	m_qInitialOrientation;
	/// The scale to use as a base for keyframe animation
	Vector3Df	m_vInitialScale;

    bool              m_bMovable;    //�Ƿ��ƶ����

	AIRString			m_szType;

    f32   m_fSphereRadius;            //��뾶

    u32 m_nCullFlag;
private:
};
