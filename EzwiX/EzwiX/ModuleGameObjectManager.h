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

	void Load3DModelFile(const char* file_path);

private:

	void PreUpdateGameObjects(GameObject* object);
	void UpdateGameObjects(GameObject* object);

	//Load 3DModels helpers
	GameObject* FindGameObject(const vector<GameObject*>& list, unsigned int uuid)const;

private:

	GameObject* root = nullptr;

};
#endif // !__MODULE_GAMEOBJECT_MANAGER_H__
