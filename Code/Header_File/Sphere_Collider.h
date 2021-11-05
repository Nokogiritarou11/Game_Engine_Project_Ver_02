#pragma once
#include "Collider.h"

namespace BeastEngine
{
	//スフィアコライダーコンポーネント
	class Sphere_Collider :public Collider
	{
	public:
		void Set_Radius(float new_radius); //半径を設定する
		[[nodiscard]] float Get_Radius() const { return radius; } //半径を取得する

	private:
		void Create_Shape() override; //形状を作成する(Sphere)
		bool Draw_ImGui() override; //ImGui描画

		float radius = 0.5f; //半径

		friend class cereal::access;
		template<class Archive>
		void serialize(Archive& archive, std::uint32_t const version)
		{
			archive(cereal::base_class<Collider>(this), radius);
		}
	};
}

REGISTER_COMPONENT(Sphere_Collider)
CEREAL_REGISTER_TYPE(BeastEngine::Sphere_Collider)
CEREAL_REGISTER_POLYMORPHIC_RELATION(BeastEngine::Collider, BeastEngine::Sphere_Collider)
CEREAL_CLASS_VERSION(BeastEngine::Sphere_Collider, 1)