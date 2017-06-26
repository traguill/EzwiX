#ifndef __GAMEOBJECT_H__
#define __GAMEOBJECT_H__

class Component;

//NOTE: This class is empty for now. It exists only to avoid errors with other classes. (I will fill it later)

class GameObject
{
public:
	GameObject();
	~GameObject();

	void RemoveComponent(Component* comp);

private:

};

#endif // !__GAMEOBJECT_H__
