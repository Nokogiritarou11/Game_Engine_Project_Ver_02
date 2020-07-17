#pragma once
#include <stdio.h>
#include <list>
#include <memory>
#include "Camera.h"

class Camera_Manager
{
public:
	static void Reset();

	static void Render();
	static void Add(std::shared_ptr<Camera> mono);

private:
	static std::list<std::weak_ptr<Camera>> Camera_list;
};