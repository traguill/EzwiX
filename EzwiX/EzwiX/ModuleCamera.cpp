#include "ModuleCamera.h"

#include "Globals.h"
#include "log.h"

#include "ModuleInput.h"
#include "ModuleTimer.h"
#include "ModuleWindow.h"

ModuleCamera::ModuleCamera(const char * name, bool start_enabled) : Module(name, start_enabled)
{
}

ModuleCamera::~ModuleCamera()
{
}

bool ModuleCamera::Init()
{
	float aspect_ratio = (float)App->window->GetScreenWidth() / (float)App->window->GetScreenHeight();
	float vertical_fov = DegToRad(60);
	float horizontal_fov = 2.0f * atanf(tanf(vertical_fov / 2.0f) * aspect_ratio);

	frustum.SetPerspective(horizontal_fov, vertical_fov);
	frustum.SetKind(FrustumProjectiveSpace::FrustumSpaceD3D, FrustumHandedness::FrustumLeftHanded);

	position = float3(0, 0, -10);
	reference = float3(0.0f);

	frustum.SetPos(position);
	frustum.SetUp(float3::unitY);
	frustum.SetFront(float3::unitZ);

	frustum.SetViewPlaneDistances(near_plane, far_plane);
	frustum.SetVerticalFovAndAspectRatio(vertical_fov, aspect_ratio);

	LookAt(reference);

	return true;
}

update_status ModuleCamera::Update()
{
	CameraMovement();

	//Reset camera
	if (App->input->GetKey(KEY_SPACEBAR) == KEY_UP)
	{
		SetPosition(float3(0, 0, -10.0f));
		reference = float3(0.0f);
		LookAt(reference);
	}

	return update_status::UPDATE_CONTINUE;
}

void ModuleCamera::SetPosition(float x, float y, float z)
{
	position.x = x;
	position.y = y;
	position.z = z;

	frustum.SetPos(position);
}

void ModuleCamera::SetPosition(const math::float3 & new_position)
{
	position.x = new_position.x;
	position.y = new_position.y;
	position.z = new_position.z;

	frustum.SetPos(position);
}

void ModuleCamera::SetRotation(float x, float y, float z)
{
}

D3DXVECTOR3 ModuleCamera::GetPosition() const
{
	return D3DXVECTOR3(position.ptr());
}

void ModuleCamera::GetViewMatrix(D3DXMATRIX & view)
{
	view = D3DXMATRIX(float4x4(frustum.ViewMatrix()).ptr());
}

void ModuleCamera::GetProjectionMatrix(D3DXMATRIX & projection)
{
	projection = D3DXMATRIX(frustum.ProjectionMatrix().ptr());
}

void ModuleCamera::LookAt(const math::float3 & point)
{
	float3 direction = (point - frustum.Pos()).Normalized();
	float3x3 matrix = float3x3::LookAt(frustum.Front(), direction, frustum.Up(), float3::unitY);
	
	frustum.SetFront(matrix.MulDir(frustum.Front()).Normalized());
	frustum.SetUp(matrix.MulDir(frustum.Up()).Normalized());
}

void ModuleCamera::CameraMovement()
{
	float speed = 8.0 * App->timer->GetDeltaTime(); //TODO: Make this editable from the engine
	
	int motion_x = 0;
	int motion_y = 0;
	App->input->GetMouseMotion(motion_x, motion_y);

	//Orbit
	if (App->input->GetMouseButton(MOUSE_BUTTON_RIGHT) == KEY_REPEAT && (motion_x != 0 || motion_y != 0))
	{
		float3 inv_dir = position - reference;

		Quat rot_x(frustum.WorldRight(), motion_y * 0.003f);
		Quat rot_y(frustum.Up(), motion_x * 0.003f);

		inv_dir = rot_x * inv_dir;
		inv_dir = rot_y * inv_dir;

		SetPosition(inv_dir + reference);
		LookAt(reference);
	}

	//Arrows
	if (App->input->GetKey(KEY_ARROW_LEFT) == KEY_REPEAT || App->input->GetKey(KEY_ARROW_RIGHT) == KEY_DOWN)
	{
		reference -= frustum.WorldRight() * speed;
		SetPosition(position - frustum.WorldRight() * speed);
	}

	if (App->input->GetKey(KEY_ARROW_RIGHT) == KEY_REPEAT || App->input->GetKey(KEY_ARROW_RIGHT) == KEY_DOWN)
	{
		reference += frustum.WorldRight() * speed;
		SetPosition(position + frustum.WorldRight() * speed);
	}

	if (App->input->GetKey(KEY_ARROW_UP) == KEY_REPEAT || App->input->GetKey(KEY_ARROW_UP) == KEY_DOWN)
	{
		reference += frustum.Up() * speed;
		SetPosition(position + frustum.Up() * speed);
	}

	if (App->input->GetKey(KEY_ARROW_DOWN) == KEY_REPEAT || App->input->GetKey(KEY_ARROW_DOWN) == KEY_DOWN)
	{
		reference -= frustum.Up() * speed;
		SetPosition(position - frustum.Up() * speed);
	}

	//Mouse button
	if (App->input->GetMouseButton(MOUSE_BUTTON_MIDDLE) == KEY_REPEAT || App->input->GetMouseButton(MOUSE_BUTTON_MIDDLE) == KEY_DOWN)
	{
		float dst = reference.Distance(position);
		float3 add_x = frustum.Up() * motion_y * (dst / 1000.0f);
		float3 add_y = frustum.WorldRight() * -motion_x * (dst / 1000.0f);

		reference += add_x + add_y;

		SetPosition(position + add_x + add_y);
	}

	//Mouse Wheel
	int zoom = App->input->GetMouseWheel();
	if (zoom != 0)
	{
		float dst = reference.Distance(position);
		if (dst < 1.0f)
			dst = 1.0f;
		SetPosition(position + frustum.Front() * zoom * dst * 0.05f);
	}
}
