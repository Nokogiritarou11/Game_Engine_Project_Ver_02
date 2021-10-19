#pragma once
#include "MonoBehaviour.h"
#include "Interface_Character_State_Setter.h"

namespace BeastEngine
{
	class Character_Parameter;

	class Enemy_Normal_01_State_Setter final : public MonoBehaviour, public Interface_Character_State_Setter
	{
	public:
		void Set_State() override;

	private:
		void Awake() override;
		bool Draw_ImGui() override;

		std::weak_ptr<Animator> animator;

		// �V���A���C�Y�֐�
		friend class cereal::access;
		template<class Archive>
		void serialize(Archive& archive, std::uint32_t const version)
		{
			archive(cereal::base_class<MonoBehaviour>(this));
		}
	};
}

REGISTER_COMPONENT(Enemy_Normal_01_State_Setter)
CEREAL_REGISTER_TYPE(BeastEngine::Enemy_Normal_01_State_Setter)
CEREAL_REGISTER_POLYMORPHIC_RELATION(BeastEngine::MonoBehaviour, BeastEngine::Enemy_Normal_01_State_Setter)
CEREAL_CLASS_VERSION(BeastEngine::Enemy_Normal_01_State_Setter, 1)