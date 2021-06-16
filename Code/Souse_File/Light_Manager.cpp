#include "Light_Manager.h"
#include "Light.h"
using namespace std;
using namespace BeastEngine;

void Light_Manager::Reset()
{
	Light_list.clear();
}

void Light_Manager::Add(std::shared_ptr<Light> light)
{
	Light_list.emplace_back(light);
}