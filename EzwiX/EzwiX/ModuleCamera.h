#ifndef __MODULE_CAMERA_H__
#define __MODULE_CAMERA_H__

#include <d3dx10math.h>

#include "Module.h"

class ModuleCamera : public Module
{
public:
	ModuleCamera(const char* name, bool start_enabled = true);
	~ModuleCamera();

	update_status Update();

	void SetPosition(float x, float y, float z);
	void SetRotation(float x, float y, float z);

	D3DXVECTOR3 GetPosition()const;
	D3DXVECTOR3 GetRotation()const;

	void GetViewMatrix(D3DXMATRIX& view);

private:

	float position_x = 0.0f;
	float position_y = 0.0f;
	float position_z = 0.0f;

	float rotation_x = 0.0f;
	float rotation_y = 0.0f;
	float rotation_z = 0.0f;

	D3DXMATRIX view_matrix;

	bool dirty = false;

};
#endif // !__MODULE_CAMERA_H__
