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
