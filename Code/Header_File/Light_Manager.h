#pragma once
#include "Light.h"

class Light_Manager
{
public:
	static void Reset();
	static void Add(std::shared_ptr<Light> light);
	static std::list<std::weak_ptr<Light>> Light_list;
private:

};