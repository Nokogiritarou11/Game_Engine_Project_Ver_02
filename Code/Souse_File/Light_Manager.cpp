#include "Light_Manager.h"
using namespace std;

list<weak_ptr<Light>> Light_Manager::Light_list;

void Light_Manager::Reset()
{
	Light_list.clear();
}

void Light_Manager::Add(std::shared_ptr<Light> light)
{
	Light_list.emplace_back(light);
}