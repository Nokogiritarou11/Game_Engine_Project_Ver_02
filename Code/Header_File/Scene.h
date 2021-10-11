#pragma once
#include <string>
#include <memory>
#include "GameObject.h"
#include "MonoBehaviour.h"

namespace BeastEngine
{
	class Scene
	{
	public:
		std::string name; //シーン名

		std::weak_ptr<GameObject> Find(const std::string& name);         //指定した名前のゲームオブジェクトを返す
		std::weak_ptr<GameObject> Find_With_Tag(const std::string& tag);   //指定したタグのゲームオブジェクトを返す

	private:
		std::vector<std::shared_ptr<GameObject>> gameobject_list; //シーン内の全ゲームオブジェクト

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

		std::shared_ptr<GameObject> Instance_GameObject(const std::string& name);
		void Destroy_GameObject(const std::shared_ptr<GameObject>& game_object);
		void Destroy_Component(const std::shared_ptr<Component>& component);

		void Initialize();
		void Update();
		void Reset();

		void Processing_Start();
		void Processing_Update(int state);

		std::vector<std::weak_ptr<MonoBehaviour>> monobehaviour_start_list;
		std::vector<std::weak_ptr<MonoBehaviour>> monobehaviour_start_next_list;
		std::vector<std::weak_ptr<MonoBehaviour>> monobehaviour_update_list;
		std::vector<std::weak_ptr<MonoBehaviour>> monobehaviour_update_next_list;
	};
}

CEREAL_CLASS_VERSION(BeastEngine::Scene, 1)