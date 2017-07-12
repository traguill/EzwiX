#include "ModuleGameObjectManager.h"

#include "GameObject.h"

ModuleGameObjectManager::ModuleGameObjectManager(const char* name, bool start_enabled) : Module(name, start_enabled)
{
}

ModuleGameObjectManager::~ModuleGameObjectManager()
{
}

bool ModuleGameObjectManager::Init()
{
	root = new GameObject(nullptr);

	//Testing hierarchy
	GameObject* a = CreateGameObject(nullptr);
	CreateGameObject(nullptr);
	CreateGameObject(a);

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
