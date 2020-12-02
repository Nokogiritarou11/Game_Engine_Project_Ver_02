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
	std::string name; //シーン名

	std::weak_ptr<GameObject> Find(std::string Name);         //指定した名前のゲームオブジェクトを返す
	std::weak_ptr<GameObject> FindWithTag(std::string Tag);   //指定したタグのゲームオブジェクトを返す

private:
	std::vector<std::shared_ptr<GameObject>> gameObject_List; //シーン内の全ゲームオブジェクト

	friend class cereal::access;
	template<class Archive>
	void serialize(Archive& archive)
	{
		archive(gameObject_List);
	}

	friend class Scene_Manager;
	friend class Object;
	friend class MonoBehaviour;
	friend class Resources;
	friend class Transform;
	friend class Debug_UI;

	std::shared_ptr<GameObject> Instance_GameObject(std::string name);
	void Destroy_GameObject(std::shared_ptr<GameObject> gameObject);
	void Destroy_Component(std::shared_ptr<Component> component);

	void Initialize();
	void Update();
	void Reset();

	void Processing_Start();
	void Processing_Update(int state);

	std::vector<std::weak_ptr<MonoBehaviour>> MonoBehaviour_Start_list;
	std::vector<std::weak_ptr<MonoBehaviour>> MonoBehaviour_Start_Next_list;
	std::vector<std::weak_ptr<MonoBehaviour>> MonoBehaviour_Update_list;
	std::vector<std::weak_ptr<MonoBehaviour>> MonoBehaviour_Update_Next_list;
};