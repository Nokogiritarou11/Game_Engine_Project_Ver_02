#pragma once
#include "MonoBehaviour.h"

namespace BeastEngine
{
	class Player_Parameter;

	class UI_Targeting final : public MonoBehaviour
	{
	public:
		void Set_State(int value);

	private:
		void Awake() override;
		void LateUpdate() override;
		bool Draw_ImGui() override;

		std::weak_ptr<Sprite_Renderer> sprite_target;
		std::weak_ptr<Player_Parameter> parameter;
		std::weak_ptr<Camera> camera;
		int state = 0;
		Vector4 default_color = { 1,1,1,1 };
		Vector4 locking_color = { 1,1,1,1 };

		// シリアライズ関数
		friend class cereal::access;
		template<class Archive>
		void serialize(Archive& archive, std::uint32_t const version)
		{
			archive(cereal::base_class<MonoBehaviour>(this), default_color, locking_color);
		}
	};
}

REGISTER_COMPONENT(UI_Targeting)
CEREAL_REGISTER_TYPE(BeastEngine::UI_Targeting)
CEREAL_REGISTER_POLYMORPHIC_RELATION(BeastEngine::MonoBehaviour, BeastEngine::UI_Targeting)
CEREAL_CLASS_VERSION(BeastEngine::UI_Targeting, 1)