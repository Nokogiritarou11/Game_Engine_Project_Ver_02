#pragma once
#include <stdio.h>
#include <string>
#include <list>
#include <memory>
#include "GameObject.h"
#include "MonoBehaviour.h"

class Scene
{
public:
	std::string name;

	std::shared_ptr<GameObject> Instance_GameObject(std::string name);
	void Destroy_GameObject(std::shared_ptr<GameObject> gameObject);
	void Destroy_Component(std::shared_ptr<Component> component);
	void Update();
	void Reset();

	void Add(std::shared_ptr<MonoBehaviour> mono);
	void Add_Disable(std::shared_ptr<MonoBehaviour> mono);
	void Add_Enable(std::shared_ptr<MonoBehaviour> mono);

	std::weak_ptr<GameObject> Find(std::string Name);
	std::weak_ptr<GameObject> FindWithTag(std::string Tag);

	std::list<std::shared_ptr<GameObject>> gameObject_List;

private:

	friend class cereal::access;
	template<class Archive>
	void serialize(Archive& archive)
	{
		archive(gameObject_List);
	}

	std::list<std::weak_ptr<MonoBehaviour>> MonoBehaviour_Update_list;
	std::list<std::weak_ptr<MonoBehaviour>> MonoBehaviour_Awake_list;
	std::list<std::weak_ptr<MonoBehaviour>> MonoBehaviour_Start_list;
	std::list<std::weak_ptr<MonoBehaviour>> MonoBehaviour_Disable_list;
	std::list<std::weak_ptr<MonoBehaviour>> MonoBehaviour_Enable_list;

	std::list<std::weak_ptr<MonoBehaviour>> MonoBehaviour_Next_Update_list;

	int Update_Stage = 0;
};

/////////////////////////////////////////////////////////////////////////
/*
class Game_01_Scene : public Scene
{
public:
	void Initialize();

private:
};
*/