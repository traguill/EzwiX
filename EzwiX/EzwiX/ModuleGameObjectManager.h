#ifndef __MODULE_GAMEOBJECT_MANAGER_H__
#define __MODULE_GAMEOBJECT_MANAGER_H__

#include "Module.h"

class GameObject;

class ModuleGameObjectManager : public Module
{
public:
	ModuleGameObjectManager(const char* name, bool start_enabled = true);
	~ModuleGameObjectManager();

	bool Init();
	bool CleanUp();

	update_status PreUpdate();
	update_status Update();

	GameObject* CreateGameObject(GameObject* parent); //For external use

	const GameObject* GetRoot()const;

private:

	void PreUpdateGameObjects(GameObject* object);
	void UpdateGameObjects(GameObject* object);

private:

	GameObject* root = nullptr;

};
#endif // !__MODULE_GAMEOBJECT_MANAGER_H__
