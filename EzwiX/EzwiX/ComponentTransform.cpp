#include "ComponentTransform.h"

#include "ImGui\imgui.h"
#include "ImGuizmo\ImGuizmo.h"

#include "ModuleEditor.h"

#include "GameObject.h"

#include "log.h"

using namespace std;

ComponentTransform::ComponentTransform(ComponentType type, GameObject* game_object, math::float4x4** global_matrix) : Component(type, game_object)
{
	CalculateGlobalTransform();
	*global_matrix = &(this->global_matrix);
}

ComponentTransform::~ComponentTransform()
{

}

void ComponentTransform::Update()
{
	UpdateMatrix();
}

void ComponentTransform::OnInspector(bool debug)
{
	string str = (string("Transform") + string("##") + std::to_string(uuid));
	if (ImGui::CollapsingHeader(str.c_str(), ImGuiTreeNodeFlags_DefaultOpen))
	{
		if (ImGui::IsItemClicked(1))
		{
			ImGui::OpenPopup("reset##transform");
		}

		if (ImGui::BeginPopup("reset##transform"))
		{
			if (ImGui::MenuItem("Reset"))
			{
				Reset();
			}
			ImGui::EndPopup();
		}

		ImVec4 white = ImVec4(1, 1, 1, 1);

		ImGuizmo::BeginFrame();

		//Inspector selection
		if (ImGui::RadioButton("Translate", App->editor->guizmo_operation == ImGuizmo::OPERATION::TRANSLATE))
		{
			App->editor->guizmo_operation = ImGuizmo::OPERATION::TRANSLATE;
		}
		ImGui::SameLine();

		if (ImGui::RadioButton("Rotate", App->editor->guizmo_operation == ImGuizmo::OPERATION::ROTATE))
		{
			App->editor->guizmo_operation = ImGuizmo::OPERATION::ROTATE;
		}
		ImGui::SameLine();
		if (ImGui::RadioButton("Scale", App->editor->guizmo_operation == ImGuizmo::OPERATION::SCALE))
		{
			App->editor->guizmo_operation = ImGuizmo::OPERATION::SCALE;
		}

		ImGui::Checkbox("Enable Gizmo", &App->editor->guizmo_enabled);

		//Position
		ImGui::TextColored(white, "Position: ");
		ImGui::SameLine();

		float3 position = this->position;
		if (ImGui::DragFloat3("##pos", position.ptr()))
		{
			SetPosition(position);
		}

		//Rotation
		ImGui::TextColored(white, "Rotation: ");
		ImGui::SameLine();

		float3 rotation = this->rotation_euler;
		if (ImGui::DragFloat3("##rot", rotation_euler.ptr(), 1.0f, -360.0f, 360.0f))
		{
			SetRotation(rotation_euler);
		}

		//Scale
		ImGui::TextColored(white, "Scale:  ");
		ImGui::SameLine();

		float3 scale = this->scale;
		if (ImGui::DragFloat3("##scale", scale.ptr()))
		{
			SetScale(scale);
		}

		if (ImGui::RadioButton("World", App->editor->guizmo_mode == ImGuizmo::MODE::WORLD))
		{
			App->editor->guizmo_mode = ImGuizmo::MODE::WORLD;
		}
		ImGui::SameLine();

		if (ImGui::RadioButton("Local", App->editor->guizmo_mode == ImGuizmo::MODE::LOCAL))
		{
			App->editor->guizmo_mode = ImGuizmo::MODE::LOCAL;
		}

		//Local Matrix
		if (debug == true)
		{
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(255, 255, 0, 255));
			ImGui::Text("%0.2f %0.2f %0.2f %0.2f", local_matrix.v[0][0], local_matrix.v[0][1], local_matrix.v[0][2], local_matrix.v[0][3]);
			ImGui::Text("%0.2f %0.2f %0.2f %0.2f", local_matrix.v[1][0], local_matrix.v[1][1], local_matrix.v[1][2], local_matrix.v[1][3]);
			ImGui::Text("%0.2f %0.2f %0.2f %0.2f", local_matrix.v[2][0], local_matrix.v[2][1], local_matrix.v[2][2], local_matrix.v[2][3]);
			ImGui::Text("%0.2f %0.2f %0.2f %0.2f", local_matrix.v[3][0], local_matrix.v[3][1], local_matrix.v[3][2], local_matrix.v[3][3]);
			ImGui::PopStyleColor();
		}
	}
}

