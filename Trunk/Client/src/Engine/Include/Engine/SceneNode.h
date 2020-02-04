/********************************************************************
	created:	2009/10/21
	created:	21:10:2009   11:17
	filename: 	d:\My Codes\3DGraphic\Include\SceneNode.h
	file path:	d:\My Codes\3DGraphic\Include
	file base:	SceneNode
	file ext:	h
	author:		liangairan
	
	purpose:	场景中的一个节点，只放静态物体，控制渲染物在世界中的方位
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
	//节点关系
	enum NodeRelation
	{
		NR_LParentRChild, //左节点是右节点的父节点
		NR_LChildRParent, //左节点是有节点的子节点
		NR_LEqualR,  //左节点和右节点一样
		NR_LNoneRelateR	 //左节点和有节点不存在关系
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

    //旋转结点
    //@axis    旋转轴
    //@fRadian 弧度
    virtual void Rotate(const Vector3Df& axis, float fRadian, TransformSpace space = TS_LOCAL);

    //旋转结点
    //@qRotate 旋转的四元数
    virtual void Rotate(const Quaternion& qRotate, TransformSpace space = TS_LOCAL);

    //平移
    //@vPosition 平移向量
    virtual void Translate(const Vector3Df& vPosition, TransformSpace space = TS_PARENT);

    //缩放
    //@vScale    缩放向量
    virtual void Scale(const Vector3Df& vScale);

    //设置结点的方位
    //@qRotate 方位
    virtual void SetRotation(const Quaternion& qRotate);

    //设置位置
    //@vPosition 
    virtual void SetPosition(const Vector3Df& vPosition);

    virtual void SetPosition(float x, float y, float z);

    //设置缩放
    //@vScale 缩放比例
    virtual void SetScale(const Vector3Df& vScale);

    //设置世界位置
    virtual void SetDerivedPosition(const Vector3Df& position);

    //设置世界旋转
    virtual void SetDerivedRotation(const Quaternion& rotation);

    //把世界坐标转到该结点的
    virtual Vector3Df ConvertWorldToLocalPosition(const Vector3Df& worldPos);

    //把本地坐标转成世界坐标
    virtual Vector3Df ConvertLocalToWorldPosition(const Vector3Df& localPos);

    //把世界旋转变换到该结点的本地旋转
    virtual Quaternion ConvertWorldToLocalOrientation(const Quaternion& worldOrient);

    //把本地旋转变换到世界旋转
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

    //挂一个渲染物上去节点上
    virtual void AttachRenderable(Renderable* pRenderable);

    //取消挂渲染物
    virtual Renderable* DetachRenderable(Renderable* pRenderable);

	//virtual Renderable* DetachRenderable(const AIRString& strName);

    //取消所有渲染物
    virtual void DetachAllRenderable();

    //virtual Renderable* GetAttachRenderable(const AIRString& strName) const;

    //virtual Renderable* GetAttachRenderable() const
    //{
    //    return m_pRenderable;
    //}

    //绕Z轴旋转
    //@fRadian 旋转弧度
    virtual void Roll(const float fRadian, TransformSpace relativeTo = TS_LOCAL);

    //绕X轴旋转
    virtual void Pitch(const float fRadian, TransformSpace relativeTo = TS_LOCAL);

    //绕Y轴旋转
    virtual void Yaw(const float fRadian, TransformSpace relativeTo = TS_LOCAL);

    //获得世界矩阵变换
    virtual void MakeWorldTransform(/*Matrix4f* pMatWorld*/) const;

    //获得世界矩阵
    virtual const Matrix4f& GetFullTransform() const;

    //获得AABB
    virtual const AABBBox3Df& GetWorldBoundingBox() const;
    

    //这里的更新只更新位置和逻辑等，和Renderable的Update不同，Renderable的是渲染前Update渲染数据
    //这里是场景Update的时候Update，只更新世界坐标中的位置，boundingbox等
    virtual void Update();

    //是否在可视范围内
    virtual bool IsVisable(const CCamera* pCamera) const;

    //获得结点与摄像机的距离
    float GetDistanceFromCamera(CCamera* pCamera)
    {
        Vector3Df vCameraPos;
        pCamera->GetPosition(vCameraPos);
        return (m_worldBox.GetCenter() - vCameraPos).GetLength();
    }

    //获得结点名字
    virtual const AIRString& GetName() const
    {
        return m_strName;
    }

    //设置名字
    void SetName(const char* szName);

    void SetName(const AIRString& strName);

    CSceneNode* GetParent()
    {
        return m_pParent;
    }

    //创建子结点
    virtual CSceneNode* CreateChild(const AIRString& strName, bool bMovable = false, const Vector3Df& position = Vector3Df::ZERO, const Quaternion& rotate = Quaternion(0, 0, 0, 1), 
        const Vector3Df& scale = Vector3Df::UNIT_SCALE);

    //添加子结点
    virtual void AddChild(CSceneNode* pNode);

    //删除子结点
    virtual CSceneNode* RemoveChild(const AIRString& strName);

    virtual CSceneNode* RemoveChild(CSceneNode* pNode);

    //通过名字获得子结点
    virtual CSceneNode* GetChild(const AIRString& strName) const;

    

    virtual unsigned short GetChildrenNum();

    //从父亲update
    virtual void UpdateFromParent();

    //清除所有子结点
    virtual void RemoveAllChildren();

    //获得可视范围的renderable
    virtual void GetVisableRenderables(CCamera* pCamera, CRenderQueue* pRenderQueue);

	//获取与aabbTest向碰撞的渲染物
	virtual void GetCollisionRenderables(const AABBBox3Df& aabbTest, AIRList<Renderable*>& renderList);

    //把attach的renderable添加到渲染队列中
    virtual void AddAttachObjToRenderQueue(CRenderQueue* pRenderQueue);

    //vOrg，射线起点
    //vEnd，射线上的一点
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

    //更新到渲染队列
    void UpdateToRenderQueue(CCamera* pCamera, CRenderQueue* pQueue );

    virtual void FillRenderContext(CCamera* pCamera, RenderContextQueue* pQueue, eRenderFlow flow);

    //查找影响它的光照
    //@destLists  输出队列
    //@fRaduis    影响半径
    virtual void FindLights(LightList& destLists, float fRaduis);

    //添加世界aabb
    //void AddWorldBoundingBox(const AABBBox3Df& worldBox);

    void SetIsUpdatePerFrame(bool bUpdate)
    {
        m_bIsUpdatePerFrame = bUpdate;
    }

    virtual size_t GetAttachRenderablesNum() const
    {
        return m_lstRenderable.size();
    }

    //设置父结点
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

    //获得阻碍物
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
    //更新boundingbox
    virtual void UpdateBounds();

    //通知父亲更新boundingbox
    virtual void NotifyParentUpdateBounds();
