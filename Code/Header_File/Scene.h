#pragma once
#include <stdio.h>
#include <string>
#include <list>
#include <memory>
#include "GameObject.h"
#include "MonoBehaviour.h"

class Scene_Manager;

class Scene
{
public:
	std::string name; //シーン名

	std::weak_ptr<GameObject> Find(std::string Name);         //指定した名前のゲームオブジェクトを返す
	std::weak_ptr<GameObject> FindWithTag(std::string Tag);   //指定したタグのゲームオブジェクトを返す

	std::vector<std::shared_ptr<GameObject>> gameObject_List; //シーン内の全ゲームオブジェクト

private:

	friend class cereal::access;
	template<class Archive>
	void serialize(Archive& archive)
	{
		archive(gameObject_List);
	}

	friend class Scene_Manager;
	friend class Object;
	friend class MonoBehaviour;

	std::shared_ptr<GameObject> Instance_GameObject(std::string name);
	void Destroy_GameObject(std::shared_ptr<GameObject> gameObject);
	void Destroy_Component(std::shared_ptr<Component> component);
	//void Add(std::shared_ptr<MonoBehaviour> mono);
	//void Add_Enable(std::shared_ptr<MonoBehaviour> mono);

	void Initialize();
	void Update();
	void Reset();

	void Processing_Start();
	void Processing_Update(int state);

	//std::vector<std::weak_ptr<MonoBehaviour>> MonoBehaviour_Awake_list;
	std::vector<std::weak_ptr<MonoBehaviour>> MonoBehaviour_Start_list;
	std::vector<std::weak_ptr<MonoBehaviour>> MonoBehaviour_Start_Next_list;
	std::vector<std::weak_ptr<MonoBehaviour>> MonoBehaviour_Update_list;
	//std::vector<std::weak_ptr<MonoBehaviour>> MonoBehaviour_Update_Next_list;
	//std::vector<std::weak_ptr<MonoBehaviour>> MonoBehaviour_Enable_list;

	//int Update_Stage = 0;
};