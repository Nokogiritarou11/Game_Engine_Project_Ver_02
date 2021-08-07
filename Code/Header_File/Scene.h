#pragma once
#include <stdio.h>
#include <string>
#include <list>
#include <memory>
#include "GameObject.h"
#include "MonoBehaviour.h"

namespace BeastEngine
{
	class Scene
	{
	public:
		std::string name; //�V�[����

		std::weak_ptr<BeastEngine::GameObject> Find(std::string Name);         //�w�肵�����O�̃Q�[���I�u�W�F�N�g��Ԃ�
		std::weak_ptr<BeastEngine::GameObject> Find_With_Tag(std::string Tag);   //�w�肵���^�O�̃Q�[���I�u�W�F�N�g��Ԃ�

	private:
		std::vector<std::shared_ptr<BeastEngine::GameObject>> gameobject_list; //�V�[�����̑S�Q�[���I�u�W�F�N�g

		friend class cereal::access;
		template<class Archive>
		void serialize(Archive& archive, std::uint32_t const version)
		{
			archive(name, gameobject_list);
		}

		friend class Scene_Manager;
		friend class Object;
		friend class MonoBehaviour;
		friend class Resources;
		friend class Transform;
		friend class Editor;

		std::shared_ptr<GameObject> Instance_GameObject(std::string name);
		void Destroy_GameObject(std::shared_ptr<GameObject> gameObject);
		void Destroy_Component(std::shared_ptr<Component> component);

		void Initialize();
		void Update();
		void Reset();

		void Processing_Start();
		void Processing_Update(int state);

		std::vector<std::weak_ptr<MonoBehaviour>> monobehaviour_Start_list;
		std::vector<std::weak_ptr<MonoBehaviour>> monobehaviour_Start_next_list;
		std::vector<std::weak_ptr<MonoBehaviour>> monobehaviour_Update_list;
		std::vector<std::weak_ptr<MonoBehaviour>> monobehaviour_Update_next_list;
	};
}

CEREAL_CLASS_VERSION(BeastEngine::Scene, 1)