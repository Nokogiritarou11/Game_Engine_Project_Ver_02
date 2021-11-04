#pragma once
#include "Collider.h"

namespace BeastEngine
{
	//シリンダーコライダー
	class Cylinder_Collider final :public Collider
	{
	public:
		void Set_Radius(float new_radius); //半径を設定する
		void Set_Height(float new_height); //高さと設定する
		[[nodiscard]] float Get_Radius() const { return radius; } //半径を取得する
		[[nodiscard]] float Get_Height() const { return height; } //高さを取得する

	private:
		void Create_Shape() override; //形状を作成する(Cylinder)
		bool Draw_ImGui() override;    //ImGui描画

		float radius = 0.5f;
		float height = 1.0f;

		friend class cereal::access;
		template<class Archive>
		void serialize(Archive& archive, std::uint32_t const version)
		{
			archive(cereal::base_class<Collider>(this), radius, height);
		}
	};
}

REGISTER_COMPONENT(Cylinder_Collider)
CEREAL_REGISTER_TYPE(BeastEngine::Cylinder_Collider)
CEREAL_REGISTER_POLYMORPHIC_RELATION(BeastEngine::Collider, BeastEngine::Cylinder_Collider)
CEREAL_CLASS_VERSION(BeastEngine::Cylinder_Collider, 1)