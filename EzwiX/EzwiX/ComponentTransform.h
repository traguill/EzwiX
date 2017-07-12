#ifndef __COMPONENT_TRANSFORM_H__
#define __COMPONENT_TRANSFORM_H__

#include "Component.h"

#include "MathGeoLib\include\MathGeoLib.h"

class ComponentTransform : public Component
{
public:
	ComponentTransform(ComponentType type, GameObject* game_object, math::float4x4** global_matrix);
	~ComponentTransform();

	void Update();

	void OnInspector(bool debug);

	void SetPosition(const math::float3& pos);
	void SetRotation(const math::float3 &rot_euler);
	void SetRotation(const math::Quat& rot);
	void SetScale(const math::float3& scale);
	void Set(math::float4x4 matrix);
	void SetGlobal(float4x4 global);

	math::float3 GetPosition()const;
	math::float3 GetRotationEuler()const;
	math::Quat GetRotation()const;
	math::float3 GetScale()const;

	void Rotate(const math::Quat& quaternion);
	void Reset();

	math::float4x4 GetLocalTransformMatrix()const;
	math::float4x4 GetGlobalMatrixTransposed()const;
	math::float4x4 GetGlobalMatrix()const;

	math::float3 GetForward() const;

	void UpdateMatrix();

private:
	void CalculateGlobalTransform();

private:
	math::float3 position = math::float3::zero;
	math::Quat rotation = math::Quat::identity;
	math::float3 scale = math::float3::one;

	math::float3 rotation_euler = math::float3::zero;

	math::float4x4 local_matrix = math::float4x4::identity;
	math::float4x4 global_matrix = math::float4x4::identity;

	bool transform_modified = false;
};
#endif // !__COMPONENT_TRANSFORM_H__
