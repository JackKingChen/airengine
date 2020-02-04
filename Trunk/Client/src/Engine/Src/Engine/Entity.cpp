#include "Entity.h"
#include "RenderContext.h"

IMPLEMENT_ROOT_RTTI(Entity);

Entity::Entity() : m_bNeedUpdate(false)
, m_pParent(NULL), m_pAttachedTree(NULL)
, m_pMaterial(NULL)
{

}

Entity::~Entity()
{

}

inline const Vector3Df& Entity::GetLocalPosition() const
{
	return m_position;
}

inline const Quaternion& Entity::GetLocalRotatioin() const
{
	return m_rotation;
}

inline const Vector3Df& Entity::GetLocalScale() const
{
	return m_scale;
}

inline void Entity::SetLocalPosition(const Vector3Df& position)
{
	if (m_position != position)
	{
		m_position = position;
		NeedUpdate();
	}
}

inline void Entity::SetLocalRotation(const Quaternion& rotate)
{
	if (rotate != m_rotation)
	{
		m_rotation = rotate;
		NeedUpdate();
	}
}

inline void Entity::SetLocalScale(const Vector3Df& scale)
{
	if (m_scale != scale)
	{
		m_scale = scale;
		NeedUpdate();
	}
}

inline void Entity::SetWorldPosition(const Vector3Df& position)
{
	if (m_WorldPosition != position)
	{
		m_WorldPosition = position;

		ResetLocalTransform();
	}
}

inline void Entity::SetWorldRotation(const Quaternion& rotate)
{
	if (rotate != m_WorldRotation)
	{
		m_WorldRotation = rotate;

		ResetLocalTransform();
	}
}

inline void Entity::SetWorldScale(const Vector3Df& scale)
{
	if (m_WorldScale != scale)
	{
		m_WorldScale = scale;
		ResetLocalTransform();
	}
}

inline void Entity::NeedUpdate()
{
	m_bNeedUpdate = true;
}

void Entity::Update()
{
}

void Entity::SetActive(bool bActive)
{
	m_bActive = bActive;
}

void Entity::UpdateWorldTransform()
{
	if (m_pParent)
	{
		m_pParent->m_matTransform.TransformCoord(m_position, m_WorldPosition);
		m_pParent->m_matTransform.TransformCoord(m_scale, m_WorldScale);
		Quaternion parentRotation;
		parentRotation.FromMatrix(m_pParent->m_matTransform);
		m_WorldRotation = parentRotation * m_rotation;
	}
}

void Entity::ResetLocalTransform()
{
	if (m_pParent)
	{
		Matrix4f matWorld;
		matWorld.MakeTransform(m_WorldPosition, m_WorldScale, m_WorldRotation);
		//Matrix4f matWorldParent = m_pParent->m_matTransform;
		matWorld.Inverse();
		Matrix4f matLocalToParent = m_pParent->m_matTransform * matWorld;
		m_position = matLocalToParent.GetTranslation();
		m_rotation.FromMatrix(matLocalToParent);
		m_scale = matLocalToParent.GetScale();
	}
	else
	{
		m_position = m_WorldPosition;
		m_rotation = m_WorldRotation;
		m_scale = m_WorldScale;
	}
}

void Entity::Clear()
{

}

void Entity::_GetPrimitive()
{

}

void Entity::FillRenderContext(CCamera* pCamera, RenderContextQueue* pQueue, eRenderFlow flow)
{
	_GetPrimitive();

	pQueue->DrawEntityToContext(this, pCamera, flow);
}
