#pragma once
#include <stdlib.h>
#include <memory>

class GameObject;

class All_Component_List
{
public:
	static void Add(std::shared_ptr<GameObject> obj);
};