protected:
    //父结点
    CSceneNode* m_pParent;

    //注意：所有这些都是相对于世界坐标，暂时的设计是这样，以后可能有子节点
    //相对父亲的位置，没有父亲就是世界位置
    Vector3Df  m_position;  
    //相对父亲的缩放，没有父亲就是相对世界坐标的
    Vector3Df  m_vScale;
    //相对父亲的旋转，没有父亲就是相对世界的
    Quaternion m_rotation;

    //继承所有父亲的位置，即世界位置
    Vector3Df m_DerivedPosition;

    //继承所有父亲的旋转
    Quaternion m_DerivedRotation;

    //继承所有父亲的缩放
    Vector3Df m_DerivedScale;

    //下面的一般是用在骨骼动画上
    
    
    //可渲染物体
    RenderableList     m_lstRenderable;
    //Renderable* m_pRenderable;

    mutable Matrix4f m_matTransform;   //最终的世界矩阵变换

    bool    m_bNeedUpdate;     //是否需要重新计算矩阵

    AABBBox3Df m_worldBox;     //世界矩阵

    AIRString  m_strName;    //结点名字

    ChildNode_Map  m_mapChildren;   //子结点表

    mutable bool         m_bTransform;    //是否已经变换了

    ISceneManager*       m_pCreator;      //结点创建器

    u32               m_dwRayQueryMask; //射线查询掩码(只有射线查询器的查询掩码与场景节点的查询掩码做"与"运算不为0,该节点才会被选中,默认是不被选中的)

    bool                m_bIsUpdatePerFrame;   //是否每帧更新

    Obstruct            m_obstruct;   //阻碍物
protected:
	/// The position to use as a base for keyframe animation
	Vector3Df	m_vInitialPosition;
	/// The orientation to use as a base for keyframe animation
	Quaternion	m_qInitialOrientation;
	/// The scale to use as a base for keyframe animation
	Vector3Df	m_vInitialScale;

    bool              m_bMovable;    //是否移动结点

	AIRString			m_szType;

    f32   m_fSphereRadius;            //球半径

    u32 m_nCullFlag;
private:
};
