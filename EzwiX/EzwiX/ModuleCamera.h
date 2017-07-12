#ifndef __MODULE_CAMERA_H__
#define __MODULE_CAMERA_H__

#include <d3dx10math.h>

#include "MathGeoLib\include\MathGeoLib.h"

#include "Module.h"

class ModuleCamera : public Module
{
public:
	ModuleCamera(const char* name, bool start_enabled = true);
	~ModuleCamera();

	bool Init();
	update_status Update();

	void SetPosition(float x, float y, float z);
	void SetPosition(const math::float3& new_position);
	void SetRotation(float x, float y, float z);

	D3DXVECTOR3 GetPosition()const;

	void GetViewMatrix(math::float4x4& view)const;
	void GetViewMatrix(D3DXMATRIX& view);
	void GetProjectionMatrix(math::float4x4& projection)const;
	void GetProjectionMatrix(D3DXMATRIX& projection);

	void LookAt(const math::float3& point);

private:

	void CameraMovement();

private:

	math::float3 position;
	math::float3 reference;

	float near_plane = 0.3f;
	float far_plane = 300.0f;

	math::Frustum frustum;

};
#endif // !__MODULE_CAMERA_H__
