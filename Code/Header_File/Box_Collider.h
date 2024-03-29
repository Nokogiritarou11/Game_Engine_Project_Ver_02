#pragma once
#include "Collider.h"

namespace BeastEngine
{
	//ボックスコライダー
	class Box_Collider final :public Collider
	{
	public:
		void Set_Size(const Vector3& new_size); //サイズを設定する
		[[nodiscard]] Vector3 Get_Size() const { return size; } //サイズを取得する

	private:
		void Create_Shape() override; //形状を作成する(Box)
		bool Draw_ImGui() override; //ImGui描画

		Vector3 size = { 1.0f,1.0f ,1.0f };

		friend class cereal::access;
		template<class Archive>
		void serialize(Archive& archive, std::uint32_t const version)
		{
			archive(cereal::base_class<Collider>(this), size);
		}
	};
}

REGISTER_COMPONENT(Box_Collider)
CEREAL_REGISTER_TYPE(BeastEngine::Box_Collider)
CEREAL_REGISTER_POLYMORPHIC_RELATION(BeastEngine::Collider, BeastEngine::Box_Collider)
CEREAL_CLASS_VERSION(BeastEngine::Box_Collider, 1)