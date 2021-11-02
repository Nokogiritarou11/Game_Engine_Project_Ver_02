#pragma once
#include "Component.h"

namespace BeastEngine
{
	//Enableを持つコンポーネントの基底クラス
	class Behaviour : public Component
	{
	public:
		Behaviour();

		void Set_Enabled(bool value);           //アクティブ状態を切り替える
		[[nodiscard]] bool Get_Enabled() const; //アクティブ状態を取得する

	protected:
		bool Draw_ImGui_Header(const std::string& component_name, bool& open); //Inspector内で表示する共通のタブを描画する 各Draw_ImGui関数内で最初に呼ぶ必要がある

	private:
		bool enabled = true;      //アクティブ状態
		bool enabled_old = false; //変更時トリガー用キャッシュ

		friend class cereal::access;
		template<class Archive>
		void serialize(Archive& archive, std::uint32_t const version)
		{
			archive(cereal::base_class<Component>(this), enabled);
		}
	};
}

CEREAL_REGISTER_TYPE(BeastEngine::Behaviour)
CEREAL_REGISTER_POLYMORPHIC_RELATION(BeastEngine::Component, BeastEngine::Behaviour)
CEREAL_CLASS_VERSION(BeastEngine::Behaviour, 1)