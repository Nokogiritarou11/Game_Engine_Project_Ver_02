#pragma once
#include "Light.h"

class Light_Manager
{
public:
	void Reset();
	void Add(std::shared_ptr<Light> light);
	std::vector<std::weak_ptr<Light>> Light_list;
private:

};