void ComponentTransform::SetPosition(const math::float3& pos)
{
	position = pos;

	transform_modified = true;
}

void ComponentTransform::SetRotation(const math::float3& rot_euler)
{
	rotation_euler = rot_euler;

	float3 rot_deg = DegToRad(rot_euler);

	rotation = rotation.FromEulerXYZ(rot_deg.x, rot_deg.y, rot_deg.z);

	transform_modified = true;
}

void ComponentTransform::SetRotation(const math::Quat& rot)
{
	rotation = rot;

	rotation_euler = RadToDeg(rotation.ToEulerXYZ());

	transform_modified = true;
}

void ComponentTransform::SetScale(const math::float3& scale)
{
	this->scale = scale;

	transform_modified = true;
}

void ComponentTransform::Set(math::float4x4 matrix)
{
	transform_modified = true;

	float3 pos, scal;
	Quat rot;
	matrix.Decompose(pos, rot, scal);

	SetPosition(pos);
	SetRotation(rot);
	SetScale(scal);
}

void ComponentTransform::SetGlobal(float4x4 global)
{
	GameObject* parent = game_object->GetParent();
	if (parent != nullptr)
	{
		float4x4 new_local = ((ComponentTransform*)parent->GetComponent(C_TRANSFORM))->GetGlobalMatrix().Inverted() * global;
		float3 translate, scale;
		Quat rotation;
		new_local.Decompose(translate, rotation, scale);
		SetPosition(translate);
		SetRotation(rotation);
		SetScale(scale);
	}
}

math::float3 ComponentTransform::GetPosition() const
{
	return position;
}

math::float3 ComponentTransform::GetRotationEuler() const
{
	return rotation_euler;
}

math::Quat ComponentTransform::GetRotation() const
{
	return rotation;
}

math::float3 ComponentTransform::GetScale()const
{
	return scale;
}

math::float4x4 ComponentTransform::GetLocalTransformMatrix() const
{
	return local_matrix;
}

math::float4x4 ComponentTransform::GetGlobalMatrixTransposed()const
{
	return global_matrix.Transposed();
}

math::float4x4 ComponentTransform::GetGlobalMatrix()const
{
	return global_matrix;
}

math::float3 ComponentTransform::GetForward() const
{
	return global_matrix.WorldZ();
}

void ComponentTransform::UpdateMatrix()
{
	if (transform_modified)
	{
		local_matrix = local_matrix.FromTRS(position, rotation, scale);
		CalculateGlobalTransform();
		transform_modified = false;
	}
}

void ComponentTransform::CalculateGlobalTransform()
{
	GameObject* game_object = GetGameObject();
	if (game_object)
	{
		if (game_object->GetParent())
		{
			global_matrix = game_object->GetParent()->transform->global_matrix * local_matrix;

			std::vector<GameObject*>::const_iterator go_childs = game_object->GetChilds()->begin();
			for (go_childs; go_childs != game_object->GetChilds()->end(); ++go_childs)
				(*go_childs)->transform->CalculateGlobalTransform();
		}
		else
		{
			global_matrix = local_matrix;
		}

		game_object->TransformModified();
	}
	else
	{
		LOG("[WARNING] Component created but not attached to any gameobject");
	}
}

void ComponentTransform::Rotate(const math::Quat & quaternion)
{
	rotation = quaternion * rotation;
	transform_modified = true;
}

void ComponentTransform::Reset()
{
	SetPosition(float3::zero);
	SetRotation(Quat::identity);
	SetScale(float3::one);
}