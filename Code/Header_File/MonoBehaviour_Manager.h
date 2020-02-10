#pragma once
#include <stdio.h>
#include <list>
#include <string>
#include <memory>
#include "MonoBehaviour.h"

class MonoBehaviour_Manager
{
public:

	static void Reset();

	static void Update();
	static void Add(std::shared_ptr<MonoBehaviour> mono);
	static void Add_Disable(std::shared_ptr<MonoBehaviour> mono);
	static void Add_Enable(std::shared_ptr<MonoBehaviour> mono);

private:
	static std::list<std::weak_ptr<MonoBehaviour>> MonoBehaviour_Update_list;
	static std::list<std::weak_ptr<MonoBehaviour>> MonoBehaviour_Awake_list;
	static std::list<std::weak_ptr<MonoBehaviour>> MonoBehaviour_Start_list;
	static std::list<std::weak_ptr<MonoBehaviour>> MonoBehaviour_Disable_list;
	static std::list<std::weak_ptr<MonoBehaviour>> MonoBehaviour_Enable_list;
};