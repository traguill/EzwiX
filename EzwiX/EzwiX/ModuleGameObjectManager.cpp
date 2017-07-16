#include "ModuleGameObjectManager.h"

#include "ModuleFileSystem.h"

#include "GameObject.h"
#include "Component.h"
#include "ComponentTransform.h"
#include "ComponentMesh.h"
#include "ComponentTexture.h"

#include "log.h"
#include "Data.h"

ModuleGameObjectManager::ModuleGameObjectManager(const char* name, bool start_enabled) : Module(name, start_enabled)
{
}

ModuleGameObjectManager::~ModuleGameObjectManager()
{
}

bool ModuleGameObjectManager::Init()
{
	root = new GameObject(nullptr);

	return true;
}

bool ModuleGameObjectManager::CleanUp()
{
	delete root;

	return true;
}

update_status ModuleGameObjectManager::PreUpdate()
{
	if (root)
		PreUpdateGameObjects(root);

	return update_status::UPDATE_CONTINUE;
}

update_status ModuleGameObjectManager::Update()
{
	if (root)
		UpdateGameObjects(root);

	return update_status::UPDATE_CONTINUE;
}

GameObject * ModuleGameObjectManager::CreateGameObject(GameObject * parent)
{
	GameObject* go = nullptr;

	//If no parent is sent the new GameObject is son of the root
	if (parent)
	{
		go = new GameObject(parent);
	}
	else
	{
		go = new GameObject(root);
	}
	return go;
}

const GameObject * ModuleGameObjectManager::GetRoot() const
{
	return root;
}

void ModuleGameObjectManager::Load3DModelFile(const char* file_path)
{
	char* buffer = nullptr;
	unsigned int size = App->file_system->Load(file_path, &buffer);

	if (size == 0)
	{
		if (buffer)
			delete[] buffer;
		LOG("Could not open the file %s to load", file_path);
	}

	Data data(buffer);
	unsigned int num_objects = data.GetArraySize("GameObjects");

	Data object;
	vector<GameObject*> items_created;
	for (int i = 0; i < num_objects; ++i)
	{
		object = data.GetArray("GameObjects", i);

		//Create the GameObjects
		unsigned int uuid_parent = object.GetUInt("parent");

		string name = object.GetString("name");
		unsigned int uuid = object.GetUInt("UUID");

		GameObject* parent = nullptr;
		if (uuid_parent == 0)
			parent = root;
		else
			parent = FindGameObject(items_created, uuid_parent);

		GameObject* game_object = new GameObject(name.data(), uuid, parent);
		
		//Link parent with this GameObject
		parent->AddChild(game_object);

		items_created.push_back(game_object);

		//Create all the components (Transform, Mesh, Texture/Material (future))
		unsigned int num_components = object.GetArraySize("components");
		for (int c = 0; c < num_components; ++c)
		{
			Data component_data = object.GetArray("components", c);

			ComponentType type = (ComponentType)component_data.GetUInt("type");

			switch (type)
			{
			case ComponentType::C_TRANSFORM:
			{
				ComponentTransform* c_trans = (ComponentTransform*)game_object->AddComponent(C_TRANSFORM);
				c_trans->Load(component_data);
				break;
			}
			case ComponentType::C_MESH:
			{
				ComponentMesh* c_mesh = (ComponentMesh*)game_object->AddComponent(C_MESH);
				c_mesh->Load(component_data);
				break;
			}
			case ComponentType::C_TEXTURE:
			{
				ComponentTexture* c_tex = (ComponentTexture*)game_object->AddComponent(C_TEXTURE);
				c_tex->Load(component_data);
				break;
			}
			default:
				break;
			}
		}

	}

	items_created.clear();
	delete[] buffer;

}

void ModuleGameObjectManager::PreUpdateGameObjects(GameObject * obj)
{
	if (root != obj && obj->IsActive() == true)
		obj->PreUpdate();

	std::vector<GameObject*>::const_iterator child = obj->GetChilds()->begin();
	for (child; child != obj->GetChilds()->end(); ++child)
	{
		PreUpdateGameObjects((*child));
	}
}

void ModuleGameObjectManager::UpdateGameObjects(GameObject* object)
{
	if (root != object && object->IsActive() == true)
		object->Update();

	std::vector<GameObject*>::const_iterator child = object->GetChilds()->begin();
	for (child; child != object->GetChilds()->end(); ++child)
	{
		UpdateGameObjects((*child));
	}
}

GameObject * ModuleGameObjectManager::FindGameObject(const vector<GameObject*>& list, unsigned int uuid) const
{
	for (vector<GameObject*>::const_iterator go = list.begin(); go != list.end(); ++go)
	{
		if ((*go)->GetUUID() == uuid)
			return (*go);
	}
	return nullptr;
}
