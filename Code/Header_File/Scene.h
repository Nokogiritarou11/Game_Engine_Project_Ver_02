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
	std::string name; //�V�[����

	std::weak_ptr<GameObject> Find(std::string Name);         //�w�肵�����O�̃Q�[���I�u�W�F�N�g��Ԃ�
	std::weak_ptr<GameObject> FindWithTag(std::string Tag);   //�w�肵���^�O�̃Q�[���I�u�W�F�N�g��Ԃ�

	std::vector<std::shared_ptr<GameObject>> gameObject_List; //�V�[�����̑S�Q�[���I�u�W�F�N�g

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

	void Initialize();
	void Update();
	void Reset();

	void Processing_Start();
	void Processing_Update(int state);

	std::vector<std::weak_ptr<MonoBehaviour>> MonoBehaviour_Start_list;
	std::vector<std::weak_ptr<MonoBehaviour>> MonoBehaviour_Start_Next_list;
	std::vector<std::weak_ptr<MonoBehaviour>> MonoBehaviour_Update_list;
};