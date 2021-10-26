#pragma once
#include "MonoBehaviour.h"

namespace BeastEngine
{
	class Enemy_Manager final : public MonoBehaviour
	{
	public:
		void Add_Enemy_List(const std::weak_ptr<GameObject>& obj);
		void Remove_Enemy_List(const std::weak_ptr<GameObject>& obj);

		void Add_Attacking_List(const std::weak_ptr<GameObject>& obj);
		void Remove_Attacking_List(const std::weak_ptr<GameObject>& obj);

		bool Get_Has_Enemy() const { return !enemy_list.empty(); };
		bool Get_Has_Attacking() const { return !attacking_list.empty(); };
		Vector3 Get_Nearest_Enemy_Position(const Vector3& position);

		std::vector<std::weak_ptr<GameObject>> enemy_list{};
		std::vector<std::weak_ptr<GameObject>> attacking_list{};

	private:
		bool Draw_ImGui() override;
		
		// シリアライズ関数
		friend class cereal::access;
		template<class Archive>
		void serialize(Archive& archive, std::uint32_t const version)
		{
			archive(cereal::base_class<MonoBehaviour>(this));
		}
	};
}

REGISTER_COMPONENT(Enemy_Manager)
CEREAL_REGISTER_TYPE(BeastEngine::Enemy_Manager)
CEREAL_REGISTER_POLYMORPHIC_RELATION(BeastEngine::MonoBehaviour, BeastEngine::Enemy_Manager)
CEREAL_CLASS_VERSION(BeastEngine::Enemy_Manager, 1)