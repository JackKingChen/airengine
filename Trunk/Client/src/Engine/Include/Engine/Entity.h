#pragma once
#include "EngineConfig.h"
#include "Common.h"
#include "AABBBox3D.h"
#include "Quaternion.h"
#include "RTTI.h"
#include "EngineDefination.h"

class COctree;
class CCamera;

class EngineExport Entity
{
	EngineAllocator(Entity)
		DECLARE_ROOT_RTTI;
public:
	Entity();
	virtual ~Entity();

	inline const Vector3Df& GetLocalPosition() const;

	inline const Quaternion& GetLocalRotatioin() const;

	inline const Vector3Df& GetLocalScale() const;

	inline void SetLocalPosition(const Vector3Df& position);

	inline void SetLocalRotation(const Quaternion& rotate);

	inline void SetLocalScale(const Vector3Df& scale);

	inline void SetWorldPosition(const Vector3Df& position);

	inline void SetWorldRotation(const Quaternion& rotate);

	inline void SetWorldScale(const Vector3Df& scale);

	Entity* GetParent()
	{
		return m_pParent;
	}

	virtual void Update();

	bool IsActive() const
	{
		return m_bActive;
	}

	void SetActive(bool bActive);

	u32 GetCullFlag() const
	{
		return m_nCullFlag;
	}

	void SetCullFlag(u32 nFlag)
	{
		m_nCullFlag = nFlag;
	}

	const AABBBox3D& GetWorldBox() const
	{
		return m_worldBox;
	}

	COctree* GetOctree() 
	{
		return m_pAttachedTree;
	}

	void SetAttachedTree(COctree* pTree)
	{
		m_pAttachedTree = pTree;
	}

	const AABBBox3D& GetWorldBoundingBox() const
	{
		return m_worldBox;
	}

	void Clear();

	//填充到渲染队列
	void FillRenderContext(CCamera* pCamera, RenderContextQueue* pQueue, eRenderFlow flow);

	IMaterial* GetMaterial()
	{
		return m_pMaterial;
	}
protected:
	//相对父亲的位置，没有父亲就是世界位置
	Vector3Df  m_position;
	//相对父亲的缩放，没有父亲就是相对世界坐标的
	Vector3Df  m_scale;
	//相对父亲的旋转，没有父亲就是相对世界的
	Quaternion m_rotation;

	//继承所有父亲的位置，即世界位置
	Vector3Df m_WorldPosition;

	//继承所有父亲的旋转
	Quaternion m_WorldRotation;

	//继承所有父亲的缩放
	Vector3Df m_WorldScale;

	AABBBox3D m_worldBox;


	mutable Matrix4f m_matTransform;   //最终的世界矩阵变换

	bool m_bNeedUpdate;

	inline void NeedUpdate();

	void UpdateWorldTransform();

	Entity* m_pParent;

	COctree* m_pAttachedTree;   //挂在上面的树
	
	bool m_bActive;

	void _GetPrimitive();
private:
	void ResetLocalTransform();

	u32 m_nCullFlag;

	void UpdateAttachedTree();

	IMaterial* m_pMaterial;
};

