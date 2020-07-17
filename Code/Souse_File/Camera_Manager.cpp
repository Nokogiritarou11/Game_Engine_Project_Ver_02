#include "Camera_Manager.h"
#include "GameObject.h"
#include "Render_Manager.h"
using namespace std;

list<weak_ptr<Camera>> Camera_Manager::Camera_list;

void Camera_Manager::Reset()
{
	Camera_list.clear();
}

void Camera_Manager::Render()
{
	for (list<weak_ptr<Camera>>::iterator itr = Camera_list.begin(); itr != Camera_list.end();)
	{
		if (itr->expired())
		{
			itr = Camera_list.erase(itr);
			continue;
		}
		shared_ptr<Camera> camera = itr->lock();
		if (camera->gameObject->activeSelf())
		{
			if (camera->enabled)
			{
				camera->Update();
				Render_Manager::Render(camera);
			}
		}
		itr++;
	}
}

void Camera_Manager::Add(shared_ptr<Camera> mono)
{
	Camera_list.emplace_back(mono);
}