#include "ModuleCamera.h"

#include "Globals.h"
#include "log.h"

#include "ModuleInput.h"

ModuleCamera::ModuleCamera(const char * name, bool start_enabled) : Module(name, start_enabled)
{
}

ModuleCamera::~ModuleCamera()
{
}

update_status ModuleCamera::Update()
{
	if (dirty)
	{
		D3DXVECTOR3 up;
		up.x = 0.0f;
		up.y = 1.0f;
		up.z = 0.0f;

		D3DXVECTOR3 position;
		position.x = position_x;
		position.y = position_y;
		position.z = position_z;

		D3DXVECTOR3 look_at;
		look_at.x = 0.0f;
		look_at.y = 0.0f;
		look_at.z = 1.0f;

		float pitch = rotation_x * DEGTORAD;
		float yaw = rotation_y * DEGTORAD;
		float roll = rotation_z * DEGTORAD;

		D3DXMATRIX rotation_matrix;
		D3DXMatrixRotationYawPitchRoll(&rotation_matrix, yaw, pitch, roll);

		D3DXVec3TransformCoord(&look_at, &look_at, &rotation_matrix);
		D3DXVec3TransformCoord(&up, &up, &rotation_matrix);

		//Translate the rotated camera position to the location of the viewer
		look_at = position + look_at;

		D3DXMatrixLookAtLH(&view_matrix, &position, &look_at, &up);

		dirty = false;
	}

	return update_status::UPDATE_CONTINUE;
}

void ModuleCamera::SetPosition(float x, float y, float z)
{
	position_x = x;
	position_y = y;
	position_z = z;

	dirty = true;
}

void ModuleCamera::SetRotation(float x, float y, float z)
{
	rotation_x = x;
	rotation_y = y;
	rotation_z = z;

	dirty = true;
}

D3DXVECTOR3 ModuleCamera::GetPosition() const
{
	return D3DXVECTOR3(position_x, position_y, position_z);
}

D3DXVECTOR3 ModuleCamera::GetRotation() const
{
	return D3DXVECTOR3(rotation_x, rotation_y, rotation_z);
}

void ModuleCamera::GetViewMatrix(D3DXMATRIX & view)
{
	view = view_matrix;
}
