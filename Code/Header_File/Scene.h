#pragma once
#include <string>
#include <memory>
#include "GameObject.h"
#include "MonoBehaviour.h"

namespace BeastEngine
{
	//シーン
	class Scene
	{
	public:
		std::weak_ptr<GameObject> Find(const std::string& name);         //指定した名前のゲームオブジェクトを返す
		std::weak_ptr<GameObject> Find_With_Tag(const std::string& tag); //指定したタグのゲームオブジェクトを返す

		std::string name; //シーン名

	private:
		std::shared_ptr<GameObject> Instance_GameObject(const std::string& name); //シーン内にオブジェクトを配置する
		void Destroy_GameObject(const std::shared_ptr<GameObject>& game_object); //オブジェクトを削除する
		void Destroy_Component(const std::shared_ptr<Component>& component); //コンポーネントを削除する

		void Initialize(); //初期化
		void Update(); //更新
		void Reset(); //シーンのリセット

		void Processing_Start(); //MonoBehaviourのStartを実行する
		void Processing_Update(int state); //MonoBehaviourのUpdateを実行する

		std::vector<std::shared_ptr<GameObject>> gameobject_list; //シーン内の全ゲームオブジェクト

		std::vector<std::weak_ptr<MonoBehaviour>> monobehaviour_start_list; //startの実行リスト
		std::vector<std::weak_ptr<MonoBehaviour>> monobehaviour_start_next_list; //次のフレームで行うstartの実行リスト
		std::vector<std::weak_ptr<MonoBehaviour>> monobehaviour_update_list; //updateの実行リスト
		std::vector<std::weak_ptr<MonoBehaviour>> monobehaviour_update_next_list; //次のフレームで行うupdateの実行リスト

		friend class Scene_Manager;
		friend class Object;
		friend class MonoBehaviour;
		friend class Resources;
		friend class Transform;
		friend class Editor;

		friend class cereal::access;
		template<class Archive>
		void serialize(Archive& archive, std::uint32_t const version)
		{
			archive(name, gameobject_list);
		}

	};
}

CEREAL_CLASS_VERSION(BeastEngine::Scene, 1)