#pragma once
#include "Light.h"

class Light_Manager
{
public:
	void Reset();
	void Add(std::shared_ptr<Light> light);
	std::list<std::weak_ptr<Light>> Light_list;
private:

};