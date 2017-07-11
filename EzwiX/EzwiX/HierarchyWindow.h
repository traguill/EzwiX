#ifndef __HIERARCHY_WINDOW_H__
#define __HIERARCHY_WINDOW_H__

#include "Window.h"

#include <vector>

class GameObject;

class HierarchyWindow : public Window
{
public:
	HierarchyWindow();
	~HierarchyWindow();

	void Draw();

private:

	void DisplayGameObjects(const std::vector<GameObject*>* childs)const;
};

#endif
